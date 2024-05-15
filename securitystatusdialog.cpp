#include "securitystatusdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include "version.h"

SslInformation::SslInformation()
    : hasError(false)
    , withSSL(false)
{

}

bool SslInformation::is_secure() const
{
    return url.scheme().compare(Version::scheme().toUtf8()) == 0 ||
           url.isEmpty() ||
           (withSSL && SslSecure == statusId);
}

void SslInformation::analyse()
{
    if (url.scheme().compare("https", Qt::CaseInsensitive) != 0)
    {
        statusId = SslNoSsl;
        withSSL = false;
        return;
    }

    withSSL = true;
    if (hasError)
    {
        statusId = SslError;
    }
    else
    {
        statusId = SslSecure;
    }
}

QString SslInformation::get_ssl_status() const
{
    QString result;
    switch (statusId)
    {
    case SslSecure:
        result = "SSL is secure";
        break;
    case SslInsecure:
        result = "SSL is insecure";
        break;
    case SslError:
        result = "SSL has some errors";
        break;

    default:
        result = "Without SSL.";
        result += "\r\n";
        if (is_secure())
        {
            result += "Because this page is an internal page, so this page is secure.";
        }
        else
        {
            result += "Because this connection is not secure, information such as passwords is not securely sent to this site"
                      " and may be intercepted or seen by others."
                      "\r\nThe web data you see may be the data that has been tampered with by the attacker, "
                      "rather than the data sent by the original website.";
        }
        break;
    }
    return result;
}

SecurityStatusDialog::SecurityStatusDialog(const SslInformation& sslInfo, QWidget *parent)
    : Dialog{parent}
    , ssl_info(sslInfo)
{

    ssl_info.analyse();

    QVBoxLayout* main_layout = new QVBoxLayout();
    setLayout(main_layout);

    main_layout->addWidget(new QLabel("URL"));
    auto url_edit = new QLineEdit(ssl_info.url.toString());
    url_edit->setReadOnly(true);
    main_layout->addWidget(url_edit);

    main_layout->addWidget(new QLabel("Secure?"));
    auto secure_edit = new QLineEdit(ssl_info.is_secure() ? "This web site is secure" : "This web site is dangerous");
    secure_edit->setReadOnly(true);
    main_layout->addWidget(secure_edit);

    main_layout->addWidget(new QLabel("SSL enabled?"));
    auto ssl_enabled_edit = new QLineEdit(ssl_info.withSSL ? "YES" : "NO");
    ssl_enabled_edit->setReadOnly(true);
    main_layout->addWidget(ssl_enabled_edit);

    main_layout->addWidget(new QLabel("SSL status"));
    auto ssl_status_edit = new QTextEdit(ssl_info.get_ssl_status());
    ssl_status_edit->setReadOnly(true);
    main_layout->addWidget(ssl_status_edit);

    QPushButton* ok_button = new QPushButton();
    ok_button->setText("OK");
    main_layout->addWidget(ok_button);

    connect(ok_button, &QPushButton::clicked, this, &QDialog::close);

    main_layout->addStretch();
    resize(800, 600);
}

