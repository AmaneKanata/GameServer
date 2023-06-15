#include "GameSession.h"
#include "Server_Singleton.h"
#include "RoomBase.h"
#include "ClientBase.h"

GameSession::GameSession(boost::asio::io_context& ioc) 
	: PacketSession(ioc)
	, isRegistered(false)
{
}

GameSession::~GameSession()
{
	if (!isRegistered)
		return;

	auto sessionNumber = session_num.fetch_sub(1);

	if (client != nullptr)
	{
		GLogManager->Log("Session Destroyed : ", client->clientId, ", Session Number : ", std::to_string(sessionNumber - 1));
	}
	else
	{
		GLogManager->Log("Session Destroyed, Session Number : ", std::to_string(sessionNumber - 1));
	}
}

void GameSession::SetClient(std::shared_ptr<ClientBase> _client)
{
	std::lock_guard<std::recursive_mutex> lock(isConnected_mtx);

	if (!isConnected)
	{
		_client->Post(&ClientBase::OnDisconnected);
		return;
	}

	client = _client;
}

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	lastMessageArrived = std::time(0);
	GRoom->HandlePacket(std::static_pointer_cast<GameSession>(shared_from_this()), buffer, len);
}

void GameSession::OnConnected()
{
	auto sessionNumber = session_num.fetch_add(1);
	GLogManager->Log("Session Created, Session Number : ", std::to_string(sessionNumber + 1));
	isRegistered = true;
}

void GameSession::OnDisconnected()
{
	if (client != nullptr)
	{
		client->Post(&ClientBase::OnDisconnected);
	}
}
