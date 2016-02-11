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

#include <memory>
#include <cassert>
#include "tabwidget.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"

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


#include "views/record_table/RecordScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/record_table/Record.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "views/record_table/RecordView.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "models/tree/TreeItem.h"
#include "toolbarsearch.h"
#include "entrance.h"
#include "views/main_window/MainWindow.h"
#include "main.h"
#include "views/record/MetaEditor.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/WalkHistory.h"
#include "libraries/wyedit/EditorTextArea.h"



extern GlobalParameters globalparameters;
extern AppConfig appconfig;
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
        connect(this, &TabBar::customContextMenuRequested, this, &TabBar::contextMenuRequested);

        QString ctrl = QLatin1String("Ctrl+%1");

        for(int i = 1; i < 10; ++i) {
            QShortcut *shortCut = new QShortcut(ctrl.arg(i), this);
            _tabshortcuts.append(shortCut);
            connect(shortCut, &QShortcut::activated, this, &TabBar::selectTabAction);
        }

        setTabsClosable(true);
        connect(static_cast<QTabBar *const>(this), &QTabBar::tabCloseRequested, this, &TabBar::closeTabSignal);
        setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
        setMovable(true);
        //        setStyleSheet("QTabBar::tab { left: 1px; max-width: 200px; align: left; text-align: left; margin-left: 2px; padding: 2px;}");  // ?
        setStyleSheet("QTabBar::tab { max-width: 200px; padding: 2px; margin-left: 2px; }");
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
            QAction *action = menu.addAction(tr("Clone Tab"), this, &TabBar::cloneTab);
            action->setData(index);

            menu.addSeparator();

            action = menu.addAction(tr("&Close Tab"), this, &TabBar::closeTab, QKeySequence::Close);
            action->setData(index);

            action = menu.addAction(tr("Close &Other Tabs"), this, &TabBar::closeOtherTabs);
            action->setData(index);

            menu.addSeparator();

            action = menu.addAction(tr("Reload Tab"), this, &TabBar::reloadTab, QKeySequence::Refresh);
            action->setData(index);
        } else {
            menu.addSeparator();
        }

        menu.addAction(tr("Reload All Tabs"), this, &TabBar::reloadAllTabs);
        menu.exec(QCursor::pos());
    }

    void TabBar::cloneTab()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit cloneTabSignal(index);
        }
    }

    void TabBar::closeTab()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit closeTabSignal(index);
        }
    }

    void TabBar::closeOtherTabs()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            int index = action->data().toInt();
            emit closeOtherTabsSignal(index);
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
            emit reloadTabSignal(index);
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

        if(_fullscreennotification)delete _fullscreennotification;

        if(_fullscreenview)delete _fullscreenview;

    }

    TabWidget::TabWidget(RecordController *_record_controller
                         , boost::intrusive_ptr<TreeItem> _shadow_branch_root
                         , Browser *parent)
        : QTabWidget(parent)
        , _recentlyclosedtabsaction(new QAction(tr("Recently Closed Tabs"), this))
        , _newtabaction(new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this))
        , _closetabaction(new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this))
        , _nexttabaction(new QAction(tr("Show Next Tab"), this))
        , _previoustabaction(new QAction(tr("Show Previous Tab"), this))
        , _recentlyclosedtabsmenu(new QMenu(this))
          // , _completionModel(new HistoryCompletionModel(this))
        , _lineeditcompleter(nullptr)                 // new QCompleter(_completionModel, this)
        , _lineedits(new QStackedWidget(this))
        , _tabbar(new TabBar(this))
        , _profile(QWebEngineProfile::defaultProfile())
        , _fullscreenview(nullptr)
        , _fullscreennotification(nullptr)
        , _record_controller(_record_controller)
          //        , _page_controller(_page_controller)
          //        , _active_record(this)
          //        , _active("", &active_record::operator(), &_active_record)
        , _shadow_branch_root(_shadow_branch_root)
          //        , _shadow_source_model(new TableModel(QString(table_screen_singleton_name) + QString("_shadow"), _tree_item, this))
          //        , _table_data(std::make_shared<TableData>(_tree_item))
        , _window(parent)
    {
        //        _lineedits = globalparameters.find_screen()->toolbarsearch()->lineedits();
        //        connect(parent, []() {}, globalparameters.find_screen()->toolbarsearch(), [this]() {globalparameters.find_screen()->toolbarsearch()->lineedits(_lineedits);});
        //        move to     void Browser::activateWindow();


        setElideMode(Qt::ElideRight);
        //        _active_record = [this](Record * const record)-> WebView * {return globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);};
        connect(
            _tabbar
            , &TabBar::newTab   //                , this
        , [this]() {
            boost::shared_ptr<browser::TabWidget::ActiveRecordBinder> arint = boost::make_shared<browser::TabWidget::ActiveRecordBinder>(this, true);
            //            auto r =
            this->_record_controller->request_item(
                QUrl(Browser::_defaulthome)

                //            [this](Record * const record)-> WebView * {
                //                return globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);
                //            }   // nested    //
                //                [this](Record * const record)-> WebView * {return this->newTab(record);}
                , std::make_shared <
                sd::_interface <
                sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
                >
                // , std::string, browser::WebView *(TabWidget::ActiveRecordBinder::*)(boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)), boost::shared_ptr<TabWidget::ActiveRecordBinder>
                > (
                    std::string(""), &TabWidget::ActiveRecordBinder::binder, std::forward<boost::shared_ptr<TabWidget::ActiveRecordBinder>>(arint))  //                _active
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>
                // , std::string, browser::WebView *(TabWidget::ActiveRecordBinder::*)(boost::intrusive_ptr<TreeItem>), boost::shared_ptr<TabWidget::ActiveRecordBinder>
                >(
                    std::string(""), &TabWidget::ActiveRecordBinder::activator, std::forward<boost::shared_ptr<TabWidget::ActiveRecordBinder>>(arint))
            );
            //            r->active();
        });

        connect(_tabbar, &TabBar::closeTabSignal, this, &TabWidget::requestCloseTab);
        connect(this, &TabWidget::tabsChanged, this, &TabWidget::onTabsChanged);
        connect(_tabbar, &TabBar::cloneTabSignal, this, &TabWidget::cloneTab);
        connect(_tabbar, &TabBar::closeOtherTabsSignal, this, &TabWidget::closeOtherTabs);
        connect(_tabbar, &TabBar::reloadTabSignal, this, &TabWidget::reloadTab);
        connect(_tabbar, &TabBar::reloadAllTabs, this, &TabWidget::reloadAllTabs);
        connect(_tabbar, &TabBar::tabMoved, this, &TabWidget::moveTab);
        setTabBar(_tabbar);
        setDocumentMode(true);

        //        _tabbar->setMaximumSize(0, 0);

        //        _tabbar->hide();

        // Actions
        //        _newtabaction = ;
        _newtabaction->setShortcuts(QKeySequence::AddTab);
        _newtabaction->setIconVisibleInMenu(false);
        connect(
            _newtabaction
            , &QAction::triggered   //                , this
        , [this](bool make_current) {
            auto arint = boost::make_shared<ActiveRecordBinder>(this, make_current);
            this->_record_controller->request_item(
                QUrl(Browser::_defaulthome)
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                    "", &ActiveRecordBinder::binder, arint)   // [&](Record * const record)->WebView* {return newTab(record, make_current);}
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                    "", &ActiveRecordBinder::activator, arint)
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
        connect(_nexttabaction, &QAction::triggered, this, &TabWidget::nextTab);

        //        _previoustabaction = ;
        shortcuts.clear();
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
        _previoustabaction->setShortcuts(shortcuts);
        connect(_previoustabaction, &QAction::triggered, this, &TabWidget::previousTab);

        //        _recentlyclosedtabsmenu = ;
        connect(_recentlyclosedtabsmenu, &QMenu::aboutToShow, this, &TabWidget::aboutToShowRecentTabsMenu);
        connect(_recentlyclosedtabsmenu, &QMenu::triggered, this, &TabWidget::aboutToShowRecentTriggeredAction);
        //        _recentlyclosedtabsaction = ;
        _recentlyclosedtabsaction->setMenu(_recentlyclosedtabsmenu);
        _recentlyclosedtabsaction->setEnabled(false);

        connect(static_cast<QTabWidget *>(this), &QTabWidget::currentChanged, this, &TabWidget::currentChanged);

        //        _lineedits = ;

        //        _lineedits->setMaximumSize(0, 0);

        _lineedits->hide();
        setTabPosition(TabPosition::South);
        setTabShape(TabShape::Triangular);
        setStyleSheet("QTabWidget::tab-bar { max-width: 200px; align: left; text-align: left; margin-left: 2px; padding: 2px;}"); // left: 1px; alignment: left; // does not work
        //        Qt::Alignment tabAlignment = Qt::Alignment(q->style()->styleHint(QStyle::SH_TabBar_Alignment, 0, q));
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
                disconnect(static_cast<QWebEnginePage *>(oldWebView->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
                disconnect(oldWebView, &WebView::loadProgress, this, &TabWidget::loadProgress);
                disconnect(oldWebView->page()->profile(), &QWebEngineProfile::downloadRequested, this, &TabWidget::downloadRequested);
                disconnect(static_cast<QWebEnginePage *>(oldWebView->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);
            }

#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
            connect(webView, &WebView::statusBarMessage, this, &TabWidget::showStatusBarMessage);
#endif
            connect(static_cast<QWebEnginePage *>(webView->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
            connect(webView, &WebView::loadProgress, this, &TabWidget::loadProgress);
            connect(webView->page()->profile(), &QWebEngineProfile::downloadRequested, this, &TabWidget::downloadRequested);
            connect(static_cast<QWebEnginePage *>(webView->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

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
                auto record = webView->page()->current_item();

                if(// controller != nullptr &&
                    record != nullptr) {
                    //                    QModelIndex proxyindex = controller->convertIdToProxyIndex(record->getField("id"));
                    //                    int position = controller->convertProxyIndexToPos(proxyindex);
                    //                    RecordTableView *recordtableview = controller->getView();
                    //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                    webView->setFocus();
                    //                    globalparameters.mainwindow()
                    _record_controller->select_id(record->natural_field_source("id"));
                    //                    webView->setFocus();
                    MetaEditor *metaeditor = find_object<MetaEditor>(meta_editor_singleton_name);
                    assert(metaeditor);

                    if(metaeditor->record() != record) {

                        webView->page()->sychronize_metaeditor_to_record(record);  // metaeditor->bind(record);
                    }
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


    WebView *TabWidget::newTab(boost::intrusive_ptr<TreeItem> record   // , bool openinnewtab
                               , bool make_current
                               , RecordController *_record_controller
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

        //        // webview
        //        if(!record->page_valid()
        //           //           && !record->unique_page()
        //          ) {
        view = new WebView(record, _profile // use record for return
                           // , openinnewtab
                           , this
                           , _record_controller
                           //                               , _page_controller
                          ); //globalParameters.getRecordTableScreen()->getRecordTableController()    //
        //        } else {
        //            view = record->unique_page()->view();
        //        }

        //        record->view(webView);  // inside PageView initialization
        //webView->setPage(new WebPage(_profile, webView));
        assert(record->page_valid() && record->unique_page());
        assert(view);
        urlLineEdit->setWebView(view);
        connect(view, &WebView::loadStarted, this, &TabWidget::webViewLoadStarted);
        connect(view, &WebView::iconChanged, this, &TabWidget::webViewIconChanged);
        connect(view, &WebView::titleChanged, this, &TabWidget::webViewTitleChanged);
        connect(view, &WebView::urlChanged, this, &TabWidget::webViewUrlChanged);
        connect(static_cast<QWebEnginePage *>(view->page()), &QWebEnginePage::windowCloseRequested, this, &TabWidget::windowCloseRequested);
        connect(static_cast<QWebEnginePage *>(view->page()), &QWebEnginePage::geometryChangeRequested, this, &TabWidget::geometryChangeRequested);

#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(view->page(), &WebPage::printRequested, this, &TabWidget::printRequested);
#endif

#if defined(QWEBENGINEPAGE_MENUBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), &WebPage::menuBarVisibilityChangeRequested, this, &TabWidget::menuBarVisibilityChangeRequested);
#endif

#if defined(QWEBENGINEPAGE_STATUSBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), &WebPage::statusBarVisibilityChangeRequested, this, &TabWidget::statusBarVisibilityChangeRequested);
#endif

#if defined(QWEBENGINEPAGE_TOOLBARVISIBILITYCHANGEREQUESTED)
        connect(view->page(), &WebPage::toolBarVisibilityChangeRequested, this, &TabWidget::toolBarVisibilityChangeRequested);
#endif
        int index = addTab(view, record->natural_field_source("name"));  //, tr("(Untitled)")
        setTabToolTip(index, record->natural_field_source("name"));
        //record->page()->load(record);
        //globalparameters.entrance()->invoke_view(record);

        assert(record->page_valid() && record->unique_page());

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
        assert(record->page_valid() && record->unique_page());
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

        assert(webView(index)->page()->current_item()->page_valid() && webView(index)->page()->current_item()->unique_page());
        //WebView *tab =
        newTab(webView(index)->page()->current_item()
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

            WebView *previous_webview = static_cast<WebView *>(widget(index));

            if(previous_webview) {
                previous_webview->page()->break_records();
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
                    boost::intrusive_ptr<TreeItem> record = view->page()->current_item();

                    if(record) {
                        assert(record->page_valid() && record->unique_page());
                        //                    QModelIndex proxyindex = view->recordtablecontroller()->convertIdToProxyIndex(record->getField("id"));
                        //                    int position = view->recordtablecontroller()->convertProxyIndexToPos(proxyindex);
                        //                    RecordTableView *recordtableview = view->recordtablecontroller()->getView();

                        //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                        view->setFocus();
                        //                        globalparameters.mainwindow()
                        _record_controller->select_id(record->natural_field_source("id"));
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

            auto arint = boost::make_shared<ActiveRecordBinder>(this, true);
            _record_controller->request_item(
                QUrl(Browser::_defaulthome)
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                    "", &ActiveRecordBinder::binder, arint)
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                    "", &ActiveRecordBinder::activator, arint)
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

    void TabWidget::resizeEvent(QResizeEvent *e)
    {
        for(int i = 0; i < count(); i++) {
            WebView *view = webView(i);

            if(view)view->resizeEvent(e);
        }

        QTabWidget::resizeEvent(e);
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
                auto arint = boost::make_shared<ActiveRecordBinder>(this, true);
                auto r = _record_controller->request_item(
                             url
                             , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                                 "", &ActiveRecordBinder::binder, arint)
                             , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                                 "", &ActiveRecordBinder::activator, arint)
                         );
                r->active();
            }
        }
    }

    void TabWidget::loadUrlInCurrentTab(const QUrl &url)
    {
        WebView *webView = currentWebView();

        if(webView) {
            //            Record *record;
            boost::intrusive_ptr<TreeItem> record_ = webView->page()->current_item();

            if(record_->page_valid() && record_->unique_page()->url() != url) {
                //                //                record = record_;
                //                //            } else {
                //                record = request_record(url);
                //                //            }

                //                // assert(record);
                //                // record.setNaturalFieldSource("url", url.toString());
                //                webView->page()->load(record);    //webView->load(record); //loadUrl(url);
                //                webView->setFocus();
                auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(webView->page());
                auto r = _record_controller->request_item(
                             url
                             , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>> (
                                 std::string("")
                                 , &WebPage::ActiveRecordBinder::binder
                                 , ar
                             )
                             , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<TreeItem>>> (
                                 std::string("")
                                 , &WebPage::ActiveRecordBinder::activator
                                 , ar
                             )
                         );
                r->active();
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
                auto arint = boost::make_shared<ActiveRecordBinder>(this, true);
                auto r
                    = _record_controller->request_item(
                          url
                          , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                              "", &ActiveRecordBinder::binder, arint)
                          , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                              "", &ActiveRecordBinder::activator, arint)
                      );
                r->active();
            } else {
                if(webView(0)->page()->url() != url) {
                    //                    webView(0)->load(_record);    //loadUrl(_url);
                    auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(webView(0)->page());
                    auto r = _record_controller->request_item(
                                 url
                                 , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>> (
                                     ""
                                     , &WebPage::ActiveRecordBinder::binder
                                     , ar
                                 )
                                 , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>> (
                                     ""
                                     , &WebPage::ActiveRecordBinder::activator
                                     , ar
                                 )
                             );
                    r->active();
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
                if(vi->page()->current_item()) {
                    if(vi->page()->current_item()->natural_field_source("pin")
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


                if(vi->page()->current_item()) {
                    QString checkrecordurl = vi->page()->current_item()->natural_field_source("url");
                    assert(checkrecordurl != "");

                    if(vi->page()->current_item()->natural_field_source("url")
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

        connect(_root, &QAction::triggered, this, &WebActionMapper::rootTriggered);
        connect(_root, &QAction::destroyed, this, &WebActionMapper::rootDestroyed);
        _root->setEnabled(false);
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

        connect(action, &QAction::changed, this, &WebActionMapper::childChanged);
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
            disconnect(static_cast<QObject *>(_currentparent), &QObject::destroyed, this, &WebActionMapper::currentDestroyed);

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
        connect(static_cast<QObject *>(_currentparent), &QObject::destroyed, this, &WebActionMapper::currentDestroyed);
    }


    //    PopupWindow::PopupWindow(TabWidget *tabmanager
    //                             , QWebEngineProfile *profile
    //                             , QUrl const &url
    //                             , TableController *_record_controller
    //                             , TableController *_page_controller
    //                             //                             , Browser *parent
    //                            ) :
    //        //        QWidget(nullptr)
    //        //        TabWidget(_record_controller, _page_controller, parent)
    //        Browser(url         // Record *const record
    //                , _record_controller
    //                , _page_controller
    //                , nullptr   //_entrance   // nullptr can not work!
    //                //, QDockWidget *parent
    //                , QString()
    //               )
    //        , _addressbar(new QLineEdit(this))
    //        , _view(nullptr
    //                //              new WebView(record, profile
    //                //                          // , false
    //                //                          , this
    //                //                          , _record_ontroller    // globalparameters.getRecordTableScreen()->getRecordTableController()
    //                //                         )

    //                //            [tabmanager, this, url, profile, _record_controller, _page_controller]
    //                //    {
    //                //        boost::shared_ptr<ActiveRecordBinder> wvh = boost::make_shared<ActiveRecordBinder>(
    //                //                                                        tabmanager
    //                //                                                        , this
    //                //                                                        , profile
    //                //                                                        , _record_controller
    //                //                                                        , _page_controller
    //                //                                                    );
    //                //        boost::intrusive_ptr<Record> record
    //                //            = _record_controller->request_record(
    //                //                  url
    //                //                  , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<Record>>> (
    //                //                      ""
    //                //                      , &ActiveRecordBinder::binder
    //                //                      , wvh
    //                //                  )
    //                //                  , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<Record>>> (
    //                //                      ""
    //                //                      , &ActiveRecordBinder::activator
    //                //                      , wvh
    //                //                  )
    //                //              );  // ->binded_only_page()->view();
    //                //        record->active();
    //                //        return record->unique_page()->view();
    //                //    }()

    //               )
    //    {
    //        //        assert(record);
    //        //        assert(record->binded_only_page());

    //        //        _view->page(new WebPage(profile, url
    //        //                                // , false
    //        //                                , _record_ontroller
    //        //                                , _view));
    //        _view = [tabmanager, this, url, profile, _record_controller, _page_controller] {
    //            boost::shared_ptr<ActiveRecordBinder> wvh = boost::make_shared<ActiveRecordBinder>(
    //                tabmanager
    //                , this
    //                , profile
    //                , _record_controller
    //                , _page_controller
    //            );
    //            boost::intrusive_ptr<Record> record
    //            = _record_controller->request_record(
    //                url
    //                , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<Record>>> (
    //                    ""
    //                    , &ActiveRecordBinder::binder
    //                    , wvh
    //                )
    //                , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *, boost::intrusive_ptr<Record>>> (
    //                    ""
    //                    , &ActiveRecordBinder::activator
    //                    , wvh
    //                )
    //            );  // ->binded_only_page()->view();
    //            record->active();
    //            return record->unique_page()->view();
    //        }();


    //        //        _addressbar->hide();
    //        QVBoxLayout *layout = new QVBoxLayout(this);
    //        layout->setMargin(0);

    //        layout->addWidget(_addressbar);
    //        layout->addWidget(_view);
    //        setLayout(layout);

    //        _view->setFocus();

    //        connect(_view, &WebView::titleChanged, static_cast<QWidget *const>(this), &QWidget::setWindowTitle);
    //        connect(_view, &WebView::urlChanged, this, &PopupWindow::setUrl);
    //        connect(static_cast<QWebEnginePage *>(page()), &QWebEnginePage::geometryChangeRequested, this, &PopupWindow::adjustGeometry);
    //        connect(static_cast<QWebEnginePage *>(page()), &QWebEnginePage::windowCloseRequested, static_cast<QWidget *const>(this), &QWidget::close);
    //    }

    //    //    QWebEnginePage
    //    WebPage *PopupWindow::page() const { return _view->page(); }


    //    void PopupWindow::setUrl(const QUrl &url)
    //    {
    //        _addressbar->setText(url.toString());
    //    }


    //    void PopupWindow::adjustGeometry(const QRect &newGeometry)
    //    {
    //        const int x1 = frameGeometry().left() - geometry().left();
    //        const int y1 = frameGeometry().top() - geometry().top();
    //        const int x2 = frameGeometry().right() - geometry().right();
    //        const int y2 = frameGeometry().bottom() - geometry().bottom();

    //        setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
    //    }


}


