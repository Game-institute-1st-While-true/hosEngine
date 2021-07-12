// �ڱ� ���
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
	// ����̽� ���� ����
	*this = desc;

	// ���� ���� �߸��� ������ ������ �����մϴ�.
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
	// ��ũ�� �����, ����� �� ����
	if (this->ScreenDepth < 1)
	{
		this->ScreenDepth = 1.0f;
	}

	if (this->ScreenNear <= 0.0f)
	{
		this->ScreenNear = 0.001f;
	}

	// �þ� ���� ����
	if (this->ScreenNear > this->ScreenDepth)
	{
		this->ScreenDepth = this->ScreenNear + 0.001f;
	}

	// AA ����
	if (this->ValueAA < 1)
	{
		this->ValueAA = 1;	// ��������
	}

	else if (this->ValueAA > 8)
	{
		this->ValueAA = 8;	// �ִ밪
	}

	// AF ����
	if (this->ValueAF < 1)
	{
		this->ValueAF = 1;	// ��������
	}

	else if (this->ValueAF > 16)
	{
		this->ValueAF = 16;	// �ִ밪
	}
}
