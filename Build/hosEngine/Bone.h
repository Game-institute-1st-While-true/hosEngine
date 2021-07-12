#ifndef _BONE_H
#define _BONE_H

// [2021/03/15 ��ȣ��]
// �� ������ ������ ���� �ӽ� Bone ������
// ������ ���������� ������ �ֽ��ϴ�.

// LOG
// [2021/03/15 ��ȣ��] �� ������ ������ ���� �������� �߰�.

#pragma once

#include "hosUtility\hosUtility.h"
#include "hosUtility\hosMath.h"
#include "HContainer.h"

namespace hos
{
	class Mesh;

	class Node
	{
		friend class FbxLoader;

	public:
		enum class NODE_TYPE
		{
			NONE,
			BONE,
			MESH,
			ROOT
		};

	private:
		hos::string NodeName;
		NODE_TYPE NodeType;
		HContainer<Node> Hierarchy;

		// �޽� ����(�޽��� ��쿡��)
		Mesh* MeshData;

		// Transform
		Vector3 vPos;
		Vector3 vRot;
		Vector3 vScale;

		// Ani
		bool bSkin;
		bool bAnimation;

	public:
		Node();
		~Node();

	public:
		void SetParent(Node* parentsData);
		void SetChild(Node* childData);

		hos::string GetName()const;
		std::vector<Node*>& GetChilds();
		hos::string GetParentsName() { return Hierarchy.GetParent()->GetName(); };

		NODE_TYPE GetNodeType() { return NodeType; };
		Mesh* GetMesh() { return MeshData; };

		Vector3 GetPos() { return vPos; };
		Vector3 GetRot() { return vRot; };
		Vector3 GetScale() { return vScale; };

	};

}

#endif //!_BONE_H