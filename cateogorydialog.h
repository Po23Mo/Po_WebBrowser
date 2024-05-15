#ifndef CATEOGORYDIALOG_H
#define CATEOGORYDIALOG_H

#include <QDialog>
#include "dialog.h"

class QLineEdit;



class CateogoryDialog : public Dialog
{
    Q_OBJECT
public:
    CateogoryDialog(QWidget* p = nullptr);

    void set_category_name(const QString& name);
    QString get_category_name() const;

private:
    QString category_name;
    QLineEdit* category_name_edit;
};

#endif // CATEOGORYDIALOG_H
