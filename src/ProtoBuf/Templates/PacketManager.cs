using Google.Protobuf;
using Protocol;
using System;
using System.Collections.Generic;

namespace Framework.Network
{
    public enum MsgId : ushort
    {
        {%- for pkt in parser.total_pkt %}
        PKT_{{pkt.name}} = {{ pkt.id}},
        {%- endfor %}
    }

    public static class PacketManager
    {
        private static readonly Dictionary<ushort, Action<ArraySegment<byte>, ushort, PacketQueue>> onRecv = new();

        static PacketManager()
        {
            {%- for pkt in parser.recv_pkt %}
            onRecv.Add((ushort)MsgId.PKT_{{pkt.name}}, MakePacket<{{pkt.name}}>);
            {%- endfor %}
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
        {% for pkt in parser.send_pkt %}
        public static ArraySegment<byte> MakeSendBuffer( Protocol.{{pkt.name}} pkt ) { return MakeSendBuffer(pkt, {{pkt.id}}); }
        {%- endfor %}

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