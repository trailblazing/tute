#ifndef _BROWSERMANAGER_H_
#define _BROWSERMANAGER_H_

#include <set>

#include "utility/delegate.h"
#include <QDockWidget>
#include <QList>
#include <QUrl>
#include <QWidget>
// #include "models/record_table/Record.h"
// #include "libraries/GlobalParameters.h"
// #include "browser.h"
// #include "webview.h"
// #include "views/record/MetaEditor.h"
// #include "models/tree/TreeItem.h"

//#include "views/browser/tabwidget.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class tkm_t;
class QNetworkReply;
class QSslError;
class sapp_t;
class rctrl_t;
class HidableTab;
class ts_t;
class FindScreen;
class EditorWrap;
class wn_t;
class AppConfig;
class Record;
class TreeItem;
class gl_para;
class Blogger;
class EditorDock;

extern std::shared_ptr<gl_para> gl_paras;

namespace web {
	class WebView;
	class Browser;
	class ToolbarSearch;
}



// extern boost::intrusive_ptr<Record> request_record(const QUrl &_url
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, web::WebView *, boost::intrusive_ptr<Record>>>
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, web::WebView *, boost::intrusive_ptr<Record>>>
// );

// QT_BEGIN_NAMESPACE

namespace web {
// Architecture:
// Entrance(<=>DockWidget)__DockedWindow(s)
// |
// |____Browser(s)
// |
// |____TabWidget
// |
// |____WebView(s)
// |
// |____WebPage

// class DockWidget : public QDockWidget {
// public:
// explicit DockWidget(QWidget *parent, DockedWindow *browser, Qt::WindowFlags flags = Qt::WindowFlags());
// };

// entrance.ini

	class Docker : public QDockWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(Entrance)
#else
	Q_OBJECT
#endif

	public:
		Docker(QString object_name
		      , wn_t *main_window                     // , std::shared_ptr<AppConfig> appconfig_
		      , Qt::WindowFlags flags = 0);
		~Docker();

		template<typename T> struct initialize_prior_to {static constexpr bool value = true;};

		// BrowserWindow *getBrowserWindow() {return mainWindow();}
		// void setupDynamicSignals(void);

		// std::pair<Browser *, WebView *> invoke_page(Record *const record);  //= register_record(QUrl(DockedWindow::_defaulthome))

		// WebView *active_record_alternative(Record *const record) ;

		// struct ActiveRecordBinder {
		// Entrance    *_the;
		////            WebPage     *_page;
		// ActiveRecordBinder(Entrance *the): _the(the)    //              , _page(nullptr)
		// {}

		// WebView *binder(boost::intrusive_ptr<TreeItem> item, boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *))
		// {
		////                _page = _the->equip_registered(record).second->page();
		// WebView *view = _the->equip_registered(item).second;
		// (item.get()->*_bind)(view->page());
		// return view;
		// }

		// WebView *activator(boost::intrusive_ptr<TreeItem> item) {assert(item);assert(item->page_valid()); return item->unique_page()->active();}
		// };

		// Browser		*activated_browser();
		// std::set<Browser *> &browsers();	//        QList<DockedWindow*> window_raw_list();

		////        void clean();

		////        std::pair<DockedWindow *, WebView *> active_record(Record *const record);

		// boost::intrusive_ptr<TreeItem> item_bind(RecordModel::ModelIndex modelindex);    // = boost::intrusive_ptr<Record>(nullptr)

		////        //        template<typename url_type>
		////        void activate(const QUrl &_find_url
		////                      , const TreeScreen::paste_strategy &_view_paste_strategy
		////                      , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
		////                     );

		////        void activate(boost::intrusive_ptr<TreeItem> item
		////                      , const TreeScreen::paste_strategy &_view_paste_strategy
		////                      , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
		////                     );

		// bool restore_state(const QByteArray &state);
		// std::pair<Browser *, WebView *>
		// WebView *find(boost::intrusive_ptr<const TreeItem> item) const;
		// std::pair<Browser *, WebView *>
		// WebView *find(const std::function<bool (boost::intrusive_ptr<const ::Binder>)> &_equal) const;
		// BrowserView *create_view(Record *record, BrowserWindow *window);

		// Q_INVOKABLE void runScriptOnOpenViews(const QString &);
//		void style_source(const QString &style_source_);
//		QString style_source();
		// ItemsFlat const *shadow_branch()const {return _shadow_branch;}

		// WebView *find(const std::function<bool(boost::intrusive_ptr<const TreeItem>)> &_equal) const;
		// boost::intrusive_ptr<const TreeItem> is_registered_to_browser(const std::function<bool(boost::intrusive_ptr<const TreeItem>)> &_equal) const;
		template<typename real_t>
		Docker *prepend(real_t *browser){
			browser->setParent(this);
			setWidget(browser);
			browser->show();
//			resize_event = std::bind(&real_t::resizeEvent, browser, std::placeholders::_1);
			_children_resize_event[browser] = std::bind(&real_t::resizeEvent, browser, std::placeholders::_1);
			connect(browser, &real_t::close_request, this, &Docker::remove_child);
			return this;
		}

	public slots:

		void init_setting(void);

		void set_scrollbars(bool hide);
		void remove_child(QWidget *cl);
//		void set_cache(bool cache, int cache_size);
//		void finished(QNetworkReply *reply);
//		void ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors);
		// QAction *getactionFreeze() { return _actionFreeze; }
		// void openLinkInNewTab();
		// void onLoadFinished(bool);
//		void setup_actions(void);

		// WebView *new_dockedwindow(Record *const record);
		// WebView *new_view(QUrl const &_url);

		// Browser *new_browser();
		// Browser *new_browser(QUrl const &url);

		// Browser *new_browser(boost::intrusive_ptr<TreeItem> record);
		// Browser *new_browser(const QByteArray &state);
		// Browser *activate_browser(boost::intrusive_ptr<TreeItem> item);
//		template<typename return_t, typename child_t>
//		Docker *prepend(child_t *){
//			browser->setParent(this);
//			setWidget(browser);
//			browser->show();

//			return this;
//		}
//		void on_activate_window();
// void on_splitter_moved(int pos, int index);

#if defined(Q_OS_OSX)
		void lastWindowClosed();
#endif

	private slots:
		// void loadUrl(Record *record);
		// void loadUrl(const int pos);
		// void open_url(const QUrl &url);
		// void setUrl(const QUrl &url);
	protected:
		void resizeEvent(QResizeEvent *);

	private:
		// void(Entrance::*invoke_ptr)(const QUrl &url);
//		std::function<void (QResizeEvent *)> resize_event;
		std::map<QWidget *, std::function<void (QResizeEvent *)> > _children_resize_event;
//		void setup_ui(void);
		// void setup_signals(ToolbarSearch *toolbarsearch);
//		void assembly(void);
		// void setupMenu();
		// std::set<Browser *>             _browsers;
		// ItemsFlat                       *_shadow_branch;
//		ts_t *_tree_screen;
//		FindScreen *_find_screen;
//		EditorDock *_editor_docker;
		// HidableTabWidget                *_vtab_tree;
//		wn_t *_main_window;
		// std::shared_ptr<AppConfig> _appconfig;
		// RecordController                *_record_controller;
//		QString _style_source;
//		web::Profile *_profile;
		// void urlChanged(const QUrl &_url){onUrlChanged(_url);}
		// QAction                         *_actionFreeze;
		// struct BrowserViewPrivate *d;
		// QDockWidget *_dockwidget;
		// DockedWindow *_browser;
		QWidget *_hidetitlebar;
		QMetaObject::Connection _home_connection; // for disconnect
		friend class sapp_t;
	};
	//
	template<> struct Docker::initialize_prior_to<wn_t>{static constexpr bool value = false;};
	template<> struct Docker::initialize_prior_to<sapp_t>{static constexpr bool value = false;};
	//
	template<typename child_t>
	class DockerImpl : public Docker {
	public:
		DockerImpl(QString object_name
			  , wn_t *main_window                     // , std::shared_ptr<AppConfig> appconfig_
			  , Qt::WindowFlags flags = 0) : Docker(object_name, main_window, flags){}

		DockerImpl *prepend(child_t *browser){
			browser->setParent(this);
			setWidget(browser);
			browser->show();

			return this;
		}
	};
//	template<>
//	DockerSignal *DockerSignal::prepend<DockerSignal, Browser>(Browser *);
//	template<>
//	EditorDock *DockerSignal::prepend<EditorDock, Blog>(Blog *);
}

// QT_END_NAMESPACE

#endif // _BROWSERMANAGER_H_
