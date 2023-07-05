#pragma once

#include "RecvBuffer.h"
#include "JobQueue.h"

#include <mutex>
#include <deque>
#include <boost/asio.hpp>

class SendBuffer;

const int LINGER_TIME = 5;

class Session : public JobQueue
{
	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	Session(boost::asio::io_context& context);
	virtual ~Session();

	shared_ptr<boost::asio::ip::tcp::socket> GetSocket() { return socket; }

	void Connect(boost::asio::ip::tcp::endpoint ep);
	void ProcessConnect();
	virtual void OnConnected() {};

	void RegisterDisconnect();
	void Disconnect();
	virtual void OnDisconnected() {};

	void Send(shared_ptr<SendBuffer> sendBuffer);
	void RegisterSend();
	void ProcessSend(std::size_t bytes_transferred);

	void RegisterRecv();
	void ProcessRecv(std::size_t bytes_transferred);
	virtual int OnRecv(unsigned char* buffer, int len) { return len; }

protected:
	bool isConnected = false;
	bool isDisconnectRegistered = false;

private:
	shared_ptr<boost::asio::ip::tcp::socket> socket;
	RecvBuffer recvBuffer;
	std::deque<shared_ptr<SendBuffer>> pendingSendBuffers;
	std::deque<shared_ptr<SendBuffer>> inFlightSendBuffers;

	bool isSendRegistered = false;
};

class PacketSession : public Session
{
public:
	PacketSession(boost::asio::io_context& ioc) : Session(ioc)
	{}
	virtual ~PacketSession() {};

protected:
	virtual int OnRecv(unsigned char* buffer, int len) final;
	virtual void OnRecvPacket(unsigned char* buffer, int len) = 0;
};