#include "mainwindow.h"

#include <time.h>
#include <chrono>
#include <thread>
#include <stdexcept>

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsWidget>
#include <QPair>

unsigned long MainWindow::instruction_duration = 1000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), timer(this), workerThread(nullptr), background(nullptr)
{
    initialize();
}

Array &MainWindow::newRandomArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
    array.fillRandom(0,255);
	numberItems.push_back(QVector<NumberGraphicsItem*>());
	this->dirty = true;
    return array;
}

Array &MainWindow::newSortedRandomArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
    array.fillSortedRandom(0,500);
    numberItems.push_back(QVector<NumberGraphicsItem*>());
    this->dirty = true;
    return array;
}

Array &MainWindow::newArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
	numberItems.push_back(QVector<NumberGraphicsItem*>());
	this->dirty = true;
    return array;
}

void MainWindow::pushFunctionCall(QString functionName, int parameter)
{
    functionCalls.push(QString("%1(%2)").arg(functionName).arg(QString::number(parameter)));
    std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
}

void MainWindow::pushFunctionCall(QString functionName, int parameter1, int parameter2)
{
    functionCalls.push(QString("%1(%2, %3)").arg(functionName)
                                            .arg(parameter1)
                                            .arg(parameter2));
    std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
}

void MainWindow::pushFunctionCall(QString functionName, QVector2D parameter1, int parameter2, QVector2D parameter3)
{
    functionCalls.push(QString("%1([%2, %3], %4, [%5, %6])").arg(functionName)
                                            .arg(parameter1.x())
                                            .arg(parameter1.y())
                                            .arg(parameter2)
                                            .arg(parameter3.x())
                                            .arg(parameter3.y()));
    std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
}

void MainWindow::pushFunctionCall(QString functionName, const Array& parameter1, int parameter2)
{
    functionCalls.push(QString("%1(array<%2 elements>, %3)").arg(functionName)
                                            .arg(parameter1.size())
                                            .arg(parameter2));
    std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
}

void MainWindow::pushFunctionCall(QString functionName, const Array &parameter1, const Array &parameter2, int parameter3, int parameter4)
{
    functionCalls.push(QString("%1(array<%2 elements>, array<%2 elements>, %4, %3)")
                                            .arg(functionName)
                                            .arg(parameter1.size())
                                            .arg(parameter2.size())
                                            .arg(parameter3)
                                            .arg(parameter4));
    std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
}

void MainWindow::popFunctionCall(int result)
{
    popFunctionCall(QString::number(result));
}

void MainWindow::popFunctionCall(QString result)
{
    if (!functionCalls.empty())
    {
        functionCalls.pop();
        QGraphicsTextItem* item = functionCallItems.takeLast();
        while (toAdd.contains(item))
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        toUpdate.push_back(QPair<QGraphicsTextItem*, QString>(item, item->toPlainText() + QString(" -> %1").arg(result)));
        std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration));
        toRemove.push_back(item);
    }
}

void MainWindow::updateLayout()
{
    int itemWidth = 50;
    this->scene.setSceneRect(0, 0, width(), qMax(height(), 2 * itemWidth * functionCalls.size()));
    QFont font("Arial", itemWidth * 0.35, QFont::DemiBold);
	statusItem.setFont(font);
    statusItem.setPos(scene.width() * 0.05f, scene.height() * 0.05f);
    if (background)
    {
        background->setPos(0,0);
        background->setPixmap(background->pixmap().scaledToWidth(scene.width()));
    }
	this->dirty=false;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
	this->dirty = true;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if (workerThread && workerThread->isRunning())
    {
        MainWindow::instruction_duration = 1;
        workerThread->wait(500);
    }
    QMainWindow::closeEvent(e);
}

void MainWindow::setBackground(QImage *image)
{
    if (background)
    {
        scene.removeItem(background);
    }
    if (!image)
    {
        background = nullptr;
        QRadialGradient gradient(-20, -40, 1920);
        gradient.setSpread(QGradient::ReflectSpread);
        gradient.setColorAt(0,QColor(120,40,70));
        gradient.setColorAt(1,QColor(25,8,60));
        scene.setBackgroundBrush(gradient);
    }
    else
    {
        scene.setBackgroundBrush(QBrush(Qt::black));
        background = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
        background->setPos(0, 0);
        background->setZValue(-1);
        toAdd.push_back(background);
    }
}

MainWindow::~MainWindow()
{
    if (workerThread && workerThread->isRunning())
    {
        MainWindow::instruction_duration = 1;
        workerThread->wait(500);
    }
}

void MainWindow::updateScene()
{
    if (workerThread && !workerThread->isRunning() && !workerThread->isFinished())
    {
		workerThread->start();
    }

    QFont font("Arial", 15, QFont::DemiBold);
    while (functionCallItems.size() <functionCalls.size())
    {
        QGraphicsTextItem* item = new QGraphicsTextItem(functionCalls[functionCallItems.size()]);
        functionCallItems.push_back(item);
        toAdd.push_back(item);

		item->setPos(50 + 30 * functionCallItems.size(),
					 150 + 30 * functionCallItems.size());
        item->setFont(font);
        item->setDefaultTextColor(QColor(215,235,255));
    }

    for (QGraphicsItem* item : toAdd)
    {
        scene.addItem(item);
        dirty=true;
    }
	toAdd.clear();

    for (QPair<QGraphicsTextItem*, QString>& update : toUpdate)
    {
        update.first->setPlainText(update.second);
        dirty=true;
    }
    toUpdate.clear();

    for (QGraphicsItem* item : toRemove)
    {
        item->hide();
        dirty=true;
    }
    toRemove.clear();

    if (dirty)
        updateLayout();
}

void MainWindow::handleResult()
{
	if (workerThread)
	{
		if (workerThread->succeeded())
		{
            statusItem.setPlainText(QString("Success, bien ouej: %1").arg(workerThread->message()));
			statusItem.setDefaultTextColor(QColor(100,230,100));
		}
		else
		{
            statusItem.setPlainText(QString("Fail: %1").arg(workerThread->message()));
			statusItem.setDefaultTextColor(QColor(230,50,100));
		}
		workerThread = nullptr;
    }
}

void MainWindow::initialize()
{
    qsrand(time(nullptr));

    setBackground(nullptr);

    view.setScene(&scene);
    view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setCentralWidget(&view);

    toAdd.append(&statusItem);
    statusItem.setDefaultTextColor(QColor(215,235,255));

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateScene()));
    arrays.reserve(40);
    this->resize(600, 600);
}

NumberGraphicsItem::NumberGraphicsItem(const int number, QGraphicsItem* parent) :
	QGraphicsEllipseItem(parent), text(QString::number(number)), number(number)
{
	QPen pen;
	pen.setWidth(3);
	setPen(pen);
    displayDefault();
}

void NumberGraphicsItem::setNumber(const int number)
{
    if(this->number != number)
    {
        text = QString::number(number);
        this->number = number;
    }
}

void NumberGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
							   QWidget *widget)
{
	QGraphicsEllipseItem::paint(painter, option, widget);
	painter->setFont(widget->parentWidget()->font());
	painter->drawText(this->boundingRect(), Qt::AlignCenter, text);
}

void NumberGraphicsItem::displayDefault()
{
	QPen pen(this->pen());
    pen.setColor(QColor(150,220,183).lighter(60+80*number/255.f));
	setPen(pen);
}
void NumberGraphicsItem::displayReadenState()
{
	QPen pen(this->pen());
    pen.setColor(QColor(255,100,160).lighter(100+20*number/255.f));
	setPen(pen);
}
void NumberGraphicsItem::displayWrittenState()
{
	QPen pen(this->pen());
    pen.setColor(QColor(150,150,255).lighter(100+20*number/255.f));
	setPen(pen);
}

bool TestThread::succeeded() const
{
    return isFinished() && success;
}

const QString &TestThread::message() const
{
    return _message;
}


SumWindow::SumWindow(std::function<int (int)> sumFunction, QWidget *parent)
    : MainWindow (parent)
{
    workerThread = new SumThread(this, sumFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}

PowerWindow::PowerWindow(std::function<int (int, int)> powerFunction, QWidget *parent)
    : MainWindow (parent)
{
    workerThread = new PowerThread(this, powerFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}

FibonacciWindow::FibonacciWindow(std::function<int (int)> fibonacciFunction, QWidget *parent)
    : MainWindow (parent)
{
    workerThread = new FibonacciThread(this, fibonacciFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}


SearchWindow::SearchWindow(std::function<int(int, Array&, int)> searchFunction, QWidget *parent)
 : MainWindow (parent)
{
    workerThread = new SearchThread(this, searchFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}


AllEvensWindow::AllEvensWindow(std::function<void (Array &, Array &, int, int)> allEvensFunction, QWidget *parent)
 : MainWindow (parent)
{
    workerThread = new AllEvensThread(this, allEvensFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}

MandelbrotWindow::MandelbrotWindow(std::function<int (QVector2D, int, QVector2D)> isMandelbrotFunction, QWidget *parent)
{
    resize(1280, 720);
    workerThread = new MandelbrotThread(this, isMandelbrotFunction, this);
    connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

    timer.setInterval(MainWindow::instruction_duration * 0.3f);
    timer.start();
}



void SumThread::run() {
	try
	{
        qsrand(time(nullptr));

        int n = qrand() % 10 + 5;
        int result = sumFunction(n);
        assert(n, result);
        _message = QString("Sum of 1..%1 is %2").arg(n).arg(result);
        success = true;
	}
	catch(std::exception& e)
	{
        _message = QString(e.what());
	}
}

void SumThread::assert(int param, int result) const
{
    int sum=0;
    for (int i=0;i<=param;++i) {
        sum += i;
    }
    if (result != sum)
    {
        QString message("Sum of 1..%1 failed: %2 has been processed but it should be %3");
        throw std::runtime_error(message.arg(param)
                                        .arg(result)
                                        .arg(sum)
                                        .toStdString());
    }
}

void PowerThread::run() {
    try
    {
        qsrand(time(nullptr));

        int value = qrand() % 15;
        int n = qrand() % 8;
        int result = powerFunction(value, n);
        assert(value, n, result);
        _message = QString("Power %1 of %2 is %3").arg(value).arg(n).arg(result);
        success = true;
    }
    catch(std::exception& e)
    {
        _message = QString(e.what());
    }
}

void PowerThread::assert(int value, int param, int result) const
{
    int power=pow(value, param);
    if (result != power)
    {
        QString message("Power %1 of %2 failed: %3 has been processed but it should be %4");
        throw std::runtime_error(message.arg(param)
                                        .arg(value)
                                        .arg(result)
                                        .arg(power)
                                        .toStdString());
    }
}

void FibonacciThread::run()
{
    try
    {
        qsrand(time(nullptr));

        int n = qrand() % 10 + 5;
        int result = fibonacciFunction(n);
        assert(n, result);
        _message = QString("Fibonacci %1 is %2").arg(n).arg(result);
        success = true;
    }
    catch(std::exception& e)
    {
        _message = QString(e.what());
    }
}

void FibonacciThread::assert(int n, int result) const
{
    int fibo1 = 0;
    int fibo2 = 1;
    int fibo = fibo1;
    if (n == 1)
        fibo = 1;
    else if (n > 1)
    {
        for (int i=1; i<n; ++i)
        {
            fibo=fibo1+fibo2;
            fibo1=fibo2;
            fibo2=fibo;
        }
    }
    if (result != fibo)
    {
        QString message("Fibonacci %1 failed: %2 has been processed but it should be %3");
        throw std::runtime_error(message.arg(n)
                                        .arg(result)
                                        .arg(fibo)
                                        .toStdString());
    }
}


void SearchThread::run()
{
    try
    {
        qsrand(time(nullptr));

        int n = qrand() % 10 + 5;
        Array& a = mainWindow->newRandomArray(n);
        int index = qrand() % a.size() - 1;
        int toSearch = -1;
        if (index >= 0)
            toSearch = a.get(index);
        int result = searchFunction(toSearch, a, a.size());
        if (result != index)
        {
            QString message("Search failed: %1 has been found but it should be %2\nArray was: ");
            message.append(a.toString());
            throw std::runtime_error(message.arg(result)
                                            .arg(index)
                                            .toStdString());
        }
        if (index >= 0)
            _message = QString("%1 is at index %2\nArray was:").arg(toSearch).arg(result);
        else
            _message = QString("%1 is not in the array\nArray was:").arg(toSearch);
        _message.append(a.toString());
        success = true;
    }
    catch(std::exception& e)
    {
        _message = QString(e.what());
    }
}

void AllEvensThread::run()
{
    try
    {
        qsrand(time(nullptr));

        int n = qrand() % 10 + 5;
        Array& a = mainWindow->newRandomArray(n);
        Array& result = mainWindow->newArray(n);
        searchFunction(result, a, 0, a.size());
        assert(a, result);
        _message = QString("%1 evens found\nArray was:").arg(result.effectiveSize());
        _message.append(a.toString());
        success = true;
    }
    catch(std::exception& e)
    {
        _message = QString(e.what());
    }
}

void AllEvensThread::assert(const Array &origin, const Array &result) const
{
    for (int i=0; i<origin.size(); ++i)
    {
        if (origin.__get__(i) % 2 == 0)
        {
            if (!result.contains(origin.__get__(i)))
                throw std::runtime_error(QString("%1 is an even number but it's not in result\nArray was %2")
                                         .arg(origin.__get__(i))
                                         .arg(origin.toString())
                                              .toStdString());
        }
        else
        {
            if (result.contains(origin.__get__(i)))
                throw std::runtime_error(QString("%1 is an odd number but it's in result\nArray was %2")
                                         .arg(origin.__get__(i))
                                          .arg(origin.toString())
                                              .toStdString());
        }
    }
}

void MandelbrotThread::run()
{
    qsrand(time(nullptr));

    QImage* mandelbrotPicture = new QImage(1024, 1024, QImage::Format_RGB32);
    double n = qrand() % 96 + 5;
    QRgb white = qRgb(qrand() % 125 + 50, qrand() % 125 + 75, qrand() % 125 + 50);
    QRgb black = qRgb(qrand() % 20, qrand() % 30, qrand() % 20);
    QVector2D z((qrand() % 200 - 100) * 0.0002, (qrand() % 200 - 100) * 0.0002);

    double factor = 0.5 + (qrand() % 2000 - 1500) * 0.001;
    double precision = 1000000. / factor;
    double width = mandelbrotPicture->width() * precision * 0.25, height = mandelbrotPicture->height() * precision * 0.5;


    for (int i=0; i<mandelbrotPicture->width(); ++i)
        for (int j=0; j<mandelbrotPicture->height(); ++j)
        {
            QVector2D point(factor * (-2.5 + (precision * (double)(i)) / width) - 0.5 * (1-factor),
                            factor * ((-.5 + (precision * (double)(j)) / height)) - 0.25 * (1-factor));
            double iter = isMandelbrotFunction(z, n, point);
            if (iter > 0.f)
            {
                QColor color(white);
                mandelbrotPicture->setPixel(i, j, color.darker(200. * iter / n).rgb());
            }
            else
            {
                mandelbrotPicture->setPixel(i, j, black);
            }
        }
    mainWindow->setBackground(mandelbrotPicture);
    success=true;
    _message = QString("Mandebrot: n=%1 z=[%2, %3] factor=%4").arg(n).arg(z.x()).arg(z.y()).arg(factor);
}
