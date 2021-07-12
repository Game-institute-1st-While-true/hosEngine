#ifndef _SPHERE_COLLISION_H
#define _SPHERE_COLLISION_H

// [2020/1/11 ������]
// �� �ݸ���
// ������Ʈ�� �浹ó���� ���� ������Ʈ

// LOG
// [2020/01/19] ������ : ���� Ŭ������ ���� ��ӹ޵��� ����
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�

#include "Collision.h"

namespace hos::com
{
	class SphereCollision : public Collision
	{
	public:
		static constexpr float DEFAULT_RADIUS = 0.5f;

	private:
		float Radius;

	public:
		SphereCollision(float radius = DEFAULT_RADIUS, PhysicMaterial* material = nullptr);
		virtual ~SphereCollision();

	protected:
		SphereCollision(const SphereCollision& dest);

	public:
		float GetRadius();
		void SetRadius(float radius);

	public:
		SphereCollision* Clone() const override;
		void Reset()override;
		void Awake()override;
		void OnEnable()override;
		void Start()override;
		void FixedUpdate()override;
		void PreRender()override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;
	};
}


#endif // !_SPHERE_COLLISION_H