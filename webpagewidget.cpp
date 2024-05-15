#include "webpagewidget.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QKeyEvent>
#include "webengineview.h"
#include "webenginepage.h"


WebPageWidget::WebPageWidget(QWebEngineProfile* profile, QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* main_layout = new QVBoxLayout();
    setLayout(main_layout);

    view_widget = new QSplitter(Qt::Horizontal);
    view = new WebEngineView(profile);
    view_devtools = new WebEngineView(profile);
    view_widget->addWidget(view);
    view_widget->addWidget(view_devtools);
    view_widget->setHandleWidth(20);
    main_layout->addWidget(view_widget);

    connect(view, &WebEngineView::sig_toggleFullscreen,
            this, &WebPageWidget::sig_toggleFullscreen);

    connect(view_widget, &QSplitter::splitterMoved, this, [this]{
        auto page = view->page();
        if (!page)
        {
            return;
        }
        if (!page->devToolsPage())
        {
            return;
        }
        view_sizes = view_widget->sizes();
    });
    setFocusPolicy(Qt::StrongFocus);
}

WebPageWidget::~WebPageWidget()
{
    uninitialize();
    auto page = view->getPage();
    auto page_devtools = view_devtools->getPage();
    view->setPage(nullptr);
    view_devtools->setPage(nullptr);
    page->deleteLater();
    page_devtools->deleteLater();
}

void WebPageWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    rebuildLayout();
}

void WebPageWidget::rebuildLayout()
{
    QList<int> sizes;
    if (getPage()->devToolsPage())
    {
        if (view_sizes.isEmpty())
        {
            view_sizes << width() / 2 << width() / 2;
        }
        else
        {
            double factor = view_sizes[0] * 1.0 / (view_sizes[0] + view_sizes[1]);
            sizes << width() * factor << width()  * (1.0 - factor);
        }
        sizes = view_sizes;
    }
    else
    {
        sizes << width() << 0;
    }

    view_devtools->setVisible(getPage()->devToolsPage());
    view_widget->setSizes(sizes);
    view_widget->refresh();
}

WebEngineView* WebPageWidget::getView() const
{
    return view;
}

WebEnginePage* WebPageWidget::getPage() const
{
    return view->getPage();
}

WebEngineView* WebPageWidget::getViewDevTools() const
{
    return view_devtools;
}

WebEnginePage* WebPageWidget::getPageDevTools() const
{
    return view_devtools->getPage();
}

void WebPageWidget::uninitialize()
{
    closeDevTools();
    getPage()->setUrl(QUrl());
}

void WebPageWidget::openDevTools()
{
    getPage()->setDevToolsPage(getPageDevTools());
    rebuildLayout();
}

void WebPageWidget::closeDevTools()
{
    getPage()->setDevToolsPage(nullptr);
    getPageDevTools()->setInspectedPage(nullptr);
    rebuildLayout();
}

void WebPageWidget::keyPressEvent(QKeyEvent* e)
{
    QWidget::keyPressEvent(e);
    view->keyPressEvent(e);
    if (e->key() == Qt::Key_F9)
    {
        toggleFullscreen(true);
    }
    else if (e->key() == Qt::Key_F10)
    {
        toggleFullscreen(false);
    }
}

void WebPageWidget::focusInEvent(QFocusEvent* e)
{
    QWidget::focusInEvent(e);
}

void WebPageWidget::toggleFullscreen(bool fullscreen)
{
    view->setProperty("fullscreen", fullscreen);
    emit sig_toggleFullscreen(fullscreen);
}
