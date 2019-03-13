#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w=nullptr;

void recursivQuickSort(Array& toSort, int size)
{
	Array& lowerArray = w->newArray(size);
	Array& greaterArray= w->newArray(size);

	// Recursiv Quick sort
}

void quickSort(Array& toSort){
	recursivQuickSort(toSort, toSort.size());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	Array::instruction_duration = 200;
	w = new MainWindow(quickSort, 15);
    w->show();

    return a.exec();
}
