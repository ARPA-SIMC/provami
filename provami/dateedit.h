#ifndef PROVAMI_DATEEDIT_H
#define PROVAMI_DATEEDIT_H

#include <QLineEdit>
#include <QDateTime>

namespace dballe {
struct Datetime;
struct Query;
}

namespace provami {
struct Model;

class DateEdit : public QLineEdit
{
    Q_OBJECT

protected:
    Model* model = 0;
    QRegExp fmt1;
    QRegExp fmt2;

    // To reset on ESC
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * Set the string value from a Datetime.
     *
     * Return the QDateTime equivalent of dt.
     */
    QDateTime set_value(const dballe::Datetime& dt);

    /**
     * Check if the date entered is a valid date, and return it parsed.
     *
     * If it was not a valid date, return a Null QDateTime
     */
    virtual bool validate(const QString& text, QDateTime& parsed);

    /// Fill in MISSING_INT elements in vals with what is appropriate
    virtual void complete_datetime(int* vals) const;

public:
    explicit DateEdit(QWidget *parent = 0);

    void set_model(Model& model);

    /// Reset using the original value
    virtual void reset() = 0;

signals:
    void activate(QDateTime dt);

public slots:

protected slots:
    void on_editing_finished();
    void on_text_edited(const QString& text);
    virtual void on_minmax_changed() = 0;

};

class MinDateEdit : public DateEdit
{
protected:
    void on_minmax_changed() override;

public:
    explicit MinDateEdit(QWidget *parent = 0);
    void reset() override;
};

class MaxDateEdit : public DateEdit
{
protected:
    void complete_datetime(int* vals) const override;
    void on_minmax_changed() override;

public:
    explicit MaxDateEdit(QWidget *parent = 0);

    void reset() override;
};

}

#endif // DATEEDIT_H
