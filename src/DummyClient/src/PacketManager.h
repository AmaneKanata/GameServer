#pragma once

#include "pch.h"
#include "Protocols.h"

class GameSession;

using PacketHandlerFunc = std::function<bool(shared_ptr<GameSession>&, unsigned char*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

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
	PKT_C_INSTANTIATE_GAME_OBJECT = 100,
	PKT_S_INSTANTIATE_GAME_OBJECT = 101,
	PKT_C_GET_GAME_OBJECT = 102,
	PKT_S_ADD_GAME_OBJECT = 103,
	PKT_S_REMOVE_GAME_OBJECT = 104,
	PKT_C_SET_TRANSFORM = 105,
	PKT_S_SET_TRANSFORM = 106,
};

bool Handle_INVALID(shared_ptr<GameSession>& session, unsigned char* buffer, int len);
bool Handle_S_ENTER(shared_ptr<GameSession>& session, Protocol::S_ENTER& pkt);
bool Handle_S_REENTER(shared_ptr<GameSession>& session, Protocol::S_REENTER& pkt);
bool Handle_S_ADD_CLIENT(shared_ptr<GameSession>& session, Protocol::S_ADD_CLIENT& pkt);
bool Handle_S_REMOVE_CLIENT(shared_ptr<GameSession>& session, Protocol::S_REMOVE_CLIENT& pkt);
bool Handle_S_DISCONNECT(shared_ptr<GameSession>& session, Protocol::S_DISCONNECT& pkt);
bool Handle_S_INSTANTIATE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_INSTANTIATE_GAME_OBJECT& pkt);
bool Handle_S_ADD_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_ADD_GAME_OBJECT& pkt);
bool Handle_S_REMOVE_GAME_OBJECT(shared_ptr<GameSession>& session, Protocol::S_REMOVE_GAME_OBJECT& pkt);
bool Handle_S_SET_TRANSFORM(shared_ptr<GameSession>& session, Protocol::S_SET_TRANSFORM& pkt);

class PacketManager
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_ENTER] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_ENTER > (Handle_S_ENTER, session, buffer, len); };
		GPacketHandler[PKT_S_REENTER] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_REENTER > (Handle_S_REENTER, session, buffer, len); };
		GPacketHandler[PKT_S_ADD_CLIENT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_ADD_CLIENT > (Handle_S_ADD_CLIENT, session, buffer, len); };
		GPacketHandler[PKT_S_REMOVE_CLIENT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_REMOVE_CLIENT > (Handle_S_REMOVE_CLIENT, session, buffer, len); };
		GPacketHandler[PKT_S_DISCONNECT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_DISCONNECT > (Handle_S_DISCONNECT, session, buffer, len); };
		GPacketHandler[PKT_S_INSTANTIATE_GAME_OBJECT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_INSTANTIATE_GAME_OBJECT > (Handle_S_INSTANTIATE_GAME_OBJECT, session, buffer, len); };
		GPacketHandler[PKT_S_ADD_GAME_OBJECT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_ADD_GAME_OBJECT > (Handle_S_ADD_GAME_OBJECT, session, buffer, len); };
		GPacketHandler[PKT_S_REMOVE_GAME_OBJECT] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_REMOVE_GAME_OBJECT > (Handle_S_REMOVE_GAME_OBJECT, session, buffer, len); };
		GPacketHandler[PKT_S_SET_TRANSFORM] = [](shared_ptr<GameSession>& session, unsigned char* buffer, int len) { return HandlePacket < Protocol::S_SET_TRANSFORM > (Handle_S_SET_TRANSFORM, session, buffer, len); };
	}

	static bool HandlePacket(shared_ptr<GameSession>& session, unsigned char* buffer, int len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_REENTER& pkt) { return MakeSendBuffer(pkt, PKT_C_REENTER); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_GET_CLIENT& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_CLIENT); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_INSTANTIATE_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_INSTANTIATE_GAME_OBJECT); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_GET_GAME_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_GAME_OBJECT); }
	static shared_ptr<SendBuffer> MakeSendBuffer(Protocol::C_SET_TRANSFORM& pkt) { return MakeSendBuffer(pkt, PKT_C_SET_TRANSFORM); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, shared_ptr<GameSession>& session, unsigned char* buffer, int len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static shared_ptr<SendBuffer> MakeSendBuffer(T& pkt, unsigned short pktId)
	{
		const unsigned short dataSize = static_cast<unsigned short>(pkt.ByteSizeLong());
		const unsigned short packetSize = dataSize + sizeof(PacketHeader);

		shared_ptr<SendBuffer> sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};