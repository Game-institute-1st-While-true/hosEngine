#include "pch.h"
#include "EditorManager.h"
//#include <thread>

using namespace hos;
using namespace com;

int mousePosX = 0;
int mousePosY = 0;
bool bLBTDOWN = false;
bool bStaticClicked = false;

EditorManager::EditorManager()
{
	bDebug = true;
}

EditorManager::~EditorManager()
{

}

void EditorManager::Initialize(HWND hwnd)
{
	/////////////////////////////////////////////////////////////////////////////////////////

	Time = new hos::ut::GameTimer();
	bClient = false;

	/////////////////////////////////////////////////////////////////////////////////////////

	hos::cg::RendererDesc temp;
	temp.Driver = hos::cg::RendererDesc::DriverType::DRIVER_DIRECT3D11;
	temp.ScreenWidth = 1920;
	temp.ScreenHeight = 1080;
	temp.Fullscreen = false;
	temp.Hwnd = hwnd;

	g_Renderer = hos::cg::Renderer::CreateRenderer(temp);
	g_Renderer->SetViewFrustum(false);
	g_DataManager = new hos::com::DataManager();

	/////////////////////////////////////////////////////////////////////////////////////////

	g_Px = new hos::PhysX();

	/////////////////////////////////////////////////////////////////////////////////////////

	//										3D Sound환경리버브 효과 활성화				|	3D Sound환경리버브 추가기능				|	xAPO 활성화
	DirectX::AUDIO_ENGINE_FLAGS audioFlag = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters | DirectX::AudioEngine_UseMasteringLimiter;

#if defined(_DEBUG)|defined(DEBUG)
	audioFlag |= DirectX::AudioEngine_Debug;
#endif // defined(_DEBUG)|defined(DEBUG)

	g_Audio = new DirectX::AudioEngine(audioFlag);

	/////////////////////////////////////////////////////////////////////////////////////////

	g_Navigation = new hos::nv::Navigation();

	/////////////////////////////////////////////////////////////////////////////////////////

	NowScene = new hos::com::Scene();
	firstScene = NowScene;
	NowScene->SetName(L"NewScene");

	/////////////////////////////////////////////////////////////////////////////////////////

	GameObject* testCamera = new GameObject(L"MainCamera");
	Camera* cam = new Camera(Camera::eMode::DEFAULT);
	testCamera->AddComponent(cam);
	testCamera->GetComponent<Transform>()->SetLocalPosition(Vector3(0, 10, -20));
	testCamera->GetComponent<Transform>()->SetLocalRotation(hos::Rotator(20, 0, 0));
	AudioListener* al = new AudioListener();
	testCamera->AddComponent(al);
	NowScene->AddGameObject(testCamera);

	GameObject* testLight1 = new GameObject(L"DirectionalLight1");
	Light* light1 = new Light();
	testLight1->AddComponent(light1);
	light1->m_GameObject->transform->SetRotation(Rotator(45, 0, 0));
	light1->SetColor(hos::Color(0.8f, 0.8f, 0.8f, 1.0f));
	NowScene->AddGameObject(testLight1);

	GameObject* testLight2 = new GameObject(L"DirectionalLight2");
	Light* light2 = new Light();
	testLight2->AddComponent(light2);
	light2->m_GameObject->transform->SetRotation(Rotator(-15, 0, 0));
	light2->SetColor(hos::Color(0.3f, 0.3f, 0.3f, 1.0f));
	NowScene->AddGameObject(testLight2);

	GameObject* testLight3 = new GameObject(L"DirectionalLight3");
	Light* light3 = new Light();
	testLight3->AddComponent(light3);
	light3->m_GameObject->transform->SetRotation(Rotator(45, 135, 0));
	light3->SetColor(hos::Color(0.5f, 0.5f, 0.5f, 1.0f));
	NowScene->AddGameObject(testLight3);

	NowScene->Awake();

	/////////////////////////////////////////////////////////////////////////////////////////

	//NowScene->Start();

	/*
	ComSystem = new hos::com::COM_System(hwnd);

	NowScene = new hos::com::Scene();
	NowScene->SetName(L"NewScene");

	//GameObject* player = new GameObject();
	//BoxCollision* bc = new BoxCollision();
	////PlayerMove* pm = new PlayerMove();
	//player->AddComponent(bc);
	////player->AddComponent(pm);
	////player->RemoveComponent(pm);
	//NowScene->AddGameObject(player);

	GameObject* testCamera = new GameObject(L"MainCamera");
	Camera* cam = new Camera(1600, 900);
	testCamera->GetComponent<Transform>()->SetLocalPosition(Vector3(0, 10, -20));
	testCamera->GetComponent<Transform>()->SetLocalRotation(Vector3(0.5, 0, 0));
	testCamera->AddComponent(cam);
	NowScene->AddGameObject(testCamera);
	////CameraMove* cm = new CameraMove();
	//testCamera->AddComponent(cam);
	////testCamera->AddComponent(cm);
	//testCamera->GetComponent<Transform>()->SetPosition(Vector3(0, 10, -20));
	//NowScene->AddGameObject(testCamera);

	ComSystem->Manager->AddScene(NowScene);
	*/
}

void EditorManager::Initialize()
{
	NowScene = new hos::com::Scene();
	firstScene = NowScene;
	NowScene->SetName(L"NewScene");
}

void EditorManager::Finalize()
{
	delete firstScene;
	SafeDelete(g_Audio);
	SafeDelete(g_Navigation);
	SafeDelete(g_DataManager);
	SafeDelete(g_Renderer);
	SafeDelete(g_Px);
	SafeDelete(Time);
	Debug->Destory();
}

CString EditorManager::CreateGameObject(CString* gameObjectParent)
{
	GameObject* _NewGameObject = new GameObject(L"GameObject");

	if (!gameObjectParent)
	{
		NowScene->AddGameObject(_NewGameObject);
	}
	else
	{
		// 부모 찾기
		NowScene->AddGameObject(_NewGameObject, NowScene->FindGameObjectWithName(T2W(gameObjectParent->GetBuffer())));
	}

	return _NewGameObject->GetName().c_str();

}

void EditorManager::RemoveGameObject(CString* gameObjectName)
{
	NowScene->RemoveGameObject(NowScene->FindGameObjectWithName(T2W(gameObjectName->GetBuffer())));
}

CString EditorManager::ChangeGameObjectName(CString* gameObject, CString* changeName)
{
	GameObject* _ChangeGameObjectName;

	_ChangeGameObjectName = NowScene->FindGameObjectWithName(T2W(gameObject->GetBuffer()));

	// 같은 이름이 있으면 안 됨

	_ChangeGameObjectName->GameObject::SetName(T2W(changeName->GetBuffer()));

	return _ChangeGameObjectName->GetName().c_str();

}

void EditorManager::AddComponent(CString* gameObject, CString* componentName, int index)
{
	GameObject* _GameObject;

	_GameObject = NowScene->FindGameObjectWithName(T2W(gameObject->GetBuffer()));

	if (componentName->Collate(L"Camera") == 0)
	{
		Camera* _Camera = new Camera(Camera::eMode::DEFAULT);
		_GameObject->AddComponent(_Camera);
		_Camera->Start();
	}
	else if (componentName->Collate(L"BoxCollision") == 0)
	{
		BoxCollision* _BoxCollision = new BoxCollision();
		_GameObject->AddComponent(_BoxCollision);
		_BoxCollision->Awake();
		_BoxCollision->Start();

	}
	else if (componentName->Collate(L"SphereCollision") == 0)
	{
		SphereCollision* _SphereCollision = new SphereCollision();
		_GameObject->AddComponent(_SphereCollision);
		_SphereCollision->Awake();
		_SphereCollision->Start();
	}
	else if (componentName->Collate(L"CapsuleCollision") == 0)
	{
		CapsuleCollision* _CapsuleCollision = new CapsuleCollision();
		_GameObject->AddComponent(_CapsuleCollision);
		_CapsuleCollision->Awake();
		_CapsuleCollision->Start();
	}
	else if (componentName->Collate(L"Rigidbody") == 0)
	{
		Rigidbody* _Rigidbody = new Rigidbody();
		_GameObject->AddComponent(_Rigidbody);
		_Rigidbody->Awake();
	}
	else if (componentName->Collate(L"AudioListener") == 0)
	{
		AudioListener* _AudioListener = new AudioListener();
		_GameObject->AddComponent(_AudioListener);
		_AudioListener->Awake();
	}
	else if (componentName->Collate(L"AudioSource") == 0)
	{
		AudioSource* _AudioSource = new AudioSource();
		_GameObject->AddComponent(_AudioSource);
		_AudioSource->Awake();
	}
	else if (componentName->Collate(L"Script") == 0)
	{
		Script* _Script = new Script(L"NewScript");
		_GameObject->AddComponent(_Script);
		_Script->SetEditorIndex(index);
	}
	else if (componentName->Collate(L"Light") == 0)
	{
		Light* _Light = new Light();
		_GameObject->AddComponent(_Light);
	}
	else if (componentName->Collate(L"MeshFilter") == 0)
	{
		MeshFilter* _MeshFilter = new MeshFilter();
		_GameObject->AddComponent(_MeshFilter);
	}
	else if (componentName->Collate(L"MeshRenderer") == 0)
	{
		MeshRenderer* _MeshRenderer = new MeshRenderer();
		_GameObject->AddComponent(_MeshRenderer);
	}
	else if (componentName->Collate(L"SkinnedMeshRenderer") == 0)
	{
		SkinnedMeshRenderer* _SkinnedMeshRenderer = new SkinnedMeshRenderer();
		_GameObject->AddComponent(_SkinnedMeshRenderer);
	}
	else if (componentName->Collate(L"NavAgent") == 0)
	{
		NavAgent* _NavAgent = new NavAgent();
		_GameObject->AddComponent(_NavAgent);
	}
	else if (componentName->Collate(L"Networkcomponent") == 0)
	{
		Networkcomponent* _Networkcomponent = new Networkcomponent();
		_GameObject->AddComponent(_Networkcomponent);
	}
	else if (componentName->Collate(L"Animation") == 0)
	{
		Animation* _Animation = new Animation();
		_GameObject->AddComponent(_Animation);
	}
	else if (componentName->Collate(L"UIImage") == 0)
	{
		UIImage* _UIImage = new UIImage();
		_GameObject->AddComponent(_UIImage);
	}
	else if (componentName->Collate(L"UIText") == 0)
	{
		UIText* _UIText = new UIText();
		_GameObject->AddComponent(_UIText);
	}
	else if (componentName->Collate(L"UIButton") == 0)
	{
		UIButton* _UIButton = new UIButton();
		_GameObject->AddComponent(_UIButton);
	}
	else if (componentName->Collate(L"UIInputField") == 0)
	{
	
		UIInputField* _UIInputField = new UIInputField();
		_GameObject->AddComponent(_UIInputField);
	}

}

void EditorManager::RemoveComponent(CString* gameObject, CString* componentName, int index)
{
	GameObject* _GameObject;

	_GameObject = NowScene->FindGameObjectWithName(T2W(gameObject->GetBuffer()));


	if (componentName->Collate(L"Camera") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<Camera>());
	}
	else if (componentName->Collate(L"BoxCollision") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<BoxCollision>());
	}
	else if (componentName->Collate(L"SphereCollision") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<SphereCollision>());
	}
	else if (componentName->Collate(L"CapsuleCollision") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<CapsuleCollision>());
	}
	else if (componentName->Collate(L"Rigidbody") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<Rigidbody>());
	}
	else if (componentName->Compare(L"AudioListener") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<AudioListener>());
	}
	else if (componentName->Compare(L"AudioSource") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<AudioSource>());
	}
	else if (componentName->Compare(L"Script") == 0)
	{
		Script* script = nullptr;

		for (int i = 0; i < _GameObject->GetComponents().size(); i++)
		{
			script = dynamic_cast<hos::com::Script*>(_GameObject->GetComponents()[i]);
			if (script)
			{
				if (script->GetEditorIndex() == index)
				{
					_GameObject->RemoveComponent(script);
				}

			}
		}

		for (int i = 0; i < _GameObject->GetComponents().size(); i++)
		{
			script = dynamic_cast<hos::com::Script*>(_GameObject->GetComponents()[i]);
			if (script)
			{
				if (script->GetEditorIndex() > index)
				{
					script->SetEditorIndex(script->GetEditorIndex() - 1);
				}
			}
		}
	}
	else if (componentName->Compare(L"Light") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<Light>());
	}
	else if (componentName->Compare(L"MeshFilter") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<MeshFilter>());
	}
	else if (componentName->Compare(L"MeshRenderer") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<MeshRenderer>());
	}
	else if (componentName->Compare(L"SkinnedMeshRenderer") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<SkinnedMeshRenderer>());
	}
	else if (componentName->Compare(L"NavAgent") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<NavAgent>());
	}
	else if (componentName->Compare(L"Networkcomponent") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<Networkcomponent>());
	}
	else if (componentName->Compare(L"Animation") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<Animation>());
	}
	else if (componentName->Compare(L"UIImage") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<UIImage>());
	}
	else if (componentName->Compare(L"UIText") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<UIText>());
	}
	else if (componentName->Compare(L"UIButton") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<UIButton>());
	}
	else if (componentName->Compare(L"UIInputField") == 0)
	{
		_GameObject->RemoveComponent(_GameObject->GetComponent<UIInputField>());
	}
}

bool EditorManager::LoadScene(CString* scenePath)
{
	Scene* _LoadScene;

	_LoadScene = g_DataManager->LoadScene(T2W(scenePath->GetBuffer()));

	if (_LoadScene)
	{
		//SafeDelete(NowScene);
		NowScene = nullptr;

		NowScene = _LoadScene;
		NowScene->Awake();
		return true;
	}
	else
	{
		return false;
	}

	// 실패
}

bool EditorManager::SaveScene(CString* scenePath)
{
	// 에셋 폴더가 있나 확인

	// 해당하는 경로에 저장하기
	if (g_DataManager->SaveScene(T2W(scenePath->GetBuffer()), NowScene))
	{
		// 데이터 매니저에 있는 데이터들을 저장

		return true;
	}
	else
	{
		return false;
	}

}

void EditorManager::SavePrefab(CString* prefabPath, CString* gameObjectName)
{
	if (!gameObjectName)
	{
		return;
	}

	GameObject* _NewPrefab = NowScene->FindGameObjectWithName(T2W(gameObjectName->GetBuffer()));

	g_DataManager->SavePrefab(T2W(prefabPath->GetBuffer()), _NewPrefab);

}

bool EditorManager::LoadPrefab(CString* prefabPath, CString* parentGameObjectName)
{
	// 부모 게임 오브젝트의 이름이 없으면 루트 게임 오브젝트로 추가

	GameObject* _NewPrefab;

	_NewPrefab = g_DataManager->LoadPrefab(T2W(prefabPath->GetBuffer()));

	if (_NewPrefab)
	{
		if (parentGameObjectName)
		{
			GameObject* _ParentObject = NowScene->FindGameObjectWithName(T2W(parentGameObjectName->GetBuffer()));

			NowScene->AddGameObjectRecursive(_NewPrefab, _ParentObject);

		}
		else
		{
			NowScene->AddGameObjectRecursive(_NewPrefab);
		}

		return true;

	}
	else
	{
		return false;
	}
}

CString EditorManager::AddPrefab(CString* prefabName, CString* gameObjectPerent /*= nullptr*/)
{

	GameObject* _Prefab = g_DataManager->GetPrefab(T2W(prefabName->GetBuffer()));

	if (gameObjectPerent)
	{
		// 부모의 자식으로 넣기
		NowScene->AddGameObjectRecursive(_Prefab, NowScene->FindGameObjectWithName(T2W(gameObjectPerent->GetBuffer())));
	}
	else
	{
		NowScene->AddGameObjectRecursive(_Prefab);
	}

	return _Prefab->GetName().c_str();

}

void EditorManager::LoadGameData(CString* dataPath)
{
	g_DataManager->LoadData(T2W(dataPath->GetBuffer()));
}

void EditorManager::SaveDataManager(CString* savePath)
{
	g_DataManager->SaveAll(T2W(savePath->GetBuffer()));
}

void EditorManager::SetNowGameObject(CString* name)
{
	NowGameObject = NowScene->FindGameObjectWithName(T2W(name->GetBuffer()));
}

void EditorManager::EditorLoop()
{
	if (g_Renderer && NowScene)
	{
		////////////////////////////////////////////////////////////////////////////////////////

		//if (bLBTDOWN)
		//	g_Renderer->_DebugText(100, 200, 30, Vector4(0, 1, 0, 1), L"LBT DOWN : OK");
		//else
		//	g_Renderer->_DebugText(100, 200, 30, Vector4(1, 1, 1, 1), L"LBT DOWN : NOB");

		////////////////////////////////////////////////////////////////////////////////////////

		Time->Tick();

		NowScene->OnGUI(1.0f, 1.0f);

		// Update
		NowScene->FixedUpdate();
		NowScene->Update();
		NowScene->PhysicsUpdate();
		NowScene->LateUpdate();

		// Render To Camera RenderTarget
		NowScene->PreRender();
		NowScene->Render();

		// Render To SwapChin RnderTarget
		g_Renderer->BeginDraw();
		NowScene->PostRender();
		g_Renderer->RenderUI_ToSwapChainBuffer();
		g_Renderer->EndDraw();
	}
}