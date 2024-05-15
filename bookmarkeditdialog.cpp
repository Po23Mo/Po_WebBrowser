#include "bookmarkeditdialog.h"
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

BookmarkEditDialog::BookmarkEditDialog(const QSharedPointer<Bookmark>& bk, QWidget* parent)
    : Dialog(parent)
    , bookmark(bk)
{
    QFormLayout* main_layout = new QFormLayout();
    setLayout(main_layout);

    QLabel* title_label = new QLabel();
    title_label->setText("Category title");

    QLineEdit* title_edit = new QLineEdit();
    title_edit->setMaxLength(100);
    title_edit->setText(bk->title);
    main_layout->addRow(title_label, title_edit);

    QLabel* url_label = new QLabel();
    url_label->setText("Category url");

    QLineEdit* url_edit = new QLineEdit();
    url_edit->setMaxLength(100);
    url_edit->setText(bk->url);
    main_layout->addRow(url_label, url_edit);

    QLabel* status_label = new QLabel();
    main_layout->addRow(status_label);

    QPushButton* ok_button = new QPushButton();
    ok_button->setText("OK");
    QPushButton* cancel_button = new QPushButton();
    cancel_button->setText("Cancel");
    main_layout->addRow(ok_button, cancel_button);

    resize(800, 600);

    connect(ok_button, &QPushButton::clicked, this, [this, title_edit, url_edit, status_label]{
        QString title = title_edit->text().trimmed();
        QString url = url_edit->text().trimmed();
        if (bookmark->list.length() == 0 && url.isEmpty())
        {
            status_label->setText("URL of this bookmark should not be empty!");
            return;
        }
        if (title.isEmpty())
        {
            status_label->setText("Title of this bookmark should not be empty!");
            return;
        }
        bookmark->title = title;
        bookmark->url = url;
        accept();
    });

    connect(cancel_button, &QPushButton::clicked, this, [this]{
        reject();
    });
}
