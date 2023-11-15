#pragma once

#include <queue>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <json.hpp>

#include "RoomBase.h"

class FPSPlayer;
class FPSClient;

class FPSRoom : public RoomBase
{
public:
	FPSRoom(boost::asio::io_context& ioc) : RoomBase(ioc)
	{}

	virtual void HandleInit() override;
	virtual void HandleClose() override;

	virtual void Leave(std::shared_ptr<ClientBase> client, std::string code) override;

	void Update();

#if _WIN32
	void Draw();
#endif

protected:
	virtual void Handle_C_INSTANTIATE_FPS_PLAYER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt) override;
	virtual void Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt) override;
	virtual void Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt) override;
	virtual void Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt) override;
	
	virtual void Handle_C_FIRE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FIRE> pkt) override;
	virtual void Handle_C_LOOK(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_LOOK> pkt) override;
	virtual void Handle_C_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_RELOAD> pkt) override;
	virtual void Handle_C_LEAN(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_LEAN> pkt) override;
	virtual void Handle_C_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_WEAPON> pkt) override;
	virtual void Handle_C_AIM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_AIM> pkt) override;

	virtual void Handle_C_SHOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOT> pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId, std::shared_ptr<GameSession> session) override;

private:
	void InstantiatePlayer(std::shared_ptr<FPSClient> client, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt);

	void LoadMap();
#if _WIN32
	void InitDraw();
#endif

	virtual void RemovePlayer(int playerId);

public:
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

private:
	long long lastUpdated = 0;

	int idGenerator = 0;
	std::map<int, std::shared_ptr<FPSPlayer>> players;

	vector<queue<pair<btVector3, btVector3>>> shots;

	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
};