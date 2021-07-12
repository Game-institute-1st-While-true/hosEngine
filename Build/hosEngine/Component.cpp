#include "Component.h"
#include "GameObject.h"

using namespace hos;

hos::com::Component::Component() :Object(), m_GameObject(nullptr), ExecutionOrder(DEFAULT_EXECUTION_ORDER)
{
	mbstring tt = typeid(*this).name();

	string str(tt.begin(), tt.end());
	SetName(str);
}

hos::com::Component::Component(const Component& dest) :
	Object(dest), m_GameObject(nullptr), ExecutionOrder(dest.ExecutionOrder)
{

}

hos::com::Component::Component(string_view _name) : Object(_name), m_GameObject(nullptr), ExecutionOrder(DEFAULT_EXECUTION_ORDER)
{
}

hos::com::Component::~Component()
{
}

int hos::com::Component::GetExecutionOrder() const
{
	return ExecutionOrder;
}

com::Component* hos::com::Component::Clone() const
{
	return new Component(*this);
}

void hos::com::Component::Reset()
{
}

void hos::com::Component::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	m_GameObject = gameObject;
}

void hos::com::Component::UnBind(int index)
{
	OnDisable();
	if (m_GameObject)
	{
		m_GameObject = nullptr;
	}
}

void hos::com::Component::Awake()
{
	if (m_GameObject->GetActive() && GetActive())
	{
		OnEnable();
	}
}

const std::vector<U8> hos::com::Component::Serialize()
{
	return std::vector<U8>();
}

bool hos::com::Component::Deserialize(mbstring_view data)
{
	return false;
}
