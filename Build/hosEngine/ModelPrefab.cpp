#include "ModelPrefab.h"

hos::ModelPrefab::ModelPrefab(hos::string modelName)
	:ModelName(modelName), RootNode(nullptr), bSkin(false)
{
	MeshList.reserve(10);
	BoneList.reserve(50);
}

hos::ModelPrefab::~ModelPrefab()
{
	// BoneList Delete
	/// Mesh Data는 DataManager에서 관리합니다.
	for (int i = 0; i < BoneList.size(); i++)
	{
		SafeDelete(BoneList[i]);
	}
	BoneList.clear();
	BoneList.shrink_to_fit();

	// MeshList Clear
	for (int i = 0; i < MeshList.size(); i++)
	{
		SafeDelete(MeshList[i]);
	}
	MeshList.clear();
	MeshList.shrink_to_fit();

	SafeDelete(RootNode);
}

void hos::ModelPrefab::AddMesh(Node* meshData)
{
	MeshList.push_back(meshData);
}

void hos::ModelPrefab::AddBone(Node* boneData)
{
	//// 처음 등록되는 뼈라면 RootBone으로 등록해준다.
	//if (BoneList.size() == 0)
	//{
	//	this->RootBoneData = boneData;
	//}

	BoneList.push_back(boneData);
}
