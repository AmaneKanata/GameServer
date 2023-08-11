#pragma once

#include "Protocols.h"
#include "Server_Singleton.h"
#include "LogManager.h"

class GameObject
{
public:
	GameObject(int gameObjectId)
		: gameObjectId(gameObjectId)
	{
		//GLogManager->Log("GameObject Created : ", std::to_string(gameObjectId));
	};
	~GameObject()
	{	
		//GLogManager->Log("GameObject Destroyed : ", std::to_string(gameObjectId));
	}

	void SetPosition(Protocol::Vector3 position)
	{
		transform.mutable_position()->CopyFrom(position);
	}

	void SetRotation(Protocol::Vector3 rotation)
	{
		transform.mutable_rotation()->CopyFrom(rotation);
	}

	void MakeGameObjectInfo(Protocol::S_ADD_GAME_OBJECT_GameObjectInfo* gameObjectInfo)
	{
		gameObjectInfo->set_id(gameObjectId);
		gameObjectInfo->set_prefabname(prefabName);
		gameObjectInfo->mutable_position()->CopyFrom(transform.position());
		gameObjectInfo->mutable_rotation()->CopyFrom(transform.rotation());
	}

	void UpdateTransform(std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt)
	{
		isDirty = true;

		transform.set_timestamp(pkt->timestamp());
		transform.set_gameobjectid(pkt->gameobjectid());
		transform.set_allocated_position(pkt->release_position());
		transform.set_allocated_velocity(pkt->release_velocity());
		transform.set_allocated_rotation(pkt->release_rotation());
		transform.set_allocated_angularvelocity(pkt->release_angularvelocity());
	}

public:
	int gameObjectId;
	std::string prefabName;

	bool isDirty = false;

	Protocol::S_SET_TRANSFORM transform;
};