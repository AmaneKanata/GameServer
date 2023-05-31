#include "PacketManager.h"
#include "Session/GameSession.h"
#include "Contents/Base/RoomBase.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<GameSession>& session, unsigned char* buffer, int len)
{
	return false;
}

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(shared_ptr<GameSession>& session, Protocol::{{pkt.name}}& pkt)
{
	GRoom->Handle_{{pkt.name}}(session, pkt);
	return true;
}
{%- endfor %}
