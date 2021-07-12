// CameraView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "CameraView.h"


// CameraView

IMPLEMENT_DYNCREATE(CameraView, CFormView)

CameraView::CameraView()
	: CFormView(IDD_CameraView)
{

}

CameraView::~CameraView()
{
}

void CameraView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOV, EditFOV);
	DDX_Control(pDX, IDC_EDIT_NEAR, EditNear);
	DDX_Control(pDX, IDC_EDIT_FAR, EditFar);
	DDX_Control(pDX, IDC_EDIT_CLEAR_COLOR_R, EditClearColorR);
	DDX_Control(pDX, IDC_EDIT_CLEAR_COLOR_G, EditClearColorG);
	DDX_Control(pDX, IDC_EDIT_CLEAR_COLOR_B, EditClearColorB);
	DDX_Control(pDX, IDC_COMBO_MODE, ComboMode);
	DDX_Control(pDX, IDC_COMBO_VIEWMODE, ComboViewMode);
	DDX_Control(pDX, IDC_COMBO_CLEARFLAG, ComboClearFlag);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_EDIT_VIEW_PORT_LEFT, EditViewPortLeft);
	DDX_Control(pDX, IDC_EDIT_VIEW_PORT_TOP, EditViewPortTop);
	DDX_Control(pDX, IDC_EDIT_VIEW_PORT_RIGHT, EditViewPortRight);
	DDX_Control(pDX, IDC_EDIT_VIEW_PORT_BOTTOM, EditViewPortBottom);
	DDX_Control(pDX, IDC_STATIC_SKY_BOX_NAME, StaticSkyBoxName);
	DDX_Control(pDX, IDC_LIST_SKY_BOX_LIST, ListSkyBoxList);
}

BEGIN_MESSAGE_MAP(CameraView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_FOV, &CameraView::OnEnChangeEditFov)
	ON_EN_CHANGE(IDC_EDIT_NEAR, &CameraView::OnEnChangeEditNear)
	ON_EN_CHANGE(IDC_EDIT_FAR, &CameraView::OnEnChangeEditFar)
	ON_EN_CHANGE(IDC_EDIT_CLEAR_COLOR_R, &CameraView::OnEnChangeEditClearColorR)
	ON_EN_CHANGE(IDC_EDIT_CLEAR_COLOR_G, &CameraView::OnEnChangeEditClearColorG)
	ON_EN_CHANGE(IDC_EDIT_CLEAR_COLOR_B, &CameraView::OnEnChangeEditClearColorB)
	ON_CBN_CLOSEUP(IDC_COMBO_MODE, &CameraView::OnCbnCloseupComboMode)
	ON_CBN_CLOSEUP(IDC_COMBO_VIEWMODE, &CameraView::OnCbnCloseupComboViewmode)
	ON_CBN_CLOSEUP(IDC_COMBO_CLEARFLAG, &CameraView::OnCbnCloseupComboClearflag)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA_RESET, &CameraView::OnBnClickedButtonCameraReset)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &CameraView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_VIEW_PORT_LEFT, &CameraView::OnEnChangeEditViewPortLeft)
	ON_EN_CHANGE(IDC_EDIT_VIEW_PORT_TOP, &CameraView::OnEnChangeEditViewPortTop)
	ON_EN_CHANGE(IDC_EDIT_VIEW_PORT_RIGHT, &CameraView::OnEnChangeEditViewPortRight)
	ON_EN_CHANGE(IDC_EDIT_VIEW_PORT_BOTTOM, &CameraView::OnEnChangeEditViewPortBottom)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SKY_BOX, &CameraView::OnBnClickedButtonLoadSkyBox)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_SKY_BOX, &CameraView::OnBnClickedButtonRemoveSkyBox)
END_MESSAGE_MAP()


// CameraView 진단

#ifdef _DEBUG
void CameraView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CameraView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CameraView::ResetCameraView()
{
	EditFOV.SetWindowTextW(L"60");
	EditNear.SetWindowTextW(L"1");
	EditFar.SetWindowTextW(L"500");
	EditClearColorR.SetWindowTextW(L"0");
	EditClearColorG.SetWindowTextW(L"0.5");
	EditClearColorB.SetWindowTextW(L"0");
	ComboMode.SetCurSel(0);
	ComboViewMode.SetCurSel(0);
	ComboClearFlag.SetCurSel(0);

}

void CameraView::SetCameraView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	// ViewPort
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetViewPortLeft());
	EditViewPortLeft.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetViewPortTop());
	EditViewPortTop.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetViewPortRight());
	EditViewPortRight.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetViewPortBottom());
	EditViewPortBottom.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetFOV());
	EditFOV.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetNear());
	EditNear.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetFar());
	EditFar.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetClearColor().R());
	EditClearColorR.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetClearColor().G());
	EditClearColorG.SetWindowTextW(_tempString);	
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Camera>()->GetClearColor().B());
	EditClearColorB.SetWindowTextW(_tempString);

	ComboMode.SetCurSel((int)_NowGameObject->GetComponent<hos::com::Camera>()->GetMode());
	ComboViewMode.SetCurSel((int)_NowGameObject->GetComponent<hos::com::Camera>()->GetViewMode());
	ComboClearFlag.SetCurSel((int)_NowGameObject->GetComponent<hos::com::Camera>()->GetClearFlag());

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::Camera>()->GetActive());

	_tempString = L"Sky Box Name : ";
	_tempString.Append(_NowGameObject->GetComponent<hos::com::Camera>()->GetSkyBoxName().c_str());
	StaticSkyBoxName.SetWindowTextW(_tempString);

	while (ListSkyBoxList.GetCount() != 0)
	{
		ListSkyBoxList.DeleteString(0);
	}

	for (auto _iter : g_DataManager->SkyBoxList)
	{
		ListSkyBoxList.AddString(_iter.c_str());
	}

}

// CameraView 메시지 처리기


BOOL CameraView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CameraView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	ComboMode.AddString(L"Default");
	ComboMode.AddString(L"Follow");
	ComboViewMode.AddString(L"Perspective");
	ComboViewMode.AddString(L"Orthographic");
	ComboClearFlag.AddString(L"Skybox");
	ComboClearFlag.AddString(L"Solid Color");
	ComboClearFlag.AddString(L"None");
}


void CameraView::OnEnChangeEditFov()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditFOV.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditFOV.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetFOV(_tstof(_tempString));
		}

	}
}


void CameraView::OnEnChangeEditNear()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditNear.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditNear.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetNear(_tstof(_tempString));
		}

	}
}


void CameraView::OnEnChangeEditFar()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditFar.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditFar.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetFar(_tstof(_tempString));
		}

	}
}


void CameraView::OnEnChangeEditClearColorR()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditClearColorR.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditClearColorR.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetClearColor(_temp);
		}

	}
}


void CameraView::OnEnChangeEditClearColorG()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditClearColorG.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditClearColorG.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetClearColor(_temp);
		}

	}
}


void CameraView::OnEnChangeEditClearColorB()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditClearColorB.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditClearColorB.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->GetClearColor().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetClearColor(_temp);
		}

	}
}


void CameraView::OnCbnCloseupComboMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboMode.GetCurSel() && ComboMode.GetCurSel() < 2)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->ChangeMode((hos::com::Camera::eMode)ComboMode.GetCurSel());
	}
}


void CameraView::OnCbnCloseupComboViewmode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboViewMode.GetCurSel() && ComboViewMode.GetCurSel() <2 )
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->ChangeView((hos::com::Camera::eViewMode)ComboViewMode.GetCurSel());
	}
}


void CameraView::OnCbnCloseupComboClearflag()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboClearFlag.GetCurSel() && ComboClearFlag.GetCurSel() < 3)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->ChangeClearFlag((hos::com::Camera::eClearFlag)ComboClearFlag.GetCurSel());
	}
}


void CameraView::OnBnClickedButtonCameraReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetCameraView();
}


void CameraView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetActive(CheckComponentActive.GetCheck());
}


void CameraView::OnEnChangeEditViewPortLeft()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditViewPortLeft.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditViewPortLeft.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetViewPortLeft(_tstof(_tempString));
		}

	}

}


void CameraView::OnEnChangeEditViewPortTop()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditViewPortTop.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditViewPortTop.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetViewPortTop(_tstof(_tempString));
		}

	}
}


void CameraView::OnEnChangeEditViewPortRight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditViewPortRight.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditViewPortRight.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetViewPortRight(_tstof(_tempString));
		}

	}
}


void CameraView::OnEnChangeEditViewPortBottom()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditViewPortBottom.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditViewPortBottom.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Camera>()->SetViewPortBottom(_tstof(_tempString));
		}

	}
}


void CameraView::OnBnClickedButtonLoadSkyBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListSkyBoxList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _SkyBoxName;
	ListSkyBoxList.GetText(ListSkyBoxList.GetCurSel(), _SkyBoxName);

	_NowGameObject->GetComponent<hos::com::Camera>()->SetSkyBox(T2W(_SkyBoxName.GetBuffer()));

	SetCameraView();

}


void CameraView::OnBnClickedButtonRemoveSkyBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	_NowGameObject->GetComponent<hos::com::Camera>()->SetSkyBox(L"");

	SetCameraView();

}
