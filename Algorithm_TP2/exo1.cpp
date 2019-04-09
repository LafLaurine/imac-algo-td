#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w = nullptr;

int power(int value, int n)
{
    NOTIFY_START("power", value, n);
    // notify the call of this function to the MainWindow
	// recursiv power
    if(n==0)
        return (1);
    return(value*power(value,n-1));
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow::instruction_duration = 200;
    w = new PowerWindow(power);
    w->show();

    a.exec();
}
