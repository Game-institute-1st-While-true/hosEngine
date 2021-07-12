#pragma once
#include <thread>
#include <mutex>
#include <atomic>

#include <deque>
#include <map>

#include "NavDefine.h"
#include "NavigationMaker.h"
#include "NavQuery.h"


// [2021/03/18 ��ȿ��]
// �޽� �����͸� �̿��Ͽ� ������ NavMesh �� �����ϴ� Ŭ����. 
// Agent�� �����͸� �̿��Ͽ� ��ã�� ������ �����ϰ� ��ȯ�ϴ� Ŭ����.

namespace hos::nv
{
	constexpr int MAX_AGENT = 300;

	class Navigation
	{
		friend class NavigationMaker;
	
	private:
		int MeshSeed;

		BuildContext* Context;
		NavigationMaker* Maker;

		std::map<int, dtNavMesh*> NavMeshPool;	// int : MapDataIdx
		std::vector<NavQuery*> QueryPool;
		std::deque<int> QueryIdPool;

		std::atomic<bool> IsRun;
		std::atomic<bool> IsConfig;
		std::mutex AgentLock;

		std::thread	NavWorker;
	
	public:
		Navigation();
		virtual ~Navigation();

	public:
		NavigationMaker* GetMaker() { return Maker; }		
		void CreateMaker();
		void Load();
		void Run();

	public:
		bool AddQuery(NavQuery* query);
		void DelQuery(NavQuery* query);
		
		int AddMesh(dtNavMesh* mesh);
		int AddMesh(dtNavMesh* mesh, int seed);

	private:
		void MakeAgentPool();
		void StartThread();
		void DestroyThread();
		void WorkFunc();

		void QueryProcss();
	};
}
