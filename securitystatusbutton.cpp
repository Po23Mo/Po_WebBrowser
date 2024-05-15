#include "securitystatusbutton.h"

SecurityStatusButton::SecurityStatusButton(QWidget* p)
    : QPushButton(p)
{
    setFlat(true);
    setToolTip("This  site is secure.");
    setIconSize(QSize(32, 32));
    status_pixmaps.insert(SslSecure, QPixmap(":/images/https_secure.png"));
    status_pixmaps.insert(SslInsecure, QPixmap(":/images/https_insecure.png"));
    status_pixmaps.insert(SslError, QPixmap(":/images/https_error.png"));
    status_pixmaps.insert(SslNoSsl, QPixmap(":/images/http.png"));

    setIcon(QIcon(status_pixmaps.value(SslSecure)));
}

void SecurityStatusButton::showSslInformation(const SslInformation& info)
{
    SecurityStatusDialog dlg(info);
    dlg.exec();
}

void SecurityStatusButton::updateSslInformation(const SslInformation& info)
{
    SslInformation ssl_info = info;
    ssl_info.analyse();
    if (ssl_info.is_secure())
    {
        setIcon(QIcon(status_pixmaps.value(SslSecure)));
    }
    else
    {
        setIcon(QIcon(status_pixmaps.value(ssl_info.statusId)));
    }
}
