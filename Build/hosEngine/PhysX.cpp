#include "PhysX.h"
#include <thread>
#include "PhysicMaterial.h"

using namespace hos;
using namespace physx;

hos::PhysX::PhysX() :
	NumDispatcherThread(std::thread::hardware_concurrency()),
	IsDebugMemory(true)
{
	Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);
	if (Foundation == nullptr)
	{
		Foundation = &PxGetFoundation();
	}
	PVD = PxCreatePvd(*Foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	PVD->connect(*transport, PxPvdInstrumentationFlag::eALL);

	Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), IsDebugMemory, PVD);
	Dispatcher = PxDefaultCpuDispatcherCreate(NumDispatcherThread == 0 ? 0 : NumDispatcherThread - 1);

	DefaultMaterial = Physics->createMaterial(PhysicMaterial::DEFAULT_STATIC_FRICTION, PhysicMaterial::DEFAULT_DYNAMIC_FRICTION, PhysicMaterial::DEFAULT_BOUNCINESS);
	if (!DefaultMaterial)
	{
		throw std::invalid_argument("[PhysX] Default Material Constructor Fail");
	}
	DefaultMaterial->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(PhysicMaterial::DEFAULT_FRICTION_COMBINE));
	DefaultMaterial->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(PhysicMaterial::DEFAULT_BOUNCE_COMBINE));
}

hos::PhysX::~PhysX()
{
	SafeRelease(Dispatcher);
	SafeRelease(DefaultMaterial);
	SafeRelease(Physics);
	if (PVD)
	{
		PxPvdTransport* transport = PVD->getTransport();
		PVD->release();
		PVD = nullptr;
		if (transport)
		{
			transport->release();
		}
	}
	SafeRelease(Foundation);
}

physx::PxCooking* hos::PhysX::CreateCooking(const physx::PxCookingParams& params)
{
	return PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, params);
}