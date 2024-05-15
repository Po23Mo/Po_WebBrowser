#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QWebEnginePage>
#include <QSharedPointer>
#include "securitystatusdialog.h"
#include "tabdata.h"

class WebEnginePage : public QWebEnginePage, public QEnableSharedFromThis<WebEnginePage>
{
    Q_OBJECT
public:
    explicit WebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);
    SslInformation& sslInformation();
    int tabIndex() const;
    void setTabIndex(int index);
    void setTabData( QSharedPointer<TabData> data);
    QSharedPointer<TabData> tabData();


protected:
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                  const QString &message, int lineNumber, const QString &sourceID) override;
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;


private:
    int tab_index;
    QWeakPointer<TabData> tab_data;
    SslInformation ssl_info;
};

#endif // WEBENGINEPAGE_H
