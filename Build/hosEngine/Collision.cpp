#include "Collision.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Scene.h"

using namespace hos;

hos::com::Collision::Collision(string_view name, PhysicMaterial* material) :
	Component(name), Material(material)
{
	ExecutionOrder = I32_MIN + 2;

	Reset();
}

hos::com::Collision::~Collision()
{
	if (Shape)
	{
		if (m_GameObject && m_GameObject->m_Scene)
		{
			m_GameObject->m_Scene->RemoveCollision(this);
		}
		physx::PxRigidActor* actor = Shape->getActor();
		if (actor)
		{
			actor->detachShape(*Shape);
		}
		Shape->release();
	}
}

hos::com::Collision::Collision(const Collision& dest) :
	Component(dest.GetName()),
	Material(dest.Material),
	IsTrigger(dest.IsTrigger),
	Center(dest.Center),
	Shape(nullptr),
	Layer(dest.Layer)
{
}

PhysicMaterial* hos::com::Collision::GetMaterial()
{
	return Material;
}

bool hos::com::Collision::GetIsTrigger()
{
	return IsTrigger;
}

physx::PxShape* hos::com::Collision::GetShape()
{
	return Shape;
}

void hos::com::Collision::SetLayer(U32 index)
{
	if (index < Scene::MAX_LAYER)
	{
		Layer = index;
		physx::PxFilterData fd(index, 0, 0, 0);
		if (Shape)
		{
			Shape->setSimulationFilterData(fd);

			fd.word0 = (1 << index);
			Shape->setQueryFilterData(fd);
		}
	}
}

Vector3 hos::com::Collision::GetCenter() const
{
	return Center;
}

void hos::com::Collision::SetCenter(Vector3 center)
{
	Center = center;

	if (Shape)
	{
		physx::PxTransform lt(Center);
		Shape->setLocalPose(lt);
	}
}

U32 hos::com::Collision::GetLayer() const
{
	return Layer;
}

void hos::com::Collision::SetMaterial(PhysicMaterial* material)
{
	SafeDelete(Material);
	if (material)
	{
		Material = material;
	}

	if (Shape)
	{
		physx::PxMaterial* material[1];
		if (Material)
		{
			material[0] = Material->Get();
		}
		else
		{
			material[0] = g_Px->DefaultMaterial;
		}
		Shape->setMaterials(material, 1);
	}
}

void hos::com::Collision::SetIsTrigger(bool b)
{
	IsTrigger = b;

	if (Shape)
	{
		Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !IsTrigger);
		Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
	}
}

void hos::com::Collision::Bind(hos::com::GameObject* gameObject)
{
	Component::Bind(gameObject);
	if (gameObject->m_Scene)
	{
		gameObject->m_Scene->AddCollision(this);
	}
}

void hos::com::Collision::Reset()
{
	SetIsTrigger(false);
	SetCenter(Vector3::Zero);
	SetLayer(Scene::DEFAULT_LAYER);
}

void hos::com::Collision::Awake()
{
	//Rigid Body가 없을경우
	//다른 콜리전에서 Actor를 생성하지않았을경우
	if (nullptr == m_GameObject->PhysicsActor)
	{
		Transform* transform = m_GameObject->transform;
		physx::PxTransform tt(transform->GetPosition(), transform->GetRotation());

		if (g_Px)
		{
			m_GameObject->PhysicsActor = g_Px->Physics->createRigidStatic(tt);
			m_GameObject->m_Scene->PxScene->addActor(*m_GameObject->PhysicsActor);
		}
	}
}

void hos::com::Collision::Start()
{
}

void hos::com::Collision::OnEnable()
{
	if (m_GameObject)
	{
		if (m_GameObject->m_Scene)
		{
			m_GameObject->m_Scene->AddCollision(this);
		}
	}
	if (Shape)
	{
		if (m_GameObject && m_GameObject->PhysicsActor)
		{
			physx::PxRigidActor* actor = Shape->getActor();
			if (actor)
			{
				actor->detachShape(*Shape);
			}
			m_GameObject->PhysicsActor->attachShape(*Shape);
		}
	}
	SetCenter(Center);
	SetIsTrigger(IsTrigger);
	SetLayer(Layer);
}

void hos::com::Collision::OnDisable()
{
	if (Shape)
	{
		physx::PxRigidActor* actor = Shape->getActor();
		if (actor)
		{
			actor->detachShape(*Shape);
			if (actor != m_GameObject->PhysicsActor)
			{
				m_GameObject->PhysicsActor->detachShape(*Shape);
			}
		}
	}
}
