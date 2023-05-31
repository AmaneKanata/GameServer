#include "Session.h"
#include "../pch.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using namespace std;
using namespace boost::asio;

TCPSession::TCPSession(io_context& context)
	: JobQueue(context)
	, socket(make_shared<ip::tcp::socket>(context))
	, recvBuffer(BUFFER_SIZE)
{
}

TCPSession::~TCPSession()
{
	GLogManager->Log("Socket Destroyed");
}

void TCPSession::Connect(ip::tcp::endpoint ep)
{
	socket->connect(ep);

	ProcessConnect();
}

void TCPSession::Disconnect()
{
	if (isConnected.exchange(false) == false)
		return;

	socket->close();

	OnDisconnected();
}

void TCPSession::ProcessConnect()
{
	boost::asio::socket_base::linger option(true, 100);
	socket->set_option(option);

	isConnected.store(true);
	
	OnConnected();

	RegisterRecv();
}

void TCPSession::RegisterRecv()
{
	mutable_buffer buffer(reinterpret_cast<char*>(recvBuffer.WritePos()), recvBuffer.FreeSize());

	auto ref = shared_from_this();

	socket->async_receive(buffer, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred) {

		if (bytes_transferred == 0)
		{
			Disconnect();
			return;
		}

		if (recvBuffer.OnWrite(bytes_transferred) == false)
		{
			Disconnect();
			return;
		}

		int dataSize = recvBuffer.DataSize();
		int processLen = OnRecv(recvBuffer.ReadPos(), dataSize);
		if (processLen < 0 || dataSize < processLen || recvBuffer.OnRead(processLen) == false)
		{
			Disconnect();
			return;
		}

		recvBuffer.Clean();

		RegisterRecv();
		});
}

void TCPSession::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	bool registerSend = false;

	{
		lock_guard<recursive_mutex> lock(mtx);

		sendQueue.push(sendBuffer);

		if (isSendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
	{
		jobs.post(std::bind(&TCPSession::RegisterSend, static_pointer_cast<TCPSession>(shared_from_this())));
	}
}

void TCPSession::RegisterSend()
{
	{
		lock_guard<recursive_mutex> lock(mtx);

		sendBufferRefs.reserve(sendQueue.size());

		while (sendQueue.empty() == false)
		{
			sendBufferRefs.emplace_back(sendQueue.front());
			sendQueue.pop();
		}
	}

	vector<boost::asio::const_buffer> sendBuffers = vector<boost::asio::const_buffer>();
	
	for (shared_ptr<SendBuffer> sendBuffer : sendBufferRefs)
		sendBuffers.emplace_back(sendBuffer->Buffer(), sendBuffer->WriteSize());

	auto ref = static_pointer_cast<TCPSession>(shared_from_this());

	socket->async_send(sendBuffers, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred) {
		
		sendBufferRefs.clear();

		if (!isConnected)
			return;

		if (error)
			return;

		if (bytes_transferred == 0)
		{
			Disconnect();
			return;
		}

		{
			lock_guard<recursive_mutex> lock(mtx);
			if (sendQueue.empty())
				isSendRegistered.store(false);
			else
				jobs.post(std::bind(&TCPSession::RegisterSend, ref));
		}

		});
}

int PacketSession::OnRecv(unsigned char* buffer, int len)
{
	int processLen = 0;

	while (true)
	{
		int dataSize = len - processLen;
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}