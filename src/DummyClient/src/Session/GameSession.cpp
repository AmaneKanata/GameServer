#include "GameSession.h"
#include "ClientBase.h"

void GameSession::OnRecvPacket(unsigned char* buffer, int len)
{
	client->HandlePacket(std::static_pointer_cast<GameSession>(shared_from_this()), buffer, len);
}