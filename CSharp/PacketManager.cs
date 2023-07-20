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
        PKT_S_SERVERTIME = 12,
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
    }

    public static class PacketManager
    {
        private static readonly Dictionary<ushort, Action<ArraySegment<byte>, ushort, PacketQueue>> onRecv = new();

        static PacketManager()
        {
            onRecv.Add((ushort)MsgId.PKT_S_ENTER, MakePacket<S_ENTER>);
            onRecv.Add((ushort)MsgId.PKT_S_REENTER, MakePacket<S_REENTER>);
            onRecv.Add((ushort)MsgId.PKT_S_ADD_CLIENT, MakePacket<S_ADD_CLIENT>);
            onRecv.Add((ushort)MsgId.PKT_S_REMOVE_CLIENT, MakePacket<S_REMOVE_CLIENT>);
            onRecv.Add((ushort)MsgId.PKT_S_DISCONNECT, MakePacket<S_DISCONNECT>);
            onRecv.Add((ushort)MsgId.PKT_S_PING, MakePacket<S_PING>);
            onRecv.Add((ushort)MsgId.PKT_S_SERVERTIME, MakePacket<S_SERVERTIME>);
            onRecv.Add((ushort)MsgId.PKT_S_INSTANTIATE_GAME_OBJECT, MakePacket<S_INSTANTIATE_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_ADD_GAME_OBJECT, MakePacket<S_ADD_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_DESTORY_GAME_OBJECT, MakePacket<S_DESTORY_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_REMOVE_GAME_OBJECT, MakePacket<S_REMOVE_GAME_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_CHANGE_GMAE_OBJECT, MakePacket<S_CHANGE_GMAE_OBJECT>);
            onRecv.Add((ushort)MsgId.PKT_S_CHANGE_GMAE_OBJECT_NOTICE, MakePacket<S_CHANGE_GMAE_OBJECT_NOTICE>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_TRANSFORM, MakePacket<S_SET_TRANSFORM>);
            onRecv.Add((ushort)MsgId.PKT_S_SET_ANIMATION, MakePacket<S_SET_ANIMATION>);
        }

        public static void OnRecv( ArraySegment<byte> buffer, PacketQueue packetQueue )
        {
            ushort count = 0;

            ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
            count += 2;
            ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
            count += 2;

            if (onRecv.TryGetValue(id, out Action<ArraySegment<byte>, ushort, PacketQueue> action))
            {
                action.Invoke(buffer, id, packetQueue);
            }
        }

        private static void MakePacket<T>( ArraySegment<byte> buffer, ushort id, PacketQueue packetQueue ) where T : IMessage, new()
        {
            T pkt = new();
            pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

            packetQueue.Push(id, pkt);
        }
        
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_ENTER pkt ) { return MakeSendBuffer(pkt, 0); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_REENTER pkt ) { return MakeSendBuffer(pkt, 2); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_LEAVE pkt ) { return MakeSendBuffer(pkt, 4); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_GET_CLIENT pkt ) { return MakeSendBuffer(pkt, 5); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_HEARTBEAT pkt ) { return MakeSendBuffer(pkt, 9); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_PING pkt ) { return MakeSendBuffer(pkt, 10); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_INSTANTIATE_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 100); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_GET_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 102); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_DESTORY_GAME_OBJECT pkt ) { return MakeSendBuffer(pkt, 104); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_CHANGE_GMAE_OBJECT pkt ) { return MakeSendBuffer(pkt, 107); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_TRANSFORM pkt ) { return MakeSendBuffer(pkt, 110); }
        public static ArraySegment<byte> MakeSendBuffer( Protocol.C_SET_ANIMATION pkt ) { return MakeSendBuffer(pkt, 112); }

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