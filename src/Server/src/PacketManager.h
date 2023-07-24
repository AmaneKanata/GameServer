#pragma once

#include <PacketHeader.h>
#include <JobQueue.h>
#include <SendBuffer.h>
#include <CoreLib_Singleton.h>

#include "Protocols.h"

class GameSession;

enum : unsigned short
{
	PKT_C_ENTER = 0,
	PKT_S_ENTER = 1,
	PKT_C_REENTER = 2,
	PKT_S_REENTER = 3,
	PKT_C_LEAVE = 4,
	PKT_C_GET_CLIENT = 5,
	PKT_S_ADD_CLIENT = 6,
	PKT_S_REMOVE_CLIENT = 7,
	PKT_S_DISCONNECT = 8,
	PKT_C_HEARTBEAT = 9,
	PKT_C_PING = 10,
	PKT_S_PING = 11,
	PKT_C_SERVERTIME = 12,
	PKT_S_SERVERTIME = 13,
	PKT_C_INSTANTIATE_GAME_OBJECT = 100,
	PKT_S_INSTANTIATE_GAME_OBJECT = 101,
	PKT_C_GET_GAME_OBJECT = 102,
	PKT_S_ADD_GAME_OBJECT = 103,
	PKT_C_DESTORY_GAME_OBJECT = 104,
	PKT_S_DESTORY_GAME_OBJECT = 105,
	PKT_S_REMOVE_GAME_OBJECT = 106,
	PKT_C_CHANGE_GMAE_OBJECT = 107,
	PKT_S_CHANGE_GMAE_OBJECT = 108,
	PKT_S_CHANGE_GMAE_OBJECT_NOTICE = 109,
	PKT_C_SET_TRANSFORM = 110,
	PKT_S_SET_TRANSFORM = 111,
	PKT_C_SET_ANIMATION = 112,
	PKT_S_SET_ANIMATION = 113,
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

static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REENTER& pkt) { return MakeSendBuffer(pkt, PKT_S_REENTER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ADD_CLIENT& pkt) { return MakeSendBuffer(pkt, PKT_S_ADD_CLIENT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REMOVE_CLIENT& pkt) { return MakeSendBuffer(pkt, PKT_S_REMOVE_CLIENT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_DISCONNECT& pkt) { return MakeSendBuffer(pkt, PKT_S_DISCONNECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_PING& pkt) { return MakeSendBuffer(pkt, PKT_S_PING); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SERVERTIME& pkt) { return MakeSendBuffer(pkt, PKT_S_SERVERTIME); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_INSTANTIATE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_INSTANTIATE_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ADD_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_ADD_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_DESTORY_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_DESTORY_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REMOVE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_REMOVE_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CHANGE_GMAE_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_GMAE_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CHANGE_GMAE_OBJECT_NOTICE& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_GMAE_OBJECT_NOTICE); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_TRANSFORM& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_TRANSFORM); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_ANIMATION); }

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
			PacketHandlers[i] = PacketHandlers[i] = std::bind(&PacketHandler::Handle_INVALID, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		
		PacketHandlers[PKT_C_ENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_ENTER> pkt = std::make_shared<Protocol::C_ENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_ENTER, session, pkt);
		};
		PacketHandlers[PKT_C_REENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_REENTER> pkt = std::make_shared<Protocol::C_REENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_REENTER, session, pkt);
		};
		PacketHandlers[PKT_C_LEAVE] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_LEAVE> pkt = std::make_shared<Protocol::C_LEAVE>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_LEAVE, session, pkt);
		};
		PacketHandlers[PKT_C_GET_CLIENT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_GET_CLIENT> pkt = std::make_shared<Protocol::C_GET_CLIENT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_GET_CLIENT, session, pkt);
		};
		PacketHandlers[PKT_C_HEARTBEAT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_HEARTBEAT> pkt = std::make_shared<Protocol::C_HEARTBEAT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_HEARTBEAT, session, pkt);
		};
		PacketHandlers[PKT_C_PING] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_PING> pkt = std::make_shared<Protocol::C_PING>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_PING, session, pkt);
		};
		PacketHandlers[PKT_C_SERVERTIME] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SERVERTIME> pkt = std::make_shared<Protocol::C_SERVERTIME>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SERVERTIME, session, pkt);
		};
		PacketHandlers[PKT_C_INSTANTIATE_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt = std::make_shared<Protocol::C_INSTANTIATE_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_INSTANTIATE_GAME_OBJECT, session, pkt);
		};
		PacketHandlers[PKT_C_GET_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt = std::make_shared<Protocol::C_GET_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_GET_GAME_OBJECT, session, pkt);
		};
		PacketHandlers[PKT_C_DESTORY_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt = std::make_shared<Protocol::C_DESTORY_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_DESTORY_GAME_OBJECT, session, pkt);
		};
		PacketHandlers[PKT_C_CHANGE_GMAE_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_CHANGE_GMAE_OBJECT> pkt = std::make_shared<Protocol::C_CHANGE_GMAE_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_CHANGE_GMAE_OBJECT, session, pkt);
		};
		PacketHandlers[PKT_C_SET_TRANSFORM] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt = std::make_shared<Protocol::C_SET_TRANSFORM>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_TRANSFORM, session, pkt);
		};
		PacketHandlers[PKT_C_SET_ANIMATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_ANIMATION> pkt = std::make_shared<Protocol::C_SET_ANIMATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_ANIMATION, session, pkt);
		};
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
	virtual void Handle_C_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_ENTER> pkt) {};
	virtual void Handle_C_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_REENTER> pkt) {};
	virtual void Handle_C_LEAVE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_LEAVE> pkt) {};
	virtual void Handle_C_GET_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_CLIENT> pkt) {};
	virtual void Handle_C_HEARTBEAT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_HEARTBEAT> pkt) {};
	virtual void Handle_C_PING(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_PING> pkt) {};
	virtual void Handle_C_SERVERTIME(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SERVERTIME> pkt) {};
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt) {};
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt) {};
	virtual void Handle_C_DESTORY_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt) {};
	virtual void Handle_C_CHANGE_GMAE_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_GMAE_OBJECT> pkt) {};
	virtual void Handle_C_SET_TRANSFORM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt) {};
	virtual void Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt) {};

private:
	PacketHandlerFunc PacketHandlers[UINT16_MAX];
	HandlerState state;
};