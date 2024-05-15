#ifndef BOOKMARKTREEWIDGET_H
#define BOOKMARKTREEWIDGET_H

#include <QTreeWidget>
#include "bookmarkmanager.h"

class QTreeWidgetItem;

struct TreeWidgetDragData
{
    TreeWidgetDragData();

    QTreeWidgetItem* fromItem;
    QTreeWidgetItem* toItem;
    QTreeWidgetItem* fromParent;
    QTreeWidgetItem* toParent;
};

class BookmarkTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    BookmarkTreeWidget();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void sig_drag(const TreeWidgetDragData& data);
    void sig_drop(const TreeWidgetDragData& data);


private:
    void clearBackgroundItems();
    void clearDragFromItems();
    void clearDropToItems();

private:
    QList<QTreeWidgetItem*> dragFromItems;
    QList<QTreeWidgetItem*> dropToItems;
};

#endif // BOOKMARKTREEWIDGET_H
