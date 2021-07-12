#pragma once



// SceneInfoView 폼 보기

class SceneInfoView : public CFormView
{
	DECLARE_DYNCREATE(SceneInfoView)

public:
	SceneInfoView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~SceneInfoView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SceneInfoView };
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
	void ResetSceneInfoView();
	void SetSceneInfoView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnCbnSelendokCombo1();
	// 레이어 콤보박스
	CComboBox LayerComboBox;
	CCheckListBox LayerCheckList;
	int LayerSelected;
	afx_msg void OnCbnEditchangeCombo1();
	afx_msg void OnClbnChkchange1();
	afx_msg void OnEnChangeEdit1();
	CEdit GravityX;
	CEdit GravityY;
	CEdit GravityZ;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
};