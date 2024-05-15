#include "tabdata.h"
#include "webpagewidget.h"
#include "webenginepage.h"

QSharedPointer<TabData> TabData::null_data(new TabData());

TabData::TabData()
    : web_widget(nullptr)
    , download_widget(nullptr)
    , bookmark_widget(nullptr)
{
}

TabData::TabData(const TabData& other)
    : web_widget(other.web_widget)
    , download_widget(other.download_widget)
    , bookmark_widget(other.bookmark_widget)
{
}

QSharedPointer<TabData> TabData::createWidget(
    QWebEngineProfile* profile, QWidget* parent)
{
    QSharedPointer<TabData> data(new TabData());
    data->web_widget.reset(new WebPageWidget(profile, parent));
    data->webWidget()->getPage()->setTabData(data);
    return data;
}

QSharedPointer<TabData> TabData::createWidget(
    QSharedPointer<DownloadWidget> widget)
{
    QSharedPointer<TabData> data(new TabData());
    data->download_widget = widget;
    return data;
}

QSharedPointer<TabData> TabData::createWidget(
    QSharedPointer<BookmarkWidget> widget)
{
    QSharedPointer<TabData> data(new TabData());
    data->bookmark_widget = widget;
    return data;
}

QSharedPointer<WebPageWidget> TabData::webWidget() const
{
    return web_widget;
}

WebEnginePage* TabData::page() const
{
    return web_widget ? web_widget->getPage() : nullptr;
}

QSharedPointer<DownloadWidget> TabData::downloadWidget() const
{
    return download_widget;
}

QSharedPointer<BookmarkWidget> TabData::bookmarkWidget() const
{
    return bookmark_widget;
}

TabData::operator QVariant () const
{
    return QVariant::fromValue(*this);
}

QSharedPointer<TabData> TabData::nullData()
{
    return null_data;
}
