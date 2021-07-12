// 참조 헤더
#include "GameObject.h"
#include "Transform.h"
#include "UIImage.h"

// 자기 헤더
#include "UIButton.h"

// 네임스페이스
using namespace hos;
//using namespace hos::com;


hos::com::UIButton::UIButton()
	: Component(L"UIButton")
{
	Reset();
}

hos::com::UIButton::~UIButton()
{
}

hos::com::UIButton::UIButton(const UIButton& dest)
	: Component(L"UIButton"), Interactable(dest.Interactable), Transition(dest.Transition),
	NormalColor(dest.NormalColor), HoverColor(dest.HoverColor), PressColor(dest.PressColor), SelectedColor(dest.SelectedColor), DisableColor(dest.DisableColor),
	NormalSprite(dest.NormalSprite), HoverSprite(dest.HoverSprite), PressSprite(dest.PressSprite), SelectedSprite(dest.SelectedSprite), DisableSprite(dest.DisableSprite),
	State(eState::None), bSelected(false), DefaultColor(0,0,0,0), DefaultSprite(L"")
{
}

bool hos::com::UIButton::GetInteractable() const
{
	return Interactable;
}

void hos::com::UIButton::SetInteractable(bool b)
{
	Interactable = b;
}

Color hos::com::UIButton::GetNormalColor() const
{
	return NormalColor;
}

void hos::com::UIButton::SetNormalColor(Color color)
{
	NormalColor = color;
}

Color hos::com::UIButton::GetHoverColor() const
{
	return HoverColor;
}

void hos::com::UIButton::SetHoverColor(Color color)
{
	HoverColor = color;
}

Color hos::com::UIButton::GetPressColor() const
{
	return PressColor;
}

void hos::com::UIButton::SetPressColor(Color color)
{
	PressColor = color;
}

Color hos::com::UIButton::GetSelectedColor() const
{
	return SelectedColor;
}

void hos::com::UIButton::SetSelectedColor(Color color)
{
	SelectedColor = color;
}

Color hos::com::UIButton::GetDisableColor() const
{
	return DisableColor;
}

void hos::com::UIButton::SetDisableColor(Color color)
{
	DisableColor = color;
}

string hos::com::UIButton::GetNormalSprite() const
{
	return NormalSprite;
}

void hos::com::UIButton::SetNormalSprite(string name)
{
	if (g_DataManager->HasTexture(name))
	{
		NormalSprite = name;
	}
	else
	{
		NormalSprite = L"";
	}
}

string hos::com::UIButton::GetHoverSprite() const
{
	return HoverSprite;
}

void hos::com::UIButton::SetHoverSprite(string name)
{
	if (g_DataManager->HasTexture(name))
	{
		HoverSprite = name;
	}
	else
	{
		HoverSprite = L"";
	}
}

string hos::com::UIButton::GetPressSprite() const
{
	return PressSprite;
}

void hos::com::UIButton::SetPressSprite(string name)
{
	if (g_DataManager->HasTexture(name))
	{
		PressSprite = name;
	}
	else
	{
		PressSprite = L"";
	}
}

string hos::com::UIButton::GetSelectedSprite() const
{
	return SelectedSprite;
}

void hos::com::UIButton::SetSelectedSprite(string name)
{
	if (g_DataManager->HasTexture(name))
	{
		SelectedSprite = name;
	}
	else
	{
		SelectedSprite = L"";
	}
}

string hos::com::UIButton::GetDisableSprite() const
{
	return DisableSprite;
}

void hos::com::UIButton::SetDisableSprite(string name)
{
	if (g_DataManager->HasTexture(name))
	{
		DisableSprite = name;
	}
	else
	{
		DisableSprite = L"";
	}
}

com::UIButton::eState hos::com::UIButton::GetState() const
{
	return State;
}

void hos::com::UIButton::SetState(eState state)
{
	State = state;
}

void hos::com::UIButton::ChangeTransition(eTransitionMode mode)
{
	if (Transition == mode)
	{
		return;
	}

	if (m_GameObject && m_GameObject->uiimage)
	{
		switch (mode)
		{
		case hos::com::UIButton::eTransitionMode::None:
			if (DefaultSprite.size() > 0)
			{
				m_GameObject->uiimage->SetIsTexture(true);
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			m_GameObject->uiimage->SetColor(DefaultColor);
			break;
		case hos::com::UIButton::eTransitionMode::Color:
			if (DefaultSprite.size() > 0)
			{
				m_GameObject->uiimage->SetIsTexture(true);
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		case hos::com::UIButton::eTransitionMode::Sprite:
			m_GameObject->uiimage->SetIsTexture(true);
			break;
		}

		Transition = mode;
	}
}

bool hos::com::UIButton::IsClick() const
{
	if (m_GameObject)
	{
		return bSelected && m_GameObject->GetActive();
	}
	return false;
}

bool hos::com::UIButton::OnHover() const
{
	return (PrevState == eState::None && State == eState::Hover);
}

void hos::com::UIButton::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->uibutton)
	{
		throw std::invalid_argument("this Object already has UI_Button");
	}

	if (gameObject->uiinputfield)
	{
		throw std::invalid_argument("UI_Button and UI_InputField cannot be used together.");
	}

	gameObject->uibutton = this;
	m_GameObject = gameObject;
}

void hos::com::UIButton::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->uibutton = nullptr;
		m_GameObject = nullptr;
	}
}

void hos::com::UIButton::Start()
{
}

void hos::com::UIButton::OnGUI(float aspect_x, float aspect_y)
{
	// UIImage 체크
	if (nullptr == m_GameObject->uiimage)
	{
		Debug->LogConsole(L"UI_Button", m_GameObject->GetName() + L"UIImage Component가 없는 Object의 UIButton Update를 할 수 없습니다.");
		return;
	}

	PrevState = State;

	// 필요 정보 Get
	Vector3 _pos = m_GameObject->transform->GetPosition();
	U32 width = m_GameObject->uiimage->GetWidthSize();
	U32 height = m_GameObject->uiimage->GetHeightSize();

	// Rect 생성(Align 적용 전)
	RECT _rect;
	_rect.left = (LONG)(_pos.x * aspect_x);
	_rect.right = (LONG)((_pos.x + width) * aspect_x);
	_rect.top = (LONG)(_pos.y * aspect_y);
	_rect.bottom = (LONG)((_pos.y + height) * aspect_y);

	width = U32(width * aspect_x);
	height = U32(height * aspect_y);

	// Align에 의한 Rect 영역 보정
	int _alignType = m_GameObject->uiimage->GetAlignType();

	switch (_alignType)
	{
	case 1:	// LeftBottom
		_rect.top -= (LONG)height;
		_rect.bottom -= (LONG)height;
		break;

	case 2:	// Bottom
		_rect.left -= (LONG)(width / 2);
		_rect.right -= (LONG)(width / 2);
		_rect.top -= (LONG)height;
		_rect.bottom -= (LONG)height;
		break;

	case 3:	// RightBottom
		_rect.left -= (LONG)width;
		_rect.right -= (LONG)width;
		_rect.top -= (LONG)height;
		_rect.bottom -= (LONG)height;
		break;

	case 4:	// Left
		_rect.top -= (LONG)(height / 2);
		_rect.bottom -= (LONG)(height / 2);
		break;

	case 5:	// Center
		_rect.left -= (LONG)(width / 2);
		_rect.right -= (LONG)(width / 2);
		_rect.top -= (LONG)(height / 2);
		_rect.bottom -= (LONG)(height / 2);
		break;

	case 6:	// Right
		_rect.left -= (LONG)width;
		_rect.right -= (LONG)width;
		_rect.top -= (LONG)(height / 2);
		_rect.bottom -= (LONG)(height / 2);
		break;

	default:
	case 7:	// LeftTop
		break;

	case 8:	// Top
		_rect.left -= (LONG)(width / 2);
		_rect.right -= (LONG)(width / 2);
		break;

	case 9:	// RightTop
		_rect.left -= (LONG)width;
		_rect.right -= (LONG)width;
		break;
	}

	// 충돌 체크
	/// 임시 마우스 커서
	if (Input)
	{
		POINT _cursor = Input->GetMousePosition();
		bSelected = false;
		if (Interactable)
		{
			if (PtInRect(&_rect, _cursor))
			{
				if (State != eState::Selected)
				{
					State = eState::Hover;
				}
				if (Input->GetKeyDown(KeyCode::MouseLeft))
				{
					bSelected = true;
				}
				if (Input->GetKey(KeyCode::MouseLeft))
				{
					State = eState::Pressed;
				}
				if (Input->GetKeyUp(KeyCode::MouseLeft))
				{
					State = eState::Selected;
				}
			}
			else
			{
				if (State != eState::Selected)
				{
					State = eState::None;
				}
				else
				{
					State = eState::Selected;
					if (Input->GetKeyDown(KeyCode::MouseLeft))
					{
						State = eState::None;
					}
				}
			}
		}
		else
		{
			State = eState::Disable;
		}
	}

	switch (Transition)
	{
	case hos::com::UIButton::eTransitionMode::None:
		m_GameObject->uiimage->SetIsTexture(false);
		break;
	case hos::com::UIButton::eTransitionMode::Color:
		switch (State)
		{
		case hos::com::UIButton::eState::None:
			m_GameObject->uiimage->SetColor(NormalColor);
			break;
		case hos::com::UIButton::eState::Hover:
			m_GameObject->uiimage->SetColor(HoverColor);
			break;
		case hos::com::UIButton::eState::Pressed:
			m_GameObject->uiimage->SetColor(PressColor);
			break;
		case hos::com::UIButton::eState::Selected:
			m_GameObject->uiimage->SetColor(SelectedColor);
			break;
		case hos::com::UIButton::eState::Disable:
			m_GameObject->uiimage->SetColor(DisableColor);
			break;
		}
		break;
	case hos::com::UIButton::eTransitionMode::Sprite:
		switch (State)
		{
		case hos::com::UIButton::eState::None:
			m_GameObject->uiimage->SetColor(NormalColor);
			if (NormalSprite.size() > 0)
			{
				m_GameObject->uiimage->SetTexture(NormalSprite);
			}
			else
			{
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		case hos::com::UIButton::eState::Hover:
			m_GameObject->uiimage->SetColor(HoverColor);
			if (HoverSprite.size() > 0)
			{
				m_GameObject->uiimage->SetTexture(HoverSprite);
			}
			else
			{
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		case hos::com::UIButton::eState::Pressed:
			m_GameObject->uiimage->SetColor(PressColor);
			if (PressSprite.size() > 0)
			{
				m_GameObject->uiimage->SetTexture(PressSprite);
			}
			else
			{
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		case hos::com::UIButton::eState::Selected:
			m_GameObject->uiimage->SetColor(SelectedColor);
			if (SelectedSprite.size() > 0)
			{
				m_GameObject->uiimage->SetTexture(SelectedSprite);
			}
			else
			{
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		case hos::com::UIButton::eState::Disable:
			m_GameObject->uiimage->SetColor(DisableColor);
			if (DisableSprite.size() > 0)
			{
				m_GameObject->uiimage->SetTexture(DisableSprite);
			}
			else
			{
				m_GameObject->uiimage->SetTexture(DefaultSprite);
			}
			break;
		}
		break;
	}
}

hos::com::UIButton* hos::com::UIButton::Clone() const
{
	return new UIButton(*this);
}

void hos::com::UIButton::Reset()
{
	Interactable = true;

	State = eState::None;
	bSelected = false;
	Transition = eTransitionMode::Color;

	DefaultColor = DEFAULT_NORMAL_COLOR;
	NormalColor = DEFAULT_NORMAL_COLOR;
	HoverColor = DEFAULT_HOVER_COLOR;
	PressColor = DEFAULT_PRESS_COLOR;
	SelectedColor = DEFAULT_SELECTED_COLOR;
	DisableColor = DEFAULT_DISABLE_COLOR;
	DefaultSprite = L"";
	NormalSprite = L"";
	HoverSprite = L"";
	PressSprite = L"";
	SelectedSprite = L"";
	DisableSprite = L"";
}

void hos::com::UIButton::OnEnable()
{
	bSelected = false;
	if (m_GameObject->uiimage)
	{
		DefaultColor = m_GameObject->uiimage->GetColor();
		DefaultSprite = m_GameObject->uiimage->GetTextureName();
	}
}

void hos::com::UIButton::OnDisable()
{
	bSelected = false;
	if (m_GameObject && m_GameObject->uiimage)
	{
		if (DefaultSprite.size() > 0)
		{
			m_GameObject->uiimage->SetIsTexture(true);
			m_GameObject->uiimage->SetTexture(DefaultSprite);
		}
		m_GameObject->uiimage->SetColor(DefaultColor);
	}
}

const std::vector<U8> hos::com::UIButton::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring ns = ut::UTF16ToAnsi(NormalSprite);
	mbstring hs = ut::UTF16ToAnsi(HoverSprite);
	mbstring ps = ut::UTF16ToAnsi(PressSprite);
	mbstring ss = ut::UTF16ToAnsi(SelectedSprite);
	mbstring ds = ut::UTF16ToAnsi(DisableSprite);

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.Bool("Interactable", Interactable);
		builder.Int("Transition", static_cast<I32>(Transition));
		builder.TypedVector("NormalColor", [&] {
			builder.Float(NormalColor.x);
			builder.Float(NormalColor.y);
			builder.Float(NormalColor.z);
			builder.Float(NormalColor.w);
			});
		builder.TypedVector("HoverColor", [&] {
			builder.Float(HoverColor.x);
			builder.Float(HoverColor.y);
			builder.Float(HoverColor.z);
			builder.Float(HoverColor.w);
			});
		builder.TypedVector("PressColor", [&] {
			builder.Float(PressColor.x);
			builder.Float(PressColor.y);
			builder.Float(PressColor.z);
			builder.Float(PressColor.w);
			});
		builder.TypedVector("SelectedColor", [&] {
			builder.Float(SelectedColor.x);
			builder.Float(SelectedColor.y);
			builder.Float(SelectedColor.z);
			builder.Float(SelectedColor.w);
			});
		builder.TypedVector("DisableColor", [&] {
			builder.Float(DisableColor.x);
			builder.Float(DisableColor.y);
			builder.Float(DisableColor.z);
			builder.Float(DisableColor.w);
			});
		builder.String("NormalSprite", ns);
		builder.String("HoverSprite", hs);
		builder.String("PressSprite", ps);
		builder.String("SelectedSprite", ss);
		builder.String("DisableSprite", ds);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::UIButton::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"UI_Button try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	Interactable = m["Interactable"].AsBool();
	int t = m["Transition"].AsInt32();
	Transition = static_cast<eTransitionMode>(t);

	auto normalColor = m["NormalColor"].AsTypedVector();
	NormalColor.x = normalColor[0].AsFloat();
	NormalColor.y = normalColor[1].AsFloat();
	NormalColor.z = normalColor[2].AsFloat();
	NormalColor.w = normalColor[3].AsFloat();

	auto hoverColor = m["HoverColor"].AsTypedVector();
	HoverColor.x = hoverColor[0].AsFloat();
	HoverColor.y = hoverColor[1].AsFloat();
	HoverColor.z = hoverColor[2].AsFloat();
	HoverColor.w = hoverColor[3].AsFloat();

	auto pressColor = m["PressColor"].AsTypedVector();
	PressColor.x = pressColor[0].AsFloat();
	PressColor.y = pressColor[1].AsFloat();
	PressColor.z = pressColor[2].AsFloat();
	PressColor.w = pressColor[3].AsFloat();

	auto selectedColor = m["SelectedColor"].AsTypedVector();
	SelectedColor.x = selectedColor[0].AsFloat();
	SelectedColor.y = selectedColor[1].AsFloat();
	SelectedColor.z = selectedColor[2].AsFloat();
	SelectedColor.w = selectedColor[3].AsFloat();

	auto disableColor = m["DisableColor"].AsTypedVector();
	DisableColor.x = disableColor[0].AsFloat();
	DisableColor.y = disableColor[1].AsFloat();
	DisableColor.z = disableColor[2].AsFloat();
	DisableColor.w = disableColor[3].AsFloat();

	mbstring str;
	str = m["NormalSprite"].AsString().str();
	NormalSprite = ut::AnsiToUTF16(str);

	g_DataManager->LoadTexture(NormalSprite);

	str = m["HoverSprite"].AsString().str();
	HoverSprite = ut::AnsiToUTF16(str);

	g_DataManager->LoadTexture(HoverSprite);

	str = m["PressSprite"].AsString().str();
	PressSprite = ut::AnsiToUTF16(str);

	g_DataManager->LoadTexture(PressSprite);

	str = m["SelectedSprite"].AsString().str();
	SelectedSprite = ut::AnsiToUTF16(str);

	g_DataManager->LoadTexture(SelectedSprite);

	str = m["DisableSprite"].AsString().str();
	DisableSprite = ut::AnsiToUTF16(str);

	g_DataManager->LoadTexture(DisableSprite);

	State = eState::None;
	bSelected = false;
	DefaultColor = DEFAULT_NORMAL_COLOR;
	DefaultSprite = L"";

	return true;
}
