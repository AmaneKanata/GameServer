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
	virtual void Handle_C_LEAVE(shared_ptr<ClientBase>& client, Protocol::C_LEAVE& pkt) {};
	virtual void Handle_C_GET_CLIENT(shared_ptr<ClientBase>& client, Protocol::C_GET_CLIENT& pkt) {};
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) {};
	virtual void Handle_C_GET_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_GET_GAME_OBJECT& pkt) {};
	virtual void Handle_C_SET_TRANSFORM(shared_ptr<ClientBase>& client, Protocol::C_SET_TRANSFORM& pkt) {};
};