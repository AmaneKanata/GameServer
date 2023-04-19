#pragma once

#include <Network/UDPSocket.h>

class GameUDPSocket : public UDPSocket
{
public :
	GameUDPSocket(io_context& ioc, ip::udp::endpoint ep) : UDPSocket(ioc, ep)
	{}
	virtual void OnRecv(std::size_t bytes_received) override;
};