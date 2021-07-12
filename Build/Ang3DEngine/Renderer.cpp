// �ý��� ���
#include "GoodFunc.h"
#include "DebugConsole.h"

// DXTK ���̺귯��
#ifdef _DEBUG 
#pragma comment(lib, "DirectXTK64d")	//64bit + ����� ����.
#pragma comment(lib, "DirectXTex64d")	//64bit + ����� ����.

// For D3D Resource Check
#include <d3d11_4.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include "dxgidebug.h"

#else
#pragma comment(lib, "DirectXTK64")	//64bit ���� 
#pragma comment(lib, "DirectXTex64")	//64bit ���� 
#endif

// ���� ���
#include "D3D11Renderder.h"

// �ڱ� ���
#include "Renderer.h"

// ���ӽ����̽�, ��������
using namespace DirectX::SimpleMath;


hos::cg::Renderer::Renderer(RendererDesc desc)
	: RenderQueInstance(nullptr), CameraInstance(nullptr), bViewCull(true), FogColor(0, 0, 0, 0), dTime(0.f),
	ShadowMapCullMode(hos::cg::RendererDesc::ShadowMapCullList::SHADOWMAP_SOLID)
{
	// ���� Desc �������
	this->RenderDesc = desc;

	// ����ť ����
	this->RenderQueInstance = new RenderQue();

	// ī�޶� ����
	this->CameraInstance = new Camera(this->MatrixBuff, this->CameraViewTM, this->CameraProjTM, this->UIViewTM, this->UIProjTM);

}

hos::cg::Renderer::~Renderer()
{
	DEBUG_CONSOLE(L"Renderer�� �����մϴ�.");

	// ���� ť, ī�޶� ����
	SAFE_DELETE(CameraInstance);
	SAFE_DELETE(RenderQueInstance);

	// ����� �ܼ� ����
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
		DEBUG_CONSOLE(L"RendererDesc�� Hwnd�� �����Ǿ� ������ ������ �����մϴ�.");

		switch (desc.Driver)
		{
		case RendererDesc::DriverType::DRIVER_DIRECT3D11:
			renderer = new D3D11Renderder(desc);

			break;

		default:
			DEBUG_CONSOLE("Renderer ���� ����! �� �� ���� Ÿ���� RenderDriver�Դϴ�.");
			break;
		}
	}

	else
	{
		DEBUG_CONSOLE(L"RendererDesc�� �ùٸ� Hwnd�� ���޵��� �ʾҽ��ϴ�.");
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
	// dTime�� ������ ����
	this->dTime = dTime;
	this->ScreenBuff.SetDeltaTime(this->dTime);
	this->ScreenBuff.SetShadowMapSize(SHADOWMAPSIZE, SHADOWMAPSIZE);
	//this->PreRender(cameraMode, cameraPos, cameraRot, cameraFovDegree, screenNear, screenDepth);

	// ���� Ÿ�� Ŭ����
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
		// ��Ǻ� ����
		this->Draw_GameObject_VelocityMap();
	}
}

void hos::cg::Renderer::RenderCamera_ToSwapChainBuffer(bool bRadial, unsigned int cameraIndex, unsigned int outputIndex)
{
	// ���� �� �غ� �Ѵ�
	SetRasterizeState(RS_NODEPTH);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_WRITE_OFF);

	// ī�޶� ����
	this->CameraInstance->Set3DUICamera((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort(0, 1, 0, 1);

	// �׸���
	this->LastRender(bRadial, cameraIndex, outputIndex);

	// Light���ۿ� ����ť�� ����ش�.
	LightBuff.ClearDirLight();
	RenderQueInstance->ClearObjectRenderQue();
}

void hos::cg::Renderer::RenderUI_ToSwapChainBuffer()
{
	// ȭ�� ũ��
	float _screenWidth = (float)this->RenderDesc.ScreenWidth;
	float _screenHeight = (float)this->RenderDesc.ScreenHeight;

	// ī�޶� �� ����Ʈ ����
	this->CameraInstance->Set3DUICamera((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort(0.0f, 1.0f, 0.0f, 1.0f);

	// UI�� ���� �ø�
	this->RenderQueInstance->ViewFrustum(0.1f, this->CameraProjTM, this->CameraViewTM, true);

	// Layer ����
	this->RenderQueInstance->SortUIQue_LayerOrder();

	// UI ����ť ��ȸ
	std::vector<hos::cg::RenderQue::RenderUIObejct>& _renderList = RenderQueInstance->GetUIObjectList();
	int renderQueSize = RenderQueInstance->GetUIObjectListSize();
	for (int i = 0; i < renderQueSize; i++)
	{
		RenderQue::RenderUIObejct& _TargetQue = _renderList[i];

		// Frustum �⺻��
		bool bGood = true;

		// Object FollowUI�� ��� Frustum Check�� �����ϰ�, �⺻��(bGood)�� �������ش�.
		if (_TargetQue.bFollow)
		{
			bGood = this->RenderQueInstance->CheckFrustum_Point(_TargetQue.Pos);
		}

		// Frustum ���ο� ���� UI or Text�� �׷��ش�.
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
