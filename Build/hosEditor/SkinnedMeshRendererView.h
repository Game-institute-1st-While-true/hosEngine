#pragma once



// SkinnedMeshRendererView 폼 보기

class SkinnedMeshRendererView : public CFormView
{
	DECLARE_DYNCREATE(SkinnedMeshRendererView)
public:
	SkinnedMeshRendererView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~SkinnedMeshRendererView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SkinnedMeshRendererView };
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
	void ResetSkinnedMeshRendererView();
	void SetSkinnedMeshRendererView();
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CStatic StaticMaterialName;
	CListBox ListMaterialList;
	CComboBox ComboRenderMode;
	afx_msg void OnBnClickedButtonMeshLoad();
	afx_msg void OnBnClickedButtonMeshRemove();
	afx_msg void OnCbnCloseupComboRenderMode();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
};


