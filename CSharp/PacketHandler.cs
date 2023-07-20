using Google.Protobuf;
using System;
using System.Collections.Generic;

namespace Framework.Network
{
    public class PacketHandler
    {
        public Dictionary<ushort, Action<IMessage>> Handlers = new();
        private Action<Protocol.S_ENTER> S_ENTER_Handler;
        private Action<Protocol.S_REENTER> S_REENTER_Handler;
        private Action<Protocol.S_ADD_CLIENT> S_ADD_CLIENT_Handler;
        private Action<Protocol.S_REMOVE_CLIENT> S_REMOVE_CLIENT_Handler;
        private Action<Protocol.S_DISCONNECT> S_DISCONNECT_Handler;
        private Action<Protocol.S_PING> S_PING_Handler;
        private Action<Protocol.S_SERVERTIME> S_SERVERTIME_Handler;
        private Action<Protocol.S_INSTANTIATE_GAME_OBJECT> S_INSTANTIATE_GAME_OBJECT_Handler;
        private Action<Protocol.S_ADD_GAME_OBJECT> S_ADD_GAME_OBJECT_Handler;
        private Action<Protocol.S_DESTORY_GAME_OBJECT> S_DESTORY_GAME_OBJECT_Handler;
        private Action<Protocol.S_REMOVE_GAME_OBJECT> S_REMOVE_GAME_OBJECT_Handler;
        private Action<Protocol.S_CHANGE_GMAE_OBJECT> S_CHANGE_GMAE_OBJECT_Handler;
        private Action<Protocol.S_CHANGE_GMAE_OBJECT_NOTICE> S_CHANGE_GMAE_OBJECT_NOTICE_Handler;
        private Action<Protocol.S_SET_TRANSFORM> S_SET_TRANSFORM_Handler;
        private Action<Protocol.S_SET_ANIMATION> S_SET_ANIMATION_Handler;

        public PacketHandler()
        {
            Handlers.Add(1, _Handle_S_ENTER);
            Handlers.Add(3, _Handle_S_REENTER);
            Handlers.Add(6, _Handle_S_ADD_CLIENT);
            Handlers.Add(7, _Handle_S_REMOVE_CLIENT);
            Handlers.Add(8, _Handle_S_DISCONNECT);
            Handlers.Add(11, _Handle_S_PING);
            Handlers.Add(12, _Handle_S_SERVERTIME);
            Handlers.Add(101, _Handle_S_INSTANTIATE_GAME_OBJECT);
            Handlers.Add(103, _Handle_S_ADD_GAME_OBJECT);
            Handlers.Add(105, _Handle_S_DESTORY_GAME_OBJECT);
            Handlers.Add(106, _Handle_S_REMOVE_GAME_OBJECT);
            Handlers.Add(108, _Handle_S_CHANGE_GMAE_OBJECT);
            Handlers.Add(109, _Handle_S_CHANGE_GMAE_OBJECT_NOTICE);
            Handlers.Add(111, _Handle_S_SET_TRANSFORM);
            Handlers.Add(113, _Handle_S_SET_ANIMATION);
        }
        public void AddHandler( Action<Protocol.S_ENTER> handler )
        {
            S_ENTER_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_ENTER> handler )
        {
            S_ENTER_Handler -= handler;
        }
        private void _Handle_S_ENTER( IMessage message )
        {
            S_ENTER_Handler?.Invoke((Protocol.S_ENTER)message);
        }
        public void AddHandler( Action<Protocol.S_REENTER> handler )
        {
            S_REENTER_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_REENTER> handler )
        {
            S_REENTER_Handler -= handler;
        }
        private void _Handle_S_REENTER( IMessage message )
        {
            S_REENTER_Handler?.Invoke((Protocol.S_REENTER)message);
        }
        public void AddHandler( Action<Protocol.S_ADD_CLIENT> handler )
        {
            S_ADD_CLIENT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_ADD_CLIENT> handler )
        {
            S_ADD_CLIENT_Handler -= handler;
        }
        private void _Handle_S_ADD_CLIENT( IMessage message )
        {
            S_ADD_CLIENT_Handler?.Invoke((Protocol.S_ADD_CLIENT)message);
        }
        public void AddHandler( Action<Protocol.S_REMOVE_CLIENT> handler )
        {
            S_REMOVE_CLIENT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_REMOVE_CLIENT> handler )
        {
            S_REMOVE_CLIENT_Handler -= handler;
        }
        private void _Handle_S_REMOVE_CLIENT( IMessage message )
        {
            S_REMOVE_CLIENT_Handler?.Invoke((Protocol.S_REMOVE_CLIENT)message);
        }
        public void AddHandler( Action<Protocol.S_DISCONNECT> handler )
        {
            S_DISCONNECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_DISCONNECT> handler )
        {
            S_DISCONNECT_Handler -= handler;
        }
        private void _Handle_S_DISCONNECT( IMessage message )
        {
            S_DISCONNECT_Handler?.Invoke((Protocol.S_DISCONNECT)message);
        }
        public void AddHandler( Action<Protocol.S_PING> handler )
        {
            S_PING_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_PING> handler )
        {
            S_PING_Handler -= handler;
        }
        private void _Handle_S_PING( IMessage message )
        {
            S_PING_Handler?.Invoke((Protocol.S_PING)message);
        }
        public void AddHandler( Action<Protocol.S_SERVERTIME> handler )
        {
            S_SERVERTIME_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SERVERTIME> handler )
        {
            S_SERVERTIME_Handler -= handler;
        }
        private void _Handle_S_SERVERTIME( IMessage message )
        {
            S_SERVERTIME_Handler?.Invoke((Protocol.S_SERVERTIME)message);
        }
        public void AddHandler( Action<Protocol.S_INSTANTIATE_GAME_OBJECT> handler )
        {
            S_INSTANTIATE_GAME_OBJECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_INSTANTIATE_GAME_OBJECT> handler )
        {
            S_INSTANTIATE_GAME_OBJECT_Handler -= handler;
        }
        private void _Handle_S_INSTANTIATE_GAME_OBJECT( IMessage message )
        {
            S_INSTANTIATE_GAME_OBJECT_Handler?.Invoke((Protocol.S_INSTANTIATE_GAME_OBJECT)message);
        }
        public void AddHandler( Action<Protocol.S_ADD_GAME_OBJECT> handler )
        {
            S_ADD_GAME_OBJECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_ADD_GAME_OBJECT> handler )
        {
            S_ADD_GAME_OBJECT_Handler -= handler;
        }
        private void _Handle_S_ADD_GAME_OBJECT( IMessage message )
        {
            S_ADD_GAME_OBJECT_Handler?.Invoke((Protocol.S_ADD_GAME_OBJECT)message);
        }
        public void AddHandler( Action<Protocol.S_DESTORY_GAME_OBJECT> handler )
        {
            S_DESTORY_GAME_OBJECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_DESTORY_GAME_OBJECT> handler )
        {
            S_DESTORY_GAME_OBJECT_Handler -= handler;
        }
        private void _Handle_S_DESTORY_GAME_OBJECT( IMessage message )
        {
            S_DESTORY_GAME_OBJECT_Handler?.Invoke((Protocol.S_DESTORY_GAME_OBJECT)message);
        }
        public void AddHandler( Action<Protocol.S_REMOVE_GAME_OBJECT> handler )
        {
            S_REMOVE_GAME_OBJECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_REMOVE_GAME_OBJECT> handler )
        {
            S_REMOVE_GAME_OBJECT_Handler -= handler;
        }
        private void _Handle_S_REMOVE_GAME_OBJECT( IMessage message )
        {
            S_REMOVE_GAME_OBJECT_Handler?.Invoke((Protocol.S_REMOVE_GAME_OBJECT)message);
        }
        public void AddHandler( Action<Protocol.S_CHANGE_GMAE_OBJECT> handler )
        {
            S_CHANGE_GMAE_OBJECT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_CHANGE_GMAE_OBJECT> handler )
        {
            S_CHANGE_GMAE_OBJECT_Handler -= handler;
        }
        private void _Handle_S_CHANGE_GMAE_OBJECT( IMessage message )
        {
            S_CHANGE_GMAE_OBJECT_Handler?.Invoke((Protocol.S_CHANGE_GMAE_OBJECT)message);
        }
        public void AddHandler( Action<Protocol.S_CHANGE_GMAE_OBJECT_NOTICE> handler )
        {
            S_CHANGE_GMAE_OBJECT_NOTICE_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_CHANGE_GMAE_OBJECT_NOTICE> handler )
        {
            S_CHANGE_GMAE_OBJECT_NOTICE_Handler -= handler;
        }
        private void _Handle_S_CHANGE_GMAE_OBJECT_NOTICE( IMessage message )
        {
            S_CHANGE_GMAE_OBJECT_NOTICE_Handler?.Invoke((Protocol.S_CHANGE_GMAE_OBJECT_NOTICE)message);
        }
        public void AddHandler( Action<Protocol.S_SET_TRANSFORM> handler )
        {
            S_SET_TRANSFORM_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_TRANSFORM> handler )
        {
            S_SET_TRANSFORM_Handler -= handler;
        }
        private void _Handle_S_SET_TRANSFORM( IMessage message )
        {
            S_SET_TRANSFORM_Handler?.Invoke((Protocol.S_SET_TRANSFORM)message);
        }
        public void AddHandler( Action<Protocol.S_SET_ANIMATION> handler )
        {
            S_SET_ANIMATION_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_ANIMATION> handler )
        {
            S_SET_ANIMATION_Handler -= handler;
        }
        private void _Handle_S_SET_ANIMATION( IMessage message )
        {
            S_SET_ANIMATION_Handler?.Invoke((Protocol.S_SET_ANIMATION)message);
        }
    }
}