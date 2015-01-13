#ifndef FILTERCOMBOBOX_H
#define FILTERCOMBOBOX_H

#include <QComboBox>
#include "model.h"

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
    
};

#endif // FILTERCOMBOBOX_H
