#ifndef TP3_H
#define TP3_H

#include "mainwindow.h"
#include "binarytree.h"
#include "thread.h"

#include "array.h"

class TreeNumberGraphicsItem;

class _TestMainWindow : public MainWindow
{
public:
	typedef MainWindow Base;

	_TestMainWindow(QWidget *parent=nullptr)
		: MainWindow(parent)
	{}

	void addBinaryNode(AbstractNode* node);
	void updateLayout();
	void updateScene();
	void updateTreeItems(int itemWidth, int &maxY, int &maxX);

private:
	QVector<AbstractNode*> nodes;
	QLinkedList<TreeNumberGraphicsItem*> treeItems;
};

template <class ThreadType>
class TestMainWindow : public MainWindow
{

public:
	typedef ThreadType WindowThreadType;
	typedef typename ThreadType::ThreadFunctionType FunctionType;

	TestMainWindow(FunctionType function, QWidget *parent=nullptr)
		: MainWindow(parent)
	{
		workerThread = new WindowThreadType(this, function, this);
		connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
	}
};

class TreeNumberGraphicsItem : public NumberGraphicsItem
{
public:
	typedef	NumberGraphicsItem Base;

	explicit TreeNumberGraphicsItem(AbstractNode* node, QGraphicsItem* parent=nullptr,
									float leftShiftting=0, float rightShiftting=0);
	void setNode(AbstractNode* node);
	AbstractNode* node() const;

	int width() const;
	int height() const;

	bool needUpdate() const;

	int _treeHeight() const;

	virtual void updateNode();
	void updateLayout();

	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget=nullptr) override;


public:
	AbstractNode* _node;

	TreeNumberGraphicsItem* left;
	TreeNumberGraphicsItem* right;

	float leftShifting=0;
	float rightShifting=0;
};


class BinarySearchThread : public TestThread<std::function<int(Array&, int)> >
{

public:
	BinarySearchThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
};


class BinarySearchWindow : public TestMainWindow<BinarySearchThread>
{
	Q_OBJECT

public:
	BinarySearchWindow(BinarySearchThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<BinarySearchThread>(function, parent) {}
};


class BinarySearchAllThread : public TestThread<std::function<void(Array&, int, int&, int&)> >
{

public:
	BinarySearchAllThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
};


class BinarySearchAllWindow : public TestMainWindow<BinarySearchAllThread>
{
	Q_OBJECT

public:
	BinarySearchAllWindow(BinarySearchAllThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<BinarySearchAllThread>(function, parent) {}
};


class BinarySearchTreeThread : public Thread
{

public:
	BinarySearchTreeThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: Thread(parent), mainWindow(mainWindow), success(false), tree(tree) {}
	void run() override;

	virtual bool succeeded() const {return success;}
	virtual const QString& message() const {return _message;}
	~BinarySearchTreeThread() override{}
protected:
	_TestMainWindow* mainWindow;
	QString _message;
	bool success;

	BinaryTree& tree;
};


class BinarySearchTreeHeightThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeHeightThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeHeightThread() override{}
private:
	uint treeHeight(const BinaryTree& tree) const;
};


class BinarySearchTreeCountThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeCountThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeCountThread() override{}
};


class BinarySearchTreeLeafThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeLeafThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeLeafThread() override{}
private:
	const Node* assertLeaf(const BinaryTree& tree) const;
};


void assertNodesOrder(Node* nodes1[], Node* nodes2[], size_t count);


class BinarySearchTreeInorderThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeInorderThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeInorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};


class BinarySearchTreePreorderThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreePreorderThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreePreorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};


class BinarySearchTreePostorderThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreePostorderThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreePostorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};


class BinarySearchTreeSearchThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeSearchThread(_TestMainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeSearchThread() override{}
};


class BinarySearchTreeWindow : public _TestMainWindow
{
public:
	typedef MainWindow Base;

	BinarySearchTreeWindow(BinaryTree& tree, QWidget *parent=nullptr)
		: _TestMainWindow(parent)
	{
		threads.push_back(new BinarySearchTreeThread(this, tree, this));
		threads.push_back(new BinarySearchTreeHeightThread(this, tree, this));
		threads.push_back(new BinarySearchTreeCountThread(this, tree, this));
		threads.push_back(new BinarySearchTreeLeafThread(this, tree, this));
		threads.push_back(new BinarySearchTreeInorderThread(this, tree, this));
		threads.push_back(new BinarySearchTreePreorderThread(this, tree, this));
		threads.push_back(new BinarySearchTreePostorderThread(this, tree, this));
		threads.push_back(new BinarySearchTreeSearchThread(this, tree, this));

		currentThread = threads.begin();
		workerThread = *currentThread;
		connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
	}

	void handleResult() override;

private:
	QLinkedList<Thread*> threads;
	QLinkedList<Thread*>::iterator currentThread;
};

#endif // TP3_H
