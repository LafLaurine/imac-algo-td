#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QTimer>
#include <QThread>

#include "array.h"

class MainWindow;

class NumberGraphicsItem : public QGraphicsEllipseItem
{
public:
	explicit NumberGraphicsItem(const int number, QGraphicsItem* parent=nullptr);
    void setNumber(const int number);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget=nullptr) override;

	void displayDefault();
	void displayReadenState();
	void displayWrittenState();
private:
	QString text;
	int number;
};

class TestThread : public QThread
{
public:
	TestThread(MainWindow* mainWindow, QObject* parent=nullptr)
		: QThread(parent), mainWindow(mainWindow), terminated(false) {}
	virtual bool succeeded() const = 0;
	virtual const QString& errorMessage() const = 0;
	virtual ~TestThread() {}

	bool isTerminated() const;
protected:
	MainWindow* mainWindow;
	bool terminated;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit MainWindow(std::function<void(Array&)> sortFunction, uint elementCount=25, QWidget *parent=nullptr);
    explicit MainWindow(std::function<int(const Array&, const int)> findFunction);

	Array& mainArray() {return arrays[0];}
    Array& newRandomArray(uint size);
    Array& newSortedRandomArray(uint size);
    Array& newArray(uint size);

	void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

	void updateLayout();

    ~MainWindow();
public slots:
    void updateScene();
	void handleResult();

private:
	QGraphicsScene scene;
	QGraphicsView view;

    QVector<QVector<NumberGraphicsItem*>> numberItems;
	QVector<QGraphicsTextItem *> accessInfoItem;
	QGraphicsTextItem statusItem;
    QVector<Array> arrays;

    QTimer timer;
	TestThread* workerThread;

	QVector<QGraphicsItem*> toAdd;

	bool dirty;
};

class SorterThread : public TestThread
{
public:
	explicit SorterThread(MainWindow* mainWindow,
						  std::function<void(Array&)> sortFunction, QObject *parent = nullptr)
		: TestThread(mainWindow, parent), sortFunction(sortFunction)
		{}
	void run() override;
	bool succeeded() const override;
	void assertSort(const Array& array) const;
	const QString& errorMessage() const override;
private:
	std::function<void(Array&)> sortFunction;
	QString _error;
};

class FinderThread : public TestThread
{
public:
	explicit FinderThread(MainWindow* mainWindow,
						  std::function<int(const Array&, const int)> findFunction,
						  QObject *parent = nullptr)
		: TestThread(mainWindow, parent), findFunction(findFunction)
	{
		indexToFind = qrand() % mainWindow->mainArray().size();
	}
	void run() override;
	bool succeeded() const override;
	const QString& errorMessage() const override;
private:
	std::function<int(const Array&, const int)> findFunction;
	int indexToFind;
	QString _error;
};

#endif // MAINWINDOW_H
