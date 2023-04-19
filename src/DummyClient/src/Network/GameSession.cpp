#include "GameSession.h"

#include "../PacketManager.h"
#include "../Client/Client.h"

void GameSession::OnDisconnected()
{
	if (owner == nullptr)
		return;

	owner->OnDisconnected();
}

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	auto session= static_pointer_cast<GameSession>(shared_from_this());
	PacketManager::HandlePacket(session, buffer, len);
}
