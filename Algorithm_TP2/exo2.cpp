#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>
#define return(return_value) int result=return_value; w->popFunctionCall(result); return result;

MainWindow* w = nullptr;

int fibonacci(int n)
{
    NOTIFY_START("fibonacci", n); // notify the call of this function to the MainWindow

    // recursiv fibonacci
    if(n < 2)
        return n;
    return (fibonacci(n-1) + fibonacci(n-2));

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow::instruction_duration = 20;
    w = new FibonacciWindow(fibonacci);
    w->show();

    a.exec();
}
