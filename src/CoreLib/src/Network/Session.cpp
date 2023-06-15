#include "Session.h"
#include "SendBuffer.h"
#include "PacketHeader.h"

Session::Session(boost::asio::io_context& context) 
	: socket(make_shared<boost::asio::ip::tcp::socket>(context))
	, recvBuffer(BUFFER_SIZE)
	, isConnected(false)
{
}

void Session::Connect(boost::asio::ip::tcp::endpoint ep)
{
	socket->connect(ep);

	ProcessConnect();
}

void Session::ProcessConnect()
{
	boost::asio::socket_base::linger option(true, 100);
	socket->set_option(option);

	{
		lock_guard<recursive_mutex> lock(isConnected_mtx);
		isConnected = true;
	}

	OnConnected();

	RegisterRecv();
}

void Session::Disconnect()
{
	lock_guard<recursive_mutex> lock(isConnected_mtx);

	if(!isConnected)
		return;

	isConnected = false;

	socket->close();

	OnDisconnected();
}

void Session::RegisterRecv()
{
	boost::asio::mutable_buffer buffer(reinterpret_cast<char*>(recvBuffer.WritePos()), recvBuffer.FreeSize());

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

void Session::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	bool registerSend = false;

	{
		lock_guard<recursive_mutex> lock(send_mtx);

		sendQueue.push(sendBuffer);

		if (isSendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void Session::RegisterSend()
{
	{
		lock_guard<recursive_mutex> lock(send_mtx);

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

	auto ref = shared_from_this();

	socket->async_send(sendBuffers, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred) {
		
		sendBufferRefs.clear();

		if (bytes_transferred == 0)
		{
			Disconnect();
			return;
		}

		{
			lock_guard<recursive_mutex> lock(send_mtx);
			if (sendQueue.empty())
				isSendRegistered.store(false);
			else
				RegisterSend();
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