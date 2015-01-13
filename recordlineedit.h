#ifndef RECORDLINEEDIT_H
#define RECORDLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <dballe/core/record.h>

class RecordLineEdit : public QLineEdit
{
    Q_OBJECT

protected:
    dballe::Record* rec;
    dballe::dba_keyword key;

    // To reset on ESC
    void keyPressEvent(QKeyEvent* event);

public:
    explicit RecordLineEdit(QWidget *parent = 0);
    void set_record(dballe::Record& rec, dballe::dba_keyword key);
    /// Reset using the value from the record
    void reset();

signals:
    void canceled();
    
public slots:

protected slots:
    void on_editing_finished();
    void on_text_edited(const QString& text);
};

#endif // RECORDLINEEDIT_H
