#pragma once



// NavInfoView 폼 보기

class NavInfoView : public CFormView
{
	DECLARE_DYNCREATE(NavInfoView)

public:
	NavInfoView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~NavInfoView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NavInfoView };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void ResetNavInfoView();
	void SetNavInfoView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditCellSize;
	CEdit EditCellHeight;
	CEdit EditAgentHeight;
	CEdit EditAgentMaxClimb;
	CEdit EditAgentRadius;
	CEdit EditAgentMaxSlope;
	CEdit EditRegionMinSize;
	CEdit EditRegionMergeSize;
	CEdit EditEdgeMaxLen;
	CEdit EditEdgeMaxError;
	CEdit EditDetailSampleDist;
	CEdit EditDetailSampleMaxError;
	CEdit EditVertsPerPoly;
	CButton CheckFilterLowHangingObstacles;
	CButton CheckFilterLedgeSpans;
	CButton CheckFilterWalkableLowHeightSpans;
	afx_msg void OnEnChangeEditCellSize();
	afx_msg void OnEnChangeEditCellHeight();
	afx_msg void OnEnChangeEditAgentHeight();
	afx_msg void OnEnChangeEditAgentMaxClimb();
	afx_msg void OnEnChangeEditAgentRadius();
	afx_msg void OnEnChangeEditAgentMaxSlope();
	afx_msg void OnEnChangeEditRegionMinSize();
	afx_msg void OnEnChangeEditRegionMergeSize();
	afx_msg void OnEnChangeEditEdgeMaxLen();
	afx_msg void OnEnChangeEditEdgeMaxError();
	afx_msg void OnEnChangeEditDetailSampleDist();
	afx_msg void OnEnChangeEditDetailSampleMaxError2();
	afx_msg void OnEnChangeEditVertsPerPoly();
	afx_msg void OnBnClickedCheckFilterLowHangingObstacles();
	afx_msg void OnBnClickedCheckFilterLedgeSpans();
	afx_msg void OnBnClickedCheckFilterWalkableLowHeightSpans();
	afx_msg void OnBnClickedButtonBake();
};


