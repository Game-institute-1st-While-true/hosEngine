/// �ý��� ��� ///
#include "GoodFunc.h"

/// DirectX ��� ///
// LINKING
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// INCLUDE
#include "dxgidebug.h"

/// �ڱ� ��� ///
#include "D3D11.h"

using namespace hos::cg;

D3D11::D3D11(RendererDesc& desc)
	:RenderDesc(desc), NowDSState(DS_DEFAULT), NowRSState(RS_SOLID), NowBSState(BS_DEFAULT)
{
	// COM ���̺귯�� �ʱ�ȭ
	// TODO: ������� ���� �б� ó��
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// �ʱ�ȭ �� �߸��� ���� ������ �������ݴϴ�.
	this->RenderDesc.CorrectDescValue();

	// VGA ���� ȹ��
	if (FAILED(GetVGAInfo()))
	{
		throw 1;
	}

	// ����Ÿ�� �� ����
	if (FAILED(CreateRTView()))
	{
		throw 2;
	}

	// ���̽��ٽǺ� ����
	if (FAILED(CreateDSView()))
	{
		throw 3;
	}

	// ����Ʈ �¾�
	if (FAILED(SetViewport()))
	{
		throw 4;
	}

	// ������ ���� ��ü ����
	if (FAILED(CreateStateObject()))
	{
		throw 5;
	}

	// ������ ��� ��� ���̽��ٽǹ��۸� ��� ���� ���������ο� ���ε�.
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

D3D11::~D3D11()
{
	// ���� �� ��������� �������� ������ ���� ü���� ������ �� ���� �߻��� �� ����.
	if (SwapChain)
	{
		SwapChain->SetFullscreenState(false, NULL);
	}

	// COM ���̺귯�� ����
	CoUninitialize();

	// COM��ü ����Ƚ�� üũ(for Debug)
#if defined(_DEBUG)||defined(DEBUG)
		//CheckReferenceCountForDebug();
#endif
}

void D3D11::BeginScene(float red, float green, float blue, float alpha)
{
	// ���� �ʱ�ȭ ��
	float color[4] = { red, green, blue, alpha };

	// ���� ����Ÿ�� ����
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	// ����� �����
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), color);

	// ���� ���� �����
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11::EndScene()
{
	// ������ �Ϸ�-> ����� ǥ��
	if (RenderDesc.Vsync)
	{
		// ȭ�� ���� ��ħ ���� ����
		SwapChain->Present(1, 0);
	}

	else
	{
		// �ִ��� ������ ���
		SwapChain->Present(0, 0);
	}
}

void hos::cg::D3D11::SetMainRenderTarget()
{
	// ���� ����Ÿ�� ����
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
	// ����Ÿ�ٺ� ����
	RenderTargetView.Reset();
	RenderTargetBuffer.Reset();

	// ���� ���ٽ� �� ����
	DepthStencilView.Reset();
	DepthStencilBuffer.Reset();

	DeviceContext->Flush();

	HRESULT hr = SwapChain.Get()->ResizeBuffers(0, screenWidth, screenHeight, DXGI_FORMAT_UNKNOWN, 0);

	// ����� �����͸� ���ɴϴ�.
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)RenderTargetBuffer.GetAddressOf());

	// ����� �����ͷ� ���� Ÿ�� �並 �����Ѵ�.
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
	// D3D �׷��� �������̽� ���丮 ����
	Microsoft::WRL::ComPtr <IDXGIFactory> pFactory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
		return S_FALSE;

	// ���丮 ��ü�� �׷��� ī�� �������̽� ����͸� ����
	Microsoft::WRL::ComPtr <IDXGIAdapter> pAdapter;
	if (FAILED(pFactory.Get()->EnumAdapters(0, pAdapter.GetAddressOf())))
		return S_FALSE;

	// �����ġ�� ���� ����� ����.
	Microsoft::WRL::ComPtr <IDXGIOutput> pAdapterOutput;
	if (FAILED(pAdapter.Get()->EnumOutputs(0, pAdapterOutput.GetAddressOf())))
		return S_FALSE;

	// �����ġ�� �´� ���÷��� ��帮��Ʈ�� ȹ���մϴ�.
	UINT NumModes = 0;
	if (FAILED(pAdapterOutput.Get()->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, NULL)))
		return S_FALSE;

	// �����ġ�� �׷���ī���� ��� ������ ������ ����Ʈ�� �����մϴ�.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[NumModes];
	if (pDisplayModeList == nullptr)
		return S_FALSE;

	// ���÷��� ��� ����Ʈ�� ä��ϴ�.
	if (FAILED(pAdapterOutput.Get()->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, pDisplayModeList)))
		return S_FALSE;

	// ȭ�� ���¿� �´� ���÷��� ��带 ã���ϴ�.
	// ���� ����� ã���� �����ġ ���ΰ�ħ ������ �и�/���ڸ� �����մϴ�.

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

	// VGA�� ����ü�� ����ϴ�.
	DXGI_ADAPTER_DESC AdapterDesc;
	if (FAILED(pAdapter.Get()->GetDesc(&AdapterDesc)))
		return S_FALSE;

	// ����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ �����մϴ�.
	RenderDesc.VideoCardMemory_MB = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ����ī�� �̸��� �����մϴ�.
	/// ����ó�� ��
	size_t StrLength = 0;
	//_tcscpy_s(VgaInfo.VideoCardName, AdapterDesc.Description);
	RenderDesc.VideoCardName.append(AdapterDesc.Description);

	// ���÷��� ��� ����Ʈ ����
	SAFE_DELARRY(pDisplayModeList);

	// ��� ����� ����
	//SAFE_RELEASE(pAdapterOutput);

	// ��� ����
	//SAFE_RELEASE(pAdapter);

	// ���丮 ��ü ����
	//SAFE_RELEASE(pFactory);

	return S_OK;
}

HRESULT D3D11::CreateRTView()
{
	// ����ü�� ����ü �ʱ�ȭ
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// ������� ������ 1���Դϴ�.
	SwapChainDesc.BufferCount = 1;

	// ������� ũ�⸦ �����մϴ�.
	SwapChainDesc.BufferDesc.Width = RenderDesc.ScreenWidth;
	SwapChainDesc.BufferDesc.Height = RenderDesc.ScreenHeight;

	// 32bit�� �����̽��� �����մϴ�.
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ������ �����մϴ�.
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

	// ������� ��� �뵵�� �����մϴ�.
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� ���� ������ �ڵ��� �����մϴ�.
	SwapChainDesc.OutputWindow = RenderDesc.Hwnd;

	// ��Ƽ���ø��� ���ϴ�.
	SwapChainDesc.SampleDesc.Count = RenderDesc.ValueAA;		// AA ����
	SwapChainDesc.SampleDesc.Quality = 0;

	// â��� or Ǯ��ũ�� ��带 �����մϴ�.
	if (RenderDesc.Fullscreen)
		SwapChainDesc.Windowed = false;
	else
		SwapChainDesc.Windowed = true;

	// ��ĵ ���� ���� �� ũ�⸦ �������� �������� �����մϴ�.
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ���� ����۸� ���쵵�� �����մϴ�.
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��״� ������� �ʽ��ϴ�.
	SwapChainDesc.Flags = 0;

	// ���� ������ DirectX 11�� �����մϴ�.
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	if (SdkLayersAvailable())
	{
		// ������Ʈ�� ����� ���� ���� ��쿡�� �� �÷��װ� �ִ� SDK ���̾ ���� ������� ����Ͻʽÿ�.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// ���� ü��, D3D��ġ �� ���ؽ�Ʈ�� ����ϴ�.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, &FeatureLevel, 1, D3D11_SDK_VERSION,
		&SwapChainDesc, &SwapChain, &Device, NULL, &DeviceContext)))
	{
		return S_FALSE;
	}

	// ����� �����͸� ���ɴϴ�.
	//ID3D11Texture2D* pBackBuffer = nullptr;
	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)RenderTargetBuffer.GetAddressOf())))
		return S_FALSE;

	// ����� �����ͷ� ���� Ÿ�� �並 �����Ѵ�.
	if (FAILED(Device->CreateRenderTargetView(RenderTargetBuffer.Get(), NULL, &RenderTargetView)))
		return S_FALSE;

	// ����� �����͸� �����Ѵ�.
	/// ����Ÿ���� ����۸� �ϴ� �����غ��ô�.
	//SAFE_RELEASE(pBackBuffer);

	return S_OK;
}

HRESULT D3D11::CreateDSView()
{
	// ���� ���� ����ü�� �ʱ�ȭ.
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// ���� ���� ����ü�� �ۼ��մϴ�.
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

	// ������ ���̹��� ����ü�� ���� ���� �ؽ��ĸ� ����ϴ�.
	if (FAILED(Device->CreateTexture2D(&DepthBufferDesc, NULL, &DepthStencilBuffer)))
		return S_FALSE;

	// ���� ���ٽǺ� ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// ���� ���� ����ü�� �ۼ��մϴ�.

	// ���� ���ٽ� �� ����ü ����
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// AA ���ο� ���� ����� ����
	if (RenderDesc.ValueAA > 1)
	{
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	// AA ����
	}

	else
	{
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// AA ����
	}

	// ���� ���ٽ� �並 �����մϴ�.
	if (FAILED(Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &DepthStencilViewDesc, DepthStencilView.GetAddressOf())))
		return S_FALSE;

	return S_OK;
}

HRESULT D3D11::SetViewport()
{
	// �������� ���� ����Ʈ�� �����մϴ�.
	D3D11_VIEWPORT ViewPort;
	ViewPort.Width = (float)RenderDesc.ScreenWidth;
	ViewPort.Height = (float)RenderDesc.ScreenHeight;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.TopLeftX = 0.0f;
	ViewPort.TopLeftY = 0.0f;

	// ����Ʈ ����
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
	// ���� ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// ���� ���� ����ü �ۼ�
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// ���ٽ� ���� ����ü
	DepthStencilDesc.StencilEnable = FALSE;
	DepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// �ȼ� ������ ���ٽ� ����
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	// �ȼ� �ĸ��� ���ٽǵ� �����մϴ�.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;

	//// �ȼ� �ĸ��� ���ٽ� ����
	//DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// [0] �⺻ ���� ���ٽ� ����(����ON, ���ٽ�OFF)
	if (FAILED(Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON])))
		return S_FALSE;

	// �⺻ ���� ���ٽ� ���� ����
	DeviceContext->OMSetDepthStencilState(DepthStencilState[DS_DEPTH_ON].Get(), 1);

	////////////////////////////////////////////////////
	// �߰� ���¸� ����ϴ�.
	////////////////////////////////////////////////////

	// [1] ����OFF, ���ٽ�OFF
	DepthStencilDesc.DepthEnable = FALSE;					//���� ���� On.
	DepthStencilDesc.StencilEnable = FALSE;					//���ٽ� ���� Off!
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF]);

	// [2] ���� Z���⸸ ���ϴ�.
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//���̰� ���� ��.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_WRITE_OFF]);

	//----------------------------------------------------------------------
	// ���ٽ� ���� ���� ��ü�� ����.
	//----------------------------------------------------------------------
	// ���ٽ� ���� ��Ʈ ���� ����.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : ���� �˻��� �ȼ��� ���ٽǰ�.
	// *ComFunc : �� �Լ�. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS ���°�ü #4 :  ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "����/���ٽ� ���" 
	DepthStencilDesc.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	//DepthStencilDesc.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	//DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_ON]);


	// DS ���°�ü #5 : ���̹��� On, ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���� ��ġ���� �׸���" 
	//DepthStencilDesc.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS ���°�ü #6 : ���̹��� On, ���ٽǹ��� ON (�ٸ���, ������ ����) : "���� ��ġ �̿ܿ� �׸���" 
	//DepthStencilDesc.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//���Լ� : "���� ������?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	// DS ���°�ü #7 : ���̹��� Off, ���ٽǹ��� ON (������ ����) : "���ٽǸ� ���"
	DepthStencilDesc.DepthEnable = FALSE;										//���̹��� OFF!
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF_STENCIL_ON]);


	// DS ���°�ü #8 : ���̹��� On, ���ٽǹ��� ON (���Ϻ�, ���� ����) : "���߱׸��� ������"
	//DepthStencilDesc.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	//DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//DepthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON!
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?"
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//������ : ���� (+1)
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_ON_STENCIL_EQUAL_INCR]);


	// DS ���°�ü #9 : ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "���ٽǸ� ���" 
	DepthStencilDesc.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̹��� ���� OFF.
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;						//���̿��� ON. (�⺻��)
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//���̹��� Off (Write Off), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����.
	DepthStencilDesc.DepthEnable = FALSE;										//�����׽�Ʈ Off!
	//DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̰� ���� Off.
	//DepthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?" 
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;		//������ : ���� (+1) 
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;										//�޸� ���� ����.
	Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState[DS_DEPTH_OFF_STENCIL_EQUAL_INCR]);

	return S_OK;
}

HRESULT D3D11::CreateRasterizeState()
{
	HRESULT hr = S_OK;

	//[���°�ü 1] �⺻ ������ ���� ��ü.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//�ﰢ�� ���� ä���.(�⺻��)
	rd.CullMode = D3D11_CULL_NONE;		//�ø� ����. (�⺻���� �ø� Back)		
	rd.FrontCounterClockwise = false;   //���� �⺻��...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA ����.
	rd.AntialiasedLineEnable = false;

	// �⺻ �����Ͷ����� ���� ��ü ����.
	if (FAILED(Device->CreateRasterizerState(&rd, &RasterState[RS_SOLID])))
		return S_FALSE;

	// �����Ͷ����� ���� ����
	DeviceContext->RSSetState(RasterState[RS_SOLID].Get());

	/////////////////////////////////////////////////////
	//// �߰� ���°�ü ����
	/////////////////////////////////////////////////////

	//[���°�ü2] ���̾� ������ �׸���. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//�����Ͷ����� ��ü ����.
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIREFRM]);

	//[���°�ü3] �ĸ� �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	Device->CreateRasterizerState(&rd, &RasterState[RS_CULLBACK]);

	//[���°�ü4] ���̾� ������ + �ĸ��ø� On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIRECULLBACK]);

	//[���°�ü5] ���� �ø� On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	Device->CreateRasterizerState(&rd, &RasterState[RS_CULLFRONT]);

	//[���°�ü6] ���̾� ������ + �����ø� On! "CW" 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_FRONT;
	Device->CreateRasterizerState(&rd, &RasterState[RS_WIRECULLFRONT]);

	//[���°�ü 7] UI �׸���
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = false;			/// ������ �߶��� �ʾƾ� 2.5D UI�� ȸ���� ����� ����
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
	// ȥ�հ�ü1 : ����ȥ�� ����. "Blending OFF"
	//------------------------------
	//���� ���� ��ü ���� �ɼ� : ���� �⺻����.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//���� ���� ����. �⺻���� FALSE(OFF)		
	//���� ���� ȥ�� : Color Blending.(�⺻��)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ����(Color - Operation), �⺻���� ���� : ������ = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//Ÿ��(���) ȥ�� ����, ��� 0%   : Dest.Color = Dest * 0;  ��� 0�� RT �� "�����"�� �ǹ��մϴ�.	
	//���� ���� ȥ�� : Alpha Blending.(�⺻��)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//���� ȥ�� �Լ�(Alpha - Opertion), �⺻���� ����.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//�ҽ�(����) ���� ȥ�� ����.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//Ÿ��(���) ���� ȥ�� ����.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//����Ÿ�ٿ� ���� �ɼ�.
	//bd.AlphaToCoverageEnable = FALSE;								//���� �߰� �ɼ�.(�⺻��, ����)
	//bd.IndependentBlendEnable = FALSE;

	// �⺻ ���� ��ü.����.
	if (FAILED(Device->CreateBlendState(&bd, &BlendState[BS_DEFAULT])))
		return S_FALSE;

	// �⺻ ���� ���� ����
	DeviceContext->OMSetBlendState(BlendState[BS_DEFAULT].Get(), NULL, 0xFFFFFFFF);

	//------------------------------
	// ȥ�հ�ü2 : ���� ȥ�� "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//�ҽ�(����) ȥ�� ����, ���� ���ĺ���  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//Ÿ��(���) ȥ�� ����, ���� ���ĺ��� ���� : Dest.Color = Dest * (1-Src.a);			 		
	Device->CreateBlendState(&bd, &BlendState[BS_ALPHA_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * (1-Src.Color);			 	
	Device->CreateBlendState(&bd, &BlendState[BS_COLOR_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "��� ���� Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� "��Ⱝ��" : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * 1;			 	
	Device->CreateBlendState(&bd, &BlendState[BS_COLOR_BLEND_ONE]);

	return hr;
}

bool D3D11::SdkLayersAvailable()
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

// DirectX COM��ü�� ����Ƚ���� ��� �����Ǿ����� üũ�մϴ�.
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