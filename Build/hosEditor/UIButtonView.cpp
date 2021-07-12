// UIButtonView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "UIButtonView.h"


// UIButtonView

IMPLEMENT_DYNCREATE(UIButtonView, CFormView)

UIButtonView::UIButtonView()
	: CFormView(IDD_UIButtonView)
{

}

UIButtonView::~UIButtonView()
{
}

void UIButtonView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRANSITION, ComboTransition);
	DDX_Control(pDX, IDC_CHECK_INTERACTABLE, CheckInteractable);
	DDX_Control(pDX, IDC_COMBO_SELECT_STATE, ComboSelectState);
	DDX_Control(pDX, IDC_EDIT_COLOR_R, EditColorR);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, EditColorG);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, EditColorB);
	DDX_Control(pDX, IDC_EDIT_COLOR_A, EditColorA);
	DDX_Control(pDX, IDC_STATIC_SPRITE_NAME, StaticSpriteName);
	DDX_Control(pDX, IDC_LIST_SPRITE_LIST, ListSpriteList);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
}

BEGIN_MESSAGE_MAP(UIButtonView, CFormView)
	ON_CBN_CLOSEUP(IDC_COMBO_TRANSITION, &UIButtonView::OnCbnCloseupComboTransition)
	ON_BN_CLICKED(IDC_CHECK_INTERACTABLE, &UIButtonView::OnBnClickedCheckInteractable)
	ON_CBN_CLOSEUP(IDC_COMBO_SELECT_STATE, &UIButtonView::OnCbnCloseupComboSelectState)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, &UIButtonView::OnEnChangeEditColorR)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, &UIButtonView::OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, &UIButtonView::OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, &UIButtonView::OnEnChangeEditColorA)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SPRITE, &UIButtonView::OnBnClickedButtonLoadSprite)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SPRITE, &UIButtonView::OnBnClickedButtonRemoveSprite)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &UIButtonView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// UIButtonView 진단

#ifdef _DEBUG
void UIButtonView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void UIButtonView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void UIButtonView::ResetUIButtonView()
{

}

void UIButtonView::SetUIButtonView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

	CString _tempString;

	// 선택 돼어 있는 State Index에 따라 다른 정보를 출력하자

	ComboTransition.SetCurSel((int)_UIButton->GetTransition());

	CheckInteractable.SetCheck(_UIButton->GetInteractable());

	ComboSelectState.SetCurSel(StateIndex);
	_UIButton->SetState(static_cast<hos::com::UIButton::eState>(StateIndex));
	switch (StateIndex)
	{
	case 0:
	{
		_tempString.Format(L"%f", _UIButton->GetNormalColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetNormalColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetNormalColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetNormalColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Normal Sprite Name : ";

		_tempString.Append(_UIButton->GetNormalSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 1:
	{
		_tempString.Format(L"%f", _UIButton->GetHoverColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetHoverColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetHoverColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetHoverColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Hover Sprite Name : ";

		_tempString.Append(_UIButton->GetHoverSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 2:
	{
		_tempString.Format(L"%f", _UIButton->GetPressColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetPressColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetPressColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetPressColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Press Sprite Name : ";

		_tempString.Append(_UIButton->GetPressSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 3:
	{
		_tempString.Format(L"%f", _UIButton->GetSelectedColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetSelectedColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetSelectedColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetSelectedColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Selected Sprite Name : ";

		_tempString.Append(_UIButton->GetSelectedSprite().c_str());

		StaticSpriteName.SetWindowTextW(_tempString);
	}
	break;
	case 4:
	{
		_tempString.Format(L"%f", _UIButton->GetDisableColor().x);
		EditColorR.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetDisableColor().y);
		EditColorG.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetDisableColor().z);
		EditColorB.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _UIButton->GetDisableColor().w);
		EditColorA.SetWindowTextW(_tempString);

		_tempString = L"Disable Sprite Name : ";

		_tempString.Append(_UIButton->GetDisableSprite().c_str());

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

// UIButtonView 메시지 처리기


BOOL UIButtonView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void UIButtonView::OnInitialUpdate()
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

}


void UIButtonView::OnCbnCloseupComboTransition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (0 <= ComboTransition.GetCurSel() && ComboTransition.GetCurSel() < 3)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>()->ChangeTransition((hos::com::UIButton::eTransitionMode)ComboTransition.GetCurSel());
	}

}


void UIButtonView::OnBnClickedCheckInteractable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>()->SetInteractable(CheckInteractable.GetCheck());
}


void UIButtonView::OnCbnCloseupComboSelectState()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboSelectState.GetCurSel() && ComboSelectState.GetCurSel() < 5)
	{
		StateIndex = ComboSelectState.GetCurSel();
		SetUIButtonView();
	}
}


void UIButtonView::OnEnChangeEditColorR()
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

			hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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


void UIButtonView::OnEnChangeEditColorG()
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

			hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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


void UIButtonView::OnEnChangeEditColorB()
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

			hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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


void UIButtonView::OnEnChangeEditColorA()
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

			hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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


void UIButtonView::OnBnClickedButtonLoadSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListSpriteList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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

	SetUIButtonView();

}


void UIButtonView::OnBnClickedButtonRemoveSprite()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	hos::com::UIButton* _UIButton = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>();

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


void UIButtonView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIButton>()->SetActive(CheckComponentActive.GetCheck());
}
