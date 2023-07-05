#pragma once

#include "ClientBase.h"

class GameObject;

class GameObjectClient : public ClientBase
{
public:
	GameObjectClient(std::string clientId) 
		: ClientBase(clientId)
	{}

	std::shared_ptr<GameObject> gameObject;
};