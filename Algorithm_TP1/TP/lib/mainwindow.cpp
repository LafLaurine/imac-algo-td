#include "mainwindow.h"

#include <time.h>
#include <stdexcept>

#include <QGraphicsItem>
#include <QGraphicsWidget>

MainWindow::MainWindow(std::function<void(Array&)> sortFunction, uint elementCount, QWidget *parent) :
    QMainWindow(parent), timer(this), workerThread(nullptr)
{
	qsrand(time(nullptr));

	QRadialGradient gradient(-20, -40, 1920);
    gradient.setSpread(QGradient::ReflectSpread);
	gradient.setColorAt(0,QColor(60,20,70));
	gradient.setColorAt(1,QColor(25,8,60));
	scene.setBackgroundBrush(gradient);

	view.setScene(&scene);
	view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	this->setCentralWidget(&view);

	toAdd.append(&statusItem);
	statusItem.setDefaultTextColor(QColor(200,164,187));

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateScene()));
	arrays.reserve(elementCount*2);
	numberItems.reserve(elementCount*2);
    newRandomArray(elementCount);
	this->resize(600, 600);

	workerThread = new SorterThread(this, sortFunction, this);
	connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

	timer.setInterval(Array::instruction_duration);
	timer.start();
}

MainWindow::MainWindow(std::function<int (const Array &, const int)> findFunction)
    : QMainWindow(nullptr), workerThread(nullptr), timer(this)
{
	qsrand(time(nullptr));

	QRadialGradient gradient(-20, -40, 1920);
	gradient.setSpread(QGradient::ReflectSpread);
	gradient.setColorAt(0,QColor(60,20,70));
	gradient.setColorAt(1,QColor(80,8,60));
	scene.setBackgroundBrush(gradient);

	view.setScene(&scene);
	view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	this->setCentralWidget(&view);

	toAdd.append(&statusItem);
	statusItem.setDefaultTextColor(QColor(200,164,187));

	connect(&timer, SIGNAL(timeout()), this, SLOT(updateScene()));
	arrays.reserve(20);
	numberItems.reserve(20);
    newSortedRandomArray(20);
	this->resize(600, 600);

	workerThread = new FinderThread(this, findFunction, this);
	connect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));

	timer.setInterval(Array::instruction_duration);
	timer.start();
}

Array &MainWindow::newRandomArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
    array.fillRandom(0,255);
	numberItems.push_back(QVector<NumberGraphicsItem*>());
	this->dirty = true;
	Array::registerArray(array);
    return array;
}

Array &MainWindow::newSortedRandomArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
    array.fillSortedRandom(0,500);
    numberItems.push_back(QVector<NumberGraphicsItem*>());
    this->dirty = true;
	Array::registerArray(array);
    return array;
}

Array &MainWindow::newArray(uint size)
{
    arrays.push_back(Array(size));
    Array& array = arrays.last();
	numberItems.push_back(QVector<NumberGraphicsItem*>());
	this->dirty = true;
	Array::registerArray(array);
    return array;
}

void MainWindow::updateLayout()
{
	uint maxSize = numberItems[0].size();
	for (QVector<NumberGraphicsItem*>& items : numberItems)
		if (items.size() > maxSize)
			maxSize = items.size();

	int itemWidth = qBound(30, (int)(0.75f*view.width()/maxSize), 100);
	this->scene.setSceneRect(0, 0, qMax<int>(itemWidth * maxSize * 1.2f, scene.width()),
							 qMax<int>(itemWidth * numberItems.size() * 3.1, scene.height()));
	int startX = (scene.width() - itemWidth * maxSize * 1.2f) * 0.5f;
	int startY = (scene.height() - itemWidth * 3 * numberItems.size()) * 0.5f;


	QFont font("Arial", itemWidth * 0.25, QFont::DemiBold);

	uint j = 0;
	for (QVector<NumberGraphicsItem*>& items : numberItems)
	{
		for (uint i=0; i<items.size(); ++i)
		{
			items[i]->setRect(startX + i * itemWidth * 1.2,
							  startY + itemWidth,
							  itemWidth,itemWidth);
		}
        if (accessInfoItem.size() > j)
        {
            accessInfoItem[j]->setPos(startX, startY);
            accessInfoItem[j]->setFont(font);
            accessInfoItem[j]->setDefaultTextColor(QColor(255,255,150));
        }

		startY += itemWidth * 3;
		++j;
	}
	this->view.setFont(font);
	font.setPointSize(itemWidth * 0.33f);
	statusItem.setFont(font);
	statusItem.setPos(itemWidth * 3, startY - itemWidth);
	this->dirty=false;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    if (arrays.isEmpty())
        return;

	this->scene.setSceneRect(0, 0, this->view.width()-10, this->view.height()-10);
	this->dirty = true;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if (workerThread && workerThread->isRunning())
    {
        Array::instruction_duration = 1;
        workerThread->wait(500);
    }
    QMainWindow::closeEvent(e);
}

MainWindow::~MainWindow()
{
    if (workerThread && workerThread->isRunning())
    {
        Array::instruction_duration = 1;
        workerThread->wait(500);
    }
}

void MainWindow::updateScene()
{
    if (arrays.isEmpty())
        return;

	if (workerThread && !workerThread->isRunning())
    {
		workerThread->start();
    }

    uint j = 0;
    QString accessInfoText("Read: %1; Write: %2");
	for (QVector<NumberGraphicsItem*>& items : numberItems)
    {
		Array& array = arrays[j];
		if (array.empty())
			continue;
		for (uint i=items.size(); i<array.size(); ++i)
		{
			items.push_back(new NumberGraphicsItem(array.__get__(i)));
			items.last()->setRect(0,0,30,30);
			toAdd.append(items.last());
		}
        for (uint i=0; i<items.size(); ++i)
        {
			items[i]->setNumber(array.__get__(i));
			if (array.hasBeenReadenAt(i))
                items[i]->displayReadenState();
			else if (array.hasBeenWrittenAt(i))
                items[i]->displayWrittenState();
            else
                items[i]->displayDefault();
        }
        if (j >= accessInfoItem.size())
        {
            accessInfoItem.push_back(new QGraphicsTextItem ());
            toAdd.append(accessInfoItem.last());
        }
		accessInfoItem[j]->setPlainText(accessInfoText.arg(QString::number(array.readAccessCount()))
												 .arg(QString::number(array.writeAccessCount())));
        ++j;
	}

	for (QGraphicsItem* item : toAdd)
	{
		scene.addItem(item);
		dirty=true;
	}
	toAdd.clear();

	if (dirty)
		updateLayout();
}

void MainWindow::handleResult()
{
	if (workerThread && workerThread->isFinished())
	{
		if (workerThread->succeeded())
		{
			statusItem.setPlainText(QString("Success: GG Gros\nTotal Read: %1\nTotal Write: %2")
										.arg(QString::number(Array::totalReadAccess()))
										.arg(QString::number(Array::totalWriteAccess())));
			statusItem.setDefaultTextColor(QColor(100,230,100));
		}
		else
		{
			statusItem.setPlainText(QString("Fail:%1").arg(workerThread->errorMessage()));
			statusItem.setDefaultTextColor(QColor(230,50,100));
		}
		workerThread = nullptr;
	}
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


void SorterThread::run() {
	if (terminated)
		return;
	try
	{
		Array& arrayToSort = mainWindow->mainArray();
		this->sortFunction(arrayToSort);
		assertSort(arrayToSort);
	}
	catch(std::exception& e)
	{
		_error = QString(e.what());
	}
	terminated = true;
}

void SorterThread::assertSort(const Array& array) const
{
	for (uint i=0; i<array.size()-1; ++i)
		if (array.__get__(i) > array.__get__(i+1))
		{
			QString message("Sorting failed: %1 > %2");
            throw std::runtime_error(message.arg(array.__get__(i))
										.arg(array.__get__(i+1))
										.toStdString());
		}
}

bool SorterThread::succeeded() const
{
	return isFinished() && _error.isEmpty();
}

const QString &SorterThread::errorMessage() const
{
	return _error;
}

void FinderThread::run()
{
	try
	{
		Array& array = mainWindow->mainArray();
		int foundIndex = this->findFunction(array, array.__get__(indexToFind));
		if (foundIndex != indexToFind)
		{
			QString message("Index to found was: %1, index found: %2");
			throw std::runtime_error(message.arg(indexToFind)
											.arg(foundIndex)
											.toStdString());
		}
	}
	catch(std::exception& e)
	{
		_error = QString(e.what());
	}
}

bool FinderThread::succeeded() const
{
	return isFinished() && _error.isEmpty();
}

const QString &FinderThread::errorMessage() const
{
	return _error;
}

bool TestThread::isTerminated() const
{
	return terminated;
}
