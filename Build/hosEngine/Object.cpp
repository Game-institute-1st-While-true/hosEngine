#include "Object.h"
#include "GameObject.h"
#include "Transform.h"

using namespace hos;

hos::string g_Path = hos::string();
hos::ut::Logger* Debug = hos::ut::Logger::GetIns();
hos::ut::GameTimer* Time = nullptr;
hos::cg::Renderer* g_Renderer = nullptr;
hos::com::DataManager* g_DataManager = nullptr;
DirectX::AudioEngine* g_Audio = nullptr;
hos::PhysX* g_Px = nullptr;
hos::nv::Navigation* g_Navigation = nullptr;
hos::com::NetworkManager* Network = nullptr;

hos::com::Object::Object() : Object(L"EmptyObject")
{
}

hos::com::Object::Object(string_view _name, bool active) : Name(_name.data()), IsActive(active)
{
}

hos::com::Object::~Object()
{
}

string hos::com::Object::GetName() const
{
	return Name;
}

void hos::com::Object::SetName(string_view _name)
{
	Name = _name.data();
}

bool hos::com::Object::GetActive() const
{
	return IsActive;
}

void hos::com::Object::SetActive(bool active)
{
	if (IsActive == active)
	{
		return;
	}
	if (active)
	{
		Debug->LogConsole(L"Debug", Name + L"Object OnEnable");
		OnEnable();
	}
	else
	{
		Debug->LogConsole(L"Debug", Name + L"Object OnDisable");
		OnDisable();
	}
	IsActive = active;
}

string hos::com::Object::ToString() const
{
	return GetName();
}

unsigned long long hos::com::Object::GetHashCode() const
{
	return (reinterpret_cast<long long>(this));
}

const std::vector<U8> hos::com::Object::Serialize()
{
	return std::vector<U8>();
}

bool hos::com::Object::Deserialize(mbstring_view data)
{
	return false;
}
