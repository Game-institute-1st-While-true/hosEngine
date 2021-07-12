#include "Object.h"
#include "FontData.h"
#include <fstream>

using namespace hos;

hos::FontData::FontData() :
	Name(string())
{
}

hos::FontData::~FontData()
{
}

string hos::FontData::GetName() const
{
	return Name;
}

void hos::FontData::SetName(string_view name)
{
	Name = name;
}

const std::vector<U8> hos::FontData::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(Name);

	flexbuffers::Builder builder;
	builder.Vector([&] {
		builder.String(Blob);
		builder.String(name);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::FontData::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsVector();
	Blob = m[0].AsString().str();

	mbstring pathName = m[1].AsString().str();
	Name = ut::AnsiToUTF16(pathName);

	if (g_Renderer)
	{
		g_Renderer->CreateSpriteFont(Name, reinterpret_cast<uint8_t*>(Blob.data()), Blob.size());
	}

	return true;
}