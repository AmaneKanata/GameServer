#include "RoomBase.h"
#include "ClientBase.h"
#include "GameSession.h"
#include "PacketManager.h"

#include <iostream>

void RoomBase::HandleInit()
{
	Post(&RoomBase::SendServerTime);
	
	//Post(&RoomBase::Test);

	if(CLOSE_ON_IDLE)
		DelayPost(CHECK_IDEL_INTERVAL, &RoomBase::CheckIdle);
}

void RoomBase::HandleClose()
{
	for (const auto& [key, client] : clients)
		Post(&RoomBase::Leave, client, std::string("CLOSING"));

	agones_sdk->Shutdown();
}

void RoomBase::Handle_INVALID(std::shared_ptr<GameSession> session, unsigned char* buffer, int len)
{
	GLogManager->Log("Invalid Packet");
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
	
	auto client = MakeClient(pkt->clientid(), session);

	clients.insert({ pkt->clientid(), client });
	//GLogManager->Log("Client Added : ", pkt->clientid(), ",				Client Number : ", std::to_string(clients.size()));

	Protocol::S_ENTER res;
	res.set_result("SUCCESS");
	client->Send(MakeSendBuffer(res));

	Protocol::S_ADD_CLIENT addClient;
	auto clientInfo = addClient.add_clientinfos();
	clientInfo->set_clientid(pkt->clientid());
	Broadcast(MakeSendBuffer(addClient));
}

void RoomBase::Handle_C_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_REENTER> pkt)
{
	Protocol::S_REENTER res;

	auto client = clients.find(pkt->clientid());
	if (client == clients.end())
	{
		res.set_success(false);
		session->Post(&Session::Send, MakeSendBuffer(res));
		session->Post(&GameSession::ReleaseClient);
		session->Post(&GameSession::RegisterDisconnect);
		return;
	}

	client->second->SetSession(session);

	res.set_success(true);
	client->second->Send(MakeSendBuffer(res));
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

	session->client->Send(MakeSendBuffer(res));
}

void RoomBase::Handle_C_SERVERTIME(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SERVERTIME> pkt)
{
	Protocol::S_SERVERTIME res;
	res.set_tick(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	session->client->Send(MakeSendBuffer(res));
}

void RoomBase::Handle_C_TEST(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_TEST> pkt)
{
	//Protocol::S_TEST res;
	//res.set_msg(pkt->msg());
	//session->client->Send(MakeSendBuffer(res));

	//tests.push(pkt->msg());
}

void RoomBase::Leave(std::shared_ptr<ClientBase> _client, std::string code)
{
	Protocol::S_DISCONNECT disconnect;
	disconnect.set_code(code);
	_client->Send(MakeSendBuffer(disconnect));

	auto client = clients.find(_client->clientId);
	if (client == clients.end() || _client.get() != client->second.get())
	{
		_client->Disconnect();
	}
	else
	{
		client->second->Disconnect();

		clients.erase(client);

		Protocol::S_REMOVE_CLIENT removeClient;
		removeClient.add_clientids(_client->clientId);
		Broadcast(MakeSendBuffer(removeClient));
	}

	if (code != "DUPLICATED" && CLOSE_ON_EMPTY && clients.size() == 0)
	{
		GLogManager->Log("All Client Leaved, Close Room");
		Post(&RoomBase::Close);
	}
}

std::shared_ptr<ClientBase> RoomBase::MakeClient(string clientId, std::shared_ptr<GameSession> session)
{
	auto client = std::make_shared<ClientBase>(clientId);
	client->SetSession(session);
	return client;
}

void RoomBase::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	for (const auto& [key, client] : clients)
		client->Send(sendBuffer);
}

void RoomBase::BroadcastMany(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers)
{
	for (const auto& [key, client] : clients)
		client->SendMany(sendBuffers);
}

void RoomBase::SendServerTime()
{
	Protocol::S_SERVERTIME res;
	res.set_tick(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	Broadcast(MakeSendBuffer(res));

	DelayPost(5000, &RoomBase::SendServerTime);
}

void RoomBase::CheckIdle()
{
	if(clients.size() == 0)
		Post(&RoomBase::Close);
}

void RoomBase::Test()
{
	auto start = std::chrono::system_clock::now().time_since_epoch();

	std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers = std::make_shared<std::vector<std::shared_ptr<SendBuffer>>>();

	Protocol::S_TEST res;
	res.set_msg("this is test");
	sendBuffers->push_back(MakeSendBuffer(res));

	if (sendBuffers->size() > 0)
		BroadcastMany(sendBuffers);

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - start).count();

	if (elapsed >= 50)
	{
		DelayPost(50, &RoomBase::Test);
	}
	else
	{
		DelayPost(50 - elapsed, &RoomBase::Test);
	}
}