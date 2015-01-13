#ifndef PROVAMI_ATTRGRIDMODEL_H
#define PROVAMI_ATTRGRIDMODEL_H

#include <QAbstractTableModel>
#include <dballe/core/defs.h>
#include <provami/model.h>

namespace provami {

class AttrGridModel : public QAbstractTableModel
{
    Q_OBJECT

protected:
    Model& model;
    std::vector<wreport::Var> values;
    wreport::Varcode owner_varcode = 0;
    int owner_id = dballe::MISSING_INT;


public:
    enum ColumnType {
        CT_INVALID,
        CT_VARCODE,
        CT_VALUE,
    };

    explicit AttrGridModel(Model& model, QObject *parent = 0);

    Model& dataModel() { return model; }
    const Model& dataModel() const { return model; }

    ColumnType resolveColumnType(int column) const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);
    const wreport::Var* valueAt(const QModelIndex& index) const;

signals:

public slots:
    void on_highlight_changed();
};

}
#endif // ATTRGRIDMODEL_H
