#ifndef PROVAMIMAINWINDOW_H
#define PROVAMIMAINWINDOW_H

#include <QMainWindow>
#include "model.h"
#include "datagridmodel.h"
#include "mapscene.h"

namespace Ui {
class ProvamiMainWindow;
}

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    Model& model;
    DataGridModel datagrid_model;
    MapScene map_scene;

public:
    explicit ProvamiMainWindow(Model& model, QWidget *parent = 0);
    ~ProvamiMainWindow();
    
private slots:
    void on_refresh_clicked();

    void on_filter_report_activated(int index);
    void on_filter_level_activated(int index);
    void on_filter_trange_activated(int index);
    void on_filter_varcode_activated(int index);

private:
    Ui::ProvamiMainWindow *ui;
};

#endif // PROVAMIMAINWINDOW_H
