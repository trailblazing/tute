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
#include "dockedwindow.h"

#include "autosaver.h"
#include "bookmarks.h"
#include "libraries/qtSingleApplication5/qtsingleapplication.h"

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
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/recordTable/Record.h"
#include "views/recordTable/RecordTableView.h"
#include "libraries/GlobalParameters.h"
#include "views/browser/entrance.h"
#include "views/recordTable/RecordTableScreen.h"
#include "controllers/recordTable/RecordTableController.h"
#include "views/browser/tabwidget.h"
#include "views/mainWindow/MainWindow.h"
#include "views/findInBaseScreen/FindScreen.h"
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

    DockedWindow::~DockedWindow()
    {

        _autosaver->changeOccurred();
        _autosaver->saveIfNeccessary();
        delete _autosaver;
        delete _bookmarkstoolbar;
        delete _tabmanager;
        //    delete dock_widget;
        delete _layout;
        delete _centralwidget;

    }

    void DockedWindow::init(QUrl const &url, const QString &style_source)
    {
        //        browser->setWidget(this);
        //        this->setParent(browser);
        setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
        setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        setWebAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        setWebAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        setToolButtonStyle(Qt::ToolButtonFollowStyle);
        setAttribute(Qt::WA_DeleteOnClose, true);
        //statusBar()
        status_bar()->setSizeGripEnabled(true);
        setupMenu();
        setupToolBar();

        //        QWidget *_centralwidget = new QWidget(this);
        //        BookmarksModel *bookmarksModel = QtSingleApplication::bookmarksManager()->bookmarksModel();
        //        _bookmarkstoolbar = new BookmarksToolBar(bookmarksModel, this);
        connect(_bookmarkstoolbar, SIGNAL(openUrl(QUrl)), _tabmanager, SLOT(loadUrlInCurrentTab(QUrl)));
        connect(_bookmarkstoolbar->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(updateBookmarksToolbarActionText(bool)));

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

        connect(_tabmanager, SIGNAL(loadPage(QString)), this, SLOT(loadPage(QString)));
        connect(_tabmanager, SIGNAL(setCurrentTitle(QString)), this, SLOT(slotUpdateWindowTitle(QString)));
        connect(_tabmanager, SIGNAL(showStatusBarMessage(QString)), status_bar(), SLOT(showMessage(QString)));
        connect(_tabmanager, SIGNAL(linkHovered(QString)), status_bar(), SLOT(showMessage(QString)));
        connect(_tabmanager, SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
        connect(_tabmanager, SIGNAL(tabsChanged()), _autosaver, SLOT(changeOccurred()));
        connect(_tabmanager, SIGNAL(geometryChangeRequested(QRect)), this, SLOT(geometryChangeRequested(QRect)));
#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        connect(_tabwidget, SIGNAL(printRequested(QWebEngineFrame *)), this, SLOT(printRequested(QWebEngineFrame *)));
#endif
        connect(_tabmanager, SIGNAL(menuBarVisibilityChangeRequested(bool)), menuBar(), SLOT(setVisible(bool)));
        connect(_tabmanager, SIGNAL(statusBarVisibilityChangeRequested(bool)), status_bar(), SLOT(setVisible(bool)));
        //        connect(_tabmanager, SIGNAL(toolBarVisibilityChangeRequested(bool)), navigater, SLOT(setVisible(bool)));
        connect(_tabmanager, SIGNAL(toolBarVisibilityChangeRequested(bool)), _bookmarkstoolbar, SLOT(setVisible(bool)));
#if defined(Q_OS_OSX)
        connect(_tabwidget, SIGNAL(lastTabClosed()), this, SLOT(close()));
#else
        connect(
            _tabmanager
            , &TabWidget::lastTabClosed
            , _tabmanager
        , [this]() ->void {
            //            _tabmanager->newTab(request_record(QUrl(DockedWindow::_defaulthome)));
            //            WebView *view = nullptr;
            //            sd::method(
            //                ""  //std::string _method_name
            //                , &TabWidget::newTab    //return_type(object_type::*f)(Arg...)          //  f can be std::function<> template?
            //                , view  //return_type *r            // = (return_type *)0
            //                , _tabmanager   //typename STATIC_IF_SHARED<object_type, object_pointer_type>::type o   //shared_ptr<object_type> o // = (object_type *const)0  //nullptr
            //                , Arg &&... arg
            //            );
            auto arint = boost::make_shared<TabWidget::NewTab>(_tabmanager, true);
            request_record(
                QUrl(DockedWindow::_defaulthome)
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                    ""
                    , &TabWidget::NewTab::generator
                    , arint
                )
                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                    ""
                    , &TabWidget::NewTab::activator
                    , arint
                )
            );
        });
#endif

        slotUpdateWindowTitle();
        loadDefaultState();

        //        connect(this, &DockedWindow::activateWindow, _entrance, &Entrance::on_activate_window);

        //Record *_backup = record;
        //PageView *new_view =

        //        _tabmanager->newTab(url);  // , false
        auto arint = boost::make_shared<TabWidget::NewTab>(_tabmanager, true);
        Record *record = request_record(    // why do this?
                             url
                             , std::make_shared <
                             sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const >> (
                                 ""
                                 , &TabWidget::NewTab::generator
                                 , arint
                             )
                             , std::make_shared <
                             sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void >> (
                                 ""
                                 , &TabWidget::NewTab::activator
                                 , arint
                             )
                         );

        //assert(new_view->page());
        //record->page(new_view->page());
        //assert(_backup->page());

        assert(record->binded_only_page());

        //        int size = _tabmanager->lineEditStack()->sizeHint().height();

        //        navigater->setIconSize(QSize(size, size));

        //    initUrl();
        //dock_widget->setWidget(this);
        //setParent(dock_widget);

        //        connect(globalparameters.mainwindow(), &MainWindow::setEnabled, this, &DockedWindow::setEnabled);
        //        connect(globalparameters.mainwindow(), &MainWindow::blockSignals, this, &DockedWindow::blockSignals);

        QMetaObject::invokeMethod(this, "runScriptOnOpenViews", Qt::QueuedConnection, Q_ARG(QString, style_source));
    }

    DockedWindow::DockedWindow(
        const QByteArray &state
        , RecordTableController *recordtablecontroller
        , Entrance *entrance   //, QDockWidget *parent
        , const QString &style_source
        , Qt::WindowFlags flags
    ) : QMainWindow(0, flags)
        , _recordtablecontroller(recordtablecontroller)
        , _entrance(entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
        , _tabmanager(new TabWidget(recordtablecontroller, this))
        , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
        , _chasewidget(globalparameters.getFindScreen()->chasewidget())
        , _autosaver(new AutoSaver(this))
        , _historyhome(globalparameters.getFindScreen()->historyhome())
        , _historyback(globalparameters.getFindScreen()->historyback())
        , _historyforward(globalparameters.getFindScreen()->historyforward())
        , _stop(0)
        , _reload(0)
        , _stopreload(globalparameters.getFindScreen()->stopreload())
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
    {
        init(QUrl(DockedWindow::_defaulthome), style_source);
        this->restore_state(state);
        QMainWindow::setWindowFlags(
            //Qt::Window |
            Qt::FramelessWindowHint);
        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();
    }

    DockedWindow::DockedWindow(const QUrl &url
                               , RecordTableController *recordtablecontroller
                               , Entrance *entrance   //, QDockWidget *parent
                               , const QString &style_source
                               , Qt::WindowFlags flags
                              )
        : QMainWindow(0, flags)
        , _recordtablecontroller(recordtablecontroller)
        , _entrance(entrance->prepend(this))         //    , dock_widget(new QDockWidget(parent, Qt::MaximizeUsingFullscreenGeometryHint))
        , _tabmanager(new TabWidget(recordtablecontroller, this))
        , _bookmarkstoolbar(new BookmarksToolBar(QtSingleApplication::bookmarksManager()->bookmarksModel(), this))
        , _chasewidget(globalparameters.getFindScreen()->chasewidget())
        , _autosaver(new AutoSaver(this))
        , _historyhome(globalparameters.getFindScreen()->historyhome())
        , _historyback(globalparameters.getFindScreen()->historyback())
        , _historyforward(globalparameters.getFindScreen()->historyforward())
        , _stop(0)
        , _reload(0)
        , _stopreload(globalparameters.getFindScreen()->stopreload())
        , _centralwidget(new QWidget(this))
        , _layout(new QVBoxLayout)
    {
        init(url, style_source);
        //        assert(record->linkpage());
        QMainWindow::setWindowFlags(
            //Qt::Window |
            Qt::FramelessWindowHint);
        QMainWindow::menuBar()->hide();
        QMainWindow::statusBar()->hide();
        show();
    }

    void DockedWindow::activateWindow()
    {
        _entrance->setWidget(this);
        this->setParent(_entrance);
        _entrance->on_activate_window();

        QMainWindow::activateWindow();
    }

    void DockedWindow::loadDefaultState()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("DockedWindow"));
        QByteArray data = settings.value(QLatin1String("defaultState")).toByteArray();
        restore_state(data);
        settings.endGroup();
    }

    QSize DockedWindow::sizeHint() const
    {
        QRect desktopRect = QApplication::desktop()->screenGeometry();
        QSize size = desktopRect.size() * qreal(0.9);
        return size;
    }

    void DockedWindow::save()
    {
        QtSingleApplication::instance()->saveSession();

        QSettings settings;
        settings.beginGroup(QLatin1String("DockedWindow"));
        QByteArray data = save_state(false);
        settings.setValue(QLatin1String("defaultState"), data);
        settings.endGroup();
    }

    static const qint32 DockedWindowMagic = 0xba;

    QByteArray DockedWindow::save_state(bool withTabs) const
    {
        FindScreen *findscreen = globalparameters.getFindScreen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);

        int version = 2;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << qint32(DockedWindowMagic);
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

    bool DockedWindow::restore_state(const QByteArray &state)
    {
        FindScreen *findscreen = globalparameters.getFindScreen();
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

        if(marker != DockedWindowMagic || v != version)
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
        updateBookmarksToolbarActionText(showBookmarksBar);

        status_bar()->setVisible(showStatusbar);
        updateStatusbarActionText(showStatusbar);

        if(!tabWidget()->restoreState(tabState))
            return false;

        return true;
    }

    void DockedWindow::runScriptOnOpenViews(const QString &source)
    {
        for(int i = 0; i < tabWidget()->count(); ++i) {
            auto browserview = tabWidget()->webView(i);

            if(browserview != nullptr)
                browserview->page()->runJavaScript(source);
        }
    }

    void DockedWindow::setupMenu()
    {
        new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(slotSwapFocus()));

        // File
        QMenu *filemenu = globalparameters.mainwindow()->filemenu();    //menuBar()->addMenu(tr("&File"));

        filemenu->addAction(tr("&New Window"), this, SLOT(slotFileNew()), QKeySequence::New);
        filemenu->addAction(_tabmanager->newTabAction());
        filemenu->addAction(tr("&Open File..."), this, SLOT(slotFileOpen()), QKeySequence::Open);
        filemenu->addAction(tr("Open &Location..."), this,
                            SLOT(slotSelectLineEdit()), QKeySequence(Qt::ControlModifier + Qt::Key_L));
        filemenu->addSeparator();
        filemenu->addAction(_tabmanager->closeTabAction());
        filemenu->addSeparator();
#if defined(QWEBENGINE_SAVE_AS_FILE)
        fileMenu->addAction(tr("&Save As..."), this,
                            SLOT(slotFileSaveAs()), QKeySequence(QKeySequence::Save));
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
        connect(QtSingleApplication::instance(), SIGNAL(privateBrowsingChanged(bool)), action, SLOT(setChecked(bool)));
        filemenu->addSeparator();

        //#if defined(Q_OS_OSX)
        //        fileMenu->addAction(tr("&Quit"), QtSingleApplication::instance(), SLOT(quitBrowser()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        //#else
        //        fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_Q));
        //#endif


        QAction *quit = new QAction(tr("&Quit"), globalparameters.mainwindow());
        quit->setShortcut(Qt::CTRL + Qt::Key_Q);
        connect(quit, SIGNAL(triggered()), globalparameters.mainwindow(), SLOT(applicationExit()));
        filemenu->addAction(quit);

        // Edit
        QMenu *editMenu = globalparameters.mainwindow()->menuBar()->addMenu(tr("&Edit"));
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
        connect(find_, SIGNAL(triggered()), this, SLOT(slotEditFind()));
        new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

        QAction *findnext_ = editMenu->addAction(tr("&Find Next"));
        findnext_->setShortcuts(QKeySequence::FindNext);
        connect(findnext_, SIGNAL(triggered()), this, SLOT(slotEditFindNext()));

        QAction *findprevious_ = editMenu->addAction(tr("&Find Previous"));
        findprevious_->setShortcuts(QKeySequence::FindPrevious);
        connect(findprevious_, SIGNAL(triggered()), this, SLOT(slotEditFindPrevious()));
        editMenu->addSeparator();

        editMenu->addAction(tr("&Browser preferences"), this, SLOT(slotPreferences()), tr("Ctrl+,"));

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(editMenu);

        // View
        QMenu *viewMenu = globalparameters.mainwindow()->menuBar()->addMenu(tr("&View"));

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
        updateBookmarksToolbarActionText(true);
        _viewbookmarkbar->setShortcut(tr("Shift+Ctrl+B"));
        connect(_viewbookmarkbar, SIGNAL(triggered()), this, SLOT(slotViewBookmarksBar()));
        viewMenu->addAction(_viewbookmarkbar);

        _viewtoolbar = new QAction(this);
        updateToolbarActionText(true);
        _viewtoolbar->setShortcut(tr("Ctrl+|"));
        connect(_viewtoolbar, SIGNAL(triggered()), this, SLOT(slotViewToolbar()));
        viewMenu->addAction(_viewtoolbar);

        _viewstatusbar = new QAction(this);
        updateStatusbarActionText(true);
        _viewstatusbar->setShortcut(tr("Ctrl+/"));
        connect(_viewstatusbar, SIGNAL(triggered()), this, SLOT(slotViewStatusbar()));
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

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(viewMenu);

        // History
        HistoryMenu *historyMenu = new HistoryMenu(
            globalparameters.mainwindow()   //this
        );
        connect(historyMenu, SIGNAL(openUrl(QUrl)), _tabmanager, SLOT(loadUrlInCurrentTab(QUrl)));
        connect(historyMenu, SIGNAL(hovered(QString)), this, SLOT(slotUpdateStatusbar(QString)));
        historyMenu->setTitle(tr("Hi&story"));

        globalparameters.mainwindow()->menuBar()->addMenu(historyMenu);

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(historyMenu);

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

        // Bookmarks
        BookmarksMenu *bookmarksMenu = new BookmarksMenu(
            globalparameters.mainwindow()   //this
        );
        connect(bookmarksMenu, SIGNAL(openUrl(QUrl)), _tabmanager, SLOT(loadUrlInCurrentTab(QUrl)));
        connect(bookmarksMenu, SIGNAL(hovered(QString)), this, SLOT(slotUpdateStatusbar(QString)));
        bookmarksMenu->setTitle(tr("&Bookmarks"));
        globalparameters.mainwindow()->menuBar()->addMenu(bookmarksMenu);

        QList<QAction *> bookmarksActions;

        QAction *showAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
        connect(showAllBookmarksAction, SIGNAL(triggered()), this, SLOT(slotShowBookmarksDialog()));
        _addbookmark = new QAction(QIcon(QLatin1String(":addbookmark.png")), tr("Add Bookmark..."), this);
        _addbookmark->setIconVisibleInMenu(false);

        connect(_addbookmark, SIGNAL(triggered()), this, SLOT(slotAddBookmark()));
        _addbookmark->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

        bookmarksActions.append(showAllBookmarksAction);
        bookmarksActions.append(_addbookmark);
        bookmarksMenu->setInitialActions(bookmarksActions);

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(bookmarksMenu);

        // Window
        _windowmenu = globalparameters.mainwindow()->menuBar()->addMenu(tr("&Window"));
        connect(_windowmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowWindowMenu()));
        slotAboutToShowWindowMenu();

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(_windowmenu);

        QMenu *toolsMenu = globalparameters.mainwindow()->toolsmenu();  //menuBar()->addMenu(tr("&Tools"));
        toolsMenu->addAction(tr("Web &Search"), this, SLOT(slotWebSearch()), QKeySequence(tr("Ctrl+K", "Web Search")));
#if defined(QWEBENGINEINSPECTOR)
        a = toolsMenu->addAction(tr("Enable Web &Inspector"), this, SLOT(slotToggleInspector(bool)));
        a->setCheckable(true);
#endif
        globalparameters.mainwindow()->initHelpMenu();
        assert(globalparameters.mainwindow()->helpmenu());
        QMenu *helpMenu = globalparameters.mainwindow()->helpmenu();  //menuBar()->addMenu(tr("&Help"));
        //        helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
        helpMenu->addAction(tr("About &Embedded Browser"), this, SLOT(slotAboutApplication()));

        globalparameters.getTreeScreen()->buttonmenu()->addMenu(helpMenu);
    }

    void DockedWindow::setupToolBar()
    {
        //auto mainwindow = globalparameters.mainwindow();
        FindScreen *findscreen = globalparameters.getFindScreen();
        assert(findscreen);
        //        QToolBar *navigater = findscreen->navigater();
        //        assert(navigater);
        //        navigater = addToolBar(tr("Navigation"));  //mainwindow->addToolBar(tr("Navigation"));
        //        connect(navigater->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(updateToolbarActionText(bool)));

        //_historyback = findscreen->historyback();

        _historyback->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, findscreen)); //this

        _historybackmenu = new QMenu(this);
        _historyback->setMenu(_historybackmenu);
        connect(_historybackmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowBackMenu()));
        connect(_historybackmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));

        //navigater->addAction(_historyback);

        _historyforward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, findscreen)); //this

        _historyforwardmenu = new QMenu(this);
        connect(_historyforwardmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowForwardMenu()));
        connect(_historyforwardmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));
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

    void DockedWindow::slotShowBookmarksDialog()
    {
        BookmarksDialog *dialog = new BookmarksDialog(this);
        connect(dialog, SIGNAL(openUrl(QUrl)), _tabmanager, SLOT(loadUrlInCurrentTab(QUrl)));
        dialog->show();
    }

    void DockedWindow::slotAddBookmark()
    {
        WebView *webView = currentTab();
        QString url = webView->page()->url().toString();
        QString title = webView->title();
        AddBookmarkDialog dialog(url, title);
        dialog.exec();
    }

    void DockedWindow::slotViewToolbar()
    {
        FindScreen *findscreen = globalparameters.getFindScreen();
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

    void DockedWindow::slotViewBookmarksBar()
    {
        if(_bookmarkstoolbar->isVisible()) {
            updateBookmarksToolbarActionText(false);
            _bookmarkstoolbar->close();
        } else {
            updateBookmarksToolbarActionText(true);
            _bookmarkstoolbar->show();
        }

        _autosaver->changeOccurred();
    }

    void DockedWindow::updateStatusbarActionText(bool visible)
    {
        _viewstatusbar->setText(!visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
    }

    void DockedWindow::handleFindTextResult(bool found)
    {
        if(!found)
            slotUpdateStatusbar(tr("\"%1\" not found.").arg(_lastsearch));
    }

    void DockedWindow::updateToolbarActionText(bool visible)
    {
        _viewtoolbar->setText(!visible ? tr("Show Find in base") : tr("Hide Find in base"));
    }

    void DockedWindow::updateBookmarksToolbarActionText(bool visible)
    {
        _viewbookmarkbar->setText(!visible ? tr("Show Bookmarks bar") : tr("Hide Bookmarks bar"));
    }

    void DockedWindow::slotViewStatusbar()
    {
        if(status_bar()->isVisible()) {
            updateStatusbarActionText(false);
            status_bar()->close();
        } else {
            updateStatusbarActionText(true);
            status_bar()->show();
        }

        _autosaver->changeOccurred();
    }

    void DockedWindow::loadUrl(const QUrl &url)
    {
        if(!currentTab() || !url.isValid())
            return;

        _tabmanager->currentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
        _tabmanager->loadUrlInCurrentTab(url);
    }

    void DockedWindow::slotDownloadManager()
    {
        QtSingleApplication::downloadManager()->show();
    }

    void DockedWindow::slotSelectLineEdit()
    {
        _tabmanager->currentLineEdit()->selectAll();
        _tabmanager->currentLineEdit()->setFocus();
    }

    void DockedWindow::slotFileSaveAs()
    {
        // not implemented yet.
    }

    void DockedWindow::slotPreferences()
    {
        SettingsDialog *s = new SettingsDialog(this);
        s->show();
    }

    void DockedWindow::slotUpdateStatusbar(const QString &string)
    {
        status_bar()->showMessage(string, 2000);
    }

    void DockedWindow::slotUpdateWindowTitle(const QString &title)
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

    void DockedWindow::slotAboutApplication()
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

    void DockedWindow::slotFileNew()
    {
        //QtSingleApplication::instance()->
        //        _browser->new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));
        DockedWindow *mw = _entrance->active_chain().first;    //QtSingleApplication::instance()->mainWindow();
        mw->slotHome();
    }

    void DockedWindow::slotFileOpen()
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
                                                    tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

        if(file.isEmpty())
            return;

        loadPage(file);
    }

    void DockedWindow::slotFilePrintPreview()
    {
#ifndef QT_NO_PRINTPREVIEWDIALOG

        if(!currentTab())
            return;

        QPrintPreviewDialog *dialog = new QPrintPreviewDialog(this);
        connect(dialog, SIGNAL(paintRequested(QPrinter *)),
                currentTab(), SLOT(print(QPrinter *)));
        dialog->exec();
#endif
    }

    void DockedWindow::slotFilePrint()
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

    void DockedWindow::slotPrivateBrowsing()
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

    void DockedWindow::closeEvent(QCloseEvent *event)
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

    void DockedWindow::slotEditFind()
    {
        if(!currentTab())
            return;

        bool ok;
        QString search = QInputDialog::getText(this, tr("Find"),
                                               tr("Text:"), QLineEdit::Normal,
                                               _lastsearch, &ok);

        if(ok && !search.isEmpty()) {
            _lastsearch = search;
            currentTab()->findText(_lastsearch, 0, invoke(this, &DockedWindow::handleFindTextResult));
        }
    }

    void DockedWindow::slotEditFindNext()
    {
        if(!currentTab() && !_lastsearch.isEmpty())
            return;

        currentTab()->findText(_lastsearch);
    }

    void DockedWindow::slotEditFindPrevious()
    {
        if(!currentTab() && !_lastsearch.isEmpty())
            return;

        currentTab()->findText(_lastsearch, QWebEnginePage::FindBackward);
    }

    void DockedWindow::slotViewZoomIn()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    }

    void DockedWindow::slotViewZoomOut()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    }

    void DockedWindow::slotViewResetZoom()
    {
        if(!currentTab())
            return;

        currentTab()->setZoomFactor(1.0);
    }

    void DockedWindow::slotViewFullScreen(bool makeFullScreen)
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

    void DockedWindow::slotViewPageSource()
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
    void DockedWindow::slotHome()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("MainWindow"));
        QString home = settings.value(QLatin1String("home"), QLatin1String(_defaulthome)).toString();
        //loadPage(home);
        auto ara = boost::make_shared<Entrance::active_record_alternative>(_entrance);
        request_record(
            QUrl(home)
            , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                ""
                , &Entrance::active_record_alternative::generator
                , ara
            )
            , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                ""
                , &Entrance::active_record_alternative::activator
                , ara
            )
        );

    }

    void DockedWindow::slotWebSearch()
    {
        _toolbarsearch->lineEdit()->selectAll();
        _toolbarsearch->lineEdit()->setFocus();
    }

    void DockedWindow::slotToggleInspector(bool enable)
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

    void DockedWindow::slotSwapFocus()
    {
        if(currentTab()->hasFocus())
            _tabmanager->currentLineEdit()->setFocus();
        else
            currentTab()->setFocus();
    }

    void DockedWindow::loadPage(const QString &page)
    {
        QUrl url = QUrl::fromUserInput(page);
        loadUrl(url);
    }

    //    BrowserTabManager *BrowserWindow::tabWidget() const
    //    {
    //        return _tabwidget;
    //    }

    WebView *DockedWindow::currentTab() const
    {
        return _tabmanager->currentWebView();
    }

    void DockedWindow::slotLoadProgress(int progress)
    {
        if(progress < 100 && progress > 0) {
            _chasewidget->setAnimated(true);
            disconnect(_stopreload, SIGNAL(triggered()), _reload, SLOT(trigger()));

            if(_stopicon.isNull())
                _stopicon = style()->standardIcon(QStyle::SP_BrowserStop);

            _stopreload->setIcon(_stopicon);
            connect(_stopreload, SIGNAL(triggered()), _stop, SLOT(trigger()));
            _stopreload->setToolTip(tr("Stop loading the current page"));
        } else {
            _chasewidget->setAnimated(false);
            disconnect(_stopreload, SIGNAL(triggered()), _stop, SLOT(trigger()));
            _stopreload->setIcon(_reloadicon);
            connect(_stopreload, SIGNAL(triggered()), _reload, SLOT(trigger()));
            _stopreload->setToolTip(tr("Reload the current page"));
        }
    }

    void DockedWindow::slotAboutToShowBackMenu()
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

    void DockedWindow::slotAboutToShowForwardMenu()
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

    void DockedWindow::slotAboutToShowWindowMenu()
    {
        _windowmenu->clear();
        _windowmenu->addAction(_tabmanager->nextTabAction());
        _windowmenu->addAction(_tabmanager->previousTabAction());
        _windowmenu->addSeparator();
        _windowmenu->addAction(tr("Downloads"), this, SLOT(slotDownloadManager()), QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));
        _windowmenu->addSeparator();

        QList<QPointer<DockedWindow > > windows = _entrance->window_list();  //QtSingleApplication::instance()->mainWindows();

        for(int i = 0; i < windows.count(); ++i) {
            QPointer<DockedWindow> window = windows.at(i);
            QAction *action = _windowmenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
            action->setData(i);
            action->setCheckable(true);

            if(window == this)
                action->setChecked(true);
        }
    }

    void DockedWindow::slotShowWindow()
    {
        if(QAction *action = qobject_cast<QAction *>(sender())) {
            QVariant v = action->data();

            if(v.canConvert<int>()) {
                int offset = qvariant_cast<int>(v);
                QList<QPointer<DockedWindow> > windows = _entrance->window_list();   //QtSingleApplication::instance()->mainWindows();
                windows.at(offset)->activateWindow();
                windows.at(offset)->currentTab()->setFocus();
            }
        }
    }

    void DockedWindow::slotOpenActionUrl(QAction *action)
    {
        int offset = action->data().toInt();
        QWebEngineHistory *history = currentTab()->history();

        if(offset < 0)
            history->goToItem(history->backItems(-1 * offset).first()); // back
        else if(offset > 0)
            history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
    }

    void DockedWindow::geometryChangeRequested(const QRect &geometry)
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

    QStatusBar *DockedWindow::status_bar()
    {
        return globalparameters.getStatusBar();
    }

    QStatusBar *DockedWindow::status_bar() const
    {
        return globalparameters.getStatusBar();
    }



    WebView *DockedWindow::invoke_page(Record *const record)
    {
        // clean();

        WebView *view = nullptr;

        //        if(_mainWindows.isEmpty()) {
        //            dp = new_dockedwindow(record);
        //        } else
        //        {
        //        for(auto &i : _mainWindows) {
        view = tabWidget()->find(record->getNaturalFieldSource("url"));

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
            blankview = tab->find(QUrl(DockedWindow::_defaulthome));
            //PageView *no_pin = nullptr;
            WebView *nopin_view = tab->find_nopin();
            // assert(dp.first);

            if(blankview != nullptr) {
                view = blankview;
                view->page()->load(record);
            } else if(nopin_view != nullptr) {   // no_pin
                view = nopin_view;

                if(view->page()->url().toString() != record->getNaturalFieldSource("url")) {

                    view->page()->load(record);
                }
            } else {
                view = tab->newTab(record);  // , false
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






