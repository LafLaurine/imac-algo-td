#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w=nullptr;

void insertionSort(Array& toSort){
	Array& secondArray=w->newArray(toSort.size());

	// insertion Sort

	toSort=secondArray;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Array::instruction_duration = 200;
	w = new MainWindow(insertionSort, 15);
	w->show();

	return a.exec();
}
