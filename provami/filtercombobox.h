#ifndef PROVAMI_FILTERCOMBOBOX_H
#define PROVAMI_FILTERCOMBOBOX_H

#include <QComboBox>
#include <provami/model.h>

namespace provami {

class FilterComboBox : public QComboBox
{
    Q_OBJECT

protected:
    FilterModelQObjectBase* model;

public:
    explicit FilterComboBox(QWidget *parent = 0);

    void setModel(FilterModelQObjectBase* model);
    
signals:
    
public slots:
    void on_next_filter_changed();
    
};

}

#endif // FILTERCOMBOBOX_H
