#include "ClientBase.h"
#include "Server_Singleton.h"
#include "GameSession.h"
#include "RoomBase.h"
#include "PacketManager.h"

void ClientBase::Disconnect()
{
	if (DELAY_SEND)
	{
		session->DelayPost(DELAY_SEND_INTERVAL, &GameSession::ReleaseClient);
		session->DelayPost(DELAY_SEND_INTERVAL, &GameSession::RegisterDisconnect);
	}
	else
	{
		session->Post(&GameSession::ReleaseClient);
		session->Post(&GameSession::RegisterDisconnect);
	}
}

void ClientBase::OnDisconnected()
{
	GRoom->Post(&RoomBase::Leave, static_pointer_cast<ClientBase>(shared_from_this()), std::string("DISCONNECTED"));
}

void ClientBase::SetSession(std::shared_ptr<GameSession> session)
{
	if (this->session != nullptr)
	{
		this->session->Post(&GameSession::ReleaseClient);
		this->session->Post(&GameSession::RegisterDisconnect);
	}

	this->session = session;
	this->session->Post(&GameSession::SetClient, static_pointer_cast<ClientBase>(shared_from_this()));
}

void ClientBase::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	if (session == nullptr)
		return;

	if(DELAY_SEND)
		session->DelayPost(DELAY_SEND_INTERVAL, &Session::Send, sendBuffer);
	else
		session->Post(&Session::Send, sendBuffer);
}

void ClientBase::SendMany(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers)
{
	if (session == nullptr)
		return;

	if (DELAY_SEND)
		session->DelayPost(DELAY_SEND_INTERVAL, &Session::SendMany, sendBuffers);
	else
		session->Post(&Session::SendMany, sendBuffers);
}
