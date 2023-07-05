#include "GameObjectRoom.h"
#include "GameObject.h"
#include "GameObjectClient.h"
#include "GameSession.h"

static int idGenerator = 0;

void GameObjectRoom::Leave(std::shared_ptr<ClientBase> client, std::string code)
{
	auto gClient = static_pointer_cast<GameObjectClient>(client);

	if (gClient->gameObject != nullptr)
	{
		gameObjects.erase(gClient->gameObject->gameObjectId);
		//GLogManager->Log("GameObject Removed : ", std::to_string(gClient->gameObject->gameObjectId), ", GameObject Number : ", std::to_string(gameObjects.size()));

		Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
		removeGameObject.add_gameobjects(gClient->gameObject->gameObjectId);
		Broadcast(MakeSendBuffer(removeGameObject));

		gClient->gameObject = nullptr;
	}

	RoomBase::Leave(client, code);
}

void GameObjectRoom::Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt)
{
	auto gameObject = make_shared<GameObject>(idGenerator++);
	gameObject->SetPosition(pkt->position());
	gameObject->SetRotation(pkt->rotation());

	auto gClient = static_pointer_cast<GameObjectClient>(session->client);
	gClient->gameObject = gameObject;

	gameObjects.insert({ gameObject->gameObjectId, gameObject });
	//GLogManager->Log("GameObject Added : ", std::to_string(gameObject->gameObjectId), ", GameObject Number : ", std::to_string(gameObjects.size()));

	Protocol::S_INSTANTIATE_GAME_OBJECT res;
	res.set_success(true);
	res.set_gameobjectid(gameObject->gameObjectId);
	session->client->Send(MakeSendBuffer(res));

	Protocol::S_ADD_GAME_OBJECT addGameObject;
	auto gameObjectInfo = addGameObject.add_gameobjects();
	gameObject->MakeGameObjectInfo(gameObjectInfo);
	Broadcast(MakeSendBuffer(addGameObject));
}

void GameObjectRoom::Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt)
{
	Protocol::S_ADD_GAME_OBJECT addGameObject;

	for (const auto& [key, gameObject] : gameObjects)
	{
		auto gameObjectInfo = addGameObject.add_gameobjects();
		gameObject->MakeGameObjectInfo(gameObjectInfo);
	}

	session->client->Send(MakeSendBuffer(addGameObject));
}

void GameObjectRoom::Handle_C_SET_TRANSFORM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt)
{
	auto gameObject = gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end())
		return;

	gameObject->second->SetPosition(pkt->position());
	gameObject->second->SetRotation(pkt->rotation());

	Protocol::S_SET_TRANSFORM setTransform;
	setTransform.set_gameobjectid(pkt->gameobjectid());
	setTransform.set_allocated_position(pkt->release_position());
	setTransform.set_allocated_rotation(pkt->release_rotation());
	Broadcast(MakeSendBuffer(setTransform));
}

std::shared_ptr<ClientBase> GameObjectRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<GameObjectClient>(clientId);
	client->SetSession(session);
	return client;
}
