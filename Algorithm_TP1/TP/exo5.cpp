#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w=nullptr;

void merge(Array& first, Array& second, Array& result);

void recursivSplitAndMerge(Array& origin)
{

	// recursiv split and merge

	// recursivSplitAndMerge(first);
	// recursivSplitAndMerge(second);

	// merge(first, second, origin);
}

void merge(Array& first, Array& second, Array& result)
{
	// Sorted merge
}

void mergeSort(Array& toSort)
{
	recursivSplitAndMerge(toSort);
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Array::instruction_duration = 200;
	w = new MainWindow(mergeSort, 15);
	w->show();

	return a.exec();
}
