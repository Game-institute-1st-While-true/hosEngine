#include "Script.h"

using namespace hos;

hos::com::Script::Script(string_view scriptName)
	:Component(L"Script"), ScriptName(scriptName.data()), EditerIndex(-1)
{
}

hos::com::Script::Script()
	: Component(L"Script"), ScriptName(L"NewScript"), EditerIndex(-1)
{

}

hos::com::Script::~Script()
{
}

hos::com::Script::Script(const Script& dest) :
	Component(dest),
	ScriptName(dest.ScriptName), EditerIndex(dest.EditerIndex)
{
}

string hos::com::Script::GetScriptName()
{
	return ScriptName;
}

void hos::com::Script::SetScriptName(string_view scriptName)
{
	ScriptName = scriptName;
}

void hos::com::Script::SetExecutionOrder(int order)
{
	ExecutionOrder = order;
}

com::Script* hos::com::Script::Clone() const
{
	return new Script(*this);
}

void hos::com::Script::Reset()
{
}

void hos::com::Script::OnEnable()
{
}

void hos::com::Script::OnDisable()
{
}

const std::vector<U8> hos::com::Script::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	mbstring script = ut::UTF16ToAnsi(ScriptName);

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.String("Script", script);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Script::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}
	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);
	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"Script try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	mbstring script = m["Script"].AsString().str();
	ScriptName = ut::AnsiToUTF16(script);

	return true;
}

