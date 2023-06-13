#pragma once

#include "RoomBase.h"

class GameObject;

class GameObjectRoom : public RoomBase
{
public:
	GameObjectRoom(boost::asio::io_context& ioc) : RoomBase(ioc)
	{}

	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code) override;

protected:
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::C_INSTANTIATE_GAME_OBJECT pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::C_GET_GAME_OBJECT pkt) override;
	virtual void Handle_C_SET_TRANSFORM(std::shared_ptr<GameSession> session, Protocol::C_SET_TRANSFORM pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId) override;

	std::map<int, std::shared_ptr<GameObject>> gameObjects;
};