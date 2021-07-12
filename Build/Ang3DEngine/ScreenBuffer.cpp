// 자기 헤더
#include "ScreenBuffer.h"

// 네임스페이스
using namespace DirectX::SimpleMath;


hos::cg::ScreenBuffer::ScreenBuffer()
	: NowIvsViewProj(Matrix::Identity), OldViewProj(Matrix::Identity),
	ScreenWidth(0.0f), ScreenHeight(0.0f), ShadowMapWidth(0.0f), ShadowMapHeight(0.0f), EmissMapWidth(0.0f), EmissMapHeight(0.0f),
	EmissIntensity(1.0f), BlurPixelInterval(1.0f),
	RadialValue(0.5f, 0.5f, 0.3f, 3.0f),
	FogColor(0.0f, 0.f, 0.0f), FogDensity(0.0f),
	dTime(0.0f), MortionBlurSampleCount(0),
	BloomPower(0.8f), bFog(true)
{

}

hos::cg::ScreenBuffer::~ScreenBuffer()
{
}

void hos::cg::ScreenBuffer::SetNowCameraInverseMatrix(DirectX::SimpleMath::Matrix nowViewTM, DirectX::SimpleMath::Matrix nowProjTM)
{
	NowIvsViewProj = nowProjTM.Invert() * nowViewTM.Invert();
}

void hos::cg::ScreenBuffer::SetOldCameraMatrix(DirectX::SimpleMath::Matrix oldViewTM, DirectX::SimpleMath::Matrix oldProjTM)
{
	OldViewProj = oldViewTM * oldProjTM;
}

void hos::cg::ScreenBuffer::SetScreenSize(unsigned int width, unsigned int height)
{
	this->ScreenWidth = (float)width;
	this->ScreenHeight = (float)height;
}

void hos::cg::ScreenBuffer::SetShadowMapSize(unsigned int width, unsigned int height)
{
	this->ShadowMapWidth = (float)width;
	this->ShadowMapHeight = (float)height;
}

void hos::cg::ScreenBuffer::SetEmissMapSize(float width, float height)
{
	this->EmissMapWidth = (float)width;
	this->EmissMapHeight = (float)height;
}

void hos::cg::ScreenBuffer::SetEmissIntensity(float intensity)
{
	this->EmissIntensity = intensity;
}

void hos::cg::ScreenBuffer::SetBlurPixelInterval(float interval)
{
	this->BlurPixelInterval = interval;
}

void hos::cg::ScreenBuffer::SetDeltaTime(float dTime)
{
	this->dTime = dTime;
}

void hos::cg::ScreenBuffer::SetMotionBlurSampleCount(unsigned int sampleCount)
{
	this->MortionBlurSampleCount = sampleCount;
}

void hos::cg::ScreenBuffer::SetFog(float r, float g, float b, float density, bool bFog)
{
	this->bFog = bFog;

	this->FogColor.x = r;
	this->FogColor.y = g;
	this->FogColor.z = b;

	this->FogDensity = density;
}

void hos::cg::ScreenBuffer::SetRadialBlurValue(float uv_x, float uv_y, float distance, float strength)
{
	this->RadialValue.x = uv_x;
	this->RadialValue.y = uv_y;
	this->RadialValue.z = distance;
	this->RadialValue.w = strength;
}
