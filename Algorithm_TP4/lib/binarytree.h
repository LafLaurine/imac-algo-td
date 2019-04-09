#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <qglobal.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

struct AbstractNode
{
	AbstractNode(int value=0) : value(value) {}

	virtual ~AbstractNode() {}

    virtual AbstractNode* leftChild() {return NULL;}
    virtual AbstractNode* rightChild() {return NULL;}

	virtual uint height() const {return 0;}
	virtual uint nodesCount() const {return 0;}
	virtual bool isLeaf() const {return false;}
	virtual void insertNumber(int) {return;}
	virtual void processCodes(std::string baseCode) {}

	void dropChildren();

	int value;
};

template <typename NodeClass>
struct TemplateNode : public AbstractNode
{
	TemplateNode(int value) : AbstractNode(value), left(nullptr), right(nullptr) {}
	TemplateNode(const NodeClass& other) : AbstractNode(other.value), left(nullptr), right(nullptr) {}

	virtual ~TemplateNode()
	{
		dropChildren();
	}

	virtual NodeClass* find(int) {return nullptr;}

	virtual void allLeaves(NodeClass*[], uint&) {return;}
	virtual void inorderTravel(NodeClass*[], uint&) {return;}
	virtual void preorderTravel(NodeClass*[], uint&) {return;}
	virtual void postorderTravel(NodeClass*[], uint&) {return;}
	virtual void insertNode(NodeClass*) {}

	void buildFromValues(const std::vector<int>& values) {
		dropChildren();
		if (values.empty())
			return;
		this->value = values[0];
		for (int i=1; i<values.size(); ++i)
		{
			insertNumber(values[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}

	void dropChildren()
	{
		if (left)
			delete left;
		if (right)
			delete right;
		left = right = nullptr;
	}

	virtual AbstractNode* leftChild() {return left;}
	virtual AbstractNode* rightChild() {return right;}

	NodeClass* left;
	NodeClass* right;
};

struct Node : public TemplateNode<Node>
{
	Node(int value) : TemplateNode<Node>(value) {}
	Node(const Node& other) : TemplateNode<Node>(other) {}

	void insertNode(Node* node) {
		if (node->value < this->value)
		{
			if (this->left)
			{
				this->left->insertNode(node);
			}
			else
				this->left = node;
		}
		else
		{
			if (this->right)
			{
				this->right->insertNode(node);
			}
			else
				this->right = node;
		}
		this->value += node->value;
	}
};

Node* createNode(int value);
uint _treeNodesCount(const Node& node);

typedef Node BinaryTree;

#endif // BINARYTREE_H
