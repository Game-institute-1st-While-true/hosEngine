// ���� ���
#include "Object.h"
#include "DXTK\SimpleMath.h"
#include <DirectXMath.h>
#include "hosUtility\hosUtility.h"
#include <tchar.h>

#include <intsafe.h>

// �ִϸ��̼�
#include "AnimationClip.h"
#include "BoneAnimation.h"
#include "Keyframe.h"

// �ڱ� ���
#include "FbxLoader.h"
using namespace DirectX::SimpleMath;
using namespace std;

hos::FbxLoader::FbxLoader(
	std::unordered_map<hos::string, hos::ModelPrefab*>& modelList,
	std::unordered_map<hos::string, hos::Mesh*>& meshList,
	std::unordered_map<hos::string, hos::AnimationBundle*>& aniBundleList,
	std::unordered_map <hos::string, hos::Material*>& materialList,
	std::unordered_map<hos::string, bool>& TextureList,
	std::unordered_map <hos::string, BoneAnimation*>& uiAnimationRef
)
	: ModelListRef(modelList), MeshListRef(meshList), AnimationBundleListRef(aniBundleList), MaterialListRef(materialList),
	TextureListRef(TextureList), UIAnimationRef(uiAnimationRef),
	Manager(nullptr), IosSetting(nullptr), Importer(nullptr),
	FileRoot(L""), ModelName(L""), ClipName(L""), bAnimation(true), nowBoneCount(0),
	TempModel(nullptr), BundleRef(nullptr), TempClip(nullptr),
	FullVtxCount(0), FaceCount(0), IndexVtxCount(0)
{
	// Fbx�Ŵ��� ����
	this->Manager = FbxManager::Create();

	// �Ŵ����� ios�� ���ε�
	this->IosSetting = FbxIOSettings::Create(Manager, IOSROOT);
	this->Manager->SetIOSettings(IosSetting);

	// ������ ����
	this->Importer = FbxImporter::Create(Manager, "");

	// �̸� �Ҵ�� ������ �迭 ����
	this->tempFaceList = new hos::cg::VertexDataType::FACE_INDEX[MAX_FACE];
	this->tempFullVtxList = new hos::cg::VertexDataType::FULL_VTX[MAX_FULL_VTX];

	Debug->LogConsole(L"System", L"FBX_Loader�� �����߽��ϴ�.");
}

hos::FbxLoader::~FbxLoader()
{
	this->FileRoot = L"";
	this->FileName = L"";
	this->ModelName = L"";
	this->ClipName = L"";
	this->TempModel = nullptr;

	SafeDeleteArray(this->tempFaceList);
	SafeDeleteArray(this->tempFullVtxList);

	// ControlPoint �ӽ� ����� ����� ����
	for (auto [i, point] : TempControlPoints)
	{
		SafeDelete(point);
	}
	TempControlPoints.clear();

	SafeDelete(TempModel);
	SafeDelete(BundleRef);
	SafeDelete(TempClip);

	// Importer ����
	if (this->Importer)
	{
		this->Importer->Destroy();
		this->Importer = nullptr;
	}

	// IosSetting ����
	if (this->IosSetting)
	{
		this->IosSetting->Destroy();
		this->IosSetting = nullptr;
	}

	// Fbx�Ŵ��� ����
	if (this->Manager)
	{
		this->Manager->Destroy();
		this->Manager = nullptr;
	}
	Debug->LogConsole(L"DataManager", L"FBX_Loader�� �����߽��ϴ�.");
}

// Fbx���� �δ�
hos::ModelPrefab* hos::FbxLoader::LoadFbxFile(hos::mbstring_view fileRoot)
{
	// �ӽ� ������ �ʱ�ȭ
	this->ClearTempData();

	// ���� ��� �� �̸� ȹ��
	if (this->ReadFileName(fileRoot) == false)
	{
		Debug->LogConsole(L"System", L"Fbx���� Load�� �����߽��ϴ�.");
		return nullptr;
	}

	// �����ͷ� ���� �ε�
	bool status = this->Importer->Initialize(fileRoot.data(), -1, Manager->GetIOSettings());

	// ������ ����
	if (!status)
	{
		Debug->LogConsole(L"DataManager", L"FbxImporter�� �ش� ���� �ε忡 �����߽��ϴ�.");
		return nullptr;
	}

	Debug->LogConsole(L"DataManager", L"�� �ε带 �����մϴ�.");

	// ���� �����ϰ� �����Ϳ� ����Ʈ
	FbxScene* scene = FbxScene::Create(Manager, "scene");
	this->Importer->Import(scene);

	// �ӽ� �� ���� �� ��Ʈ ������ �Ľ� ����
	this->TempModel = new ModelPrefab(this->ModelName);
	this->TempModel->bAni = this->bAnimation;
	FbxNode* RootNode = scene->GetRootNode();

	// �ִϸ��̼��� �ִ� ���
	if (this->TempModel->bAni)
	{
		// �ش� ���� ������ ���ٸ� ������ְ�, �޽��� �ҷ��ش�.
		if (AnimationBundleListRef.count(this->ModelName) == 0)
		{
			AnimationBundle* bundle = new AnimationBundle(this->ModelName);
			AnimationBundleListRef.insert({ this->ModelName, bundle });
			bundle = nullptr;

			LoadFbxData(scene, RootNode, false);
		}

		// ������ �ִٸ�, ���鵵 �� �����, �޽��� �� �θ���.
		else
		{
			LoadFbxData(scene, RootNode, true);
		}

	}

	// �ִϸ��̼��� ���� ���
	else
	{
		LoadFbxData(scene, RootNode, false);
	}

	// ���ҽ� �Ŵ����� '��' Ŭ������ ����.
	ModelListRef.insert({ this->ModelName, this->TempModel });

	// ��Ʈ ��� ����
	RootNode->Destroy();

	// �� ����
	scene->Destroy();

	// TODO: �����ʹ� Ŭ���� �� �־�� �ϳ�?

	Debug->LogConsole(L"DataManager", ModelName + L"�� �ε忡 �����߽��ϴ�.");

	// �ӽ� ������ �ʱ�ȭ
	ModelPrefab* temp = this->TempModel;
	this->ClearTempData();

	return temp;
}

hos::BoneAnimation* hos::FbxLoader::LoadFbxFile_UIAnimation(hos::mbstring_view fileRoot)
{
	// �ӽ� ������ �ʱ�ȭ
	this->ClearTempData();


	// ���� ��� �� �̸� ȹ��
	if (this->ReadFileName(fileRoot) == false)
	{
		Debug->LogConsole(L"DataManager", FileRoot + L"Fbx���� Load�� �����߽��ϴ�.");
		return nullptr;
	}

	// �����ͷ� ���� �ε�
	bool status = this->Importer->Initialize(fileRoot.data(), -1, Manager->GetIOSettings());

	// ������ ����
	if (!status)
	{
		Debug->LogConsole(L"DataManager", FileRoot + L"FbxImporter�� �ش� ���� �ε忡 �����߽��ϴ�.");
		return nullptr;
	}

	Debug->LogConsole(L"DataManager", ModelName + L"UI Animation �ε带 �����մϴ�.");

	// ���� �����ϰ� �����Ϳ� ����Ʈ
	FbxScene* scene = FbxScene::Create(Manager, "scene");
	this->Importer->Import(scene);

	// �ӽ� ��ü ���� �� ��Ʈ ������ �Ľ� ����
	BoneAnimation* TempAnimation = new BoneAnimation();
	FbxNode* RootNode = scene->GetRootNode();

	// �ε� ����
	this->LoadFbxData_UIAnimation(scene, RootNode, TempAnimation);

	// ���ҽ� �Ŵ����� '��' Ŭ������ ����.
	this->UIAnimationRef.insert({ this->ModelName, TempAnimation });

	// ��Ʈ ��� �� �� ����
	RootNode->Destroy();
	scene->Destroy();

	Debug->LogConsole(L"DataManager", ModelName + L"UI_Animation �ε忡 �����߽��ϴ�.");

	// �ӽ� ������ �ʱ�ȭ
	this->ClearTempData();

	return TempAnimation;
}

bool hos::FbxLoader::LoadFbxData(FbxScene* fbxScene, FbxNode* fbxRootNode, bool onlyAnimation)
{
	////////////////////////////	��ȿ�� �˻�		////////////////////////////

	// Node�� Mesh����, Bone���� �Ǵ��ϱ� ���� Attribute�� ���´�.
	FbxNodeAttribute* nodeAttribute = fbxRootNode->GetNodeAttribute();

	// ����� �̸��� ��´�.
	std::string NodeName = fbxRootNode->GetName();

	// fbx������ ��Ʈ����� ���
	if (NodeName == "RootNode")
	{
		int ChildCount = fbxRootNode->GetChildCount();

		// RootNode ����(�ӽ��� �ӽÿ�)
		Node* _rootNode = new Node();
		_rootNode->NodeName = L"RootNode";
		_rootNode->NodeType = Node::NODE_TYPE::ROOT;
		this->TempModel->RootNode = _rootNode;

		// �ִϸ��̼��� �ִ� ��� Bundle Load ��, Clip�� ����
		if (this->TempModel->bAni)
		{
			this->CreateBoneDataAndAniClip(onlyAnimation, fbxRootNode);
		}

		// Fbx RootNode ������ fbx�޽����� �ε��Ѵ�.
		for (int i = 0; i < ChildCount; i++)
		{
			FbxNode* fbxChildNode = fbxRootNode->GetChild(i);
			LoadNodeData(fbxScene, fbxChildNode, _rootNode, onlyAnimation);
		}

		// BoneData�� ��ȿ���� �˻��մϴ�.
		if (this->TempModel->bAni)
		{
			this->CorrectBoneDataCount(onlyAnimation);
		}

		// ��Ű�� ���� ��� Skinning�� BoundingBox ���ϱ�
		if (this->TempModel->bSkin)
		{
			this->GetSkinningBBox(fbxScene);
		}
	}

	return true;
}

bool hos::FbxLoader::LoadFbxData_UIAnimation(FbxScene* fbxScene, FbxNode* fbxRootNode, hos::BoneAnimation* animationData)
{
	////////////////////////////	��ȿ�� �˻�		////////////////////////////

	// Node�� Mesh����, Bone���� �Ǵ��ϱ� ���� Attribute�� ���´�.
	FbxNodeAttribute* nodeAttribute = fbxRootNode->GetNodeAttribute();

	// ����� �̸��� ��´�.
	std::string NodeName = fbxRootNode->GetName();

	// fbx������ ��Ʈ����� ���
	if (NodeName == "RootNode")
	{
		// ��� ���� �˻�
		int ChildCount = fbxRootNode->GetChildCount();
		if (ChildCount != 1)
		{
			Debug->LogConsole(L"DataManager", ModelName + L"UI Animation Load ����! UI�� Node�� 1������ �մϴ�.");
			return false;
		}

		// �ִϸ��̼� �ε�
		FbxNode* fbxChildNode = fbxRootNode->GetChild(0);
		this->GetUIAnimation(fbxScene, fbxChildNode, animationData);
	}

	return true;
}

void hos::FbxLoader::CreateBoneDataAndAniClip(bool onlyAnimation, FbxNode* fbxRootNode)
{
		// Bundle Load
		this->BundleRef = AnimationBundleListRef.at(this->ModelName);

		// Animation�� �ְ�, �ִϸ��̼Ǹ� �ҷ����°� �ƴ� ��� Bundle�� �������� ������Ʈ ���ش�.
		if (this->TempModel->bAni && onlyAnimation == false)
		{
			int childCount = fbxRootNode->GetChildCount();

			for (int i = 0; i < childCount; i++)
			{
				FbxNode* childFbxNode = fbxRootNode->GetChild(i);

				this->GetAllBone(childFbxNode);
			}
		}

		// Clip Create
		this->TempClip = new AnimationClip(this->ClipName);
		this->TempClip->SetBoneAnimationSize(150);	// �ϴ� 150��(MAX)
}

void hos::FbxLoader::CorrectBoneDataCount(bool onlyAnimation)
{
	// Bundle�� �� ������ŭ Clip�� BoneList Vector ������ ����
	hos::I32 boneCoint = (hos::I32)this->BundleRef->GetBoneListSize();

	this->TempClip->SetBoneAnimationSize(boneCoint);
	this->BundleRef->AddClip(this->ClipName, this->TempClip);

	// �ִϸ��̼Ǹ� �ҷ����°� �ƴ� ��� Bundle�� BoneOff �������� �������ش�.
	if (onlyAnimation == false)
	{
		this->BundleRef->ResizeBoneOffSetList(this->nowBoneCount);
	}
}

void hos::FbxLoader::CreateMeshData(FbxMesh* fbxMesh, Node* nodeData)
{
	Mesh* _meshData = nodeData->MeshData;

	// FullVtxList�� �����.
	MakeVertexList(fbxMesh);

	// �����丵(���� �� ����)
	/// Weld �ϵ縻�� ���⼭ �������־�� �Ѵ�.
	CopyVtxDataToMesh(nodeData->MeshData);

	// ���� �� tempFullVtxList û��
	for (unsigned int i = 0; i < FullVtxCount; i++)
	{
		memset(&tempFullVtxList[i], 0, sizeof(hos::cg::VertexDataType::FULL_VTX));
	}

	// Resource Manager�� ���
	bool bSuccess = MeshListRef.insert({ _meshData->MeshName, _meshData }).second;

	// Resource Manage���� ���� Key�� Mesh�� �̹� �ִ� ��� �Ĺ̿� _1.. _2.._3...�� �ٿ� ���̹� ���ݴϴ�.
	int _addNum = 0;
	hos::string _newName;
	while (bSuccess == false)
	{
		_newName = nodeData->MeshData->MeshName + L"@" + to_wstring(_addNum);
		bSuccess = MeshListRef.insert({ _newName, nodeData->MeshData }).second;
		_addNum++;

		if (bSuccess)
		{
			nodeData->MeshData->MeshName = _newName;
			nodeData->NodeName = _newName;
		}
	}

	// Renderer Resource ����
	if (g_Renderer)
	{
		g_Renderer->CreateMeshVariable(_meshData->MeshName, _meshData->FullVtxList, _meshData->VtxCount,
			_meshData->IndexList, _meshData->IndexCount);
	}
}

void hos::FbxLoader::MakeVertexList(FbxMesh* fbxMesh)
{
	// Face�� ������ ���, FullVtx�� ������ �� �غ� �Ѵ�.
	this->FaceCount = fbxMesh->GetPolygonCount();	// �ﰢ�� ����
	this->FullVtxCount = 0;						// ���� ���� ����

	// FaceIndex�� ���� FullVtx�� �����.
	for (unsigned int i = 0; i < this->FaceCount; i++)
	{
		// ������!!
		for (unsigned int j = 0; j < 3; ++j)
		{
			// �ﰢ���� �ε����� ��´�.
			int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
			tempFaceList[i].Index[j] = (unsigned int)FullVtxCount;

			// ������ Indexed Vtx ����Ʈ�� ������� FullVtx����Ʈ�� �ݵ���ش�.
			MakeFullVertexList(fbxMesh, controlPointIndex, FullVtxCount);

			// Normal�� Ǯ���ؽ��� �о��ش�.
			ReadNormal(fbxMesh, controlPointIndex, FullVtxCount);
			ReadBiNormal(fbxMesh, controlPointIndex, FullVtxCount);
			ReadTangent(fbxMesh, controlPointIndex, FullVtxCount);

			// UV�� �о��ش�.
			int index = fbxMesh->GetTextureUVIndex(i, j);
			ReadUV(fbxMesh, FullVtxCount, index);

			FullVtxCount++;
		}
	}
}

void hos::FbxLoader::CreateControlPoints(FbxNode* fbxNode, FbxMesh* fbxMesh, Mesh* mesh)
{
	// ��Ʈ�� ����Ʈ�� ����
	this->IndexVtxCount = fbxMesh->GetControlPointsCount();

	// �ǹ��� Vertex�������� �������ش�.
	FbxAMatrix geometryTransform = GetGeometryTransformation(fbxNode);
	float fbx_X = static_cast<float>(geometryTransform.mData[3][0]);	//x
	float fbx_Y = static_cast<float>(geometryTransform.mData[3][1]);	//fbx�� y => D3D���� z
	float fbx_Z = static_cast<float>(geometryTransform.mData[3][2]);	//fbx�� z => D3D���� y

	// �ٿ�� �ڽ�
	fbxMesh->ComputeBBox();
	FbxDouble3 _min = fbxMesh->BBoxMin;
	FbxDouble3 _max = fbxMesh->BBoxMax;

	Vector3 _vMin, _vMax, _vCenter;
	_vMin.x = static_cast<float>(_min.mData[0]) + fbx_X;
	_vMin.z = static_cast<float>(_min.mData[1]) + fbx_Y;
	_vMin.y = static_cast<float>(_min.mData[2]) + fbx_Z;

	_vMax.x = static_cast<float>(_max.mData[0]) + fbx_X;
	_vMax.z = static_cast<float>(_max.mData[1]) + fbx_Y;
	_vMax.y = static_cast<float>(_max.mData[2]) + fbx_Z;

	_vCenter.x = (_vMin.x + _vMax.x) / 2.0f;
	_vCenter.y = (_vMin.y + _vMax.y) / 2.0f;
	_vCenter.z = (_vMin.z + _vMax.z) / 2.0f;

	mesh->BoundingBoxSize.x = _vMax.x - _vMin.x;
	mesh->BoundingBoxSize.y = _vMax.y - _vMin.y;
	mesh->BoundingBoxSize.z = _vMax.z - _vMin.z;

	mesh->BoundingBox[0] = _vCenter;

	mesh->BoundingBox[1].x = _vMin.x;
	mesh->BoundingBox[1].y = _vMax.y;
	mesh->BoundingBox[1].z = _vMin.z;

	mesh->BoundingBox[2].x = _vMax.x;
	mesh->BoundingBox[2].y = _vMax.y;
	mesh->BoundingBox[2].z = _vMin.z;

	mesh->BoundingBox[3].x = _vMin.x;
	mesh->BoundingBox[3].y = _vMin.y;
	mesh->BoundingBox[3].z = _vMin.z;

	mesh->BoundingBox[4].x = _vMax.x;
	mesh->BoundingBox[4].y = _vMin.y;
	mesh->BoundingBox[4].z = _vMin.z;

	mesh->BoundingBox[5].x = _vMin.x;
	mesh->BoundingBox[5].y = _vMax.y;
	mesh->BoundingBox[5].z = _vMax.z;

	mesh->BoundingBox[6].x = _vMax.x;
	mesh->BoundingBox[6].y = _vMax.y;
	mesh->BoundingBox[6].z = _vMax.z;

	mesh->BoundingBox[7].x = _vMin.x;
	mesh->BoundingBox[7].y = _vMin.y;
	mesh->BoundingBox[7].z = _vMax.z;

	mesh->BoundingBox[8].x = _vMax.x;
	mesh->BoundingBox[8].y = _vMin.y;
	mesh->BoundingBox[8].z = _vMax.z;

	for (unsigned int i = 0; i < this->IndexVtxCount; i++)
	{
		// �ӽ� ��Ʈ������Ʈ ����ü ����
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		currCtrlPoint->vPos.x = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[0]) + fbx_X;
		currCtrlPoint->vPos.z = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[1]) + fbx_Y;
		currCtrlPoint->vPos.y = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[2]) + fbx_Z;

		TempControlPoints[i] = currCtrlPoint;
	}
}


void hos::FbxLoader::MakeFullVertexList(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter)
{
	// Ǯ���ؽ��� ����Ʈ�� ControllPoint�� �����͸� �������ش�.
	tempFullVtxList[this->FullVtxCount].vPos.x = TempControlPoints[controlPointIndex]->vPos.x;
	tempFullVtxList[this->FullVtxCount].vPos.y = TempControlPoints[controlPointIndex]->vPos.y;
	tempFullVtxList[this->FullVtxCount].vPos.z = TempControlPoints[controlPointIndex]->vPos.z;

	// ��Ʈ������Ʈ ����ü ���� BoneWeight�� ������������ �������ش�.
	TempControlPoints[controlPointIndex]->SortBlendingInfoByWeight();

	// ��Ʈ�� ����Ʈ ���� Weight ������ŭ�� �������ش�.
	unsigned int weightCount = (unsigned int)TempControlPoints[controlPointIndex]->BoneInfoList.size();

	for (unsigned int i = 0; i < weightCount; i++)
	{
		// 5�� �̻��� �������ϸ� �������ش�.
		if (i > 3)
		{
			break;
		}

		U32 a = TempControlPoints[controlPointIndex]->BoneInfoList[i].BoneIndices;

		tempFullVtxList[this->FullVtxCount].index[i] = a;
		tempFullVtxList[this->FullVtxCount].weight[i] = TempControlPoints[controlPointIndex]->BoneInfoList[i].BoneWeight;
	}
}

void hos::FbxLoader::ReadNormal(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter)
{
	// �븻�� ���� ��
	if (fbxMesh->GetElementNormalCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"�ش� Node�� Normal�� �������� �ʽ��ϴ�.");
		return;
	}

	// �븻 �ε�
	const FbxGeometryElementNormal* vertexNormal = fbxMesh->GetElementNormal(0);

	// ���� ��带 Ȯ���մϴ�.
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:

		break;

	case FbxGeometryElement::eByPolygonVertex:

		switch (vertexNormal->GetReferenceMode())
		{

		case FbxGeometryElement::eDirect:
		{
			tempFullVtxList[vertexCounter].vNrm.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			tempFullVtxList[vertexCounter].vNrm.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			tempFullVtxList[vertexCounter].vNrm.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{

		}
		break;
		}
	}

	return;
}


void hos::FbxLoader::ReadBiNormal(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter)
{
	// ���̳븻�� ���� ��
	if (fbxMesh->GetElementBinormalCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"�ش� Node�� BiNormal�� �������� �ʽ��ϴ�.");
		return;
	}

	// ���̳븻 �ε�
	const FbxGeometryElementBinormal* vertexBiNormal = fbxMesh->GetElementBinormal(0);

	// ���� ��带 Ȯ���մϴ�.
	switch (vertexBiNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:

		break;

	case FbxGeometryElement::eByPolygonVertex:

		switch (vertexBiNormal->GetReferenceMode())
		{

		case FbxGeometryElement::eDirect:
		{
			tempFullVtxList[vertexCounter].vBiNrm.x = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			tempFullVtxList[vertexCounter].vBiNrm.z = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			tempFullVtxList[vertexCounter].vBiNrm.y = static_cast<float>(vertexBiNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{

		}
		break;
		}
	}

	return;
}


void hos::FbxLoader::ReadTangent(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter)
{
	// ź��Ʈ�븻�� ���� ��
	if (fbxMesh->GetElementTangentCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"�ش� Node�� TangentNormal�� �������� �ʽ��ϴ�.");
		return;
	}

	// ź��Ʈ�븻 �ε�
	const FbxGeometryElementTangent* vertexTangent = fbxMesh->GetElementTangent(0);

	// ���� ��带 Ȯ���մϴ�.
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:

		break;

	case FbxGeometryElement::eByPolygonVertex:

		switch (vertexTangent->GetReferenceMode())
		{

		case FbxGeometryElement::eDirect:
		{
			tempFullVtxList[vertexCounter].vTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[0]);
			tempFullVtxList[vertexCounter].vTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[1]);
			tempFullVtxList[vertexCounter].vTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{

		}
		break;
		}
	}

	return;
}

void hos::FbxLoader::ReadUV(const FbxMesh* fbxMesh, int fullVtxCount, int index)
{
	// UV�� �����!!
	if (fbxMesh->GetElementUVCount() < 1)
	{
		return;
	}

	const FbxGeometryElementUV* vertexUV = fbxMesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:

		break;

	case FbxGeometryElement::eByPolygonVertex:

		switch (vertexUV->GetReferenceMode())
		{

		case FbxGeometryElement::eDirect:
		{

		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			FbxVector2 fbxIndex = vertexUV->GetDirectArray().GetAt(index);
			tempFullVtxList[fullVtxCount].vUV.x = static_cast<float>(fbxIndex.mData[0]);
			tempFullVtxList[fullVtxCount].vUV.y = 1.0f - static_cast<float>(fbxIndex.mData[1]);
		}
		break;
		}
	}
}

void hos::FbxLoader::CopyVtxDataToMesh(Mesh* meshData)
{
	// FullVtx ����
	meshData->FullVtxList = new hos::cg::VertexDataType::FULL_VTX[FullVtxCount];
	meshData->VtxCount = FullVtxCount;

	for (U32 i = 0; i < FullVtxCount; i++)
	{
		// vPos
		meshData->FullVtxList[i].vPos.x = tempFullVtxList[i].vPos.x;
		meshData->FullVtxList[i].vPos.y = tempFullVtxList[i].vPos.y;
		meshData->FullVtxList[i].vPos.z = tempFullVtxList[i].vPos.z;

		// Nrm
		meshData->FullVtxList[i].vNrm.x = tempFullVtxList[i].vNrm.x;
		meshData->FullVtxList[i].vNrm.y = tempFullVtxList[i].vNrm.y;
		meshData->FullVtxList[i].vNrm.z = tempFullVtxList[i].vNrm.z;

		// Uv
		meshData->FullVtxList[i].vUV.x = tempFullVtxList[i].vUV.x;
		meshData->FullVtxList[i].vUV.y = tempFullVtxList[i].vUV.y;

		// BiNrm
		meshData->FullVtxList[i].vBiNrm.x = tempFullVtxList[i].vBiNrm.x;
		meshData->FullVtxList[i].vBiNrm.y = tempFullVtxList[i].vBiNrm.y;
		meshData->FullVtxList[i].vBiNrm.z = tempFullVtxList[i].vBiNrm.z;

		// Tangent
		meshData->FullVtxList[i].vTangent.x = tempFullVtxList[i].vTangent.x;
		meshData->FullVtxList[i].vTangent.y = tempFullVtxList[i].vTangent.y;
		meshData->FullVtxList[i].vTangent.z = tempFullVtxList[i].vTangent.z;

		// Weight
		float totalWeight = tempFullVtxList[i].weight[0] + tempFullVtxList[i].weight[1]
			+ tempFullVtxList[i].weight[2];

		meshData->FullVtxList[i].weight[0] = tempFullVtxList[i].weight[0];
		meshData->FullVtxList[i].weight[1] = tempFullVtxList[i].weight[1];
		meshData->FullVtxList[i].weight[2] = tempFullVtxList[i].weight[2];
		meshData->FullVtxList[i].weight[3] = 1.0f - totalWeight;

		// index
		meshData->FullVtxList[i].index[0] = tempFullVtxList[i].index[0];
		meshData->FullVtxList[i].index[1] = tempFullVtxList[i].index[1];
		meshData->FullVtxList[i].index[2] = tempFullVtxList[i].index[2];
		meshData->FullVtxList[i].index[3] = tempFullVtxList[i].index[3];
	}

	// Index ����
	meshData->IndexList = new U32[FaceCount * 3];
	meshData->IndexCount = FaceCount * 3;

	U32 _count = 0;

	for (U32 i = 0; i < FaceCount; i++)
	{
		meshData->IndexList[_count] = tempFaceList[i].Index[0];
		_count++;

		meshData->IndexList[_count] = tempFaceList[i].Index[2];
		_count++;

		meshData->IndexList[_count] = tempFaceList[i].Index[1];
		_count++;
	}

}

void hos::FbxLoader::GetMaterialData(FbxNode* fbxNode, Mesh* nodeData, bool onlyAnimation)
{
	int MaterialCount = fbxNode->GetMaterialCount();

	for (int i = 0; i < MaterialCount; i++)
	{
		Material* TempMaterial = new Material;
		FbxSurfaceMaterial* SurfaceMaterial = fbxNode->GetMaterial(i);

		GetMaterialAttribute(SurfaceMaterial, TempMaterial);	// ���� ��
		GetMaterialTexture(SurfaceMaterial, TempMaterial);		// �ؽ��� ���ϸ�

		// ���׸��� �̸� ����
		mbstring tempName = SurfaceMaterial->GetName();
		TempMaterial->MaterialName = nodeData->MeshName + L"(" + ut::AnsiToUTF16(tempName) + L")";

		// �ִϸ��̼��� �ִٸ� �⺻ ���� �̸��� �����Ѵ�.
		if (onlyAnimation)
		{
			nodeData->DefaultMaterialName = TempMaterial->MaterialName;
			SafeDelete(TempMaterial);
			return;
		}

		// ���ҽ� �Ŵ����� ���
		bool bSuccess = MaterialListRef.insert({ TempMaterial->MaterialName, TempMaterial }).second;

		// ���� Key�� Mesh�� �̹� �ִ� ��� �Ĺ̿� _1.. _2.._3...�� �ٿ� ���̹� ���ݴϴ�.
		int _addNum = 0;
		hos::string _newName;
		while (bSuccess == false)
		{
			_newName = TempMaterial->MaterialName + L"_" + to_wstring(_addNum);
			bSuccess = MaterialListRef.insert({ _newName, TempMaterial }).second;
			_addNum++;

			if (bSuccess)
			{
				TempMaterial->MaterialName = _newName;
			}
		}

		// node�� Deafult Mtrl ���
		nodeData->DefaultMaterialName = TempMaterial->MaterialName;

		// Renderer Resource ����
		CreateMaterialRenderResource(TempMaterial);

		TempMaterial = nullptr;
	}
}

void hos::FbxLoader::GetMaterialAttribute(FbxSurfaceMaterial* fbxMaterial, Material* materialData)
{
	FbxDouble3 Double3;
	FbxDouble Double1;

	//const char* abc = fbxMaterial->GetClassId().GetName();

	if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		// Diffuse Color
		Double3 = reinterpret_cast<FbxSurfacePhong*>(fbxMaterial)->Diffuse;
		materialData->DiffuseColor.x = static_cast<float>(Double3.mData[0]);
		materialData->DiffuseColor.y = static_cast<float>(Double3.mData[1]);
		materialData->DiffuseColor.z = static_cast<float>(Double3.mData[2]);

		// Amibent Color
		Double3 = reinterpret_cast<FbxSurfacePhong*>(fbxMaterial)->Ambient;
		materialData->AmbientColor.x = static_cast<float>(Double3.mData[0]);
		materialData->AmbientColor.y = static_cast<float>(Double3.mData[1]);
		materialData->AmbientColor.z = static_cast<float>(Double3.mData[2]);

		// Specular Color
		Double3 = reinterpret_cast<FbxSurfacePhong*>(fbxMaterial)->Specular;
		materialData->SpecularColor.x = static_cast<float>(Double3.mData[0]);
		materialData->SpecularColor.y = static_cast<float>(Double3.mData[1]);
		materialData->SpecularColor.z = static_cast<float>(Double3.mData[2]);

		// Specular Factor
		Double1 = reinterpret_cast<FbxSurfacePhong*>(fbxMaterial)->SpecularFactor;
		materialData->SpecularPower = (float)Double1;

		// MaterialType
		materialData->MaterialType = hos::Material::MATERIAL_TYPE::MATERIAL_LEGACY_PHONG;
	}

	else if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Diffuse Color
		Double3 = reinterpret_cast<FbxSurfaceLambert*>(fbxMaterial)->Diffuse;
		materialData->DiffuseColor.x = static_cast<float>(Double3.mData[0]);
		materialData->DiffuseColor.y = static_cast<float>(Double3.mData[1]);
		materialData->DiffuseColor.z = static_cast<float>(Double3.mData[2]);

		// Amibent Color
		Double3 = reinterpret_cast<FbxSurfaceLambert*>(fbxMaterial)->Ambient;
		materialData->AmbientColor.x = static_cast<float>(Double3.mData[0]);
		materialData->AmbientColor.y = static_cast<float>(Double3.mData[1]);
		materialData->AmbientColor.z = static_cast<float>(Double3.mData[2]);

		// MaterialType
		materialData->MaterialType = hos::Material::MATERIAL_TYPE::MATERIAL_LEGACY_LAMBERT;
	}
}

void hos::FbxLoader::CreateMaterialRenderResource(Material* materialData)
{
	if (materialData->IsThereAlbedo())
	{
		g_DataManager->LoadTexture(materialData->TexNameAlbedo);
	}

	if (materialData->IsThereNormal())
	{
		g_DataManager->LoadTexture(materialData->TexNameNormal);
	}

	if (materialData->IsThereARM())
	{
		g_DataManager->LoadTexture(materialData->TexNameARM);
	}

	if (materialData->IsThereEmiss())
	{
		g_DataManager->LoadTexture(materialData->TexNameEmiss);
	}
}

void hos::FbxLoader::GetMaterialTexture(FbxSurfaceMaterial* fbxMaterial, Material* materialData)
{
	unsigned int textureIndex = 0;
	FbxProperty property;

	FBXSDK_FOR_EACH_TEXTURE(textureIndex)
	{
		property = fbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
		if (property.IsValid())
		{
			unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
			for (unsigned int i = 0; i < textureCount; ++i)
			{
				FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
				if (layeredTexture)
				{
					//throw std::exception("Layered Texture is currently unsupported\n");
					return;
				}
				else
				{
					FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
					if (texture)
					{
						// �ؽ��� �Ӽ� �˻�
						std::string textureType = property.GetNameAsCStr();
						FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

						if (fileTexture)
						{
							std::string temp = fileTexture->GetFileName();

							if (textureType == "DiffuseColor")
							{
								materialData->bTexAlbedo = true;
								materialData->TexNameAlbedo = GetFileName(temp);

							}

							else if (textureType == "Bump")
							{
								materialData->bTexNormal = true;
								materialData->TexNameNormal = GetFileName(temp);
							}

							else if (textureType == "SpecularColor")
							{
								materialData->bTexARM = true;
								materialData->TexNameARM = GetFileName(temp);
								materialData->MaterialType = hos::Material::MATERIAL_TYPE::MATERIAL_STANDARD_PBR;
							}

							else if (textureType == "SpecularFactor")
							{
								materialData->bTexEmiss = true;
								materialData->TexNameEmiss = GetFileName(temp);
							}
						}
					}
				}
			}
		}
	}
}

FbxAMatrix hos::FbxLoader::GetGeometryTransformation(FbxNode* fbxNode)
{
	if (!fbxNode)
	{

	}

	const FbxVector4 lT = fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = fbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = fbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void hos::FbxLoader::ClearTempData()
{
	// string ����
	this->FileRoot = L"";
	this->FileName = L"";
	this->ModelName = L"";
	this->ClipName = L"";

	// AniInfo ����
	this->bAnimation = true;
	this->nowBoneCount = 0;

	// Temp class Data �ʱ�ȭ
	this->TempModel = nullptr;
	this->BundleRef = nullptr;
	this->TempClip = nullptr;

	// Temp Vertex Data �ʱ�ȭ
	FullVtxCount = 0;
	FaceCount = 0;
	IndexVtxCount = 0;
	memset(tempFullVtxList, 0, sizeof(hos::cg::VertexDataType::FULL_VTX) * FullVtxCount);
	memset(tempFaceList, 0, sizeof(hos::cg::VertexDataType::FACE_INDEX) * FaceCount);
}

bool hos::FbxLoader::ReadFileName(hos::mbstring_view fileRoot)
{
	// ���� ��Ʈ ȹ��
	this->FileRoot = ut::AnsiToUTF16(fileRoot);

	for (int i = 0; i < FileRoot.length(); i++)
	{
		if (FileRoot[i] == '\\')
		{
			FileRoot[i] = '/';
		}
	}

	// ���ϸ� ȹ��
	size_t pos1 = this->FileRoot.find_last_of('/');
	this->FileName = this->FileRoot.substr(pos1 + 1);

	// �𵨸� ȹ��
	size_t pos2 = this->FileName.find_last_of('@');

	// ���ϸ� '@' ������ ���η� �ִϸ��̼� ������ �Ǵ��Ѵ�.
	if (pos2 == SIZE_T_MAX)	// ����
	{
		this->bAnimation = false;	// Default������ = true

		// Ȯ���ڸ� �ٽ� ã�´�.
		pos2 = this->FileName.find_last_of('.');

		// Ȯ���� ������(.)�� ���ٸ� �߸��� �����̴�.
		if (pos2 == SIZE_T_MAX)
		{
			Debug->LogConsole(L"DataManager", FileName + L"�ùٸ� ���� ��� ������ �ƴմϴ�.");
			return false;
		}
	}

	this->ModelName = this->FileName.substr(0, pos2);

	// Clip�� ȹ��
	size_t pos3 = this->FileName.find_last_of('.');
	if (this->bAnimation)
	{
		this->ClipName = this->FileName.substr(pos2 + 1, (pos3 - 1) - pos2);

		if (pos3 == SIZE_T_MAX)
		{
			Debug->LogConsole(L"DataManager", FileName + L"�ùٸ� ���� ��� ������ �ƴմϴ�.");
			return false;
		}
	}

	// Ȯ���� ȹ��
	hos::string extention = this->FileName.substr(pos3);
	hos::string ref = L".fbx";
	hos::string ref2 = L".uifbx";

	if (_wcsicmp(extention.c_str(), ref.c_str()) != 0 && _wcsicmp(extention.c_str(), ref2.c_str()) != 0)
	{
		Debug->LogConsole(L"DataManager", FileName + L"FbxLoad�� ���� Ȯ���ڴ� .fbx �̾�߸� �մϴ�.");
		return false;
	}

	return true;
}

float hos::FbxLoader::DegreeToRadian(double degree)
{
	return static_cast<float>(degree * 3.14159265358979f / 180);
}

std::wstring hos::FbxLoader::GetFileName(std::string string)
{
	std::string inData = string;

	std::wstring Data = ut::AnsiToUTF16(inData);

	WCHAR outData[256];
	//std::wstring outData;

	// ������ '\\'�� ���ڿ� �� ������ ������ �����Ѵ�.
	size_t pos = Data.find_last_of('\\');
	_tcscpy_s(outData, &Data[pos + 1]);

	std::wstring lastData = outData;

	return lastData;
}

void hos::FbxLoader::ConvertFbxMatrixToD3DMatrix(DirectX::SimpleMath::Matrix& dest, FbxAMatrix data)
{
	dest._11 = (float)data.mData[0][0];
	dest._12 = (float)data.mData[0][2];
	dest._13 = (float)data.mData[0][1];
	dest._14 = (float)data.mData[0][3];

	dest._21 = (float)data.mData[2][0];
	dest._22 = (float)data.mData[2][2];
	dest._23 = (float)data.mData[2][1];
	dest._24 = (float)data.mData[2][3];

	dest._31 = (float)data.mData[1][0];
	dest._32 = (float)data.mData[1][2];
	dest._33 = (float)data.mData[1][1];
	dest._34 = (float)data.mData[1][3];

	dest._41 = (float)data.mData[3][0];
	dest._42 = (float)data.mData[3][2];
	dest._43 = (float)data.mData[3][1];
	dest._44 = (float)data.mData[3][3];
}

void hos::FbxLoader::ConvertFbxMatrixToD3DMatrix_Direct(DirectX::SimpleMath::Matrix& dest, FbxAMatrix data)
{
	dest._11 = (float)data.mData[0][0];
	dest._12 = (float)data.mData[0][1];
	dest._13 = (float)data.mData[0][2];
	dest._14 = (float)data.mData[0][3];

	dest._21 = (float)data.mData[1][0];
	dest._22 = (float)data.mData[1][1];
	dest._23 = (float)data.mData[1][2];
	dest._24 = (float)data.mData[1][3];

	dest._31 = (float)data.mData[2][0];
	dest._32 = (float)data.mData[2][1];
	dest._33 = (float)data.mData[2][2];
	dest._34 = (float)data.mData[2][3];

	dest._41 = (float)data.mData[3][0];
	dest._42 = (float)data.mData[3][1];
	dest._43 = (float)data.mData[3][2];
	dest._44 = (float)data.mData[3][3];
}

void hos::FbxLoader::ConvertFbxVector4ToSimpleMathVector3(DirectX::SimpleMath::Vector3& dest, FbxVector4 data, float multipleValue)
{
	dest.x = static_cast<float>(data.mData[0]) * multipleValue;
	dest.y = static_cast<float>(data.mData[2]) * multipleValue;
	dest.z = static_cast<float>(data.mData[1]) * multipleValue;
}

void hos::FbxLoader::ConvertFbxQuatToSimpleMathVector4(DirectX::SimpleMath::Vector4& dest, FbxQuaternion data)
{
	dest.x = static_cast<float>(data.mData[0]);
	dest.y = static_cast<float>(data.mData[2]);
	dest.z = static_cast<float>(data.mData[1]);
	dest.w = static_cast<float>(data.mData[3]);
}

void hos::FbxLoader::LoadNodeData(FbxScene* fbxScene, FbxNode* fbxNode, Node* parentsNode, bool onlyAnimation)
{
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	FbxString fbxName = fbxNode->GetName();
	FbxNodeAttribute::EType AttributeType;

	// TODO: FootStep Bipad�� Export���� �ʾƾ� �Ѵ�(Node Attribute�� �������� �ʱ� ����)
	// �ϴ� ���� ó�����ش�.
	if (fbxMesh == nullptr)
	{
		LoadBoneData(fbxScene, fbxNode, parentsNode, onlyAnimation);
	}

	else
	{
		AttributeType = fbxMesh->GetAttributeType();
	}

	// ����!
	if (!fbxMesh || !AttributeType)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxNode->GetName()) + L"�ش� Node�� Attribute�� ��ȿ���� �ʽ��ϴ�.");
		Debug->LogConsole(L"DataManager", ModelName + L"�ش� Model�� FbxLoad�� �����߽��ϴ�.");
		return;
	}

	// Mesh ������ ȹ��
	if (AttributeType == FbxNodeAttribute::eMesh)
	{
		LoadMeshData(fbxScene, fbxNode, fbxMesh, parentsNode, onlyAnimation);
	}

	// Bone Data ȹ��
	else if (AttributeType == FbxNodeAttribute::eSkeleton || AttributeType == FbxNodeAttribute::eNull)
	{
		LoadBoneData(fbxScene, fbxNode, parentsNode, onlyAnimation);
	}

	else
	{
		Debug->LogConsole(L"DataManager", ModelName + L"�ش� �� �ε� ��, ��ȿ���� ���� Node�� Ž���Ǿ����ϴ�.");
		return;
	}
}

void hos::FbxLoader::LoadMeshData(FbxScene* fbxScene, FbxNode* fbxNode, FbxMesh* fbxMesh, Node* parentsNode, bool onlyAnimation)
{
	// NodeName
	mbstring tempName = fbxNode->GetName();

	// MeshData
	Mesh* _meshData = new Mesh();
	_meshData->MeshName = ut::AnsiToUTF16(tempName);

	// NodeData
	Node* _nodeData = new Node();
	_nodeData->NodeName = _meshData->MeshName;
	_nodeData->MeshData = _meshData;
	_nodeData->NodeType = Node::NODE_TYPE::MESH;

	// Node Transform
	// Fbx���� �� transform�� ���� �����Ѵ�.
	FbxAMatrix nodeTransform = fbxNode->EvaluateGlobalTransform();
	FbxVector4 t = nodeTransform.GetT();
	FbxVector4 r = nodeTransform.GetR();
	FbxVector4 s = nodeTransform.GetS();

	// FbxVector�� D3D������ ��ȯ�ؼ� nodeTransform�� �����Ѵ�.
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vPos, t);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vRot, r, -1.0f);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vScale, s);


	// �θ� �ִ� ��� �������� ����
	if (parentsNode)
	{
		_nodeData->SetParent(parentsNode);
		parentsNode->SetChild(_nodeData);
	}

	// ��Ʈ�� ����Ʈ ȹ��
	CreateControlPoints(fbxNode, fbxMesh, _meshData);

	// �ִϸ��̼��� �ִ� ��� ���� �о��ش�.
	if (this->TempModel->bAni)
	{
		_nodeData->bAnimation = true;
		GetAnimation(fbxScene, fbxNode, _nodeData, onlyAnimation);
	}

	// OnlyAnimation�� ��� Mesh Load �ܰ踦 ��ŵ�Ѵ�.
	if (onlyAnimation == false)
	{
		// Mesh
		CreateMeshData(fbxMesh, _nodeData);
	}

	// Material
	GetMaterialData(fbxNode, _meshData, onlyAnimation);

	// ControlPoints �ʱ�ȭ
	for (unsigned int i = 0; i < IndexVtxCount; i++)
	{
		SafeDelete(TempControlPoints[i]);
	}
	TempControlPoints.clear();

	// ���
	int childCount = fbxNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		FbxNode* fbxChildNode = fbxNode->GetChild(i);

		LoadNodeData(fbxScene, fbxChildNode, _nodeData, onlyAnimation);
	}

	// ModelData�� MeshData ���� 
	this->TempModel->AddMesh(_nodeData);
	TempControlPoints.clear();
}

void hos::FbxLoader::LoadBoneData(FbxScene* fbxScene, FbxNode* fbxNode, Node* parentsNode, bool onlyAnimation)
{
	// NodeName
	mbstring tempName = fbxNode->GetName();

	// NodeData
	Node* _nodeData = new Node();
	_nodeData->NodeName = ut::AnsiToUTF16(tempName);
	_nodeData->NodeType = Node::NODE_TYPE::BONE;

	// Node Transform
	hos::Matrix mNodeTM;
	FbxAMatrix nodeTransform = fbxNode->EvaluateGlobalTransform();
	ConvertFbxMatrixToD3DMatrix(mNodeTM, nodeTransform);
	FbxVector4 t = nodeTransform.GetT();
	FbxVector4 r = nodeTransform.GetR();
	FbxVector4 s = nodeTransform.GetS();

	// FbxVector�� D3D������ ��ȯ�ؼ� nodeTransform�� �����Ѵ�.
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vPos, t);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vRot, r, -1.0f);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vScale, s);

	if (BundleRef && onlyAnimation == false)
	{
		// BoneOffSetTM ����
		hos::AnimationBundle::BoneTransform boneTransform;
		boneTransform.vPos = _nodeData->vPos;
		boneTransform.vRot = _nodeData->vRot;
		boneTransform.vScale = _nodeData->vScale;
		boneTransform.mTM = mNodeTM;
		this->BundleRef->GetBoneTransformList().push_back(boneTransform);
	}

	// �θ� �ִ� ��� �������� ����
	if (parentsNode)
	{
		_nodeData->SetParent(parentsNode);
		parentsNode->SetChild(_nodeData);
	}

	// ModelPrefab�� BoneData ����
	this->TempModel->AddBone(_nodeData);

	// ���
	int childCount = fbxNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		FbxNode* fbxChildNode = fbxNode->GetChild(i);

		LoadNodeData(fbxScene, fbxChildNode, _nodeData, onlyAnimation);
	}
}

void hos::FbxLoader::GetAnimation(FbxScene* fbxScene, FbxNode* fbxNode, Node* nodeData, bool onlyAnimation)
{
	// ������ ���� ���
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	int deformerCount = fbxMesh->GetDeformerCount();

	I32 currJointIndex = -1;

	// �����Ӱ� �ִٸ� �б�
	if (deformerCount != 0)
	{
		this->TempModel->bSkin = true;
		nodeData->bSkin = true;
		GetSkinningAnimation(fbxScene, fbxNode, fbxMesh, nodeData, deformerCount, onlyAnimation);

	}

	// �����Ӱ� ���ٸ� BoneAnimation�� ���ɼ�!
	else
	{
		GetBoneAnimation(fbxScene, fbxNode, nodeData);
	}

}

void hos::FbxLoader::GetBoneAnimation(FbxScene* fbxScene, FbxNode* fbxNode, Node* nodeData)
{
	hos::I32 currJointIndex = this->BundleRef->GetBoneIndexByName(nodeData->GetParentsName());

	if (currJointIndex == -1)
	{
		return;
	}

	// �ӽ� BoneAnimation ����
	BoneAnimation tempBoneAnimation;

	// �ִϸ��̼� �⺻ ���� ȹ��
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// �ִϸ��̼��� ������ ���
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	// �ִϸ��̼��� ������������ ������ ���� Node�� World �����
	FbxAMatrix currentTrans = fbxNode->EvaluateGlobalTransform();
	FbxAMatrix currentTransOffset = currentTrans.Inverse();

	FbxAMatrix boneTrans = fbxNode->GetParent()->EvaluateGlobalTransform();

	for (int i = 0; i < mAnimationLength; i++)
	{
		// ������ ����
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// Ű������ ����
		/// TODO: �� 8.0f�� ������..? �˾ƺ���
		/// 8.0f ���� �� 60�������� 7.5�ʷ� ���.
		Keyframe tempKeyframe;
		tempKeyframe.SetTimePos(static_cast<float>(i) / 30.0f);

		// WorldTM���� ����� �ִϸ��̼� TM
		FbxAMatrix Ani = pSceneEvaluator->GetNodeGlobalTransform(fbxNode->GetParent(), currTime);

		// �ִϸ��̼��� ���� �������� �����ش�.
		FbxAMatrix Result = Ani;

		DirectX::SimpleMath::Matrix TM;
		DirectX::XMVECTOR vPos, vScale, vQuat;
		ConvertFbxMatrixToD3DMatrix(TM, Result);

		DirectX::XMMatrixDecompose(&vScale, &vQuat, &vPos, TM);
		tempKeyframe.SetTransform(vPos, vScale, vQuat);

		// BoneAnimation�� Keyframe ���
		tempBoneAnimation.AddKeyframe(tempKeyframe);
	}

	// Ŭ���� Bone�ִϸ��̼� ���
	this->TempClip->SetBoneAnimation(tempBoneAnimation, currJointIndex);
}

void hos::FbxLoader::GetSkinningAnimation(FbxScene* fbxScene, FbxNode* fbxNode, FbxMesh* fbxMesh, Node* nodeData, I32 deformerCount, bool onlyAnimation)
{
	/// ���� �޽��� ��Ų�� �ϳ��� �پ������Ƿ� �����Ӵ� 1��
	for (int i = 0; i < deformerCount; i++)
	{
		// �����ӿ��� ��Ų�� ��´�.
		FbxSkin* pCurrSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(i, FbxDeformer::eSkin));

		// ��Ų�� ���ٸ� ���� �����Ӹ� Ȯ���Ѵ�.
		if (!pCurrSkin)
		{
			continue;
		}

		// �ش� ��Ų�� ����� Ŭ������ ������ ���Ѵ�.
		/// Ŭ�����ʹ� �ش� �޽�(��Ų)�� ����Ǿ� �ִ� ���� ������ŭ �����ȴ�.
		int clusterCount = pCurrSkin->GetClusterCount();

		// Ŭ�����͸� ���鼭 BoneOffset ��İ� Animation ����� ��´�.
		for (int i = 0; i < clusterCount; i++)
		{
			// ��Ų���� Ŭ�����͸� ���, Ŭ�����Ϳ��� �ش� ���� �̸��� ��´�.
			FbxCluster* pCurrCluster = pCurrSkin->GetCluster(i);
			std::string currJointName = pCurrCluster->GetLink()->GetName();

			// ���� ����Ʈ�̸��� ������Ʈ�� �̸��� ���� �� ����Ʈ���� ���� ����(�ε���)�� ���Ѵ�.
			hos::I32  currJointIndex;
			for (currJointIndex = 0; i < this->BundleRef->GetBoneListSize(); currJointIndex++)
			{
				std::wstring temp = this->BundleRef->GetBoneName(currJointIndex);
				std::string CompareBoneName = ut::UTF16ToAnsi(temp);

				if (CompareBoneName == currJointName)
				{
					break;
				}
			}

			//// �ִϸ��̼� �ߺ� üũ
			//bool bAready = this->TempClip->CheckBoneAnimation(currJointIndex);
			//if (bAready)
			//{
			//	break;
			//}

			// ���� ��Ų�� ���� �������
			FbxAMatrix transformMatrix, transformLinkMatrix, BoneOff;
			hos::Matrix BoneOff_D3D;
			pCurrCluster->GetTransformMatrix(transformMatrix);
			ConvertFbxMatrixToD3DMatrix(nodeData->MeshData->mNodeTM, transformMatrix);

			// �ִϸ��̼Ǹ� �ҷ����°� �ƴ� ��� BoneOffTM�� �����ݴϴ�.
			if (onlyAnimation == false)
			{
				std::vector<DirectX::SimpleMath::Matrix>& BoneOffSetListRef = this->BundleRef->GetBoneOffSetList();

				pCurrCluster->GetTransformLinkMatrix(transformLinkMatrix);	// ���� ��Ų�� Ŭ����Ʈ(�θ� ��)�� �������
				BoneOff = transformLinkMatrix.Inverse();
				ConvertFbxMatrixToD3DMatrix(BoneOff_D3D, BoneOff);
				BoneOffSetListRef[currJointIndex] = BoneOff_D3D;

				// ���ε��� ����Ʈ�� �ϴ� ������ �ҷ��´�.
				auto controlPointIndices = pCurrCluster->GetControlPointIndices();
				int indexCount = pCurrCluster->GetControlPointIndicesCount();
				for (int i = 0; i < indexCount; i++)
				{
					BoneInfo currBoneInfo;
					currBoneInfo.BoneIndices = currJointIndex;
					currBoneInfo.BoneWeight = (F32)pCurrCluster->GetControlPointWeights()[i];

					TempControlPoints[controlPointIndices[i]]->BoneInfoList.push_back(currBoneInfo);
					TempControlPoints[controlPointIndices[i]]->BoneName = ut::AnsiToUTF16(currJointName);
				}
			}

			// �ӽ� BoneAnimation ����
			BoneAnimation tempBoneAnimation;

			// �ִϸ��̼� �⺻ ���� ȹ��
			FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
			FbxString animStackName = pCurrAnimStack->GetName();

			// �ִϸ��̼��� ������ ���
			FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

			for (int i = 0; i < mAnimationLength; i++)
			{
				// ������ ����
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eDefaultMode);

				// Ű������ ����
				/// TODO: �� 8.0f�� ������..? �˾ƺ���
				/// 8.0f ���� �� 60�������� 7.5�ʷ� ���.
				Keyframe tempKeyframe;
				tempKeyframe.SetTimePos(static_cast<float>(i) / 30.0f);

				FbxAMatrix currentTrans = pSceneEvaluator->GetNodeGlobalTransform(fbxNode, currTime);
				FbxAMatrix currentTransOffset = currentTrans.Inverse();
				FbxAMatrix Ani = pSceneEvaluator->GetNodeGlobalTransform(pCurrCluster->GetLink(), currTime);

				FbxAMatrix Result = Ani;

				DirectX::SimpleMath::Matrix TM;
				DirectX::XMVECTOR vPos, vScale, vQuat;
				ConvertFbxMatrixToD3DMatrix(TM, Result);

				DirectX::XMMatrixDecompose(&vScale, &vQuat, &vPos, TM);
				tempKeyframe.SetTransform(vPos, vScale, vQuat);

				// BoneAnimation�� Keyframe ���
				tempBoneAnimation.AddKeyframe(tempKeyframe);
			}

			// Ŭ���� Bone�ִϸ��̼� ���
			this->TempClip->SetBoneAnimation(tempBoneAnimation, currJointIndex);
		}
	}
}

void hos::FbxLoader::GetUIAnimation(FbxScene* fbxScene, FbxNode* fbxNode, BoneAnimation* animationData)
{
	// �ִϸ��̼� �⺻ ���� ȹ��
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// �ִϸ��̼��� ������ ���
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	// �ִϸ��̼��� ������������ ������ ���� Node�� World �����
	//FbxAMatrix currentTrans = fbxNode->EvaluateGlobalTransform();
	//FbxAMatrix currentTransOffset = currentTrans.Inverse();

	FbxSurfaceMaterial* SurfaceMaterial = fbxNode->GetMaterial(0);

	for (int i = 0; i < mAnimationLength; i++)
	{
		// ������ ����
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// Ű������ ����
		/// TODO: �� 8.0f�� ������..? �˾ƺ���
		/// 8.0f ���� �� 60�������� 7.5�ʷ� ���.
		Keyframe tempKeyframe;
		tempKeyframe.SetTimePos(static_cast<float>(i) / 30.0f);

		// WorldTM���� ����� �ִϸ��̼� TM
		FbxAMatrix Ani = pSceneEvaluator->GetNodeGlobalTransform(fbxNode, currTime);

		// �ִϸ��̼��� ���� �������� �����ش�.
		FbxAMatrix Result = Ani;

		DirectX::SimpleMath::Matrix TM;
		DirectX::XMVECTOR vPos, vScale, vQuat;
		ConvertFbxMatrixToD3DMatrix_Direct(TM, Result);

		DirectX::XMMatrixDecompose(&vScale, &vQuat, &vPos, TM);
		tempKeyframe.SetTransform(vPos, vScale, vQuat);

		// BoneAnimation�� Keyframe ���
		animationData->AddKeyframe(tempKeyframe);
	}
}

void hos::FbxLoader::GetAllBone(FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	FbxString fbxName = fbxNode->GetName();
	FbxNodeAttribute::EType AttributeType;

	if (fbxMesh != nullptr)
	{
		AttributeType = fbxMesh->GetAttributeType();

		// �����?
		if (AttributeType == FbxNodeAttribute::eSkeleton || AttributeType == FbxNodeAttribute::eNull)
		{
			hos::mbstring tempName = fbxNode->GetName();
			hos::string tempNameWstring = ut::AnsiToUTF16(tempName);

			this->BundleRef->AddBoneHierarchy(this->nowBoneCount);
			this->BundleRef->SetBoneName(tempNameWstring);
			this->nowBoneCount++;

			int childCount = fbxNode->GetChildCount();

			for (int i = 0; i < childCount; i++)
			{
				FbxNode* fbxChildNode = fbxNode->GetChild(i);

				this->GetAllBone(fbxChildNode);
			}
		}
	}

	// FootStep Bipad... �ϴ� ���� ��������...
	else
	{
		hos::mbstring tempName = fbxNode->GetName();
		hos::string tempNameWstring = ut::AnsiToUTF16(tempName);

		this->BundleRef->AddBoneHierarchy(this->nowBoneCount);
		this->BundleRef->SetBoneName(tempNameWstring);
		this->nowBoneCount++;

		int childCount = fbxNode->GetChildCount();

		for (int i = 0; i < childCount; i++)
		{
			FbxNode* fbxChildNode = fbxNode->GetChild(i);

			this->GetAllBone(fbxChildNode);
		}
	}
}

void hos::FbxLoader::GetSkinningBBox(FbxScene* fbxScene)
{
	// �ִϸ��̼� �⺻ ���� ȹ��
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// �ִϸ��̼��� ������ ���
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	FbxVector4 BBoxMin, BBoxMax, BBoxCenter;

	for (int i = 0; i < mAnimationLength; i++)
	{
		// ������ ����
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// �ٿ�� �ڽ� ���
		FbxVector4 _min, _max, _center;
		fbxScene->ComputeBoundingBoxMinMaxCenter(_min, _max, _center, false, currTime);

		// �ּҰ��� ���� x ���� ������ ������ �۴ٸ�?
		if (_min.mData[0] < BBoxMin.mData[0])
			BBoxMin.mData[0] = _min.mData[0];

		// �ּҰ��� ���� y ���� ������ ������ �۴ٸ�?
		if (_min.mData[1] < BBoxMin.mData[1])
			BBoxMin.mData[1] = _min.mData[1];

		// �ּҰ��� ���� z ���� ������ ������ �۴ٸ�?
		if (_min.mData[2] < BBoxMin.mData[2])
			BBoxMin.mData[2] = _min.mData[2];

		// �ִ밪�� ���� x ���� ������ ������ ũ�ٸ�?
		if (_max.mData[0] > BBoxMax.mData[0])
			BBoxMax.mData[0] = _max.mData[0];

		// �ִ밪�� ���� y ���� ������ ������ ũ�ٸ�?
		if (_max.mData[1] > BBoxMax.mData[1])
			BBoxMax.mData[1] = _max.mData[1];

		// �ִ밪�� ���� z ���� ������ ������ ũ�ٸ�?
		if (_max.mData[2] > BBoxMax.mData[2])
			BBoxMax.mData[2] = _max.mData[2];
	}

	BBoxCenter.mData[0] = (BBoxMin.mData[0] + BBoxMax.mData[0]) / 2.0f;
	BBoxCenter.mData[1] = (BBoxMin.mData[1] + BBoxMax.mData[1]) / 2.0f;
	BBoxCenter.mData[2] = (BBoxMin.mData[2] + BBoxMax.mData[2]) / 2.0f;


	this->TempClip->BBox[0].x = (float)BBoxCenter.mData[0];
	this->TempClip->BBox[0].z = (float)BBoxCenter.mData[1];
	this->TempClip->BBox[0].y = (float)BBoxCenter.mData[2];

	this->TempClip->BBox[1].x = (float)BBoxMin.mData[0];
	this->TempClip->BBox[1].z = (float)BBoxMax.mData[1];
	this->TempClip->BBox[1].y = (float)BBoxMin.mData[2];

	this->TempClip->BBox[2].x = (float)BBoxMax.mData[0];
	this->TempClip->BBox[2].z = (float)BBoxMax.mData[1];
	this->TempClip->BBox[2].y = (float)BBoxMin.mData[2];

	this->TempClip->BBox[3].x = (float)BBoxMin.mData[0];
	this->TempClip->BBox[3].z = (float)BBoxMin.mData[1];
	this->TempClip->BBox[3].y = (float)BBoxMin.mData[2];

	this->TempClip->BBox[4].x = (float)BBoxMax.mData[0];
	this->TempClip->BBox[4].z = (float)BBoxMin.mData[1];
	this->TempClip->BBox[4].y = (float)BBoxMin.mData[2];

	this->TempClip->BBox[5].x = (float)BBoxMin.mData[0];
	this->TempClip->BBox[5].z = (float)BBoxMax.mData[1];
	this->TempClip->BBox[5].y = (float)BBoxMax.mData[2];

	this->TempClip->BBox[6].x = (float)BBoxMax.mData[0];
	this->TempClip->BBox[6].z = (float)BBoxMax.mData[1];
	this->TempClip->BBox[6].y = (float)BBoxMax.mData[2];

	this->TempClip->BBox[7].x = (float)BBoxMin.mData[0];
	this->TempClip->BBox[7].z = (float)BBoxMin.mData[1];
	this->TempClip->BBox[7].y = (float)BBoxMax.mData[2];

	this->TempClip->BBox[8].x = (float)BBoxMax.mData[0];
	this->TempClip->BBox[8].z = (float)BBoxMin.mData[1];
	this->TempClip->BBox[8].y = (float)BBoxMax.mData[2];
}
