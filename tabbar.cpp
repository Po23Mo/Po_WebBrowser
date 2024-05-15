#include "tabbar.h"
#include <QMouseEvent>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include "tabdata.h"
#include "webenginepage.h"

TabBar::TabBar()
{
    setVisible(false);
    connect(this, &QTabBar::tabMoved, this, [this](int from, int to) {
        rebuildTabIndex();
    });

    tab_copy_action = new QAction();
    tab_copy_menu = new QMenu(this);
    tab_copy_action->setText("Copy title or url of this page");
    tab_copy_action->setMenu(tab_copy_menu);

    copy_url_tab_action = new QAction(tab_copy_menu);
    copy_url_tab_action->setText("Copy URL to clipboard");
    tab_copy_menu->addAction(copy_url_tab_action);

    copy_title_tab_action = new QAction(tab_copy_menu);
    copy_title_tab_action->setText("Copy title to clipboard");
    tab_copy_menu->addAction(copy_title_tab_action);

    tab_close_action = new QAction();
    tab_close_menu = new QMenu(this);
    tab_close_action->setText("Close one or more pages");
    tab_close_action->setMenu(tab_close_menu);

    tab_reopen_action = new QAction();
    tab_reopen_menu = new QMenu(this);
    tab_reopen_action->setText("Reopen one or more pages");
    tab_reopen_action->setMenu(tab_reopen_menu);

    tab_advanced_action = new QAction();
    tab_advanced_menu = new QMenu(this);
    tab_advanced_action->setText("Advanced operations");
    tab_advanced_action->setMenu(tab_advanced_menu);

    close_all_tab_action = new QAction(tab_close_menu);
    close_all_tab_action->setText("Close all the pages");
    tab_close_menu->addAction(close_all_tab_action);

    close_all_except_this_tab_action = new QAction(tab_close_menu);
    close_all_except_this_tab_action->setText("Close all the pages except this");
    tab_close_menu->addAction(close_all_except_this_tab_action);

    close_left_tab_action = new QAction(tab_close_menu);
    close_left_tab_action->setText("Close the pages on the left");
    tab_close_menu->addAction(close_left_tab_action);

    close_right_tab_action = new QAction(tab_close_menu);
    close_right_tab_action->setText("Close the pages on the right");
    tab_close_menu->addAction(close_right_tab_action);

    close_this_tab_action = new QAction(tab_close_menu);
    close_this_tab_action->setText("Close this page");
    tab_close_menu->addAction(close_this_tab_action);

    reopen_one_closed_tab_action = new QAction(tab_reopen_menu);
    reopen_one_closed_tab_action->setText("Reopen one web page recently closed");
    tab_reopen_menu->addAction(reopen_one_closed_tab_action);

    reopen_ten_closed_tab_action = new QAction(tab_reopen_menu);
    reopen_ten_closed_tab_action->setText("Reopen 10 web pages recently closed");
    tab_reopen_menu->addAction(reopen_ten_closed_tab_action);

    reopen_all_closed_tab_action = new QAction(tab_reopen_menu);
    reopen_all_closed_tab_action->setText("Reopen 100 web pages recently closed");
    tab_reopen_menu->addAction(reopen_all_closed_tab_action);

    devtools_tab_action = new QAction(tab_advanced_menu);
    devtools_tab_action->setText("Developer tools of this page");
    tab_advanced_menu->addAction(devtools_tab_action);

    QList<QAction*> all_actions = actions();
    all_actions << tab_copy_action << tab_close_action << tab_reopen_action << tab_advanced_action;
    insertActions(nullptr, all_actions);
    setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    connectPopupMenuSignalsAndSlots();
}

void TabBar::mouseReleaseEvent(QMouseEvent *e)
{
    int index = tabAt(e->pos());
    if (index >= 0)
    {
        setCurrentIndex(index);
    }
    QTabBar::mouseReleaseEvent(e);
}

void TabBar::tabInserted(int index)
{
    setVisible(true);
}

void TabBar::tabRemoved(int index)
{
    if (0 == count())
    {
        setVisible(false);
    }
    rebuildTabIndex();
}

void TabBar::rebuildTabIndex()
{
    int cnt = count();
    for (int i = 0; i < cnt; i++)
    {
        auto page = tabData(i)->page();
        if (page)
        {
            page->setTabIndex(i);
        }
    }
}

void TabBar::clearTabData(int index)
{
    QTabBar::setTabData(index, QVariant());
}

void TabBar::setTabData(int index, QSharedPointer<TabData> data)
{
    QTabBar::setTabData(index, QVariant::fromValue(data));
}

QSharedPointer<TabData> TabBar::currentTabData()
{
    return tabData(currentIndex());
}

QSharedPointer<TabData> TabBar::tabData(int index)
{
    auto data = QTabBar::tabData(index).value<QSharedPointer<TabData>>();;
    return data ? data : TabData::nullData();
}

void TabBar::connectPopupMenuSignalsAndSlots()
{
    connect(close_all_tab_action, &QAction::triggered, this, &TabBar::sig_close_all);
    connect(close_all_except_this_tab_action, &QAction::triggered, this, &TabBar::sig_close_all_except_this);
    connect(close_left_tab_action, &QAction::triggered, this, &TabBar::sig_close_left);
    connect(close_right_tab_action, &QAction::triggered, this, &TabBar::sig_close_right);
    connect(close_this_tab_action, &QAction::triggered, this, &TabBar::sig_close_this);
    connect(reopen_one_closed_tab_action, &QAction::triggered, this, &TabBar::sig_reopen_last_one);
    connect(reopen_ten_closed_tab_action, &QAction::triggered, this, &TabBar::sig_reopen_last_ten);
    connect(reopen_all_closed_tab_action, &QAction::triggered, this, &TabBar::sig_reopen_last_hundred);
    connect(copy_url_tab_action, &QAction::triggered, this, &TabBar::sig_copy_url);
    connect(copy_title_tab_action, &QAction::triggered, this, &TabBar::sig_copy_title);
    connect(devtools_tab_action, &QAction::triggered, this, &TabBar::sig_devtools);
}
