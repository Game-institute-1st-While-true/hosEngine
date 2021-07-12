// <Concept>
// Source Name		: D3D11ShadowBuffer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.06.09

// <Explanation>
// D3D11�� ShadowBuffer Ŭ����.
// ���� ExtraRenderTarget�� �ٸ��� DepthStencil Buffer�� ShaderResourceView�� Ȱ���ؾ� �ϱ� ������ DXGI_FORMAT�� �޶����� ��
// �� ���� �������� ���� ���� Ŭ������ ��ȭ�ߴ�.

// Working Log
// 2021.06.09.Hosik	: ����

#pragma once

#include <d3d11_4.h>
#include <wrl.h>

namespace hos::cg
{

	class D3D11ShadowBuffer
	{
		// ��� �ν��Ͻ�
	private:
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> DepthStencilShaderResourceView;

		D3D11_VIEWPORT ViewPortInfo;
		float Width;
		float Height;

		// ������ �Ҹ���
	public:
		D3D11ShadowBuffer() = delete;
		D3D11ShadowBuffer(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width = 512, unsigned int height = 512);
		~D3D11ShadowBuffer();

		// ���� �Լ�
	public:
		void ReadyToDraw();
		//void SetRenderTarget();

		//void ClearRenderTarget(float r, float g, float b, float a);
		//void ClearDepthStencil(bool depthClear = true, float depthValue = 1.0f, bool stencilClear = true, unsigned char stencilValue = 0);

		//ID3D11Texture2D* GetTexutreBuffer();
		//ID3D11Texture2D* GetDepthStencilBuffer();
		ID3D11ShaderResourceView* GetShaderResourceView() { return this->DepthStencilShaderResourceView.Get(); };

		// ���� �Լ�
	private:
		HRESULT CreateTexture();
		HRESULT CreateDepthStencilView();
		HRESULT CreateShaderResourceView();
		void CreateViewportInfo();
		

	};

}