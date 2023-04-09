#pragma once
#include "PacketHandler.h"

class GameSession;
class ClientBase;

enum class RoomState
{
	Idle,
	Running,
	Closing,
	Closed
};

class RoomBase : public PacketHandler
{
public:
	RoomBase();
	virtual ~RoomBase();

	virtual void Init();
	virtual void Close();
	virtual void HandleClose();

	virtual void Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt) override;
	virtual void Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt) override;
	virtual void Handle_C_LEAVE(shared_ptr<ClientBase>& client, Protocol::C_LEAVE& pkt) override;
	virtual void Handle_C_GET_CLIENT(shared_ptr<ClientBase>& client, Protocol::C_GET_CLIENT& pkt) override;
	
	virtual void Enter(shared_ptr<GameSession> session, Protocol::C_ENTER pkt);
	virtual void ReEnter(shared_ptr<GameSession> session, string clientId);
	virtual void Leave(shared_ptr<ClientBase> session);
	virtual void GetClient(shared_ptr<ClientBase> session);

	virtual void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	virtual shared_ptr<ClientBase> MakeClient(string clientId);
	
	string roomId;
	RoomState state;

	map<string, shared_ptr<ClientBase>> clients;
};