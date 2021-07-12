
// hosEditorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "hosEditor.h"
#include "SceneView.h"
#include "TransformView.h"
#include "BoxCollisionview.h"
#include "SphereCollisionView.h"
#include "CapsuleCollisionView.h"
#include "CameraView.h"
#include "RigidbodyView.h"
#include "AudioListenerView.h"
#include "AudioSourceView.h"
#include "ScriptView.h"
#include "LIghtView.h"
#include "MeshFilterView.h"
#include "MeshRendererView.h"
#include "SkinnedMeshRendererView.h"
#include "AnimationView.h"
#include "UIImageView.h"
#include "UITextView.h"
#include "UIButtonView.h"
#include "UIInputFieldView.h"
#include "SceneInfoView.h"
#include "NavInfoView.h"
#include "hosEditorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// ChosEditorDlg 대화 상자



ChosEditorDlg::ChosEditorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HOSEditor_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChosEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_GAME_OBJECT_HIERARCHY, GameObjectHierachy);
	DDX_Control(pDX, IDC_EDIT_GAME_OBJECT_NAME, GameObjectName);
	DDX_Control(pDX, IDC_LIST_COMPONENT_LIST, ComponentList);
	DDX_Control(pDX, IDC_COMBO_COMPONENT_LIST, ComponentSelectList);
	DDX_Control(pDX, IDC_EDIT_SCENE_NAME, EditSceneName);
	DDX_Control(pDX, IDC_LIST_PREFAB_LIST, ListPrefabList);
}

BEGIN_MESSAGE_MAP(ChosEditorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CREATE_GAME_OBJECT, &ChosEditorDlg::OnBnClickedButtonCreateGameObject)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_GAME_OBJECT_NAME, &ChosEditorDlg::OnBnClickedButtonChangeGameObjectName)
	//	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GAME_OBJECT_HIERARCHY, &ChosEditorDlg::OnTvnSelchangedTreeGameObjectHierarchy)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_GAME_OBJECT, &ChosEditorDlg::OnBnClickedButtonRemoveGameObject)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_GAME_OBJECT_HIERARCHY, &ChosEditorDlg::OnNMRClickTreeGameObjectHierarchy)
	ON_BN_CLICKED(IDC_BUTTON_ADD_COMPONENT, &ChosEditorDlg::OnBnClickedButtonAddComponent)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_COMPONENT, &ChosEditorDlg::OnBnClickedButtonRemoveComponent)
	ON_LBN_SELCHANGE(IDC_LIST_COMPONENT_LIST, &ChosEditorDlg::OnLbnSelchangeListComponentList)
	ON_EN_CHANGE(IDC_EDIT_SCENE_NAME, &ChosEditorDlg::OnEnChangeEditSceneName)
	ON_BN_CLICKED(IDC_BUTTON_SCENE_SAVE, &ChosEditorDlg::OnBnClickedButtonSceneSave)
	ON_BN_CLICKED(IDC_BUTTON_SCENE_LOAD, &ChosEditorDlg::OnBnClickedButtonSceneLoad)
	ON_BN_CLICKED(IDC_BUTTON_GAME_OBJECT_SAVE, &ChosEditorDlg::OnBnClickedButtonGameObjectSave)
	ON_BN_CLICKED(IDC_BUTTON_GAME_OBJECT_LOAD, &ChosEditorDlg::OnBnClickedButtonGameObjectLoad)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_GAME_OBJECT_HIERARCHY, &ChosEditorDlg::OnTvnBegindragTreeGameObjectHierarchy)
	ON_BN_CLICKED(IDC_BUTTON_ADD_GAME_DATA, &ChosEditorDlg::OnBnClickedButtonAddGameData)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_GAME_DATA, &ChosEditorDlg::OnBnClickedButtonLoadGameData)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_GAME_OBJECT_HIERARCHY, &ChosEditorDlg::OnNMDblclkTreeGameObjectHierarchy)
	ON_BN_CLICKED(IDC_BUTTON_SCENE_INFO, &ChosEditorDlg::OnBnClickedButtonSceneInfo)
	ON_BN_CLICKED(IDC_BUTTON_NAV_INFO, &ChosEditorDlg::OnBnClickedButtonNavInfo)
END_MESSAGE_MAP()


// ChosEditorDlg 메시지 처리기

BOOL ChosEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	/// 다이얼로그 만들어야 함
	m_SceneView = new SceneView();
	m_SceneView->Create(IDD_SceneView, CWnd::GetDesktopWindow());

	RECT Size;
	Size.left = 0;
	Size.top = 0;
	Size.right = 1920;
	Size.bottom = 1080;
	AdjustWindowRect(&Size, WS_OVERLAPPEDWINDOW, false);
	m_SceneView->SetWindowPos(NULL, 0, 0, Size.right - Size.left, Size.bottom - Size.top, SWP_NOMOVE | SWP_NOZORDER);
	m_SceneView->ShowWindow(SW_SHOW);


	EditorManager::GetIns()->Initialize(m_SceneView->GetSafeHwnd());


	ComponentSelectList.AddString(L"Camera");
	ComponentSelectList.AddString(L"BoxCollision");
	ComponentSelectList.AddString(L"SphereCollision");
	ComponentSelectList.AddString(L"CapsuleCollision");
	ComponentSelectList.AddString(L"Rigidbody");
	ComponentSelectList.AddString(L"AudioListener");
	ComponentSelectList.AddString(L"AudioSource");
	ComponentSelectList.AddString(L"Script");
	ComponentSelectList.AddString(L"Light");
	ComponentSelectList.AddString(L"MeshFilter");
	ComponentSelectList.AddString(L"MeshRenderer");
	ComponentSelectList.AddString(L"SkinnedMeshRenderer");
	ComponentSelectList.AddString(L"Animation");
	ComponentSelectList.AddString(L"NavAgent");
	ComponentSelectList.AddString(L"Networkcomponent");
	ComponentSelectList.AddString(L"UIImage");
	ComponentSelectList.AddString(L"UIText");
	ComponentSelectList.AddString(L"UIButton");
	ComponentSelectList.AddString(L"UIInputField");

	EditSceneName.SetWindowTextW(EditorManager::GetIns()->GetNowScene()->GetName().c_str());

	GameObjectHierachy.InsertItem(L"MainCamera");
	GameObjectHierachy.InsertItem(L"DirectionalLight1");
	GameObjectHierachy.InsertItem(L"DirectionalLight2");
	GameObjectHierachy.InsertItem(L"DirectionalLight3");

	CCreateContext context;
	ZeroMemory(&context, sizeof(context));

	CRect _rect;

	GetDlgItem(IDC_STATIC_INSPECTOR_AREA)->GetWindowRect(&_rect);
	ScreenToClient(&_rect);
	m_TransformView = new TransformView();
	m_TransformView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_TransformView->OnInitialUpdate();
	m_TransformView->ShowWindow(SW_HIDE);

	m_BoxCollisionView = new BoxCollisionView();
	m_BoxCollisionView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_BoxCollisionView->OnInitialUpdate();
	m_BoxCollisionView->ShowWindow(SW_HIDE);

	m_SphereCollisionView = new SphereCollisionView();
	m_SphereCollisionView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_SphereCollisionView->OnInitialUpdate();
	m_SphereCollisionView->ShowWindow(SW_HIDE);

	m_CapsuleCollisionView = new CapsuleCollisionView();
	m_CapsuleCollisionView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_CapsuleCollisionView->OnInitialUpdate();
	m_CapsuleCollisionView->ShowWindow(SW_HIDE);

	m_CameraView = new CameraView();
	m_CameraView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_CameraView->OnInitialUpdate();
	m_CameraView->ShowWindow(SW_HIDE);

	m_RigidbodyView = new RigidbodyView();
	m_RigidbodyView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_RigidbodyView->OnInitialUpdate();
	m_RigidbodyView->ShowWindow(SW_HIDE);

	m_AudioListenerView = new AudioListenerView();
	m_AudioListenerView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_AudioListenerView->OnInitialUpdate();
	m_AudioListenerView->ShowWindow(SW_HIDE);

	m_AudioSourceView = new AudioSourceView();
	m_AudioSourceView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_AudioSourceView->OnInitialUpdate();
	m_AudioSourceView->ShowWindow(SW_HIDE);

	m_ScriptView = new ScriptView();
	m_ScriptView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_ScriptView->OnInitialUpdate();
	m_ScriptView->ShowWindow(SW_HIDE);

	m_LightView = new LIghtView();
	m_LightView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_LightView->OnInitialUpdate();
	m_LightView->ShowWindow(SW_HIDE);

	m_MeshFilterView = new MeshFilterView();
	m_MeshFilterView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_MeshFilterView->OnInitialUpdate();
	m_MeshFilterView->ShowWindow(SW_HIDE);

	m_MeshRendererView = new MeshRendererView();
	m_MeshRendererView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_MeshRendererView->OnInitialUpdate();
	m_MeshRendererView->ShowWindow(SW_HIDE);

	m_SkinnedMeshRendererView = new SkinnedMeshRendererView();
	m_SkinnedMeshRendererView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_SkinnedMeshRendererView->OnInitialUpdate();
	m_SkinnedMeshRendererView->ShowWindow(SW_HIDE);

	m_AnimationView = new AnimationView();
	m_AnimationView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_AnimationView->OnInitialUpdate();
	m_AnimationView->ShowWindow(SW_HIDE);

	m_UIImageView = new UIImageView();
	m_UIImageView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_UIImageView->OnInitialUpdate();
	m_UIImageView->ShowWindow(SW_HIDE);

	m_UITextView = new UITextView();
	m_UITextView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_UITextView->OnInitialUpdate();
	m_UITextView->ShowWindow(SW_HIDE);

	m_UIButtonView = new UIButtonView();
	m_UIButtonView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_UIButtonView->OnInitialUpdate();
	m_UIButtonView->ShowWindow(SW_HIDE);

	m_UIInputFieldView = new UIInputFieldView();
	m_UIInputFieldView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_UIInputFieldView->OnInitialUpdate();
	m_UIInputFieldView->ShowWindow(SW_HIDE);

	m_SceneInfoView = new SceneInfoView();
	m_SceneInfoView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_SceneInfoView->OnInitialUpdate();
	m_SceneInfoView->ShowWindow(SW_HIDE);

	m_NavInfoView = new NavInfoView();
	m_NavInfoView->Create(NULL, NULL, WS_CHILD, _rect, this, IDD_HOSEditor_DIALOG, &context);
	m_NavInfoView->OnInitialUpdate();
	m_NavInfoView->ShowWindow(SW_HIDE);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void ChosEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void ChosEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR ChosEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void ChosEditorDlg::ResetComponentList(CString* gameObject)
{
	while (ComponentList.GetCount() != 0)
	{
		ComponentList.DeleteString(0);
	}

	if (gameObject)
	{
		hos::com::GameObject* obj = EditorManager::GetIns()->GetNowScene()->FindGameObjectWithName(T2W(gameObject->GetBuffer()));
		if (obj)
		{
			int _ComponentCount = obj->GetComponents().size();

			for (int i = 0; i < _ComponentCount; i++)
			{
				ComponentList.AddString(obj->GetComponents()[i]->GetName().c_str());
			}
		}
	}
}

void ChosEditorDlg::HideAllComponentView()
{
	m_TransformView->ShowWindow(SW_HIDE);
	m_BoxCollisionView->ShowWindow(SW_HIDE);
	m_SphereCollisionView->ShowWindow(SW_HIDE);
	m_CapsuleCollisionView->ShowWindow(SW_HIDE);
	m_CameraView->ShowWindow(SW_HIDE);
	m_RigidbodyView->ShowWindow(SW_HIDE);
	m_AudioListenerView->ShowWindow(SW_HIDE);
	m_AudioSourceView->ShowWindow(SW_HIDE);
	m_ScriptView->ShowWindow(SW_HIDE);
	m_LightView->ShowWindow(SW_HIDE);
	m_MeshFilterView->ShowWindow(SW_HIDE);
	m_MeshRendererView->ShowWindow(SW_HIDE);
	m_SkinnedMeshRendererView->ShowWindow(SW_HIDE);
	m_AnimationView->ShowWindow(SW_HIDE);
	m_UIImageView->ShowWindow(SW_HIDE);
	m_UITextView->ShowWindow(SW_HIDE);
	m_UIButtonView->ShowWindow(SW_HIDE);
	m_UIInputFieldView->ShowWindow(SW_HIDE);
	m_SceneInfoView->ShowWindow(SW_HIDE);
	m_NavInfoView->ShowWindow(SW_HIDE);
}

void ChosEditorDlg::ResetAllComponentView()
{
	m_TransformView->ResetTransformView();
	m_BoxCollisionView->ResetBoxCollisionView();
	m_SphereCollisionView->ResetSphereCollisionView();
	m_CapsuleCollisionView->ResetCapsuleCollisionView();
	m_CameraView->ResetCameraView();
	m_RigidbodyView->ResetRigidbodyView();
	m_AudioListenerView->ResetAudioListenerView();
	m_AudioSourceView->ResetAudioSourceView();
	m_ScriptView->ResetScriptView();
	m_LightView->ResetLightView();
	m_MeshFilterView->ResetMeshFilterView();
	m_MeshRendererView->ResetMeshRendererView();
	m_SkinnedMeshRendererView->ResetSkinnedMeshRendererView();
	m_AnimationView->ResetAnimationView();
	m_UIImageView->ResetUIImageView();
	m_UITextView->ResetUITextView();
	m_UIButtonView->ResetUIButtonView();
	m_UIInputFieldView->ResetUIInputFieldView();
	m_SceneInfoView->ResetSceneInfoView();
	m_NavInfoView->ResetNavInfoView();
}

void ChosEditorDlg::UpdateGameObjectHierarchy()
{
	// 기존 계층구조 삭제
	GameObjectHierachy.DeleteAllItems();

	// 현재 씬의 오브젝트 계층구조를 가져와서 갱신시키기
	for (int i = 0; i < EditorManager::GetIns()->GetNowScene()->GetRoots().size(); i++)
	{
		// 루트 오브젝트 가져오기
		hos::com::GameObject* _GameObject = EditorManager::GetIns()->GetNowScene()->GetRoots()[i];

		// 루트 오브젝트 이름 계층구조에 넣기
		HTREEITEM _TreeItem = GameObjectHierachy.InsertItem(_GameObject->GetName().c_str());;


		// 자식 오브젝트 있는지 확인 후 계층구조에 추가 (반복)
		MakeGameObjectHierarchy(_GameObject, &_TreeItem);
	}

	HideAllComponentView();
	ResetComponentList(nullptr);
	hos::com::GameObject* _ObjectTemp = nullptr;
	EditorManager::GetIns()->SetNowGameObject(_ObjectTemp);

}

void ChosEditorDlg::MakeGameObjectHierarchy(hos::com::GameObject* gameObject, HTREEITEM* treeItem)
{
	int _ChildGameObjectCount = gameObject->GetComponent<hos::com::Transform>()->GetChildCount();

	if (_ChildGameObjectCount > 0)
	{
		for (int i = 0; i < _ChildGameObjectCount; i++)
		{
			hos::com::GameObject* _GameObject = gameObject->GetComponent<hos::com::Transform>()->GetChilds()[i]->m_GameObject;
			HTREEITEM _TreeItem = GameObjectHierachy.InsertItem(_GameObject->GetName().c_str(), *treeItem);

			MakeGameObjectHierarchy(_GameObject, &_TreeItem);
		}
	}

}

void ChosEditorDlg::OnBnClickedButtonCreateGameObject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	HideAllComponentView();

	//ResetAllComponentView();

	HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
	CString _temp;

	if (_TreeItem)
	{
		// 하위에 오브젝트 생성
		CString _ParentName = GameObjectHierachy.GetItemText(_TreeItem);
		_temp = EditorManager::GetIns()->CreateGameObject(&_ParentName);
		GameObjectHierachy.InsertItem(_temp, _TreeItem);
	}
	else
	{
		// 루트 오브젝트 생성
		_temp = EditorManager::GetIns()->CreateGameObject();
		//GameObjectHierachy.InsertItem(_temp, nullptr, nullptr);
		GameObjectHierachy.InsertItem(_temp);
	}

	// 선택 해제
	GameObjectHierachy.SelectItem(nullptr);

	Invalidate(TRUE);
}


void ChosEditorDlg::OnBnClickedButtonChangeGameObjectName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int _StringCount = GameObjectName.GetWindowTextLengthW();

	if (_StringCount <= 0)
	{
		return;
	}

	CString _ChangedName;
	GameObjectName.GetWindowTextW(_ChangedName);

	if (_ChangedName.GetLength() > 0)
	{
		HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
		if (_TreeItem != nullptr)
		{

			CString _temp = GameObjectHierachy.GetItemText(_TreeItem);

			_ChangedName = EditorManager::GetIns()->ChangeGameObjectName(&_temp, &_ChangedName);

			GameObjectHierachy.SetItemText(_TreeItem, _ChangedName);

			GameObjectHierachy.SelectItem(nullptr);

			Invalidate(TRUE);
		}
	}
}


void ChosEditorDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	EditorManager::GetIns()->Finalize();
	EditorManager::GetIns()->Destory();
	delete m_SceneView;
	m_SceneView = nullptr;
	delete m_TransformView;
	delete m_BoxCollisionView;
	delete m_SphereCollisionView;
	delete m_CapsuleCollisionView;
	delete m_CameraView;
	delete m_RigidbodyView;
	delete m_AudioListenerView;
	delete m_AudioSourceView;
	delete m_ScriptView;
	delete m_LightView;
	delete m_MeshFilterView;
	delete m_MeshRendererView;
	delete m_SkinnedMeshRendererView;
	delete m_AnimationView;
	delete m_UIImageView;
	delete m_UIButtonView;
	delete m_UIInputFieldView;
	delete m_SceneInfoView;
	delete m_NavInfoView;

	CDialogEx::OnClose();
}


void ChosEditorDlg::OnTvnSelchangedTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


	// 현재 선택한 GameObject Hierarchy Node 가져오기
	CString _temp = GameObjectHierachy.GetItemText(pNMTreeView->itemNew.hItem);
	GameObjectName.SetWindowTextW(_temp);

	ResetComponentList(nullptr);

	HideAllComponentView();

	hos::com::GameObject* _ObjectTemp = nullptr;

	EditorManager::GetIns()->SetNowGameObject(_ObjectTemp);

	if (_temp.GetLength() > 0)
	{
		// 컴포넌트 리스트 초기화
		// 선택한 오브젝트의 컴포넌트들을 가져와야 함
		ResetComponentList(&_temp);

		EditorManager::GetIns()->SetNowGameObject(&_temp);

	}

	ComponentList.SetCurSel(0);
	OnLbnSelchangeListComponentList();

	/// 테스트 해봄
	//SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP)
	//CFileFind finder;

	//bool IsFind = finder.FindFile(L"..\\Resource");

	//IsFind = finder.FindNextFileW();

	//CString temp = finder.GetFileTitle();

	//temp = finder.GetFilePath();

	//temp = finder.GetFileName();

	Invalidate(TRUE);
}


void ChosEditorDlg::OnBnClickedButtonRemoveGameObject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ResetComponentList(nullptr);

	HideAllComponentView();

	HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
	CString _temp = GameObjectHierachy.GetItemText(_TreeItem);

	if (_TreeItem)
	{
		EditorManager::GetIns()->RemoveGameObject(&_temp);
		GameObjectHierachy.DeleteItem(_TreeItem);

		GameObjectHierachy.SelectItem(nullptr);

		Invalidate(TRUE);
	}
}


void ChosEditorDlg::OnNMRClickTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	ResetComponentList(nullptr);

	GameObjectHierachy.SelectItem(nullptr);

	hos::com::GameObject* _temp = nullptr;

	EditorManager::GetIns()->SetNowGameObject(_temp);

	HideAllComponentView();

	ResetAllComponentView();

	Invalidate(TRUE);
}


void ChosEditorDlg::OnBnClickedButtonAddComponent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 선택한 오브젝트와 추가를 하고싶은 컴포넌트를 보내자
	HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
	CString _SelectedGameObject = GameObjectHierachy.GetItemText(_TreeItem);

	CString _ComponentName;
	ComponentSelectList.GetLBText(ComponentSelectList.GetCurSel(), _ComponentName);

	if (_SelectedGameObject.GetLength() > 0 && _ComponentName.GetLength() > 0)
	{
		EditorManager::GetIns()->AddComponent(&_SelectedGameObject, &_ComponentName, ComponentList.GetCount());

		// 컴포넌트 리스트 초기화 후 갱신
		ResetComponentList(&_SelectedGameObject);
	}

}


void ChosEditorDlg::OnBnClickedButtonRemoveComponent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Transform은 제거 안 됨
	HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
	CString _SelectedGameObject = GameObjectHierachy.GetItemText(_TreeItem);

	CString _ComponentName;
	ComponentList.GetText(ComponentList.GetCurSel(), _ComponentName);

	if (_SelectedGameObject.GetLength() > 0 && _ComponentName.GetLength() > 0)
	{
		EditorManager::GetIns()->RemoveComponent(&_SelectedGameObject, &_ComponentName, ComponentList.GetCurSel());

		// 컴포넌트 리스트 초기화 후 갱신
		ResetComponentList(&_SelectedGameObject);
		HideAllComponentView();
	}
}


void ChosEditorDlg::OnLbnSelchangeListComponentList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 선택한 것이 있으면 띄우자

	HideAllComponentView();

	//////////////////////////////////////////////////////////////////////////
	/// 해당하는 컴포넌트마다 띄울게 다르다..

	CString _ComponentName;
	if (ComponentList.GetCurSel() >= 0)
	{
		ComponentList.GetText(ComponentList.GetCurSel(), _ComponentName);
	}

	if (_ComponentName.Collate(L"Transform") == 0)
	{
		m_TransformView->SetTransformView();
		m_TransformView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"BoxCollision") == 0)
	{
		m_BoxCollisionView->SetBoxCollisionView();
		m_BoxCollisionView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"SphereCollision") == 0)
	{
		m_SphereCollisionView->SetSphereCollisionView();
		m_SphereCollisionView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"CapsuleCollision") == 0)
	{
		m_CapsuleCollisionView->SetCapsuleCollisionView();
		m_CapsuleCollisionView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"Rigidbody") == 0)
	{
		m_RigidbodyView->SetRigidbodyView();
		m_RigidbodyView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"Camera") == 0)
	{
		m_CameraView->SetCameraView();
		m_CameraView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"AudioListener") == 0)
	{
		m_AudioListenerView->SetAudioListenerView();
		m_AudioListenerView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"AudioSource") == 0)
	{
		m_AudioSourceView->SetAudioSourceView();
		m_AudioSourceView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"Script") == 0)
	{
		m_ScriptView->SetScriptView(ComponentList.GetCurSel());
		m_ScriptView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"Light") == 0)
	{
		m_LightView->SetLightView();
		m_LightView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"MeshFilter") == 0)
	{
		m_MeshFilterView->SetMeshFilterView();
		m_MeshFilterView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"MeshRenderer") == 0)
	{
		m_MeshRendererView->SetMeshRendererView();
		m_MeshRendererView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"SkinnedMeshRenderer") == 0)
	{
		m_SkinnedMeshRendererView->SetSkinnedMeshRendererView();
		m_SkinnedMeshRendererView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"Animation") == 0)
	{
		m_AnimationView->SetAnimationView();
		m_AnimationView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"UIImage") == 0)
	{
		m_UIImageView->SetUIImageView();
		m_UIImageView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"UIText") == 0)
	{
		m_UITextView->SetUITextView();
		m_UITextView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"UIButton") == 0)
	{
		m_UIButtonView->SetUIButtonView();
		m_UIButtonView->ShowWindow(SW_SHOW);
	}
	else if (_ComponentName.Collate(L"UIInputField") == 0)
	{
		m_UIInputFieldView->SetUIInputFieldView();
		m_UIInputFieldView->ShowWindow(SW_SHOW);
	}
	else
	{
		HideAllComponentView();
	}
}




void ChosEditorDlg::OnEnChangeEditSceneName()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int _Length = EditSceneName.GetWindowTextLengthW();

	if (_Length > 0)
	{
		CString _tempString;
		EditSceneName.GetWindowTextW(_tempString);

		EditorManager::GetIns()->GetNowScene()->SetName(T2W(_tempString.GetBuffer()));

	}

}

void ChosEditorDlg::OnBnClickedButtonSceneSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//static TCHAR BASED_CODE szFilter[] = _T("*.*||");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(false, L"", L"저장 할 위치나 씬 선택", OFN_HIDEREADONLY);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();
		CString _File = _PathDialog.GetFileName();

		// 씬이 있다면
		int _ScenesInPath = _Path.Find(L"Scenes");

		int _AssetsInPath = _Path.Find(L"Assets");

		if (_AssetsInPath > 0 && _ScenesInPath > 0)
		{
			if (_AssetsInPath < _ScenesInPath)
			{
				CString _Assets;

				_Assets = _Path.Left(_AssetsInPath);

				_Assets.Append(L"Assets");

				_Path = _Assets;
			}


		}
		else
		{
			// 끝에 씬 이름 빼기
			_Path.TrimRight(_File);
			_Path.TrimRight(L"\\");
		}

		//////////////////////////////////////////////////////////////////////////
		// Assets 폴더에 생성하는지 확인하기
		// Assets 폴더가 아니라면 해당 경로에 Assets 폴더 생성

		hos::string _FindAssets;
		hos::string _IsAssets;

		_FindAssets = T2W(_Path.GetBuffer());

		_IsAssets = _FindAssets.substr(_FindAssets.find_last_of(L"\\") + 1);

		if (_IsAssets.compare(L"Assets") == 0)
		{
			_Path.Append(L"\\");
		}
		else
		{
			_Path.Append(L"\\Assets\\");

			CreateDirectory(_Path, nullptr);

		}

		//////////////////////////////////////////////////////////////////////////
		// 매니저에 경로를 보낸다.
		EditorManager::GetIns()->SaveScene(&_Path);

		// 데이터 매니저 내부의 데이터들을 해당 폴더에 저장하기
		EditorManager::GetIns()->SaveDataManager(&_Path);

		MessageBox(L"Save Complete!!!", L"Save", MB_OK);
	}
}


void ChosEditorDlg::OnBnClickedButtonSceneLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("*.scene, *.scene | *.scene; *.scene |");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(true, L"*.scene", L"", OFN_HIDEREADONLY, szFilter);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();

		// 매니저에 경로를 보낸다.
		bool b = EditorManager::GetIns()->LoadScene(&_Path);

		if (b)
		{
			// 씬 이름 변경하기
			EditSceneName.SetWindowTextW(EditorManager::GetIns()->GetNowScene()->GetName().c_str());

			// Object Hierarchy 갱신해야 함.. 얽
			UpdateGameObjectHierarchy();

		}

	}
}


void ChosEditorDlg::OnBnClickedButtonGameObjectSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 프리팹을 저장한다.

	// 오브젝트 계층구조에서 선택된 오브젝트가 있는가
	if (GameObjectHierachy.GetSelectedItem())
	{
		// 경로 가져와서 저장하기
		static TCHAR BASED_CODE szFilter[] = _T("*.prefab, *.Prefab | *.prefab; *.Prefab |");// 모든파일(*.*) | *.* || ");

		// 경로 불러오기
		CFileDialog _PathDialog(false, L"*.prefab", L"이 이름으로 저장 안 됨", OFN_HIDEREADONLY, szFilter);

		if (IDOK == _PathDialog.DoModal())
		{
			CString _Path = _PathDialog.GetPathName();
			CString _File = _PathDialog.GetFileName();
			// 끝에 오브젝트 이름 빼기
			_Path.TrimRight(_File);

			HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
			CString _temp;

			_temp = GameObjectHierachy.GetItemText(_TreeItem);

			// 경로와 오브젝트의 이름을 보낸다.
			EditorManager::GetIns()->SavePrefab(&_Path, &_temp);

		}
	}
}


void ChosEditorDlg::OnBnClickedButtonGameObjectLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("*.prefab, *.Prefab | *.prefab; *.Prefab |");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(true, L"*.prefab", L"", OFN_HIDEREADONLY, szFilter);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();

		bool b;

		// 오브젝트 계층구조에서 선택된 오브젝트가 있는다.
		if (GameObjectHierachy.GetSelectedItem())
		{
			HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
			CString _temp;

			_temp = GameObjectHierachy.GetItemText(_TreeItem);

			b = EditorManager::GetIns()->LoadPrefab(&_Path, &_temp);
		}
		else
		{
			// 없으면 루트 오브젝트로 넣는다.
			b = EditorManager::GetIns()->LoadPrefab(&_Path, nullptr);
		}

		if (b)
		{
			// Object Hierarchy 갱신해야 함.. 얽
			UpdateGameObjectHierarchy();
		}

	}
}


void ChosEditorDlg::OnTvnBegindragTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void ChosEditorDlg::OnBnClickedButtonAddGameData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//static TCHAR BASED_CODE szFilter[] = _T("*.prefab, *.Prefab | *.prefab; *.Prefab |");// 모든파일(*.*) | *.* || ");

	// 경로 불러오기
	CFileDialog _PathDialog(true, L"", L"", OFN_HIDEREADONLY);

	if (IDOK == _PathDialog.DoModal())
	{
		CString _Path = _PathDialog.GetPathName();

		EditorManager::GetIns()->LoadGameData(&_Path);

		UpdateGameObjectHierarchy();

		// 프리팹 리스트를 업데이트 한다.
		while (ListPrefabList.GetCount() != 0)
		{
			ListPrefabList.DeleteString(0);
		}

		for (auto [name, data] : EditorManager::GetIns()->GetDataManager()->Prefabs)
		{
			ListPrefabList.AddString(name.c_str());
		}
	}
}


void ChosEditorDlg::OnBnClickedButtonLoadGameData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 선택한 프리팹을 오브젝트 하이어라키에 불러오자
	CString _PrefabName;
	ListPrefabList.GetText(ListPrefabList.GetCurSel(), _PrefabName);

	// 오브젝트 하이어라키에서 선택한 오브젝트가 있는 경우
	HideAllComponentView();
	ResetComponentList(nullptr);

	HTREEITEM _TreeItem = GameObjectHierachy.GetSelectedItem();
	CString _temp;

	if (_TreeItem)
	{
		// 하위에 오브젝트 생성
		CString _ParentName = GameObjectHierachy.GetItemText(_TreeItem);
		_temp = EditorManager::GetIns()->AddPrefab(&_PrefabName, &_ParentName);
		GameObjectHierachy.InsertItem(_temp, _TreeItem);
	}
	else
	{
		// 루트 오브젝트 생성
		_temp = EditorManager::GetIns()->AddPrefab(&_PrefabName);
		GameObjectHierachy.InsertItem(_temp);
	}

	// 선택 해제
	GameObjectHierachy.SelectItem(nullptr);

	UpdateGameObjectHierarchy();

	Invalidate(TRUE);
}


BOOL ChosEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}

void ChosEditorDlg::ExpandAll(HTREEITEM hUserRoot)
{
	if (GameObjectHierachy.ItemHasChildren(hUserRoot))
	{
		// 확장  
		GameObjectHierachy.Expand(hUserRoot, TVE_EXPAND);
		// 자식 노드 확인  
		hUserRoot = GameObjectHierachy.GetChildItem(hUserRoot);
		if (hUserRoot)
		{
			do
			{
				// 재귀로 하위 확인  
				ExpandAll(hUserRoot);
			} while ((hUserRoot = GameObjectHierachy.GetNextSiblingItem(hUserRoot)) != NULL);
		}
	}
}

void ChosEditorDlg::CollapseAll(HTREEITEM hUserRoot)
{
	if (GameObjectHierachy.ItemHasChildren(hUserRoot))
	{
		// 확장  
		GameObjectHierachy.Expand(hUserRoot, TVE_COLLAPSE);
		// 자식 노드 확인  
		hUserRoot = GameObjectHierachy.GetChildItem(hUserRoot);
		if (hUserRoot)
		{
			do
			{
				// 재귀로 하위 확인  
				ExpandAll(hUserRoot);
			} while ((hUserRoot = GameObjectHierachy.GetNextSiblingItem(hUserRoot)) != NULL);
		}
	}
}

void ChosEditorDlg::OnNMDblclkTreeGameObjectHierarchy(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hSelectItem = GameObjectHierachy.GetSelectedItem();
	static bool bClose = true;
	if (bClose)
	{
		ExpandAll(hSelectItem);
	}
	else
	{
		CollapseAll(hSelectItem);
	}
	bClose = !bClose;

	*pResult = 1;
}


void ChosEditorDlg::OnBnClickedButtonSceneInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HideAllComponentView();
	m_SceneInfoView->SetSceneInfoView();
	m_SceneInfoView->ShowWindow(SW_SHOW);

}


void ChosEditorDlg::OnBnClickedButtonNavInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HideAllComponentView();

	m_NavInfoView->SetNavInfoView();
	m_NavInfoView->ShowWindow(SW_SHOW);

}
