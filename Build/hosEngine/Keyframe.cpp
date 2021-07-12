#include "Keyframe.h"

hos::Keyframe::Keyframe()
	: TimePos(0.0f), vPos(0,0,0), vScale(1,1,1), vRotQuat(0,0,0,0)
{
}

hos::Keyframe::~Keyframe()
{
}

void hos::Keyframe::SetTransform(Vector3 vPos, Vector3 vScale, Vector4 vRotQuat)
{
	this->vPos = vPos;
	this->vScale = vScale;
	this->vRotQuat = vRotQuat;
}

void hos::Keyframe::SetTransform(DirectX::SimpleMath::Vector3 vPos, DirectX::SimpleMath::Vector3 vScale, DirectX::SimpleMath::Vector4 vRotQuat)
{
	this->vPos = vPos;
	this->vScale = vScale;
	this->vRotQuat = vRotQuat;
}