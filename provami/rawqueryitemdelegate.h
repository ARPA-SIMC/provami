#ifndef PROVAMI_RAWQUERYITEMDELEGATE_H
#define PROVAMI_RAWQUERYITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace provami {

class RawQueryItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

protected:

public:
    explicit RawQueryItemDelegate(QObject *parent=0);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

signals:

public slots:

};

}
#endif
