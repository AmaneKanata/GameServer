#include "pch.h"
#include "Client/Client.h"

string localHostIp;
int port = 7777;

map<string, shared_ptr<Client>> clients;