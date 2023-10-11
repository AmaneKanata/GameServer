using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;

namespace Framework.Network
{
    public enum MsgId : ushort
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
    }

    public static class PacketManager
    {
        private static readonly Dictionary<ushort, Action<ArraySegment<byte>, ushort, Connection>> onRecv = new();

        static PacketManager()
        {
            onRecv.Add((ushort)MsgId.PKT_S_ENTER, MakePacket<S_ENTER>);
            onRecv.Add((ushort)MsgId.PKT_S_REENTER, MakePacket<S_REENTER>);
            onRecv.Add((ushort)MsgId.PKT_S_ADD_CLIENT, MakePacket<S_ADD_CLIENT>);
            onRecv.Add((ushort)MsgId.PKT_S_REMOVE_CLIENT, MakePacket<S_REMOVE_CLIENT>);
            onRecv.Add((ushort)MsgId.PKT_S_DISCONNECT, MakePacket<S_DISCONNECT>);
            onRecv.Add((ushort)MsgId.PKT_S_PING, MakePacket<S_PING>);
            onRecv.Add((ushort)MsgId.PKT_S_SERVERTIME, MakePacket<S_SERVERTIME>);
            onRecv.Add((ushort)MsgId.PKT_S_TEST, MakePacket<S_TEST>);
            onRecv.Add((ushort)MsgId.PKT_S_INSTANTIATE_GAME_OBJECT, MakePacket<S_INSTANTIATE_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_ADD_GAME_OBJECT, MakePacket<S_ADD_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_DESTORY_GAME_OBJECT, MakePacket<S_DESTORY_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_REMOVE_GAME_OBJECT, MakePacket<S_REMOVE_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_GAME_OBJECT_PREFAB, MakePacket<S_SET_GAME_OBJECT_PREFAB>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_GAME_OBJECT_OWNER, MakePacket<S_SET_GAME_OBJECT_OWNER>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_TRANSFORM, MakePacket<S_SET_TRANSFORM>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_ANIMATION, MakePacket<S_SET_ANIMATION>);
            onRecv.Add((ushort)MsgId.PKT_S_ADD_FPS_PLAYER, MakePacket<S_ADD_FPS_PLAYER>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_FPS_POSITION, MakePacket<S_SET_FPS_POSITION>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_FPS_ROTATION, MakePacket<S_SET_FPS_ROTATION>);
            onRecv.Add((ushort)MsgId.PKT_S_SHOT, MakePacket<S_SHOT>);
            onRecv.Add((ushort)MsgId.PKT_S_ATTACKED, MakePacket<S_ATTACKED>);
        }

        public static void OnRecv( ArraySegment<byte> buffer, Connection connection )
        {
            ushort count = 0;

            ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
            count += 2;
            ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
            count += 2;

            if (onRecv.TryGetValue(id, out Action<ArraySegment<byte>, ushort, Connection> action))
            {
                action.Invoke(buffer, id, connection);
            }
        }

        private static void MakePacket<T>( ArraySegment<byte> buffer, ushort id, Connection connection ) where T : IMessage, new()
        {
            T pkt = new();
            pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

            if (id == (ushort)MsgId.PKT_S_PING)
            {
                Protocol.S_PING ping = pkt as Protocol.S_PING;
                connection.Handle_S_PING(ping);
            }
            if (id == (ushort)MsgId.PKT_S_SERVERTIME)
            {
                Protocol.S_SERVERTIME serverTime = pkt as Protocol.S_SERVERTIME;
                connection.Handle_S_SERVERTIME(serverTime);
            }

            connection.PacketQueue.Push(id, pkt);
        }
        
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_ENTER pkt ) { return MakeSendBuffer(pkt, 0); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_REENTER pkt ) { return MakeSendBuffer(pkt, 2); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_LEAVE pkt ) { return MakeSendBuffer(pkt, 4); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_GET_CLIENT pkt ) { return MakeSendBuffer(pkt, 5); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_HEARTBEAT pkt ) { return MakeSendBuffer(pkt, 9); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_PING pkt ) { return MakeSendBuffer(pkt, 10); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SERVERTIME pkt ) { return MakeSendBuffer(pkt, 12); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_TEST pkt ) { return MakeSendBuffer(pkt, 14); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_INSTANTIATE_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 100); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_GET_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 102); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_DESTORY_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 104); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_GAME_OBJECT_PREFAB pkt ) { return MakeSendBuffer(pkt, 107); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_GAME_OBJECT_OWNER pkt ) { return MakeSendBuffer(pkt, 109); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_TRANSFORM pkt ) { return MakeSendBuffer(pkt, 111); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_ANIMATION pkt ) { return MakeSendBuffer(pkt, 113); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_INSTANTIATE_FPS_PLAYER pkt ) { return MakeSendBuffer(pkt, 200); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_FPS_POSITION pkt ) { return MakeSendBuffer(pkt, 202); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_FPS_ROTATION pkt ) { return MakeSendBuffer(pkt, 204); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SHOT pkt ) { return MakeSendBuffer(pkt, 206); }

        private static ArraySegment<byte> MakeSendBuffer( IMessage pkt, ushort pktId )
        {
            ushort size = (ushort)pkt.CalculateSize();
            byte[] sendBuffer = new byte[size + 4];

            Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
            Array.Copy(BitConverter.GetBytes(pktId), 0, sendBuffer, 2, sizeof(ushort));
            Array.Copy(pkt.ToByteArray(), 0, sendBuffer, 4, size);

            return sendBuffer;
        }
    }
}