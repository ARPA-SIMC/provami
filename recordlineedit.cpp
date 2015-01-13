#include "recordlineedit.h"

using namespace dballe;
using namespace std;

RecordLineEdit::RecordLineEdit(QWidget *parent) :
    QLineEdit(parent), rec(0), key(DBA_KEY_ERROR)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_text_edited(QString)));
}

void RecordLineEdit::set_record(Record &rec, dba_keyword key)
{
    this->rec = &rec;
    this->key = key;
}

void RecordLineEdit::reset()
{
    if (!rec) return;

    if (rec->contains(key))
    {
        string val = rec->get(key).format();
        setText(QString::fromStdString(val));
    }
    else
    {
        setText("");
    }
    setStyleSheet("");
}

void RecordLineEdit::on_editing_finished()
{
    setStyleSheet("");
}

void RecordLineEdit::on_text_edited(const QString &text)
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
