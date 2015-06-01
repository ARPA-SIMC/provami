#ifndef PROVAMI_RECORDLINEEDIT_H
#define PROVAMI_RECORDLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <provami/model.h>

namespace provami {

class RecordLineEdit : public QLineEdit
{
    Q_OBJECT

protected:
    Model* model = nullptr;
    std::string key;
    std::function<QString(const Model&)> query_to_string = nullptr;

    // To reset on ESC
    void keyPressEvent(QKeyEvent* event);

public:
    explicit RecordLineEdit(QWidget *parent = 0);
    void set_record(Model& model, const std::string& key);
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
