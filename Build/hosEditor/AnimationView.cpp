// AnimationView.cpp: 구현 파일
//

#include "pch.h"
#include "hosEditor.h"
#include "AnimationView.h"


// AnimationView

IMPLEMENT_DYNCREATE(AnimationView, CFormView)

AnimationView::AnimationView()
	: CFormView(IDD_AnimationView)
{

}

AnimationView::~AnimationView()
{
}

void AnimationView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ANIMATION_BUNDLE, StaticBundleName);
	DDX_Control(pDX, IDC_STATIC_ANIMATION_CLIP, StaticAnimationClip);
	DDX_Control(pDX, IDC_LIST_BUNDLE_LIST, ListBundleList);
	DDX_Control(pDX, IDC_LIST_CLIP_LIST, ListClipList);
	DDX_Control(pDX, IDC_STATIC_NOW_TICK, StaticNowTick);
	DDX_Control(pDX, IDC_CHECK_ANIMATION_PLAY, CheckAnimationPlay);
	DDX_Control(pDX, IDC_CHECK_COMPONENT_ACTIVE, CheckComponentActive);
	DDX_Control(pDX, IDC_STATIC_END_TICK, StaticEndTick);
}

BEGIN_MESSAGE_MAP(AnimationView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CLIP, &AnimationView::OnBnClickedButtonLoadClip)
	ON_LBN_SELCHANGE(IDC_LIST_BUNDLE_LIST, &AnimationView::OnLbnSelchangeListBundleList)
	ON_BN_CLICKED(IDC_CHECK_ANIMATION_PLAY, &AnimationView::OnBnClickedCheckAnimationPlay)
	ON_BN_CLICKED(IDC_CHECK_COMPONENT_ACTIVE, &AnimationView::OnBnClickedCheckComponentActive)
END_MESSAGE_MAP()


// AnimationView 진단

#ifdef _DEBUG
void AnimationView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AnimationView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void AnimationView::ResetAnimationView()
{

}

void AnimationView::SetAnimationView()
{
	if (!EditorManager::GetIns()->GetNowGameObject())
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _tempString;

	// 애니메이션 번들 이름
	_tempString = L"Bundle Name : ";

	if (_NowGameObject->GetComponent<hos::com::Animation>()->GetAniBundle())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::Animation>()->GetAniBundleName().c_str());
	}

	StaticBundleName.SetWindowTextW(_tempString);

	// 애니메이션 클립 이름
	_tempString = L"Clip Name : ";

	if (_NowGameObject->GetComponent<hos::com::Animation>()->GetAniClip())
	{
		_tempString.Append(_NowGameObject->GetComponent<hos::com::Animation>()->GetAniClipName().c_str());
	}

	StaticAnimationClip.SetWindowTextW(_tempString);

	// 현재 애니메이션의 시간 (Tick)
	_tempString = L"Now Tick : ";
	_tempString.Format(L"Now Tick : %f", _NowGameObject->GetComponent<hos::com::Animation>()->GetNowTick());

	StaticNowTick.SetWindowTextW(_tempString);

	// 애니메이션의  끝 시간 (Tick)
	_tempString = L"End Tick : ";
	_tempString.Format(L"End Tick : %f", _NowGameObject->GetComponent<hos::com::Animation>()->GetLastTick());

	StaticEndTick.SetWindowTextW(_tempString);

	CheckAnimationPlay.SetCheck(_NowGameObject->GetComponent<hos::com::Animation>()->GetPlayState());

	// 데이터매니저에서 번들과 클립 리스트 가져오기

	while (ListBundleList.GetCount() != 0)
	{
		ListBundleList.DeleteString(0);
	}

	for (auto [name, data] : EditorManager::GetIns()->GetDataManager()->AnimationList)
	{
		ListBundleList.AddString(name.c_str());
	}

	while (ListClipList.GetCount() != 0)
	{
		ListClipList.DeleteString(0);
	}

	CheckComponentActive.SetCheck(_NowGameObject->GetComponent<hos::com::Animation>()->GetActive());

}

// AnimationView 메시지 처리기


BOOL AnimationView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void AnimationView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void AnimationView::OnBnClickedButtonLoadClip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListBundleList.GetCurSel() == -1)
	{
		return;
	}

	if (ListClipList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	CString _Bundle;
	ListBundleList.GetText(ListBundleList.GetCurSel(), _Bundle);

	CString _Clip;
	ListClipList.GetText(ListClipList.GetCurSel(), _Clip);

	_NowGameObject->GetComponent<hos::com::Animation>()->SetAnimation(T2W(_Bundle.GetBuffer()), T2W(_Clip.GetBuffer()));

	SetAnimationView();

}


void AnimationView::OnLbnSelchangeListBundleList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (ListBundleList.GetCurSel() == -1)
	{
		return;
	}

	hos::com::GameObject* _NowGameObject = EditorManager::GetIns()->GetNowGameObject();

	// 번들 리스트에서 선택하는 번들에 따라 클립 리스트가 변함
	while (ListClipList.GetCount() != 0)
	{
		ListClipList.DeleteString(0);
	}

	hos::com::Animation* anicom = _NowGameObject->GetComponent<hos::com::Animation>();
	if (anicom)
	{
		hos::AnimationBundle* bundle = anicom->GetAniBundle();
		if (bundle)
		{
			for (auto iter : bundle->GetAnimationClipName())
			{
				ListClipList.AddString(iter.c_str());
			}
		}
	}
}


void AnimationView::OnBnClickedCheckAnimationPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	bool b = EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Animation>()->GetPlayState();

	b = !b;

	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Animation>()->SetPlayState(b);

	CheckAnimationPlay.SetCheck(b);

}


void AnimationView::OnBnClickedCheckComponentActive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EditorManager::GetIns()->GetNowGameObject()->GetComponent<hos::com::Animation>()->SetActive(CheckComponentActive.GetCheck());
}
