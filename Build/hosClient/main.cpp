#include "hosUtility\Dump.h"
#include "hosEngine\COM_Client.h"
#include "SceneManager.h"
#include <filesystem>
#include <thread>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Dump minidump(L"minidump");

	HANDLE process;
	process = CreateSemaphore(NULL, 1, 1, L"HOS_CLIENT");

	if (NULL == process)
	{
		return 1;
	}

	DWORD result = WaitForSingleObject(process, 0);

	if (result == WAIT_TIMEOUT)
	{
		CloseHandle(process);
		return 2;
	}
	else if (result == WAIT_OBJECT_0)
	{
		hos::com::COM_Client* system = new hos::com::COM_Client(hInstance);

	#pragma region loading
		HANDLE loadEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
		std::thread loadthread = std::thread([&] {

			//Loading Sample
			g_DataManager->LoadTexture(L"loading.png");

			RECT rt = system->GetApp()->GetSize();
			hos::U32 width = rt.right - rt.left;
			hos::U32 height = rt.bottom - rt.top;

			hos::cg::RenderQue::RenderUIObejct loadingBack;
			loadingBack.Width = width;
			loadingBack.Height = height;
			loadingBack.bTexture = true;
			loadingBack.TextureName = L"loading.png";

			while (true)
			{
				if (system->Update() == false)
				{
					break;
				}

				if (g_Renderer)
				{
					g_Renderer->AddRenderUIObejct(loadingBack);

					RECT windowSize = system->GetApp()->GetSize();

					g_Renderer->ForwardRender_ToCameraBuffer(0.0f, 0.1f, hos::Vector3(), hos::Vector3(), 90.f, (float)width, (float)height, 0.1f, 100.f, hos::cg::RendererDesc::CameraModeList::CAMERA3D_DEFAULT, 0, 0, false, L"", hos::Vector4(), bDebug);
					g_Renderer->BeginDraw(windowSize.right, windowSize.bottom);
					g_Renderer->RenderUI_ToSwapChainBuffer();
					g_Renderer->EndDraw();
				}

				DWORD result = WaitForSingleObject(loadEnd, 1);
				if (result == WAIT_OBJECT_0)
				{
					break;
				}
			}
			});

		system->Load();
		SetEvent(loadEnd);
		loadthread.join();
	#pragma endregion

		g_Renderer->SetViewFrustum(false);
		SceneManagement = new hos::SceneManager();
		SceneManagement->ChangeScene(hos::SceneManager::START_SCENE);
		bool Valid = false;
		hos::com::Scene* scene = nullptr;
		while (true)
		{
			if (system->Update() == false)
			{
				break;
			}
			if (SceneManagement->IsLoop())
			{
				scene = SceneManagement->GetScene();
				if (nullptr == scene)
				{
					if (!Valid)
					{
						if (MessageBox(system->GetApp()->GetH_Wnd(), L"실행가능한 씬이 없습니다.\n종료하시겠습니까?", L"Error", MB_YESNO | MB_ICONERROR) == IDYES)
						{
							system->GetApp()->Destory();
							break;
						}
						else
						{
							Valid = true;
						}
					}
					continue;
				}

				RECT windowRECT = system->GetApp()->GetSize();
				float aspect_x = ((float)(windowRECT.right - windowRECT.left) / 1920.f);
				float aspect_y = ((float)(windowRECT.bottom - windowRECT.top) / 1080.f);

				scene->OnGUI(aspect_x, aspect_y);
				scene->FixedUpdate();
				scene->Update();
				scene->PhysicsUpdate();
				scene->LateUpdate();
				scene->PreRender();
				scene->Render();

				// Device RenderTarget Set
				if (g_Renderer)
				{
					RECT windowSize = system->GetApp()->GetSize();
					g_Renderer->BeginDraw(windowSize.right, windowSize.bottom);
				}
				scene->PostRender();

				g_Renderer->RenderUI_ToSwapChainBuffer();

				// Present
				if (g_Renderer)
				{
					g_Renderer->EndDraw();
				}

				SceneManagement->CheckScene();
			}
			else
			{
				break;
			}
		}

		hos::SafeDelete(g_Navigation);
		hos::SafeDelete(SceneManagement);
		hos::SafeDelete(system);
	}

	CloseHandle(process);
#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif
	return 0;
}