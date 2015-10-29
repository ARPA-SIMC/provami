#include "provami/sparsetabwidget.h"
#include "provami/sparsewindows.h"
#include <dballe/core/query.h>
#include <QTabBar>
#include <QMenu>
#include <QDebug>

using namespace dballe;
using namespace wreport;
using namespace std;

namespace provami {

// Labels to use for each tab type
static const char* tab_labels[] = {
    "",
    "&1 Filter",
    "&2 Map",
    "&3 Station",
    "&4 Attrs",
    "&5 Query",
};


SparseTabWidget::SparseTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_tabbar_context_menu(QPoint)));

//    connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
//    connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_text_edited(QString)));
}

void SparseTabWidget::add_tab(QWidget *page)
{
    // Get the tab type
    unsigned tab = page->property("provami_tab_type").toUInt();

    for (unsigned i = 0; i < count(); ++i)
    {
        if (widget(i)->property("provami_tab_type").toUInt() > tab)
        {
            insertTab(i, page, tab_labels[tab]);
            return;
        }
    }

    // If we did not find an insert position, just append
    addTab(page, tab_labels[tab]);
}

QWidget *SparseTabWidget::remove_tab(unsigned tab)
{
    for (int i = 0; ; ++i)
    {
        QWidget* page = widget(i);
        if (!page) return nullptr;
        if (page->property("provami_tab_type").toUInt() == tab)
        {
            removeTab(i);
            return page;
        }
    }
}

void SparseTabWidget::set_master_tabs()
{
    is_master_tabs = true;
}

void SparseTabWidget::on_tabbar_context_menu(QPoint pos)
{
    // Skip if the click was not on a tab
    int idx = tabBar()->tabAt(pos);
    if (idx == -1)
        return;

    // Figure out which actual tab is at this index
    unsigned tab = widget(idx)->property("provami_tab_type").toUInt();
    qDebug() << "IDX" << idx << "For tab" << tab;

    QMenu myMenu;

    SparseWindows& sw = SparseWindows::instance();

    sw.iterate_existing_windows([&](int idx, SparseWindow* win) {
        // Skip if idx is the current window
        if (!is_master_tabs &&
              win->tabs.property("provami_window_index").toUInt() ==
                property("provami_window_index").toUInt())
            return;

        QAction* a = myMenu.addAction(QString("Move to window %1").arg(idx));
        a->setProperty("provami_action", "move_tab");
        a->setProperty("provami_move_tab_target", idx);
    });

    // Do not move to new window if this tab widget only has one tab,
    // unless this is the master window, so that it is possible to create
    // a window with only results
    if (is_master_tabs || count() > 1)
    {
        QAction* a = myMenu.addAction("Move to a new window");
        a->setProperty("provami_action", "move_tab_to_new");
    }

    if (!is_master_tabs)
    {
        QAction* a = myMenu.addAction("Move back to main window");
        a->setProperty("provami_action", "move_tab_to_master");
    }

    QPoint global_pos = tabBar()->mapToGlobal(pos);

    QAction* selectedItem = myMenu.exec(global_pos);
    if (!selectedItem) return;

    QString action = selectedItem->property("provami_action").toString();
    if (action == "move_tab")
    {
        unsigned dest = selectedItem->property("provami_move_tab_target").toUInt();
        sw.move_to_window(tab, dest);
    } else if (action == "move_tab_to_new") {
        sw.create_new_window(tab);
    } else if (action == "move_tab_to_master") {
        sw.move_to_master(tab);
    }
}

}
