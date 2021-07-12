#include "MeshCollision.h"
#include "Transform.h"
#include "Rigidbody.h"

using namespace hos;

hos::com::MeshCollision::MeshCollision(Mesh* mesh, PhysicMaterial* material) :
	Collision(L"MeshCollision", material),
	Cooking(nullptr),
	Data(mesh),
	IsConvex(false),
	CookingFaster(false),
	CookingMeshCleaning(false),
	CookingVerticesWeld(false),
	CookingUseFastMidphase(false)
{
}

hos::com::MeshCollision::~MeshCollision()
{
	SafeRelease(Cooking);
}

bool hos::com::MeshCollision::GetIsConvex() const
{
	return IsConvex;
}

bool hos::com::MeshCollision::GetCookingFaster() const
{
	return CookingFaster;
}

bool hos::com::MeshCollision::GetCookingMeshCleaning() const
{
	return CookingMeshCleaning;
}

bool hos::com::MeshCollision::GetCookingVerticesWeld() const
{
	return CookingVerticesWeld;
}

bool hos::com::MeshCollision::GetCookingUseFastMidphase() const
{
	return CookingUseFastMidphase;
}

void hos::com::MeshCollision::SetIsConvex(bool b)
{
	IsConvex = b;
}

void hos::com::MeshCollision::SetIsTrigger(bool b)
{
	IsTrigger = b;

	if (Shape)
	{
		Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger && IsConvex);
	}
}

void hos::com::MeshCollision::SetCookingFaster(bool b)
{
	CookingFaster = b;
}

void hos::com::MeshCollision::SetCookingMeshCleaning(bool b)
{
	CookingMeshCleaning = b;
}

void hos::com::MeshCollision::SetCookingVerticesWeld(bool b)
{
	CookingVerticesWeld = b;
}

void hos::com::MeshCollision::SetUseFastMidphase(bool b)
{
	CookingUseFastMidphase = b;
}

void hos::com::MeshCollision::Reset()
{
	Collision::Reset();
	Data = nullptr;
	IsConvex = false;
	IsTrigger = false;
	CookingFaster = false;
	CookingMeshCleaning = false;
	CookingVerticesWeld = false;
	CookingUseFastMidphase = false;
}

void hos::com::MeshCollision::Awake()
{
	Collision::Awake();

	physx::PxCookingParams cookingParams(g_Px->Physics->getTolerancesScale());
	cookingParams.midphaseDesc.setToDefault(CookingUseFastMidphase ? physx::PxMeshMidPhase::eBVH34 : physx::PxMeshMidPhase::eBVH33);

	physx::PxMeshPreprocessingFlags flags;
	if (!CookingMeshCleaning)
	{
		if (CookingVerticesWeld)
		{
			flags = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
		}
		else
		{
			flags = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
		}
	}
	cookingParams.meshPreprocessParams = flags;

	Cooking = g_Px->CreateCooking(cookingParams);

	//TODO : MeshDesc에 메시 데이터 채워넣기
	if (IsConvex)
	{
		physx::PxConvexMeshDesc desc;
		physx::PxConvexMesh* mesh = Cooking->createConvexMesh(desc, g_Px->Physics->getPhysicsInsertionCallback());
		if (Material)
		{
			Shape = g_Px->Physics->createShape(physx::PxConvexMeshGeometry(mesh), *(Material->Get()), true);
		}
		else
		{
			Shape = g_Px->Physics->createShape(physx::PxConvexMeshGeometry(mesh), *g_Px->DefaultMaterial, true);
		}
	}
	else
	{
		physx::PxTriangleMeshDesc desc;
		physx::PxTriangleMesh* mesh = Cooking->createTriangleMesh(desc, g_Px->Physics->getPhysicsInsertionCallback());
		if (Material)
		{
			Shape = g_Px->Physics->createShape(physx::PxTriangleMeshGeometry(mesh), *(Material->Get()), true);
		}
		else
		{
			Shape = g_Px->Physics->createShape(physx::PxTriangleMeshGeometry(mesh), *g_Px->DefaultMaterial, true);
		}
	}

	Component::Awake();
}
