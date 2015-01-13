#include "provamimainwindow.h"
#include "model.h"
#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    Model model;
    QApplication a(argc, argv);
    ProvamiMainWindow w;
    w.show();

    if (argc > 1)
        model.dballe_connect(argv[1]);

    return a.exec();
}
