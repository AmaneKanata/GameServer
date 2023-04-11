#pragma once

#include "../pch.h"

class GameSession;
class SendBuffer;

class Client
{
public:
	Client(string clientId)
		: clientId(clientId)
	{}
	~Client()
	{
		GLogManager->Log("Client ", clientId, " Removed");
	}

	void Send(shared_ptr<SendBuffer> sendBuffer);
	void OnDisconnected();

public:
	string clientId;
	shared_ptr<GameSession> session;

	int objectId;
	float position_x = 0;
	float position_y = 0;
	float position_z = 0;
};