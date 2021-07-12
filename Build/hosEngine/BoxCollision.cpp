#include "BoxCollision.h"
#include "Transform.h"
#include "Rigidbody.h"

using namespace hos;

hos::com::BoxCollision::BoxCollision(float x, float y, float z, PhysicMaterial* material) :
	Collision(L"BoxCollision", material)
{
	Reset();
	SetSize(Vector3(x, y, z));
}

hos::com::BoxCollision::~BoxCollision()
{
}

hos::com::BoxCollision::BoxCollision(const BoxCollision& dest) :
	Collision(dest),
	Size(dest.Size)
{
}

Vector3 hos::com::BoxCollision::GetSize() const
{
	return Size;
}

float hos::com::BoxCollision::GetSizeX() const
{
	return Size.x;
}

float hos::com::BoxCollision::GetSizeY() const
{
	return Size.y;
}

float hos::com::BoxCollision::GetSizeZ() const
{
	return Size.z;
}

void hos::com::BoxCollision::SetSizeX(float x)
{
	Size.x = Max(x, CORRECTION_VALUE);
	if (Shape)
	{
		Shape->setGeometry(physx::PxBoxGeometry(Vector3(Size / 2)));
	}
}

void hos::com::BoxCollision::SetSizeY(float y)
{
	Size.y = Max(y, CORRECTION_VALUE);
	if (Shape)
	{
		Shape->setGeometry(physx::PxBoxGeometry(Vector3(Size / 2)));
	}
}

void hos::com::BoxCollision::SetSizeZ(float z)
{
	Size.z = Max(z, CORRECTION_VALUE);
	if (Shape)
	{
		Shape->setGeometry(physx::PxBoxGeometry(Vector3(Size / 2)));
	}
}

void hos::com::BoxCollision::SetSize(Vector3 size)
{
	Size.x = Max(size.x, CORRECTION_VALUE);
	Size.y = Max(size.y, CORRECTION_VALUE);
	Size.z = Max(size.z, CORRECTION_VALUE);

	if (Shape)
	{
		Shape->setGeometry(physx::PxBoxGeometry(Vector3(Size / 2)));
	}
}

com::BoxCollision* hos::com::BoxCollision::Clone() const
{
	return new BoxCollision(*this);
}

void hos::com::BoxCollision::Reset()
{
	Collision::Reset();
	SetSize(Vector3(DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_SIZE));
}

void hos::com::BoxCollision::Awake()
{
	Collision::Awake();

	if (g_Px)
	{
		if (Material)
		{
			Shape = g_Px->Physics->createShape(physx::PxBoxGeometry(Vector3(Size / 2)), *(Material->Get()), true);
		}
		else
		{
			Shape = g_Px->Physics->createShape(physx::PxBoxGeometry(Vector3(Size / 2)), *g_Px->DefaultMaterial, true);
		}
	}
	else
	{
		Debug->LogConsole(L"Physx", L"Physx is not init, Collision Detection Fails");
	}

	Component::Awake();
}

void hos::com::BoxCollision::OnEnable()
{
	Collision::OnEnable();
	SetSize(Size);
}

void hos::com::BoxCollision::FixedUpdate()
{
	Vector3 size = (Size / 2) * m_GameObject->transform->GetScale();

	size.x = Max(size.x, CORRECTION_VALUE);
	size.y = Max(size.y, CORRECTION_VALUE);
	size.z = Max(size.z, CORRECTION_VALUE);

	if (Shape)
	{
		Shape->setGeometry(physx::PxBoxGeometry(size));
	}
}

void hos::com::BoxCollision::PreRender()
{
	if (g_Renderer)
	{
		cg::RenderQue::RenderObject obj;

		obj.mTM = Matrix::CreateScale(Size) * Matrix::CreateTranslation(Center) * m_GameObject->transform->GetTransformMatrix();
		obj.PrimitiveType = cg::RenderQue::RenderPrimitiveType::BOX;
		obj.CollisionState = false;

		g_Renderer->AddDebugObject(obj);
	}
}

const std::vector<U8> hos::com::BoxCollision::Serialize()
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
		builder.TypedVector("Shape", [&] {
			builder.Float(Size.x);
			builder.Float(Size.y);
			builder.Float(Size.z);
			});
		builder.UInt("Layer", Layer);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::BoxCollision::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"BoxCollision try Deserialize to" + name);
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
	x = m["Shape"].AsTypedVector()[0].AsFloat();
	y = m["Shape"].AsTypedVector()[1].AsFloat();
	z = m["Shape"].AsTypedVector()[2].AsFloat();
	Size = Vector3(x, y, z);

	Layer = m["Layer"].AsUInt32();

	return true;
}