#pragma once

// SceneView 대화 상자

class SceneView : public CDialogEx
{
	DECLARE_DYNAMIC(SceneView)

public:
	SceneView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~SceneView();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SceneView };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

private:
	bool IsDraw = false;
	bool IsMove = false;

public:
//	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
