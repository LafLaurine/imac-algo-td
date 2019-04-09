#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <qglobal.h>
#include <vector>


struct Node
{
	Node(int value=0) : left(nullptr), right(nullptr), value(value) {}

	virtual ~Node()
	{
		dropChildren();
	};

	virtual uint height() const {return 0;}
	virtual uint nodesCount() const {return 0;}
	virtual bool isLeaf() const {return false;}
	virtual void allLeaves(Node*[], uint&) {return;}
	virtual void inorderTravel(Node*[], uint&) {return;}
	virtual void preorderTravel(Node*[], uint&) {return;}
	virtual void postorderTravel(Node*[], uint&) {return;}
	virtual Node* find(int) {return nullptr;}
	virtual void insertNumber(int) {return;}

	void dropChildren();
	void buildFromValues(const std::vector<int>& values);


	Node* left;
	Node* right;
	int value;
};

Node* createNode(int value);
uint _treeNodesCount(const Node& node);

typedef Node BinaryTree;

#endif // BINARYTREE_H
