#include <vector>

#include "FPSRoom.h"
#include "FPSObject.h"
#include "FPSClient.h"
#include "GameSession.h"

void FPSRoom::HandleInit() 
{
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
		InitGame();

		roomState = RoomState::Loading;

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

	player->second->position = ConvertVector3(pkt->position()) + btVector3(0, 1, 0);
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
	itemPosition = position;
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
	float positionX = -5.0f;
	int cnt = 0;
	for (auto& [clientId, client] : clients)
	{
		btVector3 position(positionX + cnt * 10, 0, 0);
		btQuaternion rotation(0, 0, 0, 1);
		InstantiatePlayer(std::static_pointer_cast<FPSClient>(client), position, rotation);
		cnt++;
	}

	prevUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	Post(&FPSRoom::Update);
	
	delayedJobs.push_back(
		DelayPost(3000, &FPSRoom::SpawnItem, btVector3(0, 1, 7))
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
			if ((player->position - itemPosition).length() < itemDistance)
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
			if ((player->position - itemPosition).length() < itemDistance)
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

				destination.setValue(0, 1, -7);
				Protocol::S_FPS_SPAWN_DESTINATION destPkt;
				destPkt.set_allocated_position(ConvertVector3(destination));
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

		if ((occupier->position - destination).length() < destinationDistance)
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

		delayedJobs.push_back(
			DelayPost(respawnTime, &FPSRoom::SpawnItem, btVector3(0, 1, 7))
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

			while (roomState == RoomState::Playing)
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