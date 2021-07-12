// LIghtView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "LIghtView.h"


// LIghtView

IMPLEMENT_DYNCREATE(LIghtView, CFormView)

LIghtView::LIghtView()
	: CFormView(IDD_LIghtView)
{

}

LIghtView::~LIghtView()
{
}

void LIghtView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RANGE, EditRange);
	DDX_Control(pDX, IDC_EDIT_ANGLE, EditAngle);
	DDX_Control(pDX, IDC_EDIT_INTENSITY, EditIntensity);
	DDX_Control(pDX, IDC_EDIT_COLOR_X, EditColorX);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, EditColorY);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, EditColorZ);
	DDX_Control(pDX, IDC_COMBO_TYPE, ComboType);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_EDIT_CONST, EditConst);
	DDX_Control(pDX, IDC_EDIT_LINEAR, EditLinear);
	DDX_Control(pDX, IDC_EDIT_QUAD, EditQuad);
}

BEGIN_MESSAGE_MAP(LIghtView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_RANGE, &LIghtView::OnEnChangeEditRange)
	ON_EN_CHANGE(IDC_EDIT_ANGLE, &LIghtView::OnEnChangeEditAngle)
	ON_EN_CHANGE(IDC_EDIT_INTENSITY, &LIghtView::OnEnChangeEditIntensity)
	ON_EN_CHANGE(IDC_EDIT_COLOR_X, &LIghtView::OnEnChangeEditColorX)
	ON_EN_CHANGE(IDC_EDIT_COLOR_G, &LIghtView::OnEnChangeEditColorG)
	ON_EN_CHANGE(IDC_EDIT_COLOR_B, &LIghtView::OnEnChangeEditColorB)
	ON_CBN_CLOSEUP(IDC_COMBO_TYPE, &LIghtView::OnCbnCloseupComboType)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &LIghtView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_CONST, &LIghtView::OnEnChangeEditConst)
	ON_EN_CHANGE(IDC_EDIT_LINEAR, &LIghtView::OnEnChangeEditLinear)
	ON_EN_CHANGE(IDC_EDIT_QUAD, &LIghtView::OnEnChangeEditQuad)
END_MESSAGE_MAP()


// LIghtView 진단

#ifdef _DEBUG
void LIghtView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LIghtView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void LIghtView::ResetLightView()
{
	EditRange.SetWindowTextW(L"10");
	EditAngle.SetWindowTextW(L"30");
	EditIntensity.SetWindowTextW(L"1");
	EditColorX.SetWindowTextW(L"1");
	EditColorY.SetWindowTextW(L"1");
	EditColorZ.SetWindowTextW(L"1");
	ComboType.SetCurSel(0);

}

void LIghtView::SetLightView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetRange());
	EditRange.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetAngle());
	EditAngle.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetIntensity());
	EditIntensity.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetColor().R());
	EditColorX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetColor().G());
	EditColorY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetColor().B());
	EditColorZ.SetWindowTextW(_tempString);

	ComboType.SetCurSel((int)_NowGameObject->GetComponent<hos::com::Light>()->GetType());

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::Light>()->GetActive());

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetAttenuation_Const());
	EditConst.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetAttenuation_Linear());
	EditLinear.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Light>()->GetAttenuation_Quad());
	EditQuad.SetWindowTextW(_tempString);
}

// LIghtView 메시지 처리기


BOOL LIghtView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void LIghtView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ComboType.AddString(L"Directional");
	ComboType.AddString(L"Point");
	ComboType.AddString(L"Spot");
	ComboType.AddString(L"Ambient");
}


void LIghtView::OnEnChangeEditRange()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRange.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditRange.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetRange(_tstof(_tempString));
		}

	}
}


void LIghtView::OnEnChangeEditAngle()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAngle.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditAngle.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetAngle(_tstof(_tempString));
		}

	}
}


void LIghtView::OnEnChangeEditIntensity()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditIntensity.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditIntensity.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetIntensity(_tstof(_tempString));
		}

	}
}


void LIghtView::OnEnChangeEditColorX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditColorX.GetWindowTextW(_tempString);

			_temp.R(_tstof(_tempString));
			_temp.G(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().G());
			_temp.B(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().B());

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetColor(_temp);
		}

	}
}


void LIghtView::OnEnChangeEditColorG()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditColorY.GetWindowTextW(_tempString);

			_temp.R(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().R());
			_temp.G(_tstof(_tempString));
			_temp.B(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().B());

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetColor(_temp);
		}

	}
}


void LIghtView::OnEnChangeEditColorB()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditColorZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Color _temp;

			CString _tempString;
			EditColorZ.GetWindowTextW(_tempString);

			_temp.R(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().R());
			_temp.G(EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->GetColor().G());
			_temp.B(_tstof(_tempString));

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetColor(_temp);
		}

	}
}


void LIghtView::OnCbnCloseupComboType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboType.GetCurSel() && ComboType.GetCurSel() < (int)hos::com::Light::eType::MAX)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->ChangeType((hos::com::Light::eType)ComboType.GetCurSel());
	}
}


void LIghtView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetActive(CheckComponentActive.GetCheck());
}


void LIghtView::OnEnChangeEditConst()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int _Length = EditConst.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditConst.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetAttenuation_Const(_tstof(_tempString));
		}

	}
}


void LIghtView::OnEnChangeEditLinear()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int _Length = EditLinear.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditLinear.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetAttenuation_Linear(_tstof(_tempString));
		}

	}
}


void LIghtView::OnEnChangeEditQuad()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int _Length = EditQuad.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditQuad.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Light>()->SetAttenuation_Quad(_tstof(_tempString));
		}

	}
}
