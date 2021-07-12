#pragma once



// ModelView 폼 보기

class ModelView : public CFormView
{
	DECLARE_DYNCREATE(ModelView)
public:
	ModelView();           // 동적 만들기에 사용되는 protected 생성자입니다.
protected:
	virtual ~ModelView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ModelView };
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
	void ResetModelView();
	void SetModelView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedButtonModelLoad();
	afx_msg void OnBnClickedButtonModelRemove();
	CStatic ModelViewPath;
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	CListBox ListModelList;
};


