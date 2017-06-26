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


//#include <mutex>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QtCore/QSettings>

#include <QDockWidget>
#include <QStackedWidget>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QtCore/QDebug>
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

//#include "models/tree/tree_item.dec"

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
#include "toolbar_search.h"
#include "ui_passworddialog.h"
#include "webview.h"

#include "controllers/record_table/record_controller.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "views/browser/bookmarks.h"
#include "views/browser/docker.h"
#include "views/browser/history.h"
#include "views/browser/tabwidget.h"
#include "views/browser/toolbar_search.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<gl_para> gl_paras;
// Record *default_record = nullptr;
const char* profile_storage_name = "cookie";
extern const char* program_title;

const url_value web::Browser::_defaulthome = url_value("about:blank");

namespace web {
    template <typename Arg, typename R, typename C>
    struct InvokeWrapper {
        R* receiver;
        void (C::*memberFun)(Arg);
        void operator()(Arg result)
        {
            (receiver->*memberFun)(result);
        }
    };

    template <typename Arg, typename R, typename C>
    InvokeWrapper<Arg, R, C> invoke(R* receiver, void (C::*memberFun)(Arg))
    {
        InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};

        return wrapper;
    }

    UrlRequestInterceptor::UrlRequestInterceptor(QObject* p)
        : QWebEngineUrlRequestInterceptor(p)
    {
    }

    void UrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
    {
        if (info.navigationType() == QWebEngineUrlRequestInfo::NavigationTypeLink) info.requestUrl();
    }

    Profile::~Profile()
    {
        _urlrequestinterceptor->deleteLater();
    }

    Profile::Profile(const QString& name, QObject* p)
        : QWebEngineProfile(name, p) // QQuickWebEngineProfile(p)
        , _urlrequestinterceptor(new UrlRequestInterceptor(p))
    {
        // setStorageName(profile_storage_name);
        setRequestInterceptor(_urlrequestinterceptor);

        // QSettings settings;

        // settings.beginGroup(QLatin1String("downloadmanager"));
        // QString downloadDirectory =
        // settings.value(QLatin1String("downloadDirectory")).toString();
        //// downloadsLocation->setText(downloadDirectory);
        // settings.endGroup();

        //// this group lead chrome_odthread deadlock? or initialize failure
        // settings.beginGroup(QLatin1String("cookies"));

        // setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
        // setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::ForcePersistentCookies);
        // // AllowPersistentCookies

        // QWebEngineProfile::PersistentCookiesPolicy persistentCookiesPolicy =
        // QWebEngineProfile::PersistentCookiesPolicy(
        // settings.value(QLatin1String("persistentCookiesPolicy")).toInt()    //
        // QWebEngineProfile::ForcePersistentCookies
        // //vQWebEngineProfile::AllowPersistentCookies   //
        // );
        // setPersistentCookiesPolicy(persistentCookiesPolicy);

        // QString persistent_data_path =
        // settings.value(QLatin1String("persistentDataPath")).toString();

        // if(persistent_data_path == "")persistent_data_path = downloadDirectory;

        // QDir
        // cookie_path(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::CacheLocation));
        // // persistent_data_path

        // if(!cookie_path.exists()) {
        ////        QDir dir;
        // cookie_path.mkpath(persistent_data_path);
        // }

        // setPersistentStoragePath(persistent_data_path);

        // settings.endGroup();
    }

    // const char *DockedWindow::_defaulthome = "about:blank";


    void Browser::setup_navigate(void)
    {
        // assert(globalparameters.entrance());
        // assert(globalparameters.entrance()->activebrowser());
        // this->addAction(globalparameters.entrance()->activebrowser()->historyback());

        // historyback_ = new FlatToolButton(this);
        // historyback_->setStatusTip(tr("Back"));
        // historyback_->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0,
        // this));    //QIcon(":/resource/pic/pentalpha.svg");
        // historyback_->setObjectName("Back");

        // _history_home = new FlatToolButton(this);

        // _historyhome = new QAction(tr("Home"), _navigater);

        _historyhome->setIcon(QIcon(":/resource/pic/mobile_up.svg")); // ":/resource/pic/streamline_home.svg"
        // style()->standardIcon(QStyle::SP_ArrowUp, 0, this)

        _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

        add_action<QToolButton>(_navigater, _historyhome); // _navigater->addAction(_historyhome);

        // _history_back = new FlatToolButton(this);

        // _historyback = new QAction(tr("Back"), _navigater);

        _historyback->setShortcuts(QKeySequence::Back);
        // _historyback->setIconVisibleInMenu(false);
        _historyback->setIcon(QIcon(":/resource/pic/mobile_back.svg")); // QIcon(":/resource/pic/walk_history_next.svg")//
        // style()->standardIcon(QStyle::SP_ArrowBack, 0, this)

        // QMenu *_historybackmenu = new QMenu(this);
        // _historyback->setMenu(_historybackmenu);
        // connect(_historybackmenu, SIGNAL(aboutToShow()), this,
        // SLOT(slotAboutToShowBackMenu()));
        // connect(_historybackmenu, SIGNAL(triggered(QAction *)), this,
        // SLOT(slotOpenActionUrl(QAction *)));
        add_action<QToolButton>(_navigater, _historyback); // _navigater->addAction(_historyback);
        // insertActionAsButton(_container, _historyback);

        // _history_forward = new FlatToolButton(this);

        // _historyforward = new QAction(tr("Forward"), _navigater);

        _historyforward->setShortcuts(QKeySequence::Forward);
        _historyforward->setIcon(QIcon(":/resource/pic/mobile_forward.svg")); // style()->standardIcon(QStyle::SP_ArrowForward,
                                                                              // 0, this)

        // _historyforwardmenu = new QMenu(this);
        // connect(_historyforwardmenu, SIGNAL(aboutToShow()), this,
        // SLOT(slotAboutToShowForwardMenu()));
        // connect(_historyforwardmenu, SIGNAL(triggered(QAction *)), this,
        // SLOT(slotOpenActionUrl(QAction *)));
        // _historyforward->setMenu(_historyforwardmenu);
        add_action<QToolButton>(_navigater, _historyforward); // _navigater->addAction(_historyforward);

        // _stop_reload = new FlatToolButton(this);

        // _stopreload = new QAction(_navigater);

        // _reloadicon = style()->standardIcon(QStyle::SP_BrowserReload);
        _stopreload->setIcon(QIcon(":/resource/pic/mobile_reload.svg")); // style()->standardIcon(QStyle::SP_BrowserReload)

        add_action<QToolButton>(_navigater, _stopreload); // _navigater->addAction(_stopreload);
    }

    void Browser::init()
    {
        // before construct record_screen, we need final browser menu set up,
        // for record_view will need it to set up content menu
        init_main_menu();
        // reset_find_screen_navigater();

        setup_navigate();
        // QUrl const &url,
        // browser->setWidget(this);
        // this->setParent(browser);
        setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
        // setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        setWebAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        setToolButtonStyle(Qt::ToolButtonFollowStyle);
#ifdef USE_QT_DELETE_ON_CLOSE
        setAttribute(Qt::WA_DeleteOnClose, true);
#endif // USE_QT_DELETE_ON_CLOSE
        // statusBar()
        gl_paras->status_bar()->setSizeGripEnabled(true);
        //		init_main_menu();
        //		reset_find_screen_tool_bar();

        // QWidget *_centralwidget = new QWidget(this);
        // BookmarksModel *bookmarksModel =
        // QtSingleApplication::bookmarksManager()->bookmarksModel();
        // _bookmarkstoolbar = new BookmarksToolBar(bookmarksModel, this);
        connect(_bookmarkstoolbar, &BookmarksToolBar::openUrl, _tab_widget, &TabWidget::loadUrlInCurrentTab);
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
        _layout->addWidget(_tab_widget);
        _centralwidget->setLayout(_layout);
        setCentralWidget(_centralwidget);

        connect(_tab_widget, &TabWidget::openLinkInNewTab, // this, &Browser::loadPage
            [&](const QString& search_text) {
                //			    assert(to_be_url(search_text) != QUrl());
                tv_t* tree_view = _tree_screen->view();
                auto it = tree_view->session_root_auto();
                real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(url_value(search_text),
                    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                        return TreeIndex::url_activate(
                            real_target_url_, //real_url_t<url_value>::instance(url_value(search_text)),
                            [&] { return tree_view->source_model(); },
                            it,
                            std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                            [&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(detail::to_qstring(it_->field<home_key>()), search_text) || url_equal(detail::to_qstring(it_->field<url_key>()), search_text); },
                            browser_ref(this));
                    });
            });
        connect(_tab_widget, &TabWidget::setCurrentTitle, _main_window, &wn_t::synchronize_title); // this, &Browser::slotUpdateWindowTitle
        //			, [&](const QString &title_){
        ////				auto it = _tab_widget->current_item();
        //				_main_window->synchronize_title(title_);//it ?
        //it->field<name_type>() : ""
        //			}
        //			);
        //		_tab_widget->connect(&TabWidget::showStatusBarMessage, gl_paras->status_bar(), &QStatusBar::showMessage);
        connect(_tab_widget, &TabWidget::showStatusBarMessage, gl_paras->status_bar(), &QStatusBar::showMessage);
        connect(_tab_widget, &TabWidget::linkHovered, gl_paras->status_bar(), [&](const QString& link) {
            // &QStatusBar::showMessage
            gl_paras->status_bar()->showMessage(link);
        });
        connect(_tab_widget, &TabWidget::loadProgress, this, &Browser::slotLoadProgress);
        connect(_tab_widget, &TabWidget::tabsChanged, _autosaver, &AutoSaver::changeOccurred);
        connect(_tab_widget, &TabWidget::geometryChangeRequested, this, &Browser::geometry_change_requested);
#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(_tab_widget, &TabWidget::printRequested, this, &Browser::printRequested);
#endif
        connect(_tab_widget, &TabWidget::menuBarVisibilityChangeRequested, menuBar(), &QMenuBar::setVisible);
        connect(_tab_widget, &TabWidget::statusBarVisibilityChangeRequested, gl_paras->status_bar(), &QStatusBar::setVisible);
        // connect(_tab_widget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
        // navigater, SLOT(setVisible(bool)));
        connect(_tab_widget, &TabWidget::toolBarVisibilityChangeRequested, _bookmarkstoolbar, &BookmarksToolBar::setVisible);
#if defined(Q_OS_OSX)
        connect(_tab_widget, &TabWidget::lastTabClosed, this, &Browser::close);
#else
        connect(_tab_widget, &TabWidget::lastTabClosed, this, [&]() -> void {
            // _tab_widget->newTab(request_record(QUrl(DockedWindow::_defaulthome)));

            //				// HidableTabWidget *_vtab_tree =
            //_main_window->vtab_tree();
            //				HidableTab *_vtab_record =
            //_main_window->vtab_record();
            //				int index =
            //_vtab_record->indexOf(_record_screen);
            //				if(index != -1){
            //				        emit _vtab_record->tabCloseRequested(index); //
            //removeTab(index);
            //				        // static_cast<Browser
            //*>(globalparameters.vtab()->currentWidget())->append_to_main_menu();
            //				        // for(int i = 0; i < _vtab_tree->count(); i
            //++){
            //				        // auto wg = _vtab_tree->widget(i);
            //				        // if(wg->objectName() ==
            //tree_screen_viewer_name){
            //				        // auto tree_viewer = dynamic_cast<tsv_t
            //*>(wg);
            //				        // if(tree_viewer){
            //				        // if(tree_viewer->widget_right() ==
            //_record_screen){
            //				        // if(_main_window->tree_viewers().size() >
            //1)_vtab_tree->removeTab(i);
            //				        // else assert(dynamic_cast<DownloadManager
            //*>(_main_window->tree_viewers()[0]->widget_right()));
            //				        // }
            //				        // }
            //				        // }
            //				        // }
            //				}
            //			self_close_request();
            //
            //			int count_ = _tab_widget->count();
            //			assert(0 == count_);

            destroy_trigger_from_others()(_blogger); //this->close();

            //			auto _record_screen = _tab_widget->record_screen();
            //			if (_record_screen) {
            //				int index = _vtab_record->indexOf(_record_screen);
            //				if (index != -1)
            //					emit _vtab_record->tabBar()->tabCloseRequested(index);
            //			}
        });
#endif


        // int size = _tab_widget->lineEditStack()->sizeHint().height();

        // navigater->setIconSize(QSize(size, size));

        // initUrl();
        // dock_widget->setWidget(this);
        // setParent(dock_widget);

        // connect(_main_window, &MainWindow::setEnabled, this,
        // &DockedWindow::setEnabled);
        // connect(_main_window, &MainWindow::blockSignals, this,
        // &DockedWindow::blockSignals);

        //// test:
        // connect(const_cast<Browser *>(this), &Browser::~Browser, [&]() {
        // auto bs = _entrance->browsers();

        // for(auto i = bs.begin(); i != bs.end(); i++) {
        // if(*i == this)
        // bs.erase(i);
        // }
        // });
        auto show_bookmak_bar = _configuration->value("show_bookmark_bar").toBool();
        if (!show_bookmak_bar) {
            update_bookmarks_toolbar_action_text(false);
            _bookmarkstoolbar->close();
        }
        //		connect(this, &Browser::close_request, _blogger, &Blogger::on_close_requested);


        load_state();

        //		slotUpdateWindowTitle();
        _main_window->synchronize_title(_blogger ? _blogger->topic() : "");
#ifdef USE_SIGNAL_CLOSE
        //		// let Browser::~Browser() do it
        //		if (_tab_widget) {
        //			//			close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &web::TabWidget::on_close_requested, _tab_widget, static_cast<sd::renter* const>(this)));
        //			destroy_transfer(_tab_widget->destroy_trigger_from_others()); //std::bind(&web::TabWidget::close_requested_from_others, _tab_widget, static_cast<sd::renter* const>(this))
        //		}

        destroy_transfer([&](renter* const r) {
            (void)r;
            if (r != this) { //&& !this->_destroyed_request_sent
#ifdef USE_QT_DELETE_ON_CLOSE
                assert(this->testAttribute(Qt::WA_DeleteOnClose));
#endif // USE_QT_DELETE_ON_CLOSE
                close_trigger_from_others()(r);
                //				save();//move to Blogger::save()
                //				this->_destroyed_request_sent =
                this->close();
                //				//				delete this;
                //				if (_blogger) {
                //					//			close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &Blogger::on_close_requested, _blogger, static_cast<sd::renter* const>(this))); // close_requested.connect(_blogger->close_requested);                         //(std::bind(&Blogger::self_close_request, _blogger));
                //					//					destroy_transfer(
                //					_blogger->destroy_trigger_from_others()(this);
                //					//					    );
                //				}
            }
        }); //  std::make_shared<sd::method<sd::meta_info<void>>>("", &web::Browser::close, &_closed, this)                                                       //close_requested.connect(std::bind(&Browser::close, this));


#endif //USE_SIGNAL_CLOSE
        //		if (0 == _tab_widget->count()) _find_screen->switch_stack();

        _websearch.connect([&] {
            // _toolbarsearch
            //        static_cast<ToolbarSearch*>(_find_screen->lineedit_stack()->currentWidget())
            auto view = currentTab();
            if (view) {
                auto toolbarsearch = view->toolbarsearch();
                if (toolbarsearch) {
                    auto line = toolbarsearch->lineEdit();

                    if (line) {
                        line->selectAll();
                        // _toolbarsearch
                        //            _find_screen->lineedit_stack()->currentWidget()
                        line->setFocus();
                    }
                }
            }
        });
    }

    Browser::~Browser()
    {
//		int count_ = _tab_widget->count();
//		assert(0 == count_);
//		this->_destroyed_request_triggered = true;
#ifndef USE_SIGNAL_CLOSE
        _prepended(this);
#endif
        {
            _autosaver->saveIfNeccessary();
            //			save(); // move to blogger
            //				//		for(int i = 0; i < _tab_widget->count(); i++)
            //				//_tab_widget->webView(i)->close();
            //				//			_autosaver->changeOccurred();
            //				//			_autosaver->saveIfNeccessary();
        }

// mmove to tabmanager
//		{
//			//			HidableTab *_vtab_record = _main_window->vtab_record();
//			//			auto index = _vtab_record->indexOf(_record_screen);
//			//			if(index != -1)	emit _vtab_record->tabCloseRequested(index);
//			////_vtab_record->removeTab(index);
//			auto _record_screen = _tab_widget->record_screen();
//			if (_record_screen) {
//				_close_request.connect(std::bind(&rs_t::on_browser_close_requested, _record_screen));
//				_record_screen = nullptr;
//			}
//			//			if (_blogger) connect(this, &Browser::close_request, _blogger, &Blogger::on_browser_close_requested);

//			//			self_close_request(); //emit close_request(this);
//		}
//		//		_rctrl->close_request();
//		close_requested(this);
//		delete _tab_widget;

#ifdef USE_SIGNAL_CLOSE
        //		if (_blogger) {
        //			//			close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &Blogger::on_close_requested, _blogger, static_cast<sd::renter* const>(this))); // close_requested.connect(_blogger->close_requested);                         //(std::bind(&Blogger::self_close_request, _blogger));
        //			//					destroy_transfer(
        //			if (!_blogger->close_request_sent())
        //				_blogger->destroy_trigger_from_others()(this);
        //			//					    );
        //		}

        if (_tab_widget) {
            if (!_tab_widget->close_request_sent() && !_tab_widget->destroy_request_sent())

                //			_browser->close_connect([&](renter* const r) {if(r != this && !this->_closed) this->_closed = this->close(); });
                //			destroy_transfer(
                _tab_widget->destroy_trigger_from_others()(this);
            //				    ); //std::bind(&Browser::close_requested_from_others, _browser, static_cast<sd::renter* const>(this))
        }

        if (_blogger) {
            delete static_cast<Blogger*>(_blogger); //_blogger->destroy_trigger_from_others()(nullptr);
        }
#endif // USE_SIGNAL_CLOSE


        if (_find_screen) {
            if (_find_screen->navigater() == _navigater) _find_screen->switch_navigater();
        }
    }

    void Browser::run_script(const QString& style_source)
    {
        QMetaObject::invokeMethod(this, "runScriptOnOpenViews", Qt::QueuedConnection, Q_ARG(QString, style_source));
    }

// boost::intrusive_ptr<TreeItem>
// Browser::item_bind(boost::intrusive_ptr<TreeItem> item)
// {
////        auto binder = [](boost::shared_ptr<TabWidget::Coupler> ar) {
////            return
///std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
///web::WebView *, boost::intrusive_ptr<TreeItem>>>("",
///&TabWidget::Coupler::binder, ar);   // ,
///boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)

////        };
////        auto activator = [](boost::shared_ptr<TabWidget::Coupler> ar) {
////            return
///std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
///web::WebView *>> ("", &TabWidget::Coupler::activator, ar);
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
////                                std::make_shared<bounded_item_interface>("",
///&TabWidget::Coupler::bounded_item, ar)
////                                ,
///std::make_shared<bounded_page_interface>("",
///&TabWidget::Coupler::bounded_page, ar)
////                                , std::make_shared<bind_interface>("",
///&TabWidget::Coupler::binder, ar)   // binder(ar)
////                                , std::make_shared<activate_interface> ("",
///&TabWidget::Coupler::activator, ar)   // activator(ar)
////                            ));
////        //        _tab_widget->newTab(record);
////        //        assert(record->binded_only_page());
// return _tab_widget->item_bind(item);
// }

// boost::intrusive_ptr<TreeItem> Browser::item_request_from_tree(
// QUrl const &url
// , const TreeScreen::paste_strategy &_view_paste_strategy
// , equal_url_t _equal
// )
// {
// auto tree_screen = globalparameters.tree_screen();
////        connect(this, &DockedWindow::activateWindow, _entrance,
///&Entrance::on_activate_window);

// return tree_screen->item_request_from_tree(url, _view_paste_strategy,
// _equal);
// }

// boost::intrusive_ptr<TreeItem> Browser::item_request_from_tree(
// boost::intrusive_ptr<TreeItem> item
// , const TreeScreen::paste_strategy &_view_paste_strategy
// , equal_t _equal
// )
// {
// auto tree_screen = globalparameters.tree_screen();
////        connect(this, &DockedWindow::activateWindow, _entrance,
///&Entrance::on_activate_window);

// return tree_screen->item_request_from_tree(item, _view_paste_strategy,
// _equal);
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
////        ,  boost::intrusive_ref_counter<Browser,
///boost::thread_safe_counter>()
// , _tree_screen(_tree_screen)
// , _find_screen(_find_screen)
// , _record_screen(new RecordScreen(_find_screen
// , _editor_screen
// , this
// , _vtabwidget
// , _main_window))
// , _main_window(_main_window)
// , _tab_widget(_record_screen->tabmanager())
////        , _toolbarsearch(_find_screen->toolbarsearch())
// , _bookmarkstoolbar(new
// BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(),
// this))
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
// , _entrance(_entrance->prepend(this))         //    , dock_widget(new
// QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
// {
// init();
////        auto r = request_item(QUrl(web::Browser::_defaulthome));
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
////        ,  boost::intrusive_ref_counter<Browser,
///boost::thread_safe_counter>()
// , _tree_screen(_tree_screen)
// , _find_screen(_find_screen)
// , _record_screen(new RecordScreen(_find_screen
// , _editor_screen
// , this
// , _vtabwidget
// , _main_window))
// , _main_window(_main_window)
// , _tab_widget(_record_screen->tabmanager())
////        , _toolbarsearch(_find_screen->toolbarsearch())
// , _bookmarkstoolbar(new
// BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(),
// this))
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
// , _entrance(_entrance->prepend(this))         //    , dock_widget(new
// QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
// {
// init();

// _tree_screen->item_register_and_bind(
// url
// , std::bind(&TreeScreen::view_paste_as_child, _tree_screen,
// std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
// )->activate();

// run_script(style_source);       //        assert(record->linkpage());

// QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

// QMainWindow::menuBar()->hide();
// QMainWindow::statusBar()->hide();
// show();
// }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(Browser)
#endif

    Browser::Browser(Blogger* blogger_, // sd::intrusive_ptr<rs_t> record_screen_,
        const QByteArray& state_,       //= QByteArray(), QStringList hide_editor_tools_ //=/appconfig->hide_editor_tools(), HidableTab *vtab_record //= gl_paras->vtab_record(), const QString &style_source //=gl_paras->style_source(), web::Profile *profile// = gl_paras->profile()
        Qt::WindowFlags flags           // = Qt::MaximizeUsingFullscreenGeometryHint
        )
        : QMainWindow(0, flags) // ,  boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>()
#ifdef USE_SIGNAL_CLOSE
        , sd::renter() //sd::renter<Browser>()
#endif
        , _blogger([&] {
			static_assert(Browser::initialize_prior_to<Blogger>::value == false, "Browser instance need a blogger was ready");
			assert(blogger_);
			return blogger_; }()) // new Blogger(vtab_record, this, hide_editor_tools_, new_post_topic, new_post_content)
        , _configuration_full_name(_blogger->current_topic_folder_name() + "/" + gl_para::_browser_conf_filename)
        , _configuration([&]() -> std::unique_ptr<QSettings> {
            auto _current_topic_folder_name = _blogger->current_topic_folder_name();
            if (!QDir(_current_topic_folder_name).exists())
                if (!QDir::root().mkpath(_current_topic_folder_name)) critical_error("Can not create directory: \"" + _current_topic_folder_name + "\"");
            auto _current_topic_config_name = _current_topic_folder_name + "/" + gl_para::_browser_conf_filename;
            if (!QFile(_current_topic_config_name).exists())
                if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + ::gl_para::_editor_conf_filename, _current_topic_config_name)) critical_error(
                    QString("Can not copy \"") + ::gl_para::_editor_conf_filename + "\""); // throw std::runtime_error("Can not copy document.ini");
            if ((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(_current_topic_config_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(_current_topic_config_name, QFile::ReadUser | QFile::WriteUser);
            return std::make_unique<QSettings>(_current_topic_config_name, QSettings::IniFormat);
        }())
        , _state([&] {
            auto st = state_;
            if (st == QByteArray()) {
                st = DiskHelper::get_topic_from_directory(gl_paras->root_path() + "/" + gl_para::_blog_root_dir, _blogger->topic());
            };
            return st;
        }())
        , _vtab_record(gl_paras->vtab_record())
        , _tree_screen(gl_paras->tree_screen())
        , _find_screen(gl_paras->find_screen())
        , _main_window(gl_paras->main_window())
        , _navigater([&] {
            _navigater = nullptr;
            auto nav = new QToolBar(this);
            nav->setObjectName(gl_para::_browser_navigater_name);
            nav->setMaximumWidth(130);
            return nav;
        }())
        //	    , _toolbarsearch(_find_screen->toolbarsearch())
        , _bookmarkstoolbar(new BookmarksToolBar(::sapp_t::bookmarksManager()->bookmarksModel(), this))
        , _file_menu([&]() -> QMenu* {
			auto fm = new QMenu(tr("&File"), this);
			fm->setContentsMargins(0, 0, 0, 0);
			return fm; }())
        , _edit_menu([&]() -> QMenu* {
			auto fm = new QMenu(tr("&Edit"), this);
			fm->setContentsMargins(0, 0, 0, 0);
			return fm; }())
        , _view_menu([&]() -> QMenu* {
			auto fm = new QMenu(tr("&View"), this);
			fm->setContentsMargins(0, 0, 0, 0);
			return fm; }())
        , _tools_menu(_main_window->tools_menu()) //([&]() -> QMenu *{auto fm = new QMenu(tr("&Tools"), this); fm->setContentsMargins(0, 0, 0, 0); return fm;} ())
        , _bookmarks_menu(new BookmarksMenu())
#ifdef USE_CLEAR_BUTTON
        , _chasewidget(_find_screen->chasewidget())
#endif // USE_CLEAR_BUTTON
        , _history_menu(new HistoryMenu(this))
        , _historyhome(new QAction(tr("Home"), _navigater))       //(_find_screen->historyhome())
        , _historyback(new QAction(tr("Back"), _navigater))       //(_find_screen->historyback())
        , _historyforward(new QAction(tr("Forward"), _navigater)) //(_find_screen->historyforward())
        , _windowmenu([&]() -> QMenu* {
			auto fm = new QMenu(tr("&Window"), this);
			fm->setContentsMargins(0, 0, 0, 0);
			return fm; }())
        , _stop(new QAction(tr("&Stop"), _navigater))         //(_find_screen->stop())
        , _reload(new QAction(tr("Reload Page"), _navigater)) //(_find_screen->reload())
        , _stopreload(new QAction(_navigater))                //(_find_screen->stopreload())
        , _autosaver(new AutoSaver(this))
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
        //	    , _record_screen(record_screen_)
        , _tab_widget(new web::TabWidget(blogger_, const_cast<Browser*>(this), _main_window)
              //		  [&] {
              //		    //		    _tab_widget = nullptr;
              //		    return //auto t =
              //#ifdef USE_SIGNAL_CLOSE
              //			sd::make_intrusive<web::TabWidget>(blogger_, this, _main_window); //, gl_paras->profile()
              //#else
              //			new web::TabWidget(blogger_, this, _main_window); //, gl_paras->profile()
              //#endif //USE_SIGNAL_CLOSE

              //		    //			  close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &web::TabWidget::on_close_requested, t, static_cast<sd::renter* const>(this))); //close_requested.connect(t->self_close_request);//(std::bind(&web::TabWidget::self_close_request,t));
              //		    //std::make_shared<sd::method<sd::meta_info<void>>>("", &web::TabWidget::close, &_closed, this)
              ////		    return std::forward<sd::intrusive_ptr<web::TabWidget>>(t);
              //	    }()
              )
        , _browser_docker(gl_paras->browser_docker()->prepend(this))
    { // , dock_widget(new QDockWidget(parent,
        // Qt::MaximizeUsingFullscreenGeometryHint))
        //		_tab_widget = sd::make_intrusive<web::TabWidget>(blogger_, this, _main_window);
        init();

        run_script(gl_paras->style_source()); // assert(record->linkpage());

        QMainWindow::setWindowFlags(Qt::FramelessWindowHint); // Qt::Window |

        QMainWindow::menuBar()->hide();
        _bookmarkstoolbar->hide(); //:)
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
    // : gl_paras->editors_shared_full_path_name()(0, flags)
    ////        ,  boost::intrusive_ref_counter<Browser,
    ///boost::thread_safe_counter>()
    // , _tree_screen(_tree_screen)
    // , _find_screen(_find_screen)
    // , _record_screen(new RecordScreen(_find_screen
    // , _editor_screen
    // , this
    // , _vtabwidget
    // , _main_window))
    // , _main_window(_main_window)
    // , _tab_widget(_record_screen->tabmanager())
    ////        , _toolbarsearch(_find_screen->toolbarsearch())
    // , _bookmarkstoolbar(new
    // BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(),
    // this))
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
    // , _entrance(_entrance->prepend(this))         //    , dock_widget(new
    // QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    // {
    // assert(item);

    // init();

    // _tree_screen->item_register_and_bind(
    // item
    // , std::bind(&TreeScreen::view_paste_as_child, _tree_screen,
    // std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    // )->activate();

    // assert(item->page_valid() && item->page_link());

    // run_script(style_source);       //        assert(record->linkpage());

    // gl_paras->editors_shared_full_path_name()::setWindowFlags(Qt::FramelessWindowHint);
    // //Qt::Window |

    // gl_paras->editors_shared_full_path_name()::menuBar()->hide();
    // QMainWindow::statusBar()->hide();
    // show();

    // }

    bool Browser::is_under_construction() const
    {
        return _is_under_construction;
    }

    QAction* Browser::historyback()
    {
        return _historyback;
    }

    void Browser::activateWindow()
    {
        if (_browser_docker->widget() != this) _browser_docker->prepend(this);
        if (gl_paras->editor_docker()->widget() != _blogger) gl_paras->editor_docker()->prepend(_blogger);
        //		this->setParent(_browser_docker);
        //		_browser_docker->setWidget(this);

        //		_entrance->on_activate_window();

        //		_find_screen->switch_stack(this->tab_widget()->lineEditStack());

        //		auto _vtab_record = _record_screen->vtab_record();
        auto _record_screen = _tab_widget->record_screen();
        if (_record_screen) {
            int index = _vtab_record->indexOf(_record_screen);
            if (index != -1) {
                if (_vtab_record->currentIndex() != index) {
                    auto previous_browser = _vtab_record->currentWidget();
                    if (previous_browser) previous_browser->lower();
                    _vtab_record->setCurrentIndex(index);
                    // synchronize tree view and record view
                }
            }
        }
        // update tree current
        auto current_tree_id = _tree_screen->view()->current_item()->id();
        if (!_tab_widget->find([&](boost::intrusive_ptr<const ::Binder> b) { return b->host()->id() == current_tree_id; })) {
            auto _rctrl = _tab_widget->record_screen()->record_ctrl();
            if (_rctrl) {
                auto it = _rctrl->view()->current_item();
                if (it) {
                    _tree_screen->view()->select_as_current(TreeIndex::item_require_treeindex([&] { return _tree_screen->view()->source_model(); }, it));
                }
            }
        }
        synchronize_find_screen_reset_main_menu();
        auto it = _tab_widget->current_item();
        if (it) _main_window->synchronize_title(it->field<name_key>());
        this->setVisible(true);
        this->adjustSize();
        //		if (0 == _tab_widget->count()) _find_screen->switch_stack();
        QMainWindow::activateWindow();
    }

    bool Browser::restore_state(const QString& new_topic_)
    {
        bool result = false;
        auto state = DiskHelper::get_topic_from_directory(gl_paras->root_path() + "/" + gl_para::_blog_root_dir, new_topic_);
        if (state != QByteArray()) result = restore_state(state);
        return result;
    }


    bool Browser::restore_state(const QByteArray& state)
    {
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);

        int version = 2;
        QByteArray sd = state;
        QDataStream stream(&sd, QIODevice::ReadOnly);
        if (stream.atEnd()) return false;
        qint32 marker;
        qint32 v;
        stream >> marker;
        stream >> v;
        if (marker != Browser::browser_magic || v != version) return false;

        QString topic;
        QString title;
        QSize size;
        bool showMenuBar;
        bool showToolbar;
        bool showBookmarksBar;
        bool showStatusbar;
        QByteArray tabState;

        stream >> topic;
        stream >> title;
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
        [this](bool show) {
            _viewmenubar->setText(show ? tr("Hide Menu bar") : tr("Show Menu bar"));
        }(showMenuBar);

        _bookmarkstoolbar->setVisible(showBookmarksBar);
        update_bookmarks_toolbar_action_text(showBookmarksBar);

        gl_paras->status_bar()->setVisible(showStatusbar);
        update_statusbar_action_text(showStatusbar);
        if (!_tab_widget->restoreState(tabState)) return false;
        if (_blogger && _blogger->topic() != topic) {
            if (_blogger->title() != title) _blogger->title(title);
            _blogger->topic(topic);
            auto _current_topic_folder_name = _blogger->current_topic_folder_name();
            auto _current_topic_config_name = _current_topic_folder_name + "/" + gl_para::_browser_conf_filename;
            if (this->_configuration_full_name != _current_topic_config_name) {
                init_configuration( // std::make_unique<QSettings>(_current_topic_folder_name + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat)
                    [&]() -> std::unique_ptr<QSettings> {
                        if (!QDir(_current_topic_folder_name).exists())
                            if (!QDir::root().mkpath(_current_topic_folder_name)) critical_error("Can not create directory: \"" + _current_topic_folder_name + "\"");
                        if (!QFile(_current_topic_config_name).exists()) {
                            if (QFile(this->_configuration_full_name).exists()) {
                                //
                                if (!QFile::copy(this->_configuration_full_name, _current_topic_config_name)) critical_error(QString("Can not copy \"") + this->_configuration_full_name + "\"");
                            } else {
                                //
                                if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + ::gl_para::_editor_conf_filename, _current_topic_config_name)) critical_error(QString("Can not copy default \"") + ::gl_para::_editor_conf_filename + "\""); // throw std::runtime_error("Can not copy
                                                                                                                                                                                                                                                                                    // document.ini");
                            }
                        }
                        if ((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(_current_topic_config_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(_current_topic_config_name, QFile::ReadUser | QFile::WriteUser);
                        this->_configuration_full_name = _current_topic_config_name;
                        return std::make_unique<QSettings>(_current_topic_config_name, QSettings::IniFormat);
                    }());
            }
            //
        }
        if (this->windowTitle() == topic) this->setWindowTitle(topic);
        //			if (_record_screen) _record_screen->tabbar_topic(topic);

        return true;
    }

    void Browser::load_state()
    {
        if (_state != QByteArray()) {
            //
            restore_state(_state);
        } else {
            // QSettings _configuration(gl_paras->root_path() + "/" +
            // gl_paras->target_os() + "/" + gl_para::_browser_conf_filename,
            // QSettings::IniFormat);
            _configuration->beginGroup(QLatin1String("browser"));
            QByteArray data = _configuration->value(QLatin1String("default_state")).toByteArray();
            restore_state(data);
            _configuration->endGroup();
        }
    }

    QSize Browser::sizeHint() const
    {
        QRect desktopRect = QApplication::desktop()->screenGeometry();
        QSize size = desktopRect.size() * qreal(0.9);

        return size;
    }

    web::TabWidget* Browser::tab_widget()
    {
        return static_cast<web::TabWidget*>(_tab_widget);
    }

    web::TabWidget* Browser::tab_widget() const
    {
        return _tab_widget;
    }

    //	sd::intrusive_ptr<web::TabWidget> Browser::tabWidget()
    //	{
    //		return _tab_widget;
    //	}

    //	sd::intrusive_ptr<web::TabWidget> Browser::tabWidget() const
    //	{
    //		return _tab_widget;
    //	}

    void Browser::save()
    {
        auto get_state = [&](bool withTabs = true) -> QByteArray //const
        {
            // FindScreen *_find_screen = globalparameters.find_screen();
            assert(_find_screen);
            // QToolBar *navigater = findscreen->navigater();
            // assert(navigater);//

            int version = 2;
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            auto topic = _blogger->topic();
            stream << qint32(Browser::browser_magic);
            stream << qint32(version);
            stream << topic;
            stream << _blogger->title();
            stream << size();
            stream << menuBar()->isVisible();
            stream << !_find_screen->isHidden();
            stream << !_bookmarkstoolbar->isHidden();
            stream << !gl_paras->status_bar()->isHidden();
            if (withTabs)
                stream << _tab_widget->saveState();
            else
                stream << QByteArray();
            return data;
        };
        _tab_widget->topic_replace_traverse(_blogger->topic());
        //		::sapp_t::instance()->saveSession();//recursive call
        // std::shared_ptr<QSettings> _configuration =
        // std::make_shared<QSettings>(gl_paras->root_path() + "/" +
        // gl_paras->target_os() + "/" + gl_paras->_browser_conf_filename,
        // QSettings::IniFormat);
        // QSettings settings(globalparameters.work_directory() + "/" +
        // globalparameters.target_os() +
        // "/"+globalparameters._browser_conf_filename, QSettings::IniFormat);
        _configuration->beginGroup(QLatin1String("browser"));
        QByteArray data = get_state(); // false
        _configuration->setValue(QLatin1String("default_state"), data);
        _configuration->endGroup();
        //		if (0 == _tab_widget->count()) _find_screen->switch_stack();
    }


    std::tuple<qint32, qint32, QString, QString, QSize, bool, bool, bool, bool, std::tuple<qint32, qint32, QStringList, QString>> Browser::state(const QByteArray& state_)
    {
        //		assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);
        std::tuple<qint32, qint32, QString, QString, QSize, bool, bool, bool, bool, std::tuple<qint32, qint32, QStringList, QString>> result;
        int version = 2;
        QByteArray sd = state_;
        QDataStream stream(&sd, QIODevice::ReadOnly);
        //		if(stream.atEnd()) return false;
        if (!stream.atEnd()) {
            qint32 marker;
            qint32 v;
            stream >> marker;
            std::get<0>(result) = marker;
            stream >> v;
            std::get<1>(result) = v;
            //		if(marker != browser_magic || v != version) return false;
            if (marker == browser_magic && v == version) {
                QString topic;
                QString title;
                QSize size;
                bool showMenuBar;
                bool showToolbar;
                bool showBookmarksBar;
                bool showStatusbar;
                QByteArray tab_state;

                stream >> topic;
                std::get<3>(result) = topic;
                stream >> title;
                std::get<2>(result) = title;
                stream >> size;
                std::get<4>(result) = size;
                stream >> showMenuBar;
                std::get<5>(result) = showMenuBar;
                stream >> showToolbar;
                std::get<6>(result) = showToolbar;
                stream >> showBookmarksBar;
                std::get<7>(result) = showBookmarksBar;
                stream >> showStatusbar;
                std::get<8>(result) = showStatusbar;
                stream >> tab_state;

                std::get<9>(result) = TabWidget::state(tab_state);
            }
        }
        return result;
    }


    QAction* Browser::action_restore_last_session()
    {
        return _restorelastsession;
    }

    void Browser::runScriptOnOpenViews(const QString& source)
    {
        for (int i = 0; i < tab_widget()->count(); ++i) {
            auto browserview = tab_widget()->webView(i);
            if (browserview != nullptr) browserview->page()->runJavaScript(source);
        }
    }

    void Browser::setWebAttribute(QWebEngineSettings::WebAttribute attribute, bool enabled)
    {
        _webattribute = attribute;
        _webattributeenabled = enabled;
    }

    QString& Browser::lastsearch()
    {
        return _lastsearch;
    }

    const QString& Browser::lastsearch() const
    {
        return _lastsearch;
    }

    void Browser::init_main_menu()
    {
        auto sc = new QShortcut(QKeySequence(Qt::Key_F6), this);
        connect(sc, &QShortcut::activated, this, &Browser::slotSwapFocus);

        init_file_menu();

        init_tools_menu();

        init_edit_menu();

        init_view_menu();

        init_history_menu();

        init_bookmark_menu();

        init_window_menu();

        //		init_help_menu();

        synchronize_find_screen_reset_main_menu();

        //		auto button = _tree_screen->menus_in_button();
        //		button->clear();
        //		for (auto menu : _main_window->main_menu_map()) button->addMenu(menu.second);
        //		_tree_screen->init_context_menu();
        //		_tree_screen->append_main_menu();
        setMenuBar(_main_window->menuBar());

        auto rctrl = _tab_widget->record_screen()->record_ctrl();
        if (rctrl) rctrl->view()->assembly_context_menu(_main_window->main_menu_map());
    }

    void Browser::init_file_menu()
    {
        // File
        //		QMenu *_file_menu = _main_window->file_menu(); //
        //menuBar()->addMenu(tr("&File"));

        //		_main_window->init_file_menu();//clear

        _file_menu->addAction(tr("&New Window"), this, &Browser::slotFileNew, QKeySequence::New);
        _file_menu->addAction(_tab_widget->newTabAction()); // tr("New &Tab")
        _file_menu->addAction(tr("&Open File..."), this, &Browser::slotFileOpen, QKeySequence::Open);
        _file_menu->addAction(tr("Open &Location..."), this, &Browser::slotSelectLineEdit, QKeySequence(Qt::ControlModifier + Qt::Key_L));
        _file_menu->addSeparator();
        _file_menu->addAction(_tab_widget->closeTabAction()); // tr("&Close Tab")
        _file_menu->addSeparator();
#if defined(QWEBENGINE_SAVE_AS_FILE)
        fileMenu->addAction(tr("&Save As..."), this, &Browser::slotFileSaveAs, QKeySequence(QKeySequence::Save));
        fileMenu->addSeparator();
#endif
        BookmarksManager* bookmarksManager = ::sapp_t::bookmarksManager();
        _file_menu->addAction(tr("&Import Bookmarks..."), bookmarksManager, &BookmarksManager::importBookmarks);
        _file_menu->addAction(tr("&Export Bookmarks..."), bookmarksManager, &BookmarksManager::exportBookmarks);
        _file_menu->addSeparator();
#if defined(QWEBENGINEPAGE_PRINT)
        fileMenu->addAction(tr("P&rint Preview..."), this, &Browser::slotFilePrintPreview);
        fileMenu->addAction(tr("&Print..."), this, &Browser::slotFilePrint, QKeySequence::Print);
        fileMenu->addSeparator();
#endif
        QAction* action = _file_menu->addAction(tr("Private &Browsing..."), this, &Browser::slotPrivateBrowsing);
        action->setCheckable(true);
        action->setChecked(::sapp_t::instance()->privateBrowsing());
        connect(::sapp_t::instance(), &::sapp_t::privateBrowsingChanged, action, &QAction::setChecked);
        _file_menu->addSeparator();

        //// #if defined(Q_OS_OSX)
        //// fileMenu->addAction(tr("&Quit"), QtSingleApplication::instance(),
        ///SLOT(quitBrowser()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        //// #else
        //// fileMenu->addAction(tr("&Quit"), this, SLOT(close()),
        ///QKeySequence(Qt::CTRL | Qt::Key_Q));
        //// #endif

        // _main_window->append_quit_menu();
        //// QAction *quit = new QAction(tr("&Quit"), _main_window);
        //// quit->setShortcut(Qt::CTRL + Qt::Key_Q);
        //// connect(quit, &QAction::triggered, _main_window,
        ///&MainWindow::application_exit);
        //// filemenu->addAction(quit);
        //		auto append_quit_menu = [&]() {
        _file_menu->addAction(_main_window->quit_action());
        //		};
        //		append_quit_menu();
        //// _tree_screen->buttonmenu()->addMenu(filemenu);
    }

    void Browser::init_edit_menu()
    {
        //		// Edit
        //		_edit_menu = _main_window->edit_menu();
        //		// editMenu = _main_window->menuBar()->addMenu(tr("&Edit"));
        //		_edit_menu->clear();

        QAction* undo_ = _edit_menu->addAction(tr("&Undo"));
        undo_->setShortcuts(QKeySequence::Undo);
        _tab_widget->addWebAction(undo_, QWebEnginePage::Undo);
        QAction* redo_ = _edit_menu->addAction(tr("&Redo"));
        redo_->setShortcuts(QKeySequence::Redo);
        _tab_widget->addWebAction(redo_, QWebEnginePage::Redo);
        _edit_menu->addSeparator();
        QAction* cut_ = _edit_menu->addAction(tr("Cu&t"));
        cut_->setShortcuts(QKeySequence::Cut);
        _tab_widget->addWebAction(cut_, QWebEnginePage::Cut);
        QAction* copy_ = _edit_menu->addAction(tr("&Copy"));
        copy_->setShortcuts(QKeySequence::Copy);
        _tab_widget->addWebAction(copy_, QWebEnginePage::Copy);
        QAction* paste_ = _edit_menu->addAction(tr("&Paste"));
        paste_->setShortcuts(QKeySequence::Paste);
        _tab_widget->addWebAction(paste_, QWebEnginePage::Paste);
        _edit_menu->addSeparator();

        QAction* find_ = _edit_menu->addAction(tr("&Find"));
        find_->setShortcuts(QKeySequence::Find); // setShortcut(QKeySequence::fromString("Ctrl+F"));
        connect(find_, &QAction::triggered, this, &Browser::slotEditFind);
        // new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

        QAction* findnext_ = _edit_menu->addAction(tr("&Find Next"));
        findnext_->setShortcuts(QKeySequence::FindNext);
        connect(findnext_, &QAction::triggered, this, &Browser::slotEditFindNext);

        QAction* findprevious_ = _edit_menu->addAction(tr("&Find Previous"));
        findprevious_->setShortcuts(QKeySequence::FindPrevious);
        connect(findprevious_, &QAction::triggered, this, &Browser::slotEditFindPrevious);
        _edit_menu->addSeparator();

        _edit_menu->addAction(tr("&Browser preferences"), this, &Browser::slotPreferences, tr("Ctrl+,"));
    }

    void Browser::init_view_menu()
    {
        //		// View
        //		_view_menu = _main_window->view_menu(); //
        //_main_window->menuBar()->addMenu(tr("&View"));
        //		_view_menu->clear();

        _viewmenubar = new QAction(this);
        // updateMenuBarActionText(true);
        _viewmenubar->setText(tr("Hide Menu bar"));
        _viewmenubar->setShortcut(tr("Shift+Ctrl+M"));
        connect(_viewmenubar, &QAction::triggered, this
            // &DockedWindow::slotViewMenuBar
            ,
            [this]() {
                if (_main_window->menuBar()->isVisible()) {
                    _viewmenubar->setText(
                        tr("Show Menu bar")); // updateMenuBarActionText(false);
                    // _main_window->menuBar()->hide();
                    _main_window->menuBar()->close();
                } else {
                    _viewmenubar->setText(
                        tr("Hide Menu bar")); // updateMenuBarActionText(true);
                    _main_window->menuBar()->show();
                }
                _autosaver->changeOccurred();
            });

        _view_menu->addAction(_viewmenubar);

        _viewbookmarkbar = new QAction(this);
        update_bookmarks_toolbar_action_text(true);
        //		_viewbookmarkbar->setShortcut(tr("Shift+Ctrl+B"));
        connect(_viewbookmarkbar, &QAction::triggered, this, &Browser::slotViewBookmarksBar);
        _view_menu->addAction(_viewbookmarkbar);

        _viewtoolbar = new QAction(this);
        updateToolbarActionText(true);
        _viewtoolbar->setShortcut(tr("Ctrl+|"));
        connect(_viewtoolbar, &QAction::triggered, this, &Browser::slotViewToolbar);
        _view_menu->addAction(_viewtoolbar);

        _viewstatusbar = new QAction(this);
        update_statusbar_action_text(true);
        _viewstatusbar->setShortcut(tr("Ctrl+/"));
        connect(_viewstatusbar, &QAction::triggered, this, &Browser::slotViewStatusbar);
        _view_menu->addAction(_viewstatusbar);

        _view_menu->addSeparator();

        // _stop = viewMenu->addAction(tr("&Stop"));
        QList<QKeySequence> shortcuts;
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
        shortcuts.append(Qt::Key_Escape);
        _stop->setShortcuts(shortcuts);
        _tab_widget->addWebAction(_stop, QWebEnginePage::Stop);

        // _reload = viewMenu->addAction(tr("Reload Page"));
        _reload->setShortcuts(QKeySequence::Refresh);
        _tab_widget->addWebAction(_reload, QWebEnginePage::Reload);

        _view_menu->addAction(tr("Zoom &In"), this, &Browser::slotViewZoomIn, QKeySequence(Qt::CTRL | Qt::Key_Plus));
        _view_menu->addAction(tr("Zoom &Out"), this, &Browser::slotViewZoomOut, QKeySequence(Qt::CTRL | Qt::Key_Minus));
        _view_menu->addAction(tr("Reset &Zoom"), this, &Browser::slotViewResetZoom, QKeySequence(Qt::CTRL | Qt::Key_0));

        _view_menu->addSeparator();
        _view_menu->addAction(tr("Page S&ource"), this, &Browser::slotViewPageSource, tr("Ctrl+Alt+U"));
        QAction* a = _view_menu->addAction(tr("&Full Screen"), this, &Browser::slotViewFullScreen, Qt::Key_F11);
        a->setCheckable(true);
    }

    void Browser::init_history_menu()
    {
        // History
        //		_history_menu = new
        //HistoryMenu(this);//_main_window->histry_menu();
        // new HistoryMenu(
        // _main_window   //this
        // );
        //		_history_menu->clear();

        connect(_history_menu, &HistoryMenu::openUrl, _tab_widget, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu*>(_history_menu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        _history_menu->setTitle(tr("Hi&story"));

        //		_main_window->menuBar()->addMenu(historyMenu);

        // _tree_screen->menus_in_button()->addMenu(historyMenu);// ?

        QList<QAction*> historyActions;

        {
            // _historyback = findscreen->historyback();
            //			_historyback->disconnect();
            _tab_widget->addWebAction(_historyback, QWebEnginePage::Back);
            _historyback->setIcon(QIcon(":/resource/pic/mobile_back.svg")); // style()->standardIcon(QStyle::SP_ArrowBack,
            // 0, _find_screen)
            // this

            _historybackmenu = new QMenu(this);

            connect(_historybackmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowBackMenu);
            connect(_historybackmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);
            _historyback->setMenu(_historybackmenu);
            // navigater->addAction(_historyback);
            //			_historyforward->disconnect();
            _tab_widget->addWebAction(_historyforward, QWebEnginePage::Forward);
            _historyforward->setIcon(QIcon(":/resource/pic/mobile_forward.svg")); // style()->standardIcon(QStyle::SP_ArrowForward,
                                                                                  // 0, _find_screen)
            // this

            _historyforwardmenu = new QMenu(this);
            connect(_historyforwardmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowForwardMenu);
            connect(_historyforwardmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);
            _historyforward->setMenu(_historyforwardmenu);
        }
        // _historyback = new QAction(tr("Back"), this);
        _tab_widget->addWebAction(_historyback, QWebEnginePage::Back);
        _historyback->setShortcuts(QKeySequence::Back);
        _historyback->setIconVisibleInMenu(false);
        historyActions.append(_historyback);

        // _historyforward = new QAction(tr("Forward"), this);
        _tab_widget->addWebAction(_historyforward, QWebEnginePage::Forward);
        _historyforward->setShortcuts(QKeySequence::Forward);
        _historyforward->setIconVisibleInMenu(false);
        historyActions.append(_historyforward);

        // QAction *_historyhome = new QAction(tr("Home"), this);

        connect(_historyhome, &QAction::triggered, // this, &Browser::slotHome
            //		auto to_home = _browser->_historyhome;
            //		to_home->disconnect();
            //		QObject::connect(
            //		    to_home, &QAction::triggered, this,
            [&](bool checked = true) -> void {
                Q_UNUSED(checked)
                auto v = _tab_widget->currentWebView();
                if (v) {
                    auto _page = v->page();
                    if (_page) {
                        auto binder_ = _page->binder();
                        if (binder_) {
                            boost::intrusive_ptr<i_t> _item = binder_->host();
                            assert(_item);
                            auto home = _item->field<home_key>();
                            QUrl homeurl = QUrl(detail::to_qstring(home));
                            if (homeurl.isValid() && homeurl != _page->url()) {
                                _item->field<url_key>(url_value(detail::to_qstring(home))); // "url",
                                _page->load(_item, true, true);
                            }
                        }
                    }
                }
            }
            //			});
            );

        _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

        historyActions.append(_historyhome);

#if defined(QWEBENGINEHISTORY_RESTORESESSION)
        _restorelastsession = new QAction(tr("Restore Last Session"), this);
        connect(_restorelastsession, &QAction::triggered, ::sapp_t::instance(), &::sapp_t::restoreLastSession);
        _restorelastsession->setEnabled(::sapp_t::instance()->canRestoreSession());
        historyActions.append(_tab_widget->recentlyClosedTabsAction());
        historyActions.append(_restorelastsession);
#endif

        _history_menu->setInitialActions(historyActions);
    }

    void Browser::init_bookmark_menu()
    {
        // Bookmarks
        //		_bookmarks_menu = _main_window->bookmark_menu();
        // new BookmarksMenu(
        // _main_window   //this
        // );
        //		_bookmarks_menu->clear();

        connect(_bookmarks_menu, &BookmarksMenu::openUrl, _tab_widget, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu*>(_bookmarks_menu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        _bookmarks_menu->setTitle(tr("&Bookmarks"));
        //		_main_window->menuBar()->addMenu(bookmarksMenu);

        QList<QAction*> bookmarksActions;

        QAction* showAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
        connect(showAllBookmarksAction, &QAction::triggered, this, &Browser::slotShowBookmarksDialog);
        _addbookmark = new QAction(QIcon(QLatin1String(":addbookmark.png")), tr("Add Bookmark..."), this);
        _addbookmark->setIconVisibleInMenu(false);

        connect(_addbookmark, &QAction::triggered, this, &Browser::slotAddBookmark);
        _addbookmark->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

        bookmarksActions.append(showAllBookmarksAction);
        bookmarksActions.append(_addbookmark);
        _bookmarks_menu->setInitialActions(bookmarksActions);
    }

    void Browser::init_window_menu()
    {
        //		// Window
        //		_windowmenu = _main_window->window_menu(); //
        //menuBar()->addMenu(tr("&Window"));
        //		_windowmenu->clear();
        connect(_windowmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowWindowMenu);
        slotAboutToShowWindowMenu();
    }

    void Browser::init_tools_menu()
    {
        _main_window->init_tools_menu();
//		_tools_menu = _main_window->tools_menu(); //
//menuBar()->addMenu(tr("&Tools"));
//		_tools_menu->addAction(tr("Web &Search"), this,
//&Browser::slotWebSearch, QKeySequence(tr("Ctrl+K", "Web Search")));
#if defined(QWEBENGINEINSPECTOR)
        a = _tools_menu->addAction(tr("Enable Web &Inspector"), this, &Browser::slotToggleInspector);
        a->setCheckable(true);
#endif
    }

    //// moved to _main_wondow
    //	void Browser::init_help_menu(){
    //		_main_window->init_help_menu();
    //		assert(_main_window->help_menu());
    //		_help_menu = _main_window->help_menu(); //
    //menuBar()->addMenu(tr("&Help"));
    //		// helpMenu->addAction(tr("About &Qt"), ::sapp_t::instance(), SLOT(aboutQt()));
    //		_help_menu->addAction(tr("About &Embedded Browser"), this,
    //&Browser::slotAboutApplication);
    //	}

    void Browser::synchronize_find_screen_reset_main_menu()
    {
        //		_main_window->menuBar()->clear();
        auto& _main_window_menu_map = _main_window->main_menu_map();
        auto reset_file_menu = [&]() {
            if (_main_window_menu_map[gl_para::_file_menu_name] != _file_menu)
                _main_window_menu_map[gl_para::_file_menu_name] = _file_menu;
        };
        auto reset_edit_menu = [&]() {
            if (_main_window_menu_map[gl_para::_edit_menu_name] != _edit_menu)
                _main_window_menu_map[gl_para::_edit_menu_name] = _edit_menu;
        };

        auto reset_tools_menu = [&]() {
            _main_window->init_tools_menu();
//						_tools_menu = _main_window->tools_menu();
//// menuBar()->addMenu(tr("&Tools"));
//						_tools_menu->addAction(tr("Web &Search"),
//this, &Browser::slotWebSearch, QKeySequence(tr("Ctrl+K", "Web Search")));
#if defined(QWEBENGINEINSPECTOR)
            a = _tools_menu->addAction(tr("Enable Web &Inspector"), this, &Browser::slotToggleInspector);
            a->setCheckable(true);
#endif
        };
        auto reset_view_menu = [&]() {
            if (_main_window_menu_map[gl_para::_view_menu_name] != _view_menu)
                _main_window_menu_map[gl_para::_view_menu_name] = _view_menu;
        };
        auto reset_history_menu = [&]() {
            if (_main_window_menu_map[gl_para::_history_menu_name] != _history_menu)
                _main_window_menu_map[gl_para::_history_menu_name] = _history_menu;
        };
        auto reset_bookmark_menu = [&]() {
            if (_main_window_menu_map[gl_para::_bookmark_menu_name] != _bookmarks_menu)
                _main_window_menu_map[gl_para::_bookmark_menu_name] = _bookmarks_menu;
        };
        auto reset_window_menu = [&]() {
            if (_main_window_menu_map[gl_para::_window_menu_name] != _windowmenu)
                _main_window_menu_map[gl_para::_window_menu_name] = _windowmenu;
        };
        //		auto reset_help_menu = [&](){
        //					       _main_window_menu_map["help_menu"] =
        //_help_menu;
        //				       };


        auto synchronize_find_screen_navigater = [&]() {
            assert(_find_screen);
            //		_find_screen->historyhome(_historyhome);
            //		_find_screen->historyforward(_historyforward);
            //		_find_screen->historyback(_historyback);
            //		_find_screen->stop(_stop);
            //		_find_screen->reload(_reload);
            //		_find_screen->stopreload(_stopreload);
            _find_screen->switch_navigater(_navigater);
            // auto mainwindow = _main_window;
            // FindScreen *_find_screen = globalparameters.find_screen();

            // QToolBar *navigater = findscreen->navigater();
            // assert(navigater);
            // navigater = addToolBar(tr("Navigation"));
            // //mainwindow->addToolBar(tr("Navigation"));
            // connect(navigater->toggleViewAction(), SIGNAL(toggled(bool)), this,
            // SLOT(updateToolbarActionText(bool)));

            // navigater->addAction(_historyforward);

            // _stopreload = new QAction(navigater); //this
            _stopicon = QIcon(":/resource/pic/mobile_stop.svg");     // style()->standardIcon(QStyle::SP_BrowserStop);
            _reloadicon = QIcon(":/resource/pic/mobile_reload.svg"); // style()->standardIcon(QStyle::SP_BrowserReload);
            _stopreload->setIcon(_reloadicon);

            ////navigater->addAction(_stopreload);

            ////navigater->addWidget(_tab_widget->lineEditStack());
            // _toolbarsearch = _find_screen->toolbarsearch();
            // assert(_toolbarsearch);
            assert(_tab_widget->lineEditStack());

            // if(_tab_widget->lineEditStack() == nullptr)
            // _tab_widget->lineEditStack(_find_screen->toolbarsearch()->lineedits());
            _find_screen->switch_stack(this->tab_widget()->lineEditStack());
            ////_toolbarsearch = new ToolbarSearch(_tab_widget->lineEditStack(),
            ///findscreen->findtable(), navigater);

            ////navigater->addWidget(_toolbarsearch);

            ////connect(_toolbarsearch, SIGNAL(search(QUrl)), SLOT(loadUrl(QUrl)));   //
            ///move to Entrance

            ////_chasewidget = new ChaseWidget(findscreen->size(), findscreen); //this
            ////navigater->addWidget(_chasewidget);
            _find_screen->browser(this);


        };

        //
        reset_file_menu();
        reset_tools_menu();
        reset_edit_menu();
        reset_view_menu();
        reset_history_menu();
        reset_bookmark_menu();
        reset_window_menu();
        //		reset_help_menu();

        auto button = _tree_screen->menus_in_button();
        button->clear();
        for (auto menu : _main_window_menu_map) button->addMenu(menu.second);
        _tree_screen->init_context_menu();
        //		_tree_screen->append_main_menu();
        if (menuBar() != _main_window->menuBar()) setMenuBar(_main_window->menuBar());

        synchronize_find_screen_navigater();
    }


    void Browser::slotShowBookmarksDialog()
    {
        BookmarksDialog* dialog = new BookmarksDialog(this);
        connect(dialog, &BookmarksDialog::openUrl, _tab_widget, &TabWidget::loadUrlInCurrentTab);
        dialog->show();
    }

    void Browser::slotAddBookmark()
    {
        WebView* webView = currentTab();
        QString url = webView->page()->url().toString();
        QString title = webView->title();
        AddBookmarkDialog dialog(url, title);
        dialog.exec();
    }

    void Browser::slotViewToolbar()
    {
        // FindScreen *_find_screen = globalparameters.find_screen();
        assert(_find_screen);
        // QToolBar *navigater = findscreen->navigater();
        // assert(navigater);
        if (_find_screen->isVisible()) {
            updateToolbarActionText(false);
            _find_screen->close();
        } else {
            updateToolbarActionText(true);
            _find_screen->show();
        }
        _autosaver->changeOccurred();
    }

    void Browser::slotViewBookmarksBar()
    {
        if (_bookmarkstoolbar->isVisible()) {
            update_bookmarks_toolbar_action_text(false);
            _bookmarkstoolbar->close();
            _configuration->setValue("show_bookmark_bar", false);
        } else {
            update_bookmarks_toolbar_action_text(true);
            _bookmarkstoolbar->show();
            _configuration->setValue("show_bookmark_bar", true);
        }
        _autosaver->changeOccurred();
    }

    void Browser::update_statusbar_action_text(bool visible)
    {
        _viewstatusbar->setText(!visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
    }

    void Browser::handle_find_text_result(bool found)
    {
        if (!found) slotUpdateStatusbar(tr("\"%1\" not found.").arg(_lastsearch));
    }

    void Browser::updateToolbarActionText(bool visible)
    {
        _viewtoolbar->setText(!visible ? tr("Show Find in base") : tr("Hide Find in base"));
    }

    void Browser::update_bookmarks_toolbar_action_text(bool visible)
    {
        _viewbookmarkbar->setText(!visible ? tr("Show Bookmarks bar") : tr("Hide Bookmarks bar"));
    }

    void Browser::slotViewStatusbar()
    {
        if (gl_paras->status_bar()->isVisible()) {
            update_statusbar_action_text(false);
            gl_paras->status_bar()->close();
        } else {
            update_statusbar_action_text(true);
            gl_paras->status_bar()->show();
        }
        _autosaver->changeOccurred();
    }

    // void Browser::loadUrl(const QUrl &url){
    // if(! currentTab() || ! url.isValid())return;
    // _tab_widget->currentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
    // _tab_widget->loadUrlInCurrentTab(url);
    // }

    void Browser::slotDownloadManager()
    {
        gl_paras->request_download_manager()->show();
    }

    void Browser::slotSelectLineEdit()
    {
        auto line = currentTab()->toolbarsearch()->lineEdit(); //_tab_widget->currentLineEdit();
        std::mutex m;
        if (m.try_lock()) {
            //            std::lock_guard<std::mutex> lock(m);
            if (line) {
                line->selectAll();
                line->setFocus();
            }
            m.unlock();
        }
    }

    void Browser::slotFileSaveAs()
    {
        // not implemented yet.
    }

    void Browser::slotPreferences()
    {
        SettingsDialog* s = new SettingsDialog(this);
        s->show();
    }

    void Browser::slotUpdateStatusbar(const QString& string)
    {
        gl_paras->status_bar()->showMessage(string, 2000);
    }

    //	// move to _main_window->synchronize_title
    //	void Browser::slotUpdateWindowTitle(const QString &title){
    //		auto item = this->_tab_widget->current_item();
    //		if(title.isEmpty())
    //_main_window->setWindowTitle(tr(QString(QString("%1 - ") +
    //program_title).toStdString().c_str())
    //				.arg(item ?
    //item->field<name_type>().toStdString().c_str() : ""));
    //		else{
    //#if defined(Q_OS_OSX)
    //			_main_window->setWindowTitle(title);
    //#else

    //			_main_window->setWindowTitle(tr(QString(QString("%1 - ") +
    //program_title).toStdString().c_str())
    //				.arg(title));
    //#endif
    //		}
    //	}

    void Browser::slotAboutApplication()
    {
        QMessageBox::about(
            this, tr("About"), tr("Version %1"
                                  "<p>This demo demonstrates the facilities "
                                  "of Qt WebEngine in action, providing an note with "
                                  "browser for you to experiment with.<p>"
                                  "<p>Qt WebEngine is based on the Chromium open source project "
                                  "developed at <a "
                                  "href=\"http://www.chromium.org/\">http://www.chromium.org/</a>.")
                                   .arg(QCoreApplication::applicationVersion()));
    }

    void Browser::slotFileNew()
    {
        // QtSingleApplication::instance()->
        // _browser->new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));

        // if(_entrance->window_list().count() == 0) { // ?
        // globalparameters.entrance()->new_browser(QUrl(web::Browser::_defaulthome));
        // }

        // web::Browser *browser = _entrance->activiated_registered().first;
        // //QtSingleApplication::instance()->mainWindow();
        // assert(browser == this); // ?
        // browser->slotHome();

        this->slotHome();
    }

    void Browser::slotFileOpen()
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(), tr("Web Resources (*.html *.htm *.svg *.png "
                                                                                                 "*.gif *.svgz);;All files (*.*)"));
        if (file.isEmpty()) return;
        // loadPage(file);
        tv_t* tree_view = _tree_screen->view();
        auto it = tree_view->session_root_auto();
        real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(url_value(file),
            [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                return TreeIndex::url_activate(real_target_url_,
                    [&] { return tree_view->source_model(); },
                    it,
                    std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                    [&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(detail::to_qstring(it_->field<home_key>()), file) || url_equal(detail::to_qstring(it_->field<url_key>()), file); },
                    browser_ref(this));
            });
    }

#define QT_NO_PRINTPREVIEWDIALOG

    void Browser::slotFilePrintPreview()
    {
#ifndef QT_NO_PRINTPREVIEWDIALOG
        if (!currentTab()) return;
        QPrintPreviewDialog* dialog = new QPrintPreviewDialog(this);
        connect(dialog, &QPrintPreviewDialog::paintRequested, currentTab(), &WebView::print);
        dialog->exec();
#endif
    }

    void Browser::slotFilePrint()
    {
#if defined(QWEBENGINEPAGE_PRINT)
        if (!currentTab()) return;
        printRequested(currentTab()->page()->mainFrame());
#endif
    }

#if defined(QWEBENGINEPAGE_PRINT)
    void BrowserWindow::printRequested(QWebEngineFrame* frame)
    {
#ifndef QT_NO_PRINTDIALOG
        QPrinter printer;
        QPrintDialog* dialog = new QPrintDialog(&printer, this);
        dialog->setWindowTitle(tr("Print Document"));
        if (dialog->exec() != QDialog::Accepted) return;
        frame->print(&printer);
#endif
    }

#endif

    void Browser::slotPrivateBrowsing()
    {
        if (!::sapp_t::instance()->privateBrowsing()) {
            QString title = tr("Are you sure you want to turn on private browsing?");
            QString text = tr("<b>%1</b><br><br>"
                              "This action will reload all open tabs.<br>"
                              "When private browsing in turned on,"
                              " webpages are not added to the history,"
                              " items are automatically removed from the Downloads window,"
                              " new cookies are not stored, current cookies can't be accessed,"
                              " site icons wont be stored, session wont be saved, "
                              " and searches are not added to the pop-up menu in the Google "
                              "search box."
                              "  Until you close the window, you can still click the Back and "
                              "Forward buttons"
                              " to return to the webpages you have opened.")
                               .arg(title);

            QMessageBox::StandardButton button = QMessageBox::question(
                this, QString(), text, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            if (button == QMessageBox::Ok) ::sapp_t::instance()->setPrivateBrowsing(true);
        } else {
            // TODO: Also ask here
            ::sapp_t::instance()->setPrivateBrowsing(false);
        }
    }

    void Browser::resizeEvent(QResizeEvent* e)
    {
        _tab_widget->resizeEvent(e);
    }

    void Browser::focusOutEvent(QFocusEvent* event)
    {
        (void)event;
        //
    }

    void Browser::showEvent(QShowEvent* event)
    {
        _destroy_request_sent = false;
        QMainWindow::showEvent(event);
    }

    void Browser::closeEvent(QCloseEvent* event)
    {
#ifdef CONFIRM_MULTI_CLOSE
        if (_tab_widget) {
            //			int count_ = _tab_widget->count();
            if (_tab_widget->count() > 1) {
                int ret = QMessageBox::warning(
                    this, QString(), tr("Are you sure you want to close the window?  There are %1 tabs open").arg(_tab_widget->count()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (ret == QMessageBox::No) {
                    event->ignore();
                    return;
                }
            }
            //			//			for (int i = 0; i < _tab_widget->count(); i++) {
            //			//				count_ = _tab_widget->count(); //for debug
            //			//				_tab_widget->closeTab(i);      //webView(i)->close();
            //			//			}
            //			while (_tab_widget->count() > 0) {
            //				_tab_widget->closeTab(_tab_widget->count() - 1);
            //			}
        }
#endif // CONFIRM_MULTI_CLOSE
        event->accept();
        deleteLater();
    }

    void Browser::slotEditFind()
    {
        if (!currentTab()) return;
        bool ok;
        QString search = QInputDialog::getText(this, tr("Find"), tr("Text:"), QLineEdit::Normal, _lastsearch, &ok);
        if (ok && !search.isEmpty()) {
            _lastsearch = search;
            currentTab()->findText(_lastsearch, 0, invoke(this, &Browser::handle_find_text_result));
        }
    }

    void Browser::slotEditFindNext()
    {
        if (!currentTab() && !_lastsearch.isEmpty()) return;
        currentTab()->findText(_lastsearch);
    }

    void Browser::slotEditFindPrevious()
    {
        if (!currentTab() && !_lastsearch.isEmpty()) return;
        currentTab()->findText(_lastsearch, QWebEnginePage::FindBackward);
    }

    void Browser::slotViewZoomIn()
    {
        if (!currentTab()) return;
        currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    }

    void Browser::slotViewZoomOut()
    {
        if (!currentTab()) return;
        currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    }

    void Browser::slotViewResetZoom()
    {
        if (!currentTab()) return;
        currentTab()->setZoomFactor(1.0);
    }

    void Browser::slotViewFullScreen(bool makeFullScreen)
    {
        if (makeFullScreen)
            showFullScreen();
        else {
            if (isMinimized())
                showMinimized();
            else if (isMaximized())
                showMaximized();
            else
                showNormal();
        }
    }

    void Browser::slotViewPageSource()
    {
        if (!currentTab()) return;
        QPlainTextEdit* view = new QPlainTextEdit;
        view->setWindowTitle(tr("Page Source of %1").arg(currentTab()->title()));
        view->setMinimumWidth(640);
        view->setAttribute(Qt::WA_DeleteOnClose);
        view->show();

        currentTab()->page()->toHtml(invoke(view, &QPlainTextEdit::setPlainText));
    }

    // deprecated by record::preoperty::home
    void Browser::slotHome()
    {
        // QSettings _configuration(gl_paras->root_path() + "/" +
        // gl_paras->target_os() + "/" + gl_paras->_browser_conf_filename,
        // QSettings::IniFormat);
        _configuration->beginGroup(QLatin1String("MainWindow"));
        QString home = _configuration->value(QLatin1String("home"), QLatin1String(detail::to_string(Browser::_defaulthome).c_str())).toString();
        auto tree_view = _tree_screen->view();
        // boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

        auto current_item = tree_view->current_item();
        auto parent = current_item->parent();
        if (!parent) throw std::runtime_error(formatter() << "! parent");
        real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(url_value(home),
            [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                return TreeIndex::url_activate(real_target_url_,
                    [&] { return tree_view->source_model(); },
                    current_item,
                    std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                    [&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(detail::to_qstring(it_->field<home_key>()), home) || url_equal(detail::to_qstring(it_->field<url_key>()), home); },
                    browser_ref(this));
            });
        _configuration->endGroup();
    }

    //    void Browser::slotWebSearch()
    //    {
    //        // _toolbarsearch
    //        //        static_cast<ToolbarSearch*>(_find_screen->lineedit_stack()->currentWidget())
    //        auto view = currentTab();
    //        if (view) {
    //            auto toolbarsearch = view->toolbarsearch();
    //            if (toolbarsearch) {
    //                auto line = toolbarsearch->lineEdit();

    //                if (line) {
    //                    line->selectAll();
    //                    // _toolbarsearch
    //                    //            _find_screen->lineedit_stack()->currentWidget()
    //                    line->setFocus();
    //                }
    //            }
    //        }
    //    }

    void Browser::slotToggleInspector(bool enable)
    {
#if defined(QWEBENGINEINSPECTOR)
        QWebEngineSettings::globalSettings()->setAttribute(
            QWebEngineSettings::DeveloperExtrasEnabled, enable);
        if (enable) {
            int result = QMessageBox::question(this, tr("Web Inspector"), tr("The web inspector will only work correctly "
                                                                             "for pages that were loaded after enabling.\n"
                                                                             "Do you want to reload all pages?"),
                QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::Yes) _tabwidget->reloadAllTabs();
        }
#else
        Q_UNUSED(enable);
#endif
    }

    void Browser::slotSwapFocus()
    {
        if (currentTab()->hasFocus()) {
            std::mutex m;
            if (m.try_lock()) {
                //                std::lock_guard<std::mutex> lock(m);
                auto line = currentTab()->toolbarsearch()->lineEdit(); //_tab_widget->currentLineEdit();
                if (line) line->setFocus();
                m.unlock();
            }
        } else
            currentTab()->setFocus();
    }

    // void Browser::loadPage(const QString &page){
    // QUrl url = QUrl::fromUserInput(page);
    // loadUrl(url);
    // }

    // BrowserTabManager *BrowserWindow::tabWidget() const
    // {
    // return _tabwidget;
    // }

    WebView* Browser::currentTab() const
    {
        return _tab_widget->currentWebView();
    }

    void Browser::slotLoadProgress(int progress)
    {

        auto _chasewidget =
#ifdef USE_CLEAR_BUTTON
            _find_screen->chasewidget();
#else
            [&] {
                ChaseWidget* chasewidget_ = nullptr;
                auto view = currentTab();
                if (view) {
                    auto s = view->toolbarsearch();
                    if (s) {
                        chasewidget_ = s->chasewidget();
                    }
                }
                return chasewidget_;
            }();
#endif // USE_CLEAR_BUTTON

        if (_chasewidget) {
            //			std::mutex m;
            //			std::lock_guard<std::mutex> lock(m);
            if (progress < 100 && progress > 0) {
                _chasewidget->setAnimated(true);
                disconnect(_stopreload, &QAction::triggered, _reload, &QAction::trigger);
                if (_stopicon.isNull()) _stopicon = QIcon(":/resource/pic/mobile_stop.svg"); // style()->standardIcon(QStyle::SP_BrowserStop);
                                                                                             //				try {
                _stopreload->setIcon(_stopicon);                                             // crashs? -- recursive calling select_as_current
                                                                                             //				} catch (std::exception& e) {
                                                                                             //					(void)e;
                                                                                             //				}

                connect(_stopreload, &QAction::triggered, _stop, &QAction::trigger);
                _stopreload->setToolTip(tr("Stop loading the current page"));
            } else {
                _chasewidget->setAnimated(false);
                disconnect(_stopreload, &QAction::triggered, _stop, &QAction::trigger); // &QAction::setChecked
                if (_reloadicon.isNull()) _reloadicon = QIcon(":/resource/pic/mobile_reload.svg");
                //				try {
                _stopreload->setIcon(_reloadicon); // crashs? -- recursive calling select_as_current
                                                   //				} catch (std::exception& e) {
                                                   //					(void)e;
                                                   //				}

                connect(_stopreload, &QAction::triggered, _reload, &QAction::trigger); // &QAction::setChecked
                _stopreload->setToolTip(tr("Reload the current page"));
            }
        }
    }

    void Browser::slotAboutToShowBackMenu()
    {
        _historybackmenu->clear();
        if (!currentTab()) return;
        QWebEngineHistory* history = currentTab()->history();
        int historyCount = history->count();
        for (int i = history->backItems(historyCount).count() - 1; i >= 0; --i) {
            QWebEngineHistoryItem item = history->backItems(history->count()).at(i);
            QAction* action = new QAction(this);
            action->setData(-1 * (historyCount - i - 1));
            QIcon icon = ::sapp_t::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historybackmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowForwardMenu()
    {
        _historyforwardmenu->clear();
        if (!currentTab()) return;
        QWebEngineHistory* history = currentTab()->history();
        int historyCount = history->count();
        for (int i = 0; i < history->forwardItems(history->count()).count(); ++i) {
            QWebEngineHistoryItem item = history->forwardItems(historyCount).at(i);
            QAction* action = new QAction(this);
            action->setData(historyCount - i);
            QIcon icon = ::sapp_t::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historyforwardmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowWindowMenu()
    {
        _windowmenu->clear();
        _windowmenu->addAction(_tab_widget->nextTabAction());
        _windowmenu->addAction(_tab_widget->previousTabAction());
        _windowmenu->addSeparator();
        _windowmenu->addAction(tr("Downloads"), this, &Browser::slotDownloadManager, QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));
        _windowmenu->addSeparator();

        //		auto _browsers = [&] {
        //			set<sd::intrusive_ptr<web::Browser>> bs;
        //			for (auto rs : _main_window->record_screens()) bs.insert(rs->browser());
        //			return bs;
        //		}(); // QtSingleApplication::instance()->mainWindows();
        size_t index = 0;
        for (auto& _browser : gl_paras->main_window()->browsers()) { // for(size_t i = 0; i < _browsers.size(); i++) {
            // auto window = it;   // _browsers[i];
            qDebug() << "_browser->windowTitle()=" << _browser->windowTitle();
            auto topic = _browser->blogger()->topic();
            assert(topic != "");
            if (_browser->windowTitle() != topic) {
                _browser->setWindowTitle(topic); //.leftJustified(8, '.', true)
            }
            QAction* action = _windowmenu->addAction(_browser->windowTitle(), this, &Browser::slotShowWindow);
            action->setData(static_cast<uint>(index));
            action->setCheckable(true);
            if (_browser == this) action->setChecked(true);
            index++;
        }
    }

    void Browser::slotShowWindow()
    {
        if (QAction* action = qobject_cast<QAction*>(sender())) {
            QVariant v = action->data();
            if (v.canConvert<uint>()) {
                uint offset = qvariant_cast<uint>(v);
                auto _browsers = gl_paras->main_window()->browsers();
                //				    [&] {
                //					    set<sd::intrusive_ptr<web::Browser>> bs;
                //					    for (auto rs : _main_window->record_screens()) bs.insert(rs->browser());
                //					    return bs;
                //				    }(); // QtSingleApplication::instance()->mainWindows();
                auto _browser = [&] {
                    web::Browser* r = nullptr;
                    size_t index = 0;
                    for (auto& it : _browsers) {
                        if (index == offset) {
                            r = it;
                            break;
                        }
                        index++;
                    }
                    return r;
                }(); // _browsers[offset];
                if (_browser) {
                    _browser->activateWindow();
                    auto v = _browser->currentTab();
                    if (v) v->setFocus();
                }
            }
        }
    }

    void Browser::slotOpenActionUrl(QAction* action)
    {
        int offset = action->data().toInt();
        QWebEngineHistory* history = currentTab()->history();
        if (offset < 0)
            history->goToItem(history->backItems(-1 * offset).first()); // back
        else if (offset > 0)
            history->goToItem(
                history->forwardItems(history->count() - offset + 1).back()); // forward
    }

    void Browser::geometry_change_requested(const QRect& geometry)
    {
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

    // QStatusBar *Browser::status_bar(){return gl_paras->status_bar();}

    // QStatusBar *Browser::status_bar() const {return gl_paras->status_bar();}

    boost::intrusive_ptr<i_t> Browser::bind(boost::intrusive_ptr<RecordIndex> record_index, bool make_current)
    {
        // boost::intrusive_ptr<TreeItem> result(nullptr);
        // boost::intrusive_ptr<TreeItem> tab_brother =
        // record_index->target_sibling();
        boost::intrusive_ptr<i_t> result(nullptr);
        boost::intrusive_ptr<i_t> target = record_index->host();
        if (target->is_lite()) target->to_fat();
        // clean();
        // assert(_it->is_registered_to_browser() || _it->field("url") ==
        // web::Browser::_defaulthome);
        // assert(tab_brother != target);

        WebView* view = nullptr;
        // TabWidget *const _tab_widget = _tab_widget;
        // if(_mainWindows.isEmpty()) {
        // dp = new_dockedwindow(record);
        // } else
        // {
        // for(auto &i : _mainWindows) {
        auto url_euql = [&](boost::intrusive_ptr<const ::Binder> b) {
            return url_equal(detail::to_qstring(b->host()->field<home_key>()), detail::to_qstring(target->field<home_key>())) && b->host()->id() == target->id() && b == target->binder();
        };
        view = _tab_widget->find(url_euql); // if _item->field("url") ==
        // web::Browser::_defaulthome , force
        // rebind
        // if(view != nullptr) {
        ////            dp.first = i.data();
        // if(!isVisible()) {
        // raise();
        // activateWindow();
        // }
        ////            break;
        // } else
        if (!view) {
            //			{
            auto view_found =
                gl_paras->main_window()->find(url_euql);
            if (view_found) {
                auto new_tab = view_found->tabmanager();
                if (new_tab) {
                    if (new_tab != _tab_widget) {
                        //						view_found->close();
                        new_tab->closeTab(new_tab->indexOf(view_found));
                    }
                }
            }
            //			}
            view = _tab_widget->find(
                [&](boost::intrusive_ptr<const ::Binder> b) {
                    return url_equal(detail::to_qstring(b->host()->field<home_key>()), detail::to_qstring(Browser::_defaulthome)) || url_equal(detail::to_qstring(b->host()->field<url_key>()), detail::to_qstring(Browser::_defaulthome));
                });
            //// Record *blank_url =
            ///check_register_record(QUrl(DockedWindow::_defaulthome));

            ////            if(blank.isLite())blank.switchToFat();
            ////            blank.setNaturalFieldSource("url",
            ///DockedWindow::_defaulthome);

            // WebView *blankview = tab->find([&](boost::intrusive_ptr<const TreeItem>
            // it) {return it->field("url") == web::Browser::_defaulthome;}); //
            // QUrl(web::Browser::_defaulthome)

            ////            //PageView *no_pin = nullptr;

            ////            WebView *nopin_view = tab->find_nopin();

            ////            // assert(dp.first);

            // if(blankview != nullptr) {
            // view = blankview;
            ////                boost::intrusive_ptr<TreeIndex> tree_index;

            ////                try {
            ////                    tree_index = new TreeIndex([&] {return
            ///globalparameters.tree_screen()->tree_view()->source_model();},
            ///_item->parent(),
            ///_item->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il)
            ///{return il->host() == _item && _item->linker() == il && _item->parent()
            ///== il->host_parent();}));
            ////                } catch(std::exception &e) {throw e;}
            // result = view->page()->item_bind(_item);
            // result->activate();//                view->page()->load(record);
            // }
            ////            else if(nopin_view != nullptr) {   // no_pin
            ////                view = nopin_view;

            ////                if(view->page()->url().toString() != _it->field("url"))
            ///{
            ////                    result = view->page()->item_bind(_it);
            ////                    result->activate(); // view->page()->load(record);
            ////                }
            ////            }
            // else {
            if (!view) {
                view = _tab_widget->newTab(record_index, make_current); // , false
                auto page = view->page();
                if (page) {
                    result = page->binder()->host();
                    if (result != target) result = page->bind(target);
                }
                //// auto load
                // }

                ////            tab->setCurrentWidget(dp.second);   //
                ///tab->setCurrentIndex(tab->webViewIndex(dp.second));
                ////            dp.second->show();
                assert(view);
                assert(target == result);
                assert(target->binder()->integrity_external(target, view->page()));
            } else {
                auto page = view->page();
                if (page) {
                    result = page->bind(target);
                }
                assert(view);
                assert(target == result);
                assert(target->binder()->integrity_external(target, view->page()));
            }
        } else {
            auto page = view->page();
            if (page) {
                result = page->binder()->host();
                if (result != target) result = page->bind(target);
            }
            assert(view);
            assert(target == result);
            assert(target->binder()->integrity_external(target, view->page()));
        }
        // tab->setCurrentWidget(dp.second);
        // dp.second->show();
        // assert(dp.first);
        if (_browser_docker->widget() != this) _browser_docker->setWidget(this);
        auto _vtab_record = _main_window->vtab_record();
        auto _record_screen = _tab_widget->record_screen();
        if (_vtab_record->currentWidget() != _record_screen && make_current)
            _vtab_record->setCurrentWidget(_record_screen);
        assert(view);
        assert(target == result);
        assert(target->binder()->integrity_external(target, view->page()));

        return target; // _mainWindows[0];
    }
    //#ifdef USE_SIGNAL_CLOSE
    //	sd::intrusive_ptr<rs_t>
    //#else
    rs_t*
    //#endif //USE_SIGNAL_CLOSE
    Browser::record_screen()
    {
        //		auto _record_screen = _tab_widget->record_screen();
        return _tab_widget->record_screen();
    }

    //	Docker *Browser::browser_docker(){return _browser_docker;}
    void Browser::init_configuration(std::unique_ptr<QSettings>&& conf)
    {
        _configuration = std::move(conf);
    }
    void Browser::configuration_full_name(const QString& conf_name)
    {
        _configuration_full_name = conf_name;
    }
    Blogger* Browser::blogger()
    {
        return _blogger;
    }
    //	void Browser::on_blogger_close_requested()
    //	{
    //		//		if (_blogger) _blogger->deleteLater();
    //		_blogger = nullptr;
    //		//		self_close_request();
    //		close();
    //		//		auto _record_screen = _tab_widget->record_screen();
    //		//		if (_record_screen)
    //		//			_record_screen->on_blogger_close_requested();
    //	}

    //	void Browser::on_record_screen_close_requested()
    //	{
    //		//		if (_record_screen) _record_screen->deleteLater();
    //		//		_record_screen = nullptr;
    //		//		self_close_request();
    //		close();
    //	}


    void Browser::on_topic_changed(const QString& original_topic_, const QString& topic_, bool append_mode)
    {
        QString new_topic = topic_;
        QString original_topic_folder = gl_paras->editors_shared_full_path_name() + "/" + original_topic_;
        QString dest_topic_folder = gl_paras->editors_shared_full_path_name() + "/" + new_topic;

        _tab_widget->topic_replace_traverse(new_topic, original_topic_);
        //                if (_browser) {
        if (!append_mode) {
            auto original_config_name = original_topic_folder + "/" + gl_para::_browser_conf_filename;
            auto _current_topic_config_name = dest_topic_folder + "/" + gl_para::_browser_conf_filename;
            this->configuration_full_name(_current_topic_config_name);
            this->init_configuration( // std::make_unique<QSettings>(_current_topic_folder_name + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat)
                [&]() -> std::unique_ptr<QSettings> {
                    if (!QDir(dest_topic_folder).exists())
                        if (!QDir::root().mkpath(dest_topic_folder)) critical_error("Can not create directory: \"" + dest_topic_folder + "\"");
                    if (!QFile(_current_topic_config_name).exists()) {
                        if (QFile(original_config_name).exists()) {
                            //
                            if (!QFile::copy(original_config_name, _current_topic_config_name)) critical_error(QString("Can not copy \"") + original_config_name + "\"");
                        } else {
                            //
                            if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + ::gl_para::_editor_conf_filename, _current_topic_config_name)) critical_error(QString("Can not copy default \"") + ::gl_para::_editor_conf_filename + "\""); // throw std::runtime_error("Can
                                                                                                                                                                                                                                                                                // not copy document.ini");
                        }
                    }
                    if ((QFile::ReadUser | QFile::WriteUser) !=
                        (QFile::permissions(_current_topic_config_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(_current_topic_config_name, QFile::ReadUser | QFile::WriteUser);
                    return std::make_unique<QSettings>(_current_topic_config_name, QSettings::IniFormat);
                }());
        } else
            this->restore_state(new_topic); //lineedit_topic_->setText(original_topic_name);
        //                }
        auto _record_screen = _tab_widget->record_screen();
        _record_screen->on_topic_changed(original_topic_, new_topic, append_mode);
        if (this->windowTitle() == new_topic) this->setWindowTitle(new_topic); //.leftJustified(8, '.', true)
        this->save();
    }
    //	void Browser::test()
    //	{
    //		QDialog dlg;
    //		dlg.exec();
    //	}
    web::WebView* Browser::find(const std::function<bool(boost::intrusive_ptr<const ::Binder>)>& _equal) const
    {
        return _tab_widget->find(_equal);
    }

    QToolBar* Browser::navigater() const { return _navigater; }
}

//
