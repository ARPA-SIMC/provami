#ifndef PROVAMIMAINWINDOW_H
#define PROVAMIMAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QDoubleValidator>
#include "model.h"
#include "datagridmodel.h"
#include "mapscene.h"

namespace Ui {
class ProvamiMainWindow;
}

class QOptionalIntValidator : public QIntValidator
{
public:
    QOptionalIntValidator(QObject* parent=0);
    QOptionalIntValidator(int minimum, int maximum, QObject* parent=0);
    virtual QValidator::State validate(QString & input, int& pos) const;
};

class QOptionalDoubleValidator : public QDoubleValidator
{
public:
    QOptionalDoubleValidator(QObject* parent=0);
    QOptionalDoubleValidator(double bottom, double top, int decimals, QObject* parent=0);
    virtual QValidator::State validate(QString& input, int& pos) const;
};

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    Model& model;
    DataGridModel datagrid_model;
    MapScene map_scene;
    QOptionalDoubleValidator lat_validator;
    QOptionalDoubleValidator lon_validator;
    QOptionalIntValidator id_validator;


public:
    explicit ProvamiMainWindow(Model& model, QWidget *parent = 0);
    ~ProvamiMainWindow();
    
private slots:
    void next_filter_changed();
    void filter_latlon_changed();
    void on_filter_ana_id_editingFinished();

    void on_actionExit_triggered();

    void on_actionRefresh_triggered();

private:
    Ui::ProvamiMainWindow *ui;
};

#endif // PROVAMIMAINWINDOW_H
