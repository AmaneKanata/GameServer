#pragma once

#include "SendBuffer.h"

#include <memory>
#include <boost/asio.hpp>

using namespace boost::asio;

class UDPSocket
{
public:
	UDPSocket(io_context& ioc, ip::udp::endpoint& ep)
		: socket(ioc, ep)
	{}

	void Start();
	void RegisterReceive();
	void Send(shared_ptr<SendBuffer> sendBuffer, ip::udp::endpoint ep);

protected:
	virtual void OnRecv(std::size_t bytes_received) = 0;
	std::array<char, 1024> buffer;
	ip::udp::endpoint sender_endpoint;

private:
	ip::udp::socket socket;
};