#ifndef DATAGRIDVIEW_H
#define DATAGRIDVIEW_H

#include <QTableView>

class DataGridView : public QTableView
{
    Q_OBJECT

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public:
    explicit DataGridView(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // DATAGRIDVIEW_H
