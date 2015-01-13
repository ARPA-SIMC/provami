#include "provamimainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProvamiMainWindow w;
    w.show();
    
    return a.exec();
}
