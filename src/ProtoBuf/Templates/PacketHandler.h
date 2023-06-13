#pragma once

#include <JobQueue.h>
#include "Protocols.h"

class GameSession;

class PacketHandler : public JobQueue
{
public:
	PacketHandler::PacketHandler(boost::asio::io_context& ioc)
		: JobQueue(ioc)
	{}

{%- for pkt in parser.recv_pkt %}
	virtual void Handle_{{pkt.name}}(std::shared_ptr<GameSession> session, Protocol::{{pkt.name}}& pkt) {};
{%- endfor %}
};