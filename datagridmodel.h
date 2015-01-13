#ifndef DATAGRIDMODEL_H
#define DATAGRIDMODEL_H

#include <QAbstractTableModel>
#include <model.h>

class DataGridModel : public QAbstractTableModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit DataGridModel(Model &model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:
    
public slots:
    void on_model_refreshed();
};

class FilterReportModel : public QAbstractListModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit FilterReportModel(Model &model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

};

class FilterLevelModel : public QAbstractListModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit FilterLevelModel(Model &model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

};

class FilterTrangeModel : public QAbstractListModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit FilterTrangeModel(Model &model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

};

class FilterVarcodeModel : public QAbstractListModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit FilterVarcodeModel(Model &model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

};

#endif // DATAGRIDMODEL_H
