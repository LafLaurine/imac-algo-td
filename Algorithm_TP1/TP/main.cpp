#include "mainwindow.h"
#include <QApplication>
#include <time.h>

int main(int argc, char *argv[])
{
	qsrand(time(NULL));
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
