/// 시스템 헤더 ///
#include "GoodFunc.h"

/// DirectX 헤더 ///
// LINKING
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// INCLUDE
#include "dxgidebug.h"

/// 자기 헤더 ///
#include "D3D11.h"

using namespace hos::cg;

D3D11::D3D11(RendererDesc& desc)
	:RenderDesc(desc), NowDSState(DS_DEFAULT), NowRSState(RS_SOLID), NowBSState(BS_DEFAULT)
{
	// COM 라이브러리 초기화
	// TODO: 결과값에 따른 분기 처리
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// 초기화 전 잘못된 렌더 설정을 교정해줍니다.
	this->RenderDesc.CorrectDescValue();

	// VGA 정보 획득
	if (FAILED(GetVGAInfo()))
	{
		throw 1;
	}

	// 렌더타겟 뷰 생성
	if (FAILED(CreateRTView()))
	{
		throw 2;
	}

	// 깊이스텐실뷰 생성
	if (FAILED(CreateDSView()))
	{
		throw 3;
	}

	// 뷰포트 셋업
	if (FAILED(SetViewport()))
	{
		throw 4;
	}

	// 렌더링 상태 객체 생성
	if (FAILED(CreateStateObject()))
	{
		throw 5;
	}

	// 렌더링 대상 뷰와 깊이스텐실버퍼를 출력 렌더 파이프라인에 바인딩.
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

D3D11::~D3D11()
{
	// 종료 전 윈도우모드로 설정하지 않으면 스왑 체인을 해제할 때 예외 발생할 수 있음.
	if (SwapChain)
	{
		SwapChain->SetFullscreenState(false, NULL);
	}

	// COM 라이브러리 해제
	CoUninitialize();

	// COM객체 참조횟수 체크(for Debug)
#if defined(_DEBUG)||defined(DEBUG)
		//CheckReferenceCountForDebug();
#endif
}

void D3D11::BeginScene(float red, float green, float blue, float alpha)
{
	// 버퍼 초기화 색
	float color[4] = { red, green, blue, alpha };

	// 메인 렌더타겟 세팅
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	// 백버퍼 지우기
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), color);

	// 깊이 버퍼 지우기
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11::EndScene()
{
	// 렌더링 완료-> 백버퍼 표시
	if (RenderDesc.Vsync)
	{
		// 화면 새로 고침 비율 고정
		SwapChain->Present(1, 0);
	}

	else
	{
		// 최대한 빠르게 출력
		SwapChain->Present(0, 0);
	}
}

void hos::cg::D3D11::SetMainRenderTarget()
{
	// 메인 렌더타겟 세팅
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView.Get());
}

void hos::cg::D3D11::SetViewPort(float left, float right, float top, float bottom)
{
	unsigned int _screenWidth = this->RenderDesc.ScreenWidth;
	unsigned int _screenHeight = this->RenderDesc.ScreenHeight;

	D3D11_VIEWPORT vp;
	vp.TopLeftX = (FLOAT)(_screenWidth * left);
	vp.TopLeftY = (FLOAT)(_screenHeight * top);
	vp.Width = (FLOAT)(_screenWidth * right) - (FLOAT)(vp.TopLeftX);
	vp.Height = (FLOAT)(_screenHeight * bottom) - (FLOAT)(vp.TopLeftY);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	this->DeviceContext->RSSetViewports(1, &vp);
}

void hos::cg::D3D11::SetViewPort_Size(float left, float right, float top, float bottom)
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = (FLOAT)left;
	vp.TopLeftY = (FLOAT)top;
	vp.Width = (FLOAT)(right - left);
	vp.Height = (FLOAT)(bottom - top);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	this->DeviceContext->RSSetViewports(1, &vp);
}

void hos::cg::D3D11::SetRasterizeState(unsigned int stateIndex)
{
	//if (this->NowRSState != stateIndex)
	//{
	//	DeviceContext->RSSetState(RasterState[stateIndex].Get());
	//	this->NowRSState = stateIndex;
	//}
	DeviceContext->RSSetState(RasterState[stateIndex].Get());
}

void hos::cg::D3D11::SetDepthStencilState(unsigned int stateIndex)
{
	//if (this->NowDSState != stateIndex)
	//{
	//	DeviceContext->OMSetDepthStencilState(DepthStencilState[stateIndex].Get(), 1);
	//	this->NowDSState = stateIndex;
	//}
	DeviceContext->OMSetDepthStencilState(DepthStencilState[stateIndex].Get(), 1);
}

void hos::cg::D3D11::SetBlendState(unsigned int stateIndex)
{
	//if (this->NowBSState != stateIndex)
	//{
	//	DeviceContext->OMSetBlendState(BlendState[stateIndex].Get(), NULL, 0xFFFFFFFF);
	//	this->NowBSState = stateIndex;
	//}
	DeviceContext->OMSetBlendState(BlendState[stateIndex].Get(), NULL, 0xFFFFFFFF);
}

bool hos::cg::D3D11::GetFullScreenState() const
{
	BOOL bFull;
	this->SwapChain->GetFullscreenState(&bFull, NULL);

	return (bool)bFull;
}

bool hos::cg::D3D11::ResizeSwapChain(unsigned int screenWidth, unsigned int screenHeight, unsigned int valueAA)
{
	// 렌더타겟뷰 해제
	RenderTargetView.Reset();
	RenderTargetBuffer.Reset();

	// 뎁스 스텐실 뷰 해제
	DepthStencilView.Reset();
	DepthStencilBuffer.Reset();

	DeviceContext->Flush();

	HRESULT hr = SwapChain.Get()->ResizeBuffers(0, screenWidth, screenHeight, DXGI_FORMAT_UNKNOWN, 0);

	// 백버퍼 포인터를 얻어옵니다.
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)RenderTargetBuffer.GetAddressOf());

	// 백버퍼 포인터로 렌더 타겟 뷰를 생성한다.
	hr = Device->CreateRenderTargetView(RenderTargetBuffer.Get(), NULL, &RenderTargetView);

	this->CreateDSView();

	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	return false;
}

//void hos::cg::D3D11::_DrawText(std::wstring_view text, DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector4 color, float fontSize, float rot)
//{
//	SpriteFontList[0]->DrawString(this->SpriteFontBatch, text.data(), pos, color, rot, DirectX::SimpleMath::Vector2::Zero, DirectX::SimpleMath::Vector2(fontSize * 0.01f, fontSize * 0.01f));
//}

HRESULT D3D11::GetVGAInfo()
{
	// D3D 그래픽 인터페이스 팩토리 생성
	Microsoft::WRL::ComPtr <IDXGIFactory> pFactory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
		return S_FALSE;

	// 팩토리 객체로 그래픽 카드 인터페이스 어댑터를 생성
	Microsoft::WRL::ComPtr <IDXGIAdapter> pAdapter;
	if (FAILED(pFactory.Get()->EnumAdapters(0, pAdapter.GetAddressOf())))
		return S_FALSE;

	// 출력장치에 대한 어댑터 지정.
	Microsoft::WRL::ComPtr <IDXGIOutput> pAdapterOutput;
	if (FAILED(pAdapter.Get()->EnumOutputs(0, pAdapterOutput.GetAddressOf())))
		return S_FALSE;

	// 출력장치에 맞는 디스플레이 모드리스트를 획득합니다.
	UINT NumModes = 0;
	if (FAILED(pAdapterOutput.Get()->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, NULL)))
		return S_FALSE;

	// 출력장치와 그래픽카드의 모든 조합을 저장할 리스트를 생성합니다.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[NumModes];
	if (pDisplayModeList == nullptr)
		return S_FALSE;

	// 디스플레이 모드 리스트를 채웁니다.
	if (FAILED(pAdapterOutput.Get()->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, pDisplayModeList)))
		return S_FALSE;

	// 화면 상태에 맞는 디스플레이 모드를 찾습니다.
	// 적합 대상을 찾으면 출력장치 새로고침 비율의 분모/분자를 저장합니다.

	for (int i = 0; i < (int)NumModes; i++)
	{
		if (pDisplayModeList[i].Width == (UINT)RenderDesc.ScreenWidth)
		{
			if (pDisplayModeList[i].Height == (UINT)RenderDesc.ScreenHeight)
			{
				RenderDesc.Numerator = pDisplayModeList[i].RefreshRate.Numerator;
				RenderDesc.Denominator = pDisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// VGA의 구조체를 얻습니다.
	DXGI_ADAPTER_DESC AdapterDesc;
	if (FAILED(pAdapter.Get()->GetDesc(&AdapterDesc)))
		return S_FALSE;

	// 비디오카드 메모리 용량 단위를 메가바이트 단위로 저장합니다.
	RenderDesc.VideoCardMemory_MB = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오카드 이름을 저장합니다.
	/// 에러처리 ☆
	size_t StrLength = 0;
	//_tcscpy_s(VgaInfo.VideoCardName, AdapterDesc.Description);
	RenderDesc.VideoCardName.append(AdapterDesc.Description);

	// 디스플레이 모드 리스트 해제
	SAFE_DELARRY(pDisplayModeList);

	// 출력 어댑터 해제
	//SAFE_RELEASE(pAdapterOutput);

	// 어뎁터 해제
	//SAFE_RELEASE(pAdapter);

	// 팩토리 객체 해제
	//SAFE_RELEASE(pFactory);

	return S_OK;
}

HRESULT D3D11::CreateRTView()
{
	// 스왑체인 구조체 초기화
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// 백버퍼의 갯수는 1개입니다.
	SwapChainDesc.BufferCount = 1;

	// 백버퍼의 크기를 지정합니다.
	SwapChainDesc.BufferDesc.Width = RenderDesc.ScreenWidth;
	SwapChainDesc.BufferDesc.Height = RenderDesc.ScreenHeight;

	// 32bit의 서페이스를 설정합니다.
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 새로고침 비율을 설정합니다.
	if (RenderDesc.Vsync)
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = RenderDesc.Numerator;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = RenderDesc.Denominator;
	}

	else
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 사용 용도를 지정합니다.
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링이 사용될 윈도우 핸들을 지정합니다.
	SwapChainDesc.OutputWindow = RenderDesc.Hwnd;

	// 멀티샘플링을 끕니다.
	SwapChainDesc.SampleDesc.Count = RenderDesc.ValueAA;		// AA 설정
	SwapChainDesc.SampleDesc.Quality = 0;

	// 창모드 or 풀스크린 모드를 설정합니다.
	if (RenderDesc.Fullscreen)
		SwapChainDesc.Windowed = false;
	else
		SwapChainDesc.Windowed = true;

	// 스캔 라인 순서 및 크기를 지정하지 않음으로 설정합니다.
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 다음 백버퍼를 지우도록 설정합니다.
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그는 사용하지 않습니다.
	SwapChainDesc.Flags = 0;

	// 피쳐 레벨은 DirectX 11로 설정합니다.
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	if (SdkLayersAvailable())
	{
		// 프로젝트가 디버그 빌드 중인 경우에는 이 플래그가 있는 SDK 레이어를 통해 디버깅을 사용하십시오.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// 스왑 체인, D3D장치 및 컨텍스트를 만듭니다.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, &FeatureLevel, 1, D3D11_SDK_VERSION,
		&SwapChainDesc, &SwapChain, &Device, NULL, &DeviceContext)))
	{
		return S_FALSE;
	}

	// 백버퍼 포인터를 얻어옵니다.
	//ID3D11Texture2D* pBackBuffer = nullptr;
	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)RenderTargetBuffer.GetAddressOf())))
		return S_FALSE;

	// 백버퍼 포인터로 렌더 타겟 뷰를 생성한다.
	if (FAILED(Device->CreateRenderTargetView(RenderTargetBuffer.Get(), NULL, &RenderTargetView)))
		return S_FALSE;

	// 백버퍼 포인터를 해제한다.
	/// 렌더타겟의 백버퍼를 일단 보관해봅시다.
	//SAFE_RELEASE(pBackBuffer);

	return S_OK;
}

HRESULT D3D11::CreateDSView()
{
	// 깊이 버퍼 구조체를 초기화.
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// 깊이 버퍼 구조체를 작성합니다.
	DepthBufferDesc.Width = RenderDesc.ScreenWidth;
	DepthBufferDesc.Height = RenderDesc.ScreenHeight;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = RenderDesc.ValueAA;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	// 설정된 깊이버퍼 구조체로 깊이 버퍼 텍스쳐를 만듭니다.
	if (FAILED(Device->CreateTexture2D(&DepthBufferDesc, NULL, &DepthStencilBuffer)))
		return S_FALSE;

	// 깊이 스텐실뷰 구조체 초기화
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// 깊이 버퍼 구조체를 작성합니다.

	// 깊이 스텐실 뷰 구조체 설정
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// AA 여부에 따라 뷰디멘션 설정
	if (RenderDesc.ValueAA > 1)
	{
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	// AA 있음
	}

	else
	{
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// AA 없음
	}

	// 깊이 스텐실 뷰를 생성합니다.
	if (FAILED(Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &DepthStencilViewDesc, DepthStencilView.GetAddressOf())))
		return S_FALSE;

	return S_OK;
}

HRESULT D3D11::SetViewport()
{
	// 렌더링을 위한 뷰포트를 설정합니다.
	D3D11_VIEWPORT ViewPort;
	ViewPort.Width = (float)RenderDesc.ScreenWidth;
	ViewPort.Height = (float)RenderDesc.ScreenHeight;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.TopLeftX = 0.0f;
	ViewPort.TopLeftY = 0.0f;

	// 뷰포트 생성
	DeviceContext->RSSetViewports(1, &ViewPort);

	return S_OK;
}

HRESULT D3D11::CreateStateObject()
{
	if (FAILED(CreateDSState()))
	{
		throw 5;
	}

	if (FAILED(CreateRasterizeState()))
	{
		throw 6;
	}

	if (FAILED(CreateBlendState()))
	{
		throw 7;
	}

	return S_OK;
}

HRESULT D3D11::CreateDSState()
{
	// 뎁스 스텐실 상태 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// 뎁스 상태 구조체 작성
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// 스텐실 상태 구조체
	DepthStencilDesc.StencilEnable = FALSE;
	DepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// 픽셀 정면의 스텐실 설정
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	// 픽셀 후면의 스텐실도 동일합니다.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;

	//// 픽셀 후면의 스텐실 설정
	//DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// [0] 기본 깊이 스텐실 상태(뎁스ON, 스텐실OFF)
	if (FAILED(Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON])))
		return S_FALSE;

	// 기본 깊이 스텐실 상태 설정
	DeviceContext->OMSetDepthStencilState(DepthStencilState[DS_DEPTH_ON].Get(), 1);

	////////////////////////////////////////////////////
	// 추가 상태를 만듭니다.
	////////////////////////////////////////////////////

	// [1] 뎁스OFF, 스텐실OFF
	DepthStencilDesc.DepthEnable = FALSE;					//깊이 버퍼 On.
	DepthStencilDesc.StencilEnable = FALSE;					//스텐실 버퍼 Off!
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF]);

	// [2] 뎁스 Z쓰기만 끕니다.
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이값 쓰기 끔.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_WRITE_OFF]);

	//----------------------------------------------------------------------
	// 스텐실 버퍼 연산 객체들 생성.
	//----------------------------------------------------------------------
	// 스텐실 버퍼 비트 연산 공식.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : 현재 검사할 픽셀의 스텐실값.
	// *ComFunc : 비교 함수. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS 상태객체 #4 :  깊이버퍼 On, 스텐실버퍼 ON (항상, 참조값 쓰기) : "깊이/스텐실 기록" 
	DepthStencilDesc.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	//DepthStencilDesc.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	//DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_ON]);


	// DS 상태객체 #5 : 깊이버퍼 On, 스텐실버퍼 ON (동일비교, 성공시 유지) : "지정 위치에만 그리기" 
	//DepthStencilDesc.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS 상태객체 #6 : 깊이버퍼 On, 스텐실버퍼 ON (다름비교, 성공시 유지) : "지정 위치 이외에 그리기" 
	//DepthStencilDesc.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//비교함수 : "같이 않은가?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	// DS 상태객체 #7 : 깊이버퍼 Off, 스텐실버퍼 ON (참조값 쓰기) : "스텐실만 기록"
	DepthStencilDesc.DepthEnable = FALSE;										//깊이버퍼 OFF!
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF_STENCIL_ON]);


	// DS 상태객체 #8 : 깊이버퍼 On, 스텐실버퍼 ON (동일비교, 성시 증가) : "이중그리기 방지용"
	//DepthStencilDesc.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	//DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//DepthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?"
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//성공시 : 증가 (+1)
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_EQUAL_INCR]);


	// DS 상태객체 #9 : 깊이버퍼 On, 스텐실버퍼 ON (항상, 성공시 증가) : "스텐실만 기록" 
	DepthStencilDesc.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이버퍼 쓰기 OFF.
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;						//깊이연산 ON. (기본값)
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//깊이버퍼 Off (Write Off), 스텐실버퍼 ON (동일비교, 성공시 증가) : "이중그리기 방지.
	DepthStencilDesc.DepthEnable = FALSE;										//깊이테스트 Off!
	//DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이값 쓰기 Off.
	//DepthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//성공시 : 증가 (+1) 
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//뒷면 설정 동일.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF_STENCIL_EQUAL_INCR]);

	return S_OK;
}

HRESULT D3D11::CreateRasterizeState()
{
	HRESULT hr = S_OK;

	//[상태객체 1] 기본 렌더링 상태 개체.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//삼각형 색상 채우기.(기본값)
	rd.CullMode = D3D11_CULL_NONE;		//컬링 없음. (기본값은 컬링 Back)		
	rd.FrontCounterClockwise = false;   //이하 기본값...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA 적용.
	rd.AntialiasedLineEnable = false;

	// 기본 레스터라이져 상태 객체 생성.
	if (FAILED(Device->CreateRasterizerState(&rd, &RasterState[RS_SOLID])))
		return S_FALSE;

	// 래스터라이저 상태 세팅
	DeviceContext->RSSetState(RasterState[RS_SOLID].Get());

	/////////////////////////////////////////////////////
	//// 추가 상태객체 생성
	/////////////////////////////////////////////////////

	//[상태객체2] 와이어 프레임 그리기. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//레스터라이져 객체 생성.
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIREFRM]);

	//[상태객체3] 후면 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	Device->CreateRasterizerState(&rd, &RasterState[RS_CULLBACK]);

	//[상태객체4] 와이어 프레임 + 후면컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIRECULLBACK]);

	//[상태객체5] 정면 컬링 On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	Device->CreateRasterizerState(&rd, &RasterState[RS_CULLFRONT]);

	//[상태객체6] 와이어 프레임 + 정면컬링 On! "CW" 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_FRONT;
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIRECULLFRONT]);

	//[상태객체 7] UI 그리기
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = false;			/// 뎁스를 잘라내지 않아야 2.5D UI를 회전시 제대로 나옴
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;
	rd.AntialiasedLineEnable = false;
	Device->CreateRasterizerState(&rd, &RasterState[RS_NODEPTH]);

	return hr;
}

HRESULT D3D11::CreateBlendState()
{
	HRESULT hr = S_OK;

	//------------------------------
	// 혼합객체1 : 색상혼합 없음. "Blending OFF"
	//------------------------------
	//블렌딩 상태 객체 구성 옵션 : 이하 기본값들.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//블렌딩 동작 결정. 기본값은 FALSE(OFF)		
	//색상 성분 혼합 : Color Blending.(기본값)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산(Color - Operation), 기본값은 덧셈 : 최종색 = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//타겟(대상) 혼합 비율, 대상 0%   : Dest.Color = Dest * 0;  통상 0번 RT 는 "백버퍼"를 의미합니다.	
	//알파 성분 혼합 : Alpha Blending.(기본값)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//알파 혼합 함수(Alpha - Opertion), 기본값은 덧셈.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//소스(원본) 알파 혼합 비율.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//타겟(대상) 알파 혼합 비율.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//렌더타겟에 쓰기 옵션.
	//bd.AlphaToCoverageEnable = FALSE;								//이하 추가 옵션.(기본값, 생략)
	//bd.IndependentBlendEnable = FALSE;

	// 기본 상태 개체.생성.
	if (FAILED(Device->CreateBlendState(&bd, &BlendState[BS_DEFAULT])))
		return S_FALSE;

	// 기본 블렌드 상태 세팅
	DeviceContext->OMSetBlendState(BlendState[BS_DEFAULT].Get(), NULL, 0xFFFFFFFF);

	//------------------------------
	// 혼합객체2 : 알파 혼합 "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//소스(원본) 혼합 비율, 원본 알파비율  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//타겟(대상) 혼합 비율, 원본 알파비율 반전 : Dest.Color = Dest * (1-Src.a);			 		
	Device->CreateBlendState(&bd, &BlendState[BS_ALPHA_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * (1-Src.Color);			 	
	Device->CreateBlendState(&bd, &BlendState[BS_COLOR_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "밝기 강조 Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 "밝기강조" : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * 1;			 	
	Device->CreateBlendState(&bd, &BlendState[BS_COLOR_BLEND_ONE]);

	return hr;
}

bool D3D11::SdkLayersAvailable()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // 실제 하드웨어 디바이스를 만들 필요가 없습니다.
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // SDK 레이어를 확인하세요.
		nullptr,                    // 모든 기능 수준이 적용됩니다.
		0,
		D3D11_SDK_VERSION,          // Microsoft Store 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
		nullptr,                    // D3D 디바이스 참조를 보관할 필요가 없습니다.
		nullptr,                    // 기능 수준을 알 필요가 없습니다.
		nullptr                     // D3D 디바이스 컨텍스트 참조를 보관할 필요가 없습니다.
	);

	return SUCCEEDED(hr);
}

// DirectX COM객체의 참조횟수가 모두 해제되었는지 체크합니다.
void D3D11::CheckReferenceCountForDebug()
{
#ifdef DEBUG
	IDXGIDebug* debug;
	DXGIGetDebugInterface1(0, IID_IDXGIDebug, (void**)&debug);
	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	SAFE_RELEASE(debug);
#endif // DEBUG
}

void hos::cg::D3D11::CheckScreenState()
{
	bool bFull = this->GetFullScreenState();
	if (this->RenderDesc.Fullscreen != bFull)
	{
		this->RenderDesc.Fullscreen = bFull;
	}
}