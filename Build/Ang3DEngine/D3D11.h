// <Concept>
// Source Name		: D3D11.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.01.10

// <Explanation>
// D3D11�� API Ŭ����

// Working Log
// 2021.01.10.Hosik	: ����
// 2021.02.04.Hosil : DeviceManager�� ����̽� �ڿ��� ���� �߰�

#pragma once

#include <d3d11_4.h>
#include <wrl.h>

#include "D3D11Data.h"
#include "RendererDesc.h"


namespace hos::cg
{
	class D3D11
	{
		// Device�� Context�� DeviceManager�� ���ļ��� ������ �� �ִ�.
		friend class D3D11DeviceManager;

		// ��� �ν��Ͻ�
	private:
		// �׷��� ���� ����
		//DEVICE_INFO DeviceInfo;
		//VGA_INFO	VgaInfo;
		//HWND Hwnd;
		RendererDesc& RenderDesc;

		// DirectX ����̽� ��ü
		Microsoft::WRL::ComPtr<ID3D11Device> Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;

		// ����ü��, ����Ÿ�� ��
		Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> RenderTargetBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

		// ���� ���ٽ� ���� �� ��
		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

		// ���°�ü ���
		unsigned int NowDSState;
		unsigned int NowRSState;
		unsigned int NowBSState;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState[DS_MAX];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterState[RS_MAX_];
		Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState[BS_MAX_];

		// ������ �Ҹ���
	public:
		D3D11() = delete;
		D3D11(RendererDesc& desc);
		~D3D11();

		// �ܺο� �Լ�
	public:
		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();

		void SetMainRenderTarget();

		ID3D11Device* GetDevice() { return Device.Get(); };
		ID3D11DeviceContext* GetContext() { return DeviceContext.Get(); };

		// ����Ʈ ����
		void SetViewPort(float left, float right, float top, float bottom);
		void SetViewPort_Size(float left, float right, float top, float bottom);

		// �������� ����
		void SetRasterizeState(unsigned int stateIndex);
		void SetDepthStencilState(unsigned int stateIndex);
		void SetBlendState(unsigned int stateIndex);

		bool GetFullScreenState() const;

		//void _DrawText(std::wstring_view text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float fontSize, float rot);

		bool ResizeSwapChain(unsigned int screenWidth, unsigned int screenHeight, unsigned int valueAA);

		ID3D11BlendState* GetAlphaBlendState() { return BlendState[BS_ALPHA_BLEND].Get(); };

		// ���ο� �Լ�
	private:
		// ����̽� ����
		//void SetupDeviceInfo(HWND hwnd, DWORD screenWidth, DWORD screenHeight, bool vsync, bool fullScreen,
		//	float screenDepth, float screenNear, DWORD valueAA, DWORD valueAF);
		// �����ڿ� ���޹��� ������� ����
		HRESULT GetVGAInfo();		// ���� ī�� ���� ȹ��
		HRESULT CreateRTView();		// ����Ÿ�� �� ����
		HRESULT CreateDSView();		// ����, ���ٽ� �� ����
		HRESULT SetViewport();		// ����Ʈ ����

		// �������°�ü ����
		HRESULT CreateStateObject();		// ������ ���°�ü ����
		HRESULT CreateDSState();			// �������ٽ� ���� ����
		HRESULT CreateRasterizeState();		// �����Ͷ����� ���� ����
		HRESULT CreateBlendState();			// ���� ���� ����

		// ����׿�
		bool SdkLayersAvailable();
		void CheckReferenceCountForDebug();

		// ����̽� �Ŵ�����
		ID3D11Device* GetD3D11Device() const { return Device.Get(); };
		ID3D11DeviceContext* GetD3D11Context() const { return DeviceContext.Get(); };

		// ScreenMode Check
		void CheckScreenState();
	};
}