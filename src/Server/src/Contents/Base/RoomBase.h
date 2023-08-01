#pragma once

#include "PacketManager.h"

class ClientBase;

class RoomBase : public PacketHandler
{
public:
	RoomBase(boost::asio::io_context& ioc) : PacketHandler(ioc)
	{}
	virtual ~RoomBase() {};

	virtual void HandleInit();
	virtual void HandleClose();

	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code);

	virtual void Broadcast(shared_ptr<SendBuffer> sendBuffer);

protected:
	virtual void Handle_INVALID(std::shared_ptr<GameSession> session, unsigned char* buffer, int len) override;

	virtual void Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt) override;
	virtual void Handle_C_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_REENTER> pkt) override;
	virtual void Handle_C_LEAVE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_LEAVE> pkt) override;
	virtual void Handle_C_GET_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_CLIENT> pkt) override;
	virtual void Handle_C_PING(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_PING> pkt) override;
	virtual void Handle_C_SERVERTIME(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SERVERTIME> pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId, std::shared_ptr<GameSession> session);

private:
	void SendServerTime();
	void CheckIdle();

protected:
	map<string, std::shared_ptr<ClientBase>> clients;
};