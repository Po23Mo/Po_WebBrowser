#include <QApplication>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineProfile>
#include <QIcon>
#include <QScreen>
#include "powidget.h"
#include <QWebEngineProfile>
#include <QSharedPointer>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
#include "profile.h"
#include "version.h"
#include "urlschemehandler.h"


static int S__REMOTE_DEBUG_PORT = 10000;

int main(int argc, char *argv[])
{
    QRandomGenerator rg((quint32)time(nullptr));
    S__REMOTE_DEBUG_PORT = 10000 + rg.generate() % 40000;
    QString debug_str = QString("127.0.0.1:%1").arg(S__REMOTE_DEBUG_PORT);
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", debug_str.toLocal8Bit());
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging");

    UrlSchemeHandler::initialize();

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/PoWebBrowser.png"));
    a.setApplicationDisplayName(Version::fullVersion());

    QSharedPointer<Profile> profile(new Profile(&a));

    QScreen* screen = a.primaryScreen();
    PoWidget w(profile);
    w.resize(screen->availableSize());
    w.show();
    return a.exec();
}
