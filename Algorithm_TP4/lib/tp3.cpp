#include "tp3.h"

void _TestMainWindow::addBinaryNode(AbstractNode* node)
{
	nodes.push_back(node);
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
}

void _TestMainWindow::updateLayout()
{
	int itemWidth = qMax<int>(50, width() * 0.01f);
	int maxX = 0, maxY = 0;
	updateStatusItem(itemWidth);
	updateBackground();
	updateNumberItems(itemWidth, maxX, maxY);
	updateTreeItems(itemWidth, maxX, maxY);

	this->scene.setSceneRect(0, 0, qMax(width(), maxX), qMax(height(),maxY+itemWidth * 2));
	this->dirty=false;
}

void _TestMainWindow::updateTreeItems(int itemWidth, int &maxY, int &maxX)
{

	for (TreeNumberGraphicsItem* item : treeItems)
	{
		item->updateNode();
		int treeWidth = item->width();
		int startX = scene.width() * 0.5f;
		item->setRect(startX, maxY, itemWidth, itemWidth);
		item->updateLayout();
		maxY += item->height();
		if (startX + treeWidth > maxX)
		{
			maxX = startX + treeWidth;
		}
	}
}

void _TestMainWindow::updateScene()
{
	while (treeItems.size() < nodes.size())
	{
		AbstractNode* node = nodes[treeItems.size()];
		TreeNumberGraphicsItem* item = new TreeNumberGraphicsItem(node);
		treeItems.push_back(item);
		toAdd.append(item);
	}

	for (TreeNumberGraphicsItem* item : treeItems)
	{
		if (item->needUpdate())
			dirty = true;
	}
	Base::updateScene();
}

void BinarySearchThread::run()
{
	try
	{
		qsrand(time(nullptr));

		int n = qrand() % 30 + 10;
		Array& a = mainWindow->newSortedRandomArray(n);
		int index = qrand() % a.size() - 1;
		int toSearch = -1;
		if (index >= 0)
			toSearch = a.__get__(index);
		Array& _toSearchArray = mainWindow->newArray(1);
		_toSearchArray.__set__(0, toSearch);

		int result = function(a, toSearch);

		if (index >= 0)
		{
			if (a.__get__(result) == toSearch)
			{
				_message = QString("%1 is at index %2").arg(toSearch).arg(result);
				success = true;
			}
		}
		else
		{
			if (result < 0)
			{
				_message = QString("%1 is not in the array").arg(toSearch);
				success = true;
			}
		}
		if (!success)
		{
			QString message("Search failed: %1 has been found but it should be %2");
			message.append(a.toString());
			throw std::runtime_error(message.arg(result)
											.arg(index)
											.toStdString());
		}
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void BinarySearchAllThread::run()
{
	try
	{
		qsrand(time(nullptr));

		int n = qrand() % 30 + 10;
		Array& a = mainWindow->newSortedRandomArray(n);
		int index = qrand() % a.size() - 1;
		int toSearch = -1;
		if (index >= 0)
			toSearch = a.__get__(index);
		Array& _toSearchArray = mainWindow->newArray(1);
		_toSearchArray.__set__(0, toSearch);

		int resultMin=-1, resultMax=-1;
		function(a, toSearch, resultMin, resultMax);

		if (index >= 0)
		{
			if (a.__get__(resultMin) == toSearch && a.__get__(resultMax) == toSearch)
			{
				_message = QString("%1 is at indexes (%2..%3)").arg(toSearch).arg(resultMin).arg(resultMax);
				success = true;
			}
		}
		else
		{
			if (resultMin < 0 && resultMax < 0)
			{
				_message = QString("%1 is not in the array").arg(toSearch);
				success = true;
			}
		}
		if (!success)
		{
			QString message("Search failed: (%1, %2) has been found but values at this indexes are (%1, %2)\nTo Search: %5");
			message.append(a.toString());
			throw std::runtime_error(message.arg(resultMin)
											.arg(resultMax)
											.arg(a.__get__(resultMin))
											.arg(a.__get__(resultMax))
											.arg(toSearch)
											.toStdString());
		}
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void BinarySearchTreeThread::run()
{
	qsrand(time(nullptr));

	int n = qrand() % 20 + 20;
	Array a(n);
	a.fillRandom(0, 255);
	try {
		mainWindow->addBinaryNode(&tree);
		tree.buildFromValues(a.data());
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void BinarySearchTreeHeightThread::run()
{
	try {
		int result = tree.height();
		int toFind = treeHeight(tree);
		if (result != toFind)
		{
			QString message("Height processing failed: %1 has been processed but height of this tree should be %2");
			throw std::runtime_error(message.arg(result)
											.arg(toFind)
											.toStdString());
		}
		_message = QString("Height of this tree is %1").arg(result);
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

uint BinarySearchTreeHeightThread::treeHeight(const BinaryTree& tree) const
{
	uint height = 1;
	uint childrenHeight = 0;
	if (tree.left)
		childrenHeight = treeHeight(*tree.left);
	if (tree.right)
		childrenHeight = qMax(childrenHeight, treeHeight(*tree.right));
	return height + childrenHeight;
}

TreeNumberGraphicsItem::TreeNumberGraphicsItem(AbstractNode* node, QGraphicsItem* parent,
											   float leftShifting, float rightShifting)
	: Base(node->value, parent), _node(node), left(nullptr), right(nullptr),
	  leftShifting(leftShifting), rightShifting(rightShifting)
{}

void TreeNumberGraphicsItem::setNode(AbstractNode* node)
{
	this->_node = node;

	if (node)
		setNumber(node->value);
	else
		setNumber(0);
}

AbstractNode *TreeNumberGraphicsItem::node() const
{
	return _node;
}

int TreeNumberGraphicsItem::width() const
{
	int width = rect().width() * (1.f+qMax(rightShifting,leftShifting));
	int childrenWidth = 0;
	if (left)
		childrenWidth = left->width();
	if (right)
		childrenWidth = qMax(childrenWidth, right->width());
	return width + childrenWidth;
}

int TreeNumberGraphicsItem::height() const
{
	int height = this->rect().width() * (1+this->_treeHeight());
	int childrenheight = 0;
	if (left)
		childrenheight = left->height();
	if (right)
		childrenheight = qMax(childrenheight, right->height());
	return height + childrenheight;
}

bool TreeNumberGraphicsItem::needUpdate() const
{
	if (number() != _node->value)
		return true;

	if (_node->leftChild() && !left)
		return true;

	if (_node->rightChild() && !right)
		return true;

	if (left && left->needUpdate())
		return true;

	if (right && right->needUpdate())
		return true;
	return false;
}

int TreeNumberGraphicsItem::_treeHeight() const
{
	int height = 1;
	if (left)
		height = 1 + left->_treeHeight();
	if (right)
		height = qMax(height, 1 + right->_treeHeight());
	return height;
}

void TreeNumberGraphicsItem::updateNode()
{
	if (!parentItem())
	{
		if (right)
			rightShifting = right->_treeHeight() + 1;

		if (left)
			leftShifting = left->_treeHeight() + 1;
	}
	if (number() != _node->value)
		setNumber(_node->value);
	if (_node->leftChild())
	{
		if (!left)
			left = new TreeNumberGraphicsItem(_node->leftChild(), this);
		left->leftShifting = left->_treeHeight();
		left->rightShifting = (rightShifting / _treeHeight()) / 2 ;
		left->updateNode();
	}
	if (_node->rightChild())
	{
		if (!right)
			right = new TreeNumberGraphicsItem(_node->rightChild(), this);
		right->leftShifting = (leftShifting / _treeHeight()) / 2;
		right->rightShifting = right->_treeHeight();
		right->updateNode();
	}
}

void TreeNumberGraphicsItem::updateLayout()
{
	const QRectF& rect = this->rect();
	int width = rect.width();

	if (left)
	{
		if (this->scene() && !left->scene())
			this->scene()->addItem(left);
		left->setRect(rect.x() - width * leftShifting, rect.y() +
					  width  * (2+left->_treeHeight()) / 2.f, width, width);
		left->updateLayout();
	}
	if (right)
	{
		if (this->scene() && !right->scene())
			this->scene()->addItem(right);
		right->setRect(rect.x() + width * rightShifting, rect.y() +
					   width * (2+right->_treeHeight()) / 2.f, width, width);
		right->updateLayout();
	}
	update();
}

QRectF TreeNumberGraphicsItem::boundingRect() const
{
	const QRectF& rect = this->rect();
	int treeHeight = this->_treeHeight();
	int width = rect.width();
	int totalWidth = width * (1.f+rightShifting+leftShifting);
	return QRectF(rect.x() - totalWidth, rect.y(), totalWidth * 2, width * (1+treeHeight) / 2);
}

void TreeNumberGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Base::paint(painter, option, widget);
	const QRectF& rect = this->rect();
	if (left)
	{
		const QRectF& rectl = left->rect();
		painter->drawLine(rect.x()+rect.width()*0.5,rect.y() + rect.height(), rectl.x()+rectl.width()*0.5, rectl.y());
	}
	if (right)
	{
		const QRectF& rectr = right->rect();
		painter->drawLine(rect.x()+rect.width()*0.5, rect.y() + rect.height(), rectr.x()+rectr.width()*0.5, rectr.y());
	}
}

void BinarySearchTreeWindow::handleResult()
{
	Base::handleResult();

	currentThread++;
	if (currentThread != threads.end())
	{
		workerThread = *currentThread;
		connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
	}
}

void BinarySearchTreeLeafThread::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	try {
		const Node* mistaken = assertLeaf(tree);
		if (mistaken)
		{
			QString message("Node <%1> has been processed as %2 but this is not the case");
			throw std::runtime_error(message.arg(mistaken->value)
											.arg(mistaken->isLeaf() ? "leaf" : "not leaf")
											.toStdString());
		}
		const size_t nodesCount = _treeNodesCount(tree);
		Node** leaves = new Node*[nodesCount];
		memset(leaves, 0, sizeof(Node*) * nodesCount);
		uint leavesCount = 0;
		tree.allLeaves(leaves, leavesCount);

		_message = QString("Found leaves:\n");
		QStringList leavesList;
		for (size_t i = 0; i<nodesCount; ++i)
		{
			Node* leaf = leaves[i];
			if (leaf)
				leavesList.push_back(QString("Node <%1>").arg(leaf->value));
			else
				break;
		}
		_message.append(QString("[ %1 ]").arg(leavesList.join(", ")));
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

const Node* BinarySearchTreeLeafThread::assertLeaf(const BinaryTree& tree) const
{
	if (tree.isLeaf())
		if(!tree.left && !tree.right)
			return nullptr;
		else
			return &tree;
	if (tree.left)
	{
		const Node* mistaken = assertLeaf(*tree.left);
		if (mistaken)
			return mistaken;
		if (tree.right)
			return assertLeaf(*tree.right);
	}
	else if (tree.right)
		return assertLeaf(*tree.right);
	else
		return &tree;
	return nullptr;
}

void BinarySearchTreeCountThread::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	try {
		uint nodesCount = _treeNodesCount(tree);
		uint result = tree.nodesCount();
		if (result != nodesCount)
		{
			QString message("%1 nodes has been counted but the tree contains %2 nodes");
			throw std::runtime_error(message.arg(result)
											.arg(nodesCount)
											.toStdString());
		}

		_message = QString("The tree contains %1 nodes").arg(result);
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void inorderTravel(Node& node, std::vector<Node*>& nodes)
{
	if (node.left)
	{
		inorderTravel(*node.left, nodes);
	}
	nodes.push_back(&node);
	if (node.right)
	{
		inorderTravel(*node.right, nodes);
	}
}

void preorderTravel(Node& node, std::vector<Node*>& nodes)
{
	nodes.push_back(&node);
	if (node.left)
	{
		inorderTravel(*node.left, nodes);
	}
	if (node.right)
	{
		inorderTravel(*node.right, nodes);
	}
}

void postorderTravel(Node& node, std::vector<Node*>& nodes)
{
	if (node.left)
	{
		inorderTravel(*node.left, nodes);
	}
	if (node.right)
	{
		inorderTravel(*node.right, nodes);
	}
	nodes.push_back(&node);
}


void BinarySearchTreeInorderThread::_assert(Node *nodes[], size_t count) const
{
	std::vector<Node*> nodes2;
	nodes2.reserve(count);
	inorderTravel(tree, nodes2);
	assertNodesOrder(nodes, nodes2.data(), count);
}

void BinarySearchTreeInorderThread::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	try {
		const size_t nodesCount = _treeNodesCount(tree);
		Node** nodes = new Node*[nodesCount];
		memset(nodes, 0, sizeof(Node*) * nodesCount);
		uint effectivNodesCount = 0;
		tree.inorderTravel(nodes, effectivNodesCount);

		_assert(nodes, nodesCount);

		_message = QString("Inorder nodes list:\n");
		QStringList leavesList;
		for (size_t i = 0; i<nodesCount; ++i)
		{
			Node* node = nodes[i];
			if (node)
				leavesList.push_back(QString("Node <%1>").arg(node->value));
			else
				break;
		}
		_message.append(QString("[ %1 ]").arg(leavesList.join(", ")));
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void BinarySearchTreePreorderThread::_assert(Node *nodes[], size_t count) const
{
	std::vector<Node*> nodes2;
	nodes2.reserve(count);
	preorderTravel(tree, nodes2);
	assertNodesOrder(nodes, nodes2.data(), count);
}

void BinarySearchTreePreorderThread::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	try {
		const size_t nodesCount = _treeNodesCount(tree);
		Node** nodes = new Node*[nodesCount];
		memset(nodes, 0, sizeof(Node*) * nodesCount);
		uint effectivNodesCount = 0;
		tree.preorderTravel(nodes, effectivNodesCount);

		_assert(nodes, nodesCount);

		_message = QString("Preorder nodes list:\n");
		QStringList leavesList;
		for (size_t i = 0; i<nodesCount; ++i)
		{
			Node* node = nodes[i];
			if (node)
				leavesList.push_back(QString("Node <%1>").arg(node->value));
			else
				break;
		}
		_message.append(QString("[ %1 ]").arg(leavesList.join(", ")));
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void BinarySearchTreePostorderThread::_assert(Node *nodes[], size_t count) const
{
	std::vector<Node*> nodes2;
	nodes2.reserve(count);
	postorderTravel(tree, nodes2);
	assertNodesOrder(nodes, nodes2.data(), count);
}

void BinarySearchTreePostorderThread::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	try {
		const size_t nodesCount = _treeNodesCount(tree);
		Node** nodes = new Node*[nodesCount];
		memset(nodes, 0, sizeof(Node*) * nodesCount);
		uint effectivNodesCount = 0;
		tree.postorderTravel(nodes, effectivNodesCount);

		_assert(nodes, nodesCount);

		_message = QString("Postorder nodes list:\n");
		QStringList leavesList;
		for (size_t i = 0; i<nodesCount; ++i)
		{
			Node* node = nodes[i];
			if (node)
				leavesList.push_back(QString("Node <%1>").arg(node->value));
			else
				break;
		}
		_message.append(QString("[ %1 ]").arg(leavesList.join(", ")));
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void assertNodesOrder(Node *nodes1[], Node *nodes2[], size_t count)
{
	for (size_t i=0; i<count; ++i)
		if (nodes1[i] != nodes2[i])
		{
			QString message("This order is not valid, Node<%1> found at %2 but"
							" should be Node<%3>");
			throw std::runtime_error(message.arg(nodes1[i] ? QString::number(nodes1[i]->value) : QString("null"))
											.arg(i)
											.arg(nodes2[i] ? QString::number(nodes2[i]->value) : QString("null"))
											.toStdString());
		}
}

void BinarySearchTreeSearchThread::run()
{
	try
	{
		qsrand(time(nullptr));

		std::vector<Node*> nodes;
		inorderTravel(tree, nodes);
		int index = qrand() % nodes.size() - 1;
		int toSearch = -1;
		if (index >= 0)
			toSearch = nodes[index]->value;
		Node* result = tree.find(toSearch);

		if (index >= 0)
		{
			if (result && result->value == toSearch)
			{
				_message = QString("Node<%1> was found for %2").arg(result->value).arg(toSearch);
				success = true;
			}
		}
		else
		{
			if (result != nullptr)
			{
				_message = QString("%1 is not in the tree").arg(toSearch);
				success = true;
			}
		}
		if (!success)
		{
			QString message("Search failed: Node<%1> has been found but the value to search was %2");
			throw std::runtime_error(message.arg(result ? QString::number(result->value) : "null")
											.arg(toSearch)
											.toStdString());
		}
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}
