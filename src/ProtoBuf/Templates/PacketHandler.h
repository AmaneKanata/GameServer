#pragma once

#include "../../pch.h"
#include "../../Protocols.h"

class GameSession;
class ClientBase;

class PacketHandler : public JobQueue
{
public:
	virtual void Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt) {};
	virtual void Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt) {};

{%- for pkt in parser.recv_pkt %}
	{%- if (pkt.name != 'C_ENTER') and (pkt.name != 'C_REENTER')%}
	virtual void Handle_{{pkt.name}}(shared_ptr<ClientBase>& client, Protocol::{{pkt.name}}& pkt) {};
	{%- endif %}
{%- endfor %}
};