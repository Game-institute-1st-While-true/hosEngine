#pragma once



// UITextView 폼 보기

class UITextView : public CFormView
{
	DECLARE_DYNCREATE(UITextView)

public:
	UITextView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~UITextView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UITextView };
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
	void ResetUITextView();
	void SetUITextView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnEnChangeEditLayerOrder();
	afx_msg void OnEnChangeEditFontSize();
	afx_msg void OnEnChangeEditFontRotation();
	afx_msg void OnEnChangeEditColorR();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnEnChangeEditColorA();
	afx_msg void OnBnClickedButtonLoadFont();
	CEdit EditLayerOrder;
	CEdit EditFontSize;
	CEdit EditFontRotation;
	CEdit EditOriginX;
	CEdit EditOriginY;
	CStatic StaticFontName;
	CListBox ListFontList;
	CEdit EditColorR;
	CEdit EditColorG;
	CEdit EditColorB;
	CEdit EditColorA;
	afx_msg void OnEnChangeEditText();
	CEdit EditText;
	afx_msg void OnBnClickedButtonLoadAnimation();
		CListBox ListTextAnimationList;
	CButton CheckUseAnimation;
	CStatic StaticAnimationName;
	CComboBox ComboAlignment;
	afx_msg void OnBnClickedCheckUseAnimation();
	afx_msg void OnCbnCloseupComboAlignment();
	afx_msg void OnBnClickedButtonDefaultFont();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	afx_msg void OnBnClickedCheckFollowUi();
	CButton CheckFollowUI;
	CButton CheckIsLoop;
	afx_msg void OnBnClickedCheckIsLoop();
	afx_msg void OnEnChangeEditCoordU2();
	afx_msg void OnEnChangeEditCoordV();
	CEdit EditCoordU;
	CEdit EditCoordV;
};


