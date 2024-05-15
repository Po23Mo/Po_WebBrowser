#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(QWidget* parent = nullptr);

signals:
    void sig_closed();

protected:
    void closeEvent(QCloseEvent* e) override;
};

#endif // DIALOG_H
