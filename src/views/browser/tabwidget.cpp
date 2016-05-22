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

#include "models/record_table/linker.hxx"
#include "views/record_table/RecordScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/record_table/Record.h"
#include "models/record_table/recordindex.hxx"
#include "models/tree/treeindex.hxx"
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
#include "models/tree/KnowModel.h"
#include "views/tree/TreeScreen.h"
#include "views/tree/KnowView.h"
#include "libraries/DiskHelper.h"
#include "models/tree/binder.hxx"


extern GlobalParameters globalparameters;
extern AppConfig appconfig;
extern QMap<Qt::CheckState, QString> _string_from_check_state;
extern QMap<QString, Qt::CheckState> _state_check_from_string;
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
        //        //        delete _profile;
        //        delete _tabbar;
        //        delete _lineedits;
        //        delete _lineeditcompleter;
        //        // delete _completionModel;
        //        delete _recentlyclosedtabsmenu;
        //        delete _previoustabaction;
        //        delete _nexttabaction;
        //        delete _closetabaction;
        //        delete _newtabaction;
        //        delete _recentlyclosedtabsaction;

        //        if(_fullscreennotification)delete _fullscreennotification;

        //        if(_fullscreenview)delete _fullscreenview;

        //        delete _record_controller;

    }

    TabWidget::TabWidget(TreeScreen      *_tree_screen
        , FindScreen    *_find_screen
        , MetaEditor    *_editor_screen
        , RecordScreen  *_record_screen
        , Entrance      *_entrance
        , Browser       *_browser
        , MainWindow    *_main_window
        , browser::Profile  *_profile
        )
        : QTabWidget(_browser)
        , _tree_screen(_tree_screen)
        , _editor_screen(_editor_screen)
        , _entrance(_entrance)
        , _browser(_browser)
        , _record_controller(new RecordController(_editor_screen
            , this
            , _record_screen
            , _main_window
            )
            )
        , _recentlyclosedtabsaction(new QAction(tr("Recently Closed Tabs"), this))
        , _newtabaction(new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this))
        , _closetabaction(new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this))
        , _nexttabaction(new QAction(tr("Show Next Tab"), this))
        , _previoustabaction(new QAction(tr("Show Previous Tab"), this))
        , _recentlyclosedtabsmenu(new QMenu(this))
        // , _completionModel(new HistoryCompletionModel(this))
        , _lineeditcompleter(nullptr)                         // new QCompleter(_completionModel, this)
        , _lineedits(new QStackedWidget(this))
        , _tabbar(new TabBar(this))
        , _profile(_profile)         // globalparameters.profile()   // QWebEngineProfile::defaultProfile()
        , _fullscreenview(nullptr)
        , _fullscreennotification(nullptr)
        //        , _active_record(this)
        //        , _active("", &active_record::operator(), &_active_record)
        //        , _shadow_branch_root(_shadow_branch_root)
        //        , _shadow_source_model(new TableModel(QString(table_screen_singleton_name) + QString("_shadow"), _tree_item, this))
        //        , _table_data(std::make_shared<TableData>(_tree_item))

    {
        _find_screen->toolbarsearch()->lineedits(_lineedits);

        //        _lineedits = globalparameters.find_screen()->toolbarsearch()->lineedits();
        //        connect(parent, []() {}, globalparameters.find_screen()->toolbarsearch(), [this]() {globalparameters.find_screen()->toolbarsearch()->lineedits(_lineedits);});
        //        move to     void Browser::activateWindow();

        //        _tree_screen->tabmanager = std::make_shared<sd::_interface<sd::meta_info<void *>, TabWidget *>>("", &TabWidget::tabmanager, this);
        //        _find_screen->tabmanager = std::make_shared<sd::_interface<sd::meta_info<void *>, TabWidget *>>("", &TabWidget::tabmanager, this);

        setElideMode(Qt::ElideRight);
        //        _active_record = [this](Record * const record)-> WebView * {return globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);};

        auto tree_view = _tree_screen->tree_view();
        auto current_item = tree_view->current_item();
        auto parent = current_item->parent();

        if(!parent) throw std::exception();

        connect(
            _tabbar
            , &TabBar::newTab             //                , this
            , [&] () {

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host() == current_item && il == current_item->linker() && parent == il->host_parent();
                }));
            } catch(std::exception &e) {}

            if(modelindex) {
                modelindex->item_bind(
                    current_item
                    , QUrl(Browser::_defaulthome)
                    , std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3)
                    , [] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                    return it->field("url") == Browser::_defaulthome;
                }
                    )->activate();
            }

        });

        connect(_tabbar, &TabBar::closeTabSignal, this, &TabWidget::requestCloseTab);
        connect(_tabbar, &TabBar::closeTabSignal, this, &TabWidget::closeTab);          // added by hughvonyoung@gmail.com
        //        connect(_tabbar, &TabBar::cloneTabSignal, this, &TabWidget::cloneTab);
        connect(_tabbar, &TabBar::closeOtherTabsSignal, this, &TabWidget::closeOtherTabs);
        connect(_tabbar, &TabBar::reloadTabSignal, this, &TabWidget::reloadTab);
        connect(_tabbar, &TabBar::reloadAllTabs, this, &TabWidget::reloadAllTabs);
        connect(_tabbar, &TabBar::tabMoved, this, &TabWidget::moveTab);
        setTabBar(_tabbar);

        connect(this, &TabWidget::tabsChanged, this, &TabWidget::onTabsChanged);

        setDocumentMode(true);

        //        _tabbar->setMaximumSize(0, 0);

        _tabbar->hide();

        // Actions
        //        _newtabaction = ;
        _newtabaction->setShortcuts(QKeySequence::AddTab);
        _newtabaction->setIconVisibleInMenu(false);
        connect(
            _newtabaction
            , &QAction::triggered             //                , this
            , [&] (bool make_current) {
            Q_UNUSED(make_current)
            // auto tree_view = _tree_screen->tree_view();

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host() == current_item && il == current_item->linker() && parent == il->host_parent();
                }));
            } catch(std::exception &e) {}

            if(modelindex) {
                modelindex->item_bind(
                    current_item
                    , QUrl(Browser::_defaulthome)
                    , std::bind(&KnowView::view_paste_child, _tree_screen->tree_view(), modelindex, std::placeholders::_2, std::placeholders::_3)
                    , [] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                    return it->field("url") == Browser::_defaulthome;
                }
                    )->activate();
            }

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
        setTabPosition(TabPosition::West);         // South
        setTabShape(TabShape::Triangular);
        setStyleSheet("QTabWidget::tab-bar { max-width: 200px; align: left; text-align: left; margin-left: 2px; padding: 2px;}");         // left: 1px; alignment: left; // does not work
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

    QLineEdit *TabWidget::currentLineEdit() const
    {
        return lineEdit(_lineedits->currentIndex());
    }


    WebView *TabWidget::currentWebView() const
    {
        return webView(currentIndex());
    }

    void TabWidget::currentChanged(int index)   // switch tab
    {
        WebView *view_current = this->webView(index);

        if(view_current) {          // return;

            //            int lc = _lineedits->count();
            //            int c = count();
            //            assert(lc == c);
            Q_ASSERT(_lineedits->count() == count());

            qDebug() << "_lineedits->count()\t" << _lineedits->count() << "\tcount()\n";

            WebView *view_previous = this->webView(_lineedits->currentIndex());

            if(view_previous && view_current != view_previous) {
#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
                disconnect(oldWebView, &PageView::statusBarMessage, this, &TabManager::showStatusBarMessage);
#endif
                disconnect(static_cast<QWebEnginePage *>(view_previous->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
                disconnect(view_previous, &WebView::loadProgress, this, &TabWidget::loadProgress);
                disconnect(view_previous->page()->profile(), &Profile::downloadRequested, this, &TabWidget::downloadRequested);
                disconnect(static_cast<QWebEnginePage *>(view_previous->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

                auto it_current = view_current->page()->item();

                if(it_current) {

                    auto _tree_view = _tree_screen->tree_view();
                    QModelIndex _i = _tree_view->source_model()->index(it_current);

                    if(_i.isValid() && _tree_view->current_index() != _i) {
                        //                        _tree_view->selectionModel()->setCurrentIndex(_i, QItemSelectionModel::SelectionFlag::Current);
                        //                        _tree_view->selectionModel()->select(_i, current_tree_selection_mode);  //
                        auto parent = it_current->parent();
                        boost::intrusive_ptr<TreeIndex> tree_index;
                        try {tree_index = new TreeIndex([&] {return _tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                                return il == it_current->linker() && il->host() == it_current && it_current->parent() == il->host_parent();
                            })); } catch(std::exception &e) {throw e; }

                        _tree_view->select_as_current(tree_index);

                        //                    //                    globalparameters.mainwindow()
                        //                    if(_record_controller->view()->selection_first_id() != _record->field("id")) {
                        //                        _record_controller->select_id(_record->field("id"));
                        //                    }
                    }
                }
            }

#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
            connect(webView, &WebView::statusBarMessage, this, &TabWidget::showStatusBarMessage);
#endif
            connect(static_cast<QWebEnginePage *>(view_current->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
            connect(view_current, &WebView::loadProgress, this, &TabWidget::loadProgress);
            connect(view_current->page()->profile(), &Profile::downloadRequested, this, &TabWidget::downloadRequested);
            connect(static_cast<QWebEnginePage *>(view_current->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

            for(int i = 0; i < _actions.count(); ++i) {
                WebActionMapper *mapper = _actions[i];
                mapper->updateCurrent(view_current->page());
            }

            emit setCurrentTitle(                             //"test"//
                view_current->title()
                );

            _lineedits->setCurrentIndex(index);
            emit loadProgress(view_current->progress());
            emit showStatusBarMessage(view_current->lastStatusBarText());

            if(view_current->page()->url().isEmpty()) {
                _lineedits->currentWidget()->setFocus();
            } else {
                view_current->setFocus();

                //                auto controller = webView->recordtablecontroller();
                auto it_current = view_current->page()->item();

                if(it_current) {                                                 // controller != nullptr &&
                    //                    QModelIndex proxyindex = controller->convertIdToProxyIndex(record->getField("id"));
                    //                    int position = controller->convertProxyIndexToPos(proxyindex);
                    //                    RecordTableView *recordtableview = controller->getView();
                    //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                    view_current->setFocus();

                    auto _tree_view = _tree_screen->tree_view();
                    QModelIndex tree_index_it_current = _tree_view->source_model()->index(it_current);

                    if(tree_index_it_current.isValid()) {
                        if(_tree_view->current_index() != tree_index_it_current) {
                            //                        _tree_view->selectionModel()->setCurrentIndex(_i, QItemSelectionModel::SelectionFlag::Current);
                            boost::intrusive_ptr<TreeIndex> tree_index;
                            try {tree_index = new TreeIndex([&] {return _tree_view->source_model(); }, it_current->parent(), it_current->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                                    return il == it_current->linker() && il->host() == it_current && it_current->parent() == il->host_parent();
                                })); } catch(std::exception &e) {}

                            if(tree_index) {
                                // if(it_current != _tree_view->current_item())
                                _tree_view->select_as_current(tree_index);

                                //                    //                    globalparameters.mainwindow()
                                if(_record_controller->view()->selection_first<IdType>() != it_current->field("id")) {
                                    _record_controller->cursor_to_index(_record_controller->index<PosProxy>(it_current));                                                                                                                                                     // IdType(it->field("id"))
                                }
                            }
                        }
                    } else {
                        auto _index_record_table = _record_controller->source_model()->index(_tree_view->current_item());

                        if(((QModelIndex)_index_record_table).isValid()) {
                            auto pos_source = _record_controller->index<PosSource>(_index_record_table);

                            currentChanged(pos_source);
                        } else {
                            auto tree_current = _tree_view->current_index();

                            if(tree_current.isValid())
                                _tree_view->index_invoke(tree_current);
                            else {
                                auto tree_previous = _tree_view->previous_index();

                                if(tree_previous.isValid())
                                    _tree_view->index_invoke(tree_previous);
                            }
                        }
                    }

                    auto _mainwindow = globalparameters.mainwindow();

                    if(!_mainwindow->windowTitle().contains(view_current->page()->title())) {_mainwindow->setWindowTitle(QString(application_name) + " : " + view_current->page()->title()); }

                    //                    webView->setFocus();
                    MetaEditor *metaeditor = globalparameters.meta_editor();                                                                     // find_object<MetaEditor>(meta_editor_singleton_name);
                    assert(metaeditor);

                    if(metaeditor->item() != it_current) {

                        view_current->page()->sychronize_metaeditor_to_item();                                                                                         // metaeditor->bind(record);
                    }
                }
            }
        }
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

//    void TabWidget::lineEditStack(QStackedWidget *lineedits)
//    {
//        _lineedits = lineedits;
//    }




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


    WebView *TabWidget::view_no_pinned()
    {
        int found = 0;
        WebView *r = nullptr;

        for(int i = 0; i < count(); i++) {
            WebView *v = webView(i);
            WebPage *p = v->page();

            if(p) {
                auto it = p->item();

                if(it && it->field("pin") == _string_from_check_state[Qt::Unchecked]) {
                    found++;

                    if(found == 1) {
                        r = webView(i);
                    } else {
                        closeTab(i);
                    }
                }
            }
        }

        return r;
    }


    QAction *TabWidget::newTabAction() const
    {
        return _newtabaction;
    }


    WebView *TabWidget::newTab(boost::intrusive_ptr<RecordIndex> record_modelindex   // boost::intrusive_ptr<TreeItem> tab_brother, boost::intrusive_ptr<TreeItem> target
        , bool make_current)                          // , bool openinnewtab   // , const TreeScreen::paste_strategy &_view_paste_strategy // , equal_t _equal
    {
        boost::intrusive_ptr<TreeItem> result(nullptr);
        // auto _record_model = modelindex.current_model();
        boost::intrusive_ptr<TreeItem> tab_brother = record_modelindex->sibling();
        boost::intrusive_ptr<TreeItem> target = record_modelindex->target();
        assert(target);
        assert(tab_brother != target);
        // assert(!target->is_lite());

        if(target->is_lite()) target->to_fat();

        //        if(record == nullptr) {
        //            record = register_record(QUrl(DockedWindow::_defaulthome));
        //        } else {
        //            record = register_record(QUrl(record->getNaturalFieldSource("url")));
        //        }

        WebView *view = find([&] (boost::intrusive_ptr<const TreeItem> it) {
            return it->id() == target->id();
        });                                                                                                                      // view_no_pinned();   // nullptr;   // find(record);

        if(view == nullptr) {
            view = find([&] (boost::intrusive_ptr<const TreeItem> it) {
                return it->field("url") == target->field("url");
            });

            if(view == nullptr) {
                //        if(view == nullptr) {




                //                // line edit, move after new WebView!!!
                //                UrlLineEdit *urlLineEdit = new UrlLineEdit;
                //                QLineEdit *lineEdit = urlLineEdit->lineEdit();

                //                if(!_lineeditcompleter && count() > 0) {
                //                    HistoryCompletionModel *completionModel = new HistoryCompletionModel(this);
                //                    completionModel->setSourceModel(QtSingleApplication::historyManager()->historyFilterModel());
                //                    _lineeditcompleter = new QCompleter(completionModel, this);
                //                    // Should this be in Qt by default?
                //                    QAbstractItemView *popup = _lineeditcompleter->popup();
                //                    QListView *listView = qobject_cast<QListView *>(popup);

                //                    if(listView)
                //                        listView->setUniformItemSizes(true);
                //                }

                //                lineEdit->setCompleter(_lineeditcompleter);
                //                connect(lineEdit, &QLineEdit::returnPressed, this, &TabWidget::lineEditReturnPressed);
                //                _lineedits->addWidget(urlLineEdit);
                //                _lineedits->setSizePolicy(lineEdit->sizePolicy());




                // optimization to delay creating the more expensive WebView, history, etc
                //        if(count() == 0) {return new_dummy();}

                //        // webview
                //        if(!record->page_valid()
                //           //           && !record->unique_page()
                //          ) {

                view = new WebView(target
                        , _profile                                                            // use record for return
                        , _tree_screen
                        , _editor_screen
                        , _entrance
                        , _browser
                        , this
                        , _record_controller
                        );
                //        } else {
                //            view = record->unique_page()->view();
                //        }

                //        record->view(webView);  // inside PageView initialization
                //webView->setPage(new WebPage(_profile, webView));

                //        assert(item->page_valid() && item->unique_page());
                assert(view);
                assert(view->page()->binder()->integrity_internal());
                assert(view->page()->binder()->integrity_external(target, view->page()));



                // line edit
                UrlLineEdit *urlLineEdit = new UrlLineEdit(view);
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





                //                urlLineEdit->setWebView(view);    // move inside UrlLineEdit::UrlLineEdit

                connect(view, &WebView::loadStarted, this, &TabWidget::webViewLoadStarted);
                connect(view, &WebView::iconChanged, this, &TabWidget::webViewIconChanged);
                connect(view, &WebView::titleChanged, this, &TabWidget::webViewTitleChanged);
                connect(view, &WebView::urlChanged, this, &TabWidget::webViewUrlChanged);
                connect(static_cast<QWebEnginePage *>(view->page()), &QWebEnginePage::windowCloseRequested, this, &TabWidget::windowCloseRequested);
                connect(static_cast<QWebEnginePage *>(view->page()), &QWebEnginePage::geometryChangeRequested, this, &TabWidget::geometryChangeRequested);

#if             defined(QWEBENGINEPAGE_PRINTREQUESTED)
                connect(view->page(), &WebPage::printRequested, this, &TabWidget::printRequested);
#endif

#if             defined(QWEBENGINEPAGE_MENUBARVISIBILITYCHANGEREQUESTED)
                connect(view->page(), &WebPage::menuBarVisibilityChangeRequested, this, &TabWidget::menuBarVisibilityChangeRequested);
#endif

#if             defined(QWEBENGINEPAGE_STATUSBARVISIBILITYCHANGEREQUESTED)
                connect(view->page(), &WebPage::statusBarVisibilityChangeRequested, this, &TabWidget::statusBarVisibilityChangeRequested);
#endif

#if             defined(QWEBENGINEPAGE_TOOLBARVISIBILITYCHANGEREQUESTED)
                connect(view->page(), &WebPage::toolBarVisibilityChangeRequested, this, &TabWidget::toolBarVisibilityChangeRequested);
#endif

                //                int index = addTab(view, target->field("name").leftJustified(5, '.', true)); //, tr("(Untitled)")
                int index = insertTab(tab_brother ? tab_brother->binder() ? webViewIndex(tab_brother->binder()->page()->view()) + 1 : 0 : 0, view, target->field("name").leftJustified(5, '.', true));

                setTabToolTip(index, target->field("name"));

                //record->page()->load(record);
                //globalparameters.entrance()->invoke_view(record);

                //        assert(item->page_valid() && item->unique_page());

                //        int lc = _lineedits->count();
                //        int c = count();
                //        assert(lc == c);
                //        Q_ASSERT(_lineedits->count() == count());
                // webview actions
                for(int i = 0; i < _actions.count(); ++i) {
                    WebActionMapper *mapper = _actions[i];
                    mapper->addChild(view->page()->action(mapper->webAction()));
                }

                result = view->page()->item();
                //else {
                assert(result->binder()->integrity_external(result, view->page()));

            } else {                                     // url is equal, id is not equal
                //                auto _record_binder = view->page()->binder();

                //                if(_record_binder && _record_binder->item() != target) {
                //                    view->page()->binder_reset(); // _record_binder->break_page();    // view->page()->item_break(_record_binder->item());
                //                    // view->page()->binder(nullptr);   // _record_binder.reset();
                //                    //                    _record_binder->item() = target;
                //                    //                    _record_binder->page() = view->page();
                //                    //                    result = target;

                //                    result = view->page()->item_bind(target);
                //                    assert(result->binder()->integrity_external(result, view->page()));
                //                }

                //                assert(result->binder()->integrity_external(result, view->page()));

                auto page_item = view->page()->item();
                boost::intrusive_ptr<TreeIndex> tree_index;
                try {tree_index = new TreeIndex([&] () {
                        return _tree_screen->tree_view()->source_model();
                    }, page_item); } catch(std::exception &e) {throw e; }

                if(page_item != target) page_item = _tree_screen->tree_view()->view_merge(tree_index, target);

                //            if(!target->binder())target->binder(std::forward<boost::intrusive_ptr<TreeItem::coupler>>(view->page()->binder()));

                result = page_item;                                                 // target;
                assert(result->binder());
                assert(result->binder()->integrity_external(result, view->page()));

            }

            assert(result->binder());
            assert(result->binder()->integrity_external(result, view->page()));
        } else {                    // id is equal
            auto page_item = view->page()->item();
            boost::intrusive_ptr<TreeIndex> tree_index;
            try {tree_index = new TreeIndex([&] () {
                    return _tree_screen->tree_view()->source_model();
                }, page_item); } catch(std::exception &e) {throw e; }

            if(page_item != target) page_item = _tree_screen->tree_view()->view_merge(tree_index, target);

            //            if(!target->binder())target->binder(std::forward<boost::intrusive_ptr<TreeItem::coupler>>(view->page()->binder()));

            result = page_item;                             // target;
            assert(result->binder());
            assert(result->binder()->integrity_external(result, view->page()));
        }


        assert(result == view->page()->binder()->item());          // old one choosed
        assert(result == target || result->field("url") == target->field("url"));
        //        assert(result->record_binder()->bounded_page());

        if(!result->binder()->page()) {
            result->binder()->page(view->page());                             //            result->bind();   // this is recursively call
        }

        assert(result->binder()->item());          // 0
        assert(result->binder()->page());          // 1
        assert(view->page()->binder());                 // 8
        assert(result->binder() == view->page()->binder());         // 10
        assert(result->binder()->integrity_external(result, view->page()));
        //            result->record_binder()->binder(result);
        //}

        // result->record_binder()->binder();   //recursive calling


        if(make_current) {
            setCurrentWidget(view);
            auto _tree_view = _tree_screen->tree_view();
            QModelIndex _i = _tree_view->source_model()->index(result->binder()->item());

            if(_i != _tree_view->current_index() && result->binder()->integrity_external(result, view->page()))
                _tree_view->selectionModel()->select(_i, QItemSelectionModel::SelectionFlag::Select);                                         // _tree_view->selectionModel()->select(_i, QItemSelectionModel::SelectionFlag::Deselect);

            //                    _tree_view->select_and_current(_record_binder->bounded_item());
        }

        if(count() == 1)
            currentChanged(currentIndex());                     // default this is input the new index

        emit tabsChanged();
        //webView->openLinkInNewTab(QUrl());    // don't need, initialized
        //        }

        assert(view);
        //        assert(item->page_valid() && item->unique_page());
        assert(!result->is_lite());



        //        if(!_record_controller->source_model()->find(item))
        _record_controller->update_record_view(result);        //        _record_controller->addnew_item_fat(item);

        //        item->activate(); // activate after initialization of browser


        //        view_no_pinned();

        return view;         // TabWidget::newTabFull::WebView *
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

//    // When index is -1 index chooses the current tab
//    void TabWidget::cloneTab(int index)
//    {
//        if(index < 0)
//            index = currentIndex();

//        if(index < 0 || index >= count())
//            return;

//        assert(webView(index)->page()->item()->page_valid() && webView(index)->page()->item()->page());
//        assert(webView(index)->page()->item()->is_registered_to_browser() || webView(index)->page()->item()->field("url") == browser::Browser::_defaulthome);

//        newTab(webView(index)->page()->item(), webView(index)->page()->item(), true);

//    }


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

    QAction *TabWidget::closeTabAction() const
    {
        return _closetabaction;
    }

// When index is -1 index chooses the current tab
    void TabWidget::closeTab(int index)
    {
        int current_index = currentIndex();

        if(index != current_index) index = current_index;

        //        if(index < 0)index = currentIndex();

        if(index >= 0 && index < count()) {         // if(index < 0 || index >= count())
            // return;

            bool hasFocus = false;

            if(WebView *_view_to_close = webView(index)) {
                assert(widget(index) == _view_to_close);                                                 // debug
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
                hasFocus = _view_to_close->hasFocus();

                if(_profile == globalparameters.profile()                                                 // QWebEngineProfile::defaultProfile()
                    ) {
                    _recentlyclosedtabsaction->setEnabled(true);
                    _recentlyclosedtabs.prepend(_view_to_close->page()->url());

                    if(_recentlyclosedtabs.size() >= TabWidget::_recentlyclosedtabssize)
                        _recentlyclosedtabs.removeLast();
                }


                QWidget *lineEdit = _lineedits->widget(index);

                if(lineEdit) {
                    _lineedits->removeWidget(lineEdit);

                    lineEdit->deleteLater();                                                                     //delete lineEdit;
                }

                auto it = _view_to_close->page()->item();
                assert(it->page() == _view_to_close->page());

                //                auto _tree_screen = _tree_screen;
                _tree_screen->tree_view()->selectionModel()->select(_tree_screen->tree_view()->source_model()->index(it), QItemSelectionModel::SelectionFlag::Deselect);
                //                _tree_screen->tree_view()->select_and_current(it, [](KnowView * v, const QModelIndex & _i)->QModelIndex {
                //                    v->selectionModel()->select(_i, QItemSelectionModel::SelectionFlag::Deselect);
                //                    assert(_i != v->selectionModel()->selectedIndexes().last());
                //                    return v->selectionModel()->selectedIndexes().last();
                //                });




                //                if(!_record_controller->source_model()->is_item_exists(to_be_closed_view->page()->current_item()->id())) {
                //                    assert(_record_controller->source_model()->size() == _tabbar->count());
                //                } else {
                //                    _record_controller->source_model()->remove_child(to_be_closed_view->page()->current_item());
                //                    to_be_closed_view->page()->break_records(); // same as _record_controller->source_model()->remove_child()
                //                }

                removeTab(index);

                emit _view_to_close->close_requested();                                                 // _record_controller->remove_child(to_be_closed_view->page()->current_item()->id());
                // delete to_be_closed_view;
                _view_to_close->deleteLater();

                // move to WebView::on_close_requested
                //                int tab_widget_count = count();
                //                int tab_bar_count = _tabbar->count();
                //                int source_model_size = _record_controller->source_model()->size();

                //                if(source_model_size > tab_widget_count) {
                //                    bool found = false;

                //                    for(int i = 0; i < source_model_size; i++) {
                //                        if(_record_controller->source_model()->child(i)->unique_page() == to_be_closed_view->page()) {
                //                            _record_controller->remove_child(_record_controller->source_model()->child(i)->id());
                //                            found = true;
                //                            break;
                //                        }
                //                    }

                //                    assert(found == true);
                //                }



            }

            // move forward before removeTab(index);
            //            QWidget *lineEdit = _lineedits->widget(index);

            //            if(lineEdit) {
            //                _lineedits->removeWidget(lineEdit);
            //                //delete lineEdit;    //
            //                lineEdit->deleteLater();
            //            }



            emit tabsChanged();

            if(hasFocus && count() > 0) {
                WebView *current_view = nullptr;
                current_view = currentWebView();

                if(current_view != nullptr && current_view != webView(index)) {
                    current_view->setFocus();
                    boost::intrusive_ptr<TreeItem> aurrent_item = current_view->page()->item();

                    if(aurrent_item) {
                        assert((aurrent_item->page_valid() && aurrent_item->page()) || (aurrent_item->field("url") == Browser::_defaulthome));

                        //                    QModelIndex proxyindex = view->recordtablecontroller()->convertIdToProxyIndex(record->getField("id"));
                        //                    int position = view->recordtablecontroller()->convertProxyIndexToPos(proxyindex);
                        //                    RecordTableView *recordtableview = view->recordtablecontroller()->getView();

                        //                    if(recordtableview)recordtableview->setSelectionToPos(position); // work
                        current_view->setFocus();

                        //                        globalparameters.mainwindow()
                        if(_record_controller->view()->selection_first<IdType>() != aurrent_item->field("id"))
                            _record_controller->cursor_to_index(_record_controller->index<PosProxy>(aurrent_item));                                                                                                     // IdType(item->field("id"))
                    }
                }
            }

            //            int tab_widget_count = count();
            //            int tab_bar_count = _tabbar->count();
            //            int source_model_size = _record_controller->source_model()->size();

            assert(count() == _tabbar->count());
            assert(_record_controller->source_model()->size() == _tabbar->count());

            //            if(count() == 0)
            if(_tabbar->count() == 0)
                emit lastTabClosed();
        }
    }

//deprecated, profile should be assigned at the construction of BrowserPage
    void TabWidget::setProfile(Profile *profile // QWebEngineProfile *profile
        )
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
        auto _real_title = title.leftJustified(5, '.', true);

        if(-1 != index) {
            setTabText(index, _real_title);
        }

        if(currentIndex() == index)
            emit setCurrentTitle(
                _real_title                         // "test"//
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
            auto tree_view = _tree_screen->tree_view();

            auto current_item = tree_view->current_item();
            auto parent = current_item->parent();

            if(!parent) throw std::exception();

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host() == current_item && il == current_item->linker() && parent == il->host_parent();
                }));
            } catch(std::exception &e) {}

            if(modelindex) {
                modelindex->item_bind(
                    current_item
                    , QUrl(Browser::_defaulthome)
                    , std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3)
                    , [] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                    return it->field("url") == Browser::_defaulthome;
                }
                    )->activate();
            }

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

            if(view) view->resizeEvent(e);
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

            auto tree_view = _tree_screen->tree_view();

            auto current_item = tree_view->current_item();
            auto parent = current_item->parent();

            if(!parent) throw std::exception();

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host() == current_item && il == current_item->linker() && parent == il->host_parent();
                }));
            } catch(std::exception &e) {}

            if(modelindex) {
                if(!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
                    modelindex->item_bind(
                        current_item
                        , url
                        , std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3)
                        , [&] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                        return it->field("url") == url.toString();
                    }
                        )->activate();
                }
            }
        }
    }

    void TabWidget::loadUrlInCurrentTab(const QUrl &_url)
    {
        WebView *webView = currentWebView();

        if(webView) {
            //            Record *record;
            boost::intrusive_ptr<TreeItem> _item = webView->page()->item();

            if(_item->page_valid() && _item->page()->url() != _url) {

                auto tree_view = _tree_screen->tree_view();
                auto current_item = tree_view->current_item();
                auto parent = current_item->parent();

                if(!parent) throw std::exception();

                boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

                try {
                    modelindex = new TreeIndex([&] {return tree_view->source_model(); }, parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                        return il->host() == current_item && il == current_item->linker() && parent == il->host_parent();
                    }));
                } catch(std::exception &e) {}

                auto it = modelindex->item_register(_url, std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3), [&] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                    return it->field("url") == _url.toString();
                });


                //                boost::intrusive_ptr<RecordModel::ModelIndex> record_modelindex(nullptr);

                //                try {
                //                    record_modelindex = new RecordModel::ModelIndex([&] {return _record_controller->source_model();}, _record_controller->source_model()->sibling(it), it);
                //                } catch(std::exception &e) {}

                //                if(record_modelindex) {
                auto r =                                                 // _record_controller
                    webView->page()->item_bind(it);
                //                        _url
                //                        , std::bind(&TreeScreen::view_paste_as_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)


                r->activate();
                //                }
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

        QStringList tabs_url;

        for(int i = 0; i < count(); ++i) {
            if(WebView *view = qobject_cast<WebView *>(widget(i))) {
                tabs_url.append(
                    view->page()->url().toString()
                    );
            } else {
                tabs_url.append(QString::null);
            }
        }

        stream << tabs_url;
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

        QStringList open_tabs;
        stream >> open_tabs;

        for(int i = 0; i < open_tabs.count(); ++i) {
            auto _url = open_tabs.at(i);
            //            Record *_record = request_record(_url);

            auto tree_view = _tree_screen->tree_view();

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model(); }, tree_view->current_item()->parent(), tree_view->current_item()->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host() == tree_view->current_item() && il == tree_view->current_item()->linker() && tree_view->current_item()->parent() == il->host_parent();
                }));
            } catch(std::exception &e) {}

            if(modelindex) {
                if(i != 0) {
                    modelindex->item_bind(
                        tree_view->current_item()
                        , _url
                        , std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3)
                        , [&] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                        return it->field("url") == _url;
                    }
                        )->activate();
                } else {
                    if(webView(0)->page()->url() != _url) {
                        //                    webView(0)->load(_record);    //loadUrl(_url);
                        //                    auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(webView(0)->page());
                        auto it = modelindex->item_register(QUrl(_url), std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3), [&] (boost::intrusive_ptr<const TreeItem> it) -> bool {
                            return it->field("url") == _url;
                        });
                        //                        boost::intrusive_ptr<RecordModel::ModelIndex> record_modelindex(nullptr);

                        //                        try {
                        //                            record_modelindex = new RecordModel::ModelIndex([&] {return _record_controller->source_model();}, _record_controller->source_model()->sibling(it), it);
                        //                        } catch(std::exception &e) {}

                        webView(0)->page()->item_bind(it)->activate();
                    }
                }
            }
        }

        int currentTab;
        stream >> currentTab;
        setCurrentIndex(currentTab);

        return true;
    }

    void TabWidget::current_download_acceptance(std::pair<QUrl, bool> policy){
        _current_download_acceptance = policy;
    }

    std::pair<QUrl, bool> TabWidget::current_download_acceptance() const {
        return _current_download_acceptance;
    }


    void TabWidget::downloadRequested(QWebEngineDownloadItem *download)
    {
        int ret = !QMessageBox::Ok;

        if(!_current_download_acceptance.second && _current_download_acceptance.first != download->url()) {
            QMessageBox message_box;

            message_box.setText(tr("Do you want to download the file?"));
            message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            message_box.setDefaultButton(QMessageBox::Cancel);
            ret = message_box.exec();
        }

        auto state = download->state();
        if(ret == QMessageBox::Ok) {_current_download_acceptance = {download->url() ,true};}
        if(_current_download_acceptance.second) {
            if(state == QWebEngineDownloadItem::DownloadRequested || state == QWebEngineDownloadItem::DownloadInProgress) {
                // download->accept();  // default in construction
                QtSingleApplication::downloadManager()->download(this, download);
            }else{  // if(state == QWebEngineDownloadItem::DownloadCompleted || state == QWebEngineDownloadItem::DownloadCancelled || state == QWebEngineDownloadItem::DownloadInterrupted) {
                _current_download_acceptance.second = false;
            }
        }else{
            download->cancel();
            _current_download_acceptance.first = download->url();
        }
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


        //        auto _tree_view = _tree_screen->tree_view();
        //        QModelIndex _current = _tree_view->current_index();

        //        for(int i = 0; i < count(); i++) {
        //            auto vi = webView(i);

        //            if(vi != nullptr) {
        //                auto it = vi->page()->bounded_item();

        //                if(it) {

        //                    // _tree_view->select_and_current(it);
        //                    QModelIndex _i = _tree_view->source_model()->index(it);
        //                    _tree_view->selectionModel()->select(_i, QItemSelectionModel::SelectionFlag::Select);
        //                }
        //            }
        //        }

        //        _tree_view->select_and_current(_current);
    }

    boost::intrusive_ptr<TreeItem> TabWidget::sibling(boost::intrusive_ptr<TreeItem> it) const
    {
        boost::intrusive_ptr<TreeItem> r;
        auto v = it->binder()->page()->view();
        auto index = webViewIndex(v);

        if(index != -1 && index > 0) r = webView(index - 1)->page()->binder()->item();

        return r;
    }

    WebView *TabWidget::find_nopin() const
    {
        WebView *bv = nullptr;

        for(int i = 0; i < count(); i++) {
            auto vi = webView(i);

            if(vi != nullptr) {
                if(vi->page()->item()) {
                    if(vi->page()->item()->field("pin") == _string_from_check_state[Qt::Unchecked]) {
                        bv = vi; break;
                    }
                }
            }
        }

        return bv;
    }

//    template<>
//    WebView *TabWidget::find<url_full>(const QUrl &find_url)const
//    {
//        WebView *view = nullptr;

//        for(int i = 0; i < count(); i++) {
//            auto current_view = webView(i);

//            if(current_view != nullptr) {
//                auto it = current_view->page()->bounded_item();

//                if(it) {

//                    if(it->url() == find_url) {
//                        view = current_view;
//                        break;
//                    }

//                }
//            }
//        }

//        return view;
//    }

//    template<>
//    WebView *TabWidget::find<url_fragment>(const QUrl &find_url)const
//    {
//        WebView *view = nullptr;
//        //        boost::intrusive_ptr<TreeItem> re;
//        //if(record) {

//        for(int i = 0; i < count(); i++) {
//            auto current_view = webView(i);

//            if(current_view != nullptr) {
//                auto it = current_view->page()->bounded_item();

//                //                //                Record *checkrecord = vi->page()->record();
//                //                QString checkurl = vi->page()->url().toString();    // debug
//                //                QString checktitle = vi->page()->title();           // debug
//                if(it) {

//                    QString s_item_url = it->field("url");
//                    assert(s_item_url != "");
//                    // QString _url = find_url.toString();
//                    // QUrl find_url = QUrl(_url);
//                    QUrl item_url = QUrl(s_item_url);
//                    // QString s_find_url = find_url.toString();
//                    // QString s_item_url = item_url.toString();
//                    // QUrl page_url = vi->page()->url();
//                    // QString s_page_url = page_url.toString();
//                    // int i = 0;

//                    if(item_url.fragment() == find_url.fragment()  //it->field("url") == _url
//                       //.toString()
//                      ) {   // record->getNaturalFieldSource("url")
//                        view = current_view;
//                        //                        re = it;
//                        break;
//                    }

//                }
//            }
//        }

//        //}

//        return view;
//    }


//    template<typename url_type = url_full>
    WebView *TabWidget::find(const std::function<bool(boost::intrusive_ptr<const TreeItem>)> &_equal) const  // const QUrl &_find_url, equal_type _equal
    {
        WebView *view = nullptr;

        for(int i = 0; i < count(); i++) {
            auto current_view = webView(i);

            if(current_view != nullptr) {
                auto it = current_view->page()->item();

                if(it) {
                    if(_equal(it)) {                                                                     //it->url<url_type>() == url_type()(find_url)
                        view = current_view;
                        break;
                    }

                }
            }
        }

        return view;
    }

//    WebView *TabWidget::find(boost::intrusive_ptr<const TreeItem> it_find)const
//    {
//        // boost::intrusive_ptr<TreeItem> re;
//        WebView *bv = nullptr;

//        for(int i = 0; i < count(); i++) {
//            if(webView(i)->page()->bounded_item() == it_find) {
//                bv = webView(i); //re = webView(i)->page()->bounded_item();
//                break;
//            }
//        }

//        return bv;
//    }

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





//    boost::intrusive_ptr<TreeItem> TabWidget::item_request_from_tree_impl(const QUrl &_url)
//    {
//        TreeScreen *_tree_screen    = _tree_screen;
//        //    auto _know_model_root = tree_screen->know_root();
//        auto _know_model_board      = _tree_screen->know_model_board();
//        auto _current_view_model    = _tree_screen->tree_view()->source_model();

//        boost::intrusive_ptr<TreeItem> _result(nullptr);    // =  _know_model_board->root_item();
//        //    boost::intrusive_ptr<TreeItem> _source_root_item = tree_screen->know_branch()->item(TreeModel::delegater(_url));    // on know_root semantic
//        boost::intrusive_ptr<TreeItem> _source_item = _know_model_board->item_by_url(_url);

//        //    if(_source_root_item && !_source_item) {
//        //        auto result = tree_screen->cut_from_root(_source_root_item);

//        //        if(result)_source_item = tree_screen->paste_to_branch(result, _know_model_branch);

//        //        assert(result);
//        //        assert(_source_item);
//        //        assert((_source_item == result) && (result == _source_root_item));
//        //    }

//        bool item_is_brand_new = false;
//        //    //    if(_record_controller) {
//        //    auto browser_pages = this->_source_model->browser_pages();
//        //    assert(browser_pages);

//        //    if(browser_pages) {
//        auto v = find(_url);

//        if(v) {
//            _result = v->page()->record_binder()->bounded_item();
//            assert(_result->url() == _url);
//        }

//        if(_source_item && _source_item != _know_model_board->root_item()) {
//            if(!_result) {

//                if(_source_item->is_lite())_source_item->to_fat();

//                //            //            _source_item->binder(generator);
//                //            //            _source_item->activator(activator);

//                //            //            _item = register_item_to_browser_source_model(_source_item);

//                //            _source_item->is_registered_to_record_controller_and_tabmanager(false);
//                //            // _source_item->self_bind();
//                _result = _source_item;
//            } else {
//                assert(_result->url() == _url);
//                // assert(_result->fragment() == _source_item->fragment());

//                if(_result != _source_item) {
//                    _result = _source_item; // assert(_item.get() == _source_item.get());
//                    // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, _source_item);
//                }

//                if(_result->is_lite())_result->to_fat();

//                if(_result->field("id") == "")_result->field("id", get_unical_id());

//                assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

//                //            //            _result->binder(generator);
//                //            //            _result->activator(activator);

//                //            _result->is_registered_to_record_controller_and_tabmanager(false);
//                //            // _item->self_bind();
//            }

//            assert(!_result->is_lite());
//            //        assert(_result->is_registered_to_record_controller_and_tabmanager());

//        } else {
//            item_is_brand_new = true;

//            if(!_result) {

//                //                int pos = _record_ontroller->getFirstSelectionPos();
//                //                Record *previous_record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

//                //                if(previous_record) {

//                //                    Record record;

//                //                    if(record.isLite())record.switchToFat();

//                //                    //QString title = d->view->title(); // not ready yet
//                //                    //record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));   // id concept?
//                //                    record.setNaturalFieldSource("pin",   "");
//                //                    record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
//                //                    record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
//                //                    record.setNaturalFieldSource("url",    _url.toString());    // only changed
//                //                    record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

//                //                    _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
//                //                    _record = recordtabledata->getRecordByUrl(_url);
//                //                    //                int pos = _record_ontroller->getFirstSelectionPos();
//                //                    //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);
//                //                } else {




//                //    record.generator(generator);


//                // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
//                QString directory = DiskHelper::create_temp_directory();  //

//                QMap<QString, QString> data;
//                data["id"]      = get_unical_id();
//                data["pin"]     = _string_from_check_state[Qt::Unchecked];
//                data["name"]    = "";
//                data["author"]  = "";
//                data["home"]    = _url.toString();
//                data["url"]     = _url.toString();
//                data["tags"]    = "";
//                data["dir"]     = data["id"];
//                data["file"]    = "text.html";

//                boost::intrusive_ptr<TreeItem> item
//                    = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

//                //                if(record.isLite())
//                item->to_fat();
//                item->text_to_fat("");
//                //            //                QString title = _url.toString(); // not ready yet
//                //            item->field("id",       get_unical_id());
//                //            item->field("pin",      _check_state[Qt::Unchecked]);
//                //            item->field("name",     "");
//                //            item->field("author",   "");
//                //            item->field("home",     _url.toString());    // only changed
//                //            item->field("url",      _url.toString());    // only changed
//                //            item->field("tags",     "");

//                //            //                _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
//                //            //                _record = recordtabledata->getRecordByUrl(_url);
//                //            //                //                int pos = _record_ontroller->getFirstSelectionPos();
//                //            //                //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

//                //            //                //            }

//                //            //                record->binder(generator);
//                //            //                record->activator(activator);

//                item->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));


//                // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
//                // Запись должна быть создана, потом можно аттачить файлы.
//                // Это ограничение для "ленивого" программинга, но пока так
//                // record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

//                // Временная директория с картинками и приаттаченными файлами удаляется
//                DiskHelper::remove_directory(directory);

//                if(item->is_lite())item->to_fat();

//                //            //            item->binder(generator);
//                //            //            item->activator(activator);

//                //            //            _item = register_item_to_browser_source_model(item);
//                //            item->is_registered_to_record_controller_and_tabmanager(true);
//                //            // item->self_bind();
//                //            //                assert(_record);
//                //            //                assert(_record->is_registered());
//                //            //                _record->active_immediately(active_immediately);
//                //            //                _record->generator(generator);


//                _result = item; // assert(_item.get() == item.get());
//            } else {
//                assert(_result->url() == _url);

//                if(_result->is_lite())_result->to_fat();

//                if(_result->field("id") == "")_result->field("id", get_unical_id());

//                assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

//                //            //            _result->binder(generator);
//                //            //            _result->activator(activator);

//                //            _result->is_registered_to_record_controller_and_tabmanager(true);
//                //            // _item->self_bind();
//            }

//            {
//                auto _view_index = _tree_screen->tree_view()->index_current();

//                // if(idx.isValid()) {

//                auto it = _current_view_model->item(_view_index);  //item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
//                assert(it);
//                assert(_current_view_model->is_id_exists(it->id()));

//                //        if(_item->is_lite())_item->to_fat();

//                if(it != _result && item_is_brand_new) {
//                    assert(_result->url() == _url);
//                    // assert(_result->fragment() == _url.fragment());
//                    // int pos
//                    _result = _tree_screen->branch_add(_view_index, _result, true, _current_view_model); // it->insert_new_item(it->current_count() - 1, _result);
//                    assert(_result);
//                    _tree_screen->synchronized(false);
//                    // assert(_result == it->child(pos));
//                }

//                _tree_screen->knowtree_save();
//                // }
//            }
//        }

//        if(_result->is_lite())_result->to_fat();

//        //    //        //            else {
//        //    //        //                //                assert(_record->is_registered());
//        //    //        //                _record->binder(generator);
//        //    //        //                _record->activator(activator);
//        //    //        //                //                _record->generate();    // why?
//        //    //        //            }

//        //    _result->record_binder(_record_binder);
//        //    //    _result->activator(activator);

//        assert(_result != _know_model_board->root_item());
//        //    //    assert(_result->is_registered_to_record_controller_and_tabmanager());
//        //    assert(_result->field("url") == _url.toString());   // maybe other url loaded !
//        //    //    } // browser_pages

//        //    //    }

//        //    //    }

//        if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

//        if(_result->field("file") == "")_result->field("file", "text.html");

//        //    //    assert(_record);
//        assert(_result->url() == _url);
//        // assert(_result->fragment() == _url.fragment());
//        return _result;
//    }


//    boost::intrusive_ptr<TreeItem> TabWidget::item_request_from_tree_fragment(const QUrl &_url)
//    {
//        boost::intrusive_ptr<TreeItem> re;
//        auto it = item_request_from_tree_impl<url_fragment>(_url);

//        re = item_registered_resetup_record_binder(it);

//        return re;
//    }

//    boost::intrusive_ptr<TreeItem> TabWidget::item_request_from_tree(const QUrl &_url)
//    {
//        boost::intrusive_ptr<TreeItem> re;
//        auto it = item_request_from_tree_impl<url_full>(_url);

//        re = item_registered_resetup_record_binder(it);

//        return re;
//        //        auto ar = boost::make_shared<TabWidget::Coupler>(this, true);

//        //        //        bind_helper binder = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
//        //        //                             >>("", &TabWidget::RecordBinder::binder, ar);
//        //        //        active_helper activator = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &TabWidget::RecordBinder::activator, ar);

//        //        return _record_controller->item_request_from_tree(_url
//        //                                                          , std::make_shared<CouplerDelegation>(
//        //                                                              std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
//        //                                                              , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
//        //                                                              , std::make_shared<bind_interface>("", &Coupler::binder, ar)
//        //                                                              , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
//        //                                                          ));
//    }

//    boost::intrusive_ptr<TreeItem> TabWidget::item_bind(boost::intrusive_ptr<TreeItem> item)
//    {
//        //        auto binder = [](boost::shared_ptr<TabWidget::RecordBinder> ar) {
//        //            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>   // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
//        //                   >> ("", &TabWidget::RecordBinder::binder, ar);
//        //        };
//        //        auto activator = [](boost::shared_ptr<TabWidget::RecordBinder> ar) {
//        //            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>> ("", &TabWidget::RecordBinder::activator, ar);
//        //        };

//        // registered record, but have no generator:
//        //        boost::shared_ptr<TabWidget::Coupler> ar =


//        item->binder(std::make_shared<TreeItem::coupler>(std::make_shared<TabWidget::coupler>(this, item)));

//        //        std::make_shared<TabWidget::Coupler>(this, item);

//        //        //        record->binder(
//        //        //            binder(ar)
//        //        //        );

//        //        //        record->activator(
//        //        //            activator(ar)
//        //        //        );

//        //        item->record_binder(std::make_shared<CouplerDelegation>(
//        //                                std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
//        //                                , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
//        //                                , std::make_shared<bind_interface>("", &Coupler::binder, ar)
//        //                                , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
//        //                            ));
//        assert(item->binder());
//        return  item;
//    }

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





//    // Слот для добавления новой записи в конец таблицы
//    void TabWidget::addnew_to_end(void)
//    {
//        _record_controller->addnew_to_end();    // addnew_blank(ADD_NEW_RECORD_TO_END);
//    }
//    // Слот для добавления новой записи перед выделенной строкой
//    void TabWidget::addnew_before(void)
//    {
//        _record_controller->addnew_before();
//    }

//    // Слот для добавления новой записи после выделенной строки
//    // Slot to add a new record after the highlighted row
//    void TabWidget::addnew_after(void)
//    {
//        _record_controller->addnew_after();
//    }
//    void TabWidget::on_edit_fieldcontext(void)
//    {
//        _record_controller->on_edit_fieldcontext();
//    }

//    // Обработка клика по удалению записи в контекстном меню и по кнопке на панели
//    void TabWidget::close_context(void)
//    {
//        _record_controller->close_context();

//    }

//    // Копирование отмеченных записей в буфер обмена с удалением
//    // из таблицы конечных записей
//    void TabWidget::cut(void)
//    {
//        _record_controller->cut();
//    }

//    // Копирование отмеченных записей в буфер обмена
//    void TabWidget::copy(void)
//    {
//        _record_controller->copy();
//    }

//    // Insert records from the clipboard    // Вставка записей из буфера обмена
//    void TabWidget::paste(void)
//    {
//        _record_controller->paste();
//    }
//    // Слот, срабатывающий при вызове настроек
//    void TabWidget::settings(void)
//    {
//        _record_controller->settings();
//    }

//    // Перемещение записи вверх
//    void TabWidget::move_up(void)
//    {
//        _record_controller->move_up();
//    }


//    // Перемещение записи вниз
//    void TabWidget::move_dn(void)
//    {
//        _record_controller->move_dn();
//    }

//    // Клик по пункту "Сортировка" в контекстном меню
//    void TabWidget::on_sort_click(void)
//    {
//        _record_controller->on_sort_click();
//    }

//    void TabWidget::on_print_click(void)
//    {
//        _record_controller->on_print_click();
//    }

//    TabWidget::coupler::coupler(
//        TabWidget                           *_tabmanager
//        , boost::intrusive_ptr<TreeItem>    item_link_
//        , bool                              make_current_
//    )
//        : // std::enable_shared_from_this<Coupler>()
//          _tabmanager(_tabmanager)
//        , _item_link(item_link_)
//        , _page_link(nullptr)
//        , _make_current(make_current_)
//    {
//        //        //        _bounded_item->record_binder(boost::intrusive_ptr<CouplerDelegation>(new CouplerDelegation(shared_from_this())));
//        //        _bounded_page->record_binder(new TreeItem::coupler_delegation(std::const_pointer_cast<Coupler>(shared_from_this())));
//        //        _bounded_item->record_binder(_bounded_page->record_binder());
//    }

//    WebView *TabWidget::coupler::binder()   // boost::intrusive_ptr<TreeItem> item
//    {
//        WebView *view = nullptr;
//        // assert(item);

//        //        boost::intrusive_ptr<TreeItem> result = _tabmanager->_record_controller->source_model()->find(QUrl(Browser::_defaulthome));
//        //        WebView *view = nullptr;

//        //        if(!result) {
//        // assert(!item->is_registered_to_browser());
//        if(!_page_link) {
//            view = _tabmanager->newTab(_item_link, _make_current); //

//            //            if(!_page_link) _page_link = view->page();
//        }


//        //        view->page()->record_binder()->binder(item);  // included in newTab()

//        //        (item.get()->*_bind)(view->page());
//        //        //        } else {
//        //        //            //                    // assert(item->is_registered_to_browser());
//        //        //            //                    view = item->bind(); //result->unique_page()->view();
//        //        //            assert(result->unique_page());
//        //        //            assert(result->unique_page()->view());
//        //        //            view = result->unique_page()->view();
//        //        //            auto it = (item.get()->*_bind)(result->unique_page());
//        //        //            assert(it->is_registered_to_browser());
//        //        //        }
//        //        _bounded_item = item;

//        //        assert(_bounded_page == view->page());    // we can give it up now
//        //        assert(_bounded_page->record_binder());
//        assert(_item_link->binder());
//        //        _bounded_page->record_binder(_bounded_item->record_binder());

//        assert(_item_link->is_registered_to_browser() || (_item_link->field("url") == Browser::_defaulthome));

//        return view;
//    }

//    WebView *TabWidget::coupler::activator(
//        // boost::intrusive_ptr<TreeItem> item
//    )
//    {
//        assert(_item_link->page_link() == _page_link);
//        assert(_item_link == _page_link->item_link());

//        assert(_item_link);
//        assert(_item_link->page_valid());
//        return _item_link->page_link()->activate();
//    }


}
