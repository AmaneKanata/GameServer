#pragma once

#include "RecvBuffer.h"

#include <mutex>
#include <deque>
#include <boost/asio.hpp>

class SendBuffer;

const int LINGER_TIME = 5;

class Session : public std::enable_shared_from_this<Session>
{
	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	Session(boost::asio::io_context& context);
	~Session();

	void Connect(boost::asio::ip::tcp::endpoint ep);
	void ProcessConnect();
	void RegisterDisconnect();
	void Disconnect();

	void Send(shared_ptr<SendBuffer> sendBuffer);

	shared_ptr<boost::asio::ip::tcp::socket> GetSocket() { return socket; }

protected:
	virtual int OnRecv(unsigned char* buffer, int len) { return len; }
	virtual void OnConnected() {};
	virtual void OnDisconnected() {};

private:
	void RegisterRecv();
	void RegisterSend();

protected:
	bool isConnected;
	std::recursive_mutex isConnected_mtx;

private:
	shared_ptr<boost::asio::ip::tcp::socket> socket;

	RecvBuffer recvBuffer;

	std::atomic<bool> isSendRegistered = { false };
	std::atomic<bool> isDisconnectedRegistered = { false };
	std::deque<shared_ptr<SendBuffer>> pendingSendBuffers;
	std::recursive_mutex send_mtx;

	boost::asio::steady_timer timer;
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