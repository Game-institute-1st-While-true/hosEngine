#include "Object.h"

#include "Navigation.h"
#include "Scene.h"
#include <filesystem>

hos::nv::Navigation::Navigation()
	:Maker(nullptr), MeshSeed(0)
{
	Context = new BuildContext();
	MakeAgentPool();
	Maker = new NavigationMaker(this);
}

hos::nv::Navigation::~Navigation()
{
	SafeDelete(Maker);
	SafeDelete(Context);
	DestroyThread();
	for (auto mesh : NavMeshPool)
	{
		if (mesh.second)
		{
			dtFreeNavMesh(mesh.second);
		}
	}

	/*for (auto query : QueryPool)
	{
		SafeDelete(query);
	}*/

	NavMeshPool.clear();
}

void hos::nv::Navigation::CreateMaker()
{
	if (Maker)
	{
		Maker = new NavigationMaker(this);
	}
}

void hos::nv::Navigation::Load()
{
	string navipath = L"\\Assets\\Data\\Navi";
	try
	{
		for (auto scene : g_DataManager->Scenes)
		{
			string name = scene.second->GetName();
			string judpath = g_Path + navipath + L"\\" + name + L".bin";
			dtNavMesh* mesh = nullptr;

			if (Maker->Load(ut::UTF16ToAnsi(judpath)))
			{
				mesh = Maker->GetMesh();
			}

			if (mesh)
			{
				AddMesh(mesh, scene.second->GetBuildIndex());
			}
		}
	}
	catch (...)
	{
		Debug->LogConsole(L"Navigation", L"Load Fail");
		return;
	}
}

void hos::nv::Navigation::Run()
{
	IsRun.store(true);
	StartThread();
}

bool hos::nv::Navigation::AddQuery(NavQuery* query)
{
	std::lock_guard<std::mutex> lock(AgentLock);
	// 현재 속한 네비 agent인지 확인
	if (query->NavId >= 0) return false;
	if (query->MeshId < 0) return false;

	// nav mesh를 더 수용할 수 있는지 확인
	if (QueryIdPool.empty()) return false;

	// mesh id로 nav mesh 가져오기
	dtNavMesh* mesh;
	auto iter = NavMeshPool.find(query->MeshId);

	if (iter == NavMeshPool.end())
	{
		return false;
	}
	else
	{
		mesh = iter->second;
	}

	query->SetDefaultFilter();
	if (query->InitQuery(mesh))
	{
		// agent id 부여
		int id = QueryIdPool.front();
		QueryIdPool.pop_front();
		query->NavId = id;
		QueryPool[id] = query;
		return true;
	}
	return false;
}

void hos::nv::Navigation::DelQuery(NavQuery* query)
{
	std::lock_guard<std::mutex> lock(AgentLock);

	query->MeshId = -1;
	QueryIdPool.push_back(query->NavId);
	QueryPool[query->NavId] = nullptr;
}

int hos::nv::Navigation::AddMesh(dtNavMesh* mesh)
{
	NavMeshPool.insert(std::make_pair(++MeshSeed, mesh));
	return MeshSeed;
}

int hos::nv::Navigation::AddMesh(dtNavMesh* mesh, int seed)
{
	NavMeshPool.insert(std::make_pair(seed, mesh));
	return seed;
}

void hos::nv::Navigation::WorkFunc()
{
	while (IsRun.load() == true)
	{
		QueryProcss();
	}
}

void hos::nv::Navigation::QueryProcss()
{
	for (NavQuery* query : QueryPool)
	{
		if (query)
		{
			query->Process();

			if (query->Del.load())
			{
				DelQuery(query);
			}
		}
	}
}

void hos::nv::Navigation::MakeAgentPool()
{
	QueryPool.reserve(MAX_AGENT);
	for (int i = 0; i < MAX_AGENT; i++)
	{
		QueryPool.push_back(nullptr);
		QueryIdPool.push_back(i);
	}
}

void hos::nv::Navigation::StartThread()
{
	NavWorker = std::thread([&]() { WorkFunc(); });
}

void hos::nv::Navigation::DestroyThread()
{
	IsRun.store(false);
	if (NavWorker.joinable() == true)
	{
		NavWorker.join();
	}
}