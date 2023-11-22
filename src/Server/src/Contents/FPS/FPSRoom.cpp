#include <vector>

#include "FPSRoom.h"
#include "FPSObject.h"
#include "FPSClient.h"
#include "GameSession.h"

void FPSRoom::HandleInit() {}

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

void FPSRoom::Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt)
{
	if (roomState != RoomState::Idle)
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

	if (clients.size() == 2)
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

	btVector3 position(0, 0, 0);
	btQuaternion rotation(0, 0, 0, 1);

	InstantiatePlayer(client, position, rotation);

	client->isLoaded = true;


	bool allLoaded = true;
	{
		for (auto& client : clients)
		{
			auto fClient = static_pointer_cast<FPSClient>(client.second);
			if (!fClient->isLoaded)
			{
				allLoaded = false;
				break;
			}
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

void FPSRoom::Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	auto position = ConvertVector3(pkt->position());
	position.setY(position.y() + 1.0f);
	player->second->position = position;
	player->second->velocity = ConvertVector3(pkt->velocity());
	player->second->timestamp = pkt->timestamp();

	Protocol::S_SET_FPS_POSITION res;
	res.set_playerid(player->second->id);
	res.set_allocated_position(pkt->release_position());
	res.set_allocated_velocity(pkt->release_velocity());
	res.set_timestamp(pkt->timestamp());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	player->second->transform.setRotation(ConvertQuaternion(pkt->rotation()));

	Protocol::S_SET_FPS_ROTATION res;
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

void FPSRoom::Handle_C_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_RELOAD> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	Protocol::S_RELOAD res;
	res.set_playerid(player->second->id);

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_WEAPON> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	auto player = players.find(client->player->id);
	if (player == players.end())
		return;

	Protocol::S_CHANGE_WEAPON res;
	res.set_playerid(player->second->id);
	res.set_weaponid(pkt->weaponid());
	res.set_timestamp(pkt->timestamp());

	Broadcast(MakeSendBuffer(res));
}

void FPSRoom::Handle_C_SHOOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOOT> pkt)
{
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	Protocol::S_SHOOT res;
	res.set_playerid(client->player->id);
	Broadcast(MakeSendBuffer(res));

	btVector3 from(pkt->position().x() * -1, pkt->position().y(), pkt->position().z());
	btVector3 direction(pkt->direction().x() * -1, pkt->direction().y(), pkt->direction().z());
	btVector3 to = from + direction * rayDistance;

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	dynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		shots[0].push({ from, rayCallback.m_hitPointWorld });

		auto id = rayCallback.m_collisionObject->getUserIndex();

		auto player = players.find(id);
		if (player == players.end())
			return;

		player->second->hp -= damage;

		if (player->second->hp == 0)
		{
			Protocol::S_REMOVE_GAME_OBJECT remove;
			remove.add_gameobjects(player->second->id);
			Broadcast(MakeSendBuffer(remove));

			{
				auto owner = clients.find(player->second->ownerId);
				if (owner != clients.end())
					DelayPost(5000, 
						&FPSRoom::InstantiatePlayer, 
						std::static_pointer_cast<FPSClient>(owner->second),
						btVector3(0, 0, 0),
						btQuaternion(0, 0, 0, 1)
					);
			}

			dynamicsWorld->removeCollisionObject(player->second->collisionObject.get());
			players.erase(player);

			if (player->second->id == occupierId)
			{
				GLogManager->Log("Player ", std::to_string(client->player->id), " Scored!");
				itemState = ItemState::Respawning;
				DelayPost(5000, &FPSRoom::SpawnItem, btVector3(0, 1, 7));
				occupierId = -1;
			}
		}

		Protocol::S_ATTACKED res;
		res.set_playerid(player->second->id);
		res.set_damage(damage);
		res.set_hp(player->second->hp);
		Broadcast(MakeSendBuffer(res));
	}
	else
	{
		shots[0].push({ from, to });
	}
}

void FPSRoom::InstantiatePlayer(std::shared_ptr<FPSClient> client, btVector3 position, btQuaternion rotation)
{
	auto player = std::make_shared<FPSPlayer>(client->clientId, idGenerator++, position, rotation);

	players.insert({ player->id, player });

	client->player = player;

	dynamicsWorld->addCollisionObject(player->collisionObject.get());

	{
		Protocol::S_INSTANTIATE_GAME_OBJECT res;
		res.set_gameobjectid(player->id);
		res.set_success(true);
		client->Send(MakeSendBuffer(res));
	}

	{
		Protocol::S_ADD_FPS_PLAYER res;
		auto playerInfo = res.add_gameobjects();
		playerInfo->set_allocated_position(ConvertVector3(position));
		playerInfo->set_allocated_rotation(ConvertQuaternion(rotation));
		playerInfo->set_playerid(player->id);
		playerInfo->set_hp(player->hp);
		playerInfo->set_ownerid(player->ownerId);

		Broadcast(MakeSendBuffer(res));
	}
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

void FPSRoom::SpawnItem(btVector3 position)
{
	itemPosition = position;
	itemState = ItemState::Idle;
	currentOccupyTime = 0;

	Protocol::S_FPS_SPAWN_ITEM spawn;
	auto itemPosition = spawn.mutable_position();
	itemPosition->set_x(position.x());
	itemPosition->set_y(position.y());
	itemPosition->set_z(position.z());
	Broadcast(MakeSendBuffer(spawn));
}

void FPSRoom::InitGame()
{
	shots = vector<queue<pair<btVector3, btVector3>>>(2);

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
	Post(&RoomBase::SendServerTime);
	Post(&FPSRoom::Update);

	Protocol::S_FPS_ANNOUNCE anno;
	anno.set_message("Item Spawn after 5 Second");
	Broadcast(MakeSendBuffer(anno));
	
	DelayPost(5000, &FPSRoom::SpawnItem, btVector3(0, 1, 7));
}

void FPSRoom::Update()
{
	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	switch (itemState)
	{
	case ItemState::Idle:
	{
		int cnt = 0;
		for (auto& player : players)
		{
			if ((player.second->position - itemPosition).length() < occupyDistance)
				cnt++;
		}

		if (cnt > 0)
			itemState = ItemState::BeingOccupied;
		else
		{
			int prevOccupyTime = currentOccupyTime;

			if (currentOccupyTime >= occupyTimeCap)
				currentOccupyTime = max(currentOccupyTime - 50, occupyTimeCap);
			else
				currentOccupyTime = max(currentOccupyTime - 50, 0);

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
		std::shared_ptr<FPSPlayer> occupier = nullptr;
		int cnt = 0;
		for (auto& player : players)
		{
			if ((player.second->position - itemPosition).length() < occupyDistance)
			{
				if (occupier == nullptr)
				{
					cnt++;
					occupier = player.second;
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
			currentOccupyTime += 50;

			{
				Protocol::S_FPS_ITEM_OCCUPY_PROGRESS_STATE progress;
				progress.set_occupyprogressstate(currentOccupyTime);
				sendBuffers->push_back(MakeSendBuffer(progress));
			}

			if (currentOccupyTime >= totalOccupyTime)
			{
				itemState = ItemState::Occupied;

				occupierId = occupier->id;

				Protocol::S_FPS_ITEM_OCCUPIED occupied;
				occupied.set_occupier(occupier->id);
				sendBuffers->push_back(MakeSendBuffer(occupied));
			}
		}

		break;
	}
	case ItemState::Occupied:
	{
		auto player = players.find(occupierId);
		if (player == players.end())
			break;

		if ((player->second->position - destination).length() < 1)
		{
			GLogManager->Log("Player ", std::to_string(occupierId), " Scored!");

			itemState = ItemState::Respawning;

			DelayPost(5000, &FPSRoom::SpawnItem, btVector3(0, 1, 7));

			occupierId = -1;
		}

		break;
	}
	}

	long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	for(auto& player : players)
	{	
		if (!player.second->velocity.isZero())
		{
			int timeGap = now - player.second->timestamp;
			btVector3 newPosition = player.second->position + player.second->velocity * timeGap;
			player.second->transform.setOrigin(newPosition);
		}

		player.second->UpdateTransform();
	}

	dynamicsWorld->updateAabbs();

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	DelayPost(50, &FPSRoom::Update);
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

	for (int i = shots.size() - 1; i >= 0; i--)
	{
		while (!shots[i].empty())
		{
			auto start = shots[i].front().first;
			auto end = shots[i].front().second;
			shots[i].pop();

			if (i + 1 != shots.size())
				shots[i + 1].push({ start, end });

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

			while (agones_state != "Shutdown")
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