#include "version.h"
#include <QVersionNumber>
#include <QWebEngineProfile>
#include <QSysInfo>

const char* C__WEB_BROWSER_FULL_VERSION ="2024";

QString Version::productName()
{
    return " Po Web Browser";
}

QString Version::scheme()
{
    return "PoWebBrowser";
}

QString Version::fullVersion()
{
    return QString("%1 %2")
        .arg(productName())
        .arg(C__WEB_BROWSER_FULL_VERSION)
        ;
}


QString Version::qtVersion()
{
    return qVersion();
}

QString Version::chromiumVersion()
{
    return QString("%1, Security patch: %2")
        .arg(qWebEngineChromiumVersion())
        .arg(qWebEngineChromiumSecurityPatchVersion());
}


QString Version::osVersion()
{
    QSysInfo si;
    return si.prettyProductName();
}
