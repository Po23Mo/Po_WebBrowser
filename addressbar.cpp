#include "addressbar.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QActionGroup>
#include "urledit.h"
#include "securitystatusbutton.h"
#include "useragent.h"
#include "bookmarkmanager.h"
#include "version.h"
#include "browser.h"
#include "bookmarkmanager.h"


AddressBar::AddressBar(BookmarkManager* bm, QWidget *parent)
    : QWidget{parent}
    , bookmark_manager(bm)
{
    QHBoxLayout* main_layout  = new QHBoxLayout();
    setLayout(main_layout);

    menu_button = new QToolButton();
    menu_button->setToolTip("Popup menu");
    menu_button->setIcon(QIcon(":/images/menu.png"));
    menu_button->setIconSize(QSize(32, 32));
    menu_button->setToolTip("Popup menu");

    button_popup_menu = new QMenu(this);
    menu_button->setMenu(button_popup_menu);
    menu_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    download_action = new QAction(button_popup_menu);
    download_action->setText("Download manager");
    download_action->setToolTip("Show download manager window");
    button_popup_menu->addAction(download_action);

    savetohtml_action = new QAction(button_popup_menu);
    savetohtml_action->setText("Save assingle HTML file");
    savetohtml_action->setToolTip("Save as single HTML file");
    button_popup_menu->addAction(savetohtml_action);

    bookmark_action = new QAction(button_popup_menu);
    bookmark_action->setText("Bookmarks");
    bookmark_action->setToolTip("Bookmarks");
    button_popup_menu->addAction(bookmark_action);

    bookmark_menu = new QMenu(button_popup_menu);
    bookmark_action->setMenu(bookmark_menu);
    bookmark_action_group = new QActionGroup(bookmark_menu);

    bookmark_add_action = new QAction(button_popup_menu);
    bookmark_add_action->setText("Add bookmark");
    bookmark_add_action->setToolTip("add bookmark of this webpage");
    bookmark_action_group->addAction(bookmark_add_action);
    bookmark_menu->addAction(bookmark_add_action);

    bookmark_remove_action = new QAction(button_popup_menu);
    bookmark_remove_action->setText("Remove bookmark");
    bookmark_remove_action->setToolTip("Remove bookmark of this webpage");
    bookmark_action_group->addAction(bookmark_remove_action);
    bookmark_menu->addAction(bookmark_remove_action);

    bookmark_manager_action = new QAction(button_popup_menu);
    bookmark_manager_action->setText("Bookmark manager window");
    bookmark_manager_action->setToolTip("Show bookmark manager window");
    bookmark_action_group->addAction(bookmark_manager_action);
    bookmark_menu->addAction(bookmark_manager_action);

    useragent_action = new QAction(button_popup_menu);
    useragent_action->setText("Http user agent");
    useragent_action->setToolTip("Select an useragent for this web browser");

    useragent_menu = new QMenu(button_popup_menu);
    useragent_action->setMenu(useragent_menu);

    ua_action_group = new QActionGroup(useragent_menu);
    ua_action_group->setExclusive(true);
    ua_action_group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    int ua_count = get_useragent_count();
    for (int i = 0; i < ua_count; i++)
    {
        const UserAgentItem ua = get_useragent(i);
        auto ua_action = new QAction(button_popup_menu);
        ua_action->setCheckable(true);
        ua_action->setChecked(0 == i);
        ua_action->setText(ua.name);
        ua_action->setData(ua.id);
        ua_action_group->addAction(ua_action);
        useragent_menu->addAction(ua_action);
    }

    button_popup_menu->addAction(useragent_action);

    main_layout->addWidget(menu_button);
    back_button = new QPushButton();
    back_button->setFlat(true);
    back_button->setIcon(QIcon(":/images/back.png"));
    back_button->setIconSize(QSize(32, 32));
    back_button->setToolTip("Back");
    main_layout->addWidget(back_button);

    forward_button = new QPushButton();
    forward_button->setFlat(true);
    forward_button->setIcon(QIcon(":/images/forward.png"));
    forward_button->setIconSize(QSize(32, 32));
    forward_button->setToolTip("forward");
    main_layout->addWidget(forward_button);

    reload_button = new QPushButton();
    reload_button->setFlat(true);
    reload_button->setIcon(QIcon(":/images/reload.png"));
    reload_button->setIconSize(QSize(32, 32));
    reload_button->setToolTip("reload");
    main_layout->addWidget(reload_button);

    ssl_security_button = new SecurityStatusButton();
    main_layout->addWidget(ssl_security_button);

    url_edit = new UrlEdit();
    main_layout->addWidget(url_edit);

    go_button = new QPushButton();
    go_button->setFlat(true);
    go_button->setIcon(QIcon(":/images/go.png"));
    go_button->setIconSize(QSize(32, 32));
    go_button->setToolTip("Go to navigate this web page");
    main_layout->addWidget(go_button);

    bookmark_button = new QPushButton();
    bookmark_button->setFlat(true);
    bookmark_button->setIcon(QIcon(":/images/bookmark.png"));
    bookmark_button->setIconSize(QSize(32, 32));
    bookmark_button->setToolTip("Bookmark this web page");
    main_layout->addWidget(bookmark_button);
    bookmark_button->setVisible(false);

    unbookmark_button = new QPushButton();
    unbookmark_button->setFlat(true);
    unbookmark_button->setIcon(QIcon(":/images/unbookmark.png"));
    unbookmark_button->setIconSize(QSize(32, 32));
    unbookmark_button->setToolTip("Unbookmark this web page");
    main_layout->addWidget(unbookmark_button);
    unbookmark_button->setVisible(false);

    connectSignalsAndSlots();
}

void AddressBar::go()
{
    QString url = url_edit->text().trimmed();
    if (url.isEmpty())
    {
        return;
    }
    emit sig_navigate(url);
}

void AddressBar::connectSignalsAndSlots()
{
    connect(back_button, &QPushButton::clicked, this, &AddressBar::sig_back);
    connect(forward_button, &QPushButton::clicked, this, &AddressBar::sig_forward);
    connect(reload_button, &QPushButton::clicked, this, &AddressBar::sig_reload);
    connect(url_edit, &QLineEdit::editingFinished, this, &AddressBar::go);
    connect(go_button, &QPushButton::clicked, this, &AddressBar::go);
    connect(bookmark_button, &QPushButton::clicked, this, &AddressBar::sig_bookmark);
    connect(unbookmark_button, &QPushButton::clicked, this, &AddressBar::sig_unbookmark);
    connect(bookmark_add_action, &QAction::triggered, this, &AddressBar::sig_bookmark);
    connect(bookmark_remove_action, &QAction::triggered, this, &AddressBar::sig_unbookmark);
    connect(bookmark_manager_action, &QAction::triggered, this, &AddressBar::sig_show_bookmark_manager);
    connect(download_action, &QAction::triggered, this, &AddressBar::sig_show_download_manager);
    connect(ssl_security_button, &QPushButton::clicked, this, &AddressBar::sig_show_ssl_security);
    connect(savetohtml_action, &QAction::triggered, this, [this]{
        emit sig_save_to_file( SaveToSingleHTMLFile);
    });
    connect(this, & AddressBar::sig_update_completer, url_edit, & UrlEdit::updateCompleter);
}

void  AddressBar::setUrl(const QString& url)
{
    url_edit->setText(url);
}

void  AddressBar::setCanBack(bool enabled)
{
    back_button->setEnabled(enabled);
}

void  AddressBar::setCanForward(bool enabled)
{
    forward_button->setEnabled(enabled);
}

void  AddressBar::setCanBookmark(bool enabled)
{
    bookmark_button->setEnabled(enabled);
    bookmark_action->setEnabled(enabled);
    unbookmark_button->setEnabled(!enabled);
    bookmark_remove_action->setEnabled(!enabled);
}

void  AddressBar::updateSslInformation(const  SslInformation& info)
{
    ssl_security_button->updateSslInformation(info);
}

void  AddressBar::switchBookmarkButtons(bool forceInvisible)
{
    if (forceInvisible)
    {
        bookmark_button->setVisible(false);
        unbookmark_button->setVisible(false);
        bookmark_add_action->setEnabled(false);
        bookmark_remove_action->setEnabled(false);
    }
    else
    {
        QString url = url_edit->text();
        const auto& bookmark = bookmark_manager->get(url);
        bool isBookmarked = bookmark->isBookmarked();
        bookmark_button->setVisible(!isBookmarked);
        unbookmark_button->setVisible(isBookmarked);
        bookmark_add_action->setEnabled(!isBookmarked);
        bookmark_remove_action->setEnabled(isBookmarked);
    }
}

void  AddressBar::appendComplete(QSharedPointer< Bookmark> bookmark)
{
    url_edit->append(bookmark);
}
