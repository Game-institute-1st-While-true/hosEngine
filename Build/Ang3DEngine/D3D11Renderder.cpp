// 필수 헤더
#include "GoodFunc.h"
#include "DebugConsole.h"

// 참조 헤더
#include "D3D11.h"
#include "RendererDesc.h"
#include "VertexDataType.h"

#include "D3D11VariableManager.h"

#include "D3D11Data.h"
#include "D3D11Shader.h"
#include "D3D11ShaderDesc.h"

#include "D3D11ExtraRenderTarget.h"
#include "D3D11Texture2D.h"

#include "D3D11ShadowBuffer.h"

#include "TempDefine.h"

#include "DXTK/SpriteBatch.h"
#include "DXTK/SpriteFont.h"

// 그리기용 헤더
#include "D3D11ParticleSystem.h"

#include "DebugGrid.h"

// 자기 헤더
#include "D3D11Renderder.h"

#define DOWNSAMPLESIZE_1 2
#define DOWNSAMPLESIZE_2 4
#define DOWNSAMPLESIZE_3 6

// 생성자는 상위클래스의 생성자 선택을 위해 헤더에만 구현
// TODO: cpp에 정의하면서 생성자 선택을 할 수 있을까?

using namespace hos::cg;
using namespace DirectX;
using namespace DirectX::SimpleMath;

D3D11Renderder::D3D11Renderder(RendererDesc& desc)
	: Renderer(desc), D3D11Instance(nullptr), DeviceRef(nullptr), DeviceContextRef(nullptr), VariableManagerInstance(nullptr), DebugGridInstance(nullptr), PreRenderTarget{ nullptr, },
	ParticleSystem(nullptr)
{
	// Direct11 초기화
	this->D3D11Instance = new D3D11(RenderDesc);	// Renderer 클래스의 Desc를 참조한다.
	if (D3D11Instance)
	{
		DEBUG_CONSOLE(L"D3D11 초기화가 완료되었습니다.");
	}

	// Direct11에서 사용할 변수 매니저 초기화
	VariableManagerInstance = new D3D11VariableManager(*(this->D3D11Instance->GetDevice()), *(this->D3D11Instance->GetContext()));

	// DeviceManager 생성 및 D3D11 바인딩
	//HRESULT hr = D3D11DEVICE_MANAGER->BindD3D11(D3D11Instance, VariableManagerInstance);
	//if (FAILED(hr))
	//{
	//	DEBUG_CONSOLE(L"D3D11DeviceManager 생성에 실패했습니다.");
	//}

	// ContextRef 등록
	this->DeviceRef = this->D3D11Instance->GetDevice();
	this->DeviceContextRef = this->D3D11Instance->GetContext();

	// 상수버퍼 생성
	CreateAllConstBuffer();

	// 샘플러 생성
	CreateAllSampler();

	// 쉐이더 생성
	CreateAllShader();

	// Extra RenderTarget 생성
	this->CreateExtraRenderTarget();

	// Deferred용 텍스쳐 생성
	//this->CreateDeferredTexture();

	// 디버그 그리드 생성
//#ifdef _DEBUG
	this->DebugGridInstance = new DebugGrid(*(this->DeviceRef), *(this->DeviceContextRef));
//#endif

	// 파티클 시스템
	//this->ParticleSystem = new D3D11ParticleSystem(this->DeviceRef, this->DeviceContextRef);

};

// 소멸자
D3D11Renderder::~D3D11Renderder()
{
//	for (int i = 0; i < MAX_CAMERA; i++)
//	{
//		SAFE_DELETE(PreRenderTarget[i]);
//	}
	SAFE_DELETE(ParticleSystem);

	SAFE_DELETE(PreRenderTarget);

	// Blur용
	SAFE_DELETE(RT_DownSample_x16);
	SAFE_DELETE(RT_DownSample_x16_2);
	SAFE_DELETE(RT_DownSample_x64);
	SAFE_DELETE(RT_DownSample_x64_2);
	SAFE_DELETE(RT_DownSample_x4);
	SAFE_DELETE(RT_DownSample_x4_2);


	// 그림자용
	SAFE_DELETE(RenderTarget_LightViewShadowMap);
	SAFE_DELETE(RT_SoftShadow);
	
	// Velocity(모션블러)용
	SAFE_DELETE(RenderTarget_NowDepth);
	SAFE_DELETE(RT_Velocity);

	SAFE_DELETE(DebugGridInstance);

	//D3D11DEVICE_MANAGER->ShutDown();		// D3D11 DeviceManager 제거(싱글톤)

	SAFE_DELETE(VariableManagerInstance);	// D3D11 변수 매니저 제거
	SAFE_DELETE(D3D11Instance);			// D3D11 API 제거
}

void D3D11Renderder::BeginDraw(unsigned int screenWidth, unsigned int screenHeight, float r, float g, float b, float a)
{
	// 최소화 된 경우 체크
	bool bMinBar = false;
	if (screenWidth == 0 && screenHeight == 0)
	{
		bMinBar = true;
	}

	// 현재 클라이언트 해상도가 RenderTarget들의 해상도와 동일한 지 확인한다.
	// 넓이와 높이 중 하나라도 다르다면 해상도와 관련된 모든 RenderTarget들을 재생성해준다.
	// 최소화 된 경우라면 Resize 하지 않는다.
	if ((RenderDesc.ScreenWidth != screenWidth || RenderDesc.ScreenHeight != screenHeight ) && bMinBar == false)
	{
		// RenderDesc 정보 변경
		this->RenderDesc.ScreenWidth = screenWidth;
		this->RenderDesc.ScreenHeight = screenHeight;

		// Extra RenderTarget들을 새 해상도에 맞게 재생성한다(D3D11Renderer 단계)
		this->ResizeExtraRenderTarget(screenWidth, screenHeight, this->RenderDesc.ValueAA);
		//this->CreateExtraRenderTarget();

		// SwapChain의 크기를 재조정한다(D3D11Device 단계)
		this->D3D11Instance->ResizeSwapChain(screenWidth, screenHeight, RenderDesc.ValueAA);
	}

	D3D11Instance->BeginScene(r, g, b, a);
}

void D3D11Renderder::EndDraw()
{
	D3D11Instance->EndScene();
}

bool hos::cg::D3D11Renderder::CreateSpriteFont(std::wstring_view fontName, uint8_t const* data, size_t dataSize)
{
	return this->VariableManagerInstance->CreateSpriteFont(fontName, data, dataSize);
}

void hos::cg::D3D11Renderder::_DebugText(int posX, int posY, int size, DirectX::SimpleMath::Vector4 col, const WCHAR* text, ...)
{
	// 뒤의 가변인자를 처리해줍니다.
	TCHAR* buffer;
	int len;

	va_list ap;
	va_start(ap, text);

	len = _vscwprintf(text, ap) + 1;
	buffer = new TCHAR[len];

	vswprintf_s(buffer, len, text, ap);
	va_end(ap);

	// 저장!
	std::wstring _text = buffer;

	// 삭제!
	delete[] buffer;
	buffer = nullptr;

	// RenderQue에 등록, LayerOrder는 넉넉하게 3000: 숫자가 클수록 나중에 그린다.
	hos::cg::RenderQue::RenderUIObejct _textObj;
	_textObj.bFont = true;
	_textObj.Color = col;
	_textObj.FontName = L"Arial";
	_textObj.FontSize = size;
	_textObj.LayerOrder = 3000;
	_textObj.Text = _text;
	_textObj.Pos.x = (float)posX;
	_textObj.Pos.y = (float)posY;

	this->RenderQueInstance->AddUIObject(_textObj);
}

bool hos::cg::D3D11Renderder::LoadSkyBox(std::wstring_view skyBoxName, std::wstring_view filePath)
{
	return VariableManagerInstance->LoadSkyBox(skyBoxName, filePath);
}

void hos::cg::D3D11Renderder::SetViewPort(float left, float right, float top, float bottom)
{
	D3D11Instance->SetViewPort(left, right, top, bottom);
}

void hos::cg::D3D11Renderder::SetViewPort_Size(float left, float right, float top, float bottom)
{
	D3D11Instance->SetViewPort_Size(left, right, top, bottom);
}

void hos::cg::D3D11Renderder::CreateMeshVariable(std::wstring meshName, void* vtxData, unsigned int vtxCount, void* indexData, unsigned int indexCount)
{
	VariableManagerInstance->CreateMeshBuffer(meshName, vtxData, vtxCount, indexData, indexCount);
}

bool hos::cg::D3D11Renderder::CreateTextureVariable(std::wstring textureName, std::wstring fileRoot, bool* bAlpha)
{
	return VariableManagerInstance->CreateD3D11Texture(textureName, fileRoot, bAlpha);
}

void hos::cg::D3D11Renderder::ReleaseMeshVariable(std::wstring meshName)
{
	VariableManagerInstance->ReleaseMeshBuffer(meshName);
}

void hos::cg::D3D11Renderder::ReleaseTextureVariable(std::wstring textureName)
{
	VariableManagerInstance->ReleaseD3D11Texture(textureName);
}

void hos::cg::D3D11Renderder::AllClearMeshVariableList()
{
	VariableManagerInstance->AllClearMeshVariableList();
}

void hos::cg::D3D11Renderder::AllClearTextureVariableList()
{
	VariableManagerInstance->AllClearTextureVariableList();
}

bool hos::cg::D3D11Renderder::GetFullScreenState() const
{
	return this->D3D11Instance->GetFullScreenState();
}

void hos::cg::D3D11Renderder::PreRender(RendererDesc::CameraModeList cameraMode, Vector3 cameraPos, Vector3 cameraRot, float fov, float nearValue, float depthValue)
{
	float _screenWidth = (float)this->RenderDesc.ScreenWidth;
	float _screenHeight = (float)this->RenderDesc.ScreenHeight;

	if (this->bViewSet == false || this->bProjSet)
	{
		//this->CameraInstance->Set3DCamera_DefaultView(cameraPos, cameraRot, fov, _screenWidth, _screenHeight, near, depth);
		this->CameraInstance->Set3DCamera_DefaultView(cameraPos, cameraRot, fov, _screenWidth, _screenHeight, nearValue, depthValue);
	}
}

void hos::cg::D3D11Renderder::LastRender(bool bRadial, unsigned int cameraIndex, unsigned int ouputIndex)
{
	// Quad_UI용 행렬
	//Matrix mTrans = Matrix::Identity;
	//Matrix mRot = Matrix::Identity;
	//Matrix mScale = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight, 1.0f);
	//Matrix mTM = mScale * mRot * mTrans;

	Matrix mTM = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight, 1.0f);

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// Material
	MaterialBuff.SetDiffColor(Vector4(1, 1, 1, 1));
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);

	// VariableManager에서 Buffer를 가져온다.
	ID3D11Buffer* VB = VariableManagerInstance->Get3DUIBuffer(5);	// 5번이 Center Quad

	// 버퍼 설정.
	UINT stride = sizeof(VertexDataType::UI_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11ShaderResourceView* _emissMap = nullptr;

	ID3D11ShaderResourceView* _SRV = nullptr;
	std::wstring _drawMode;

	// 텍스쳐
	switch (ouputIndex)
	{
	default:
	case 1:
		_SRV = this->PreRenderTarget->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Forwrad Render: Final";

		_emissMap = this->RT_Velocity->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(4, 1, &_emissMap);

		_emissMap = this->RT_DownSample_x4_2->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(5, 1, &_emissMap);
		break;

	case 2:
		_SRV = this->RT_DownSample_x4_2->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Forward Render : Bloom_Map";
		break;

	case 3:
		_SRV = this->RenderTarget_LightViewShadowMap->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Deferred Render : LightView_ShadowMap";
		break;

	case 4:
		_SRV = this->RT_SoftShadow->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Forward Render : ShadowMap";
		break;

	case 5:
		_SRV = this->RenderTarget_NowDepth->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Deferred Render : Now_DepthMap";
		break;

	case 6:
		_SRV = this->RT_Velocity->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
		_drawMode = L"Now RenderMode: Deferred Render : Velocity Map";
		break;
	}

	// POST PROCESSING
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);

	bool bMotionBlur = false;
	if (this->RenderDesc.MotionBlurState && this->ScreenBuff.GetMBSampleCount() != 0)
	{
		bMotionBlur = true;
	}

	if (bRadial && bMotionBlur)
	{
		VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TOSWAPCHAIN_ALLBLUR, 4);
	}

	else if (bRadial)
	{
		VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TOSWAPCHAIN_RADIALBLUR, 4);
	}

	else if (bMotionBlur)
	{
		VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TOSWAPCHAIN_MOTIONNBLUR, 4);
	}

	else
	{
		VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TOSWAPCHAIN, 4);
	}

	ID3D11ShaderResourceView* nulltexure = nullptr;
	this->DeviceContextRef->PSSetShaderResources(4, 1, &nulltexure);
	this->DeviceContextRef->PSSetShaderResources(5, 1, &nulltexure);

	// 이번 프레임의 카메라ProjTM을 저장.
	this->OldViewTM = this->CameraViewTM;
	this->OldProjTM = this->CameraProjTM;
	this->bViewSet = false;
	this->bProjSet = false;
}

void hos::cg::D3D11Renderder::ClearAllRenderTarget(unsigned int cameraIndex, float r, float g, float b, float a, bool bDeferred)
{
	// Set ViewPort
	this->SetViewPort(0.0f, 1.0f, 0.0f, 1.0f);

	// Camera RenderTarget Clear
	this->PreRenderTarget->ReadyToDraw(r, g, b, a);

	this->RT_SoftShadow->ClearRenderTarget(1.f, 1.f, 1.f, 1.f);

	this->RT_DownSample_x4->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	this->RT_DownSample_x4->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	this->RT_DownSample_x16->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	this->RT_DownSample_x16_2->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	this->RT_DownSample_x64->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);
	this->RT_DownSample_x64_2->ClearRenderTarget(0.0f, 0.0f, 0.0f, 0.0f);

	//this->RenderTarget_LightViewShadowMap->ClearRenderTarget(1.0f, 0.0f, 0.0f, 1.0f);

	this->RenderTarget_NowDepth->ClearRenderTarget(1.0f, 0.0f, 0.0f, 1.0f);

	this->RT_Velocity->ClearRenderTarget(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 세팅
	this->MatrixBuff.SetViewTM(this->CameraViewTM);
	this->MatrixBuff.SetProjTM(this->CameraProjTM);

	// 라이트버퍼 업데이트
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_LIGHT);

	/// MaterialBuffer는 비우고, 업데이트 해 주어야 할까?

	/// 컬링


	/// 렌더큐 정렬(알파소팅)


}

void hos::cg::D3D11Renderder::Draw_GameObject(float nearDepth, Vector3 cameraPos, bool bDeferred, unsigned int cameraIndex)
{
	// 카메라 세팅
	this->MatrixBuff.SetViewTM(this->CameraViewTM);
	this->MatrixBuff.SetProjTM(this->CameraProjTM);
	this->SetViewPort(0.0f, 1.0f, 0.0f, 1.0f);

	// 뷰 프러스텀
	if (this->bViewCull)
	{
		this->RenderQueInstance->ViewFrustum(nearDepth, this->CameraProjTM, this->CameraViewTM);
	}

	////////////////////////////////////////////////

	// Render State Setting
	SetRasterizeState(RS_CULLBACK);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_ON);

	std::vector<hos::cg::RenderQue::RenderObject>& _renderList = RenderQueInstance->GetRenderObjectList();
	int renderQueSize = RenderQueInstance->GetRenderObjectListSize();

	for (int i = 0; i < renderQueSize; i++)
	{
		RenderQue::RenderObject& _TargetQue = _renderList[i];
		if (_TargetQue.InView)
		{
			DrawEachObject(_TargetQue, false, bDeferred, cameraIndex);
		}
	}


	////////////////////////////////////////////////

	// Render State Setting
	SetBlendState(BS_ALPHA_BLEND);

	std::vector<hos::cg::RenderQue::RenderObject>& _alphaList = RenderQueInstance->GetAlphaObjectList();
	int alphaQueSize = RenderQueInstance->GetAlphaObjectListSize();

	// 알파소팅 
	RenderQueInstance->CalculateViewDistanceInAlphaQue(cameraPos);

	for (int i = 0; i < alphaQueSize; i++)
	{
		RenderQue::RenderObject& _AlphaQue = _alphaList[i];
		if (_AlphaQue.InView)
		{
			DrawEachObject(_AlphaQue, true, bDeferred, cameraIndex);
		}
	}

	//////////////////////////

	// Render State Setting
	SetDepthStencilState(DS_DEPTH_WRITE_OFF);

	std::vector<hos::cg::RenderQue::RenderObject>& _effectList = RenderQueInstance->GetEffectList();
	int effectQueSize = RenderQueInstance->GetEffectListSize();

	for (int i = 0; i < effectQueSize; i++)
	{
		RenderQue::RenderObject& _effectQue = _effectList[i];
		if (_effectQue.InView)
		{
			DrawEachObject(_effectQue, true, bDeferred, cameraIndex);
		}
	}

	//////////////////////////


	//this->ParticleSystem->Frame(this->dTime);

	//this->ParticleSystem->Render();


	///////////////////////////

	ID3D11ShaderResourceView* nullSRV[6] = { nullptr, };
	this->DeviceContextRef->PSSetShaderResources(0, 6, nullSRV);
}

void hos::cg::D3D11Renderder::Draw_GameObject_ShadowMap(Vector3 cameraPos, float cameraFovDegree)
{
	// 렌더타겟 갱신
	this->RenderTarget_LightViewShadowMap->ReadyToDraw();

	// Render State Setting
	switch (this->ShadowMapCullMode)
	{
	case hos::cg::RendererDesc::ShadowMapCullList::SHADOWMAP_SOLID:
		SetRasterizeState(RS_SOLID);
		break;

	case hos::cg::RendererDesc::ShadowMapCullList::SHADOWMAP_CULLBACK:
		SetRasterizeState(RS_CULLBACK);
		break;

	case hos::cg::RendererDesc::ShadowMapCullList::SHADOWMAP_CULLFRONT:
		SetRasterizeState(RS_CULLFRONT);
		break;
	}

	
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_ON);

	// 그림자를 위한 LightBuffer 갱신
	this->LightBuff.UpdateLightBuffForShadow(cameraPos, cameraFovDegree, this->RenderDesc.ScreenWidth, this->RenderDesc.ScreenHeight);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_LIGHT);

	// 렌더큐 준비
	std::vector<hos::cg::RenderQue::RenderObject>& _renderList = RenderQueInstance->GetRenderObjectList();
	int renderQueSize = RenderQueInstance->GetRenderObjectListSize();
	for (int i = 0; i < renderQueSize; i++)
	{
		RenderQue::RenderObject& _TargetQue = _renderList[i];
		bool bSkin = _TargetQue.bSkinning;

		// 스키닝 Mesh
		if (bSkin)
		{
			DrawSkinShadowMap(_TargetQue);
		}

		// 정적 Mesh or Only Bone Animation
		else
		{
			DrawShadowMap(_TargetQue);
		}
	}


	// 알파 렌더큐 준비
	std::vector<hos::cg::RenderQue::RenderObject>& _alphaList = RenderQueInstance->GetAlphaObjectList();
	int alphaQueSize = RenderQueInstance->GetAlphaObjectListSize();
	for (int i = 0; i < alphaQueSize; i++)
	{
		RenderQue::RenderObject& _TargetQue = _alphaList[i];
		bool bSkin = _TargetQue.bSkinning;

		// 스키닝 Mesh
		if (bSkin)
		{
			DrawSkinShadowMap(_TargetQue);
		}

		// 정적 Mesh or Only Bone Animation
		else
		{
			DrawShadowMap(_TargetQue);
		}
	}
}

void hos::cg::D3D11Renderder::Draw_GameObject_EmissMap(Vector3 vPos, Vector3 vRot, float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth,
	float intensity, float pixelInterval)
{
	// Render State Setting
	SetRasterizeState(RS_NODEPTH);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_WRITE_OFF);

	// 렌더타겟 갱신
	this->RT_DownSample_x4->SetRenderTarget();

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort_Size(0, screenWidth/ DOWNSAMPLESIZE_1, 0, screenHeight / DOWNSAMPLESIZE_1);

	// 그리기
	// Quad_UI용 행렬
	Matrix mTrans = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix mRot = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	Matrix mScale = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth / DOWNSAMPLESIZE_1, (float)this->RenderDesc.ScreenHeight / DOWNSAMPLESIZE_1, 1.0f);
	Matrix mTM = mScale * mRot * mTrans;

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// Material
	MaterialBuff.SetDiffColor(Vector4(1, 1, 1, 1));
	MaterialBuff.SetOffTill(Vector4(0, 1, 0, 1));
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);

	// VariableManager에서 Buffer를 가져온다.
	ID3D11Buffer* VB = VariableManagerInstance->Get3DUIBuffer(5);	// 5번이 Center Quad

	// 버퍼 설정.
	UINT stride = sizeof(VertexDataType::UI_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// 텍스쳐 맵 설정
	ID3D11ShaderResourceView* _SRV = nullptr;
	_SRV = this->PreRenderTarget->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);

	// 그리기
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_BLOOMCURVE, 4);

	// 텍스쳐 제거
	ID3D11ShaderResourceView* _null = nullptr;
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);

	// 렌더타겟 갱신
	this->RT_DownSample_x16->SetRenderTarget();

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort_Size(0, screenWidth / DOWNSAMPLESIZE_2, 0, screenHeight / DOWNSAMPLESIZE_2);

	// 스크린 버퍼
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);


	// 그리기
	// Quad_UI용 행렬
	mTrans = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	mRot = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	mScale = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth / DOWNSAMPLESIZE_2, (float)this->RenderDesc.ScreenHeight / DOWNSAMPLESIZE_2, 1.0f);
	mTM = mScale * mRot * mTrans;

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// 텍스쳐 맵 설정
	_SRV = this->RT_DownSample_x4->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);

	// 그리기
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TEXON_UI, 4);

	// 텍스쳐 제거
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);















	// 렌더타겟 갱신
	this->RT_DownSample_x64->SetRenderTarget();

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera(screenWidth / DOWNSAMPLESIZE_3, screenHeight / DOWNSAMPLESIZE_3);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort_Size(0, screenWidth / DOWNSAMPLESIZE_3, 0, screenHeight / DOWNSAMPLESIZE_3);

	// 스크린 버퍼
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_3, screenHeight / DOWNSAMPLESIZE_3);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);


	// 그리기
	// Quad_UI용 행렬
	mTrans = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	mRot = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	mScale = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth / DOWNSAMPLESIZE_3, (float)this->RenderDesc.ScreenHeight / DOWNSAMPLESIZE_3, 1.0f);
	mTM = mScale * mRot * mTrans;

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// 텍스쳐 맵 설정
	_SRV = this->RT_DownSample_x16->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);

	// 그리기
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_TEXON_UI, 4);

	// 텍스쳐 제거
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);


	














	// 64배율 가우시안 블러
	this->RT_DownSample_x64_2->SetRenderTarget();
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_3, screenHeight / DOWNSAMPLESIZE_3);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);
	_SRV = this->RT_DownSample_x64->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_X, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);

	this->RT_DownSample_x64->SetRenderTarget();
	_SRV = this->RT_DownSample_x64_2->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_Y, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);


	/// 64를 16에 합침
	
	// 카메라 세팅
	this->CameraInstance->Set3DUICamera(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort_Size(0, screenWidth / DOWNSAMPLESIZE_2, 0, screenHeight / DOWNSAMPLESIZE_2);

	// 스크린 버퍼
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);

	// 합치기
	this->RT_DownSample_x16_2->SetRenderTarget();
	_SRV = this->RT_DownSample_x16->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	_SRV = this->RT_DownSample_x64->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(1, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_COMBINE, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);
	this->DeviceContextRef->PSSetShaderResources(1, 1, &_null);

	// 16짜리 가우시안 블러
	this->RT_DownSample_x16->SetRenderTarget();
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);
	_SRV = this->RT_DownSample_x16_2->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_X, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);

	this->RT_DownSample_x16_2->SetRenderTarget();
	_SRV = this->RT_DownSample_x16->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_Y, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);



	/// 16을 4에 합침

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);
	this->SetViewPort_Size(0, screenWidth / DOWNSAMPLESIZE_1, 0, screenHeight / DOWNSAMPLESIZE_1);

	// 스크린 버퍼
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);

	// 합치기
	this->RT_DownSample_x4_2->SetRenderTarget();
	_SRV = this->RT_DownSample_x16_2->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	_SRV = this->RT_DownSample_x4->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(1, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_COMBINE, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);
	this->DeviceContextRef->PSSetShaderResources(1, 1, &_null);


	// 4짜리 가우시안 블러
	this->RT_DownSample_x4->SetRenderTarget();
	this->ScreenBuff.SetEmissMapSize(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);
	_SRV = this->RT_DownSample_x4_2->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_X, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);

	this->RT_DownSample_x4_2->SetRenderTarget();
	_SRV = this->RT_DownSample_x4->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_SRV);
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_GBLUR_Y, 4);
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_null);
}

void hos::cg::D3D11Renderder::Draw_GameObject_VelocityMap()
{
	// 렌더타겟 세팅
	this->RT_Velocity->SetRenderTarget();

	// 렌더 할 준비를 한다
	SetRasterizeState(RS_NODEPTH);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_WRITE_OFF);

	// 카메라 세팅
	this->CameraInstance->Set3DUICamera((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight);
	this->MatrixBuff.SetViewTM(this->UIViewTM);
	this->MatrixBuff.SetProjTM(this->UIProjTM);

	// 렌더타겟용 상수버퍼 업데이트
	this->ScreenBuff.SetNowCameraInverseMatrix(this->CameraViewTM, this->CameraProjTM);
	this->ScreenBuff.SetOldCameraMatrix(this->OldViewTM, this->OldProjTM);
	this->VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_SCREEN);

	// 뷰포트 세팅
	this->SetViewPort(0, 1, 0, 1);

	// Quad_UI용 행렬
	Matrix mTrans = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix mRot = Matrix::CreateFromYawPitchRoll(0.0f, -XM_PI, 0.0f);
	Matrix mScale = Matrix::CreateScale((float)this->RenderDesc.ScreenWidth, (float)this->RenderDesc.ScreenHeight, 1.0f);
	Matrix mTM = mScale * mRot * mTrans;

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// Material
	MaterialBuff.SetDiffColor(Vector4(1, 1, 1, 1));
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);

	// VariableManager에서 Buffer를 가져온다.
	ID3D11Buffer* VB = VariableManagerInstance->Get3DUIBuffer(5);	// 5번이 Center Quad

	// 버퍼 설정.
	UINT stride = sizeof(VertexDataType::UI_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 텍스쳐 설정
	ID3D11ShaderResourceView* _inputTexture = nullptr;

	// NowDepthMap
	_inputTexture = this->RenderTarget_NowDepth->GetShaderResourceView();
	this->DeviceContextRef->PSSetShaderResources(0, 1, &_inputTexture);

	// 그리기
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_MAP_VELOCITY, 4);

	// 텍스쳐 파이프라인에서 해제
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, };
	this->DeviceContextRef->PSSetShaderResources(0, 2, nullSRV);
}

void hos::cg::D3D11Renderder::Draw_DebugObject(unsigned int cameraIndex)
{
	// 와이어프레임 세팅
	SetRasterizeState(RS_WIREFRM);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_ON);

	// 카메라 세팅
	this->MatrixBuff.SetViewTM(this->CameraViewTM);
	this->MatrixBuff.SetProjTM(this->CameraProjTM);

	// Debug RenderQue
	std::vector<hos::cg::RenderQue::RenderObject>& _queList = RenderQueInstance->GetDebugObjectList();
	int debugQueSize = (int)_queList.size();
	for (int i = 0; i < debugQueSize; i++)
	{
		RenderQue::RenderObject& _TargetQue = _queList[i];
		DrawDebugPrimitive(_TargetQue);
	}
}

void hos::cg::D3D11Renderder::DrawEachObject(RenderQue::RenderObject& targetQue, bool bAlpha, bool bDeferred, unsigned int cameraIndex)
{
	// 노드 관련 정보 획득
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;

	// 행렬 설정
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// 스킨 행렬 설정
	if (targetQue.bSkinning)
	{
		BoneBuff.SetBoneTM(targetQue.mSkinnedAni, targetQue.BoneCount);
		VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_BONE);
	}

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet& _meshRef = VariableManagerInstance->GetMeshBuffer(targetQue.MeshName);
	ID3D11Buffer* VB = _meshRef.VertexBuffer;
	ID3D11Buffer* IB = _meshRef.IndexBuffer;
	unsigned int _indexCount = _meshRef.IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 재질 및 텍스쳐 설정
	bool bAlbedo = this->SetMaterial(targetQue.MaterialInfo);

	// 쉐이더 갱신
	DrawByShaderType(targetQue, bAlpha, bDeferred, bAlbedo, cameraIndex, _indexCount);
}

bool hos::cg::D3D11Renderder::SetMaterial(RenderQue::RenderMaterial& targetMaterial)
{
	ID3D11ShaderResourceView* inputTexture = nullptr;

	bool bAlbedo = targetMaterial.bAlbedo;
	bool bNormal = targetMaterial.bNormal;
	bool bARM = targetMaterial.bARM;
	bool bEmiss = targetMaterial.bEmiss;

	// 텍스쳐가 있는 경우
	if (bAlbedo)
	{
		inputTexture = VariableManagerInstance->GetTexture(targetMaterial.TexNameAlbedo);
		this->DeviceContextRef->PSSetShaderResources(0, 1, &inputTexture);
	}

	// 텍스쳐가 없을 경우
	else
	{
		// 마테리얼 갱신
		MaterialBuff.SetDiffColor(targetMaterial.DiffColor);
		MaterialBuff.SetAmbColor(targetMaterial.AmbColor);
		MaterialBuff.SetSpecColor(targetMaterial.SpecColor);
		MaterialBuff.SetSpecPower(targetMaterial.SpecPower);

		// 상수버퍼 갱신
		VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);
	}

	if (bNormal)
	{
		inputTexture = VariableManagerInstance->GetTexture(targetMaterial.TexNameNormal);
		this->DeviceContextRef->PSSetShaderResources(1, 1, &inputTexture);
	}

	else
	{
		inputTexture = nullptr;
		this->DeviceContextRef->PSSetShaderResources(1, 1, &inputTexture);
	}

	if (bARM)
	{
		inputTexture = VariableManagerInstance->GetTexture(targetMaterial.TexNameARM);
		this->DeviceContextRef->PSSetShaderResources(2, 1, &inputTexture);
	}

	else
	{
		inputTexture = nullptr;
		this->DeviceContextRef->PSSetShaderResources(2, 1, &inputTexture);
	}

	if (bEmiss)
	{
		inputTexture = VariableManagerInstance->GetTexture(targetMaterial.TexNameEmiss);
		this->DeviceContextRef->PSSetShaderResources(3, 1, &inputTexture);
	}

	else
	{
		inputTexture = nullptr;
		this->DeviceContextRef->PSSetShaderResources(3, 1, &inputTexture);
	}

	return bAlbedo;
}

void hos::cg::D3D11Renderder::DrawByShaderType(RenderQue::RenderObject& targetQue, bool bAlpha, bool bDeferred, bool bAlbedo, unsigned int cameraIndex, unsigned int indexCount)
{
	SHADER_LIST ShaderType = SHADER_LIST::SHADER_ERROR;	// 미리 에러
	SHADER_LIST ShaderDEPTH = SHADER_LIST::SHADER_ERROR;	// 미리 에러
	SHADER_LIST ShaderShadow = SHADER_LIST::SHADER_ERROR;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 	   쉐이더 판정
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 스키닝
	if (targetQue.bSkinning)
	{
		// 포워드
		if (bDeferred == false)
		{
			ShaderType = SHADER_LIST::SHADER_SKIN_PBR;
		}

		ShaderDEPTH = SHADER_LIST::SHADER_MAP_DEPTH_SKIN;
		ShaderShadow = SHADER_LIST::SHADER_SOFTSHADOW_SKIN;
	}

	// 스키닝 X
	else
	{
		// 포워드
		if (bDeferred == false)
		{
			ShaderType = SHADER_LIST::SHADER_PBR;
		}

		ShaderDEPTH = SHADER_LIST::SHADER_MAP_DEPTH;
		ShaderShadow = SHADER_LIST::SHADER_SOFTSHADOW;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 	   그리기
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 포워드 그리기
	if (bDeferred == false)
	{
		// 렌더타겟 세팅
		PreRenderTarget->SetRenderTarget();

		// 그림자 세팅
		ID3D11ShaderResourceView* inputTexture = RenderTarget_LightViewShadowMap->GetShaderResourceView();
		this->DeviceContextRef->PSSetShaderResources(6, 1, &inputTexture);

		// 알파 체크
		if (bAlpha == true)
		{
			/// 알파모델 내부 Render 위해
			SetBlendState(BS_ALPHA_BLEND);

			// 전면 컬링으로 1번 그리고
			SetRasterizeState(RS_CULLFRONT);
			VariableManagerInstance->DrawByIndex(ShaderType, indexCount);

			// 후면 컬링으로 다시 1번 그려준다
			SetRasterizeState(RS_CULLBACK);
			VariableManagerInstance->DrawByIndex(ShaderType, indexCount);
		}

		else
		{
			VariableManagerInstance->DrawByIndex(ShaderType, indexCount);
		}

		SetBlendState(BS_DEFAULT);

		// Depth 그리기
		this->RenderTarget_NowDepth->SetRenderTarget();
		VariableManagerInstance->DrawByIndex(ShaderDEPTH, indexCount);

		// Shadow 그리기
		this->RT_SoftShadow->SetRenderTarget();
		VariableManagerInstance->DrawByIndex(ShaderShadow, indexCount);

		// 리소스 제거
		inputTexture = nullptr;
		this->DeviceContextRef->PSSetShaderResources(6, 1, &inputTexture);
	}


}

void hos::cg::D3D11Renderder::DrawUIObject_3D(RenderQue::RenderUIObejct& targetQue, float screenWidth, float screenHeight)
{
	SetRasterizeState(RS_NODEPTH);
	SetDepthStencilState(DS_DEPTH_OFF);
	SetBlendState(BS_ALPHA_BLEND);

	Vector3 vPos, vScale, vRot;
	float width = (float)targetQue.Width;
	float height = (float)targetQue.Height;
	//width *= targetQue.Aspect.x;
	//height *= targetQue.Aspect.y;

	if (targetQue.bFollow)
	{
		Vector4 ProjPos = Vector4(targetQue.Pos.x, targetQue.Pos.y, targetQue.Pos.z, 1.0f);

		Matrix mView = this->CameraViewTM;
		Matrix mProj = this->CameraProjTM;

		ProjPos = Vector4::Transform(ProjPos, mView);
		ProjPos = Vector4::Transform(ProjPos, mProj);

		ProjPos /= ProjPos.w;

		float viewPortX = (ProjPos.x * 0.5f + 0.5f) * screenWidth;
		float viewPortY = (ProjPos.y * -0.5f + 0.5f) * screenHeight;

		vPos.x = viewPortX + targetQue.CoordU;
		vPos.y = viewPortY + targetQue.CoordV;
		vPos.z = 1.0f;

		vScale = targetQue.Scale;
	}

	else
	{
		vPos.x = targetQue.Pos.x * targetQue.Aspect.x;
		vPos.y = targetQue.Pos.y * targetQue.Aspect.y;
		vPos.z = 1.0f;

		vScale = targetQue.Scale * targetQue.Aspect;
	}

	vRot = targetQue.Rot;

	if (targetQue.bAni)
	{
		vPos.x += targetQue.vAniPos.x * vScale.x;
		vPos.y -= targetQue.vAniPos.y * vScale.y;

		vRot += targetQue.vAniRot;

		vScale.x = vScale.x * targetQue.vAniScale.x;
		vScale.y = vScale.y * targetQue.vAniScale.y;
	}

	Matrix mTrans = Matrix::CreateTranslation(vPos.x - screenWidth / 2, -vPos.y + screenHeight / 2, 0.0f);
	Matrix mRot = Matrix::CreateFromYawPitchRoll(vRot.y, vRot.x, -vRot.z);
	Matrix mScale = Matrix::CreateScale(width * vScale.x, height * vScale.y, 1.0f);

	Matrix mTM = mScale * mRot * mTrans;//

	// 기본 상수버퍼 갱신
	MatrixBuff.SetWorldTM(mTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// VariableManager에서 Buffer를 가져온다.
	ID3D11Buffer* VB = VariableManagerInstance->Get3DUIBuffer(targetQue.Align);

	// 버퍼 설정.
	UINT stride = sizeof(VertexDataType::UI_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 재질 갱신
	Vector4 _color = targetQue.Color;
	Vector4 _offTill = targetQue.OffTill;
	MaterialBuff.SetDiffColor(_color);
	MaterialBuff.SetOffTill(_offTill);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);

	// 텍스쳐 유무 및 쉐이더 갱신
	SHADER_LIST ShaderType = SHADER_LIST::SHADER_ERROR;	// 미리 에러
	if (targetQue.bTexture)	// 있음
	{
		ID3D11ShaderResourceView* inputTexture = VariableManagerInstance->GetTexture(targetQue.TextureName);

		if (inputTexture)
		{
			ShaderType = SHADER_LIST::SHADER_TEXON_UI;
			this->DeviceContextRef->PSSetShaderResources(0, 1, &inputTexture);
		}

		else
		{
			DEBUG_CONSOLET(targetQue.TextureName, L"D3D11_VariableManager에 해당 name의 Texture가 존재하지 않습니다.");
		}
	}

	else // 없음
	{
		ShaderType = SHADER_LIST::SHADER_TEXOFF_UI;
	}

	// 그리기
	VariableManagerInstance->DrawByVertex(ShaderType, 4);
}

void hos::cg::D3D11Renderder::DrawSkyBox(std::wstring_view skyboxName, unsigned int cameraIndex)
{
	// Get SkyBoxTexture
	ID3D11ShaderResourceView* _skyBoxTexture = VariableManagerInstance->GetSkyBox(skyboxName);
	if (_skyBoxTexture == nullptr)
	{
		DEBUG_CONSOLET(skyboxName.data(), L"해당 Key의 SkyBox Texture가 존재하지 않습니다.");
		return;
	}

	// Set RenderTarget
	PreRenderTarget->SetRenderTarget();

	// Set RenderState
	SetRasterizeState(RS_NODEPTH);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_OFF);

	// Update ConstBuffer
	this->MatrixBuff.SetViewTM(this->CameraViewTM);
	this->MatrixBuff.SetProjTM(this->CameraProjTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// Set VertexBuffer Data
	ID3D11Buffer* _skyboxBuffer = VariableManagerInstance->GetSkyBoxBuffer();
	UINT stride = sizeof(Vector3);
	UINT offset = 0;
	DeviceContextRef->IASetVertexBuffers(0, 1, &_skyboxBuffer, &stride, &offset);
	DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set Texture
	DeviceContextRef->PSSetShaderResources(0, 1, &_skyBoxTexture);	//PS 에 텍스처 설정.

	// Draw by Shader
	VariableManagerInstance->DrawByVertex(SHADER_LIST::SHADER_SKYBOX, 4);
}

void hos::cg::D3D11Renderder::DrawFontText(RenderQue::RenderUIObejct& targetQue)
{
	SpriteBatch* _batch = VariableManagerInstance->GetBatch();
	SpriteFont* _font = VariableManagerInstance->GetFont(targetQue.FontName);

	// 텍스트 사이즈 측정
	Vector2 size = _font->MeasureString(targetQue.Text.data());
	Vector2 origin;

	Vector3 vPos, vScale;

	vScale.x = targetQue.Scale.x * targetQue.Aspect.x;
	vScale.y = targetQue.Scale.y * targetQue.Aspect.y;
	float vRot = targetQue.Rot.z + targetQue.FontRot;

	float screenWidth = this->RenderDesc.ScreenWidth;
	float screenHeight = this->RenderDesc.ScreenHeight;

	if (targetQue.bFollow)
	{
		Vector4 ProjPos = Vector4(targetQue.Pos.x, targetQue.Pos.y, targetQue.Pos.z, 1.0f);

		Matrix mView = this->CameraViewTM;
		Matrix mProj = this->CameraProjTM;

		ProjPos = Vector4::Transform(ProjPos, mView);
		ProjPos = Vector4::Transform(ProjPos, mProj);

		ProjPos /= ProjPos.w;

		float viewPortX = (ProjPos.x * 0.5f + 0.5f) * screenWidth;
		float viewPortY = (ProjPos.y * -0.5f + 0.5f) * screenHeight;

		vPos.x = viewPortX + targetQue.CoordU;
		vPos.y = viewPortY + targetQue.CoordV;
		vPos.z = 1.0f;

		vScale = targetQue.Scale;
	}

	else
	{
		vPos.x = targetQue.Pos.x * targetQue.Aspect.x;
		vPos.y = targetQue.Pos.y * targetQue.Aspect.y;
		vPos.z = 1.0f;

		vScale = targetQue.Scale * targetQue.Aspect;
	}

	// 애니메이션 처리
	if (targetQue.bAni)
	{
		vPos.x += targetQue.vAniPos.x * vScale.x;
		vPos.y -= targetQue.vAniPos.y * vScale.y;

		vRot += targetQue.vAniRot.z;

		vScale.x = vScale.x * targetQue.vAniScale.x;
		vScale.y = vScale.y * targetQue.vAniScale.y;
	}


	// Align 설정에 따른 처리
	switch (targetQue.Align)
	{
	case 1:
		origin.y = size.y;
		break;

	case 2:
		origin.x = size.x * 0.5f;
		origin.y = size.y;
		break;

	case 3:
		origin.x = size.x;
		origin.y = size.y;
		break;

	case 4:
		origin.y = size.y * 0.5f;
		break;

	case 5:
		origin = size * 0.5f;
		break;

	case 6:
		origin.x = size.x;
		origin.y = size.y * 0.5f;
		break;

	default:
	case 7:
		break;

	case 8:
		origin.x = size.x * 0.5f;
		break;

	case 9:
		origin.x = size.x;
		break;
	}
	this->BeginFont(this->D3D11Instance->GetAlphaBlendState());
	_font->DrawString(_batch, targetQue.Text.data(), vPos, targetQue.Color, vRot, origin, Vector2(targetQue.FontSize * 0.01f * vScale.x, targetQue.FontSize * 0.01f * vScale.y));
	this->EndFont();
}

DirectX::SimpleMath::Vector2 hos::cg::D3D11Renderder::GetTextBoundSize(std::wstring_view text, float fontSize, Vector2 scale, std::wstring_view FontName)
{
	SpriteFont* _font = VariableManagerInstance->GetFont(FontName.data());

	RECT _boundRect = _font->MeasureDrawBounds(text.data(), Vector2(0, 0));

	Vector2 _size;
	_size.x = (float)_boundRect.right;
	_size.y = (float)_boundRect.bottom;
	_size *= fontSize / 100 * scale;

	return _size;
}

void hos::cg::D3D11Renderder::DrawSkinShadowMap(RenderQue::RenderObject& targetQue)
{
	// 노드 관련 정보 획득
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;

	// 행렬 설정
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// 스킨 행렬 설정
	BoneBuff.SetBoneTM(targetQue.mSkinnedAni, targetQue.BoneCount);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_BONE);

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet& _meshRef = VariableManagerInstance->GetMeshBuffer(targetQue.MeshName);
	ID3D11Buffer* VB = _meshRef.VertexBuffer;
	ID3D11Buffer* IB = _meshRef.IndexBuffer;
	unsigned int _indexCount = _meshRef.IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 쉐이더로 그리기
	VariableManagerInstance->DrawByIndex(SHADER_LIST::SHADER_MAP_SHADOW_SKIN, _indexCount);
}

void hos::cg::D3D11Renderder::DrawShadowMap(RenderQue::RenderObject& targetQue)
{
	// 노드 관련 정보 획득
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;

	// 행렬 설정
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet& _meshRef = VariableManagerInstance->GetMeshBuffer(targetQue.MeshName);
	ID3D11Buffer* VB = _meshRef.VertexBuffer;
	ID3D11Buffer* IB = _meshRef.IndexBuffer;
	unsigned int _indexCount = _meshRef.IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 쉐이더로 그리기
	VariableManagerInstance->DrawByIndex(SHADER_LIST::SHADER_MAP_SHADOW, _indexCount);
}

void hos::cg::D3D11Renderder::DrawSkinEmissMap(RenderQue::RenderObject& targetQue)
{
	// 노드 관련 정보 획득
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;

	// 행렬 설정
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// 스킨 행렬 설정
	BoneBuff.SetBoneTM(targetQue.mSkinnedAni, targetQue.BoneCount);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_BONE);

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet& _meshRef = VariableManagerInstance->GetMeshBuffer(targetQue.MeshName);
	ID3D11Buffer* VB = _meshRef.VertexBuffer;
	ID3D11Buffer* IB = _meshRef.IndexBuffer;
	unsigned int _indexCount = _meshRef.IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Texutre Set
	ID3D11ShaderResourceView* inputTexture = VariableManagerInstance->GetTexture(targetQue.MaterialInfo.TexNameEmiss);
	this->DeviceContextRef->PSSetShaderResources(3, 1, &inputTexture);

	// 쉐이더로 그리기
	VariableManagerInstance->DrawByIndex(SHADER_LIST::SHADER_MAP_EMISS_SKIN, _indexCount);
}

void hos::cg::D3D11Renderder::DrawEmissMap(RenderQue::RenderObject& targetQue)
{
	// 노드 관련 정보 획득
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;

	// 행렬 설정
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet& _meshRef = VariableManagerInstance->GetMeshBuffer(targetQue.MeshName);
	ID3D11Buffer* VB = _meshRef.VertexBuffer;
	ID3D11Buffer* IB = _meshRef.IndexBuffer;
	unsigned int _indexCount = _meshRef.IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Texutre Set
	ID3D11ShaderResourceView* inputTexture = VariableManagerInstance->GetTexture(targetQue.MaterialInfo.TexNameEmiss);
	this->DeviceContextRef->PSSetShaderResources(3, 1, &inputTexture);

	// 쉐이더로 그리기
	VariableManagerInstance->DrawByIndex(SHADER_LIST::SHADER_MAP_EMISS, _indexCount);
}

void hos::cg::D3D11Renderder::DrawDebugGrid(unsigned int cameraIndex)
{
	// 렌더타겟 세팅
	PreRenderTarget->SetRenderTarget();

	// Set RenderState
	SetRasterizeState(RS_SOLID);
	SetBlendState(BS_DEFAULT);
	SetDepthStencilState(DS_DEPTH_ON);

	// 행렬 세팅: Grid와 Axis 공용
	MatrixBuff.SetWorldTM(DirectX::SimpleMath::Matrix::Identity);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// Grid Draw
	DebugGridInstance->RenderGrid();
	VariableManagerInstance->GetShader(SHADER_LIST::SHADER_LINE)->Draw_Index(DebugGridInstance->GetGridIndexCount());

	// Axis Draw
	DebugGridInstance->RenderAxis();
	VariableManagerInstance->GetShader(SHADER_LIST::SHADER_LINE)->Draw_Index(DebugGridInstance->GetAxisIndexCount());
}

void hos::cg::D3D11Renderder::DrawDebugPrimitive(RenderQue::RenderObject& targetQue)
{
	// 행렬 획득 및 세팅
	DirectX::SimpleMath::Matrix mUserTM = targetQue.mTM;
	MatrixBuff.SetWorldTM(mUserTM);
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_DEFAULT);

	// VariableManager에서 Mesh Data를 가져온다.
	D3D11VariableManager::MeshBufferSet* _meshRef = nullptr;
	switch (targetQue.PrimitiveType)
	{
	case RenderQue::RenderPrimitiveType::BOX:
	{
		_meshRef = &(VariableManagerInstance->GetMeshBuffer(L"DebugBox"));
	}
	break;

	case RenderQue::RenderPrimitiveType::SPHERE:
	{
		_meshRef = &(VariableManagerInstance->GetMeshBuffer(L"DebugSphere"));
	}
	break;

	case RenderQue::RenderPrimitiveType::CAPSULE:
	{
		_meshRef = &(VariableManagerInstance->GetMeshBuffer(L"DebugCapsule"));
	}
	break;

	default:
	{
		_meshRef = &(VariableManagerInstance->GetMeshBuffer(L"DebugBox"));
	}
	break;
	}

	ID3D11Buffer* VB = _meshRef->VertexBuffer;
	ID3D11Buffer* IB = _meshRef->IndexBuffer;
	unsigned int _indexCount = _meshRef->IndexCount;

	// 인덱스버퍼 설정.
	UINT stride = sizeof(VertexDataType::FULL_VTX);
	UINT offset = 0;
	this->DeviceContextRef->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	this->DeviceContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, offset);

	// 기하 위상 구조 설정
	this->DeviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11ShaderResourceView* inputTexture = nullptr;

	// 재질 및 텍스쳐 설정
	if (targetQue.CollisionState == true)
	{
		MaterialBuff.SetDiffColor(Vector4(1, 0, 0, 1));
	}

	else
	{
		MaterialBuff.SetDiffColor(Vector4(0, 1, 0, 1));
	}
	VariableManagerInstance->UpdateConstBuffer((unsigned int)ConstBufferType::CB_MATERIAL);

	// 쉐이더 갱신 및 그리기
	VariableManagerInstance->DrawByIndex(SHADER_LIST::SHADER_PRIMITIVE, _indexCount);

}

void D3D11Renderder::CreateAllConstBuffer()
{
	// 변수 매니저가 초기화 되지 않았다면?
	if (!VariableManagerInstance)
	{
		DEBUG_CONSOLE(L"D3D11_Variablemanager가 생성되지 않았습니다.");
		return;
	}

	// Default 상수버퍼 생성
	VariableManagerInstance->CreateConstBuffer(this->DeviceRef, this->DeviceContextRef, &MatrixBuff, sizeof(MatrixBuffer), ConstBufferType::CB_DEFAULT, L"CB_DEFAULT");

	// Mateiral 상수버퍼 생성
	VariableManagerInstance->CreateConstBuffer(this->DeviceRef, this->DeviceContextRef, &MaterialBuff, sizeof(MaterialBuffer), ConstBufferType::CB_MATERIAL, L"CB_MATERIAL");

	// Bone 상수버퍼 생성
	VariableManagerInstance->CreateConstBuffer(this->DeviceRef, this->DeviceContextRef, &BoneBuff, sizeof(BoneBuffer), ConstBufferType::CB_BONE, L"CB_BONE");

	// Light 상수버퍼 생성
	VariableManagerInstance->CreateConstBuffer(this->DeviceRef, this->DeviceContextRef, &LightBuff, sizeof(LightBuffer), ConstBufferType::CB_LIGHT, L"CB_LIGHT");

	// Screen 상수버퍼 생성
	VariableManagerInstance->CreateConstBuffer(this->DeviceRef, this->DeviceContextRef, &ScreenBuff, sizeof(ScreenBuffer), ConstBufferType::CB_SCREEN, L"CB_SCREEN");

	DEBUG_CONSOLE(L"모든 상수버퍼 생성에 성공했습니다.");
}

void D3D11Renderder::CreateAllShader()
{
	// 변수 매니저가 초기화 되지 않았다면?
	if (!VariableManagerInstance)
	{
		DEBUG_CONSOLE(L"D3D11_Variablemanager가 생성되지 않았습니다.");
		return;
	}

	// Shader 생성
	std::vector<ID3D11SamplerState*>& _samplerList = this->VariableManagerInstance->GetSamplerList();

	// 0번 쉐이더: Debug Primitive
	WCHAR shaderRoot0_1[64] = L"./Assets/Shader/Primitive_VS.fxo";
	WCHAR shaderRoot0_2[64] = L"./Assets/Shader/Primitive_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot0_1, shaderRoot0_2, (int)SHADER_LIST::SHADER_PRIMITIVE, L"SHADER_PRIMITIVE", _samplerList);

	// 1번 쉐이더: Debug_Grid
	WCHAR shaderRoot1_1[64] = L"./Assets/Shader/Line_VS.fxo";
	WCHAR shaderRoot1_2[64] = L"./Assets/Shader/Line_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot1_1, shaderRoot1_2, (int)SHADER_LIST::SHADER_LINE, L"SHADER_LINE", _samplerList);

	// 2번 쉐이더: Error
	WCHAR shaderRoot2_1[64] = L"./Assets/Shader/Error_VS.fxo";
	WCHAR shaderRoot2_2[64] = L"./Assets/Shader/Error_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot2_1, shaderRoot2_2,(int)SHADER_LIST::SHADER_ERROR, L"SHADER_ERROR", _samplerList);

	// 3번 쉐이더: 정적 객체 PBR
	WCHAR shaderRoot3_1[64] = L"./Assets/Shader/Pbr_VS.fxo";
	WCHAR shaderRoot3_2[64] = L"./Assets/Shader/Pbr_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot3_1, shaderRoot3_2, (int)SHADER_LIST::SHADER_PBR, L"SHADER_PBR", _samplerList);

	// 4번 쉐이더: 스키닝 객체 PBR
	WCHAR shaderRoot4_1[64] = L"./Assets/Shader/PBR_Skin_VS.fxo";
	WCHAR shaderRoot4_2[64] = L"./Assets/Shader/PBR_Skin_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot4_1, shaderRoot4_2, (int)SHADER_LIST::SHADER_SKIN_PBR, L"SHADER_SKIN_PBR", _samplerList);

	// 5번 쉐이더: TEXOFF_UI
	WCHAR shaderRoot5_1[64] = L"./Assets/Shader/TexOff_UI_VS.fxo";
	WCHAR shaderRoot5_2[64] = L"./Assets/Shader/TexOff_UI_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot5_1, shaderRoot5_2, (int)SHADER_LIST::SHADER_TEXOFF_UI, L"SHADER_TEXOFF_UI", _samplerList);

	// 6번 쉐이더: TEXON_UI
	WCHAR shaderRoot6_1[64] = L"./Assets/Shader/TexOn_UI_VS.fxo";
	WCHAR shaderRoot6_2[64] = L"./Assets/Shader/TexOn_UI_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot6_1, shaderRoot6_2, (int)SHADER_LIST::SHADER_TEXON_UI, L"SHADER_TEXON_UI", _samplerList);

	// 7번 쉐이더: SKYBOX
	WCHAR shaderRoot7_1[64] = L"./Assets/Shader/SkyBox_VS.fxo";
	WCHAR shaderRoot7_2[64] = L"./Assets/Shader/SkyBox_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot7_1, shaderRoot7_2, (int)SHADER_LIST::SHADER_SKYBOX, L"SHADER_SKYBOX", _samplerList);

	// 8번 쉐이더: Depth Map
	WCHAR shaderRoot8_1[64] = L"./Assets/Shader/Map_Depth_VS.fxo";
	WCHAR shaderRoot8_2[64] = L"./Assets/Shader/Map_Depth_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot8_1, shaderRoot8_2, (int)SHADER_LIST::SHADER_MAP_DEPTH, L"SHADER_MAP_DEPTH", _samplerList);

	// 9번 쉐이더: Depth Map_SKIN
	WCHAR shaderRoot9_1[64] = L"./Assets/Shader/Map_Depth_Skin_VS.fxo";
	WCHAR shaderRoot9_2[64] = L"./Assets/Shader/Map_Depth_Skin_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot9_1, shaderRoot9_2, (int)SHADER_LIST::SHADER_MAP_DEPTH_SKIN, L"SHADER_MAP_DEPTH_SKIN", _samplerList);

	// 10번 쉐이더: SHADOWMAP
	WCHAR shaderRoot10_1[64] = L"./Assets/Shader/Map_Shadow_VS.fxo";
	WCHAR shaderRoot10_2[64] = L"./Assets/Shader/Map_Shadow_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot10_1, shaderRoot10_2, (int)SHADER_LIST::SHADER_MAP_SHADOW, L"SHADER_MAP_SHADOW", _samplerList);

	// 11번 쉐이더: SHADOWMAP_SKIN
	WCHAR shaderRoot11_1[64] = L"./Assets/Shader/Map_Shadow_Skin_VS.fxo";
	WCHAR shaderRoot11_2[64] = L"./Assets/Shader/Map_Shadow_Skin_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot11_1, shaderRoot11_2, (int)SHADER_LIST::SHADER_MAP_SHADOW_SKIN, L"SHADER_MAP_SHADOW_SKIN", _samplerList);

	// 12번 쉐이더: EMISS_MAP용
	WCHAR shaderRoot12_1[64] = L"./Assets/Shader/Map_Emiss_VS.fxo";
	WCHAR shaderRoot12_2[64] = L"./Assets/Shader/Map_Emiss_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot12_1, shaderRoot12_2, (int)SHADER_LIST::SHADER_MAP_EMISS, L"SHADER_MAP_EMISS", _samplerList);

	// 13번 쉐이더: EMISS_MAP_SKIN용
	WCHAR shaderRoot13_1[64] = L"./Assets/Shader/Map_Emiss_Skin_VS.fxo";
	WCHAR shaderRoot13_2[64] = L"./Assets/Shader/Map_Emiss_Skin_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot13_1, shaderRoot13_2, (int)SHADER_LIST::SHADER_MAP_EMISS_SKIN, L"SHADER_MAP_EMISS_SKIN", _samplerList);

	// 14번 쉐이더: X_GBlur
	WCHAR shaderRoot14_1[64] = L"./Assets/Shader/XBlur_VS.fxo";
	WCHAR shaderRoot14_2[64] = L"./Assets/Shader/XBlur_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot14_1, shaderRoot14_2, (int)SHADER_LIST::SHADER_GBLUR_X, L"SHADER_GBLUR_X", _samplerList);

	// 15번 쉐이더: Y_GBlur
	WCHAR shaderRoot15_1[64] = L"./Assets/Shader/YBlur_VS.fxo";
	WCHAR shaderRoot15_2[64] = L"./Assets/Shader/YBlur_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot15_1, shaderRoot15_2, (int)SHADER_LIST::SHADER_GBLUR_Y, L"SHADER_GBLUR_Y", _samplerList);

	// 16번 쉐이더: VelocityMap
	WCHAR shaderRoot16_1[64] = L"./Assets/Shader/Map_Velocity_VS.fxo";
	WCHAR shaderRoot16_2[64] = L"./Assets/Shader/Map_Velocity_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot16_1, shaderRoot16_2, (int)SHADER_LIST::SHADER_MAP_VELOCITY, L"SHADER_MAP_VELOCITY", _samplerList);

	// 17번 쉐이더: Bloom Curve
	WCHAR shaderRoot17_1[64] = L"./Assets/Shader/BloomCurve_VS.fxo";
	WCHAR shaderRoot17_2[64] = L"./Assets/Shader/BloomCurve_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot17_1, shaderRoot17_2, (int)SHADER_LIST::SHADER_BLOOMCURVE, L"SHADER_BLOOMCURVE", _samplerList);

	// 18번 쉐이더: Screen Combine
	WCHAR shaderRoot18_1[64] = L"./Assets/Shader/CombineScreen_VS.fxo";
	WCHAR shaderRoot18_2[64] = L"./Assets/Shader/CombineScreen_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot18_1, shaderRoot18_2, (int)SHADER_LIST::SHADER_COMBINE, L"SHADER_COMBINE", _samplerList);

	// 19번 쉐이더: SoftShadow
	WCHAR shaderRoot19_1[64] = L"./Assets/Shader/SoftShadow_VS.fxo";
	WCHAR shaderRoot19_2[64] = L"./Assets/Shader/SoftShadow_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot19_1, shaderRoot19_2, (int)SHADER_LIST::SHADER_SOFTSHADOW, L"SHADER_SOFTSHADOW", _samplerList);

	// 20번 쉐이더: SoftShadow_SKIN
	WCHAR shaderRoot20_1[64] = L"./Assets/Shader/SoftShadow_Skin_VS.fxo";
	WCHAR shaderRoot20_2[64] = L"./Assets/Shader/SoftShadow_Skin_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot20_1, shaderRoot20_2, (int)SHADER_LIST::SHADER_SOFTSHADOW_SKIN, L"SHADER_SOFTSHADOW_SKIN", _samplerList);

	// 21번 쉐이더: ToSwapChain용(최종 렌더)
	WCHAR shaderRoot21_1[64] = L"./Assets/Shader/ToSwapChain_VS.fxo";
	WCHAR shaderRoot21_2[64] = L"./Assets/Shader/ToSwapChain_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot21_1, shaderRoot21_2, (int)SHADER_LIST::SHADER_TOSWAPCHAIN, L"SHADER_TOSWAPCHAIN", _samplerList);

	// 22번 쉐이더: ToSwapChain_MotionBlur용(최종 렌더)
	WCHAR shaderRoot22_1[64] = L"./Assets/Shader/ToSwapChain_MotionBlur_VS.fxo";
	WCHAR shaderRoot22_2[64] = L"./Assets/Shader/ToSwapChain_MotionBlur_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot22_1, shaderRoot22_2, (int)SHADER_LIST::SHADER_TOSWAPCHAIN_MOTIONNBLUR, L"SHADER_TOSWAPCHAIN_MOTIONNBLUR", _samplerList);

	// 23번 쉐이더: ToSwapChain_RadialBlur용(최종 렌더)
	WCHAR shaderRoot23_1[64] = L"./Assets/Shader/ToSwapChain_RadialBlur_VS.fxo";
	WCHAR shaderRoot23_2[64] = L"./Assets/Shader/ToSwapChain_RadialBlur_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot23_1, shaderRoot23_2, (int)SHADER_LIST::SHADER_TOSWAPCHAIN_RADIALBLUR, L"SHADER_TOSWAPCHAIN_RADIALBLUR", _samplerList);

	// 24번 쉐이더: ToSwapChain_MotionBlur + RadialBlur용(최종 렌더)
	WCHAR shaderRoot24_1[64] = L"./Assets/Shader/ToSwapChain_AllBlur_VS.fxo";
	WCHAR shaderRoot24_2[64] = L"./Assets/Shader/ToSwapChain_AllBlur_PS.fxo";
	VariableManagerInstance->CreateShader(shaderRoot24_1, shaderRoot24_2, (int)SHADER_LIST::SHADER_TOSWAPCHAIN_ALLBLUR, L"SHADER_TOSWAPCHAIN_ALLBLUR", _samplerList);

	// TODO: 총 갯수가 SHADER_MAX와 맞는지 체크하기
	DEBUG_CONSOLE(L"모든 쉐이더 생성에 성공했습니다.");
}

void hos::cg::D3D11Renderder::CreateAllSampler()
{
	for (int i = 0; i < (int)TEXMODE_LIST::SS_MAX; i++)
	{
		this->VariableManagerInstance->CreateSamplerIndex(i);
	}
}

void hos::cg::D3D11Renderder::CreateExtraRenderTarget()
{
	unsigned int _screenWidth = this->RenderDesc.ScreenWidth;
	unsigned int _screenHeight = this->RenderDesc.ScreenHeight;
	unsigned int _valueAA = this->RenderDesc.ValueAA;

	// 카메라용 렌더타겟
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		try
		{
			PreRenderTarget = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth, _screenHeight,
				D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, this->RenderDesc.ValueAA);
		}

		catch (...)
		{
			DEBUG_CONSOLE(i + L"번 Camera RenderTarget 생성에 실패했습니다.");
		}
	}

	// 디퍼드용 렌더타겟
	try
	{
		RenderTarget_NowDepth = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth, _screenHeight, D3D11ExtraRenderTarget::RenderTargetType::R32_MAP, 1);
		RT_SoftShadow = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth, _screenHeight, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);
		RenderTarget_LightViewShadowMap = new D3D11ShadowBuffer(this->DeviceRef, this->DeviceContextRef, SHADOWMAPSIZE, SHADOWMAPSIZE);

		RT_DownSample_x4 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_1, _screenHeight / DOWNSAMPLESIZE_1, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);
		RT_DownSample_x4_2 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_1, _screenHeight / DOWNSAMPLESIZE_1, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);

		RT_DownSample_x16 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_2, _screenHeight / DOWNSAMPLESIZE_2, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);
		RT_DownSample_x16_2 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_2, _screenHeight / DOWNSAMPLESIZE_2, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);

		RT_DownSample_x64 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_3, _screenHeight / DOWNSAMPLESIZE_3, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);
		RT_DownSample_x64_2 = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth / DOWNSAMPLESIZE_3, _screenHeight / DOWNSAMPLESIZE_3, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);

		RT_Velocity = new D3D11ExtraRenderTarget(this->DeviceRef, this->DeviceContextRef, _screenWidth, _screenHeight, D3D11ExtraRenderTarget::RenderTargetType::RGBA32_MAP, 1);
	}

	catch (...)
	{
		DEBUG_CONSOLE(L"Extra RenderTarget Group 생성에 실패했습니다.");
	}
}

void hos::cg::D3D11Renderder::ResizeExtraRenderTarget(unsigned int screenWidth, unsigned int screenHeight, unsigned int valueAA)
{
	PreRenderTarget->Resize(screenWidth, screenHeight, valueAA);
	RT_SoftShadow->Resize(screenWidth, screenHeight, 1);
	RenderTarget_NowDepth->Resize(screenWidth, screenHeight, 1);
	RT_DownSample_x4->Resize(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1, 1);
	RT_DownSample_x4_2->Resize(screenWidth / DOWNSAMPLESIZE_1, screenHeight / DOWNSAMPLESIZE_1, 1);
	RT_DownSample_x16->Resize(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2, 1);
	RT_DownSample_x16_2->Resize(screenWidth / DOWNSAMPLESIZE_2, screenHeight / DOWNSAMPLESIZE_2, 1);
	RT_DownSample_x64->Resize(screenWidth / DOWNSAMPLESIZE_3, screenHeight / DOWNSAMPLESIZE_3, 1);
	RT_DownSample_x64_2->Resize(screenWidth / DOWNSAMPLESIZE_3, screenHeight / DOWNSAMPLESIZE_3, 1);
	RT_Velocity->Resize(screenWidth, screenHeight, 1);
}

void hos::cg::D3D11Renderder::BeginFont(ID3D11BlendState* state)
{
	VariableManagerInstance->BeginFont(state);
}

void hos::cg::D3D11Renderder::EndFont()
{
	VariableManagerInstance->EndFont();
}