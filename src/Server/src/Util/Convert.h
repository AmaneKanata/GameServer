#include "Protocols.h"
#include "btBulletDynamicsCommon.h"

static Protocol::Vector3* ConvertVector3(btVector3 vec3)
{
	Protocol::Vector3* res = new Protocol::Vector3();

	res->set_x(vec3.getX() * -1);
	res->set_y(vec3.getY());
	res->set_z(vec3.getZ());

	return res;
}

static btVector3 ConvertVector3(Protocol::Vector3 vec3)
{
	return btVector3(vec3.x() * -1, vec3.y(), vec3.z());
}

static Protocol::Quaternion* ConvertQuaternion(btQuaternion quat)
{
	Protocol::Quaternion* res = new Protocol::Quaternion();

	res->set_x(quat.getX());
	res->set_y(quat.getY() * -1);
	res->set_z(quat.getZ());
	res->set_w(quat.getW());

	return res;
}

static btQuaternion ConvertQuaternion(Protocol::Quaternion quat)
{
	return btQuaternion(quat.x(), quat.y() * -1, quat.z(), quat.w());
}