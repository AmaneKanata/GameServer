#pragma once

#include "RecvBuffer.h"

#include <mutex>
#include <queue>
#include <boost/asio.hpp>

class SendBuffer;

class Session : public std::enable_shared_from_this<Session>
{
	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	Session(boost::asio::io_context& context);

	void Connect(boost::asio::ip::tcp::endpoint ep);
	void ProcessConnect();
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
	queue<shared_ptr<SendBuffer>> sendQueue;
	vector<shared_ptr<SendBuffer>> sendBufferRefs;
	std::recursive_mutex send_mtx;
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