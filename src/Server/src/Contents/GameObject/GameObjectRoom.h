#pragma once

#include "../Base/RoomBase.h"

class GameObject;

class GameObjectRoom : public RoomBase
{
public :
	GameObjectRoom(boost::asio::io_context& ioc) : RoomBase(ioc)
	{}

	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::C_GET_GAME_OBJECT& pkt) override;
	virtual void Handle_C_SET_TRANSFORM(shared_ptr<GameSession>& session, Protocol::C_SET_TRANSFORM& pkt) override;

	virtual void Leave(shared_ptr<GameSession> session) override;

	void InstantiateGameObject(shared_ptr<GameSession> session, Protocol::C_INSTANTIATE_GAME_OBJECT pkt);
	void GetGameObject(shared_ptr<GameSession> session);
	void SetTransform(Protocol::C_SET_TRANSFORM pkt);

	virtual shared_ptr<ClientBase> MakeClient() override;

	map<int, shared_ptr<GameObject>> gameObjects;
};
