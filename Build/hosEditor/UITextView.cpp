// UITextView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "UITextView.h"


// UITextView

IMPLEMENT_DYNCREATE(UITextView, CFormView)

UITextView::UITextView()
	: CFormView(IDD_UITextView)
{

}

UITextView::~UITextView()
{
}

void UITextView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LAYER_ORDER, EditLayerOrder);
	DDX_Control(pDX, IDC_EDIT_FONT_SIZE, EditFontSize);
	DDX_Control(pDX, IDC_EDIT_FONT_ROTATION, EditFontRotation);
	DDX_Control(pDX, IDC_STATIC_FONT_NAME, StaticFontName);
	DDX_Control(pDX, IDC_LIST_FONT_LIST, ListFontList);
	DDX_Control(pDX, IDC_EDIT_COLOR_R, EditColorR);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, EditColorG);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, EditColorB);
	DDX_Control(pDX, IDC_EDIT_COLOR_A, EditColorA);
	DDX_Control(pDX, IDC_EDIT_TEXT, EditText);
	DDX_Control(pDX, IDC_COMBO_ALIGNMENT, ComboAlignment);
	DDX_Control(pDX, IDC_CHECK_USE_ANIMATION, CheckUseAnimation);
	DDX_Control(pDX, IDC_LIST_TEXT_ANIMATION_LIST, ListTextAnimationList);
	DDX_Control(pDX, IDC_CHECK_USE_ANIMATION, CheckUseAnimation);
	DDX_Control(pDX, IDC_STATIC_ANIMATION_NAME, StaticAnimationName);
	DDX_Control(pDX, IDC_COMBO_ALIGNMENT, ComboAlignment);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_CHECK_FOLLOW_UI, CheckFollowUI);
	DDX_Control(pDX, IDC_CHECK_IS_LOOP, CheckIsLoop);
	DDX_Control(pDX, IDC_EDIT_COORD_U2, EditCoordU);
	DDX_Control(pDX, IDC_EDIT_COORD_V, EditCoordV);
}

BEGIN_MESSAGE_MAP(UITextView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_LAYER_ORDER, &UITextView::OnEnChangeEditLayerOrder)
	ON_EN_CHANGE(IDC_EDIT_FONT_SIZE, &UITextView::OnEnChangeEditFontSize)
	ON_EN_CHANGE(IDC_EDIT_FONT_ROTATION, &UITextView::OnEnChangeEditFontRotation)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, &UITextView::OnEnChangeEditColorR)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, &UITextView::OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, &UITextView::OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, &UITextView::OnEnChangeEditColorA)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_FONT, &UITextView::OnBnClickedButtonLoadFont)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &UITextView::OnEnChangeEditText)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_ANIMATION, &UITextView::OnBnClickedButtonLoadAnimation)
	ON_BN_CLICKED(IDC_CHECK_USE_ANIMATION, &UITextView::OnBnClickedCheckUseAnimation)
	ON_CBN_CLOSEUP(IDC_COMBO_ALIGNMENT, &UITextView::OnCbnCloseupComboAlignment)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_FONT, &UITextView::OnBnClickedButtonDefaultFont)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &UITextView::OnBnClickedCheckComponentActive)
	ON_BN_CLICKED(IDC_CHECK_FOLLOW_UI, &UITextView::OnBnClickedCheckFollowUi)
	ON_BN_CLICKED(IDC_CHECK_IS_LOOP, &UITextView::OnBnClickedCheckIsLoop)
	ON_EN_CHANGE(IDC_EDIT_COORD_U2, &UITextView::OnEnChangeEditCoordU2)
	ON_EN_CHANGE(IDC_EDIT_COORD_V, &UITextView::OnEnChangeEditCoordV)
END_MESSAGE_MAP()


// UITextView 진단

#ifdef _DEBUG
void UITextView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void UITextView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void UITextView::ResetUITextView()
{

}

void UITextView::SetUITextView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	// 레이어오더
	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIText>()->GetLayerOrder());
	EditLayerOrder.SetWindowTextW(_tempString);

	// 폰트 사이즈
	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontSize());
	EditFontSize.SetWindowTextW(_tempString);

	// 폰트 로테이션
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontRot());
	EditFontRotation.SetWindowTextW(_tempString);

	// 컬러
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontColor().x);
	EditColorR.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontColor().y);
	EditColorG.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontColor().z);
	EditColorB.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIText>()->GetFontColor().w);
	EditColorA.SetWindowTextW(_tempString);

	// 폰트 이름
	_tempString = L"Font Name : ";
	if (_NowGameObject->GetComponent<hos::com::UIText>()->GetFontData())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::UIText>()->GetFontName().c_str());
	}
	StaticFontName.SetWindowTextW(_tempString);

	_tempString = _NowGameObject->GetComponent<hos::com::UIText>()->GetText().c_str();
	EditText.SetWindowTextW(_tempString);

	// 애니메이션 이름
	_tempString = L"Animation Name : ";
	if (_NowGameObject->GetComponent<hos::com::UIText>()->GetIsAnimation())
	{
		// 있으면 이름
		_tempString.Append(_NowGameObject->GetComponent<hos::com::UIText>()->GetAnimationName().c_str());
	}

	StaticAnimationName.SetWindowTextW(_tempString);

	if (_NowGameObject->GetComponent<hos::com::UIText>()->GetIsAnimation())
	{
		CheckUseAnimation.SetCheck(true);
	}
	else
	{
		CheckUseAnimation.SetCheck(false);
	}

	// Alignment
	ComboAlignment.SetCurSel(_NowGameObject->GetComponent<hos::com::UIText>()->GetAlignType() - 1);


	// 폰트 리스트 갱신
	while (ListFontList.GetCount() != 0)
	{
		ListFontList.DeleteString(0);
	}

	for (auto [name, data] : g_DataManager->Fonts)
	{
		ListFontList.AddString(name.c_str());
	}

	while (ListTextAnimationList.GetCount() != 0)
	{
		ListTextAnimationList.DeleteString(0);
	}

	for (auto [name, data] : g_DataManager->UIAnimationList)
	{
		ListTextAnimationList.AddString(name.c_str());
	}

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::UIText>()->GetActive());

	CheckFollowUI.SetCheck(_NowGameObject->GetComponent<hos::com::UIText>()->GetFollowText());

	CheckIsLoop.SetCheck(_NowGameObject->GetComponent<hos::com::UIText>()->GetIsLoop());

	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIText>()->GetCoordU());
	EditCoordU.SetWindowTextW(_tempString);

	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIText>()->GetCoordV());
	EditCoordV.SetWindowTextW(_tempString);
}

// UITextView 메시지 처리기


BOOL UITextView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void UITextView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	ComboAlignment.AddString(L"LeftBottom");
	ComboAlignment.AddString(L"Bottom");
	ComboAlignment.AddString(L"RightBottom");
	ComboAlignment.AddString(L"Left");
	ComboAlignment.AddString(L"Center");
	ComboAlignment.AddString(L"Right");
	ComboAlignment.AddString(L"LeftTop");
	ComboAlignment.AddString(L"Top");
	ComboAlignment.AddString(L"RightTop");


}


void UITextView::OnEnChangeEditLayerOrder()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditLayerOrder.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditLayerOrder.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetLayerOrder(_tstoi(_tempString));
		}

	}
}


void UITextView::OnEnChangeEditFontSize()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditFontSize.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditFontSize.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontSize(_tstoi(_tempString));
		}

	}
}


void UITextView::OnEnChangeEditFontRotation()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditFontRotation.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditFontRotation.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontRot(_tstof(_tempString));
		}

	}
}

void UITextView::OnEnChangeEditColorR()
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
			hos::Vector4 _temp;

			CString _tempString;
			EditColorR.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontColor(_temp);
		}

	}
}


void UITextView::OnEnChangeEditColorG()
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
			hos::Vector4 _temp;

			CString _tempString;
			EditColorG.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontColor(_temp);
		}

	}
}


void UITextView::OnEnChangeEditColorB()
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
			hos::Vector4 _temp;

			CString _tempString;
			EditColorB.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().y;
			_temp.z = _tstof(_tempString);
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontColor(_temp);
		}

	}
}


void UITextView::OnEnChangeEditColorA()
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
			hos::Vector4 _temp;

			CString _tempString;
			EditColorA.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetFontColor().z;
			_temp.w = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFontColor(_temp);
		}

	}
}


void UITextView::OnBnClickedButtonLoadFont()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListFontList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _FontName;
	ListFontList.GetText(ListFontList.GetCurSel(), _FontName);

	_NowGameObject->GetComponent<hos::com::UIText>()->SetFont(T2W(_FontName.GetBuffer()));
	// 정보 초기화

	SetUITextView();

}


void UITextView::OnEnChangeEditText()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditText.GetWindowTextLengthW();

	if (_Length >= 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector2 _temp;

			CString _tempString;
			EditText.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetTextSafe(T2W(_tempString.GetBuffer()));

		}

	}
}


void UITextView::OnBnClickedButtonLoadAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListTextAnimationList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _AnimationName;
	ListTextAnimationList.GetText(ListTextAnimationList.GetCurSel(), _AnimationName);

	_NowGameObject->GetComponent<hos::com::UIText>()->SetAnimation(T2W(_AnimationName.GetBuffer()));

	SetUITextView();
}


void UITextView::OnBnClickedCheckUseAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->GetIsAnimation())
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetIsAnimation(false);
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->RemoveAnimation();
		CheckUseAnimation.SetCheck(false);
	}
	else
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetIsAnimation(true);
		CheckUseAnimation.SetCheck(true);
	}

}


void UITextView::OnCbnCloseupComboAlignment()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboAlignment.GetCurSel() && ComboAlignment.GetCurSel() < 10)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetAlignType(ComboAlignment.GetCurSel() + 1);
	}
}


void UITextView::OnBnClickedButtonDefaultFont()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _FontName = L"Arial";

	_NowGameObject->GetComponent<hos::com::UIText>()->SetFont(T2W(_FontName.GetBuffer()));
	// 정보 초기화

	SetUITextView();

}


void UITextView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetActive(CheckComponentActive.GetCheck());
}


void UITextView::OnBnClickedCheckFollowUi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetFollowText(CheckFollowUI.GetCheck());
}


void UITextView::OnBnClickedCheckIsLoop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetIsLoop(CheckIsLoop.GetCheck());
}


void UITextView::OnEnChangeEditCoordU2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCoordU.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditCoordU.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetCoordU(_tstoi(_tempString));
		}

	}
}


void UITextView::OnEnChangeEditCoordV()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCoordV.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditCoordV.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIText>()->SetCoordV(_tstoi(_tempString));
		}

	}
}
