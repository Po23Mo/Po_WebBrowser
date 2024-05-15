#ifndef WEBPAGEWIDGET_H
#define WEBPAGEWIDGET_H

#include <QWidget>

class QSplitter;
class WebEngineView;
class WebEnginePage;
class QWebEngineProfile;


class WebPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WebPageWidget(QWebEngineProfile* profile, QWidget *parent = nullptr);
    ~WebPageWidget();
    WebEngineView* getView() const;
    WebEnginePage* getPage() const;
    WebEngineView* getViewDevTools() const;
    WebEnginePage* getPageDevTools() const;
    void uninitialize();
    void openDevTools();
    void closeDevTools();
    void toggleFullscreen(bool fullscreen);


protected:
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;

protected:
    void rebuildLayout();

signals:
    void sig_toggleFullscreen(bool fullscreen);

private:
    QSplitter* view_widget;
    WebEngineView* view;
    WebEngineView* view_devtools;
    QList<int> view_sizes;
};

#endif // WEBPAGEWIDGET_H
