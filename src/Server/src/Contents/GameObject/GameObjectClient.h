#pragma once

#include <map>
#include "ClientBase.h"

class GameObject;

class GameObjectClient : public ClientBase
{
public:
	GameObjectClient(std::string clientId) 
		: ClientBase(clientId)
	{}

	std::map<int, std::shared_ptr<GameObject>> gameObjects;
};