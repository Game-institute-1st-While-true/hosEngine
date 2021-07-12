// SceneInfoView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "SceneInfoView.h"
#include "EditorManager.h"

// SceneInfoView

IMPLEMENT_DYNCREATE(SceneInfoView, CFormView)

SceneInfoView::SceneInfoView()
	: CFormView(IDD_SceneInfoView)
{

}

SceneInfoView::~SceneInfoView()
{
}

void SceneInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, LayerComboBox);
	DDX_Control(pDX, IDC_EDIT1, GravityX);
	DDX_Control(pDX, IDC_EDIT2, GravityY);
	DDX_Control(pDX, IDC_EDIT3, GravityZ);
}

BEGIN_MESSAGE_MAP(SceneInfoView, CFormView)
	ON_CLBN_CHKCHANGE(IDC_LIST1, &SceneInfoView::OnClbnChkchange1)
	ON_CBN_SELENDOK(IDC_COMBO1, &SceneInfoView::OnCbnSelendokCombo1)
	ON_CBN_EDITCHANGE(IDC_COMBO1, &SceneInfoView::OnCbnEditchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT1, &SceneInfoView::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &SceneInfoView::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &SceneInfoView::OnEnChangeEdit3)
END_MESSAGE_MAP()


// SceneInfoView 진단

#ifdef _DEBUG
void SceneInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SceneInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void SceneInfoView::ResetSceneInfoView()
{

}

void SceneInfoView::SetSceneInfoView()
{
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	hos::string str;

	LayerComboBox.ResetContent();

	for (int i = 0; i < hos::com::Scene::MAX_LAYER; i++)
	{
		str = scene->GetLayerName(i);
		LayerComboBox.InsertString(i, str.c_str());
	}

	hos::Vector3 g = scene->GetGravity();
	GravityX.SetWindowTextW(std::to_wstring(g.x).c_str());
	GravityY.SetWindowTextW(std::to_wstring(g.y).c_str());
	GravityZ.SetWindowTextW(std::to_wstring(g.z).c_str());
}

// SceneInfoView 메시지 처리기


BOOL SceneInfoView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void SceneInfoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	LayerCheckList.SubclassDlgItem(IDC_LIST1, this);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void SceneInfoView::OnCbnSelendokCombo1()
{
	LayerSelected = LayerComboBox.GetCurSel();
	LayerCheckList.ResetContent();
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	hos::string str;
	for (int i = 0; i < hos::com::Scene::MAX_LAYER; i++)
	{
		str = scene->GetLayerName(i);
		LayerCheckList.InsertString(i, str.c_str());
		LayerCheckList.SetCheck(i, scene->GetLayerCollision(LayerSelected, i));
	}
}

void SceneInfoView::OnCbnEditchangeCombo1()
{
	CString str;
	if (LayerSelected < hos::com::Scene::BUILTIN_LAYER_COUNT)
	{
		LayerComboBox.GetLBText(LayerSelected, str);
		LayerComboBox.SetWindowTextW(str);
	}
	else
	{
		LayerComboBox.DeleteString(LayerSelected);
		LayerComboBox.GetWindowTextW(str);
		LayerComboBox.InsertString(LayerSelected, str);

		hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
		scene->SetLayerName(LayerSelected, str.operator LPCWSTR());
	}
}

void SceneInfoView::OnClbnChkchange1()
{
	int selected = LayerCheckList.GetCurSel();
	bool b = LayerCheckList.GetCheck(selected);
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	scene->SetLayerCollision(LayerSelected, selected, b);
}


void SceneInfoView::OnEnChangeEdit1()
{
	CString str;
	GravityX.GetWindowTextW(str);
	float x;
	try
	{
		x = std::stof(str.GetString());
	}
	catch (std::invalid_argument e)
	{
		x = hos::com::Scene::DEFAULT_GRAVITY.x;
	}
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	hos::Vector3 g = scene->GetGravity();
	scene->SetGravity(hos::Vector3(x, g.y, g.z));
}


void SceneInfoView::OnEnChangeEdit2()
{
	CString str;
	GravityY.GetWindowTextW(str);
	float y;
	try
	{
		y = std::stof(str.GetString());
	}
	catch (std::invalid_argument e)
	{
		y = hos::com::Scene::DEFAULT_GRAVITY.y;
	}
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	hos::Vector3 g = scene->GetGravity();
	scene->SetGravity(hos::Vector3(g.x, y, g.z));
}


void SceneInfoView::OnEnChangeEdit3()
{
	CString str;
	GravityZ.GetWindowTextW(str);
	float z;
	try
	{
		z = std::stof(str.GetString());
	}
	catch (std::invalid_argument e)
	{
		z = hos::com::Scene::DEFAULT_GRAVITY.z;
	}
	hos::com::Scene* scene = EditorManager::GetIns()->GetNowScene();
	hos::Vector3 g = scene->GetGravity();
	scene->SetGravity(hos::Vector3(g.x, g.y, z));
}
