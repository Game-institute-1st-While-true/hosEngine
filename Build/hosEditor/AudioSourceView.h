#pragma once



// AudioSourceView 폼 보기

class AudioSourceView : public CFormView
{
	DECLARE_DYNCREATE(AudioSourceView)

public:
	AudioSourceView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AudioSourceView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AudioSourceView };
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
	void ResetAudioSourceView();
	void SetAudioSourceView();

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	CButton CheckMute;
	CButton CheckLoop;
	CButton CheckPlayOnAwake;
	CEdit EditVolume;
	CButton CheckComponentActive;
	afx_msg void OnBnClickedButtonAudioSourceReset();
	afx_msg void OnBnClickedCheckMute();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnBnClickedCheckPlayOnAwake();
	afx_msg void OnEnChangeEditVolume();
	afx_msg void OnBnClickedCheckComponentActive();
	afx_msg void OnBnClickedButtonLoadAudioSource();
	CStatic StaticAudioSourceName;
	afx_msg void OnBnClickedCheck2d3dCheck();
	CButton Check2D3DCheck;
	CListBox ListAudioSourceList;
	CButton PlayBtn;
	afx_msg void OnBnClickedPlayAudioSource();
};


