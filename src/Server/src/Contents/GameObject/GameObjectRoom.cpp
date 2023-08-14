#include "GameObjectRoom.h"
#include "GameObject.h"
#include "GameObjectClient.h"
#include "GameSession.h"

void GameObjectRoom::HandleInit()
{
	Post(&GameObjectRoom::UpdateTransform);

	RoomBase::HandleInit();
}

void GameObjectRoom::Leave(std::shared_ptr<ClientBase> client, std::string code)
{
	auto gClient = static_pointer_cast<GameObjectClient>(client);

	for (auto& [gameObjectId, gameObject] : gClient->gameObjects)
	{
		if (gameObjects.erase(gameObjectId))
		{
			GLogManager->Log("GameObject Removed : ", gClient->clientId, " ", std::to_string(gameObject->gameObjectId), ", GameObject Number : ", std::to_string(gameObjects.size()));

			Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
			removeGameObject.add_gameobjects(gameObjectId);
			Post(&RoomBase::Broadcast, MakeSendBuffer(removeGameObject));
		}
	}

	gClient->gameObjects.clear();

	RoomBase::Leave(client, code);
}

void GameObjectRoom::InstantiateGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects.insert({ gameObject->gameObjectId, gameObject });

	Protocol::S_ADD_GAME_OBJECT addGameObject;
	auto gameObjectInfo = addGameObject.add_gameobjects();
	gameObject->MakeGameObjectInfo(gameObjectInfo);
	Broadcast(MakeSendBuffer(addGameObject));
}

void GameObjectRoom::DestroyGameObject(int gameObjectId)
{
	auto gameObject = gameObjects.find(gameObjectId);
	if (gameObject == gameObjects.end())
		return;

	gameObjects.erase(gameObjectId);

	Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
	removeGameObject.add_gameobjects(gameObjectId);
	Broadcast(MakeSendBuffer(removeGameObject));
}

void GameObjectRoom::ChangeGameObject(int gameObjectId, std::string prefabName)
{
	auto gameObject = gameObjects.find(gameObjectId);
	if (gameObject == gameObjects.end())
		return;
	
	gameObject->second->prefabName = prefabName;

	Protocol::S_CHANGE_GMAE_OBJECT_NOTICE notice;
	notice.set_gameobjectid(gameObjectId);
	notice.set_prefabname(prefabName);
	Broadcast(MakeSendBuffer(notice));
}

void GameObjectRoom::Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt)
{
	auto gameObject = make_shared<GameObject>(idGenerator++);
	gameObject->SetPosition(pkt->position());
	gameObject->SetRotation(pkt->rotation());
	gameObject->prefabName = pkt->prefabname();
	gameObject->ownerId = session->client->clientId;
	
	Protocol::S_INSTANTIATE_GAME_OBJECT res;
	res.set_success(true);
	res.set_gameobjectid(gameObject->gameObjectId);
	session->client->Send(MakeSendBuffer(res));

	auto gClient = static_pointer_cast<GameObjectClient>(session->client);
	gClient->gameObjects.insert({ gameObject->gameObjectId, gameObject });

	InstantiateGameObject(gameObject);
}

void GameObjectRoom::Handle_C_DESTORY_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt)
{
	auto gClient = static_pointer_cast<GameObjectClient>(session->client);

	auto gameObject = gClient->gameObjects.find(pkt->gameobjectid());
	if (gameObject == gClient->gameObjects.end())
	{
		Protocol::S_DESTORY_GAME_OBJECT res;
		res.set_gameobjectid(pkt->gameobjectid());
		res.set_success(false);
		return;
	}

	gClient->gameObjects.erase(gameObject);

	DestroyGameObject(pkt->gameobjectid());
}

void GameObjectRoom::Handle_C_CHANGE_GMAE_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_GMAE_OBJECT> pkt)
{
	Protocol::S_CHANGE_GMAE_OBJECT res;
	
	auto gClient = static_pointer_cast<GameObjectClient>(session->client);
	auto gameObject = gClient->gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end())
	{
		res.set_gameobjectid(pkt->gameobjectid());
		res.set_success(false);
		gClient->Send(MakeSendBuffer(res));
		return;
	}

	ChangeGameObject(pkt->gameobjectid(), pkt->prefabname());

	if(gameObject->second->prefabName != pkt->prefabname())
	{
		res.set_gameobjectid(pkt->gameobjectid());
		res.set_success(false);
		gClient->Send(MakeSendBuffer(res));
		return;
	}
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

	gameObject->second->UpdateTransform(pkt);
}

void GameObjectRoom::Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt)
{
	auto gameObject = gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end())
		return;

	Protocol::S_SET_ANIMATION setAnimation;
	setAnimation.set_gameobjectid(pkt->gameobjectid());
	setAnimation.mutable_params()->insert(pkt->params().begin(), pkt->params().end());

	Broadcast(MakeSendBuffer(setAnimation));
}

std::shared_ptr<ClientBase> GameObjectRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<GameObjectClient>(clientId);
	client->SetSession(session);
	return client;
}

void GameObjectRoom::UpdateTransform()
{
	auto start = std::chrono::system_clock::now().time_since_epoch();

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	for (const auto& [key, gameObject] : gameObjects)
	{
		if (!gameObject->isDirty)
		{
			continue;
		}
		
		gameObject->isDirty = false;
		sendBuffers->push_back(MakeSendBuffer(gameObject->transform));
	}

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - start).count();

	if (elapsed >= 50)
	{
		DelayPost(50, &GameObjectRoom::UpdateTransform);
	}
	else
	{
		DelayPost(50 - elapsed, &GameObjectRoom::UpdateTransform);
	}
}
