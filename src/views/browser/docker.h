#ifndef _DOCKER_H_
#define _DOCKER_H_

#include <set>

#include "libraries/global_parameters.h"
//#include "libraries/qtm/blogger.h"
#include "utility/delegate.h"
#include "utility/lease.h"
//#include "views/browser/browser.h"
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

//#if QT_VERSION == 0x050600
//#include <QObject>
//#include <wobjectdefs.h>
//#endif

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
class r_t;
class i_t;
class gl_para;
class Blogger;
class EditorDock;

extern std::shared_ptr<gl_para> gl_paras;

namespace web {
	class WebView;
	class Browser;
	class ToolbarSearch;
}


// QT_BEGIN_NAMESPACE

namespace web {
	// Architecture:
	// Entrance(<=>DockWidget<=>Docker<=>DockedWindow(s))
	// |
	// |____Browser(s),Blogger(s)
	// |
	// |____TabWidget
	// |
	// |____WebView(s)
	// |
	// |____WebPage


	// entrance.ini
	template <typename real_t>
	class Docker : public QDockWidget {
#ifdef USE_MOC
#if QT_VERSION == 0x050600
		W_OBJECT(Entrance)
#else
		Q_OBJECT
#endif
#endif //USE_MOC
	    public:
		Docker(QString object_name, wn_t* main_window, Qt::WindowFlags flags = 0);

		~Docker();

		template <typename T>
		struct initialize_prior_to {
			static constexpr bool value = true;
		};


		void set_scrollbars(bool hide);
#ifdef USE_MOC
		void remove_child(real_reference cl);
#endif //USE_MOC
		//		template <typename real_t>
		Docker* prepend(real_t* child_, Qt::WindowFlags flags = Qt::Widget);
		//                {
		//                        child_->setParent(this, flags);
		//                        setWidget(child_);
		//                        child_->show();
		//                        //			resize_event = std::bind(&real_t::resizeEvent, browser,
		//                        //std::placeholders::_1);
		//                        _children_resize_event[child_] =
		//                            std::bind(&real_t::resizeEvent, child_, std::placeholders::_1);
		//                        connect(child_, &real_t::close_request, this, &Docker::remove_child);
		//                        return this;
		//                }

	    public: //slots
		    //		void on_activate_window();
		    // void on_splitter_moved(int pos, int index);

		//#if defined(Q_OS_OSX)
		//		void lastWindowClosed();
		//#endif


	    protected:
		void init_setting(void);
		void resizeEvent(QResizeEvent* e);

	    private:
// void(Entrance::*invoke_ptr)(const QUrl &url);
//		std::function<void (QResizeEvent *)> resize_event;
#ifdef USE_MOC
		std::map<real_reference, std::function<void(QResizeEvent*)>> _children_resize_event;
#endif //USE_MOC
		QWidget* _hidetitlebar;
		//		QMetaObject::Connection _home_connection; // for disconnect
		friend class sapp_t;
	};
	//
	template <>
	template <>
	struct Docker<web::Browser>::initialize_prior_to<wn_t> {
		static constexpr bool value = false;
	};
	template <>
	template <>
	struct Docker<Blogger>::initialize_prior_to<wn_t> {
		static constexpr bool value = false;
	};
	template <>
	template <>
	struct Docker<web::Browser>::initialize_prior_to<sapp_t> {
		static constexpr bool value = false;
	};
	template <>
	template <>
	struct Docker<Blogger>::initialize_prior_to<sapp_t> {
		static constexpr bool value = false;
	};
	//

	extern template class Docker<Blogger>;
	extern template class Docker<web::Browser>;
	//	extern template Docker<Blogger>* Docker<Blogger>::prepend(sd::intrusive_ptr<Blogger> child_, Qt::WindowFlags flags = Qt::Widget);
	//	extern template Docker<web::Browser>* Docker<web::Browser>::prepend(sd::intrusive_ptr<web::Browser> child_, Qt::WindowFlags flags = Qt::Widget);
}

// QT_END_NAMESPACE

#endif // _DOCKER_H_
