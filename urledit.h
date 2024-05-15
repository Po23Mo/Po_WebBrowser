#ifndef URLEDIT_H
#define URLEDIT_H

#include <QLineEdit>
#include <QSharedPointer>

class QCompleter;
class QTableView;
struct Bookmark;
class UrlCompleterModel;

class UrlEdit : public QLineEdit
{
    Q_OBJECT
public:
    UrlEdit(QWidget* p = nullptr);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

public slots:
    void updateCompleter();
    void append(const QSharedPointer<Bookmark>& bookmark);

private:
    QCompleter* completer;
    UrlCompleterModel* completerModel;
    QList<QString> bookmarks;
    QList<QString> history;
    QTableView* completerView;
};

#endif // URLEDIT_H
