// RigidbodyView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "RigidbodyView.h"


// RigidbodyView

IMPLEMENT_DYNCREATE(RigidbodyView, CFormView)

RigidbodyView::RigidbodyView()
	: CFormView(IDD_RigidbodyView)
{

}

RigidbodyView::~RigidbodyView()
{
}

void RigidbodyView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MASS, EditMass);
	DDX_Control(pDX, IDC_EDIT_DRAG, EditDrag);
	//  DDX_Control(pDX, IDC_EDIT_ANGULAR_DRAG, EditUseGravity);
	DDX_Control(pDX, IDC_EDIT_ANGULAR_DRAG, EditAngularDrag);
	DDX_Control(pDX, IDC_CHECK_USE_GRAVITY, CheckUseGravity);
	DDX_Control(pDX, IDC_CHECK_FREEZE_POSITION_X, CheckFreezePositionX);
	DDX_Control(pDX, IDC_CHECK_IDC_CHECK_FREEZE_POSITION_y, CheckFreezePositionY);
	DDX_Control(pDX, IDC_CHECKIDC_CHECK_FREEZE_POSITION_Z, CheckFreezePositionZ);
	DDX_Control(pDX, IDC_CHECK_FREEZE_ROTATION_X, CheckFreezeRotationX);
	DDX_Control(pDX, IDC_CHECK_FREEZE_ROTATION_Y, CheckFreezeRotationY);
	DDX_Control(pDX, IDC_CHECK_FREEZE_ROTATION_Z, CheckFreezeRotationZ);
	DDX_Control(pDX, IDC_CHECK_IS_KINEMATIC, CheckIsKinematic);
}

BEGIN_MESSAGE_MAP(RigidbodyView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_MASS, &RigidbodyView::OnEnChangeEditMass)
	ON_EN_CHANGE(IDC_EDIT_DRAG, &RigidbodyView::OnEnChangeEditDrag)
	ON_EN_CHANGE(IDC_EDIT_ANGULAR_DRAG, &RigidbodyView::OnEnChangeEditAngularDrag)
	ON_BN_CLICKED(IDC_CHECK_USE_GRAVITY, &RigidbodyView::OnBnClickedCheckUseGravity)
	ON_BN_CLICKED(IDC_CHECK_FREEZE_POSITION_X, &RigidbodyView::OnBnClickedCheckFreezePositionX)
//	ON_BN_CLICKED(IDC_CHECK_IDC_CHECK_FREEZE_POSITION_y, &RigidbodyView::OnBnClickedCheckIdcCheckFreezePositiony)
	ON_BN_CLICKED(IDC_CHECKIDC_CHECK_FREEZE_POSITION_Z, &RigidbodyView::OnBnClickedCheckidcCheckFreezePositionZ)
	ON_BN_CLICKED(IDC_CHECK_FREEZE_ROTATION_X, &RigidbodyView::OnBnClickedCheckFreezeRotationX)
	ON_BN_CLICKED(IDC_CHECK_FREEZE_ROTATION_Y, &RigidbodyView::OnBnClickedCheckFreezeRotationY)
	ON_BN_CLICKED(IDC_CHECK_FREEZE_ROTATION_Z, &RigidbodyView::OnBnClickedCheckFreezeRotationZ)
	ON_BN_CLICKED(IDC_BUTTON_RIGIDBODY_RESET, &RigidbodyView::OnBnClickedButtonRigidbodyReset)
	ON_BN_CLICKED(IDC_CHECK_IS_KINEMATIC, &RigidbodyView::OnBnClickedCheckIsKinematic)
	ON_BN_CLICKED(IDC_CHECK_IDC_CHECK_FREEZE_POSITION_y, &RigidbodyView::OnBnClickedCheckIdcCheckFreezePositiony)
END_MESSAGE_MAP()


// RigidbodyView 진단

#ifdef _DEBUG
void RigidbodyView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void RigidbodyView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void RigidbodyView::ResetRigidbodyView()
{
	EditMass.SetWindowTextW(L"1");
	EditDrag.SetWindowTextW(L"0");
	EditAngularDrag.SetWindowTextW(L"0.05");
	CheckUseGravity.SetCheck(TRUE);
	CheckIsKinematic.SetCheck(FALSE);
	CheckFreezePositionX.SetCheck(FALSE);
	CheckFreezePositionY.SetCheck(FALSE);
	CheckFreezePositionZ.SetCheck(FALSE);
	CheckFreezeRotationX.SetCheck(FALSE);
	CheckFreezeRotationY.SetCheck(FALSE);
	CheckFreezeRotationZ.SetCheck(FALSE);
	
}

void RigidbodyView::SetRigidbodyView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Rigidbody>()->GetMass());
	EditMass.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Rigidbody>()->GetDrag());
	EditDrag.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Rigidbody>()->GetAngularDrag());
	EditAngularDrag.SetWindowTextW(_tempString);

	CheckUseGravity.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetUseGravity());
	CheckIsKinematic.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetIsKinematic());

	CheckFreezePositionX.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezePositionX());
	CheckFreezePositionY.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezePositionY());
	CheckFreezePositionZ.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezePositionZ());

	CheckFreezeRotationX.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezeRotationX());
	CheckFreezeRotationY.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezeRotationY());
	CheckFreezeRotationZ.SetCheck(_NowGameObject->GetComponent<hos::com::Rigidbody>()->GetFreezeRotationZ());

}

// RigidbodyView 메시지 처리기


BOOL RigidbodyView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void RigidbodyView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void RigidbodyView::OnEnChangeEditMass()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditMass.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditMass.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetMass(_tstof(_tempString));
		}

	}
}


void RigidbodyView::OnEnChangeEditDrag()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditDrag.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditDrag.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetDrag(_tstof(_tempString));
		}

	}
}


void RigidbodyView::OnEnChangeEditAngularDrag()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAngularDrag.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditAngularDrag.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetAngularDrag(_tstof(_tempString));
		}

	}
}


void RigidbodyView::OnBnClickedCheckUseGravity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetUseGravity(CheckUseGravity.GetCheck());
}


void RigidbodyView::OnBnClickedCheckIsKinematic()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetIsKinematic(CheckIsKinematic.GetCheck());
}

void RigidbodyView::OnBnClickedCheckFreezePositionX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezePositionX(CheckFreezePositionX.GetCheck());
}


void RigidbodyView::OnBnClickedCheckIdcCheckFreezePositiony()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezePositionY(CheckFreezePositionY.GetCheck());
}


void RigidbodyView::OnBnClickedCheckidcCheckFreezePositionZ()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezePositionZ(CheckFreezePositionZ.GetCheck());
}


void RigidbodyView::OnBnClickedCheckFreezeRotationX()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezeRotationX(CheckFreezeRotationX.GetCheck());
}


void RigidbodyView::OnBnClickedCheckFreezeRotationY()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezeRotationY(CheckFreezeRotationY.GetCheck());
}


void RigidbodyView::OnBnClickedCheckFreezeRotationZ()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Rigidbody>()->SetFreezeRotationZ(CheckFreezeRotationZ.GetCheck());
}


void RigidbodyView::OnBnClickedButtonRigidbodyReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetRigidbodyView();
}

