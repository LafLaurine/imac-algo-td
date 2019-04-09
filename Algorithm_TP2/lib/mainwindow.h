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
#include <QStack>
#include <QLinkedList>
#include <QVector2D>
#include <math.h>
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
        : QThread(parent), mainWindow(mainWindow), success(false) {}
    virtual bool succeeded() const;
    virtual const QString& message() const;
	virtual ~TestThread() {}
protected:
	MainWindow* mainWindow;
    QString _message;
    bool success;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=nullptr);

    Array& mainArray() {return arrays[0];}
    Array& newRandomArray(uint size);
    Array& newSortedRandomArray(uint size);
    Array& newArray(uint size);

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

    ~MainWindow();
public slots:
    void updateScene();
	void handleResult();

protected:
    void initialize();

	QGraphicsScene scene;
	QGraphicsView view;

    QVector<QVector<NumberGraphicsItem*>> numberItems;
	QVector<QGraphicsTextItem *> accessInfoItem;
    QLinkedList<QGraphicsTextItem *> functionCallItems;
	QGraphicsTextItem statusItem;
    QVector<Array> arrays;

    QTimer timer;
	TestThread* workerThread;

	QVector<QGraphicsItem*> toAdd;
    QVector<QPair<QGraphicsTextItem*, QString>> toUpdate;
    QVector<QGraphicsItem*> toRemove;
    QStack<QString> functionCalls;

    QGraphicsPixmapItem* background;

	bool dirty;
};

class SumWindow : public MainWindow
{
public:
    explicit SumWindow(std::function<int(int)> sumFunction, QWidget *parent=nullptr);
};

class PowerWindow : public MainWindow
{
public:
    explicit PowerWindow(std::function<int(int, int)> powerFunction, QWidget *parent=nullptr);
};

class FibonacciWindow : public MainWindow
{
public:
    explicit FibonacciWindow(std::function<int(int)> fibonacciFunction, QWidget *parent=nullptr);
};

class SearchWindow : public MainWindow
{
public:
    explicit SearchWindow(std::function<int(int, Array&, int)> searchFunction, QWidget *parent=nullptr);
};

class AllEvensWindow : public MainWindow
{
public:
    explicit AllEvensWindow(std::function<void(Array&, Array&, int, int)> allEvensFunction, QWidget *parent=nullptr);
};

class MandelbrotWindow : public MainWindow
{
public:
    explicit MandelbrotWindow(std::function<int(QVector2D, int, QVector2D)> isMandelbrotFunction, QWidget *parent=nullptr);
};



class SumThread : public TestThread
{
public:
    explicit SumThread(MainWindow* mainWindow, std::function<int(int)> sumFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), sumFunction(sumFunction)
		{}
	void run() override;
    void assert(int param, int result) const;
private:
    std::function<int(int)> sumFunction;
};

class PowerThread : public TestThread
{
public:
    explicit PowerThread(MainWindow* mainWindow, std::function<int(int, int)> powerFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), powerFunction(powerFunction)
        {}
    void run() override;
    void assert(int value, int power, int result) const;
private:
    std::function<int(int, int)> powerFunction;
};

class FibonacciThread : public TestThread
{
public:
    explicit FibonacciThread(MainWindow* mainWindow, std::function<int(int)> fibonacciFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), fibonacciFunction(fibonacciFunction)
        {}
    void run() override;
    void assert(int n, int result) const;
private:
    std::function<int(int)> fibonacciFunction;
};

class SearchThread : public TestThread
{
public:
    explicit SearchThread(MainWindow* mainWindow, std::function<int(int, Array&, int)> searchFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), searchFunction(searchFunction)
        {}
    void run() override;
private:
    std::function<int(int, Array&, int)> searchFunction;
};

class AllEvensThread : public TestThread
{
public:
    explicit AllEvensThread(MainWindow* mainWindow, std::function<void(Array&, Array&, int, int)> searchFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), searchFunction(searchFunction)
        {}
    void run() override;
    void assert(const Array& origin, const Array& result) const;
private:
    std::function<void(Array&, Array&, int, int)> searchFunction;
};

class MandelbrotThread : public TestThread
{
public:
    explicit MandelbrotThread(MainWindow* mainWindow, std::function<int (QVector2D, int, QVector2D)> isMandelbrotFunction,
                       QObject *parent = nullptr)
        : TestThread(mainWindow, parent), isMandelbrotFunction(isMandelbrotFunction)
        {}
    void run() override;
private:
    std::function<int (QVector2D, int, QVector2D)> isMandelbrotFunction;
};

#define NOTIFY_START(name, ...) w->pushFunctionCall(name, __VA_ARGS__);

#endif // MAINWINDOW_H
