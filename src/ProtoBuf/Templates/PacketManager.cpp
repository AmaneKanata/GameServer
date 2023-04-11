#include "PacketManager.h"
#include "Session/GameSession.h"
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

{%- for pkt in parser.recv_pkt %}
	{%- if (pkt.name != 'C_ENTER') and (pkt.name != 'C_REENTER') %}
bool Handle_{{pkt.name}}(shared_ptr<GameSession>& session, Protocol::{{pkt.name}}& pkt)
{
	if (session->owner == nullptr)
		return false;

	GRoom->Handle_{{pkt.name}}(session->owner, pkt);
	return true;
}
	{%- endif %}
{%- endfor %}
