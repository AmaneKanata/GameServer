#pragma once

#include <pch.h>

extern std::shared_ptr<class RoomBase> GRoom;

extern class GameUDPSocket* GUDPSocket;
extern class boost::asio::ip::udp::endpoint defaultUDPEndPoint;

extern string localHostIp;
extern int port;