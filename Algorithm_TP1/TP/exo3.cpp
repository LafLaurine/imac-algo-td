#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w=nullptr;

void bubbleSort(Array& toSort){
	// Bubble Sort
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	Array::instruction_duration = 200;
	w = new MainWindow(bubbleSort, 15);
    w->show();

    return a.exec();
}
