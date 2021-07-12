#include "Rotator.h"
#include "hosMath.h"

using namespace hos;

const Rotator Rotator::Identity = Rotator();

hos::Rotator::Rotator(F32 pitch, F32 yaw, F32 roll) :
	x(pitch), y(yaw), z(roll)
{
	Normalize();
}

hos::Rotator::Rotator(Quaternion q)
{
	*this = q;
}

hos::Rotator::~Rotator()
{
}

void hos::Rotator::Normalize()
{
	while (true)
	{
		if (x < -180.f)
		{
			x += 360.f;
		}
		else if (x > 180.f)
		{
			x -= 360.f;
		}
		else
		{
			break;
		}
	}

	while (true)
	{
		if (y < -180.f)
		{
			y += 360.f;
		}
		else if (x > 180.f)
		{
			y -= 360.f;
		}
		else
		{
			break;
		}
	}

	while (true)
	{
		if (z < -180.f)
		{
			z += 360.f;
		}
		else if (x > 180.f)
		{
			z -= 360.f;
		}
		else
		{
			break;
		}
	}
}

Rotator& hos::Rotator::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	Normalize();

	return*this;
}

Rotator& hos::Rotator::operator=(const Quaternion& q)
{
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;
	float unit = sqx + sqy + sqz + sqw;
	float test = q.x * q.w - q.y * q.z;

	if (test > 0.4995f * unit)
	{
		y = 2.f * atan2f(q.y, q.w);
		x = PI<float> / 2.f;
		z = 0;
		x = RadToDegree(x);
		y = RadToDegree(y);
		z = RadToDegree(z);
		return *this;
	}
	if (test < -0.4995f * unit)
	{
		y = -2.f * atan2f(q.y, q.x);
		x = -PI<float> / 2.f;
		z = 0;
		x = RadToDegree(x);
		y = RadToDegree(y);
		z = RadToDegree(z);
		return *this;
	}

	Quaternion rot = Quaternion(q.w, q.z, q.x, q.y);
	y = atan2f(2.f * rot.x * rot.w + 2.f * rot.y * rot.z, 1 - 2.f * (rot.z * rot.z + rot.w * rot.w));     // Yaw
	x = asinf(2.f * (rot.x * rot.z - rot.w * rot.y));													 // Pitch
	z = atan2f(2.f * rot.x * rot.y + 2.f * rot.z * rot.w, 1 - 2.f * (rot.y * rot.y + rot.z * rot.z));    // Roll

	x = RadToDegree(x);
	y = RadToDegree(y);
	z = RadToDegree(z);
	return *this;
}

Rotator& hos::Rotator::operator-()
{
	x *= -1.f;
	y *= -1.f;
	z *= -1.f;

	return *this;
}

hos::Rotator::operator Vector3()
{
	return Vector3(x, y, z);
}

hos::Rotator::operator Quaternion()
{
	return Quaternion::CreateFromYawPitchRoll(DegreeToRad(y), DegreeToRad(x), DegreeToRad(z));
}

#ifdef PHYSX_MATH
hos::Rotator::Rotator(physx::PxQuat q)
{
	*this = q;
}

Rotator& hos::Rotator::operator=(const physx::PxQuat& q)
{
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;
	float unit = sqx + sqy + sqz + sqw;
	float test = q.x * q.w - q.y * q.z;

	if (test > 0.4995f * unit)
	{
		y = 2.f * atan2f(q.y, q.w);
		x = PI<float> / 2.f;
		z = 0;
		x = RadToDegree(x);
		y = RadToDegree(y);
		z = RadToDegree(z);
		return *this;
	}
	if (test < -0.4995f * unit)
	{
		y = -2.f * atan2f(q.y, q.x);
		x = -PI<float> / 2.f;
		z = 0;
		x = RadToDegree(x);
		y = RadToDegree(y);
		z = RadToDegree(z);
		return *this;
	}

	Quaternion rot = Quaternion(q.w, q.z, q.x, q.y);
	y = atan2f(2.f * rot.x * rot.w + 2.f * rot.y * rot.z, 1 - 2.f * (rot.z * rot.z + rot.w * rot.w));     // Yaw
	x = asinf(2.f * (rot.x * rot.z - rot.w * rot.y));													 // Pitch
	z = atan2f(2.f * rot.x * rot.y + 2.f * rot.z * rot.w, 1 - 2.f * (rot.y * rot.y + rot.z * rot.z));    // Roll

	x = RadToDegree(x);
	y = RadToDegree(y);
	z = RadToDegree(z);
	return *this;
}

hos::Rotator::operator physx::PxQuat()
{
	Quaternion q = Quaternion::CreateFromYawPitchRoll(DegreeToRad(y), DegreeToRad(x), DegreeToRad(z));
	return physx::PxQuat(q.x, q.y, q.z, q.w);
}
#endif // PHYSX_MATH

Rotator hos::operator+(const Rotator& lhs, const Rotator& rhs)
{
	return Rotator(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Rotator hos::operator-(const Rotator& lhs, const Rotator& rhs)
{
	return Rotator(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Rotator hos::operator*(const Rotator& r, float s)
{
	return Rotator(r.x * s, r.y * s, r.z * s);
}