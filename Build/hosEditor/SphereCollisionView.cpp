// SphereCollisionView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "SphereCollisionView.h"


// SphereCollisionView

IMPLEMENT_DYNCREATE(SphereCollisionView, CFormView)

SphereCollisionView::SphereCollisionView()
	: CFormView(IDD_SphereCollisionView)
{

}

SphereCollisionView::~SphereCollisionView()
{
}

void SphereCollisionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CENTER_X, EditCenterX);
	DDX_Control(pDX, IDC_EDIT_CENTER_Y, EditCenterY);
	DDX_Control(pDX, IDC_EDIT_CENTER_Z, EditCenterZ);
	DDX_Control(pDX, IDC_EDIT_RADIUS, EditRadius);
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

BEGIN_MESSAGE_MAP(SphereCollisionView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_CENTER_X, &SphereCollisionView::OnEnChangeEditCenterX)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Y, &SphereCollisionView::OnEnChangeEditCenterY)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Z, &SphereCollisionView::OnEnChangeEditCenterZ)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &SphereCollisionView::OnEnChangeEditRadius)
	ON_BN_CLICKED(IDC_CHECK_IS_TRIGGER, &SphereCollisionView::OnBnClickedCheckIsTrigger)
	ON_BN_CLICKED(IDC_BUTTON_SPHERE_COLLISION_RESET, &SphereCollisionView::OnBnClickedButtonSphereCollisionReset)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &SphereCollisionView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_DYNAMIC_FRICTION, &SphereCollisionView::OnEnChangeEditDynamicFriction)
	ON_EN_CHANGE(IDC_EDIT_STATIC_FRICTION, &SphereCollisionView::OnEnChangeEditStaticFriction)
	ON_EN_CHANGE(IDC_EDIT_BOUNCINESS, &SphereCollisionView::OnEnChangeEditBounciness)
	ON_CBN_CLOSEUP(IDC_COMBO_FRICTION_COMBINE, &SphereCollisionView::OnCbnCloseupComboFrictionCombine)
	ON_CBN_CLOSEUP(IDC_COMBO_BOUNCE_COMBINE, &SphereCollisionView::OnCbnCloseupComboBounceCombine)
	ON_BN_CLICKED(IDC_CHECK_MATERIAL_ACTIVE, &SphereCollisionView::OnBnClickedCheckMaterialActive)
	ON_CBN_CLOSEUP(IDC_COMBO_LAYER, &SphereCollisionView::OnCbnCloseupComboLayer)
END_MESSAGE_MAP()


// SphereCollisionView 진단

#ifdef _DEBUG
void SphereCollisionView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SphereCollisionView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void SphereCollisionView::ResetSphereCollisionView()
{
	EditCenterX.SetWindowTextW(L"0");
	EditCenterY.SetWindowTextW(L"0");
	EditCenterZ.SetWindowTextW(L"0");
	EditRadius.SetWindowTextW(L"0.5");
	CheckIsTrigger.SetCheck(FALSE);

	EditDynamicFriction.SetWindowTextW(L"0.6");
	EditStaticFriction.SetWindowTextW(L"0.6");
	EditBounciness.SetWindowTextW(L"0");
	ComboFrictionCombine.SetCurSel(0);
	ComboBounceCombine.SetCurSel(0);

}

void SphereCollisionView::SetSphereCollisionView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetCenter().x);
	EditCenterX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetCenter().y);
	EditCenterY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetCenter().z);
	EditCenterZ.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetRadius());
	EditRadius.SetWindowTextW(_tempString);

	CheckIsTrigger.SetCheck(_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetIsTrigger());

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetActive());

	if (_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial())
	{
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial()->GetDynamicFriction());
		EditDynamicFriction.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial()->GetStaticFriction());
		EditStaticFriction.SetWindowTextW(_tempString);
		_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial()->GetBounciness());
		EditBounciness.SetWindowTextW(_tempString);

		ComboFrictionCombine.SetCurSel((int)_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial()->GetFrictionCombine());
		ComboBounceCombine.SetCurSel((int)_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetMaterial()->GetBounceCombine());
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

	ComboLayer.SetCurSel(_NowGameObject->GetComponent<hos::com::SphereCollision>()->GetLayer());

}

// SphereCollisionView 메시지 처리기


BOOL SphereCollisionView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void SphereCollisionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	ComboFrictionCombine.AddString(L"Average");
	ComboFrictionCombine.AddString(L"Min");
	ComboFrictionCombine.AddString(L"Multiply");
	ComboFrictionCombine.AddString(L"Max");

	ComboBounceCombine.AddString(L"Average");
	ComboBounceCombine.AddString(L"Min");
	ComboBounceCombine.AddString(L"Multiply");
	ComboBounceCombine.AddString(L"Max");

	ResetSphereCollisionView();
}


void SphereCollisionView::OnEnChangeEditCenterX()
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
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetCenter(_temp);
		}

	}
}


void SphereCollisionView::OnEnChangeEditCenterY()
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

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetCenter(_temp);
		}

	}
}


void SphereCollisionView::OnEnChangeEditCenterZ()
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

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetCenter().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetCenter(_temp);
		}

	}
}


void SphereCollisionView::OnEnChangeEditRadius()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetRadius(_tstof(_tempString));
		}

	}
}


void SphereCollisionView::OnBnClickedCheckIsTrigger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetIsTrigger(CheckIsTrigger.GetCheck());

}


void SphereCollisionView::OnBnClickedButtonSphereCollisionReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetSphereCollisionView();
}


void SphereCollisionView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetActive(CheckComponentActive.GetCheck());
}


void SphereCollisionView::OnEnChangeEditDynamicFriction()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial()->SetDynamicFriction(_tstof(_tempString));
		}

	}
}


void SphereCollisionView::OnEnChangeEditStaticFriction()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial()->SetStaticFriction(_tstof(_tempString));
		}

	}
}


void SphereCollisionView::OnEnChangeEditBounciness()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial()->SetBounciness(_tstof(_tempString));
		}

	}
}


void SphereCollisionView::OnCbnCloseupComboFrictionCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboFrictionCombine.GetCurSel() && ComboFrictionCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboFrictionCombine.GetCurSel());
	}
}


void SphereCollisionView::OnCbnCloseupComboBounceCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboBounceCombine.GetCurSel() && ComboBounceCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboBounceCombine.GetCurSel());
	}
}


void SphereCollisionView::OnBnClickedCheckMaterialActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->GetMaterial())
	{
		CheckMaterialActive.SetCheck(false);

		// 비활성화
		EditDynamicFriction.EnableWindow(FALSE);
		EditStaticFriction.EnableWindow(FALSE);
		EditBounciness.EnableWindow(FALSE);
		ComboFrictionCombine.EnableWindow(FALSE);
		ComboBounceCombine.EnableWindow(FALSE);

		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetMaterial(nullptr);

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
			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetMaterial(_DefaultMaterial);
		}
		else
		{
			hos::SafeDelete(_DefaultMaterial);
		}
	}

}


void SphereCollisionView::OnCbnCloseupComboLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	if (0 <= ComboLayer.GetCurSel() && ComboLayer.GetCurSel() < 32)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SphereCollision>()->SetLayer(ComboLayer.GetCurSel());
	}

}
