#include "RoomBase.h"
#include "ClientBase.h"
#include "../../Network/GameSession.h"
#include "../../PacketManager.h"

RoomBase::RoomBase()
	: state(RoomState::Idle)
{
	GLogManager->Log("Room Created");
}

RoomBase::~RoomBase()
{
	GLogManager->Log("Room Destroyed");
}

void RoomBase::Init()
{
	state = RoomState::Running;
}

void RoomBase::Close()
{
	if (state != RoomState::Running) return;

	state = RoomState::Closing;

	HandleClose();

	state = RoomState::Closed;
}

void RoomBase::HandleClose()
{
	for (const auto& [key, client] : clients)
		client->DoAsync(&ClientBase::Leave, string("Closing"));
}

void RoomBase::Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt) { DoAsync(&RoomBase::Enter, session, pkt); }
void RoomBase::Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt) { DoAsync(&RoomBase::ReEnter, session, pkt.clientid()); }
void RoomBase::Handle_C_LEAVE(shared_ptr<ClientBase>& client, Protocol::C_LEAVE& pkt) { client->DoAsync(&ClientBase::Leave, string("Leaved")); }
void RoomBase::Handle_C_GET_CLIENT(shared_ptr<ClientBase>& client, Protocol::C_GET_CLIENT& pkt) { DoAsync(&RoomBase::GetClient, client); }

void RoomBase::Enter(shared_ptr<GameSession> session, Protocol::C_ENTER pkt)
{
	if (state != RoomState::Running) return;

	auto duplicatedClient = clients.find(pkt.clientid());
	if (duplicatedClient != clients.end())
	{
		duplicatedClient->second->DoAsync(&ClientBase::Leave, string("Duplicated"));
		DoTimer(1000, &RoomBase::Enter, session, pkt);
		return;
	}

	auto client = MakeClient(pkt.clientid());
	client->session = session;
	session->owner = client;

	clients.insert({ pkt.clientid(), client });

	Protocol::S_ENTER res;
	res.set_result("SUCCESS");
	session->Send(PacketManager::MakeSendBuffer(res));

	Protocol::S_ADD_CLIENT addClient;
	auto clientInfo = addClient.add_clientinfos();
	clientInfo->set_clientid(pkt.clientid());
	Broadcast(PacketManager::MakeSendBuffer(addClient));
}

void RoomBase::ReEnter(shared_ptr<GameSession> session, string clientId)
{
	if (state != RoomState::Running) return;

	auto client = clients.find(clientId);
	if (client == clients.end())
		return;

	client->second->DoAsync(&ClientBase::ReEnter, session);
}

void RoomBase::Leave(shared_ptr<ClientBase> _client)
{
	if (state != RoomState::Running) return;

	auto client = clients.find(_client->clientId);
	if (client == clients.end())
		return;

	clients.erase(client);

	Protocol::S_REMOVE_CLIENT removeClient;
	removeClient.add_clientids(_client->clientId);
	Broadcast(PacketManager::MakeSendBuffer(removeClient));
}

void RoomBase::GetClient(shared_ptr<ClientBase> client)
{
	if (state != RoomState::Running) return;

	Protocol::S_ADD_CLIENT res;

	for (const auto& [key, _client] : clients) 
	{
		auto clientInfo = res.add_clientinfos();
		clientInfo->set_clientid(_client->clientId);
	}

	client->Send(PacketManager::MakeSendBuffer(res));
}

void RoomBase::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for (const auto& [key, client] : clients)
		client->Send(sendBuffer);
}

void RoomBase::UDP_Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	for (const auto& [key, client] : clients)
		client->UDP_Send(sendBuffer);
}


void RoomBase::SetUDPEndPoint(string clientId, boost::asio::ip::udp::endpoint ep)
{
	auto client = clients.find(clientId);
	if (client == clients.end())
		return;

	client->second->udpEp = ep;

	//send ack to client
}

shared_ptr<ClientBase> RoomBase::MakeClient(string clientId)
{
	return make_shared<ClientBase>(clientId);
}