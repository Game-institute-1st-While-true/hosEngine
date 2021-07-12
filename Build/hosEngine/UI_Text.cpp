// ���� ���
#include "Transform.h"
#include "GameObject.h"
#include "UIImage.h"
#include "BoneAnimation.h"

// �ڱ� ���
#include "UI_Text.h"

using namespace hos;


//bool bAni;
//bool bAnimationEnd;
//hos::F32 NowTick;
//hos::string AnimationName;
//BoneAnimation* AnimationData;

hos::com::UIText::UIText()
	:Component(L"UIText"), Text(L"New Text"), FontName(L"Arial"), Font(nullptr), FontSize(10), FontColor(1, 1, 1, 1), Origin(0, 0), FontRot(0.0f), LayerOrder(0),
	bAni(false), bAnimationEnd(false), NowTick(0.0f), AnimationName(L""), AnimationData(nullptr), AlignType(7),
	Aspect_X(1.0f), Aspect_Y(1.0f), bFollowText(false), IsLoop(false), CoordU(0), CoordV(0)
{

}

hos::com::UIText::~UIText()
{
}

hos::com::UIText::UIText(const UIText& dest)
	:Component(L"UIText"), Text(dest.Text), FontName(dest.FontName), Font(nullptr), FontSize(dest.FontSize), FontColor(dest.FontColor),
	Origin(dest.Origin), FontRot(dest.FontRot), LayerOrder(dest.LayerOrder),
	bAni(dest.bAni), bAnimationEnd(false), NowTick(0.0f), AnimationName(dest.AnimationName), AnimationData(nullptr), AlignType(dest.AlignType),
	Aspect_X(1.0f), Aspect_Y(1.0f), bFollowText(dest.bFollowText), IsLoop(dest.IsLoop), CoordU(dest.CoordU), CoordV(dest.CoordV)
{
	FontData* _fontData = g_DataManager->GetFont(this->FontName);
	if (_fontData == nullptr)
	{
		Debug->LogConsole(L"UI_Text", FontName + L"�����Ϸ��� UIText Component�� FontData�� ��ȿ���� �ʾ�, �⺻��Ʈ(Arial)�� ������ �� �����մϴ�.");
		this->FontName = L"Arial";
	}

	this->Font = _fontData;

	BoneAnimation* _aniData = g_DataManager->GetUIAnimation(this->AnimationName);
	if (_aniData == nullptr)
	{
		Debug->LogConsole(L"UI_Text", AnimationName + L"�����Ϸ��� UIImage Component�� AnimationData�� ��ȿ���� �ʾ�, Animation�� ������ ä �����մϴ�.");
		this->bAni = false;
		this->AnimationName = L"";
	}

	this->AnimationData = _aniData;
}

void hos::com::UIText::SetText(string_view text)
{
	Text = text.data();
}

void hos::com::UIText::SetTextSafe(string_view text)
{
	string str = text.data();
	for (int i = 0xAC00; i <= 0xD7A3; i++)
	{
		wchar_t c = static_cast<wchar_t>(i);
		string tmp(1, c);
		str = ut::replaceAll(str, tmp, L"");
	}
	for (int i = 0x3131; i <= 0x318E; i++)
	{
		wchar_t c = static_cast<wchar_t>(i);
		string tmp(1, c);
		str = ut::replaceAll(str, tmp, L"");
	}

	this->Text = str;
}

void hos::com::UIText::SetFont(string_view fontName)
{
	// ȹ�� �� ��ȿ�� �˻�
	FontData* _fontData = g_DataManager->GetFont(fontName);
	this->Font = _fontData;
	if (_fontData)
	{
		this->FontName = fontName;
	}

	else
	{
		Debug->LogConsole(L"UI_Text", string(fontName) + L"�ش� ��Ʈ�� ���� �⺻ ��Ʈ�� Arial ��Ʈ�� ���õ˴ϴ�.");
		this->FontName = L"Arial";
	}
}

void hos::com::UIText::SetFontSize(int size)
{
	this->FontSize = size;
}

void hos::com::UIText::SetFontColor(Color color)
{
	this->FontColor = color;
}

string hos::com::UIText::GetText() const
{
	return Text;
}

hos::string hos::com::UIText::GetFontName() const
{
	return FontName;
}

hos::FontData* hos::com::UIText::GetFontData() const
{
	return Font;
}
int hos::com::UIText::GetFontSize() const
{
	return FontSize;
}

Color hos::com::UIText::GetFontColor() const
{
	return FontColor;
}

void hos::com::UIText::SetAnimation(hos::string aniName)
{
	BoneAnimation* _aniData = g_DataManager->GetUIAnimation(aniName);

	if (_aniData == nullptr)
	{
		Debug->LogConsole(L"UI_Text", aniName + L"�ش� �̸��� UI_Animation�� DataManager�� �������� �ʾ�, UI_Text_Animation�� Set���� �ʽ��ϴ�.");
		this->bAni = false;
		return;
	}

	this->bAni = true;
	this->NowTick = 0.0f;
	this->AnimationName = aniName;
	this->AnimationData = _aniData;
}

void hos::com::UIText::RemoveAnimation()
{
	this->bAni = false;
	this->AnimationName = L"";
	this->AnimationData = nullptr;
}

hos::Vector2 hos::com::UIText::GetTextBoundSize()
{
	if (g_Renderer)
	{
		// �ʼ� Component Check
		Transform* _transform = m_GameObject->transform;
		if (_transform == nullptr)
		{
			Debug->LogConsole(L"UI_Text", m_GameObject->GetName() + L"Transform ������Ʈ�� ������ TextBoundSize�� ���� �� �����ϴ�.");

			return Vector2::Zero;
		}

		Vector3 _scale = _transform->GetScale();
		Vector2 boundSize = g_Renderer->GetTextBoundSize(this->Text.data(), (float)FontSize, { _scale.x,_scale.y },  FontName);

		return boundSize;
	}

	else
	{
		Debug->LogConsole(L"UI_Text_GetTextBoundSize", L"g_Renderer�� ���� ZeroVector�� ��ȯ�մϴ�.");
		return Vector2::Zero;
	}
}

void hos::com::UIText::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->uitext)
	{
		throw std::invalid_argument("this Object already has UI_Text");
	}

	if (gameObject->uiimage)
	{
		throw std::invalid_argument("UI_Text and UI_Image cannot be used together.");
	}

	gameObject->uitext = this;
	m_GameObject = gameObject;
}

void hos::com::UIText::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->uitext = nullptr;
		m_GameObject = nullptr;
	}
}

void hos::com::UIText::OnGUI(float aspect_x, float aspect_y)
{
	this->Aspect_X = aspect_x;
	this->Aspect_Y = aspect_y;
}

void hos::com::UIText::Update()
{
	if (g_Renderer)
	{
		// �ʼ� Component Check
		Transform* _transform = m_GameObject->transform;
		if (_transform == nullptr)
		{
			Debug->LogConsole(L"UI_Text", m_GameObject->GetName() + L"Transform ������Ʈ�� ������ UI Text ����� �Ұ����մϴ�.");
			return;
		}

		// Transform Info Get
		Vector3 _pos = _transform->GetPosition();
		Vector3 _Rot = _transform->GetRotation();
		Vector3 _Scale = _transform->GetScale();

		// RenderObject
		hos::cg::RenderQue::RenderUIObejct _renderUI;
		_renderUI.Aspect.x = this->Aspect_X;
		_renderUI.Aspect.y = this->Aspect_Y;

		_renderUI.bFont = true;
		_renderUI.LayerOrder = this->LayerOrder;
		_renderUI.Text = this->Text;
		_renderUI.FontName = this->FontName;
		//_renderUI.FontSize = this->FontSize * ((Aspect_X + Aspect_Y) / 2.f );
		_renderUI.FontSize = this->FontSize;
		_renderUI.Color = this->FontColor;
		_renderUI.Pos = _pos;
		_renderUI.Scale = _Scale;
		_renderUI.Align = this->AlignType;
		_renderUI.FontRot = DegreeToRad(this->FontRot + _Rot.z);

		// Follow Check
		_renderUI.bFollow = this->bFollowText;
		_renderUI.CoordU = CoordU;
		_renderUI.CoordV = CoordV;

		// �ִϸ��̼� üũ
		if (this->bAni)
		{
			BoneAnimation* _boneAnimation = g_DataManager->GetUIAnimation(this->AnimationName);

			if (_boneAnimation == nullptr)
			{
				Debug->LogConsole(L"UI_Text", m_GameObject->GetName() + L"������ UI_Animation�� ��ȿ���� �ʽ��ϴ�.");
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
				hos::Vector3 vPos, vScale, vRot;
				hos::Quaternion vQuat;
				_boneAnimation->_Interpolate(0, this->NowTick, &mAniTM, &vPos, &vScale, &vQuat);
				vRot = QuaterniontoEulerAngle(vQuat);

				vRot.x = DegreeToRad(vRot.x);
				vRot.y = DegreeToRad(vRot.y);
				vRot.z = DegreeToRad(vRot.z);

				_renderUI.bAni = true;
				_renderUI.AniTM = mAniTM;
				_renderUI.vAniPos = vPos;
				_renderUI.vAniScale = vScale;
				_renderUI.vAniRot = vRot;

				// �ִϸ��̼� ����
				_renderUI.Color.w = this->FontColor.w * vScale.z;
			}
		}

		// ����ť�� ����
		g_Renderer->AddRenderUIObejct(_renderUI);
	}
}

hos::com::UIText* hos::com::UIText::Clone() const
{
	return new UIText(*this);
}

void hos::com::UIText::OnEnable()
{
	NowTick = 0;
}

void hos::com::UIText::ProcessTickCount()
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

const std::vector<U8> hos::com::UIText::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	mbstring fontName = ut::UTF16ToAnsi(FontName);
	mbstring aniName = ut::UTF16ToAnsi(AnimationName);
	mbstring text = ut::UTF16ToAnsi(Text);

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.String("Text", text);
		builder.Int("FontSize", FontSize);
		builder.TypedVector("Color", [&] {
			builder.Float(FontColor.x);
			builder.Float(FontColor.y);
			builder.Float(FontColor.z);
			builder.Float(FontColor.w);
			});
		builder.TypedVector("Origin", [&] {
			builder.Float(Origin.x);
			builder.Float(Origin.y);
			});
		builder.Float("FontRot", FontRot);
		builder.Int("LayerOrder", LayerOrder);
		builder.Int("AlignType", AlignType);
		builder.String("FontName", fontName);
		builder.Bool("bAni", bAni);
		builder.String("AnimationName", aniName);
		builder.Bool("bFollow", bFollowText);
		builder.Bool("IsLoop", IsLoop);
		builder.Int("CoordU", CoordU);
		builder.Int("CoordV", CoordV);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::UIText::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"UI_Text try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	mbstring text = m["Text"].AsString().str();
	Text = ut::AnsiToUTF16(text);

	FontSize = m["FontSize"].AsInt32();

	auto color = m["Color"].AsTypedVector();
	FontColor.x = color[0].AsFloat();
	FontColor.y = color[1].AsFloat();
	FontColor.z = color[2].AsFloat();
	FontColor.w = color[3].AsFloat();

	auto origin = m["Origin"].AsTypedVector();
	Origin.x = origin[0].AsFloat();
	Origin.y = origin[1].AsFloat();

	FontRot = m["FontRot"].AsFloat();
	LayerOrder = m["LayerOrder"].AsInt32();
	AlignType = m["AlignType"].AsInt32();

	mbstring texname = m["FontName"].AsString().str();
	FontName = ut::AnsiToUTF16(texname);

	Font = g_DataManager->GetFont(FontName);
	if (nullptr == Font)
	{
		Font = g_DataManager->LoadFont(g_Path + L"\\Assets\\Font\\" + FontName + FontData::FILE_EXTENSION);
		if (nullptr == Font)
		{
			Font = g_DataManager->LoadFont(g_Path + L"\\Assets\\Font\\" + FontName + FontData::SPRITE_FONT_FILE);
			if (nullptr == Font)
			{
				Font = g_DataManager->GetFont(L"Arial");
			}
		}
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

	bFollowText = m["bFollow"].AsBool();

	IsLoop = m["IsLoop"].AsBool();
	CoordU = m["CoordU"].AsInt32();
	CoordV = m["CoordV"].AsInt32();

	return true;
}
