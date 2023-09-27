#include "GameObjectRoom.h"
#include "GameObject.h"
#include "GameObjectClient.h"
#include "GameSession.h"

void GameObjectRoom::HandleInit()
{
	Post(&GameObjectRoom::Update);

	RoomBase::HandleInit();
}

void GameObjectRoom::Leave(std::shared_ptr<ClientBase> client, std::string code)
{
	auto gClient = static_pointer_cast<GameObjectClient>(client);

	for (auto& [id, gameObject] : gClient->gameObjects)
	{
		if (gameObjects.erase(id))
		{
			GLogManager->Log("GameObject Removed : ", gClient->clientId, " ", std::to_string(gameObject->id), ", GameObject Number : ", std::to_string(gameObjects.size()));

			Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
			removeGameObject.add_gameobjects(id);
			Post(&RoomBase::Broadcast, MakeSendBuffer(removeGameObject));
		}
	}

	gClient->gameObjects.clear();

	RoomBase::Leave(client, code);
}

void GameObjectRoom::InstantiateGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects.insert({ gameObject->id, gameObject });

	Protocol::S_ADD_GAME_OBJECT addGameObject;
	auto gameObjectInfo = addGameObject.add_gameobjects();
	gameObject->MakeGameObjectInfo(gameObjectInfo);
	Broadcast(MakeSendBuffer(addGameObject));
}

void GameObjectRoom::DestroyGameObject(int id)
{
	auto gameObject = gameObjects.find(id);
	if (gameObject == gameObjects.end())
		return;

	gameObjects.erase(id);

	Protocol::S_REMOVE_GAME_OBJECT removeGameObject;
	removeGameObject.add_gameobjects(id);
	Broadcast(MakeSendBuffer(removeGameObject));
}

void GameObjectRoom::SetGameObjectPrefab(int id, std::string prefabName)
{
	auto gameObject = gameObjects.find(id);
	if (gameObject == gameObjects.end())
		return;
	
	if(gameObject->second->prefabName == prefabName)
		return;

	gameObject->second->prefabName = prefabName;

	Protocol::S_SET_GAME_OBJECT_PREFAB notice;
	notice.set_gameobjectid(id);
	notice.set_prefabname(prefabName);
	Broadcast(MakeSendBuffer(notice));
}

void GameObjectRoom::SetGameObjectOwner(int id, std::string ownerId)
{
	auto gameObject = gameObjects.find(id);
	if (gameObject == gameObjects.end())
		return;

	if (gameObject->second->ownerId != "")
	{
		auto prevOwner = clients.find(gameObject->second->ownerId);
		if (prevOwner != clients.end())
		{
			auto gClient = static_pointer_cast<GameObjectClient>(prevOwner->second);
			gClient->gameObjects.erase(id);
		}
	}

	gameObject->second->ownerId = ownerId;

	Protocol::S_SET_GAME_OBJECT_OWNER notice;
	notice.set_gameobjectid(id);
	notice.set_ownerid(ownerId);
	Broadcast(MakeSendBuffer(notice));
}

void GameObjectRoom::Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt)
{
	auto gameObject = make_shared<GameObject>(idGenerator++);
	gameObject->type = static_cast<GameObjectType>(pkt->type());
	gameObject->ownerId = session->client->clientId;
	gameObject->prefabName = pkt->prefabname();
	gameObject->SetPosition(pkt->position());
	gameObject->SetRotation(pkt->rotation());

	Protocol::S_INSTANTIATE_GAME_OBJECT res;
	res.set_success(true);
	res.set_gameobjectid(gameObject->id);
	session->client->Send(MakeSendBuffer(res));

	auto gClient = static_pointer_cast<GameObjectClient>(session->client);
	gClient->gameObjects.insert({ gameObject->id, gameObject });

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

void GameObjectRoom::Handle_C_SET_GAME_OBJECT_PREFAB(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_PREFAB> pkt)
{
	auto gClient = static_pointer_cast<GameObjectClient>(session->client);
	auto gameObject = gClient->gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end() || gameObject->second->ownerId != session->client->clientId)
	{
		return;
	}

	SetGameObjectPrefab(pkt->gameobjectid(), pkt->prefabname());
}

void GameObjectRoom::Handle_C_SET_GAME_OBJECT_OWNER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_OWNER> pkt)
{
	auto gameObject = gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end())
	{
		return;
	}

	if (gameObject->second->ownerId == session->client->clientId)
	{
		return;
	}
	else
	{
		auto prevOwner = clients.find(gameObject->second->ownerId);
		if (prevOwner != clients.end())
		{
			static_pointer_cast<GameObjectClient>(prevOwner->second)->gameObjects.erase(pkt->gameobjectid());
		}
	}

	auto curOwner = static_pointer_cast<GameObjectClient>(session->client);
	gameObject->second->ownerId = curOwner->clientId;
	curOwner->gameObjects.insert({ pkt->gameobjectid(), gameObject->second });

	SetGameObjectOwner(pkt->gameobjectid(), session->client->clientId);
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
	if (gameObject == gameObjects.end() || gameObject->second->ownerId != session->client->clientId)
		return;

	gameObject->second->UpdateTransform(pkt);
}

void GameObjectRoom::Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt)
{
	auto gameObject = gameObjects.find(pkt->gameobjectid());
	if (gameObject == gameObjects.end() || gameObject->second->ownerId != session->client->clientId)
		return;

	gameObject->second->UpdateAnimation(pkt);
}

std::shared_ptr<ClientBase> GameObjectRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<GameObjectClient>(clientId);
	client->SetSession(session);
	return client;
}

void GameObjectRoom::Update()
{
	auto start = std::chrono::system_clock::now().time_since_epoch();

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	for (const auto& [key, gameObject] : gameObjects)
	{
		if (gameObject->isTransformDirty)
		{
			gameObject->isTransformDirty = false;
			sendBuffers->push_back(MakeSendBuffer(gameObject->transform));
		}

		if (gameObject->isAnimationDirty)
		{
			gameObject->isAnimationDirty = false;
			sendBuffers->push_back(MakeSendBuffer(gameObject->setAnimation));
		}
	}

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - start).count();

	if (elapsed >= 50)
	{
		DelayPost(50, &GameObjectRoom::Update);
	}
	else
	{
		DelayPost(50 - elapsed, &GameObjectRoom::Update);
	}
}