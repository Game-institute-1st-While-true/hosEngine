#include "Vector4.h"

using namespace hos;
using namespace DirectX;

hos::Vector4::Vector4(F32 x, F32 y, F32 z, F32 w) :
	SimpleMath::Vector4(x, y, z, w)
{
}



hos::Vector4::Vector4(DirectX::SimpleMath::Vector4 vec) :
	SimpleMath::Vector4(vec)
{
}

hos::Vector4::~Vector4()
{
}

hos::Vector4& hos::Vector4::operator=(const DirectX::SimpleMath::Vector4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;

	return *this;
}

hos::Vector4& hos::Vector4::operator=(const Vector4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;

	return *this;
}

#ifdef PHYSX_MATH
hos::Vector4::Vector4(physx::PxVec4 vec) :
	SimpleMath::Vector4(vec.x, vec.y, vec.z, vec.w)
{
}

hos::Vector4& hos::Vector4::operator=(const physx::PxVec4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;

	return *this;
}

hos::Vector4::operator physx::PxVec4()
{
	return physx::PxVec4(x, y, z, w);
}

#endif // PHYSX_MATH