#ifndef _DXTK_H
#define _DXTK_H

/*
Author - Roh jong won

DirectX Tool Kit - jun 1, 2020

2020/06/06
DXTK�� ����ϱ����� ���̺귯���߰�& ��� �߰�
������Ʈ �Ӽ� - VC++ ���͸� - ���� ���͸� <- DXTK ��� ���� �߰�
������Ʈ �Ӽ� - VC++ ���͸� - ���̺귯�� ���͸� <- DXTK ���̺귯�� ���� �߰�

2020/06/20
ConvertDipsToPixels()	�߰�
SdkLayersAvailable()	�߰�

*/

//���̺귯�� �߰�
#ifdef _DEBUG 
#ifdef _WIN64
#pragma comment(lib, "DirectXTK64D")	//64bit �����
#elif _WIN32
#pragma comment(lib, "DirectXTKD")		//32bit �����
#endif
#else
#ifdef _WIN64 
#pragma comment(lib, "DirectXTK64")		//64bit
#elif _WIN32 
#pragma comment(lib, "DirectXTK")		//32bit
#endif
#endif

//DXTK ��� ����
#pragma warning(push)
#pragma warning(disable:26812)

// ������� �߰�
//low-level audio API using XAudio2 (DirectXTK for Audio public header)
#include "Audio.h"
//C++ helpers for creating D3D resources from CPU data
#include "BufferHelpers.h"
//factory providing commonly used D3D state objects
#include "CommonStates.h"
// light - weight DDS file texture loader
#include "DDSTextureLoader.h"
//misc C++ helpers for D3D programming
#include "DirectXHelpers.h"
//set of built - in shaders for common rendering tasks
#include "Effects.h"
//gamepad controller helper using XInput
#include "GamePad.h"
//draws basic shapes such as cubes and spheres
#include "GeometricPrimitive.h"
//helper for managing dynamic graphics memory allocation
#include "GraphicsMemory.h"
//keyboard state tracking helper
#include "Keyboard.h"
//draws meshes loaded from.CMO, .SDKMESH, or .VBO files
#include "Model.h"
//mouse helper
#include "Mouse.h"
//set of built - in shaders for common post - processing operations
#include "PostProcess.h"
//simple and efficient way to draw user primitives
#include "PrimitiveBatch.h"
//light - weight screen shot saver
#include "ScreenGrab.h"
//simplified C++ wrapper for DirectXMath
#include "SimpleMath.h"
//simple & efficient 2D sprite rendering
#include "SpriteBatch.h"
//bitmap based text rendering
#include "SpriteFont.h"
//structures for commonly used vertex data formats
#include "VertexTypes.h"
//WIC - based image file texture loader
#include "WICTextureLoader.h"
//Xbox One exclusive apps variant of DDSTextureLoader
#ifdef _XBOX_ONE
#include "XboxDDSTextureLoader.h"
#endif //!_XBOX_ONE

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <exception>
namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}

	// DIP(����̽� ������ �ȼ�) ���̸� ������ �ȼ� ���̷� ��ȯ�մϴ�.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // ���� ������ ������ �ݿø��մϴ�.
	}

#if defined(_DEBUG)
	// SDK ���̾� ������ Ȯ���ϼ���.
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // ���� �ϵ���� ����̽��� ���� �ʿ䰡 �����ϴ�.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // SDK ���̾ Ȯ���ϼ���.
			nullptr,                    // ��� ��� ������ ����˴ϴ�.
			0,
			D3D11_SDK_VERSION,          // Microsoft Store ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
			nullptr,                    // D3D ����̽� ������ ������ �ʿ䰡 �����ϴ�.
			nullptr,                    // ��� ������ �� �ʿ䰡 �����ϴ�.
			nullptr                     // D3D ����̽� ���ؽ�Ʈ ������ ������ �ʿ䰡 �����ϴ�.
			);

		return SUCCEEDED(hr);
	}
#endif
}

//DXTK ��� ���� ����
#pragma warning(pop)

#endif // !_DXTK_H