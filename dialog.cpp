#include "dialog.h"
#include "securityutils.h"

Dialog::Dialog(QWidget* parent)
    : QDialog(parent)
{

    SecurityUtils::initialize(this);

}

void Dialog::closeEvent(QCloseEvent* e)
{
    emit sig_closed();
    QDialog::closeEvent(e);
}
