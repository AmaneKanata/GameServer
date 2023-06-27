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
        private Action<Protocol.S_TEST> S_TEST_Handler;
        private Action<Protocol.S_INSTANTIATE_GAME_OBJECT> S_INSTANTIATE_GAME_OBJECT_Handler;
        private Action<Protocol.S_ADD_GAME_OBJECT> S_ADD_GAME_OBJECT_Handler;
        private Action<Protocol.S_REMOVE_GAME_OBJECT> S_REMOVE_GAME_OBJECT_Handler;
        private Action<Protocol.S_SET_TRANSFORM> S_SET_TRANSFORM_Handler;

        public PacketHandler()
        {
            Handlers.Add(1, _Handle_S_ENTER);
            Handlers.Add(3, _Handle_S_REENTER);
            Handlers.Add(6, _Handle_S_ADD_CLIENT);
            Handlers.Add(7, _Handle_S_REMOVE_CLIENT);
            Handlers.Add(8, _Handle_S_DISCONNECT);
            Handlers.Add(11, _Handle_S_TEST);
            Handlers.Add(101, _Handle_S_INSTANTIATE_GAME_OBJECT);
            Handlers.Add(103, _Handle_S_ADD_GAME_OBJECT);
            Handlers.Add(104, _Handle_S_REMOVE_GAME_OBJECT);
            Handlers.Add(106, _Handle_S_SET_TRANSFORM);
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
        public void AddHandler( Action<Protocol.S_TEST> handler )
        {
            S_TEST_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_TEST> handler )
        {
            S_TEST_Handler -= handler;
        }
        private void _Handle_S_TEST( IMessage message )
        {
            S_TEST_Handler?.Invoke((Protocol.S_TEST)message);
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
    }
}