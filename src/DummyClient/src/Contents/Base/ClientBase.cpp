#include "ClientBase.h"
#include "GameSession.h"
#include "DummyClient_Singleton.h"
#include "LogManager.h"

ClientBase::~ClientBase()
{
	GLogManager->Log("[Client ", clientId, "]	Client Destroyed");
}

void ClientBase::HandleClose()
{
	session->Post(&GameSession::RegisterDisconnect);
	session = nullptr;

	std::lock_guard<std::recursive_mutex> lock(clients_mtx);

	auto client = clients.find(clientId);
	if (client != clients.end() && this == client->second.get())
	{
		clients.erase(client);
	}
}

void ClientBase::SetSession(std::shared_ptr<GameSession> session)
{
	this->session = session;
}

void ClientBase::Enter()
{
	Protocol::C_ENTER enter;
	enter.set_clientid(clientId);
	Send(MakeSendBuffer(enter));
}

void ClientBase::ReEnter()
{
	Protocol::C_REENTER reEnter;
	reEnter.set_clientid(clientId);
	Send(MakeSendBuffer(reEnter));
}

void ClientBase::Leave()
{
	Protocol::C_LEAVE leave;
	Send(MakeSendBuffer(leave));
}

void ClientBase::Disconnect()
{
	session->Post(&GameSession::RegisterDisconnect);
}

void ClientBase::Handle_S_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ENTER> pkt)
{
	GLogManager->Log("[Client ", clientId, "]	Entered");

	std::lock_guard<std::recursive_mutex> lock(clients_mtx);
	auto client = clients.find(clientId);
	if (client != clients.end())
	{
		GLogManager->Log("Replace ", clientId);
		client->second->Post(&ClientBase::Close);
		client->second = static_pointer_cast<ClientBase>(shared_from_this());
	}
	else
	{
		clients.insert({ clientId, static_pointer_cast<ClientBase>(shared_from_this()) });
	}
}

void ClientBase::Handle_S_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REENTER> pkt)
{
	if (pkt->success())
	{
		GLogManager->Log("[Client ", clientId, "]	ReEntered");
	}
	else
	{
		GLogManager->Log("[Client ", clientId, "]	ReEnter Fail");

		Post(&ClientBase::Close);
	}
}

void ClientBase::Handle_S_ADD_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ADD_CLIENT> pkt)
{
	std::stringstream ss;
	for (int i = 0; i < pkt->clientinfos_size(); i++)
	{
		ss << pkt->clientinfos()[i].clientid() << " ";
	}
	GLogManager->Log("[Client ", clientId, "]	Add Clients : ", ss.str());
}

void ClientBase::Handle_S_REMOVE_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REMOVE_CLIENT> pkt)
{
	std::stringstream ss;
	for (int i = 0; i < pkt->clientids_size(); i++)
	{
		ss << pkt->clientids()[i] << " ";
	}
	GLogManager->Log("[Client ", clientId, "]	Remove Clients : ", ss.str());
}

void ClientBase::Handle_S_DISCONNECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_DISCONNECT> pkt)
{
	GLogManager->Log("[Client ", clientId, "]	Disconnected : ", pkt->code());
	
	Post(&ClientBase::Close);
}

void ClientBase::CheckAlive()
{
	if (time(0) - lastMessageSent > 5)
	{
		Protocol::C_HEARTBEAT hb;
		Send(MakeSendBuffer(hb));
	}

	DelayPost(5000, &ClientBase::CheckAlive);
}

void ClientBase::Send(shared_ptr<SendBuffer> sendBuffer)
{
	session->Send(sendBuffer);
	lastMessageSent = time(0);
}