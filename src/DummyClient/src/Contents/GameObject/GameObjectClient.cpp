#include "GameObjectClient.h"
#include "DummyClient_Singleton.h"
#include "LogManager.h"

void GameObjectClient::InstantiateGameObject()
{
	Protocol::C_INSTANTIATE_GAME_OBJECT instantiate;

	auto position = instantiate.mutable_position();
	position->set_x(go.position_x);
	position->set_y(go.position_y);
	position->set_z(go.position_z);

	auto rotation = instantiate.mutable_position();
	rotation->set_x(0);
	rotation->set_y(0);
	rotation->set_z(0);

	Send(MakeSendBuffer(instantiate));
}

void GameObjectClient::Move()
{
	go.position_x++;

	Protocol::C_SET_TRANSFORM setTransform;
	auto position = setTransform.mutable_position();
	position->set_x(go.position_x);
	position->set_y(go.position_y);
	position->set_z(go.position_z);
	setTransform.set_gameobjectid(go.id);

	Send(MakeSendBuffer(setTransform));
}

void GameObjectClient::Handle_S_ENTER(std::shared_ptr<GameSession> session, Protocol::S_ENTER pkt)
{
	ClientBase::Handle_S_ENTER(session, pkt);

	Post(&GameObjectClient::InstantiateGameObject);
}

void GameObjectClient::Handle_S_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_INSTANTIATE_GAME_OBJECT pkt)
{
	go.id = pkt.gameobjectid();
}

void GameObjectClient::Handle_S_ADD_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_ADD_GAME_OBJECT pkt)
{
	std::stringstream ss;
	for (int i = 0; i < pkt.gameobjects_size(); i++)
	{
		ss << "\n	" 
			<< "Object Id : " 
			<< std::to_string(pkt.gameobjects()[i].id()) 
			<< ", Position : [ "
			<< pkt.gameobjects()[i].position().x() << ", "
			<< pkt.gameobjects()[i].position().y() << ". "
			<< pkt.gameobjects()[i].position().z() << " ]";
	}
	GLogManager->Log("[Client ", clientId, "]	Add GameObjects :", ss.str());
}

void GameObjectClient::Handle_S_REMOVE_GAME_OBJECT(std::shared_ptr<GameSession> session, Protocol::S_REMOVE_GAME_OBJECT pkt)
{
	std::stringstream ss;
	ss << "Object Ids : ";
	for (int i = 0; i < pkt.gameobjects_size(); i++)
	{
		ss << std::to_string(pkt.gameobjects()[i]) << " ";
	}
	GLogManager->Log("[Client ", clientId, "]	Remove GameObjects :\n	", ss.str());
}

void GameObjectClient::Handle_S_SET_TRANSFORM(std::shared_ptr<GameSession> session, Protocol::S_SET_TRANSFORM pkt)
{
	std::stringstream ss;
	GLogManager->Log("[Client ", clientId, "]	Set Transform :"
		, " Object Id : ", std::to_string(pkt.gameobjectid())
		, ", Position : [ "
		, std::to_string(pkt.position().x()), " "
		, std::to_string(pkt.position().y()), " "
		, std::to_string(pkt.position().z()), " ]"
	);
}