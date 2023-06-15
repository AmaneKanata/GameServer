#pragma once

#include "PacketManager.h"

enum class ClientState
{
	NORMAL,
	DISCONNECTED
};

class ClientBase : public PacketHandler
{
public:
	ClientBase(boost::asio::io_context& ioc, std::string clientId, std::shared_ptr<GameSession> session)
		: PacketHandler(ioc)
		, clientId(clientId)
		, session(session)
		, state(ClientState::NORMAL)
	{}

	virtual void Enter();
	virtual void Leave();
	virtual void Disconnect();

	virtual void Handle_S_ENTER(std::shared_ptr<GameSession> session, Protocol::S_ENTER pkt) override;
	virtual void Handle_S_ADD_CLIENT(std::shared_ptr<GameSession> session, Protocol::S_ADD_CLIENT pkt) override;
	virtual void Handle_S_REMOVE_CLIENT(std::shared_ptr<GameSession> session, Protocol::S_REMOVE_CLIENT pkt) override;
	virtual void Handle_S_DISCONNECT(std::shared_ptr<GameSession> session, Protocol::S_DISCONNECT pkt) override;

	void Send(shared_ptr<SendBuffer> sendBuffer);

	const std::string clientId;
	const std::weak_ptr<GameSession> session;

private:
	ClientState state;
};