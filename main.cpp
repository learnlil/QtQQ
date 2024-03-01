#include "CCMainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CCMainwindow w;
    w.show();
    return a.exec();
}
