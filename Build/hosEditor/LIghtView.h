#pragma once



// LIghtView 폼 보기

class LIghtView : public CFormView
{
	DECLARE_DYNCREATE(LIghtView)
public:
	LIghtView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LIghtView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIghtView };
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
	void ResetLightView();
	void SetLightView();
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditRange;
	CEdit EditAngle;
	CEdit EditIntensity;
	CEdit EditColorX;
	CEdit EditColorY;
	CEdit EditColorZ;
	CComboBox ComboType;
	afx_msg void OnEnChangeEditRange();
	afx_msg void OnEnChangeEditAngle();
	afx_msg void OnEnChangeEditIntensity();
	afx_msg void OnEnChangeEditColorX();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnCbnCloseupComboType();
	afx_msg void OnBnClickedCheckComponentActive();
	CButton CheckComponentActive;
	afx_msg void OnEnChangeEditConst();
	afx_msg void OnEnChangeEditLinear();
	afx_msg void OnEnChangeEditQuad();
	CEdit EditConst;
	CEdit EditLinear;
	CEdit EditQuad;
};


