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
	PKT_C_INSTANTIATE_FPS_PLAYER = 200,
	PKT_S_ADD_FPS_PLAYER = 201,
	PKT_C_SET_FPS_POSITION = 202,
	PKT_S_SET_FPS_POSITION = 203,
	PKT_C_SET_FPS_ROTATION = 204,
	PKT_S_SET_FPS_ROTATION = 205,
	PKT_C_SHOT = 206,
	PKT_S_SHOT = 207,
	PKT_S_ATTACKED = 208,
	PKT_C_FIRE = 209,
	PKT_S_FIRE = 210,
	PKT_C_LOOK = 211,
	PKT_S_LOOK = 212,
	PKT_C_RELOAD = 213,
	PKT_S_RELOAD = 214,
	PKT_C_LEAN = 215,
	PKT_S_LEAN = 216,
	PKT_C_CHANGE_WEAPON = 217,
	PKT_S_CHANGE_WEAPON = 218,
	PKT_C_AIM = 219,
	PKT_S_AIM = 220,
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

static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_REENTER& pkt) { return MakeSendBuffer(pkt, PKT_C_REENTER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_GET_CLIENT& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_CLIENT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_HEARTBEAT& pkt) { return MakeSendBuffer(pkt, PKT_C_HEARTBEAT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_PING& pkt) { return MakeSendBuffer(pkt, PKT_C_PING); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SERVERTIME& pkt) { return MakeSendBuffer(pkt, PKT_C_SERVERTIME); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_TEST& pkt) { return MakeSendBuffer(pkt, PKT_C_TEST); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_INSTANTIATE_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_GET_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_DESTORY_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_DESTORY_GAME_OBJECT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_GAME_OBJECT_PREFAB& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_GAME_OBJECT_PREFAB); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_GAME_OBJECT_OWNER& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_GAME_OBJECT_OWNER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_TRANSFORM& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_TRANSFORM); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_ANIMATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_INSTANTIATE_FPS_PLAYER& pkt) { return MakeSendBuffer(pkt, PKT_C_INSTANTIATE_FPS_PLAYER); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_FPS_POSITION& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_FPS_POSITION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_FPS_ROTATION& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_FPS_ROTATION); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SHOT& pkt) { return MakeSendBuffer(pkt, PKT_C_SHOT); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_FIRE& pkt) { return MakeSendBuffer(pkt, PKT_C_FIRE); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_LOOK& pkt) { return MakeSendBuffer(pkt, PKT_C_LOOK); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_RELOAD& pkt) { return MakeSendBuffer(pkt, PKT_C_RELOAD); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_LEAN& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAN); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_CHANGE_WEAPON& pkt) { return MakeSendBuffer(pkt, PKT_C_CHANGE_WEAPON); }
static std::shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_AIM& pkt) { return MakeSendBuffer(pkt, PKT_C_AIM); }

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

		
		PacketHandlers[PKT_S_ENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_ENTER> pkt = std::make_shared<Protocol::S_ENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_ENTER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_REENTER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_REENTER> pkt = std::make_shared<Protocol::S_REENTER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_REENTER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_ADD_CLIENT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_ADD_CLIENT> pkt = std::make_shared<Protocol::S_ADD_CLIENT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_ADD_CLIENT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_REMOVE_CLIENT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_REMOVE_CLIENT> pkt = std::make_shared<Protocol::S_REMOVE_CLIENT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_REMOVE_CLIENT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_DISCONNECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_DISCONNECT> pkt = std::make_shared<Protocol::S_DISCONNECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_DISCONNECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_PING] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_PING> pkt = std::make_shared<Protocol::S_PING>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_PING, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SERVERTIME] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SERVERTIME> pkt = std::make_shared<Protocol::S_SERVERTIME>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SERVERTIME, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_TEST] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_TEST> pkt = std::make_shared<Protocol::S_TEST>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_TEST, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_INSTANTIATE_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_INSTANTIATE_GAME_OBJECT> pkt = std::make_shared<Protocol::S_INSTANTIATE_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_INSTANTIATE_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_ADD_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_ADD_GAME_OBJECT> pkt = std::make_shared<Protocol::S_ADD_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_ADD_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_DESTORY_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_DESTORY_GAME_OBJECT> pkt = std::make_shared<Protocol::S_DESTORY_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_DESTORY_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_REMOVE_GAME_OBJECT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_REMOVE_GAME_OBJECT> pkt = std::make_shared<Protocol::S_REMOVE_GAME_OBJECT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_REMOVE_GAME_OBJECT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_GAME_OBJECT_PREFAB] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_GAME_OBJECT_PREFAB> pkt = std::make_shared<Protocol::S_SET_GAME_OBJECT_PREFAB>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_GAME_OBJECT_PREFAB, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_GAME_OBJECT_OWNER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_GAME_OBJECT_OWNER> pkt = std::make_shared<Protocol::S_SET_GAME_OBJECT_OWNER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_GAME_OBJECT_OWNER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_TRANSFORM] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_TRANSFORM> pkt = std::make_shared<Protocol::S_SET_TRANSFORM>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_TRANSFORM, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_ANIMATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_ANIMATION> pkt = std::make_shared<Protocol::S_SET_ANIMATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_ANIMATION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_ADD_FPS_PLAYER] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_ADD_FPS_PLAYER> pkt = std::make_shared<Protocol::S_ADD_FPS_PLAYER>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_ADD_FPS_PLAYER, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_FPS_POSITION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_FPS_POSITION> pkt = std::make_shared<Protocol::S_SET_FPS_POSITION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_FPS_POSITION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SET_FPS_ROTATION] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SET_FPS_ROTATION> pkt = std::make_shared<Protocol::S_SET_FPS_ROTATION>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SET_FPS_ROTATION, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_SHOT] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_SHOT> pkt = std::make_shared<Protocol::S_SHOT>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_SHOT, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_ATTACKED] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_ATTACKED> pkt = std::make_shared<Protocol::S_ATTACKED>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_ATTACKED, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_FIRE] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_FIRE> pkt = std::make_shared<Protocol::S_FIRE>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_FIRE, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_LOOK] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_LOOK> pkt = std::make_shared<Protocol::S_LOOK>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_LOOK, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_RELOAD] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_RELOAD> pkt = std::make_shared<Protocol::S_RELOAD>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_RELOAD, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_LEAN] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_LEAN> pkt = std::make_shared<Protocol::S_LEAN>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_LEAN, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_CHANGE_WEAPON] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_CHANGE_WEAPON> pkt = std::make_shared<Protocol::S_CHANGE_WEAPON>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_CHANGE_WEAPON, session, pkt);
			else
				Post(&PacketHandler::Handle_INVALID, session, buffer, len);
		};
		PacketHandlers[PKT_S_AIM] = [this](std::shared_ptr<GameSession> session, unsigned char* buffer, int len) 
		{ 
			std::shared_ptr<Protocol::S_AIM> pkt = std::make_shared<Protocol::S_AIM>();
			if (pkt->ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)))
				Post(&PacketHandler::Handle_S_AIM, session, pkt);
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
	virtual void Handle_S_ENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ENTER> pkt) {};
	virtual void Handle_S_REENTER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REENTER> pkt) {};
	virtual void Handle_S_ADD_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ADD_CLIENT> pkt) {};
	virtual void Handle_S_REMOVE_CLIENT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REMOVE_CLIENT> pkt) {};
	virtual void Handle_S_DISCONNECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_DISCONNECT> pkt) {};
	virtual void Handle_S_PING(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_PING> pkt) {};
	virtual void Handle_S_SERVERTIME(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SERVERTIME> pkt) {};
	virtual void Handle_S_TEST(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_TEST> pkt) {};
	virtual void Handle_S_INSTANTIATE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_INSTANTIATE_GAME_OBJECT> pkt) {};
	virtual void Handle_S_ADD_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ADD_GAME_OBJECT> pkt) {};
	virtual void Handle_S_DESTORY_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_DESTORY_GAME_OBJECT> pkt) {};
	virtual void Handle_S_REMOVE_GAME_OBJECT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_REMOVE_GAME_OBJECT> pkt) {};
	virtual void Handle_S_SET_GAME_OBJECT_PREFAB(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_GAME_OBJECT_PREFAB> pkt) {};
	virtual void Handle_S_SET_GAME_OBJECT_OWNER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_GAME_OBJECT_OWNER> pkt) {};
	virtual void Handle_S_SET_TRANSFORM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_TRANSFORM> pkt) {};
	virtual void Handle_S_SET_ANIMATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_ANIMATION> pkt) {};
	virtual void Handle_S_ADD_FPS_PLAYER(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ADD_FPS_PLAYER> pkt) {};
	virtual void Handle_S_SET_FPS_POSITION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_FPS_POSITION> pkt) {};
	virtual void Handle_S_SET_FPS_ROTATION(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SET_FPS_ROTATION> pkt) {};
	virtual void Handle_S_SHOT(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_SHOT> pkt) {};
	virtual void Handle_S_ATTACKED(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_ATTACKED> pkt) {};
	virtual void Handle_S_FIRE(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_FIRE> pkt) {};
	virtual void Handle_S_LOOK(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_LOOK> pkt) {};
	virtual void Handle_S_RELOAD(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_RELOAD> pkt) {};
	virtual void Handle_S_LEAN(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_LEAN> pkt) {};
	virtual void Handle_S_CHANGE_WEAPON(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_CHANGE_WEAPON> pkt) {};
	virtual void Handle_S_AIM(std::shared_ptr<GameSession> session, std::shared_ptr<Protocol::S_AIM> pkt) {};

private:
	PacketHandlerFunc PacketHandlers[UINT16_MAX];
	HandlerState state;
};