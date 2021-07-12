// ���� ���
#include "DebugConsole.h"

// �ڱ� ���
#include "D3D11ShadowBuffer.h"

// ���ӽ����̽�


hos::cg::D3D11ShadowBuffer::D3D11ShadowBuffer(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height)
	:DeviceRef(*device), ContextRef(*context), ViewPortInfo{0,}, Width((float)width), Height((float)height)
{
	// 0. Create Texture Buffer
	this->CreateTexture();

	// 1. Create DepthStencilView
	this->CreateDepthStencilView();

	// 2. Create ShaderResourceView
	this->CreateShaderResourceView();

	// 3. Create ViewPort Info
	this->CreateViewportInfo();
}

hos::cg::D3D11ShadowBuffer::~D3D11ShadowBuffer()
{
}

void hos::cg::D3D11ShadowBuffer::ReadyToDraw()
{
	// 0. Set ViewPort
	this->ContextRef.RSSetViewports(1, &(this->ViewPortInfo));

	// 1. Not Render. Just Write DSBuffer => Not Set RenderTarget
	ID3D11RenderTargetView* _nullRT[1] = { nullptr, };
	this->ContextRef.OMSetRenderTargets(1, _nullRT, this->DepthStencilView.Get());

	// 2. Clear DepthStencilView
	this->ContextRef.ClearDepthStencilView(this->DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

HRESULT hos::cg::D3D11ShadowBuffer::CreateTexture()
{
	HRESULT hr = S_OK;

	// 0.�ؽ�ó(2D) �Ӽ� ���� �� �ʱ�ȭ
	D3D11_TEXTURE2D_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(TexDesc));

	// 1.�Ӽ� ����
	TexDesc.Width = (UINT)this->Width;					// ���� ũ��(�Ķ����)
	TexDesc.Height = (UINT)this->Height;					// ���� ũ��(�Ķ����)
	TexDesc.MipLevels = 1;							// �Ӹ��� �ִ� ����			��(�˾ƺ��� ��ũ)
	TexDesc.ArraySize = 1;							// �ؽ��� �迭�� �ؽ��� ��	��(�˾ƺ��� ��ũ)
	TexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// ����(�Ķ����)
	TexDesc.SampleDesc.Count = 1;					// AA Sampling Ƚ��
	TexDesc.SampleDesc.Quality = 0;					// AA ����Ƽ
	TexDesc.Usage = D3D11_USAGE_DEFAULT;			// �ؽ��� �а� ���� ���(�⺻)
	TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
													// �뵵(���� ���ٽǰ� ���̴����ҽ� �뵵)
	TexDesc.CPUAccessFlags = 0;						// CPU�� ������ ������� �ʴ´�.
	TexDesc.MiscFlags = 0;							// ���ҽ� �ɼ�(�ƹ��͵� ���� ����)

	// 2.�ؽ��� ����
	ID3D11Texture2D* pTex = NULL;
	hr = this->DeviceRef.CreateTexture2D(&TexDesc, NULL, DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ShadowBuffer�� Texture2D ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ShadowBuffer::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// ����/���ٽ� �� �ɼ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Flags = 0;
	dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dd.Texture2D.MipSlice = 0;

	//����/���ٽ� �� ����.
	hr = this->DeviceRef.CreateDepthStencilView(this->DepthStencilBuffer.Get(), &dd, this->DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� CreateDepthStencilView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ShadowBuffer::CreateShaderResourceView()
{
	//���̴����ҽ��� ���� ����.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	sd.Texture2D.MipLevels = 1;								//2D SRV �߰� ���� : �Ӹ� ����.
	sd.Texture2D.MostDetailedMip = 0;

	//���̴����ҽ��� ����.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(this->DepthStencilBuffer.Get(), &sd, this->DepthStencilShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� CreateShaderResourceView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

void hos::cg::D3D11ShadowBuffer::CreateViewportInfo()
{
	this->ViewPortInfo.TopLeftX = 0.0f;
	this->ViewPortInfo.TopLeftY = 0.0f;
	this->ViewPortInfo.Width = (float)this->Width;
	this->ViewPortInfo.Height = (float)this->Height;
	this->ViewPortInfo.MinDepth = 0.0f;
	this->ViewPortInfo.MaxDepth = 1.0f;
}
