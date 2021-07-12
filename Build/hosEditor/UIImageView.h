#pragma once



// UIImageView 폼 보기

class UIImageView : public CFormView
{
	DECLARE_DYNCREATE(UIImageView)

public:
	UIImageView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~UIImageView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UIImageView };
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
	void ResetUIImageView();
	void SetUIImageView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditLayerOrder;
	CEdit EditColorR;
	CEdit EditColorG;
	CEdit EditColorB;
	CEdit EditColorA;
	CEdit EditOffX;
	CEdit EditTilX;
	CEdit EditOffY;
	CEdit EditTilY;
	CStatic StaticTextureName;
	CStatic StaticAnimationName;
	CListBox ListTextureList;
	CListBox ListAnimationList;
	CButton CheckUseTexture;
	CButton CheckUseAnimation;
	afx_msg void OnEnChangeEditLayerOrder();
	afx_msg void OnEnChangeEditColorR();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnEnChangeEditColorA();
	afx_msg void OnEnChangeEditOffX();
	afx_msg void OnEnChangeEditTilX();
	afx_msg void OnEnChangeEditOffY();
	afx_msg void OnEnChangeEditTilY();
	afx_msg void OnBnClickedCheckUseTexture();
	afx_msg void OnBnClickedCheckUseAnimation();
	afx_msg void OnBnClickedButtonTextureLoad();
	afx_msg void OnBnClickedButtonAnimationLoad();
	afx_msg void OnCbnCloseupComboAlignment();
	CComboBox ComboAlignment;
	afx_msg void OnBnClickedCheckComponentActive();
	CButton CheckComponentActive;
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnEnChangeEditHeight();
	CEdit EditWidth;
	CEdit EditHeight;
	afx_msg void OnBnClickedCheckFollowUi();
	CButton CheckFollowUI;
	afx_msg void OnBnClickedCheck1();
	CButton IsLoopCheck;
	afx_msg void OnEnChangeEditCoordU();
	afx_msg void OnEnChangeEditCoordV();
	CEdit EditCoordU;
	CEdit EditCoordV;
};


