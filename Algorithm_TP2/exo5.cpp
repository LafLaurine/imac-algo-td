#include "lib/mainwindow.h"
#include <QApplication>
#include <time.h>

MainWindow* w = nullptr;

int isMandelbrot(QVector2D z, int n, QVector2D point){
	// recursiv Mandelbrot
    if(n==0){
        return false;
    }
    float x,y;
    x = (z.x()) * (z.x()) - (z.y()) * (z.y()) + point.x();
    y = 2*z.x()*z.y() + point.y();
    z.setX(x);
    z.setY(y);

    if(z.length() > 2){
        return n;
    }
    return isMandelbrot(z,n-1,point);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new MandelbrotWindow(isMandelbrot);
    w->show();

    a.exec();
}



