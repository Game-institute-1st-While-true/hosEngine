﻿// SkinnedMeshRendererView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "SkinnedMeshRendererView.h"


// SkinnedMeshRendererView

IMPLEMENT_DYNCREATE(SkinnedMeshRendererView, CFormView)

SkinnedMeshRendererView::SkinnedMeshRendererView()
	: CFormView(IDD_SkinnedMeshRendererView)
{

}

SkinnedMeshRendererView::~SkinnedMeshRendererView()
{
}

void SkinnedMeshRendererView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MATERIAL_NAME, StaticMaterialName);
	DDX_Control(pDX, IDC_LIST_MATERIAL_LIST, ListMaterialList);
	DDX_Control(pDX, IDC_COMBO_RENDER_MODE, ComboRenderMode);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
}

BEGIN_MESSAGE_MAP(SkinnedMeshRendererView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MESH_LOAD, &SkinnedMeshRendererView::OnBnClickedButtonMeshLoad)
	ON_BN_CLICKED(IDC_BUTTON_MESH_REMOVE, &SkinnedMeshRendererView::OnBnClickedButtonMeshRemove)
	ON_CBN_CLOSEUP(IDC_COMBO_RENDER_MODE, &SkinnedMeshRendererView::OnCbnCloseupComboRenderMode)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &SkinnedMeshRendererView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// SkinnedMeshRendererView 진단

#ifdef _DEBUG
void SkinnedMeshRendererView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SkinnedMeshRendererView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void SkinnedMeshRendererView::ResetSkinnedMeshRendererView()
{

}

void SkinnedMeshRendererView::SetSkinnedMeshRendererView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString = L"Material Name : ";
	if (_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->GetMaterialRef())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->GetMaterialName().c_str());
	}

	StaticMaterialName.SetWindowTextW(_tempString);

	ComboRenderMode.SetCurSel((int)_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->GetRenderMode());

	while (ListMaterialList.GetCount() != 0)
	{
		ListMaterialList.DeleteString(0);
	}

	for (auto [name, data] : EditorManager::GetIns()->GetDataManager()->MaterialList)
	{
		ListMaterialList.AddString(name.c_str());
	}

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->GetActive());

}

// SkinnedMeshRendererView 메시지 처리기


BOOL SkinnedMeshRendererView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void SkinnedMeshRendererView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ComboRenderMode.AddString(L"PBR");
	ComboRenderMode.AddString(L"Lambert");
}


void SkinnedMeshRendererView::OnBnClickedButtonMeshLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (ListMaterialList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->RemoveMaterial();

	CString _MaterialName;
	ListMaterialList.GetText(ListMaterialList.GetCurSel(), _MaterialName);

	_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->SetMaterial(T2W(_MaterialName.GetBuffer()));

	SetSkinnedMeshRendererView();
}


void SkinnedMeshRendererView::OnBnClickedButtonMeshRemove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	_NowGameObject->GetComponent<hos::com::SkinnedMeshRenderer>()->RemoveMaterial();

	SetSkinnedMeshRendererView();
}


void SkinnedMeshRendererView::OnCbnCloseupComboRenderMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 <= ComboRenderMode.GetCurSel() && ComboRenderMode.GetCurSel() < (int)hos::cg::RendererDesc::RenderModeList::RENDER_MAX)
	{
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SkinnedMeshRenderer>()->SetRenderMode((hos::cg::RendererDesc::RenderModeList)ComboRenderMode.GetCurSel());

		SetSkinnedMeshRendererView();

	}
}


void SkinnedMeshRendererView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::SkinnedMeshRenderer>()->SetActive(CheckComponentActive.GetCheck());
}
