using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;

public class RealtimePacket
{
    public RealtimePacket()
    {
        Register();
    }

    public void Clear()
    {
        onRecv.Clear();
    }

    readonly Dictionary<ushort, Action<ArraySegment<byte>, ushort, PacketQueue>> onRecv = new Dictionary<ushort, Action<ArraySegment<byte>, ushort, PacketQueue>>();

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
        PKT_C_INSTANTIATE_GAME_OBJECT = 100,
        PKT_S_INSTANTIATE_GAME_OBJECT = 101,
        PKT_C_GET_GAME_OBJECT = 102,
        PKT_S_ADD_GAME_OBJECT = 103,
        PKT_S_REMOVE_GAME_OBJECT = 104,
        PKT_C_SET_TRANSFORM = 105,
        PKT_S_SET_TRANSFORM = 106,
    }

    public void Register()
    {
        onRecv.Add((ushort)MsgId.PKT_S_ENTER, MakePacket<S_ENTER>);
        onRecv.Add((ushort)MsgId.PKT_S_REENTER, MakePacket<S_REENTER>);
        onRecv.Add((ushort)MsgId.PKT_S_ADD_CLIENT, MakePacket<S_ADD_CLIENT>);
        onRecv.Add((ushort)MsgId.PKT_S_REMOVE_CLIENT, MakePacket<S_REMOVE_CLIENT>);
        onRecv.Add((ushort)MsgId.PKT_S_DISCONNECT, MakePacket<S_DISCONNECT>);
        onRecv.Add((ushort)MsgId.PKT_S_INSTANTIATE_GAME_OBJECT, MakePacket<S_INSTANTIATE_GAME_OBJECT>);
        onRecv.Add((ushort)MsgId.PKT_S_ADD_GAME_OBJECT, MakePacket<S_ADD_GAME_OBJECT>);
        onRecv.Add((ushort)MsgId.PKT_S_REMOVE_GAME_OBJECT, MakePacket<S_REMOVE_GAME_OBJECT>);
        onRecv.Add((ushort)MsgId.PKT_S_SET_TRANSFORM, MakePacket<S_SET_TRANSFORM>);
    }

    public void OnRecvPacket( ArraySegment<byte> buffer, PacketQueue packetQueue )
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += 2;

        Action<ArraySegment<byte>, ushort, PacketQueue> action;
        if (onRecv.TryGetValue(id, out action))
            action.Invoke(buffer, id, packetQueue);
    }

    private void MakePacket<T>( ArraySegment<byte> buffer, ushort id, PacketQueue packetQueue ) where T : IMessage, new()
    {
        T pkt = new();
        pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

        packetQueue.Push(id, pkt);
    }
}