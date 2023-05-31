#pragma once

#include "../../pch.h"

class RoomBase;
class GameSession;
class SendBuffer;

class ClientBase
{
public:
	ClientBase(string clientId) : clientId(clientId)
	{}

	~ClientBase()
	{
		GLogManager->Log("Client Destroyed :			", clientId);
	}

public:
	string clientId;
	shared_ptr<GameSession> session;
};