#pragma once

#include <JobQueue.h>
#include "Server_Singleton.h"
#include "LogManager.h"

class GameSession;
class SendBuffer;

enum class ClientState
{
	NORMAL,
	LEAVING
};

class ClientBase : public std::enable_shared_from_this<ClientBase>
{
public:
	ClientBase(std::string clientId)
		: clientId(clientId)
	{
		auto clientNum = client_num.fetch_add(1);
		GLogManager->Log("Client Created : ", clientId, ", Client Number : ", std::to_string(clientNum + 1));
	}
	virtual ~ClientBase()
	{
		auto clientNum = client_num.fetch_sub(1);
		GLogManager->Log("Client Destroyed : ", clientId, ", Client Number : ", std::to_string(clientNum - 1));
	}

	void Disconnect();
	void OnDisconnected();

	void SetSession(std::shared_ptr<GameSession> session);

	void Send(std::shared_ptr<SendBuffer> sendBuffer);
	void SendMany(std::shared_ptr<std::vector<std::shared_ptr<SendBuffer>>> sendBuffers);

	const std::string clientId;

private:
	std::shared_ptr<GameSession> session;
};