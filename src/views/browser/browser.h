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

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <QBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QWebEngineSettings>

#include <QWebEngineProfile>
// #include <QtWebEngine/QQuickWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>

#include "controllers/record_table/record_controller.h"

// #include "models/tree/TreeItem.h"
#include "models/tree/tree_model.h"
#include "views/tree/tree_screen.h"



QT_BEGIN_NAMESPACE
class QWebEngineFrame;

QT_END_NAMESPACE

class HidableTabWidget;
// extern Record *default_record;
class FingScreen;
class tm_t;
class ts_t;
class tsv_t;
struct RecordIndex;

extern const char *profile_storage_name;

QT_BEGIN_NAMESPACE

namespace browser {

    class AutoSaver;
    class BookmarksToolBar;
    class ChaseWidget;
    class TabWidget;
    class ToolbarSearch;
    class WebView;
    class Entrance;



    class UrlRequestInterceptor
        :	// public boost::intrusive_ref_counter<UrlRequestInterceptor, boost::thread_safe_counter>,
          public QWebEngineUrlRequestInterceptor {
        public:
            UrlRequestInterceptor(QObject *p);
            virtual void interceptRequest(QWebEngineUrlRequestInfo &info);
    };

    class Profile
        :	// public boost::intrusive_ref_counter<Profile, boost::thread_safe_counter>,
          public QWebEngineProfile {	// QQuickWebEngineProfile
        public:
            Profile(const QString &name, QObject *p);
            ~Profile();
        private:
            UrlRequestInterceptor *_urlrequestinterceptor;
    };

        /*!
            The MainWindow of the Browser Application.

            Handles the tab widget and all the actions
         */
    class Browser :
                  public QMainWindow
                        //        , public boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>
    {
        Q_OBJECT

        public:
            Browser(ts_t          *_tree_screen
                   , FindScreen        *_find_screen
                   , MetaEditor        *_editor_screen
                   , wn_t        *_main_window
                   , Entrance          *_entrance
                   , const QString     &style_source
                   , browser::Profile  *_profile
                   , Qt::WindowFlags flags);

                //        Browser(QUrl const &url         // Record *const record
                //                , TreeScreen        *_tree_screen
                //                , FindScreen        *_find_screen
                //                , MetaEditor        *_editor_screen
                //                , HidableTabWidget  *_vtabwidget
                //                , MainWindow        *_main_window
                //                , Entrance          *_entrance   //, QDockWidget *parent
                //                , const QString     &style_source
                //                , Qt::WindowFlags   flags = 0
                //               );


                //        Browser(const QByteArray    &state
                //                , TreeScreen        *_tree_screen
                //                , FindScreen        *_find_screen
                //                , MetaEditor        *_editor_screen
                //                , HidableTabWidget  *_vtabwidget
                //                , MainWindow        *_main_window
                //                , Entrance          *_entrance   //, QDockWidget *parent
                //                , const QString     &style_source
                //                , Qt::WindowFlags   flags = 0
                //               );

                //        Browser(boost::intrusive_ptr<TreeItem> item
                //                , TreeScreen        *_tree_screen
                //                , FindScreen        *_find_screen
                //                , MetaEditor        *_editor_screen  //, RecordController *record_controller
                //                , HidableTabWidget  *_vtabwidget
                //                , MainWindow        *_main_window
                //                , Entrance          *_entrance   //, QDockWidget *parent
                //                , const QString     &style_source
                //                , Qt::WindowFlags   flags = 0
                //               );

            ~Browser();
            QSize sizeHint() const;

            static constexpr const char *_defaulthome = "about:blank";

        public:
                //        typedef Binder coupler_delegation;
                //        typedef Binder::bind_interface          bind_interface;
                //        typedef Binder::activate_interface      activate_interface;
                //        typedef Binder::item_interface  bounded_item_interface;
                //        typedef Binder::page_interface  bounded_page_interface;

                //        typedef Binder::bind_helper         bind_helper;
                //        typedef Binder::activate_helper     activate_helper;
                //        typedef Binder::item_helper bounded_item_helper;
                //        typedef Binder::page_helper bounded_page_helper;

            TabWidget   *tabWidget();
            TabWidget   *tabWidget() const;
            TabWidget   *tabmanager();
            TabWidget   *tabmanager() const;

            WebView     *currentTab() const;
            QByteArray  save_state(bool withTabs = true) const;
            bool        restore_state(const QByteArray &state);
            Q_INVOKABLE void runScriptOnOpenViews(const QString &);
            void        setWebAttribute(QWebEngineSettings::WebAttribute attribute, bool enabled);
            QString     &lastsearch();
            const QString &lastsearch() const;

            void        activateWindow();
            bool is_under_construction() const;
            QAction     *historyback();
            QStatusBar  *statusBar() = delete;
            QStatusBar  *status_bar();
            QStatusBar  *status_bar() const;
            boost::intrusive_ptr<TreeItem> page_instantiate(boost::intrusive_ptr<RecordIndex> record_index);
                //        boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<TreeItem> item);
            rs_t *record_screen();
            Entrance    *entrance();
        public slots:
//            void loadPage(const QString &url) = delete;
            void slotHome();
            void updateToolbarActionText(bool visible);	// void updateToolbarActionText(bool visible);

        protected:
            void closeEvent(QCloseEvent *event);
            void resizeEvent(QResizeEvent *);
        private slots:
            void save();

            void slotLoadProgress(int);
            void slotUpdateStatusbar(const QString &string);
            void slotUpdateWindowTitle(const QString &title = QString());

//            void loadUrl(const QUrl &url) = delete;
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
            void geometry_change_requested(const QRect &geometry);

            void update_bookmarks_toolbar_action_text(bool visible);

        private:
            void init();
                //        boost::intrusive_ptr<TreeItem> item_request_from_tree(QUrl const &url
                //            , const TreeScreen::paste_strategy &_view_paste_strategy
                //            , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
                //        );

                //        boost::intrusive_ptr<TreeItem> item_request_from_tree(boost::intrusive_ptr<TreeItem> item
                //            , const TreeScreen::paste_strategy &_view_paste_strategy
                //            , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
                //        );

            void run_script(const QString &style_source);
            void load_default_state();
            void append_to_file_menu();
            void append_edit_menu();
            void append_view_menu();
            void append_history_menu();
            void append_bookmark_menu();
            void append_window_menu();
            void append_to_tools_menu();
            void append_to_help_menu();
            void append_to_main_menu();
            void setup_tool_bar();
            void update_statusbar_action_text(bool visible);
            void handle_find_text_result(bool found);
                //    void initUrl();
        private:

            bool _is_under_construction = true;
            ts_t          *_tree_screen;
            FindScreen          *_find_screen;

            wn_t          *_main_window;

                // RecordController    *_record_controller;


                //        QDockWidget   *dock_widget;
                //        QToolBar      *navigater;
                //        ToolbarSearch       *_toolbarsearch;
            BookmarksToolBar    *_bookmarkstoolbar;
            ChaseWidget         *_chasewidget;

            AutoSaver           *_autosaver;

            QAction             *_historyhome;
            QAction             *_historyback;
            QMenu               *_historybackmenu;
            QAction             *_historyforward;
            QMenu               *_historyforwardmenu;
            QMenu               *_windowmenu;

            QAction             *_stop;
            QAction             *_reload;
            QAction             *_stopreload;
            QAction             *_viewmenubar;
            QAction             *_viewbookmarkbar;
            QAction             *_viewtoolbar;
            QAction             *_viewstatusbar;
            QAction             *_restorelastsession;
            QAction             *_addbookmark;

            QIcon _reloadicon;
            QIcon _stopicon;

            QString _lastsearch;

            QWebEngineSettings::WebAttribute _webattribute;
            bool _webattributeenabled;
            QWidget             *_centralwidget;
            QVBoxLayout         *_layout;

            rs_t        *_record_screen;
            TabWidget           *_tabmanager;
            Entrance            *_entrance;

            friend class QtSingleApplication;
            friend class Entrance;
            friend class WebView;
    };
}


QT_END_NAMESPACE


#endif	// BROWSERWINDOW_H



