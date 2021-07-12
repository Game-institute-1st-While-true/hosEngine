#include "Vector3.h"
#include "Matrix.h"

using namespace hos;
using namespace DirectX;

hos::Vector3::Vector3(F32 x, F32 y, F32 z) :
	SimpleMath::Vector3(x, y, z)
{
}



hos::Vector3::Vector3(DirectX::SimpleMath::Vector3 vec) :
	SimpleMath::Vector3(vec)
{
}

hos::Vector3::~Vector3()
{
}

hos::Vector3& hos::Vector3::operator=(const DirectX::SimpleMath::Vector3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;

	return *this;
}

hos::Vector3& hos::Vector3::operator=(const Vector3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;

	return *this;
}

#ifdef PHYSX_MATH
hos::Vector3::Vector3(physx::PxVec3 vec) :
	SimpleMath::Vector3(vec.x, vec.y, vec.z)
{
}

hos::Vector3& hos::Vector3::operator=(const physx::PxVec3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;

	return *this;
}

hos::Vector3::operator physx::PxVec3()
{
	return physx::PxVec3(x, y, z);
}
#endif // PHYSX_MATH

Vector3 hos::operator*(const Vector3& v, const Matrix& m)
{
	return Vector3::Transform(v, m);
}

Vector3 hos::operator*(const Vector3& v, float s)
{
	return SimpleMath::operator*(v,s);
}
