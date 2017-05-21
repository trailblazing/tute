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

#include <tuple>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <QBoxLayout>
#include <QWebEngineSettings>
#include <QtCore/QUrl>
#include <QtGui/QIcon>
#include <QtWidgets/QMainWindow>

#include <QWebEngineProfile>
// #include <QtWebEngine/QQuickWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>

#include "controllers/record_table/record_controller.h"

// #include "models/tree/TreeItem.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "models/tree/tree_model.h"
#include "utility/lease.h"
#include "views/tree/tree_screen.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QWebEngineFrame;

QT_END_NAMESPACE

class HidableTab;
// extern Record *default_record;
class FingScreen;
class tm_t;
class ts_t;
class tsv_t;
struct RecordIndex;
class EditorDock;
class Blogger;

extern std::shared_ptr<AppConfig> appconfig;
extern const char* profile_storage_name;

// QT_BEGIN_NAMESPACE

namespace web {
	class AutoSaver;
	class BookmarksToolBar;
	class ChaseWidget;
	class TabWidget;
	class ToolbarSearch;
	class WebView;
	template <typename>
	class Docker;
	class HistoryMenu;
	class BookmarksMenu;

	class UrlRequestInterceptor
	    : // public boost::intrusive_ref_counter<UrlRequestInterceptor,
	      // boost::thread_safe_counter>,
	      public QWebEngineUrlRequestInterceptor {
	    public:
		UrlRequestInterceptor(QObject* p);
		virtual void interceptRequest(QWebEngineUrlRequestInfo& info);
	};

	class Profile : // public boost::intrusive_ref_counter<Profile,
			// boost::thread_safe_counter>,
			public QWebEngineProfile { // QQuickWebEngineProfile
	    public:
		Profile(const QString& name, QObject* p);
		~Profile();

	    private:
		UrlRequestInterceptor* _urlrequestinterceptor;
	};

	/*!
            The MainWindow of the Browser Application.

            Handles the tab widget and all the actions
 */
	class Browser : public QMainWindow
#ifdef USE_SIGNAL_CLOSE
			,
//			public boost::intrusive_ref_counter<Browser, boost::thread_safe_counter>,
			public sd::renter //<Browser>
#endif                                    //USE_SIGNAL_CLOSE

	{
#if QT_VERSION == 0x050600
		W_OBJECT(Browser)
#else
		Q_OBJECT
#endif
	    public:
		Browser(Blogger* blogger_, const QByteArray& state_ = QByteArray(), Qt::WindowFlags flags = Qt::MaximizeUsingFullscreenGeometryHint);

		// Browser(QUrl const &url         // Record *const record
		// , TreeScreen        *_tree_screen
		// , FindScreen        *_find_screen
		// , MetaEditor        *_editor_screen
		// , HidableTabWidget  *_vtabwidget
		// , MainWindow        *_main_window
		// , Entrance          *_entrance   //, QDockWidget *parent
		// , const QString     &style_source
		// , Qt::WindowFlags   flags = 0
		// );

		// Browser(const QByteArray    &state
		// , TreeScreen        *_tree_screen
		// , FindScreen        *_find_screen
		// , MetaEditor        *_editor_screen
		// , HidableTabWidget  *_vtabwidget
		// , MainWindow        *_main_window
		// , Entrance          *_entrance   //, QDockWidget *parent
		// , const QString     &style_source
		// , Qt::WindowFlags   flags = 0
		// );

		// Browser(boost::intrusive_ptr<TreeItem> item
		// , TreeScreen        *_tree_screen
		// , FindScreen        *_find_screen
		// , MetaEditor        *_editor_screen  //, RecordController
		// *record_controller
		// , HidableTabWidget  *_vtabwidget
		// , MainWindow        *_main_window
		// , Entrance          *_entrance   //, QDockWidget *parent
		// , const QString     &style_source
		// , Qt::WindowFlags   flags = 0
		// );

		~Browser();
		QSize sizeHint() const;

		static const url_value _defaulthome;
		template <typename T>
		struct initialize_prior_to {
			static constexpr bool value = true;
		};

	    public:
		// typedef Binder coupler_delegation;
		// typedef Binder::bind_interface          bind_interface;
		// typedef Binder::activate_interface      activate_interface;
		// typedef Binder::item_interface  bounded_item_interface;
		// typedef Binder::page_interface  bounded_page_interface;

		// typedef Binder::bind_helper         bind_helper;
		// typedef Binder::activate_helper     activate_helper;
		// typedef Binder::item_helper bounded_item_helper;
		// typedef Binder::page_helper bounded_page_helper;

		web::TabWidget* tab_widget();
		web::TabWidget* tab_widget() const;
		//		sd::intrusive_ptr<TabWidget> tabWidget();
		//		sd::intrusive_ptr<TabWidget> tabWidget() const;

		WebView* currentTab() const;
		// move into save()
		//		QByteArray get_state(bool withTabs = true) const;

		// move into load_state()
		//		bool restore_state(const QByteArray& state_);
		QAction* action_restore_last_session();
		// Q_INVOKABLE
		void runScriptOnOpenViews(const QString&);
		void setWebAttribute(QWebEngineSettings::WebAttribute attribute, bool enabled);
		QString& lastsearch();
		const QString& lastsearch() const;

		void activateWindow();
		bool is_under_construction() const;
		QAction* historyback();
		QStatusBar* statusBar() = delete;
		// QStatusBar *status_bar();
		// QStatusBar *status_bar() const;
		boost::intrusive_ptr<i_t> bind(boost::intrusive_ptr<RecordIndex> record_index, bool make_current = true);
		web::WebView* find(const std::function<bool(boost::intrusive_ptr<const ::Binder>)>& _equal) const;
		// boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<TreeItem>
		// item);
		//#ifdef USE_SIGNAL_CLOSE
		//		sd::intrusive_ptr<rs_t>
		//#else
		rs_t*
		    //#endif //USE_SIGNAL_CLOSE
		    record_screen();
		//		web::Docker *browser_docker();
		void init_configuration(std::unique_ptr<QSettings>&& conf);
		void configuration_full_name(const QString& conf_name);
		static std::tuple<qint32, qint32, QString, QString, QSize, bool, bool, bool, bool, std::tuple<qint32, qint32, QStringList, QString>> state(const QByteArray& state_);
		Blogger* blogger();
		//		void test();

		QToolBar* navigater() const;
	    public slots:
		// void loadPage(const QString &url) = delete;
		void slotHome();
		void updateToolbarActionText(
		    bool visible); // void updateToolbarActionText(bool visible);

		//		void on_blogger_close_requested();
		//		void on_record_screen_close_requested();
		void save();

		bool restore_state(const QString& new_topic_);
		void on_topic_changed(const QString& original_topic_, const QString& topic_, bool append_mode = false);
		void init_main_menu();
		//		void reset_find_screen_navigater();

		bool restore_state(const QByteArray& state);

	    protected:
		void closeEvent(QCloseEvent* event);
		void resizeEvent(QResizeEvent*);
		void focusOutEvent(QFocusEvent* event);
		void showEvent(QShowEvent* event);
	    private slots:


		void slotLoadProgress(int);
		void slotUpdateStatusbar(const QString& string);
		//		void slotUpdateWindowTitle(const QString &title = QString());

		// void loadUrl(const QUrl &url) = delete;
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
		void slotOpenActionUrl(QAction* action);
		void slotShowWindow();
		void slotSwapFocus();

#if defined(QWEBENGINEPAGE_PRINT)
		void printRequested(QWebEngineFrame* frame);
#endif
		void geometry_change_requested(const QRect& geometry);

		void update_bookmarks_toolbar_action_text(bool visible);

	    private:
		void init();
		// boost::intrusive_ptr<TreeItem> item_request_from_tree(QUrl const &url
		// , const TreeScreen::paste_strategy &_view_paste_strategy
		// , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const
		// QUrl &_url)->bool {return it->field("url") == _url.toString();}
		// );

		// boost::intrusive_ptr<TreeItem>
		// item_request_from_tree(boost::intrusive_ptr<TreeItem> item
		// , const TreeScreen::paste_strategy &_view_paste_strategy
		// , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it,
		// boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() ==
		// target->id();}
		// );

		void run_script(const QString& style_source);
		void load_state();

		void synchronize_find_screen_reset_main_menu();

		void update_statusbar_action_text(bool visible);
		void handle_find_text_result(bool found);
		// void initUrl();
	    public:
		static const qint32 browser_magic = 0xba;

	    private:
		blogger_ref _blogger;
		QString _configuration_full_name = "";
		std::unique_ptr<QSettings> _configuration;
		QByteArray _state;
		bool _is_under_construction = true;
		HidableTab* _vtab_record;
		ts_t* _tree_screen;
		FindScreen* _find_screen;
		wn_t* _main_window;

		// RecordController    *_record_controller;

		// QDockWidget   *dock_widget;
		QToolBar* _navigater;
		// ToolbarSearch       *_toolbarsearch;
		BookmarksToolBar* _bookmarkstoolbar;

		QMenu* _file_menu;
		QMenu* _edit_menu;
		QMenu* _view_menu;
		//		QMenu *_help_menu;
		QMenu* _tools_menu;
		BookmarksMenu* _bookmarks_menu;
#ifdef USE_CLEAR_BUTTON
		ChaseWidget* _chasewidget;
#endif // USE_CLEAR_BUTTON
		HistoryMenu* _history_menu;
		QAction* _historyhome;
		QAction* _historyback;
		QAction* _historyforward;

		QMenu* _historybackmenu;
		QMenu* _historyforwardmenu;
		QMenu* _windowmenu;

		QAction* _stop;
		QAction* _reload;
		QAction* _stopreload;

		AutoSaver* _autosaver;
		QAction* _viewmenubar;
		QAction* _viewbookmarkbar;
		QAction* _viewtoolbar;
		QAction* _viewstatusbar;
		QAction* _restorelastsession;
		QAction* _addbookmark;
		QIcon _reloadicon;
		QIcon _stopicon;
		QString _lastsearch;

		QWebEngineSettings::WebAttribute _webattribute;
		bool _webattributeenabled;
		QWidget* _centralwidget;
		QVBoxLayout* _layout;


		//		sd::intrusive_ptr<rs_t> _record_screen;
		tabwidget_ref _tab_widget;


		web::Docker<web::Browser>* _browser_docker;

		//		bool _closed = false;
		friend class sapp_t; // QtSingleApplication;

		friend class web::Docker<web::Browser>;
		friend class WebView;
		friend class ::Blogger;
		void init_history_menu();

		void init_file_menu();
		void init_edit_menu();
		void init_view_menu();
		//		void init_help_menu();
		void init_tools_menu();
		void init_bookmark_menu();
		void init_window_menu();
		void setup_navigate();
	    signals:
		//		void close_request(QWidget*);


	    protected:
#ifndef USE_SIGNAL_CLOSE
		boost::signals2::signal<void(Browser*)> _prepended;
#endif // NO USE_SIGNAL_CLOSE
	};

	template <>
	struct Browser::initialize_prior_to<Blogger> {
		static constexpr bool value = false;
	};
}

// QT_END_NAMESPACE

#endif // BROWSERWINDOW_H
