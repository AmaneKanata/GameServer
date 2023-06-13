#pragma once

#include "PacketManager.h"

class ClientBase;

const bool CLOSE_ON_EMPTY = true;

class RoomBase : public PacketHandler
{
public:
	RoomBase(boost::asio::io_context& ioc) : PacketHandler(ioc)
	{}

	virtual void HandleInit();
	virtual void HandleClose();

	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code);

protected:
	virtual void Handle_INVALID(std::shared_ptr<GameSession> session, unsigned char* buffer, int len) override;

	virtual void Handle_C_ENTER(std::shared_ptr<GameSession> session, Protocol::C_ENTER pkt) override;
	virtual void Handle_C_REENTER(std::shared_ptr<GameSession> session, Protocol::C_REENTER pkt) override;
	virtual void Handle_C_LEAVE(std::shared_ptr<GameSession> session, Protocol::C_LEAVE pkt) override;
	virtual void Handle_C_GET_CLIENT(std::shared_ptr<GameSession> session, Protocol::C_GET_CLIENT pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId);

	virtual void Broadcast(shared_ptr<SendBuffer> sendBuffer);

	string roomId;

	map<string, std::shared_ptr<ClientBase>> clients;
};