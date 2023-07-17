#pragma once

#include "PacketManager.h"

class ClientBase : public PacketHandler
{
public:
	ClientBase(boost::asio::io_context& ioc, std::string clientId)
		: PacketHandler(ioc)
		, clientId(clientId)
	{}

	~ClientBase();

	virtual void HandleClose() override;

	virtual void SetSession(std::shared_ptr<GameSession> session);

	virtual void Enter();
	virtual void ReEnter();
	virtual void Leave();
	virtual void Disconnect();

	virtual void Handle_S_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ENTER> pkt) override;
	virtual void Handle_S_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REENTER> pkt) override;
	virtual void Handle_S_ADD_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ADD_CLIENT> pkt) override;
	virtual void Handle_S_REMOVE_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REMOVE_CLIENT> pkt) override;
	virtual void Handle_S_DISCONNECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_DISCONNECT> pkt) override;

	void CheckAlive();

	void Send(shared_ptr<SendBuffer> sendBuffer);

public:
	const std::string clientId;
	std::shared_ptr<GameSession> session;

private:
	std::time_t lastMessageSent = -1;
};