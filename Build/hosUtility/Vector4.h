#ifndef _VECTOR4_H
#define _VECTOR4_H

// [2021/1/12 ������]
// vector4
// �������� vector4 Ÿ���� ������ ������ �� vector4Ŭ����
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

//LOG
// [2121/03/19] ������ : ���۷����� ������������ ������ �߻��ؼ� simpleMath�� ��ӹް� PhysX�� ĳ���� ���۷����͸� �����ε��ؼ� �ذ��Ѵ�.

#include "BaseTypes.h"

namespace hos
{
	class Vector4 : public DirectX::SimpleMath::Vector4
	{
	public:
		Vector4(F32 x = 0.f, F32 y = 0.f, F32 z = 0.f, F32 w = 0.f);
		Vector4(DirectX::SimpleMath::Vector4 vec);
		~Vector4();

		Vector4& operator=(const DirectX::SimpleMath::Vector4& vec);
		Vector4& operator=(const Vector4& vec);

	#ifdef PHYSX_MATH
		Vector4(physx::PxVec4 vec);
		Vector4& operator=(const physx::PxVec4& vec);
		operator physx::PxVec4();
	#endif // PHYSX_MATH

	};
}

#endif // !_VECTOR4_H