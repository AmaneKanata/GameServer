#include <vector>

#include "FPSRoom.h"
#include "FPSObject.h"
#include "FPSClient.h"
#include "GameSession.h"

void FPSRoom::HandleInit() 
{
	srand((unsigned int)time(NULL));

	Post(&RoomBase::SendServerTime);
}

void FPSRoom::Leave(std::shared_ptr<ClientBase> client, std::string code)
{
	if (roomState != RoomState::Idle)
		Post(&FPSRoom::FinishGame);

	RoomBase::Leave(client, code);
}

void FPSRoom::Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt)
{
	if (clients.size() >= 2)
	{
		session->Disconnect();
		return;
	}

	{
		auto client = clients.find(pkt->clientid());
		if (client != clients.end())
		{
			Post(&FPSRoom::Leave, client->second, std::string("DUPLICATED"));
			Post(&FPSRoom::Handle_C_ENTER, session, std::move(pkt));
			return;
		}
	}

	auto client = MakeClient(pkt->clientid(), session);

	clients.insert({ pkt->clientid(), client });

	Protocol::S_ENTER res;
	res.set_result("SUCCESS");
	client->Send(MakeSendBuffer(res));

	Protocol::S_ADD_CLIENT addClient;
	auto clientInfo = addClient.add_clientinfos();
	clientInfo->set_clientid(pkt->clientid());
	Broadcast(MakeSendBuffer(addClient));
}

void FPSRoom::Handle_C_FPS_READY(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_READY> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client == nullptr)
		return;

	client->isReady = pkt->isready();

	if (!client->isReady)
		return;

	bool allReady = true;
	for (auto& iter : clients)
	{
		auto client = static_pointer_cast<FPSClient>(iter.second);
		if (!client->isReady)
		{
			allReady = false;
			break;
		}
	}

	if (allReady)
	{
		roomState = RoomState::Loading;

		InitGame();

		Protocol::S_FPS_LOAD load;
		Broadcast(MakeSendBuffer(load));
	}
}

void FPSRoom::Handle_C_FPS_LOAD_COMPLETE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_LOAD_COMPLETE> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client == nullptr)
		return;

	client->isLoaded = true;

	bool allLoaded = true;
	for (auto& iter : clients)
	{
		auto client = static_pointer_cast<FPSClient>(iter.second);
		if (!client->isLoaded)
		{
			allLoaded = false;
			break;
		}
	}

	if (allLoaded)
	{
		StartGame();

		roomState = RoomState::Playing;

		Protocol::S_FPS_START start;
		Broadcast(MakeSendBuffer(start));
	}
}

void FPSRoom::Handle_C_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_POSITION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->position = ConvertVector3(pkt->position()) + player->second->center;
	player->second->velocity = ConvertVector3(pkt->velocity());
	player->second->timestamp = pkt->timestamp();

	Protocol::S_FPS_POSITION res;
	res.set_playerid(player->second->id);
	res.set_allocated_position(pkt->release_position());
	res.set_allocated_velocity(pkt->release_velocity());
	res.set_timestamp(pkt->timestamp());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ROTATION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->transform.setRotation(ConvertQuaternion(pkt->rotation()));

	Protocol::S_FPS_ROTATION res;
	res.set_playerid(player->second->id);
	res.set_allocated_rotation(pkt->release_rotation());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_FPS_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ANIMATION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	Protocol::S_FPS_ANIMATION res;
	res.set_playerid(player->second->id);
	res.set_allocated_fpsanimation(pkt->release_fpsanimation());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_FPS_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_RELOAD> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	Protocol::S_FPS_RELOAD res;
	res.set_playerid(player->second->id);

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_FPS_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_CHANGE_WEAPON> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	Protocol::S_FPS_CHANGE_WEAPON res;
	res.set_playerid(player->second->id);
	res.set_weaponid(pkt->weaponid());
	res.set_timestamp(pkt->timestamp());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_FPS_SHOOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_SHOOT> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	shoots.push_back({ client->clientId, ConvertVector3(pkt->position()), ConvertVector3(pkt->direction()) });

	Protocol::S_FPS_SHOOT res;
	res.set_playerid(client->player->id);
	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::InstantiatePlayer(std::shared_ptr<FPSClient> client, btVector3 position, btQuaternion rotation)
{
	auto player = std::make_shared<FPSPlayer>(client->clientId, idGenerator++, position, rotation);

	players.insert({ player->id, player });

	client->player = player;

	dynamicsWorld->addCollisionObject(player->collisionObject.get());

	Protocol::S_FPS_INSTANTIATE instantiate;
	instantiate.set_ownerid(player->ownerId);
	instantiate.set_playerid(player->id);
	instantiate.set_allocated_position(ConvertVector3(position));
	instantiate.set_allocated_rotation(ConvertQuaternion(rotation));
	instantiate.set_hp(player->hp);
	Broadcast(MakeSendBuffer(instantiate));
}

void FPSRoom::SpawnItem(btVector3 position)
{
	currentItemPosition = position;
	itemState = ItemState::Idle;
	currentOccupyTime = 0;
	occupierId = -1;

	Protocol::S_FPS_SPAWN_ITEM spawn;
	spawn.set_allocated_position(ConvertVector3(position));
	Broadcast(MakeSendBuffer(spawn));
}

void FPSRoom::InitGame()
{
	shoots_draw = vector<queue<pair<btVector3, btVector3>>>(2);

	scores = map<string, int>();
	for (auto& client : clients)
		scores.insert({ client.second->clientId, 0 });

	collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
	dispatcher = std::make_shared<btCollisionDispatcher>(collisionConfiguration.get());
	broadphase = std::make_shared<btDbvtBroadphase>();
	solver = std::make_shared<btSequentialImpulseConstraintSolver>();
	dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());

	LoadMap();

#ifdef _WIN32
	if (FPS_DRAW)
		InitDraw();
#endif
}

void FPSRoom::StartGame()
{
	int spawnPositionIndex = rand() % (spawnPositions.size() / 2 + 1);

	int cnt = 0;
	for (auto& [clientId, client] : clients)
	{
		btQuaternion rotation(0, 0, 0, 1);
		InstantiatePlayer(
			std::static_pointer_cast<FPSClient>(client), 
			spawnPositions[spawnPositionIndex * 2 + cnt],
			rotation);
		cnt++;
	}

	prevUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	Post(&FPSRoom::Update);
	
	int itemPositionIndex = rand() % (itemPositions.size() + 1);
	delayedJobs.push_back(
		DelayPost(3000, &FPSRoom::SpawnItem, itemPositions[itemPositionIndex])
	);
}

void FPSRoom::FinishGame()
{
	if(roomState == RoomState::Idle)
		return;

	for (auto& delayedJob : delayedJobs)
		delayedJob->isValid = false;
	delayedJobs.clear();

	roomState = RoomState::Idle;

	shoots.clear();
	shoots_draw.clear();
	scores.clear();

	for (auto& [clientId, client] : clients)
	{
		auto fClient = std::static_pointer_cast<FPSClient>(client);
			
		fClient->player = nullptr;

		fClient->isLoaded = false;
		fClient->isReady = false;
	}

	for (auto& [playerId, player] : players)
		dynamicsWorld->removeCollisionObject(player->collisionObject.get());
	players.clear();

	dynamicsWorld.reset();
	solver.reset();
	broadphase.reset();
	dispatcher.reset();
	collisionConfiguration.reset();

	Protocol::S_FPS_FINISH finish;
	Broadcast(MakeSendBuffer(finish));
}

void FPSRoom::Update()
{
	currentUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	deltaTime = currentUpdateTime - prevUpdateTime;

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	UpdateShoot(sendBuffers);
	UpdateItem(sendBuffers);
	UpdatePlayerState(sendBuffers);
	UpdateScore();
	UpdateTransform();

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	prevUpdateTime = currentUpdateTime;

	delayedJobs.push_back(
		DelayPost(updateInterval, &FPSRoom::Update)
	);
}

void FPSRoom::UpdateShoot(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers)
{
	for (auto& [shooterId, from, direction] : shoots)
	{
		btVector3 to = from + direction * rayDistance;

		btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
		dynamicsWorld->rayTest(from, to, rayCallback);

		if (!rayCallback.hasHit())
		{
			shoots_draw[0].push({ from, to });
			continue;
		}

		shoots_draw[0].push({ from, rayCallback.m_hitPointWorld });

		auto attackedPlayerId = rayCallback.m_collisionObject->getUserIndex();
		auto iter = players.find(attackedPlayerId);
		if (iter == players.end())
			continue;
		auto& attackedPlayer = iter->second;

		if(attackedPlayer->hp <= 0)
			continue;

		attackedPlayer->hp -= damage;

		Protocol::S_FPS_ATTACKED res;
		res.set_playerid(attackedPlayerId);
		res.set_damage(damage);
		res.set_hp(attackedPlayer->hp);
		sendBuffers->push_back(MakeSendBuffer(res));

		if (attackedPlayer->hp <= 0 && attackedPlayerId == occupierId)
		{
			scorerId = shooterId;
			itemState = ItemState::Scored;
		}
	}

	shoots.clear();
}

void FPSRoom::UpdateItem(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers)
{
	switch (itemState)
	{
	case ItemState::Idle:
	{
		int cnt = 0;
		for (auto& [playerId, player] : players)
		{
			if ((player->position - currentItemPosition).length() < itemDistance)
				cnt++;
		}

		if (cnt > 0)
			itemState = ItemState::BeingOccupied;
		else
		{
			int prevOccupyTime = currentOccupyTime;

			if (currentOccupyTime >= occupyTimeCap)
				currentOccupyTime = max(currentOccupyTime - deltaTime, occupyTimeCap);
			else
				currentOccupyTime = max(currentOccupyTime - deltaTime, 0);

			if (prevOccupyTime != currentOccupyTime)
			{
				Protocol::S_FPS_ITEM_OCCUPY_PROGRESS_STATE progress;
				progress.set_occupyprogressstate(currentOccupyTime);
				sendBuffers->push_back(MakeSendBuffer(progress));
			}
		}

		break;
	}
	case ItemState::BeingOccupied:
	{
		int cnt = 0;
		int occupierId = -1;
		for (auto& [playerId, player] : players)
		{
			if ((player->position - currentItemPosition).length() < itemDistance)
			{
				if (occupierId == -1)
				{
					cnt++;
					occupierId = player->id;
				}
				else
				{
					cnt++;
					break;
				}
			}
		}

		if (cnt == 0)
		{
			itemState = ItemState::Idle;
			break;
		}

		if (cnt == 1)
		{
			currentOccupyTime += deltaTime;

			Protocol::S_FPS_ITEM_OCCUPY_PROGRESS_STATE progress;
			progress.set_occupyprogressstate(currentOccupyTime);
			sendBuffers->push_back(MakeSendBuffer(progress));

			if (currentOccupyTime >= totalOccupyTime)
			{
				itemState = ItemState::Occupied;

				this->occupierId = occupierId;

				Protocol::S_FPS_ITEM_OCCUPIED occupied;
				occupied.set_occupier(occupierId);
				sendBuffers->push_back(MakeSendBuffer(occupied));

				int destinationPositionIndex = rand() % (destinationPositions.size() + 1);
				currentDestinationPosition = destinationPositions[destinationPositionIndex];
				Protocol::S_FPS_SPAWN_DESTINATION destPkt;
				destPkt.set_allocated_position(ConvertVector3(currentDestinationPosition));
				sendBuffers->push_back(MakeSendBuffer(destPkt));
			}
		}

		break;
	}
	case ItemState::Occupied:
	{
		auto iter = players.find(occupierId);
		if (iter == players.end())
			break;

		auto& occupier = iter->second;

		if ((occupier->position - currentDestinationPosition).length() < destinationDistance)
		{
			scorerId = occupier->ownerId;
			itemState = ItemState::Scored;
		}

		break;
	}
	case ItemState::Scored:
	{
		scores[scorerId]++;
		scorerId = "";
		occupierId = -1;

		Protocol::S_FPS_SCORED scored;
		scored.set_scorer(scorerId);
		sendBuffers->push_back(MakeSendBuffer(scored));

		Protocol::S_FPS_DESTROY_DESTINATION destroyDestination;
		sendBuffers->push_back(MakeSendBuffer(destroyDestination));

		itemState = ItemState::Respawning;

		int itemPositionIndex = rand() % (itemPositions.size() + 1);
		delayedJobs.push_back(
			DelayPost(respawnTime, &FPSRoom::SpawnItem, itemPositions[itemPositionIndex])
		);

		break;
	}
	}
}

void FPSRoom::UpdatePlayerState(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers)
{
	for (auto iter = players.begin(); iter != players.end();)
	{
		auto& [playerId, player] = *iter;

		if (player->hp <= 0)
		{
			Protocol::S_REMOVE_GAME_OBJECT remove;
			remove.add_gameobjects(playerId);
			sendBuffers->push_back(MakeSendBuffer(remove));

			dynamicsWorld->removeCollisionObject(player->collisionObject.get());

			{
				auto iter = clients.find(player->ownerId);
				if (iter != clients.end())
				{
					auto owner = std::static_pointer_cast<FPSClient>(iter->second);
					owner->player = nullptr;
					delayedJobs.push_back(
						DelayPost(respawnTime, &FPSRoom::InstantiatePlayer, owner, btVector3(0, 0, 0), btQuaternion(0, 0, 0, 1))
					);
				}
			}

			iter = players.erase(iter);
		}
		else
			iter++;
	}
}

void FPSRoom::UpdateScore()
{
	for (auto& [scorer, score] : scores)
	{
		if (score >= scoreToWin)
		{
			Post(&FPSRoom::FinishGame);
			return;
		}
	}
}

void FPSRoom::UpdateTransform()
{
	for (auto& [playerId, player] : players)
	{
		if (!player->velocity.isZero())
		{
			int timeGap = currentUpdateTime - player->timestamp;
			btVector3 newPosition = player->position + player->velocity * timeGap;
			player->transform.setOrigin(newPosition);
		}

		player->collisionObject->setWorldTransform(player->transform);
	}

	dynamicsWorld->updateAabbs();
}

std::shared_ptr<ClientBase> FPSRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<FPSClient>(clientId);
	client->SetSession(session);
	return client;
}

void FPSRoom::LoadMap()
{
	//size x, y, z, position x, y, z, rotation x, y, z, w
	std::vector<tuple<float, float, float, float, float, float, float, float, float, float>> mapData = 
	{
		{ 30, 0, 30, 0, 0, 0, 0, 0, 0, 1 },

		{ 0.191482, 0.1096047, 0.1096047, 4.853, 0.717, 11.567, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 7.826, 1.007, 11.426, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 4.178, 0.553, 11.426, 0, -1, 0, 0 },
		{ 2, 1.2, 0.15, 6, 1.2, 11.5, 0, -1, 0, 0 },
		{ 2, 1.2, 0.15, -6, 1.2, -11.5, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, 1.79482, 0.3510164, 6.906516, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, -4.853, 0.717, 11.433, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, -7.826, 1.007, 11.574, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, -4.178, 0.553, 11.574, 0, 0, 0, 1 },
		{ 2, 1.2, 0.15, -6, 1.2, 11.5, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, 0.176, 0.3480164, -0.07894011, 0, -0.1448502, 0, 0.9894537 },
		{ 5.000001, 2.5, 0.1820844, -10, 2.5, 10, 0, -0.7071068, 0, 0.7071068 },
		{ 0.11687, 0.1773408, 0.11687, -1.48, 1.712994, 10.466, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, -0.271, 1.084016, -0.07894011, 0, 0.02456351, 0, 0.9996983 },
		{ 0.3011319, 2.625, 0.3011319, 9.8, 2.625, 14.8, 0, 0, 0, 1 },
		{ 0.3399515, 1.75, 0.05141113, 4.07556, 1.75, 6.772652, -0.03169185, 0, 0, 0.9994977 },
		{ 5, 2.5, 0.1820843, -5, 2.5, -15, 0, 0, 0, 1 },
		{ 0.3011319, 2.625, 0.3011319, -9.8, 2.625, 5, 0, 0, 0, 1 },
		{ 0.3011319, 2.625, 0.3011319, 0, 2.625, -14.8, 0, 0, 0, 1 },
		{ 0.5173578, 0.0166529, 0.07359252, -1.037, -9.313226E-10, -4.027029, 0, -0.7226786, 0, 0.6911843 },
		{ 0.371776, 0.3700715, 0.3718254, 1.842, 3.856016, 2.589, 0, -0.9238248, 0, 0.3828156 },
		{ 0.191482, 0.1096047, 0.1096047, -7.503, 1.54, -6.167, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, -7.058581, 1.03946, -6.034, 0, 0, 0, 1 },
		{ 0.7762985, 1.1, 0.1720278, -7.2, 1.1, -6.1, 0, 0, 0, 1 },
		{ 2, 1.2, 0.15, 0, 1.2, 10.5, 0, 0, 0, 1 },
		{ 0.3011319, 2.625, 0.3011319, -9.8, 2.625, 14.8, 0, 0, 0, 1 },
		{ 0.371776, 0.3700715, 0.3718254, -1.884, 1.096016, -6.96, 0, 0.6648194, 0, 0.7470042 },
		{ 0.1504328, 0.2477243, 0.1504328, -6.683976, 2.447724, -6.134164, 0, 0, 0, 1 },
		{ 0.3399517, 1.75, 0.05141117, -4.062254, 1.75, -6.806196, 0, -0.9991705, -0.04072312, 0 },
		{ 2.579929, 1.75, 2.662793, 4, 1.75, 4, 0, -0.7071058, 0, 0.7071078 },
		{ 0.5163507, 0.0166529, 0.07622095, 1.11425, -9.313226E-10, 3.9936, 0, 0.6918522, 0, 0.7220392 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.936999, 0.86, 3.48, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.936999, 1.102, 3.269001, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 3.48, 0.36, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 3.722, 0.5770009, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 1.618136, -3.926693, 0, 0.7071068, 0, 0.7071068 },
		{ 5.000001, 2.5, 0.1820844, -10, 2.5, 0, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1233001, 0.187098, 0.1233001, -0.404, 1.623994, 0.04700012, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 6.897, 1.54, 6.033, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 7.341418, 1.03946, 6.166, 0, 0, 0, 1 },
		{ 0.7762985, 1.1, 0.1720278, 7.2, 1.1, 6.1, 0, 0, 0, 1 },
		{ 0.371776, 0.3700715, 0.3718255, -2.659, 3.856016, -2.045, 0, -0.973072, 0, 0.2305017 },
		{ 0.3011319, 2.625, 0.3011319, -9.8, 2.625, -14.8, 0, 0, 0, 1 },
		{ 5.000001, 2.5, 0.1820844, 10, 2.5, -10, 0, -0.7071068, 0, 0.7071068 },
		{ 1.774077, 0.0166529, 0.07162117, -0.123, 3.505, 5.058, 0, 0, 0, 1 },
		{ 1.730096, 0.0166529, 0.07359251, -0.09, 3.516, -5.309866, 0, 0, 0, 1 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.936999, 3.24, -6.52, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.936999, 3.48, -6.730999, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 1.39, -9.64, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 1.63, -9.422999, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 4.55, -13.92669, 0, 0.7071068, 0, 0.7071068 },
		{ 5.000001, 2.5, 0.1820844, -10, 2.5, -10, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1248592, 0.2056112, 0.1248592, -1.261, 1.729724, 10.514, 0, 0, 0, 1 },
		{ 1.774077, 0.0166529, 0.07162116, -0.123, 3.505, 5.45, 0, -0.9998844, 0, 0.01520674 },
		{ 0.7762985, 1.1, 0.1720278, 7.2, 1.1, -6.1, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, -2.785, 3.846016, 4.099, 0, 0, 0, 1 },
		{ 0.1262146, 0.2078432, 0.1262146, -1.323, 2.447724, -1.798, 0, 0, 0, 1 },
		{ 0.5163508, 0.0166529, 0.07622096, -1.26175, -9.313226E-10, -4.0444, 0, 0.7336196, 0, 0.6795604 },
		{ 1.774077, 0.0166529, 0.07162116, -0.123, 3.505, -5.104, 0, -0.9998844, 0, 0.01520674 },
		{ 2.579929, 1.75, 2.662793, 4, 1.75, -4, 0, 1, 0, 2.950429E-06 },
		{ 0.3717759, 0.3700715, 0.3718253, -1.884, 0.3510164, -6.960001, 0, 0, 0, 1 },
		{ 0.1914821, 0.1096047, 0.1096047, 9.937, 0.86, -3.48, 0, -0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, 9.937, 1.102, -3.269001, 0, -0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, 9.937, 3.48, -0.3600001, 0, -0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, 9.937, 3.722, -0.5770009, 0, -0.7071068, 0, 0.7071068 },
		{ 0.1914821, 0.1096047, 0.1096047, 9.936999, 1.618136, 3.926693, 0, -0.7071068, 0, 0.7071068 },
		{ 5.000001, 2.5, 0.1820844, 10, 2.5, 0, 0, -0.7071068, 0, 0.7071068 },
		{ 0.191482, 0.1096047, 0.1096047, -2.153, 1.702, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, -1.982, 1.469, 14.94, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, -7.81, 3.52, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, -7.59, 3.76, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, -5.99, 1.08, 14.937, 0, -1, 0, 0 },
		{ 5, 2.5, 0.1820843, -5, 2.5, 15, 0, -1, 0, 0 },
		{ 0.5173578, 0.0166529, 0.07359252, 1.324, -9.313226E-10, 4.005971, 0, -0.7226786, 0, 0.6911843 },
		{ 0.3717759, 0.3700715, 0.3718253, 2.44, 3.856016, 1.93, 0, 0.1903553, 0, 0.9817153 },
		{ 0.3717759, 0.3700715, 0.3718253, 1.938883, 3.846016, -4.357198, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 7.33, 0.86, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 9.26, 2.95, 14.94, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 3.73, 1.83, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 3.95, 2.07, 14.937, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, 0.9099998, 3.79, 14.937, 0, -1, 0, 0 },
		{ 5, 2.5, 0.1820843, 5, 2.5, 15, 0, -1, 0, 0 },
		{ 0.3011319, 2.625, 0.3011319, 9.8, 2.625, -5, 0, 0, 0, 1 },
		{ 0.5163507, 0.0166529, 0.07622095, 1.46725, 0.491, 10.6936, -0.5134605, 0.486167, -0.5134605, 0.486167 },
		{ 5.000001, 2.5, 0.1820844, 10, 2.5, 10, 0, -0.7071068, 0, 0.7071068 },
		{ 0.191482, 0.1096047, 0.1096047, -1.197, 1.54, -1.735, 0, -1, 0, 0 },
		{ 0.191482, 0.1096047, 0.1096047, -1.027, 0.484, -1.868, 0, -1, 0, 0 },
		{ 0.7762985, 1.1, 0.1720278, -1.5, 1.1, -1.802, 0, -1, 0, 0 },
		{ 0.3011319, 2.625, 0.3011319, -9.8, 2.625, -5, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 1.197, 1.54, 1.785, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 0.915, 0.477, 1.918, 0, 0, 0, 1 },
		{ 0.7762985, 1.1, 0.1720278, 1.5, 1.1, 1.852, 0, 0, 0, 1 },
		{ 2, 1.2, 0.15, 6, 1.2, -11.5, 0, 0, 0, 1 },
		{ 2.579929, 1.75, 2.662793, -4, 1.75, -4, 0, 0.7071058, 0, 0.7071078 },
		{ 0.3011319, 2.625, 0.3011319, 9.8, 2.625, -14.8, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 1.52, 0.86, -14.937, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 1.731, 1.102, -14.937, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 4.64, 3.48, -14.937, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 4.422999, 3.722, -14.937, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 8.926693, 1.618136, -14.937, 0, 0, 0, 1 },
		{ 5, 2.5, 0.1820843, 5, 2.5, -15, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, 2.308, 4.564016, -4.241, 0, 0.2076412, 0, 0.9782051 },
		{ 0.3717759, 0.3700715, 0.3718253, 2.812, 3.846016, -4.382, 0, -0.09146509, 0, 0.9958083 },
		{ 0.191482, 0.1096047, 0.1096047, 1.147, 0.717, -10.567, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, -1.826, 1.007, -10.426, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 1.822, 0.553, -10.426, 0, 0, 0, 1 },
		{ 2, 1.2, 0.15, 0, 1.2, -10.5, 0, 0, 0, 1 },
		{ 2.579929, 1.75, 2.662793, -4, 1.75, 4, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, -1.893, 3.856016, -2.523, 0, -0.1702681, 0, 0.9853978 },
		{ 0.3011319, 2.625, 0.3011319, 9.8, 2.625, 5, 0, 0, 0, 1 },
		{ 0.191482, 0.1096047, 0.1096047, 1.825, 0.794, 10.563, 0, 0, 0, 1 },
		{ 0.371776, 0.3700715, 0.3718254, 1.79482, 1.096016, 6.906516, 0, 0.6648194, 0, 0.7470042 },
		{ 0.3011319, 2.625, 0.3011319, 0, 2.625, 14.8, 0, 0, 0, 1 },
		{ 0.5494488, 0.01772036, 0.08110671, 1.246002, 0.534, 10.7226, 0.5423486, 0.4910519, -0.4992591, -0.4641837 },
		{ 0.3717759, 0.3700715, 0.3718253, -0.7139103, 0.3480164, -0.07894011, 0, 0.02456351, 0, 0.9996983 },
		{ 0.1504328, 0.2477243, 0.1504328, -0.118, 1.648724, -0.236, 0, 0, 0, 1 },
		{ 0.1914821, 0.1096047, 0.1096047, -9.937, 1.63, -9.842, 0, 0.7071068, 0, 0.7071068 },
		{ 0.1166161, 0.1769555, 0.1166161, 1.375, 2.423994, 1.8614, 0, 0, 0, 1 },
		{ 1.774077, 0.0166529, 0.07162117, -0.123, 3.505, -5.496, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, -1.911883, 3.846016, 4.215199, 0, -0.09146509, 0, 0.9958083 },
		{ 1.730096, 0.0166529, 0.07359251, -0.09, 3.516, 5.244134, 0, 0, 0, 1 },
		{ 0.3717759, 0.3700715, 0.3718253, -2.415883, 4.564016, 4.215199, 0, 0.2076412, 0, 0.9782051 },
		{ 0.7762985, 1.1, 0.1720278, -7.2, 1.1, 6.1, 0, 0, 0, 1 },
	};

	for (int i = 0; i < mapData.size(); i++)
	{
		auto& [sizeX, sizeY, sizeZ, posX, posY, posZ, rotX, rotY, rotZ, rotW] = mapData[i];

		btCollisionShape* boxShape = new btBoxShape(btVector3(sizeX, sizeY, sizeZ));
		btCollisionObject* boxCollisionObject = new btCollisionObject();

		boxCollisionObject->setUserIndex(idGenerator++);

		boxCollisionObject->setCollisionShape(boxShape);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(posX, posY, posZ));
		transform.setRotation(btQuaternion(rotX, rotY, rotZ, rotW));
		boxCollisionObject->setWorldTransform(transform);

		dynamicsWorld->addCollisionObject(boxCollisionObject);
	}
}

#if _WIN32
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ThreadManager.h>

#include "DebugDrawer.h"

void FPSRoom::Draw()
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = shoots_draw.size() - 1; i >= 0; i--)
	{
		while (!shoots_draw[i].empty())
		{
			auto start = shoots_draw[i].front().first;
			auto end = shoots_draw[i].front().second;
			shoots_draw[i].pop();

			if (i + 1 != shoots_draw.size())
				shoots_draw[i + 1].push({ start, end });

			glVertex3f(start.x(), start.y(), start.z());
			glVertex3f(end.x(), end.y(), end.z());
		}
	}

	glEnd();

	dynamicsWorld->debugDrawWorld();
}

double lastX = 320, lastY = 240;
double yaw = -90.0f, pitch = 0.0f;
float cameraSpeed = 0.2f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void FPSRoom::InitDraw()
{
	BulletDebugDrawer* myDebugDrawer = new BulletDebugDrawer();
	myDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld->setDebugDrawer(myDebugDrawer);

	GThreadManager->Launch([this]()
		{
			if (!glfwInit()) {
				return -1;
			}

			GLFWwindow* window = glfwCreateWindow(960, 540, "Bullet Debug Draw", NULL, NULL);
			if (!window) {
				glfwTerminate();
				return -1;
			}

			glfwMakeContextCurrent(window);

			glEnable(GL_DEPTH_TEST);

			glViewport(0, 0, 1280, 720);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(40.0, 1280.0 / 720.0, 0.1, 1000.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(0, 2, -2, 0, 0, 10, 0, 1, 0);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
				double xoffset = xpos - lastX;
				double yoffset = lastY - ypos;
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.1f;
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				yaw += xoffset;
				pitch += yoffset;

				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front;
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);
				});

			while (roomState != RoomState::Idle)
			{
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					cameraPos += cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					cameraPos -= cameraSpeed * cameraFront;
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
					cameraPos.y += cameraSpeed;
				if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
					cameraPos.y -= cameraSpeed;

				glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glLoadIdentity();
				glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
				glLoadMatrixf(glm::value_ptr(view));

				Draw();

				glfwSwapBuffers(window);
				glfwPollEvents();

				std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 / 60 });
			}

			glfwDestroyWindow(window);
			glfwTerminate();
		});
}
#endif