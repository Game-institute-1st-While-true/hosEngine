#include "ScriptFactory.h"

//아래에 유저스크립트 추가
#include "UserScriptList.h"

using namespace hos;

constexpr string_literal Script[] = {
	//user Scripts Names
	//for sample
	L"SampleScript"
};

//Script Name Helper
constexpr U32 SNH(string_view name)
{
	for (U32 i = 0; i < sizeof(Script); i++)
	{
		if (name.compare(Script[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

hos::ScriptFactory::ScriptFactory()
{
}

hos::ScriptFactory::~ScriptFactory()
{
}

com::Script* hos::ScriptFactory::Create(string_view name)
{
	switch (SNH(name))
	{
	//for sample
	case SNH(L"SampleScript"):
		return new SampleScript();
		break;
	}
	return nullptr;
}

void hos::ScriptFactory::BindObject(com::GameObject* object)
{
	std::vector<com::Script*> RemoveScriptComponent;

	com::Script* script;
	for (int i = 0; i < object->Components.size(); i++)
	{
		script = dynamic_cast<com::Script*>(object->Components[i]);
		if (script)
		{
			RemoveScriptComponent.push_back(script);
		}
	}

	int _ScriptCount = (int)RemoveScriptComponent.size();

	for (int i = 0; i < _ScriptCount; i++)
	{
		com::Script* newScript = Create(RemoveScriptComponent[i]->GetScriptName());
		object->AddComponent(newScript);
	}

	for (com::Script* _iter : RemoveScriptComponent)
	{
		object->RemoveComponent(_iter);
		SafeDelete(_iter);
	}
}

void hos::ScriptFactory::BindObject(com::Scene* Scene)
{
	std::vector<com::GameObject*> objs = Scene->GetObjects();

	for (com::GameObject* obj : objs)
	{
		BindObject(obj);
	}
}