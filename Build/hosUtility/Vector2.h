#ifndef _VECTOR2_H
#define _VECTOR2_H

// [2020/12/21 ������]
// vector2
// �������� vector2 Ÿ���� ������ ������ �� vector2Ŭ����
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2121/03/19] ������ : ���۷����� ������������ ������ �߻��ؼ� simpleMath�� ��ӹް� PhysX�� ĳ���� ���۷����͸� �����ε��ؼ� �ذ��Ѵ�.

#include "BaseTypes.h"

namespace hos
{
	class Vector2 : public DirectX::SimpleMath::Vector2
	{
	public:
		Vector2(F32 x = 0.f, F32 y = 0.f);
		Vector2(DirectX::SimpleMath::Vector2 vec);
		~Vector2();

		Vector2& operator=(const DirectX::SimpleMath::Vector2& vec);
		Vector2& operator=(const Vector2& vec);

	#ifdef PHYSX_MATH
		Vector2(physx::PxVec2 vec);
		Vector2& operator=(const physx::PxVec2& vec);
		operator physx::PxVec2();
	#endif // PHYSX_MATH

	};
}

#endif // !_VECTOR2_H