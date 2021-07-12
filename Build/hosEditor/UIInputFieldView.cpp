// UIInputFieldView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "UIInputFieldView.h"


// UIInputFieldView

IMPLEMENT_DYNCREATE(UIInputFieldView, CFormView)

UIInputFieldView::UIInputFieldView()
	: CFormView(IDD_UIInputFieldView)
{

}

UIInputFieldView::~UIInputFieldView()
{
}

void UIInputFieldView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRANSITION, ComboTransition);
	DDX_Control(pDX, IDC_COMBO_SELECT_STATE, ComboSelectState);
	DDX_Control(pDX, IDC_CHECK_INTERACTABLE, CheckInteractable);
	DDX_Control(pDX, IDC_COMBO_MODE, ComboMode);
	DDX_Control(pDX, IDC_EDIT_TEXT_LIMIT, EditTextLimit);
	DDX_Control(pDX, IDC_EDIT_COLOR_R, EditColorR);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, EditColorG);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, EditColorB);
	DDX_Control(pDX, IDC_EDIT_COLOR_A, EditColorA);
	DDX_Control(pDX, IDC_STATIC_SPRITE_NAME, StaticSpriteName);
	DDX_Control(pDX, IDC_LIST_SPRITE_LIST, ListSpriteList);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
}

BEGIN_MESSAGE_MAP(UIInputFieldView, CFormView)
	ON_CBN_CLOSEUP(IDC_COMBO_TRANSITION, &UIInputFieldView::OnCbnCloseupComboTransition)
	ON_CBN_CLOSEUP(IDC_COMBO_SELECT_STATE, &UIInputFieldView::OnCbnCloseupComboSelectState)
	ON_BN_CLICKED(IDC_CHECK_INTERACTABLE, &UIInputFieldView::OnBnClickedCheckInteractable)
	ON_CBN_CLOSEUP(IDC_COMBO_MODE, &UIInputFieldView::OnCbnCloseupComboMode)
	ON_EN_CHANGE(IDC_EDIT_TEXT_LIMIT, &UIInputFieldView::OnEnChangeEditTextLimit)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, &UIInputFieldView::OnEnChangeEditColorR)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, &UIInputFieldView::OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, &UIInputFieldView::OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, &UIInputFieldView::OnEnChangeEditColorA)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SPRITE, &UIInputFieldView::OnBnClickedButtonLoadSprite)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SPRITE, &UIInputFieldView::OnBnClickedButtonRemoveSprite)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &UIInputFieldView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// UIInputFieldView 진단

#ifdef _DEBUG
void UIInputFieldView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void UIInputFieldView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void UIInputFieldView::ResetUIInputFieldView()
{

}

void UIInputFieldView::SetUIInputFieldView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::UIInputField* _UIInutField = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

	CString _tempString;

	// 선택 돼어 있는 State Index에 따라 다른 정보를 출력하자

	ComboTransition.SetCurSel((int)_UIInutField->GetTransition());

	CheckInteractable.SetCheck(_UIInutField->GetInteractable());

	ComboSelectState.SetCurSel(StateIndex);
	_UIInutField->SetState(static_cast<hos::com::UIInputField::eState>(StateIndex));

	ComboMode.SetCurSel((int)_UIInutField->GetMode());

	_tempString.Format(L"%d", _UIInutField->GetTextLimit());
	EditTextLimit.SetWindowTextW(_tempString);

	switch (StateIndex)
	{
	case 0:
	{
		_tempString.Format(L"%f", _UIInutField->GetNormalColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetNormalColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetNormalColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetNormalColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Normal Sprite Name : ";

		_tempString.Append(_UIInutField->GetNormalSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 1:
	{
		_tempString.Format(L"%f", _UIInutField->GetHoverColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetHoverColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetHoverColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetHoverColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Hover Sprite Name : ";

		_tempString.Append(_UIInutField->GetHoverSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 2:
	{
		_tempString.Format(L"%f", _UIInutField->GetPressColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetPressColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetPressColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetPressColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Press Sprite Name : ";

		_tempString.Append(_UIInutField->GetPressSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 3:
	{
		_tempString.Format(L"%f", _UIInutField->GetSelectedColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetSelectedColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetSelectedColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetSelectedColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Selected Sprite Name : ";

		_tempString.Append(_UIInutField->GetSelectedSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 4:
	{
		_tempString.Format(L"%f", _UIInutField->GetDisableColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetDisableColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetDisableColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIInutField->GetDisableColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Disable Sprite Name : ";

		_tempString.Append(_UIInutField->GetDisableSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	default:
		break;
	}

	while (ListSpriteList.GetCount() != 0)
	{
		ListSpriteList.DeleteString(0);
	}

	for (auto [name, data] : g_DataManager->TextureList)
	{
		ListSpriteList.AddString(name.c_str());
	}
}

// UIInputFieldView 메시지 처리기


BOOL UIInputFieldView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void UIInputFieldView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	StateIndex = 0;

	ComboTransition.AddString(L"None");
	ComboTransition.AddString(L"Color");
	ComboTransition.AddString(L"Sprite");

	ComboSelectState.AddString(L"Normal");
	ComboSelectState.AddString(L"Hover");
	ComboSelectState.AddString(L"Pressed");
	ComboSelectState.AddString(L"Selected");
	ComboSelectState.AddString(L"Disable");

	ComboMode.AddString(L"Default");
	ComboMode.AddString(L"Password");
	ComboMode.AddString(L"Number");

}


void UIInputFieldView::OnCbnCloseupComboTransition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		if (0 <= ComboTransition.GetCurSel() && ComboTransition.GetCurSel() < 3)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>()->ChangeTransition((hos::com::UIButton::eTransitionMode)ComboTransition.GetCurSel());
	}

}


void UIInputFieldView::OnCbnCloseupComboSelectState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboSelectState.GetCurSel() && ComboSelectState.GetCurSel() < 5)
	{
		StateIndex = ComboSelectState.GetCurSel();
		SetUIInputFieldView();
	}
}


void UIInputFieldView::OnBnClickedCheckInteractable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>()->SetInteractable(CheckInteractable.GetCheck());

}


void UIInputFieldView::OnCbnCloseupComboMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboMode.GetCurSel() && ComboMode.GetCurSel() < 3)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>()->SetMode((hos::com::UIInputField::eMode)ComboMode.GetCurSel());
	}
}


void UIInputFieldView::OnEnChangeEditTextLimit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int _Length = EditTextLimit.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditTextLimit.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>()->SetTextLimit(_tstoi(_tempString));
		}

	}

}


void UIInputFieldView::OnEnChangeEditColorR()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorR.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

			CString _tempString;
			EditColorR.GetWindowTextW(_tempString);

			switch (StateIndex)
			{
			case 0:
			{
				_temp.x = _tstof(_tempString);
				_temp.y = _UIButton->GetNormalColor().y;
				_temp.z = _UIButton->GetNormalColor().z;
				_temp.w = _UIButton->GetNormalColor().w;

				_UIButton->SetNormalColor(_temp);
			}
			break;
			case 1:
			{
				_temp.x = _tstof(_tempString);
				_temp.y = _UIButton->GetHoverColor().y;
				_temp.z = _UIButton->GetHoverColor().z;
				_temp.w = _UIButton->GetHoverColor().w;

				_UIButton->SetHoverColor(_temp);
			}
			break;
			case 2:
			{
				_temp.x = _tstof(_tempString);
				_temp.y = _UIButton->GetPressColor().y;
				_temp.z = _UIButton->GetPressColor().z;
				_temp.w = _UIButton->GetPressColor().w;

				_UIButton->SetPressColor(_temp);
			}
			break;
			case 3:
			{
				_temp.x = _tstof(_tempString);
				_temp.y = _UIButton->GetSelectedColor().y;
				_temp.z = _UIButton->GetSelectedColor().z;
				_temp.w = _UIButton->GetSelectedColor().w;

				_UIButton->SetSelectedColor(_temp);
			}
			break;
			case 4:
			{
				_temp.x = _tstof(_tempString);
				_temp.y = _UIButton->GetDisableColor().y;
				_temp.z = _UIButton->GetDisableColor().z;
				_temp.w = _UIButton->GetDisableColor().w;

				_UIButton->SetDisableColor(_temp);
			}
			break;
			default:
				break;
			}
		}

	}
}


void UIInputFieldView::OnEnChangeEditColorG()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorG.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

			CString _tempString;
			EditColorG.GetWindowTextW(_tempString);

			switch (StateIndex)
			{
			case 0:
			{
				_temp.x = _UIButton->GetNormalColor().x;
				_temp.y = _tstof(_tempString);
				_temp.z = _UIButton->GetNormalColor().z;
				_temp.w = _UIButton->GetNormalColor().w;

				_UIButton->SetNormalColor(_temp);
			}
			break;
			case 1:
			{
				_temp.x = _UIButton->GetHoverColor().x;
				_temp.y = _tstof(_tempString);
				_temp.z = _UIButton->GetHoverColor().z;
				_temp.w = _UIButton->GetHoverColor().w;

				_UIButton->SetHoverColor(_temp);
			}
			break;
			case 2:
			{
				_temp.x = _UIButton->GetPressColor().x;
				_temp.y = _tstof(_tempString);
				_temp.z = _UIButton->GetPressColor().z;
				_temp.w = _UIButton->GetPressColor().w;

				_UIButton->SetPressColor(_temp);
			}
			break;
			case 3:
			{
				_temp.x = _UIButton->GetSelectedColor().x;
				_temp.y = _tstof(_tempString);
				_temp.z = _UIButton->GetSelectedColor().z;
				_temp.w = _UIButton->GetSelectedColor().w;

				_UIButton->SetSelectedColor(_temp);
			}
			break;
			case 4:
			{
				_temp.x = _UIButton->GetDisableColor().x;
				_temp.y = _tstof(_tempString);
				_temp.z = _UIButton->GetDisableColor().z;
				_temp.w = _UIButton->GetDisableColor().w;

				_UIButton->SetDisableColor(_temp);
			}
			break;
			default:
				break; 
			}
		}

	}
}


void UIInputFieldView::OnEnChangeEditColorB()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorB.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

			CString _tempString;
			EditColorB.GetWindowTextW(_tempString);

			switch (StateIndex)
			{
			case 0:
			{
				_temp.x = _UIButton->GetNormalColor().x;
				_temp.y = _UIButton->GetNormalColor().y;
				_temp.z = _tstof(_tempString);
				_temp.w = _UIButton->GetNormalColor().w;

				_UIButton->SetNormalColor(_temp);
			}
			break;
			case 1:
			{
				_temp.x = _UIButton->GetHoverColor().x;
				_temp.y = _UIButton->GetHoverColor().y;
				_temp.z = _tstof(_tempString);
				_temp.w = _UIButton->GetHoverColor().w;

				_UIButton->SetHoverColor(_temp);
			}
			break;
			case 2:
			{
				_temp.x = _UIButton->GetPressColor().x;
				_temp.y = _UIButton->GetPressColor().y;
				_temp.z = _tstof(_tempString);
				_temp.w = _UIButton->GetPressColor().w;

				_UIButton->SetPressColor(_temp);
			}
			break;
			case 3:
			{
				_temp.x = _UIButton->GetSelectedColor().x;
				_temp.y = _UIButton->GetSelectedColor().y;
				_temp.z = _tstof(_tempString);
				_temp.w = _UIButton->GetSelectedColor().w;

				_UIButton->SetSelectedColor(_temp);
			}
			break;
			case 4:
			{
				_temp.x = _UIButton->GetDisableColor().x;
				_temp.y = _UIButton->GetDisableColor().y;
				_temp.z = _tstof(_tempString);
				_temp.w = _UIButton->GetDisableColor().w;

				_UIButton->SetDisableColor(_temp);
			}
			break;
			default:
				break;
			}
		}

	}
}


void UIInputFieldView::OnEnChangeEditColorA()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorA.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

			CString _tempString;
			EditColorA.GetWindowTextW(_tempString);

			switch (StateIndex)
			{
			case 0:
			{
				_temp.x = _UIButton->GetNormalColor().x;
				_temp.y = _UIButton->GetNormalColor().y;
				_temp.z = _UIButton->GetNormalColor().z;
				_temp.w = _tstof(_tempString);

				_UIButton->SetNormalColor(_temp);
			}
			break;
			case 1:
			{
				_temp.x = _UIButton->GetHoverColor().x;
				_temp.y = _UIButton->GetHoverColor().y;
				_temp.z = _UIButton->GetHoverColor().z;
				_temp.w = _tstof(_tempString);

				_UIButton->SetHoverColor(_temp);
			}
			break;
			case 2:
			{
				_temp.x = _UIButton->GetPressColor().x;
				_temp.y = _UIButton->GetPressColor().y;
				_temp.z = _UIButton->GetPressColor().z;
				_temp.w = _tstof(_tempString);

				_UIButton->SetPressColor(_temp);
			}
			break;
			case 3:
			{
				_temp.x = _UIButton->GetSelectedColor().x;
				_temp.y = _UIButton->GetSelectedColor().y;
				_temp.z = _UIButton->GetSelectedColor().z;
				_temp.w = _tstof(_tempString);

				_UIButton->SetSelectedColor(_temp);
			}
			break;
			case 4:
			{
				_temp.x = _UIButton->GetDisableColor().x;
				_temp.y = _UIButton->GetDisableColor().y;
				_temp.z = _UIButton->GetDisableColor().z;
				_temp.w = _tstof(_tempString);

				_UIButton->SetDisableColor(_temp);
			}
			break;
			default:
				break;
			}
		}

	}
}


void UIInputFieldView::OnBnClickedButtonLoadSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListSpriteList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

	CString _SpriteName;
	ListSpriteList.GetText(ListSpriteList.GetCurSel(), _SpriteName);


	switch (StateIndex)
	{
	case 0:
	{
		_UIButton->SetNormalSprite(T2W(_SpriteName.GetBuffer()));
	}
	break;
	case 1:
	{
		_UIButton->SetHoverSprite(T2W(_SpriteName.GetBuffer()));
	}
	break;
	case 2:
	{
		_UIButton->SetPressSprite(T2W(_SpriteName.GetBuffer()));
	}
	break;
	case 3:
	{
		_UIButton->SetSelectedSprite(T2W(_SpriteName.GetBuffer()));
	}
	break;
	case 4:
	{
		_UIButton->SetDisableSprite(T2W(_SpriteName.GetBuffer()));
	}
	break;
	default:
		break;
	}

	SetUIInputFieldView();
}


void UIInputFieldView::OnBnClickedButtonRemoveSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	hos::com::UIInputField* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>();

	switch (StateIndex)
	{
	case 0:
	{
		_UIButton->SetNormalSprite(L"");
	}
	break;
	case 1:
	{
		_UIButton->SetHoverSprite(L"");
	}
	break;
	case 2:
	{
		_UIButton->SetPressSprite(L"");
	}
	break;
	case 3:
	{
		_UIButton->SetSelectedSprite(L"");
	}
	break;
	case 4:
	{
		_UIButton->SetDisableSprite(L"");
	}
	break;
	default:
		break;
	}

}


void UIInputFieldView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIInputField>()->SetActive(CheckComponentActive.GetCheck());
}
