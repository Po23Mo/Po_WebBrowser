#ifndef PROFILE_H
#define PROFILE_H

#include <QWebEngineProfile>

class Profile : public QWebEngineProfile
{
    Q_OBJECT
public:
    explicit Profile(QObject *parent = nullptr);
    void setHttpUserAgent(int useragent);

protected:
    void initializeSecurityProfile();
    void initializeNotification();
};

#endif // PROFILE_H
