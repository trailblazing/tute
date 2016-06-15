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

#include "browser.h"
#include "utility/delegate.h"
#include <cassert>

#include "autosaver.h"
#include "bookmarks.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"

#include "chasewidget.h"
#include "downloadmanager.h"
#include "history.h"
#include "settings.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "ui_passworddialog.h"
#include "webview.h"

#include <QtCore/QSettings>

#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>

#include <QDockWidget>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QtCore/QDebug>

#include "controllers/record_table/RecordController.h"
#include "libraries/GlobalParameters.h"
#include "main.h"
#include "models/record_table/ItemsFlat.h"
#include "models/record_table/Record.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/recordindex.hxx"
#include "models/tree/binder.hxx"
#include "models/tree/treeindex.hxx"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/main_window/MainWindow.h"
#include "views/record_table/RecordScreen.h"
#include "views/record_table/RecordView.h"
#include "views/tree/KnowView.h"
#include "views/tree/TreeScreen.h"

extern GlobalParameters globalparameters;
// Record *default_record = nullptr;
const char *profile_storage_name = "cookie";

namespace browser {

    template <typename Arg, typename R, typename C>
    struct InvokeWrapper {
        R *receiver;
        void (C::*memberFun)(Arg);
        void operator()(Arg result){
            (receiver->*memberFun)(result);
        }
    };

    template <typename Arg, typename R, typename C>
    InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg)){
        InvokeWrapper<Arg, R, C> wrapper = { receiver, memberFun };
        return wrapper;
    }

    UrlRequestInterceptor::UrlRequestInterceptor(QObject *p)
        : QWebEngineUrlRequestInterceptor(p)
    {}

    void UrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info){
        if(info.navigationType() == QWebEngineUrlRequestInfo::NavigationTypeLink) {
            info.requestUrl();
        }
    }

    Profile::~Profile(){
        _urlrequestinterceptor->deleteLater();
    }

    Profile::Profile(const QString &name, QObject *p)
        : QWebEngineProfile(name, p) // QQuickWebEngineProfile(p)
          , _urlrequestinterceptor(new UrlRequestInterceptor(p)){
        // setStorageName(profile_storage_name);
        setRequestInterceptor(_urlrequestinterceptor);

        // QSettings settings;

        // settings.beginGroup(QLatin1String("downloadmanager"));
        // QString downloadDirectory = settings.value(QLatin1String("downloadDirectory")).toString();
        //// downloadsLocation->setText(downloadDirectory);
        // settings.endGroup();

        //// this group lead chrome_odthread deadlock? or initialize failure
        // settings.beginGroup(QLatin1String("cookies"));

        // setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
        // setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::ForcePersistentCookies); // AllowPersistentCookies

        // QWebEngineProfile::PersistentCookiesPolicy persistentCookiesPolicy = QWebEngineProfile::PersistentCookiesPolicy(
        // settings.value(QLatin1String("persistentCookiesPolicy")).toInt()    // QWebEngineProfile::ForcePersistentCookies  //vQWebEngineProfile::AllowPersistentCookies   //
        // );
        // setPersistentCookiesPolicy(persistentCookiesPolicy);

        // QString persistent_data_path = settings.value(QLatin1String("persistentDataPath")).toString();

        // if(persistent_data_path == "")persistent_data_path = downloadDirectory;

        // QDir cookie_path(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::CacheLocation)); // persistent_data_path

        // if(!cookie_path.exists()) {
        ////        QDir dir;
        // cookie_path.mkpath(persistent_data_path);
        // }

        // setPersistentStoragePath(persistent_data_path);

        // settings.endGroup();
    }

    // const char *DockedWindow::_defaulthome = "about:blank";

    Browser::~Browser(){
        std::set<Browser *> &_browsers = _entrance->browsers();
        if(_browsers.find(this) != _browsers.end()) {
            for(auto &it : _browsers) { // for(auto it = _browsers.begin(); it != _browsers.end(); ++it) {
                // if(_browsers.size() > 0) {
                if(it)
                    if(it == this) // && it != _entrance->widget()
                        _browsers.erase(it);
                // } else
                // break;
            }
        }
        _autosaver->changeOccurred();
        // _autosaver->saveIfNeccessary();
        _autosaver->deleteLater();
        _bookmarkstoolbar->deleteLater();
        // delete _tabmanager;
        if(globalparameters.vtab_tree()->indexOf(_record_screen) != - 1) globalparameters.vtab_tree()->removeTab(globalparameters.vtab_tree()->indexOf(_record_screen));
        // if(_record_screen) {
        // _record_screen->deleteLater();
        ////            delete _record_screen; _record_screen = nullptr;
        // }

        _layout->deleteLater();
        _centralwidget->deleteLater();
    }

    void Browser::init(){ // QUrl const &url,
        // browser->setWidget(this);
        // this->setParent(browser);
        setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
        // setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        setWebAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        setToolButtonStyle(Qt::ToolButtonFollowStyle);
        setAttribute(Qt::WA_DeleteOnClose, true);
        // statusBar()
        status_bar()->setSizeGripEnabled(true);
        append_to_main_menu();
        setup_tool_bar();

        // QWidget *_centralwidget = new QWidget(this);
        // BookmarksModel *bookmarksModel = QtSingleApplication::bookmarksManager()->bookmarksModel();
        // _bookmarkstoolbar = new BookmarksToolBar(bookmarksModel, this);
        connect(_bookmarkstoolbar, &BookmarksToolBar::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(_bookmarkstoolbar->toggleViewAction(), &QAction::toggled, this, &Browser::update_bookmarks_toolbar_action_text);

        // QVBoxLayout *_layout = new QVBoxLayout;
        _layout->setSpacing(0);
        _layout->setMargin(0);
#if defined(Q_OS_OSX)
        layout->addWidget(_bookmarkstoolbar);
        layout->addWidget(new QWidget); // <- OS X tab widget style bug
#else
        addToolBarBreak();
        addToolBar(_bookmarkstoolbar);
#endif
        _layout->addWidget(_tabmanager);
        _centralwidget->setLayout(_layout);
        setCentralWidget(_centralwidget);

        connect(_tabmanager, &TabWidget::loadPage, this, &Browser::loadPage);
        connect(_tabmanager, &TabWidget::setCurrentTitle, this, &Browser::slotUpdateWindowTitle);
        connect(_tabmanager, &TabWidget::showStatusBarMessage, status_bar(), &QStatusBar::showMessage);
        connect(_tabmanager, &TabWidget::linkHovered, status_bar(), [this] (const QString & link) {
                // &QStatusBar::showMessage
                this->status_bar()->showMessage(link);
            });
        connect(_tabmanager, &TabWidget::loadProgress, this, &Browser::slotLoadProgress);
        connect(_tabmanager, &TabWidget::tabsChanged, _autosaver, &AutoSaver::changeOccurred);
        connect(_tabmanager, &TabWidget::geometryChangeRequested, this, &Browser::geometry_change_requested);
#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(_tabmanager, &TabWidget::printRequested, this, &Browser::printRequested);
#endif
        connect(_tabmanager, &TabWidget::menuBarVisibilityChangeRequested, menuBar(), &QMenuBar::setVisible);
        connect(_tabmanager, &TabWidget::statusBarVisibilityChangeRequested, status_bar(), &QStatusBar::setVisible);
        // connect(_tabmanager, SIGNAL(toolBarVisibilityChangeRequested(bool)), navigater, SLOT(setVisible(bool)));
        connect(_tabmanager, &TabWidget::toolBarVisibilityChangeRequested, _bookmarkstoolbar, &BookmarksToolBar::setVisible);
#if defined(Q_OS_OSX)
        connect(_tabmanager, &TabWidget::lastTabClosed, this, &Browser::close);
#else
        connect(
            _tabmanager, &TabWidget::lastTabClosed, this, [&] () -> void {
                // _tabmanager->newTab(request_record(QUrl(DockedWindow::_defaulthome)));
                // WebView *view = nullptr;
                // sd::method(
                // ""  //std::string _method_name
                // , &TabWidget::newTab    //return_type(object_type::*f)(Arg...)          //  f can be std::function<> template?
                // , view  //return_type *r            // = (return_type *)0
                // , _tabmanager   //typename STATIC_IF_SHARED<object_type, object_pointer_type>::type o   //shared_ptr<object_type> o // = (object_type *const)0  //nullptr
                // , Arg &&... arg
                // );

                // auto arint = boost::make_shared<TabWidget::ActiveRecordBinder>(_tabmanager, true);
                // auto record = _record_controller->request_item(
                // QUrl(Browser::_defaulthome)
                // , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                // ""
                // , &TabWidget::ActiveRecordBinder::binder
                // , arint
                // )
                // , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                // ""
                // , &TabWidget::ActiveRecordBinder::activator
                // , arint
                // )
                // );
                // record->activate();

                this->close();
                HidableTabWidget *vtab_tree = _main_window->vtab_tree();
                HidableTabWidget *vtab_record = _main_window->vtab_record();
                int index = vtab_record->indexOf(_record_screen);
                if(index != - 1) {
                    vtab_record->removeTab(index);
                    // static_cast<Browser *>(globalparameters.vtab()->currentWidget())->append_to_main_menu();
                    for(int i = 0; i < vtab_tree->count(); i ++) {
                        auto wg = vtab_tree->widget(i);
                        if(wg->objectName() == tree_screen_viewer_name) {
                            auto tree_screen_viewer = dynamic_cast<TreeScreenViewer *>(wg);
                            if(tree_screen_viewer ) {
                                if(tree_screen_viewer->record_screen() == _record_screen) {
                                    vtab_tree->removeTab(i);
                                }
                            }
                        }
                    }
                }
            });
#endif

        slotUpdateWindowTitle();
        load_default_state();






        auto vtab_tree = _main_window->vtab_tree();
        auto vtab_record = _main_window->vtab_record();
        if(vtab_record->indexOf(_record_screen) == - 1) {
            ////        int index = vtab_tree->currentIndex();
            TreeScreenViewer *tsv = nullptr;
            std::vector<TreeScreenViewer *> tsvs;
            int tree_viewer_count = 0;
            for(int i = 0; i < vtab_tree->count(); i ++) {
                if(vtab_tree->widget(i)->objectName() == tree_screen_viewer_name) {
                    tsvs.push_back( dynamic_cast<TreeScreenViewer *>(vtab_tree->widget(i)));
                    tree_viewer_count ++;
                }
            }
            if(1 == tree_viewer_count) {
                tsv = tsvs.back();
                if(tsv) {
                    RecordScreen *rs = tsv->record_screen();
                    if(rs) {
                        vtab_tree->setUpdatesEnabled(false);

                        vtab_tree->insertTab(vtab_tree->indexOf(tsv) + 1, new TreeScreenViewer(_tree_screen, _record_screen), QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Browser")); // QString("Browser ") + QString::number(tree_viewer_count)
                        // vtab_tree->setCurrentIndex(index);
                        vtab_tree->setUpdatesEnabled(true);
                    }else  {
                        tsv->record_screen(_record_screen);
                        // emit vtab_tree->tabBarClicked(vtab_tree->indexOf(tsv));    // vtab_tree->currentChanged(vtab_tree->indexOf(tsv));
                    }
                }
            }else{
                vtab_tree->setUpdatesEnabled(false);

                vtab_tree->insertTab(vtab_tree->indexOf(tsvs.back()) + 1, new TreeScreenViewer(_tree_screen, _record_screen), QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Browser")); // QString("Browser ") + QString::number(tree_viewer_count)
                // vtab_tree->setCurrentIndex(index);
                vtab_tree->setUpdatesEnabled(true);
            }
            // int index = vtab_record->currentIndex();
            vtab_record->addTab(_record_screen, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Browser")); // QString("Browser ") + QString::number(vtab_record->count())
            _record_screen->adjustSize();
            // vtab_record->setCurrentIndex(index);
        }
        // QSplitter *_h_right_splitter = _main_window->h_right_splitter();
        // if(_h_right_splitter->count() > 1) {
        // for(int i = 0; i < _h_right_splitter->count(); i++) {
        // auto wg = _h_right_splitter->widget(i);
        // if(wg->objectName() == record_screen_multi_instance_name) {wg->hide(); wg->setParent(nullptr); }
        // }
        // }
        // if(_h_right_splitter->indexOf(this) == -1) {
        // _h_right_splitter->insertWidget(0, _record_screen); // vtab_record->addTab(_record_screen, QIcon(":/resource/pic/clover.svg"), QString("Browser ") + QString::number(vtab_record->count()));
        // }

////        auto vtab_record = _main_window->vtab_tree();
// QSplitter *_h_right_splitter = _main_window->h_right_splitter();
// if(_h_right_splitter->count() > 1) {
// for(int i = 0; i < _h_right_splitter->count(); i++) {
// auto wg = _h_right_splitter->widget(i);
// if(wg->objectName() == record_screen_multi_instance_name) {wg->hide(); wg->parent(nullptr); }
// }
// }
// if(_h_right_splitter->indexOf(_record_screen) == -1) {
// _h_right_splitter->insertWidget(0, _record_screen); // vtab_record->addTab(_record_screen, QIcon(":/resource/pic/clover.svg"), QString("Browser ") + QString::number(vtab_record->count()));
////            vtab_record->setCurrentWidget(_record_screen);
// }






        // int size = _tabmanager->lineEditStack()->sizeHint().height();

        // navigater->setIconSize(QSize(size, size));

        // initUrl();
        // dock_widget->setWidget(this);
        // setParent(dock_widget);

        // connect(_main_window, &MainWindow::setEnabled, this, &DockedWindow::setEnabled);
        // connect(_main_window, &MainWindow::blockSignals, this, &DockedWindow::blockSignals);

        //// test:
        // connect(const_cast<Browser *>(this), &Browser::~Browser, [&]() {
        // auto bs = _entrance->browsers();

        // for(auto i = bs.begin(); i != bs.end(); i++) {
        // if(*i == this)
        // bs.erase(i);
        // }
        // });
    }

    void Browser::run_script(const QString &style_source){
        QMetaObject::invokeMethod(this, "runScriptOnOpenViews", Qt::QueuedConnection, Q_ARG(QString, style_source));
    }

    // boost::intrusive_ptr<TreeItem> Browser::item_bind(boost::intrusive_ptr<TreeItem> item)
    // {
    ////        auto binder = [](boost::shared_ptr<TabWidget::Coupler> ar) {
    ////            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>("", &TabWidget::Coupler::binder, ar);   // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)

    ////        };
    ////        auto activator = [](boost::shared_ptr<TabWidget::Coupler> ar) {
    ////            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>> ("", &TabWidget::Coupler::activator, ar);
    ////        };

    //// registered record, but have no generator:
    ////        boost::shared_ptr<TabWidget::Coupler> ar =

    ////        //        record->binder(
    ////        //            binder(ar)
    ////        //        );

    ////        //        record->activator(
    ////        //            activator(ar)
    ////        //        );

    ////        item->record_binder(std::make_shared<CouplerDelegation>(
    ////                                std::make_shared<bounded_item_interface>("", &TabWidget::Coupler::bounded_item, ar)
    ////                                , std::make_shared<bounded_page_interface>("", &TabWidget::Coupler::bounded_page, ar)
    ////                                , std::make_shared<bind_interface>("", &TabWidget::Coupler::binder, ar)   // binder(ar)
    ////                                , std::make_shared<activate_interface> ("", &TabWidget::Coupler::activator, ar)   // activator(ar)
    ////                            ));
    ////        //        _tabmanager->newTab(record);
    ////        //        assert(record->binded_only_page());
    // return _tabmanager->item_bind(item);
    // }

    // boost::intrusive_ptr<TreeItem> Browser::item_request_from_tree(
    // QUrl const &url
    // , const TreeScreen::paste_strategy &_view_paste_strategy
    // , equal_url_t _equal
    // )
    // {
    // auto tree_screen = globalparameters.tree_screen();
    ////        connect(this, &DockedWindow::activateWindow, _entrance, &Entrance::on_activate_window);

    // return tree_screen->item_request_from_tree(url, _view_paste_strategy, _equal);
    // }

    // boost::intrusive_ptr<TreeItem> Browser::item_request_from_tree(
    // boost::intrusive_ptr<TreeItem> item
    // , const TreeScreen::paste_strategy &_view_paste_strategy
    // , equal_t _equal
    // )
    // {
    // auto tree_screen = globalparameters.tree_screen();
    ////        connect(this, &DockedWindow::activateWindow, _entrance, &Entrance::on_activate_window);

    // return tree_screen->item_request_from_tree(item, _view_paste_strategy, _equal);
    // }

    // Browser::Browser(const QByteArray   &state
    // , TreeScreen       *_tree_screen
    // , FindScreen       *_find_screen
    // , MetaEditor       *_editor_screen
    // , HidableTabWidget *_vtabwidget
    // , MainWindow       *_main_window
    // , Entrance         *_entrance
    // , const QString    &style_source
    // , Qt::WindowFlags  flags
    // )
    // : QMainWindow(0, flags)
    ////        ,  boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>()
    // , _tree_screen(_tree_screen)
    // , _find_screen(_find_screen)
    // , _record_screen(new RecordScreen(_find_screen
    // , _editor_screen
    // , this
    // , _vtabwidget
    // , _main_window))
    // , _main_window(_main_window)
    // , _tabmanager(_record_screen->tabmanager())
    ////        , _toolbarsearch(_find_screen->toolbarsearch())
    // , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
    // , _chasewidget(_find_screen->chasewidget())
    // , _autosaver(new AutoSaver(this))
    // , _historyhome(_find_screen->historyhome())
    // , _historyback(_find_screen->historyback())
    // , _historyforward(_find_screen->historyforward())
    // , _stop(_find_screen->stop())
    // , _reload(_find_screen->reload())
    // , _stopreload(_find_screen->stopreload())
    // , _centralwidget(new QWidget(this))
    // , _layout(new QVBoxLayout)
    // , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    // {
    // init();
    ////        auto r = request_item(QUrl(Browser::_defaulthome));
    ////        r->activate();
    // run_script(style_source);
    // this->restore_state(state);

    // QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

    // QMainWindow::menuBar()->hide();
    // QMainWindow::statusBar()->hide();
    // show();
    // }

    // Browser::Browser(const QUrl         &url
    // , TreeScreen       *_tree_screen
    // , FindScreen       *_find_screen
    // , MetaEditor       *_editor_screen
    // , HidableTabWidget *_vtabwidget
    // , MainWindow       *_main_window
    // , Entrance         *_entrance
    // , const QString    &style_source
    // , Qt::WindowFlags  flags
    // )
    // : QMainWindow(0, flags)
    ////        ,  boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>()
    // , _tree_screen(_tree_screen)
    // , _find_screen(_find_screen)
    // , _record_screen(new RecordScreen(_find_screen
    // , _editor_screen
    // , this
    // , _vtabwidget
    // , _main_window))
    // , _main_window(_main_window)
    // , _tabmanager(_record_screen->tabmanager())
    ////        , _toolbarsearch(_find_screen->toolbarsearch())
    // , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
    // , _chasewidget(_find_screen->chasewidget())
    // , _autosaver(new AutoSaver(this))
    // , _historyhome(_find_screen->historyhome())
    // , _historyback(_find_screen->historyback())
    // , _historyforward(_find_screen->historyforward())
    // , _stop(_find_screen->stop())
    // , _reload(_find_screen->reload())
    // , _stopreload(_find_screen->stopreload())
    // , _centralwidget(new QWidget(this))
    // , _layout(new QVBoxLayout)
    // , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    // {
    // init();

    // _tree_screen->item_register_and_bind(
    // url
    // , std::bind(&TreeScreen::view_paste_as_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    // )->activate();

    // run_script(style_source);       //        assert(record->linkpage());

    // QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

    // QMainWindow::menuBar()->hide();
    // QMainWindow::statusBar()->hide();
    // show();
    // }

    Browser::Browser(TreeScreen *_tree_screen, FindScreen *_find_screen, MetaEditor *_editor_screen, MainWindow *_main_window, Entrance *_entrance, const QString &style_source, Profile *_profile, Qt::WindowFlags flags)
        : QMainWindow(0, flags) // ,  boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>()
          , _tree_screen(_tree_screen)
          , _find_screen(_find_screen)
          , _main_window(_main_window)
          // , _toolbarsearch(_find_screen->toolbarsearch())
          , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
          , _chasewidget(_find_screen->chasewidget())
          , _autosaver(new AutoSaver(this))
          , _historyhome(_find_screen->historyhome())
          , _historyback(_find_screen->historyback())
          , _historyforward(_find_screen->historyforward())
          , _stop(_find_screen->stop())
          , _reload(_find_screen->reload())
          , _stopreload(_find_screen->stopreload())
          , _centralwidget(new QWidget(this))
          , _layout(new QVBoxLayout)
          , _record_screen(new RecordScreen(_tree_screen, _find_screen, _editor_screen, _entrance, this, _main_window, _profile))
          , _tabmanager(_record_screen->tabmanager())
          , _entrance(_entrance->prepend(this)){ // , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
        init();

        run_script(style_source); // assert(record->linkpage());

        QMainWindow::setWindowFlags(Qt::FramelessWindowHint); // Qt::Window |

        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();
        _is_under_construction = false;
    }

    // Browser::Browser(boost::intrusive_ptr<TreeItem> item
    // , TreeScreen       *_tree_screen
    // , FindScreen       *_find_screen
    // , MetaEditor       *_editor_screen
    // , HidableTabWidget *_vtabwidget
    // , MainWindow       *_main_window
    // , Entrance         *_entrance
    // , const QString    &style_source
    // , Qt::WindowFlags  flags
    // )
    // : QMainWindow(0, flags)
    ////        ,  boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>()
    // , _tree_screen(_tree_screen)
    // , _find_screen(_find_screen)
    // , _record_screen(new RecordScreen(_find_screen
    // , _editor_screen
    // , this
    // , _vtabwidget
    // , _main_window))
    // , _main_window(_main_window)
    // , _tabmanager(_record_screen->tabmanager())
    ////        , _toolbarsearch(_find_screen->toolbarsearch())
    // , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
    // , _chasewidget(_find_screen->chasewidget())
    // , _autosaver(new AutoSaver(this))
    // , _historyhome(_find_screen->historyhome())
    // , _historyback(_find_screen->historyback())
    // , _historyforward(_find_screen->historyforward())
    // , _stop(_find_screen->stop())
    // , _reload(_find_screen->reload())
    // , _stopreload(_find_screen->stopreload())
    // , _centralwidget(new QWidget(this))
    // , _layout(new QVBoxLayout)
    // , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    // {
    // assert(item);

    // init();

    // _tree_screen->item_register_and_bind(
    // item
    // , std::bind(&TreeScreen::view_paste_as_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    // )->activate();

    // assert(item->page_valid() && item->page_link());

    // run_script(style_source);       //        assert(record->linkpage());

    // QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

    // QMainWindow::menuBar()->hide();
    // QMainWindow::statusBar()->hide();
    // show();

    // }

    bool Browser::is_under_construction() const {
        return _is_under_construction;
    }

    QAction *Browser::historyback() {
        return _historyback;
    }

    void Browser::activateWindow(){
        this->setParent(_entrance);
        _entrance->setWidget(this);

        _entrance->on_activate_window();

        _find_screen->toolbarsearch()->lineedits(this->tabWidget()->lineEditStack());

        auto vtab_tree = globalparameters.vtab_tree();
        int index = vtab_tree->indexOf(_record_screen);
        if(vtab_tree->currentIndex() != index) {
            auto previous_browser = static_cast<Browser *>(vtab_tree->currentWidget());
            if(previous_browser) {
                previous_browser->lower();
            }
            vtab_tree->setCurrentIndex(index);
            if(! _tabmanager->find([&] (boost::intrusive_ptr<const ::Binder> b) {
                    return b->host()->id() == _tree_screen->view()->current_item()->id();
                })) {
                auto it = _record_screen->record_controller()->view()->current_item();
                if(it) {
                    _tree_screen->view()->select_as_current(TreeIndex::instance([&] { return _tree_screen->view()->source_model(); }, it->parent(), it));
                }
            }
        }
        QMainWindow::activateWindow();
    }

    void Browser::load_default_state(){
        QSettings settings;
        settings.beginGroup(QLatin1String("browser"));
        QByteArray data = settings.value(QLatin1String("default_state")).toByteArray();
        restore_state(data);
        settings.endGroup();
    }

    QSize Browser::sizeHint() const {
        QRect desktopRect = QApplication::desktop()->screenGeometry();
        QSize size = desktopRect.size() * qreal(0.9);
        return size;
    }

    TabWidget *Browser::tabWidget() {
        return _tabmanager;
    }

    TabWidget *Browser::tabWidget() const {
        return _tabmanager;
    }

    TabWidget *Browser::tabmanager() {
        return _tabmanager;
    }

    TabWidget *Browser::tabmanager() const {
        return _tabmanager;
    }

    void Browser::save(){
        QtSingleApplication::instance()->saveSession();

        QSettings settings;
        settings.beginGroup(QLatin1String("browser"));
        QByteArray data = save_state(false);
        settings.setValue(QLatin1String("default_state"), data);
        settings.endGroup();
    }

    static const qint32 browser_magic = 0xba;

    QByteArray Browser::save_state(bool withTabs) const {
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);

        int version = 2;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << qint32(browser_magic);
        stream << qint32(version);

        stream << size();
        stream << menuBar()->isVisible();
        stream << ! _find_screen->isHidden();
        stream << ! _bookmarkstoolbar->isHidden();
        stream << ! status_bar()->isHidden();
        if(withTabs) stream << tabWidget()->saveState();
        else stream << QByteArray();
        return data;
    }

    bool Browser::restore_state(const QByteArray &state){
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);

        int version = 2;
        QByteArray sd = state;
        QDataStream stream(&sd, QIODevice::ReadOnly);
        if(stream.atEnd()) return false;
        qint32 marker;
        qint32 v;
        stream >> marker;
        stream >> v;
        if(marker != browser_magic || v != version) return false;
        QSize size;
        bool showMenuBar;
        bool showToolbar;
        bool showBookmarksBar;
        bool showStatusbar;
        QByteArray tabState;

        stream >> size;
        stream >> showMenuBar;
        stream >> showToolbar;
        stream >> showBookmarksBar;
        stream >> showStatusbar;
        stream >> tabState;

        resize(size);

        _find_screen->setVisible(showToolbar);
        updateToolbarActionText(showToolbar);

        _main_window->menuBar()->setVisible(showMenuBar);
        [this] (bool show) {
            _viewmenubar->setText(show ? tr("Hide Menu bar") : tr("Show Menu bar"));
        } (showMenuBar);

        _bookmarkstoolbar->setVisible(showBookmarksBar);
        update_bookmarks_toolbar_action_text(showBookmarksBar);

        status_bar()->setVisible(showStatusbar);
        update_statusbar_action_text(showStatusbar);
        if(! tabWidget()->restoreState(tabState)) return false;
        return true;
    }

    void Browser::runScriptOnOpenViews(const QString &source){
        for(int i = 0; i < tabWidget()->count(); ++ i) {
            auto browserview = tabWidget()->webView(i);
            if(browserview != nullptr) browserview->page()->runJavaScript(source);
        }
    }

    void Browser::setWebAttribute(QWebEngineSettings::WebAttribute attribute, bool enabled) {
        _webattribute = attribute; _webattributeenabled = enabled;
    }

    QString &Browser::lastsearch() {
        return _lastsearch;
    }

    const QString &Browser::lastsearch() const {
        return _lastsearch;
    }

    void Browser::append_to_file_menu(){
        // File
        QMenu *filemenu = _main_window->file_menu(); // menuBar()->addMenu(tr("&File"));

        filemenu->addAction(tr("&New Window"), this, SLOT(slotFileNew()), QKeySequence::New);
        filemenu->addAction(_tabmanager->newTabAction()); // tr("New &Tab")
        filemenu->addAction(tr("&Open File..."), this, SLOT(slotFileOpen()), QKeySequence::Open);
        filemenu->addAction(tr("Open &Location..."), this, SLOT(slotSelectLineEdit()), QKeySequence(Qt::ControlModifier + Qt::Key_L));
        filemenu->addSeparator();
        filemenu->addAction(_tabmanager->closeTabAction()); // tr("&Close Tab")
        filemenu->addSeparator();
#if defined(QWEBENGINE_SAVE_AS_FILE)
        fileMenu->addAction(tr("&Save As..."), this, SLOT(slotFileSaveAs()), QKeySequence(QKeySequence::Save));
        fileMenu->addSeparator();
#endif
        BookmarksManager *bookmarksManager = QtSingleApplication::bookmarksManager();
        filemenu->addAction(tr("&Import Bookmarks..."), bookmarksManager, SLOT(importBookmarks()));
        filemenu->addAction(tr("&Export Bookmarks..."), bookmarksManager, SLOT(exportBookmarks()));
        filemenu->addSeparator();
#if defined(QWEBENGINEPAGE_PRINT)
        fileMenu->addAction(tr("P&rint Preview..."), this, SLOT(slotFilePrintPreview()));
        fileMenu->addAction(tr("&Print..."), this, SLOT(slotFilePrint()), QKeySequence::Print);
        fileMenu->addSeparator();
#endif
        QAction *action = filemenu->addAction(tr("Private &Browsing..."), this, SLOT(slotPrivateBrowsing()));
        action->setCheckable(true);
        action->setChecked(QtSingleApplication::instance()->privateBrowsing());
        connect(QtSingleApplication::instance(), &QtSingleApplication::privateBrowsingChanged, action, &QAction::setChecked);
        filemenu->addSeparator();

        // #if defined(Q_OS_OSX)
        // fileMenu->addAction(tr("&Quit"), QtSingleApplication::instance(), SLOT(quitBrowser()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        // #else
        // fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        // #endif

        _main_window->append_quit_menu();
        // QAction *quit = new QAction(tr("&Quit"), _main_window);
        // quit->setShortcut(Qt::CTRL + Qt::Key_Q);
        // connect(quit, &QAction::triggered, _main_window, &MainWindow::application_exit);
        // filemenu->addAction(quit);

        // _tree_screen->buttonmenu()->addMenu(filemenu);
    }

    void Browser::append_edit_menu(){

        // Edit
        QMenu *editMenu = _main_window->edit_menu();
        // editMenu = _main_window->menuBar()->addMenu(tr("&Edit"));
        editMenu->clear();

        QAction *undo_ = editMenu->addAction(tr("&Undo"));
        undo_->setShortcuts(QKeySequence::Undo);
        _tabmanager->addWebAction(undo_, QWebEnginePage::Undo);
        QAction *redo_ = editMenu->addAction(tr("&Redo"));
        redo_->setShortcuts(QKeySequence::Redo);
        _tabmanager->addWebAction(redo_, QWebEnginePage::Redo);
        editMenu->addSeparator();
        QAction *cut_ = editMenu->addAction(tr("Cu&t"));
        cut_->setShortcuts(QKeySequence::Cut);
        _tabmanager->addWebAction(cut_, QWebEnginePage::Cut);
        QAction *copy_ = editMenu->addAction(tr("&Copy"));
        copy_->setShortcuts(QKeySequence::Copy);
        _tabmanager->addWebAction(copy_, QWebEnginePage::Copy);
        QAction *paste_ = editMenu->addAction(tr("&Paste"));
        paste_->setShortcuts(QKeySequence::Paste);
        _tabmanager->addWebAction(paste_, QWebEnginePage::Paste);
        editMenu->addSeparator();

        QAction *find_ = editMenu->addAction(tr("&Find"));
        find_->setShortcuts(QKeySequence::Find);
        connect(find_, &QAction::triggered, this, &Browser::slotEditFind);
        // new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

        QAction *findnext_ = editMenu->addAction(tr("&Find Next"));
        findnext_->setShortcuts(QKeySequence::FindNext);
        connect(findnext_, &QAction::triggered, this, &Browser::slotEditFindNext);

        QAction *findprevious_ = editMenu->addAction(tr("&Find Previous"));
        findprevious_->setShortcuts(QKeySequence::FindPrevious);
        connect(findprevious_, &QAction::triggered, this, &Browser::slotEditFindPrevious);
        editMenu->addSeparator();

        editMenu->addAction(tr("&Browser preferences"), this, SLOT(slotPreferences()), tr("Ctrl+,"));

        _tree_screen->menus_in_button()->addMenu(editMenu);
    }

    void Browser::append_view_menu(){
        // View
        QMenu *viewMenu = _main_window->view_menu(); // _main_window->menuBar()->addMenu(tr("&View"));
        viewMenu->clear();

        _viewmenubar = new QAction(this);
        // updateMenuBarActionText(true);
        _viewmenubar->setText(tr("Hide Menu bar"));
        _viewmenubar->setShortcut(tr("Shift+Ctrl+M"));
        connect(_viewmenubar, &QAction::triggered, this
            // &DockedWindow::slotViewMenuBar
               ,
            [this] () {
                if(_main_window->menuBar()->isVisible()) {
                    _viewmenubar->setText(tr("Show Menu bar")); // updateMenuBarActionText(false);
                    // _main_window->menuBar()->hide();
                    _main_window->menuBar()->close();
                } else {
                    _viewmenubar->setText(tr("Hide Menu bar")); // updateMenuBarActionText(true);
                    _main_window->menuBar()->show();
                }
                _autosaver->changeOccurred();
            });

        viewMenu->addAction(_viewmenubar);

        _viewbookmarkbar = new QAction(this);
        update_bookmarks_toolbar_action_text(true);
        _viewbookmarkbar->setShortcut(tr("Shift+Ctrl+B"));
        connect(_viewbookmarkbar, &QAction::triggered, this, &Browser::slotViewBookmarksBar);
        viewMenu->addAction(_viewbookmarkbar);

        _viewtoolbar = new QAction(this);
        updateToolbarActionText(true);
        _viewtoolbar->setShortcut(tr("Ctrl+|"));
        connect(_viewtoolbar, &QAction::triggered, this, &Browser::slotViewToolbar);
        viewMenu->addAction(_viewtoolbar);

        _viewstatusbar = new QAction(this);
        update_statusbar_action_text(true);
        _viewstatusbar->setShortcut(tr("Ctrl+/"));
        connect(_viewstatusbar, &QAction::triggered, this, &Browser::slotViewStatusbar);
        viewMenu->addAction(_viewstatusbar);

        viewMenu->addSeparator();

        // _stop = viewMenu->addAction(tr("&Stop"));
        QList<QKeySequence> shortcuts;
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
        shortcuts.append(Qt::Key_Escape);
        _stop->setShortcuts(shortcuts);
        _tabmanager->addWebAction(_stop, QWebEnginePage::Stop);

        // _reload = viewMenu->addAction(tr("Reload Page"));
        _reload->setShortcuts(QKeySequence::Refresh);
        _tabmanager->addWebAction(_reload, QWebEnginePage::Reload);

        viewMenu->addAction(tr("Zoom &In"), this, SLOT(slotViewZoomIn()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
        viewMenu->addAction(tr("Zoom &Out"), this, SLOT(slotViewZoomOut()), QKeySequence(Qt::CTRL | Qt::Key_Minus));
        viewMenu->addAction(tr("Reset &Zoom"), this, SLOT(slotViewResetZoom()), QKeySequence(Qt::CTRL | Qt::Key_0));

        viewMenu->addSeparator();
        viewMenu->addAction(tr("Page S&ource"), this, SLOT(slotViewPageSource()), tr("Ctrl+Alt+U"));
        QAction *a = viewMenu->addAction(tr("&Full Screen"), this, SLOT(slotViewFullScreen(bool)), Qt::Key_F11);
        a->setCheckable(true);

        _tree_screen->menus_in_button()->addMenu(viewMenu);
    }

    void Browser::append_history_menu(){
        // History
        HistoryMenu *historyMenu = _main_window->histry_menu();
        // new HistoryMenu(
        // _main_window   //this
        // );
        historyMenu->clear();

        connect(historyMenu, &HistoryMenu::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu *>(historyMenu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        historyMenu->setTitle(tr("Hi&story"));

        _main_window->menuBar()->addMenu(historyMenu);

        _tree_screen->menus_in_button()->addMenu(historyMenu);

        QList<QAction *> historyActions;

        // _historyback = new QAction(tr("Back"), this);
        _tabmanager->addWebAction(_historyback, QWebEnginePage::Back);
        _historyback->setShortcuts(QKeySequence::Back);
        _historyback->setIconVisibleInMenu(false);
        historyActions.append(_historyback);

        // _historyforward = new QAction(tr("Forward"), this);
        _tabmanager->addWebAction(_historyforward, QWebEnginePage::Forward);
        _historyforward->setShortcuts(QKeySequence::Forward);
        _historyforward->setIconVisibleInMenu(false);
        historyActions.append(_historyforward);

        // QAction *_historyhome = new QAction(tr("Home"), this);
        // connect(_historyhome, SIGNAL(triggered()), this, SLOT(slotHome()));

        // connect(_historyhome, &QAction::triggered, this, &DockedWindow::slotHome);

        _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

        historyActions.append(_historyhome);

#if defined(QWEBENGINEHISTORY_RESTORESESSION)
        _restorelastsession = new QAction(tr("Restore Last Session"), this);
        connect(_restorelastsession, &QAction::triggered, QtSingleApplication::instance(), &QtSingleApplication::restoreLastSession);
        _restorelastsession->setEnabled(QtSingleApplication::instance()->canRestoreSession());
        historyActions.append(_tabwidget->recentlyClosedTabsAction());
        historyActions.append(_restorelastsession);
#endif

        historyMenu->setInitialActions(historyActions);
        _tree_screen->menus_in_button()->addMenu(historyMenu);
    }

    void Browser::append_bookmark_menu(){
        // Bookmarks
        BookmarksMenu *bookmarksMenu = _main_window->bookmark_menu();
        // new BookmarksMenu(
        // _main_window   //this
        // );
        bookmarksMenu->clear();

        connect(bookmarksMenu, &BookmarksMenu::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu *>(bookmarksMenu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        bookmarksMenu->setTitle(tr("&Bookmarks"));
        _main_window->menuBar()->addMenu(bookmarksMenu);

        QList<QAction *> bookmarksActions;

        QAction *showAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
        connect(showAllBookmarksAction, &QAction::triggered, this, &Browser::slotShowBookmarksDialog);
        _addbookmark = new QAction(QIcon(QLatin1String(":addbookmark.png")), tr("Add Bookmark..."), this);
        _addbookmark->setIconVisibleInMenu(false);

        connect(_addbookmark, &QAction::triggered, this, &Browser::slotAddBookmark);
        _addbookmark->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

        bookmarksActions.append(showAllBookmarksAction);
        bookmarksActions.append(_addbookmark);
        bookmarksMenu->setInitialActions(bookmarksActions);

        _tree_screen->menus_in_button()->addMenu(bookmarksMenu);
    }

    void Browser::append_window_menu(){
        // Window
        _windowmenu = _main_window->window_menu(); // menuBar()->addMenu(tr("&Window"));
        _windowmenu->clear();
        connect(_windowmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowWindowMenu);
        slotAboutToShowWindowMenu();

        _tree_screen->menus_in_button()->addMenu(_windowmenu);
    }

    void Browser::append_to_tools_menu(){
        QMenu *toolsMenu = _main_window->tools_menu(); // menuBar()->addMenu(tr("&Tools"));
        toolsMenu->addAction(tr("Web &Search"), this, SLOT(slotWebSearch()), QKeySequence(tr("Ctrl+K", "Web Search")));
#if defined(QWEBENGINEINSPECTOR)
        a = toolsMenu->addAction(tr("Enable Web &Inspector"), this, SLOT(slotToggleInspector(bool)));
        a->setCheckable(true);
#endif
    }

    void Browser::append_to_help_menu(){
        _main_window->init_help_menu();
        assert(_main_window->help_menu());
        QMenu *helpMenu = _main_window->help_menu(); // menuBar()->addMenu(tr("&Help"));
        // helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
        helpMenu->addAction(tr("About &Embedded Browser"), this, SLOT(slotAboutApplication()));

        _tree_screen->menus_in_button()->addMenu(helpMenu);
    }

    void Browser::append_to_main_menu(){
        new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(slotSwapFocus()));

        _main_window->menuBar()->clear();
        _main_window->init_file_menu();
        _main_window->init_tools_menu();

        append_to_file_menu();

        append_edit_menu();

        append_view_menu();

        append_history_menu();

        append_bookmark_menu();

        append_window_menu();

        append_to_tools_menu();

        append_to_help_menu();
    }

    void Browser::setup_tool_bar(){
        // auto mainwindow = _main_window;
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);
        // navigater = addToolBar(tr("Navigation"));  //mainwindow->addToolBar(tr("Navigation"));
        // connect(navigater->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(updateToolbarActionText(bool)));

        // _historyback = findscreen->historyback();

        _historyback->setIcon(QIcon(":/resource/pic/mobile_back.svg") // style()->standardIcon(QStyle::SP_ArrowBack, 0, _find_screen)
            ); // this

        _historybackmenu = new QMenu(this);
        _historyback->setMenu(_historybackmenu);
        connect(_historybackmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowBackMenu);
        connect(_historybackmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);

        // navigater->addAction(_historyback);

        _historyforward->setIcon(QIcon(":/resource/pic/mobile_forward.svg") // style()->standardIcon(QStyle::SP_ArrowForward, 0, _find_screen)
            ); // this

        _historyforwardmenu = new QMenu(this);
        connect(_historyforwardmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowForwardMenu);
        connect(_historyforwardmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);
        _historyforward->setMenu(_historyforwardmenu);

        // navigater->addAction(_historyforward);

        // _stopreload = new QAction(navigater); //this

        _reloadicon = QIcon(":/resource/pic/mobile_reload.svg"); // style()->standardIcon(QStyle::SP_BrowserReload);
        _stopreload->setIcon(_reloadicon);

        ////navigater->addAction(_stopreload);

        ////navigater->addWidget(_tabmanager->lineEditStack());
        // _toolbarsearch = _find_screen->toolbarsearch();
        // assert(_toolbarsearch);
        assert(_tabmanager->lineEditStack());

        // if(_tabmanager->lineEditStack() == nullptr)
        // _tabmanager->lineEditStack(_find_screen->toolbarsearch()->lineedits());

        ////_toolbarsearch = new ToolbarSearch(_tabmanager->lineEditStack(), findscreen->findtable(), navigater);

        ////navigater->addWidget(_toolbarsearch);

        ////connect(_toolbarsearch, SIGNAL(search(QUrl)), SLOT(loadUrl(QUrl)));   // move to Entrance

        ////_chasewidget = new ChaseWidget(findscreen->size(), findscreen); //this
        ////navigater->addWidget(_chasewidget);
    }

    void Browser::slotShowBookmarksDialog(){
        BookmarksDialog *dialog = new BookmarksDialog(this);
        connect(dialog, &BookmarksDialog::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        dialog->show();
    }

    void Browser::slotAddBookmark(){
        WebView *webView = currentTab();
        QString url = webView->page()->url().toString();
        QString title = webView->title();
        AddBookmarkDialog dialog(url, title);
        dialog.exec();
    }

    void Browser::slotViewToolbar(){
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);
        if(_find_screen->isVisible()) {
            updateToolbarActionText(false);
            _find_screen->close();
        } else {
            updateToolbarActionText(true);
            _find_screen->show();
        }
        _autosaver->changeOccurred();
    }

    void Browser::slotViewBookmarksBar(){
        if(_bookmarkstoolbar->isVisible()) {
            update_bookmarks_toolbar_action_text(false);
            _bookmarkstoolbar->close();
        } else {
            update_bookmarks_toolbar_action_text(true);
            _bookmarkstoolbar->show();
        }
        _autosaver->changeOccurred();
    }

    void Browser::update_statusbar_action_text(bool visible){
        _viewstatusbar->setText(! visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
    }

    void Browser::handle_find_text_result(bool found){
        if(! found) slotUpdateStatusbar(tr("\"%1\" not found.").arg(_lastsearch));
    }

    void Browser::updateToolbarActionText(bool visible){
        _viewtoolbar->setText(! visible ? tr("Show Find in base") : tr("Hide Find in base"));
    }

    void Browser::update_bookmarks_toolbar_action_text(bool visible){
        _viewbookmarkbar->setText(! visible ? tr("Show Bookmarks bar") : tr("Hide Bookmarks bar"));
    }

    void Browser::slotViewStatusbar(){
        if(status_bar()->isVisible()) {
            update_statusbar_action_text(false);
            status_bar()->close();
        } else {
            update_statusbar_action_text(true);
            status_bar()->show();
        }
        _autosaver->changeOccurred();
    }

    void Browser::loadUrl(const QUrl &url){
        if(! currentTab() || ! url.isValid()) return;
        _tabmanager->currentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
        _tabmanager->loadUrlInCurrentTab(url);
    }

    void Browser::slotDownloadManager(){
        QtSingleApplication::downloadManager()->show();
    }

    void Browser::slotSelectLineEdit(){
        _tabmanager->currentLineEdit()->selectAll();
        _tabmanager->currentLineEdit()->setFocus();
    }

    void Browser::slotFileSaveAs(){
        // not implemented yet.
    }

    void Browser::slotPreferences(){
        SettingsDialog *s = new SettingsDialog(this);
        s->show();
    }

    void Browser::slotUpdateStatusbar(const QString &string){
        status_bar()->showMessage(string, 2000);
    }

    void Browser::slotUpdateWindowTitle(const QString &title){
        if(title.isEmpty()) {
            setWindowTitle(tr(application_name));
        } else {
#if defined(Q_OS_OSX)
            setWindowTitle(title);
#else
            setWindowTitle(tr("%1 - MyTetra", "Page title and Browser name").arg(title));
#endif
        }
    }

    void Browser::slotAboutApplication(){
        QMessageBox::about(this, tr("About"), tr(
                "Version %1"
                "<p>This demo demonstrates the facilities "
                "of Qt WebEngine in action, providing an note with "
                "browser for you to experiment with.<p>"
                "<p>Qt WebEngine is based on the Chromium open source project "
                "developed at <a href=\"http://www.chromium.org/\">http://www.chromium.org/</a>.")
            .arg(QCoreApplication::applicationVersion()));
    }

    void Browser::slotFileNew(){
        // QtSingleApplication::instance()->
        // _browser->new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));

        // if(_entrance->window_list().count() == 0) { // ?
        // globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
        // }

        // browser::Browser *browser = _entrance->activiated_registered().first;   //QtSingleApplication::instance()->mainWindow();
        // assert(browser == this); // ?
        // browser->slotHome();

        this->slotHome();
    }

    void Browser::slotFileOpen(){
        QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
        if(file.isEmpty()) return;
        loadPage(file);
    }
#define QT_NO_PRINTPREVIEWDIALOG

    void Browser::slotFilePrintPreview(){
#ifndef QT_NO_PRINTPREVIEWDIALOG
        if(! currentTab()) return;
        QPrintPreviewDialog *dialog = new QPrintPreviewDialog(this);
        connect(dialog, &QPrintPreviewDialog::paintRequested, currentTab(), &WebView::print);
        dialog->exec();
#endif
    }

    void Browser::slotFilePrint(){
#if defined(QWEBENGINEPAGE_PRINT)
        if(! currentTab()) return;
        printRequested(currentTab()->page()->mainFrame());
#endif
    }

#if defined(QWEBENGINEPAGE_PRINT)
    void BrowserWindow::printRequested(QWebEngineFrame *frame){
#ifndef QT_NO_PRINTDIALOG
        QPrinter printer;
        QPrintDialog *dialog = new QPrintDialog(&printer, this);
        dialog->setWindowTitle(tr("Print Document"));
        if(dialog->exec() != QDialog::Accepted) return;
        frame->print(&printer);
#endif
    }
#endif

    void Browser::slotPrivateBrowsing(){
        if(! QtSingleApplication::instance()->privateBrowsing()) {
            QString title = tr("Are you sure you want to turn on private browsing?");
            QString text = tr("<b>%1</b><br><br>"
                    "This action will reload all open tabs.<br>"
                    "When private browsing in turned on,"
                    " webpages are not added to the history,"
                    " items are automatically removed from the Downloads window,"
                    " new cookies are not stored, current cookies can't be accessed,"
                    " site icons wont be stored, session wont be saved, "
                    " and searches are not added to the pop-up menu in the Google search box."
                    "  Until you close the window, you can still click the Back and Forward buttons"
                    " to return to the webpages you have opened.")
                .arg(title);

            QMessageBox::StandardButton button = QMessageBox::question(this, QString(), text,
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Ok);
            if(button == QMessageBox::Ok) QtSingleApplication::instance()->setPrivateBrowsing(true);
        } else {
            // TODO: Also ask here
            QtSingleApplication::instance()->setPrivateBrowsing(false);
        }
    }

    void Browser::resizeEvent(QResizeEvent *e){
        _tabmanager->resizeEvent(e);
    }

    void Browser::closeEvent(QCloseEvent *event){
        if(_tabmanager->count() > 1) {
            int ret = QMessageBox::warning(this, QString(),
                    tr("Are you sure you want to close the window?"
                    "  There are %1 tabs open")
                    .arg(_tabmanager->count()),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No);
            if(ret == QMessageBox::No) {
                event->ignore();
                return;
            }
        }
        event->accept();
        deleteLater();
    }

    void Browser::slotEditFind(){
        if(! currentTab()) return;
        bool ok;
        QString search = QInputDialog::getText(this, tr("Find"), tr("Text:"), QLineEdit::Normal, _lastsearch, &ok);
        if(ok && ! search.isEmpty()) {
            _lastsearch = search;
            currentTab()->findText(_lastsearch, 0, invoke(this, &Browser::handle_find_text_result));
        }
    }

    void Browser::slotEditFindNext(){
        if(! currentTab() && ! _lastsearch.isEmpty()) return;
        currentTab()->findText(_lastsearch);
    }

    void Browser::slotEditFindPrevious(){
        if(! currentTab() && ! _lastsearch.isEmpty()) return;
        currentTab()->findText(_lastsearch, QWebEnginePage::FindBackward);
    }

    void Browser::slotViewZoomIn(){
        if(! currentTab()) return;
        currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    }

    void Browser::slotViewZoomOut(){
        if(! currentTab()) return;
        currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    }

    void Browser::slotViewResetZoom(){
        if(! currentTab()) return;
        currentTab()->setZoomFactor(1.0);
    }

    void Browser::slotViewFullScreen(bool makeFullScreen){
        if(makeFullScreen) {
            showFullScreen();
        } else {
            if(isMinimized()) showMinimized();
            else if(isMaximized()) showMaximized();
            else showNormal();
        }
    }

    void Browser::slotViewPageSource(){
        if(! currentTab()) return;
        QPlainTextEdit *view = new QPlainTextEdit;
        view->setWindowTitle(tr("Page Source of %1").arg(currentTab()->title()));
        view->setMinimumWidth(640);
        view->setAttribute(Qt::WA_DeleteOnClose);
        view->show();

        currentTab()->page()->toHtml(invoke(view, &QPlainTextEdit::setPlainText));
    }

    // deprecated by record::preoperty::home
    void Browser::slotHome(){
        QSettings settings;
        settings.beginGroup(QLatin1String("MainWindow"));
        QString home = settings.value(QLatin1String("home"), QLatin1String(_defaulthome)).toString();
        auto tree_view = _tree_screen->view();
// boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

        auto current_item = tree_view->current_item();
        auto parent = current_item->parent();
        if(! parent) {qDebug() << "!parent"; throw std::exception(); }
        TreeIndex::instance([&] { return tree_view->source_model(); }, parent, current_item)->item_bind(
            tree_view->current_item(), QUrl(home), std::bind(&KnowView::view_paste_child, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), [&] (boost::intrusive_ptr<const TreeItem> it_) -> bool {
                return it_->field<url_type>() == home;
            })
        ->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));
// }
        settings.endGroup();
    }

    void Browser::slotWebSearch(){
        // _toolbarsearch
        _find_screen->toolbarsearch()->lineEdit()->selectAll();
        // _toolbarsearch
        _find_screen->toolbarsearch()->lineEdit()->setFocus();
    }

    void Browser::slotToggleInspector(bool enable){
#if defined(QWEBENGINEINSPECTOR)
        QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::DeveloperExtrasEnabled, enable);
        if(enable) {
            int result = QMessageBox::question(this, tr("Web Inspector"),
                    tr("The web inspector will only work correctly for pages that were loaded after enabling.\n"
                    "Do you want to reload all pages?"),
                    QMessageBox::Yes | QMessageBox::No);
            if(result == QMessageBox::Yes) {
                _tabwidget->reloadAllTabs();
            }
        }
#else
        Q_UNUSED(enable);
#endif
    }

    void Browser::slotSwapFocus(){
        if(currentTab()->hasFocus()) _tabmanager->currentLineEdit()->setFocus();
        else currentTab()->setFocus();
    }

    void Browser::loadPage(const QString &page){
        QUrl url = QUrl::fromUserInput(page);
        loadUrl(url);
    }

    // BrowserTabManager *BrowserWindow::tabWidget() const
    // {
    // return _tabwidget;
    // }

    WebView *Browser::currentTab() const {
        return _tabmanager->currentWebView();
    }

    void Browser::slotLoadProgress(int progress){
        if(progress < 100 && progress > 0) {
            _chasewidget->setAnimated(true);
            disconnect(_stopreload, &QAction::triggered, _reload, &QAction::trigger);
            if(_stopicon.isNull()) _stopicon = QIcon(":/resource/pic/mobile_stop.svg"); // style()->standardIcon(QStyle::SP_BrowserStop);

            _stopreload->setIcon(_stopicon);
            connect(_stopreload, &QAction::triggered, _stop, &QAction::trigger);
            _stopreload->setToolTip(tr("Stop loading the current page"));
        } else {
            _chasewidget->setAnimated(false);
            disconnect(_stopreload, &QAction::triggered, _stop, &QAction::trigger); // &QAction::setChecked

            _stopreload->setIcon(_reloadicon);

            connect(_stopreload, &QAction::triggered, _reload, &QAction::trigger); // &QAction::setChecked
            _stopreload->setToolTip(tr("Reload the current page"));
        }
    }

    void Browser::slotAboutToShowBackMenu(){
        _historybackmenu->clear();
        if(! currentTab()) return;
        QWebEngineHistory *history = currentTab()->history();
        int historyCount = history->count();
        for(int i = history->backItems(historyCount).count() - 1; i >= 0; -- i) {
            QWebEngineHistoryItem item = history->backItems(history->count()).at(i);
            QAction *action = new QAction(this);
            action->setData(- 1 * (historyCount - i - 1));
            QIcon icon = QtSingleApplication::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historybackmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowForwardMenu(){
        _historyforwardmenu->clear();
        if(! currentTab()) return;
        QWebEngineHistory *history = currentTab()->history();
        int historyCount = history->count();
        for(int i = 0; i < history->forwardItems(history->count()).count(); ++ i) {
            QWebEngineHistoryItem item = history->forwardItems(historyCount).at(i);
            QAction *action = new QAction(this);
            action->setData(historyCount - i);
            QIcon icon = QtSingleApplication::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historyforwardmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowWindowMenu(){
        _windowmenu->clear();
        _windowmenu->addAction(_tabmanager->nextTabAction());
        _windowmenu->addAction(_tabmanager->previousTabAction());
        _windowmenu->addSeparator();
        _windowmenu->addAction(tr("Downloads"), this, SLOT(slotDownloadManager()), QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));
        _windowmenu->addSeparator();

        auto &_browsers = _entrance->browsers(); // QtSingleApplication::instance()->mainWindows();
        size_t index = 0;
        for(auto &window : _browsers) { // for(size_t i = 0; i < _browsers.size(); i++) {
            // auto window = it;   // _browsers[i];
            QAction *action = _windowmenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
            action->setData(static_cast<uint>(index));
            action->setCheckable(true);
            if(window == this) action->setChecked(true);
            index ++;
        }
    }

    void Browser::slotShowWindow(){
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            QVariant v = action->data();
            if(v.canConvert<uint>()) {
                uint offset = qvariant_cast<uint>(v);
                auto &_browsers = _entrance->browsers(); // QtSingleApplication::instance()->mainWindows();
                auto window = [&] () -> Browser* {
                        Browser *r = nullptr;
                        size_t index = 0;
                        for(auto &it : _browsers) {
                            if(index == offset) {
                                r = it;
                                break;
                            }
                            index ++;
                        }
                        return r;
                    } (); // _browsers[offset];
                if(window) {
                    window->activateWindow();
                    window->currentTab()->setFocus();
                }
            }
        }
    }

    void Browser::slotOpenActionUrl(QAction *action){
        int offset = action->data().toInt();
        QWebEngineHistory *history = currentTab()->history();
        if(offset < 0) history->goToItem(history->backItems(- 1 * offset).first()); // back
        else if(offset > 0) history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
    }

    void Browser::geometry_change_requested(const QRect &geometry){
        setGeometry(geometry);
    }

    // void DockedWindow::initUrl()
    // {

    //// newMainWindow() needs to be called in main() for this to happen
    ////    if(_browser->mainWindows().count() > 0) {
    ////        QStringList args = QCoreApplication::arguments();

    ////        if(args.count() > 1)
    ////            loadPage(args.last());
    ////        else
    ////            slotHome();
    ////    }
    // }

    QStatusBar *Browser::status_bar(){
        return globalparameters.status_bar();
    }

    QStatusBar *Browser::status_bar() const {
        return globalparameters.status_bar();
    }

    boost::intrusive_ptr<TreeItem> Browser::item_bind(boost::intrusive_ptr<RecordIndex> record_modelindex){
        boost::intrusive_ptr<TreeItem> result(nullptr);
        boost::intrusive_ptr<TreeItem> tab_brother = record_modelindex->sibling();
        boost::intrusive_ptr<TreeItem> target = record_modelindex->target();
        if(target->is_lite()) target->to_fat();
        // clean();
        // assert(_it->is_registered_to_browser() || _it->field("url") == browser::Browser::_defaulthome);
        assert(tab_brother != target);

        WebView *view = nullptr;
        TabWidget *const tab = tabWidget();
        // if(_mainWindows.isEmpty()) {
        // dp = new_dockedwindow(record);
        // } else
        // {
        // for(auto &i : _mainWindows) {
        view = tabWidget()->find([&] (boost::intrusive_ptr<const ::Binder> b) {
                    return b->host()->field<url_type>() == target->field<url_type>() && b->host()->id() == target->id() && b == target->binder();
                }); // if _item->field("url") == Browser::_defaulthome , force rebind
        // if(view != nullptr) {
        ////            dp.first = i.data();
        // if(!isVisible()) {
        // raise();
        // activateWindow();
        // }
        ////            break;
        // } else
        if(! view) {

            //// Record *blank_url = check_register_record(QUrl(DockedWindow::_defaulthome));

            ////            if(blank.isLite())blank.switchToFat();
            ////            blank.setNaturalFieldSource("url", DockedWindow::_defaulthome);

            // WebView *blankview = tab->find([&](boost::intrusive_ptr<const TreeItem> it) {return it->field("url") == Browser::_defaulthome;}); // QUrl(Browser::_defaulthome)

            ////            //PageView *no_pin = nullptr;

            ////            WebView *nopin_view = tab->find_nopin();

            ////            // assert(dp.first);

            // if(blankview != nullptr) {
            // view = blankview;
            ////                boost::intrusive_ptr<TreeIndex> tree_index;

            ////                try {
            ////                    tree_index = new TreeIndex([&] {return globalparameters.tree_screen()->tree_view()->source_model();}, _item->parent(), _item->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il->host() == _item && _item->linker() == il && _item->parent() == il->host_parent();}));
            ////                } catch(std::exception &e) {throw e;}
            // result = view->page()->item_bind(_item);
            // result->activate();//                view->page()->load(record);
            // }
            ////            else if(nopin_view != nullptr) {   // no_pin
            ////                view = nopin_view;

            ////                if(view->page()->url().toString() != _it->field("url")) {
            ////                    result = view->page()->item_bind(_it);
            ////                    result->activate(); // view->page()->load(record);
            ////                }
            ////            }
            // else {

            view = tab->newTab(record_modelindex); // , false
            result = view->page()->binder()->host();
            //// auto load
            // }

            ////            tab->setCurrentWidget(dp.second);   // tab->setCurrentIndex(tab->webViewIndex(dp.second));
            ////            dp.second->show();
        } else {
            result = view->page()->item();
        }
        // tab->setCurrentWidget(dp.second);
        // dp.second->show();
        // assert(dp.first);
        if(_entrance->widget() != this) _entrance->setWidget(this);
        auto vtab = globalparameters.vtab_tree();
        if(vtab->currentWidget() != _record_screen) {
            vtab->setCurrentWidget(_record_screen);
        }
        assert(view);
        assert(result->binder()->integrity_external(result, view->page()));
        return result; // _mainWindows[0];
    }

    RecordScreen *Browser::record_screen() {
        return _record_screen;
    }

    Entrance *Browser::entrance() {
        return _entrance;
    }
}
