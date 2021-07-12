// 자기 헤더
#include "MaterialBuffer.h"

// 네임스페이스
using namespace DirectX::SimpleMath;

hos::cg::MaterialBuffer::MaterialBuffer()
	:DiffuseColor(Vector4(1,1,1,1)), AmbientColor(Vector4(1,1,1,1)), SpecularColor(Vector4(1,1,1,1)),
	OffTill(0,1,0,1), SpecularPower(1.0f)
{

}

hos::cg::MaterialBuffer::~MaterialBuffer()
{

}

void hos::cg::MaterialBuffer::SetDiffColor(const Vector4& color)
{
	this->DiffuseColor = color;
}

void hos::cg::MaterialBuffer::SetAmbColor(const Vector4& color)
{
	this->AmbientColor = color;
}

void hos::cg::MaterialBuffer::SetSpecColor(const Vector4& color)
{
	this->SpecularColor = color;
}

void hos::cg::MaterialBuffer::SetOffTill(const DirectX::SimpleMath::Vector4& offTill)
{
	this->OffTill = offTill;
}

void hos::cg::MaterialBuffer::SetSpecPower(const float& power)
{
	this->SpecularPower = power;
}
