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
    
};

#endif // DATAGRIDMODEL_H
