#include "useragent.h"

const UserAgentItem C_WEBBROWSER_USERAGENTS[] = {
    {
        UserAgent_PoWebBrowser,
        "PoWebBrowser",
        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
    }
};


int get_useragent_count()
{
    return sizeof(C_WEBBROWSER_USERAGENTS) / sizeof(C_WEBBROWSER_USERAGENTS[0]);
}

const UserAgentItem get_useragent(int index)
{
    return C_WEBBROWSER_USERAGENTS[index];
}
