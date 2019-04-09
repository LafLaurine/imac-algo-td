#ifndef TP4_H
#define TP4_H

#include "mainwindow.h"
#include "heap.h"
#include "thread.h"
#include "tp3.h"

class HeapNumberGraphicsItem : public TreeNumberGraphicsItem
{
public:
	HeapNumberGraphicsItem(BaseHeap* heap, int i, QGraphicsItem* parent=nullptr,
						   float leftShiftting=0, float rightShiftting=0);
	void updateNode();
	bool needUpdate() const;

private:
	BaseHeap* heap;
	int i;
};


class HeapThread : public Thread
{

public:
	HeapThread(MainWindow* mainWindow, BaseHeap& tree, QObject *parent = nullptr)
		: Thread(parent), mainWindow(mainWindow), success(false), heap(tree) {}

	virtual bool succeeded() const {return success;}
	virtual const QString& message() const {return _message;}
	~HeapThread() override{}

	void assertHeap();
protected:
	MainWindow* mainWindow;
	QString _message;
	bool success;

	BaseHeap& heap;
};

class HeapChildsThread : public HeapThread
{

public:
	HeapChildsThread(MainWindow* mainWindow, BaseHeap& tree, QObject *parent = nullptr)
		: HeapThread(mainWindow, tree, parent) {}
	void run() override;

	~HeapChildsThread() override{}
};

class HeapInsertThread : public HeapThread
{

public:
	HeapInsertThread(MainWindow* mainWindow, BaseHeap& tree, QObject *parent = nullptr)
		: HeapThread(mainWindow, tree, parent) {}
	void run() override;

	~HeapInsertThread() override{}
};

class HeapHeapifyThread : public HeapThread
{

public:
	HeapHeapifyThread(MainWindow* mainWindow, BaseHeap& tree, QObject *parent = nullptr)
		: HeapThread(mainWindow, tree, parent) {}
	void run() override;

	~HeapHeapifyThread() override{}
};

class HeapSortThread : public HeapThread
{

public:
	HeapSortThread(MainWindow* mainWindow, BaseHeap& tree, QObject *parent = nullptr)
		: HeapThread(mainWindow, tree, parent) {}
	void run() override;

	~HeapSortThread() override{}
};

class HeapTestMainWindow : public _TestMainWindow
{
public:
	typedef _TestMainWindow Base;

	HeapTestMainWindow(QWidget *parent=nullptr)
		: _TestMainWindow(parent)
	{}

	void updateScene()
	{
		while (heapItems.size() < heaps.size())
		{
			BaseHeap& heap = *heaps[heapItems.size()];
			HeapNumberGraphicsItem* item = new HeapNumberGraphicsItem(&heap, 0);
			heapItems.push_back(item);
			toAdd.append(item);
		}

		for (HeapNumberGraphicsItem* item : heapItems)
		{
			if (item->needUpdate())
				dirty = true;
		}
		Base::updateScene();
	}

	void updateLayout()
	{
		int itemWidth = qMax<int>(50, width() * 0.01f);
		int maxX = 0, maxY = 0;
		updateStatusItem(itemWidth);
		updateBackground();
		updateNumberItems(itemWidth, maxX, maxY);
		updateTreeItems(itemWidth, maxX, maxY);
		updateHeapNumberItems(itemWidth, maxX, maxY);

		this->scene.setSceneRect(0, 0, qMax(width(), maxX), qMax(height(),maxY+itemWidth * 2));
		this->dirty=false;
	}

	void updateHeapNumberItems(int itemWidth, int &maxY, int &maxX)
	{
		for (HeapNumberGraphicsItem* item : heapItems)
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

protected:
	QVector<BaseHeap*> heaps;
	QLinkedList<HeapNumberGraphicsItem *> heapItems;
};

template<typename HeapType>
class HeapWindow : public HeapTestMainWindow
{
public:
	typedef HeapType HeapClass;
	typedef _TestMainWindow Base;

	HeapWindow(QWidget *parent=nullptr)
		: HeapTestMainWindow(parent)
	{
		heap = &this->newRandomHeap(1);

		threads.push_back(new HeapChildsThread(this, *heap, this));
		threads.push_back(new HeapInsertThread(this, *heap, this));
		threads.push_back(new HeapHeapifyThread(this, *heap, this));
		threads.push_back(new HeapSortThread(this, *heap, this));

		currentThread = threads.begin();
		workerThread = *currentThread;
		connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
	}

	HeapClass& newRandomHeap(uint size)
	{
		HeapClass* array = new HeapClass(size);
		heaps.push_back(array);
		array->fillSortedRandom(0,500);
		this->dirty = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
		return *array;
	}

	void handleResult() override
	{
		Base::handleResult();
		this->dirty = true;
		if (!(*currentThread)->succeeded())
			return;
		currentThread++;
		if (currentThread != threads.end())
		{
			workerThread = *currentThread;
			connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
		}
	}

private:
	HeapClass* heap;
	QLinkedList<Thread*> threads;
	QLinkedList<Thread*>::iterator currentThread;
};


class Heap : public BaseHeap
{
public:
	Heap(size_t size=100);
	Heap(const Heap& other);
	Heap(const std::vector<int>& other);

	virtual ~Heap() {}

	int leftChild(int nodeIndex);

	int rightChild(int nodeIndex);

	void insertHeapNode(int heapSize, int value);

	void heapify(int heapSize, int nodeIndex);

	void buildHeap(Array& numbers);

	void heapSort();
};


struct HuffmanNode : public TemplateNode<HuffmanNode>
{
	HuffmanNode(char c='\0', int frequences=0)
		: TemplateNode<HuffmanNode>(frequences)
	{
		this->character = c;
		this->code = std::string();
	}

	HuffmanNode(const HuffmanNode& node)
	: TemplateNode<HuffmanNode>(node)
	{
		this->character = node.character;
		this->value = node.value;
		this->code = std::string(node.code);
	}

	virtual ~HuffmanNode() {}

	HuffmanNode* find(int) {return nullptr;}

	virtual bool isLeaf() const {return !this->left && !this->right;}

	void fillCharactersArray(HuffmanNode** nodes)
	{
		if (!this->left && !this->right)
		{
			nodes[this->character] = this;
		}
		else {
			if (this->left)
			{
				this->left->fillCharactersArray(nodes);
			}
			if (this->right)
			{
				this->right->fillCharactersArray(nodes);
			}
		}
	}

	QString toString() const
	{
		return QString("[char: %1, frequences: %2]").arg(character).arg(value);
	}

	void insertNode(HuffmanNode*);
	void processCodes(std::string baseCode);

	char character;
	std::string code;
};


class HuffmanHeap : public TemplateArray<HuffmanNode>
{
public:
	HuffmanHeap(size_t size=100);
	HuffmanHeap(const HuffmanHeap& other);
	HuffmanHeap(const std::vector<HuffmanNode>& other);

	virtual ~HuffmanHeap() {}

	size_t effectiveSize() const;

	QString toString() const;

	int leftChild(int nodeIndex);

	int rightChild(int nodeIndex);

	void insertHeapNode(int heapSize, char c, int frequences);

	void heapify(int heapSize, int nodeIndex);

	void buildHeap(TemplateArray<HuffmanNode>& numbers);

	void heapSort();

	virtual void clear(int start, int end);
};
#endif // TP4_H
