#pragma once



// UIButtonView 폼 보기

class UIButtonView : public CFormView
{
	DECLARE_DYNCREATE(UIButtonView)

public:
	UIButtonView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~UIButtonView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UIButtonView };
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

private:
	int StateIndex;

public:
	void ResetUIButtonView();
	void SetUIButtonView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CComboBox ComboTransition;
	CButton CheckInteractable;
	CComboBox ComboSelectState;
	CEdit EditColorR;
	CEdit EditColorG;
	CEdit EditColorB;
	CEdit EditColorA;
	CStatic StaticSpriteName;
	CListBox ListSpriteList;
	CButton CheckComponentActive;
	afx_msg void OnCbnCloseupComboTransition();
	afx_msg void OnBnClickedCheckInteractable();
	afx_msg void OnCbnCloseupComboSelectState();
	afx_msg void OnEnChangeEditColorR();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnEnChangeEditColorA();
	afx_msg void OnBnClickedButtonLoadSprite();
	afx_msg void OnBnClickedButtonRemoveSprite();
	afx_msg void OnBnClickedCheckComponentActive();
};


