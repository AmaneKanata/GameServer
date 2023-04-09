#pragma once

#include "../Base/RoomBase.h"

class GameObject;

class GameObjectRoom : public RoomBase
{
public :
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_GET_GAME_OBJECT& pkt) override;
	virtual void Handle_C_SET_TRANSFORM(shared_ptr<ClientBase>& client, Protocol::C_SET_TRANSFORM& pkt) override;

	virtual void Leave(shared_ptr<ClientBase> client) override;

	virtual shared_ptr<ClientBase> MakeClient(string clientId) override;

	void InstantiateGameObject(shared_ptr<ClientBase> client, Protocol::C_INSTANTIATE_GAME_OBJECT pkt);
	void GetGameObject(shared_ptr<ClientBase> client);
	void SetTransform(Protocol::C_SET_TRANSFORM pkt);

	map<int, shared_ptr<GameObject>> gameObjects;
};
