#pragma once



// AudioListenerView 폼 보기

class AudioListenerView : public CFormView
{
	DECLARE_DYNCREATE(AudioListenerView)

public:
	AudioListenerView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AudioListenerView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AudioListenerView };
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
	void ResetAudioListenerView();
	void SetAudioListenerView();

public:
	afx_msg void OnBnClickedCheckComponentActive();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CButton CheckComponentActive;
};


