#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "RecvBuffer.h"
#include "SendBuffer.h"

#include "../Job/JobQueue.h"

using namespace boost::asio;

class Service;

class TCPSession : public JobQueue
{  
	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	TCPSession(io_context& context);
	virtual ~TCPSession();

	shared_ptr<ip::tcp::socket> GetSocket() { return socket; }

	void Connect(ip::tcp::endpoint ep);
	void Disconnect();

	void Send(shared_ptr<SendBuffer> sendBuffer);

	void ProcessConnect();

protected:
	void RegisterRecv();
	void RegisterSend();

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

//class PacketSession : public TCPSession
//{
//public:
//	PacketSession(io_context& ioc) : TCPSession(ioc)
//	{}
//	virtual ~PacketSession() {};
//
//	virtual int OnRecv(unsigned char* buffer, int len) final;
//	virtual void OnRecvPacket(unsigned char* buffer, int len) = 0;
//};