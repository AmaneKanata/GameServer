#pragma once

#include "../../pch.h"
#include "../../Protocols.h"

class GameObject
{
public:
	GameObject(int gameObjectId)
		: gameObjectId(gameObjectId)
		, position_x(0)
		, position_y(0)
		, position_z(0)
		, rotation_x(0)
		, rotation_y(0)
		, rotation_z(0)
	{
		GLogManager->Log("GameObject Created :			", to_string(gameObjectId));
	};
	~GameObject()
	{
		GLogManager->Log("GameObject Destroyed :		", to_string(gameObjectId));
	}
	
	void MakeGameObjectInfo(Protocol::S_ADD_GAME_OBJECT_GameObjectInfo* gameObjectInfo)
	{
		gameObjectInfo->set_id(gameObjectId);
		auto _position = gameObjectInfo->mutable_position();
		_position->set_x(position_x);
		_position->set_y(position_y);
		_position->set_z(position_z);
		auto _rotation = gameObjectInfo->mutable_rotation();
		_rotation->set_x(rotation_x);
		_rotation->set_y(rotation_y);
		_rotation->set_z(rotation_z);
	}

	Protocol::S_SET_TRANSFORM MakeTransform()
	{
		Protocol::S_SET_TRANSFORM transform;

		transform.set_gameobjectid(gameObjectId);

		auto _position = transform.mutable_position();
		_position->set_x(position_x);
		_position->set_y(position_y);
		_position->set_z(position_z);

		auto _rotation = transform.mutable_rotation();
		_rotation->set_x(rotation_x);
		_rotation->set_y(rotation_y);
		_rotation->set_z(rotation_z);

		return transform;
	}

	void SetPosition(Protocol::Vector3 position)
	{
		position_x = position.x();
		position_y = position.y();
		position_z = position.z();
	}

	void SetRotation(Protocol::Vector3 rotation)
	{
		rotation_x = rotation.x();
		rotation_y = rotation.y();
		rotation_z = rotation.z();
	}

	int gameObjectId;
	
	float position_x;
	float position_y;
	float position_z;

	float rotation_x;
	float rotation_y;
	float rotation_z;
};

