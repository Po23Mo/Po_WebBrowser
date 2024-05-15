#ifndef BOOKMARKEDITDIALOG_H
#define BOOKMARKEDITDIALOG_H

#include <QDialog>
#include "bookmarkmanager.h"
#include "dialog.h"


class BookmarkEditDialog : public Dialog
{
    Q_OBJECT
public:
    BookmarkEditDialog(const QSharedPointer<Bookmark>& bk, QWidget* parent = nullptr);

private:
    QSharedPointer<Bookmark> bookmark;
};

#endif // BOOKMARKEDITDIALOG_H
