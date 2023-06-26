#include "RoomBase.h"
#include "ClientBase.h"
#include "GameSession.h"
#include "PacketManager.h"

#include <iostream>

void RoomBase::HandleInit()
{
	//Custom Init
}

void RoomBase::HandleClose()
{
	for (const auto& [key, client] : clients)
		client->Post(&ClientBase::Leave, string("Closing"));
}

void RoomBase::Handle_INVALID(std::shared_ptr<GameSession> session, unsigned char* buffer, int len)
{
	//GLogManager->Log("Invalid Packet");
}

void RoomBase::Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt)
{
	{
		auto client = clients.find(pkt->clientid());
		if (client != clients.end())
		{
			Post(&RoomBase::Leave, client->second, std::string("DUPLICATED"));
			Post(&RoomBase::Handle_C_ENTER, session, std::move(pkt));
			return;
		}
	}
	
	auto client = MakeClient(pkt->clientid());
	client->Post(&ClientBase::SetSession, session);

	clients.insert({ pkt->clientid(), client });
	GLogManager->Log("Client Added : ", pkt->clientid(), ", Client Number : ", std::to_string(clients.size()));

	Protocol::S_ENTER res;
	res.set_result("SUCCESS");
	client->Post(&ClientBase::Send, MakeSendBuffer(res));

	Protocol::S_ADD_CLIENT addClient;
	auto clientInfo = addClient.add_clientinfos();
	clientInfo->set_clientid(pkt->clientid());
	Broadcast(MakeSendBuffer(addClient));
}

void RoomBase::Handle_C_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_REENTER> pkt)
{
	auto client = clients.find(pkt->clientid());
	if (client == clients.end())
	{
		Protocol::S_REENTER res;
		res.set_success(false);
		session->Send(MakeSendBuffer(res));

		session->Disconnect();

		return;
	}

	client->second->Post(&ClientBase::ReEnter, session);
}

void RoomBase::Handle_C_LEAVE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_LEAVE> pkt)
{ 
	Leave(session->client, std::string("LEAVED"));
}

void RoomBase::Handle_C_GET_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_CLIENT> pkt)
{
	Protocol::S_ADD_CLIENT res;

	for (const auto& [key, client] : clients)
	{
		auto clientInfo = res.add_clientinfos();
		clientInfo->set_clientid(client->clientId);
	}

	session->client->Post(&ClientBase::Send, MakeSendBuffer(res));
}

void RoomBase::Leave(std::shared_ptr<ClientBase> _client, std::string code)
{
	_client->Post(&ClientBase::Leave, code);

	auto client = clients.find(_client->clientId);
	if (client == clients.end() || _client.get() != client->second.get())
		return;

	clients.erase(client);
	GLogManager->Log("Client Removed : ", _client->clientId, ", Client Number : ", std::to_string(clients.size()));

	Protocol::S_REMOVE_CLIENT removeClient;
	removeClient.add_clientids(_client->clientId);
	Broadcast(MakeSendBuffer(removeClient));

	if (CLOSE_ON_EMPTY && clients.size() == 0)
	{
		Post(&RoomBase::Close);
	}
}

std::shared_ptr<ClientBase> RoomBase::MakeClient(string clientId)
{
	auto client = std::make_shared<ClientBase>(GetIoC(), clientId);
	client->DelayPost(10000, &ClientBase::CheckAlive, time(0));
	return client;
}

void RoomBase::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for (const auto& [key, client] : clients)
		client->Post(&ClientBase::Send, sendBuffer);
}
