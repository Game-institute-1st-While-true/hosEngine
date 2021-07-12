// AudioListenerView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "AudioListenerView.h"


// AudioListenerView

IMPLEMENT_DYNCREATE(AudioListenerView, CFormView)

AudioListenerView::AudioListenerView()
	: CFormView(IDD_AudioListenerView)
{

}

AudioListenerView::~AudioListenerView()
{
}

void AudioListenerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
}

BEGIN_MESSAGE_MAP(AudioListenerView, CFormView)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &AudioListenerView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// AudioListenerView 진단

#ifdef _DEBUG
void AudioListenerView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AudioListenerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void AudioListenerView::ResetAudioListenerView()
{
	//CheckComponentActive.SetCheck(FALSE);
}

void AudioListenerView::SetAudioListenerView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::AudioListener>()->GetActive());

}

// AudioListenerView 메시지 처리기


void AudioListenerView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioListener>()->SetActive(CheckComponentActive.GetCheck());
}


BOOL AudioListenerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void AudioListenerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}
