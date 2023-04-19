#pragma once
#include "../pch.h"
#include <boost/asio.hpp>

class Client;

class GameSession : public PacketSession
{
public:
	GameSession(io_context& ioc) : PacketSession(ioc)
	{}
	~GameSession() { owner = nullptr; }

	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(unsigned char* buffer, int len) override;

	shared_ptr<Client> owner;
};