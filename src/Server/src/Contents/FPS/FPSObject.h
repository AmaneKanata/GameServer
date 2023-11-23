#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <btBulletCollisionCommon.h>

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
		//collisionObject->setCollisionShape(compound);
		
		//btBoxShape* box = new btBoxShape(btVector3(0.5f, 1.0f, 0.5f));
		//collisionObject->setCollisionShape(box);

		btCapsuleShape* capsule = new btCapsuleShape(0.5f, 1.0f);
		collisionObject->setCollisionShape(capsule);

		collisionObject->setCustomDebugColor(btVector3(0, 0, 0));

		transform.setIdentity();
		position.setY(position.y() + 1);
		transform.setOrigin(position);
		transform.setRotation(rotation);
		collisionObject->setWorldTransform(transform);

		long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		this->timestamp = now;
		this->position = position;
		this->velocity = btVector3(0, 0, 0);
	}

	~FPSCharacter()
	{
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