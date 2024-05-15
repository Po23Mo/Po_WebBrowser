#include "bookmarktreewidget.h"
#include <QMimeData>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDrag>
#include <QDragLeaveEvent>
#include "bookmarkmanager.h"

TreeWidgetDragData::TreeWidgetDragData()
    : fromItem(nullptr)
    , toItem(nullptr)
    , fromParent(nullptr)
    , toParent(nullptr)
{

}

BookmarkTreeWidget::BookmarkTreeWidget()
{
    setDefaultDropAction(Qt::MoveAction);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragDropOverwriteMode(false);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setUniformRowHeights(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setTextElideMode(Qt::ElideMiddle);
}

void BookmarkTreeWidget::clearBackgroundItems()
{
    clearDragFromItems();
    clearDropToItems();
}

void BookmarkTreeWidget::clearDragFromItems()
{
    for (auto it : dragFromItems)
    {
        for (int i = 0; i < columnCount(); i++)
        {
            it->setBackground(i, QBrush(QColor(255, 255, 255)));
        }
    }
    dragFromItems.clear();
}

void BookmarkTreeWidget::clearDropToItems()
{
    for (auto it : dropToItems)
    {
        for (int i = 0; i < columnCount(); i++)
        {
            it->setBackground(i, QBrush(QColor(255, 255, 255)));
        }
    }
    dropToItems.clear();
}

void BookmarkTreeWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton)
    {
        QTreeWidget::mousePressEvent(e);
        return;
    }

    clearBackgroundItems();

    e->accept();

    QTreeWidgetItem* item = itemAt(e->position().toPoint());
    if (nullptr == item)
    {
        return;
    }

    int column = columnAt(e->position().toPoint().x());
    if (0 == column)
    {
        if (item->isExpanded())
        {
            collapseItem(item);
        }
        else
        {
            expandItem(item);
        }
        return;
    }


    if (!item->flags().testAnyFlag(Qt::ItemIsDragEnabled))
    {
        return;
    }

    auto bk = item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
    qInfo() << "drag: item:{" << bk->title << "}";

    QMimeData* md = new QMimeData();
    TreeWidgetDragData data;
    data.fromItem = item;
    data.fromParent = item->parent();
    md->setProperty("dragdata", QVariant::fromValue(data));

    QDrag* drag = new QDrag(this);
    drag->setMimeData(md);
    drag->setPixmap(QPixmap(":/images/PoWebBrowser.png").scaled(32, 32));
    emit sig_drag(data);

    for (int i = 0; i < columnCount(); i++)
    {
        item->setBackground(i, QBrush(QColor(255, 255, 0)));
    }

    dragFromItems << item;
    drag->exec(Qt::DropAction::MoveAction, Qt::DropAction::MoveAction);
}

void BookmarkTreeWidget::dragEnterEvent(QDragEnterEvent *e)
{
    QTreeWidget::dragEnterEvent(e);

    if (!e->buttons().testAnyFlag(Qt::LeftButton))
    {
        e->ignore();
        return;
    }

    QTreeWidgetItem* item = itemAt(e->position().toPoint());
    if (nullptr == item)
    {
        e->ignore();
        return;
    }

    QSharedPointer<Bookmark> bookmark =
        item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
    if (bookmark->url.isEmpty() && item->childCount() > 0)
    {
        expandItem(item);
        e->ignore();
        return;
    }

    e->acceptProposedAction();
}

void BookmarkTreeWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    QTreeWidget::dragLeaveEvent(e);
    clearBackgroundItems();
}

void BookmarkTreeWidget::dragMoveEvent(QDragMoveEvent *e)
{
    clearDropToItems();
    QTreeWidget::dragMoveEvent(e);
    QTreeWidgetItem* item = itemAt(e->position().toPoint());
    if (nullptr == item)
    {
        e->ignore();
        return;
    }

    bool ok = true;
    QSharedPointer<Bookmark> bookmark =
        item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
    if (bookmark->url.isEmpty() && item->childCount() > 0)
    {
        ok = false;
    }

    if (dragFromItems.contains(item))
    {
        ok = false;
    }

    if (!item->parent())
    {
        ok = false;
    }

    QColor color;
    if (ok)
    {
        color = QColor(0, 255, 0);
        e->acceptProposedAction();
    }
    else
    {
        color = QColor(255, 0, 0);
        e->ignore();
    }

    for (int i = 0; i < columnCount(); i++)
    {
        item->setBackground(i, QBrush(color));
    }
    dropToItems << item;


}

void BookmarkTreeWidget::dropEvent(QDropEvent *e)
{
    clearBackgroundItems();

    QTreeWidgetItem* item = itemAt(e->position().toPoint());
    if (nullptr == item)
    {
        e->ignore();
        return;
    }

    TreeWidgetDragData data =
        e->mimeData()->property("dragdata").value<TreeWidgetDragData>();
    if (!data.fromItem)
    {
        e->ignore();
        return;
    }

    if (!data.fromItem->parent())
    {
        e->ignore();
        return;
    }

    if (!item->parent())
    {
        e->ignore();
        return;
    }

    e->acceptProposedAction();
    QTreeWidget::dropEvent(e);

    data.toItem = item;
    data.toParent = item->parent();
    emit sig_drop(data);
}
