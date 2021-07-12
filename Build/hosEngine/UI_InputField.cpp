#include "UI_InputField.h"

#include <locale>
#include "GameObject.h"
#include "Transform.h"
#include "UIImage.h"
#include "UI_Text.h"

using namespace hos;

hos::com::UIInputField::UIInputField()
	: UIButton()
{
	if (!g_DataManager->HasTexture(CursorImage))
	{
		g_DataManager->LoadTexture(CursorImage);
	}
	SetName(L"UIInputField");
	Reset();
}

hos::com::UIInputField::~UIInputField()
{
}

hos::com::UIInputField::UIInputField(const UIInputField& dest)
	: UIButton(dest), Mode(dest.Mode), Text(L""), TextLimit(dest.TextLimit), BlinkTime(dest.BlinkTime)
{
	SetName(L"UIInputField");
}

com::UIInputField::eMode hos::com::UIInputField::GetMode() const
{
	return Mode;
}

void hos::com::UIInputField::SetMode(eMode mode)
{
	if (m_GameObject)
	{
		UIText* text = m_GameObject->GetComponentInChildren<UIText>();
		if (text)
		{
			if (mode == eMode::Password)
			{
				string password(Text.size(), L'*');
				text->SetText(password);
			}
			else
			{
				text->SetText(Text);
			}
		}
	}

	Mode = mode;
}

U32 hos::com::UIInputField::GetTextLimit() const
{
	return TextLimit;
}

void hos::com::UIInputField::SetTextLimit(U32 limit)
{
	TextLimit = limit;
}

string hos::com::UIInputField::GetText() const
{
	return Text;
}

void hos::com::UIInputField::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->uiinputfield)
	{
		throw std::invalid_argument("this Object already has UI_InputField");
	}

	if (gameObject->uibutton)
	{
		throw std::invalid_argument("UI_InputField and UI_Button cannot be used together.");
	}

	gameObject->uiinputfield = this;
	m_GameObject = gameObject;
}

void hos::com::UIInputField::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->uiinputfield = nullptr;
		m_GameObject = nullptr;
	}
}

com::UIInputField* hos::com::UIInputField::Clone() const
{
	return new UIInputField(*this);
}

void hos::com::UIInputField::Reset()
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

	Mode = eMode::Default;
	Text = L"";
	TextLimit = DEFAULT_TEXT_LIMIT;
}

void hos::com::UIInputField::Start()
{
}

void hos::com::UIInputField::OnGUI(float aspect_x, float aspect_y)
{
	AspectX = aspect_x;
	AspectY = aspect_y;
	// UIImage 체크
	if (nullptr == m_GameObject->uiimage)
	{
		Debug->LogConsole(L"UI_InputField", m_GameObject->GetName() + L"UIImage Component가 없는 Object의 UIButton Update를 할 수 없습니다.");
		return;
	}

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

		if (Interactable)
		{
			if (bSelected)
			{
				if (State == eState::Pressed)
				{
					if (Input->GetKeyUp(KeyCode::MouseLeft))
					{
						UIText* uitext = m_GameObject->GetComponentInChildren<UIText>();
						if (uitext)
						{
							Text = uitext->GetText();
						}
						else
						{
							Text.clear();
						}
						State = eState::Selected;
					}
				}
				if (Input->GetKeyDown(KeyCode::MouseLeft))
				{
					if (PtInRect(&_rect, _cursor))
					{
						State = eState::Selected;
					}
					else
					{
						State = eState::None;
						bSelected = false;
					}
				}
			}
			else
			{
				if (PtInRect(&_rect, _cursor))
				{
					if (Input->GetKeyDown(KeyCode::MouseLeft) || Input->GetKey(KeyCode::MouseLeft))
					{
						State = eState::Pressed;
						bSelected = true;
					}
					else
					{
						State = eState::Hover;
					}
				}
				else
				{
					State = eState::None;
				}
			}
		}
		else
		{
			State = eState::Disable;
		}

		if (State == eState::Selected)
		{
			if (Input->AnyKey())
			{
				KeyCode keyCode = Input->GetInputKey();
				if (static_cast<int>(keyCode) >= 0x60 && static_cast<int>(keyCode) <= 0x69)
				{
					keyCode = static_cast<KeyCode>(static_cast<int>(keyCode) - 0x30);
				}
				wchar_t key = static_cast<wchar_t>(keyCode);
				switch (keyCode)
				{
				case KeyCode::Back:
					if (Text.size() > 0)
					{
						Text.pop_back();
					}
					break;
				case KeyCode::Enter:
					bSelected = false;
					break;
				default:
					if (TextLimit == 0 || Text.size() < TextLimit)
					{
						switch (Mode)
						{
						case hos::com::UIInputField::eMode::Default:
						case hos::com::UIInputField::eMode::Password:
							if (std::isalpha(key))
							{
								if (Input->IsCapsLock() || Input->IsShift())
								{
									Text.push_back(key);

								}
								else
								{
									if (std::isupper(key))
									{
										Text.push_back(key + 32);
									}
								}
							}
							else if (std::isdigit(key))
							{
								if (Input->IsShift())
								{
									switch (key)
									{
									case 48:
										key = 41;
										break;
									case 49:
										key = 33;
										break;
									case 50:
										key = 64;
										break;
									case 51:
										key = 35;
										break;
									case 52:
										key = 36;
										break;
									case 53:
										key = 37;
										break;
									case 54:
										key = 94;
										break;
									case 55:
										key = 38;
										break;
									case 56:
										key = 42;
										break;
									case 57:
										key = 40;
										break;
									}
								}
								Text.push_back(key);
							}
							else
							{
								switch (key)
								{
								case 189:
									if (Input->IsShift())
									{
										Text.push_back(95);
									}
									else
									{
										Text.push_back(45);
									}
									break;
								}
							}
							break;
						case hos::com::UIInputField::eMode::Number:
							if (std::isdigit(key))
							{
								Text.push_back(key);
							}
						}
						break;
					}
				}
			}
			UIText* text = m_GameObject->GetComponentInChildren<UIText>();
			if (text)
			{
				if (Mode == eMode::Password)
				{
					string password(Text.size(), L'*');
					text->SetText(password);
				}
				else
				{
					text->SetText(Text);
				}
			}
		}
	}

	switch (Transition)
	{
	case hos::com::UIButton::eTransitionMode::None:
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

void hos::com::UIInputField::PreRender()
{
	if (g_Renderer)
	{
		if (State == eState::Selected)
		{
			UIText* text = m_GameObject->GetComponentInChildren<UIText>();
			if (text)
			{
				BlinkTime += Time->DeltaTime();
				if (BlinkTime > 0.5f)
				{
					hos::Matrix mTM;
					hos::Vector3 vPos, vRot, vScale;
					vPos = m_GameObject->transform->GetPosition();
					vRot = Vector3::Zero;
					vScale = Vector3::One;
					Vector2 textSize = text->GetTextBoundSize();

					hos::cg::RenderQue::RenderUIObejct _renderObject;

					int align = text->GetAlignType();
					int width = m_GameObject->uiimage->GetWidthSize();
					int height = m_GameObject->uiimage->GetHeightSize();
					switch (align)
					{
					case 4:
						_renderObject.Pos.x = (vPos.x + textSize.x - 10) * AspectX;
						_renderObject.Pos.y = vPos.y * AspectY;
						break;
					case 6:
						_renderObject.Pos.x = (vPos.x - 19) * AspectX;
						_renderObject.Pos.y = vPos.y * AspectY;
						break;
					default:
						[[fallthrough]];
					case 7:
						_renderObject.Pos.x = (vPos.x + textSize.x - 10) * AspectX;
						_renderObject.Pos.y = (vPos.y + (height / 2)) * AspectY;
						break;
					}

					_renderObject.Rot.x = DegreeToRad(vRot.x);
					_renderObject.Rot.y = DegreeToRad(vRot.y);
					_renderObject.Rot.z = DegreeToRad(vRot.z);

					_renderObject.Scale.x = vScale.x;
					_renderObject.Scale.y = vScale.y;
					_renderObject.Color = text->GetFontColor();
					_renderObject.OffTill = Vector4(0, 1, 0, 1);
					_renderObject.LayerOrder = m_GameObject->uiimage->GetLayerOrder() + 1;
					_renderObject.Align = 4;

					_renderObject.Width = 40 * AspectX;
					_renderObject.Height = (int)((m_GameObject->uiimage->GetHeightSize() * 0.9f) * AspectY);

					_renderObject.bTexture = true;
					_renderObject.TextureName = CursorImage;

					g_Renderer->AddRenderUIObejct(_renderObject);
					if (BlinkTime > 1.f)
					{
						BlinkTime = 0.f;
					}
				}
			}
		}
	}
}

void hos::com::UIInputField::OnEnable()
{
	if (m_GameObject->uiimage)
	{
		DefaultColor = m_GameObject->uiimage->GetColor();
		DefaultSprite = m_GameObject->uiimage->GetTextureName();
	}
}

void hos::com::UIInputField::OnDisable()
{
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

const std::vector<U8> hos::com::UIInputField::Serialize()
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
		builder.Int("Mode", static_cast<I32>(Mode));
		builder.UInt("TextLimit", TextLimit);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::UIInputField::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"UI_InputField try Deserialize to" + name);
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

	int mode = m["Mode"].AsInt32();
	Mode = static_cast<eMode>(mode);

	TextLimit = m["TextLimit"].AsUInt32();

	State = eState::None;
	bSelected = false;
	DefaultColor = DEFAULT_NORMAL_COLOR;
	DefaultSprite = L"";
	Text = L"";

	return true;
}