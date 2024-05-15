#include "cateogorydialog.h"
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

CateogoryDialog::CateogoryDialog(QWidget* p)
    : Dialog(p)
{
    QFormLayout* main_layout = new QFormLayout();
    setLayout(main_layout);

    QLabel* category_name_label = new QLabel();
    category_name_label->setText("Category title");
    category_name_edit = new QLineEdit();
    category_name_edit->setMaxLength(100);
    category_name_edit->setText(category_name);
    main_layout->addRow(category_name_label, category_name_edit);

    QPushButton* ok_button = new QPushButton();
    ok_button->setText("OK");
    QPushButton* cancel_button = new QPushButton();
    cancel_button->setText("Cancel");
    main_layout->addRow(ok_button, cancel_button);

    resize(800, 600);

    connect(category_name_edit, &QLineEdit::editingFinished, this, [this]{
        category_name = category_name_edit->text();
    });

    connect(ok_button, &QPushButton::clicked, this, [this]{
        accept();
    });

    connect(cancel_button, &QPushButton::clicked, this, [this]{
        reject();
    });
}

QString CateogoryDialog::get_category_name( ) const
{
    return category_name;
}

void CateogoryDialog::set_category_name(const QString& name)
{
    category_name = name;
    category_name_edit->setText(name);
}
