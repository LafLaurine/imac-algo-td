#include "binarytree.h"

#include "mainwindow.h"

#include <time.h>
#include <chrono>
#include <thread>


void Node::dropChildren()
{
	if (left)
		delete left;
	if (right)
		delete right;
	left = right = nullptr;
}

void Node::buildFromValues(const std::vector<int>& values) {
	dropChildren();
	if (values.empty())
		return;
	this->value = values[0];
	for (int i=1; i<values.size(); ++i)
	{
		insertNumber(values[i]);
		std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
	}
}

uint _treeNodesCount(const Node &node)
{
	uint count = 1;
	if (node.left)
		count += _treeNodesCount(*node.left);
	if (node.right)
		count += _treeNodesCount(*node.right);
	return count;
}
