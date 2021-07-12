#ifndef _TRANSFORM_H
#define _TRANSFORM_H

// [2020/10/14 ������]
// ���� ������Ʈ�� ��ġ�� ��Ÿ���� ������Ʈ
// ��ġ, �̵�, ȸ��, ���̾��Ű ���� �θ� �ڽİ������ ������������ ���ӿ�����Ʈ�� �ʼ������� ���Եȴ�.

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/26] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�
#include "Component.h"

namespace hos::com
{
	class Transform : public Component
	{
	private:
		//�ֻ��� �θ� (������ �ֻ����ϰ�� this)
		Transform* Root;
		//�θ�
		Transform* Parent;
		//�ڽĵ�
		std::vector<Transform*> Childs;

		//�������
		Vector3 Forward;
		Vector3 Up;
		Vector3 Left;

		//���� ��ġ (�θ� ���°�� Local)
		Vector3 Position;
		Rotator Rotation;
		Vector3 Scale;

		//������ġ
		Vector3 LocalPosition;
		Rotator LocalRotation;
		Vector3 LocalScale;

		//Matrix
		Matrix TransformMatrix;
		Matrix LocalTransformMatrix;
	public:
		Transform();
		~Transform();
		
	protected:
		//Only Used GameObject Copy
		Transform(const Transform& dest);

	private:
		void UpdateRoot(Transform* root);
		void UpdateTransformMatrix();

	public:
		void SetActive(bool b) = delete;

		Transform* GetRoot() const;
		Transform* GetParent() const;
		void SetParent(Transform* transform);
		void AttachChild(Transform* child);
		void DetachChild(int index, Transform* newParent = nullptr);
		void DetachChild(Transform* child, Transform* newParent = nullptr);
		Transform* GetChild(int index) const;
		Transform* Find(string_view name) const;
		std::size_t GetChildCount() const;
		std::vector<Transform*>& GetChilds();

		void SetForward(Vector3 forward);
		void SetUp(Vector3 up);
		void SetLeft(Vector3 right);
		void SetPosition(Vector3 position);
		void SetRotation(Rotator rotation);
		void SetScale(Vector3 scale);
		void SetLocalPosition(Vector3 position);
		void SetLocalRotation(Rotator rotation);
		void SetLocalScale(Vector3 scale);
		void SetTransform(Vector3 position, Rotator rotation, Vector3 scale);
		void SetLocalTransform(Vector3 position, Rotator rotation, Vector3 scale);

		Vector3 GetForward() const;
		Vector3 GetUp() const;
		Vector3 GetLeft() const;
		Vector3 GetPosition() const;
		Rotator GetRotation() const;
		Vector3 GetScale() const;
		Vector3 GetLocalPosition() const;
		Rotator GetLocalRotation() const;
		Vector3 GetLocalScale() const;

		Matrix GetLocalTransformMatrix() const;
		Matrix GetTransformMatrix() const;

		void Translate(Vector3 translation);
		void Rotate(Rotator rotate);

	public:
		void Bind(GameObject* gameObject) override;
		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view data) override;

	public:
		Transform* Clone() const override;
		void Reset() override;
		//ICallable
		void Awake() override {};
		void Start() override {};
		void FixedUpdate() override {};
		void OnTriggerEnter(Collision* ohter) override {};
		void OnTriggerStay(Collision* ohter) override {};
		void OnTriggerExit(Collision* ohter) override {};
		void OnCollisionEnter(Collision* ohter) override {};
		void OnCollisionStay(Collision* ohter) override {};
		void OnCollisionExit(Collision* ohter) override {};
		void Update() override {};
		void LateUpdate() override;
		void OnStateMachine() override {};
		void OnAnimation() override {};
		void PreRender() override {};
		void Render() override {};
		void PostRender() override {};
		void RenderImage() override {};
		void OnGUI(float aspect_x, float aspect_y) override {};
		void OnPause() override {};
		void OnClose() override {};
		void OnQuit() override {};
		void OnDestory() override {};
		void OnEnable() override;
		void OnDisable() override;
	};
}

#endif // !_TRANSFORM