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

class ClientBase : public JobQueue
{
public:
	ClientBase(boost::asio::io_context& ioc, std::string clientId)
		: JobQueue(ioc)
		, clientId(clientId)
		, state(ClientState::NORMAL) 
	{
		auto clientNum = client_num.fetch_add(1);
		GLogManager->Log("Client Created : ", clientId, ", Client Number : ", std::to_string(clientNum + 1));
	}
	~ClientBase()
	{
		auto clientNum = client_num.fetch_sub(1);
		GLogManager->Log("Client Destroyed : ", clientId, ", Client Number : ", std::to_string(clientNum - 1));
	}

	void OnDisconnected();

	void SetSession(std::shared_ptr<GameSession> session);

	void ReEnter(std::shared_ptr<GameSession> session);

	void Leave(std::string code);

	void Send(std::shared_ptr<SendBuffer> sendBuffer);

	const std::string clientId;

private:
	std::weak_ptr<GameSession> session;
	ClientState state;
};