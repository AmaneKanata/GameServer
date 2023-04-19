#include "PacketManager.h"
#include "Client/Client.h"
#include "Network/GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(shared_ptr<GameSession>& session, unsigned char* buffer, int len)
{
	return true;
}

bool Handle_S_ENTER(shared_ptr<GameSession>& session, Protocol::S_ENTER& pkt)
{
	auto client = session->owner;

	GLogManager->Log("Client ", client->clientId, " S_ENTER : ", pkt.result());

	Protocol::C_INSTANTIATE_GAME_OBJECT instantiate;
	auto position = instantiate.mutable_position();
	position->set_x(client->position_x);
	position->set_y(client->position_y);
	position->set_z(client->position_z);
	client->Send(PacketManager::MakeSendBuffer(instantiate));

	return true;
}

bool Handle_S_REENTER(shared_ptr<GameSession>& session, Protocol::S_REENTER& pkt)
{
	return true;
}

bool Handle_S_ADD_CLIENT(shared_ptr<GameSession>& session, Protocol::S_ADD_CLIENT& pkt)
{
	auto client = session->owner;

	string log = "";

	log.append("Client ").append(client->clientId).append(" S_ADD_CLIENT : ");

	for (int i = 0; i < pkt.clientinfos_size(); i++)
	{
		auto clientInfo = pkt.clientinfos(i);
		log.append(clientInfo.clientid()).append(" ");
	}

	GLogManager->Log(log);

	return true;
}

bool Handle_S_REMOVE_CLIENT(shared_ptr<GameSession>& session, Protocol::S_REMOVE_CLIENT& pkt)
{
	auto client = session->owner;

	string log = "";

	log.append("Client ").append(client->clientId).append(" S_REMOVE_CLIENT : ");

	for (int i = 0; i < pkt.clientids_size(); i++)
		log.append(pkt.clientids(i)).append(" ");

	GLogManager->Log(log);

	return true;
}

bool Handle_S_DISCONNECT(shared_ptr<GameSession>& session, Protocol::S_DISCONNECT& pkt)
{
	auto client = session->owner;

	GLogManager->Log("Client ", client->clientId, " S_DISCONNECTED : ", pkt.code());

	return true;
}

bool Handle_S_INSTANTIATE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_INSTANTIATE_GAME_OBJECT& pkt)
{
	auto client = session->owner;

	GLogManager->Log("Client ", client->clientId, " S_INSTANTIATE_OBJECT : ", to_string(pkt.gameobjectid()));

	client->objectId = pkt.gameobjectid();

	return true;
}

bool Handle_S_ADD_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_ADD_GAME_OBJECT& pkt)
{
	auto client = session->owner;

	string log = "";

	log.append("Client ").append(client->clientId).append(" S_ADD_OBJECT : ");

	for (int i = 0; i < pkt.gameobjects_size(); i++)
	{
		auto gameObjectInfo = pkt.gameobjects(i);
		log.append("\n").append("	").append(to_string(gameObjectInfo.id()));
		log.append("\n").append("	")
			.append(to_string(gameObjectInfo.position().x())).append(" ")
			.append(to_string(gameObjectInfo.position().y())).append(" ")
			.append(to_string(gameObjectInfo.position().z())).append(" ");
	}

	GLogManager->Log(log);

	return true;
}

bool Handle_S_REMOVE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_REMOVE_GAME_OBJECT& pkt)
{
	auto client = session->owner;

	string log = "";

	log.append("Client ").append(client->clientId).append(" S_REMOVE_OBJECT : ");
	for (int i = 0; i < pkt.gameobjects_size(); i++)
		log.append(to_string(pkt.gameobjects(i))).append(" ");

	GLogManager->Log(log);

	return true;
}

bool Handle_S_SET_TRANSFORM(shared_ptr<GameSession>& session, Protocol::S_SET_TRANSFORM& pkt)
{
	auto client = session->owner;

	GLogManager->Log("Client ", client->clientId, " BASE_SET_TRANSFORM : ",
		to_string(pkt.gameobjectid()), " ",
		to_string(pkt.position().x()), " ",
		to_string(pkt.position().y()), " ",
		to_string(pkt.position().z()), " "
	);

	return true;
}
