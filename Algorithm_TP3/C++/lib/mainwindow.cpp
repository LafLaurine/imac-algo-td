#include "mainwindow.h"

#include <QDebug>
#include <time.h>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <math.h>
#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsWidget>
#include <QPair>


unsigned long MainWindow::instruction_duration = 1000;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), timer(this), background(nullptr), zoom(false), scale(1)
{
	initialize();
	resize(1280, 720);

	timer.setInterval(MainWindow::instruction_duration * 0.49f);
	timer.start();
}

void MainWindow::initialize()
{
	qsrand(time(nullptr));

	setBackground(nullptr);

	view.setScene(&scene);
	view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	view.setMouseTracking(true);
	view.installEventFilter(this);
	view.viewport()->installEventFilter(this);
	this->setCentralWidget(&view);

	toAdd.append(&statusItem);
	statusItem.setDefaultTextColor(QColor(215,235,255));

	connect(&timer, SIGNAL(timeout()), this, SLOT(updateScene()));
	arrays.reserve(40);
	this->resize(600, 600);
}

Array &MainWindow::newRandomArray(uint size)
{
	arrays.push_back(Array(size));
	Array& array = arrays.last();
	array.fillRandom(0,255);
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
	return array;
}

Array &MainWindow::newSortedRandomArray(uint size)
{
	arrays.push_back(Array(size));
	Array& array = arrays.last();
	array.fillSortedRandom(0,500);
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
	return array;
}

Array &MainWindow::newArray(uint size)
{
	arrays.push_back(Array(size));
	Array& array = arrays.last();
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
	return array;
}

void MainWindow::addBinaryNode(Node* node)
{
	nodes.push_back(node);
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
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
	int itemWidth = qMax<int>(50, width() * 0.01f);
	QFont font("Arial", itemWidth * 0.35, QFont::DemiBold);
	statusItem.setFont(font);
	statusItem.setPos(scene.width() * 0.05f, scene.height() * 0.05f);
	if (background)
	{
		background->setPos(0,0);
		background->setPixmap(background->pixmap().scaledToWidth(scene.width()));
	}
	int startY = scene.height() * 0.1f;
	int maxX = 0;
	for (QVector<NumberGraphicsItem*> items : numberItems)
	{
		int startX = qMax((50 + scene.width() - itemWidth * 1.5f * items.size()) * 0.5,
							0.05 * scene.width());
		for (NumberGraphicsItem* item : items)
		{
			item->setRect(startX, startY, itemWidth, itemWidth);
			startX += itemWidth * 1.5f;
			if (startX + itemWidth * 2 > scene.width())
			{
				startY += itemWidth * 1.5f;
				startX = qMax((50 + scene.width() - itemWidth * 1.5f * items.size()) * 0.5,
											0.05 * scene.width());
			}
		}
		if (startX > maxX)
			maxX = startX;
		startY += itemWidth * 2;
	}
	for (TreeNumberGraphicsItem* item : treeItems)
	{
		item->updateNode();
		int treeWidth = item->width();
		int startX = scene.width() * 0.5f;
		item->setRect(startX, startY, itemWidth, itemWidth);
		item->updateLayout();
		startY += item->height();
		if (startX + treeWidth > maxX)
		{
			maxX = startX + treeWidth;
		}
	}
	statusItem.setTextWidth(width() / 2);
	this->scene.setSceneRect(0, 0, qMax(width(), maxX), qMax(height(),startY+itemWidth * 2));
	this->dirty=false;
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Wheel)
	{
		QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
		if (QApplication::keyboardModifiers() == Qt::KeyboardModifier::ControlModifier) {
            float wheelDelta = wheel_event->delta();
			scale += std::pow(1.01f, std::abs(wheelDelta) / 24.f) * (std::signbit(wheelDelta) ? -1 : 1) / 5.f;
			scale = qMin<float>(2.5f, qMax<float>(0.25f, scale));
			zoom = true;
            targetPos = wheel_event->pos();
			return true;
		}
	}
	return QMainWindow::eventFilter(object, event);
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
		gradient.setColorAt(0,QColor(60,28,52));
		gradient.setColorAt(1,QColor(30,10,24));
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

		item->setPos(50 + 30 * functionCallItems.size(), 150 + 30 * functionCallItems.size());
		item->setFont(font);
		item->setDefaultTextColor(QColor(215,235,255));
	}

	while (numberItems.size() < arrays.size())
	{
		Array& array = arrays[numberItems.size()];
		numberItems.push_back(QVector<NumberGraphicsItem*>());
		for (uint i=0; i < array.size(); ++i)
		{
			NumberGraphicsItem* item = new NumberGraphicsItem(array.__get__(i));
			numberItems.last().push_back(item);
			toAdd.push_back(item);
		}
	}
	int i=0;
	for (QVector<NumberGraphicsItem*>& numbers : numberItems)
	{
		Array& array = arrays[i];
		for (uint j=0; j < array.size(); ++j)
		{
			NumberGraphicsItem* item = numbers[j];
			if (item->number() != array.__get__(j))
			{
				dirty = true;
				item->setNumber(array.__get__(j));
			}
			if (array.hasBeenReadenAt(j))
				item->displayReadenState();
			else if (array.hasBeenWrittenAt(j))
				item->displayWrittenState();
			else
				item->displayDefault();
		}
		++i;
	}

	while (treeItems.size() < nodes.size())
	{
		Node* node = nodes[treeItems.size()];
		TreeNumberGraphicsItem* item = new TreeNumberGraphicsItem(node);
		treeItems.push_back(item);
		toAdd.append(item);
	}

	for (TreeNumberGraphicsItem* item : treeItems)
	{
		if (item->needUpdate())
			dirty = true;
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

	if (zoom)
	{
		dirty = true;
		qDebug() << view.mapToScene(targetPos.toPoint()) << QString::number(scale);
		view.centerOn(view.mapToScene(targetPos.toPoint()));
		view.setTransform(QTransform::fromScale(scale, scale));
		zoom = false;
	}

	if (dirty)
	{
		updateLayout();
		repaint();
	}
}

void MainWindow::handleResult()
{
	if (workerThread && workerThread->message().size())
	{
		if (workerThread->succeeded())
		{
			statusItem.setPlainText(QString("Success, bien ouej: %1").arg(workerThread->message()));
			statusItem.setDefaultTextColor(QColor(100,230,100));
		}
		else
		{
			statusItem.setPlainText(QString("Fail: %1").arg(workerThread->message()));
			statusItem.setDefaultTextColor(QColor(250,50,80));
		}
		disconnect(workerThread, SIGNAL(finished()), this, SLOT(handleResult()));
		workerThread->deleteLater();
		workerThread = nullptr;
	}
}

NumberGraphicsItem::NumberGraphicsItem(const int number, QGraphicsItem* parent) :
	QGraphicsEllipseItem(parent), text(QString::number(number)), _number(number), state(STATE_COUNT)
{
	QPen pen;
	pen.setWidth(3);
	setPen(pen);
	displayDefault();
}

void NumberGraphicsItem::setNumber(const int number)
{
	if(this->_number != number)
	{
		text = QString::number(number);
		this->_number = number;
		displayDefault(true);
		update();
	}
}

void NumberGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
							   QWidget *widget)
{
	QGraphicsEllipseItem::paint(painter, option, widget);
	painter->setFont(widget->parentWidget()->font());
	painter->drawText(this->rect(), Qt::AlignCenter, text);
}

void NumberGraphicsItem::displayDefault()
{
	displayDefault(false);
}

void NumberGraphicsItem::displayDefault(bool force)
{
	if (state == DEFAULT && !force)
		return;
	QPen pen(this->pen());
	pen.setColor(QColor(150,220,183).lighter(60+80.f*_number/255.f));
	setPen(pen);
	state = DEFAULT;
}

void NumberGraphicsItem::displayReadenState()
{
	if (state == READEN)
		return;
	QPen pen(this->pen());
	pen.setColor(QColor(255,20,100).lighter(100+20.f*_number/500.f));
	setPen(pen);
	state = READEN;
}
void NumberGraphicsItem::displayWrittenState()
{
	if (state == WRITEN)
		return;
	QPen pen(this->pen());
	pen.setColor(QColor(150,150,255).lighter(100+20.f*_number/255.f));
	setPen(pen);
	state = WRITEN;
}

int NumberGraphicsItem::number() const
{
	return _number;
}


void SumThread::run() {
	try
	{
		qsrand(time(nullptr));

		int n = qrand() % 10 + 5;
		int result = function(n);
		_assert(n, result);
		_message = QString("Sum of 1..%1 is %2").arg(n).arg(result);
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void SumThread::_assert(int param, int result) const
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
		int result = function(value, n);
		_assert(value, n, result);
		_message = QString("Power %1 of %2 is %3").arg(value).arg(n).arg(result);
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void PowerThread::_assert(int value, int param, int result) const
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
		int result = function(n);
		_assert(n, result);
		_message = QString("Fibonacci %1 is %2").arg(n).arg(result);
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void FibonacciThread::_assert(int n, int result) const
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
			toSearch = a.__get__(index);
		int result = function(toSearch, a, a.size());

		if (index >= 0)
		{
			if (a.__get__(result) == toSearch)
			{
				_message = QString("%1 is at index %2\nArray was:").arg(toSearch).arg(result);
				success = true;
			}
		}
		else
		{
			if (result < 0)
			{
				_message = QString("%1 is not in the array\nArray was:").arg(toSearch);
				success = true;
			}
		}
		if (!success)
		{
			QString message("Search failed: %1 has been found but it should be %2\nArray was: ");
			message.append(a.toString());
			throw std::runtime_error(message.arg(result)
											.arg(index)
											.toStdString());
		}
		_message.append(a.toString());
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
		function(result, a, 0, a.size());
		_assert(a, result);
		_message = QString("%1 evens found\nArray was:").arg(result.effectiveSize());
		_message.append(a.toString());
		success = true;
	}
	catch(std::exception& e)
	{
		_message = QString(e.what());
	}
}

void AllEvensThread::_assert(const Array &origin, const Array &result) const
{
	for (uint i=0; i<origin.size(); ++i)
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

	QImage* mandelbrotPicture = new QImage(2048, 2048, QImage::Format_RGB32);
	double n = qrand() % 80 + 100;
	QRgb white = qRgb(qrand() % 125 + 50, qrand() % 125 + 75, qrand() % 125 + 50);
	QRgb black = qRgb(qrand() % 20, qrand() % 30, qrand() % 20);
	QVector2D z((qrand() % 200 - 100) * 0.0002, (qrand() % 200 - 100) * 0.0002);

	double factor = 0.5 + (qrand() % 40000 - 15000) * 0.0001;
	double precision = 100000. / factor;
	double width = mandelbrotPicture->width() * precision * 0.25, height = mandelbrotPicture->height() * precision * 0.5;


	for (int i=0; i<mandelbrotPicture->width(); ++i)
		for (int j=0; j<mandelbrotPicture->height(); ++j)
		{
			QVector2D point(factor * (-2.5 + (precision * (double)(i)) / width) - 0.7 * (1-factor),
							factor * ((-.5 + (precision * (double)(j)) / height)) - 0.4 * (1-factor));
			double iter = function(z, n, point);
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

TreeNumberGraphicsItem::TreeNumberGraphicsItem(Node* node, QGraphicsItem* parent,
											   float leftShifting, float rightShifting)
	: Base(node->value, parent), _node(node), left(nullptr), right(nullptr),
	  leftShifting(leftShifting), rightShifting(rightShifting)
{}

void TreeNumberGraphicsItem::setNode(Node* node)
{
	this->_node = node;

	if (node)
		setNumber(node->value);
	else
		setNumber(0);
}

Node* TreeNumberGraphicsItem::node() const
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

	if (_node->left && !left)
		return true;

	if (_node->right && !right)
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
	if (_node->left)
	{
		if (!left)
			left = new TreeNumberGraphicsItem(_node->left, this);
		left->leftShifting = left->_treeHeight();
		left->rightShifting = (rightShifting / _treeHeight()) / 2 ;
		left->updateNode();
	}
	if (_node->right)
	{
		if (!right)
			right = new TreeNumberGraphicsItem(_node->right, this);
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
