// <Concept>
// Source Name		: RenderderDesc.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2020.11.05

// <Explanation>
// �������������� �Ķ���ͷ� �Ѱ��ֱ����� Ŭ����.
// Class�� Value Instance�� ���� ��������ڿ� ���Կ����ڸ� �����մϴ�.

// Working Log
// 2021.01.08.Hosik	: ���� // ���������, ���Կ����� ����, 1�ܰ� �׽�Ʈ.

#pragma once

#include "framework.h"
#include <string>

#define RENDERER_SCREENWIDTH 640
#define RENDERER_SCREENHEIGHT 480

namespace hos::cg
{
	class RendererDesc
	{
	public:
		// ����̹� Ÿ��
		enum class DriverType
		{
			DRIVER_DIRECT3D11,
			DRIVER_NULL,

			DRIVER_MAX,
		};

		// ���� Ÿ��
		enum class RenderModeList
		{
			RENDER_PBR,
			RENDER_LAMBERT,

			RENDER_MAX,

			RENDER_DEFAULT = RENDER_LAMBERT,
		};

		// ī�޶� Ÿ��
		enum class CameraModeList
		{
			CAMERA3D_DEFAULT,
			CAMERA3D_FOLLOW,
			CAMERA2D_DEFAULT,
			CAMERA2D_FOLLOW,
			CAMERA_SCREENSPACE,
		};

		// SHADOWMAP Ÿ��
		enum class ShadowMapCullList
		{
			SHADOWMAP_SOLID,
			SHADOWMAP_CULLBACK,
			SHADOWMAP_CULLFRONT,
		};

	public:
		// �ϵ���� ����
		// TODO: �����찡 �ƴ� ��? HWND�� �ƴ϶� ���Ŷ��??
		DriverType Driver;			// �׷��� ����̹� Ÿ��
		HWND Hwnd;					// ������ �ڵ�

		// ���� ����
		bool Fullscreen;
		bool Vsync;					// ��������ȭ
		unsigned int ScreenWidth;
		unsigned int ScreenHeight;
		float ScreenDepth;
		float ScreenNear;			// ����� (�ּ� 0���� Ŀ���Ѵ�)
		unsigned int ValueAA;		// ��Ƽ �ٸ���� (1:OFF ~ 8:�ִ�)
		unsigned int ValueAF;		// ���漺 ���͸� (1:OFF ~ 16:�ִ�)

		// VGA ����
		unsigned int VideoCardMemory_MB;
		std::wstring VideoCardName;
		unsigned int Numerator;
		unsigned int Denominator;

		// ���� �ɼ�
		bool MotionBlurState;
		bool FogState;

	public:
		RendererDesc();										// �⺻������
		RendererDesc(const RendererDesc& desc);				// ���������
		RendererDesc& operator=(const RendererDesc& rhs);	// ���Կ�����
		~RendererDesc();									// �Ҹ���

	public:
		void CorrectDescValue();	// ��/������ AA,AF�ܰ谡 �߸��� ��� �����ϴ� �Լ�
	};
}