#ifndef _COLLISION_H
#define _COLLISION_H

// [2020/1/19 ������]
// ���� �浹�� ���� �ݸ���
// ������Ʈ�� �浹ó���� ���� ������Ʈ

// LOG
// [2021/03/27] ������ : ���� ������ �߰�

#include "Component.h"
#include "GameObject.h"
#include "PhysX.h"
#include "PhysicMaterial.h"

namespace hos
{
	namespace com
	{
		class Collision : public Component
		{
		protected:
			//�ݸ����� ũ�Ⱑ 0�� ���ų� ������� ������ �����Ѵ� ���� ��ȿ�� ���е��� �����ʴ� �Ҽ��� 7��° �ڸ����� 0�� �����ʵ��� �Ѵ�.
			static constexpr float CORRECTION_VALUE = 0.0000001f;

		protected:
			PhysicMaterial* Material;
			bool IsTrigger;
			physx::PxShape* Shape;
			Vector3 Center;
			U32 Layer;

		public:
			Collision(string_view name = L"Collision", PhysicMaterial* material = nullptr);
			virtual ~Collision();

		protected:
			Collision(const Collision& dest);

		public:
			PhysicMaterial* GetMaterial();
			bool GetIsTrigger();
			physx::PxShape* GetShape();
			U32 GetLayer()const;
			void SetLayer(U32 index);
			Vector3 GetCenter()const;
			virtual void SetCenter(Vector3 center);

			void SetMaterial(PhysicMaterial* material);
			void SetIsTrigger(bool b);

		public:
			void Bind(hos::com::GameObject* gameObject);
			//ICallbale
			void Reset()override;
			void Awake()override;
			void Start()override;
			void OnEnable()override;
			void OnDisable()override;
		};
	}
}

#endif // !_COLLISION_H