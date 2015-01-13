#include "filtercombobox.h"

FilterComboBox::FilterComboBox(QWidget *parent) :
    QComboBox(parent), model(0)
{
}

void FilterComboBox::setModel(FilterModelQObjectBase *model)
{
    if (this->model)
    {
        // Disconnect from previous model
        disconnect(this->model, 0, this, 0);
        disconnect(this, 0, this->model, 0);
    }

    this->model = model;
    connect(this, SIGNAL(activated(int)), this->model, SLOT(set_next_filter(int)));

    QComboBox::setModel(model);
}
