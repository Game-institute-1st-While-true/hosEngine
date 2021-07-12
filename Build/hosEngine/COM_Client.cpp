#include "COM_Client.h"
#include "DebugConsolePaper.h"

#include <fstream>

using namespace hos;

hos::com::COM_Client::COM_Client(HINSTANCE ins)
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
	Time = new GameTimer();

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

	Network = new NetworkManager();
	Network->Initialize(1, 2, 0, new DebugConsolePaper(), 700);
	Network->Start();

	g_Px = new PhysX();
	g_Navigation = new hos::nv::Navigation();

	HANDLE threadstart = CreateEvent(NULL, TRUE, FALSE, NULL);
	//Application Thread
	int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYSCREEN);
	App = new WindowsApplication(L"PULXER Ver0.0.0.1");
	MainThread = std::thread([&]() {
		HCURSOR cur = LoadCursor(nullptr, IDC_ARROW);
		if (App->Create(ins, 1920, 1080, 0, 0, WS_OVERLAPPEDWINDOW, NULL, cur))
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
			temp.ValueAA = 8;
			temp.Vsync = true;

			g_Renderer = hos::cg::Renderer::CreateRenderer(temp);

			SetEvent(threadstart);
			App->Run();
		}
		});
	WaitForSingleObject(threadstart, INFINITE);
	g_DataManager = new hos::com::DataManager;
}

hos::com::COM_Client::~COM_Client()
{
	App->Destory();
	if (MainThread.joinable())
	{
		MainThread.join();
	}

	//렌더러 지우기 전에 디버그 종료하면 안됨
	SafeDelete(g_Audio);
	SafeDelete(g_Renderer);
	SafeDelete(Network);
	SafeDelete(g_DataManager);
	SafeDelete(g_Px);
	SafeDelete(Time);
	SafeDelete(Input);
	SafeDelete(App);

	NetLogger::GetIns()->Destory();
	Debug->Destory();

	CoUninitialize();
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	RoUninitialize();
#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
}

void hos::com::COM_Client::Load()
{
	std::wfstream file;
	file.open(g_Path + L"\\Data.hos");

	if (file.is_open())
	{
		while (!file.eof())
		{
			string sceneName;
			std::getline(file, sceneName);
			Scene* newScene = g_DataManager->LoadScene(g_Path + Scene::FILE_PATH + sceneName + Scene::FILE_EXTENSION);
		}
	}

	if (g_Navigation)
	{
		g_Navigation->Load();
		g_Navigation->Run();
	}
}

bool hos::com::COM_Client::Update()
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
	Network->Run();

	if (Input->GetKeyDown(KeyCode::Delete))
	{
		bDebug = !bDebug;
	}

	return true;
}

WindowsApplication* hos::com::COM_Client::GetApp() const
{
	return App;
}