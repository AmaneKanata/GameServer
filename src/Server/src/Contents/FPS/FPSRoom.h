#pragma once

#include <queue>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <json.hpp>

#include "Convert.h"

#include "RoomBase.h"

class FPSPlayer;
class FPSClient;

enum class ItemState
{
	Idle,
	BeingOccupied,
	Occupied,
	Scored,
	Respawning
};

enum class RoomState
{
	Idle,
	Loading,
	Playing
};

class FPSRoom : public RoomBase
{
public:
	FPSRoom(boost::asio::io_context& ioc) : RoomBase(ioc) {}

	virtual void HandleInit() override;
	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code) override;

protected:
	virtual std::shared_ptr<ClientBase> MakeClient(string clientId, std::shared_ptr<GameSession> session) override;

	virtual void Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt) override;
	virtual void Handle_C_FPS_READY(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_READY> pkt) override;
	virtual void Handle_C_FPS_LOAD_COMPLETE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_LOAD_COMPLETE> pkt) override;

	virtual void Handle_C_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_POSITION> pkt) override;
	virtual void Handle_C_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ROTATION> pkt) override;
	virtual void Handle_C_FPS_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ANIMATION> pkt) override;
	
	virtual void Handle_C_FPS_SHOOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_SHOOT> pkt) override;
	virtual void Handle_C_FPS_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_RELOAD> pkt) override;
	virtual void Handle_C_FPS_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_CHANGE_WEAPON> pkt) override;

private:
	void InstantiatePlayer(std::shared_ptr<FPSClient> client, btVector3 position, btQuaternion rotation);

	void Update();

	void SpawnItem(btVector3 position);

	void InitGame();
	void StartGame();
	void FinishGame();

	void UpdateShoot(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers);
	void UpdateItem(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers);
	void UpdatePlayerState(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>>& sendBuffers);
	void UpdateScore();
	void UpdateTransform();

	void LoadMap();

#if _WIN32
	void InitDraw();
	void Draw();
#endif

private:
	int updateInterval = 50;
	long long currentUpdateTime = 0;
	long long prevUpdateTime = 0;
	int deltaTime = 0;

	int idGenerator = 0;
	std::map<int, std::shared_ptr<FPSPlayer>> players;

	std::vector<std::shared_ptr<DelayedJob>> delayedJobs;

	RoomState roomState;

	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;

	float rayDistance = 1000;
	int damage = 10;

	ItemState itemState;

	btVector3 currentItemPosition;
	int itemDistance = 3;

	int totalOccupyTime = 3000;
	int occupyTimeCap = 1500;
	int currentOccupyTime;

	int occupierId;
	string scorerId;

	btVector3 currentDestinationPosition;
	int destinationDistance = 2;

	int scoreToWin = 3;
	std::map<string, int> scores;

	int respawnTime = 5000;

	std::vector<std::tuple<string, btVector3, btVector3>> shoots;

	std::vector<btVector3> itemPositions =
	{
		{4, 1, 0},
		{-4, 1, 0},
		{0, 1, 4},
		{0, 1, -4}
	};
	
	std::vector<btVector3> destinationPositions =
	{
		{8, 1, 13},
		{8, 1, -13},
		{-8, 1, 13},
		{-8, 1, -13}
	};

	std::vector<btVector3> spawnPositions =
	{
		{8, 0, 4},
		{-8, 0, -4},

		{8, 0, -4},
		{-8, 0, 4},

		{4, 0, 8},
		{-4, 0, -8},

		{4, 0, -8},
		{-4, 0, 8}
	};

#if _WIN32
	//draw
	vector<queue<pair<btVector3, btVector3>>> shoots_draw;
#endif
};