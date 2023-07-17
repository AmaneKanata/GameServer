#pragma once

#include <Session.h>

class ClientBase;

class GameSession : public PacketSession
{
public:
	GameSession(boost::asio::io_context& ioc) : PacketSession(ioc)
	{}
	~GameSession();

	std::shared_ptr<ClientBase> client;

protected:
	virtual void OnRecvPacket(unsigned char* buffer, int len) override;
};