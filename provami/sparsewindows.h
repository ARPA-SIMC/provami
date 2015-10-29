#ifndef PROVAMI_SPARSETWINDOWS_H
#define PROVAMI_SPARSETWINDOWS_H

#include <vector>
#include <functional>
#include <provami/sparsetabwidget.h>
#include <QObject>
#include <QWidget>
#include <QTabWidget>

namespace provami {
struct SparseTabWidget;

class SparseWindow : public QWidget
{
    Q_OBJECT

protected:

public:
    SparseTabWidget tabs;

    explicit SparseWindow(unsigned index, QWidget* parent=nullptr);
};

/**
 * @brief Track which tabs are assigned to which windows.
 *
 * Users with large monitors request the possibility of having
 * some of the filter tabs (like the map) in separate windows,
 * so that they can all be seen at the same time.
 *
 * This class tracks the various possible filter tabs, the
 * windows that are currently in use, and which window has each tab.
 */
class SparseWindows : public QObject
{
    Q_OBJECT

protected:
    SparseTabWidget& main_tabs;

    /**
     * All extra windows, by index.
     *
     * The main window is not included in this list.
     *
     * An entry in extra_windows can be nullptr if it has been destroyed.
     * This preserves the indices of existing windows when one is closed.
     */
    std::vector<SparseWindow*> extra_windows;

    /**
     * Delete all the extra windows that have no tabs
     */
    void delete_empty_extra_windows();

    QWidget* detach_tab(unsigned tab);

public:
    explicit SparseWindows(SparseTabWidget& main_tabs, QObject *parent=nullptr);

    static void init(SparseTabWidget& main_tabs);
    static SparseWindows& instance();

    /**
     * Iterate over the existing extra windows, calling iter(win_idx, window)
     * on each of them.
     */
    void iterate_existing_windows(std::function<void(int, SparseWindow*)> iter);

    /**
     * Create a new window and return its index.
     *
     * The window will contain the given tab, removing it from the window
     * currently containing it.
     */
    unsigned create_new_window(unsigned first_tab);

    /**
     * Move the given tab back to the master tab
     */
    void move_to_master(unsigned tab);

    /**
     * Move the given tab to the given window
     */
    void move_to_window(unsigned tab, unsigned win_idx);

signals:

public slots:

protected slots:

};

}
#endif
