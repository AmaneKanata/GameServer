#include <vector>

#include "FPSRoom.h"
#include "FPSObject.h"
#include "FPSClient.h"
#include "GameSession.h"

void FPSRoom::HandleInit()
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

void FPSRoom::Update()
{
	long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	for(auto& player : players)
	{
		if (player.second->isRotationDirty)
			sendBuffers->push_back(MakeSendBuffer(player.second->setRotation));
		
		if (player.second->velocity.isZero())
			continue;

		int timeGap = now - player.second->timestamp;

		btVector3 newPosition = player.second->position + player.second->velocity * timeGap;

		player.second->transform.setOrigin(newPosition);
		player.second->collisionObject->setWorldTransform(player.second->transform);
	}

	dynamicsWorld->updateAabbs();

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

	{
		Protocol::S_INSTANTIATE_GAME_OBJECT res;
		res.set_gameobjectid(player->id);
		res.set_success(true);
		session->Send(MakeSendBuffer(res));
	}

	{
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
}

void FPSRoom::Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt)
{
	Protocol::S_ADD_FPS_PLAYER res;

	for (auto& player : players)
	{
		auto playerInfo = res.add_gameobjects();
		playerInfo->mutable_position()->CopyFrom(player.second->setPosition.position());
		playerInfo->mutable_velocity()->CopyFrom(player.second->setPosition.velocity());
		playerInfo->mutable_rotation()->CopyFrom(player.second->setRotation.rotation());
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
	auto client = static_pointer_cast<FPSClient>(session->client);
	if (client->player == nullptr)
		return;

	Protocol::S_SHOT res;
	res.set_playerid(client->player->id);
	Broadcast(MakeSendBuffer(res));

	btVector3 from(pkt->position().x() * -1, pkt->position().y(), pkt->position().z());
	btVector3 direction(pkt->direction().x() * -1, pkt->direction().y(), pkt->direction().z());
	btVector3 to = from + direction * rayDistance;

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
	dynamicsWorld->rayTest(from, to, rayCallback);

	if(rayCallback.hasHit())
	{
		shots[0].push({from, rayCallback.m_hitPointWorld});

		auto id = rayCallback.m_collisionObject->getUserIndex();

		GLogManager->Log("HIt : ", std::to_string(id));

		auto player = players.find(id);
		if (player == players.end())
			return;

		player->second->hp -= damage;

		if(player->second->hp == 0)
		{
			dynamicsWorld->removeCollisionObject(player->second->collisionObject.get());
			players.erase(player);
		}

		Protocol::S_ATTACKED res;
		res.set_playerid(player->second->id);
		res.set_damage(damage);
		res.set_hp(player->second->hp);
		Broadcast(MakeSendBuffer(res));
	}
	else
	{
		shots[0].push({from, to});
	}
}

std::shared_ptr<ClientBase> FPSRoom::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<FPSClient>(clientId);
	client->SetSession(session);
	return client;
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

			if(i + 1 != shots.size())
				shots[i + 1].push({start, end});

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

			GLFWwindow* window = glfwCreateWindow(1280, 720, "Bullet Debug Draw", NULL, NULL);
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