#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <btBulletCollisionCommon.h>

#include "Protocols.h"
#include "Server_Singleton.h"
#include "LogManager.h"

class FPSCharacter
{
public:
	FPSCharacter(int id, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt)
		: id(id)
	{
		setPosition.set_playerid(id);
		setRotation.set_playerid(id);
		
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

			btTransform transform;
			transform.setOrigin(btVector3(pkt->position().x() * -1, pkt->position().y() + 1.0f, pkt->position().z()));
			btQuaternion initialRotation;
			initialRotation.setEulerZYX(pkt->rotation().z() * (M_PI / 180.0f), pkt->rotation().y() * (M_PI / 180.0f) * -1, pkt->rotation().x() * (M_PI / 180.0f));
			collisionObject->setWorldTransform(transform);
		}

		long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		auto initialVelocity = new Protocol::Vector3();
		initialVelocity->set_x(0);
		initialVelocity->set_y(0);
		initialVelocity->set_z(0);
		UpdatePosition(now, pkt->release_position(), initialVelocity);

		UpdateRotation(pkt->release_rotation());
	}

	void UpdatePosition(long long timestamp, Protocol::Vector3* position, Protocol::Vector3* velocity)
	{
		setPosition.set_timestamp(timestamp);
		setPosition.set_allocated_position(position);
		setPosition.set_allocated_velocity(velocity);

		this->timestamp = timestamp;
		this->position.setValue(position->x() * -1, position->y() + 1.0f, position->z());
		this->velocity.setValue(velocity->x() * -1, velocity->y(), velocity->z());
	}

	void UpdateRotation(Protocol::Vector3* rotation)
	{
		isRotationDirty = true;
		setRotation.set_allocated_rotation(rotation);

		this->rotation.setEulerZYX(rotation->z() * (M_PI / 180.0f), rotation->y() * (M_PI / 180.0f) * -1, rotation->x() * (M_PI / 180.0f));

		transform.setRotation(this->rotation);
		collisionObject->setWorldTransform(transform);
	}

	void UpdateAnimation(std::shared_ptr<Protocol::C_SET_ANIMATION> pkt)
	{
		isAnimationDirty = true;

		setAnimation.set_gameobjectid(pkt->gameobjectid());
		setAnimation.mutable_params()->swap(*pkt->mutable_params());
	}

public:
	int id;

	std::shared_ptr<btCollisionObject> collisionObject;

	btTransform transform;

	long long timestamp;
	btVector3 position;
	btVector3 velocity;
	Protocol::S_SET_FPS_POSITION setPosition;

	bool isRotationDirty = false;
	btQuaternion rotation;
	Protocol::S_SET_FPS_ROTATION setRotation;

	bool isAnimationDirty = false;
	Protocol::S_SET_ANIMATION setAnimation;
};

class FPSPlayer : public FPSCharacter
{
public:
	FPSPlayer(std::string ownerId, int id, std::shared_ptr<Protocol::C_INSTANTIATE_FPS_PLAYER> pkt)
		: FPSCharacter(id, pkt)
		, ownerId(ownerId)
	{
	}

public:
	std::string ownerId;
	int hp = 100;
};