#pragma once



// SphereCollisionView 폼 보기

class SphereCollisionView : public CFormView
{
	DECLARE_DYNCREATE(SphereCollisionView)

public:
	SphereCollisionView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~SphereCollisionView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SphereCollisionView };
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
	void ResetSphereCollisionView();
	void SetSphereCollisionView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditCenterX;
	CEdit EditCenterY;
	CEdit EditCenterZ;
	CEdit EditRadius;
	afx_msg void OnEnChangeEditCenterX();
	afx_msg void OnEnChangeEditCenterY();
	afx_msg void OnEnChangeEditCenterZ();
	afx_msg void OnEnChangeEditRadius();
	afx_msg void OnBnClickedCheckIsTrigger();
	CButton CheckIsTrigger;
	afx_msg void OnBnClickedButtonSphereCollisionReset();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	CEdit EditDynamicFriction;
	CEdit EditStaticFriction;
	CEdit EditBounciness;
	CComboBox ComboFrictionCombine;
	CComboBox ComboBounceCombine;
	afx_msg void OnEnChangeEditDynamicFriction();
	afx_msg void OnEnChangeEditStaticFriction();
	afx_msg void OnEnChangeEditBounciness();
	afx_msg void OnCbnCloseupComboFrictionCombine();
	afx_msg void OnCbnCloseupComboBounceCombine();
	CButton CheckMaterialActive;
	afx_msg void OnBnClickedCheckMaterialActive();
	CComboBox ComboLayer;
	afx_msg void OnCbnCloseupComboLayer();
};


