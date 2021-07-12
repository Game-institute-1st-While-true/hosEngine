// 시스템 헤더
#include "GoodFunc.h"
#include "DebugConsole.h"

// DXTK 라이브러리
#ifdef _DEBUG 
#pragma comment(lib, "DirectXTK64d")	//64bit + 디버그 버전.
#pragma comment(lib, "DirectXTex64d")	//64bit + 디버그 버전.

// For D3D Resource Check
#include <d3d11_4.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include "dxgidebug.h"

#else
#pragma comment(lib, "DirectXTK64")	//64bit 버전 
#pragma comment(lib, "DirectXTex64")	//64bit 버전 
#endif

// 참조 헤더
#include "D3D11Renderder.h"

// 자기 헤더
#include "Renderer.h"

// 네임스페이스, 전역변수
using namespace DirectX::SimpleMath;


hos::cg::Renderer::Renderer(RendererDesc desc)
	: RenderQueInstance(nullptr), CameraInstance(nullptr), bViewCull(true), FogColor(0, 0, 0, 0), dTime(0.f),
	ShadowMapCullMode(hos::cg::RendererDesc::ShadowMapCullList::SHADOWMAP_SOLID)
{
	// 렌더 Desc 복사대입
	this->RenderDesc = desc;

	// 렌더큐 생성
	this->RenderQueInstance = new RenderQue();

	// 카메라 생성
	this->CameraInstance = new Camera(this->MatrixBuff, this->CameraViewTM, this->CameraProjTM, this->UIViewTM, this->UIProjTM);

}

hos::cg::Renderer::~Renderer()
{
	DEBUG_CONSOLE(L"Renderer를 종료합니다.");

	// 렌더 큐, 카메라 해제
	SAFE_DELETE(CameraInstance);
	SAFE_DELETE(RenderQueInstance);

	// 디버그 콘솔 종료
	DebugConsole::GetInstance()->Shutdown();

#ifdef _DEBUG
	IDXGIDebug* debug;
	DXGIGetDebugInterface1(0, IID_IDXGIDebug, (void**)&debug);
	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	debug->Release();
#endif
}

hos::cg::Renderer* hos::cg::Renderer::CreateRenderer(RendererDesc& desc)
{
	Renderer* renderer = nullptr;

	if (desc.Hwnd)
	{
		DEBUG_CONSOLE(L"RendererDesc의 Hwnd가 감지되어 렌더러 생성을 시작합니다.");

		switch (desc.Driver)
		{
		case RendererDesc::DriverType::DRIVER_DIRECT3D11:
			renderer = new D3D11Renderder(desc);

			break;

		default:
			DEBUG_CONSOLE("Renderer 생성 실패! 알 수 없는 타입의 RenderDriver입니다.");
			break;
		}
	}

	else
	{
		DEBUG_CONSOLE(L"RendererDesc로 올바른 Hwnd가 전달되지 않았습니다.");
	}

	return renderer;
}

hos::cg::RendererDesc& hos::cg::Renderer::GetRenderDesc()
{
	return this->RenderDesc;
}

void hos::cg::Renderer::ForwardRender_ToCameraBuffer(float dTime, float nearDepth, Vector3 cameraPos, Vector3 cameraRot, float cameraFovDegree,
	float screenWidth, float screenHeight, float screenNear, float screenDepth, hos::cg::RendererDesc::CameraModeList cameraMode,
	unsigned int mortionBlurCount, unsigned int cameraIndex, bool skyBox, std::wstring_view skyBoxName, Vector4 clearColor, bool bDebug)
{
	// dTime을 엔진에 세팅
	this->dTime = dTime;
	this->ScreenBuff.SetDeltaTime(this->dTime);
	this->ScreenBuff.SetShadowMapSize(SHADOWMAPSIZE, SHADOWMAPSIZE);
	//this->PreRender(cameraMode, cameraPos, cameraRot, cameraFovDegree, screenNear, screenDepth);

	// 렌더 타겟 클리어
	this->ClearAllRenderTarget(cameraIndex, clearColor.x, clearColor.y, clearColor.z, clearColor.w, false);
	
	if (this->RenderDesc.FogState == false)
	{
		this->ScreenBuff.SetFog(0, 0, 0, 0, false);
	}

	else
	{
		this->ScreenBuff.SetFog(this->FogColor.x, this->FogColor.y, this->FogColor.z, this->FogColor.w, true);
	}

	// SkyBox
	if (skyBox)
	{
		this->DrawSkyBox(skyBoxName, cameraIndex);
	}

	// Debug Grid, Debug Object
	if (bDebug)
	{
		this->DrawDebugGrid(cameraIndex);
		this->Draw_DebugObject(cameraIndex);
	}

	// Shadow Map
	this->Draw_GameObject_ShadowMap(cameraPos, cameraFovDegree);

	// Render GameObject
	this->Draw_GameObject(nearDepth, cameraPos, false, cameraIndex);

	// Emiss Map
	this->Draw_GameObject_EmissMap(cameraPos, cameraRot, cameraFovDegree, screenWidth, screenHeight, screenNear, screenDepth);

	// Velocity
	this->ScreenBuff.SetMotionBlurSampleCount(mortionBlurCount);

	if (this->RenderDesc.MotionBlurState && mortionBlurCount != 0)
	{
		// 모션블러 세팅
		this->Draw_GameObject_VelocityMap();
	}
}

void hos::cg::Renderer::RenderCamera_ToSwapChainBuffer(bool bRadial, unsigned int cameraIndex, unsigned int outputIndex)
{
	// 렌더 할 준비를 한다
	SetRasterizeState(RS_NODEPTH);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_WRITE_OFF);

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort(0, 1, 0, 1);

	// 그리기
	this->LastRender(bRadial, cameraIndex, outputIndex);

	// Light버퍼와 렌더큐를 비워준다.
	LightBuff.ClearDirLight();
	RenderQueInstance->ClearObjectRenderQue();
}

void hos::cg::Renderer::RenderUI_ToSwapChainBuffer()
{
	// 화면 크기
	float _screenWidth = (float)this->RenderDesc.ScreenWidth;
	float _screenHeight = (float)this->RenderDesc.ScreenHeight;

	// 카메라 및 뷰포트 설정
	this->CameraInstance->Set3DUICamera((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort(0.0f, 1.0f, 0.0f, 1.0f);

	// UI를 위한 컬링
	this->RenderQueInstance->ViewFrustum(0.1f, this->CameraProjTM, this->CameraViewTM, true);

	// Layer 솔팅
	this->RenderQueInstance->SortUIQue_LayerOrder();

	// UI 렌더큐 순회
	std::vector<hos::cg::RenderQue::RenderUIObejct>& _renderList = RenderQueInstance->GetUIObjectList();
	int renderQueSize = RenderQueInstance->GetUIObjectListSize();
	for (int i = 0; i < renderQueSize; i++)
	{
		RenderQue::RenderUIObejct& _TargetQue = _renderList[i];

		// Frustum 기본값
		bool bGood = true;

		// Object FollowUI인 경우 Frustum Check를 시행하고, 기본값(bGood)을 갱신해준다.
		if (_TargetQue.bFollow)
		{
			bGood = this->RenderQueInstance->CheckFrustum_Point(_TargetQue.Pos);
		}

		// Frustum 여부에 따라 UI or Text를 그려준다.
		if (bGood)
		{
			if (_TargetQue.bFont)
			{
				DrawFontText(_TargetQue);
			}

			else
			{
				DrawUIObject_3D(_TargetQue, _screenWidth, _screenHeight);
			}
		}
	}
	RenderQueInstance->ClearUIRenderQue();
}

void hos::cg::Renderer::SetViewMatrixToPipeline(const Matrix mView)
{
	this->bViewSet = true;
	this->CameraViewTM = mView;
	//this->MatrixBuff.SetViewTM(mView);
}

void hos::cg::Renderer::SetProjMatrixToPipeline(const Matrix mProj)
{
	this->bProjSet = true;
	this->CameraProjTM = mProj;
	//this->MatrixBuff.SetProjTM(mProj);
}

void hos::cg::Renderer::SetDirLight(Vector4& dir, Vector4& diffColor)
{
	this->LightBuff.AddDirectionLight(dir, diffColor);
}

void hos::cg::Renderer::SetPointLight(Vector4& pos, Vector4& color, Vector4& attenuation)
{
	this->LightBuff.AddPointLight(pos, color, attenuation);
}

void hos::cg::Renderer::SetAmbientLight(DirectX::SimpleMath::Vector4& ambColor)
{
	this->LightBuff.AddAmbientLight(ambColor);
}

void hos::cg::Renderer::AddRenderObejct(RenderQue::RenderObject& objectInfo)
{
	RenderQueInstance->AddRenderObejct(objectInfo);
}

void hos::cg::Renderer::AddAlphaObject(RenderQue::RenderObject& objectInfo)
{
	RenderQueInstance->AddAlphaObject(objectInfo);
}

void hos::cg::Renderer::AddEffectObject(RenderQue::RenderObject& objectInfo)
{
	RenderQueInstance->AddEffectObject(objectInfo);
}

void hos::cg::Renderer::AddDebugObject(RenderQue::RenderObject& objectInfo)
{
	RenderQueInstance->AddDebugObject(objectInfo);
}

void hos::cg::Renderer::AddRenderUIObejct(RenderQue::RenderUIObejct& objectInfo)
{
	RenderQueInstance->AddUIObject(objectInfo);
}

void hos::cg::Renderer::SetFogOption(float r, float g, float b, float density)
{
	//this->ScreenBuff.SetFog(r, g, b, density);
	this->FogColor.x = r;
	this->FogColor.y = g;
	this->FogColor.z = b;
	this->FogColor.w = density;
}

void hos::cg::Renderer::SetRadialValue(float uv_x, float uv_y, float distance, float sterngth)
{
	this->ScreenBuff.SetRadialBlurValue(uv_x, uv_y, distance, sterngth);
}

void hos::cg::Renderer::SetBloomPower(float bloomPower)
{
	this->ScreenBuff.SetBloomPower(bloomPower);
}
