#pragma once



// MeshRendererView 폼 보기

class MeshRendererView : public CFormView
{
	DECLARE_DYNCREATE(MeshRendererView)
public:
	MeshRendererView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~MeshRendererView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MeshRendererView };
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
	void ResetMeshRendererView();
	void SetMeshRendererView();
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CStatic StaticMaterialName;
	CListBox ListMaterialList;
	CComboBox ComboRenderMode;
	afx_msg void OnCbnCloseupComboRenderMode();
	afx_msg void OnBnClickedButtonMeshLoad();
	afx_msg void OnBnClickedButtonMeshRemove();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
};


