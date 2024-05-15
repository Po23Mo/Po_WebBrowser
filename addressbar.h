#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include <QWidget>
#include <QSharedPointer>

class QHBoxLayout;
class QPushButton;
class QToolButton;
class UrlEdit;
class SecurityStatusButton;
struct SslInformation;
class BookmarkManager;
struct Bookmark;

class AddressBar : public QWidget
{
    Q_OBJECT

public:
    explicit AddressBar(BookmarkManager* bm, QWidget *parent = nullptr);
    void setUrl(const QString& url);
    void setCanBack(bool enabled);
    void setCanForward(bool enabled);
    void setCanBookmark(bool enabled);
    void updateSslInformation(const SslInformation& info);
    void switchBookmarkButtons(bool forceInvisible);
    void appendComplete(QSharedPointer<Bookmark> bookmark);

signals:
    void sig_navigate(const QString& url);
    void sig_back();
    void sig_forward();
    void sig_reload();
    void sig_bookmark();
    void sig_unbookmark();
    void sig_useragent_changed(int id);
    void sig_save_to_file(int mode);
    void sig_show_ssl_security();
    void sig_show_bookmark_manager();
    void sig_show_download_manager();
    void sig_update_completer();

protected:
    void go();
    void connectSignalsAndSlots();

private:
    QToolButton* menu_button;
    QMenu* button_popup_menu;
    QMenu* bookmark_menu;
    QPushButton* back_button;
    QPushButton* forward_button;
    QPushButton* reload_button;
    SecurityStatusButton* ssl_security_button;
    UrlEdit* url_edit;
    QPushButton* go_button;
    QPushButton* bookmark_button;
    QPushButton* unbookmark_button;
    QActionGroup* bookmark_action_group;
    QAction* bookmark_action;
    QAction* bookmark_manager_action;
    QAction* bookmark_add_action;
    QAction* bookmark_remove_action;
    QMenu* useragent_menu;
    QAction* download_action;
    QAction* savetohtml_action;
    QAction* useragent_action;
    QActionGroup* ua_action_group;
    BookmarkManager* bookmark_manager;
};

#endif // ADDRESSBAR_H
