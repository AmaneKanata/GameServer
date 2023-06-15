#include "ClientBase.h"
#include "Server_Singleton.h"
#include "GameSession.h"
#include "RoomBase.h"
#include "PacketManager.h"

void ClientBase::OnDisconnected()
{
	GRoom->Post(&RoomBase::Leave, static_pointer_cast<ClientBase>(shared_from_this()), string("DISCONNECTED"));
}

void ClientBase::SetSession(std::shared_ptr<GameSession> _session)
{
	session = _session;
	_session->SetClient(static_pointer_cast<ClientBase>(shared_from_this()));
}

void ClientBase::ReEnter(std::shared_ptr<GameSession> _session)
{
	SetSession(_session);
	
	Protocol::S_REENTER res;
	res.set_success(true);
	Send(MakeSendBuffer(res));
}

void ClientBase::Leave(std::string code)
{
	if (state == ClientState::LEAVING)
		return;

	state = ClientState::LEAVING;

	Protocol::S_DISCONNECT disconnect;
	disconnect.set_code(code);
	Send(MakeSendBuffer(disconnect));

	auto sp = session.lock();
	if (sp)
	{
		sp->Disconnect();
	}

	Clear();
}

void ClientBase::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	auto sp = session.lock();
	if (sp)
	{
		sp->Send(sendBuffer);
	}
}

void ClientBase::CheckAlive(std::time_t current)
{
	auto sp = session.lock();
	if (sp)
	{
		if (current - sp->lastMessageArrived > 10)
		{
			GLogManager->Log("Client Heartbeat Fail : ", clientId);
			Post(&ClientBase::Leave, std::string("HEARTBEAT_FAIL"));
		}
	}

	DelayPost(10000, &ClientBase::CheckAlive, time(0));
}
