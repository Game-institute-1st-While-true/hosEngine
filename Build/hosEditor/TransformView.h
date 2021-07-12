#pragma once

#include "EditorManager.h"

// TransformView 폼 보기

class TransformView : public CFormView
{
	DECLARE_DYNCREATE(TransformView)

public:
	TransformView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~TransformView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TransformView };
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
	void ResetTransformView();
	void SetTransformView();

public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CStatic StaticPositionX;
	CStatic StaticPositionY;
	CStatic StaticPositionZ;
	CEdit EditPositionX;
	CEdit EditPositionY;
	CEdit EditPositionZ;
	CStatic StaticRotationX;
	CStatic StaticRotationY;
	CStatic StaticRotationZ;
	CEdit EditRotationX;
	CEdit EditRotationY;
	CEdit EditRotationZ;
	CStatic StaticScaleX;
	CStatic StaticScaleY;
	CStatic StaticScaleZ;
	CEdit EditScaleX;
	CEdit EditScaleY;
	CEdit EditScaleZ;
	afx_msg void OnBnClickedButtonTransformReset();

	afx_msg void OnEnChangeEditPositionX();
//	afx_msg void OnEnUpdateEditPositionY();
	afx_msg void OnEnChangeEditPositionY();
	afx_msg void OnEnChangeEditPositionZ();
	afx_msg void OnEnChangeEditRotationX();
	afx_msg void OnEnChangeEditRotationY();
	afx_msg void OnEnChangeEditRotationZ();
	afx_msg void OnEnChangeEditScaleX();
	afx_msg void OnEnChangeEditScaleY();
	afx_msg void OnEnChangeEditScaleZ();
	CButton CheckComponentActive;
	afx_msg void OnBnClickedCheckComponentActive();
	CEdit EditTagName;
	afx_msg void OnEnChangeEditTagName();
};


