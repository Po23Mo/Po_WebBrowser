#ifndef SECURITYSTATUSBUTTON_H
#define SECURITYSTATUSBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include "securitystatusdialog.h"

class SecurityStatusDialog;

class SecurityStatusButton : public QPushButton
{
    Q_OBJECT
public:
    SecurityStatusButton(QWidget* p = nullptr);

    void showSslInformation(const SslInformation& info);
    void updateSslInformation(const SslInformation& info);

private:
    QHash<SecurityStatusId, QPixmap> status_pixmaps;
};

#endif // SECURITYSTATUSBUTTON_H
