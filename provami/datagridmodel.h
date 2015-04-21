#ifndef PROVAMI_DATAGRIDMODEL_H
#define PROVAMI_DATAGRIDMODEL_H

#include <QAbstractTableModel>
#include <provami/model.h>

namespace provami {

class DataGridModel : public QAbstractTableModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    enum ColumnType {
        CT_INVALID,
        CT_STATION,
        CT_NETWORK,
        CT_LEVEL,
        CT_TRANGE,
        CT_DATETIME,
        CT_VARCODE,
        CT_VALUE,
    };

    explicit DataGridModel(Model &model, QObject *parent = 0);

    Model& dataModel() { return model; }
    const Model& dataModel() const { return model; }
    ColumnType resolveColumnType(int column) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);
    const Value* valueAt(const QModelIndex& index) const;

signals:
    
public slots:
    void on_model_begin_refresh();
    void on_model_end_refresh();
};

}
#endif // DATAGRIDMODEL_H
