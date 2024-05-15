#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>

class QWebEngineNotification;
class QLabel;

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(QWidget *parent = nullptr);

    void showNotification(std::unique_ptr<QWebEngineNotification> notification);

signals:

private:
    QLabel* origin_label;
    QLabel* message_label;
    QLabel* tag_label;
    QLabel* icon_label;

};

#endif // NOTIFICATIONWIDGET_H
