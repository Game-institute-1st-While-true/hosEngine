// 참조 헤더
#include "Object.h"
#include "DXTK\SimpleMath.h"
#include <DirectXMath.h>
#include "hosUtility\hosUtility.h"
#include <tchar.h>

#include <intsafe.h>

// 애니메이션
#include "AnimationClip.h"
#include "BoneAnimation.h"
#include "Keyframe.h"

// 자기 헤더
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
	// Fbx매니저 생성
	this->Manager = FbxManager::Create();

	// 매니저에 ios를 바인딩
	this->IosSetting = FbxIOSettings::Create(Manager, IOSROOT);
	this->Manager->SetIOSettings(IosSetting);

	// 임포터 생성
	this->Importer = FbxImporter::Create(Manager, "");

	// 미리 할당된 데이터 배열 생성
	this->tempFaceList = new hos::cg::VertexDataType::FACE_INDEX[MAX_FACE];
	this->tempFullVtxList = new hos::cg::VertexDataType::FULL_VTX[MAX_FULL_VTX];

	Debug->LogConsole(L"System", L"FBX_Loader를 생성했습니다.");
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

	// ControlPoint 임시 저장소 깔끔히 제거
	for (auto [i, point] : TempControlPoints)
	{
		SafeDelete(point);
	}
	TempControlPoints.clear();

	SafeDelete(TempModel);
	SafeDelete(BundleRef);
	SafeDelete(TempClip);

	// Importer 제거
	if (this->Importer)
	{
		this->Importer->Destroy();
		this->Importer = nullptr;
	}

	// IosSetting 제거
	if (this->IosSetting)
	{
		this->IosSetting->Destroy();
		this->IosSetting = nullptr;
	}

	// Fbx매니저 제거
	if (this->Manager)
	{
		this->Manager->Destroy();
		this->Manager = nullptr;
	}
	Debug->LogConsole(L"DataManager", L"FBX_Loader를 제거했습니다.");
}

// Fbx파일 로더
hos::ModelPrefab* hos::FbxLoader::LoadFbxFile(hos::mbstring_view fileRoot)
{
	// 임시 데이터 초기화
	this->ClearTempData();

	// 파일 경로 및 이름 획득
	if (this->ReadFileName(fileRoot) == false)
	{
		Debug->LogConsole(L"System", L"Fbx파일 Load에 실패했습니다.");
		return nullptr;
	}

	// 임포터로 파일 로드
	bool status = this->Importer->Initialize(fileRoot.data(), -1, Manager->GetIOSettings());

	// 임포터 에러
	if (!status)
	{
		Debug->LogConsole(L"DataManager", L"FbxImporter가 해당 파일 로드에 실패했습니다.");
		return nullptr;
	}

	Debug->LogConsole(L"DataManager", L"모델 로드를 시작합니다.");

	// 씬을 생성하고 임포터에 임포트
	FbxScene* scene = FbxScene::Create(Manager, "scene");
	this->Importer->Import(scene);

	// 임시 모델 생성 및 루트 노드부터 파싱 시작
	this->TempModel = new ModelPrefab(this->ModelName);
	this->TempModel->bAni = this->bAnimation;
	FbxNode* RootNode = scene->GetRootNode();

	// 애니메이션이 있는 경우
	if (this->TempModel->bAni)
	{
		// 해당 모델의 번들이 없다면 만들어주고, 메쉬도 불러준다.
		if (AnimationBundleListRef.count(this->ModelName) == 0)
		{
			AnimationBundle* bundle = new AnimationBundle(this->ModelName);
			AnimationBundleListRef.insert({ this->ModelName, bundle });
			bundle = nullptr;

			LoadFbxData(scene, RootNode, false);
		}

		// 번들이 있다면, 번들도 안 만들고, 메쉬는 안 부른다.
		else
		{
			LoadFbxData(scene, RootNode, true);
		}

	}

	// 애니메이션이 없는 경우
	else
	{
		LoadFbxData(scene, RootNode, false);
	}

	// 리소스 매니저에 '모델' 클래스로 전달.
	ModelListRef.insert({ this->ModelName, this->TempModel });

	// 루트 노드 제거
	RootNode->Destroy();

	// 씬 제거
	scene->Destroy();

	// TODO: 임포터는 클리어 해 주어야 하나?

	Debug->LogConsole(L"DataManager", ModelName + L"모델 로드에 성공했습니다.");

	// 임시 데이터 초기화
	ModelPrefab* temp = this->TempModel;
	this->ClearTempData();

	return temp;
}

hos::BoneAnimation* hos::FbxLoader::LoadFbxFile_UIAnimation(hos::mbstring_view fileRoot)
{
	// 임시 데이터 초기화
	this->ClearTempData();


	// 파일 경로 및 이름 획득
	if (this->ReadFileName(fileRoot) == false)
	{
		Debug->LogConsole(L"DataManager", FileRoot + L"Fbx파일 Load에 실패했습니다.");
		return nullptr;
	}

	// 임포터로 파일 로드
	bool status = this->Importer->Initialize(fileRoot.data(), -1, Manager->GetIOSettings());

	// 임포터 에러
	if (!status)
	{
		Debug->LogConsole(L"DataManager", FileRoot + L"FbxImporter가 해당 파일 로드에 실패했습니다.");
		return nullptr;
	}

	Debug->LogConsole(L"DataManager", ModelName + L"UI Animation 로드를 시작합니다.");

	// 씬을 생성하고 임포터에 임포트
	FbxScene* scene = FbxScene::Create(Manager, "scene");
	this->Importer->Import(scene);

	// 임시 객체 생성 및 루트 노드부터 파싱 시작
	BoneAnimation* TempAnimation = new BoneAnimation();
	FbxNode* RootNode = scene->GetRootNode();

	// 로드 시작
	this->LoadFbxData_UIAnimation(scene, RootNode, TempAnimation);

	// 리소스 매니저에 '모델' 클래스로 전달.
	this->UIAnimationRef.insert({ this->ModelName, TempAnimation });

	// 루트 노드 및 씬 제거
	RootNode->Destroy();
	scene->Destroy();

	Debug->LogConsole(L"DataManager", ModelName + L"UI_Animation 로드에 성공했습니다.");

	// 임시 데이터 초기화
	this->ClearTempData();

	return TempAnimation;
}

bool hos::FbxLoader::LoadFbxData(FbxScene* fbxScene, FbxNode* fbxRootNode, bool onlyAnimation)
{
	////////////////////////////	유효성 검사		////////////////////////////

	// Node가 Mesh인지, Bone인지 판단하기 위해 Attribute를 얻어온다.
	FbxNodeAttribute* nodeAttribute = fbxRootNode->GetNodeAttribute();

	// 노드의 이름을 얻는다.
	std::string NodeName = fbxRootNode->GetName();

	// fbx파일의 루트노드인 경우
	if (NodeName == "RootNode")
	{
		int ChildCount = fbxRootNode->GetChildCount();

		// RootNode 생성(임시의 임시용)
		Node* _rootNode = new Node();
		_rootNode->NodeName = L"RootNode";
		_rootNode->NodeType = Node::NODE_TYPE::ROOT;
		this->TempModel->RootNode = _rootNode;

		// 애니메이션이 있는 경우 Bundle Load 후, Clip을 생성
		if (this->TempModel->bAni)
		{
			this->CreateBoneDataAndAniClip(onlyAnimation, fbxRootNode);
		}

		// Fbx RootNode 내부의 fbx메쉬들을 로드한다.
		for (int i = 0; i < ChildCount; i++)
		{
			FbxNode* fbxChildNode = fbxRootNode->GetChild(i);
			LoadNodeData(fbxScene, fbxChildNode, _rootNode, onlyAnimation);
		}

		// BoneData의 유효성을 검사합니다.
		if (this->TempModel->bAni)
		{
			this->CorrectBoneDataCount(onlyAnimation);
		}

		// 스키닝 모델인 경우 Skinning용 BoundingBox 구하기
		if (this->TempModel->bSkin)
		{
			this->GetSkinningBBox(fbxScene);
		}
	}

	return true;
}

bool hos::FbxLoader::LoadFbxData_UIAnimation(FbxScene* fbxScene, FbxNode* fbxRootNode, hos::BoneAnimation* animationData)
{
	////////////////////////////	유효성 검사		////////////////////////////

	// Node가 Mesh인지, Bone인지 판단하기 위해 Attribute를 얻어온다.
	FbxNodeAttribute* nodeAttribute = fbxRootNode->GetNodeAttribute();

	// 노드의 이름을 얻는다.
	std::string NodeName = fbxRootNode->GetName();

	// fbx파일의 루트노드인 경우
	if (NodeName == "RootNode")
	{
		// 노드 갯수 검사
		int ChildCount = fbxRootNode->GetChildCount();
		if (ChildCount != 1)
		{
			Debug->LogConsole(L"DataManager", ModelName + L"UI Animation Load 실패! UI의 Node는 1개여야 합니다.");
			return false;
		}

		// 애니메이션 로드
		FbxNode* fbxChildNode = fbxRootNode->GetChild(0);
		this->GetUIAnimation(fbxScene, fbxChildNode, animationData);
	}

	return true;
}

void hos::FbxLoader::CreateBoneDataAndAniClip(bool onlyAnimation, FbxNode* fbxRootNode)
{
		// Bundle Load
		this->BundleRef = AnimationBundleListRef.at(this->ModelName);

		// Animation이 있고, 애니메이션만 불러오는게 아닌 경우 Bundle의 뼈구조를 업데이트 해준다.
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
		this->TempClip->SetBoneAnimationSize(150);	// 일단 150개(MAX)
}

void hos::FbxLoader::CorrectBoneDataCount(bool onlyAnimation)
{
	// Bundle의 뼈 갯수만큼 Clip의 BoneList Vector 사이즈 조정
	hos::I32 boneCoint = (hos::I32)this->BundleRef->GetBoneListSize();

	this->TempClip->SetBoneAnimationSize(boneCoint);
	this->BundleRef->AddClip(this->ClipName, this->TempClip);

	// 애니메이션만 불러오는게 아닌 경우 Bundle의 BoneOff 갯수까지 조정해준다.
	if (onlyAnimation == false)
	{
		this->BundleRef->ResizeBoneOffSetList(this->nowBoneCount);
	}
}

void hos::FbxLoader::CreateMeshData(FbxMesh* fbxMesh, Node* nodeData)
{
	Mesh* _meshData = nodeData->MeshData;

	// FullVtxList를 만든다.
	MakeVertexList(fbxMesh);

	// 리팩토링(웰드 및 보정)
	/// Weld 하든말든 여기서 복사해주어야 한다.
	CopyVtxDataToMesh(nodeData->MeshData);

	// 복사 후 tempFullVtxList 청소
	for (unsigned int i = 0; i < FullVtxCount; i++)
	{
		memset(&tempFullVtxList[i], 0, sizeof(hos::cg::VertexDataType::FULL_VTX));
	}

	// Resource Manager에 등록
	bool bSuccess = MeshListRef.insert({ _meshData->MeshName, _meshData }).second;

	// Resource Manageㄱ에 같은 Key로 Mesh가 이미 있는 경우 후미에 _1.. _2.._3...을 붙여 네이밍 해줍니다.
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

	// Renderer Resource 생성
	if (g_Renderer)
	{
		g_Renderer->CreateMeshVariable(_meshData->MeshName, _meshData->FullVtxList, _meshData->VtxCount,
			_meshData->IndexList, _meshData->IndexCount);
	}
}

void hos::FbxLoader::MakeVertexList(FbxMesh* fbxMesh)
{
	// Face의 갯수를 얻고, FullVtx의 갯수를 셀 준비를 한다.
	this->FaceCount = fbxMesh->GetPolygonCount();	// 삼각형 갯수
	this->FullVtxCount = 0;						// 최종 정점 갯수

	// FaceIndex를 돌며 FullVtx를 만든다.
	for (unsigned int i = 0; i < this->FaceCount; i++)
	{
		// 폴리곤!!
		for (unsigned int j = 0; j < 3; ++j)
		{
			// 삼각형의 인덱스를 얻는다.
			int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
			tempFaceList[i].Index[j] = (unsigned int)FullVtxCount;

			// 기존의 Indexed Vtx 리스트를 기반으로 FullVtx리스트를 반들어준다.
			MakeFullVertexList(fbxMesh, controlPointIndex, FullVtxCount);

			// Normal을 풀버텍스에 읽어준다.
			ReadNormal(fbxMesh, controlPointIndex, FullVtxCount);
			ReadBiNormal(fbxMesh, controlPointIndex, FullVtxCount);
			ReadTangent(fbxMesh, controlPointIndex, FullVtxCount);

			// UV를 읽어준다.
			int index = fbxMesh->GetTextureUVIndex(i, j);
			ReadUV(fbxMesh, FullVtxCount, index);

			FullVtxCount++;
		}
	}
}

void hos::FbxLoader::CreateControlPoints(FbxNode* fbxNode, FbxMesh* fbxMesh, Mesh* mesh)
{
	// 컨트롤 포인트의 갯수
	this->IndexVtxCount = fbxMesh->GetControlPointsCount();

	// 피벗을 Vertex단위에서 교정해준다.
	FbxAMatrix geometryTransform = GetGeometryTransformation(fbxNode);
	float fbx_X = static_cast<float>(geometryTransform.mData[3][0]);	//x
	float fbx_Y = static_cast<float>(geometryTransform.mData[3][1]);	//fbx의 y => D3D에선 z
	float fbx_Z = static_cast<float>(geometryTransform.mData[3][2]);	//fbx의 z => D3D에선 y

	// 바운드 박스
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
		// 임시 컨트롤포인트 구조체 생성
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		currCtrlPoint->vPos.x = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[0]) + fbx_X;
		currCtrlPoint->vPos.z = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[1]) + fbx_Y;
		currCtrlPoint->vPos.y = static_cast<float>(fbxMesh->GetControlPointAt(i).mData[2]) + fbx_Z;

		TempControlPoints[i] = currCtrlPoint;
	}
}


void hos::FbxLoader::MakeFullVertexList(const FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter)
{
	// 풀버텍스를 리스트에 ControllPoint의 데이터를 복사해준다.
	tempFullVtxList[this->FullVtxCount].vPos.x = TempControlPoints[controlPointIndex]->vPos.x;
	tempFullVtxList[this->FullVtxCount].vPos.y = TempControlPoints[controlPointIndex]->vPos.y;
	tempFullVtxList[this->FullVtxCount].vPos.z = TempControlPoints[controlPointIndex]->vPos.z;

	// 컨트롤포인트 구조체 내의 BoneWeight를 내림차순으로 정렬해준다.
	TempControlPoints[controlPointIndex]->SortBlendingInfoByWeight();

	// 컨트롤 포인트 내의 Weight 갯수만큼만 복사해준다.
	unsigned int weightCount = (unsigned int)TempControlPoints[controlPointIndex]->BoneInfoList.size();

	for (unsigned int i = 0; i < weightCount; i++)
	{
		// 5개 이상을 읽으려하면 종료해준다.
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
	// 노말이 없을 때
	if (fbxMesh->GetElementNormalCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"해당 Node의 Normal이 존재하지 않습니다.");
		return;
	}

	// 노말 로드
	const FbxGeometryElementNormal* vertexNormal = fbxMesh->GetElementNormal(0);

	// 맵핑 모드를 확인합니다.
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
	// 바이노말이 없을 때
	if (fbxMesh->GetElementBinormalCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"해당 Node의 BiNormal이 존재하지 않습니다.");
		return;
	}

	// 바이노말 로드
	const FbxGeometryElementBinormal* vertexBiNormal = fbxMesh->GetElementBinormal(0);

	// 맵핑 모드를 확인합니다.
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
	// 탄젠트노말이 없을 때
	if (fbxMesh->GetElementTangentCount() < 1)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxMesh->GetName()) + L"해당 Node의 TangentNormal이 존재하지 않습니다.");
		return;
	}

	// 탄젠트노말 로드
	const FbxGeometryElementTangent* vertexTangent = fbxMesh->GetElementTangent(0);

	// 맵핑 모드를 확인합니다.
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
	// UV가 없어요!!
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
	// FullVtx 복사
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

	// Index 복사
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

		GetMaterialAttribute(SurfaceMaterial, TempMaterial);	// 재질 값
		GetMaterialTexture(SurfaceMaterial, TempMaterial);		// 텍스쳐 파일명

		// 마테리얼 이름 저장
		mbstring tempName = SurfaceMaterial->GetName();
		TempMaterial->MaterialName = nodeData->MeshName + L"(" + ut::AnsiToUTF16(tempName) + L")";

		// 애니메이션이 있다면 기본 재질 이름만 전달한다.
		if (onlyAnimation)
		{
			nodeData->DefaultMaterialName = TempMaterial->MaterialName;
			SafeDelete(TempMaterial);
			return;
		}

		// 리소스 매니저에 등록
		bool bSuccess = MaterialListRef.insert({ TempMaterial->MaterialName, TempMaterial }).second;

		// 같은 Key로 Mesh가 이미 있는 경우 후미에 _1.. _2.._3...을 붙여 네이밍 해줍니다.
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

		// node에 Deafult Mtrl 등록
		nodeData->DefaultMaterialName = TempMaterial->MaterialName;

		// Renderer Resource 생성
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
						// 텍스쳐 속성 검사
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
	// string 제거
	this->FileRoot = L"";
	this->FileName = L"";
	this->ModelName = L"";
	this->ClipName = L"";

	// AniInfo 제거
	this->bAnimation = true;
	this->nowBoneCount = 0;

	// Temp class Data 초기화
	this->TempModel = nullptr;
	this->BundleRef = nullptr;
	this->TempClip = nullptr;

	// Temp Vertex Data 초기화
	FullVtxCount = 0;
	FaceCount = 0;
	IndexVtxCount = 0;
	memset(tempFullVtxList, 0, sizeof(hos::cg::VertexDataType::FULL_VTX) * FullVtxCount);
	memset(tempFaceList, 0, sizeof(hos::cg::VertexDataType::FACE_INDEX) * FaceCount);
}

bool hos::FbxLoader::ReadFileName(hos::mbstring_view fileRoot)
{
	// 파일 루트 획득
	this->FileRoot = ut::AnsiToUTF16(fileRoot);

	for (int i = 0; i < FileRoot.length(); i++)
	{
		if (FileRoot[i] == '\\')
		{
			FileRoot[i] = '/';
		}
	}

	// 파일명 획득
	size_t pos1 = this->FileRoot.find_last_of('/');
	this->FileName = this->FileRoot.substr(pos1 + 1);

	// 모델명 획득
	size_t pos2 = this->FileName.find_last_of('@');

	// 파일명에 '@' 문자의 여부로 애니메이션 유무를 판단한다.
	if (pos2 == SIZE_T_MAX)	// 없음
	{
		this->bAnimation = false;	// Default생성자 = true

		// 확장자를 다시 찾는다.
		pos2 = this->FileName.find_last_of('.');

		// 확장자 구분자(.)가 없다면 잘못된 파일이다.
		if (pos2 == SIZE_T_MAX)
		{
			Debug->LogConsole(L"DataManager", FileName + L"올바른 파일 경로 형식이 아닙니다.");
			return false;
		}
	}

	this->ModelName = this->FileName.substr(0, pos2);

	// Clip명 획득
	size_t pos3 = this->FileName.find_last_of('.');
	if (this->bAnimation)
	{
		this->ClipName = this->FileName.substr(pos2 + 1, (pos3 - 1) - pos2);

		if (pos3 == SIZE_T_MAX)
		{
			Debug->LogConsole(L"DataManager", FileName + L"올바른 파일 경로 형식이 아닙니다.");
			return false;
		}
	}

	// 확장자 획득
	hos::string extention = this->FileName.substr(pos3);
	hos::string ref = L".fbx";
	hos::string ref2 = L".uifbx";

	if (_wcsicmp(extention.c_str(), ref.c_str()) != 0 && _wcsicmp(extention.c_str(), ref2.c_str()) != 0)
	{
		Debug->LogConsole(L"DataManager", FileName + L"FbxLoad할 파일 확장자는 .fbx 이어야만 합니다.");
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

	// 마지막 '\\'와 문자열 끝 사이의 구간을 복제한다.
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

	// TODO: FootStep Bipad는 Export하지 않아야 한다(Node Attribute가 존재하지 않기 때문)
	// 일단 뼈로 처리해준다.
	if (fbxMesh == nullptr)
	{
		LoadBoneData(fbxScene, fbxNode, parentsNode, onlyAnimation);
	}

	else
	{
		AttributeType = fbxMesh->GetAttributeType();
	}

	// 실패!
	if (!fbxMesh || !AttributeType)
	{
		Debug->LogConsole(L"DataManager", ut::AnsiToUTF16(fbxNode->GetName()) + L"해당 Node의 Attribute가 유효하지 않습니다.");
		Debug->LogConsole(L"DataManager", ModelName + L"해당 Model의 FbxLoad에 실패했습니다.");
		return;
	}

	// Mesh 데이터 획득
	if (AttributeType == FbxNodeAttribute::eMesh)
	{
		LoadMeshData(fbxScene, fbxNode, fbxMesh, parentsNode, onlyAnimation);
	}

	// Bone Data 획득
	else if (AttributeType == FbxNodeAttribute::eSkeleton || AttributeType == FbxNodeAttribute::eNull)
	{
		LoadBoneData(fbxScene, fbxNode, parentsNode, onlyAnimation);
	}

	else
	{
		Debug->LogConsole(L"DataManager", ModelName + L"해당 모델 로드 중, 유효하지 않은 Node가 탐색되었습니다.");
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
	// Fbx에서 각 transform을 직접 추출한다.
	FbxAMatrix nodeTransform = fbxNode->EvaluateGlobalTransform();
	FbxVector4 t = nodeTransform.GetT();
	FbxVector4 r = nodeTransform.GetR();
	FbxVector4 s = nodeTransform.GetS();

	// FbxVector를 D3D용으로 변환해서 nodeTransform에 저장한다.
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vPos, t);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vRot, r, -1.0f);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vScale, s);


	// 부모가 있는 경우 계층구조 세팅
	if (parentsNode)
	{
		_nodeData->SetParent(parentsNode);
		parentsNode->SetChild(_nodeData);
	}

	// 컨트롤 포인트 획득
	CreateControlPoints(fbxNode, fbxMesh, _meshData);

	// 애니메이션이 있는 경우 먼저 읽어준다.
	if (this->TempModel->bAni)
	{
		_nodeData->bAnimation = true;
		GetAnimation(fbxScene, fbxNode, _nodeData, onlyAnimation);
	}

	// OnlyAnimation인 경우 Mesh Load 단계를 스킵한다.
	if (onlyAnimation == false)
	{
		// Mesh
		CreateMeshData(fbxMesh, _nodeData);
	}

	// Material
	GetMaterialData(fbxNode, _meshData, onlyAnimation);

	// ControlPoints 초기화
	for (unsigned int i = 0; i < IndexVtxCount; i++)
	{
		SafeDelete(TempControlPoints[i]);
	}
	TempControlPoints.clear();

	// 재귀
	int childCount = fbxNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		FbxNode* fbxChildNode = fbxNode->GetChild(i);

		LoadNodeData(fbxScene, fbxChildNode, _nodeData, onlyAnimation);
	}

	// ModelData에 MeshData 전달 
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

	// FbxVector를 D3D용으로 변환해서 nodeTransform에 저장한다.
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vPos, t);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vRot, r, -1.0f);
	ConvertFbxVector4ToSimpleMathVector3(_nodeData->vScale, s);

	if (BundleRef && onlyAnimation == false)
	{
		// BoneOffSetTM 저장
		hos::AnimationBundle::BoneTransform boneTransform;
		boneTransform.vPos = _nodeData->vPos;
		boneTransform.vRot = _nodeData->vRot;
		boneTransform.vScale = _nodeData->vScale;
		boneTransform.mTM = mNodeTM;
		this->BundleRef->GetBoneTransformList().push_back(boneTransform);
	}

	// 부모가 있는 경우 계층구조 세팅
	if (parentsNode)
	{
		_nodeData->SetParent(parentsNode);
		parentsNode->SetChild(_nodeData);
	}

	// ModelPrefab에 BoneData 전달
	this->TempModel->AddBone(_nodeData);

	// 재귀
	int childCount = fbxNode->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		FbxNode* fbxChildNode = fbxNode->GetChild(i);

		LoadNodeData(fbxScene, fbxChildNode, _nodeData, onlyAnimation);
	}
}

void hos::FbxLoader::GetAnimation(FbxScene* fbxScene, FbxNode* fbxNode, Node* nodeData, bool onlyAnimation)
{
	// 디포머 갯수 얻기
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	int deformerCount = fbxMesh->GetDeformerCount();

	I32 currJointIndex = -1;

	// 디포머가 있다면 읽기
	if (deformerCount != 0)
	{
		this->TempModel->bSkin = true;
		nodeData->bSkin = true;
		GetSkinningAnimation(fbxScene, fbxNode, fbxMesh, nodeData, deformerCount, onlyAnimation);

	}

	// 디포머가 없다면 BoneAnimation의 가능성!
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

	// 임시 BoneAnimation 생성
	BoneAnimation tempBoneAnimation;

	// 애니메이션 기본 정보 획득
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// 애니메이션의 프레임 얻기
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	// 애니메이션을 원점공간으로 돌리기 위한 Node의 World 역행렬
	FbxAMatrix currentTrans = fbxNode->EvaluateGlobalTransform();
	FbxAMatrix currentTransOffset = currentTrans.Inverse();

	FbxAMatrix boneTrans = fbxNode->GetParent()->EvaluateGlobalTransform();

	for (int i = 0; i < mAnimationLength; i++)
	{
		// 프레임 세팅
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// 키프레임 세팅
		/// TODO: 왜 8.0f을 나누지..? 알아보자
		/// 8.0f 세팅 시 60프레임을 7.5초로 재생.
		Keyframe tempKeyframe;
		tempKeyframe.SetTimePos(static_cast<float>(i) / 30.0f);

		// WorldTM까지 적용된 애니메이션 TM
		FbxAMatrix Ani = pSceneEvaluator->GetNodeGlobalTransform(fbxNode->GetParent(), currTime);

		// 애니메이션을 원점 공간으로 돌려준다.
		FbxAMatrix Result = Ani;

		DirectX::SimpleMath::Matrix TM;
		DirectX::XMVECTOR vPos, vScale, vQuat;
		ConvertFbxMatrixToD3DMatrix(TM, Result);

		DirectX::XMMatrixDecompose(&vScale, &vQuat, &vPos, TM);
		tempKeyframe.SetTransform(vPos, vScale, vQuat);

		// BoneAnimation에 Keyframe 등록
		tempBoneAnimation.AddKeyframe(tempKeyframe);
	}

	// 클립에 Bone애니메이션 등록
	this->TempClip->SetBoneAnimation(tempBoneAnimation, currJointIndex);
}

void hos::FbxLoader::GetSkinningAnimation(FbxScene* fbxScene, FbxNode* fbxNode, FbxMesh* fbxMesh, Node* nodeData, I32 deformerCount, bool onlyAnimation)
{
	/// 보통 메쉬에 스킨이 하나만 붙어있으므로 디포머는 1개
	for (int i = 0; i < deformerCount; i++)
	{
		// 디포머에서 스킨을 얻는다.
		FbxSkin* pCurrSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(i, FbxDeformer::eSkin));

		// 스킨이 없다면 다음 디포머를 확인한다.
		if (!pCurrSkin)
		{
			continue;
		}

		// 해당 스킨에 연결된 클러스터 갯수를 구한다.
		/// 클러스터는 해당 메쉬(스킨)가 연결되어 있는 뼈의 갯수만큼 생성된다.
		int clusterCount = pCurrSkin->GetClusterCount();

		// 클러스터를 돌면서 BoneOffset 행렬과 Animation 행렬을 얻는다.
		for (int i = 0; i < clusterCount; i++)
		{
			// 스킨에서 클러스터를 얻고, 클러스터에서 해당 뼈의 이름을 얻는다.
			FbxCluster* pCurrCluster = pCurrSkin->GetCluster(i);
			std::string currJointName = pCurrCluster->GetLink()->GetName();

			// 현재 조인트이름과 뼈리스트의 이름을 비교해 뼈 리스트에서 뼈의 순서(인덱스)를 구한다.
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

			//// 애니메이션 중복 체크
			//bool bAready = this->TempClip->CheckBoneAnimation(currJointIndex);
			//if (bAready)
			//{
			//	break;
			//}

			// 현재 스킨의 최종 월드행렬
			FbxAMatrix transformMatrix, transformLinkMatrix, BoneOff;
			hos::Matrix BoneOff_D3D;
			pCurrCluster->GetTransformMatrix(transformMatrix);
			ConvertFbxMatrixToD3DMatrix(nodeData->MeshData->mNodeTM, transformMatrix);

			// 애니메이션만 불러오는게 아닌 경우 BoneOffTM을 구해줍니다.
			if (onlyAnimation == false)
			{
				std::vector<DirectX::SimpleMath::Matrix>& BoneOffSetListRef = this->BundleRef->GetBoneOffSetList();

				pCurrCluster->GetTransformLinkMatrix(transformLinkMatrix);	// 현재 스킨의 클러스트(부모 뼈)의 월드행렬
				BoneOff = transformLinkMatrix.Inverse();
				ConvertFbxMatrixToD3DMatrix(BoneOff_D3D, BoneOff);
				BoneOffSetListRef[currJointIndex] = BoneOff_D3D;

				// 본인덱스 웨이트를 일단 모조리 불러온다.
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

			// 임시 BoneAnimation 생성
			BoneAnimation tempBoneAnimation;

			// 애니메이션 기본 정보 획득
			FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
			FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
			FbxString animStackName = pCurrAnimStack->GetName();

			// 애니메이션의 프레임 얻기
			FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

			for (int i = 0; i < mAnimationLength; i++)
			{
				// 프레임 세팅
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eDefaultMode);

				// 키프레임 세팅
				/// TODO: 왜 8.0f을 나누지..? 알아보자
				/// 8.0f 세팅 시 60프레임을 7.5초로 재생.
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

				// BoneAnimation에 Keyframe 등록
				tempBoneAnimation.AddKeyframe(tempKeyframe);
			}

			// 클립에 Bone애니메이션 등록
			this->TempClip->SetBoneAnimation(tempBoneAnimation, currJointIndex);
		}
	}
}

void hos::FbxLoader::GetUIAnimation(FbxScene* fbxScene, FbxNode* fbxNode, BoneAnimation* animationData)
{
	// 애니메이션 기본 정보 획득
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// 애니메이션의 프레임 얻기
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	// 애니메이션을 원점공간으로 돌리기 위한 Node의 World 역행렬
	//FbxAMatrix currentTrans = fbxNode->EvaluateGlobalTransform();
	//FbxAMatrix currentTransOffset = currentTrans.Inverse();

	FbxSurfaceMaterial* SurfaceMaterial = fbxNode->GetMaterial(0);

	for (int i = 0; i < mAnimationLength; i++)
	{
		// 프레임 세팅
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// 키프레임 세팅
		/// TODO: 왜 8.0f을 나누지..? 알아보자
		/// 8.0f 세팅 시 60프레임을 7.5초로 재생.
		Keyframe tempKeyframe;
		tempKeyframe.SetTimePos(static_cast<float>(i) / 30.0f);

		// WorldTM까지 적용된 애니메이션 TM
		FbxAMatrix Ani = pSceneEvaluator->GetNodeGlobalTransform(fbxNode, currTime);

		// 애니메이션을 원점 공간으로 돌려준다.
		FbxAMatrix Result = Ani;

		DirectX::SimpleMath::Matrix TM;
		DirectX::XMVECTOR vPos, vScale, vQuat;
		ConvertFbxMatrixToD3DMatrix_Direct(TM, Result);

		DirectX::XMMatrixDecompose(&vScale, &vQuat, &vPos, TM);
		tempKeyframe.SetTransform(vPos, vScale, vQuat);

		// BoneAnimation에 Keyframe 등록
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

		// 뼈라면?
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

	// FootStep Bipad... 일단 뼈로 생각하자...
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
	// 애니메이션 기본 정보 획득
	FbxAnimStack* pCurrAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxAnimEvaluator* pSceneEvaluator = fbxScene->GetAnimationEvaluator();
	FbxString animStackName = pCurrAnimStack->GetName();

	// 애니메이션의 프레임 얻기
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	int mAnimationLength = (int)(end.GetFrameCount() - start.GetFrameCount() + 1);

	FbxVector4 BBoxMin, BBoxMax, BBoxCenter;

	for (int i = 0; i < mAnimationLength; i++)
	{
		// 프레임 세팅
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eDefaultMode);

		// 바운딩 박스 기록
		FbxVector4 _min, _max, _center;
		fbxScene->ComputeBoundingBoxMinMaxCenter(_min, _max, _center, false, currTime);

		// 최소값의 현재 x 값이 기존의 값보다 작다면?
		if (_min.mData[0] < BBoxMin.mData[0])
			BBoxMin.mData[0] = _min.mData[0];

		// 최소값의 현재 y 값이 기존의 값보다 작다면?
		if (_min.mData[1] < BBoxMin.mData[1])
			BBoxMin.mData[1] = _min.mData[1];

		// 최소값의 현재 z 값이 기존의 값보다 작다면?
		if (_min.mData[2] < BBoxMin.mData[2])
			BBoxMin.mData[2] = _min.mData[2];

		// 최대값의 현재 x 값이 기존의 값보다 크다면?
		if (_max.mData[0] > BBoxMax.mData[0])
			BBoxMax.mData[0] = _max.mData[0];

		// 최대값의 현재 y 값이 기존의 값보다 크다면?
		if (_max.mData[1] > BBoxMax.mData[1])
			BBoxMax.mData[1] = _max.mData[1];

		// 최대값의 현재 z 값이 기존의 값보다 크다면?
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
