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
        private Action<Protocol.S_TEST> S_TEST_Handler;
        private Action<Protocol.S_INSTANTIATE_GAME_OBJECT> S_INSTANTIATE_GAME_OBJECT_Handler;
        private Action<Protocol.S_ADD_GAME_OBJECT> S_ADD_GAME_OBJECT_Handler;
        private Action<Protocol.S_DESTORY_GAME_OBJECT> S_DESTORY_GAME_OBJECT_Handler;
        private Action<Protocol.S_REMOVE_GAME_OBJECT> S_REMOVE_GAME_OBJECT_Handler;
        private Action<Protocol.S_SET_GAME_OBJECT_PREFAB> S_SET_GAME_OBJECT_PREFAB_Handler;
        private Action<Protocol.S_SET_GAME_OBJECT_OWNER> S_SET_GAME_OBJECT_OWNER_Handler;
        private Action<Protocol.S_SET_TRANSFORM> S_SET_TRANSFORM_Handler;
        private Action<Protocol.S_SET_ANIMATION> S_SET_ANIMATION_Handler;
        private Action<Protocol.S_ADD_FPS_PLAYER> S_ADD_FPS_PLAYER_Handler;
        private Action<Protocol.S_SET_FPS_POSITION> S_SET_FPS_POSITION_Handler;
        private Action<Protocol.S_SET_FPS_ROTATION> S_SET_FPS_ROTATION_Handler;
        private Action<Protocol.S_SHOT> S_SHOT_Handler;
        private Action<Protocol.S_ATTACKED> S_ATTACKED_Handler;

        public PacketHandler()
        {
            Handlers.Add(1, _Handle_S_ENTER);
            Handlers.Add(3, _Handle_S_REENTER);
            Handlers.Add(6, _Handle_S_ADD_CLIENT);
            Handlers.Add(7, _Handle_S_REMOVE_CLIENT);
            Handlers.Add(8, _Handle_S_DISCONNECT);
            Handlers.Add(11, _Handle_S_PING);
            Handlers.Add(13, _Handle_S_SERVERTIME);
            Handlers.Add(15, _Handle_S_TEST);
            Handlers.Add(101, _Handle_S_INSTANTIATE_GAME_OBJECT);
            Handlers.Add(103, _Handle_S_ADD_GAME_OBJECT);
            Handlers.Add(105, _Handle_S_DESTORY_GAME_OBJECT);
            Handlers.Add(106, _Handle_S_REMOVE_GAME_OBJECT);
            Handlers.Add(108, _Handle_S_SET_GAME_OBJECT_PREFAB);
            Handlers.Add(110, _Handle_S_SET_GAME_OBJECT_OWNER);
            Handlers.Add(112, _Handle_S_SET_TRANSFORM);
            Handlers.Add(114, _Handle_S_SET_ANIMATION);
            Handlers.Add(201, _Handle_S_ADD_FPS_PLAYER);
            Handlers.Add(203, _Handle_S_SET_FPS_POSITION);
            Handlers.Add(205, _Handle_S_SET_FPS_ROTATION);
            Handlers.Add(207, _Handle_S_SHOT);
            Handlers.Add(208, _Handle_S_ATTACKED);
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
        public void AddHandler( Action<Protocol.S_SET_GAME_OBJECT_PREFAB> handler )
        {
            S_SET_GAME_OBJECT_PREFAB_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_GAME_OBJECT_PREFAB> handler )
        {
            S_SET_GAME_OBJECT_PREFAB_Handler -= handler;
        }
        private void _Handle_S_SET_GAME_OBJECT_PREFAB( IMessage message )
        {
            S_SET_GAME_OBJECT_PREFAB_Handler?.Invoke((Protocol.S_SET_GAME_OBJECT_PREFAB)message);
        }
        public void AddHandler( Action<Protocol.S_SET_GAME_OBJECT_OWNER> handler )
        {
            S_SET_GAME_OBJECT_OWNER_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_GAME_OBJECT_OWNER> handler )
        {
            S_SET_GAME_OBJECT_OWNER_Handler -= handler;
        }
        private void _Handle_S_SET_GAME_OBJECT_OWNER( IMessage message )
        {
            S_SET_GAME_OBJECT_OWNER_Handler?.Invoke((Protocol.S_SET_GAME_OBJECT_OWNER)message);
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
        public void AddHandler( Action<Protocol.S_ADD_FPS_PLAYER> handler )
        {
            S_ADD_FPS_PLAYER_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_ADD_FPS_PLAYER> handler )
        {
            S_ADD_FPS_PLAYER_Handler -= handler;
        }
        private void _Handle_S_ADD_FPS_PLAYER( IMessage message )
        {
            S_ADD_FPS_PLAYER_Handler?.Invoke((Protocol.S_ADD_FPS_PLAYER)message);
        }
        public void AddHandler( Action<Protocol.S_SET_FPS_POSITION> handler )
        {
            S_SET_FPS_POSITION_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_FPS_POSITION> handler )
        {
            S_SET_FPS_POSITION_Handler -= handler;
        }
        private void _Handle_S_SET_FPS_POSITION( IMessage message )
        {
            S_SET_FPS_POSITION_Handler?.Invoke((Protocol.S_SET_FPS_POSITION)message);
        }
        public void AddHandler( Action<Protocol.S_SET_FPS_ROTATION> handler )
        {
            S_SET_FPS_ROTATION_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SET_FPS_ROTATION> handler )
        {
            S_SET_FPS_ROTATION_Handler -= handler;
        }
        private void _Handle_S_SET_FPS_ROTATION( IMessage message )
        {
            S_SET_FPS_ROTATION_Handler?.Invoke((Protocol.S_SET_FPS_ROTATION)message);
        }
        public void AddHandler( Action<Protocol.S_SHOT> handler )
        {
            S_SHOT_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_SHOT> handler )
        {
            S_SHOT_Handler -= handler;
        }
        private void _Handle_S_SHOT( IMessage message )
        {
            S_SHOT_Handler?.Invoke((Protocol.S_SHOT)message);
        }
        public void AddHandler( Action<Protocol.S_ATTACKED> handler )
        {
            S_ATTACKED_Handler += handler;
        }
        public void RemoveHandler( Action<Protocol.S_ATTACKED> handler )
        {
            S_ATTACKED_Handler -= handler;
        }
        private void _Handle_S_ATTACKED( IMessage message )
        {
            S_ATTACKED_Handler?.Invoke((Protocol.S_ATTACKED)message);
        }
    }
}