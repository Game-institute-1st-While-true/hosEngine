#pragma once
#include "hosEngine/Com_Client.h"
#include "GzzNet/BehaviorTree.h"
#include "GzzNet/Random.h"
#include <vector>

// ���� ���
class BTNode
{
public:
	enum class SearchResult
	{
		Success, Failure, Countinue, Abort
	};

protected:
	hos::com::Script* BlackBoard;

public:
	BTNode(hos::com::Script* blackBoard);
	virtual ~BTNode();

public:
	virtual SearchResult Invoke() abstract;

};

class BTCompositeNode : public BTNode
{
private:
	std::vector<BTNode*> Children;
	//std::vector<BTNode*> Service;
	//std::vector<BTNode*> Decorate;

public:
	BTCompositeNode(hos::com::Script* blackBoard);
	virtual ~BTCompositeNode();

public:
	virtual SearchResult Invoke() override { return BTNode::SearchResult::Failure; };
	void AddChild(BTNode* node);
	virtual void RemoveAllChild();
	//void AddService(BTNode* node);
	//void AddDecorate(BTNode* node);
	const std::vector<BTNode*>& GetChildren();
	//const std::vector<BTNode*>& GetService();
	//const std::vector<BTNode*>& GetDecorate();
};

class BTRoot : public BTNode
{
	BTCompositeNode* StartNode;

public:
	BTRoot(hos::com::Script* blackBoard);
	virtual ~BTRoot();

public:
	void SetStartNode(BTCompositeNode* node);
	virtual void RemoveAllChild();

public:
	virtual SearchResult Invoke() override;
};


class BTSelector : public BTCompositeNode
{
public:
	BTSelector(hos::com::Script* blackBoard);
	virtual ~BTSelector();

public:
	virtual SearchResult Invoke() override;
};


class BTSequence : public BTCompositeNode
{
public:
	BTSequence(hos::com::Script* blackBoard);
	virtual ~BTSequence();

public:
	virtual SearchResult Invoke() override;
};

// behaivor tree
using BehaviorTree = gzz::BehaviorTree;

// func node
using BehaviorNode = gzz::BehaviorNode;
using BehaviorTask = gzz::BehaviorNode;

// sequence
using SequenceNode = gzz::SequenceNode;

// select
using SelectNode = gzz::SelectNode;

// service
using ServiceNode = gzz::ServiceNode;

// decorate
using DecoratorNode = gzz::DecoratorNode;

// Random
using Random = gzz::Random;
