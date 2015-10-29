#ifndef PROVAMI_SPARSETABWIDGET_H
#define PROVAMI_SPARSETABWIDGET_H

#include <QTabWidget>
#include <QKeyEvent>

namespace provami {

// Tab indices
static const unsigned TAB_FILTER = 1;
static const unsigned TAB_MAP = 2;
static const unsigned TAB_STATION = 3;
static const unsigned TAB_ATTRS = 4;
static const unsigned TAB_QUERY = 5;

class SparseTabWidget : public QTabWidget
{
    Q_OBJECT

protected:
    bool is_master_tabs = false;

public:
    explicit SparseTabWidget(QWidget *parent = 0);

    /**
     * Add the given tab page to the window
     */
    void add_tab(QWidget* page);

    /**
     * Remove the tab page for the given tab (by id)
     * from this window and return it.
     *
     * @return nullptr if the tab was not found.
     */
    QWidget* remove_tab(unsigned tab);

    /// Let this widget know that it is the main tab container
    void set_master_tabs();

signals:

public slots:
    void on_tabbar_context_menu(QPoint pos);

protected slots:

};

}
#endif
