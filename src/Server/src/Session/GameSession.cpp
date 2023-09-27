#include "GameSession.h"
#include "Server_Singleton.h"
#include "RoomBase.h"
#include "ClientBase.h"

GameSession::~GameSession()
{
	auto sessionNumber = session_num.fetch_sub(1);
	if (!clientId.empty())
		GLogManager->Log("Session Destroyed : ", clientId, ", Session Number : ", std::to_string(sessionNumber - 1));
}

void GameSession::SetClient(std::shared_ptr<ClientBase> client)
{
	if (!isConnected || isDisconnectRegistered)
	{
		client->OnDisconnected();
		return;
	}

	this->client = client;
	clientId = client->clientId;
}

void GameSession::ReleaseClient()
{
	this->client = nullptr;
}

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	if (!isConnected || isDisconnectRegistered)
		return;

	lastMessageArrived = std::time(0);

	GRoom->HandlePacket(std::static_pointer_cast<GameSession>(shared_from_this()), buffer, len);
}

void GameSession::CheckAlive(std::time_t current)
{
	if (!isConnected || isDisconnectRegistered)
		return;

	if (current - lastMessageArrived > CHECK_ALIVE_INTERVAL/1000)
	{
		if (client != nullptr)
		{
			GLogManager->Log("Client Heartbeat Fail : ", client->clientId);
			GRoom->Post(&RoomBase::Leave, client, std::string("HEARTBEAT_FAIL"));
		}
		else
		{
			Post(&Session::RegisterDisconnect);
		}
	}
	else
	{
		DelayPost(CHECK_ALIVE_INTERVAL, &GameSession::CheckAlive, time(0));
	}
}

void GameSession::Pong()
{
}

void GameSession::OnConnected()
{
	auto sessionNumber = session_num.fetch_add(1);
	GLogManager->Log("Session Created, Session Number : ", std::to_string(sessionNumber + 1));

	DelayPost(CHECK_ALIVE_INTERVAL, &GameSession::CheckAlive, time(0));
}

void GameSession::OnDisconnected()
{
	if (!clientId.empty())
	{
		GLogManager->Log("Session Disconnected : ", clientId);
	}

	if (DISCONNECTED_INTERVAL == 0 || client == nullptr)
	{
		ProcessDisconnect();
	}
	else
	{
		DelayPost(DISCONNECTED_INTERVAL, &GameSession::ProcessDisconnect);
	}
}

void GameSession::ProcessDisconnect()
{
	if (client != nullptr)
	{
		client->OnDisconnected();
		client = nullptr;
	}

	Clear();
}
