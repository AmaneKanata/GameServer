#include "GameObjectRoom.h"
#include "GameObjectClient.h"
#include "GameObject.h"
#include "../../PacketManager.h"
#include "../../Network/GameSession.h"

static int idGenerator = 0;

void GameObjectRoom::Handle_C_INSTANTIATE_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) { DoAsync(&GameObjectRoom::InstantiateGameObject, client, pkt); }
void GameObjectRoom::Handle_C_GET_GAME_OBJECT(shared_ptr<ClientBase>& client, Protocol::C_GET_GAME_OBJECT& pkt) { DoAsync(&GameObjectRoom::GetGameObject, client); }
void GameObjectRoom::Handle_C_SET_TRANSFORM(shared_ptr<ClientBase>& client, Protocol::C_SET_TRANSFORM& pkt) { DoAsync(&GameObjectRoom::SetTransform, pkt); }

void GameObjectRoom::Leave(shared_ptr<ClientBase> client)
{
	if (state != RoomState::Running) return;

	auto gClient = static_pointer_cast<GameObjectClient>(client);

	if (gClient->gameObject != nullptr)
	{
		gameObjects.erase(gClient->gameObject->gameObjectId);

		Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
		removeGameObject.add_gameobjects(gClient->gameObject->gameObjectId);
		Broadcast(PacketManager::MakeSendBuffer(removeGameObject));

		gClient->gameObject = nullptr;
	}

	RoomBase::Leave(client);
}

void GameObjectRoom::InstantiateGameObject(shared_ptr<ClientBase> client, Protocol::C_INSTANTIATE_GAME_OBJECT pkt)
{
	if (state != RoomState::Running) return;

	auto gameObject = make_shared<GameObject>(idGenerator++);
	gameObject->SetPosition(pkt.position());
	gameObject->SetRotation(pkt.rotation());

	auto gClient = static_pointer_cast<GameObjectClient>(client);
	gClient->gameObject = gameObject;

	gameObjects.insert({ gameObject->gameObjectId, gameObject });

	Protocol::S_INSTANTIATE_GAME_OBJECT res;
	res.set_success(true);
	res.set_gameobjectid(gameObject->gameObjectId);
	client->Send(PacketManager::MakeSendBuffer(res));

	Protocol::S_ADD_GAME_OBJECT addGameObject;
	auto gameObjectInfo = addGameObject.add_gameobjects();
	gameObject->MakeGameObjectInfo(gameObjectInfo);
	Broadcast(PacketManager::MakeSendBuffer(addGameObject));
}

void GameObjectRoom::GetGameObject(shared_ptr<ClientBase> client)
{
	if (state != RoomState::Running) return;

	Protocol::S_ADD_GAME_OBJECT addGameObject;

	for (const auto& [key, gameObject] : gameObjects)
	{
		auto gameObjectInfo = addGameObject.add_gameobjects();
		gameObject->MakeGameObjectInfo(gameObjectInfo);
	}

	client->Send(PacketManager::MakeSendBuffer(addGameObject));
}

void GameObjectRoom::SetTransform(Protocol::C_SET_TRANSFORM pkt)
{
	if (state != RoomState::Running) return;

	auto gameObject = gameObjects.find(pkt.gameobjectid());
	if (gameObject == gameObjects.end())
		return;

	gameObject->second->SetPosition(pkt.position());
	gameObject->second->SetRotation(pkt.rotation());

	Protocol::S_SET_TRANSFORM setTransform;
	setTransform.set_gameobjectid(pkt.gameobjectid());
	setTransform.set_allocated_position(pkt.release_position());
	setTransform.set_allocated_rotation(pkt.release_rotation());
	Broadcast(PacketManager::MakeSendBuffer(setTransform));
}

shared_ptr<ClientBase> GameObjectRoom::MakeClient(string clientId)
{
	return make_shared<GameObjectClient>(clientId);
}