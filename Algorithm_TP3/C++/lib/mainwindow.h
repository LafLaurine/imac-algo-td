#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>
#include <time.h>
#include <chrono>
#include <thread>
#include <stdexcept>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QTimer>
#include <QThread>
#include <QStack>
#include <QLinkedList>
#include <QVector2D>

#include "array.h"
#include "binarytree.h"


using std::size_t;

class NumberGraphicsItem : public QGraphicsEllipseItem
{
public:
	explicit NumberGraphicsItem(const int _number, QGraphicsItem* parent=nullptr);
	void setNumber(const int _number);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget=nullptr) override;

	void displayDefault();
	void displayReadenState();
	void displayWrittenState();
	int number() const;

private:
	enum State
	{
		DEFAULT=0,
		READEN,
		WRITEN,
		STATE_COUNT
	};
	void displayDefault(bool force);

	QString text;
	int _number;
	State state;
};

class TreeNumberGraphicsItem : public NumberGraphicsItem
{
public:
	typedef	NumberGraphicsItem Base;

	explicit TreeNumberGraphicsItem(Node* node, QGraphicsItem* parent=nullptr, float leftShiftting=0, float rightShiftting=0);
	void setNode(Node* node);
	Node* node() const;

	int width() const;
	int height() const;

	bool needUpdate() const;

	int _treeHeight() const;

	void updateNode();
	void updateLayout();

	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget=nullptr) override;


private:
	Node* _node;

	TreeNumberGraphicsItem* left;
	TreeNumberGraphicsItem* right;

	float leftShifting=0;
	float rightShifting=0;
};

class Thread : public QThread
{
public:
	Thread(QObject* parent) : QThread(parent) {}
	virtual bool succeeded() const = 0;
	virtual const QString& message() const = 0;
	virtual ~Thread() {}
};


class MainWindow : public QMainWindow
{

	Q_OBJECT

public:

	explicit MainWindow(QWidget* parent=nullptr);

	Array& mainArray() {return arrays[0];}
	Array& newRandomArray(uint size);
	Array& newSortedRandomArray(uint size);
	Array& newArray(uint size);

	void addBinaryNode(Node* node);

	void pushFunctionCall(QString functionName, int parameter);
	void pushFunctionCall(QString functionName, int parameter1, int parameter2);
	void pushFunctionCall(QString functionName, QVector2D parameter1, int parameter2, QVector2D parameter3);
	void pushFunctionCall(QString functionName, const Array& parameter1, int parameter2);
	void pushFunctionCall(QString functionName, const Array& parameter1, const Array& parameter2, int parameter3, int parameter4);
	void popFunctionCall(int result);
	void popFunctionCall(QString result);

	void resizeEvent(QResizeEvent*);
	void closeEvent(QCloseEvent*);

	void setBackground(QImage* image);

	void updateLayout();

	static unsigned long instruction_duration;

	bool eventFilter(QObject* object, QEvent* event);

	~MainWindow();
public slots:
	void updateScene();
	virtual void handleResult();

private:

	void initialize();

	QGraphicsScene scene;
	QGraphicsView view;

	QVector<QVector<NumberGraphicsItem*> > numberItems;
	QLinkedList<TreeNumberGraphicsItem *> treeItems;
	QVector<QGraphicsTextItem *> accessInfoItem;
	QLinkedList<QGraphicsTextItem *> functionCallItems;
	QGraphicsTextItem statusItem;
	QVector<Array> arrays;
	QVector<Node*> nodes;

	QTimer timer;

	QVector<QGraphicsItem*> toAdd;
	QVector<QPair<QGraphicsTextItem*, QString> > toUpdate;
	QVector<QGraphicsItem*> toRemove;
	QStack<QString> functionCalls;

	QGraphicsPixmapItem* background;

	bool dirty;

	bool zoom;
	float scale;
	QPointF targetPos;

protected:
	Thread* workerThread;
};

template<typename FunctionType>
class TestThread : public Thread
{
public:
	typedef FunctionType ThreadFunctionType;

	TestThread(MainWindow* mainWindow, FunctionType function,
			   QObject *parent = nullptr)
		: Thread(parent), function(function), mainWindow(mainWindow), success(false) {}

	virtual bool succeeded() const {return success;}
	virtual const QString& message() const {return _message;}
	~TestThread() override{}
protected:
	ThreadFunctionType function;
	MainWindow* mainWindow;
	QString _message;
	bool success;
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


class SumThread : public TestThread<std::function<int(int)> >
{
	public:
	SumThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<std::function<int(int)> >(mainWindow, function, parent) {}
	void run() override;
	void _assert(int param, int result) const;
};

class SumWindow : public TestMainWindow<SumThread>
{
	Q_OBJECT

public:
	SumWindow(SumThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<SumThread>(function, parent) {}
};


class PowerThread : public TestThread<std::function<int(int, int)> >
{
	public:
	PowerThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
	void _assert(int value, int power, int result) const;
};

class PowerWindow : public TestMainWindow<PowerThread>
{
	Q_OBJECT

public:
	PowerWindow(PowerThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<PowerThread>(function, parent) {}
};




class FibonacciThread : public TestThread<std::function<int(int)> >
{
	public:
	FibonacciThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
	void _assert(int n, int result) const;
};

class FibonacciWindow : public TestMainWindow<FibonacciThread>
{
	Q_OBJECT

public:
	FibonacciWindow(FibonacciThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<FibonacciThread>(function, parent) {}
};


class SearchThread
		: public TestThread<std::function<int(int, Array&, int)> >
{

	public:
	SearchThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
};

class SearchWindow : public TestMainWindow<SearchThread>
{
	Q_OBJECT

public:
	SearchWindow(SearchThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<SearchThread>(function, parent) {}
};


class AllEvensThread
		: public TestThread<std::function<void(Array&, Array&, int, int)> >
{
	public:
	AllEvensThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}

	void run() override;
	void _assert(const Array& origin, const Array& result) const;
};

class AllEvensWindow : public TestMainWindow<AllEvensThread>
{
	Q_OBJECT

public:
	AllEvensWindow(AllEvensThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<AllEvensThread>(function, parent) {}
};



class MandelbrotThread : public TestThread<std::function<int(QVector2D, int, QVector2D)> >
{
	public:
	MandelbrotThread(MainWindow* mainWindow, ThreadFunctionType function, QObject *parent = nullptr)
		: TestThread<ThreadFunctionType>(mainWindow, function, parent) {}
	void run() override;
};

class MandelbrotWindow : public TestMainWindow<MandelbrotThread>
{
	Q_OBJECT

public:
	MandelbrotWindow(MandelbrotThread::ThreadFunctionType function, QWidget* parent=nullptr)
		: TestMainWindow<MandelbrotThread>(function, parent) {}
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
	BinarySearchTreeThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: Thread(parent), mainWindow(mainWindow), success(false), tree(tree) {}
	void run() override;

	virtual bool succeeded() const {return success;}
	virtual const QString& message() const {return _message;}
	~BinarySearchTreeThread() override{}
protected:
	MainWindow* mainWindow;
	QString _message;
	bool success;

	BinaryTree& tree;
};

class BinarySearchTreeHeightThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeHeightThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeHeightThread() override{}
private:
	uint treeHeight(const BinaryTree& tree) const;
};

class BinarySearchTreeCountThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeCountThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeCountThread() override{}
};

class BinarySearchTreeLeafThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeLeafThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
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
	BinarySearchTreeInorderThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeInorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};


class BinarySearchTreePreorderThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreePreorderThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreePreorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};


class BinarySearchTreePostorderThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreePostorderThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreePostorderThread() override{}

private:
	void _assert(Node* nodes[], size_t count) const;
};

class BinarySearchTreeSearchThread : public BinarySearchTreeThread
{

public:
	BinarySearchTreeSearchThread(MainWindow* mainWindow, BinaryTree& tree, QObject *parent = nullptr)
		: BinarySearchTreeThread(mainWindow, tree, parent) {}
	void run() override;

	~BinarySearchTreeSearchThread() override{}
};


class BinarySearchTreeWindow : public MainWindow
{
public:
	typedef MainWindow Base;

	BinarySearchTreeWindow(BinaryTree& tree, QWidget *parent=nullptr)
		: MainWindow(parent)
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


#define NOTIFY_START(name, ...) w->pushFunctionCall(name, __VA_ARGS__);

#endif // MAINWINDOW_H
