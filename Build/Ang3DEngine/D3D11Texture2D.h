// <Concept>
// Source Name		: D3D11ExtraRenderTarget.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.04.20

// <Explanation>
// D3D11���� Ȱ���� Texture class.
// �ַ� Deferred �� PostProcessing ����� ������ �����̴�.

// Working Log
// 2021.05.03.Hosik	: ����

#pragma once

#include <d3d11_4.h>
#include <wrl.h>

namespace hos::cg
{

	class D3D11Texture2D
	{
		// ��� �ν��Ͻ�
	private:
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureBuffer;
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> ShaderResourceView;

		unsigned int Width;
		unsigned int Height;
		unsigned int ValueAA;

		// ������ �Ҹ���
	public:
		D3D11Texture2D() = delete;
		D3D11Texture2D(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width = 512, unsigned int height = 512, unsigned int valueAA = 1);
		~D3D11Texture2D();

		// ���� �Լ�
	public:
		void CopyTexture(ID3D11Texture2D* texture);
		ID3D11ShaderResourceView* GetSRV();

		// ���� �Լ�
	private:
		HRESULT CreateShaderResourceView(ID3D11Texture2D* texture);

	};

}