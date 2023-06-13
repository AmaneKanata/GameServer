#pragma once

#include "ClientBase.h"

class GameObject;

class GameObjectClient : public ClientBase
{
public:
	GameObjectClient(boost::asio::io_context& ioc, std::string clientId) 
		: ClientBase(ioc, clientId)
	{}

	std::shared_ptr<GameObject> gameObject;
};