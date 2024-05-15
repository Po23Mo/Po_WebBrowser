#include "securityutils.h"

#if defined(Q_OS_WIN) &&  !defined(_DEBUG)

struct security_arguments;
extern "C" void butianyun_security_initialize(QWidget* widget, security_arguments* args);

#else
extern "C" void security_initialize(QWidget* widget, void* args)
{
}

#endif

void SecurityUtils::initialize(QWidget* widget)
{

}
