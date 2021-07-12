#pragma once



// ScriptView 폼 보기

class ScriptView : public CFormView
{
	DECLARE_DYNCREATE(ScriptView)

public:
	ScriptView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~ScriptView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ScriptView };
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
	void ResetScriptView();
	void SetScriptView(int index);

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnEnChangeEditScriptName();
	afx_msg void OnBnClickedCheckComponentActive();
	CEdit EditScriptName;
	CButton CheckComponentActive;
	hos::com::Script* EditorScript;
};


