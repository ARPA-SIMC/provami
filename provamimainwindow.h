#ifndef PROVAMIMAINWINDOW_H
#define PROVAMIMAINWINDOW_H

#include <QMainWindow>
#include "model.h"

namespace Ui {
class ProvamiMainWindow;
}

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit ProvamiMainWindow(Model& model, QWidget *parent = 0);
    ~ProvamiMainWindow();
    
private slots:
    void on_refresh_clicked();
    void on_model_refreshed();

private:
    Ui::ProvamiMainWindow *ui;
};

#endif // PROVAMIMAINWINDOW_H
