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

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QWebEngineSettings>
#include "controllers/recordTable/RecordTableController.h"
//#include "tabmanager.h"



QT_BEGIN_NAMESPACE
class QWebEngineFrame;
QT_END_NAMESPACE

//extern Record *default_record;

QT_BEGIN_NAMESPACE

namespace browser {

    class AutoSaver;
    class BookmarksToolBar;
    class ChaseWidget;
    class TabWidget;
    class ToolbarSearch;
    class WebView;
    class Entrance;



    /*!
        The MainWindow of the Browser Application.

        Handles the tab widget and all the actions
     */
    class Browser : public QMainWindow {
        Q_OBJECT

    public:
        Browser(
            QUrl const &url         // Record *const record
            , RecordTableController *recordtablecontroller
            , Entrance *_entrance   //, QDockWidget *parent
            , const QString &style_source
            , Qt::WindowFlags flags = 0
        );


        Browser(
            const QByteArray &state
            , RecordTableController *recordtablecontroller
            , Entrance *_entrance   //, QDockWidget *parent
            , const QString &style_source
            , Qt::WindowFlags flags = 0
        );

        Browser(
            Record *const record
            , RecordTableController *recordtablecontroller
            , Entrance *entrance   //, QDockWidget *parent
            , const QString &style_source
            , Qt::WindowFlags flags = 0
        );

        ~Browser();
        QSize sizeHint() const;

        static constexpr const char *_defaulthome = "about:blank";

    public:
        TabWidget *tabWidget() const {return _tabmanager;}
        TabWidget *tabWidget() {return _tabmanager;}
        WebView *currentTab() const;
        QByteArray save_state(bool withTabs = true) const;
        bool restore_state(const QByteArray &state);
        Q_INVOKABLE void runScriptOnOpenViews(const QString &);
        void setWebAttribute(QWebEngineSettings::WebAttribute attribute, bool enabled) {_webattribute = attribute; _webattributeenabled = enabled;}
        QString &lastsearch() {return _lastsearch;}
        const QString &lastsearch() const {return _lastsearch;}
        //BrowserView *find_view(const Record *record) {return tabWidget()->find_view(record);}
        void activateWindow();
        QAction *historyback() {return _historyback;}
        QStatusBar *statusBar() = delete;
        QStatusBar *status_bar();       // {return globalparameters.getStatusBar();};
        QStatusBar *status_bar() const; // {return globalparameters.getStatusBar();};
        WebView *invoke_page(Record *const record);

    public slots:
        void loadPage(const QString &url);
        void slotHome();
        void updateToolbarActionText(bool visible);
        //    QAction *getactionFreeze() { return actionFreeze; }
    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        void save();

        void slotLoadProgress(int);
        void slotUpdateStatusbar(const QString &string);
        void slotUpdateWindowTitle(const QString &title = QString());

        void loadUrl(const QUrl &url);
        void slotPreferences();

        void slotFileNew();
        void slotFileOpen();
        void slotFilePrintPreview();
        void slotFilePrint();
        void slotPrivateBrowsing();
        void slotFileSaveAs();
        void slotEditFind();
        void slotEditFindNext();
        void slotEditFindPrevious();
        void slotShowBookmarksDialog();
        void slotAddBookmark();
        void slotViewZoomIn();
        void slotViewZoomOut();
        void slotViewResetZoom();
        void slotViewToolbar();
        void slotViewBookmarksBar();
        void slotViewStatusbar();
        void slotViewPageSource();
        void slotViewFullScreen(bool enable);

        void slotWebSearch();
        void slotToggleInspector(bool enable);
        void slotAboutApplication();
        void slotDownloadManager();
        void slotSelectLineEdit();

        void slotAboutToShowBackMenu();
        void slotAboutToShowForwardMenu();
        void slotAboutToShowWindowMenu();
        void slotOpenActionUrl(QAction *action);
        void slotShowWindow();
        void slotSwapFocus();

#if defined(QWEBENGINEPAGE_PRINT)
        void printRequested(QWebEngineFrame *frame);
#endif
        void geometryChangeRequested(const QRect &geometry);
        //        void updateToolbarActionText(bool visible);
        void updateBookmarksToolbarActionText(bool visible);

    private:
        void init();
        void register_url(QUrl const &url);
        void equip_registered(Record *const record);
        void run_script(const QString &style_source);
        void loadDefaultState();
        void setupMenu();
        void setupToolBar();
        void updateStatusbarActionText(bool visible);
        void handleFindTextResult(bool found);
        //    void initUrl();
    private:

        RecordTableController *_recordtablecontroller;
        Entrance *_entrance;
        TabWidget *_tabmanager;
        //    QDockWidget *dock_widget;
        //        QToolBar *navigater;
        ToolbarSearch *_toolbarsearch;
        BookmarksToolBar *_bookmarkstoolbar;
        ChaseWidget *_chasewidget;

        AutoSaver *_autosaver;

        QAction *_historyhome;
        QAction *_historyback;
        QMenu *_historybackmenu;
        QAction *_historyforward;
        QMenu *_historyforwardmenu;
        QMenu *_windowmenu;

        QAction *_stop;
        QAction *_reload;
        QAction *_stopreload;
        QAction *_viewmenubar;
        QAction *_viewbookmarkbar;
        QAction *_viewtoolbar;
        QAction *_viewstatusbar;
        QAction *_restorelastsession;
        QAction *_addbookmark;

        QIcon _reloadicon;
        QIcon _stopicon;

        QString _lastsearch;
        //    QAction *actionFreeze;
        QWebEngineSettings::WebAttribute _webattribute;
        bool _webattributeenabled;
        QWidget *_centralwidget;
        QVBoxLayout *_layout;

        friend class QtSingleApplication;
    };
}


QT_END_NAMESPACE


#endif // BROWSERWINDOW_H



