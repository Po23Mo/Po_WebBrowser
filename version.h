#ifndef VERSION_H
#define VERSION_H

#include <QString>

class Version
{
public:
    static QString productName();
    static QString scheme();
    static QString fullVersion();
    static QString qtVersion();
    static QString chromiumVersion();
    static QString osVersion();
};

#endif // VERSION_H
