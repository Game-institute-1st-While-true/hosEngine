#ifndef _BOX_COLLISION_H
#define _BOX_COLLISION_H

// [2020/12/10 ������]
// �ڽ� �ݸ���
// ������Ʈ�� �浹ó���� ���� ������Ʈ

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2020/01/20] ������ : ���� Ŭ������ ���� ��ӹ޵��� ����
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�, Awake ���׼���

#include "Collision.h"

namespace hos::com
{
	class BoxCollision : public Collision
	{
	public:
		static constexpr float DEFAULT_SIZE = 1.0f;

	private:
		Vector3 Size;

	public:
		BoxCollision(float x = DEFAULT_SIZE, float y = DEFAULT_SIZE, float z = DEFAULT_SIZE, PhysicMaterial* material = nullptr);
		virtual ~BoxCollision();

	protected:
		BoxCollision(const BoxCollision& dest);

	public:
		Vector3 GetSize()const;
		float GetSizeX()const;
		float GetSizeY()const;
		float GetSizeZ()const;

		void SetSizeX(float x);
		void SetSizeY(float y);
		void SetSizeZ(float z);
		void SetSize(Vector3 size);

	public:
		BoxCollision* Clone() const override;
		void Reset()override;
		void Awake()override;
		void OnEnable()override;
		void FixedUpdate()override;

		void PreRender()override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;
	};
}

#endif // !_BOX_COLLISION_H