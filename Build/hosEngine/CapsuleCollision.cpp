#include "CapsuleCollision.h"
#include "Transform.h"
#include "Rigidbody.h"

using namespace hos;

hos::com::CapsuleCollision::CapsuleCollision(float radius, float height, eDirection dir, PhysicMaterial* material) :
	Collision(L"CapsuleCollision", material)
{
	Reset();
	SetRadius(radius);
	SetHeight(height);
	SetDirection(dir);
}

hos::com::CapsuleCollision::~CapsuleCollision()
{
}

hos::com::CapsuleCollision::CapsuleCollision(const CapsuleCollision& dest) :
	Collision(dest),
	Radius(dest.Radius),
	HalfHeight(dest.HalfHeight),
	Height(dest.Height),
	Direction(dest.Direction)
{
}

float hos::com::CapsuleCollision::GetRadius() const
{
	return Radius;
}

float hos::com::CapsuleCollision::GetHeight() const
{
	return Height;
}

com::CapsuleCollision::eDirection hos::com::CapsuleCollision::GetDirection() const
{
	return Direction;
}

void hos::com::CapsuleCollision::SetCenter(Vector3 center)
{
	Center = center;

	SetDirection(Direction);
}

void hos::com::CapsuleCollision::SetRadius(float radius)
{
	Radius = Max(radius, CORRECTION_VALUE);
	if (Height < Radius * 2)
	{
		float height = Radius * 2.f;
		HalfHeight = Max(height - (2 * Radius), CORRECTION_VALUE);
		Height = HalfHeight * 2.f;
	}

	if (Shape)
	{
		Shape->setGeometry(physx::PxCapsuleGeometry(Radius, HalfHeight));
	}
}

void hos::com::CapsuleCollision::SetHeight(float height)
{
	Height = height;
	HalfHeight = Max(Height / 2, CORRECTION_VALUE);

	if (Shape)
	{
		Shape->setGeometry(physx::PxCapsuleGeometry(Radius, HalfHeight));
	}
}

void hos::com::CapsuleCollision::SetDirection(eDirection dir)
{
	Direction = dir;

	if (Shape)
	{
		physx::PxTransform lt;
		switch (Direction)
		{
		case hos::com::CapsuleCollision::DIRECTION_X:
			lt = physx::PxTransform(physx::PxVec3(Center), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(1, 0, 0)));
			break;
		case hos::com::CapsuleCollision::DIRECTION_Y:
			lt = physx::PxTransform(physx::PxVec3(Center), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));
			break;
		case hos::com::CapsuleCollision::DIRECTION_Z:
			lt = physx::PxTransform(physx::PxVec3(Center), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 1, 0)));
			break;
		}

		Shape->setLocalPose(lt);
	}
}

com::CapsuleCollision* hos::com::CapsuleCollision::Clone() const
{
	return new CapsuleCollision(*this);
}

void hos::com::CapsuleCollision::Reset()
{
	Collision::Reset();
	SetHeight(DEFAULT_HEIGHT);
	SetRadius(DEFAULT_RADIUS);
	SetDirection(DEFAULT_DIRECTION);
}

void hos::com::CapsuleCollision::Awake()
{
	Collision::Awake();

	if (g_Px)
	{
		if (Material)
		{
			Shape = g_Px->Physics->createShape(physx::PxCapsuleGeometry(Radius, HalfHeight), *(Material->Get()), true);
		}
		else
		{
			Shape = g_Px->Physics->createShape(physx::PxCapsuleGeometry(Radius, HalfHeight), *g_Px->DefaultMaterial, true);
		}
	}
	else
	{
		Debug->LogConsole(L"Physx", L"Physx is not init, Collision Detection Fails");
	}
	SetDirection(Direction);

	Component::Awake();
}

void hos::com::CapsuleCollision::OnEnable()
{
	Collision::OnEnable();
	SetRadius(Radius);
	SetHeight(Height);
	//SetCenterÆ÷ÇÔ
	SetDirection(Direction);
}

void hos::com::CapsuleCollision::FixedUpdate()
{
	Vector3 scale = m_GameObject->transform->GetScale();
	float radius = Radius * Max(scale.x, scale.z);
	radius = Max(radius, CORRECTION_VALUE);
	float height = HalfHeight * scale.y;
	height = Max(height, CORRECTION_VALUE);
	if (Shape)
	{
		Shape->setGeometry(physx::PxCapsuleGeometry(radius, height));
	}
}

void hos::com::CapsuleCollision::PreRender()
{
	if (g_Renderer)
	{
		cg::RenderQue::RenderObject obj;

		Matrix scale;
		float fs = Radius * 2.f;
		Vector3 vs = m_GameObject->transform->GetScale();
		float t = Max(vs.x, vs.z);
		Matrix ms = Matrix::CreateScale(t, 1, t);
		switch (Direction)
		{
		case hos::com::CapsuleCollision::DIRECTION_X:
			scale = Matrix::CreateScale(fs, Radius + (HalfHeight * vs.y), fs) * ms * Matrix::CreateRotationZ(1.5708f);
			break;
		case hos::com::CapsuleCollision::DIRECTION_Y:
			scale = Matrix::CreateScale(fs, Radius + (HalfHeight * vs.y), fs) * ms;
			break;
		case hos::com::CapsuleCollision::DIRECTION_Z:
			scale = Matrix::CreateScale(fs, Radius + (HalfHeight * vs.y), fs) * ms * Matrix::CreateRotationX(1.5708f);
			break;
		}
		Rotator r = m_GameObject->transform->GetRotation();
		Matrix m = Matrix::CreateFromQuaternion(r);
		obj.mTM = scale * Matrix::CreateTranslation(Center) * Matrix::CreateFromQuaternion(m_GameObject->transform->GetRotation()) * Matrix::CreateTranslation(m_GameObject->transform->GetPosition());
		obj.PrimitiveType = cg::RenderQue::RenderPrimitiveType::CAPSULE;
		obj.CollisionState = false;

		g_Renderer->AddDebugObject(obj);
	}
}

const std::vector<U8> hos::com::CapsuleCollision::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring materialName = "NULL";
	if (Material)
	{
		materialName = ut::UTF16ToAnsi(Material->GetName());
	}
	flexbuffers::Builder builder;

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
		builder.Float("Height", Height);
		builder.UInt("Layer", Layer);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::CapsuleCollision::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"CapsuleCollision try Deserialize to" + name);
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
	Height = m["Height"].AsFloat();
	HalfHeight = Height / 2.f;

	Layer = m["Layer"].AsUInt32();

	return true;
}