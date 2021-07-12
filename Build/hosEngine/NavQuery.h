#pragma once
#include <shared_mutex>

#include <atomic>
#include "NavDefine.h"
#include "hosUtility/hosMath.h"

namespace hos::com 
{ 
	class NavAgent;
}

namespace hos::nv
{
	class NavQuery
	{
		friend class Navigation;
		friend class com::NavAgent;
	public:
		std::mutex PositionLock;							// pos update lock

	private:
		std::atomic<bool> Del;

		int MeshId;											// �ڽ��� �����ִ� NavMesh�� ��ȣ
		int NavId;											// Navgation �� �� �ִ� �ڽ��� Id	

		dtNavMeshQuery* Query;								// NavQuery
		dtQueryFilter Filter;								// Option Filter
		dtStatus Status;									// Return Value

		dtPolyRef StartRef;									// Start Polygon Id
		dtPolyRef EndRef;									// End Polygon Id
		dtPolyRef Polys[MAX_POLYS];							// All Polygon Id
		int PolyCount;										// All Polygon Count

		dtPolyRef ExiPolys[MAX_POLYS];						// ? ����� Polygon Id�� ������� ����.
		int ExiPolyCount;									// Exisit Polygon Count

		float PolyPickExt[3];

		float StartPos[3];									// ���� ��ġ
		float EndPos[3];									// �� ��ġ

		float StraightPath[MAX_POLYS * 3];					// ������ ���� ���
		unsigned char StraightPathFlags[MAX_POLYS];
		dtPolyRef StraightPathPolys[MAX_POLYS];
		int StraightPathCount;

		int TryCount;										// Agent�� Query�� �õ��� Ƚ��
		int MaxPathCount;									// Agent�� �õ��� Max Count;
		int CurrentPathCount;								// Agent�� ���� �õ��� Count;
		Vector3 Dir;

	public:
		NavQuery();
		~NavQuery();

	public:
		// get set
		void SetMaxPathCount(int count) { MaxPathCount = count; }
		int GetMaxPathCount() { return MaxPathCount; }

		int GetPathCount() { return CurrentPathCount; }

		void SetUpdateCount(int count) { TryCount = count; }
		int GetUpdateCount() { return TryCount; }

	public:
		void SetDefaultFilter();
		void SetFilter(dtQueryFilter filter) { Filter = filter; }

		bool InitQuery(dtNavMesh* mesh);

		void SetStart(Vector3 vec);
		void SetEnd(Vector3 vec);
		
		Vector3 GetStart();
		Vector3 GetEnd();
		
		void ClearQuery();

		void NearStraightPathPolys();	// ���� �����͸� ��´�.
	private:
		void ProcessPartial();
		void Process();
	};
}
