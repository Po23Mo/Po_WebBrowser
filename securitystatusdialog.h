#ifndef SECURITYSTATUSDIALOGH_H
#define SECURITYSTATUSDIALOGH_H

#include "dialog.h"
#include <QWebEngineCertificateError>


enum SecurityStatusId
{
    SslSecure,
    SslInsecure,
    SslError,
    SslNoSsl
};

struct SslInformation
{
    SslInformation();

    void analyse();
    bool is_secure() const;
    QString get_ssl_status() const;

    bool hasError;
    bool withSSL;
    QUrl url;
    SecurityStatusId statusId;
    QWebEngineCertificateError::Type type;
    QString description;
    QList<QSslCertificate> certificateChain;
};

class SecurityStatusDialog : public Dialog
{
    Q_OBJECT
public:
    explicit SecurityStatusDialog(const SslInformation& sslInfo, QWidget *parent = nullptr);


signals:

private:
    SslInformation ssl_info;

};

#endif // SECURITYSTATUSDIALOGH_H
