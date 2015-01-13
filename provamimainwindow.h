#ifndef PROVAMIMAINWINDOW_H
#define PROVAMIMAINWINDOW_H

#include <QMainWindow>
#include "model.h"
#include "datagridmodel.h"

namespace Ui {
class ProvamiMainWindow;
}

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    Model& model;
    DataGridModel datagrid_model;
    FilterReportModel filter_report_model;
    FilterLevelModel filter_level_model;
    FilterTrangeModel filter_trange_model;
    FilterVarcodeModel filter_varcode_model;

public:
    explicit ProvamiMainWindow(Model& model, QWidget *parent = 0);
    ~ProvamiMainWindow();
    
private slots:
    void on_refresh_clicked();

private:
    Ui::ProvamiMainWindow *ui;
};

#endif // PROVAMIMAINWINDOW_H
