#include "Vector2.h"

using namespace hos;
using namespace DirectX;

hos::Vector2::Vector2(F32 x, F32 y) :
	SimpleMath::Vector2(x, y)
{
}

hos::Vector2::Vector2(DirectX::SimpleMath::Vector2 vec) :
	SimpleMath::Vector2(vec)
{
}

hos::Vector2::~Vector2()
{
}

hos::Vector2& hos::Vector2::operator=(const DirectX::SimpleMath::Vector2& vec)
{
	x = vec.x;
	y = vec.y;

	return *this;
}

hos::Vector2& hos::Vector2::operator=(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;

	return *this;
}

#ifdef PHYSX_MATH
hos::Vector2::Vector2(physx::PxVec2 vec) :
	SimpleMath::Vector2(vec.x, vec.y)
{
}

hos::Vector2& hos::Vector2::operator=(const physx::PxVec2& vec)
{
	x = vec.x;
	y = vec.y;

	return *this;
}

hos::Vector2::operator physx::PxVec2()
{
	return physx::PxVec2(x, y);
}
#endif // PHYSX_MATH