/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <cassert>
#include "tabwidget.h"

#include "libraries/qtSingleApplication5/qtsingleapplication.h"

#include "browser.h"
#include "downloadmanager.h"
#include "fullscreennotification.h"
#include "history.h"
#include "urllineedit.h"
#include "webview.h"

#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtWebEngineWidgets/QWebEngineDownloadItem>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolButton>
#include <QException>


#include "views/recordTable/RecordTableScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "views/recordTable/RecordTableView.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "toolbarsearch.h"
#include "entrance.h"
#include "views/mainWindow/MainWindow.h"
#include "main.h"

extern GlobalParameters globalparameters;
extern QMap<Qt::CheckState, QString> _check_state;
extern QMap<QString, Qt::CheckState> _state_check;
class QException;

#include <QtCore/QDebug>


namespace browser {


    TabBar::TabBar(QWidget *parent)
        : QTabBar(parent)
    {
        setContextMenuPolicy(Qt::CustomContextMenu);
        setAcceptDrops(true);
        connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));

        QString ctrl = QLatin1String("Ctrl+%1");

        for(int i = 1; i < 10; ++i) {
            QShortcut *shortCut = new QShortcut(ctrl.arg(i), this);
            _tabshortcuts.append(shortCut);
            connect(shortCut, SIGNAL(activated()), this, SLOT(selectTabAction()));
        }

        setTabsClosable(true);
        connect(this, SIGNAL(tabCloseRequested(int)), this, SIGNAL(closeTab(int)));
        setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
        setMovable(true);
    }

    void TabBar::selectTabAction()
    {
        if(QShortcut *shortCut = qobject_cast<QShortcut *>(sender())) {
            int index = _tabshortcuts.indexOf(shortCut);
            setCurrentIndex(index);
        }
    }

    void TabBar::contextMenuRequested(const QPoint &position)
    {
        QMenu menu;
        menu.addAction(tr("New &Tab"), this, &TabBar::newTab, QKeySequence::AddTab);
        int index = tabAt(position);

        if(-1 != index) {
            QAction *action = menu.addAction(tr("Clone Tab"),
                                             this, SLOT(cloneTab()));
            action->setData(index);

            menu.addSeparator();

            action = menu.addAction(tr("&Close Tab"), this, SLOT(closeTab()), QKeySequence::Close);
            action->setData(index);

            action = menu.addAction(tr("Close &Other Tabs"), this, SLOT(closeOtherTabs()));
            action->setData(index);

            menu.addSeparator();

            action = menu.addAction(tr("Reload Tab"), this, SLOT(reloadTab()), QKeySequence::Refresh);
            action->setData(index);
        } else {
            menu.addSeparator();
        }

        menu.addAction(tr("Reload All Tabs"), this, SIGNAL(reloadAllTabs()));
        menu.exec(QCursor::pos());
    }

    void TabBar::cloneTab()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit cloneTab(index);
        }
    }

    void TabBar::closeTab()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit closeTab(index);
        }
    }

    void TabBar::closeOtherTabs()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit closeOtherTabs(index);
        }
    }

    void TabBar::mousePressEvent(QMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton)
            _dragstartpos = event->pos();

        QTabBar::mousePressEvent(event);
    }

    void TabBar::mouseMoveEvent(QMouseEvent *event)
    {
        if(event->buttons() == Qt::LeftButton) {
            int diffX = event->pos().x() - _dragstartpos.x();
            int diffY = event->pos().y() - _dragstartpos.y();

            if((event->pos() - _dragstartpos).manhattanLength() > QApplication::startDragDistance()
               && diffX < 3 && diffX > -3
               && diffY < -10) {
                QDrag *drag = new QDrag(this);
                QMimeData *mimeData = new QMimeData;
                QList<QUrl> urls;
                int index = tabAt(event->pos());
                QUrl url = tabData(index).toUrl();
                urls.append(url);
                mimeData->setUrls(urls);
                mimeData->setText(tabText(index));
                mimeData->setData(QLatin1String("action"), "tab-reordering");
                drag->setMimeData(mimeData);
                drag->exec();
            }
        }

        QTabBar::mouseMoveEvent(event);
    }

    // When index is -1 index chooses the current tab
    void TabWidget::reloadTab(int index)
    {
        if(index < 0)
            index = currentIndex();

        if(index < 0 || index >= count())
            return;

        QWidget *widget = this->widget(index);

        if(WebView *tab = qobject_cast<WebView *>(widget))
            tab->reload();
    }

    void TabBar::reloadTab()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit reloadTab(index);
        }
    }

    TabWidget::~TabWidget()
    {
        //        delete _profile;
        delete _tabbar;
        delete _lineedits;
        delete _lineeditcompleter;
        // delete _completionModel;
        delete _recentlyclosedtabsmenu;
        delete _previoustabaction;
        delete _nexttabaction;
        delete _closetabaction;
        delete _newtabaction;
        delete _recentlyclosedtabsaction;
        delete _fullscreennotification;
        delete _fullscreenview;

    }

    TabWidget::TabWidget(RecordTableController *recordtablecontroller, Browser *parent)
        : QTabWidget(parent)
        , _recentlyclosedtabsaction(new QAction(tr("Recently Closed Tabs"), this))
        , _newtabaction(new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this))
        , _closetabaction(new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this))
        , _nexttabaction(new QAction(tr("Show Next Tab"), this))
        , _previoustabaction(new QAction(tr("Show Previous Tab"), this))
        , _recentlyclosedtabsmenu(new QMenu(this))
          // , _completionModel(new HistoryCompletionModel(this))
        , _lineeditcompleter(
              nullptr   // new QCompleter(_completionModel, this)
          )
          //, _lineedits(new QStackedWidget(this))
        , _tabbar(new TabBar(this))
        , _profile(QWebEngineProfile::defaultProfile())
        , _fullscreenview(0)
        , _fullscreennotification(0)
        , _recordtablecontroller(recordtablecontroller)
          //        , _active_record(this)
          //        , _active("", &active_record::operator(), &_active_record)
        , _window(parent)
    {
        _lineedits = globalparameters.getFindScreen()->toolbarsearch()->lineedits();

        setElideMode(Qt::ElideRight);
        //        _active_record = [this](Record * const record)-> WebView * {return globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);};
        connect(
            _tabbar
            , &TabBar::newTab   //                , this
        , [this]() {
            auto arint = boost::make_shared<NewTab>(this, true);
            request_record(
                QUrl(Browser::_defaulthome)

                //            [this](Record * const record)-> WebView * {
                //                return globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);
                //            }   // nested    //
                //                [this](Record * const record)-> WebView * {return this->newTab(record);}
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                    "", &NewTab::generator, arint)  //                _active
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                    "", &NewTab::activator, arint)
            );
        });
        connect(_tabbar, SIGNAL(closeTab(int)), this, SLOT(closeTab(int)));
        connect(this, &TabWidget::tabsChanged, this, &TabWidget::onTabsChanged);
        connect(_tabbar, SIGNAL(cloneTab(int)), this, SLOT(cloneTab(int)));
        connect(_tabbar, SIGNAL(closeOtherTabs(int)), this, SLOT(closeOtherTabs(int)));
        connect(_tabbar, SIGNAL(reloadTab(int)), this, SLOT(reloadTab(int)));
        connect(_tabbar, SIGNAL(reloadAllTabs()), this, SLOT(reloadAllTabs()));
        connect(_tabbar, SIGNAL(tabMoved(int, int)), this, SLOT(moveTab(int, int)));
        setTabBar(_tabbar);
        setDocumentMode(true);
        _tabbar->hide();
        // Actions
        //        _newtabaction = ;
        _newtabaction->setShortcuts(QKeySequence::AddTab);
        _newtabaction->setIconVisibleInMenu(false);
        connect(
            _newtabaction
            , &QAction::triggered   //                , this
        , [this](bool make_current) {
            auto arint = boost::make_shared<NewTab>(this, make_current);
            request_record(
                QUrl(Browser::_defaulthome)
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                    "", &NewTab::generator, arint)   // [&](Record * const record)->WebView* {return newTab(record, make_current);}
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                    "", &NewTab::activator, arint)
            );
        });

        //        _closetabaction = ;
        _closetabaction->setShortcuts(QKeySequence::Close);
        _closetabaction->setIconVisibleInMenu(false);
        connect(_closetabaction, &QAction::triggered, this, &TabWidget::closeTab);

        //        _nexttabaction = ;
        QList<QKeySequence> shortcuts;
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
        _nexttabaction->setShortcuts(shortcuts);
        connect(_nexttabaction, SIGNAL(triggered()), this, SLOT(nextTab()));

        //        _previoustabaction = ;
        shortcuts.clear();
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
        _previoustabaction->setShortcuts(shortcuts);
        connect(_previoustabaction, SIGNAL(triggered()), this, SLOT(previousTab()));

        //        _recentlyclosedtabsmenu = ;
        connect(_recentlyclosedtabsmenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowRecentTabsMenu()));
        connect(_recentlyclosedtabsmenu, SIGNAL(triggered(QAction *)), this, SLOT(aboutToShowRecentTriggeredAction(QAction *)));
        //        _recentlyclosedtabsaction = ;
        _recentlyclosedtabsaction->setMenu(_recentlyclosedtabsmenu);
        _recentlyclosedtabsaction->setEnabled(false);

        connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));

        //        _lineedits = ;
        _lineedits->hide();
    }

    void TabWidget::clear()
    {
        // clear the recently closed tabs
        _recentlyclosedtabs.clear();

        // clear the line edit history
        for(int i = 0; i < _lineedits->count(); ++i) {
            QLineEdit *qLineEdit = lineEdit(i);
            qLineEdit->setText(qLineEdit->text());
        }
    }

    void TabWidget::moveTab(int fromIndex, int toIndex)
    {
        QWidget *lineEdit = _lineedits->widget(fromIndex);
        _lineedits->removeWidget(lineEdit);
        _lineedits->insertWidget(toIndex, lineEdit);
    }

    void TabWidget::addWebAction(QAction *action, QWebEnginePage::WebAction webAction)
    {
        if(!action)
            return;

        _actions.append(new WebActionMapper(action, webAction, this));
    }

    void TabWidget::currentChanged(int index)   // switch tab
    {
        WebView *webView = this->webView(index);

        if(webView) {  // return;

            //            int lc = _lineedits->count();
            //            int c = count();
            //            assert(lc == c);
            Q_ASSERT(_lineedits->count() == count());

            qDebug() << "_lineedits->count()\t" << _lineedits->count() << "\tcount()\n";

            WebView *oldWebView = this->webView(_lineedits->currentIndex());

            if(oldWebView && webView != oldWebView) {
#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
                disconnect(oldWebView, &PageView::statusBarMessage, this, &TabManager::showStatusBarMessage);
#endif
                disconnect(oldWebView->page(), SIGNAL(linkHovered(const QString &)), this, SIGNAL(linkHovered(const QString &)));
                disconnect(oldWebView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
                disconnect(oldWebView->page()->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem *)), this, SLOT(downloadRequested(QWebEngineDownloadItem *)));
                disconnect(oldWebView->page(), SIGNAL(fullScreenRequested(QWebEngineFullScreenRequest)),
                           this, SLOT(fullScreenRequested(QWebEngineFullScreenRequest)));
            }

#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
            connect(view, SIGNAL(statusBarMessage(QString)), this, SIGNAL(showStatusBarMessage(QString)));
#endif
            connect(webView->page(), &WebPage::linkHovered, this, &TabWidget::linkHovered);
            connect(webView, &WebView::loadProgress, this, &TabWidget::loadProgress);
            connect(webView->page()->profile(), &QWebEngineProfile::downloadRequested, this, &TabWidget::downloadRequested);
            connect(webView->page(), SIGNAL(fullScreenRequested(QWebEngineFullScreenRequest)),
                    this, SLOT(fullScreenRequested(QWebEngineFullScreenRequest)));

            for(int i = 0; i < _actions.count(); ++i) {
                WebActionMapper *mapper = _actions[i];
                mapper->updateCurrent(webView->page());
            }

            emit setCurrentTitle(
                //"test"//
                webView->title()
            );
            _lineedits->setCurrentIndex(index);
            emit loadProgress(webView->progress());
            emit showStatusBarMessage(webView->lastStatusBarText());

            if(webView->page()->url().isEmpty()) {
                _lineedits->currentWidget()->setFocus();
            } else {
                webView->setFocus();

                //                auto controller = webView->recordtablecontroller();
                auto record = webView->page()->current_record();

                if(// controller != nullptr &&
                    record != nullptr) {
                    //                    QModelIndex proxyindex = controller->convertIdToProxyIndex(record->getField("id"));
                    //                    int position = controller->convertProxyIndexToPos(proxyindex);
                    //                    RecordTableView *recordtableview = controller->getView();
                    //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                    webView->setFocus();
                    //                    globalparameters.mainwindow()
                    _recordtablecontroller->select_id(record->getNaturalFieldSource("id"));
                    //                    webView->setFocus();

                }
            }
        }
    }

    QAction *TabWidget::newTabAction() const
    {
        return _newtabaction;
    }

    QAction *TabWidget::closeTabAction() const
    {
        return _closetabaction;
    }

    QAction *TabWidget::recentlyClosedTabsAction() const
    {
        return _recentlyclosedtabsaction;
    }

    QAction *TabWidget::nextTabAction() const
    {
        return _nexttabaction;
    }

    QAction *TabWidget::previousTabAction() const
    {
        return _previoustabaction;
    }

    //QWidget *
    QStackedWidget *TabWidget::lineEditStack() const
    {
        return _lineedits;
    }

    void TabWidget::lineEditStack(QStackedWidget *lineedits)
    {
        _lineedits = lineedits;
    }

    QLineEdit *TabWidget::currentLineEdit() const
    {
        return lineEdit(_lineedits->currentIndex());
    }

    WebView *TabWidget::currentWebView() const
    {
        return webView(currentIndex());
    }

    QLineEdit *TabWidget::lineEdit(int index) const
    {
        UrlLineEdit *urlLineEdit = qobject_cast<UrlLineEdit *>(_lineedits->widget(index));

        if(urlLineEdit)
            return urlLineEdit->lineEdit();

        return 0;
    }

    WebView *TabWidget::webView(int index) const
    {
        WebView *webView = nullptr;
        QWidget *widget = this->widget(index);
        webView = qobject_cast<WebView *>(widget);

        //        if(webView == nullptr) {
        //            //return webView;
        //            //} else {
        //            if(count() == 0) {

        //                BrowserTabManager *that = const_cast<BrowserTabManager *>(this);
        //                that->setUpdatesEnabled(false);
        //                that->new_dummy();
        //                //that->newTab();
        //                //that->closeTab(0);
        //                that->setUpdatesEnabled(true);
        //                return currentWebView();
        //            }

        //            // optimization to delay creating the first webview
        //            if(count() == 1) {  // current is a empty_dummy, look newTabFull
        //                BrowserTabManager *that = const_cast<BrowserTabManager *>(this);
        //                that->setUpdatesEnabled(false);
        //                that->newTab();
        //                that->closeTab(0);
        //                that->setUpdatesEnabled(true);
        //                return currentWebView();
        //            }
        //        }

        return webView;
    }

    int TabWidget::webViewIndex(WebView *view) const
    {
        int index = -1;

        try {
            index = indexOf(view);
        } catch(QException &e) {
            qDebug() << e.what();
        }

        return index;
    }


    //    void TabManager::new_view(bool make_current)
    //    {
    //        new_view(register_record(QUrl(DockedWindow::_defaulthome)), globalparameters.getRecordTableScreen()->getRecordTableController(), make_current);
    //    }

    //    BrowserView *BrowserTabManager::new_dummy()
    //    {
    //        QWidget *empty_dummy = new QWidget;
    //        QPalette p = empty_dummy->palette();
    //        p.setColor(QPalette::Window, palette().color(QPalette::Base));
    //        empty_dummy->setPalette(p);
    //        empty_dummy->setAutoFillBackground(true);
    //        disconnect(this, &BrowserTabManager::currentChanged, this, &BrowserTabManager::currentChanged);
    //        addTab(empty_dummy, tr("(Untitled)"));
    //        connect(this, &BrowserTabManager::currentChanged, this, &BrowserTabManager::currentChanged);
    //        return nullptr;
    //    }


    WebView *TabWidget::newTab(Record *const record
                               // , bool openinnewtab
                               , bool make_current
                               , RecordTableController *recordtablecontroller
                              )
    {
        //        if(record == nullptr) {
        //            record = register_record(QUrl(DockedWindow::_defaulthome));
        //        } else {
        //            record = register_record(QUrl(record->getNaturalFieldSource("url")));
        //        }

        WebView *view = nullptr;   // find(record);

        //        if(view == nullptr) {
        // line edit
        UrlLineEdit *urlLineEdit = new UrlLineEdit;
        QLineEdit *lineEdit = urlLineEdit->lineEdit();

        if(!_lineeditcompleter && count() > 0) {
            HistoryCompletionModel *completionModel = new HistoryCompletionModel(this);
            completionModel->setSourceModel(QtSingleApplication::historyManager()->historyFilterModel());
            _lineeditcompleter = new QCompleter(completionModel, this);
            // Should this be in Qt by default?
            QAbstractItemView *popup = _lineeditcompleter->popup();
            QListView *listView = qobject_cast<QListView *>(popup);

            if(listView)
                listView->setUniformItemSizes(true);
        }

        lineEdit->setCompleter(_lineeditcompleter);
        connect(lineEdit, &QLineEdit::returnPressed, this, &TabWidget::lineEditReturnPressed);
        _lineedits->addWidget(urlLineEdit);
        _lineedits->setSizePolicy(lineEdit->sizePolicy());

        // optimization to delay creating the more expensive WebView, history, etc
        //        if(count() == 0) {return new_dummy();}

        // webview

        view = new WebView(record, _profile // use record for return
                           // , openinnewtab
                           , this, recordtablecontroller); //globalParameters.getRecordTableScreen()->getRecordTableController()    //
        //        record->view(webView);  // inside PageView initialization
        //webView->setPage(new WebPage(_profile, webView));
        assert(record->binded_only_page());

        urlLineEdit->setWebView(view);
        connect(view, SIGNAL(loadStarted()), this, SLOT(webViewLoadStarted()));
        connect(view, SIGNAL(iconChanged()), this, SLOT(webViewIconChanged()));
        connect(view, SIGNAL(titleChanged(QString)), this, SLOT(webViewTitleChanged(QString)));
        connect(view, SIGNAL(urlChanged(QUrl)), this, SLOT(webViewUrlChanged(QUrl)));
        connect(view->page(), SIGNAL(windowCloseRequested()), this, SLOT(windowCloseRequested()));
        connect(view->page(), SIGNAL(geometryChangeRequested(QRect)), this, SIGNAL(geometryChangeRequested(QRect)));

#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(view->page(), SIGNAL(printRequested(QWebEngineFrame *)), this, SIGNAL(printRequested(QWebEngineFrame *)));
#endif

#if defined(QWEBENGINEPAGE_MENUBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), SIGNAL(menuBarVisibilityChangeRequested(bool)), this, SIGNAL(menuBarVisibilityChangeRequested(bool)));
#endif

#if defined(QWEBENGINEPAGE_STATUSBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), SIGNAL(statusBarVisibilityChangeRequested(bool)), this, SIGNAL(statusBarVisibilityChangeRequested(bool)));
#endif

#if defined(QWEBENGINEPAGE_TOOLBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), SIGNAL(toolBarVisibilityChangeRequested(bool)), this, SIGNAL(toolBarVisibilityChangeRequested(bool)));
#endif
        addTab(view, record->getNaturalFieldSource("name"));    //, tr("(Untitled)")

        //record->page()->load(record);
        //globalparameters.entrance()->invoke_view(record);

        assert(record->binded_only_page());

        //        int lc = _lineedits->count();
        //        int c = count();
        //        assert(lc == c);
        //        Q_ASSERT(_lineedits->count() == count());

        if(make_current)
            setCurrentWidget(view);

        // webview actions
        for(int i = 0; i < _actions.count(); ++i) {
            WebActionMapper *mapper = _actions[i];
            mapper->addChild(view->page()->action(mapper->webAction()));
        }

        if(count() == 1)
            currentChanged(currentIndex()); // default this is input the new index

        emit tabsChanged();
        //webView->openLinkInNewTab(QUrl());    // don't need, initialized
        //        }

        assert(view);
        assert(record->binded_only_page());
        return view; // TabWidget::newTabFull::WebView *
    }

    void TabWidget::reloadAllTabs()
    {
        for(int i = 0; i < count(); ++i) {
            QWidget *tabWidget = widget(i);

            if(WebView *tab = qobject_cast<WebView *>(tabWidget)) {
                tab->reload();
            }
        }
    }

    void TabWidget::lineEditReturnPressed()
    {
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender())) {
            emit loadPage(lineEdit->text());

            if(_lineedits->currentWidget() == lineEdit)
                currentWebView()->setFocus();
        }
    }

    void TabWidget::windowCloseRequested()
    {
        WebPage *webPage = qobject_cast<WebPage *>(sender());
        WebView *webView = qobject_cast<WebView *>(webPage->view());
        int index = webViewIndex(webView);

        if(index >= 0) {
            if(count() == 1)
                webView->page()->browser()->close();
            else
                closeTab(index);
        }
    }

    void TabWidget::closeOtherTabs(int index)
    {
        if(-1 == index)
            return;

        for(int i = count() - 1; i > index; --i)
            closeTab(i);

        for(int i = index - 1; i >= 0; --i)
            closeTab(i);
    }

    // When index is -1 index chooses the current tab
    void TabWidget::cloneTab(int index)
    {
        if(index < 0)
            index = currentIndex();

        if(index < 0 || index >= count())
            return;

        assert(webView(index)->page()->current_record()->binded_only_page());
        //WebView *tab =
        newTab(webView(index)->page()->current_record()
               // , false
               , true //, view(index)->recordtablecontroller()
              );
        //tab->setUrl(webView(index)->url());
    }


    // When index is -1 index chooses the current tab
    void TabWidget::requestCloseTab(int index)
    {
        if(index < 0)
            index = currentIndex();

        if(index < 0 || index >= count())
            return;

        WebView *tab = webView(index);

        if(!tab)
            return;

        tab->page()->triggerAction(QWebEnginePage::RequestClose);
    }

    // When index is -1 index chooses the current tab
    void TabWidget::closeTab(int index)
    {
        if(index < 0)
            index = currentIndex();

        if(index >= 0 && index < count()) { // if(index < 0 || index >= count())
            // return;

            bool hasFocus = false;

            if(WebView *view_ = webView(index)) {
#if defined(QWEBENGINEPAGE_ISMODIFIED)

                if(tab->isModified()) {
                    QMessageBox closeConfirmation(view_);
                    closeConfirmation.setWindowFlags(Qt::Sheet);
                    closeConfirmation.setWindowTitle(tr("Do you really want to close this page?"));
                    closeConfirmation.setInformativeText(tr("You have modified this page and when closing it you would lose the modification.\n"
                                                            "Do you really want to close this page?\n"));
                    closeConfirmation.setIcon(QMessageBox::Question);
                    closeConfirmation.addButton(QMessageBox::Yes);
                    closeConfirmation.addButton(QMessageBox::No);
                    closeConfirmation.setEscapeButton(QMessageBox::No);

                    if(closeConfirmation.exec() == QMessageBox::No)
                        return;
                }

#endif
                hasFocus = view_->hasFocus();

                if(_profile == QWebEngineProfile::defaultProfile()) {
                    _recentlyclosedtabsaction->setEnabled(true);
                    _recentlyclosedtabs.prepend(view_->page()->url());

                    if(_recentlyclosedtabs.size() >= TabWidget::_recentlyclosedtabssize)
                        _recentlyclosedtabs.removeLast();
                }
            }

            QWidget *lineEdit = _lineedits->widget(index);

            if(lineEdit) {
                _lineedits->removeWidget(lineEdit);
                //delete lineEdit;    //
                lineEdit->deleteLater();
            }

            QWidget *previous_webview = widget(index);

            if(previous_webview) {
                removeTab(index);

                //        PageView *v = static_cast<PageView *>(previous_webview);

                //        if(v) {
                //            //            Record *rc = v->record();

                //            //            if(rc)rc->view(nullptr);

                //            //delete v;           //

                //        }

                previous_webview->deleteLater();
            }

            emit tabsChanged();

            if(hasFocus && count() > 0) {
                WebView *view = nullptr;
                view = currentWebView();

                if(view != nullptr) {
                    view->setFocus();
                    Record *record = view->page()->current_record();

                    if(record) {
                        assert(record->binded_only_page());
                        //                    QModelIndex proxyindex = view->recordtablecontroller()->convertIdToProxyIndex(record->getField("id"));
                        //                    int position = view->recordtablecontroller()->convertProxyIndexToPos(proxyindex);
                        //                    RecordTableView *recordtableview = view->recordtablecontroller()->getView();

                        //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                        view->setFocus();
                        //                        globalparameters.mainwindow()
                        _recordtablecontroller->select_id(record->getNaturalFieldSource("id"));
                    }
                }
            }

            if(count() == 0)
                emit lastTabClosed();
        }
    }

    //deprecated, profile should be assigned at the construction of BrowserPage
    void TabWidget::setProfile(QWebEngineProfile *profile)
    {
        _profile = profile;

        //        for(int i = 0; i < count(); ++i) {
        //            QWidget *tabWidget = widget(i);

        //            if(BrowserView *tab = qobject_cast<BrowserView *>(tabWidget)) {
        //                BrowserPage *webPage = tab->webPage();  // ->setProfile(profile); // BrowserPage *webPage = new BrowserPage(_profile, tab);
        //                webPage->load(tab->page()->url());      // this semantic is reload
        //                tab->setPage(webPage);
        //            }
        //        }
    }

    void TabWidget::webViewLoadStarted()
    {
        WebView *webView = qobject_cast<WebView *>(sender());
        int index = webViewIndex(webView);

        if(-1 != index) {
            QIcon icon(QLatin1String(":loading.gif"));
            setTabIcon(index, icon);
        }
    }

    void TabWidget::webViewIconChanged()
    {
        WebView *webView = qobject_cast<WebView *>(sender());
        int index = webViewIndex(webView);

        if(-1 != index) {
            QIcon icon = webView->icon();
            setTabIcon(index, icon);
        }
    }

    void TabWidget::webViewTitleChanged(const QString &title)
    {
        WebView *webView = qobject_cast<WebView *>(sender());
        int index = webViewIndex(webView);

        if(-1 != index) {
            setTabText(index, title);
        }

        if(currentIndex() == index)
            emit setCurrentTitle(
                title   // "test"//
            );

        QtSingleApplication::historyManager()->updateHistoryItem(webView->page()->url(), title);
    }

    void TabWidget::webViewUrlChanged(const QUrl &url)
    {
        WebView *webView = qobject_cast<WebView *>(sender());
        int index = webViewIndex(webView);

        if(-1 != index) {
            _tabbar->setTabData(index, url);
        }

        emit tabsChanged();
    }

    void TabWidget::aboutToShowRecentTabsMenu()
    {
        _recentlyclosedtabsmenu->clear();

        for(int i = 0; i < _recentlyclosedtabs.count(); ++i) {
            QAction *action = new QAction(_recentlyclosedtabsmenu);
            action->setData(_recentlyclosedtabs.at(i));
            QIcon icon = QtSingleApplication::instance()->icon(_recentlyclosedtabs.at(i));
            action->setIcon(icon);
            action->setText(_recentlyclosedtabs.at(i).toString());
            _recentlyclosedtabsmenu->addAction(action);
        }
    }

    void TabWidget::aboutToShowRecentTriggeredAction(QAction *action)
    {
        QUrl url = action->data().toUrl();
        loadUrlInCurrentTab(url);
    }

    void TabWidget::mouseDoubleClickEvent(QMouseEvent *event)
    {
        if(!childAt(event->pos())
           // Remove the line below when QTabWidget does not have a one pixel frame
           && event->pos().y() < (tabBar()->y() + tabBar()->height())
          ) {
            //            newTab(request_record(QUrl(DockedWindow::_defaulthome))
            //                   // , false
            //                  );

            auto arint = boost::make_shared<NewTab>(this, true);
            request_record(QUrl(Browser::_defaulthome)
                           , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                               "", &NewTab::generator, arint)
                           , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                               "", &NewTab::activator, arint)
                          );
            return;
        }

        QTabWidget::mouseDoubleClickEvent(event);
    }

    void TabWidget::contextMenuEvent(QContextMenuEvent *event)
    {
        if(!childAt(event->pos())) {
            _tabbar->contextMenuRequested(event->pos());
            return;
        }

        QTabWidget::contextMenuEvent(event);
    }

    void TabWidget::mouseReleaseEvent(QMouseEvent *event)
    {
        if(
            event->button() == Qt::MidButton && !childAt(event->pos())
            // Remove the line below when QTabWidget does not have a one pixel frame
            && event->pos().y() < (tabBar()->y() + tabBar()->height())
        ) {
            QUrl url(QApplication::clipboard()->text(QClipboard::Selection));

            if(!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
                //                WebView *webView = newTab(request_record(QUrl(DockedWindow::_defaulthome))
                //                                          // , false
                //                                         );
                //                webView->setUrl(url);
                auto arint = boost::make_shared<NewTab>(this, true);
                request_record(url
                               , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                                   "", &NewTab::generator, arint)
                               , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                                   "", &NewTab::activator, arint)
                              );
            }
        }
    }

    void TabWidget::loadUrlInCurrentTab(const QUrl &url)
    {
        WebView *webView = currentWebView();

        if(webView) {
            //            Record *record;
            Record *record_ = webView->page()->current_record();

            if(record_->binded_only_page()->url() != url) {
                //                //                record = record_;
                //                //            } else {
                //                record = request_record(url);
                //                //            }

                //                // assert(record);
                //                // record.setNaturalFieldSource("url", url.toString());
                //                webView->page()->load(record);    //webView->load(record); //loadUrl(url);
                //                webView->setFocus();
                auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(webView->page(), true);
                request_record(
                    url
                    , std::make_shared <
                    sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, Record *const>
                    > (
                        std::string("")
                        , &WebPage::ActiveRecordBinder::generator
                        , ar
                    )
                    , std::make_shared <
                    sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>
                    > (
                        ""
                        , &WebPage::ActiveRecordBinder::activator
                        , ar
                    )
                );
            }
        }
    }

    void TabWidget::nextTab()
    {
        int next = currentIndex() + 1;

        if(next == count())
            next = 0;

        setCurrentIndex(next);
    }

    void TabWidget::previousTab()
    {
        int next = currentIndex() - 1;

        if(next < 0)
            next = count() - 1;

        setCurrentIndex(next);
    }

    static const qint32 TabWidgetMagic = 0xaa;

    QByteArray TabWidget::saveState() const
    {
        int version = 1;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << qint32(TabWidgetMagic);
        stream << qint32(version);

        QStringList tabs;

        for(int i = 0; i < count(); ++i) {
            if(WebView *tab = qobject_cast<WebView *>(widget(i))) {
                //Record *record = tab->page()->record();
                //assert(tab->page()->record()->getNaturalFieldSource("url") != QUrl().toString());

                //if(record) {
                //                assert(tab->page()->url() != QUrl());
                //if(record->getNaturalFieldSource("url") != QUrl().toString()) {
                tabs.append(
                    //tab->page()->record()->getNaturalFieldSource("url") //
                    tab->page()->url().toString()
                );
                //}
                //}
            } else {
                tabs.append(QString::null);
            }
        }

        stream << tabs;
        stream << currentIndex();
        return data;
    }


    bool TabWidget::restoreState(const QByteArray &state)
    {
        int version = 1;
        QByteArray sd = state;
        QDataStream stream(&sd, QIODevice::ReadOnly);

        if(stream.atEnd())
            return false;

        qint32 marker;
        qint32 v;
        stream >> marker;
        stream >> v;

        if(marker != TabWidgetMagic || v != version)
            return false;

        QStringList openTabs;
        stream >> openTabs;

        for(int i = 0; i < openTabs.count(); ++i) {
            auto url = openTabs.at(i);
            //            Record *_record = request_record(_url);

            if(i != 0) {
                //                newTab(_record
                //                       // , false
                //                      );                    //, globalParameters.getRecordTableScreen()->getRecordTableController()
                //                //v->load_record(_record);
                auto arint = boost::make_shared<NewTab>(this, true);
                request_record(url
                               , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                                   "", &NewTab::generator, arint)
                               , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                                   "", &NewTab::activator, arint)
                              );


            } else {
                if(webView(0)->page()->url() != url) {
                    //                    webView(0)->load(_record);    //loadUrl(_url);
                    auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(webView(0)->page(), true);
                    request_record(
                        url
                        , std::make_shared <
                        sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>
                        > (
                            ""
                            , &WebPage::ActiveRecordBinder::generator
                            , ar
                        )
                        , std::make_shared <
                        sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>
                        > (
                            ""
                            , &WebPage::ActiveRecordBinder::activator
                            , ar
                        )
                    );
                }
            }
        }

        int currentTab;
        stream >> currentTab;
        setCurrentIndex(currentTab);

        return true;
    }

    void TabWidget::downloadRequested(QWebEngineDownloadItem *download)
    {
        QtSingleApplication::downloadManager()->download(download);
        download->accept();
    }




    void TabWidget::onTabsChanged()
    {
        //    if(count() > 0) {
        //        WebView *tab = nullptr;
        //        tab = currentWebView();

        //        if(tab != nullptr) {
        //            tab->setFocus();
        //            auto controller = tab->recordtablecontroller();
        //            auto record = tab->record();

        //            if(controller != nullptr && record != nullptr) {
        //                QModelIndex proxyindex = controller->convertIdToProxyIndex(record->getField("id"));
        //                int position = controller->convertProxyIndexToPos(proxyindex);
        //                RecordTableView *recordtableview = controller->getView();

        //                if(recordtableview)recordtableview->setSelectionToPos(position); // work
        //            }
        //        }
        //    }
    }

    WebView *TabWidget::find_nopin()const
    {
        WebView *bv = nullptr;

        for(int i = 0; i < count(); i++) {
            auto vi = webView(i);

            if(vi != nullptr) {
                if(vi->page()->current_record()) {
                    if(vi->page()->current_record()->getNaturalFieldSource("pin")
                       == _check_state[Qt::Unchecked]
                      ) {
                        bv = vi; break;
                    }
                }
            }
        }

        return bv;
    }

    WebView *TabWidget::find(const QUrl &url)const
    {
        WebView *bv = nullptr;

        //if(record) {

        for(int i = 0; i < count(); i++) {
            auto vi = webView(i);

            if(vi != nullptr) {
                //                Record *checkrecord = vi->page()->record();
                QString checkurl = vi->page()->url().toString();    // debug
                QString checktitle = vi->page()->title();           // debug


                if(vi->page()->current_record()) {
                    QString checkrecordurl = vi->page()->current_record()->getNaturalFieldSource("url");
                    assert(checkrecordurl != "");

                    if(vi->page()->current_record()->getNaturalFieldSource("url")
                       == url.toString()   // record->getNaturalFieldSource("url")
                      ) {
                        bv = vi; break;
                    }

                }
            }
        }

        //}

        return bv;
    }


    void TabWidget::fullScreenRequested(QWebEngineFullScreenRequest request)
    {
        WebPage *webPage = qobject_cast<WebPage *>(sender());

        if(request.toggleOn()) {
            if(!_fullscreenview) {
                _fullscreenview = new QWebEngineView();
                _fullscreennotification = new FullScreenNotification(_fullscreenview);

                QAction *exitFullScreenAction = new QAction(_fullscreenview);
                exitFullScreenAction->setShortcut(Qt::Key_Escape);
                connect(exitFullScreenAction, &QAction::triggered, [webPage] {
                    webPage->triggerAction(QWebEnginePage::ExitFullScreen);
                });
                _fullscreenview->addAction(exitFullScreenAction);
            }

            webPage->setView(_fullscreenview);
            request.accept();
            _fullscreenview->showFullScreen();
            _fullscreenview->raise();
            _fullscreennotification->show();
        } else {
            if(!_fullscreenview)
                return;

            WebView *oldWebView = this->webView(_lineedits->currentIndex());
            webPage->setView(oldWebView);
            request.accept();
            raise();
            _fullscreenview->hide();
            _fullscreennotification->hide();
        }
    }




    WebActionMapper::WebActionMapper(QAction *root, QWebEnginePage::WebAction webAction, QObject *parent)
        : QObject(parent)
        , _currentparent(0)
        , _root(root)
        , _webaction(webAction)
    {
        if(!_root)
            return;

        connect(_root, SIGNAL(triggered()), this, SLOT(rootTriggered()));
        connect(root, SIGNAL(destroyed(QObject *)), this, SLOT(rootDestroyed()));
        root->setEnabled(false);
    }

    void WebActionMapper::rootDestroyed()
    {
        _root = 0;
    }

    void WebActionMapper::currentDestroyed()
    {
        updateCurrent(0);
    }

    void WebActionMapper::addChild(QAction *action)
    {
        if(!action)
            return;

        connect(action, SIGNAL(changed()), this, SLOT(childChanged()));
    }

    QWebEnginePage::WebAction WebActionMapper::webAction() const
    {
        return _webaction;
    }

    void WebActionMapper::rootTriggered()
    {
        if(_currentparent) {
            QAction *gotoAction = _currentparent->action(_webaction);
            gotoAction->trigger();
        }
    }

    void WebActionMapper::childChanged()
    {
        if(QAction *source = qobject_cast<QAction *>(sender())) {
            if(_root
               && _currentparent
               && source->parent() == _currentparent) {
                _root->setChecked(source->isChecked());
                _root->setEnabled(source->isEnabled());
            }
        }
    }

    void WebActionMapper::updateCurrent(QWebEnginePage *currentParent)
    {
        if(_currentparent)
            disconnect(_currentparent, SIGNAL(destroyed(QObject *)),
                       this, SLOT(currentDestroyed()));

        _currentparent = currentParent;

        if(!_root)
            return;

        if(!_currentparent) {
            _root->setEnabled(false);
            _root->setChecked(false);
            return;
        }

        QAction *source = _currentparent->action(_webaction);
        _root->setChecked(source->isChecked());
        _root->setEnabled(source->isEnabled());
        connect(_currentparent, SIGNAL(destroyed(QObject *)),
                this, SLOT(currentDestroyed()));
    }


    struct WebViewHelper {

        PopupWindow *_the;
        QWebEngineProfile *_profile;
        RecordTableController *_recordtablecontroller;
        WebView *_view;
        WebViewHelper(
            PopupWindow *const the, QWebEngineProfile *profile, RecordTableController *recordtablecontroller) :
            _the(the)
            , _profile(profile)
            , _recordtablecontroller(recordtablecontroller)
            , _view(nullptr)
        {}
        WebView *generator(Record *const record)
        {
            return _view =
                       new WebView(record, _profile, _the, _recordtablecontroller);

        }

        void activator()
        {
            _view->page()->active();

        }

    };

    PopupWindow::PopupWindow(QWebEngineProfile *profile, QUrl const &url, RecordTableController *recordtablecontroller, Browser *parent)
        : TabWidget(recordtablecontroller, parent)
        , _addressbar(new QLineEdit(this))
        , _view(
              //              new WebView(record, profile
              //                          // , false
              //                          , this
              //                          , _recordtablecontroller    // globalparameters.getRecordTableScreen()->getRecordTableController()
              //                         )
              [this, url, profile, recordtablecontroller]
    {
        auto wvh = boost::make_shared<WebViewHelper>(this, profile, recordtablecontroller);
        Record *record = request_record(
                             url
                             , std::make_shared <
                             sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, Record *const>
                             > (""
                                , &WebViewHelper::generator
                                , wvh
                               )
                             , std::make_shared <
                             sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>
                             > (""
                                , &WebViewHelper::activator
                                , wvh
                               )
                         );  // ->binded_only_page()->view();
        return record->binded_only_page()->view();
    }()

      )
    {
        //        assert(record);
        //        assert(record->binded_only_page());

        //        _view->page(new WebPage(profile, url
        //                                // , false
        //                                , _recordtablecontroller
        //                                , _view));




        QVBoxLayout *layout = new QVBoxLayout;
        layout->setMargin(0);
        setLayout(layout);
        layout->addWidget(_addressbar);
        layout->addWidget(_view);
        _view->setFocus();

        connect(_view, &WebView::titleChanged, this, &QWidget::setWindowTitle);
        connect(_view, &WebView::urlChanged, this, &PopupWindow::setUrl);
        connect(page(), &WebPage::geometryChangeRequested, this, &PopupWindow::adjustGeometry);
        connect(page(), &WebPage::windowCloseRequested, this, &QWidget::close);
    }

    //    QWebEnginePage
    WebPage *PopupWindow::page() const { return _view->page(); }


    void PopupWindow::setUrl(const QUrl &url)
    {
        _addressbar->setText(url.toString());
    }


    void PopupWindow::adjustGeometry(const QRect &newGeometry)
    {
        const int x1 = frameGeometry().left() - geometry().left();
        const int y1 = frameGeometry().top() - geometry().top();
        const int x2 = frameGeometry().right() - geometry().right();
        const int y2 = frameGeometry().bottom() - geometry().bottom();

        setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
    }


}


