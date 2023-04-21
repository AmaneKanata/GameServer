#include "ClientBase.h"
#include "RoomBase.h"
#include "../../Session/GameSession.h"
#include "../../PacketManager.h"

void ClientBase::Leave(string code)
{
	if (state == ClientState::LEAVING)
		return;

	state = ClientState::LEAVING;

	GRoom->DoAsync(&RoomBase::Leave, static_pointer_cast<ClientBase>(shared_from_this()));

	Protocol::S_DISCONNECT disconnect;
	disconnect.set_code(code);
	Send(PacketManager::MakeSendBuffer(disconnect));

	if (session != nullptr)
	{
		session->Disconnect();
		session = nullptr;
	}
}

void ClientBase::Send(shared_ptr<SendBuffer> sendBuffer)
{
	if (session == nullptr)
		return;

	session->Send(sendBuffer);
}

void ClientBase::ReEnter(shared_ptr<GameSession> session)
{
	Protocol::S_REENTER res;

	if (state == ClientState::LEAVING)
	{
		res.set_success(false);
		session->Send(PacketManager::MakeSendBuffer(res));
		return;
	}

	this->session = session;

	res.set_success(true);
	session->Send(PacketManager::MakeSendBuffer(res));
}

void ClientBase::OnDisconnected()
{
	if (state == ClientState::LEAVING)
		return;

	session = nullptr;
	DoAsync(&ClientBase::Leave, string("Disconnected"));
}
