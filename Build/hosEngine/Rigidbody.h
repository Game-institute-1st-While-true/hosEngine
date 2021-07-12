#ifndef _RIGIDBODY_H
#define _RIGIDBODY_H

// [2020/12/10 ������]
// �����ٵ�
// ������Ʈ�� ���������� ���� ������Ʈ
// �ӽ� ���� �س��´�

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�

#include "Component.h"
#include "PhysX.h"

namespace hos::com
{
	class Rigidbody : public Component
	{
	public:
		static constexpr float DEFAULT_MASS = 1.f;
		static constexpr float DEFAULT_DRAG = 0.f;
		static constexpr float DEFAULT_ANGULAR_DRAG = 0.005f;
		static constexpr bool DEFAULT_USE_GRAVITY = true;
		static constexpr bool DEFAULT_IS_KINEMATIC = false;
		static constexpr bool DEFAULT_FREEZE = false;

		enum eForceMode
		{
			FORCE_MODE_FORCE = physx::PxForceMode::eFORCE,
			FORCE_MODE_IMPULSE = physx::PxForceMode::eIMPULSE,
			FORCE_MODE_VELOCITYCHANGE = physx::PxForceMode::eVELOCITY_CHANGE,
			FORCE_MODE_ACCELERATION = physx::PxForceMode::eACCELERATION,

			NUM_FORCE_MODE
		};
	private:
		//physx�� �����ٵ� GameObject�� PhisicsActor�� PxRigidDynamic������
		physx::PxRigidDynamic* Actor;

		//���� [ 0 ~ F32_MAX ]
		float Mass;
		//���� (��������) [ 0 ~ F32_MAX ]
		float Drag;
		//ȸ�� ���� (ȸ�� ���� ���) [ 0 ~ F32_MAX ]
		float AngularDrag;
		//�߷� �ۿ� ���� setActorFalg() - eDISABLE_GRAVITY
		bool UseGravity;
		//�����ùķ��̼� ���� setRigidBodyFlag() - eKINEMATIC, setKinematicTarget()
		bool IsKinematic;

		//���� ��ġ, ȸ�� �� ��� ����
		bool FreezePositionX, FreezePositionY, FreezePositionZ;
		bool FreezeRotationX, FreezeRotationY, FreezeRotationZ;

	public:
		Rigidbody();
		virtual ~Rigidbody();

	protected:
		Rigidbody(const Rigidbody& dest);

	public:
		float GetMass()const;
		float GetDrag()const;
		float GetAngularDrag()const;
		bool GetUseGravity()const;
		bool GetIsKinematic()const;
		bool GetFreezePositionX()const;
		bool GetFreezePositionY()const;
		bool GetFreezePositionZ()const;
		bool GetFreezeRotationX()const;
		bool GetFreezeRotationY()const;
		bool GetFreezeRotationZ()const;

		void SetMass(float mass);
		void SetDrag(float drag);
		void SetAngularDrag(float angularDrag);
		void SetUseGravity(bool b);
		void SetIsKinematic(bool b);
		void SetFreezePositionX(bool b);
		void SetFreezePositionY(bool b);
		void SetFreezePositionZ(bool b);
		void SetFreezeRotationX(bool b);
		void SetFreezeRotationY(bool b);
		void SetFreezeRotationZ(bool b);

		void AddForce(Vector3 force, eForceMode mode = FORCE_MODE_FORCE);
		void AddTorque(Vector3 torque, eForceMode mode = FORCE_MODE_FORCE);
		void ClearForce();
		void ClearTorque();

	public:
		void Bind(GameObject* gameObject) override;
		void UnBind(int index) override;
		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;

		void SetActive(bool) = delete;

		Rigidbody* Clone()const override;
		void Reset()override;
		void Awake()override;
		void OnEnable()override;
		void OnDisable()override;
	};
}

#endif // !_RIGIDBODY_H