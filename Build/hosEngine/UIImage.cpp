// ���� ���
//#include <algorithm>

#include "GameObject.h"
#include "Transform.h"
#include "BoneAnimation.h"

// �ڱ� ���
#include "UIImage.h"

using namespace hos;

hos::com::UIImage::UIImage()
	: Component(L"UIImage"), bTexture(false), TextureName(L""), BaseColor(1.f, 1.f, 1.f, 1.f), OffTil(0, 1, 0, 1), LayerOrder(0),
	bAni(false), bAnimationEnd(false), NowTick(0.0f), AnimationName(L""), AnimationData(nullptr), AlignType(7),
	Width(0), Height(0), bFollowUI(false), IsLoop(false),
	Aspect_X(1.0f), Aspect_Y(1.0f), CoordU(0), CoordV(0)
{
}

hos::com::UIImage::UIImage(const UIImage& dest)
	: Component(L"UIImage"), bTexture(dest.bTexture), TextureName(dest.TextureName), BaseColor(dest.BaseColor), OffTil(dest.OffTil), LayerOrder(dest.LayerOrder), AlignType(dest.AlignType),
	bAni(dest.bAni), bAnimationEnd(false), NowTick(0.0f), AnimationName(dest.AnimationName), AnimationData(nullptr),
	Width(dest.Width), Height(dest.Height), bFollowUI(dest.bFollowUI), IsLoop(dest.IsLoop),
	Aspect_X(1.0f), Aspect_Y(1.0f), CoordU(dest.CoordU), CoordV(dest.CoordV)
{
	// Animation Copy Check
	BoneAnimation* _aniData = g_DataManager->GetUIAnimation(this->AnimationName);
	if (_aniData == nullptr)
	{
		Debug->LogConsole(L"UI_Image", AnimationName + L"�����Ϸ��� UI_Image Component�� AnimationData�� ��ȿ���� �ʾ�, Animation�� ������ ä �����մϴ�.");
		this->bAni = false;
		this->AnimationName = L"";
	}
	this->AnimationData = _aniData;
}

hos::com::UIImage::~UIImage()
{
}

void hos::com::UIImage::SetColor(hos::Color& color)
{
	this->BaseColor = color;
}

void hos::com::UIImage::SetOffTil(hos::Vector4& offTil)
{
	this->OffTil = offTil;
}

void hos::com::UIImage::SetTexture(hos::string textureName)
{
	if (g_DataManager->HasTexture(textureName) == false)
	{
		Debug->LogConsole(L"UI_Image", textureName + L"�ش� �̸��� Texture�� DataManager�� �������� �ʾ�, UI_Texture�� Set���� �ʽ��ϴ�.");
		this->bTexture = false;
		return;
	}

	this->bTexture = true;
	this->TextureName = textureName;
}

void hos::com::UIImage::RemoveTexture()
{
	this->bTexture = false;
	this->TextureName = L"";
}

void hos::com::UIImage::SetAnimation(hos::string aniName)
{
	BoneAnimation* _aniData = g_DataManager->GetUIAnimation(aniName);

	if (_aniData == nullptr)
	{
		Debug->LogConsole(L"UI_Image", aniName + L"�ش� �̸��� UI_Animation�� DataManager�� �������� �ʾ�, UI_Image_Animation�� Set���� �ʽ��ϴ�.");
		this->bAni = false;
		return;
	}

	this->bAni = true;
	this->NowTick = 0.0f;
	this->AnimationName = aniName;
	this->AnimationData = _aniData;
}

void hos::com::UIImage::RemoveAnimation()
{
	this->bAni = false;
	this->AnimationName = L"";
	this->AnimationData = nullptr;
}

void hos::com::UIImage::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->uiimage)
	{
		throw std::invalid_argument("this Object already has UI_Image");
	}

	if (gameObject->uitext)
	{
		throw std::invalid_argument("UI_Image and UI_Text cannot be used together.");
	}

	gameObject->uiimage = this;
	m_GameObject = gameObject;
}

void hos::com::UIImage::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->uiimage = nullptr;
		m_GameObject = nullptr;
	}
}

void hos::com::UIImage::OnGUI(float aspect_x, float aspect_y)
{
	this->Aspect_X = aspect_x;
	this->Aspect_Y = aspect_y;
}

void hos::com::UIImage::Update()
{
	if (g_Renderer)
	{
		// �ʼ� Component Check
		Transform* _transform = m_GameObject->transform;

		if (_transform == nullptr)
		{
			Debug->LogConsole(L"UI_Image", m_GameObject->GetName() + L"Transform ������Ʈ�� ������ UI Render�� �Ұ����մϴ�.");
			return;
		}

		hos::Matrix mTM;
		hos::Vector3 vPos, vRot, vScale, vFollowPos;
		vPos = _transform->GetPosition();
		vRot = _transform->GetRotation();
		vScale = _transform->GetScale();

		//vPos.x *= Aspect_X;
		//vPos.y *= Aspect_Y;

		//
		hos::cg::RenderQue::RenderUIObejct _renderObject;

		_renderObject.Aspect.x = this->Aspect_X;
		_renderObject.Aspect.y = this->Aspect_Y;

		_renderObject.Pos = vPos;

		if (this->bFollowUI)
		{
			// FollowMode Check
			_renderObject.bFollow = true;

			_renderObject.CoordU = CoordU;
			_renderObject.CoordV = CoordV;

			_renderObject.Scale.x = 1.0f;
			_renderObject.Scale.y = 1.0f;
		}

		else
		{
			_renderObject.bFollow = false;

			_renderObject.Rot.x = DegreeToRad(vRot.x);
			_renderObject.Rot.y = DegreeToRad(vRot.y);
			_renderObject.Rot.z = DegreeToRad(vRot.z);

			_renderObject.Scale.x = vScale.x;
			_renderObject.Scale.y = vScale.y;
		}

		_renderObject.Color = this->BaseColor;
		_renderObject.OffTill = this->OffTil;
		_renderObject.LayerOrder = this->LayerOrder;
		_renderObject.Align = this->AlignType;

		_renderObject.Width = this->Width;
		_renderObject.Height = this->Height;

		if (this->bTexture)
		{
			_renderObject.bTexture = true;
			_renderObject.TextureName = this->TextureName;
		}

		if (this->bAni)
		{
			BoneAnimation* _boneAnimation = g_DataManager->GetUIAnimation(this->AnimationName);

			if (_boneAnimation == nullptr)
			{
				Debug->LogConsole(L"UI_Image", m_GameObject->GetName() + L"������ UI_Animation�� ��ȿ���� �ʽ��ϴ�.");
				return;
			}

			else
			{
				// dTime, Tick ó��
				F32 dTime = (F32)Time->DeltaTime();
				this->NowTick += dTime;

				// �ִϸ��̼� Tick ����
				ProcessTickCount();

				// �ִϸ��̼Ǳ��� ���ŵ� ��ġ�� �簻��
				hos::Matrix mAniTM;
				hos::Vector3 vPos, vAniScale, vRot;
				hos::Quaternion vQuat;
				_boneAnimation->_Interpolate(0, this->NowTick, &mAniTM, &vPos, &vAniScale, &vQuat);
				vRot = QuaterniontoEulerAngle(vQuat);

				vRot.x = DegreeToRad(vRot.x);
				vRot.y = DegreeToRad(vRot.y);
				vRot.z = DegreeToRad(vRot.z);

				_renderObject.bAni = true;
				_renderObject.AniTM = mAniTM;
				_renderObject.vAniPos = vPos;
				_renderObject.vAniScale = vAniScale;
				_renderObject.vAniRot = vRot;

				_renderObject.Color.w = BaseColor.w * vAniScale.z;
			}
		}

		g_Renderer->AddRenderUIObejct(_renderObject);
	}

	else
	{
		Debug->LogConsole(L"UIImage", L"g_renderer does not exsist : " + this->GetName());
	}
}

hos::com::UIImage* hos::com::UIImage::Clone() const
{
	return new UIImage(*this);
}

void hos::com::UIImage::OnEnable()
{
	NowTick = 0;
}

const std::vector<U8> hos::com::UIImage::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	mbstring texName = ut::UTF16ToAnsi(TextureName);
	mbstring aniName = ut::UTF16ToAnsi(AnimationName);

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.TypedVector("Color", [&] {
			builder.Float(BaseColor.x);
			builder.Float(BaseColor.y);
			builder.Float(BaseColor.z);
			builder.Float(BaseColor.w);
			});
		builder.TypedVector("OffTil", [&] {
			builder.Float(OffTil.x);
			builder.Float(OffTil.y);
			builder.Float(OffTil.z);
			builder.Float(OffTil.w);
			});
		builder.Int("LayerOrder", LayerOrder);
		builder.Int("AlignType", AlignType);
		builder.Bool("bTexture", bTexture);
		builder.String("TextureName", texName);
		builder.Bool("bAni", bAni);
		builder.String("AnimationName", aniName);
		builder.UInt("Width", Width);
		builder.UInt("Height", Height);
		builder.Bool("bFollow", bFollowUI);
		builder.Bool("IsLoop", IsLoop);
		builder.Int("CoordU", CoordU);
		builder.Int("CoordV", CoordV);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::UIImage::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"UI_Image try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	auto color = m["Color"].AsTypedVector();
	BaseColor.x = color[0].AsFloat();
	BaseColor.y = color[1].AsFloat();
	BaseColor.z = color[2].AsFloat();
	BaseColor.w = color[3].AsFloat();

	Width = m["Width"].AsUInt32();
	Height = m["Height"].AsUInt32();

	auto offtil = m["OffTil"].AsTypedVector();
	OffTil.x = offtil[0].AsFloat();
	OffTil.y = offtil[1].AsFloat();
	OffTil.z = offtil[2].AsFloat();
	OffTil.w = offtil[3].AsFloat();

	LayerOrder = m["LayerOrder"].AsInt32();
	AlignType = m["AlignType"].AsInt32();

	bTexture = m["bTexture"].AsBool();
	mbstring texname = m["TextureName"].AsString().str();
	TextureName = ut::AnsiToUTF16(texname);

	if (bTexture)
	{
		g_DataManager->LoadTexture(TextureName);
	}

	bAni = m["bAni"].AsBool();
	mbstring aniName = m["AnimationName"].AsString().str();
	AnimationName = ut::AnsiToUTF16(aniName);

	bAnimationEnd = false;
	NowTick = 0.f;
	AnimationData = nullptr;
	if (bAni)
	{
		AnimationData = g_DataManager->GetUIAnimation(AnimationName);
		if (nullptr == AnimationData)
		{
			AnimationData = g_DataManager->LoadUIAnimation(g_Path + L"\\Assets\\Anim\\" + AnimationName + BoneAnimation::FILE_EXTENSION);
		}
	}

	bFollowUI = m["bFollow"].AsBool();
	IsLoop = m["IsLoop"].AsBool();

	CoordU = m["CoordU"].AsInt32();
	CoordV = m["CoordV"].AsInt32();

	return true;
}

void hos::com::UIImage::ProcessTickCount()
{
	F32 startTick = this->AnimationData->_GetStartTime();
	F32 lastTick = this->AnimationData->_GetEndTime();

	// ���� ƽ�� 0���� �۴ٸ� 0���� �ʱ�ȭ���ش�.
	if (this->NowTick < startTick)
	{
		NowTick = 0;
		//this->NowTick += lastTick;
	}

	// ���� ƽ�� ������ ƽ���� ���ٸ�,
	// ������ ƽ���� �۾��� �� ���� ������ ƽ�� ���ش�(��� ������ ����)
	if (this->NowTick > lastTick)
	{
		//while (this->NowTick > lastTick)
		//{
		//	this->NowTick -= lastTick;

		//	if (lastTick <= 0)
		//	{
		//		Debug->LogConsole(L"UI_Image", m_GameObject->GetName() + L"UI Animation �� lastTick�� 0���� �۽��ϴ�.");
		//		return;
		//	}
		//}

		bAnimationEnd = true;
		if (IsLoop)
		{
			NowTick = 0;
		}
	}
	else
	{
		bAnimationEnd = false;
	}
}