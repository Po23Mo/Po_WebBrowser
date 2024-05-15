#include "urlschemehandler.h"
#include <QWebEngineUrlRequestJob>
#include <QWebEngineProfile>
#include <QWebEngineUrlScheme>
#include <QUrl>
#include <QFile>
#include <QBuffer>
#include <QMap>
#include "version.h"

const char* C_DEFAULT_CONTENT_TYPE = "application/octet-stream";


QHash<QByteArray, QByteArray> UrlSchemeHandler::contentTypes;


UrlSchemeHandler::UrlSchemeHandler(QObject* parent)
    : QWebEngineUrlSchemeHandler(parent)
{

}


void UrlSchemeHandler::initialize()
{
    {
        QWebEngineUrlScheme scheme(Version::scheme().toUtf8());
        scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
        scheme.setFlags(QWebEngineUrlScheme::ContentSecurityPolicyIgnored);
        QWebEngineUrlScheme::registerScheme(scheme);
    }
    {
        initializeContentTypes();
    }
}

void UrlSchemeHandler::initialize(QWebEngineProfile* profile)
{
    {
        UrlSchemeHandler *handler = new UrlSchemeHandler(profile);
        profile->installUrlSchemeHandler(Version::scheme().toUtf8(), handler);
    }
}


void UrlSchemeHandler::initializeContentTypes()
{
    contentTypes.insert("html", "text/html; charset=utf-8");
    contentTypes.insert("htm", "text/html; charset=utf-8");
    contentTypes.insert("xml", "text/xml; charset=utf-8");
    contentTypes.insert("txt", "text/plain; charset=utf-8");
    contentTypes.insert("json", "application/json; charset=utf-8");
    contentTypes.insert("png", "image/png");
    contentTypes.insert("jpg", "image/jpeg");
    contentTypes.insert("gif", "image/gif");
    contentTypes.insert("pdf", "application/pdf");
    contentTypes.insert("doc", "application/msword");
    contentTypes.insert("docx", "application/msword");
    contentTypes.insert("qml", "text/qml; charset=utf-8");
}

QByteArray UrlSchemeHandler::getContentTypeByFilename(QByteArray filename)
{
    QByteArray ext = filename.right(filename.length() - filename.lastIndexOf(".") - 1);
    if (ext.length() > 0 && contentTypes.contains(ext))
    {
        return contentTypes.value(ext);
    }
    return C_DEFAULT_CONTENT_TYPE;
}

void UrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    if (request->requestUrl().scheme().compare(Version::scheme().toUtf8(), Qt::CaseInsensitive) == 0)
    {
        handleUrl(request);
    }
}

bool UrlSchemeHandler::handleUrl(QWebEngineUrlRequestJob *request)
{
    return handle(request);
}

bool UrlSchemeHandler::handle(QWebEngineUrlRequestJob *request)
{
    QUrl url = QUrl(request->requestUrl().toString().replace(Version::scheme() + "://", Version::scheme() + ":/"));
    QString path = url.path();
    QString query = url.query();
    while (path.length() > 0 && path.at(0) == '/')
    {
        path.remove(0, 1);
    }

    path.replace("/", "-").replace("\\", "-").replace(":", "-").replace("?", "-");
    QHash<QString, QString>  args = queryArguments(query);

    if (!args.contains("language"))
    {
        QStringList acceptLanguages = getLanguagesFromHeaders(request);
        if (acceptLanguages.length() > 0)
        {
            args.insert("language", acceptLanguages[0]);
        }
        else
        {
            args.insert("language", "en_US");
        }
    }
    return true;
}

QStringList UrlSchemeHandler::getLanguagesFromHeaders(QWebEngineUrlRequestJob *request)
{
    QStringList languages;
    QMap<QByteArray, QByteArray> headers = request->requestHeaders();
    if (headers.contains("Accept-Language"))
    {
        QStringList list = QString::fromUtf8(headers.value("Accept-Language")).split(",");
        for (auto str : list)
        {
            if (str.isEmpty() || !str.contains("-"))
            {
                continue;
            }
            QStringList sublist = str.split(";");
            for (auto s : sublist)
            {
                if (s.isEmpty() || !s.contains("-"))
                {
                    continue;
                }
                languages << s.trimmed();
            }
        }
    }
    return languages;
}

bool UrlSchemeHandler::readFile(const QString& filename, QByteArray& bytes)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        return false;
    }
    bytes = f.readAll();
    f.close();
    return true;
}

QHash<QString, QString> UrlSchemeHandler::queryArguments(const QString& query)
{
    QHash<QString, QString> args;
    QStringList queryList = query.split("&");

    for (auto & query : queryList)
    {
        QString key, value;
        QStringList list = query.split("=");
        if (list.length() < 1)
        {
            continue;
        }
        else if (list.length() >= 1)
        {
            key = list[0].trimmed();
            if (key.isEmpty())
            {
                continue;
            }
            for (int i = 1; i < list.length(); i++)
            {
                value += list[i];
            }
        }
        args.insert(key, value);
    }
    return args;
}

QString UrlSchemeHandler::getArgument(const QHash<QString, QString>& args, const QString& key, const QString& defaultValue)
{
    if (args.contains(key))
    {
        return args.value(key);
    }
    return defaultValue;
}
