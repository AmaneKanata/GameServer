#include "pch.h"
#include "Contents/Base/ClientManager.h"

std::shared_ptr<class RoomBase> GRoom = nullptr;
class ClientManager* GClientManager = new ClientManager();

string localHostIp;
int tcpPort = 7777;