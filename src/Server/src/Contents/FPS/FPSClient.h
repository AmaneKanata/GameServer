#pragma once

#include "ClientBase.h"

class FPSPlayer;

class FPSClient : public ClientBase
{
public:
	FPSClient(std::string clientId)
		: ClientBase(clientId)
	{}

public:
	std::shared_ptr<FPSPlayer> player;
};