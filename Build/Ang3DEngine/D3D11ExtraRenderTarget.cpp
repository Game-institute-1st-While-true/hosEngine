// 참조 헤더
#include "DebugConsole.h"

// 자기 헤더
#include "D3D11ExtraRenderTarget.h"

// 네임스페이스


hos::cg::D3D11ExtraRenderTarget::D3D11ExtraRenderTarget(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height, RenderTargetType type, unsigned int valueAA)
	: DeviceRef(*device), ContextRef(*context), Width(width), Height(height), Type(type), ValueAA(valueAA)
{
	// 1.사용 Type에 따라 DXGI_FORMAT 설정
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

	// 2.텍스쳐 생성
	if (FAILED(this->CreateTexture()))
	{
		throw;
	}

	// 3.렌더타겟뷰 생성
	if (FAILED(this->CreateRenderTargetView()))
	{
		throw;
	}

	// 4.ShaderResourceView 생성
	if (FAILED(this->CreateShaderResourceView()))
	{
		throw;
	}

	// 5.DepthStencil Resource 생성
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
	this->ContextRef.OMSetRenderTargets(1, this->RenderTargetView.GetAddressOf(), this->DepthStencilView.Get());	// 새 렌더타겟뷰와 기존 DS버퍼 세팅

	this->ClearRenderTarget(r, g, b, a);
}

void hos::cg::D3D11ExtraRenderTarget::SetRenderTarget()
{
	this->ContextRef.OMSetRenderTargets(1, this->RenderTargetView.GetAddressOf(), this->DepthStencilView.Get());	// 새 렌더타겟뷰와 기존 DS버퍼 세팅
}

void hos::cg::D3D11ExtraRenderTarget::ClearRenderTarget(float r, float g, float b, float a)
{
	// 버퍼 초기화 색
	float color[4] = { r, g, b, a };

	this->ContextRef.ClearRenderTargetView(this->RenderTargetView.Get(), color);	// 렌더타겟 초기화
	this->ClearDepthStencil();
}

void hos::cg::D3D11ExtraRenderTarget::ClearDepthStencil(bool depthClear, float depthValue, bool stencilClear, unsigned char stencilValue)
{
	UINT flag;

	// 뎁스, 스텐실 모두 클리어
	if (depthClear && stencilClear)
	{
		flag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	}

	// 스텐실만
	else if (depthClear && !stencilClear)
	{
		flag = D3D11_CLEAR_STENCIL;
	}

	// 뎁스만
	else
	{
		flag = D3D11_CLEAR_DEPTH;
	}

	this->ContextRef.ClearDepthStencilView(this->DepthStencilView.Get(), flag, depthValue, stencilValue);	// DS버퍼 초기화
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateTexture()
{
	HRESULT hr = S_OK;

	// 1.텍스처(2D) 속성 선언 및 초기화
	D3D11_TEXTURE2D_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(TexDesc));

	// 2.속성 세팅
	TexDesc.Width = this->Width;				// 가로 크기(파라미터)
	TexDesc.Height = this->Height;				// 세로 크기(파라미터)
	TexDesc.MipLevels = 1;						// 밉맵의 최대 레벨			♣(알아보자 마크)
	TexDesc.ArraySize = 1;						// 텍스쳐 배열의 텍스쳐 수	♣(알아보자 마크)
	TexDesc.Format = this->DXGIFormat;						// 형식(파라미터)
	TexDesc.SampleDesc.Count = this->ValueAA;	// AA(안티앨리어싱)
	TexDesc.Usage = D3D11_USAGE_DEFAULT;		// 텍스쳐 읽고 쓰는 방식(기본)
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;	// 용도(렌더 타겟과 쉐이더리소스 용도)
	TexDesc.CPUAccessFlags = 0;					// CPU의 접근은 허용하지 않는다.
	TexDesc.MiscFlags = 0;						// 리소스 옵션(아무것도 쓰지 않음)

	// 3.텍스쳐 생성
	ID3D11Texture2D* pTex = NULL;
	hr = this->DeviceRef.CreateTexture2D(&TexDesc, NULL, TextureBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 Texture2D 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateRenderTargetView()
{
	// 1.렌더타겟뷰 속성 선언 및 초기화
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(RTVDesc));

	// 2.속성 세팅
	RTVDesc.Format = this->DXGIFormat;		// 형식(파라미터): 렌더타겟텍스처와 동일하게.
	RTVDesc.Texture2D.MipSlice = 0;			// 2D 렌더타겟용 추가 옵션: 밉맵 분할용 ♣(알아보자 마크)

	if (this->ValueAA == 1)
	{
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		// 2D 렌더타겟
	}
	
	else
	{
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;	// 2D 렌더타겟 + AA
	}
	
	// 3.렌더타겟뷰 생성
	HRESULT hr = this->DeviceRef.CreateRenderTargetView(this->TextureBuffer.Get(), &RTVDesc, this->RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 RenderTargetView 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateShaderResourceView()
{
	//셰이더리소스뷰 정보 구성.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = this->DXGIFormat;							//텍스처와 동일포멧유지.
	sd.Texture2D.MipLevels = 1;								//2D SRV 추가 설정 : 밉멥 설정.
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;		//2D SRV+AA 추가 설정

	if (this->ValueAA == 1)
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	}

	else
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;		//2D SRV + AA
	}

	//셰이더리소스뷰 생성.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(this->TextureBuffer.Get(), &sd, this->ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 CreateShaderResourceView 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11ExtraRenderTarget::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// 텍스쳐(버퍼) 생성
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
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 DepthStencilBuffer 생성에 실패했습니다.");
		return hr;
	}

	// 깊이/스텐실 뷰 생성
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.Texture2D.MipSlice = 0;

	if (this->ValueAA == 1)
	{
		dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;			//2D (AA 없음)
	}
	
	else
	{
		dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;			//2D (AA 적용)
	}
	
	//깊이/스텐실 뷰 생성.
	hr = this->DeviceRef.CreateDepthStencilView(this->DepthStencilBuffer.Get(), &dd, this->DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_ExtraRenderTarget의 CreateDepthStencilView 생성에 실패했습니다.");
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

	// 1.사용 Type에 따라 DXGI_FORMAT 설정
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

	// 2.텍스쳐 생성
	if (FAILED(this->CreateTexture()))
	{
		throw;
	}

	// 3.렌더타겟뷰 생성
	if (FAILED(this->CreateRenderTargetView()))
	{
		throw;
	}

	// 4.ShaderResourceView 생성
	if (FAILED(this->CreateShaderResourceView()))
	{
		throw;
	}

	// 5.DepthStencil Resource 생성
	if (FAILED(this->CreateDepthStencilView()))
	{
		throw;
	}
}
