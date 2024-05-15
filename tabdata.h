#ifndef TABDATA_H
#define TABDATA_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

class WebPageWidget;
class DownloadWidget;
class BookmarkWidget;
class WebEnginePage;
class QWebEngineProfile;

class TabData : public QObject, public QEnableSharedFromThis<TabData>
{
    Q_OBJECT
public:
    TabData();
    TabData(const TabData& other);
    QSharedPointer<WebPageWidget> webWidget() const;
    QSharedPointer<DownloadWidget> downloadWidget() const;
    QSharedPointer<BookmarkWidget> bookmarkWidget() const;
    WebEnginePage* page() const;

    operator QVariant () const;
    static QSharedPointer<TabData> createWidget(QWebEngineProfile* profile, QWidget* parent);
    static QSharedPointer<TabData> createWidget(QSharedPointer<DownloadWidget> widget);
    static QSharedPointer<TabData> createWidget(QSharedPointer<BookmarkWidget> widget);

    static  QSharedPointer<TabData> nullData();

signals:


private:
    QSharedPointer<WebPageWidget> web_widget;
    QSharedPointer<DownloadWidget> download_widget;
    QSharedPointer<BookmarkWidget> bookmark_widget;
    static QSharedPointer<TabData> null_data;
};


#endif // TABDATA_H
