// 참조 헤더
#include "DebugConsole.h"

// 자기 헤더
#include "D3D11ShadowBuffer.h"

// 네임스페이스


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

	// 0.텍스처(2D) 속성 선언 및 초기화
	D3D11_TEXTURE2D_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(TexDesc));

	// 1.속성 세팅
	TexDesc.Width = (UINT)this->Width;					// 가로 크기(파라미터)
	TexDesc.Height = (UINT)this->Height;					// 세로 크기(파라미터)
	TexDesc.MipLevels = 1;							// 밉맵의 최대 레벨			♣(알아보자 마크)
	TexDesc.ArraySize = 1;							// 텍스쳐 배열의 텍스쳐 수	♣(알아보자 마크)
	TexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// 형식(파라미터)
	TexDesc.SampleDesc.Count = 1;					// AA Sampling 횟수
	TexDesc.SampleDesc.Quality = 0;					// AA 퀄리티
	TexDesc.Usage = D3D11_USAGE_DEFAULT;			// 텍스쳐 읽고 쓰는 방식(기본)
	TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
													// 용도(뎁스 스텐실과 쉐이더리소스 용도)
	TexDesc.CPUAccessFlags = 0;						// CPU의 접근은 허용하지 않는다.
	TexDesc.MiscFlags = 0;							// 리소스 옵션(아무것도 쓰지 않음)

	// 2.텍스쳐 생성
	ID3D11Texture2D* pTex = NULL;
	hr = this->DeviceRef.CreateTexture2D(&TexDesc, NULL, DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ShadowBuffer의 Texture2D 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ShadowBuffer::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// 깊이/스텐실 뷰 옵션
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Flags = 0;
	dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dd.Texture2D.MipSlice = 0;

	//깊이/스텐실 뷰 생성.
	hr = this->DeviceRef.CreateDepthStencilView(this->DepthStencilBuffer.Get(), &dd, this->DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 CreateDepthStencilView 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ShadowBuffer::CreateShaderResourceView()
{
	//셰이더리소스뷰 정보 구성.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	sd.Texture2D.MipLevels = 1;								//2D SRV 추가 설정 : 밉멥 설정.
	sd.Texture2D.MostDetailedMip = 0;

	//셰이더리소스뷰 생성.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(this->DepthStencilBuffer.Get(), &sd, this->DepthStencilShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 CreateShaderResourceView 생성에 실패했습니다.");
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
