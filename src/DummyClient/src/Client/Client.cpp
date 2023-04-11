#include "Client.h"
#include "../Session/GameSession.h"

void Client::Send(shared_ptr<SendBuffer> sendBuffer)
{
	session->Send(sendBuffer);
}

void Client::OnDisconnected()
{
	clients.erase(clientId);
	session = nullptr;
}
