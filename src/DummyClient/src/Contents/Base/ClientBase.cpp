#include "ClientBase.h"
#include "GameSession.h"
#include "DummyClient_Singleton.h"
#include "LogManager.h"

void ClientBase::Enter()
{
	Protocol::C_ENTER enter;
	enter.set_clientid(clientId);
	Send(MakeSendBuffer(enter));
}

void ClientBase::Leave()
{
	Protocol::C_LEAVE leave;
	Send(MakeSendBuffer(leave));
}

void ClientBase::Disconnect()
{
	if (state == ClientState::DISCONNECTED)
		return;

	state = ClientState::DISCONNECTED;

	auto sp = session.lock();
	if (sp)
	{
		sp->Disconnect();
	}

	{
		std::lock_guard<std::recursive_mutex> lock(clients_mtx);
		clients.erase(clientId);
	}

	Post(&ClientBase::Close);
}

void ClientBase::Handle_S_ENTER(std::shared_ptr<GameSession> session, Protocol::S_ENTER pkt)
{
	GLogManager->Log("[Client ", clientId, "]	Entered");

	{
		std::lock_guard<std::recursive_mutex> lock(clients_mtx);
		clients.insert({ clientId, static_pointer_cast<ClientBase>(shared_from_this()) });
	}
}

void ClientBase::Handle_S_ADD_CLIENT(std::shared_ptr<GameSession> session, Protocol::S_ADD_CLIENT pkt)
{
	std::stringstream ss;
	for (int i = 0; i < pkt.clientinfos_size(); i++)
	{
		ss << pkt.clientinfos()[i].clientid() << " ";
	}
	GLogManager->Log("[Client ", clientId, "]	Add Clients : ", ss.str());
}

void ClientBase::Handle_S_REMOVE_CLIENT(std::shared_ptr<GameSession> session, Protocol::S_REMOVE_CLIENT pkt)
{
	std::stringstream ss;
	for (int i = 0; i < pkt.clientids_size(); i++)
	{
		ss << pkt.clientids()[i] << " ";
	}
	GLogManager->Log("[Client ", clientId, "]	Remove Clients : ", ss.str());
}

void ClientBase::Handle_S_DISCONNECT(std::shared_ptr<GameSession> session, Protocol::S_DISCONNECT pkt)
{
	GLogManager->Log("[Client ", clientId, "]	Disconnected : ", pkt.code());
	
	Post(&ClientBase::Disconnect);
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
	auto sp = session.lock();
	if (sp)
	{
		sp->Send(sendBuffer);

		lastMessageSent = time(0);
	}
}