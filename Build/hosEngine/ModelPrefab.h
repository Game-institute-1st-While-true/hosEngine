#ifndef _MODEL_H
#define _MODEL_H

// [2021/03/15 ��ȣ��]
// �� ������ ������ ���� �ӽ� �� ������
// Bone ������ MeshList�� ������ �ֽ��ϴ�.

// LOG
// [2021/03/15 ��ȣ��] Graphics Engine Mk2���� ���� Ŭ���̾�Ʈ ���η� �̵�.
// [2021/04/05] ������ : �Ҹ��� ����

#pragma once

#include "hosUtility\hosUtility.h"
#include "HContainer.h"
#include "Bone.h"
#include "Mesh.h"

#include <vector>

namespace hos
{

	class ModelPrefab
	{
		friend class FbxLoader;

	private:
		hos::string ModelName;

		Node* RootNode;
		bool bAni;
		bool bSkin;

	public:
		std::vector<Node*> MeshList;
		std::vector<Node*> BoneList;

	public:
		ModelPrefab(hos::string modelName);
		~ModelPrefab();

	public:
		void AddMesh(Node* meshData);
		void AddBone(Node* boneData);
		Node* GetRootNode()const { return RootNode; }



		bool IsBoolSkin() { return bSkin; };
	};

}

#endif //!_MODEL_H