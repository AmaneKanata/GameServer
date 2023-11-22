#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <btBulletCollisionCommon.h>

#include "Protocols.h"
#include "Server_Singleton.h"
#include "LogManager.h"

const float ROTATION_BIAS = (M_PI / 180.0f);

class FPSCharacter
{
public:
	FPSCharacter(int id, btVector3 position, btQuaternion rotation)
		: id(id)
	{
		collisionObject = std::make_shared<btCollisionObject>();
		collisionObject->setUserIndex(id);

		//btCapsuleShape* capsule = new btCapsuleShape(0.5f, 1.0f);
		//btBoxShape* box = new btBoxShape(btVector3(0.3f, 0.3f, 0.3f));
		//btCompoundShape* compound = new btCompoundShape();
		//btTransform capsuleTransform;
		//capsuleTransform.setIdentity();
		//compound->addChildShape(capsuleTransform, capsule);
		//btTransform boxTransform;
		//boxTransform.setIdentity();
		//boxTransform.setOrigin(btVector3(0, 0, 0.5f));
		//compound->addChildShape(boxTransform, box);
		//btBoxShape* box = new btBoxShape(btVector3(0.5f, 1.0f, 0.5f));
		//collisionObject->setCollisionShape(box);

		btCapsuleShape* capsule = new btCapsuleShape(0.5f, 1.0f);
		collisionObject->setCollisionShape(capsule);

		transform.setIdentity();
		position.setY(position.y() + 1);
		transform.setOrigin(position);
		transform.setRotation(rotation);
		collisionObject->setWorldTransform(transform);

		long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		this->timestamp = now;
		this->position = position;
		this->velocity = btVector3(0, 0, 0);
		this->rotation = rotation;
	}

	void SetPosition(Protocol::Vector3 position)
	{
		this->position.setValue(position.x() * -1, position.y() + 1.0f, position.z());
	}

	btVector3 SetVelocity(Protocol::Vector3 velocity)
	{
		this->velocity.setValue(velocity.x() * -1, velocity.y(), velocity.z());
	}

	btQuaternion SetRotation(Protocol::Vector3 rotation)
	{
		this->rotation.setEulerZYX(rotation.z() * ROTATION_BIAS, rotation.y() * ROTATION_BIAS * -1, rotation.x() * ROTATION_BIAS);
		transform.setRotation(this->rotation);
	}

	void UpdateTransform()
	{
		collisionObject->setWorldTransform(transform);
	}

public:
	int id;

	std::shared_ptr<btCollisionObject> collisionObject;
	btTransform transform;
	long long timestamp;
	btVector3 position;
	btVector3 velocity;
	btQuaternion rotation;
};

class FPSPlayer : public FPSCharacter
{
public:
	FPSPlayer(std::string ownerId, int id, btVector3 position, btQuaternion rotation)
		: FPSCharacter(id, position, rotation)
		, ownerId(ownerId)
	{
	}

public:
	std::string ownerId;
	int hp = 100;
};