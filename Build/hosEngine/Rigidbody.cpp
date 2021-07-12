#include "Rigidbody.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

using namespace hos;

hos::com::Rigidbody::Rigidbody() : Component(L"Rigidbody")
{
	if (!g_Px)
	{
		throw std::invalid_argument("[RigidBody] PhysX Object is Valid");
	}
	Reset();
	ExecutionOrder = I32_MIN + 1;
}

hos::com::Rigidbody::~Rigidbody()
{
}

hos::com::Rigidbody::Rigidbody(const Rigidbody& dest):
	Component(dest),
	Actor(nullptr),
	Mass(dest.Mass),
	Drag(dest.Drag),
	AngularDrag(dest.AngularDrag),
	UseGravity(dest.UseGravity),
	IsKinematic(dest.IsKinematic),
	FreezePositionX(dest.FreezePositionX),
	FreezePositionY(dest.FreezePositionY),
	FreezePositionZ(dest.FreezePositionZ),
	FreezeRotationX(dest.FreezeRotationX),
	FreezeRotationY(dest.FreezeRotationY),
	FreezeRotationZ(dest.FreezeRotationZ)
{
}

float hos::com::Rigidbody::GetMass() const
{
	return Mass;
}

float hos::com::Rigidbody::GetDrag() const
{
	return Drag;
}

float hos::com::Rigidbody::GetAngularDrag() const
{
	return AngularDrag;
}

bool hos::com::Rigidbody::GetUseGravity() const
{
	return UseGravity;
}

bool hos::com::Rigidbody::GetIsKinematic() const
{
	return IsKinematic;
}

bool hos::com::Rigidbody::GetFreezePositionX() const
{
	return FreezePositionX;
}

bool hos::com::Rigidbody::GetFreezePositionY() const
{
	return FreezePositionY;
}

bool hos::com::Rigidbody::GetFreezePositionZ() const
{
	return FreezePositionZ;
}

bool hos::com::Rigidbody::GetFreezeRotationX() const
{
	return FreezeRotationX;
}

bool hos::com::Rigidbody::GetFreezeRotationY() const
{
	return FreezeRotationY;
}

bool hos::com::Rigidbody::GetFreezeRotationZ() const
{
	return FreezeRotationZ;
}

void hos::com::Rigidbody::SetMass(float mass)
{
	Mass = mass;

	if (Actor)
	{
		Actor->setMass(Mass);
	}
}

void hos::com::Rigidbody::SetDrag(float drag)
{
	Drag = drag;

	if (Actor)
	{
		Actor->setLinearDamping(Drag);
	}
}

void hos::com::Rigidbody::SetAngularDrag(float angularDrag)
{
	AngularDrag = angularDrag;

	if (Actor)
	{
		Actor->setAngularDamping(AngularDrag);
	}
}

void hos::com::Rigidbody::SetUseGravity(bool b)
{
	UseGravity = b;

	if (Actor)
	{
		Actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !UseGravity);
	}
}

void hos::com::Rigidbody::SetIsKinematic(bool b)
{
	IsKinematic = b;

	if (Actor)
	{
		Actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, false);
		Actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, IsKinematic);
	}
}

void hos::com::Rigidbody::SetFreezePositionX(bool b)
{
	FreezePositionX = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, FreezePositionX);
	}
}

void hos::com::Rigidbody::SetFreezePositionY(bool b)
{
	FreezePositionY = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, FreezePositionY);
	}
}

void hos::com::Rigidbody::SetFreezePositionZ(bool b)
{
	FreezePositionZ = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, FreezePositionZ);
	}
}

void hos::com::Rigidbody::SetFreezeRotationX(bool b)
{
	FreezeRotationX = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, FreezeRotationX);
	}
}

void hos::com::Rigidbody::SetFreezeRotationY(bool b)
{
	FreezeRotationY = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, FreezeRotationY);
	}
}

void hos::com::Rigidbody::SetFreezeRotationZ(bool b)
{
	FreezeRotationZ = b;

	if (Actor)
	{
		Actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, FreezeRotationZ);
	}
}

void hos::com::Rigidbody::AddForce(Vector3 force, eForceMode mode)
{
	if (Actor)
	{
		if (mode < NUM_FORCE_MODE)
		{
			Actor->addForce(force, static_cast<physx::PxForceMode::Enum>(mode));
		}
	}
}

void hos::com::Rigidbody::AddTorque(Vector3 torque, eForceMode mode)
{
	if (Actor)
	{
		if (mode < NUM_FORCE_MODE)
		{
			Actor->addTorque(torque, static_cast<physx::PxForceMode::Enum>(mode));
		}
	}
}

void hos::com::Rigidbody::ClearForce()
{
	if (Actor)
	{
		if (!IsKinematic)
		{
			Actor->setLinearVelocity(Vector3(0, 0, 0));
			//Actor->clearForce();
		}
	}
}

void hos::com::Rigidbody::ClearTorque()
{
	if (Actor)
	{
		if (!IsKinematic)
		{
			Actor->setAngularVelocity(Vector3(0, 0, 0));
			//Actor->clearTorque();
		}
	}
}

com::Rigidbody* hos::com::Rigidbody::Clone() const
{
	return new Rigidbody(*this);
}

void hos::com::Rigidbody::Reset()
{
	ClearForce();
	ClearTorque();

	SetMass(DEFAULT_MASS);
	SetDrag(DEFAULT_DRAG);
	SetAngularDrag(DEFAULT_ANGULAR_DRAG);
	SetUseGravity(DEFAULT_USE_GRAVITY);
	SetIsKinematic(DEFAULT_IS_KINEMATIC);
	SetFreezePositionX(DEFAULT_FREEZE);
	SetFreezePositionY(DEFAULT_FREEZE);
	SetFreezePositionZ(DEFAULT_FREEZE);
	SetFreezeRotationX(DEFAULT_FREEZE);
	SetFreezeRotationY(DEFAULT_FREEZE);
	SetFreezeRotationZ(DEFAULT_FREEZE);
}

void hos::com::Rigidbody::Awake()
{
	if (!Actor)
	{
		physx::PxTransform tt(m_GameObject->transform->GetPosition(), m_GameObject->transform->GetRotation());
		Actor = g_Px->Physics->createRigidDynamic(tt);
		Actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
		if (m_GameObject->PhysicsActor)
		{
			m_GameObject->m_Scene->PxScene->removeActor(*m_GameObject->PhysicsActor);
			m_GameObject->PhysicsActor->release();
			m_GameObject->PhysicsActor = nullptr;
		}
		m_GameObject->PhysicsActor = Actor;
	}

	Component::Awake();
}

void hos::com::Rigidbody::OnEnable()
{
	SetMass(Mass);
	SetDrag(Drag);
	SetAngularDrag(AngularDrag);
	SetUseGravity(UseGravity);
	SetIsKinematic(IsKinematic);
	SetFreezePositionX(FreezePositionX);
	SetFreezePositionY(FreezePositionY);
	SetFreezePositionZ(FreezePositionZ);
	SetFreezeRotationX(FreezeRotationX);
	SetFreezeRotationY(FreezeRotationY);
	SetFreezeRotationZ(FreezeRotationZ);

	if (m_GameObject && m_GameObject->PhysicsActor)
	{
		if (m_GameObject->GetActive())
		{
			physx::PxScene* scene = m_GameObject->PhysicsActor->getScene();
			if (scene)
			{
				scene->removeActor(*m_GameObject->PhysicsActor);
			}
			m_GameObject->m_Scene->PxScene->addActor(*m_GameObject->PhysicsActor);
		}
	}
}

void hos::com::Rigidbody::OnDisable()
{
	if (m_GameObject && m_GameObject->PhysicsActor)
	{
		physx::PxScene* scene = m_GameObject->PhysicsActor->getScene();
		if (scene)
		{
			scene->removeActor(*m_GameObject->PhysicsActor);
		}
		m_GameObject->m_Scene->PxScene->removeActor(*m_GameObject->PhysicsActor);
	}
}

void hos::com::Rigidbody::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->rigidbody)
	{
		throw std::invalid_argument("this Object already has Rigidbody");
	}

	m_GameObject = gameObject;
	gameObject->rigidbody = this;
}

void hos::com::Rigidbody::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->rigidbody = nullptr;
		m_GameObject = nullptr;
	}
}

const std::vector<U8> hos::com::Rigidbody::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;

	builder.Map([&] {
		builder.String("Name", name);
		builder.Float("Mass", Mass);
		builder.Float("Drag", Drag);
		builder.Float("AngularDrag", AngularDrag);
		builder.Bool("UseGravity", UseGravity);
		builder.Bool("IsKinematic", IsKinematic);

		builder.Bool("FreezePositionX", FreezePositionX);
		builder.Bool("FreezePositionY", FreezePositionY);
		builder.Bool("FreezePositionZ", FreezePositionZ);

		builder.Bool("FreezeRotationX", FreezeRotationX);
		builder.Bool("FreezeRotationY", FreezeRotationY);
		builder.Bool("FreezeRotationZ", FreezeRotationZ);

		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Rigidbody::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"Rigidbody try Deserialize to" + name);
		return false;
	}

	Mass = m["Mass"].AsFloat();
	Drag = m["Drag"].AsFloat();
	AngularDrag = m["AngularDrag"].AsFloat();
	UseGravity = m["UseGravity"].AsBool();
	IsKinematic = m["IsKinematic"].AsBool();

	FreezePositionX = m["FreezePositionX"].AsBool();
	FreezePositionY = m["FreezePositionY"].AsBool();
	FreezePositionZ = m["FreezePositionZ"].AsBool();

	FreezeRotationX = m["FreezeRotationX"].AsBool();
	FreezeRotationY = m["FreezeRotationY"].AsBool();
	FreezeRotationZ = m["FreezeRotationZ"].AsBool();

	return true;
}
