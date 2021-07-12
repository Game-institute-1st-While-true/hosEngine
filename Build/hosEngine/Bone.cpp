#include "Bone.h"

using namespace hos;

hos::Node::Node()
	:NodeName(L""), NodeType(NODE_TYPE::NONE), MeshData(nullptr),
	bAnimation(false), bSkin(false)
{
}

hos::Node::~Node()
{
	Hierarchy.Release();
}

void hos::Node::SetParent(Node* parentsData)
{
	// 부모가 있다면 등록
	if (parentsData)
	{
		Hierarchy.SetParent(parentsData);
	}
}

void hos::Node::SetChild(Node* childData)
{
	Hierarchy.AddChild(childData);
}

std::vector<Node*>& hos::Node::GetChilds()
{
	return (*Hierarchy.GetChildList());
}

string hos::Node::GetName() const
{
	return this->NodeName;
}
