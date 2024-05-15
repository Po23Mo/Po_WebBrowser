#include "urlcompletermodel.h"
#include "bookmarkmanager.h"
#include <QDebug>

static int _bookmark_compare(const QSharedPointer<Bookmark>& a, const QSharedPointer<Bookmark>& b)
{
    return a->url.compare(b->url, Qt::CaseInsensitive);
}

UrlCompleterModel::UrlCompleterModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    load();
}


void UrlCompleterModel::load(const QSharedPointer<Bookmark>& bm)
{
    if (!bm->url.isEmpty())
    {
        append(bm);
    }
    for (auto & bookmark : bm->list)
    {
        load(bookmark);
    }
}

void UrlCompleterModel::load()
{
    beginResetModel();
    bookmarks.clear();
    BookmarkManager bm;
    bm.load();
    load(bm.root());
    endResetModel();
}

void UrlCompleterModel::removeAll()
{
    beginResetModel();
    bookmarks.clear();
    endResetModel();
}

void UrlCompleterModel::append(const QSharedPointer<Bookmark>& bookmark)
{
    beginResetModel();
    appendWithoutSignals(bookmark);
    endResetModel();
}

void UrlCompleterModel::appendWithoutSignals(const QSharedPointer<Bookmark>& bookmark)
{
    int i = 0;
    for (; i < bookmarks.length(); i++)
    {
        int ret = _bookmark_compare(bookmark, bookmarks[i]);
        if (0 == ret)
        {
            if (!bookmark->title.isEmpty())
            {
                bookmarks[i] = bookmark;
                return;
            }
        }
        else if (ret < 0)
        {
            break;
        }
    }
    bookmarks.insert(i, bookmark);
}

int UrlCompleterModel::rowCount(const QModelIndex &parent) const
{
    return bookmarks.size();
}

int UrlCompleterModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant UrlCompleterModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= bookmarks.size()
        || index.column() < 0 || index.column() > 2
        || (role != Qt::EditRole && role != Qt::DisplayRole))
    {
        return QVariant();
    }

    auto& bookmark = bookmarks[index.row()];
    if (index.column() == 0)
    {
        return bookmark->icon;
    }
    else if (index.column() == 1)
    {
        return bookmark->url;
    }
    else
    {
        return bookmark->title;
    }
}

QVariant UrlCompleterModel::headerData(int section, Qt::Orientation orientation,
                                                int role) const
{

    if (section < 0 || section > 1
        || orientation != Qt::Horizontal
        || (role != Qt::EditRole && role != Qt::DisplayRole))
    {
        return QVariant();
    }
    if (0 == section)
    {
        return "Icon";
    }
    else if (1 == section)
    {
        return "URL";
    }
    else
    {
        return "Title";
    }
}
