#ifndef _VECTOR3_H
#define _VECTOR3_H

// [2020/12/31 ������]
// vector3
// �������� vector3 Ÿ���� ������ ������ �� vector3Ŭ����
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2121/03/19] ������ : ���۷����� ������������ ������ �߻��ؼ� simpleMath�� ��ӹް� PhysX�� ĳ���� ���۷����͸� �����ε��ؼ� �ذ��Ѵ�.

#include "BaseTypes.h"

namespace hos
{
	class Matrix;
	class Vector3 : public DirectX::SimpleMath::Vector3
	{
	public:
		Vector3(F32 x = 0.f, F32 y = 0.f, F32 z = 0.f);
		Vector3(DirectX::SimpleMath::Vector3 vec);
		~Vector3();

		Vector3& operator=(const DirectX::SimpleMath::Vector3& vec);
		Vector3& operator=(const Vector3& vec);

	#ifdef PHYSX_MATH
		Vector3(physx::PxVec3 vec);
		Vector3& operator=(const physx::PxVec3& vec);
		operator physx::PxVec3();
	#endif // PHYSX_MATH
	};

	Vector3 operator*(const Vector3& v, const Matrix& m);
	Vector3 operator*(const Vector3& v, float s);
}

#endif // !_VECTOR3_H