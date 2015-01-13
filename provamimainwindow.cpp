#include "provamimainwindow.h"
#include "ui_provamimainwindow.h"

ProvamiMainWindow::ProvamiMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}
