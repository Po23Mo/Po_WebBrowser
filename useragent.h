#ifndef USERAGENT_H
#define USERAGENT_H

enum UserAgent
{
    UserAgent_PoWebBrowser,
    UserAgent_Windows_Firefox,
    UserAgent_Windows_Chrome,
    UserAgent_Windows_Edge,
    UserAgent_Windows_IE,
    UserAgent_Linux_Firefox,
    UserAgent_Linux_Chrome,
    UserAgent_Mac_Safari,
    UserAgent_Android_Chrome,
    UserAgent_Android_UC,
    UserAgent_IOS_iPad_Safari,
    UserAgent_IOS_iPhone_Safari
};

struct UserAgentItem
{
    UserAgent id;
    const char* name;
    const char* ua;
};

int get_useragent_count();
const UserAgentItem get_useragent(int index);

#endif // USERAGENT_H
