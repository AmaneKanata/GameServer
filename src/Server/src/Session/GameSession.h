#pragma once
#include "../pch.h"

class ClientBase;

enum class SessionState
{
	NORMAL,
	LEAVING
};


class GameSession : public JobQueue
{
public:
	GameSession(io_context& ioc) : JobQueue(ioc)
	{}

	//virtual void OnDisconnected() override;
	void OnRecvPacket(unsigned char* buffer, int len);

	void Leave(string code);

	shared_ptr<ClientBase> client;
	SessionState state;
};