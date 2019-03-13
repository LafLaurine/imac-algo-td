#include "mainwindow.h"

#include <QGraphicsItem>
#include <QGraphicsWidget>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), numbers(25)
{
	numbers.fillRandom(0,255);
	numberItems.resize(numbers.size());
	for (uint i=0; i<numbers.size(); ++i)
	{
		numberItems[i] = new NumberGraphicsItem(numbers.__get__(i));
		numberItems[i]->setRect(0,0,30,30);
		scene.addItem(numberItems[i]);
	}
	QRadialGradient gradient(-20, -40, 1920);
	gradient.setSpread(QGradient::RepeatSpread);
	gradient.setColorAt(0,QColor(60,20,70));
	gradient.setColorAt(1,QColor(25,8,60));
	scene.setBackgroundBrush(gradient);

	view.setScene(&scene);
	view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	this->setCentralWidget(&view);
	this->resize(600, 600);

	timer.setInterval(400);
	timer.start();

	connect(&timer, SIGNAL(timeout()), this, SLOT(update_scene()));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
	this->scene.setSceneRect(0, 0, this->view.width()-10, this->view.height()-10);
	int itemWidth = qBound(30,(int)(0.75*view.width()/numberItems.size()), 100);
	int start = (scene.width() - itemWidth * numberItems.size() * 1.2) * 0.5;
	for (uint i=0; i<numberItems.size(); ++i)
	{
		numberItems[i]->setRect(start + i * itemWidth * 1.2,scene.height()*0.3,
								itemWidth,itemWidth);
	}
	QFont font("Arial", itemWidth * 0.25, QFont::DemiBold);
	this->view.setFont(font);
}

MainWindow::~MainWindow() {}

void MainWindow::update_scene()
{
	for (uint i=0; i<numberItems.size(); ++i)
	{
		if (numbers.hasBeenReadenAt(i))
			numberItems[i]->displayReadenState();
		else if (numbers.hasBeenWrittenAt(i))
			numberItems[i]->displayWrittenState();
		else
			numberItems[i]->displayDefault();
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
	pen.setColor(QColor(150,220,183).lighter(60+80*number/255));
	setPen(pen);
}
void NumberGraphicsItem::displayReadenState()
{
	QPen pen(this->pen());
	pen.setColor(QColor(255,255,210).lighter(60+80*number/255));
	setPen(pen);
}
void NumberGraphicsItem::displayWrittenState()
{
	QPen pen(this->pen());
	pen.setColor(QColor(150,255,255).lighter(60+80*number/255));
	setPen(pen);
}

