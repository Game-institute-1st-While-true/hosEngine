// BoxCollisionView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "BoxCollisionView.h"


// BoxCollisionView

IMPLEMENT_DYNCREATE(BoxCollisionView, CFormView)

BoxCollisionView::BoxCollisionView()
	: CFormView(IDD_BoxCollisionView)
{

}

BoxCollisionView::~BoxCollisionView()
{
}

void BoxCollisionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CENTER_X, EditCenterX);
	DDX_Control(pDX, IDC_EDIT_CENTER_Y, EditCenterY);
	DDX_Control(pDX, IDC_EDIT_CENTER_Z, EditCenterZ);
	DDX_Control(pDX, IDC_EDIT_SIZE_X, EditSizeX);
	DDX_Control(pDX, IDC_EDIT_SIZE_Y, EditSizeY);
	DDX_Control(pDX, IDC_EDIT_SIZE_Z, EditSizeZ);
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

BEGIN_MESSAGE_MAP(BoxCollisionView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_CENTER_X, &BoxCollisionView::OnEnChangeEditCenterX)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Y, &BoxCollisionView::OnEnChangeEditCenterY)
	ON_EN_CHANGE(IDC_EDIT_CENTER_Z, &BoxCollisionView::OnEnChangeEditCenterZ)
	ON_EN_CHANGE(IDC_EDIT_SIZE_X, &BoxCollisionView::OnEnChangeEditSizeX)
	ON_EN_CHANGE(IDC_EDIT_SIZE_Y, &BoxCollisionView::OnEnChangeEditSizeY)
	ON_EN_CHANGE(IDC_EDIT_SIZE_Z, &BoxCollisionView::OnEnChangeEditSizeZ)
	ON_BN_CLICKED(IDC_BUTTON_BOX_COLLISION_RESET, &BoxCollisionView::OnBnClickedButtonBoxCollisionReset)
	ON_BN_CLICKED(IDC_CHECK_IS_TRIGGER, &BoxCollisionView::OnBnClickedCheckIsTrigger)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &BoxCollisionView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_DYNAMIC_FRICTION, &BoxCollisionView::OnEnChangeEditDynamicFriction)
	ON_EN_CHANGE(IDC_EDIT_STATIC_FRICTION, &BoxCollisionView::OnEnChangeEditStaticFriction)
	ON_EN_CHANGE(IDC_EDIT_BOUNCINESS, &BoxCollisionView::OnEnChangeEditBounciness)
	ON_CBN_CLOSEUP(IDC_COMBO_FRICTION_COMBINE, &BoxCollisionView::OnCbnCloseupComboFrictionCombine)
	ON_CBN_CLOSEUP(IDC_COMBO_BOUNCE_COMBINE, &BoxCollisionView::OnCbnCloseupComboBounceCombine)
	ON_BN_CLICKED(IDC_CHECK_MATERIAL_ACTIVE, &BoxCollisionView::OnBnClickedCheckMaterialActive)
	ON_CBN_CLOSEUP(IDC_COMBO_LAYER, &BoxCollisionView::OnCbnCloseupComboLayer)
END_MESSAGE_MAP()


// BoxCollisionView 진단

#ifdef _DEBUG
void BoxCollisionView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void BoxCollisionView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void BoxCollisionView::ResetBoxCollisionView()
{
	EditCenterX.SetWindowTextW(L"0");
	EditCenterY.SetWindowTextW(L"0");
	EditCenterZ.SetWindowTextW(L"0");
	EditSizeX.SetWindowTextW(L"1");
	EditSizeY.SetWindowTextW(L"1");
	EditSizeZ.SetWindowTextW(L"1");
	CheckIsTrigger.SetCheck(FALSE);

	EditDynamicFriction.SetWindowTextW(L"0.6");
	EditStaticFriction.SetWindowTextW(L"0.6");
	EditBounciness.SetWindowTextW(L"0");
	ComboFrictionCombine.SetCurSel(0);
	ComboBounceCombine.SetCurSel(0);

}

void BoxCollisionView::SetBoxCollisionView()
{
	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();
	if (_NowGameObject)
	{
		CString _tempString;
		hos::com::BoxCollision* box = _NowGameObject->GetComponent<hos::com::BoxCollision>();

		if (box)
		{
			_tempString.Format(L"%f", box->GetCenter().x);
			EditCenterX.SetWindowTextW(_tempString);
			_tempString.Format(L"%f", box->GetCenter().y);
			EditCenterY.SetWindowTextW(_tempString);
			_tempString.Format(L"%f", box->GetCenter().z);
			EditCenterZ.SetWindowTextW(_tempString);

			_tempString.Format(L"%f", box->GetSizeX());
			EditSizeX.SetWindowTextW(_tempString);
			_tempString.Format(L"%f", box->GetSizeY());
			EditSizeY.SetWindowTextW(_tempString);
			_tempString.Format(L"%f", box->GetSizeZ());
			EditSizeZ.SetWindowTextW(_tempString);

			CheckIsTrigger.SetCheck(box->GetIsTrigger());

			CheckComponentActive.SetCheck(box->GetActive());

			hos::PhysicMaterial* material = box->GetMaterial();
			if (material)
			{
				_tempString.Format(L"%f", material->GetDynamicFriction());
				EditDynamicFriction.SetWindowTextW(_tempString);
				_tempString.Format(L"%f", material->GetStaticFriction());
				EditStaticFriction.SetWindowTextW(_tempString);
				_tempString.Format(L"%f", material->GetBounciness());
				EditBounciness.SetWindowTextW(_tempString);

				ComboFrictionCombine.SetCurSel((int)material->GetFrictionCombine());
				ComboBounceCombine.SetCurSel((int)material->GetBounceCombine());

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

			ComboLayer.SetCurSel(box->GetLayer());

		}
	}
}

// BoxCollisionView 메시지 처리기


BOOL BoxCollisionView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void BoxCollisionView::OnInitialUpdate()
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

	ResetBoxCollisionView();

}




void BoxCollisionView::OnEnChangeEditCenterX()
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
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetCenter(_temp);
		}

	}
}


void BoxCollisionView::OnEnChangeEditCenterY()
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

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetCenter(_temp);
		}

	}
}


void BoxCollisionView::OnEnChangeEditCenterZ()
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

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetCenter().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetCenter(_temp);
		}

	}
}


void BoxCollisionView::OnEnChangeEditSizeX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditSizeX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditSizeX.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetSizeX(_tstof(_tempString));
		}

	}
}

void BoxCollisionView::OnEnChangeEditSizeY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditSizeY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditSizeY.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetSizeY(_tstof(_tempString));
		}

	}
}

void BoxCollisionView::OnEnChangeEditSizeZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditSizeZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditSizeZ.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetSizeZ(_tstof(_tempString));
		}

	}
}


void BoxCollisionView::OnBnClickedButtonBoxCollisionReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetBoxCollisionView();
}


void BoxCollisionView::OnBnClickedCheckIsTrigger()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetIsTrigger(CheckIsTrigger.GetCheck());

}


void BoxCollisionView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetActive(CheckComponentActive.GetCheck());
}


void BoxCollisionView::OnEnChangeEditDynamicFriction()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->SetDynamicFriction(_tstof(_tempString));
		}
	}
}


void BoxCollisionView::OnEnChangeEditStaticFriction()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->SetStaticFriction(_tstof(_tempString));
		}

	}
}


void BoxCollisionView::OnEnChangeEditBounciness()
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

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->SetBounciness(_tstof(_tempString));
		}

	}
}


void BoxCollisionView::OnCbnCloseupComboFrictionCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboFrictionCombine.GetCurSel() && ComboFrictionCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboFrictionCombine.GetCurSel());
	}
}


void BoxCollisionView::OnCbnCloseupComboBounceCombine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboBounceCombine.GetCurSel() && ComboBounceCombine.GetCurSel() < (int)hos::PhysicMaterial::eCombine::NUM_COMBINE)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->SetFrictionCombine((hos::PhysicMaterial::eCombine)ComboBounceCombine.GetCurSel());
	}
}


void BoxCollisionView::OnBnClickedCheckMaterialActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 체크 누르고 상태 봐서 활성화 비활성화
	if (EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->GetMaterial()->Get() != g_Px->DefaultMaterial)
	{
		CheckMaterialActive.SetCheck(false);

		// 비활성화
		EditDynamicFriction.EnableWindow(FALSE);
		EditStaticFriction.EnableWindow(FALSE);
		EditBounciness.EnableWindow(FALSE);
		ComboFrictionCombine.EnableWindow(FALSE);
		ComboBounceCombine.EnableWindow(FALSE);

		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetMaterial(nullptr);

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
			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetMaterial(_DefaultMaterial);
		}
		else
		{
			hos::SafeDelete(_DefaultMaterial);
		}


	}


}


void BoxCollisionView::OnCbnCloseupComboLayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (0 <= ComboLayer.GetCurSel() && ComboLayer.GetCurSel() < 32)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::BoxCollision>()->SetLayer(ComboLayer.GetCurSel());
	}

}
