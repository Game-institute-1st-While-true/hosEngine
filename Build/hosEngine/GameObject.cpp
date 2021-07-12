#include "Networkcomponent.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Camera.h"
#include "BoxCollision.h"
#include "CapsuleCollision.h"
#include "SphereCollision.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "Rigidbody.h"
#include "Script.h"
#include "Light.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "UIImage.h"
#include "UI_Text.h"
#include "UIButton.h"
#include "UI_InputField.h"
#include "Animation.h"
#include "NavAgent.h"
#include "Scene.h"

using namespace hos;

hos::com::GameObject::GameObject() : Object(), m_Scene(nullptr),
transform(nullptr), rigidbody(nullptr),
meshrenderer(nullptr), skinnedmeshrenderer(nullptr), meshfilter(nullptr),
listener(nullptr), network(nullptr),
IsActiveRecursive(true)
{
	Transform* trans = new Transform();
	AddComponent(trans);
}

hos::com::GameObject::GameObject(string_view name) : Object(name), m_Scene(nullptr),
transform(nullptr), rigidbody(nullptr),
meshrenderer(nullptr), skinnedmeshrenderer(nullptr), meshfilter(nullptr),
listener(nullptr), network(nullptr),
IsActiveRecursive(true)
{
	Transform* trans = new Transform();
	this->boneTag = name;
	AddComponent(trans);
}

hos::com::GameObject::GameObject(string_view name, std::initializer_list<Component*> list) : GameObject(name)
{
	for (Component* com : list)
	{
		AddComponent(com);
	}
	this->boneTag = name;
}

hos::com::GameObject::GameObject(const GameObject& dest) :
	Object(dest), m_Scene(nullptr),
	transform(nullptr), rigidbody(nullptr),
	meshrenderer(nullptr), skinnedmeshrenderer(nullptr), meshfilter(nullptr),
	listener(nullptr), network(nullptr),
	IsActiveRecursive(dest.IsActiveRecursive),
	PhysicsActor(nullptr),boneTag(dest.boneTag),Tag(dest.Tag)
{
	for (Component* com : dest.Components)
	{
		AddComponent(com->Clone());
	}

	for (Transform* child : dest.transform->GetChilds())
	{
		GameObject* childObj = new GameObject(*(child->m_GameObject));
		transform->AttachChild(childObj->transform);
	}
}

hos::com::GameObject::~GameObject()
{
	for (Component* com : Components)
	{
		if (m_Scene)
		{
			for (auto it = m_Scene->ScriptExecution.begin(); it != m_Scene->ScriptExecution.end();)
			{
				if (it->second == com)
				{
					m_Scene->ScriptExecution.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}
		}
		delete com;
		com = nullptr;
	}
	SafeRelease(PhysicsActor);
}

hos::com::Component* hos::com::GameObject::AddComponent(Component* component)
{
	if (component)
	{
		try
		{
			component->Bind(this);
		}
		catch (std::invalid_argument e)
		{
			return nullptr;
		}
		Components.push_back(component);

		if (m_Scene)
		{
			m_Scene->ScriptExecution.insert({ component->ExecutionOrder, component });
		}

		return component;
	}
	return nullptr;
}

void hos::com::GameObject::RemoveComponent(Component* component)
{
	int index = 0;
	for (auto it = Components.begin(); it != Components.end(); ++it, index++)
	{
		if (component == (*it))
		{
			if (m_Scene)
			{
				for (auto it = m_Scene->ScriptExecution.begin(); it != m_Scene->ScriptExecution.end();)
				{
					if (it->second == component)
					{
						m_Scene->ScriptExecution.erase(it);
						break;
					}
					else
					{
						++it;
					}
				}
			}

			component->UnBind(index);
			Components.erase(it);
			return;
		}
	}
}

U32 hos::com::GameObject::FindComponentIndex(Component* component)
{
	for (int i = 0; i < Components.size(); i++)
	{
		if (Components[i] == component)
		{
			return i;
		}
	}
	return -1;
}

com::GameObject* hos::com::GameObject::GetRoot() const
{
	if (transform->GetRoot())
	{
		return transform->GetRoot()->m_GameObject;
	}
	return nullptr;
}

com::GameObject* hos::com::GameObject::GetParent() const
{
	if (transform->GetParent())
	{
		return transform->GetParent()->m_GameObject;
	}
	return nullptr;
}

com::GameObject* hos::com::GameObject::FindObject(string_view name) const
{
	if (GetName() == name.data())
	{
		return const_cast<GameObject*>(this);
	}
	else
	{
		for (Transform* child : transform->GetChilds())
		{
			if (child->m_GameObject)
			{
				GameObject* find = child->m_GameObject->FindObject(name);
				if (find)
				{
					return find;
				}
			}
		}
	}
	return nullptr;
}

com::GameObject* hos::com::GameObject::FineObjectByTag(string_view tag) const
{
	if (Tag == tag.data())
	{
		return const_cast<GameObject*>(this);
	}
	else
	{
		for (Transform* child : transform->GetChilds())
		{
			if (child->m_GameObject)
			{
				GameObject* find = child->m_GameObject->FineObjectByTag(tag);
				if (find)
				{
					return find;
				}
			}
		}
	}
	return nullptr;
}

com::GameObject* hos::com::GameObject::FindObjectByBoneTag(string_view name) const
{
	if (boneTag == name.data())
	{
		return const_cast<GameObject*>(this);
	}
	else
	{
		for (Transform* child : transform->GetChilds())
		{
			if (child->m_GameObject)
			{
				GameObject* find = child->m_GameObject->FindObjectByBoneTag(name);
				if (find)
				{
					return find;
				}
			}
		}
	}
	return nullptr;
}

void hos::com::GameObject::Bind(Scene* scene)
{
	if (m_Scene)
	{
		throw std::invalid_argument("This GameObject is Bound");
	}

	if (!scene)
	{
		throw std::invalid_argument("Valid Scene try to bind GameObject");
	}

	m_Scene = scene;

	for (Component* com : Components)
	{
		m_Scene->ScriptExecution.insert({ com->ExecutionOrder,com });
	}
}

void hos::com::GameObject::UnBind(int index)
{
	if (m_Scene)
	{
		for (auto it = m_Scene->ScriptExecution.begin(); it != m_Scene->ScriptExecution.end();)
		{
			for (Component* com : Components)
			{
				if (it->second == com)
				{
					m_Scene->ScriptExecution.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		m_Scene = nullptr;
	}
}

void hos::com::GameObject::SetActiveRecursive(bool b)
{
	bool prevActive = GetActive();
	if (transform->GetParent())
	{
		IsActiveRecursive = b && transform->GetParent()->m_GameObject->GetActive();
	}
	else
	{
		IsActiveRecursive = b;
	}
	if (ActiveEventDuplicatedCheck)
	{
		if (prevActive != GetActive())
		{
			if (GetActive())
			{
				Debug->LogConsole(L"Debug", GetName() + L"GameObject OnEnable");
				OnEnable();
			}
			else
			{
				Debug->LogConsole(L"Debug", GetName() + L"GameObject OnDisable");
				OnDisable();
			}
		}
	}
	for (Transform* child : transform->GetChilds())
	{
		//부모->자식1->자식2에서 부모와 자식2가 true이고 자식1이 false일때 자식2의 상태를 올바르게 전달하기위해서
		//GetActiveHierarchy를 자식들에게 전달한다.
		child->m_GameObject->SetActiveRecursive(GetActive());
	}
}

bool hos::com::GameObject::GetActive() const
{
	return IsActiveRecursive && Object::GetActive();
}

void hos::com::GameObject::SetActive(bool b)
{
	if (b != Object::GetActive())
	{
		Debug->LogConsole(L"Start", L"----------");
		Object::SetActive(b);
		ActiveEventDuplicatedCheck = false;
		SetActiveRecursive(b);
		ActiveEventDuplicatedCheck = true;
		Debug->LogConsole(L"End", L"===========");
	}
}

void hos::com::GameObject::OnTriggerEnter(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnTriggerEnter(other);
		}
	}
}

void hos::com::GameObject::OnTriggerStay(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnTriggerStay(other);
		}
	}
}

void hos::com::GameObject::OnTriggerExit(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnTriggerExit(other);
		}
	}
}

void hos::com::GameObject::OnCollisionEnter(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnCollisionEnter(other);
		}
	}
}

void hos::com::GameObject::OnCollisionStay(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnCollisionStay(other);
		}
	}
}

void hos::com::GameObject::OnCollisionExit(Collision* other)
{
	for (Component* com : Components)
	{
		if (com->GetActive())
		{
			com->OnCollisionExit(other);
		}
	}
}

void hos::com::GameObject::OnEnable()
{
	for (Component* com : Components)
	{
		Debug->LogConsole(L"Debug", com->GetName() + L" Enable");
		com->OnEnable();
	}
}

void hos::com::GameObject::OnDisable()
{
	for (Component* com : Components)
	{
		Debug->LogConsole(L"Debug", com->GetName() + L" Disable");
		com->OnDisable();
	}
}

void hos::com::GameObject::SetName(string_view _name)
{
	string _ChangeName = _name.data();
	if (m_Scene)
	{
		_ChangeName = m_Scene->ObjectNameDuplicateCheck(_name);
	}

	Object::SetName(_ChangeName);
}

string hos::com::GameObject::GetTag() const
{
	return Tag;
}

void hos::com::GameObject::SetTag(string_view tag)
{
	Tag = tag;
}

U32 hos::com::GameObject::GetLayer() const
{
	if (PhysicsActor)
	{
		physx::PxActorType::Enum aType = PhysicsActor->getType();
		switch (aType)
		{
		case physx::PxActorType::eRIGID_DYNAMIC:
		case physx::PxActorType::eRIGID_STATIC:
		case physx::PxActorType::eARTICULATION_LINK:
		{
			physx::PxShape* shape = nullptr;
			PhysicsActor->getShapes(&shape, 1);
			physx::PxFilterData fd = shape->getSimulationFilterData();
			Layer = fd.word0;
			return Layer;
		}
		break;

		case physx::PxActorType::eACTOR_COUNT:
		case physx::PxActorType::eACTOR_FORCE_DWORD:
			break;
		}
	}

	return 0;
}

void hos::com::GameObject::SetLayer(U32 index)
{
	if (index < Scene::MAX_LAYER)
	{
		if (PhysicsActor)
		{
			Layer = index;
			physx::PxActorType::Enum aType = PhysicsActor->getType();
			switch (aType)
			{
			case physx::PxActorType::eRIGID_DYNAMIC:
			case physx::PxActorType::eRIGID_STATIC:
			case physx::PxActorType::eARTICULATION_LINK:
			{
				physx::PxShape* shape;
				for (U32 i = 0; i < PhysicsActor->getNbShapes(); i++)
				{
					PhysicsActor->getShapes(&shape, 1, i);
					physx::PxFilterData fd(index, 0, 0, 0);
					shape->setSimulationFilterData(fd);
				}
			}
			break;
			case physx::PxActorType::eACTOR_COUNT:
			case physx::PxActorType::eACTOR_FORCE_DWORD:
				break;
			}
		}
	}
}

std::vector<hos::com::Component*>& hos::com::GameObject::GetComponents()
{
	return Components;
}

const std::vector<U8> hos::com::GameObject::Serialize()
{
	// 자신의 컴포넌트 Serialization
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring bonetag = ut::UTF16ToAnsi(boneTag);
	mbstring tag = ut::UTF16ToAnsi(Tag);
	flexbuffers::Builder builder;

	std::vector<uint8_t> temp;

	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", Object::GetActive());
		builder.String("Tag", tag);
		builder.String("BoneTag", bonetag);
		builder.UInt("Layer", Layer);
		// 컴포넌트의 갯수만큼 들어가야 함
		builder.Vector("Components", [&] {

			for (int i = 0; i < Components.size(); i++)
			{
				temp = Components[i]->Serialize();

				builder.Blob(temp);
			}

			});
		// 자식 게임 오브젝트 시리얼라이제이션
		builder.Vector("ChildGameObject", [&] {

			for (int i = 0; i < transform->GetChildCount(); i++)
			{
				std::vector<uint8_t> temp2 = transform->GetChild(i)->m_GameObject->Serialize();
				builder.Blob(temp2);
			}

			});
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::GameObject::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);

	SetName(name);

	bool active = m["IsActive"].AsBool();

	mbstring tag = m["Tag"].AsString().str();
	Tag = ut::AnsiToUTF16(tag);

	mbstring bonetag = m["BoneTag"].AsString().str();
	boneTag = ut::AnsiToUTF16(bonetag);

	Layer = m["Layer"].AsUInt32();

	auto temp = m["Components"].AsVector();

	for (int i = 0; i < temp.size(); i++)
	{
		if (!temp[i].AsBlob().IsTheEmptyBlob())
		{
			// 해당하는 컴포넌트를 찾아야 함
			auto tt = temp[i].AsBlob();
			auto ttt = flexbuffers::GetRoot(tt.data(), tt.size());
			auto tm = ttt.AsMap();
			mbstring cname = tm["Name"].AsString().str();

			// 컴포넌트에 맞춰서 생성하자
			Component* _Component = nullptr;
			mbstring _ComponentData(reinterpret_cast<const char*>(temp[i].AsBlob().data()), temp[i].AsBlob().size());
			bool b = false;
			if (cname.compare("Transform") == 0)
			{
				// 이미 트랜스폼이 있음, 없을 수가 없음
				b = transform->Deserialize(_ComponentData);
				continue;
			}
			else if (cname.compare("Camera") == 0)
			{
				_Component = new Camera(Camera::eMode::DEFAULT);

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("BoxCollision") == 0)
			{
				_Component = new BoxCollision();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("SphereCollision") == 0)
			{
				_Component = new SphereCollision();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("CapsuleCollision") == 0)
			{
				_Component = new CapsuleCollision();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("Rigidbody") == 0)
			{
				_Component = new Rigidbody();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("AudioListener") == 0)
			{
				_Component = new AudioListener();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("AudioSource") == 0)
			{
				_Component = new AudioSource();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("Script") == 0)
			{
				_Component = new Script(L"NewScript");

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("MeshFilter") == 0)
			{
				_Component = new MeshFilter();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("MeshRenderer") == 0)
			{
				_Component = new MeshRenderer();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("SkinnedMeshRenderer") == 0)
			{
				_Component = new SkinnedMeshRenderer();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("Animation") == 0)
			{
				_Component = new Animation();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("Light") == 0)
			{
				_Component = new Light();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("NavAgent") == 0)
			{
				_Component = new NavAgent();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("Networkcomponent") == 0)
			{
				_Component = new Networkcomponent(Network);

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("UIImage") == 0)
			{
				_Component = new UIImage();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("UIText") == 0)
			{
				_Component = new UIText();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("UIButton") == 0)
			{
				_Component = new UIButton();

				b = _Component->Deserialize(_ComponentData);
			}
			else if (cname.compare("UIInputField") == 0)
			{
				_Component = new UIInputField();

				b = _Component->Deserialize(_ComponentData);
			}
			if (b)
			{
				AddComponent(_Component);
			}
			else
			{
				SafeDelete(_Component);
			}
		}
	}

	// 자식 만들기
	auto _Child = m["ChildGameObject"].AsVector();

	for (int i = 0; i < _Child.size(); i++)
	{
		if (!_Child[i].AsBlob().IsTheEmptyBlob())
		{
			GameObject* _ChildGameObject = new GameObject();
			mbstring _ChildData(reinterpret_cast<const char*>(_Child[i].AsBlob().data()), _Child[i].AsBlob().size());
			bool b = _ChildGameObject->Deserialize(_ChildData);
			if (b)
			{
				transform->AttachChild(_ChildGameObject->transform);
			}
			else
			{
				SafeDelete(_ChildGameObject);
			}
		}
	}

	//자식들이 전부 추가된 뒤에 SetActive를 해야 자식들에게 올바르게 Active상태를 전달한다.
	SetActive(active);

	return true;

}