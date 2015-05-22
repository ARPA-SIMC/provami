#include "provami/recordlineedit.h"
#include <dballe/core/query.h>

using namespace dballe;
using namespace wreport;
using namespace std;

namespace provami {

RecordLineEdit::RecordLineEdit(QWidget *parent) :
    QLineEdit(parent), key(DBA_KEY_ERROR)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_text_edited(QString)));
}

void RecordLineEdit::set_record(Query& query, dba_keyword key)
{
    this->query = &query;
    switch (key)
    {
        case DBA_KEY_ANA_ID:
            query_to_string = [](const Query& q) {
                if (q.ana_id == MISSING_INT) return QString();
                return QString::number(q.ana_id);
            };
            break;
        case DBA_KEY_LATMIN:
            query_to_string = [](const Query& q) {
                if (q.coords_min.lat == MISSING_INT) return QString();
                return QString::number(q.coords_min.dlat(), 'f', 5);
            };
            break;
        case DBA_KEY_LATMAX:
            query_to_string = [](const Query& q) {
                if (q.coords_max.lat == MISSING_INT) return QString();
                return QString::number(q.coords_max.dlat(), 'f', 5);
            };
            break;
        case DBA_KEY_LONMIN:
            query_to_string = [](const Query& q) {
                if (q.coords_min.lon == MISSING_INT) return QString();
                return QString::number(q.coords_min.dlon(), 'f', 5);
            };
            break;
        case DBA_KEY_LONMAX:
            query_to_string = [](const Query& q) {
                if (q.coords_max.lon == MISSING_INT) return QString();
                return QString::number(q.coords_max.dlon(), 'f', 5);
            };
            break;
        default:
            throw error_unimplemented("RecordLineEdit not implemented for this key");
    }

    this->key = key;
}

void RecordLineEdit::reset()
{
    if (!query) return;

    setText(query_to_string(*query));
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
