#ifndef PROVAMI_RECORDLINEEDIT_H
#define PROVAMI_RECORDLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <dballe/core/record.h>

namespace provami {

class RecordLineEdit : public QLineEdit
{
    Q_OBJECT

protected:
    dballe::Query* query = 0;
    std::function<QString(const dballe::Query&)> query_to_string = nullptr;
    dballe::dba_keyword key;

    // To reset on ESC
    void keyPressEvent(QKeyEvent* event);

public:
    explicit RecordLineEdit(QWidget *parent = 0);
    void set_record(dballe::Query& query, dballe::dba_keyword key);
    /// Reset using the value from the record
    void reset();

signals:
    void canceled();

public slots:

protected slots:
    void on_editing_finished();
    void on_text_edited(const QString&);
};

}
#endif // RECORDLINEEDIT_H
