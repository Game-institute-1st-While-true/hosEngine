#pragma once



// CameraView 폼 보기

class CameraView : public CFormView
{
	DECLARE_DYNCREATE(CameraView)

public:
	CameraView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CameraView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CameraView };
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
	void ResetCameraView();
	void SetCameraView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditFOV;
	CEdit EditNear;
	CEdit EditFar;
	CEdit EditClearColorR;
	CEdit EditClearColorG;
	CEdit EditClearColorB;
	CComboBox ComboMode;
	CComboBox ComboViewMode;
	CComboBox ComboClearFlag;
	afx_msg void OnEnChangeEditFov();
	afx_msg void OnEnChangeEditNear();
	afx_msg void OnEnChangeEditFar();
	afx_msg void OnEnChangeEditClearColorR();
	afx_msg void OnEnChangeEditClearColorG();
	afx_msg void OnEnChangeEditClearColorB();
	afx_msg void OnCbnCloseupComboMode();
	afx_msg void OnCbnCloseupComboViewmode();
	afx_msg void OnCbnCloseupComboClearflag();
	afx_msg void OnBnClickedButtonCameraReset();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	CEdit EditViewPortLeft;
	CEdit EditViewPortTop;
	CEdit EditViewPortRight;
	CEdit EditViewPortBottom;
	CStatic StaticSkyBoxName;
	CListBox ListSkyBoxList;
	afx_msg void OnEnChangeEditViewPortLeft();
	afx_msg void OnEnChangeEditViewPortTop();
	afx_msg void OnEnChangeEditViewPortRight();
	afx_msg void OnEnChangeEditViewPortBottom();
	afx_msg void OnBnClickedButtonLoadSkyBox();
	afx_msg void OnBnClickedButtonRemoveSkyBox();
};


