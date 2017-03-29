
#include <utility>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QWebEngineView> // #include <QWebView> //#include <QtWebKitWidgets/QWebView>

// #include <QWebElement>
#include <QWebEnginePage> // #include <QWebPage>
#include <QtWebEngineWidgets>

// #include <QWebFrame>
#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QSettings>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslError>
#include <QUrl>
#include <QVBoxLayout>
#include <cassert>

#ifdef Q_WS_QWS
#include <QWSServer>
#endif

//#include "models/tree/tree_item.dec"

#include "browser.h"
#include "controllers/record_table/record_controller.h"
#include "docker.h"
#include "main.h"
//#include "entranceinfo.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/blogger.h"
#include "libraries/window_switcher.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "tabwidget.h"
#include "toolbar_search.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "webview.h"

namespace web {
	// struct BrowserViewPrivate {
	////Q_OBJECT
	// public:
	// QWebEngineView *view;
	// QNetworkAccessManager *nam;
	// QMap<QPair<QString, int>, QByteArray> exceptions;
	// bool msgShown;
	////~BrowserViewPrivate() = default;
	// };


	//#if QT_VERSION == 0x050600
	//	W_OBJECT_IMPL(Entrance)
	//#endif


	template <typename real_t>
	Docker<real_t>::Docker(QString object_name, wn_t* main_window, Qt::WindowFlags flags)
	    : QDockWidget(main_window, flags)
	    , _hidetitlebar(new QWidget(this, Qt::FramelessWindowHint | Qt::CustomizeWindowHint))
	{
		// | Qt::SplashScreen
		// , _dockwidget(new DockWidget(
		// this
		// , _mainWindows[0].data()
		// ,  flags  //Qt::Widget   //Qt::WindowMaximizeButtonHint
		// //Qt::MaximizeUsingFullscreenGeometryHint
		// )
		// )
		// , _browser(new
		// DockedWindow(register_record(QUrl(DockedWindow::_defaulthome))
		// , recordtablecontroller
		// , this, style_source, flags    //Qt::Widget
		// //Qt::WindowMaximizeButtonHint  // Qt::MaximizeUsingFullscreenGeometryHint
		// ))

		setObjectName(object_name);
		// invoke_ptr = &Entrance::active_url;

		// _mainWindows.prepend(browser);

		// d->view = new QWebEngineView(this);
		// d->view->page()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
		// d->view->page()->setScrollBarPolicy(Qt::Horizontal,
		// Qt::ScrollBarAlwaysOff);

		// browser->setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
		// browser->setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows,
		// true);

		// browser->msgShown = false;

		// QVBoxLayout *layout = new QVBoxLayout(this);
		// QDockWidget *dock_widget = new QDockWidget(this,
		// Qt::MaximizeUsingFullscreenGeometryHint);
		// when change dock_widget to member variable, you need rebuild the project!
		// else the behavior will be weird.
		// layout->setMargin(0);

		// _dockwidget->setWidget(_mainWindows[0].data());
		////    //    layout->addWidget(browser);
		////    //    browser->setLayout(layout);
		// _mainWindows[0]->setParent(_dockwidget);

		setWindowFlags( // Qt::Window |
		    Qt::FramelessWindowHint
		    // |Qt::Popup
		    | Qt::CustomizeWindowHint
		    // | Qt::SplashScreen  //
		    // http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
		    // | Qt::DockWidgetArea::NoDockWidgetArea
		    | Qt::MaximizeUsingFullscreenGeometryHint);

		setAutoFillBackground(true);
		adjustSize();

		setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar
		    // | Qt::DockWidgetArea::NoDockWidgetArea
		    // | Qt::MaximizeUsingFullscreenGeometryHint
		    ); // AllDockWidgetFeatures

		// this->titleBarWidget()->hide();

		QWidget* titleBar = titleBarWidget();

		// QVBoxLayout *main = new QVBoxLayout;
		////main -> addLayout(0);
		////main -> addLayout(0);
		// main->setGeometry(QRect());
		// main->setSizeConstraint(QLayout::SetMinimumSize);
		////setWindowTitle(tr("Find"));
		////setLayout(main);

		// _hidetitlebar->setMinimumWidth(0);
		// _hidetitlebar->setMaximumWidth(0);
		// _hidetitlebar->setMinimumHeight(0);
		// _hidetitlebar->setMaximumHeight(
		// 0
		//// parent->height() // for QDockWidget::DockWidgetVerticalTitleBar
		// );
		// _hidetitlebar->setMinimumSize(QSize(0, 0));
		// _hidetitlebar->resize(QSize(0, 0));

		// _hidetitlebar->setLayout(main);

		setTitleBarWidget(_hidetitlebar);
		_hidetitlebar->setGeometry(0, 0, 0, 0);
		// _hidetitlebar->hide();
		_hidetitlebar->setVisible(false);

		// _hidetitlebar->setMaximumWidth(0);
		_hidetitlebar->close();
		// _hidetitlebar->setCollapsible(true);
		// _hidetitlebar->setMaximumHeight();
		// _hidetitlebar->setMaximumSize(0, 0);

		delete titleBar;

		//		setup_actions();

		//		setup_ui();
		//		assembly();

		init_setting();

		// setup_signals(_find_screen->toolbarsearch());

		// new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));  //
		// main_window() will never fail

		// d->nam = d->view->page()->networkAccessManager();

		// connect(d->nam, SIGNAL(finished(QNetworkReply *)), this,
		// SLOT(finished(QNetworkReply *)));

		// connect(d->nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>
		// &)), this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));

		// browser->show();
	}

	template <typename real_t>
	Docker<real_t>::~Docker()
	{
		// if(_browsers.size() > 0){
		// for(auto i = _browsers.begin(); i != _browsers.end(); i ++){
		// if(*i && *i != widget()){
		//// _browsers.erase(i);
		// (*i)->deleteLater();// delete *i;
		//// *i = nullptr;
		// }
		// }
		// }
		//// if(isselfcreated())delete current_record;   // no, do not apply memory by
		///this class for record, from the original source
		//// if(_actionFreeze)delete _actionFreeze;
		//// if(_dockwidget)delete _dockwidget;
		//// if(browser)delete browser;  // I can't destroy?
		//// delete _shadow_branch;
		if (_hidetitlebar) {
			delete _hidetitlebar;
			_hidetitlebar = nullptr;
		}
	}

	template <typename real_t>
	void Docker<real_t>::set_scrollbars(bool hide)
	{
		if (!hide) {
			// d->view->page()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
			// d->view->page()->setScrollBarPolicy(Qt::Horizontal,
			// Qt::ScrollBarAsNeeded);
		}
	}


	template <typename real_t>
	void Docker<real_t>::init_setting()
	{
		// QUrl _url;

		// if(record)
		// _url = record->getField("url");

		QUrl url;
		// initialize

		QFileInfo settingsFile;

		// QString sb(QApplication::applicationDirPath());
		// sb.append(QDir::separator());
		// sb.append("browserview.ini");

		QString t = gl_paras->root_path() + "/" + gl_paras->target_os() + "/" +
		    gl_para::_dock_conf_filename;
		// check to see if we have a settings file where we started from
		// if not fall back to system hard coded path
		QFileInfo file(t.toLatin1()); // file(sb.toLatin1());
		if (file.exists()) {
			qDebug() << "using local settings file";
			settingsFile.setFile(file.filePath());
		} else {
			qDebug() << "using system defined settings file";

			auto s = QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + gl_para::_dock_conf_filename;
			// if(!QFile::copy(s, t)) critical_error("Can not copy mode.ini"); // throw
			// std::runtime_error("Can not copy mode.ini");
			// else QFile::setPermissions(_standard_path + "/" + _mode_filename,
			// QFile::ReadUser | QFile::WriteUser);
			if (!DiskHelper::copy_file_force(s, t)) critical_error("Unhandled error encountered when force copy file \"" + s + "\" to \"" + t + "\"");
			settingsFile.setFile(t);
		}
		QSettings settings(settingsFile.filePath(), QSettings::NativeFormat);

		settings.beginGroup(gl_para::_dock_settings_section_name);

		// if(_url.isEmpty()) {
		url =
		    (settings.value("browser_view")
			    .toString()); // QUrl url(settings.value("browser_view").toString());
		gl_paras->global_home(url);
		// } else {
		// url = _url;
		// }


		qDebug() << "loading url: " << settings.value("main_view").toString();
		if (settings.value("full_screen", false).toBool()) this->showFullScreen();
		if (settings.value("hide_cursor", false).toBool()) {
			this->setCursor(QCursor(Qt::BlankCursor));
#ifdef Q_WS_QWS
			QWSServer::setCursorVisible(false);
#endif
		}
		this->set_scrollbars(settings.value("hide_scrollbars", false).toBool());
		//		this->set_cache(settings.value("enable_cache", false).toBool(),
		//settings.value("cache_size_megabytes", 5).toInt());

		settings.endGroup();

		// this->setMinimumSize(QSize(0, 0));
		// setMinimumHeight(0);

		this->show();
	}

	template <typename real_t>
	void Docker<real_t>::resizeEvent(QResizeEvent* e)
	{
		auto win = this->widget();
		if (win) {
			//#ifdef USE_SIGNAL_CLOSE
			//			typedef typename real_t::element_type real_t;
			//#else
			//			typedef typename std::remove_pointer<real_t>::type real_t;
			//#endif
			auto real_win = static_cast<real_t*>(win);
			if (real_win) real_win->resizeEvent(e);
#ifdef USE_MOC
			for (auto el : _children_resize_event) el.second(e);
#endif //USE_MOC
		}
		QDockWidget::resizeEvent(e);
	}
#ifdef USE_MOC
	template <typename real_t>
	void Docker<real_t>::remove_child(real_t cl)
	{
		_children_resize_event.erase(cl);
	}
#endif //USE_MOC
	template <typename real_t>
	Docker<real_t>* Docker<real_t>::prepend(real_t* child_, Qt::WindowFlags flags)
	{
		auto win = this->widget();
		if (win) {
			win->setParent(nullptr, flags);
			win->hide();
		}
		child_->setParent(this, flags);
		setWidget(child_);
		child_->show();
//			resize_event = std::bind(&real_t::resizeEvent, browser, std::placeholders::_1);
#ifdef USE_MOC
#ifdef USE_SIGNAL_CLOSE
		typedef typename real_t::element_type real_t;
#else
		typedef typename std::remove_pointer<real_t>::type real_t;
#endif
		_children_resize_event[child_] = std::bind(&real_t::resizeEvent, child_, std::placeholders::_1);
#ifdef USE_SIGNAL_CLOSE
		child_->close_connect([=](sd::renter* const r) {
			(void)r;
			for (auto it : _children_resize_event) {
				if (it.first == child_) this->remove_child(child_);
			}
		}); //std::make_unique<sd::method<sd::meta_info<void>>>("", &Docker::remove_child, this, child_)
#else

#endif //USE_SIGNAL_CLOSE
#endif //USE_MOC

		return this;
	}

	template class Docker<Blogger>;
	template class Docker<web::Browser>;
	//	template Docker<Blogger>* Docker<Blogger>::prepend(sd::intrusive_ptr<Blogger> child_, Qt::WindowFlags flags = Qt::Widget);
	//	template Docker<web::Browser>* Docker<web::Browser>::prepend(sd::intrusive_ptr<web::Browser> child_, Qt::WindowFlags flags = Qt::Widget);
}
