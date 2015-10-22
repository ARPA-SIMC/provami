#ifndef PROVAMI_DEBUG_H
#define PROVAMI_DEBUG_H

#include <QDebug>

namespace provami {

/**
 * Extend a GridModel with proxy functions that trace the queries made to it.
 */
template<typename Parent, int Name=0>
class DebugAbstractTableModel : public Parent
{
public:
    template<typename... Args>
    DebugAbstractTableModel(Args&&... args)
        : Parent(std::forward<Args>(args)...)
    {
    }

    int rowCount(const QModelIndex &parent) const override
    {
        int res = Parent::rowCount(parent);
        qDebug() << Name << ": rowCount -> " << res;
        return res;
    }
    int columnCount(const QModelIndex &parent) const override
    {
        int res = Parent::columnCount(parent);
        qDebug() << Name << ": columnCount -> " << res;
        return res;
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        QVariant res = Parent::data(index, role);
        qDebug() << Name << ": data(" << index << ", " << role << ") -> " << res;
        return res;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        QVariant res = Parent::headerData(section, orientation, role);
        qDebug() << Name << ": headerData(" << section << ", " << orientation << ", " << role << ") -> " << res;
        return res;
    }
    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        Qt::ItemFlags res = Parent::flags(index);
        qDebug() << Name << ": flags(" << index << ") -> " << res;
        return res;
    }
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole) override
    {
        bool res = Parent::setData(index, value, role);
        qDebug() << Name << ": setData(" << index << ", " << value << ", " << role << ") -> " << res;
        return res;
    }
};

}

#endif
