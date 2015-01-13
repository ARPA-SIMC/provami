#ifndef PROVAMI_DATEEDIT_H
#define PROVAMI_DATEEDIT_H

#include <QLineEdit>
#include <QDateTime>

namespace provami {

class DateEdit : public QLineEdit
{
    Q_OBJECT

protected:
    QRegExp fmt1;
    QRegExp fmt2;

    // To reset on ESC
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * Check if the date entered is a valid date, and return it parsed.
     *
     * If it was not a valid date, return a Null QDateTime
     */
    virtual bool validate(const QString& text, QDateTime& parsed);

    /// Fill in MISSING_INT elements in vals with what is appropriate
    virtual void complete_datetime(int* vals);

public:
    explicit DateEdit(QWidget *parent = 0);

    /// Reset using the original value
    virtual void reset();

signals:

public slots:

protected slots:
    void on_editing_finished();
    void on_text_edited(const QString& text);
};

}

#endif // DATEEDIT_H
