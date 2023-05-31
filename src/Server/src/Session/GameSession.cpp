#include "GameSession.h"

#include "../pch.h"
#include "../PacketManager.h"
#include "../Contents/Base/ClientBase.h"
#include "../Contents/Base/RoomBase.h"

void GameSession::OnDisconnected()
{
	jobs.post(std::bind(&GameSession::Leave, static_pointer_cast<GameSession>(shared_from_this()), string("DISCONNECTED")));
}

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	auto session= static_pointer_cast<GameSession>(shared_from_this());
	PacketManager::HandlePacket(session, buffer, len);
}

void GameSession::Leave(string code)
{
	if (state == SessionState::LEAVING)
		return;

	state = SessionState::LEAVING;

	//GRoom->jobs.post(&RoomBase::Leave, static_pointer_cast<GameSession>(shared_from_this()));

	Protocol::S_DISCONNECT disconnect;
	disconnect.set_code(code);
	Send(PacketManager::MakeSendBuffer(disconnect));

	client = nullptr;
}
