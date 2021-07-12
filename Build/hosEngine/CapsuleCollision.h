#ifndef _CAPSULE_COLLISION_H
#define _CAPSULE_COLLISION_H

// [2020/1/11 ������]
// ĸ�� �ݸ���
// ������Ʈ�� �浹ó���� ���� ������Ʈ

// LOG
// [2020/01/20] ������ : ���� Ŭ������ ���� ��ӹ޵��� ����
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�

#include "Collision.h"

namespace hos::com
{
	class CapsuleCollision : public Collision
	{
	public:
		enum eDirection
		{
			DIRECTION_X,
			DIRECTION_Y,
			DIRECTION_Z,

			NUM_DIRECTION
		};

	public:
		static constexpr float DEFAULT_RADIUS = 0.5f;
		static constexpr float DEFAULT_HEIGHT = 1.f;
		static constexpr eDirection DEFAULT_DIRECTION = DIRECTION_Y;

	private:
		float Radius;
		float HalfHeight;
		float Height;
		eDirection Direction;

	public:
		CapsuleCollision(float radius = DEFAULT_RADIUS, float height = DEFAULT_HEIGHT, eDirection dir = DIRECTION_Y, PhysicMaterial* material = nullptr);
		virtual ~CapsuleCollision();

	protected:
		CapsuleCollision(const CapsuleCollision& dest);

	public:
		float GetRadius()const;
		float GetHeight()const;
		eDirection GetDirection()const;

		void SetCenter(Vector3 center)override;
		void SetRadius(float radius);
		void SetHeight(float height);
		void SetDirection(eDirection dir);

	public:
		CapsuleCollision* Clone() const override;
		void Reset()override;
		void Awake()override;
		void OnEnable()override;
		void FixedUpdate()override;
		void PreRender()override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;
	};
}


#endif // !_CAPSULE_COLLISION_H