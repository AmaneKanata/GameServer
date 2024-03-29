#include "Session.h"
#include "SendBuffer.h"
#include "PacketHeader.h"
#include "CoreLib_Singleton.h"

Session::Session(boost::asio::io_context& context)
	: JobQueue(context)
	, socket(make_shared<boost::asio::ip::tcp::socket>(context))
	, recvBuffer(BUFFER_SIZE)
{}

Session::~Session()
{
	socket->close();
}

void Session::Connect(boost::asio::ip::tcp::endpoint ep)
{
	socket->connect(ep);

	ProcessConnect();
}

void Session::ProcessConnect()
{
	boost::asio::socket_base::linger linger(true, LINGER_TIME);
	socket->set_option(linger);

	boost::asio::ip::tcp::no_delay noDelay(true);
	socket->set_option(noDelay);

	isConnected = true;

	OnConnected();

	RegisterRecv();
}

void Session::RegisterDisconnect()
{
	if (!isConnected || isDisconnectRegistered)
		return;

	isDisconnectRegistered = true;

	if (!isSendRegistered)
	{
		Disconnect();
	}
}

void Session::Disconnect()
{
	if (!isConnected)
		return;

	isConnected = false;

	socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);

	OnDisconnected();
}

void Session::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	if (!isConnected || isDisconnectRegistered)
		return;

	pendingSendBuffers.push_back(sendBuffer);

	if (!isSendRegistered)
	{
		isSendRegistered = true;
		Post(&Session::RegisterSend);
	}
}

void Session::SendMany(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers)
{
	if (!isConnected || isDisconnectRegistered)
		return;

	pendingSendBuffers.insert(pendingSendBuffers.end(), sendBuffers->begin(), sendBuffers->end());

	if (!isSendRegistered)
	{
		isSendRegistered = true;
		Post(&Session::RegisterSend);
	}
}

void Session::RegisterSend()
{
	inFlightSendBuffers.swap(pendingSendBuffers);

	vector<boost::asio::const_buffer> sendBuffers;
	for (auto& sendBuffer : inFlightSendBuffers)
	{
		sendBuffers.emplace_back(sendBuffer->Buffer(), sendBuffer->WriteSize());
	}

	auto ref = shared_from_this();

	socket->async_send(sendBuffers, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			this->Post(&Session::ProcessSend, bytes_transferred);
		}
	);
}

void Session::ProcessSend(std::size_t bytes_transferred)
{
	if (bytes_transferred == 0)
	{
		isSendRegistered = false;
		Disconnect();
		return;
	}

	std::size_t byte_total = 0;
	std::size_t unsent_start_index = 0;

	for (auto& sendBuffer : inFlightSendBuffers)
	{
		byte_total += sendBuffer->WriteSize();

		if (byte_total > bytes_transferred)
		{
			break;
		}

		++unsent_start_index;
	}

	pendingSendBuffers.insert(pendingSendBuffers.begin(),
		inFlightSendBuffers.begin() + unsent_start_index,
		inFlightSendBuffers.end());

	inFlightSendBuffers.clear();

	if (pendingSendBuffers.empty())
	{
		isSendRegistered = false;
		if (isDisconnectRegistered)
		{
			Disconnect();
		}
	}
	else
	{
		Post(&Session::RegisterSend);
	}
}

void Session::RegisterRecv()
{
	boost::asio::mutable_buffer buffer(reinterpret_cast<char*>(recvBuffer.WritePos()), recvBuffer.FreeSize());

	auto ref = shared_from_this();

	socket->async_receive(buffer, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			this->Post(&Session::ProcessRecv, bytes_transferred);
		}
	);
}

void Session::ProcessRecv(std::size_t bytes_transferred)
{
	if (bytes_transferred == 0)
	{
		RegisterDisconnect();
		return;
	}

	if (recvBuffer.OnWrite(bytes_transferred) == false)
	{
		RegisterDisconnect();
		return;
	}

	int dataSize = recvBuffer.DataSize();
	int processLen = OnRecv(recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || recvBuffer.OnRead(processLen) == false)
	{
		RegisterDisconnect();
		return;
	}

	recvBuffer.Clean();

	RegisterRecv();
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