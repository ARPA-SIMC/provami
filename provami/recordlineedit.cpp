#include "provami/recordlineedit.h"
#include <dballe/core/query.h>

using namespace dballe;
using namespace wreport;
using namespace std;

namespace provami {

RecordLineEdit::RecordLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_text_edited(QString)));
}

void RecordLineEdit::set_record(Model& model, const std::string& key)
{
    this->model = &model;
    this->key = key;
    if (key == "ana_id")
        query_to_string = [](const Model& m) {
            const core::Query& q = core::Query::downcast(*m.next_filter);
            if (q.ana_id == MISSING_INT) return QString();
            return QString::number(q.ana_id);
        };
    else if (key == "latmin")
        query_to_string = [](const Model& m) {
            LatRange lr = m.next_filter->get_latrange();
            if (lr.imin == LatRange::IMIN) return QString();
            return QString::number(lr.dmin(), 'f', 5);
        };
    else if (key == "latmax")
        query_to_string = [](const Model& m) {
            LatRange lr = m.next_filter->get_latrange();
            if (lr.imax == LatRange::IMAX) return QString();
            return QString::number(lr.dmax(), 'f', 5);
        };
    else if (key == "lonmin")
        query_to_string = [](const Model& m) {
            LonRange lr = m.next_filter->get_lonrange();
            if (lr.imin == MISSING_INT) return QString();
            return QString::number(lr.dmin(), 'f', 5);
        };
    else if (key == "lonmax")
        query_to_string = [](const Model& m) {
            LonRange lr = m.next_filter->get_lonrange();
            if (lr.imax == MISSING_INT) return QString();
            return QString::number(lr.dmax(), 'f', 5);
        };
    else
        throw error_unimplemented("RecordLineEdit not implemented for this key");
}

void RecordLineEdit::reset()
{
    if (!model) return;

    setText(query_to_string(*model));
    setStyleSheet("");
}

void RecordLineEdit::on_editing_finished()
{
    setStyleSheet("");
}

void RecordLineEdit::on_text_edited(const QString &)
{
    setStyleSheet("QLineEdit{background: #fcc;}");
}


void RecordLineEdit::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        reset();
        break;
    default:
        QLineEdit::keyPressEvent(event);
    }
}

}
