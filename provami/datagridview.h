#ifndef PROVAMI_DATAGRIDVIEW_H
#define PROVAMI_DATAGRIDVIEW_H

#include <QTableView>

namespace provami {

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

}

#endif // DATAGRIDVIEW_H
