#pragma once



// RigidbodyView 폼 보기

class RigidbodyView : public CFormView
{
	DECLARE_DYNCREATE(RigidbodyView)

public:
	RigidbodyView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~RigidbodyView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RigidbodyView };
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
	void ResetRigidbodyView();
	void SetRigidbodyView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CEdit EditMass;
	CEdit EditDrag;
//	CEdit EditUseGravity;
	CEdit EditAngularDrag;
	CButton CheckUseGravity;
	CButton CheckFreezePositionX;
	CButton CheckFreezePositionY;
	CButton CheckFreezePositionZ;
	CButton CheckFreezeRotationX;
	CButton CheckFreezeRotationY;
	CButton CheckFreezeRotationZ;
	afx_msg void OnEnChangeEditMass();
	afx_msg void OnEnChangeEditDrag();
	afx_msg void OnEnChangeEditAngularDrag();
	afx_msg void OnBnClickedCheckUseGravity();
	afx_msg void OnBnClickedCheckFreezePositionX();
//	afx_msg void OnBnClickedCheckIdcCheckFreezePositiony();
	afx_msg void OnBnClickedCheckidcCheckFreezePositionZ();
	afx_msg void OnBnClickedCheckFreezeRotationX();
	afx_msg void OnBnClickedCheckFreezeRotationY();
	afx_msg void OnBnClickedCheckFreezeRotationZ();
	afx_msg void OnBnClickedButtonRigidbodyReset();
	afx_msg void OnBnClickedCheckIsKinematic();
	CButton CheckIsKinematic;
	afx_msg void OnBnClickedCheckIdcCheckFreezePositiony();
	CButton CheckComponentActive;
//	afx_msg void OnBnClickedCheckComponentActive();
};


