#pragma once

#include <pch.h>

extern string localHostIp;
extern int tcpPort;

extern map<string, shared_ptr<class Client>> clients;