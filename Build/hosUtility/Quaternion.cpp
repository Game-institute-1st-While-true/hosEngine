#include "Quaternion.h"
#include "Vector3.h"
#include "Rotator.h"

using namespace hos;
using namespace DirectX;

hos::Quaternion::Quaternion(float x, float y, float z, float w) :
	SimpleMath::Quaternion(x, y, z, w)
{
}

hos::Quaternion::Quaternion(DirectX::SimpleMath::Quaternion quat) :
	SimpleMath::Quaternion(quat)
{
}

hos::Quaternion::Quaternion(Rotator r):
	SimpleMath::Quaternion(r)
{
}

hos::Quaternion::~Quaternion()
{
}

hos::Quaternion& hos::Quaternion::operator=(const DirectX::SimpleMath::Quaternion& quat)
{
	x = quat.x;
	y = quat.y;
	z = quat.z;
	w = quat.w;

	return *this;
}

Quaternion& hos::Quaternion::operator=(const Quaternion& quat)
{
	x = quat.x;
	y = quat.y;
	z = quat.z;
	w = quat.w;

	return *this;
}

Vector3 hos::Quaternion::ToEuler()
{
	float roll, pitch, yaw;
	XMVECTOR x = Vector3::UnitX;
	XMVECTOR y = Vector3::UnitY;
	XMVECTOR z = Vector3::UnitZ;
	FXMVECTOR q = *this;
	XMQuaternionToAxisAngle(&x, &roll, q);
	XMQuaternionToAxisAngle(&y, &pitch, q);
	XMQuaternionToAxisAngle(&z, &yaw, q);

	return Vector3(roll, pitch, yaw);
}

#ifdef PHYSX_MATH
hos::Quaternion::Quaternion(physx::PxQuat quat) :
	SimpleMath::Quaternion(quat.x, quat.y, quat.z, quat.w)
{
}

hos::Quaternion& hos::Quaternion::operator=(const physx::PxQuat& quat)
{
	x = quat.x;
	y = quat.y;
	z = quat.z;
	w = quat.w;

	return *this;
}

hos::Quaternion::operator physx::PxQuat()
{
	return physx::PxQuat(x, y, z, w);
}
#endif // !PHYSX_MATH