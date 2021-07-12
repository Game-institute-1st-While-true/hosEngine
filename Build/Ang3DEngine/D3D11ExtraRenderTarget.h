// <Concept>
// Source Name		: D3D11ExtraRenderTarget.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.04.20

// <Explanation>
// D3D11�� �߰� ����Ÿ��.
// PostProcessing�� Ȱ���Ѵ�.

// Working Log
// 2021.04.20.Hosik	: ����

#pragma once

#include <d3d11_4.h>
#include <wrl.h>

namespace hos::cg
{

	class D3D11ExtraRenderTarget
	{
	public:
		enum class RenderTargetType { RGBA32_MAP, R32_MAP };

		// ��� �ν��Ͻ�
	private:
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureBuffer;
		Microsoft::WRL::ComPtr <ID3D11RenderTargetView> RenderTargetView;
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> ShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

		DXGI_FORMAT DXGIFormat;

		RenderTargetType Type;
		unsigned int Width;
		unsigned int Height;
		unsigned int ValueAA;

		// ������ �Ҹ���
	public:
		D3D11ExtraRenderTarget() = delete;
		D3D11ExtraRenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width = 512, unsigned int height = 512, RenderTargetType type = RenderTargetType::RGBA32_MAP, unsigned int valueAA = 1);
		~D3D11ExtraRenderTarget();

		// ���� �Լ�
	public:
		void ReadyToDraw(float r, float g, float b, float a);
		void SetRenderTarget();

		void ClearRenderTarget(float r, float g, float b, float a);
		void ClearDepthStencil(bool depthClear = true, float depthValue = 1.0f, bool stencilClear = true, unsigned char stencilValue = 0);

		ID3D11Texture2D* GetTexutreBuffer();
		ID3D11Texture2D* GetDepthStencilBuffer();
		ID3D11ShaderResourceView* GetShaderResourceView();

		void Resize(unsigned int screenWidth, unsigned int screenHeight, unsigned int valueAA);

		// ���� �Լ�
	private:
		HRESULT CreateTexture();
		HRESULT CreateRenderTargetView();
		HRESULT CreateShaderResourceView();
		HRESULT CreateDepthStencilView();

	};

}