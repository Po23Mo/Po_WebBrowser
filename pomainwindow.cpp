#include "Pomainwindow.h"
#include "securityutils.h"

PoMainWindow::PoMainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    SecurityUtils::initialize(this);
}
