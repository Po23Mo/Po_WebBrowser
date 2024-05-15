#include <QKeyEvent>
#include <QCompleter>
#include <QTableView>
#include <QHeaderView>
#include "urledit.h"
#include "urlcompletermodel.h"

UrlEdit::UrlEdit(QWidget* p)
    : QLineEdit(p)
{
    setMinimumHeight(32);
    completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setMaxVisibleItems(10);

    completerModel = new UrlCompleterModel(this);
    completer->setModel(completerModel);
    completer->setCompletionRole(Qt::DisplayRole);
    completer->setCompletionColumn(1);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    completerView = new QTableView();
    completerView->setWindowFlag(Qt::Popup);
    completerView->setAttribute(Qt::WA_AlwaysStackOnTop);
    completerView->setShowGrid(false);
    completerView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    completerView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    completerView->setMinimumHeight(300);
    completerView->setModel(completerModel);
    completerView->horizontalHeader()->setVisible(false);
    completerView->setColumnWidth(0, 48);
    completerView->setColumnWidth(1, 300);
    completerView->setColumnWidth(2, 300);
    completer->setPopup(completerView);

    setCompleter(completer);
}

void UrlEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
}

void UrlEdit::mouseReleaseEvent(QMouseEvent* e)
{
    QLineEdit::mouseReleaseEvent(e);
}

void UrlEdit::keyReleaseEvent(QKeyEvent* e)
{
    QLineEdit::keyReleaseEvent(e);
}

void UrlEdit::updateCompleter()
{
    completerModel->load();
}

void UrlEdit::append(const QSharedPointer<Bookmark>& bookmark)
{
    completerModel->append(bookmark);
}

