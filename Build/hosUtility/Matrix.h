#ifndef _MATRIX_H
#define _MATRIX_H

// [2020/12/31 ������]
// ���
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2021/1/12 ������] PxMat44�� SimpleMath::Matrix�� �̸� �浹�� ���⶧���� ������ ������ �ʴ´�.
// [2121/03/19] ������ : ���۷����� ������������ ������ �߻��ؼ� simpleMath�� ��ӹް� PhysX�� ĳ���� ���۷����͸� �����ε��ؼ� �ذ��Ѵ�.

#include "BaseTypes.h"

namespace hos
{
	class Matrix :public DirectX::SimpleMath::Matrix
	{
	public:
		Matrix(float m00 = 1.f, float m01 = 0.f, float m02 = 0.f, float m03 = 0.f,
			float m10 = 0.f, float m11 = 1.f, float m12 = 0.f, float m13 = 0.f,
			float m20 = 0.f, float m21 = 0.f, float m22 = 1.f, float m23 = 0.f,
			float m30 = 0.f, float m31 = 0.f, float m32 = 0.f, float m33 = 1.f);
		Matrix(DirectX::SimpleMath::Matrix matrix);
		~Matrix();

		Matrix& operator=(const DirectX::SimpleMath::Matrix& mat);
		Matrix& operator=(const Matrix& mat);

	#ifdef PHYSX_MATH
		Matrix(physx::PxMat44 matrix);
		Matrix& operator=(const physx::PxMat44& mat);
		operator physx::PxMat44();
	#endif // PHYSX_MATH
	};
}

#endif // !_MATRIX_H