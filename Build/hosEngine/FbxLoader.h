// <Concept>
// Source Name		: FbxLoader.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.08

// <Explanation>
// FBX ������ Load�ϱ� ���� LoaderŬ����.

// Working Log
// 2021.02.09.Hosik	: ����.
// 2021.02.09.Hosik : Node���� Import�� �����Ҵ��� ���̱� ���� Ŭ���� ���� �ӽ� Data���� ����.
// 2021.02.17.Hosik	: Material Load �߰�.
// 2021.02.23.Hosil : Animation Load �߰�
// [2021/03/23] ������ : GameObject Prefab ����� �۾� ���� �۾����� ����� �߰��� �����.
// [2021/04/05] ������ : �Ҹ��� ����

#pragma once

#if defined(_DEBUG)||defined(DEBUG)
#if _WIN64
#pragma comment(lib, "libfbxsdk_x64D")
#pragma comment(lib, "libxml2_x64D")
#pragma comment(lib, "zlib_x64D")
#else
#pragma comment(lib, "libfbxsdk_x86D")
#pragma comment(lib, "libxml2_x86D")
#pragma comment(lib, "zlib_x86D")
#endif // _WIN64
#else
#if _WIN64
#pragma comment(lib, "libfbxsdk_x64R")
#pragma comment(lib, "libxml2_x64R")
#pragma comment(lib, "zlib_x64R")
#else
#pragma comment(lib, "libfbxsdk_x86R")
#pragma comment(lib, "libxml2_x86R")
#pragma comment(lib, "zlib_x86R")
#endif // _WIN64
#endif // defined(_DEBUG)||defined(DEBUG)

#include "fbxsdk.h"
#include <string>
#include <algorithm>
#include <unordered_map>

#include "ModelPrefab.h"
#include "Mesh.h"
#include "Material.h"

#include "AnimationBundle.h"
#include "AnimationClip.h"
#include "BoneAnimation.h"
#include "Keyframe.h"

#define MAX_FULL_VTX 3000000
#define MAX_FACE 1000000

namespace hos
{
	namespace com
	{
		class GameObject;
		class Transform;
	}

	class FbxLoader
	{
		struct BoneInfo
		{
			BYTE BoneIndices;
			float BoneWeight;

			bool operator < (const BoneInfo& rhs)
			{
				return (BoneWeight > rhs.BoneWeight);
			}
		};

		struct CtrlPoint
		{
			DirectX::SimpleMath::Vector3 vPos;
			std::vector<BoneInfo> BoneInfoList;
			hos::string BoneName;

			CtrlPoint()
			{
				BoneInfoList.reserve(4);
			}

			void SortBlendingInfoByWeight()
			{
				std::sort(BoneInfoList.begin(), BoneInfoList.end());
			}
		};

	private:
		std::unordered_map<unsigned int, CtrlPoint*> TempControlPoints;

		// Ŭ������ �ּ�
		std::unordered_map<hos::string, ModelPrefab*>& ModelListRef;
		std::unordered_map<hos::string, Mesh*>& MeshListRef;
		std::unordered_map<hos::string, AnimationBundle*>& AnimationBundleListRef;
		std::unordered_map<hos::string, Material*>& MaterialListRef;
		std::unordered_map<hos::string, bool>& TextureListRef;
		std::unordered_map<hos::string, BoneAnimation*>& UIAnimationRef;

		// Fbx SDK
		FbxManager* Manager;
		FbxIOSettings* IosSetting;
		FbxImporter* Importer;

		// Loading Info
		hos::string FileRoot;
		hos::string FileName;
		hos::string ModelName;
		hos::string ClipName;
		bool bAnimation;
		unsigned int nowBoneCount;

		// Temp Data
		ModelPrefab* TempModel;
		AnimationBundle* BundleRef;
		AnimationClip* TempClip;

		////////////////////////////////////////////////////////////////////////////////
		// �� ��带 Import�� ������ �����Ҵ��� ���� �ʱ� ���� �̸� ������ �����
		// ObjectPool�� ����� ȿ���� ��⸦ �����.
		// ���� ���� Weld�� �ϱ� ���ؼ��� �ӽ� �����͸� ������ �����ϹǷ�,
		// �̸� �ӽõ����͸� ����� ����غ��� �������� �����ϰ� �;���.
		////////////////////////////////////////////////////////////////////////////////

		// ����ȭ�� ���ؽ� ����Ʈ(�ߺ��Ǵ� ���ؽ� ����)
		unsigned int IndexVtxCount;
		//NodeData::INDEXED_VTX tempIndexedVtxList[MAX_INDEXED_VTX];

		// ���ؽ��� �ε��� ����Ʈ
		/// (����ȭ�� ���ؽ� ����Ʈ�� �ش� �ε����� ����ؼ� Ǯ���ؽ��� ����ϴ�.)
		unsigned int FaceCount;
		hos::cg::VertexDataType::FACE_INDEX* tempFaceList;

		// ������ �����ͷ� ä���� Ǯ���ؽ� ����Ʈ
		unsigned int FullVtxCount;
		hos::cg::VertexDataType::FULL_VTX* tempFullVtxList;

		////////////////////////////////////////////////////////////////////////////////


		// ������ �Ҹ���
	public:
		FbxLoader(
			std::unordered_map<hos::string, ModelPrefab*>& modelList,
			std::unordered_map<hos::string, Mesh*>& meshList,
			std::unordered_map<hos::string, AnimationBundle*>& aniBundleList,
			std::unordered_map <hos::string, Material*>& materialList,
			std::unordered_map<hos::string, bool>& TextureList,
			std::unordered_map <hos::string, BoneAnimation*>& uiAnimationRef
		);
		~FbxLoader();

		// �ܺ� ���� �Լ�
	public:
		hos::ModelPrefab* LoadFbxFile(hos::mbstring_view fileRoot);
		hos::BoneAnimation* LoadFbxFile_UIAnimation(hos::mbstring_view fileroot);

		// ���� ��� �Լ�
	private:
		// Data ȹ�� �Լ�
		bool LoadFbxData(FbxScene* fbxScene, FbxNode* fbxRootNode, bool onlyAnimation);
		bool LoadFbxData_UIAnimation(FbxScene* fbxScene, FbxNode* fbxRootNode, BoneAnimation* animationData);

		void CreateBoneDataAndAniClip(bool onlyAnimation, FbxNode* fbxRootNode);
		void CorrectBoneDataCount(bool onlyAnimation);

		// Transform ���� ȹ�� �� �޽�-�� ����
		void CreateMeshData(FbxMesh* fbxMesh, Node* nodeData);

		// ���� ������ ȹ�� ���� �Լ�
		void MakeVertexList(FbxMesh* fbxMesh);
		void CreateControlPoints(FbxNode* fbxNode, FbxMesh* fbxMesh, Mesh* mesh);

		void MakeFullVertexList(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter);
		void ReadNormal(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter);
		void ReadBiNormal(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter);
		void ReadTangent(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter);
		void ReadUV(const FbxMesh* fbxMesh, int fullVtxCount, int index);

		void CopyVtxDataToMesh(Mesh* meshData);

		// Material
		void GetMaterialData(FbxNode* fbxNode, Mesh* nodeData, bool onlyAnimation);
		void GetMaterialAttribute(FbxSurfaceMaterial* fbxMaterial, Material* materialData);
		void GetMaterialTexture(FbxSurfaceMaterial* fbxMaterial, Material* materialData);

		public:
		void CreateMaterialRenderResource(Material* materialData);

		private:

		// BoneAnimation
		FbxAMatrix GetGeometryTransformation(FbxNode* fbxNode);

		// ����۾��� �Լ�
		void ClearTempData();				// �ӽ� ����Ҹ� ����ݴϴ�(�� ��带 �ҷ��� �غ�)
		bool ReadFileName(hos::mbstring_view fileRoot);
		float DegreeToRadian(double degree);	// ���� to ����
		std::wstring GetFileName(std::string string);

		void ConvertFbxMatrixToD3DMatrix(DirectX::SimpleMath::Matrix& dest, FbxAMatrix data);
		void ConvertFbxMatrixToD3DMatrix_Direct(DirectX::SimpleMath::Matrix& dest, FbxAMatrix data);
		void ConvertFbxVector4ToSimpleMathVector3(DirectX::SimpleMath::Vector3& dest, FbxVector4 data, float mutipleValue = 1.0f);
		void ConvertFbxQuatToSimpleMathVector4(DirectX::SimpleMath::Vector4& dest, FbxQuaternion data);


		// ������
		void LoadNodeData(FbxScene* fbxScene, FbxNode* fbxNode, Node* parentsNode, bool onlyAnimation);
		void LoadMeshData(FbxScene* fbxScene, FbxNode* fbxNode, FbxMesh* fbxMesh, Node* parentsNode, bool onlyAnimation);
		void LoadBoneData(FbxScene* fbxScene, FbxNode* fbxNode, Node* parentsNode, bool onlyAnimation);

		void GetAnimation(FbxScene* fbxScene, FbxNode* fbxNode, Node* nodeData, bool onlyAnimation);

		void GetBoneAnimation(FbxScene* fbxScene, FbxNode* fbxNode, Node* nodeData);
		void GetSkinningAnimation(FbxScene* fbxScene, FbxNode* fbxNode, FbxMesh* fbxMesh, Node* nodeData, I32 deformerCount, bool onlyAnimation);
		void GetUIAnimation(FbxScene* fbxScene, FbxNode* fbxNode, BoneAnimation* animationData);

		void GetAllBone(FbxNode* fbxNode);

		void GetSkinningBBox(FbxScene* fbxScene);
	};

}

