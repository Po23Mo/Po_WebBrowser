#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>

class QMenu;
class QTreeWidget;
class DownloadRequest;

class DownloadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadWidget(QSharedPointer<QWebEngineProfile> profile, QWidget *parent = nullptr);

signals:
    void sig_reload_url(QString url);

protected slots:
    void slot_downloadRequest(QWebEngineDownloadRequest *download);
    void slot_stateChanged(QSharedPointer<DownloadRequest> wrapper,
                           QWebEngineDownloadRequest::DownloadState state);
    void slot_savePageFormatChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_receivedBytesChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_totalBytesChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_interruptReasonChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_isFinishedChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_isPausedChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_downloadDirectoryChanged(QSharedPointer<DownloadRequest> wrapper);
    void slot_downloadFileNameChanged(QSharedPointer<DownloadRequest> wrapper);

private:
    QTreeWidget* tree;
    QSharedPointer<QWebEngineProfile> profile;
    QList<QSharedPointer<DownloadRequest>> wrappers;
    QMenu* popup_menu;
};

#endif // DOWNLOADWIDGET_H
