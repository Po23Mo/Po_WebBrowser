#ifndef URLCOMPLETERMODEL_H
#define URLCOMPLETERMODEL_H

#include <QAbstractTableModel>
#include "bookmarkmanager.h"
#include <QList>

class UrlCompleterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UrlCompleterModel(QObject *parent = nullptr);

    void load();
    void removeAll();
    void append(const QSharedPointer<Bookmark>& bookmark);

    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
                                            int role = Qt::DisplayRole) const;

protected:
    void load(const QSharedPointer<Bookmark>& bm);
    void appendWithoutSignals(const QSharedPointer<Bookmark>& bookmark);

private:
    QList<QSharedPointer<Bookmark>> bookmarks;
};

#endif // URLCOMPLETERMODEL_H
