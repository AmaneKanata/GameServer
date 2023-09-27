#pragma once

#include "Protocols.h"
#include "Server_Singleton.h"
#include "LogManager.h"

enum class GameObjectType
{
	PLAYER = 1,
	ETC = 99
};

class GameObject
{
public:
	GameObject(int id)
		: id(id)
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
		gameObjectInfo->set_gameobjectid(id);
		gameObjectInfo->set_ownerid(ownerId);
		gameObjectInfo->set_type((int)type);
		gameObjectInfo->set_prefabname(prefabName);
		gameObjectInfo->mutable_position()->CopyFrom(transform.position());
		gameObjectInfo->mutable_rotation()->CopyFrom(transform.rotation());
	}

	void UpdateTransform(std::shared_ptr<Protocol::C_SET_TRANSFORM> pkt)
	{
		isTransformDirty = true;

		transform.set_timestamp(pkt->timestamp());
		transform.set_gameobjectid(pkt->gameobjectid());
		transform.set_allocated_position(pkt->release_position());
		transform.set_allocated_velocity(pkt->release_velocity());
		transform.set_allocated_rotation(pkt->release_rotation());
		transform.set_allocated_angularvelocity(pkt->release_angularvelocity());
	}

	void UpdateAnimation(std::shared_ptr<Protocol::C_SET_ANIMATION> pkt)
	{
		isAnimationDirty = true;

		setAnimation.set_gameobjectid(pkt->gameobjectid());
		setAnimation.mutable_params()->swap(*pkt->mutable_params());
	}

public:
	int id;
	GameObjectType type;
	std::string ownerId;
	std::string prefabName;

	bool isTransformDirty = false;
	bool isAnimationDirty = false;

	Protocol::S_SET_TRANSFORM transform;
	Protocol::S_SET_ANIMATION setAnimation;
};