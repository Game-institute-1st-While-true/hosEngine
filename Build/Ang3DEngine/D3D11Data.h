// <Concept>
// Source Name		: D3D11Data.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.01.10

// <Explanation>
// D3D11 API���� ����� enum list ���� ���°�

// Working Log
// 2021.01.10.Hosik	: ����
// 2021.04.20.Hosik : RenderTargetList �߰�.

#pragma once

namespace hos::cg
{

	//enum class RenderMode
	//{
	//	FORWARD,
	//	DEFERRED,

	//	RENDERMODE_MAX,
	//};

	enum class ConstBufferType
	{
		CB_DEFAULT,
		CB_MATERIAL,
		CB_BONE,
		CB_LIGHT,
		CB_SCREEN,

		CB_MAX,
	};

	// ���� ���ٽ� ���°�
	enum
	{
		DS_DEPTH_ON,		//���̹��� ON! (�⺻��), ���ٽǹ��� OFF.
		DS_DEPTH_OFF,		//���̹��� OFF!, ���ٽ� OFF.
		DS_DEPTH_WRITE_OFF,	//���̹��� ���� ����, ���ٽ� OFF. 

		//���ٽ� ���� �����.
		DS_DEPTH_ON_STENCIL_ON,				//���̹��� On (Write On), ���ٽǹ��� ON (������ ����) : "����/���ٽ� ���".
		DS_DEPTH_ON_STENCIL_EQUAL_KEEP,		//���̹��� On (Write On), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���� ��ġ���� �׸���".
		DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP,	//���̹��� On (Write On), ���ٽǹ��� ON (�ٸ���, ������ ����) : "���� ��ġ �̿ܿ� �׸���".

		DS_DEPTH_OFF_STENCIL_ON,			//���̹��� Off, ���ٽǹ��� ON (������ ����) : "���ٽǸ� ���"
		DS_DEPTH_ON_STENCIL_EQUAL_INCR,		//���̹��� On (Write On), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����".
		DS_DEPTH_WRITE_OFF_STENCIL_ON,		//���̹��� On (Write Off), ���ٽǹ��� ON (�׻���, ������ ����) :" ���ٽǸ� ���".
		DS_DEPTH_OFF_STENCIL_EQUAL_INCR,	//���̹��� Off (Write Off), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����.

		DS_MAX,						// �ִ밪 ó����

		DS_DEFAULT = DS_DEPTH_ON,	// �⺻�� ������ ���� ����.
	};


	// �����Ͷ����� ���� ��ü Rasterizer State Objects
	// 
	enum {
		RS_SOLID,				//�ﰢ�� ä��� : Fill Mode - Soild.
		RS_WIREFRM,				//�ﰢ�� ä��� : Fill Mode - Wireframe.
		RS_CULLBACK,			//�޸� �ø� (ON) : Back-Face Culling - "CCW" 
		RS_CULLFRONT,			//���� �ø� (ON) : Front-Face Culling - "CW" 	
		RS_WIRECULLBACK,		//���̾� ������ + �޸� �ø� (ON) 
		RS_WIRECULLFRONT,		//���̾� ������ + �ո� �ø� (ON) 	
		RS_NODEPTH,				//���� Ŭ�� ���� �ʱ�

		//���� ��� �ִ밪.
		RS_MAX_,

		//��� ������. MAX �� ���ʿ� �����ؾ� ��. ����.
		RS_CULL_CCW = RS_CULLBACK,
		RS_CULL_CW = RS_CULLFRONT,
		RS_WIRE_CULL_CCW = RS_WIRECULLBACK,
		RS_WIRE_CULL_CW = RS_WIRECULLFRONT,

	};

	// ����/���� ȥ�� ���� ��ü Blending State Objects 
	//
	enum {
		BS_DEFAULT,				//�⺻ȥ�� ��ü.
		BS_ALPHA_BLEND,			//���� ����. (Src.a + (1-Src.a))
		BS_COLOR_BLEND,			//���� ����. ( Src.Color + (1-Src.Color))
		BS_COLOR_BLEND_ONE,		//1:1 ����. ( Src.Color + Dest.Color) 

		BS_MAX_,

		BS_AB_OFF = BS_DEFAULT,
		BS_AB_ON = BS_ALPHA_BLEND,
		//BS_AB_ALPHA = BS_ALPHA_BLEND,
		//BS_AB_COLOR = BS_COLOR_BLEND,
	};

}