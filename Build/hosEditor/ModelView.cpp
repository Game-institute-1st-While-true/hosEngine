// ModelView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditer.h"
#include "ModelView.h"


// ModelView

IMPLEMENT_DYNCREATE(ModelView, CFormView)

ModelView::ModelView()
	: CFormView(IDD_ModelView)
{

}

ModelView::~ModelView()
{
}

void ModelView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MODEL_PATH, ModelViewPath);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_LIST_MODEL_LIST, ListModelList);
}

BEGIN_MESSAGE_MAP(ModelView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_LOAD, &ModelView::OnBnClickedButtonModelLoad)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_REMOVE, &ModelView::OnBnClickedButtonModelRemove)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &ModelView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// ModelView 진단

#ifdef _DEBUG
void ModelView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ModelView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void ModelView::ResetModelView()
{
	ModelViewPath.SetWindowTextW(L"Path : ");

	// 모델 제거?
}

void ModelView::SetModelView()
{
	if (!EditerManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditerManager::GetIns()->GetNowGameObject();

	CString _tempString = L"Path : ";

	_tempString.Append(_NowGameObject->GetComponent<hos::com::Model>()->GetPath().data());
	ModelViewPath.SetWindowTextW(_tempString);

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::Model>()->GetActive());

}

// ModelView 메시지 처리기


BOOL ModelView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void ModelView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	ListModelList.AddString(L"Machine");
	ListModelList.AddString(L"test3");
	ListModelList.AddString(L"test4");
	ListModelList.AddString(L"char");
	ListModelList.AddString(L"frog");
	//ListModelList.AddString(L"map");
	ListModelList.AddString(L"fence");
	ListModelList.AddString(L"sign");
	ListModelList.AddString(L"wall2");
	ListModelList.AddString(L"wall3");
	ListModelList.AddString(L"wirefence");

	ListModelList.AddString(L"fence1");
	ListModelList.AddString(L"fence2");
	ListModelList.AddString(L"light1");
	ListModelList.AddString(L"machine2");
	ListModelList.AddString(L"pillar3");
	ListModelList.AddString(L"sign1");
	ListModelList.AddString(L"wallprop2");

}


void ModelView::OnBnClickedButtonModelLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListModelList.GetCurSel() < 0)
	{
		return;
	}

	EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->SetActive(false);

	if (EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->GetModel())
	{
		EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->RemoveModel();
	}

	// 모델 리스트에서 선택한 모델을 불러온다.

	CString _ModelPath = L"..\\Resource\\Ase\\";
	CString _ModelName;

	ListModelList.GetText(ListModelList.GetCurSel(), _ModelName);

	_ModelName.Append(L".ase");

	_ModelPath.Append(_ModelName);

	//L"..\\Resource\\Ase\\Machine.ase"

	EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->LoadModel(T2W(_ModelPath.GetBuffer()));

	EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->SetActive(true);

	SetModelView();

	/*
	// 모델 경로를 가져와야 함

	// 제대로 된 경로인가 확인

	// 모델 로드

	// 뷰의 경로 갱신

	static TCHAR BASED_CODE szFilter[] = _T("*.ase, *.ASE | *.ase; *.ASE |");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(true, L"*.ase", L"", OFN_HIDEREADONLY, szFilter);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();

		// 기존 모델 제거
		if (EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->GetModel())
		{
			EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->RemoveModel();
		}

		// 매니저에 경로를 보낸다.
		EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->LoadModel(T2W(_Path.GetBuffer()));
		
		SetModelView();

	}
	*/
}


void ModelView::OnBnClickedButtonModelRemove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 모델 존재 여부 확인

	// 존재하면 지우고 뷰의 경로 초기화

	if (EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->GetModel())
	{
		EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->RemoveModel();

		ResetModelView();
	}

}


void ModelView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditerManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Model>()->SetActive(CheckComponentActive.GetCheck());
}
