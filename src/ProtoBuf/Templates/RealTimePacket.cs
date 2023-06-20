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
{%- for pkt in parser.total_pkt %}
        PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
    }

    public void Register()
    {
{%- for pkt in parser.recv_pkt %}
        onRecv.Add((ushort)MsgId.PKT_{{pkt.name}}, MakePacket<{{pkt.name}}>);
{%- endfor %}
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