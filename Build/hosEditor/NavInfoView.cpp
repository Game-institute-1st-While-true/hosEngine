// NavInfoView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "NavInfoView.h"


// NavInfoView

IMPLEMENT_DYNCREATE(NavInfoView, CFormView)

NavInfoView::NavInfoView()
	: CFormView(IDD_NavInfoView)
{

}

NavInfoView::~NavInfoView()
{
}

void NavInfoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CELL_SIZE, EditCellSize);
	DDX_Control(pDX, IDC_EDIT_CELL_HEIGHT, EditCellHeight);
	DDX_Control(pDX, IDC_EDIT_AGENT_HEIGHT, EditAgentHeight);
	DDX_Control(pDX, IDC_EDIT_AGENT_MAX_CLIMB, EditAgentMaxClimb);
	DDX_Control(pDX, IDC_EDIT_AGENT_RADIUS, EditAgentRadius);
	DDX_Control(pDX, IDC_EDIT_AGENT_MAX_SLOPE, EditAgentMaxSlope);
	DDX_Control(pDX, IDC_EDIT_REGION_MIN_SIZE, EditRegionMinSize);
	DDX_Control(pDX, IDC_EDIT_REGION_MERGE_SIZE, EditRegionMergeSize);
	DDX_Control(pDX, IDC_EDIT_EDGE_MAX_LEN, EditEdgeMaxLen);
	DDX_Control(pDX, IDC_EDIT_EDGE_MAX_ERROR, EditEdgeMaxError);
	DDX_Control(pDX, IDC_EDIT_DETAIL_SAMPLE_DIST, EditDetailSampleDist);
	DDX_Control(pDX, IDC_EDIT_DETAIL_SAMPLE_MAX_ERROR2, EditDetailSampleMaxError);
	DDX_Control(pDX, IDC_EDIT_VERTS_PER_POLY, EditVertsPerPoly);
	DDX_Control(pDX, IDC_CHECK_FILTER_LOW_HANGING_OBSTACLES, CheckFilterLowHangingObstacles);
	DDX_Control(pDX, IDC_CHECK_FILTER_LEDGE_SPANS, CheckFilterLedgeSpans);
	DDX_Control(pDX, IDC_CHECK_FILTER_WALKABLE_LOW_HEIGHT_SPANS, CheckFilterWalkableLowHeightSpans);
}

BEGIN_MESSAGE_MAP(NavInfoView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_CELL_SIZE, &NavInfoView::OnEnChangeEditCellSize)
	ON_EN_CHANGE(IDC_EDIT_CELL_HEIGHT, &NavInfoView::OnEnChangeEditCellHeight)
	ON_EN_CHANGE(IDC_EDIT_AGENT_HEIGHT, &NavInfoView::OnEnChangeEditAgentHeight)
	ON_EN_CHANGE(IDC_EDIT_AGENT_MAX_CLIMB, &NavInfoView::OnEnChangeEditAgentMaxClimb)
	ON_EN_CHANGE(IDC_EDIT_AGENT_RADIUS, &NavInfoView::OnEnChangeEditAgentRadius)
	ON_EN_CHANGE(IDC_EDIT_AGENT_MAX_SLOPE, &NavInfoView::OnEnChangeEditAgentMaxSlope)
	ON_EN_CHANGE(IDC_EDIT_REGION_MIN_SIZE, &NavInfoView::OnEnChangeEditRegionMinSize)
	ON_EN_CHANGE(IDC_EDIT_REGION_MERGE_SIZE, &NavInfoView::OnEnChangeEditRegionMergeSize)
	ON_EN_CHANGE(IDC_EDIT_EDGE_MAX_LEN, &NavInfoView::OnEnChangeEditEdgeMaxLen)
	ON_EN_CHANGE(IDC_EDIT_EDGE_MAX_ERROR, &NavInfoView::OnEnChangeEditEdgeMaxError)
	ON_EN_CHANGE(IDC_EDIT_DETAIL_SAMPLE_DIST, &NavInfoView::OnEnChangeEditDetailSampleDist)
	ON_EN_CHANGE(IDC_EDIT_DETAIL_SAMPLE_MAX_ERROR2, &NavInfoView::OnEnChangeEditDetailSampleMaxError2)
	ON_EN_CHANGE(IDC_EDIT_VERTS_PER_POLY, &NavInfoView::OnEnChangeEditVertsPerPoly)
	ON_BN_CLICKED(IDC_CHECK_FILTER_LOW_HANGING_OBSTACLES, &NavInfoView::OnBnClickedCheckFilterLowHangingObstacles)
	ON_BN_CLICKED(IDC_CHECK_FILTER_LEDGE_SPANS, &NavInfoView::OnBnClickedCheckFilterLedgeSpans)
	ON_BN_CLICKED(IDC_CHECK_FILTER_WALKABLE_LOW_HEIGHT_SPANS, &NavInfoView::OnBnClickedCheckFilterWalkableLowHeightSpans)
	ON_BN_CLICKED(IDC_BUTTON_BAKE, &NavInfoView::OnBnClickedButtonBake)
END_MESSAGE_MAP()


// NavInfoView 진단

#ifdef _DEBUG
void NavInfoView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void NavInfoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void NavInfoView::ResetNavInfoView()
{

}

void NavInfoView::SetNavInfoView()
{
	CString _tempString;

	// Cell
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetCellSize());
	EditCellSize.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetCellHeight());
	EditCellHeight.SetWindowTextW(_tempString);

	// Agent
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetAgentHeight());
	EditAgentHeight.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetAgentRadius());
	EditAgentRadius.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetAgentMaxClimb());
	EditAgentMaxClimb.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetAgentMaxSlope());
	EditAgentMaxSlope.SetWindowTextW(_tempString);

	// Region
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetRegionMinSize());
	EditRegionMinSize.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetRegionMergeSize());
	EditRegionMergeSize.SetWindowTextW(_tempString);

	// Edge
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetEdgeMaxLen());
	EditEdgeMaxLen.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetEdgeMaxError());
	EditEdgeMaxError.SetWindowTextW(_tempString);

	// Detail Sample
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetDetailSampleDist());
	EditDetailSampleDist.SetWindowTextW(_tempString);
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetDetailSampleMaxError());
	EditDetailSampleMaxError.SetWindowTextW(_tempString);

	// Verts Per Poly
	_tempString.Format(L"%f", g_Navigation->GetMaker()->GetVertsPerPoly());
	EditVertsPerPoly.SetWindowTextW(_tempString);

	// Filter
	/// 필요하다면 만들도록 하자

}

// NavInfoView 메시지 처리기


BOOL NavInfoView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void NavInfoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void NavInfoView::OnEnChangeEditCellSize()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCellSize.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditCellSize.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetCellSize(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditCellHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditCellHeight.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditCellHeight.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetCellHeight(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditAgentHeight()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAgentHeight.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditAgentHeight.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetAgentHeight(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditAgentMaxClimb()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAgentMaxClimb.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditAgentMaxClimb.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetAgentMaxClimb(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditAgentRadius()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAgentRadius.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditAgentRadius.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetAgentRadius(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditAgentMaxSlope()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditAgentMaxSlope.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditAgentMaxSlope.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetAgentMaxSlope(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditRegionMinSize()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRegionMinSize.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditRegionMinSize.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetRegionMinSize(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditRegionMergeSize()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditRegionMergeSize.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditRegionMergeSize.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetRegionMergeSize(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditEdgeMaxLen()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditEdgeMaxLen.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditEdgeMaxLen.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetEdgeMaxLen(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditEdgeMaxError()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditEdgeMaxError.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditEdgeMaxError.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetEdgeMaxError(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditDetailSampleDist()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditDetailSampleDist.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditDetailSampleDist.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetDetailSampleDist(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditDetailSampleMaxError2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditDetailSampleMaxError.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditDetailSampleMaxError.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetDetailSampleMaxError(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnEnChangeEditVertsPerPoly()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditVertsPerPoly.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{

			CString _tempString;
			EditVertsPerPoly.GetWindowTextW(_tempString);

			g_Navigation->GetMaker()->SetVertsPerPoly(_tstof(_tempString));
		}

	}
}


void NavInfoView::OnBnClickedCheckFilterLowHangingObstacles()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void NavInfoView::OnBnClickedCheckFilterLedgeSpans()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void NavInfoView::OnBnClickedCheckFilterWalkableLowHeightSpans()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void NavInfoView::OnBnClickedButtonBake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	bool b = false;

	auto maker = g_Navigation->GetMaker();
	b = maker->Bake(EditorManager::GetIns()->GetNowScene());

	if (!b)
	{
		MessageBox(L"!!!!!! BAKE FAIL !!!!!!", L"FAIL!!!!!!", MB_OK);
		return;
	}

	auto mesh = maker->GetMesh();

	hos::string _path = g_Path;

	_path.append(L"\\");

	_path.append(EditorManager::GetIns()->GetNowScene()->GetName());

	_path.append(L".bin");

	std::string mbs = hos::ut::UTF16ToAnsi(_path.c_str());

	b = maker->Save(mbs, mesh);

	if (b)
	{
		MessageBox(L"Bake OK!", L"Bake", MB_OK);
	}
	else
	{
		MessageBox(L"!!!!!! BAKE FAIL !!!!!!", L"FAIL!!!!!!", MB_OK);
	}

}
