#ifndef PROVAMI_RAWQUERYMODEL_H
#define PROVAMI_RAWQUERYMODEL_H

#include <QAbstractTableModel>
#include <provami/model.h>

namespace provami {

namespace rawquery {
struct Item
{
    std::string key;
    std::string val;

    Item() = default;
    Item(const std::string& key, const std::string& val) : key(key), val(val) {}
};
}

class RawQueryModel : public QAbstractTableModel
{
    Q_OBJECT

protected:
    Model& model;
    std::vector<rawquery::Item> values;

    /// Build a query with the current contents
    std::unique_ptr<dballe::Query> build_query() const;

public:
    enum ColumnType {
        CT_INVALID,
        CT_KEY,
        CT_VALUE,
    };

    explicit RawQueryModel(Model& model, QObject *parent = 0);

    Model& dataModel() { return model; }
    const Model& dataModel() const { return model; }

    ColumnType resolveColumnType(int column) const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);
    const rawquery::Item* valueAt(const QModelIndex& index) const;
    QStringList as_shell_args(bool quoted=true) const;

signals:

public slots:
    void next_filter_changed();

};

}
#endif // RAWQUERYMODEL_H
