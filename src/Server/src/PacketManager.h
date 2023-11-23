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
	PKT_C_TEST = 14,
	PKT_S_TEST = 15,
	PKT_C_INSTANTIATE_GAME_OBJECT = 100,
	PKT_S_INSTANTIATE_GAME_OBJECT = 101,
	PKT_C_GET_GAME_OBJECT = 102,
	PKT_S_ADD_GAME_OBJECT = 103,
	PKT_C_DESTORY_GAME_OBJECT = 104,
	PKT_S_DESTORY_GAME_OBJECT = 105,
	PKT_S_REMOVE_GAME_OBJECT = 106,
	PKT_C_SET_GAME_OBJECT_PREFAB = 107,
	PKT_S_SET_GAME_OBJECT_PREFAB = 108,
	PKT_C_SET_GAME_OBJECT_OWNER = 109,
	PKT_S_SET_GAME_OBJECT_OWNER = 110,
	PKT_C_SET_TRANSFORM = 111,
	PKT_S_SET_TRANSFORM = 112,
	PKT_C_SET_ANIMATION = 113,
	PKT_S_SET_ANIMATION = 114,
	PKT_S_ADD_FPS_PLAYER = 200,
	PKT_C_SET_FPS_POSITION = 201,
	PKT_S_SET_FPS_POSITION = 202,
	PKT_C_SET_FPS_ROTATION = 203,
	PKT_S_SET_FPS_ROTATION = 204,
	PKT_C_SHOOT = 205,
	PKT_S_SHOOT = 206,
	PKT_S_ATTACKED = 207,
	PKT_C_CHANGE_WEAPON = 208,
	PKT_S_CHANGE_WEAPON = 209,
	PKT_C_RELOAD = 210,
	PKT_S_RELOAD = 211,
	PKT_C_FPS_ANIMATION = 212,
	PKT_S_FPS_ANIMATION = 213,
	PKT_C_FPS_READY = 214,
	PKT_S_FPS_LOAD = 215,
	PKT_S_FPS_START = 216,
	PKT_C_FPS_LOAD_COMPLETE = 217,
	PKT_S_FPS_FINISH = 218,
	PKT_S_FPS_ANNOUNCE = 219,
	PKT_S_FPS_SPAWN_ITEM = 220,
	PKT_S_FPS_SPAWN_DESTINATION = 221,
	PKT_S_FPS_DESTROY_DESTINATION = 222,
	PKT_S_FPS_ITEM_OCCUPY_PROGRESS_STATE = 223,
	PKT_S_FPS_ITEM_OCCUPIED = 224,
	PKT_S_FPS_SCORED = 225,
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
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_INSTANTIATE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_INSTANTIATE_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ADD_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_ADD_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_DESTORY_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_DESTORY_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_REMOVE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_S_REMOVE_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_GAME_OBJECT_PREFAB& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_GAME_OBJECT_PREFAB); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_GAME_OBJECT_OWNER& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_GAME_OBJECT_OWNER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_TRANSFORM& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_TRANSFORM); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_ANIMATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ADD_FPS_PLAYER& pkt) { return MakeSendBuffer(pkt, PKT_S_ADD_FPS_PLAYER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_FPS_POSITION& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_FPS_POSITION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SET_FPS_ROTATION& pkt) { return MakeSendBuffer(pkt, PKT_S_SET_FPS_ROTATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_SHOOT& pkt) { return MakeSendBuffer(pkt, PKT_S_SHOOT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_ATTACKED& pkt) { return MakeSendBuffer(pkt, PKT_S_ATTACKED); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_CHANGE_WEAPON& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_WEAPON); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_RELOAD& pkt) { return MakeSendBuffer(pkt, PKT_S_RELOAD); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_ANIMATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_LOAD& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_LOAD); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_START& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_START); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_FINISH& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_FINISH); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_ANNOUNCE& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_ANNOUNCE); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_SPAWN_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_SPAWN_ITEM); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_SPAWN_DESTINATION& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_SPAWN_DESTINATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_DESTROY_DESTINATION& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_DESTROY_DESTINATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_ITEM_OCCUPY_PROGRESS_STATE& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_ITEM_OCCUPY_PROGRESS_STATE); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_ITEM_OCCUPIED& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_ITEM_OCCUPIED); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::S_FPS_SCORED& pkt) { return MakeSendBuffer(pkt, PKT_S_FPS_SCORED); }

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

		
		PacketHandlers[PKT_C_ENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_ENTER> pkt = std::make_shared<Protocol::C_ENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_ENTER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_REENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_REENTER> pkt = std::make_shared<Protocol::C_REENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_REENTER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_LEAVE] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_LEAVE> pkt = std::make_shared<Protocol::C_LEAVE>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_LEAVE, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_GET_CLIENT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_GET_CLIENT> pkt = std::make_shared<Protocol::C_GET_CLIENT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_GET_CLIENT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_HEARTBEAT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_HEARTBEAT> pkt = std::make_shared<Protocol::C_HEARTBEAT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_HEARTBEAT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_PING] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len)
		{
			std::shared_ptr<Protocol::C_PING> pkt = std::make_shared<Protocol::C_PING>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
			{
				Protocol::S_PING res;
				res.set_tick(pkt->tick());
				if (DELAY_SEND)
					session->DelayPost(DELAY_SEND_INTERVAL, &Session::Send, MakeSendBuffer(res));
				else
					session->Post(&Session::Send, MakeSendBuffer(res));
			}
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SERVERTIME] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SERVERTIME> pkt = std::make_shared<Protocol::C_SERVERTIME>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SERVERTIME, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_TEST] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_TEST> pkt = std::make_shared<Protocol::C_TEST>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_TEST, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_INSTANTIATE_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt = std::make_shared<Protocol::C_INSTANTIATE_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_INSTANTIATE_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_GET_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt = std::make_shared<Protocol::C_GET_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_GET_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_DESTORY_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt = std::make_shared<Protocol::C_DESTORY_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_DESTORY_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_GAME_OBJECT_PREFAB] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_GAME_OBJECT_PREFAB> pkt = std::make_shared<Protocol::C_SET_GAME_OBJECT_PREFAB>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_GAME_OBJECT_PREFAB, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_GAME_OBJECT_OWNER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_GAME_OBJECT_OWNER> pkt = std::make_shared<Protocol::C_SET_GAME_OBJECT_OWNER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_GAME_OBJECT_OWNER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_TRANSFORM] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt = std::make_shared<Protocol::C_SET_TRANSFORM>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_TRANSFORM, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_ANIMATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_ANIMATION> pkt = std::make_shared<Protocol::C_SET_ANIMATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_ANIMATION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_FPS_POSITION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt = std::make_shared<Protocol::C_SET_FPS_POSITION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_FPS_POSITION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SET_FPS_ROTATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt = std::make_shared<Protocol::C_SET_FPS_ROTATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SET_FPS_ROTATION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_SHOOT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_SHOOT> pkt = std::make_shared<Protocol::C_SHOOT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_SHOOT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_CHANGE_WEAPON] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_CHANGE_WEAPON> pkt = std::make_shared<Protocol::C_CHANGE_WEAPON>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_CHANGE_WEAPON, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_RELOAD] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_RELOAD> pkt = std::make_shared<Protocol::C_RELOAD>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_RELOAD, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_FPS_ANIMATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_FPS_ANIMATION> pkt = std::make_shared<Protocol::C_FPS_ANIMATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_FPS_ANIMATION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_FPS_READY] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_FPS_READY> pkt = std::make_shared<Protocol::C_FPS_READY>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_FPS_READY, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_C_FPS_LOAD_COMPLETE] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::C_FPS_LOAD_COMPLETE> pkt = std::make_shared<Protocol::C_FPS_LOAD_COMPLETE>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_C_FPS_LOAD_COMPLETE, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
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
	virtual void Handle_C_SERVERTIME(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SERVERTIME> pkt) {};
	virtual void Handle_C_TEST(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_TEST> pkt) {};
	virtual void Handle_C_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_INSTANTIATE_GAME_OBJECT> pkt) {};
	virtual void Handle_C_GET_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_GET_GAME_OBJECT> pkt) {};
	virtual void Handle_C_DESTORY_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_DESTORY_GAME_OBJECT> pkt) {};
	virtual void Handle_C_SET_GAME_OBJECT_PREFAB(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_PREFAB> pkt) {};
	virtual void Handle_C_SET_GAME_OBJECT_OWNER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_GAME_OBJECT_OWNER> pkt) {};
	virtual void Handle_C_SET_TRANSFORM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt) {};
	virtual void Handle_C_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_ANIMATION> pkt) {};
	virtual void Handle_C_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_POSITION> pkt) {};
	virtual void Handle_C_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SET_FPS_ROTATION> pkt) {};
	virtual void Handle_C_SHOOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_SHOOT> pkt) {};
	virtual void Handle_C_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_CHANGE_WEAPON> pkt) {};
	virtual void Handle_C_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_RELOAD> pkt) {};
	virtual void Handle_C_FPS_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_ANIMATION> pkt) {};
	virtual void Handle_C_FPS_READY(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_READY> pkt) {};
	virtual void Handle_C_FPS_LOAD_COMPLETE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::C_FPS_LOAD_COMPLETE> pkt) {};

private:
	PacketHandlerFunc PacketHandlers[UINT16_MAX];
	HandlerState state;
};