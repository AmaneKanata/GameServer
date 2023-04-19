#include "PacketManager.h"
#include "Network/GameSession.h"
#include "Contents/Base/RoomBase.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<GameSession>& session, unsigned char* buffer, int len)
{
	return false;
}

bool Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt)
{
	GRoom->Handle_C_ENTER(session, pkt);
	return true;
}

bool Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt)
{
	GRoom->Handle_C_REENTER(session, pkt);
	return true;
}
bool Handle_C_LEAVE(shared_ptr<GameSession>& session, Protocol::C_LEAVE& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_C_LEAVE(session->owner, pkt);
	return true;
}
bool Handle_C_GET_CLIENT(shared_ptr<GameSession>& session, Protocol::C_GET_CLIENT& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_C_GET_CLIENT(session->owner, pkt);
	return true;
}
bool Handle_C_INSTANTIATE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::C_INSTANTIATE_GAME_OBJECT& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_C_INSTANTIATE_GAME_OBJECT(session->owner, pkt);
	return true;
}
bool Handle_C_GET_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::C_GET_GAME_OBJECT& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_C_GET_GAME_OBJECT(session->owner, pkt);
	return true;
}
bool Handle_C_SET_TRANSFORM(shared_ptr<GameSession>& session, Protocol::C_SET_TRANSFORM& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_C_SET_TRANSFORM(session->owner, pkt);
	return true;
}