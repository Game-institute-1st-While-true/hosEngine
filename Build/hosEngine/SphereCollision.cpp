#include "SphereCollision.h"
#include "Transform.h"
#include "Rigidbody.h"

using namespace hos;

hos::com::SphereCollision::SphereCollision(float radius, PhysicMaterial* material) :
	Collision(L"SphereCollision", material)
{
	Reset();
	SetRadius(radius);
}

hos::com::SphereCollision::~SphereCollision()
{
}

hos::com::SphereCollision::SphereCollision(const SphereCollision& dest) :
	Collision(dest),
	Radius(dest.Radius)
{
}


float hos::com::SphereCollision::GetRadius()
{
	return Radius;
}

void hos::com::SphereCollision::SetRadius(float radius)
{
	Radius = Max(radius, CORRECTION_VALUE);

	if (Shape)
	{
		Shape->setGeometry(physx::PxSphereGeometry(Radius));
	}
}

com::SphereCollision* hos::com::SphereCollision::Clone() const
{
	return new SphereCollision(*this);
}

void hos::com::SphereCollision::Reset()
{
	Collision::Reset();
	SetRadius(DEFAULT_RADIUS);
}

void hos::com::SphereCollision::Awake()
{
	Collision::Awake();

	if (g_Px)
	{
		if (Material)
		{
			Shape = g_Px->Physics->createShape(physx::PxSphereGeometry(Radius), *(Material->Get()), true);
		}
		else
		{
			Shape = g_Px->Physics->createShape(physx::PxSphereGeometry(Radius), *g_Px->DefaultMaterial, true);
		}
	}
	else
	{
		Debug->LogConsole(L"Physx", L"Physx is not init, Collision Detection Fails");
	}

	Component::Awake();
}

void hos::com::SphereCollision::OnEnable()
{
	Collision::OnEnable();
	SetRadius(Radius);
}

void hos::com::SphereCollision::Start()
{
	Collision::Start();
}

void hos::com::SphereCollision::FixedUpdate()
{
	Vector3 scale = m_GameObject->transform->GetScale();
	float radius = Radius * Max(Max(scale.x, scale.y), scale.z);
	radius = Max(radius, CORRECTION_VALUE);
	if (Shape)
	{
		Shape->setGeometry(physx::PxSphereGeometry(radius));
	}
}

void hos::com::SphereCollision::PreRender()
{
	if (g_Renderer)
	{
		cg::RenderQue::RenderObject obj;
		Vector3 vs = m_GameObject->transform->GetScale();
		float fs = Max(Max(vs.x, vs.y), vs.z);
		Matrix scale = Matrix::CreateScale(fs);

		obj.mTM = Matrix::CreateScale(Radius * 2.f) * scale * Matrix::CreateTranslation(Center) * Matrix::CreateFromQuaternion(m_GameObject->transform->GetRotation()) * Matrix::CreateTranslation(m_GameObject->transform->GetPosition());
		obj.PrimitiveType = cg::RenderQue::RenderPrimitiveType::SPHERE;
		obj.CollisionState = false;

		g_Renderer->AddDebugObject(obj);
	}
}

const std::vector<U8> hos::com::SphereCollision::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring materialName = "NULL";
	flexbuffers::Builder builder;
	std::vector<uint8_t> mat;
	if (Material)
	{
		materialName = ut::UTF16ToAnsi(Material->GetName());
	}

	builder.Map([&] {
		builder.String("Name", name.c_str());
		builder.Bool("IsActive", GetActive());
		builder.Bool("IsTrigger", IsTrigger);
		builder.String("Material", materialName);
		builder.TypedVector("Center", [&] {
			builder.Float(Center.x);
			builder.Float(Center.y);
			builder.Float(Center.z);
			});
		builder.Float("Radius", Radius);
		builder.UInt("Layer", Layer);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::SphereCollision::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"Sphere try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	IsTrigger = m["IsTrigger"].AsBool();

	mbstring matm = m["Material"].AsString().str();
	string materialName = ut::AnsiToUTF16(matm);
	if (materialName == L"NULL")
	{
		Material = nullptr;
	}
	else
	{
		Material = g_DataManager->GetPhysicMaterial(materialName);
		if (!Material)
		{
			Material = g_DataManager->LoadPhysicMaterial(g_Path + PhysicMaterial::FILE_PATH + materialName + PhysicMaterial::FILE_EXTENSION);
		}
	}

	float x, y, z;
	x = m["Center"].AsTypedVector()[0].AsFloat();
	y = m["Center"].AsTypedVector()[1].AsFloat();
	z = m["Center"].AsTypedVector()[2].AsFloat();
	Center = Vector3(x, y, z);
	Radius = m["Radius"].AsFloat();

	Layer = m["Layer"].AsUInt32();

	return true;
}