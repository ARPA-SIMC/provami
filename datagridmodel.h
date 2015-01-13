#ifndef DATAGRIDMODEL_H
#define DATAGRIDMODEL_H

#include <QAbstractTableModel>

class DataGridModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataGridModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:
    
public slots:
    
};

#endif // DATAGRIDMODEL_H
