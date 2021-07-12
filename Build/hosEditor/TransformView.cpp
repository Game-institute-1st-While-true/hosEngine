// TransformView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "TransformView.h"


// TransformView

IMPLEMENT_DYNCREATE(TransformView, CFormView)

TransformView::TransformView()
	: CFormView(IDD_TransformView)
{

}

TransformView::~TransformView()
{
}

void TransformView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_POSITION_X, EditPositionX);
	DDX_Control(pDX, IDC_EDIT_POSITION_Y, EditPositionY);
	DDX_Control(pDX, IDC_EDIT_POSITION_Z, EditPositionZ);
	DDX_Control(pDX, IDC_EDIT_ROTATION_X, EditRotationX);
	DDX_Control(pDX, IDC_EDIT_ROTATION_Y, EditRotationY);
	DDX_Control(pDX, IDC_EDIT_ROTATION_Z, EditRotationZ);
	DDX_Control(pDX, IDC_EDIT_SCALE_X, EditScaleX);
	DDX_Control(pDX, IDC_EDIT_SCALE_Y, EditScaleY);
	DDX_Control(pDX, IDC_EDIT_SCALE_Z, EditScaleZ);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_EDIT_TAG_NAME, EditTagName);
}

BEGIN_MESSAGE_MAP(TransformView, CFormView)
	//	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_TRANSFORM_RESET, &TransformView::OnBnClickedButtonTransformReset)
	ON_EN_CHANGE(IDC_EDIT_POSITION_X, &TransformView::OnEnChangeEditPositionX)
	//ON_EN_UPDATE(IDC_EDIT_POSITION_Y, &TransformView::OnEnUpdateEditPositionY)
	ON_EN_CHANGE(IDC_EDIT_POSITION_Y, &TransformView::OnEnChangeEditPositionY)
	ON_EN_CHANGE(IDC_EDIT_POSITION_Z, &TransformView::OnEnChangeEditPositionZ)
	ON_EN_CHANGE(IDC_EDIT_ROTATION_X, &TransformView::OnEnChangeEditRotationX)
	ON_EN_CHANGE(IDC_EDIT_ROTATION_Y, &TransformView::OnEnChangeEditRotationY)
	ON_EN_CHANGE(IDC_EDIT_ROTATION_Z, &TransformView::OnEnChangeEditRotationZ)
	ON_EN_CHANGE(IDC_EDIT_SCALE_X, &TransformView::OnEnChangeEditScaleX)
	ON_EN_CHANGE(IDC_EDIT_SCALE_Y, &TransformView::OnEnChangeEditScaleY)
	ON_EN_CHANGE(IDC_EDIT_SCALE_Z, &TransformView::OnEnChangeEditScaleZ)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &TransformView::OnBnClickedCheckComponentActive)
	ON_EN_CHANGE(IDC_EDIT_TAG_NAME, &TransformView::OnEnChangeEditTagName)
END_MESSAGE_MAP()


// TransformView 진단

#ifdef _DEBUG
void TransformView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void TransformView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// TransformView 메시지 처리기


void TransformView::ResetTransformView()
{
	EditPositionX.SetWindowTextW(L"0");
	EditPositionY.SetWindowTextW(L"0");
	EditPositionZ.SetWindowTextW(L"0");
	EditRotationX.SetWindowTextW(L"0");
	EditRotationY.SetWindowTextW(L"0");
	EditRotationZ.SetWindowTextW(L"0");
	EditScaleX.SetWindowTextW(L"1");
	EditScaleY.SetWindowTextW(L"1");
	EditScaleZ.SetWindowTextW(L"1");
}

void TransformView::SetTransformView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalPosition().x);
	EditPositionX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalPosition().y);
	EditPositionY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalPosition().z);
	EditPositionZ.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalRotation().x);
	EditRotationX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalRotation().y);
	EditRotationY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalRotation().z);
	EditRotationZ.SetWindowTextW(_tempString);

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalScale().x);
	EditScaleX.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalScale().y);
	EditScaleY.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::Transform>()->GetLocalScale().z);
	EditScaleZ.SetWindowTextW(_tempString);

	CheckComponentActive.SetCheck(_NowGameObject->Object::GetActive());

	_tempString = _NowGameObject->GetTag().c_str();
	EditTagName.SetWindowTextW(_tempString);

}

//int TransformView::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CFormView::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
//
//	return 0;
//}


BOOL TransformView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void TransformView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// 현재 선택된 트랜스폼의 값을 초기화한다.
	ResetTransformView();

}


void TransformView::OnBnClickedButtonTransformReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 현재 선택된 트랜스폼의 값을 초기화한다.
	ResetTransformView();
}





void TransformView::OnEnChangeEditPositionX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditPositionX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditPositionX.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalPosition(_temp);
		}

	}
}


void TransformView::OnEnChangeEditPositionY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditPositionY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditPositionY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalPosition(_temp);
		}

	}
}


void TransformView::OnEnChangeEditPositionZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditPositionZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditPositionZ.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalPosition().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalPosition(_temp);
		}

	}
}


void TransformView::OnEnChangeEditRotationX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRotationX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Rotator _temp;

			CString _tempString;
			EditRotationX.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalRotation(_temp);
		}

	}
}


void TransformView::OnEnChangeEditRotationY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRotationY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Rotator _temp;

			CString _tempString;
			EditRotationY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalRotation(_temp);
		}

	}
}


void TransformView::OnEnChangeEditRotationZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRotationZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Rotator _temp;

			CString _tempString;
			EditRotationZ.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalRotation().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalRotation(_temp);
		}

	}
}


void TransformView::OnEnChangeEditScaleX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditScaleX.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditScaleX.GetWindowTextW(_tempString);

			_temp.x = _tstof(_tempString);
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().y;
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalScale(_temp);
		}

	}
}


void TransformView::OnEnChangeEditScaleY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditScaleY.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditScaleY.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().x;
			_temp.y = _tstof(_tempString);
			_temp.z = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().z;

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalScale(_temp);
		}

	}
}


void TransformView::OnEnChangeEditScaleZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditScaleZ.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditScaleZ.GetWindowTextW(_tempString);

			_temp.x = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().x;
			_temp.y = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->GetLocalScale().y;
			_temp.z = _tstof(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Transform>()->SetLocalScale(_temp);
		}

	}
}


void TransformView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->SetActive(CheckComponentActive.GetCheck());
}


void TransformView::OnEnChangeEditTagName()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditTagName.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditTagName.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->SetTag(T2W(_tempString.GetBuffer()));
		}

	}
}
