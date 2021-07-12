#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"

using namespace hos;

bool g_bDeferred = false;
unsigned int g_outputMap = 0;

hos::com::Camera::Camera(eMode mode, Vector3 followTarget, eViewMode view, eClearFlag flag, float fov, float far, float near)
	: Component(L"Camera"), ViewPortLeft(0.0f), ViewPortRight(1.0f), ViewPortTop(0.0f), ViewPortBottom(1.0f),
	AspectRatio(1.777777f), ScreenWidth(1920), ScreenHeight(1080),
	FOV(fov), Near(near), Far(far), FollowTarget(followTarget), ClearColor(Color(0.f, 0.5f, 0.f)),
	ViewMatrix(Matrix::Identity), ProjMatrix(Matrix::Identity), Mode(mode), ViewMode(view), ClearFlag(flag), SkyBoxName(L""),
	MortionBlurSampleCount(0), RadialBlurState(false)
{
}

hos::com::Camera::~Camera()
{
}

hos::com::Camera::Camera(const Camera& dest) :
	Component(dest),
	ViewPortLeft(dest.ViewPortLeft),
	ViewPortRight(dest.ViewPortRight),
	ViewPortTop(dest.ViewPortTop),
	ViewPortBottom(dest.ViewPortBottom),
	AspectRatio(dest.AspectRatio),
	FOV(dest.FOV),
	Near(dest.Near),
	Far(dest.Far),
	FollowTarget(dest.FollowTarget),
	ClearColor(dest.ClearColor),
	ViewMatrix(dest.ViewMatrix),
	ProjMatrix(dest.ProjMatrix),
	Mode(dest.Mode),
	ViewMode(dest.ViewMode),
	ClearFlag(dest.ClearFlag),
	SkyBoxName(dest.SkyBoxName),
	MortionBlurSampleCount(0),
	RadialBlurState(false)
{
}

float hos::com::Camera::GetViewPortLeft() const
{
	return this->ViewPortLeft;
}

float hos::com::Camera::GetViewPortRight() const
{
	return this->ViewPortRight;
}

float hos::com::Camera::GetViewPortTop() const
{
	return this->ViewPortTop;
}

float hos::com::Camera::GetViewPortBottom() const
{
	return this->ViewPortBottom;
}

void hos::com::Camera::SetViewPortLeft(float ratioValue)
{
	this->ViewPortLeft = ratioValue;
}

void hos::com::Camera::SetViewPortRight(float ratioValue)
{
	this->ViewPortRight = ratioValue;
}

void hos::com::Camera::SetViewPortTop(float ratioValue)
{
	this->ViewPortTop = ratioValue;
}

void hos::com::Camera::SetViewPortBottom(float ratioValue)
{
	this->ViewPortBottom = ratioValue;
}

void hos::com::Camera::SetViewPort(float left, float right, float top, float bottom)
{
	this->ViewPortLeft = left;
	this->ViewPortRight = right;
	this->ViewPortTop = top;
	this->ViewPortBottom = bottom;
}

float hos::com::Camera::GetFOV() const
{
	return FOV;
}

void hos::com::Camera::SetFOV(float fov)
{
	FOV = fov;
}

float hos::com::Camera::GetNear() const
{
	return Near;
}

void hos::com::Camera::SetNear(float near)
{
	Near = near;
}

float hos::com::Camera::GetFar() const
{
	return Far;
}

void hos::com::Camera::SetFar(float far)
{
	Far = far;
}

Vector3 hos::com::Camera::GetFollowTarget() const
{
	return FollowTarget;
}

void hos::com::Camera::SetFollowTarget(const Vector3& followTarget)
{
	FollowTarget = followTarget;
}

Color hos::com::Camera::GetClearColor() const
{
	return ClearColor;
}

void hos::com::Camera::SetClearColor(const Color& color)
{
	ClearColor = color;
}

Matrix hos::com::Camera::GetViewMatrix() const
{
	return ViewMatrix;
}

Matrix hos::com::Camera::GetProjMatrix() const
{
	return ProjMatrix;
}

void hos::com::Camera::ChangeMode(eMode mode)
{
	if (Mode == mode)
	{
		return;
	}

	Mode = mode;

	if (m_GameObject->transform)
	{
		switch (Mode)
		{
		case eMode::DEFAULT:
			ViewMatrix = m_GameObject->transform->GetTransformMatrix().Invert();
			break;
		case eMode::FOLLOW:
			ViewMatrix = DirectX::XMMatrixLookAtLH(m_GameObject->transform->GetPosition(), FollowTarget, m_GameObject->transform->GetUp());
			break;
		}
	}
}

void hos::com::Camera::ChangeView(eViewMode view)
{
	if (ViewMode == view)
	{
		return;
	}

	ViewMode = view;

	switch (ViewMode)
	{
	case eViewMode::PERSPECTIVE:
		ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(DegreeToRad(FOV), AspectRatio, Near, Far);
		break;
	case eViewMode::ORTHOGRAPHIC:
		ProjMatrix = DirectX::XMMatrixOrthographicLH((F32)this->ScreenWidth, (F32)this->ScreenHeight, Near, Far);
		break;
	}
}

void hos::com::Camera::ChangeClearFlag(eClearFlag flag)
{
	if (ClearFlag == flag)
	{
		return;
	}

	ClearFlag = flag;
}

hos::string hos::com::Camera::GetSkyBoxName() const
{
	return this->SkyBoxName;
}

void hos::com::Camera::SetSkyBox(hos::string_view skyBoxname)
{
	this->SkyBoxName = skyBoxname;

}

void hos::com::Camera::FixedUpdate()
{
	// Renderer에서 필요 정보 획득
	if (g_Renderer)
	{
		hos::cg::RendererDesc& _renderDesc = g_Renderer->GetRenderDesc();
		this->ScreenWidth = _renderDesc.ScreenWidth;
		this->ScreenHeight = _renderDesc.ScreenHeight;
		this->AspectRatio = (float)this->ScreenWidth / (float)this->ScreenHeight;

		//TODO : 카메라 클리어하는부분 작성해야함
		switch (ClearFlag)
		{
		case hos::com::Camera::eClearFlag::SKYBOX:
			//skybox로 클리어
			break;
		case hos::com::Camera::eClearFlag::SOLID_COLOR:
			//ClearColor으로 클리어
			break;
		case hos::com::Camera::eClearFlag::NONE:
			//검은색으로 클리어
			break;
		}

		if (m_GameObject->transform)
		{
			switch (Mode)
			{
			case eMode::DEFAULT:
				ViewMatrix = m_GameObject->transform->GetTransformMatrix().Invert();
				break;
			case eMode::FOLLOW:
				ViewMatrix = DirectX::XMMatrixLookAtLH(m_GameObject->transform->GetPosition(), FollowTarget, m_GameObject->transform->GetUp());
				break;
			}

			switch (ViewMode)
			{
			case eViewMode::PERSPECTIVE:
				ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(DegreeToRad(FOV), AspectRatio, Near, Far);
				break;
			case eViewMode::ORTHOGRAPHIC:
				ProjMatrix = DirectX::XMMatrixOrthographicLH((F32)this->ScreenWidth, (F32)this->ScreenHeight, Near, Far);
				break;
			}
		}
	}
}

void hos::com::Camera::OnEnable()
{
}

void hos::com::Camera::OnDisable()
{
}

void hos::com::Camera::SetRadialBlurState(bool bRadial, float uv_x, float uv_y, float distance, float strength)
{
	this->RadialBlurState = bRadial;

	if (this->RadialBlurState)
	{
		g_Renderer->SetRadialValue(uv_x, uv_y, distance, strength);
	}
}

com::Camera* hos::com::Camera::Clone() const
{
	return new Camera(*this);
}

void hos::com::Camera::Reset()
{
}

void hos::com::Camera::Bind(GameObject* gameObject)
{
	if (gameObject->transform == nullptr)
	{
		throw std::exception("Camera Must Have Transform");
	}

	switch (Mode)
	{
	case eMode::DEFAULT:
		ViewMatrix = gameObject->transform->GetTransformMatrix().Invert();
		break;
	case eMode::FOLLOW:
		ViewMatrix = DirectX::XMMatrixLookAtLH(gameObject->transform->GetPosition(), FollowTarget, gameObject->transform->GetUp());
		break;
	}

	switch (ViewMode)
	{
	case eViewMode::PERSPECTIVE:
		ProjMatrix = DirectX::XMMatrixPerspectiveFovLH(DegreeToRad(FOV), AspectRatio, Near, Far);
		break;
	case eViewMode::ORTHOGRAPHIC:
		ProjMatrix = DirectX::XMMatrixOrthographicLH((F32)this->ScreenWidth, (F32)this->ScreenHeight, Near, Far);
		break;
	}

	Component::Bind(gameObject);
}

void hos::com::Camera::Start()
{
}

void hos::com::Camera::PreRender()
{
	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x31))) // 컨트롤 + 1
	{
		g_outputMap = 1;	// Forward
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x32))) // 컨트롤 + 2
	{
		g_outputMap = 2;	// Emiss
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x33))) // 컨트롤 + 3
	{
		g_outputMap = 3;	// H_BLUR
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x34))) // 컨트롤 + 4
	{
		g_outputMap = 4;	// V_BLUR
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x35))) // 컨트롤 + 5
	{
		g_outputMap = 5;	// Shadow_LightView
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x36))) // 컨트롤 + 6
	{
		g_outputMap = 6;	// DepthMap
		g_bDeferred = false;
	}

	if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x37))) // 컨트롤 + 7
	{
		g_outputMap = 7;	// DepthMap(Old)
		g_bDeferred = false;
	}

	//if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x35))) // 컨트롤 + 5
	//{
	//	g_outputMap = 5;	// Deferred Albedo
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_F6))
	//{
	//	g_outputMap = 3;	// Depth
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_F7))
	//{
	//	g_outputMap = 4;	// ARM
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_F8))
	//{
	//	g_outputMap = 5;	// ViewVector
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_F10))
	//{
	//	g_outputMap = 6;	// Complete
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_F11))
	//{
	//	g_outputMap = 7;	// Complete
	//	g_bDeferred = true;
	//}

	//if (GetAsyncKeyState(VK_INSERT))
	//{
	//	g_outputMap = 8;	// Complete
	//	g_bDeferred = true;
	//}
}

void hos::com::Camera::Render()
{
	g_Renderer->SetViewMatrixToPipeline(ViewMatrix);
	g_Renderer->SetProjMatrixToPipeline(ProjMatrix);

	if (g_Renderer == nullptr)
	{
		Debug->LogConsole(L"Camera", L"Renderer가 초기화되지 않아 Render할 수 없습니다.");
		return;
	}

	// 필수 Component Check
	Transform* _transform = m_GameObject->transform;

	if (_transform == nullptr)
	{
		Debug->LogConsole(L"Camera", m_GameObject->GetName() + L" 카메라의 Transform 컴포넌트가 없으면 Render가 불가능합니다.");
	}

	Vector3 _cameraPos = _transform->GetPosition();
	Vector3 _cameraRot = _transform->GetRotation();
	Vector4 _color = Vector4(this->ClearColor.x, this->ClearColor.y, this->ClearColor.z, this->ClearColor.w);
	bool _bSky = false;

	if (this->ClearFlag == eClearFlag::SKYBOX)
	{
		_bSky = true;
	}

	F32 dTime = (F32)Time->DeltaTime();
	F32 FPS = Time->GetFPS();

	if (bDebug)
	{
		g_Renderer->_DebugText(0, 0, 20, Vector4(1, 1, 1, 1), L"FPS : %.0f", FPS);
	}

	g_Renderer->ForwardRender_ToCameraBuffer(dTime, this->Near, _cameraPos, _cameraRot, this->FOV, (float)this->ScreenWidth, (float)this->ScreenHeight, this->Near, this->Far,
		hos::cg::RendererDesc::CameraModeList::CAMERA3D_DEFAULT, this->MortionBlurSampleCount, 0, _bSky, SkyBoxName, _color, bDebug);
}

void hos::com::Camera::PostRender()
{
	if (g_Renderer == nullptr)
	{
		Debug->LogConsole(L"Camera", L"Renderer가 초기화되지 않아 PostRender할 수 없습니다.");
		return;
	}

	//static float distance = 0.25f;
	//static float valueSampleStr = 3.0f;

	//float dTime = Time->DeltaTime();

	//if (GetAsyncKeyState(VK_HOME))
	//{
	//	distance += 1.0f * dTime;
	//}

	//if (GetAsyncKeyState(VK_END))
	//{
	//	distance -= 1.0f * dTime;
	//}

	//if (GetAsyncKeyState(VK_PRIOR))
	//{
	//	valueSampleStr += 1.0f * dTime;
	//}

	//if (GetAsyncKeyState(VK_NEXT))
	//{
	//	valueSampleStr -= 1.0f * dTime;
	//}
	//
	//g_Renderer->_DebugText(300, 300, 30, Vector4(1, 1, 1, 1), L"dist : %f", distance);
	//g_Renderer->_DebugText(300, 400, 30, Vector4(1, 1, 1, 1), L"sampleStr : %f", valueSampleStr);

	//g_Renderer->SetRadialValue(0.5f, 0.5f, distance, valueSampleStr);

	g_Renderer->SetViewPort(ViewPortLeft, ViewPortRight, ViewPortTop, ViewPortBottom);

	g_Renderer->RenderCamera_ToSwapChainBuffer(this->RadialBlurState, 0, g_outputMap);
}

const std::vector<U8> hos::com::Camera::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring skybox = ut::UTF16ToAnsi(SkyBoxName);

	flexbuffers::Builder builder;

	builder.Map([&] {
		builder.String("Name", name.c_str());
		builder.Bool("IsActive", GetActive());
		builder.Float("ViewPortLeft", ViewPortLeft);
		builder.Float("ViewPortRight", ViewPortRight);
		builder.Float("ViewPortTop", ViewPortTop);
		builder.Float("ViewPortBottom", ViewPortBottom);
		builder.Float("FOV", FOV);
		builder.Float("Near", Near);
		builder.Float("Far", Far);
		builder.TypedVector("ClearColor", [&] {
			builder.Float(ClearColor.x);
			builder.Float(ClearColor.y);
			builder.Float(ClearColor.z);
			});
		builder.Int("Mode", (int)Mode);
		builder.Int("ViewMode", (int)ViewMode);
		builder.Int("ClearFlag", (int)ClearFlag);
		builder.String("SkyBoxName", skybox);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Camera::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);
	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"Camera try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	ViewPortLeft = m["ViewPortLeft"].AsFloat();
	ViewPortRight = m["ViewPortRight"].AsFloat();
	ViewPortTop = m["ViewPortTop"].AsFloat();
	ViewPortBottom = m["ViewPortBottom"].AsFloat();
	FOV = m["FOV"].AsFloat();
	Near = m["Near"].AsFloat();
	Far = m["Far"].AsFloat();

	float x, y, z;
	x = m["ClearColor"].AsTypedVector()[0].AsFloat();
	y = m["ClearColor"].AsTypedVector()[1].AsFloat();
	z = m["ClearColor"].AsTypedVector()[2].AsFloat();
	ClearColor = Color(x, y, z);

	Mode = (eMode)m["Mode"].AsInt32();
	ViewMode = (eViewMode)m["ViewMode"].AsInt32();
	ClearFlag = (eClearFlag)m["ClearFlag"].AsInt32();

	mbstring skybox = m["SkyBoxName"].AsString().str();
	SkyBoxName = ut::AnsiToUTF16(skybox);

	if (ClearFlag == eClearFlag::SKYBOX)
	{
		if (g_DataManager->HasSkyBox(SkyBoxName) == false)
		{
			g_DataManager->LoadSkyBox(g_Path + L"\\Assets\\Texture\\" + SkyBoxName);
		}
	}

	ChangeMode(Mode);
	ChangeView(ViewMode);

	return true;
}