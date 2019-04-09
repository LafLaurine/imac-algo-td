#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>
#include <stdexcept>

#include <qglobal.h>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QLinkedList>
#include <QVector2D>
#include <QTimer>

#include "thread.h"

class AbstractArray;
class Array;

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

	static QColor baseColor;

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

class MainWindow : public QMainWindow
{

	Q_OBJECT

public:

	explicit MainWindow(QWidget* parent=nullptr);

	Array& mainArray() {return *arrays[0];}
	Array& newRandomArray(uint size);
	Array& newSortedRandomArray(uint size);
	Array& newArray(uint size);

	void resizeEvent(QResizeEvent*);
	void closeEvent(QCloseEvent*);

	void setBackground(QImage* image);

	virtual void updateLayout();
	void updateStatusItem(int itemWidth);
	void updateBackground();
	int updateNumberItems(int itemWidth, int &maxY, int &maxX);

	static unsigned long instruction_duration;

	bool eventFilter(QObject* object, QEvent* event);

	~MainWindow();
public slots:
	virtual void updateScene();
	virtual void handleResult();

private:

	void initialize();

	QGraphicsView view;

	QVector<QVector<NumberGraphicsItem*> > numberItems;
	QVector<QGraphicsTextItem *> accessInfoItem;
	QGraphicsTextItem statusItem;
	QVector<Array*> arrays;

	QTimer timer;

	QGraphicsPixmapItem* background;


	bool zoom;
	float scale;
	QPointF targetPos;

protected:
	QGraphicsScene scene;

	QVector<QGraphicsItem*> toAdd;
	QVector<QPair<QGraphicsTextItem*, QString> > toUpdate;
	QVector<QGraphicsItem*> toRemove;

	Thread* workerThread;
	bool dirty;
};


#endif // MAINWINDOW_H
