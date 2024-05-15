#include "DownloadRequest.h"

DownloadRequest::DownloadRequest(QWebEngineDownloadRequest* download, QTreeWidgetItem* item, QObject *parent)
    : QObject{parent}
    , download_request(download)
    , tree_item(item)
{
    request_url = download->url();
    connect(download, &QWebEngineDownloadRequest::stateChanged, this, &DownloadRequest::slot_stateChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::savePageFormatChanged, this, &DownloadRequest::slot_savePageFormatChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::receivedBytesChanged, this, &DownloadRequest::slot_receivedBytesChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::totalBytesChanged, this, &DownloadRequest::slot_totalBytesChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::interruptReasonChanged, this, &DownloadRequest::slot_interruptReasonChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::isFinishedChanged, this, &DownloadRequest::slot_isFinishedChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::isPausedChanged, this, &DownloadRequest::slot_isPausedChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::downloadDirectoryChanged, this, &DownloadRequest::slot_downloadDirectoryChanged, Qt::QueuedConnection);
    connect(download, &QWebEngineDownloadRequest::downloadFileNameChanged, this, &DownloadRequest::slot_downloadFileNameChanged, Qt::QueuedConnection);
}

QWebEngineDownloadRequest* DownloadRequest::request()
{
    return download_request;
}

QTreeWidgetItem* DownloadRequest::item()
{
    return tree_item;
}

QUrl DownloadRequest::url()
{
    return request_url;
}

void DownloadRequest::slot_stateChanged(QWebEngineDownloadRequest::DownloadState state)
{
    emit sig_stateChanged(sharedFromThis(), state);
}

void DownloadRequest::slot_savePageFormatChanged()
{
    emit sig_savePageFormatChanged(sharedFromThis());
}

void DownloadRequest::slot_receivedBytesChanged()
{
    emit sig_receivedBytesChanged(sharedFromThis());
}

void DownloadRequest::slot_totalBytesChanged()
{
    emit sig_totalBytesChanged(sharedFromThis());
}

void DownloadRequest::slot_interruptReasonChanged()
{
    emit sig_interruptReasonChanged(sharedFromThis());
}

void DownloadRequest::slot_isFinishedChanged()
{
    emit sig_isFinishedChanged(sharedFromThis());
}

void DownloadRequest::slot_isPausedChanged()
{
    emit sig_isPausedChanged(sharedFromThis());
}

void DownloadRequest::slot_downloadDirectoryChanged()
{
    emit sig_downloadDirectoryChanged(sharedFromThis());
}

void DownloadRequest::slot_downloadFileNameChanged()
{
    emit sig_downloadFileNameChanged(sharedFromThis());
}
