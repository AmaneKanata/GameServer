#include "RoomBase.h"
#include "ClientBase.h"
#include "../../Session/GameSession.h"
#include "../../PacketManager.h"

RoomBase::RoomBase(boost::asio::io_context& ioc)
	: JobQueue(ioc)
	, state(RoomState::Idle)
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
	//for (const auto& [key, client] : sessions)
	//	client->DoAsync(&ClientBase::Leave, string("Closing"));
}

void RoomBase::Handle_C_ENTER(shared_ptr<GameSession>& session, Protocol::C_ENTER& pkt) { jobs.post(std::bind(&RoomBase::Enter_CheckDuplicated, this, session, pkt)); }
void RoomBase::Handle_C_REENTER(shared_ptr<GameSession>& session, Protocol::C_REENTER& pkt) { jobs.post(std::bind(&RoomBase::ReEnter, this, session, pkt.clientid())); }
void RoomBase::Handle_C_LEAVE(shared_ptr<GameSession>& session, Protocol::C_LEAVE& pkt) { /* TODO */ }
void RoomBase::Handle_C_GET_CLIENT(shared_ptr<GameSession>& session, Protocol::C_GET_CLIENT& pkt) { jobs.post(std::bind(&RoomBase::GetClients, this, session)); }

void RoomBase::Enter(shared_ptr<GameSession> session, Protocol::C_ENTER pkt)
{
	/*if (state != RoomState::Running) return;

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
	Broadcast(PacketManager::MakeSendBuffer(addClient));*/
}

void RoomBase::Enter_CheckDuplicated(shared_ptr<GameSession> session, Protocol::C_ENTER pkt)
{
	/*if (state != RoomState::Running) return;

	auto duplicatedSession = sessions.find(pkt.clientid());
	if (duplicatedSession != sessions.end())
	{
		duplicatedSession->Jobs(&ClientBase::Leave, string("Duplicated"));

		DoTimer(1000, &RoomBase::Enter, session, pkt);
		return;
	}*/
}

void RoomBase::ReEnter(shared_ptr<GameSession> session, string clientId)
{
	if (state != RoomState::Running) return;

	auto prevSession = sessions.find(clientId);
	if (prevSession == sessions.end())
		return;

	//client->second->DoAsync(&ClientBase::ReEnter, session);
}

void RoomBase::Leave(shared_ptr<GameSession> _client)
{
	//if (state != RoomState::Running) return;

	//auto client = clients.find(_client->clientId);
	//if (client == clients.end())
	//	return;

	//clients.erase(client);

	//Protocol::S_REMOVE_CLIENT removeClient;
	//removeClient.add_clientids(_client->clientId);
	//Broadcast(PacketManager::MakeSendBuffer(removeClient));
}

void RoomBase::GetClients(shared_ptr<GameSession> client)
{
	//if (state != RoomState::Running) return;

	//Protocol::S_ADD_CLIENT res;

	//for (const auto& [key, _client] : clients) 
	//{
	//	auto clientInfo = res.add_clientinfos();
	//	clientInfo->set_clientid(_client->clientId);
	//}

	//client->Send(PacketManager::MakeSendBuffer(res));
}

shared_ptr<ClientBase> RoomBase::MakeClient()
{
	return shared_ptr<ClientBase>();
}

void RoomBase::Broadcast(shared_ptr<SendBuffer> sendBuffer)
{
	//if (state != RoomState::Running) return;

	//for (const auto& [key, client] : clients)
	//	client->Send(sendBuffer);
}