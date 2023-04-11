#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "RecvBuffer.h"
#include "SendBuffer.h"

using namespace boost::asio;

class Service;

class Session : public enable_shared_from_this<Session>
{
	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	Session(io_context& context);
	virtual ~Session();

	shared_ptr<ip::tcp::socket> GetSocket() { return socket; }

	void Connect(ip::tcp::endpoint ep);
	void Disconnect();

	void Send(shared_ptr<SendBuffer> sendBuffer);
	
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();

protected:
	virtual void OnConnected() {};
	virtual void OnDisconnected() {};
	virtual int OnRecv(unsigned char* buffer, int len) { return len; }

private:
	recursive_mutex mtx;

	shared_ptr<ip::tcp::socket> socket;
	shared_ptr<Service> service;
	RecvBuffer recvBuffer;
	queue<shared_ptr<SendBuffer>> sendQueue;
	vector<shared_ptr<SendBuffer>> sendBufferRefs;
	atomic<bool> isSendRegistered = { false };
	atomic<bool> isConnected = { false };
};

struct PacketHeader
{
	unsigned short size;
	unsigned short id;
};

class PacketSession : public Session
{
public:
	PacketSession(io_context& ioc) : Session(ioc)
	{}
	virtual ~PacketSession() {};

	virtual int OnRecv(unsigned char* buffer, int len) final;
	virtual void OnRecvPacket(unsigned char* buffer, int len) = 0;
};