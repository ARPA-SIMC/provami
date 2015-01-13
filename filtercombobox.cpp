#include "filtercombobox.h"
#include <QDebug>

FilterComboBox::FilterComboBox(QWidget *parent) :
    QComboBox(parent), model(0)
{
}

void FilterComboBox::setModel(FilterModelQObjectBase *model)
{
    if (this->model)
    {
        // Disconnect from previous model
        Model& main_model = this->model->get_model();
        disconnect(&main_model, 0, this, 0);
        disconnect(this, 0, this->model, 0);
    }

    this->model = model;
    connect(this, SIGNAL(activated(int)), this->model, SLOT(set_next_filter(int)));
    Model& main_model = this->model->get_model();
    connect(&main_model, SIGNAL(next_filter_changed()), this, SLOT(on_next_filter_changed()));

    QComboBox::setModel(model);
}

void FilterComboBox::on_next_filter_changed()
{
    int idx = model->get_current_index_from_model();
    setCurrentIndex(idx);
}
