#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    qRegisterMetaType<OPCHANDLE>("OPCHANDLE");
    qRegisterMetaType<DWORD>("DWORD");

    return a.exec();
}
