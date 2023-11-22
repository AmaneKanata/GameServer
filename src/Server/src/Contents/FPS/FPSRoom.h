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
	virtual void Handle_C_FPS_LOAD_COMPLETE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_LOAD_COMPLETE> pkt) override;
	
	virtual void Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt) override;
	virtual void Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt) override;
	virtual void Handle_C_FPS_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ANIMATION> pkt) override;
	
	virtual void Handle_C_SHOOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOOT> pkt) override;
	virtual void Handle_C_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_RELOAD> pkt) override;
	virtual void Handle_C_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_WEAPON> pkt) override;

private:
	void InstantiatePlayer(std::shared_ptr<FPSClient> client, btVector3 position, btQuaternion rotation);
	void RemovePlayer(int playerId);

	void Update();

	void SpawnItem(btVector3 position);

	void InitGame();
	void StartGame();

	void LoadMap();	

#if _WIN32
	void InitDraw();
	void Draw();
#endif

private:
	RoomState roomState = RoomState::Idle;

	int idGenerator = 0;
	std::map<int, std::shared_ptr<FPSPlayer>> players;

	vector<queue<pair<btVector3, btVector3>>> shots;

	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;

	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	float rayDistance = 1000;
	int damage = 10;

	btVector3 itemPosition;
	ItemState itemState = ItemState::Idle;
	int totalOccupyTime = 3000;
	int currentOccupyTime = 0;
	int occupyTimeCap = 1500;
	int occupyDistance = 3;
	int occupierId = -1;

	btVector3 destination;
};