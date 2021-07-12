#include "NavQuery.h"

using namespace hos;

hos::nv::NavQuery::NavQuery() :
	Query(nullptr), Status(DT_FAILURE), CurrentPathCount(0), MaxPathCount(0), TryCount(0), PolyCount(0), StartRef(0), EndRef(0),
	StraightPathCount(0), MeshId(-1), NavId(-1), Del(false)
{
	Query = dtAllocNavMeshQuery();
	PolyPickExt[0] = 2;
	PolyPickExt[1] = 4;
	PolyPickExt[2] = 2;
}

hos::nv::NavQuery::~NavQuery()
{
	if (Query)
	{
		dtFreeNavMeshQuery(Query);
		Query = nullptr;
	}
}

void hos::nv::NavQuery::SetDefaultFilter()
{
	Filter.setAreaCost(POLYAREA_GROUND, 1.0f);
	Filter.setAreaCost(POLYAREA_WATER, 1.0f);
	Filter.setAreaCost(POLYAREA_ROAD, 1.0f);
	Filter.setAreaCost(POLYAREA_GRASS, 1.0f);
	Filter.setAreaCost(POLYAREA_JUMP, 1.0f);
}

bool hos::nv::NavQuery::InitQuery(dtNavMesh* mesh)
{
	dtStatus status = Query->init(mesh, 2048);
	if (dtStatusFailed(status))
		return false;

	return true;
}

void hos::nv::NavQuery::SetStart(Vector3 vec)
{
	std::lock_guard<std::mutex> lock(PositionLock);
	StartPos[0] = vec.x;
	StartPos[1] = vec.y;
	StartPos[2] = vec.z;
}

void hos::nv::NavQuery::SetEnd(Vector3 vec)
{
	std::lock_guard<std::mutex> lock(PositionLock);
	EndPos[0] = vec.x;
	EndPos[1] = vec.y;
	EndPos[2] = vec.z;
}

Vector3 hos::nv::NavQuery::GetStart()
{
	std::lock_guard<std::mutex> lock(PositionLock);
	return Vector3(StartPos[0], StartPos[1], StartPos[2]);
}

Vector3 hos::nv::NavQuery::GetEnd()
{
	std::lock_guard<std::mutex> lock(PositionLock);
	return Vector3(EndPos[0], EndPos[1], EndPos[2]);
}

void hos::nv::NavQuery::ClearQuery()
{
	Del.store(false);
}

void hos::nv::NavQuery::NearStraightPathPolys()
{
	std::lock_guard<std::mutex> lock(PositionLock);
	dtPolyRef Start = 0;
	dtPolyRef End = 0;

	Query->findNearestPoly(StartPos, PolyPickExt, &Filter, &Start, 0);
	Query->findNearestPoly(EndPos, PolyPickExt, &Filter, &End, 0);

	if (Start == End)
	{
		Dir = Vector3(EndPos[0], EndPos[1], EndPos[2]) - Vector3(StartPos[0], StartPos[1], StartPos[2]);
		Dir.Normalize();
		return;
	}
	else
	{
		Dir = Vector3(StraightPath[3], StraightPath[4], StraightPath[5]) - Vector3(StraightPath[0], StraightPath[1], StraightPath[2]);
		Dir.Normalize();
		return;
	}
}

void hos::nv::NavQuery::ProcessPartial()
{
	if (dtStatusFailed(Status)) // Status가 false 인 경우 
	{
		dtPolyRef Start = 0;
		dtPolyRef End = 0;

		std::scoped_lock<std::mutex> lock(PositionLock);

		Query->findNearestPoly(StartPos, PolyPickExt, &Filter, &Start, 0);
		Query->findNearestPoly(EndPos, PolyPickExt, &Filter, &End, 0);

		// 만약 이전 데이터와 같다면 리턴.
		if (Start == End)	return;

		StartRef = Start;
		EndRef = End;
		if (StartRef && EndRef)
		{
			StraightPathCount = 0;
			Status = Query->initSlicedFindPath(StartRef, EndRef, StartPos, EndPos, &Filter, DT_FINDPATH_ANY_ANGLE);
		}
	}
	else
	{
		++CurrentPathCount;
		Status = Query->updateSlicedFindPath(TryCount, 0);

		if (CurrentPathCount >= MaxPathCount || dtStatusSucceed(Status))
		{
			Status = Query->finalizeSlicedFindPathPartial(Polys, PolyCount, Polys, &PolyCount, MAX_POLYS);
			CurrentPathCount = 0;
			Status = DT_FAILURE;
		}
	}
}

void hos::nv::NavQuery::Process()
{
	std::lock_guard<std::mutex> lock(PositionLock);

	if (dtStatusFailed(Status)) // Status가 false 인 경우 
	{
		dtPolyRef Start = 0;
		dtPolyRef End = 0;

		Query->findNearestPoly(StartPos, PolyPickExt, &Filter, &Start, 0);
		Query->findNearestPoly(EndPos, PolyPickExt, &Filter, &End, 0);

		// 만약 이전 데이터와 같다면 리턴.
		if (Start == End)	return;

		StartRef = Start;
		EndRef = End;

		if (StartRef && EndRef)
		{
			StraightPathCount = 0;
			Status = Query->initSlicedFindPath(StartRef, EndRef, StartPos, EndPos, &Filter, DT_FINDPATH_ANY_ANGLE);
		}
	}

	if (dtStatusInProgress(Status))
	{
		Status = Query->updateSlicedFindPath(1, 0);
	}

	if (dtStatusSucceed(Status))
	{
		Query->finalizeSlicedFindPath(Polys, &PolyCount, MAX_POLYS);
		StraightPathCount = 0;
		if (PolyCount)
		{
			// In case of partial path, make sure the end point is clamped to the last polygon.
			float epos[3];
			dtVcopy(epos, EndPos);
			if (Polys[PolyCount - 1] != EndRef)
				Query->closestPointOnPoly(Polys[PolyCount - 1], EndPos, epos, 0);

			Query->findStraightPath(StartPos, epos, Polys, PolyCount,
				StraightPath, StraightPathFlags,
				StraightPathPolys, &StraightPathCount, MAX_POLYS, DT_STRAIGHTPATH_ALL_CROSSINGS);
		}
		Status = DT_FAILURE;
	}
}
