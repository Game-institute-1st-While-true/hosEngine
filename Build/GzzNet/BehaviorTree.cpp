#include "BehaviorTree.h"

gzz::BehaviorTree::BehaviorTree() : BtSeed(0), Root(nullptr)
{
	Root = new Node(BtSeed++);
	RunFunc = [=]() { return this->Root->Invoke(); };
}

gzz::BehaviorTree::~BehaviorTree()
{
	delete Root;
}

bool gzz::BehaviorTree::Insert(BehaviorNode* node, int dst_node)
{
	if (!Root || !node)
	{
		return false;
	}
	else
	{
		Node* ret = Root->SearchNode(dst_node);
		if (!ret)
		{
			return false;
		}
		else
		{
			node->Up = ret;
			ret->Nodes.push_back(node);
		}
	}
	node->Seed = BtSeed++;
	FuncNames.push_back(node->funcname);
	return true;
}

bool gzz::BehaviorTree::Erase(Node* node)
{
	Node* up = node->Up;

	__int64 size = up->Nodes.size();

	for (int i = 0; i < size; i++)
	{
		if (up->Nodes[i] == node)
		{
			if (node != nullptr)
			{
				delete node;
				node = up->Nodes[i] = nullptr;
			}
		}
	}
	return true;
}

int gzz::BehaviorTree::Run()
{
	int ret = RunFunc();
	switch (ret)
	{
	case 0xff:
		if (ret != PreRet)
		{
			RunFunc = [=]()
			{
				return this->Root->SelectInvoke();
			};
			break;
		}
	default:
		if (ret != PreRet)
		{
			RunFunc = [=]()
			{
				return this->Root->Invoke();
			};
			break;
		}
	}
	PreRet = ret;
	return ret;
}