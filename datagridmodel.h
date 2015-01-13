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
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

signals:
    
public slots:
    void on_model_refreshed();
};

#endif // DATAGRIDMODEL_H
