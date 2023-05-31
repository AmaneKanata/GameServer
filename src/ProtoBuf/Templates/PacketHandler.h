#pragma once

#include "../../pch.h"
#include "../../Protocols.h"

class GameSession;
class ClientBase;

class PacketHandler
{
public:
{%- for pkt in parser.recv_pkt %}
	virtual void Handle_{{pkt.name}}(shared_ptr<GameSession>& session, Protocol::{{pkt.name}}& pkt) {};
{%- endfor %}
};