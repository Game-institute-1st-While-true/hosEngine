// 자기 헤더
#include "RendererDesc.h"


hos::cg::RendererDesc::RendererDesc()
	: Driver(DriverType::DRIVER_NULL), Hwnd(nullptr),
	Fullscreen(false), Vsync(false),
	ScreenWidth(RENDERER_SCREENWIDTH), ScreenHeight(RENDERER_SCREENHEIGHT),
	ScreenDepth(1000.0f), ScreenNear(1.0f),
	ValueAA(1), ValueAF(1),
	VideoCardMemory_MB(0), VideoCardName(L""),
	Numerator(0), Denominator(0),
	MotionBlurState(true), FogState(true)
{

}

hos::cg::RendererDesc::RendererDesc(const hos::cg::RendererDesc& desc)
{
	// 디바이스 정보 저장
	*this = desc;

	// 복사 도중 잘못된 정보가 있으면 교정합니다.
	CorrectDescValue();
}

hos::cg::RendererDesc& hos::cg::RendererDesc::operator=(const hos::cg::RendererDesc& rhs)
{
	this->Driver = rhs.Driver;
	this->Hwnd = rhs.Hwnd;
	this->Fullscreen = rhs.Fullscreen;
	this->Vsync = rhs.Vsync;
	this->ScreenWidth = rhs.ScreenWidth;
	this->ScreenHeight = rhs.ScreenHeight;
	this->ScreenDepth = rhs.ScreenDepth;
	this->ScreenNear = rhs.ScreenNear;
	this->ValueAA = rhs.ValueAA;
	this->ValueAF = rhs.ValueAF;

	this->MotionBlurState = rhs.MotionBlurState;
	this->FogState = rhs.FogState;

	return *this;
}

hos::cg::RendererDesc::~RendererDesc()
{

}

void hos::cg::RendererDesc::CorrectDescValue()
{
	// 스크린 근평면, 원평면 값 교정
	if (this->ScreenDepth < 1)
	{
		this->ScreenDepth = 1.0f;
	}

	if (this->ScreenNear <= 0.0f)
	{
		this->ScreenNear = 0.001f;
	}

	// 시야 오류 교정
	if (this->ScreenNear > this->ScreenDepth)
	{
		this->ScreenDepth = this->ScreenNear + 0.001f;
	}

	// AA 교정
	if (this->ValueAA < 1)
	{
		this->ValueAA = 1;	// 꺼져있음
	}

	else if (this->ValueAA > 8)
	{
		this->ValueAA = 8;	// 최대값
	}

	// AF 교정
	if (this->ValueAF < 1)
	{
		this->ValueAF = 1;	// 꺼져있음
	}

	else if (this->ValueAF > 16)
	{
		this->ValueAF = 16;	// 최대값
	}
}
