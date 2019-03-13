#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w = nullptr;

void selectionSort(Array& toSort){
    for(uint i=0; i<toSort.size(); ++i)
    {
		uint minIndex = i;
        int minValue = toSort[minIndex];
        for (uint j=i+1; j<toSort.size(); ++j)
        {
            int value = toSort[j];
            if (value < minValue)
            {
                minIndex=j;
                minValue=value;
            }
        }
		toSort.swap(i, minIndex);
    }
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Array::instruction_duration = 200;
	w = new MainWindow(selectionSort, 15);
    w->show();

	return a.exec();
}
