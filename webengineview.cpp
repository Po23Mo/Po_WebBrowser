#include "webengineview.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QWebEngineContextMenuRequest>
#include <QContextMenuEvent>
#include <QActionGroup>
#include <QGuiApplication>
#include <QClipboard>
#include <QScreen>
#include "version.h"
#include "bookmarkmanager.h"
#include "webenginepage.h"
#include "browser.h"

WebEngineView::WebEngineView(QWebEngineProfile *profile, QWidget *parent)
    : QWebEngineView(profile, parent)
{
    view_page = new WebEnginePage(profile);
    setPage(view_page);

    search_bm = new BookmarkManager(this);
    search_bm->load(":/data/search.json");

    setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);

    QActionGroup* search_text_action_group = new QActionGroup(this);

    for (auto & category : search_bm->root()->list)
    {
        if (category->list.length() == 0)
        {
            continue;
        }

        QMenu* search_text_menu = new QMenu(this);
        search_text_menu->setTitle(category->title);
        search_text_menus << search_text_menu;

        {
            auto action = new QAction(search_text_menu);
            action->setText("Search with all engines");
            action->setProperty("url", "");
            action->setProperty("category", QVariant::fromValue(category));
            search_text_menu->addAction(action);
            search_text_action_group->addAction(action);
        }

        for (auto & bookmark : category->list)
        {
            if (bookmark->url.isEmpty())
            {
                continue;
            }
            auto action = new QAction(search_text_menu);
            action->setText(bookmark->title);
            action->setProperty("url", bookmark->url);
            search_text_menu->addAction(action);
            search_text_action_group->addAction(action);
        }
    }

    connect(search_text_action_group, &QActionGroup::triggered, this, [this](QAction* action){
        if (selected_text.isEmpty())
        {
            return;
        }
        QString url = action->property("url").toString().arg(selected_text);

        if (!url.isEmpty())
        {
            selected_text.clear();
            emit sig_navigate(url);
            return;
        }

        auto  category = action->property("category").value<QSharedPointer<Bookmark>>();
        for (auto & bookmark : category->list)
        {
            if (bookmark->url.isEmpty())
            {
                continue;
            }
            emit sig_navigate(bookmark->url.arg(selected_text));
        }
        selected_text.clear();

    });


    advanced_text_menu = new QMenu(this);
    advanced_text_menu->setTitle("Advanced text operations");
    copy_text_action = new QAction(advanced_text_menu);
    copy_text_action->setText("Copy selected text   <Ctrl + Shift + C>");
    advanced_text_menu->addAction(copy_text_action);
    search_text_menus << advanced_text_menu;

    connect(copy_text_action, &QAction::triggered, this, [this]{
        if (selected_text.isEmpty())
        {
            return;
        }
        QClipboard* clip = QGuiApplication::clipboard();
        if (!clip)
        {
            return;
        }
        clip->setText(selected_text);
    });

    url_menu = new QMenu(this);
    url_menu->setTitle("Selected URL operations");
    navigate_selected_url_action = new QAction(url_menu);
    navigate_selected_url_action->setText("Navigate selected URL   <F4>");
    url_menu->addAction(navigate_selected_url_action);

    connect(navigate_selected_url_action, &QAction::triggered, this, [this]{
        if (selected_text.isEmpty())
        {
            return;
        }
        emit sig_navigate(selected_text);
    });


    page_menu = new QMenu(this);
    page_menu->setTitle("Page operation");
    reload_with_cache_action = new QAction(this);
    reload_with_cache_action->setText("Reload with cache   <F5>");
    page_menu->addAction(reload_with_cache_action);
    connect(reload_with_cache_action, &QAction::triggered, this, [this]{
        if (nullptr == page())
        {
            return;
        }
        page()->triggerAction(QWebEnginePage::Reload, true);
    });

    reload_without_cache_action = new QAction(this);
    reload_without_cache_action->setText("Reload without cache   <Shift + F5>");
    page_menu->addAction(reload_without_cache_action);
    connect(reload_without_cache_action, &QAction::triggered, this, [this]{
        if (nullptr == page())
        {
            return;
        }
        page()->triggerAction(QWebEnginePage::ReloadAndBypassCache, true);
    });

    fullsceen_enter_action = new QAction(this);
    fullsceen_enter_action->setText("Enter Fullscreen mode   <F9>");
    page_menu->addAction(fullsceen_enter_action);
    connect(fullsceen_enter_action, &QAction::triggered, this, [this]{
        emit sig_toggleFullscreen(true);
    });

    fullsceen_exit_action = new QAction(this);
    fullsceen_exit_action->setText("Exit Fullscreen mode  <F10>");
    page_menu->addAction(fullsceen_exit_action);
    connect(fullsceen_exit_action, &QAction::triggered, this, [this]{
        emit sig_toggleFullscreen(false);
    });


    file_menus << page_menu;

    QActionGroup* save_to_file_action_group = new QActionGroup(this);

    QMenu* save_to_file_menu = new QMenu(this);
    save_to_file_menu->setTitle("Save to file");
    save_to_html_action = new QAction(save_to_file_menu);
    save_to_html_action->setText("Save to single HTML file   <Ctrl + S>");
    save_to_html_action->setProperty("mode", SaveToSingleHTMLFile);
    save_to_file_menu->addAction(save_to_html_action);
    save_to_file_action_group->addAction(save_to_html_action);

    file_menus << save_to_file_menu;

    connect(save_to_file_action_group, &QActionGroup::triggered, this, [this](QAction* action){
        emit sig_save_to_file(action->property("mode").toInt());
    });

}

void WebEngineView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu = nullptr;

    QWebEngineContextMenuRequest* request = lastContextMenuRequest();
    if (!request)
    {
        QWebEngineView::contextMenuEvent(event);
        return;
    }

    menu = createStandardContextMenu();

    if (nullptr == menu)
    {
        menu = new QMenu(this);
    }

    if (property("fullscreen").toBool())
    {
        connect(menu, &QMenu::triggered, this, [menu]{
            menu->close();
        });

        menu->setWindowFlag(Qt::FramelessWindowHint);
        menu->setWindowFlag(Qt::CoverWindow);
    }

    selected_text = selectedText().trimmed();
    menu->addSeparator();

    if (selected_text.isEmpty())
    {
        for (auto file_menu : file_menus)
        {
            menu->addMenu(file_menu);
        }
    }
    else
    {
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(selected_text);
        }

        if (selected_text.startsWith("https://", Qt::CaseInsensitive)
            || selected_text.startsWith("http://", Qt::CaseInsensitive)
            || selected_text.startsWith("ftp://", Qt::CaseInsensitive)
            || selected_text.startsWith(Version::scheme() + "://", Qt::CaseInsensitive)
            )
        {
            menu->addMenu(url_menu);
        }

        for (auto search_text_menu : search_text_menus)
        {
            menu->addMenu(search_text_menu);
        }
    }

    menu->exec(event->globalPos());
}

WebEnginePage* WebEngineView::getPage() const
{
    return view_page;
}

void WebEngineView::keyPressEvent(QKeyEvent* e)
{
    QWebEngineView::keyPressEvent(e);
    if (e->key() == Qt::Key_F5 && e->modifiers().testAnyFlag(Qt::ShiftModifier))
    {
        reload_without_cache_action->trigger();
    }
    else if (e->key() == Qt::Key_F5 && e->modifiers() == Qt::NoModifier)
    {
        reload_with_cache_action->trigger();
    }
    else if (e->key() == Qt::Key_S && e->modifiers() == Qt::ControlModifier)
    {
        save_to_html_action->trigger();
    }
    else if (e->key() == Qt::Key_F4)
    {
        navigate_selected_url_action->trigger();
    }
    else if (e->key() == Qt::Key_C && e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        copy_text_action->trigger();
    }
}

void WebEngineView::focusOutEvent(QFocusEvent* e)
{
    QWebEngineView::focusOutEvent(e);
}
