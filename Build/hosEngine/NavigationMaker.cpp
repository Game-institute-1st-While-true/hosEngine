#include "GameObject.h"

#include "NavigationMaker.h"
#include "Navigation.h"
#include "Detour/DetourNavMesh.h"
#include "Scene.h"
#include "MeshFilter.h"
#include "Transform.h"

hos::nv::NavigationMaker::NavigationMaker(Navigation* nav) :
	Vtxs(nullptr), VtxCount(0), VtxNow(0), FaceBuffers(nullptr), FaceCount(0), FaceNow(0), NavFaceBuffers(nullptr), CellSize(DEFAULT_CELL_SIZE), CellHeight(DEFAULT_CELL_HEIGHT), AgentHeight(DEFAULT_AGENT_HEIGHT),
	AgentRadius(DEFAULT_AGENT_RAIDUS), AgentMaxClimb(DEFAULT_AGENT_MAX_CLIMB), AgentMaxSlope(DEFAULT_AGENT_MAX_SLOPE), RegionMinSize(DEFAULT_REGION_MIN_SIZE), RegionMergeSize(DEFAULT_REGION_MERGE_SIZE),
	EdgeMaxLen(DEFAULT_EDGE_MAX_LEN), EdgeMaxError(DEFAULT_EDGE_MAX_ERROR), VertsPerPoly(DEFAULT_VERTSPERPOLY), DetailSampleDist(DEFAULT_DETAIL_SAMPLE_DIST), DetailSampleMaxError(DEFAULT_DETAIL_SAMPLE_MAX_ERROR),
	Context(nullptr), Type(0), Nav(nav), FilterLowHangingObstacles(true), FilterLedgeSpans(true), FilterWalkableLowHeightSpans(true)
{
	Context = Nav->Context;
}

hos::nv::NavigationMaker::~NavigationMaker()
{
}

bool hos::nv::NavigationMaker::AddMesh(Mesh* mesh)
{
	if (mesh == nullptr)
	{
		return false;
	}
	InputMeshs.emplace_back(mesh);
	return true;
}

void hos::nv::NavigationMaker::ClearMesh()
{
	InputMeshs.clear();

	//delete[] Vtxs;
	//delete[] FaceBuffers;
	//delete[] NavFaceBuffers;
}

bool hos::nv::NavigationMaker::MakeInputData()
{
	// 모든 count 를 더함.
	for (Mesh* mesh : InputMeshs)
	{
		VtxCount += mesh->GetVtxCount();
		FaceCount += mesh->GetIndexCount();
	}

	// 버퍼를 넣어줄 공간 생성.
	int vcap = 0;
	int tcap = 0;

	for (Mesh* mesh : InputMeshs)
	{
		int vtx_size = VtxCount;
		hos::cg::VertexDataType::FULL_VTX* _vtxs = reinterpret_cast<hos::cg::VertexDataType::FULL_VTX*>(mesh->GetVtxDataAdress());

		for (int i = 0; i < vtx_size; ++i)
		{
			AddVertex((_vtxs + i)->vPos, vcap);
		}

		hos::U32* _Idxs = reinterpret_cast<hos::U32*>(mesh->GetIndexDataAdress());

		int offset = FaceNow;
		int idx_size = mesh->GetIndexCount();

		for (int j = 0; j < idx_size; j += 3)
		{
			AddTriangle(_Idxs[j + 0], _Idxs[j + 1], _Idxs[j + 2], offset, tcap);
		}
	}

	VtxCount = VtxNow;
	FaceCount = FaceNow;

	rcCalcBounds(Vtxs, VtxCount, BoundBoxMin, BoundBoxMax);
	return true;
}

void hos::nv::NavigationMaker::Bake()
{
	SettingConfig();
	MakeTriangle();
	FilteringWalkableSurfaces();
	ParttionWalkableSurfacesSimple();
	TraceAndSimplifyRgionContours();
	CreatePolyMesh();
	CreatePolyMeshDetail();

	dtNavMeshCreateParams params;

	memset(&params, 0, sizeof(params));
	params.verts = PolyMesh->verts;
	params.vertCount = PolyMesh->nverts;
	params.polys = PolyMesh->polys;
	params.polyAreas = PolyMesh->areas;
	params.polyFlags = PolyMesh->flags;
	params.polyCount = PolyMesh->npolys;
	params.nvp = PolyMesh->nvp;
	params.detailMeshes = PolyMeshDetail->meshes;
	params.detailVerts = PolyMeshDetail->verts;
	params.detailVertsCount = PolyMeshDetail->nverts;
	params.detailTris = PolyMeshDetail->tris;
	params.detailTriCount = PolyMeshDetail->ntris;

	params.walkableHeight = AgentHeight;
	params.walkableRadius = AgentRadius;
	params.walkableClimb = AgentMaxClimb;

	rcVcopy(params.bmin, PolyMesh->bmin);
	rcVcopy(params.bmax, PolyMesh->bmax);
	params.cs = Config.cs;
	params.ch = Config.ch;
	params.buildBvTree = true;

	unsigned char* navData = 0;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))							// Config 데이터를 기반하여 MeshData를 만듭니다.
	{
		Context->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
		return;
	}

	NavMesh = dtAllocNavMesh();															// 네부메시 포인터를 할당합니다.
	if (!NavMesh)
	{
		dtFree(navData);
		Context->log(RC_LOG_ERROR, "Could not create Detour navmesh");
		return;
	}

	dtStatus status;

	status = NavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		dtFree(NavMesh);
		Context->log(RC_LOG_ERROR, "Could not init Detour navmesh");
		return;
	}
	ClearMesh();
	return;
}

bool hos::nv::NavigationMaker::Bake(com::Scene* scene)
{
	std::vector<float> vertices;
	std::vector<int> indices;
	int indexCount = 0;
	for (com::GameObject* obj : scene->GetObjects())
	{
		//게임오브젝트에 메쉬 데이터가 있을때
		if (obj->meshfilter)
		{
			//메쉬가 맵데이터에 포함되지않는 메쉬임
			if (obj->rigidbody /*추가->에니메이션*/)
			{
				continue;
			}
			Mesh* tmp = obj->meshfilter->GetMeshData();
			if (nullptr == tmp)
			{
			//메쉬필터가 있지만 메쉬가 없음
				continue;
			}
			for (U32 i = 0; i < tmp->GetVtxCount(); i++)
			{
				Vector3 pos = tmp->GetVtxDataAdress()[i].vPos * tmp->GetGeoTM() * tmp->GetNodeTM() * obj->transform->GetTransformMatrix();
				vertices.emplace_back(pos.x);
				vertices.emplace_back(pos.y);
				vertices.emplace_back(pos.z);
			}
			for (U32 i = 0; i < tmp->GetIndexCount(); i++)
			{
				indices.emplace_back(tmp->GetIndexDataAdress()[i] + indexCount);
			}
			indexCount += tmp->GetIndexCount();
		}
	}

	Vtxs = vertices.data();
	VtxCount = (U32)(vertices.size() / 3);

	FaceBuffers = indices.data();
	FaceCount = (U32)(indices.size() / 3);

	if (VtxCount <= 0 || FaceCount <= 0)
	{
		return false;
	}

	rcCalcBounds(Vtxs, VtxCount, BoundBoxMin, BoundBoxMax);

	SettingConfig();
	MakeTriangle();
	FilteringWalkableSurfaces();
	ParttionWalkableSurfacesSimple();
	TraceAndSimplifyRgionContours();
	CreatePolyMesh();
	CreatePolyMeshDetail();

	dtNavMeshCreateParams params;

	memset(&params, 0, sizeof(params));
	params.verts = PolyMesh->verts;
	params.vertCount = PolyMesh->nverts;
	params.polys = PolyMesh->polys;
	params.polyAreas = PolyMesh->areas;
	params.polyFlags = PolyMesh->flags;
	params.polyCount = PolyMesh->npolys;
	params.nvp = PolyMesh->nvp;
	params.detailMeshes = PolyMeshDetail->meshes;
	params.detailVerts = PolyMeshDetail->verts;
	params.detailVertsCount = PolyMeshDetail->nverts;
	params.detailTris = PolyMeshDetail->tris;
	params.detailTriCount = PolyMeshDetail->ntris;

	params.walkableHeight = AgentHeight;
	params.walkableRadius = AgentRadius;
	params.walkableClimb = AgentMaxClimb;

	rcVcopy(params.bmin, PolyMesh->bmin);
	rcVcopy(params.bmax, PolyMesh->bmax);
	params.cs = Config.cs;
	params.ch = Config.ch;
	params.buildBvTree = true;

	unsigned char* navData = 0;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))							// Config 데이터를 기반하여 MeshData를 만듭니다.
	{
		Context->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
		return false;
	}

	NavMesh = dtAllocNavMesh();															// 네부메시 포인터를 할당합니다.
	if (!NavMesh)
	{
		dtFree(navData);
		Context->log(RC_LOG_ERROR, "Could not create Detour navmesh");
		return false;
	}

	dtStatus status;

	status = NavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		dtFree(NavMesh);
		Context->log(RC_LOG_ERROR, "Could not init Detour navmesh");
		return false;
	}
	ClearMesh();
	return true;
}

bool hos::nv::NavigationMaker::Save(mbstring name, const dtNavMesh* mesh)
{
	if (!mesh) return false;
	FILE* fp;
	fopen_s(&fp, name.c_str(), "wb");
	if (!fp) return false;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}

	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}
	fclose(fp);
	return true;
}

bool hos::nv::NavigationMaker::Load(mbstring name)
{
	FILE* fp;
	fopen_s(&fp, name.c_str(), "rb");
	if (!fp) return false;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return false;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return false;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return false;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return false;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return false;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return false;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return false;
		}
		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	NavMesh = mesh;
	return true;
}

void hos::nv::NavigationMaker::AddVertex(DirectX::SimpleMath::Vector3 vertex, int& cap)
{
	if (VtxNow + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		float* nv = new float[cap * 3];
		if (VtxNow)
			memcpy(nv, Vtxs, VtxNow * 3 * sizeof(float));
		delete[] Vtxs;
		Vtxs = nv;
	}
	float* dst = &Vtxs[VtxNow * 3];
	*dst++ = vertex.x;
	*dst++ = vertex.y;
	*dst++ = vertex.z;
	VtxNow++;
}

void hos::nv::NavigationMaker::AddTriangle(int triA, int triB, int triC, int offset, int& cap)
{
	if (FaceNow + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		int* nv = new int[cap * 3];
		if (FaceNow)
			memcpy(nv, FaceBuffers, FaceNow * 3 * sizeof(int));
		delete[] FaceBuffers;
		FaceBuffers = nv;
	}
	int* dst = &FaceBuffers[FaceNow * 3];
	*dst++ = triA + offset;
	*dst++ = triB + offset;
	*dst++ = triC + offset;
	FaceNow++;
}

void hos::nv::NavigationMaker::SettingConfig()
{
	memset(&Config, 0, sizeof(Config));
	Config.cs = CellSize;
	Config.ch = CellHeight;
	Config.walkableSlopeAngle = AgentMaxSlope;
	Config.walkableHeight = (int)ceilf(AgentHeight / Config.ch);
	Config.walkableClimb = (int)floorf(AgentMaxClimb / Config.ch);
	Config.walkableRadius = (int)ceilf(AgentRadius / Config.cs);
	Config.maxEdgeLen = (int)(EdgeMaxLen / CellSize);
	Config.maxSimplificationError = EdgeMaxError;
	Config.minRegionArea = (int)rcSqr(RegionMinSize);		// Note: area = size*size
	Config.mergeRegionArea = (int)rcSqr(RegionMergeSize);	// Note: area = size*size
	Config.maxVertsPerPoly = (int)VertsPerPoly;
	Config.detailSampleDist = DetailSampleDist < 0.9f ? 0 : CellSize * DetailSampleDist;
	Config.detailSampleMaxError = CellHeight * DetailSampleMaxError;

	// 내비게이션이 빌드 될 영역을 설정합니다.
	// GemotryData에서 BoundBox의 값을 가져옵니다. (박스의 최소 좌표 최대 좌표 x, y, z)
	// 지금은 입력된 GeometryData의 바운드 박스의 경계가 사용되지만 값을 수정할 수 있습니다.

	rcVcopy(Config.bmin, BoundBoxMin);
	rcVcopy(Config.bmax, BoundBoxMax);

	// 바운드 박스의 크기에 맞는 그리드를 계산합니다.
	rcCalcGridSize(Config.bmin, Config.bmax, Config.cs, &Config.width, &Config.height);

	// 디버그 정보, 현재 사용하지 않습니다.
	// Reset build times gathering.
	Context->resetTimers();

	// Start the build process.	
	//Context->startTimer(RC_TIMER_TOTAL);

	Context->log(RC_LOG_PROGRESS, "Building navigation:");
	Context->log(RC_LOG_PROGRESS, " - %d x %d cells", Config.width, Config.height);
	Context->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", VtxCount / 1000.0f, FaceCount / 1000.0f);


}

bool hos::nv::NavigationMaker::MakeTriangle()
{
	// 입력 데이터를 resterizing 하는 곳에 복쉘 높이맵을 할당합니다.
	Solid = rcAllocHeightfield();

	if (Solid == nullptr)
	{
		return false;
	}
	if (!rcCreateHeightfield(Context, *Solid, Config.width, Config.height, Config.bmin, Config.bmax, Config.cs, Config.ch))
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// 삼각형 영역을 보유할 수 있는 배열을 할당합니다.
	// 처리해야 하는 Mesh가 여러 개 있는 경우 처리해야하는 최대 삼각형 수를 저장할 수 있는 배열입니다.

	NavFaceBuffers = new unsigned char[FaceCount];
	memset(NavFaceBuffers, 0, FaceCount);
	if (NavFaceBuffers == nullptr)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", FaceCount);
		return false;
	}

	memset(NavFaceBuffers, 0, sizeof(unsigned char) * FaceCount);

	// 경사도에 따라 걸을 수있는 삼각형을 찾아 Rasterize 합니다.
	// 입력 데이터가 여러 메쉬 인 경우 여기에서 변환하고 계산할 수 있습니다.
	// 각 메쉬에 대한 유형을 지정하고 래스터 화합니다.

	rcMarkWalkableTriangles(Context,
		Config.walkableSlopeAngle,
		Vtxs,
		VtxCount,
		FaceBuffers,
		FaceCount,
		NavFaceBuffers);

	bool result = rcRasterizeTriangles(
		Context,
		Vtxs,
		VtxCount,
		FaceBuffers,
		NavFaceBuffers,
		FaceCount,
		*Solid,
		Config.walkableClimb);



	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
		return false;
	}
	return true;
}

void hos::nv::NavigationMaker::FilteringWalkableSurfaces()
{
	if (FilterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(Context, Config.walkableClimb, *Solid);
	if (FilterLedgeSpans)
		rcFilterLedgeSpans(Context, Config.walkableHeight, Config.walkableClimb, *Solid);
	if (FilterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(Context, Config.walkableHeight, *Solid);
}

bool hos::nv::NavigationMaker::ParttionWalkableSurfacesSimple()
{
	// 더 빠르게 처리 할 수 있도록 높이 필드를 압축합니다.
	// 이것은 주변 뿐 아니라 더 많은 캐시 일관성 데이터를 생성 합니다.
	// 걸어 갈 수있는 셀 사이가 계산됩니다.

	CompactHeightField = rcAllocCompactHeightfield();
	if (CompactHeightField == nullptr)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return false;
	}

	bool result = false;
	result = rcBuildCompactHeightfield(Context,
		Config.walkableHeight,
		Config.walkableClimb,
		*Solid,
		*CompactHeightField
	);

	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return false;
	}

	// 가상 에이전트 반경으로 걸어갈 수 있는 영역을 침식합니다.
	result = rcErodeWalkableArea(Context,
		Config.walkableRadius,
		*CompactHeightField);

	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return false;
	}

	/// CHECK : 필요한지 모르겠다.
	// (Optional) 영역을 마크 합니다. 지정된 볼록 다각형 내의 모든 범위에 영역 ID를 적용합니다.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);
	///


	// 나중에 간단한 알고리즘을 사용하여 걸을 수있는 영역을 삼각 측량 할 수 있도록 높이 필드를 분할합니다.
	// 	3 가지 무술 방법이 있으며 각각 장단점이 있습니다.
	// 	1) 유역 분할
	// 	- 고전적인 Recast 파티셔닝
	// 	- 가장 멋진 테셀레이션을 만듭니다.
	// 	- 일반적으로 가장 느림
	// 	- 높이 필드를 구멍이나 겹침없이 멋진 영역으로 분할
	// 	- 이 방법으로 인해 구멍과 겹침이 생성되는 몇 가지 코너 케이스가 있습니다.
	// 	- 작은 장애물이 넓은 개방 영역에 가까울 때 구멍이 나타날 수 있습니다(삼각 측량으로 처리 할 수 ​​있음).
	// 	- 좁은 나선형 복도(예 : 계단)가있는 경우 겹침이 발생할 수 있으며, 이로 인해 삼각 측량이 실패합니다.
	// 	* 일반적으로 nacmesh를 미리 계산하는 경우 가장 좋은 선택, 넓은 개방 영역이있는 경우 사용

	// 	2) 모노톤 분할
	// 	- 가장 빠름
	// 	- 높이 필드를 구멍과 겹침이없는 영역으로 분할(보장)
	// 	- 길고가는 폴리곤을 생성하여 때때로 우회 경로를 유발합니다.
	// 	* 빠른 내비 메시 생성을 원한다면 이것을 사용하십시오

	// 	3) 레이어 분할
	// 	- 매우 빠름
	// 	- heighfield를 겹치지 않는 영역으로 분할
	// 	- 구멍에 대처하기 위해 삼각 측량 코드에 의존(따라서 모노톤 분할보다 느림)
	// 	- 모노톤 분할보다 더 나은 삼각형 생성
	// 	- 유역 분할의 코너 케이스가 없습니다.
	// 	- 느리고 약간 못생긴 테셀레이션을 만들 수 있습니다(모노톤보다 여전히 좋습니다).
	// 	작은 장애물이있는 넓은 열린 공간이있는 경우(타일을 사용하면 문제가되지 않음)
	// 	* 중형 및 소형 타일이있는 타일 내비 메시에 사용하기에 좋은 선택

	switch (Type)
	{
	case PartitionType::WATERSHED:
		// 걸어 갈 수있는 표면을 따라 거리 필드를 계산하여 영역 분할을 준비합니다.
		if (!rcBuildDistanceField(Context, *CompactHeightField))
		{
			Context->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return false;
		}
		// 보행 가능한 표면을 구멍이없는 단순한 영역으로 분할합니다.
		if (!rcBuildRegions(Context, *CompactHeightField, 0, Config.minRegionArea, Config.mergeRegionArea))
		{
			Context->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
			return false;
		}
		break;
	case PartitionType::MONOTONE:
		//  보행 가능한 표면을 구멍이없는 단순한 영역으로 분할합니다.
		// 	모노톤 분할에는 distancefield가 필요하지 않습니다.
		if (!rcBuildRegionsMonotone(Context, *CompactHeightField, 0, Config.minRegionArea, Config.mergeRegionArea))
		{
			Context->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
			return false;
		}
		break;
	case PartitionType::LAYERS:
		// 보행 가능한 표면을 구멍이없는 단순한 영역으로 분할합니다.
		if (!rcBuildLayerRegions(Context, *CompactHeightField, 0, Config.minRegionArea))
		{
			Context->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
			return false;
		}
		break;
	}
	return true;
}

bool hos::nv::NavigationMaker::TraceAndSimplifyRgionContours()
{
	ContourSet = rcAllocContourSet();
	if (ContourSet == nullptr)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'ContourSet'.");
		return false;
	}

	bool result = false;
	result = rcBuildContours(Context,
		*CompactHeightField,
		Config.maxSimplificationError,
		Config.maxEdgeLen,
		*ContourSet);

	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return false;
	}
	return true;
}

bool hos::nv::NavigationMaker::CreatePolyMesh()
{
	// 윤곽선에서 다각형 내비 메시를 만듭니다.
	PolyMesh = rcAllocPolyMesh();
	if (PolyMesh == nullptr)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}

	bool result = false;
	result = rcBuildPolyMesh(Context,
		*ContourSet,
		Config.maxVertsPerPoly,
		*PolyMesh);

	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return false;
	}
	return true;
}

bool hos::nv::NavigationMaker::CreatePolyMeshDetail()
{
	PolyMeshDetail = rcAllocPolyMeshDetail();
	if (PolyMeshDetail == nullptr)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	bool result = false;
	result = rcBuildPolyMeshDetail(Context,
		*PolyMesh,
		*CompactHeightField,
		Config.detailSampleDist,
		Config.detailSampleMaxError,
		*PolyMeshDetail);

	if (result == false)
	{
		Context->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (Config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < PolyMesh->npolys; ++i)
		{
			if (PolyMesh->areas[i] == RC_WALKABLE_AREA)
				PolyMesh->areas[i] = POLYAREA_GROUND;

			if (PolyMesh->areas[i] == POLYAREA_GROUND ||
				PolyMesh->areas[i] == POLYAREA_GRASS ||
				PolyMesh->areas[i] == POLYAREA_ROAD)
			{
				PolyMesh->flags[i] = POLYFLAGS_WALK;
			}
			else if (PolyMesh->areas[i] == POLYAREA_WATER)
			{
				PolyMesh->flags[i] = POLYFLAGS_SWIM;
			}
			else if (PolyMesh->areas[i] == POLYAREA_DOOR)
			{
				PolyMesh->flags[i] = POLYFLAGS_WALK | POLYFLAGS_DOOR;
			}
		}
	}
	return true;
}