#ifndef DOWNLOADREQUEST_H
#define DOWNLOADREQUEST_H

#include <QObject>
#include <QWebEngineDownloadRequest>
#include <QSharedPointer>

class QTreeWidgetItem;


class DownloadRequest : public QObject, public QEnableSharedFromThis<DownloadRequest>
{
    Q_OBJECT
public:
    explicit DownloadRequest(QWebEngineDownloadRequest* req, QTreeWidgetItem* item, QObject *parent = nullptr);

    QWebEngineDownloadRequest* request();
    QTreeWidgetItem* item();
    QUrl url();

signals:
    void sig_stateChanged(QSharedPointer<DownloadRequest> wrapper,QWebEngineDownloadRequest::DownloadState state);
    void sig_savePageFormatChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_receivedBytesChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_totalBytesChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_interruptReasonChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_isFinishedChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_isPausedChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_downloadDirectoryChanged(QSharedPointer<DownloadRequest> wrapper);
    void sig_downloadFileNameChanged(QSharedPointer<DownloadRequest> wrapper);

protected slots:
    void slot_stateChanged(QWebEngineDownloadRequest::DownloadState state);
    void slot_savePageFormatChanged();
    void slot_receivedBytesChanged();
    void slot_totalBytesChanged();
    void slot_interruptReasonChanged();
    void slot_isFinishedChanged();
    void slot_isPausedChanged();
    void slot_downloadDirectoryChanged();
    void slot_downloadFileNameChanged();

private:
    QWebEngineDownloadRequest* download_request;
    QTreeWidgetItem* tree_item;
    QUrl request_url;

};

#endif // DOWNLOADREQUEST_H
