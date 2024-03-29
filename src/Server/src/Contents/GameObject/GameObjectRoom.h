#pragma once

#include "RoomBase.h"

class GameObject;
class GameObjectClient;

class GameObjectRoom : public RoomBase
{
public:
	GameObjectRoom(boost::asio::io_context& ioc) : RoomBase(ioc)
	{}

	virtual void HandleInit() override;

	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code) override;

	virtual void InstantiateGameObject(std::shared_ptr<GameObject> gameObject);
	virtual void DestroyGameObject(int id);
	virtual void SetGameObjectPrefab(int id, std::string prefabName);
	virtual void SetGameObjectOwner(int id, std::string ownerId);
	
protected:
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt) override;
	virtual void Handle_C_DESTORY_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt) override;
	virtual void Handle_C_SET_GAME_OBJECT_PREFAB(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_PREFAB> pkt) override;
	virtual void Handle_C_SET_GAME_OBJECT_OWNER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_OWNER> pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt) override;
	virtual void Handle_C_SET_TRANSFORM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt) override;
	virtual void Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId, std::shared_ptr<GameSession> session) override;

private:
	void Update();

private:
	int idGenerator = 0;
	std::map<int, std::shared_ptr<GameObject>> gameObjects;
};