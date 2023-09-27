#include <vector>

#include "FPSRoom.h"
#include "FPSObject.h"
#include "FPSClient.h"
#include "GameSession.h"

#include "DebugDrawer.h"

void FPSRoom::HandleInit()
{
	collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
	dispatcher = std::make_shared<btCollisionDispatcher>(collisionConfiguration.get());
	broadphase = std::make_shared<btDbvtBroadphase>();
	solver = std::make_shared<btSequentialImpulseConstraintSolver>();
	dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());

	LoadMap();

	BulletDebugDrawer* myDebugDrawer = new BulletDebugDrawer();
	myDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld->setDebugDrawer(myDebugDrawer);

	RoomBase::HandleInit();

	Post(&FPSRoom::Update);
}

void FPSRoom::HandleClose()
{
	RoomBase::HandleClose();
}

void FPSRoom::Leave(std::shared_ptr<ClientBase> client, std::string code)
{
	auto fClient = static_pointer_cast<FPSClient>(client);

	if (fClient->player != nullptr)
	{
		RemovePlayer(fClient->player->id);
		fClient->player = nullptr;
	}

	RoomBase::Leave(client, code);
}

void FPSRoom::RemovePlayer(int playerId)
{
	auto player = players.find(playerId);
	if (player == players.end())
		return;

	dynamicsWorld->removeCollisionObject(player->second->collisionObject.get());

	players.erase(player);

	Protocol::S_REMOVE_GAME_OBJECT remove;
	remove.add_gameobjects(playerId);
	Broadcast(MakeSendBuffer(remove));
}

void FPSRoom::Draw()
{
	dynamicsWorld->debugDrawWorld();
}

void FPSRoom::Update()
{
	long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	for(auto& player : players)
	{
		if (player.second->isRotationDirty)
		{
			sendBuffers->push_back(MakeSendBuffer(player.second->setRotation));
		}
		
		if (player.second->velocity.isZero())
			continue;

		int timeGap = now - player.second->timestamp;

		btVector3 newPosition = player.second->position + player.second->velocity * timeGap;

		btTransform transform;
		transform.setOrigin(newPosition);
		transform.setRotation(player.second->rotation);
		player.second->collisionObject->setWorldTransform(transform);
	}

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	DelayPost(50, &FPSRoom::Update);
}

void FPSRoom::Handle_C_INSTANTIATE_FPS_PLAYER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client == nullptr)
		return;

	auto player = std::make_shared<FPSPlayer>(client->clientId, idGenerator++, pkt);

	players.insert({ player->id, player });
	
	client->player = player;

	dynamicsWorld->addCollisionObject(player->collisionObject.get());

	Protocol::S_ADD_FPS_PLAYER res;
	auto playerInfo = res.add_gameobjects();
	playerInfo->mutable_position()->CopyFrom(player->setPosition.position());
	playerInfo->mutable_velocity()->CopyFrom(player->setPosition.velocity());
	playerInfo->mutable_rotation()->CopyFrom(player->setRotation.rotation());
	playerInfo->set_playerid(player->id);
	playerInfo->set_hp(player->hp);
	playerInfo->set_ownerid(player->ownerId);

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt)
{
	Protocol::S_ADD_FPS_PLAYER res;

	for (auto& player : players)
	{
		auto playerInfo = res.add_gameobjects();
		playerInfo->mutable_position()->CopyFrom(player.second->setPosition.position());
		playerInfo->mutable_velocity()->CopyFrom(player.second->setPosition.velocity());
		playerInfo->set_playerid(player.second->id);
		playerInfo->set_hp(player.second->hp);
		playerInfo->set_ownerid(player.second->ownerId);
	}

	session->Send(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->UpdatePosition(pkt->timestamp(), pkt->release_position(), pkt->release_velocity());

	Broadcast(MakeSendBuffer(player->second->setPosition));
}

void FPSRoom::Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->UpdateRotation(pkt->release_rotation());
}

void FPSRoom::Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->UpdateAnimation(pkt);

	Broadcast(MakeSendBuffer(player->second->setAnimation));
}

const static float rayDistance = 1000;
const static int damage = 10;
void FPSRoom::Handle_C_SHOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOT> pkt)
{
	btVector3 from(pkt->position().x() * -1, pkt->position().y(), pkt->position().z());
	btVector3 direction(pkt->direction().x() * -1, pkt->direction().y(), pkt->direction().z());
	btVector3 to = from + direction * rayDistance;

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	dynamicsWorld->rayTest(from, to, rayCallback);

	fromX = from.x();
	fromY = from.y();
	fromZ = from.z();
	toX = to.x();
	toY = to.y();
	toZ = to.z();

	if(rayCallback.hasHit())
	{
		auto id = rayCallback.m_collisionObject->getUserIndex();

		GLogManager->Log("HIt : ", std::to_string(id));

		toX = rayCallback.m_hitPointWorld.x();
		toY = rayCallback.m_hitPointWorld.y();
		toZ = rayCallback.m_hitPointWorld.z();

		auto player = players.find(id);
		if (player == players.end())
			return;

		player->second->hp -= damage;

		if(player->second->hp == 0)
		{
			RemovePlayer(player->second->id);
		}

		Protocol::S_ATTACKED res;
		res.set_playerid(player->second->id);
		res.set_damage(damage);
		Broadcast(MakeSendBuffer(res));
	}
}

std::shared_ptr<ClientBase> FPSRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<FPSClient>(clientId);
	client->SetSession(session);
	return client;
}

void FPSRoom::LoadMap()
{
	string mapJsonString = 
		"[{ \"SizeX\": 100.00,  \"SizeY\": 0.00,  \"SizeZ\": 100.00,  \"PositionX\": 0.00,  \"PositionY\": 0.00,  \"PositionZ\": 0.00,  \"RotationX\": 0.00,  \"RotationY\": 0.00,  \"RotationZ\": 0.00   },   {  \"SizeX\": 1.00,  \"SizeY\": 3.00,  \"SizeZ\": 1.00,  \"PositionX\": 0.00,  \"PositionY\": 1.50,  \"PositionZ\": 9.25,  \"RotationX\": 0.00,  \"RotationY\": 0.00,  \"RotationZ\": 0.00   },   {  \"SizeX\": 1.00,  \"SizeY\": 5.00,  \"SizeZ\": 1.00,  \"PositionX\": -1.00,  \"PositionY\": 2.50,  \"PositionZ\": -3.50,  \"RotationX\": 0.00,  \"RotationY\": 0.00,  \"RotationZ\": 0.00 }]";
	nlohmann::json mapJson = nlohmann::json::parse(mapJsonString);

	for (int i = 0; i < mapJson.size(); i++)
	{
		nlohmann::json mapObject = mapJson[i];

		btCollisionShape* boxShape = new btBoxShape(btVector3(mapObject["SizeX"].get<float>() / 2, mapObject["SizeY"].get<float>() / 2, mapObject["SizeZ"].get<float>() / 2));
		btCollisionObject* boxCollisionObject = new btCollisionObject();

		boxCollisionObject->setUserIndex(idGenerator++);

		boxCollisionObject->setCollisionShape(boxShape);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(mapObject["PositionX"].get<float>(), mapObject["PositionY"].get<float>(), mapObject["PositionZ"].get<float>()));
		transform.setRotation(btQuaternion(mapObject["RotationX"].get<float>(), mapObject["RotationY"].get<float>(), mapObject["RotationZ"].get<float>()));
		boxCollisionObject->setWorldTransform(transform);

		dynamicsWorld->addCollisionObject(boxCollisionObject);
	}
}