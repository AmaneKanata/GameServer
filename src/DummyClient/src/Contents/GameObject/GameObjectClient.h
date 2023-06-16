#pragma once

#include "ClientBase.h"

struct GameObject
{
	int id;

	int position_x;
	int position_y;
	int position_z;
};

class GameObjectClient : public ClientBase
{
public:
	GameObjectClient(boost::asio::io_context& ioc, std::string clientId)
		: ClientBase(ioc, clientId)
		, go()
	{}

	void InstantiateGameObject();
	void Move();

	virtual void Handle_S_ENTER(std::shared_ptr<GameSession> session, Protocol::S_ENTER pkt) override;
	virtual void Handle_S_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_INSTANTIATE_GAME_OBJECT pkt) override;
	virtual void Handle_S_ADD_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_ADD_GAME_OBJECT pkt) override;
	virtual void Handle_S_REMOVE_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_REMOVE_GAME_OBJECT pkt) override;
	virtual void Handle_S_SET_TRANSFORM(std::shared_ptr<GameSession> session, Protocol::S_SET_TRANSFORM pkt) override;

private:
	GameObject go;
};
