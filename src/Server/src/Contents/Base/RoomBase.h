#pragma once
#include "PacketHandler.h"
#include "boost/asio.hpp"

class GameSession;
class ClientBase;

enum class RoomState
{
	Idle,
	Running,
	Closing,
	Closed
};

class RoomBase : public PacketHandler, public JobQueue
{
public:
	RoomBase(boost::asio::io_context& ioc);
	virtual ~RoomBase();

	virtual void Init();
	virtual void Close();
	virtual void HandleClose();

	virtual void Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt) override;
	virtual void Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt) override;
	virtual void Handle_C_LEAVE(shared_ptr<GameSession>& session, Protocol::C_LEAVE& pkt) override;
	virtual void Handle_C_GET_CLIENT(shared_ptr<GameSession>& session, Protocol::C_GET_CLIENT& pkt) override;
	
	virtual void Enter(shared_ptr<GameSession> session, Protocol::C_ENTER pkt);
	virtual void Enter_CheckDuplicated(shared_ptr<GameSession> session, Protocol::C_ENTER pkt);
	virtual void ReEnter(shared_ptr<GameSession> session, string clientId);
	virtual void Leave(shared_ptr<GameSession> session);
	virtual void GetClients(shared_ptr<GameSession> session);

	virtual shared_ptr<ClientBase> MakeClient();

	virtual void Broadcast(shared_ptr<SendBuffer> sendBuffer);
	
	string roomId;
	RoomState state;

	map<string, shared_ptr<GameSession>> sessions;
};