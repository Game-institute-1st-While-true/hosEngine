#ifndef _MESH_COLLISION_H
#define _MESH_COLLISION_H

// [2020/1/11 ������]
// �޽� �ݸ���
// ������Ʈ�� �浹ó���� ���� ������Ʈ

// LOG
// [2020/01/20] ������ : ���� Ŭ������ ���� ��ӹ޵��� ����

#include "Collision.h"
#include "Mesh.h"

namespace hos::com
{
	class MeshCollision : public Collision
	{
	private:
		physx::PxCooking* Cooking;
		Mesh* Data;

		bool IsConvex;
		bool CookingFaster;
		bool CookingMeshCleaning;
		bool CookingVerticesWeld;
		bool CookingUseFastMidphase;

	public:
		MeshCollision(Mesh* mesh = nullptr, PhysicMaterial* material = nullptr);
		~MeshCollision();

	public:
		bool GetIsConvex()const;
		bool GetCookingFaster()const;
		bool GetCookingMeshCleaning()const;
		bool GetCookingVerticesWeld()const;
		bool GetCookingUseFastMidphase()const;

		void SetIsConvex(bool b);
		void SetIsTrigger(bool b);
		void SetCookingFaster(bool b);
		void SetCookingMeshCleaning(bool b);
		void SetCookingVerticesWeld(bool b);
		void SetUseFastMidphase(bool b);

	public:
		void Reset()override;
		void Awake()override;

		const std::vector<U8> Serialize() override { return std::vector<U8>(); }
		bool Deserialize(mbstring_view) override { return true; }
	};
}

#endif // !_MESH_COLLISION_H