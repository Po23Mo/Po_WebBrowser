#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QWidget>
#include <QSharedPointer>

struct Bookmark;
class BookmarkManager;
class QTreeWidgetItem;
class BookmarkTreeWidget;
struct TreeWidgetDragData;

class BookmarkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BookmarkWidget(BookmarkManager* manager, QWidget *parent = nullptr);
    void load();

public slots:
    void slot_drag(const TreeWidgetDragData& data);
    void slot_drop(const TreeWidgetDragData& data);

signals:
    void sig_reload_url(const QString& url);

private:
    void load(const QSharedPointer<Bookmark>& bookmark, QTreeWidgetItem* item);

private:
    BookmarkTreeWidget* tree;
    BookmarkManager* bookmark_manager;
};

#endif // BOOKMARKWIDGET_H
