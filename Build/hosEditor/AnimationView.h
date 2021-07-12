#pragma once



// AnimationView 폼 보기

class AnimationView : public CFormView
{
	DECLARE_DYNCREATE(AnimationView)

public:
	AnimationView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AnimationView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AnimationView };
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
	void ResetAnimationView();
	void SetAnimationView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CStatic StaticBundleName;
	CStatic StaticAnimationClip;
	CListBox ListBundleList;
	CListBox ListClipList;
	afx_msg void OnBnClickedButtonLoadClip();
	CStatic StaticNowTick;
	afx_msg void OnLbnSelchangeListBundleList();
	afx_msg void OnBnClickedCheckAnimationPlay();
	CButton CheckAnimationPlay;
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	CStatic StaticEndTick;
};


