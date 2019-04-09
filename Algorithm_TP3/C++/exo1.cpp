#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w = nullptr;
using std::size_t;

int binarySearch(Array& array, int toSearch)
{
    int start = 0;
    int end = array.size() - 1;
    int mid;
    while(start < end)
    {
        mid = (start+end)/2;
        if(toSearch == array[mid])
            return mid;
        else if(toSearch < array[mid])
            end = mid - 1;
        else if(toSearch > array[mid])
            start = mid + 1;
    }
    return -1;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow::instruction_duration = 500;
	w = new BinarySearchWindow(binarySearch);
	w->show();

	return a.exec();
}
