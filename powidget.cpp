#include <QVBoxLayout>
#include <QStackedLayout>
#include <QWebEngineHistory>
#include <QWebEngineNewWindowRequest>
#include <QLabel>
#include <QPushButton>
#include <QTabBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QPixmap>
#include <QFile>
#include <QStandardPaths>
#include <QThreadPool>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <QWebEngineProfile>
#include <QWebEngineLoadingInfo>
#include <QWebEngineCertificateError>
#include <QPageLayout>
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QActionGroup>
#include <QToolButton>
#include <QGuiApplication>
#include <QClipboard>
#include <QWebEngineDownloadRequest>
#include <QSplitter>
#include <QShortcut>
#include <QWebEngineFullScreenRequest>
#include "version.h"
#include "webengineview.h"
#include "webenginepage.h"
#include "downloadwidget.h"
#include "profile.h"
#include "bookmarkwidget.h"
#include "bookmarkmanager.h"
#include "tabbar.h"
#include "powidget.h"
#include "webpagewidget.h"
#include "addressbar.h"
#include "browser.h"


PoWidget::PoWidget(QSharedPointer<Profile> vprofile, QWidget *parent)
    : PoMainWindow{parent}
    , profile(vprofile)
    , fullscreen_widget(nullptr)
    , fullscreen_layout_index(-1)
{
    bookmark_manager.reset(new BookmarkManager(this));
    bookmark_manager->load();

    status_bar = statusBar();
    status_label = new QLabel("Ready");
    status_bar->addPermanentWidget(status_label);
    progress_pb = new QProgressBar();
    progress_pb->setTextVisible(true);
    progress_pb->setMinimumWidth(300);
    progress_pb->setAlignment(Qt::AlignCenter);
    status_bar->addPermanentWidget(progress_pb);
    status_bar->addPermanentWidget(new QLabel(), 1);

    central_widget = new QWidget();
    main_layout = new QVBoxLayout();
    central_widget->setLayout(main_layout);

    url_address_bar = new AddressBar(bookmark_manager.get());
    main_layout->addWidget(url_address_bar);

    QHBoxLayout* tab_layout = new QHBoxLayout();
    tab = new TabBar();
    tab->setTabsClosable(true);
    tab->setMovable(true);
    tab_layout->addWidget(tab);

    newpage_button = new QPushButton();
    newpage_button->setFlat(true);
    newpage_button->setIcon(QIcon(":/images/addtab.png"));
    newpage_button->setToolTip("add a blank page");
    tab_layout->addWidget(newpage_button);
    tab_layout->addStretch(0);
    main_layout->addLayout(tab_layout);

    stack_layout = new QStackedLayout();
    stack_layout->addWidget(new QWidget());

    download_widget.reset(new DownloadWidget(profile));
    stack_layout->addWidget(download_widget.get());

    bookmark_widget.reset(new BookmarkWidget(bookmark_manager.get()));

    stack_layout->addWidget(bookmark_widget.get());

    stack_layout->setCurrentIndex(0);
    main_layout->addLayout(stack_layout);

    setCentralWidget(central_widget);
    connectSignalsAndSlots();

    gotoHomePage();
}

void PoWidget::connectTabSignalsAndSlots()
{
    connect(tab, &QTabBar::currentChanged, this, [this] (int index){
        auto page = tab->tabData(index)->page();
        if (nullptr == page)
        {
            status_bar->setVisible(false);
            switchBookmarkButtons(false);

            auto download_page = tab->tabData(index)->downloadWidget();
            if (download_page)
            {
                stack_layout->setCurrentWidget(download_page.get());
                setWindowTitle(download_page->windowTitle());
            }
            else
            {
                auto bookmark_page = tab->tabData(index)->bookmarkWidget();
                if (bookmark_page)
                {
                    stack_layout->setCurrentWidget(bookmark_page.get());
                    setWindowTitle(bookmark_page->windowTitle());
                }
            }
            return;
        }

        auto page_widget = page->tabData()->webWidget().get();
        page_widget->setFocus();
        stack_layout->setCurrentWidget(page_widget);
        setWindowTitle(page->title());
        url_address_bar->setUrl(page->url().toDisplayString());
        url_address_bar->setCanBack(page->history()->canGoBack());
        url_address_bar->setCanForward(page->history()->canGoForward());
        status_bar->setVisible(page->isLoading());
        switchBookmarkButtons();

        page->sslInformation().url = page->url();
        url_address_bar->updateSslInformation(page->sslInformation());
    });

    connect(tab, &QTabBar::tabCloseRequested, this, [this](int index) {
        auto page = tab->tabData(index)->page();
        if (nullptr == page)
        {
            auto download_page = tab->tabData(index)->downloadWidget();
            auto bookmark_page = tab->tabData(index)->bookmarkWidget();
            if (download_page || bookmark_page)
            {
                tab->removeTab(index);
            }
            auto Widget = tab->currentTabData()->webWidget();
            if (Widget)
            {
                stack_layout->setCurrentWidget(Widget.get());
            }
            switchBookmarkButtons();
            workOnLastTabClosed();
            return;
        }

        recent_closed_pages << page->url().toString();
        uninitializeTab(index);
        tab->removeTab(index);
        switchBookmarkButtons();
        workOnLastTabClosed();
    });

    connect(tab, &QTabBar::tabMoved, this, [this](int from, int to) {
        switchBookmarkButtons();
    });
}

void PoWidget::connectTabPopupMenuSignalsAndSlots()
{
    connect(tab, &TabBar::sig_close_all, this, [this]{
        int count = tab->count();
        for (int i = count; i > 0; i--)
        {
            auto page = tab->tabData(i - 1)->page();
            if (page)
            {
                recent_closed_pages << page->url().toString();
            }
            uninitializeTab(i - 1);
            tab->removeTab(i - 1);
        }
        workOnLastTabClosed();
    });

    connect(tab, &TabBar::sig_close_all_except_this, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        int count = tab->count();
        for (int i = count; i > 0; i--)
        {
            if (index + 1 == i)
            {
                continue;
            }
            auto page = tab->tabData(i - 1)->page();
            if (page)
            {
                recent_closed_pages << page->url().toString();
            }
            uninitializeTab(i - 1);
            tab->removeTab(i - 1);
        }
    });

    connect(tab, &TabBar::sig_close_left, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        for (int i = index; i > 0; i--)
        {
            auto page = tab->tabData(i - 1)->page();
            if (page)
            {
                recent_closed_pages << page->url().toString();
            }
            uninitializeTab(i - 1);
            tab->removeTab(i - 1);
        }
    });

    connect(tab, &TabBar::sig_close_right, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        int count = tab->count();
        for (int i = count; i > index + 1; i--)
        {
            auto page = tab->tabData(i - 1)->page();
            if (page)
            {
                recent_closed_pages << page->url().toString();
            }
            uninitializeTab(i - 1);
            tab->removeTab(i - 1);
        }
    });

    connect(tab, &TabBar::sig_close_this, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }

        auto page = tab->tabData(index)->page();
        if (page)
        {
            recent_closed_pages << page->url().toString();
        }
        uninitializeTab(index);
        tab->removeTab(index);
        workOnLastTabClosed();
    });


    connect(tab, &TabBar::sig_reopen_last_one, this, [this]{
        if (recent_closed_pages.count() == 0)
        {
            return;
        }

        QString url = recent_closed_pages.last();
        recent_closed_pages.removeOne(url);
        createWebPage(url);
    });


    connect(tab, &TabBar::sig_reopen_last_ten, this, [this]{
        for (int i = 0; i < 10 && recent_closed_pages.count() > 0; i++)
        {
            QString url = recent_closed_pages.last();
            recent_closed_pages.removeOne(url);
            createWebPage(url);
        }
    });

    connect(tab, &TabBar::sig_reopen_last_hundred, this, [this]{
        for (int i = 0; i < 100 && recent_closed_pages.count() > 0; i++)
        {
            QString url = recent_closed_pages.last();
            recent_closed_pages.removeOne(url);
            createWebPage(url);
        }
    });


    connect(tab, &TabBar::sig_copy_url, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        auto page = tab->tabData(index)->page();
        if (nullptr == page)
        {
            return;
        }

        QString url = page->url().toString();
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(url);
        }
    });

    connect(tab, &TabBar::sig_copy_title, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        auto page = tab->tabData(index)->page();
        if (nullptr == page)
        {
            return;
        }

        QString title = page->title();
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(title);
        }
    });

    connect(tab, &TabBar::sig_devtools, this, [this]{
        int index = tab->currentIndex();
        if (index < 0)
        {
            return;
        }
        auto po_Widget = tab->tabData(index)->webWidget();
        if (po_Widget)
        {
            po_Widget->openDevTools();
        }
    });
}

void PoWidget::connectSignalsAndSlots()
{
    connectTabSignalsAndSlots();
    connectTabPopupMenuSignalsAndSlots();
    connectAddressBarSignalsAndSlots();
    connectBookmarkSignalsAndSlots();
}

void PoWidget::connectBookmarkSignalsAndSlots()
{
    connect(bookmark_manager.get(), &BookmarkManager::sig_update,
            url_address_bar, &AddressBar::sig_update_completer);
    connect(url_address_bar, &AddressBar::sig_bookmark, this, [this]{
        auto page = tab->currentTabData()->page();
        if (!page)
        {
            return;
        }
        QSharedPointer<Bookmark> bookmark(new Bookmark(page->title(), page->url().toString()));
        bookmark_manager->add(bookmark);
        switchBookmarkButtons();
    });

    connect(url_address_bar, &AddressBar::sig_unbookmark, this, [this]{
        auto page = tab->currentTabData()->page();
        if (!page)
        {
            return;
        }
        QSharedPointer<Bookmark> bookmark(new Bookmark(page->title(), page->url().toString()));
        bookmark_manager->remove(bookmark);
        switchBookmarkButtons();
    });

    connect(url_address_bar, &AddressBar::sig_show_bookmark_manager, this, &PoWidget::showBookmarkManager);
    connect(bookmark_widget.get(), &BookmarkWidget::sig_reload_url, this, &PoWidget::sig_loadURL);
    connect(download_widget.get(), &DownloadWidget::sig_reload_url, this, &PoWidget::sig_loadURL);
}

void PoWidget::connectAddressBarSignalsAndSlots()
{
    connect(url_address_bar, &AddressBar::sig_show_download_manager,
            this, &PoWidget::showDownloadManager);

    connect(url_address_bar, &AddressBar::sig_navigate,
            this, &PoWidget::sig_loadURL);

    connect(url_address_bar, &AddressBar::sig_save_to_file,
            this, &PoWidget::slot_save_to_file);

    connect(url_address_bar, &AddressBar::sig_useragent_changed,
            profile.get(), &Profile::setHttpUserAgent);

    connect(url_address_bar, &AddressBar::sig_show_ssl_security, this, [this]{
        auto page = tab->currentTabData()->page();
        if (nullptr == page)
        {
            return;
        }
        page->sslInformation().url = page->url();
        url_address_bar->updateSslInformation(page->sslInformation());
    });

    connect(url_address_bar, &AddressBar::sig_back, this, [this]{
        auto page = tab->currentTabData()->page();
        if (nullptr == page)
        {
            return;
        }
        page->history()->back();
    });

    connect(url_address_bar, &AddressBar::sig_forward, this, [this]{
        auto page = tab->currentTabData()->page();
        if (nullptr == page)
        {
            return;
        }
        page->history()->forward();
    });

    connect(url_address_bar, &AddressBar::sig_reload, this, [this]{
        auto page = tab->currentTabData()->page();
        if (nullptr == page)
        {
            return;
        }
        page->triggerAction(QWebEnginePage::Reload, true);
    });

    connect(newpage_button, &QPushButton::clicked, this, [this]{
        createWebPage("");
    });

    connect(this, &PoWidget::sig_loadURL, this, [this](QString url){
        if (url.isEmpty())
        {
            return;
        }
        createWebPage(url);
    });

    connect(profile.get(), &QWebEngineProfile::downloadRequested, this, &PoWidget::slot_downloadRequest);
}

void PoWidget::createWebPage(const QString& url)
{
    auto data = TabData::createWidget(profile.get(), central_widget);
    stack_layout->addWidget(data->webWidget().get());
    auto page = data->page();
    initializeWebPage(page);

    connect(data->webWidget().get(), &WebPageWidget::sig_toggleFullscreen,
            this, &PoWidget::toggleFullscreen);

    if (!url.isEmpty())
    {
        page->sslInformation().url = url;
        url_address_bar->updateSslInformation(page->sslInformation());
        page->load(QUrl(url));
    }
}

void PoWidget::workOnLastTabClosed()
{
    if (tab->count() > 0)
    {
        return;
    }
    setWindowTitle("");
    stack_layout->setCurrentIndex(0);
    url_address_bar->setUrl("");
    switchBookmarkButtons(true);
}

void PoWidget::uninitializeTab(int index)
{
    auto widget = tab->tabData(index)->webWidget();
    tab->clearTabData(index);
    if (widget)
    {
        widget->uninitialize();
    }
}

void PoWidget::switchBookmarkButtons(bool forceInvisible)
{
    url_address_bar->switchBookmarkButtons(forceInvisible);
}

void PoWidget::slot_save_to_file(int mode)
{
    int index = tab->currentIndex();
    auto page = tab->tabData(index)->page();
    if (!page)
    {
        return;
    }
    QString title = page->title().replace(":", "_");
    QString filename = profile->downloadPath() + QDir::separator() + title
                       +  (SaveToSingleHTMLFile == mode ?".mhtml" : ".pdf");
    if (SaveToSingleHTMLFile == mode)
    {
        page->save(filename);
    }
}

void PoWidget::initializeWebPage(WebEnginePage* page)
{
    int index = tab->addTab("Untitled");
    page->setTabIndex(index);
    tab->setTabData(index, page->tabData());
    tab->setCurrentIndex(index);
    stack_layout->setCurrentWidget(page->tabData()->webWidget().get());

    connect(page->tabData()->webWidget()->getView(), &WebEngineView::sig_navigate, this,
            &PoWidget::sig_loadURL, Qt::QueuedConnection);

    connect(page->tabData()->webWidget()->getView(), &WebEngineView::sig_save_to_file,
            this, &PoWidget::slot_save_to_file);

    connect(page, &QWebEnginePage::urlChanged, this, [this, page](const QUrl& url){
        int index = page->tabIndex();
        if (tab->currentIndex() != index)
        {
            return;
        }
        url_address_bar->setUrl(url.toDisplayString());
        url_address_bar->setCanBack(page->history()->canGoBack());
        url_address_bar->setCanForward(page->history()->canGoForward());
        switchBookmarkButtons();
        tab->setTabToolTip(index, page->title() + "\r\n" + page->url().toDisplayString());
        page->sslInformation().url = url;
        url_address_bar->updateSslInformation(page->sslInformation());

        QSharedPointer<Bookmark> bookmark(new Bookmark(page->title(), page->url().toString()));
        url_address_bar->appendComplete(bookmark);
    });

    connect(page, &QWebEnginePage::titleChanged, this, [this, page](const QString& text) {
        int index = page->tabIndex();
        if (tab->currentIndex() == index)
        {
            setWindowTitle(text);
        }
        if (index >= 0)
        {
            tab->setTabText(index, text.left(10));
            tab->setTabToolTip(index, text + "\r\n" + page->url().toDisplayString());
        }
        QSharedPointer<Bookmark> bookmark(new Bookmark(page->title(), page->url().toString()));
        url_address_bar->appendComplete(bookmark);
    });

    connect(page, &QWebEnginePage::iconChanged, this, [this, page](const QIcon& icon) {
        int index = page->tabIndex();
        if (index >= 0)
        {
            tab->setIconSize(QSize(24, 24));
            tab->setTabIcon(index, icon);
        }
    });

    connect(page, &QWebEnginePage::loadStarted, this, [this, page]{
        int index = page->tabIndex();
        if (tab->currentIndex() != index)
        {
            return;
        }
        statusBar()->setVisible(true);
        status_label->setText("Loading ...");
        progress_pb->setValue(0);
        progress_pb->setVisible(true);
        switchBookmarkButtons();
    });

    connect(page, &QWebEnginePage::loadFinished, this, [this, page](bool ok){
        int index = page->tabIndex();
        if (tab->currentIndex() != index)
        {
            return;
        }
        status_label->setText(ok ? "SUCCESS" : "FAILED");
        progress_pb->setVisible(false);
        statusBar()->setVisible(false);
        switchBookmarkButtons();
    });

    connect(page, &QWebEnginePage::loadProgress, this, [this, page](int progress){
        int index = page->tabIndex();
        if (tab->currentIndex() != index)
        {
            return;
        }
        progress_pb->setValue(progress);
        progress_pb->setVisible(progress < 100);
        statusBar()->setVisible(progress < 100);
    });

    connect(page, &QWebEnginePage::loadingChanged, this, [this, page](const QWebEngineLoadingInfo &loadingInfo) {
        int index = page->tabIndex();
        if (tab->currentIndex() != index)
        {
            return;
        }
        if (loadingInfo.status() != QWebEngineLoadingInfo::LoadStartedStatus)
        {
            progress_pb->setVisible(false);
        }
    });

    connect(page, &QWebEnginePage::newWindowRequested, this, [this](QWebEngineNewWindowRequest& request){
        createWebPage(request.requestedUrl().toString());
    });

    connect(page, &QWebEnginePage::certificateError, this, [page](const QWebEngineCertificateError &ce) {
        SslInformation& info = page->sslInformation();
        info.hasError = true;
        info.type = ce.type();
        info.certificateChain = ce.certificateChain();
        info.description = ce.description();
    });

    connect(page, &QWebEnginePage::fullScreenRequested, this, [page](QWebEngineFullScreenRequest request){
        request.accept();
        page->tabData()->webWidget()->toggleFullscreen(request.toggleOn());
    });

    connect(page, &QWebEnginePage::featurePermissionRequested, this,
            [page](const QUrl &securityOrigin, QWebEnginePage::Feature feature){
                QWebEnginePage::PermissionPolicy policy = QWebEnginePage::PermissionGrantedByUser;
                page->setFeaturePermission(securityOrigin, feature, policy);
            });
}

void PoWidget::slot_downloadRequest(QWebEngineDownloadRequest *download)
{
    download->accept();
    showDownloadManager();
}

void PoWidget::gotoPageUserInput(const QString& sURL)
{
    WebEnginePage* page = nullptr;
    int index = tab->currentIndex();
    if (index >= 0)
    {
        page = tab->tabData(index)->page();
    }

    QString strURL = sURL;
    if (strURL.startsWith("about:"))
    {
        strURL.replace(0, strlen("about:"), Version::scheme());
    }
    QUrl url = QUrl::fromUserInput(strURL);
    if (!url.isValid())
    {
        return;
    }
    if (page)
    {
        page->load(url);
    }
    else
    {
        createWebPage(url.toString());
    }
    switchBookmarkButtons();
}

void PoWidget::showDownloadManager()
{
    int index = -1;
    int count = tab->count();
    for (int i = 0; i < count; i++)
    {
        if (tab->tabData(i)->downloadWidget() == download_widget)
        {
            index = i;
            break;
        }
    }
    if (index >= 0)
    {
        tab->setCurrentIndex(index);
    }
    else
    {
        index = tab->addTab(download_widget->windowTitle());
        tab->setTabData(index, TabData::createWidget(download_widget));
        tab->setCurrentIndex(index);
        stack_layout->setCurrentWidget(download_widget.get());
    }
}

void PoWidget::showBookmarkManager()
{
    int index = -1;
    int count = tab->count();
    for (int i = 0; i < count; i++)
    {
        if (tab->tabData(i)->bookmarkWidget() == bookmark_widget)
        {
            index = i;
            break;
        }
    }
    if (index >= 0)
    {
        tab->setCurrentIndex(index);
    }
    else
    {
        index = tab->addTab(bookmark_widget->windowTitle());
        tab->setTabData(index, TabData::createWidget(bookmark_widget));
        tab->setCurrentIndex(index);
        stack_layout->setCurrentWidget(bookmark_widget.get());
    }
}

void PoWidget::gotoHomePage()
{
    createWebPage("https://www.baidu.com/");
}

void PoWidget::toggleFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        switchToFullscreen();
    }
    else
    {
        switchToNormal();
    }
}

void PoWidget::switchToFullscreen()
{
    if (fullscreen_widget)
    {
        return;
    }
    fullscreen_widget = stack_layout->currentWidget();
    fullscreen_layout_index = stack_layout->indexOf(fullscreen_widget);
    stack_layout->removeWidget(fullscreen_widget);
    fullscreen_widget->setParent(nullptr);
    fullscreen_widget->showFullScreen();
    fullscreen_widget->setFocus(Qt::FocusReason::NoFocusReason);
    hide();
}

void PoWidget::switchToNormal()
{
    if (!fullscreen_widget)
    {
        return;
    }
    fullscreen_widget->showNormal();
    stack_layout->insertWidget(fullscreen_layout_index, fullscreen_widget);
    stack_layout->setCurrentWidget(fullscreen_widget);
    fullscreen_widget->setFocus(Qt::FocusReason::MouseFocusReason);
    fullscreen_widget = nullptr;
    fullscreen_layout_index = -1;
    show();

    int index = tab->currentIndex();
    tab->setCurrentIndex(0);
    tab->setCurrentIndex(index);

}
