#include "SceneManager.h"

using namespace hos;

hos::com::SceneManager* SceneManagement = nullptr;

hos::com::SceneManager::SceneManager() : NowScene(Scene::VALID), IsChange(false)
{
}

hos::com::SceneManager::~SceneManager()
{
	for (Scene* scene : Scenes)
	{
		delete scene;
	}
}

void hos::com::SceneManager::AddScene(Scene* scene)
{
	if (scene->GetIsLoaded())
	{
		scene->SetBuildIndex((int)Scenes.size());
		Scenes.emplace_back(scene);
	}
}

void hos::com::SceneManager::RemoveScene(int buildIndex)
{
	auto it = Scenes.begin();
	if (0 <= buildIndex && buildIndex < Scenes.size())
	{
		Scene* _scene = nullptr;
		for (int _index = 0; _index < Scenes.size(); _index++)
		{
			if (_scene == nullptr)
			{
				if (_index == buildIndex)
				{
					_scene = *it;
				}
				else
				{
					++it;
				}
			}
			else
			{
				Scenes[_index]->SetBuildIndex(_index - 1);
			}
		}

		if (_scene)
		{
			Scenes.erase(it);
			delete _scene;
		}
	}
}

void hos::com::SceneManager::ChangeScene(int buildIndex)
{
	IsChange = true;
	if (NowScene != Scene::VALID)
	{
		Scenes[NowScene]->OnClose();
	}
	NowScene = buildIndex;
	Scenes[NowScene]->Awake();
	Scenes[NowScene]->Start();
}

void hos::com::SceneManager::ChangeScene(string_view name)
{
	for (auto _scene : Scenes)
	{
		if (_scene->GetName() == name.data())
		{
			ChangeScene(_scene->GetBuildIndex());
			return;
		}
	}
}

bool hos::com::SceneManager::HasScene()
{
	return 0 < Scenes.size();
}

com::Scene* hos::com::SceneManager::GetScene(int buildIndex)
{
	return Scenes.at(buildIndex);
}

std::vector<com::Scene*>& hos::com::SceneManager::GetScenes()
{
	return Scenes;
}

void hos::com::SceneManager::Loop()
{
	if (!IsChange)
	{
		Scenes[NowScene]->FixedUpdate();
		Scenes[NowScene]->PhysicsUpdate();
		Scenes[NowScene]->Update();
		Scenes[NowScene]->OnStateMachine();
		Scenes[NowScene]->OnAnimation();
		Scenes[NowScene]->LateUpdate();

		Scenes[NowScene]->PreRender();
		if (g_Renderer)
		{

		}
		Scenes[NowScene]->Render();
		if (g_Renderer)
		{
			// 렌더큐에 있는거 다 그려라
			g_Renderer->RenderAll(true, true);
		}
		Scenes[NowScene]->PostRender();

		//UI Render start
		Scenes[NowScene]->RenderImage();
		Scenes[NowScene]->OnGUI();

		//오브젝트 추가된것들 awake, start
	}
	IsChange = false;
}

void hos::com::SceneManager::NonBockingLoop()
{
	auto scene = Scenes[NowScene];
	if (!IsChange)
	{
		scene->Loop();
	}
	IsChange = false;
}
