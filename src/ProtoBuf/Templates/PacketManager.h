#pragma once

#include <PacketHeader.h>
#include <JobQueue.h>
#include <SendBuffer.h>
#include <CoreLib_Singleton.h>

#include "GameSession.h"
#include "Protocols.h"

class GameSession;

enum : unsigned short
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

template<typename T>
static std::shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, unsigned short pktId)
{
	const unsigned short dataSize = static_cast<unsigned short>(pkt.ByteSizeLong());
	const unsigned short packetSize = dataSize + sizeof(PacketHeader);

	std::shared_ptr<SendBuffer> sendBuffer = GSendBufferManager->Open(packetSize);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->size = packetSize;
	header->id = pktId;
	pkt.SerializeToArray(&header[1], dataSize);
	sendBuffer->Close(packetSize);

	return sendBuffer;
}
{% for pkt in parser.send_pkt %}
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}); }
{%- endfor %}

enum class HandlerState
{
	Idle,
	Running,
	Closing,
	Closed
};

class PacketHandler : public JobQueue
{
	using PacketHandlerFunc = std::function<void(std::shared_ptr<GameSession>&, unsigned char*, int)>;

public:
	PacketHandler(boost::asio::io_context& ioc)
		: JobQueue(ioc)
		, state(HandlerState::Idle)
	{
		for (int i = 0; i < UINT16_MAX; i++)
			PacketHandlers[i] = std::bind(&PacketHandler::Handle_INVALID, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		{% for pkt in parser.recv_pkt %}
			{%- if (pkt.name == 'C_PING') %}
		PacketHandlers[PKT_C_PING] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len)
		{
			std::shared_ptr<Protocol::C_PING> pkt = std::make_shared<Protocol::C_PING>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
			{
				Protocol::S_PING res;
				res.set_tick(pkt->tick());
				session->Post(&Session::Send, MakeSendBuffer(res));
			}
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
			{%- else %}
		PacketHandlers[PKT_{{pkt.name}}] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::{{pkt.name}}> pkt = std::make_shared<Protocol::{{pkt.name}}>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_{{pkt.name}}, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
			{%- endif %}

		{%- endfor %}
	}

	void Init()
	{
		state = HandlerState::Running;
		HandleInit();
	}
	virtual void HandleInit() {};
	
	void Close()
	{
		if (state != HandlerState::Running) return;

		state = HandlerState::Closing;

		HandleClose();

		state = HandlerState::Closed;
		
		Clear();
	}
	virtual void HandleClose() {};

	virtual void HandlePacket_Not_Running(std::shared_ptr<GameSession>& session) {}

	void HandlePacket(std::shared_ptr<GameSession> session, unsigned char* buffer, int len)
	{
		if (state != HandlerState::Running)
		{
			HandlePacket_Not_Running(session);
			return;
		}

		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		PacketHandlers[header->id](session, buffer, len);
	}

	HandlerState GetState() { return state; }

protected:
	virtual void Handle_INVALID(std::shared_ptr<GameSession> session, unsigned char* buffer, int len) {};
{%- for pkt in parser.recv_pkt %}
	{%- if (pkt.name != 'C_PING')%}
	virtual void Handle_{{pkt.name}}(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::{{pkt.name}}> pkt) {};
	{%- endif %}
{%- endfor %}

private:
	PacketHandlerFunc PacketHandlers[UINT16_MAX];
	HandlerState state;
};
