// AudioSourceView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "AudioSourceView.h"


// AudioSourceView

IMPLEMENT_DYNCREATE(AudioSourceView, CFormView)

AudioSourceView::AudioSourceView()
	: CFormView(IDD_AudioSourceView)
{

}

AudioSourceView::~AudioSourceView()
{
}

void AudioSourceView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MUTE, CheckMute);
	DDX_Control(pDX, IDC_CHECK_LOOP, CheckLoop);
	DDX_Control(pDX, IDC_CHECK_PLAY_ON_AWAKE, CheckPlayOnAwake);
	DDX_Control(pDX, IDC_EDIT_VOLUME, EditVolume);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_STATIC_AUDIO_SOURCE_NAME, StaticAudioSourceName);
	DDX_Control(pDX, IDC_CHECK_2D_3D_CHECK, Check2D3DCheck);
	DDX_Control(pDX, IDC_LIST_AUDIO_SOURCE_LIST, ListAudioSourceList);
	DDX_Control(pDX, IDC_PLAY_AUDIO_SOURCE, PlayBtn);
}

BEGIN_MESSAGE_MAP(AudioSourceView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_SOURCE_RESET, &AudioSourceView::OnBnClickedButtonAudioSourceReset)
	ON_BN_CLICKED(IDC_CHECK_MUTE, &AudioSourceView::OnBnClickedCheckMute)
	ON_BN_CLICKED(IDC_CHECK_LOOP, &AudioSourceView::OnBnClickedCheckLoop)
	ON_BN_CLICKED(IDC_CHECK_PLAY_ON_AWAKE, &AudioSourceView::OnBnClickedCheckPlayOnAwake)
	ON_EN_CHANGE(IDC_EDIT_VOLUME, &AudioSourceView::OnEnChangeEditVolume)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &AudioSourceView::OnBnClickedCheckComponentActive)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_AUDIO_SOURCE, &AudioSourceView::OnBnClickedButtonLoadAudioSource)
	ON_BN_CLICKED(IDC_CHECK_2D_3D_CHECK, &AudioSourceView::OnBnClickedCheck2d3dCheck)
	ON_BN_CLICKED(IDC_PLAY_AUDIO_SOURCE, &AudioSourceView::OnBnClickedPlayAudioSource)
END_MESSAGE_MAP()


// AudioSourceView 진단

#ifdef _DEBUG
void AudioSourceView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AudioSourceView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void AudioSourceView::ResetAudioSourceView()
{
	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();
	if (_NowGameObject)
	{
		hos::com::AudioSource* source = _NowGameObject->GetComponent<hos::com::AudioSource>();
		if (source)
		{
			source->Reset();
		}
		SetAudioSourceView();
	}
}

void AudioSourceView::SetAudioSourceView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	_tempString.Format(L"%f", _NowGameObject->GetComponent<hos::com::AudioSource>()->GetVolume());
	EditVolume.SetWindowTextW(_tempString);

	CheckMute.SetCheck(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetMute());
	CheckLoop.SetCheck(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetLoop());
	CheckPlayOnAwake.SetCheck(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetPlayOnAwake());

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetActive());


	_tempString = L"Clip Name : ";

	if (_NowGameObject->GetComponent<hos::com::AudioSource>()->GetAudioClip())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetAudioClip()->GetName().c_str());
	}
	StaticAudioSourceName.SetWindowTextW(_tempString);

	while (ListAudioSourceList.GetCount() != 0)
	{
		ListAudioSourceList.DeleteString(0);
	}

	for (auto [name, data] : EditorManager::GetIns()->GetDataManager()->AudioClips)
	{
		ListAudioSourceList.AddString(name.c_str());
	}

	Check2D3DCheck.SetCheck(_NowGameObject->GetComponent<hos::com::AudioSource>()->GetIs3D());
	if (_NowGameObject->GetComponent<hos::com::AudioSource>()->GetAudioClip())
	{
		PlayBtn.EnableWindow(TRUE);
		if (_NowGameObject->GetComponent<hos::com::AudioSource>()->GetState() != DirectX::SoundState::PLAYING)
		{
			PlayBtn.SetWindowTextW(L"Play");
		}
		else
		{
			PlayBtn.SetWindowTextW(L"Stop");
		}
	}
	else
	{
		PlayBtn.EnableWindow(FALSE);
	}
}

// AudioSourceView 메시지 처리기


BOOL AudioSourceView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void AudioSourceView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void AudioSourceView::OnBnClickedButtonAudioSourceReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResetAudioSourceView();
}


void AudioSourceView::OnBnClickedCheckMute()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetMute(CheckMute.GetCheck());
}


void AudioSourceView::OnBnClickedCheckLoop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetLoop(CheckLoop.GetCheck());
}


void AudioSourceView::OnBnClickedCheckPlayOnAwake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetPlayOnAwake(CheckPlayOnAwake.GetCheck());
}


void AudioSourceView::OnEnChangeEditVolume()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditVolume.GetWindowTextLengthW();

	if (_Length > 0)
	{
		if (EditorManager::GetIns()->GetNowGameObject())
		{
			hos::Vector3 _temp;

			CString _tempString;
			EditVolume.GetWindowTextW(_tempString);

			EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetVolume(_tstof(_tempString));
		}
	}
}


void AudioSourceView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetActive(CheckComponentActive.GetCheck());
}


void AudioSourceView::OnBnClickedButtonLoadAudioSource()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListAudioSourceList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _ClipName;
	ListAudioSourceList.GetText(ListAudioSourceList.GetCurSel(), _ClipName);

	_NowGameObject->GetComponent<hos::com::AudioSource>()->SetAudioClip(T2W(_ClipName.GetBuffer()));

	SetAudioSourceView();


	/*
	static TCHAR BASED_CODE szFilter[] = _T("*.wav, *.WAV | *.wav; *.WAV |");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(true, L"*.wav", L"", OFN_HIDEREADONLY, szFilter);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();

		// 매니저에 경로를 보낸다.
		EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetAudioClip(T2W(_Path.GetBuffer()));

		SetAudioSourceView();

	}
	*/
}


void AudioSourceView::OnBnClickedCheck2d3dCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>()->SetIs3D(Check2D3DCheck.GetCheck());
}


void AudioSourceView::OnBnClickedPlayAudioSource()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	hos::com::AudioSource* sound = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::AudioSource>();
	if (sound)
	{
		if (sound->GetState() != DirectX::SoundState::PLAYING)
		{
			PlayBtn.SetWindowTextW(L"Stop");
			sound->Play();
		}
		else
		{
			PlayBtn.SetWindowTextW(L"Play");
			sound->Stop();
		}
	}
}
