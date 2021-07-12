// 참조 헤더
#include "Object.h"
#include "Ang3DEngine\RenderQue.h"

// 자기 헤더
#include "Material.h"

using namespace hos;

hos::Material::Material()
	:MaterialName(L""), MaterialType(hos::Material::MATERIAL_TYPE::MATERIAL_LEGACY_LAMBERT),
	DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f), AmbientColor(0.0f, 0.0f, 0.0f, 1.0f), SpecularColor(1.0f, 1.0f, 1.0f, 1.0f), SpecularPower(0.0f),
	bTexAlbedo(false), bTexNormal(false), bTexARM(false), bTexEmiss(false),
	TexNameAlbedo(L""), TexNameNormal(L""), TexNameARM(L""), TexNameEmiss(L"")
{

}

hos::Material::~Material()
{
}

void hos::Material::CopyMaterialValue(hos::cg::RenderQue::RenderMaterial& material) const
{
	material.DiffColor = this->DiffuseColor;
	material.AmbColor = this->AmbientColor;
	material.SpecColor = this->SpecularColor;
	material.SpecPower = this->SpecularPower;
}

void hos::Material::SetAlbedoTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexAlbedo = true;
		this->TexNameAlbedo = texName;
	}
}

void hos::Material::SetNormalTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexNormal = true;
		this->TexNameNormal = texName;
	}
}

void hos::Material::SetARMTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexARM = true;
		this->TexNameARM = texName;
	}
}

void hos::Material::SetEmissTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexEmiss = true;
		this->TexNameEmiss = texName;
	}
}

void hos::Material::ChangeAlbedoTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexAlbedo = true;
		this->TexNameAlbedo = texName;
	}

	else
	{
		Debug->LogConsole(L"[Material: ChangeAlbedoTex] ", MaterialName + L": 존재하지 않는 Texture입니다. :" + texName);
	}
}

void hos::Material::ChangeNormalTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexNormal = true;
		this->TexNameNormal = texName;
	}

	else
	{
		Debug->LogConsole(L"[Material: ChangeNormalTex] ", MaterialName + L": 존재하지 않는 Texture입니다. :" + texName);
	}
}

void hos::Material::ChangeARMTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexARM = true;
		this->TexNameARM = texName;
	}

	else
	{
		Debug->LogConsole(L"[Material: ChangeARMTex] ", MaterialName + L": 존재하지 않는 Texture입니다. :" + texName);
	}
}

void hos::Material::ChangeEmissTex(hos::string texName)
{
	if (g_DataManager->HasTexture(texName))
	{
		this->bTexEmiss = true;
		this->TexNameEmiss = texName;
	}

	else
	{
		Debug->LogConsole(L"[Material: ChangeEmissTex] ", MaterialName + L": 존재하지 않는 Texture입니다. :" + texName);
	}
}

void hos::Material::RemoveAlbedoTex()
{
	this->bTexAlbedo = false;
}

void hos::Material::RemoveNormalTex()
{
	this->bTexNormal = false;
}

void hos::Material::RemoveARMTex()
{
	this->bTexARM = false;
}

void hos::Material::RemoveEmissTex()
{
	this->bTexEmiss = false;
}

const std::vector<U8> hos::Material::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(MaterialName);
	mbstring albedoName("NULL");
	mbstring normalName("NULL");
	mbstring ARMName("NULL");
	mbstring emissName("NULL");
	if (bTexAlbedo)
	{
		albedoName = ut::UTF16ToAnsi(TexNameAlbedo);
	}
	if (bTexNormal)
	{
		normalName = ut::UTF16ToAnsi(TexNameNormal);
	}
	if (bTexARM)
	{
		ARMName = ut::UTF16ToAnsi(TexNameARM);
	}
	if (bTexEmiss)
	{
		emissName = ut::UTF16ToAnsi(TexNameEmiss);
	}

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("MaterialName", name);
		builder.UInt("MaterialType", static_cast<U32>(MaterialType));
		builder.TypedVector("DiffuseColor", [&] {
			builder.Float(DiffuseColor.x);
			builder.Float(DiffuseColor.y);
			builder.Float(DiffuseColor.z);
			builder.Float(DiffuseColor.w);
			});
		builder.TypedVector("AmbientColor", [&] {
			builder.Float(AmbientColor.x);
			builder.Float(AmbientColor.y);
			builder.Float(AmbientColor.z);
			builder.Float(AmbientColor.w);
			});
		builder.TypedVector("SpecularColor", [&] {
			builder.Float(SpecularColor.x);
			builder.Float(SpecularColor.y);
			builder.Float(SpecularColor.z);
			builder.Float(SpecularColor.w);
			});
		builder.Float("SpecularPower", SpecularPower);
		builder.Bool("bTexAlbedo", bTexAlbedo);
		builder.Bool("bTexNormal", bTexNormal);
		builder.Bool("bTexARM", bTexARM);
		builder.Bool("bTexEmiss", bTexEmiss);
		builder.String("TexNameAlbedo", albedoName);
		builder.String("TexNameNormal", normalName);
		builder.String("TexNameARM", ARMName);
		builder.String("TexNameEmiss", emissName);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::Material::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring name = m["MaterialName"].AsString().str();
	MaterialName = ut::AnsiToUTF16(name);
	MaterialType = static_cast<MATERIAL_TYPE>(m["MaterialType"].AsUInt32());
	auto diffuse = m["DiffuseColor"].AsVector();
	DiffuseColor.x = diffuse[0].AsFloat();
	DiffuseColor.y = diffuse[1].AsFloat();
	DiffuseColor.z = diffuse[2].AsFloat();
	DiffuseColor.w = diffuse[3].AsFloat();
	auto Ambient = m["AmbientColor"].AsVector();
	AmbientColor.x = Ambient[0].AsFloat();
	AmbientColor.y = Ambient[1].AsFloat();
	AmbientColor.z = Ambient[2].AsFloat();
	AmbientColor.w = Ambient[3].AsFloat();
	auto Specular = m["SpecularColor"].AsVector();
	SpecularColor.x = Specular[0].AsFloat();
	SpecularColor.y = Specular[1].AsFloat();
	SpecularColor.z = Specular[2].AsFloat();
	SpecularColor.w = Specular[3].AsFloat();

	SpecularPower = m["SpecularPower"].AsFloat();

	bTexAlbedo = m["bTexAlbedo"].AsBool();
	bTexNormal = m["bTexNormal"].AsBool();
	bTexARM = m["bTexARM"].AsBool();
	bTexEmiss = m["bTexEmiss"].AsBool();

	mbstring texname = m["TexNameAlbedo"].AsString().str();
	TexNameAlbedo = ut::AnsiToUTF16(texname);
	texname = m["TexNameNormal"].AsString().str();
	TexNameNormal = ut::AnsiToUTF16(texname);
	texname = m["TexNameARM"].AsString().str();
	TexNameARM = ut::AnsiToUTF16(texname);
	texname = m["TexNameEmiss"].AsString().str();
	TexNameEmiss = ut::AnsiToUTF16(texname);

	if (bTexAlbedo)
	{
		g_DataManager->LoadTexture(TexNameAlbedo);
	}
	if (bTexNormal)
	{
		g_DataManager->LoadTexture(TexNameNormal);
	}
	if (bTexARM)
	{
		g_DataManager->LoadTexture(TexNameARM);
	}
	if (bTexEmiss)
	{
		g_DataManager->LoadTexture(TexNameEmiss);
	}

	return true;
}
