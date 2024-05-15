#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#include <QAction>
#include <QMenu>
#include <QSharedPointer>

class BookmarkManager;
class WebEnginePage;


class WebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    WebEngineView(QWebEngineProfile *profile, QWidget *parent = nullptr);

    WebEnginePage* getPage() const;
    void keyPressEvent(QKeyEvent* e) override;

signals:
    void sig_navigate(const QString& url);
    void sig_save_to_file(int mode);
    void sig_toggleFullscreen(bool Fullscreen);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void focusOutEvent(QFocusEvent* e) override;

private:
    QMenu* page_menu;
    QAction* reload_with_cache_action;
    QAction* reload_without_cache_action;
    QAction* fullsceen_enter_action;
    QAction* fullsceen_exit_action;
    QAction* save_to_html_action;
    QString selected_text;
    BookmarkManager* search_bm;
    QList<QMenu*> search_text_menus;
    QList<QAction*> search_text_actions;
    QMenu* advanced_text_menu;
    QAction* copy_text_action;
    QMenu* url_menu;
    QAction* navigate_selected_url_action;
    QList<QMenu*> file_menus;
    WebEnginePage* view_page;
};

#endif // WEBENGINEVIEW_H
