#include "Session.h"
#include "SendBuffer.h"
#include "PacketHeader.h"

Session::Session(boost::asio::io_context& context) 
	: socket(make_shared<boost::asio::ip::tcp::socket>(context))
	, recvBuffer(BUFFER_SIZE)
	, isConnected(false)
	, timer(context, std::chrono::milliseconds{ LINGER_TIME*1000 })
{
}

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
	
	//boost::asio::socket_base::keep_alive keepAlive();

	boost::asio::ip::tcp::no_delay noDelay(true);
	socket->set_option(noDelay);

	{
		lock_guard<recursive_mutex> lock(isConnected_mtx);
		isConnected = true;
	}

	OnConnected();

	RegisterRecv();
}

void Session::RegisterDisconnect()
{
	lock_guard<recursive_mutex> lock(send_mtx);

	isDisconnectedRegistered.store(true);

	if (!isSendRegistered)
	{
		Disconnect();
	}
}

void Session::Disconnect()
{
	lock_guard<recursive_mutex> lock(isConnected_mtx);

	if(!isConnected)
		return;

	isConnected = false;

	socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);

	OnDisconnected();
}

void Session::RegisterRecv()
{
	boost::asio::mutable_buffer buffer(reinterpret_cast<char*>(recvBuffer.WritePos()), recvBuffer.FreeSize());

	auto ref = shared_from_this();

	socket->async_receive(buffer, [this, ref](const boost::system::error_code& error, std::size_t bytes_transferred) {

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
		});
}

void Session::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	bool registerSend = false;

	{
		lock_guard<recursive_mutex> lock(send_mtx);

		if (isDisconnectedRegistered)
			return;

		pendingSendBuffers.push_back(sendBuffer);

		if (isSendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void Session::RegisterSend()
{
	shared_ptr<deque<shared_ptr<SendBuffer>>> temp_pendingSendBuffers = std::make_shared<deque<shared_ptr<SendBuffer>>>();
	vector<boost::asio::const_buffer> sendBuffers;

	{
		lock_guard<recursive_mutex> lock(send_mtx);
		temp_pendingSendBuffers->swap(pendingSendBuffers);
	}

	for(auto sendBuffer : *temp_pendingSendBuffers)
	{
		sendBuffers.emplace_back(sendBuffer->Buffer(), sendBuffer->WriteSize());
	}

	auto ref = shared_from_this();

	socket->async_send(sendBuffers, [this, ref, temp_pendingSendBuffers](const boost::system::error_code& error, std::size_t bytes_transferred) {

		if (bytes_transferred == 0)
		{
			Disconnect();
			return;
		}

		std::size_t byte_total = 0;
		std::size_t unsent_start_index = 0;

		for (auto& sendBuffer : *temp_pendingSendBuffers)
		{
			byte_total += sendBuffer->WriteSize();

			if (byte_total > bytes_transferred)
			{
				break;
			}

			++unsent_start_index;
		}

		{
			lock_guard<recursive_mutex> lock(send_mtx);

			pendingSendBuffers.insert(pendingSendBuffers.begin(),
				temp_pendingSendBuffers->begin() + unsent_start_index,
				temp_pendingSendBuffers->end());

			if (pendingSendBuffers.empty())
			{
				isSendRegistered.store(false);

				if (isDisconnectedRegistered == true)
				{
					Disconnect();
				}
			}	
			else
			{
				RegisterSend();
			}
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