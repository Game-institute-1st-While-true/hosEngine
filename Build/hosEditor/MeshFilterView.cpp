// MeshFilterView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "MeshFilterView.h"


// MeshFilterView

IMPLEMENT_DYNCREATE(MeshFilterView, CFormView)

MeshFilterView::MeshFilterView()
	: CFormView(IDD_MeshFilterView)
{

}

MeshFilterView::~MeshFilterView()
{
}

void MeshFilterView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESH_LIST, ListMeshList);
	DDX_Control(pDX, IDC_STATIC_MESH_NAME, StaticMeshName);
}

BEGIN_MESSAGE_MAP(MeshFilterView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MESH_LOAD, &MeshFilterView::OnBnClickedButtonMeshLoad)
	ON_BN_CLICKED(IDC_BUTTON_MESH_REMOVE, &MeshFilterView::OnBnClickedButtonMeshRemove)
END_MESSAGE_MAP()


// MeshFilterView 진단

#ifdef _DEBUG
void MeshFilterView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void MeshFilterView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void MeshFilterView::ResetMeshFilterView()
{

}

void MeshFilterView::SetMeshFilterView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString = L"Mesh Name : ";
	if (_NowGameObject->GetComponent<hos::com::MeshFilter>()->GetMeshData())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::MeshFilter>()->GetMeshName().c_str());
	}
	StaticMeshName.SetWindowTextW(_tempString);

	while (ListMeshList.GetCount() != 0)
	{
		ListMeshList.DeleteString(0);
	}

	for (auto [name, data] : EditorManager::GetIns()->GetDataManager()->MeshList)
	{
		ListMeshList.AddString(name.c_str());
	}



}

// MeshFilterView 메시지 처리기


BOOL MeshFilterView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void MeshFilterView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void MeshFilterView::OnBnClickedButtonMeshLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (ListMeshList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	_NowGameObject->GetComponent<hos::com::MeshFilter>()->RemoveMeshData();
	
	CString _MeshName;
	ListMeshList.GetText(ListMeshList.GetCurSel(), _MeshName);

	_NowGameObject->GetComponent<hos::com::MeshFilter>()->SetMeshData(g_DataManager->MeshList.find(T2W(_MeshName.GetBuffer()))->second);

	SetMeshFilterView();

}


void MeshFilterView::OnBnClickedButtonMeshRemove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	_NowGameObject->GetComponent<hos::com::MeshFilter>()->RemoveMeshData();

	SetMeshFilterView();

}

