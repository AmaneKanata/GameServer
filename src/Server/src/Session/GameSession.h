#pragma once

#include <boost/asio.hpp>
#include <Session.h>
#include <JobQueue.h>

#include "Server_Singleton.h"

class ClientBase;

class GameSession : public PacketSession
{
public:
	GameSession(boost::asio::io_context& ioc) : PacketSession(ioc) 
	{}
	~GameSession();

	void SetClient(std::shared_ptr<ClientBase> client);
	void ReleaseClient();

	virtual void OnConnected() override;

	virtual void OnDisconnected() override;
	void ProcessDisconnect();

	virtual void OnRecvPacket(unsigned char* buffer, int len) override;

	void CheckAlive(std::time_t current);

public:
	std::shared_ptr<ClientBase> client;

private:
	std::time_t lastMessageArrived = -1;

	string clientId;
};