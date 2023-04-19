#include "pch.h"
#include "Network/GameUDPSocket.h"

std::shared_ptr<class RoomBase> GRoom = nullptr;
GameUDPSocket* GUDPSocket = nullptr;
boost::asio::ip::udp::endpoint defaultUDPEndPoint = boost::asio::ip::udp::endpoint();

string localHostIp;
int port = 7777;