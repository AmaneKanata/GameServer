#include "GameSession.h"
#include "ClientBase.h"
#include "LogManager.h"
#include "DummyClient_Singleton.h"

GameSession::~GameSession()
{
	if(client)
		GLogManager->Log("[Client ", client->clientId, "]	Session Destroyed");
	else
		GLogManager->Log("Session Destroyed");
}

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	client->HandlePacket(std::static_pointer_cast<GameSession>(shared_from_this()), buffer, len);
}