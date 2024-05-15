#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include "tabdata.h"


class TabBar : public QTabBar
{
    Q_OBJECT
public:
    TabBar();

    QSharedPointer<TabData> currentTabData();
    QSharedPointer<TabData> tabData(int index);
    void setTabData(int index, QSharedPointer<TabData> data);
    void clearTabData(int index);
    void rebuildTabIndex();

signals:
    void sig_close_all();
    void sig_close_left();
    void sig_close_right();
    void sig_close_all_except_this();
    void sig_close_this();
    void sig_reopen_last_one();
    void sig_reopen_last_ten();
    void sig_reopen_last_hundred();
    void sig_copy_url();
    void sig_copy_title();
    void sig_devtools();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void tabInserted(int index) override;
    void tabRemoved(int index) override;
    void connectPopupMenuSignalsAndSlots();

private:
    QAction* tab_close_action;
    QMenu* tab_close_menu;
    QAction* tab_reopen_action;
    QMenu* tab_reopen_menu;
    QAction* tab_copy_action;
    QAction* tab_advanced_action;
    QMenu* tab_advanced_menu;
    QAction* close_all_tab_action;
    QAction* close_all_except_this_tab_action;
    QAction* close_left_tab_action;
    QAction* close_right_tab_action;
    QAction* close_this_tab_action;
    QAction* reopen_one_closed_tab_action;
    QAction* reopen_ten_closed_tab_action;
    QAction* reopen_all_closed_tab_action;
    QMenu* tab_copy_menu;
    QAction* copy_url_tab_action;
    QAction* copy_title_tab_action;
    QAction* devtools_tab_action;
};

#endif // TABBAR_H
