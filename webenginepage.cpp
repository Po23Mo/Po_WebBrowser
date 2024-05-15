#include "webenginepage.h"
#include <QWebEngineSettings>
#include <QDebug>
#include <QWebEngineNavigationRequest>


WebEnginePage::WebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage{profile, parent}
    , tab_index(-1)
{
    connect(this, &WebEnginePage::navigationRequested,
            this, [](QWebEngineNavigationRequest &request) {
                request.accept();
            });
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                                      const QString &message, int lineNumber, const QString &sourceID)
{

}

SslInformation& WebEnginePage::sslInformation()
{
    return ssl_info;
}

int WebEnginePage::tabIndex() const
{
    return tab_index;
}

void WebEnginePage::setTabIndex(int index)
{
    tab_index = index;
}

void WebEnginePage::setTabData( QSharedPointer<TabData> data)
{
    tab_data = data.toWeakRef();
}

QSharedPointer<TabData> WebEnginePage::tabData()
{
    return tab_data.lock();
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    return true;
}
