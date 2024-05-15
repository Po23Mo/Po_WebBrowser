#ifndef WEBENGINEURLSCHEMEHANDLER_H
#define WEBENGINEURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QHash>

class QWebEngineProfile;


class UrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    UrlSchemeHandler(QObject* parent = nullptr);

    static void initialize();
    static void initialize(QWebEngineProfile* profile);
    static void initializeContentTypes();
    static QByteArray getContentTypeByFilename(QByteArray filename);

protected:
    void requestStarted(QWebEngineUrlRequestJob *request) override;

private:
    bool handleUrl(QWebEngineUrlRequestJob *request);
    bool handle(QWebEngineUrlRequestJob *request);
    bool readFile(const QString& filename, QByteArray& bytes);

private:
    static QHash<QString, QString> queryArguments(const QString& query);
    static QString getArgument(const QHash<QString, QString>& args,
                               const QString& key,
                               const QString& defaultValue = QString());
    static QStringList getLanguagesFromHeaders(QWebEngineUrlRequestJob *request);

private:
    static QHash<QByteArray, QByteArray> contentTypes;
};

#endif // WEBENGINEURLSCHEMEHANDLER_H
