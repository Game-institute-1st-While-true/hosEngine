#pragma once
#include <map>
#include "NavDefine.h"
#include "BuildContext.h"
#include "Mesh.h"

// [2021/03/18 ��ȿ��]
// �޽� �����͸� �̿��Ͽ� Recast�� �׺θ޽ø� �����ϴ� Ŭ����

namespace hos
{
	namespace com
	{
		class Scene;
	}
	namespace nv
	{
		class Navigation;

		struct NavMeshSetHeader
		{
			int magic;
			int version;
			int numTiles;
			dtNavMeshParams params;
		};

		struct NavMeshTileHeader
		{
			dtTileRef tileRef;
			int dataSize;
		};

		class NavigationMaker
		{
			enum PartitionType
			{
				WATERSHED,
				MONOTONE,
				LAYERS,
			};

		public:
			NavigationMaker() = delete;
			NavigationMaker(Navigation* nav);
			virtual ~NavigationMaker();

			std::vector<Mesh*>	InputMeshs;
		private:
			// Input Data�� ���ĳ��� ������ (�� ������)
			float* Vtxs;
			unsigned int VtxCount;
			int VtxNow;

			int* FaceBuffers;
			int FaceCount;
			int FaceNow;


			// ��ȯ�� NavMesh ������
		private:
			Navigation* Nav;
			unsigned char* NavFaceBuffers;

			float BoundBoxMin[3];
			float BoundBoxMax[3];

			// NavMesh Bake �� �ɼ�
		private:
			float CellSize;																		// ���� �ʵ带 �����ϴ� Cell�� ���� 
			float CellHeight;																	// ���� �ʵ带 �����ϴ� Cell�� ���� 

			float AgentHeight;																	// ������ ��ü�� �ö� �� �ִ� ��絵
			float AgentRadius;																	// ������ ��ü�� ����
			float AgentMaxClimb;																// ������ ��ü�� �ö� �� �ִ� ����.
			float AgentMaxSlope;																// ���� ��ü�� �ѷ�

			float RegionMinSize;																// �׺���̼� �޽��� ��踦 ��Ÿ���� �ٰ��� �����ڸ��� �ִ� ���̸� ������ �� �ִ�
			float RegionMergeSize;																// �׺���̼� �޽��� �����ڸ��� �ִ��� ���� �޽��� ������ ���� ����� �ش�.
			float EdgeMaxLen;																	// ����Ǿ� ���� ���� �κп� ���ؼ� �������� �׺���̼� �޽��� �������ش�.
			float EdgeMaxError;																	// ���� ������ ���� ������ ū ������ ���ս����ش�.
			float VertsPerPoly;																	// �ٰ����� �̷�� �������� �� 
			float DetailSampleDist;																// �����޽��� ǥ�鿡 �׺���̼� �޽��� ��ġ��ų �� ����ϴ� ���ø� �Ÿ��� �����Ѵ�.
			float DetailSampleMaxError;															// Sample Distance���� �߻��� �� �ִ� ������ �κ��� �� �� �������ϰ� ������ִ� ������ �Ѵ�.

			bool FilterLowHangingObstacles;														// ��ܰ� ���� ���� �����ִ� ��ü�� �� �� �ִ��� ����
			bool FilterLedgeSpans;																// ���߿� �Ŵ޷� �ִ� ������ üũ�� �� ���� ����.
			bool FilterWalkableLowHeightSpans;

			int Type;

		private:
			rcConfig Config;
			rcHeightfield* Solid;																// ���� ������ ��Ÿ���� ���� �����ʵ�
			rcCompactHeightfield* CompactHeightField;											// ���ع����ʴ� ������ ��Ÿ���� ����Ʈ�� ���� �����ʵ�
			rcContourSet* ContourSet;															// ���õ� ���� �׷��� ��Ÿ���ϴ�.
																								// ���� ���� ��� �������� ������ �ּ� ���� �� ũ�⸦ �����մϴ�.

			rcPolyMesh* PolyMesh;																// Ž�� �޽ø� �ۼ��ϴ� �� ����ϱ⿡ ������ �ٰ��� �޽�
			rcPolyMeshDetail* PolyMeshDetail;													// ������ �ٰ��� �޽� ��ü�� �ٰ����� ���õ� �ڼ��� ���� �����͸� ��Ÿ���� �ﰢ�� �޽��� �����մϴ�.
																								// ���� �޽ô� ����� �ٰ��� �޽��� �� �ٰ����� ���� �߰� ���� ���� ������ �����ϴ� �ﰢ�� ���� �޽÷� �����˴ϴ�.

			BuildContext* Context;
			dtNavMesh* NavMesh;

		public:
			bool AddMesh(Mesh* mesh);

			void ClearMesh();
			bool MakeInputData();

			void Bake();
			bool Bake(com::Scene* scene);

			bool Save(mbstring name, const dtNavMesh* mesh);									// �ڽ��� ���� �� ���� �̸��� mesh ������
			bool Load(mbstring name);															// �����õ�����

			dtNavMesh* GetMesh() { return NavMesh; }

		public:
			// get, set
			void SetCellSize(float size) { CellSize = size; }
			float GetCellSize() { return CellSize; }
			void SetCellHeight(float height) { CellHeight = height; }
			float GetCellHeight() { return CellHeight; }

			void SetAgentHeight(float height) { AgentHeight = height; }
			float GetAgentHeight() { return AgentHeight; }
			void SetAgentRadius(float radius) { AgentRadius = radius; }
			float GetAgentRadius() { return AgentRadius; }
			void SetAgentMaxClimb(float climb) { AgentMaxClimb = climb; }
			float GetAgentMaxClimb() { return AgentMaxClimb; }
			void SetAgentMaxSlope(float slope) { AgentMaxSlope = slope; }
			float GetAgentMaxSlope() { return AgentMaxSlope; }

			void SetRegionMinSize(float size) { RegionMinSize = size; }
			float GetRegionMinSize() { return RegionMinSize; }
			void SetRegionMergeSize(float size) { RegionMergeSize = size; }
			float GetRegionMergeSize() { return RegionMergeSize; }

			void SetEdgeMaxLen(float len) { EdgeMaxLen = len; }
			float GetEdgeMaxLen() { return EdgeMaxLen; }
			void SetEdgeMaxError(float err) { EdgeMaxError = err; }
			float GetEdgeMaxError() { return EdgeMaxError; }

			void SetVertsPerPoly(float perpoly) { VertsPerPoly = perpoly; }
			float GetVertsPerPoly() { return VertsPerPoly; }
			void SetDetailSampleDist(float dist) { DetailSampleDist = dist; }
			float GetDetailSampleDist() { return DetailSampleDist; }
			void SetDetailSampleMaxError(float err) { DetailSampleMaxError = err; }
			float GetDetailSampleMaxError() { return DetailSampleMaxError; }

		private:
			void AddVertex(DirectX::SimpleMath::Vector3 vertex, int& cap);
			void AddTriangle(int triA, int triB, int triC, int offset, int& cap);

		private:
			void SettingConfig();																	// Step 1. Initialize build config. ���� ���Ǳ׸� �̴ϼȶ����� �մϴ�.
			bool MakeTriangle();																	// Step 2. Rasterize input polygon soup. ������ ������ �����Ͷ����� �մϴ�.  ** �ٰ��� ������ ����ȭ �۾��� �����ϱ� ���� ����ȭ���� ���� �ٰ����� ����
			void FilteringWalkableSurfaces();														// Step 3. ���� �� �ִ� ������ ���͸� �մϴ�.
			bool ParttionWalkableSurfacesSimple();													// Step 4. ���� ���ִ� ǥ���� �ܼ��� �������� �����մϴ�.
			bool TraceAndSimplifyRgionContours();													// Step 5. ���� ������ �����ϰ� �ܼ�ȭ�մϴ�.
			bool CreatePolyMesh();																	// Step 6. ���������� �ٰ��� �޽��� ����ϴ�.
			bool CreatePolyMeshDetail();															// Step 7. �� �ٰ����� �뷫���� ���̿� �׼��� �� ���ִ� ���� �޽ø� ����ϴ�.
		};
	}
}

