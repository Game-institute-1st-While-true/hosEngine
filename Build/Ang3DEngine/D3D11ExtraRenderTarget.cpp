// ���� ���
#include "DebugConsole.h"

// �ڱ� ���
#include "D3D11ExtraRenderTarget.h"

// ���ӽ����̽�


hos::cg::D3D11ExtraRenderTarget::D3D11ExtraRenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height, RenderTargetType type, unsigned int valueAA)
	: DeviceRef(*device), ContextRef(*context), Width(width), Height(height), Type(type), ValueAA(valueAA)
{
	// 1.��� Type�� ���� DXGI_FORMAT ����
	switch (this->Type)
	{
	case RenderTargetType::RGBA32_MAP:
	default:
		this->DXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;

	case RenderTargetType::R32_MAP:
		this->DXGIFormat = DXGI_FORMAT_R32_FLOAT;
		break;
	}

	// 2.�ؽ��� ����
	if (FAILED(this->CreateTexture()))
	{
		throw;
	}

	// 3.����Ÿ�ٺ� ����
	if (FAILED(this->CreateRenderTargetView()))
	{
		throw;
	}

	// 4.ShaderResourceView ����
	if (FAILED(this->CreateShaderResourceView()))
	{
		throw;
	}

	// 5.DepthStencil Resource ����
	if(FAILED(this->CreateDepthStencilView()))
	{
		throw;
	}
}

hos::cg::D3D11ExtraRenderTarget::~D3D11ExtraRenderTarget()
{
}

void hos::cg::D3D11ExtraRenderTarget::ReadyToDraw(float r, float g, float b, float a)
{
	this->ContextRef.OMSetRenderTargets(1, this->RenderTargetView.GetAddressOf(), this->DepthStencilView.Get());	// �� ����Ÿ�ٺ�� ���� DS���� ����

	this->ClearRenderTarget(r, g, b, a);
}

void hos::cg::D3D11ExtraRenderTarget::SetRenderTarget()
{
	this->ContextRef.OMSetRenderTargets(1, this->RenderTargetView.GetAddressOf(), this->DepthStencilView.Get());	// �� ����Ÿ�ٺ�� ���� DS���� ����
}

void hos::cg::D3D11ExtraRenderTarget::ClearRenderTarget(float r, float g, float b, float a)
{
	// ���� �ʱ�ȭ ��
	float color[4] = { r, g, b, a };

	this->ContextRef.ClearRenderTargetView(this->RenderTargetView.Get(), color);	// ����Ÿ�� �ʱ�ȭ
	this->ClearDepthStencil();
}

void hos::cg::D3D11ExtraRenderTarget::ClearDepthStencil(bool depthClear, float depthValue, bool stencilClear, unsigned char stencilValue)
{
	UINT flag;

	// ����, ���ٽ� ��� Ŭ����
	if (depthClear && stencilClear)
	{
		flag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	}

	// ���ٽǸ�
	else if (depthClear && !stencilClear)
	{
		flag = D3D11_CLEAR_STENCIL;
	}

	// ������
	else
	{
		flag = D3D11_CLEAR_DEPTH;
	}

	this->ContextRef.ClearDepthStencilView(this->DepthStencilView.Get(), flag, depthValue, stencilValue);	// DS���� �ʱ�ȭ
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateTexture()
{
	HRESULT hr = S_OK;

	// 1.�ؽ�ó(2D) �Ӽ� ���� �� �ʱ�ȭ
	D3D11_TEXTURE2D_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(TexDesc));

	// 2.�Ӽ� ����
	TexDesc.Width = this->Width;				// ���� ũ��(�Ķ����)
	TexDesc.Height = this->Height;				// ���� ũ��(�Ķ����)
	TexDesc.MipLevels = 1;						// �Ӹ��� �ִ� ����			��(�˾ƺ��� ��ũ)
	TexDesc.ArraySize = 1;						// �ؽ��� �迭�� �ؽ��� ��	��(�˾ƺ��� ��ũ)
	TexDesc.Format = this->DXGIFormat;						// ����(�Ķ����)
	TexDesc.SampleDesc.Count = this->ValueAA;	// AA(��Ƽ�ٸ����)
	TexDesc.Usage = D3D11_USAGE_DEFAULT;		// �ؽ��� �а� ���� ���(�⺻)
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;	// �뵵(���� Ÿ�ٰ� ���̴����ҽ� �뵵)
	TexDesc.CPUAccessFlags = 0;					// CPU�� ������ ������� �ʴ´�.
	TexDesc.MiscFlags = 0;						// ���ҽ� �ɼ�(�ƹ��͵� ���� ����)

	// 3.�ؽ��� ����
	ID3D11Texture2D* pTex = NULL;
	hr = this->DeviceRef.CreateTexture2D(&TexDesc, NULL, TextureBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� Texture2D ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateRenderTargetView()
{
	// 1.����Ÿ�ٺ� �Ӽ� ���� �� �ʱ�ȭ
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(RTVDesc));

	// 2.�Ӽ� ����
	RTVDesc.Format = this->DXGIFormat;		// ����(�Ķ����): ����Ÿ���ؽ�ó�� �����ϰ�.
	RTVDesc.Texture2D.MipSlice = 0;			// 2D ����Ÿ�ٿ� �߰� �ɼ�: �Ӹ� ���ҿ� ��(�˾ƺ��� ��ũ)

	if (this->ValueAA == 1)
	{
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// 2D ����Ÿ��
	}
	
	else
	{
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;	// 2D ����Ÿ�� + AA
	}
	
	// 3.����Ÿ�ٺ� ����
	HRESULT hr = this->DeviceRef.CreateRenderTargetView(this->TextureBuffer.Get(), &RTVDesc, this->RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� RenderTargetView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateShaderResourceView()
{
	//���̴����ҽ��� ���� ����.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = this->DXGIFormat;							//�ؽ�ó�� ������������.
	sd.Texture2D.MipLevels = 1;								//2D SRV �߰� ���� : �Ӹ� ����.
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;		//2D SRV+AA �߰� ����

	if (this->ValueAA == 1)
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	}

	else
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;		//2D SRV + AA
	}

	//���̴����ҽ��� ����.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(this->TextureBuffer.Get(), &sd, this->ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� CreateShaderResourceView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// �ؽ���(����) ����
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = this->Width;
	td.Height = this->Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	td.SampleDesc.Count = this->ValueAA;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.MiscFlags = 0;

	hr = this->DeviceRef.CreateTexture2D(&td, NULL, this->DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� DepthStencilBuffer ������ �����߽��ϴ�.");
		return hr;
	}

	// ����/���ٽ� �� ����
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.Texture2D.MipSlice = 0;

	if (this->ValueAA == 1)
	{
		dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;			//2D (AA ����)
	}
	
	else
	{
		dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;			//2D (AA ����)
	}
	
	//����/���ٽ� �� ����.
	hr = this->DeviceRef.CreateDepthStencilView(this->DepthStencilBuffer.Get(), &dd, this->DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_ExtraRenderTarget�� CreateDepthStencilView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}

ID3D11Texture2D* hos::cg::D3D11ExtraRenderTarget::GetTexutreBuffer()
{
	return this->TextureBuffer.Get();
}

ID3D11Texture2D* hos::cg::D3D11ExtraRenderTarget::GetDepthStencilBuffer()
{
	return this->DepthStencilBuffer.Get();
}

ID3D11ShaderResourceView* hos::cg::D3D11ExtraRenderTarget::GetShaderResourceView()
{
	return this->ShaderResourceView.Get();
}

void hos::cg::D3D11ExtraRenderTarget::Resize(unsigned int screenWidth, unsigned int screenHeight, unsigned int valueAA)
{
	// D3D11 Resource Reset
	TextureBuffer.Reset();
	RenderTargetView.Reset();
	ShaderResourceView.Reset();
	DepthStencilBuffer.Reset();
	DepthStencilView.Reset();

	this->Width = screenWidth;
	this->Height = screenHeight;
	this->ValueAA = valueAA;

	// 1.��� Type�� ���� DXGI_FORMAT ����
	switch (this->Type)
	{
	case RenderTargetType::RGBA32_MAP:
	default:
		this->DXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;

	case RenderTargetType::R32_MAP:
		this->DXGIFormat = DXGI_FORMAT_R32_FLOAT;
		break;
	}

	// 2.�ؽ��� ����
	if (FAILED(this->CreateTexture()))
	{
		throw;
	}

	// 3.����Ÿ�ٺ� ����
	if (FAILED(this->CreateRenderTargetView()))
	{
		throw;
	}

	// 4.ShaderResourceView ����
	if (FAILED(this->CreateShaderResourceView()))
	{
		throw;
	}

	// 5.DepthStencil Resource ����
	if (FAILED(this->CreateDepthStencilView()))
	{
		throw;
	}
}
