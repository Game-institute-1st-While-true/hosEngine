// UIImageView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "UIImageView.h"


// UIImageView

IMPLEMENT_DYNCREATE(UIImageView, CFormView)

UIImageView::UIImageView()
	: CFormView(IDD_UIImageView)
{

}

UIImageView::~UIImageView()
{
}

void UIImageView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LAYER_ORDER, EditLayerOrder);
	DDX_Control(pDX, IDC_EDIT_COLOR_R, EditColorR);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, EditColorG);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, EditColorB);
	DDX_Control(pDX, IDC_EDIT_COLOR_A, EditColorA);
	DDX_Control(pDX, IDC_EDIT_OFF_X, EditOffX);
	DDX_Control(pDX, IDC_EDIT_TIL_X, EditTilX);
	DDX_Control(pDX, IDC_EDIT_OFF_Y, EditOffY);
	DDX_Control(pDX, IDC_EDIT_TIL_Y, EditTilY);
	DDX_Control(pDX, IDC_STATIC_TEXTURE_NAME, StaticTextureName);
	DDX_Control(pDX, IDC_STATIC_ANIMATION_NAME, StaticAnimationName);
	DDX_Control(pDX, IDC_LIST_TEXTURE_LIST, ListTextureList);
	DDX_Control(pDX, IDC_LIST_ANIMATION_LIST, ListAnimationList);
	DDX_Control(pDX, IDC_CHECK_USE_TEXTURE, CheckUseTexture);
	DDX_Control(pDX, IDC_CHECK_USE_ANIMATION, CheckUseAnimation);
	DDX_Control(pDX, IDC_COMBO_ALIGNMENT, ComboAlignment);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_EDIT_WIDTH, EditWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, EditHeight);
	DDX_Control(pDX, IDC_CHECK_FOLLOW_UI, CheckFollowUI);
	DDX_Control(pDX, IDC_CHECK1, IsLoopCheck);
	DDX_Control(pDX, IDC_EDIT_COORD_U, EditCoordU);
	DDX_Control(pDX, IDC_EDIT_COORD_V, EditCoordV);
}

BEGIN_MESSAGE_MAP(UIImageView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_LAYER_ORDER, &UIImageView::OnEnChangeEditLayerOrder)
	ON_EN_CHANGE(IDC_EDIT_COLOR_R, &UIImageView::OnEnChangeEditColorR)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, &UIImageView::OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, &UIImageView::OnEnChangeEditColorB)
	ON_EN_CHANGE(IDC_EDIT_COLOR_A, &UIImageView::OnEnChangeEditColorA)
	ON_EN_CHANGE(IDC_EDIT_OFF_X, &UIImageView::OnEnChangeEditOffX)
	ON_EN_CHANGE(IDC_EDIT_TIL_X, &UIImageView::OnEnChangeEditTilX)
	ON_EN_CHANGE(IDC_EDIT_OFF_Y, &UIImageView::OnEnChangeEditOffY)
	ON_EN_CHANGE(IDC_EDIT_TIL_Y, &UIImageView::OnEnChangeEditTilY)
	ON_BN_CLICKED(IDC_CHECK_USE_TEXTURE, &UIImageView::OnBnClickedCheckUseTexture)
	ON_BN_CLICKED(IDC_CHECK_USE_ANIMATION, &UIImageView::OnBnClickedCheckUseAnimation)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_LOAD, &UIImageView::OnBnClickedButtonTextureLoad)
	ON_BN_CLICKED(IDC_BUTTON_ANIMATION_LOAD, &UIImageView::OnBnClickedButtonAnimationLoad)
	ON_CBN_CLOSEUP(IDC_COMBO_ALIGNMENT, &UIImageView::OnCbnCloseupComboAlignment)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &UIImageView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, &UIImageView::OnEnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &UIImageView::OnEnChangeEditHeight)
	ON_BN_CLICKED(IDC_CHECK_FOLLOW_UI, &UIImageView::OnBnClickedCheckFollowUi)
	ON_BN_CLICKED(IDC_CHECK1, &UIImageView::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT_COORD_U, &UIImageView::OnEnChangeEditCoordU)
	ON_EN_CHANGE(IDC_EDIT_COORD_V, &UIImageView::OnEnChangeEditCoordV)
END_MESSAGE_MAP()


// UIImageView 진단

#ifdef _DEBUG
void UIImageView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void UIImageView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void UIImageView::ResetUIImageView()
{

}

void UIImageView::SetUIImageView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	// 레이어오더
	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIImage>()->GetLayerOrder());
	EditLayerOrder.SetWindowTextW(_tempString);

	// 컬러
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetColor().x);
	EditColorR.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetColor().y);
	EditColorG.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetColor().z);
	EditColorB.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetColor().w);
	EditColorA.SetWindowTextW(_tempString);

	// 오프틸
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetOffTil().x);
	EditOffX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetOffTil().y);
	EditTilX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetOffTil().z);
	EditOffY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::UIImage>()->GetOffTil().w);
	EditTilY.SetWindowTextW(_tempString);

	// 폭, 높이
	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIImage>()->GetWidthSize());
	EditWidth.SetWindowTextW(_tempString);
	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIImage>()->GetHeightSize());
	EditHeight.SetWindowTextW(_tempString);


	// 텍스쳐가 있는지
	_tempString = L"Texture Name : ";
	if (_NowGameObject->GetComponent<hos::com::UIImage>()->GetIsTexture())
	{
		// 있으면 이름
		_tempString.Append(_NowGameObject->GetComponent<hos::com::UIImage>()->GetTextureName().c_str());
		CheckUseTexture.SetCheck(true);
	}
	else
	{
		CheckUseTexture.SetCheck(false);
	}
	StaticTextureName.SetWindowTextW(_tempString);

	// Alignment
	ComboAlignment.SetCurSel(_NowGameObject->GetComponent<hos::com::UIImage>()->GetAlignType() - 1);

	// 애니메이션이 있는지
	_tempString = L"Animation Name : ";
	if (_NowGameObject->GetComponent<hos::com::UIImage>()->GetIsAnimation())
	{
		// 있으면 이름
		_tempString.Append(_NowGameObject->GetComponent<hos::com::UIImage>()->GetAnimationName().c_str());
		CheckUseAnimation.SetCheck(true);
	}
	else
	{
		CheckUseAnimation.SetCheck(false);
	}
	StaticAnimationName.SetWindowTextW(_tempString);

	// 텍스쳐 리스트 갱신
	while (ListTextureList.GetCount() != 0)
	{
		ListTextureList.DeleteString(0);
	}

	for (auto [name, data] : g_DataManager->TextureList)
	{
		ListTextureList.AddString(name.c_str());
	}

	// 애니메이션 리스트 갱신
	while (ListAnimationList.GetCount() != 0)
	{
		ListAnimationList.DeleteString(0);
	}

	for (auto [name, data] : g_DataManager->UIAnimationList)
	{
		ListAnimationList.AddString(name.c_str());
	}

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::UIImage>()->GetActive());

	CheckFollowUI.SetCheck(_NowGameObject->GetComponent<hos::com::UIImage>()->GetFollowUI());
	IsLoopCheck.SetCheck(_NowGameObject->GetComponent<hos::com::UIImage>()->GetIsLoop());

	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIImage>()->GetCoordU());
	EditCoordU.SetWindowTextW(_tempString);

	_tempString.Format(L"%d", _NowGameObject->GetComponent<hos::com::UIImage>()->GetCoordV());
	EditCoordV.SetWindowTextW(_tempString);

}

// UIImageView 메시지 처리기


BOOL UIImageView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void UIImageView::OnInitialUpdate()
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


void UIImageView::OnEnChangeEditLayerOrder()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetLayerOrder(_tstoi(_tempString));
		}

	}

}


void UIImageView::OnEnChangeEditColorR()
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

			CString _tempString;
			EditColorR.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetColor(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditColorG()
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

			CString _tempString;
			EditColorG.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetColor(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditColorB()
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

			CString _tempString;
			EditColorB.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().y;
			_temp.z = _tstof(_tempString);
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetColor(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditColorA()
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

			CString _tempString;
			EditColorA.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetColor().z;
			_temp.w = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetColor(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditOffX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditOffX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector4 _temp;

			CString _tempString;
			EditOffX.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetOffTil(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditTilX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditTilX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector4 _temp;

			CString _tempString;
			EditTilX.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().z;
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetOffTil(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditOffY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditOffY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector4 _temp;

			CString _tempString;
			EditOffY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().y;
			_temp.z = _tstof(_tempString);
			_temp.w = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().w;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetOffTil(_temp);
		}

	}
}


void UIImageView::OnEnChangeEditTilY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditTilY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector4 _temp;

			CString _tempString;
			EditTilY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetOffTil().z;
			_temp.w = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetOffTil(_temp);
		}

	}
}


void UIImageView::OnBnClickedCheckUseTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 사용하는지 안하는지?
	// 사용 안 할 시?
	
	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetIsTexture())
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetIsTexture(false);
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->RemoveTexture();
		CheckUseTexture.SetCheck(false);
	}
	else
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetIsTexture(true);
		CheckUseTexture.SetCheck(true);
	}

}


void UIImageView::OnBnClickedCheckUseAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->GetIsAnimation())
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetIsAnimation(false);
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->RemoveAnimation();
		CheckUseAnimation.SetCheck(false);
	}
	else
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetIsAnimation(true);
		CheckUseAnimation.SetCheck(true);
	}
}


void UIImageView::OnBnClickedButtonTextureLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListTextureList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _TextureName;
	ListTextureList.GetText(ListTextureList.GetCurSel(), _TextureName);

	_NowGameObject->GetComponent<hos::com::UIImage>()->SetTexture(T2W(_TextureName.GetBuffer()));
	// 정보 초기화

	SetUIImageView();

}


void UIImageView::OnBnClickedButtonAnimationLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListAnimationList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _AnimationName;
	ListAnimationList.GetText(ListAnimationList.GetCurSel(), _AnimationName);

	_NowGameObject->GetComponent<hos::com::UIImage>()->SetAnimation(T2W(_AnimationName.GetBuffer()));

	SetUIImageView();

}


void UIImageView::OnCbnCloseupComboAlignment()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboAlignment.GetCurSel() && ComboAlignment.GetCurSel() < 10)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetAlignType(ComboAlignment.GetCurSel() + 1);
	}
}


void UIImageView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetActive(CheckComponentActive.GetCheck());
}


void UIImageView::OnEnChangeEditWidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditWidth.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditWidth.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetWidth(_tstoi(_tempString));
		}

	}

}


void UIImageView::OnEnChangeEditHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditHeight.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditHeight.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetHeight(_tstoi(_tempString));
		}

	}
}


void UIImageView::OnBnClickedCheckFollowUi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetFollowUI(CheckFollowUI.GetCheck());
}


void UIImageView::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetIsLoop(IsLoopCheck.GetCheck());
}


void UIImageView::OnEnChangeEditCoordU()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetCoordU(_tstoi(_tempString));
		}

	}
}


void UIImageView::OnEnChangeEditCoordV()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::UIImage>()->SetCoordV(_tstoi(_tempString));
		}

	}
}
