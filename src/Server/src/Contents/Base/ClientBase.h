#pragma once

#include "../../pch.h"

class RoomBase;
class GameSession;
class SendBuffer;

enum class ClientState
{
	NORMAL,
	LEAVING
};

class ClientBase : public JobQueue
{
public:
	ClientBase(string clientId) 
		: state(ClientState::NORMAL)
		, clientId(clientId)
	{}
	~ClientBase()
	{
		GLogManager->Log("Client Destroyed :			", clientId);
	}

	void Leave(string code);
	void Send(shared_ptr<SendBuffer> sendBuffer);
	void UDP_Send(shared_ptr<SendBuffer> sendBuffer);

	void ReEnter(shared_ptr<GameSession> session);

	void OnDisconnected();

public:
	string clientId;
	ClientState state;
	shared_ptr<GameSession> session;
	boost::asio::ip::udp::endpoint udpEp;
};