#ifndef POWIDGET_H
#define POWIDGET_H

#include <QMainWindow>
#include <QSharedPointer>
#include "Pomainwindow.h"
#include "downloadrequest.h"

class QPushButton;
class QLineEdit;
class QTabBar;
class QLabel;
class QProgressBar;
class QWebEngineProfile;
class WebEngineView;
class WebEnginePage;
class Profile;
class QWebEngineDownloadRequest;
class DownloadWidget;
class QDockWidget;
class QStackedLayout;
class BookmarkWidget;
class BookmarkManager;
class QVBoxLayout;
class TabBar;
class AddressBar;
class ADWidget;
class HomepageAD;
class TabData;

class PoWidget : public PoMainWindow
{
    Q_OBJECT
public:
    explicit PoWidget(QSharedPointer<Profile> profile, QWidget *parent = nullptr);

protected slots:
    void slot_downloadRequest(QWebEngineDownloadRequest *download);
    void slot_save_to_file(int mode);
    void toggleFullscreen(bool fullscreen);
    void switchToFullscreen();
    void switchToNormal();

signals:
    void sig_loadURL(QString url);

private:
    void gotoHomePage();
    void createWebPage(const QString& url);
    void initializeWebPage(WebEnginePage* page);
    void uninitializeWebPage(WebEnginePage* page);
    void uninitializeTab(int index);
    void workOnLastTabClosed();
    void gotoPageUserInput(const QString& sURL);
    void switchBookmarkButtons(bool forceInvisible = false);
    void connectSignalsAndSlots();
    void connectTabSignalsAndSlots();
    void connectTabPopupMenuSignalsAndSlots();
    void connectAddressBarSignalsAndSlots();
    void connectBookmarkSignalsAndSlots();
    void showDownloadManager();
    void showBookmarkManager();

private:
    QSharedPointer<Profile> profile;
    QSharedPointer<DownloadWidget> download_widget;
    QSharedPointer<BookmarkWidget> bookmark_widget;
    QSharedPointer<BookmarkManager> bookmark_manager;

    AddressBar* url_address_bar;
    QStatusBar* status_bar;
    QWidget* central_widget;
    QVBoxLayout* main_layout;
    QLabel* status_label;
    QProgressBar* progress_pb;
    TabBar* tab;
    QPushButton* newpage_button;
    QStackedLayout* stack_layout;
    QList<QString> recent_closed_pages;
    ADWidget* ad_status_widget;
    HomepageAD* ad_homepage;
    QWidget* fullscreen_widget;
    int fullscreen_layout_index;
};

#endif // POWIDGET_H
