#include "GameObject.h"
#include "Light.h"
#include "Transform.h"

using namespace hos;

hos::com::Light::Light(eType type)
	: Component(L"Light"), SolidColor(Color(1.f, 1.f, 1.f)), Range(10), Angle(30), Intensity(1), Type(type),
	Attenuation_Const(1.0f), Attenuation_Linear(0.09f), Attenuation_Quad(0.017f)
{
}

hos::com::Light::~Light()
{
}

hos::com::Light::Light(const Light& dest) :
	Component(dest),
	SolidColor(dest.SolidColor),
	Range(dest.Range),
	Angle(dest.Angle),
	Intensity(dest.Intensity),
	Type(dest.Type),
	Attenuation_Const(dest.Attenuation_Const),
	Attenuation_Linear(Attenuation_Linear),
	Attenuation_Quad(Attenuation_Quad)
{
}

Color hos::com::Light::GetColor() const
{
	return SolidColor;
}

void hos::com::Light::SetColor(Color color)
{
	SolidColor = color;
}

float hos::com::Light::GetRange() const
{
	return Range;
}

void hos::com::Light::SetRange(float range)
{
	Range = range;
}

float hos::com::Light::GetAngle() const
{
	return Angle;
}

void hos::com::Light::SetAngle(float angle)
{
	Angle = angle;
}

float hos::com::Light::GetIntensity() const
{
	return Intensity;
}

void hos::com::Light::SetIntensity(float intensity)
{
	Intensity = Min(Max(intensity, 0.f), 1.f);
}

void hos::com::Light::ChangeType(eType type)
{
	if (Type == type)
	{
		return;
	}

	Type = type;

	//TODO : 변경된 타입에 따른 설정 필요
	switch (Type)
	{
	case hos::com::Light::eType::DIRECTIONAL:
		break;
	case hos::com::Light::eType::POINT:
		break;
	case hos::com::Light::eType::SPOT:
		break;
	}
}

com::Light* hos::com::Light::Clone() const
{
	return new Light(*this);
}

void hos::com::Light::Reset()
{
}

void hos::com::Light::OnEnable()
{
}

void hos::com::Light::OnDisable()
{
}

void hos::com::Light::Update()
{
	if (g_Renderer)
	{
		Vector4 color = SolidColor.ToVector4() * Intensity;

		if (this->Type == eType::DIRECTIONAL)
		{
			Vector3 forward = m_GameObject->transform->GetForward();
			Vector4 dir(-forward.x, -forward.y, -forward.z, 0);
			g_Renderer->SetDirLight(dir, color);
		}

		else if (this->Type == eType::POINT)
		{
			// 필수 Component Check
			Transform* _transform = m_GameObject->transform;

			if (_transform == nullptr)
			{
				Debug->LogConsole(L"Light", m_GameObject->GetName() + L"Transform 컴포넌트가 없으면 PointLight 적용이 불가능합니다.");
				return;
			}

			Vector3 _tempPos = _transform->GetPosition();
			Vector4 _pos;
			_pos.x = _tempPos.x;
			_pos.y = _tempPos.y;
			_pos.z = _tempPos.z;
			_pos.w = 0.0f;

			Vector4 _attenuation = Vector4(this->Attenuation_Const, this->Attenuation_Linear, this->Attenuation_Quad, 0.0f);
			
			g_Renderer->SetPointLight(_pos, color, _attenuation);
		}

		else if (this->Type == eType::AMBIENT)
		{
			g_Renderer->SetAmbientLight(color); 
			
		}
	}
}

const std::vector<U8> hos::com::Light::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.TypedVector("SolidColor", [&] {
			builder.Float(SolidColor.x);
			builder.Float(SolidColor.y);
			builder.Float(SolidColor.z);
			builder.Float(SolidColor.w);
			});
		builder.Float("Range", Range);
		builder.Float("Angle", Angle);
		builder.Float("Intensity", Intensity);
		builder.UInt("Type", static_cast<U32>(Type));
		builder.Float("AttenuConst", Attenuation_Const);
		builder.Float("AttenuLinear", Attenuation_Linear);
		builder.Float("AttenuQuad", Attenuation_Quad);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Light::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"Light try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	auto color = m["SolidColor"].AsTypedVector();
	SolidColor.x = color[0].AsFloat();
	SolidColor.y = color[1].AsFloat();
	SolidColor.z = color[2].AsFloat();
	SolidColor.w = color[3].AsFloat();

	Range = m["Range"].AsFloat();
	Angle = m["Angle"].AsFloat();
	Intensity = m["Intensity"].AsFloat();

	Type = static_cast<eType>(m["Type"].AsUInt32());

	Attenuation_Const = m["AttenuConst"].AsFloat();
	Attenuation_Linear = m["AttenuLinear"].AsFloat();
	Attenuation_Quad = m["AttenuQuad"].AsFloat();

	return true;
}
