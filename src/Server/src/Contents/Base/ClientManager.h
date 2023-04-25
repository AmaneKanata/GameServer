#pragma once

#include "ClientBase.h"

#include <memory>

class ClientManager
{
public:
	template<typename T>
	shared_ptr<ClientBase> MakeClient(string clientId)
	{
		shared_ptr<ClientBase> client = make_shared<T>(clientId);
		clients.insert({ clientId, client });
		return client;
	}

	void RemoveClient(string clientId)
	{
		clients.erase(clientId);
	}

	shared_ptr<ClientBase> GetClient(string clientId)
	{
		auto client = clients.find(clientId);
		if (client == clients.end())
			return nullptr;
		return client->second;
	}

private:
	map<string, shared_ptr<ClientBase>> clients;
};