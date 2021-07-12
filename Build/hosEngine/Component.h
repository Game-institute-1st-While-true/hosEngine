#ifndef _COMPONENT_H
#define _COMPONENT_H

// [2020/10/14 ������]
// �⺻ ������Ʈ Ŭ����
// ������Ʈ�� �⺻���۰� �ൿ ����

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/26] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�

#include "Object.h"

namespace hos::com
{
	class GameObject;

	class Component : public Object
	{
		friend class Scene;
		friend class GameObject;
	public:
		enum { DEFAULT_EXECUTION_ORDER = 0 };

	protected:
		int ExecutionOrder;

	public:
		GameObject* m_GameObject;

	public:
		Component();
		Component(string_view _name);
		virtual ~Component();

	protected:
		Component(const Component& dest);

	protected:
		friend class NetworkManager;

	public:
		int GetExecutionOrder() const;

	protected:
		virtual Component* Clone() const;
		virtual void Reset();
		virtual void Bind(GameObject* gameObject);
		virtual void UnBind(int index);

		//ICallable
		void Awake() override;
		void Start() override {};
		void FixedUpdate() override {};
		void OnTriggerEnter(Collision* other) override {};
		void OnTriggerStay(Collision* other) override {};
		void OnTriggerExit(Collision* other) override {};
		void OnCollisionEnter(Collision* other) override {};
		void OnCollisionStay(Collision* other) override {};
		void OnCollisionExit(Collision* other) override {};
		void Update() override {};
		void LateUpdate() override {};
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
		void OnEnable() override {};
		void OnDisable() override {};

		virtual const std::vector<U8> Serialize();
		virtual bool Deserialize(mbstring_view data);
	};

	//std::set ���ο��� ���ĵǱ� ���ؼ� ������ �����ε�����
	__forceinline bool operator<(Component& lhs, Component& rhs) noexcept
	{
		return lhs.GetExecutionOrder() < rhs.GetExecutionOrder();
	}

	__forceinline bool operator>(Component& lhs, Component& rhs) noexcept
	{
		return lhs.GetExecutionOrder() > rhs.GetExecutionOrder();
	}
}

#endif // !_COMPONENT_H