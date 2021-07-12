// CapsuleCollisionView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "CapsuleCollisionView.h"


// CapsuleCollisionView

IMPLEMENT_DYNCREATE(CapsuleCollisionView, CFormView)

CapsuleCollisionView::CapsuleCollisionView()
	: CFormView(IDD_CapsuleCollisionView)
{

}

CapsuleCollisionView::~CapsuleCollisionView()
{
}

void CapsuleCollisionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CENTER_X, EditCenterX);
	DDX_Control(pDX, IDC_EDIT_CENTER_Y, EditCenterY);
	DDX_Control(pDX, IDC_EDIT_CENTER_Z, EditCenterZ);
	DDX_Control(pDX, IDC_EDIT_RADIUS, EditRadius);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, EditHeight);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, ComboDirection);
	DDX_Control(pDX, IDC_CHECK_IS_TRIGGER, CheckIsTrigger);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_EDIT_DYNAMIC_FRICTION, EditDynamicFriction);
	DDX_Control(pDX, IDC_EDIT_STATIC_FRICTION, EditStaticFriction);
	DDX_Control(pDX, IDC_EDIT_BOUNCINESS, EditBounciness);
	DDX_Control(pDX, IDC_COMBO_FRICTION_COMBINE, ComboFrictionCombine);
	DDX_Control(pDX, IDC_COMBO_BOUNCE_COMBINE, ComboBounceCombine);
	DDX_Control(pDX, IDC_CHECK_MATERIAL_ACTIVE, CheckMaterialActive);
	DDX_Control(pDX, IDC_COMBO_LAYER, ComboLayer);
}

BEGIN_MESSAGE_MAP(CapsuleCollisionView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_CENTER_X, &CapsuleCollisionView::OnEnChangeEditCenterX)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Y, &CapsuleCollisionView::OnEnChangeEditCenterY)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Z, &CapsuleCollisionView::OnEnChangeEditCenterZ)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &CapsuleCollisionView::OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &CapsuleCollisionView::OnEnChangeEditHeight)
	ON_CBN_CLOSEUP(IDC_COMBO_DIRECTION, &CapsuleCollisionView::OnCbnCloseupComboDirection)
	ON_BN_CLICKED(IDC_CHECK_IS_TRIGGER, &CapsuleCollisionView::OnBnClickedCheckIsTrigger)
	ON_BN_CLICKED(IDC_BUTTON_CAPSULE_COLLISION_RESET, &CapsuleCollisionView::OnBnClickedButtonCapsuleCollisionReset)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &CapsuleCollisionView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_DYNAMIC_FRICTION, &CapsuleCollisionView::OnEnChangeEditDynamicFriction)
	ON_EN_CHANGE(IDC_EDIT_STATIC_FRICTION, &CapsuleCollisionView::OnEnChangeEditStaticFriction)
	ON_EN_CHANGE(IDC_EDIT_BOUNCINESS, &CapsuleCollisionView::OnEnChangeEditBounciness)
	ON_CBN_CLOSEUP(IDC_COMBO_FRICTION_COMBINE, &CapsuleCollisionView::OnCbnCloseupComboFrictionCombine)
	ON_CBN_CLOSEUP(IDC_COMBO_BOUNCE_COMBINE, &CapsuleCollisionView::OnCbnCloseupComboBounceCombine)
	ON_BN_CLICKED(IDC_CHECK_MATERIAL_ACTIVE, &CapsuleCollisionView::OnBnClickedCheckMaterialActive)
	ON_CBN_CLOSEUP(IDC_COMBO_LAYER, &CapsuleCollisionView::OnCbnCloseupComboLayer)
END_MESSAGE_MAP()


// CapsuleCollisionView 진단

#ifdef _DEBUG
void CapsuleCollisionView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CapsuleCollisionView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CapsuleCollisionView::ResetCapsuleCollisionView()
{
	EditCenterX.SetWindowTextW(L"0");
	EditCenterY.SetWindowTextW(L"0");
	EditCenterZ.SetWindowTextW(L"0");
	EditRadius.SetWindowTextW(L"0.5");
	EditHeight.SetWindowTextW(L"1");
	ComboDirection.SetCurSel(1);
	CheckIsTrigger.SetCheck(FALSE);

	EditDynamicFriction.SetWindowTextW(L"0.6");
	EditStaticFriction.SetWindowTextW(L"0.6");
	EditBounciness.SetWindowTextW(L"0");
	ComboFrictionCombine.SetCurSel(0);
	ComboBounceCombine.SetCurSel(0);
}

void CapsuleCollisionView::SetCapsuleCollisionView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetCenter().x);
	EditCenterX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetCenter().y);
	EditCenterY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetCenter().z);
	EditCenterZ.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetRadius());
	EditRadius.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetHeight());
	EditHeight.SetWindowTextW(_tempString);

	ComboDirection.SetCurSel((int)_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetDirection());

	CheckIsTrigger.SetCheck(_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetIsTrigger());

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetActive());

	if (_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial())
	{
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->GetDynamicFriction());
		EditDynamicFriction.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->GetStaticFriction());
		EditStaticFriction.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->GetBounciness());
		EditBounciness.SetWindowTextW(_tempString);

		ComboFrictionCombine.SetCurSel((int)_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->GetFrictionCombine());
		ComboBounceCombine.SetCurSel((int)_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->GetBounceCombine());
		CheckMaterialActive.SetCheck(true);

	}
	else
	{
		// 없으면 비활성화
		CheckMaterialActive.SetCheck(false);

		EditDynamicFriction.EnableWindow(FALSE);
		EditStaticFriction.EnableWindow(FALSE);
		EditBounciness.EnableWindow(FALSE);
		ComboFrictionCombine.EnableWindow(FALSE);
		ComboBounceCombine.EnableWindow(FALSE);

	}

	ComboLayer.ResetContent();

	for (int i = 0; i < 32; i++)
	{
		ComboLayer.AddString(EditorManager::GetIns()->GetNowScene()->GetLayerName(i).c_str());
	}

	ComboLayer.SetCurSel(_NowGameObject->GetComponent<hos::com::CapsuleCollision>()->GetLayer());

}

// CapsuleCollisionView 메시지 처리기


BOOL CapsuleCollisionView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CapsuleCollisionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ComboDirection.AddString(L"X-Axis");
	ComboDirection.AddString(L"Y-Axis");
	ComboDirection.AddString(L"Z-Axis");

	ComboFrictionCombine.AddString(L"Average");
	ComboFrictionCombine.AddString(L"Min");
	ComboFrictionCombine.AddString(L"Multiply");
	ComboFrictionCombine.AddString(L"Max");

	ComboBounceCombine.AddString(L"Average");
	ComboBounceCombine.AddString(L"Min");
	ComboBounceCombine.AddString(L"Multiply");
	ComboBounceCombine.AddString(L"Max");

	ResetCapsuleCollisionView();

}


void CapsuleCollisionView::OnEnChangeEditCenterX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCenterX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditCenterX.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetCenter(_temp);
		}

	}

}


void CapsuleCollisionView::OnEnChangeEditCenterY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCenterY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditCenterY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetCenter(_temp);
		}

	}

}


void CapsuleCollisionView::OnEnChangeEditCenterZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCenterZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditCenterZ.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetCenter().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetCenter(_temp);
		}

	}

}


void CapsuleCollisionView::OnEnChangeEditRadius()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRadius.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditRadius.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetRadius(_tstof(_tempString));
		}

	}

}


void CapsuleCollisionView::OnEnChangeEditHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	float _Length = EditHeight.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditHeight.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetHeight(_tstof(_tempString));
		}

	}

}


void CapsuleCollisionView::OnCbnCloseupComboDirection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (0 <= ComboDirection.GetCurSel() && ComboDirection.GetCurSel() < hos::com::CapsuleCollision::eDirection::NUM_DIRECTION)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetDirection((hos::com::CapsuleCollision::eDirection)ComboDirection.GetCurSel());
	}

}


void CapsuleCollisionView::OnBnClickedCheckIsTrigger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetIsTrigger(CheckIsTrigger.GetCheck());

}


void CapsuleCollisionView::OnBnClickedButtonCapsuleCollisionReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetCapsuleCollisionView();

}


void CapsuleCollisionView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetActive(CheckComponentActive.GetCheck());
}


void CapsuleCollisionView::OnEnChangeEditDynamicFriction()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditDynamicFriction.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditDynamicFriction.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->SetDynamicFriction(_tstof(_tempString));
		}

	}
}


void CapsuleCollisionView::OnEnChangeEditStaticFriction()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditStaticFriction.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditStaticFriction.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->SetStaticFriction(_tstof(_tempString));
		}

	}
}


void CapsuleCollisionView::OnEnChangeEditBounciness()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditBounciness.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditBounciness.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->SetBounciness(_tstof(_tempString));
		}

	}
}


void CapsuleCollisionView::OnCbnCloseupComboFrictionCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboFrictionCombine.GetCurSel() && ComboFrictionCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboFrictionCombine.GetCurSel());
	}
}


void CapsuleCollisionView::OnCbnCloseupComboBounceCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboBounceCombine.GetCurSel() && ComboBounceCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboBounceCombine.GetCurSel());
	}
}


void CapsuleCollisionView::OnBnClickedCheckMaterialActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->GetMaterial())
	{
		CheckMaterialActive.SetCheck(false);

		// 비활성화
		EditDynamicFriction.EnableWindow(FALSE);
		EditStaticFriction.EnableWindow(FALSE);
		EditBounciness.EnableWindow(FALSE);
		ComboFrictionCombine.EnableWindow(FALSE);
		ComboBounceCombine.EnableWindow(FALSE);

		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetMaterial(nullptr);

	}
	else
	{
		CheckMaterialActive.SetCheck(true);

		EditDynamicFriction.EnableWindow(TRUE);
		EditStaticFriction.EnableWindow(TRUE);
		EditBounciness.EnableWindow(TRUE);
		ComboFrictionCombine.EnableWindow(TRUE);
		ComboBounceCombine.EnableWindow(TRUE);

		hos::PhysicMaterial* _DefaultMaterial = new hos::PhysicMaterial();
		std::pair< std::map<hos::string, hos::PhysicMaterial*>::iterator, bool> b;
		b = EditorManager::GetIns()->GetDataManager()->PhysicMaterials.insert({ L"Default" + std::to_wstring(__COUNTER__),_DefaultMaterial });
		if (b.second)
		{
			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetMaterial(_DefaultMaterial);
		}
		else
		{
			hos::SafeDelete(_DefaultMaterial);
		}
	}
}


void CapsuleCollisionView::OnCbnCloseupComboLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (0 <= ComboLayer.GetCurSel() && ComboLayer.GetCurSel() < 32)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::CapsuleCollision>()->SetLayer(ComboLayer.GetCurSel());
	}
}
