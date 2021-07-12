#include "COM_Testbed.h"
#include "Detour\detournavmesh.h"

using namespace hos;

hos::com::COM_Testbed::COM_Testbed(HINSTANCE ins)
{
	//프로그램 실행폴더 얻기
	WCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	g_Path = path;

	//GamePadInput
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	HRESULT rohr = RoInitialize(RO_INIT_MULTITHREADED);
#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	//COM 초기화
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		throw std::invalid_argument("COM Init FAIL");
	}

	//global Variable
	Time = new ut::GameTimer();

	//										3D Sound환경리버브 효과 활성화				|	3D Sound환경리버브 추가기능				|	xAPO 활성화
	DirectX::AUDIO_ENGINE_FLAGS audioFlag = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters | DirectX::AudioEngine_UseMasteringLimiter;
#if defined(_DEBUG)|defined(DEBUG)
	audioFlag |= DirectX::AudioEngine_Debug;
#endif // defined(_DEBUG)|defined(DEBUG)
	try
	{
		g_Audio = new DirectX::AudioEngine(audioFlag);
	}
	catch (...)
	{
		MessageBox(App->GetH_Wnd(), L"3D Audio 장치를 지원하지 않습니다.\n", L"ERROR", MB_OK | MB_ICONERROR);
		audioFlag = DirectX::AudioEngine_Default;
	#if defined(_DEBUG)|defined(DEBUG)
		audioFlag |= DirectX::AudioEngine_Debug;
	#endif // defined(_DEBUG)|defined(DEBUG)
		g_Audio = new DirectX::AudioEngine(audioFlag);
	}

	g_Px = new PhysX();

	g_Navigation = new hos::nv::Navigation();

	HANDLE threadstart = CreateEvent(NULL, TRUE, FALSE, NULL);
	//Application Thread
	int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYSCREEN);
	App = new WindowsApplication(L"HOS_TESTBED");
	MainThread = std::thread([&]() {
		if (App->Create(ins, monitorWidth, monitorHeight, 0, 0, WS_OVERLAPPED))
		{
			Input = new WindowsInput(App->GetH_Wnd());

			RECT rt = App->GetSize();
			// 렌더 정보 설정
			hos::cg::RendererDesc temp;
			temp.Driver = hos::cg::RendererDesc::DriverType::DRIVER_DIRECT3D11;
			temp.ScreenWidth = rt.right - rt.left;
			temp.ScreenHeight = rt.bottom - rt.top;
			temp.Fullscreen = false;
			temp.Hwnd = App->GetH_Wnd();

			g_Renderer = hos::cg::Renderer::CreateRenderer(temp);

			SetEvent(threadstart);
			App->Run();
		}
		});
	WaitForSingleObject(threadstart, INFINITE);

	g_DataManager = new hos::com::DataManager;
}

hos::com::COM_Testbed::~COM_Testbed()
{
	if (MainThread.joinable())
	{
		MainThread.join();
	}

	//렌더러 지우기 전에 디버그 종료하면 안됨
	SafeDelete(g_Audio);
	SafeDelete(g_Renderer);
	SafeDelete(g_Navigation);
	SafeDelete(g_Px);
	SafeDelete(Time);
	SafeDelete(Input);
	SafeDelete(App);
	Debug->Destory();

	CoUninitialize();
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	RoUninitialize();
#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
}

void hos::com::COM_Testbed::Load()
{
	/// ///////////////////////////////////////////////////////////

	Scene* newScene = new Scene();
	newScene->SetName(L"FirstScene");

	//여기 코드 작성
#pragma region navigation mesh bake
	/*
	GameObject* obj1 = new GameObject();
	Mesh* m1 = new Mesh();
	m1->SetMeshName(L"Quad");
	m1->SetVtxCount(6);
	cg::VertexDataType::FULL_VTX* vertex = new cg::VertexDataType::FULL_VTX[6];
	vertex[0].vPos = Vector3(100, 0, 100);
	vertex[1].vPos = Vector3(-100, 0, 100);
	vertex[2].vPos = Vector3(-100, 0, -100);
	vertex[3].vPos = Vector3(100, 0, 100);
	vertex[4].vPos = Vector3(-100, 0, -100);
	vertex[5].vPos = Vector3(100, 0, -100);
	m1->SetVtxDataAdress(vertex);
	m1->SetIndexCount(6);
	U32* index = new U32[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 3;
	index[4] = 4;
	index[5] = 5;
	m1->SetIndexDataAdress(index);
	g_Renderer->CreateMeshVariable(m1->GetMeshName(), vertex, 6, index, 6);

	MeshFilter* mf = new MeshFilter();
	obj1->AddComponent(mf);

	mf->SetMeshData(m1);
	MeshRenderer* mr = new MeshRenderer();
	obj1->AddComponent(mr);

	newScene->AddGameObject(obj1);

	GameObject* box = g_DataManager->LoadPrefab(L"../../Resource/navtest2.fbx");
	newScene->AddGameObjectRecursive(box);
	box->transform->SetPosition(Vector3(50, -1, 50));
	box->transform->SetRotation(Rotator(30, 0, 30));

	g_Navigation->CreateMaker();
	auto Maker = g_Navigation->GetMaker();
	Maker->Bake(newScene);
	Maker->Save("solo_navmesh.bin", Maker->GetMesh());

	//*/
#pragma endregion

#pragma region navigation mesh load

	//g_Navigation->CreateMaker();
	//auto Maker = g_Navigation->GetMaker();
	//GameObject* obj1 = new GameObject();
	//Mesh* m1 = new Mesh();
	//m1->SetMeshName(L"navMesh");
	//m1->SetVtxCount(6);
	//cg::VertexDataType::FULL_VTX* vertex = new cg::VertexDataType::FULL_VTX[6];
	//vertex[0].vPos = Vector3(100, 0, 100);
	//vertex[1].vPos = Vector3(-100, 0, 100);
	//vertex[2].vPos = Vector3(-100, 0, -100);
	//vertex[3].vPos = Vector3(100, 0, 100);
	//vertex[4].vPos = Vector3(-100, 0, -100);
	//vertex[5].vPos = Vector3(100, 0, -100);
	//m1->SetVtxDataAdress(vertex);
	//m1->SetIndexCount(6);
	//U32* index = new U32[6];
	//m1->SetIndexDataAdress(index);
	//if (Maker->Load("D:\\GA1stFinal_WhileTrue\\6_Engine\\hosEngine_ver_2\\hosClient\\solo_navmesh.bin"))
	//{
	//	const dtNavMesh* navmesh = Maker->GetMesh();

	//	for (int i = 0; i < navmesh->getMaxTiles(); ++i)
	//	{
	//		const dtMeshTile* tile = navmesh->getTile(i);
	//		if (!tile->header)
	//		{
	//			continue;
	//		}

	//		for (int j = 0; j < tile->header->polyCount; ++j)
	//		{
	//			const dtPoly* poly = &tile->polys[i];
	//			if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
	//			{
	//				continue;
	//			}
	//			const dtPolyDetail* polydetail = &tile->detailMeshes[i];

	//			for (int k = 0; k < polydetail->triCount; ++k)
	//			{
	//				const unsigned char* t = &tile->detailTris[(polydetail->triBase + k) * 4];
	//				for (int l = 0; l < 3; ++l)
	//				{
	//					if (t[l] < poly->vertCount)
	//					{
	//						float* vert = &tile->verts[poly->verts[t[l]] * 3];
	//					}
	//					else
	//					{
	//						float* vert = &tile->detailVerts[(polydetail->vertBase + t[l] - poly->vertCount) * 3];
	//					}
	//				}
	//			}
	//		}

	//	}
	//}

	//g_Renderer->CreateMeshVariable(m1->GetMeshName(), vertex, 6, index, 6);

#pragma endregion

	//GameObject* obj = g_DataManager->LoadPrefab(L"../../Resource/char_idle.fbx");
	//GameObject* obj = g_DataManager->LoadPrefab(L"../../Resource/box_skin.fbx");
	//newScene->AddGameObjectRecursive(obj);

	//GameObject* obj1 = g_DataManager->LoadPrefab(L"../../Resource/t1.fbx");
	//newScene->AddGameObjectRecursive(obj1);

	//GameObject* obj2 = g_DataManager->LoadPrefab(L"../../Resource/t2.fbx");
	//newScene->AddGameObjectRecursive(obj2);

	/*GameObject* obj2 = g_DataManager->LoadPrefab(L"../../Resource/box_skin.fbx");
	newScene->AddGameObjectRecursive(obj2);*/


	//auto nav1 = obj1->FindObjectRecusive(L"Box002");
	//auto nav2 = obj2->FindObjectRecusive(L"Box001");

	//Maker->AddMesh(nav1->GetComponent<MeshFilter>()->GetMeshData());
	////Maker->AddMesh(nav2->GetComponent<MeshFilter>()->GetMeshData());
	//Maker->MakeInputData();
	//Maker->Bake();




//	Maker->Load("solo_navmesh.bin");


	//newScene->AddGameObjectRecursive(obj2);

	//g_DataManager->CreateALLRenderVariable_InDataManager();

	//camera
	GameObject* CameraObj = new GameObject(L"MainCamera");
	RECT rt = App->GetSize();
	//Camera* cam = new Camera(rt.right - rt.left, rt.bottom - rt.top);
	Camera* cam = new Camera(Camera::eMode::DEFAULT);
	CameraObj->transform->SetPosition({ 0.f,0.8f,-10.f });
	CameraObj->transform->SetRotation({ 0.f,0.f,0.f });
	CameraObj->AddComponent(cam);
	Script* moveScript = new Script(L"CameraMove");
	CameraObj->AddComponent(moveScript);

	newScene->AddGameObject(CameraObj);

	//light
	GameObject* LightObj = new GameObject();
	Light* light = new Light();
	LightObj->AddComponent(light);
	LightObj->transform->SetRotation({ -60.f,0.f,0.f });
	newScene->AddGameObject(LightObj);

	//Manager->AddScene(newScene);
	//GameObject* PlayerObj = g_DataManager->LoadPrefab(L"../../Resource/testBox.fbx");
	//GameObject* PlayerObj = new GameObject(L"Player");
	//// Player
	////GameObject* PlayerObj = new GameObject(L"Player");
	//Script* PlayerScript = new Script(L"PlayerState");
	//
	////BoxObj->transform->SetParent(PlayerObj->transform);

	//PlayerObj->AddComponent(PlayerScript);

	//GameObject* p2 = new GameObject(L"p2");
	//GameObject* p3 = new GameObject(L"p3");

	//BoxCollision* bc = new BoxCollision();
	//BoxCollision* bc2 = new BoxCollision();
	//BoxCollision* bc3 = new BoxCollision();

	//PlayerObj->AddComponent(bc);
	//p2->AddComponent(bc2);
	//p3->AddComponent(bc3);

	//p2->transform->SetLocalPosition(Vector3(1, 0, 0));
	//p3->transform->SetLocalPosition(Vector3(2, 0, 0));

	//p2->transform->SetParent(PlayerObj->transform);
	//p3->transform->SetParent(p2->transform);

	//Mesh* m2 = new Mesh();
	//m2->SetMeshName(L"Player");
	//m2->SetVtxCount(8);
	//cg::VertexDataType::FULL_VTX* vertex2 = new cg::VertexDataType::FULL_VTX[8];

	//vertex2[0].vPos = Vector3(-0.5, 0, 0.5);
	//vertex2[1].vPos = Vector3(0.5, 0, 0.5);
	//vertex2[2].vPos = Vector3(-0.5, 0, -0.5);
	//vertex2[3].vPos = Vector3(0.5, 0, -0.5);
	//vertex2[4].vPos = Vector3(-0.5, 1, 0.5);
	//vertex2[5].vPos = Vector3(0.5, 1, 0.5);
	//vertex2[6].vPos = Vector3(-0.5, 1, -0.5);
	//vertex2[7].vPos = Vector3(0.5, 1.0, -0.5);

	//m2->SetVtxDataAdress(vertex2);
	//m2->SetIndexCount(6);

	//U32* index2 = new U32[36];
	//index2[0] = 0;
	//index2[1] = 2;
	//index2[2] = 3;

	//index2[3] = 3;
	//index2[4] = 1;
	//index2[5] = 0;

	//index2[6] = 4;
	//index2[7] = 5;
	//index2[8] = 7;

	//index2[9] = 7;
	//index2[10] = 6;
	//index2[11] = 4;

	//index2[12] = 0;
	//index2[13] = 1;
	//index2[14] = 5;

	//index2[15] = 5;
	//index2[16] = 4;
	//index2[17] = 0;

	//index2[18] = 1;
	//index2[19] = 3;
	//index2[20] = 7;

	//index2[21] = 7;
	//index2[22] = 5;
	//index2[23] = 1;

	//index2[24] = 3;
	//index2[25] = 2;
	//index2[26] = 6;

	//index2[27] = 6;
	//index2[28] = 7;
	//index2[29] = 3;

	//index2[30] = 2;
	//index2[31] = 0;
	//index2[32] = 4;

	//index2[33] = 4;
	//index2[34] = 6;
	//index2[35] = 2;

	//m2->SetIndexDataAdress(index2);
	//g_Renderer->CreateMeshVariable(m2->GetMeshName(), vertex2, 8, index2, 36);

	//MeshFilter* mf2 = new MeshFilter();
	//PlayerObj->AddComponent(mf2);
	//mf2->SetMeshData(m2);

	//MeshRenderer* mr2 = new MeshRenderer();
	//PlayerObj->AddComponent(mr2);

	//newScene->AddGameObjectRecursive(PlayerObj);
	// Box

	g_DataManager->AddScene(newScene);
}

bool hos::com::COM_Testbed::Update()
{
	DWORD exitCode = STILL_ACTIVE;

	if (GetExitCodeThread(MainThread.native_handle(), &exitCode))
	{
		if (exitCode != STILL_ACTIVE)
		{
			return false;
		}
	}
	Time->Tick();
	Input->Update();

	return true;
}

WindowsApplication* hos::com::COM_Testbed::GetApp() const
{
	return App;
}
