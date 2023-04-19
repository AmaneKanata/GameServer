#include "UDPSocket.h"
#include "SendBuffer.h"

#include <iostream>

void UDPSocket::Start()
{
	RegisterReceive();
}

void UDPSocket::RegisterReceive()
{
	socket.async_receive_from(boost::asio::buffer(buffer), sender_endpoint,
		[this](const boost::system::error_code& error, std::size_t bytes_received) {
			if (error) {
				RegisterReceive();
				return;
			}

			OnRecv(bytes_received);

			RegisterReceive();
			return;
		});
}

void UDPSocket::Send(shared_ptr<SendBuffer> sendBuffer, ip::udp::endpoint ep)
{
	socket.async_send_to(boost::asio::buffer(sendBuffer->Buffer(), sendBuffer->WriteSize()), ep, 
		[sendBuffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
			std::cout << "udp send success" << std::endl;
		});
}
