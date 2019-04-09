#include "binarytree.h"

#include "mainwindow.h"

#include <time.h>
#include <chrono>
#include <thread>


uint _treeNodesCount(const Node &node)
{
	uint count = 1;
	if (node.left)
		count += _treeNodesCount(*node.left);
	if (node.right)
		count += _treeNodesCount(*node.right);
	return count;
}
