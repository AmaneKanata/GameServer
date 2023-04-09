#include "../Base/ClientBase.h"

class GameObject;

class GameObjectClient : public ClientBase
{
public:
	GameObjectClient(string clientId) : ClientBase(clientId)
	{}
	
	shared_ptr<GameObject> gameObject;
};