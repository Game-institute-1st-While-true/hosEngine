// ScriptView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "ScriptView.h"


// ScriptView

IMPLEMENT_DYNCREATE(ScriptView, CFormView)

ScriptView::ScriptView()
	: CFormView(IDD_ScriptView)
{

}

ScriptView::~ScriptView()
{
}

void ScriptView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SCRIPT_NAME, EditScriptName);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
}

BEGIN_MESSAGE_MAP(ScriptView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_SCRIPT_NAME, &ScriptView::OnEnChangeEditScriptName)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &ScriptView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// ScriptView 진단

#ifdef _DEBUG
void ScriptView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ScriptView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void ScriptView::ResetScriptView()
{
	EditScriptName.SetWindowTextW(L"NewScript");
}

void ScriptView::SetScriptView(int index)
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();
	
	hos::com::Script* script;

	EditorScript = nullptr;

	for (int i = 0; i < _NowGameObject->GetComponents().size(); i++)
	{
		script = dynamic_cast<hos::com::Script*>(_NowGameObject->GetComponents()[i]);
		if (script)
		{
			if (script->GetEditorIndex() == -1)
			{
				script->SetEditorIndex(i);
			}

			if (script->GetEditorIndex() == index)
			{
				EditorScript = script;

				EditScriptName.SetWindowTextW(script->GetScriptName().c_str());

				CheckComponentActive.SetCheck(script->GetActive());
			}
		}
	}
}

// ScriptView 메시지 처리기


BOOL ScriptView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void ScriptView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void ScriptView::OnEnChangeEditScriptName()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditScriptName.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			CString _tempString;
			EditScriptName.GetWindowTextW(_tempString);

			EditorScript->SetScriptName(T2W(_tempString.GetBuffer()));
		}

	}

}


void ScriptView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorScript->SetActive(CheckComponentActive.GetCheck());
}
