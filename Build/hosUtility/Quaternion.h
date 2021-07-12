#ifndef _QUATERNION_H
#define _QUATERNION_H

// [2020/12/31 ������]
// ���ʹϾ�
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2021/1/12 ������] PxQuat�� protected����ؼ� PxQuat�� �ɹ��� �����������ϵ��� �ؼ� �ɹ��������� ��ȣ���� ���ַ��� �Ͽ�����
// �����ϵ��� �ʾƼ� �ڽ�Ŭ�������� ���۷���Ÿ���� ������ �ɹ������� ��ȣ���� �����Ͽ���.
// SimpleMath�� Quaternion�� public����ؼ� �ش� �ɹ��Լ����� �ٷ� ������ �����ְ� �ϰ�, ���۷���Ÿ���� �ش� �ɹ��� ����Ű�����ؼ�
// �ɹ��������� ���� ���Ҷ� SimpleMath������ �ɹ������� �Բ� ���ϵ��� �ߴ�. 
// PxQuat�� protected�� ��ӹ޾ұ⶧���� PxQuat���� ����ȯ�� �ڵ����� �����ʴ� ������ ����µ� Ŭ���� ���ο����� ��ȯ�� �� ��������
// To�Լ��� ���� PxQuat�� ������ �����ϰ� ����ȯ �ؼ� �����ϵ��� �ߴ�.
// �ٸ� ���� Ŭ�����鵵 �̿Ͱ��� ���� ����
// [2121/03/19] ������ : ���۷����� ������������ ������ �߻��ؼ� simpleMath�� ��ӹް� PhysX�� ĳ���� ���۷����͸� �����ε��ؼ� �ذ��Ѵ�.

#include "BaseTypes.h"

namespace hos
{
	class Vector3;
	class Rotator;
	class Quaternion :public DirectX::SimpleMath::Quaternion
	{
	public:
		Quaternion(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
		Quaternion(DirectX::SimpleMath::Quaternion quat);
		Quaternion(Rotator r);
		~Quaternion();

		Quaternion& operator=(const DirectX::SimpleMath::Quaternion& quat);
		Quaternion& operator=(const Quaternion& quat);

		Vector3 ToEuler();

	#ifdef PHYSX_MATH
		Quaternion(physx::PxQuat quat);
		Quaternion& operator=(const physx::PxQuat& quat);
		operator physx::PxQuat();
	#endif // PHYSX_MATH
	};
}

#endif // !_QUATERNION_H