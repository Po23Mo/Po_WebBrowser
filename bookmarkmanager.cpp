#include "bookmarkmanager.h"
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "version.h"

Bookmark::Bookmark()
    : id(-1)
{

}

Bookmark::Bookmark(QString vtitle, QString vurl)
    : id(-1)
    , title(vtitle)
    , url(vurl)
{
}

bool Bookmark::compare(const Bookmark& other) const
{
    return (other.id != -1 && id == other.id &&
            title == other.title && other.url.isEmpty() && url.isEmpty())
           || (!other.url.isEmpty() && !url.isEmpty()
               && url.compare(other.url) == 0);
}

bool Bookmark::isBookmarked() const
{
    return id != -1 && !url.isEmpty();
}

QJsonObject _bookmark_to_json(const QSharedPointer<const Bookmark>& bookmark)
{
    QJsonObject obj;
    obj["title"] = bookmark->title;
    obj["url"] = bookmark->url;

    QJsonArray array;
    for (int i = 0; i < bookmark->list.length(); i++)
    {
        QJsonObject bk = _bookmark_to_json(bookmark->list[i]);
        array.append(QJsonValue(bk));
    }
    if (array.size() > 0)
    {
        obj["bookmarks"] = array;
    }
    return obj;
}

QByteArray Bookmark::toJSON() const
{
    QJsonDocument doc;
    doc.setObject(_bookmark_to_json(sharedFromThis()));
    return doc.toJson();
}

BookmarkManager::BookmarkManager(QObject *parent)
    : QObject{parent}
    , nullBookmark(new Bookmark())
{
    QElapsedTimer et;
    et.start();
    id = et.elapsed();
    rootBookmark.reset(new Bookmark());
    rootBookmark->title = Version::fullVersion();
}

void BookmarkManager::add(const QSharedPointer<Bookmark>& bookmark)
{
    QSharedPointer<Bookmark> tempBookmark = bookmark;
    QSharedPointer<Bookmark> parentBookmark = bookmark->parent;

    remove(bookmark);
    QSharedPointer<Bookmark> bk = bookmark;
    bk->id = ++id;

    QSharedPointer<Bookmark> parent = parentBookmark ? parentBookmark : rootBookmark;
    bookmark->parent = parent;
    parent->list << bk;
    save();
    emit sig_update();
}

bool BookmarkManager::remove(const QSharedPointer<Bookmark>& bookmark)
{
    QSharedPointer<Bookmark> temp = bookmark;
    if (bookmark->parent)
    {
        bookmark->parent->list.removeOne(bookmark);
        bookmark->parent.clear();
        save();
        emit sig_update();
        return true;
    }

    if (remove(rootBookmark, bookmark))
    {
        save();
        emit sig_update();
        return true;
    }
    return false;
}

bool BookmarkManager::remove(const QSharedPointer<Bookmark> bookmark, const QSharedPointer<Bookmark>& removed)
{
    if (bookmark->compare(*removed) && bookmark->parent)
    {
        bookmark->parent->list.removeOne(bookmark);
        bookmark->parent.clear();
        return true;
    }

    for (int i = 0; i < bookmark->list.length(); i++)
    {
        const QSharedPointer<Bookmark>& bk = bookmark->list[i];
        if (remove(bk, removed))
        {
            return true;
        }
    }

    return false;
}

void BookmarkManager::move(const QSharedPointer<Bookmark>& from,
                           const QSharedPointer<Bookmark>& to)
{
    if (!from->parent)
    {
        return;
    }

    QSharedPointer<Bookmark> tempFrom = from;
    int fromIndex = from->parent->list.indexOf(from);
    int toIndex = to->parent->list.indexOf(to);

    QSharedPointer<Bookmark> realTo = to;
    QSharedPointer<Bookmark> realToParent;
    if (to->url.isEmpty())
    {
        realToParent = to;
    }
    else
    {
        realToParent = to->parent;
    }

    if (from->parent->compare(*realToParent))
    {
        realToParent->list.move(fromIndex, toIndex);
    }
    else
    {
        remove(from);
        from->parent = realToParent;
        if (realToParent->list.length() > toIndex)
        {
            realToParent->list.insert(toIndex, from);
        }
        else
        {
            realToParent->list.append(from);
        }
    }

    save();
    emit sig_update();
}

const QSharedPointer<Bookmark>& BookmarkManager::get(const QString& url)
{
    return get(root(), url);
}

const QSharedPointer<Bookmark>& BookmarkManager::get(const QSharedPointer<Bookmark>& bookmark,
                                                     const QString& url)
{
    if (bookmark->url.compare(url) == 0)
    {
        return bookmark;
    }
    for (int i = 0; i < bookmark->list.length(); i++)
    {
        const QSharedPointer<Bookmark>& bk = get(bookmark->list[i], url);
        if (!bk->url.isEmpty())
        {
            return bk;
        }
    }
    return nullBookmark;
}

const QSharedPointer<Bookmark>& BookmarkManager::root()
{
    return rootBookmark;
}

bool BookmarkManager::save(const QString& filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly |QIODevice::Truncate))
    {
        return false;
    }

    QByteArray result = rootBookmark->toJSON();
    if (result.length() != f.write(result))
    {
        f.close();
        return false;
    }

    f.close();
    return true;
}

bool BookmarkManager::load(const QString& filename)
{
    QElapsedTimer et;
    et.start();
    id = et.elapsed();
    rootBookmark->parent.clear();
    rootBookmark->list.clear();
    rootBookmark->url.clear();
    rootBookmark->title.clear();

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray result = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(result);
    if (!doc.isObject())
    {
        return false;
    }

    QJsonObject obj = doc.object();
    bool ret = load(obj, rootBookmark);
    rootBookmark->title =  Version::fullVersion();
    rootBookmark->url.clear();
    return ret;
}

bool BookmarkManager::load(const QJsonObject& obj, QSharedPointer<Bookmark>& bookmark)
{
    bookmark->id = ++id;
    bookmark->title = obj.value("title").toString();
    bookmark->url = obj.value("url").toString();
    QJsonArray array = obj.value("bookmarks").toArray();
    bookmark->list.resize(array.count());
    for (int i = 0; i < bookmark->list.length(); i++)
    {
        bookmark->list[i].reset(new Bookmark());
        bookmark->list[i]->parent = bookmark;
        load(array.at(i).toObject(), bookmark->list[i]);
    }
    return true;
}

bool BookmarkManager::save()
{
    QDir dir(qApp->applicationDirPath());
    QString dirpath = qApp->applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "bookmark";
    dir.mkpath(QStringLiteral("data") + QDir::separator() + "bookmark");
    QString filename = dirpath + QDir::separator() + "bookmark.json";
    return save(filename);
}

bool BookmarkManager::load()
{
    QString dirpath = qApp->applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "bookmark";
    QString filename = dirpath + QDir::separator() + "bookmark.json";
    return load(filename);
}

