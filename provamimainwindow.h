#ifndef PROVAMIMAINWINDOW_H
#define PROVAMIMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ProvamiMainWindow;
}

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ProvamiMainWindow(QWidget *parent = 0);
    ~ProvamiMainWindow();
    
private:
    Ui::ProvamiMainWindow *ui;
};

#endif // PROVAMIMAINWINDOW_H
