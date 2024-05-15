#include "bookmarkwidget.h"
#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDesktopServices>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include "bookmarkmanager.h"
#include "bookmarktreewidget.h"
#include "cateogorydialog.h"
#include "bookmarkeditdialog.h"

enum BookmarkWidgetColumn
{
    _BOOKMARK_ICON,
    _BOOKMARK_TITLE,
    _BOOKMARK_URL,

    _BOOKMARK_COLUMN_COUNT
};

BookmarkWidget::BookmarkWidget(
    BookmarkManager* manager, QWidget *parent)
    : QWidget{parent}
    , bookmark_manager(manager)
{
    QVBoxLayout* main_layout = new QVBoxLayout();
    setLayout(main_layout);
    setWindowTitle("Bookmark Manager");

    tree = new BookmarkTreeWidget();
    tree->setColumnCount(_BOOKMARK_COLUMN_COUNT);
    tree->setColumnWidth(_BOOKMARK_ICON, 100);
    tree->setColumnWidth(_BOOKMARK_TITLE, 300);
    tree->setColumnWidth(_BOOKMARK_URL, 150);

    QStringList headers;
    headers << "Icon" << "Title" << "URL";
    tree->setHeaderLabels(headers);
    main_layout->addWidget(tree);

    connect(bookmark_manager, &BookmarkManager::sig_update, this, [this]{
            load();
        }, Qt::QueuedConnection);

    connect(tree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column) {
        QString url = item->data(0, Qt::UserRole + 1).value<Bookmark>().url;
        if (url.isEmpty())
        {
            return;
        }
        emit sig_reload_url(url);
    });

    QMenu* popup_menu = new QMenu(this);

    QAction* reload_action = new QAction(popup_menu);
    reload_action->setText("Reload web page with this URL");
    popup_menu->addAction(reload_action);

    QAction* copy_url_action = new QAction(popup_menu);
    copy_url_action->setText("Copy URL to clipboard");
    popup_menu->addAction(copy_url_action);

    QAction* copy_title_action = new QAction(popup_menu);
    copy_title_action->setText("Copy title to clipboard");
    popup_menu->addAction(copy_title_action);

    QAction* remove_action = new QAction(popup_menu);
    remove_action->setText("Remove this bookmark");
    popup_menu->addAction(remove_action);

    QAction* create_category_action = new QAction(popup_menu);
    create_category_action->setText("Create new category");
    popup_menu->addAction(create_category_action);

    QAction* edit_bookmark_action = new QAction(popup_menu);
    edit_bookmark_action->setText("Edit this bookmark");
    popup_menu->addAction(edit_bookmark_action);

    QList<QAction*> actions = tree->actions();
    actions << reload_action << copy_url_action << copy_title_action << remove_action
            << create_category_action << edit_bookmark_action;
    tree->insertActions(nullptr, actions);

    tree->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    connect(copy_url_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }
        QString url = item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>()->url;
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(url);
        }
    });

    connect(copy_title_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }
        QString title = item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>()->title;
        QClipboard* clip = QGuiApplication::clipboard();
        if (clip)
        {
            clip->setText(title);
        }
    });

    connect(reload_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }
        QString url = item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>()->url;
        if (url.isEmpty())
        {
            return;
        }
        emit sig_reload_url(url);
    });

    connect(remove_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }
        QSharedPointer<Bookmark> bookmark = item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
        if (bookmark->list.length() > 0)
        {
            if (QMessageBox::question(this, "Are  you sure?",
                                      QString("Are you sure you want to delete multiple bookmarks?\r\nThere are %1 bookmarks.")
                                          .arg(bookmark->list.length()),
                                      QMessageBox::Yes |QMessageBox::No) == QMessageBox::No)
            {
                return;
            }
        }

        bookmark_manager->remove(bookmark);
    });

    connect(create_category_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }

        CateogoryDialog dlg(this);
        if (QDialog::Accepted != dlg.exec() || dlg.get_category_name().isEmpty())
        {
            return;
        }

        QSharedPointer<Bookmark> bookmark =
            item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
        QSharedPointer<Bookmark> category(new Bookmark());
        category->parent = bookmark->parent;
        category->title = dlg.get_category_name();
        bookmark_manager->add(category);
    });

    connect(edit_bookmark_action, &QAction::triggered, this, [this]{
        QTreeWidgetItem* item = tree->currentItem();
        if (nullptr == item)
        {
            return;
        }
        QSharedPointer<Bookmark> bookmark =
            item->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
        BookmarkEditDialog dlg(bookmark, this);
        if (QDialog::Accepted != dlg.exec())
        {
            return;
        }
        load();
    });


    connect(tree, &BookmarkTreeWidget::sig_drop,
            this, &BookmarkWidget::slot_drop);

    load();
}

void BookmarkWidget::load()
{
    int count = tree->topLevelItemCount();
    int i;
    for (i = count; i > 0; i--)
    {
        auto item = tree->takeTopLevelItem(i - 1);
        delete item;
    }

    const auto& rootBookmark = bookmark_manager->root();
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    tree->addTopLevelItem(rootItem);
    rootItem->setFlags(Qt::ItemIsEnabled);
    load(rootBookmark, rootItem);
    tree->expandToDepth(1);
}

void BookmarkWidget::load(
    const QSharedPointer<Bookmark>& bookmark,
    QTreeWidgetItem* item)
{
    item->setText(_BOOKMARK_TITLE, bookmark->title);
    item->setText(_BOOKMARK_URL, bookmark->url);
    item->setData(0, Qt::UserRole + 1,  QVariant::fromValue(bookmark));
    if (bookmark->list.length() > 0)
    {
        item->setFlags(item->flags() | Qt::ItemIsDropEnabled);
    }
    else
    {
        item->setFlags(item->flags() & (~Qt::ItemIsDropEnabled));
    }

    for (int i = 0; i < bookmark->list.length(); i++)
    {
        QTreeWidgetItem* newitem = new QTreeWidgetItem();
        item->addChild(newitem);
        load(bookmark->list[i], newitem);
    }
}

void BookmarkWidget::slot_drag(const TreeWidgetDragData& data)
{

}

void BookmarkWidget::slot_drop(const TreeWidgetDragData& data)
{
    QSharedPointer<Bookmark> from =
        data.fromItem->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
    QSharedPointer<Bookmark> to =
        data.toItem->data(0, Qt::UserRole + 1).value<QSharedPointer<Bookmark>>();
    bookmark_manager->move(from, to);
}
