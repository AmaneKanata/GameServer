#include "GameUDPSocket.h"

#include "../pch.h"
#include "../Contents/Base/RoomBase.h"

void GameUDPSocket::OnRecv(std::size_t bytes_received)
{
	std::string clientId(buffer.data(), bytes_received);
	GRoom->DoAsync(&RoomBase::SetUDPEndPoint, clientId, sender_endpoint);
}
