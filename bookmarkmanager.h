#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QIcon>

struct Bookmark : public QEnableSharedFromThis<Bookmark>
{
    Bookmark();
    Bookmark(QString vtitle, QString vurl);

    bool compare(const Bookmark& other) const;
    bool isBookmarked() const;
    QByteArray toJSON() const;

    qint64 id;
    QString title;
    QString url;
    QIcon icon;
    QSharedPointer<Bookmark> parent;
    QList<QSharedPointer<Bookmark>> list;
};


class BookmarkManager : public QObject
{
    Q_OBJECT
public:
    explicit BookmarkManager(QObject *parent = nullptr);
    void add(const QSharedPointer<Bookmark>& bookmark);
    bool remove(const QSharedPointer<Bookmark>& bookmark);
    void move(const QSharedPointer<Bookmark>& from,
              const QSharedPointer<Bookmark>& to);
    const QSharedPointer<Bookmark>& get(const QString& url);

    const QSharedPointer<Bookmark>& root();
    bool save(const QString& filename);
    bool load(const QString& filename);
    bool save();
    bool load();

private:
    bool load(const QJsonObject& obj, QSharedPointer<Bookmark>& bookmark);
    bool remove(const QSharedPointer<Bookmark> bookmark, const QSharedPointer<Bookmark>& removed);
    const QSharedPointer<Bookmark>& get(const QSharedPointer<Bookmark>& bookmark, const QString& url);

signals:
    void sig_update();

private:
    QSharedPointer<Bookmark> rootBookmark;
    const QSharedPointer<Bookmark> nullBookmark;
    qint64 id;
};


#endif // BOOKMARKMANAGER_H
