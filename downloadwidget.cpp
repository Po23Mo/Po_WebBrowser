#include "downloadwidget.h"
#include "downloadrequest.h"
#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWebEngineDownloadRequest>
#include <QPushButton>
#include <QProgressBar>
#include <QPushButton>
#include <QDesktopServices>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QAction>


enum DownloadWidgetColumn
{
    DOWNLOAD_STATUS,
    DOWNLOAD_PROGRESS,
    DOWNLOAD_BYTES_TOTAL,
    DOWNLOAD_BYTES_DONE,
    DOWNLOAD_FILENAME,
    DOWNLOAD_URL,

    DOWNLOAD_COLUMN_COUNT
};


QString bytes_to_str(qint64 bytes)
{
    constexpr qint64 KB = 1024LL;
    constexpr qint64 MB = 1024LL * KB;
    constexpr qint64 GB = 1024LL * MB;
    qint64 gb = bytes / GB;
    qint64 mb = (bytes % GB) / MB;
    qint64 kb = (bytes % MB) / KB;
    qint64 b = (bytes % KB);

    QString str;
    if (gb > 0)
    {
        str += QString("%1GB").arg(gb);
    }
    if (mb > 0)
    {
        str += QString("%1MB").arg(mb);
    }
    if (kb > 0)
    {
        str += QString("%1KB").arg(kb);
    }
    if (b > 0)
    {
        str += QString("%1Bytes").arg(b);
    }
    return str;
}

DownloadWidget::DownloadWidget(QSharedPointer<QWebEngineProfile> vprofile, QWidget *parent)
    : QWidget{parent}
    , profile(vprofile)
{
    QVBoxLayout* main_layout = new QVBoxLayout();
    setLayout(main_layout);
    setWindowTitle("Download Manager");

    tree = new QTreeWidget();
    tree->setUniformRowHeights(true);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setTextElideMode(Qt::ElideMiddle);
    tree->setColumnCount(DOWNLOAD_COLUMN_COUNT);
    tree->setColumnWidth(DOWNLOAD_STATUS, 100);
    tree->setColumnWidth(DOWNLOAD_PROGRESS, 100);
    tree->setColumnWidth(DOWNLOAD_BYTES_TOTAL, 150);
    tree->setColumnWidth(DOWNLOAD_BYTES_DONE, 150);
    tree->setColumnWidth(DOWNLOAD_FILENAME, 300);
    tree->setColumnWidth(DOWNLOAD_URL, 300);

    QStringList headers;
    headers << "Status" << "Progress" << "Total Bytes" << "Received Bytes" << "Filename" << "URL";
    tree->setHeaderLabels(headers);
    main_layout->addWidget(tree);

    QHBoxLayout* button_layout = new QHBoxLayout();
    QPushButton* download_dir_button = new QPushButton();
    download_dir_button->setText("Browser download directory");
    button_layout->addWidget(download_dir_button);

    QPushButton* clear_finished_button = new QPushButton();
    clear_finished_button->setText("Remove finished dwnload items");
    button_layout->addWidget(clear_finished_button);

    QPushButton* copy_all_urls_button = new QPushButton();
    copy_all_urls_button->setText("Copy all URLs to clipboard");
    button_layout->addWidget(copy_all_urls_button);

    main_layout->addLayout(button_layout);


    popup_menu = new QMenu(this);
    QAction* copy_url_action = new QAction(popup_menu);
    copy_url_action->setText("Copy URL to clipboard");
    popup_menu->addAction(copy_url_action);

    QAction* copy_filename_action = new QAction(popup_menu);
    copy_filename_action->setText("Copy filename to clipboard");
    popup_menu->addAction(copy_filename_action);

    QAction* reload_action = new QAction(popup_menu);
    reload_action->setText("Reload web page with this URL");
    popup_menu->addAction(reload_action);
    QList<QAction*> actions = tree->actions();
    actions << copy_url_action << copy_filename_action << reload_action;
    tree->insertActions(nullptr, actions);

    tree->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    connect(copy_url_action, &QAction::triggered, this, [this]{
        int index = tree->currentIndex().row();
        if (index < 0)
        {
            return;
        }
        auto& wrapper = wrappers[index];
        QString url = wrapper->url().toString();
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(url);
        }
    });

    connect(copy_filename_action, &QAction::triggered, this, [this]{
        int index = tree->currentIndex().row();
        if (index < 0)
        {
            return;
        }
        auto& wrapper = wrappers[index];
        QString filename = wrapper->request()->downloadFileName();
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(filename);
        }
    });

    connect(reload_action, &QAction::triggered, this, [this]{
        int index = tree->currentIndex().row();
        if (index < 0)
        {
            return;
        }
        auto& wrapper = wrappers[index];
        QString url = wrapper->url().toString();
        emit sig_reload_url(url);
    });

    connect(download_dir_button, &QPushButton::clicked, this, [this]{
        QDesktopServices::openUrl(QUrl::fromLocalFile(profile->downloadPath()));
    });

    connect(clear_finished_button, &QPushButton::clicked, this, [this]{
        int count = wrappers.length();
        for (int i = count; i > 0; i--)
        {
            auto wrapper = wrappers[i - 1];
            if (wrapper->request()->isFinished())
            {
                wrappers.removeAt(i - 1);
                auto item = tree->takeTopLevelItem(i - 1);
                delete item;
            }
        }
    });


    connect(copy_all_urls_button, &QPushButton::clicked, this, [this]{
        QStringList list;
        int count = wrappers.length();
        for (int i = 0; i < count; i++)
        {
            auto& wrapper = wrappers[i];
            QString url = wrapper->url().toString();
            list << url;
        }

        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(list.join("\r\n"));
        }
    });

    connect(profile.get(), &QWebEngineProfile::downloadRequested,
            this, &DownloadWidget::slot_downloadRequest);
}

void DownloadWidget::slot_downloadRequest(QWebEngineDownloadRequest *download)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    tree->addTopLevelItem(item);
    QSharedPointer<DownloadRequest> wrapper(new DownloadRequest(download, item, this));
    wrappers.append(wrapper);

    QPushButton* status_button = new QPushButton();
    status_button->setFlat(true);
    status_button->setText("Starting");
    tree->setItemWidget(item, DOWNLOAD_STATUS, status_button);

    QProgressBar* progress_pb = new QProgressBar();
    tree->setItemWidget(item, DOWNLOAD_PROGRESS, progress_pb);

    QPushButton* total_button = new QPushButton();
    total_button->setFlat(true);
    total_button->setText(bytes_to_str(download->totalBytes()));
    tree->setItemWidget(item, DOWNLOAD_BYTES_TOTAL, total_button);

    QPushButton* done_button = new QPushButton();
    done_button->setFlat(true);
    done_button->setText(bytes_to_str(download->receivedBytes()));
    tree->setItemWidget(item, DOWNLOAD_BYTES_DONE, done_button);

    QPushButton* filename_button = new QPushButton();
    filename_button->setFlat(true);
    filename_button->setText(download->downloadFileName());
    tree->setItemWidget(item, DOWNLOAD_FILENAME, filename_button);

    QPushButton* url_button = new QPushButton();
    url_button->setFlat(true);
    url_button->setText(download->url().toString());
    tree->setItemWidget(item, DOWNLOAD_URL, url_button);

    connect(wrapper.get(), &DownloadRequest::sig_stateChanged, this, &DownloadWidget::slot_stateChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_savePageFormatChanged, this, &DownloadWidget::slot_savePageFormatChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_receivedBytesChanged, this, &DownloadWidget::slot_receivedBytesChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_totalBytesChanged, this, &DownloadWidget::slot_totalBytesChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_interruptReasonChanged, this, &DownloadWidget::slot_interruptReasonChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_isFinishedChanged, this, &DownloadWidget::slot_isFinishedChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_isPausedChanged, this, &DownloadWidget::slot_isPausedChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_downloadDirectoryChanged, this, &DownloadWidget::slot_downloadDirectoryChanged, Qt::QueuedConnection);
    connect(wrapper.get(), &DownloadRequest::sig_downloadFileNameChanged, this, &DownloadWidget::slot_downloadFileNameChanged, Qt::QueuedConnection);
}

void DownloadWidget::slot_stateChanged(QSharedPointer<DownloadRequest> wrapper, QWebEngineDownloadRequest::DownloadState state)
{
    auto get_status = [](QWebEngineDownloadRequest::DownloadState state) {
        switch (state)
        {
        case QWebEngineDownloadRequest::DownloadRequested: return "Starting";
        case QWebEngineDownloadRequest::DownloadInProgress: return "Downloading";
        case QWebEngineDownloadRequest::DownloadCompleted: return "Finished";
        case QWebEngineDownloadRequest::DownloadCancelled: return "Cancelled";
        case QWebEngineDownloadRequest::DownloadInterrupted: return "Interrupted";
        default: return "Unknown";
        }
    };

    QTreeWidgetItem* item = wrapper->item();
    QPushButton* status_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_STATUS));
    status_button->setText(get_status(state));
}

void DownloadWidget::slot_savePageFormatChanged(QSharedPointer<DownloadRequest> wrapper)
{

}

void DownloadWidget::slot_receivedBytesChanged(QSharedPointer<DownloadRequest> wrapper)
{
    QWebEngineDownloadRequest* download = wrapper->request();
    QTreeWidgetItem* item = wrapper->item();

    QPushButton* done_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_BYTES_DONE));
    done_button->setText(bytes_to_str(download->receivedBytes()));

    QProgressBar* progress_pb = qobject_cast<QProgressBar*>(tree->itemWidget(item, DOWNLOAD_PROGRESS));
    progress_pb->setValue(download->totalBytes() > 0 ? download->receivedBytes() * 100 / download->totalBytes() : 0);
}

void DownloadWidget::slot_totalBytesChanged(QSharedPointer<DownloadRequest> wrapper)
{
    QWebEngineDownloadRequest* download = wrapper->request();
    QTreeWidgetItem* item = wrapper->item();

    QPushButton* total_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_BYTES_TOTAL));
    total_button->setText(bytes_to_str(download->totalBytes()));
}

void DownloadWidget::slot_interruptReasonChanged(QSharedPointer<DownloadRequest> wrapper)
{

}

void DownloadWidget::slot_isFinishedChanged(QSharedPointer<DownloadRequest> wrapper)
{
    QWebEngineDownloadRequest* download = wrapper->request();
    QTreeWidgetItem* item = wrapper->item();

    QPushButton* status_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_STATUS));
    status_button->setText(download->isFinished() ? "Finished" : "Downloading");
}

void DownloadWidget::slot_isPausedChanged(QSharedPointer<DownloadRequest> wrapper)
{
    QWebEngineDownloadRequest* download = wrapper->request();
    QTreeWidgetItem* item = wrapper->item();

    QPushButton* status_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_STATUS));
    status_button->setText(download->isPaused() ? "Paused" : "Downloading");
}

void DownloadWidget::slot_downloadDirectoryChanged(QSharedPointer<DownloadRequest> wrapper)
{

}

void DownloadWidget::slot_downloadFileNameChanged(QSharedPointer<DownloadRequest> wrapper)
{
    QWebEngineDownloadRequest* download = wrapper->request();
    QTreeWidgetItem* item = wrapper->item();

    QPushButton* filename_button = qobject_cast<QPushButton*>(tree->itemWidget(item, DOWNLOAD_FILENAME));
    filename_button->setText(download->downloadFileName());
}
