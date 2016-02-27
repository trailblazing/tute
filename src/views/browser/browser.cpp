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
#include "utility/delegate.h"
#include "browser.h"

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

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPlainTextEdit>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QInputDialog>

#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QDockWidget>
#include <QtCore/QDebug>


#include "main.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "models/record_table/Record.h"
#include "views/record_table/RecordView.h"
#include "libraries/GlobalParameters.h"
#include "views/browser/entrance.h"
#include "views/record_table/RecordScreen.h"
#include "controllers/record_table/RecordController.h"
#include "views/browser/tabwidget.h"
#include "views/main_window/MainWindow.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/tree/TreeScreen.h"

extern GlobalParameters globalparameters;
//Record *default_record = nullptr;


namespace browser {

    template<typename Arg, typename R, typename C>
    struct InvokeWrapper {
        R *receiver;
        void (C::*memberFun)(Arg);
        void operator()(Arg result)
        {
            (receiver->*memberFun)(result);
        }
    };

    template<typename Arg, typename R, typename C>
    InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
    {
        InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
        return wrapper;
    }

    // const char *DockedWindow::_defaulthome = "about:blank";

    Browser::~Browser()
    {

        _autosaver->changeOccurred();
        _autosaver->saveIfNeccessary();
        delete _autosaver;
        delete _bookmarkstoolbar;
        //        delete _tabmanager;

        if(globalparameters.vtab()->indexOf(_record_screen) != -1)
            globalparameters.vtab()->removeTab(globalparameters.vtab()->indexOf(_record_screen));

        delete _record_screen;
        delete _layout;
        delete _centralwidget;

    }

    void Browser::init()    // QUrl const &url,
    {
        //        browser->setWidget(this);
        //        this->setParent(browser);
        setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
        //        setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        setWebAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        setToolButtonStyle(Qt::ToolButtonFollowStyle);
        setAttribute(Qt::WA_DeleteOnClose, true);
        //statusBar()
        status_bar()->setSizeGripEnabled(true);
        append_to_main_menu();
        setup_tool_bar();

        //        QWidget *_centralwidget = new QWidget(this);
        //        BookmarksModel *bookmarksModel = QtSingleApplication::bookmarksManager()->bookmarksModel();
        //        _bookmarkstoolbar = new BookmarksToolBar(bookmarksModel, this);
        connect(_bookmarkstoolbar, &BookmarksToolBar::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(_bookmarkstoolbar->toggleViewAction(), &QAction::toggled, this, &Browser::update_bookmarks_toolbar_action_text);

        //        QVBoxLayout *_layout = new QVBoxLayout;
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
        connect(_tabmanager, &TabWidget::linkHovered, status_bar(), [this](const QString & link) {
            //            &QStatusBar::showMessage
            this->status_bar()->showMessage(link);
        }
               );
        connect(_tabmanager, &TabWidget::loadProgress, this, &Browser::slotLoadProgress);
        connect(_tabmanager, &TabWidget::tabsChanged, _autosaver, &AutoSaver::changeOccurred);
        connect(_tabmanager, &TabWidget::geometryChangeRequested, this, &Browser::geometry_change_requested);
#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(_tabmanager, &TabWidget::printRequested, this, &Browser::printRequested);
#endif
        connect(_tabmanager, &TabWidget::menuBarVisibilityChangeRequested, menuBar(), &QMenuBar::setVisible);
        connect(_tabmanager, &TabWidget::statusBarVisibilityChangeRequested, status_bar(), &QStatusBar::setVisible);
        //        connect(_tabmanager, SIGNAL(toolBarVisibilityChangeRequested(bool)), navigater, SLOT(setVisible(bool)));
        connect(_tabmanager, &TabWidget::toolBarVisibilityChangeRequested, _bookmarkstoolbar, &BookmarksToolBar::setVisible);
#if defined(Q_OS_OSX)
        connect(_tabmanager, &TabWidget::lastTabClosed, this, &Browser::close);
#else
        connect(
            _tabmanager
            , &TabWidget::lastTabClosed
            , this
        , [&]() ->void {
            //            _tabmanager->newTab(request_record(QUrl(DockedWindow::_defaulthome)));
            //            WebView *view = nullptr;
            //            sd::method(
            //                ""  //std::string _method_name
            //                , &TabWidget::newTab    //return_type(object_type::*f)(Arg...)          //  f can be std::function<> template?
            //                , view  //return_type *r            // = (return_type *)0
            //                , _tabmanager   //typename STATIC_IF_SHARED<object_type, object_pointer_type>::type o   //shared_ptr<object_type> o // = (object_type *const)0  //nullptr
            //                , Arg &&... arg
            //            );


            //            auto arint = boost::make_shared<TabWidget::ActiveRecordBinder>(_tabmanager, true);
            //            auto record = _record_controller->request_item(
            //                QUrl(Browser::_defaulthome)
            //                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
            //                    ""
            //                    , &TabWidget::ActiveRecordBinder::binder
            //                    , arint
            //                )
            //                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
            //                    ""
            //                    , &TabWidget::ActiveRecordBinder::activator
            //                    , arint
            //                )
            //            );
            //            record->activate();

            this->close();
            int index = globalparameters.vtab()->indexOf(_record_screen);

            if(index != -1)
                globalparameters.vtab()->removeTab(index);
            //            static_cast<Browser *>(globalparameters.vtab()->currentWidget())->append_to_main_menu();
        }
        );
#endif

        slotUpdateWindowTitle();
        load_default_state();


        if(globalparameters.vtab()->indexOf(_record_screen) == -1)
            globalparameters.vtab()->addTab(_record_screen, "Browser " + globalparameters.vtab()->count());

        //        int size = _tabmanager->lineEditStack()->sizeHint().height();

        //        navigater->setIconSize(QSize(size, size));

        //    initUrl();
        //dock_widget->setWidget(this);
        //setParent(dock_widget);

        //        connect(globalparameters.mainwindow(), &MainWindow::setEnabled, this, &DockedWindow::setEnabled);
        //        connect(globalparameters.mainwindow(), &MainWindow::blockSignals, this, &DockedWindow::blockSignals);
    }

    void Browser::run_script(const QString &style_source)
    {
        QMetaObject::invokeMethod(this, "runScriptOnOpenViews", Qt::QueuedConnection, Q_ARG(QString, style_source));
    }

    boost::intrusive_ptr<TreeItem> Browser::equip_registered(boost::intrusive_ptr<TreeItem> record)
    {
        auto binder = [](boost::shared_ptr<TabWidget::ActiveRecordBinder> ar) {
            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>> (
                       ""
                       , &TabWidget::ActiveRecordBinder::binder
                       , ar
                   );
        };
        auto activator = [](boost::shared_ptr<TabWidget::ActiveRecordBinder> ar) {
            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>> (
                       ""
                       , &TabWidget::ActiveRecordBinder::activator
                       , ar
                   );
        };

        // registered record, but have no generator:
        boost::shared_ptr<TabWidget::ActiveRecordBinder> ar = boost::make_shared<TabWidget::ActiveRecordBinder>(_tabmanager);
        record->binder(
            binder(ar)
        );

        record->activator(
            activator(ar)
        );

        //        _tabmanager->newTab(record);
        //        assert(record->binded_only_page());
        return record;
    }

    boost::intrusive_ptr<TreeItem> Browser::register_url(QUrl const &url)
    {

        //        connect(this, &DockedWindow::activateWindow, _entrance, &Entrance::on_activate_window);

        //Record *_backup = record;
        //PageView *new_view =

        //        _tabmanager->newTab(url);  // , false

        //        auto arint = boost::make_shared<TabWidget::ActiveRecordBinder>(_tabmanager, true);
        boost::intrusive_ptr<TreeItem> record
            = _tabmanager->request_item(    // why do this?
                  url
                  //                  , std::make_shared <
                  //                  sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *) >> (
                  //                      ""
                  //                      , &TabWidget::ActiveRecordBinder::binder
                  //                      , arint
                  //                  )
                  //                  , std::make_shared <
                  //                  sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> >> (
                  //                      ""
                  //                      , &TabWidget::ActiveRecordBinder::activator
                  //                      , arint
                  //                  )
              );
        //        record->activate();
        //assert(new_view->page());
        //record->page(new_view->page());
        //assert(_backup->page());
        assert(record); // && record->page_valid() && record->unique_page());
        return record;
    }

    Browser::Browser(const QByteArray &state
                     , TreeScreen   *_tree_screen
                     , FindScreen   *_find_screen
                     , MetaEditor   *_editor_screen
                     , HidableTabWidget *_vtabwidget, MainWindow   *_main_window
                     , Entrance     *_entrance
                     , const QString &style_source
                     , Qt::WindowFlags flags
                    )
        : QMainWindow(0, flags)
        , _record_screen(new RecordScreen(_tree_screen
                                          , _find_screen
                                          , _editor_screen
                                          , this
                                          , _vtabwidget
                                          , _main_window))
        , _tabmanager(_record_screen->tabmanager())
        , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
        , _chasewidget(globalparameters.find_screen()->chasewidget())
        , _autosaver(new AutoSaver(this))
        , _historyhome(globalparameters.find_screen()->historyhome())
        , _historyback(globalparameters.find_screen()->historyback())
        , _historyforward(globalparameters.find_screen()->historyforward())
        , _stop(0)
        , _reload(0)
        , _stopreload(globalparameters.find_screen()->stopreload())
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
        , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    {
        init();
        auto r = register_url(QUrl(Browser::_defaulthome));
        r->activate();
        run_script(style_source);
        this->restore_state(state);

        QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();
    }

    Browser::Browser(const QUrl &url
                     , TreeScreen   *_tree_screen
                     , FindScreen   *_find_screen
                     , MetaEditor   *_editor_screen
                     , HidableTabWidget *_vtabwidget, MainWindow   *_main_window
                     , Entrance     *_entrance
                     , const QString &style_source
                     , Qt::WindowFlags flags
                    )
        : QMainWindow(0, flags)
        , _record_screen(new RecordScreen(_tree_screen
                                          , _find_screen
                                          , _editor_screen
                                          , this
                                          , _vtabwidget
                                          , _main_window))
        , _tabmanager(_record_screen->tabmanager())
        , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
        , _chasewidget(globalparameters.find_screen()->chasewidget())
        , _autosaver(new AutoSaver(this))
        , _historyhome(globalparameters.find_screen()->historyhome())
        , _historyback(globalparameters.find_screen()->historyback())
        , _historyforward(globalparameters.find_screen()->historyforward())
        , _stop(0)
        , _reload(0)
        , _stopreload(globalparameters.find_screen()->stopreload())
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
        , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    {
        init();
        auto r = register_url(url);
        r->activate();
        run_script(style_source);       //        assert(record->linkpage());

        QMainWindow::setWindowFlags(Qt::FramelessWindowHint);    //Qt::Window |

        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();
    }


    Browser::Browser(boost::intrusive_ptr<TreeItem> item
                     , TreeScreen   *_tree_screen
                     , FindScreen   *_find_screen
                     , MetaEditor   *_editor_screen
                     , HidableTabWidget *_vtabwidget, MainWindow   *_main_window
                     , Entrance     *_entrance
                     , const QString &style_source
                     , Qt::WindowFlags flags
                    )
        : QMainWindow(0, flags)
        , _record_screen(new RecordScreen(_tree_screen
                                          , _find_screen
                                          , _editor_screen
                                          , this
                                          , _vtabwidget
                                          , _main_window))
        , _tabmanager(_record_screen->tabmanager())
        , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
        , _chasewidget(globalparameters.find_screen()->chasewidget())
        , _autosaver(new AutoSaver(this))
        , _historyhome(globalparameters.find_screen()->historyhome())
        , _historyback(globalparameters.find_screen()->historyback())
        , _historyforward(globalparameters.find_screen()->historyforward())
        , _stop(0)
        , _reload(0)
        , _stopreload(globalparameters.find_screen()->stopreload())
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
        , _entrance(_entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
    {
        assert(item);

        init();

        if(!item->is_registered_to_record_controller() || !_tabmanager->source_model()->find(item)) {
            //            auto arint = boost::make_shared<TabWidget::ActiveRecordBinder>(_tabmanager);    // this->tabWidget() does not work, because initialization has not accomplished
            _tabmanager->request_item(
                item
                //                , std::make_shared <
                //                sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *) >> (
                //                    ""
                //                    , &TabWidget::ActiveRecordBinder::binder
                //                    , arint
                //                )
                //                , std::make_shared <
                //                sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> >> (
                //                    ""
                //                    , &TabWidget::ActiveRecordBinder::activator
                //                    , arint
                //                )
            );
        }

        if(!item->binder() || !item->activator())equip_registered(item);

        if(item->binder() && !item->page_valid()) {
            //            item->self_bind();
            item->activate();
        }

        assert(item->page_valid() && item->unique_page());

        run_script(style_source);       //        assert(record->linkpage());

        QMainWindow::setWindowFlags(    //Qt::Window |
            Qt::FramelessWindowHint);
        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();

    }


    void Browser::activateWindow()
    {
        _entrance->setWidget(this);
        this->setParent(_entrance);
        _entrance->on_activate_window();

        globalparameters.find_screen()->toolbarsearch()->lineedits(this->tabWidget()->lineEditStack());

        QMainWindow::activateWindow();
    }

    void Browser::load_default_state()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("browser"));
        QByteArray data = settings.value(QLatin1String("default_state")).toByteArray();
        restore_state(data);
        settings.endGroup();
    }

    QSize Browser::sizeHint() const
    {
        QRect desktopRect = QApplication::desktop()->screenGeometry();
        QSize size = desktopRect.size() * qreal(0.9);
        return size;
    }

    void Browser::save()
    {
        QtSingleApplication::instance()->saveSession();

        QSettings settings;
        settings.beginGroup(QLatin1String("browser"));
        QByteArray data = save_state(false);
        settings.setValue(QLatin1String("default_state"), data);
        settings.endGroup();
    }

    static const qint32 browser_magic = 0xba;

    QByteArray Browser::save_state(bool withTabs) const
    {
        FindScreen *findscreen = globalparameters.find_screen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);

        int version = 2;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << qint32(browser_magic);
        stream << qint32(version);

        stream << size();
        stream << menuBar()->isVisible();
        stream << !findscreen->isHidden();
        stream << !_bookmarkstoolbar->isHidden();
        stream << !status_bar()->isHidden();

        if(withTabs)
            stream << tabWidget()->saveState();
        else
            stream << QByteArray();

        return data;
    }

    bool Browser::restore_state(const QByteArray &state)
    {
        FindScreen *findscreen = globalparameters.find_screen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);

        int version = 2;
        QByteArray sd = state;
        QDataStream stream(&sd, QIODevice::ReadOnly);

        if(stream.atEnd())
            return false;

        qint32 marker;
        qint32 v;
        stream >> marker;
        stream >> v;

        if(marker != browser_magic || v != version)
            return false;

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

        findscreen->setVisible(showToolbar);
        updateToolbarActionText(showToolbar);

        globalparameters.mainwindow()->menuBar()->setVisible(showMenuBar);
        [this](bool show) {_viewmenubar->setText(show ? tr("Hide Menu bar") : tr("Show Menu bar"));}(showMenuBar);

        _bookmarkstoolbar->setVisible(showBookmarksBar);
        update_bookmarks_toolbar_action_text(showBookmarksBar);

        status_bar()->setVisible(showStatusbar);
        update_statusbar_action_text(showStatusbar);

        if(!tabWidget()->restoreState(tabState))
            return false;

        return true;
    }

    void Browser::runScriptOnOpenViews(const QString &source)
    {
        for(int i = 0; i < tabWidget()->count(); ++i) {
            auto browserview = tabWidget()->webView(i);

            if(browserview != nullptr)
                browserview->page()->runJavaScript(source);
        }
    }

    void Browser::append_to_file_menu()
    {
        // File
        QMenu *filemenu = globalparameters.mainwindow()->file_menu();    //menuBar()->addMenu(tr("&File"));

        filemenu->addAction(tr("&New Window"), this, SLOT(slotFileNew()), QKeySequence::New);
        filemenu->addAction(_tabmanager->newTabAction());   // tr("New &Tab")
        filemenu->addAction(tr("&Open File..."), this, SLOT(slotFileOpen()), QKeySequence::Open);
        filemenu->addAction(tr("Open &Location..."), this, SLOT(slotSelectLineEdit()), QKeySequence(Qt::ControlModifier + Qt::Key_L));
        filemenu->addSeparator();
        filemenu->addAction(_tabmanager->closeTabAction()); //  tr("&Close Tab")
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

        //#if defined(Q_OS_OSX)
        //        fileMenu->addAction(tr("&Quit"), QtSingleApplication::instance(), SLOT(quitBrowser()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        //#else
        //        fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        //#endif

        globalparameters.mainwindow()->append_quit_menu();
        //        QAction *quit = new QAction(tr("&Quit"), globalparameters.mainwindow());
        //        quit->setShortcut(Qt::CTRL + Qt::Key_Q);
        //        connect(quit, &QAction::triggered, globalparameters.mainwindow(), &MainWindow::application_exit);
        //        filemenu->addAction(quit);

        //        globalparameters.tree_screen()->buttonmenu()->addMenu(filemenu);
    }


    void Browser::append_edit_menu()
    {

        // Edit
        QMenu *editMenu = globalparameters.mainwindow()->edit_menu();
        //            editMenu = globalparameters.mainwindow()->menuBar()->addMenu(tr("&Edit"));
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
        new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

        QAction *findnext_ = editMenu->addAction(tr("&Find Next"));
        findnext_->setShortcuts(QKeySequence::FindNext);
        connect(findnext_, &QAction::triggered, this, &Browser::slotEditFindNext);

        QAction *findprevious_ = editMenu->addAction(tr("&Find Previous"));
        findprevious_->setShortcuts(QKeySequence::FindPrevious);
        connect(findprevious_, &QAction::triggered, this, &Browser::slotEditFindPrevious);
        editMenu->addSeparator();

        editMenu->addAction(tr("&Browser preferences"), this, SLOT(slotPreferences()), tr("Ctrl+,"));

        globalparameters.tree_screen()->buttonmenu()->addMenu(editMenu);
    }

    void Browser::append_view_menu()
    {
        // View
        QMenu *viewMenu =  globalparameters.mainwindow()->view_menu();  // globalparameters.mainwindow()->menuBar()->addMenu(tr("&View"));
        viewMenu->clear();

        _viewmenubar = new QAction(this);
        //updateMenuBarActionText(true);
        _viewmenubar->setText(tr("Hide Menu bar"));
        _viewmenubar->setShortcut(tr("Shift+Ctrl+M"));
        connect(_viewmenubar, &QAction::triggered, this
                //&DockedWindow::slotViewMenuBar
        , [this]() {
            if(globalparameters.mainwindow()->menuBar()->isVisible()) {
                _viewmenubar->setText(tr("Show Menu bar"));    //updateMenuBarActionText(false);
                //globalparameters.mainwindow()->menuBar()->hide();
                globalparameters.mainwindow()->menuBar()->close();
            } else {
                _viewmenubar->setText(tr("Hide Menu bar"));    //updateMenuBarActionText(true);
                globalparameters.mainwindow()->menuBar()->show();
            }

            _autosaver->changeOccurred();
        }
               );

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

        _stop = viewMenu->addAction(tr("&Stop"));
        QList<QKeySequence> shortcuts;
        shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
        shortcuts.append(Qt::Key_Escape);
        _stop->setShortcuts(shortcuts);
        _tabmanager->addWebAction(_stop, QWebEnginePage::Stop);

        _reload = viewMenu->addAction(tr("Reload Page"));
        _reload->setShortcuts(QKeySequence::Refresh);
        _tabmanager->addWebAction(_reload, QWebEnginePage::Reload);

        viewMenu->addAction(tr("Zoom &In"), this, SLOT(slotViewZoomIn()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
        viewMenu->addAction(tr("Zoom &Out"), this, SLOT(slotViewZoomOut()), QKeySequence(Qt::CTRL | Qt::Key_Minus));
        viewMenu->addAction(tr("Reset &Zoom"), this, SLOT(slotViewResetZoom()), QKeySequence(Qt::CTRL | Qt::Key_0));

        viewMenu->addSeparator();
        viewMenu->addAction(tr("Page S&ource"), this, SLOT(slotViewPageSource()), tr("Ctrl+Alt+U"));
        QAction *a = viewMenu->addAction(tr("&Full Screen"), this, SLOT(slotViewFullScreen(bool)),  Qt::Key_F11);
        a->setCheckable(true);

        globalparameters.tree_screen()->buttonmenu()->addMenu(viewMenu);


    }

    void Browser::append_history_menu()
    {
        // History
        HistoryMenu *historyMenu =  globalparameters.mainwindow()->histry_menu();
        //        new HistoryMenu(
        //            globalparameters.mainwindow()   //this
        //        );
        historyMenu->clear();

        connect(historyMenu, &HistoryMenu::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu *>(historyMenu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        historyMenu->setTitle(tr("Hi&story"));

        globalparameters.mainwindow()->menuBar()->addMenu(historyMenu);

        globalparameters.tree_screen()->buttonmenu()->addMenu(historyMenu);

        QList<QAction *> historyActions;

        //_historyback = new QAction(tr("Back"), this);
        _tabmanager->addWebAction(_historyback, QWebEnginePage::Back);
        _historyback->setShortcuts(QKeySequence::Back);
        _historyback->setIconVisibleInMenu(false);
        historyActions.append(_historyback);

        //_historyforward = new QAction(tr("Forward"), this);
        _tabmanager->addWebAction(_historyforward, QWebEnginePage::Forward);
        _historyforward->setShortcuts(QKeySequence::Forward);
        _historyforward->setIconVisibleInMenu(false);
        historyActions.append(_historyforward);

        //QAction *_historyhome = new QAction(tr("Home"), this);
        //connect(_historyhome, SIGNAL(triggered()), this, SLOT(slotHome()));

        //        connect(_historyhome, &QAction::triggered, this, &DockedWindow::slotHome);

        _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

        historyActions.append(_historyhome);

#if defined(QWEBENGINEHISTORY_RESTORESESSION)
        _restorelastsession = new QAction(tr("Restore Last Session"), this);
        connect(_restorelastsession, SIGNAL(triggered()), QtSingleApplication::instance(), SLOT(restoreLastSession()));
        _restorelastsession->setEnabled(QtSingleApplication::instance()->canRestoreSession());
        historyActions.append(_tabwidget->recentlyClosedTabsAction());
        historyActions.append(_restorelastsession);
#endif

        historyMenu->setInitialActions(historyActions);
        globalparameters.tree_screen()->buttonmenu()->addMenu(historyMenu);
    }

    void Browser::append_bookmark_menu()
    {
        // Bookmarks
        BookmarksMenu *bookmarksMenu = globalparameters.mainwindow()->bookmark_menu();
        //        new BookmarksMenu(
        //            globalparameters.mainwindow()   //this
        //        );
        bookmarksMenu->clear();

        connect(bookmarksMenu, &BookmarksMenu::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        connect(static_cast<ModelMenu *>(bookmarksMenu), &ModelMenu::hovered_signal, this, &Browser::slotUpdateStatusbar);
        bookmarksMenu->setTitle(tr("&Bookmarks"));
        globalparameters.mainwindow()->menuBar()->addMenu(bookmarksMenu);

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

        globalparameters.tree_screen()->buttonmenu()->addMenu(bookmarksMenu);
    }

    void Browser::append_window_menu()
    {
        // Window
        _windowmenu = globalparameters.mainwindow()->window_menu(); //menuBar()->addMenu(tr("&Window"));
        _windowmenu->clear();
        connect(_windowmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowWindowMenu);
        slotAboutToShowWindowMenu();

        globalparameters.tree_screen()->buttonmenu()->addMenu(_windowmenu);
    }

    void Browser::append_to_tools_menu()
    {
        QMenu *toolsMenu = globalparameters.mainwindow()->tools_menu();  //menuBar()->addMenu(tr("&Tools"));
        toolsMenu->addAction(tr("Web &Search"), this, SLOT(slotWebSearch()), QKeySequence(tr("Ctrl+K", "Web Search")));
#if defined(QWEBENGINEINSPECTOR)
        a = toolsMenu->addAction(tr("Enable Web &Inspector"), this, SLOT(slotToggleInspector(bool)));
        a->setCheckable(true);
#endif

    }

    void Browser::append_to_help_menu()
    {
        globalparameters.mainwindow()->init_help_menu();
        assert(globalparameters.mainwindow()->help_menu());
        QMenu *helpMenu = globalparameters.mainwindow()->help_menu();  //menuBar()->addMenu(tr("&Help"));
        //        helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
        helpMenu->addAction(tr("About &Embedded Browser"), this, SLOT(slotAboutApplication()));

        globalparameters.tree_screen()->buttonmenu()->addMenu(helpMenu);
    }

    void Browser::append_to_main_menu()
    {
        new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(slotSwapFocus()));

        globalparameters.mainwindow()->menuBar()->clear();
        globalparameters.mainwindow()->init_file_menu();
        globalparameters.mainwindow()->init_tools_menu();

        append_to_file_menu();

        append_edit_menu();

        append_view_menu();

        append_history_menu();

        append_bookmark_menu();

        append_window_menu();

        append_to_tools_menu();

        append_to_help_menu();
    }


    void Browser::setup_tool_bar()
    {
        //auto mainwindow = globalparameters.mainwindow();
        FindScreen *findscreen = globalparameters.find_screen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);
        //        navigater = addToolBar(tr("Navigation"));  //mainwindow->addToolBar(tr("Navigation"));
        //        connect(navigater->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(updateToolbarActionText(bool)));

        //_historyback = findscreen->historyback();

        _historyback->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, findscreen)); //this

        _historybackmenu = new QMenu(this);
        _historyback->setMenu(_historybackmenu);
        connect(_historybackmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowBackMenu);
        connect(_historybackmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);

        //navigater->addAction(_historyback);

        _historyforward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, findscreen)); //this

        _historyforwardmenu = new QMenu(this);
        connect(_historyforwardmenu, &QMenu::aboutToShow, this, &Browser::slotAboutToShowForwardMenu);
        connect(_historyforwardmenu, &QMenu::triggered, this, &Browser::slotOpenActionUrl);
        _historyforward->setMenu(_historyforwardmenu);

        //navigater->addAction(_historyforward);

        //_stopreload = new QAction(navigater); //this

        _reloadicon = style()->standardIcon(QStyle::SP_BrowserReload);
        _stopreload->setIcon(_reloadicon);


        //navigater->addAction(_stopreload);


        //navigater->addWidget(_tabmanager->lineEditStack());
        _toolbarsearch = findscreen->toolbarsearch();

        if(_tabmanager->lineEditStack() == nullptr)
            _tabmanager->lineEditStack(findscreen->toolbarsearch()->lineedits());

        //_toolbarsearch = new ToolbarSearch(_tabmanager->lineEditStack(), findscreen->findtable(), navigater);

        //navigater->addWidget(_toolbarsearch);

        //connect(_toolbarsearch, SIGNAL(search(QUrl)), SLOT(loadUrl(QUrl)));   // move to Entrance

        //_chasewidget = new ChaseWidget(findscreen->size(), findscreen); //this
        //navigater->addWidget(_chasewidget);
    }

    void Browser::slotShowBookmarksDialog()
    {
        BookmarksDialog *dialog = new BookmarksDialog(this);
        connect(dialog, &BookmarksDialog::openUrl, _tabmanager, &TabWidget::loadUrlInCurrentTab);
        dialog->show();
    }

    void Browser::slotAddBookmark()
    {
        WebView *webView = currentTab();
        QString url = webView->page()->url().toString();
        QString title = webView->title();
        AddBookmarkDialog dialog(url, title);
        dialog.exec();
    }

    void Browser::slotViewToolbar()
    {
        FindScreen *findscreen = globalparameters.find_screen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);

        if(findscreen->isVisible()) {
            updateToolbarActionText(false);
            findscreen->close();
        } else {
            updateToolbarActionText(true);
            findscreen->show();
        }

        _autosaver->changeOccurred();
    }

    void Browser::slotViewBookmarksBar()
    {
        if(_bookmarkstoolbar->isVisible()) {
            update_bookmarks_toolbar_action_text(false);
            _bookmarkstoolbar->close();
        } else {
            update_bookmarks_toolbar_action_text(true);
            _bookmarkstoolbar->show();
        }

        _autosaver->changeOccurred();
    }

    void Browser::update_statusbar_action_text(bool visible)
    {
        _viewstatusbar->setText(!visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
    }

    void Browser::handle_find_text_result(bool found)
    {
        if(!found)
            slotUpdateStatusbar(tr("\"%1\" not found.").arg(_lastsearch));
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
        if(status_bar()->isVisible()) {
            update_statusbar_action_text(false);
            status_bar()->close();
        } else {
            update_statusbar_action_text(true);
            status_bar()->show();
        }

        _autosaver->changeOccurred();
    }

    void Browser::loadUrl(const QUrl &url)
    {
        if(!currentTab() || !url.isValid())
            return;

        _tabmanager->currentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
        _tabmanager->loadUrlInCurrentTab(url);
    }

    void Browser::slotDownloadManager()
    {
        QtSingleApplication::downloadManager()->show();
    }

    void Browser::slotSelectLineEdit()
    {
        _tabmanager->currentLineEdit()->selectAll();
        _tabmanager->currentLineEdit()->setFocus();
    }

    void Browser::slotFileSaveAs()
    {
        // not implemented yet.
    }

    void Browser::slotPreferences()
    {
        SettingsDialog *s = new SettingsDialog(this);
        s->show();
    }

    void Browser::slotUpdateStatusbar(const QString &string)
    {
        status_bar()->showMessage(string, 2000);
    }

    void Browser::slotUpdateWindowTitle(const QString &title)
    {
        if(title.isEmpty()) {
            setWindowTitle(tr("MyTetra"));
        } else {
#if defined(Q_OS_OSX)
            setWindowTitle(title);
#else
            setWindowTitle(tr("%1 - MyTetra", "Page title and Browser name").arg(title));
#endif
        }
    }

    void Browser::slotAboutApplication()
    {
        QMessageBox::about(this, tr("About"), tr(
                               "Version %1"
                               "<p>This demo demonstrates the facilities "
                               "of Qt WebEngine in action, providing an note with "
                               "browser for you to experiment with.<p>"
                               "<p>Qt WebEngine is based on the Chromium open source project "
                               "developed at <a href=\"http://www.chromium.org/\">http://www.chromium.org/</a>."
                           ).arg(QCoreApplication::applicationVersion()));
    }

    void Browser::slotFileNew()
    {
        //QtSingleApplication::instance()->
        //        _browser->new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));

        //        if(_entrance->window_list().count() == 0) { // ?
        //            globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
        //        }

        //        browser::Browser *browser = _entrance->activiated_registered().first;   //QtSingleApplication::instance()->mainWindow();
        //        assert(browser == this); // ?
        //        browser->slotHome();

        this->slotHome();
    }

    void Browser::slotFileOpen()
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                                                    tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

        if(file.isEmpty())
            return;

        loadPage(file);
    }
#define QT_NO_PRINTPREVIEWDIALOG

    void Browser::slotFilePrintPreview()
    {
#ifndef QT_NO_PRINTPREVIEWDIALOG

        if(!currentTab())
            return;

        QPrintPreviewDialog *dialog = new QPrintPreviewDialog(this);
        connect(dialog, &QPrintPreviewDialog::paintRequested, currentTab(), &WebView::print);
        dialog->exec();
#endif
    }

    void Browser::slotFilePrint()
    {
#if defined(QWEBENGINEPAGE_PRINT)

        if(!currentTab())
            return;

        printRequested(currentTab()->page()->mainFrame());
#endif
    }

#if defined(QWEBENGINEPAGE_PRINT)
    void BrowserWindow::printRequested(QWebEngineFrame *frame)
    {
#ifndef QT_NO_PRINTDIALOG
        QPrinter printer;
        QPrintDialog *dialog = new QPrintDialog(&printer, this);
        dialog->setWindowTitle(tr("Print Document"));

        if(dialog->exec() != QDialog::Accepted)
            return;

        frame->print(&printer);
#endif
    }
#endif

    void Browser::slotPrivateBrowsing()
    {
        if(!QtSingleApplication::instance()->privateBrowsing()) {
            QString title = tr("Are you sure you want to turn on private browsing?");
            QString text = tr("<b>%1</b><br><br>"
                              "This action will reload all open tabs.<br>"
                              "When private browsing in turned on,"
                              " webpages are not added to the history,"
                              " items are automatically removed from the Downloads window," \
                              " new cookies are not stored, current cookies can't be accessed," \
                              " site icons wont be stored, session wont be saved, " \
                              " and searches are not added to the pop-up menu in the Google search box." \
                              "  Until you close the window, you can still click the Back and Forward buttons" \
                              " to return to the webpages you have opened.").arg(title);

            QMessageBox::StandardButton button = QMessageBox::question(this, QString(), text,
                                                                       QMessageBox::Ok | QMessageBox::Cancel,
                                                                       QMessageBox::Ok);

            if(button == QMessageBox::Ok)
                QtSingleApplication::instance()->setPrivateBrowsing(true);
        } else {
            // TODO: Also ask here
            QtSingleApplication::instance()->setPrivateBrowsing(false);
        }
    }

    void Browser::resizeEvent(QResizeEvent *e)
    {
        _tabmanager->resizeEvent(e);
    }

    void Browser::closeEvent(QCloseEvent *event)
    {
        if(_tabmanager->count() > 1) {
            int ret = QMessageBox::warning(this, QString(),
                                           tr("Are you sure you want to close the window?"
                                              "  There are %1 tabs open").arg(_tabmanager->count()),
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

    void Browser::slotEditFind()
    {
        if(!currentTab())
            return;

        bool ok;
        QString search = QInputDialog::getText(this, tr("Find"),
                                               tr("Text:"), QLineEdit::Normal,
                                               _lastsearch, &ok);

        if(ok && !search.isEmpty()) {
            _lastsearch = search;
            currentTab()->findText(_lastsearch, 0, invoke(this, &Browser::handle_find_text_result));
        }
    }

    void Browser::slotEditFindNext()
    {
        if(!currentTab() && !_lastsearch.isEmpty())
            return;

        currentTab()->findText(_lastsearch);
    }

    void Browser::slotEditFindPrevious()
    {
        if(!currentTab() && !_lastsearch.isEmpty())
            return;

        currentTab()->findText(_lastsearch, QWebEnginePage::FindBackward);
    }

    void Browser::slotViewZoomIn()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    }

    void Browser::slotViewZoomOut()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    }

    void Browser::slotViewResetZoom()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(1.0);
    }

    void Browser::slotViewFullScreen(bool makeFullScreen)
    {
        if(makeFullScreen) {
            showFullScreen();
        } else {
            if(isMinimized())
                showMinimized();
            else if(isMaximized())
                showMaximized();
            else showNormal();
        }
    }

    void Browser::slotViewPageSource()
    {
        if(!currentTab())
            return;

        QPlainTextEdit *view = new QPlainTextEdit;
        view->setWindowTitle(tr("Page Source of %1").arg(currentTab()->title()));
        view->setMinimumWidth(640);
        view->setAttribute(Qt::WA_DeleteOnClose);
        view->show();

        currentTab()->page()->toHtml(invoke(view, &QPlainTextEdit::setPlainText));
    }

    // deprecated by record::preoperty::home
    void Browser::slotHome()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("MainWindow"));
        QString home = settings.value(QLatin1String("home"), QLatin1String(_defaulthome)).toString();
        //loadPage(home);
        //        auto ara = boost::make_shared<TabWidget::ActiveRecordBinder>(
        //                       _tabmanager // _entrance
        //                   );
        auto r = _tabmanager->request_item(
                     QUrl(home)
                     //                     , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)>>(
                     //                         ""
                     //                         , &TabWidget::ActiveRecordBinder::binder
                     //                         , ara
                     //                     )
                     //                     , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
                     //                         ""
                     //                         , &TabWidget::ActiveRecordBinder::activator
                     //                         , ara
                     //                     )
                 );
        assert(!r->is_lite());
        //        r->self_bind();
        r->activate();
    }

    void Browser::slotWebSearch()
    {
        _toolbarsearch->lineEdit()->selectAll();
        _toolbarsearch->lineEdit()->setFocus();
    }

    void Browser::slotToggleInspector(bool enable)
    {
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

    void Browser::slotSwapFocus()
    {
        if(currentTab()->hasFocus())
            _tabmanager->currentLineEdit()->setFocus();
        else
            currentTab()->setFocus();
    }

    void Browser::loadPage(const QString &page)
    {
        QUrl url = QUrl::fromUserInput(page);
        loadUrl(url);
    }

    //    BrowserTabManager *BrowserWindow::tabWidget() const
    //    {
    //        return _tabwidget;
    //    }

    WebView *Browser::currentTab() const
    {
        return _tabmanager->currentWebView();
    }

    void Browser::slotLoadProgress(int progress)
    {
        if(progress < 100 && progress > 0) {
            _chasewidget->setAnimated(true);
            disconnect(_stopreload, &QAction::triggered, _reload, &QAction::trigger);

            if(_stopicon.isNull())
                _stopicon = style()->standardIcon(QStyle::SP_BrowserStop);

            _stopreload->setIcon(_stopicon);
            connect(_stopreload, &QAction::triggered, _stop, &QAction::trigger);
            _stopreload->setToolTip(tr("Stop loading the current page"));
        } else {
            _chasewidget->setAnimated(false);
            disconnect(_stopreload, &QAction::triggered, _stop, &QAction::trigger);
            _stopreload->setIcon(_reloadicon);
            connect(_stopreload, &QAction::triggered, _reload, &QAction::trigger);
            _stopreload->setToolTip(tr("Reload the current page"));
        }
    }

    void Browser::slotAboutToShowBackMenu()
    {
        _historybackmenu->clear();

        if(!currentTab())
            return;

        QWebEngineHistory *history = currentTab()->history();
        int historyCount = history->count();

        for(int i = history->backItems(historyCount).count() - 1; i >= 0; --i) {
            QWebEngineHistoryItem item = history->backItems(history->count()).at(i);
            QAction *action = new QAction(this);
            action->setData(-1 * (historyCount - i - 1));
            QIcon icon = QtSingleApplication::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historybackmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowForwardMenu()
    {
        _historyforwardmenu->clear();

        if(!currentTab())
            return;

        QWebEngineHistory *history = currentTab()->history();
        int historyCount = history->count();

        for(int i = 0; i < history->forwardItems(history->count()).count(); ++i) {
            QWebEngineHistoryItem item = history->forwardItems(historyCount).at(i);
            QAction *action = new QAction(this);
            action->setData(historyCount - i);
            QIcon icon = QtSingleApplication::instance()->icon(item.url());
            action->setIcon(icon);
            action->setText(item.title());
            _historyforwardmenu->addAction(action);
        }
    }

    void Browser::slotAboutToShowWindowMenu()
    {
        _windowmenu->clear();
        _windowmenu->addAction(_tabmanager->nextTabAction());
        _windowmenu->addAction(_tabmanager->previousTabAction());
        _windowmenu->addSeparator();
        _windowmenu->addAction(tr("Downloads"), this, SLOT(slotDownloadManager()), QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));
        _windowmenu->addSeparator();

        QList<QPointer<Browser > > windows = _entrance->window_list();  //QtSingleApplication::instance()->mainWindows();

        for(int i = 0; i < windows.count(); ++i) {
            QPointer<Browser> window = windows.at(i);
            QAction *action = _windowmenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
            action->setData(i);
            action->setCheckable(true);

            if(window == this)
                action->setChecked(true);
        }
    }

    void Browser::slotShowWindow()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            QVariant v = action->data();

            if(v.canConvert<int>()) {
                int offset = qvariant_cast<int>(v);
                QList<QPointer<Browser> > windows = _entrance->window_list();   //QtSingleApplication::instance()->mainWindows();
                windows.at(offset)->activateWindow();
                windows.at(offset)->currentTab()->setFocus();
            }
        }
    }

    void Browser::slotOpenActionUrl(QAction *action)
    {
        int offset = action->data().toInt();
        QWebEngineHistory *history = currentTab()->history();

        if(offset < 0)
            history->goToItem(history->backItems(-1 * offset).first()); // back
        else if(offset > 0)
            history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
    }

    void Browser::geometry_change_requested(const QRect &geometry)
    {
        setGeometry(geometry);
    }


    //void DockedWindow::initUrl()
    //{

    //    // newMainWindow() needs to be called in main() for this to happen
    //    //    if(_browser->mainWindows().count() > 0) {
    //    //        QStringList args = QCoreApplication::arguments();

    //    //        if(args.count() > 1)
    //    //            loadPage(args.last());
    //    //        else
    //    //            slotHome();
    //    //    }
    //}

    QStatusBar *Browser::status_bar()
    {
        return globalparameters.status_bar();
    }

    QStatusBar *Browser::status_bar() const
    {
        return globalparameters.status_bar();
    }



    WebView *Browser::invoke_registered_page(boost::intrusive_ptr<TreeItem> item)
    {
        // clean();
        assert(item->is_registered_to_record_controller() == true);

        WebView *view = nullptr;

        //        if(_mainWindows.isEmpty()) {
        //            dp = new_dockedwindow(record);
        //        } else
        //        {
        //        for(auto &i : _mainWindows) {
        view = tabWidget()->find(item->field("url"));

        if(view != nullptr) {
            //            dp.first = i.data();

            if(!isVisible()) {
                raise();
                activateWindow();
            }

            //            break;
        }

        //            else if(isVisible()) {
        //                dp.first = i.data();
        //            }
        //        }

        //        if(dp.first == nullptr) {
        //            dp.first = _mainWindows[0].data();
        //        }

        //        assert(dp.first);
        //        }


        //        const DockedWindow *w = dp.first;
        TabWidget *const tab = tabWidget();

        if(view == nullptr
           // && w != nullptr
          ) {

            // Record *blank_url = check_register_record(QUrl(DockedWindow::_defaulthome));

            //            if(blank.isLite())blank.switchToFat();
            //            blank.setNaturalFieldSource("url", DockedWindow::_defaulthome);

            WebView *blankview = nullptr;
            blankview = tab->find(QUrl(Browser::_defaulthome));
            //PageView *no_pin = nullptr;
            WebView *nopin_view = tab->find_nopin();
            // assert(dp.first);

            if(blankview != nullptr) {
                view = blankview;
                //                view->page()->load(record);
                view->page()->equip_registered(item)->activate();

            } else if(nopin_view != nullptr) {   // no_pin
                view = nopin_view;

                if(view->page()->url().toString() != item->field("url")) {

                    view->page()->equip_registered(item)->activate(); // view->page()->load(record);
                }
            } else {
                view = tab->newTab(item);  // , false
                // auto load
            }

            //            tab->setCurrentWidget(dp.second);   // tab->setCurrentIndex(tab->webViewIndex(dp.second));
            //            dp.second->show();
        }

        //        tab->setCurrentWidget(dp.second);
        //        dp.second->show();
        //        assert(dp.first);
        assert(view);
        return view;  //_mainWindows[0];
    }


}






