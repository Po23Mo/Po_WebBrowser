#include "notificationwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QWebEngineNotification>
#include <QPixmap>
#include <QIcon>
#include <QTimer>

NotificationWidget::NotificationWidget(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlag(Qt::WindowType::ToolTip);
    QVBoxLayout* main_layout = new QVBoxLayout();
    setLayout(main_layout);
    main_layout->addWidget(new QLabel("Origin:"));
    origin_label = new QLabel();
    main_layout->addWidget(origin_label);

    main_layout->addWidget(new QLabel("Tag:"));
    tag_label = new QLabel();
    main_layout->addWidget(tag_label);

    icon_label = new QLabel();
    main_layout->addWidget(icon_label);

    main_layout->addWidget(new QLabel("Message:"));
    message_label = new QLabel();
    main_layout->addWidget(message_label);
}

void NotificationWidget::showNotification(std::unique_ptr<QWebEngineNotification> notification)
{
    origin_label->setText(notification->origin().toString());
    tag_label->setText(notification->tag());
    icon_label->setPixmap(QPixmap::fromImage(notification->icon().copy()));
    message_label->setText(notification->message());
    show();

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000 * 10);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &NotificationWidget::close);
    timer->start();
}
