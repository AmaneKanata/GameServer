#pragma once

#include <boost/asio.hpp>
#include <Session.h>
#include <JobQueue.h>

#include "Server_Singleton.h"

class ClientBase;

class GameSession : public PacketSession
{
public:
	GameSession(boost::asio::io_context& ioc);
	~GameSession();

	void SetClient(std::shared_ptr<ClientBase> client);

	std::shared_ptr<ClientBase> client;

protected:
	virtual void OnRecvPacket(unsigned char* buffer, int len) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;

private:
	bool isRegistered;
};