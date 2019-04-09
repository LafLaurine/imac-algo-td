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

#include "array.h"


unsigned long MainWindow::instruction_duration = 1000;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), timer(this), background(nullptr), zoom(false), scale(1),
	workerThread(nullptr)
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
	arrays.push_back(new Array(size));
	Array& array = *arrays.last();
	array.fillRandom(0,255);
	this->dirty = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 3));
	return array;
}

Array &MainWindow::newSortedRandomArray(uint size)
{
	arrays.push_back(new Array(size));
	Array& array = *arrays.last();
	array.fillSortedRandom(0,500);
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 2));
	return array;
}

Array &MainWindow::newArray(uint size)
{
	arrays.push_back(new Array(size));
	Array& array = *arrays.last();
	std::this_thread::sleep_for(std::chrono::milliseconds(MainWindow::instruction_duration / 2));
	this->dirty = true;
	return array;
}

void MainWindow::updateLayout()
{

	if (dirty)
	{
		int itemWidth = qMax<int>(50, width() * 0.01f);
		int maxX=0, maxY=0;

		updateStatusItem(itemWidth);
		updateBackground();
		updateNumberItems(itemWidth, maxX, maxY);

		this->scene.setSceneRect(0, 0, qMax(width(), maxX), qMax(height(),maxY+itemWidth * 2));
	}

	this->dirty=false;
}

void MainWindow::updateBackground()
{
	if (background)
	{
		background->setPos(0,0);
		background->setPixmap(background->pixmap().scaledToWidth(scene.width()));
	}
}

int MainWindow::updateNumberItems(int itemWidth, int& maxY, int& maxX)
{
	maxY = scene.height() * 0.2f;
	maxX = 0;
	for (QVector<NumberGraphicsItem*> items : numberItems)
	{
		int startX = 0.05 * width();
		for (NumberGraphicsItem* item : items)
		{
			item->setRect(startX, maxY, itemWidth, itemWidth);
			startX += itemWidth * 1.5f;
			if (startX + itemWidth * 2 > scene.width())
			{
				maxY += itemWidth * 1.5f;
				startX = qMax((50 + scene.width() - itemWidth * 1.5f * items.size()) * 0.5,
											0.05 * scene.width());
			}
		}
		if (startX > maxX)
			maxX = startX;
		maxY += itemWidth * 2;
	}
	return maxY;
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

void MainWindow::updateStatusItem(int itemWidth)
{
	QFont font("Arial", itemWidth * 0.35, QFont::DemiBold);
	statusItem.setFont(font);
	statusItem.setPos(scene.width() * 0.05f, scene.height() * 0.05f);
	statusItem.setTextWidth(width() / 2);
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

	while (numberItems.size() < arrays.size())
	{
		Array& array = *arrays[numberItems.size()];
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
		Array& array = *arrays[i];
		for (uint j=0; j < array.size(); ++j)
		{
			if (j >= numbers.size())
			{
				numbers.push_back(new NumberGraphicsItem(array.__get__(j)));
				// toAdd.push_back(numbers[j]);
				dirty = true;
			}
			NumberGraphicsItem* item = numbers[j];
			if (item->number() != array.__get__(j))
			{
				item->setNumber(array.__get__(j));
				dirty = true;
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
			statusItem.setPlainText(QString("Success, gg bg: %1").arg(workerThread->message()));
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

QColor NumberGraphicsItem::baseColor(150, 220, 183);

void NumberGraphicsItem::displayDefault()
{
	displayDefault(false);
}

void NumberGraphicsItem::displayDefault(bool force)
{
	if (state == DEFAULT && !force)
		return;
	QPen pen(this->pen());
	pen.setColor(baseColor.lighter(60+80.f*_number/255.f));
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
