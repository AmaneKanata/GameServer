#pragma once
#include "../pch.h"

class Scene;
class GameObject;
class RoomBase;
class ClientBase;

class GameSession : public PacketSession
{
public:
	GameSession(io_context& ioc) : PacketSession(ioc)
	{}
	~GameSession() { owner = nullptr; }

	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(unsigned char* buffer, int len) override;

	shared_ptr<ClientBase> owner;
};