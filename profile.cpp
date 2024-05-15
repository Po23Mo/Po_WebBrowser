#include "profile.h"
#include <QWebEngineSettings>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QWebEngineNotification>
#include "urlschemehandler.h"
#include "notificationwidget.h"
#include "useragent.h"


Profile::Profile(QObject *parent)
    : QWebEngineProfile{parent}
{
    QDir dir(qApp->applicationDirPath());

    setDownloadPath(qApp->applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "download");
    dir.mkpath(QStringLiteral("data") + QDir::separator() + "download");
    dir.mkpath(QStringLiteral("data") + QDir::separator() + "navigation");

    setPersistentStoragePath(qApp->applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "storage");
    dir.mkpath(QStringLiteral("data") + QDir::separator() + "storage");

    setCachePath(qApp->applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "cache");
    dir.mkpath(QStringLiteral("data") + QDir::separator() + "cache");

    setHttpCacheMaximumSize(1024 * 1024 * 1);

    setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);

    setHttpUserAgent(UserAgent_PoWebBrowser);

    initializeSecurityProfile();
    initializeNotification();
}

void Profile::setHttpUserAgent(int useragent)
{
    int index = 0;
    int count = get_useragent_count();
    for (int i = 0; i < count; i++)
    {
        const auto ua = get_useragent(i);
        if (useragent == ua.id)
        {
            index = i;
            break;
        }
    }
    const auto ua = get_useragent(index);
    QWebEngineProfile::setHttpUserAgent(ua.ua);
}

struct SettingAttribute
{
    QWebEngineSettings::WebAttribute attribute;
    bool enabled;
};

const SettingAttribute C__WEBENGINE_SETTINGS_SECURITY[] = {
    { QWebEngineSettings::AutoLoadImages, true},
    { QWebEngineSettings::JavascriptEnabled, true},
    { QWebEngineSettings::JavascriptCanOpenWindows, true},
    { QWebEngineSettings::JavascriptCanAccessClipboard, false},
    { QWebEngineSettings::LinksIncludedInFocusChain, true},
    { QWebEngineSettings::LocalStorageEnabled, true},
    { QWebEngineSettings::LocalContentCanAccessRemoteUrls, false},
    { QWebEngineSettings::XSSAuditingEnabled, true},
    { QWebEngineSettings::SpatialNavigationEnabled, false},
    { QWebEngineSettings::LocalContentCanAccessFileUrls, false},
    { QWebEngineSettings::HyperlinkAuditingEnabled, true},
    { QWebEngineSettings::ScrollAnimatorEnabled, true},
    { QWebEngineSettings::ErrorPageEnabled, true},
    { QWebEngineSettings::PluginsEnabled, false},
    { QWebEngineSettings::FullScreenSupportEnabled, true},
    { QWebEngineSettings::ScreenCaptureEnabled, false},
    { QWebEngineSettings::WebGLEnabled, true},
    { QWebEngineSettings::Accelerated2dCanvasEnabled, true},
    { QWebEngineSettings::AutoLoadIconsForPage, true},
    { QWebEngineSettings::TouchIconsEnabled, true},
    { QWebEngineSettings::FocusOnNavigationEnabled, true},
    { QWebEngineSettings::PrintElementBackgrounds, true},
    { QWebEngineSettings::AllowRunningInsecureContent, false},
    { QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false},
    { QWebEngineSettings::AllowWindowActivationFromJavaScript, true},
    { QWebEngineSettings::ShowScrollBars, true},
    { QWebEngineSettings::PlaybackRequiresUserGesture, true},
    { QWebEngineSettings::WebRTCPublicInterfacesOnly, true},
    { QWebEngineSettings::JavascriptCanPaste, false},
    { QWebEngineSettings::DnsPrefetchEnabled, false},
    { QWebEngineSettings::PdfViewerEnabled, true},
    { QWebEngineSettings::NavigateOnDropEnabled, true}
};


void Profile::initializeSecurityProfile()
{
    QWebEngineSettings* set = settings();
    for (const auto& attribute : C__WEBENGINE_SETTINGS_SECURITY)
    {
        set->setAttribute(attribute.attribute, attribute.enabled);
    }
    UrlSchemeHandler::initialize(this);
}

void Profile::initializeNotification()
{
    setNotificationPresenter([](std::unique_ptr<QWebEngineNotification> notification){
        auto notify_widget = new NotificationWidget();
        notify_widget->showNotification(std::move(notification));
    });
}
