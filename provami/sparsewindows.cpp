#include "provami/sparsewindows.h"
#include <QVBoxLayout>
#include <QSettings>
#include <QDebug>

using namespace std;

namespace provami {

namespace {
static SparseWindows* _instance = nullptr;
}

SparseWindow::SparseWindow(unsigned index, QWidget *parent)
    : QWidget(parent), tabs(this)
{
    tabs.setProperty("provami_window_index", index);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&tabs);
    setLayout(layout);
}

void SparseWindows::delete_empty_extra_windows()
{
    for (unsigned i = 0; i < extra_windows.size(); ++i)
    {
        if (!extra_windows[i]) continue;
        if (extra_windows[i]->tabs.count() == 0)
        {
            delete extra_windows[i];
            extra_windows[i] = nullptr;
        }
    }

    // Remove trailing deleted entries
    while (!extra_windows.empty() && !extra_windows.back())
        extra_windows.resize(extra_windows.size() - 1);
}

QWidget* SparseWindows::detach_tab(unsigned tab)
{
    // Look for it in main_tabs
    QWidget* res = main_tabs.remove_tab(tab);
    if (res) return res;

    // Look for it in all the extra windows
    for (unsigned i = 0; i < extra_windows.size(); ++i)
    {
        if (!extra_windows[i]) continue;
        res = extra_windows[i]->tabs.remove_tab(tab);
        if (res) return res;
    }

    // The given tab was not found: this should never happen, so we throw an exception
    throw std::runtime_error("programming error: tab was not found");
}

SparseWindows::SparseWindows(SparseTabWidget& main_tabs, QObject *parent) :
    QObject(parent), main_tabs(main_tabs)
{
//    connect(this, SIGNAL(editingFinished()), this, SLOT(on_editing_finished()));
//    connect(this, SIGNAL(textEdited(QString)), this, SLOT(on_text_edited(QString)));
}

void SparseWindows::init(SparseTabWidget& main_tabs)
{
    if (_instance) return;

    main_tabs.set_master_tabs();
    _instance = new SparseWindows(main_tabs);

    // Load window layout from settings
    QSettings settings;
    QString conf_key_template("tabs/tab%1win");
    for (unsigned tab = TAB_FIRST; tab <= TAB_LAST; ++tab)
    {
        QString conf_key = conf_key_template.arg(tab);
        if (!settings.contains(conf_key)) continue;
        unsigned win_idx = settings.value(conf_key).toUInt();

        _instance->move_to_window(tab, win_idx);
    }
}

SparseWindows& SparseWindows::instance()
{
    return *_instance;
}

void SparseWindows::iterate_existing_windows(std::function<void(int, SparseWindow*)> iter)
{
    for (unsigned idx = 0; idx < extra_windows.size(); ++idx)
    {
        if (!extra_windows[idx]) continue;
        iter(idx, extra_windows[idx]);
    }
}

unsigned SparseWindows::create_new_window(unsigned first_tab)
{
    SparseWindow* new_win = 0;
    unsigned new_index;

    // Remove first_tab from the window that has it
    QWidget* page = detach_tab(first_tab);

    // Try to insert filling a gap in the current window list
    for (unsigned i = 0; i < extra_windows.size(); ++i)
    {
        if (extra_windows[i]) continue;
        new_index = i;
        new_win = extra_windows[i] = new SparseWindow(new_index);
    }
    // If there are no gaps, append a new window
    if (!new_win)
    {
        new_index = extra_windows.size();
        extra_windows.push_back(new_win = new SparseWindow(new_index));
    }

    new_win->tabs.add_tab(page);
    new_win->show();

    delete_empty_extra_windows();

    // Save tab position in settings
    QSettings settings;
    settings.setValue(QString("tabs/tab%1win").arg(first_tab), new_index);

    return new_index;
}

void SparseWindows::move_to_master(unsigned tab)
{
    // Detach the tab from where it is now
    QWidget* page = detach_tab(tab);

    // And attach it to main_tabs
    main_tabs.add_tab(page);

    // Save tab position in settings
    QSettings settings;
    settings.remove(QString("tabs/tab%1win").arg(tab));

    delete_empty_extra_windows();
}

void SparseWindows::move_to_window(unsigned tab, unsigned win_idx)
{
    // Detach the tab from where it is now
    QWidget* page = detach_tab(tab);

    // Make sure that the window with the given index exists
    if (win_idx >= extra_windows.size())
        extra_windows.resize(win_idx + 1);
    if (!extra_windows[win_idx])
    {
        extra_windows[win_idx] = new SparseWindow(win_idx);
        extra_windows[win_idx]->show();
    }

    extra_windows[win_idx]->tabs.add_tab(page);

    // Save tab position in settings
    QSettings settings;
    settings.setValue(QString("tabs/tab%1win").arg(tab), win_idx);

    delete_empty_extra_windows();
}

void SparseWindows::close_all_windows()
{
    // Move all tabs for all windows to the main window
    for (unsigned i = 0; i < extra_windows.size(); ++i)
    {
        if (!extra_windows[i]) continue;
        while (extra_windows[i]->tabs.count())
        {
            QWidget* page = extra_windows[i]->tabs.widget(0);
            extra_windows[i]->tabs.removeTab(0);
            main_tabs.add_tab(page);
        }
    }

    delete_empty_extra_windows();
}

}
