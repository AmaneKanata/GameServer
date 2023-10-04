#pragma once

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

	virtual void RemovePlayer(int playerId);

	void Draw();

	void Update();

protected:
	virtual void Handle_C_INSTANTIATE_FPS_PLAYER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt) override;
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt) override;
	virtual void Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt) override;
	virtual void Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt) override;
	virtual void Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt) override;

	virtual void Handle_C_SHOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOT> pkt) override;

	virtual std::shared_ptr<ClientBase> MakeClient(string clientId, std::shared_ptr<GameSession> session) override;

private:
	void LoadMap();
#if _WIN32
	void InitDraw();
#endif

public:
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

private:
	long long lastUpdated = 0;

	int idGenerator = 0;
	std::map<int, std::shared_ptr<FPSPlayer>> players;

	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
};