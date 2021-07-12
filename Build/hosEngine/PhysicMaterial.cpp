#include "Object.h"
#include "PhysicMaterial.h"

using namespace hos;

hos::PhysicMaterial::PhysicMaterial(float dynamicFriction, float staticFriction, float bounciness, eCombine frictionCombine, eCombine bounceCombine) :
	DynamicFriction(dynamicFriction),
	StaticFriction(staticFriction),
	Bounciness(bounciness),
	FrictionCombine(frictionCombine),
	BounceCombine(bounceCombine)
{
	if (g_Px)
	{
		if (!g_Px)
		{
			throw std::invalid_argument("[PhysicMaterial] PhysX Object is Valid");
		}

		Material = g_Px->Physics->createMaterial(StaticFriction, DynamicFriction, Bounciness);
		if (!Material)
		{
			throw std::invalid_argument("[PhysicMaterial] Constructor Fail");
		}
		Material->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(FrictionCombine));
		Material->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(BounceCombine));
	}
}

hos::PhysicMaterial::~PhysicMaterial()
{
	SafeRelease(Material);
}

physx::PxMaterial* hos::PhysicMaterial::Get() const
{
	return Material;
}

float hos::PhysicMaterial::GetDynamicFriction() const
{
	return DynamicFriction;
}

float hos::PhysicMaterial::GetStaticFriction() const
{
	return StaticFriction;
}

float hos::PhysicMaterial::GetBounciness() const
{
	return Bounciness;
}

PhysicMaterial::eCombine hos::PhysicMaterial::GetFrictionCombine() const
{
	return FrictionCombine;
}

PhysicMaterial::eCombine hos::PhysicMaterial::GetBounceCombine() const
{
	return BounceCombine;
}

string hos::PhysicMaterial::GetName() const
{
	return Name;
}

void hos::PhysicMaterial::SetDynamicFriction(float friction)
{
	if (0 <= friction)
	{
		DynamicFriction = friction;
		Material->setDynamicFriction(DynamicFriction);
	}
}

void hos::PhysicMaterial::SetStaticFriction(float friction)
{
	if (0 <= friction)
	{
		StaticFriction = friction;
		Material->setStaticFriction(StaticFriction);
	}
}

void hos::PhysicMaterial::SetBounciness(float bounce)
{
	if (0 <= bounce && bounce <= 1)
	{
		Bounciness = bounce;
		Material->setRestitution(Bounciness);
	}
}

void hos::PhysicMaterial::SetFrictionCombine(eCombine combine)
{
	FrictionCombine = combine;
	Material->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(combine));
}

void hos::PhysicMaterial::SetBounceCombine(eCombine combine)
{
	BounceCombine = combine;
	Material->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(combine));
}

const std::vector<U8> hos::PhysicMaterial::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(Name);
	flexbuffers::Builder builder;
	builder.Vector([&] {
		builder.Float(DynamicFriction);
		builder.Float(StaticFriction);
		builder.Float(Bounciness);
		builder.Int(static_cast<int>(FrictionCombine));
		builder.Int(static_cast<int>(BounceCombine));
		builder.String(name);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::PhysicMaterial::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsVector();
	DynamicFriction = m[0].AsFloat();
	StaticFriction = m[1].AsFloat();
	Bounciness = m[2].AsFloat();
	FrictionCombine = static_cast<eCombine>(m[3].AsInt32());
	BounceCombine = static_cast<eCombine>(m[4].AsInt32());
	mbstring name = m[5].AsString().str();
	Name = ut::AnsiToUTF16(name);

	SafeRelease(Material);

	if (g_Px && g_Px->Physics)
	{
		Material = g_Px->Physics->createMaterial(StaticFriction, DynamicFriction, Bounciness);
		if (!Material)
		{
			throw std::invalid_argument("[PhysicMaterial] Deserialize Fail");
			return false;
		}
		Material->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(FrictionCombine));
		Material->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(BounceCombine));
	}
	else
	{
		return false;
	}

	return true;
}
