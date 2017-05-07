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

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QClipboard>
#include <QIcon>
#include <QLineEdit>
#include <QNetworkReply>
#include <QWebEngineView>
#include <map>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "utility/lease.h"
// #include "models/record_table/Record.h"
// #include "models/tree/TreeItem.h"
#include "controllers/record_table/record_controller.h"
#include "libraries/global_parameters.h"
//#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "views/browser/browser.h"
#include "views/browser/featurepermissionbar.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
// #include "models/tree/TreeItem.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QAuthenticator;
class QMouseEvent;
class QNetworkProxy;
class QNetworkReply;
class QSslError;
QT_END_NAMESPACE

extern std::shared_ptr<gl_para> gl_paras;
//extern boost::intrusive_ptr<r_t> check_record(const QUrl& _url);

class r_t;
struct Binder;
class i_t;
class tm_t;
class ts_t;
class rctrl_t;
class EditorDock;
class Blogger;
class FindScreen;
// QT_BEGIN_NAMESPACE

// #define USE_POPUP_WINDOW

namespace web {
	class Browser;
	class PopupWindow;
	class WebView;
	class TabWidget;
	template <typename>
	class Docker;

#ifdef USE_POPUP_WINDOW
	class PopupView;
	class PopupPage : public QWebEnginePage {
#if QT_VERSION == 0x050600
		W_OBJECT(PopupPage)
#else
		Q_OBJECT
#endif
	    signals:

#if QT_VERSION == 0x050600
		void loadingUrl(const QUrl& url) W_SIGNAL(loadingUrl, (const QUrl&), url); //
#else
		void loadingUrl(const QUrl& url);

#endif
	    public:
		PopupPage(PopupView* view_, web::Browser* browser_, Profile* profile_);
		web::Browser* browser();

	    protected:
		bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
// QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type)
// Q_DECL_OVERRIDE;
#if !defined(QT_NO_UITOOLS)
		QObject* createPlugin(const QString& classId, const QUrl& url, const QStringList& paramNames, const QStringList& paramValues);
#endif
		virtual bool
		certificateError(const QWebEngineCertificateError& error) Q_DECL_OVERRIDE;

	    private slots:
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
		void handleUnsupportedContent(QNetworkReply* reply);
#endif
		void authenticationRequired(const QUrl& requestUrl, QAuthenticator* auth);
		void proxyAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth, const QString& proxyHost);

	    private:
		friend class PopupView;

		// set the webview mousepressedevent
		PopupView* _view;
		browser_ref _browser;
		Qt::KeyboardModifiers _keyboard_modifiers;
		Qt::MouseButtons _pressed_buttons;
		QUrl _loading_url;
		bool _certificate_ignored = false;
	};

#endif // USE_POPUP_WINDOW

	class WebPage : public QWebEnginePage {
#if QT_VERSION == 0x050600
		W_OBJECT(WebPage)
#else
		Q_OBJECT
#endif
	    signals:

#if QT_VERSION == 0x050600
		void loadingUrl(const QUrl& url) W_SIGNAL(loadingUrl, (const QUrl&), url); //
#else
		void loadingUrl(const QUrl& url);
#endif
// void linkHovered(const QString &url, int time_out = 0);

#if QT_VERSION == 0x050600
		void close_requested() W_SIGNAL(close_requested); //
#else
		void close_requested();
#endif

#if QT_VERSION == 0x050600

		void become_current(boost::intrusive_ptr<::Binder> _binder) W_SIGNAL(become_current, (boost::intrusive_ptr<::Binder>), _binder);
#else
		void become_current(boost::intrusive_ptr<::Binder> _binder);
#endif

	    public:
		// typedef Binder coupler_delegation;
		// typedef typename Binder::bind_interface bind_interface;
		// typedef typename Binder::activate_interface activate_interface;
		// typedef typename Binder::item_interface item_interface;
		// typedef typename Binder::page_interface page_interface;

		// typedef typename Binder::bind_helper bind_helper;
		// typedef typename Binder::activate_helper activate_helper;
		// typedef typename Binder::item_helper item_helper;
		// typedef typename Binder::page_helper page_helper;

	    public:
		WebPage(Profile* profile, boost::intrusive_ptr<i_t> item, ts_t* tree_screen,
		    Blogger* blogger_,
		    web::Docker<web::Browser>* browser_docker_, web::Browser* browser, web::TabWidget* tabmanager,
		    rctrl_t* rctrl_,
		    WebView* parent = 0);

		~WebPage();

		web::TabWidget* tabmanager();
		web::Browser* browser();
		WebView* view(); // {assert(_view); return _view;}

		rctrl_t* record_ctrl(); // {return _record_controller;}
		WebView* activate();
		WebView* load(boost::intrusive_ptr<i_t> item, bool switch_to = true, bool force_reload = false);

		void load(const QUrl& url) = delete;

		boost::intrusive_ptr<i_t>
		host() const; // {return _record_binder->bounded_item();}
		bool activated() const;
		struct Binder : public std::enable_shared_from_this<
				    Binder> { // boost::intrusive_ref_counter<Binder,
					      // boost::thread_safe_counter>    //
		    private:
			boost::intrusive_ptr<i_t> _host;
			WebPage* _page;

			// bool _make_current;
		    public:
			Binder(boost::intrusive_ptr<i_t> item_, WebPage* page_);
			~Binder();
			void host(boost::intrusive_ptr<i_t> it)
			{
				_host = it;
			}

			void page(WebPage* p)
			{
				_page = p;
			}

			boost::intrusive_ptr<i_t> host() const
			{
				return _host;
			}

			WebPage* page() const
			{
				return _page;
			}

			WebView*
			bind(); // , boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *)
			WebView* activator();
			static QString binder_type()
			{
				return "page_binder";
			}
		};

		//		void metaeditor_sychronize();

		boost::intrusive_ptr<i_t> bind(boost::intrusive_ptr<i_t> host_);

		boost::intrusive_ptr<::Binder> binder();
		//		const boost::intrusive_ptr<::Binder>&& binder() const;
		boost::intrusive_ptr<::Binder>
		binder(boost::intrusive_ptr<::Binder>&& binder_);

	    protected:
		// void setUrl(const QUrl &url);

		bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
		QWebEnginePage*
		createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if !defined(QT_NO_UITOOLS)
		QObject* createPlugin(const QString& classId, const QUrl& url, const QStringList& paramNames, const QStringList& paramValues);
#endif
		virtual bool
		certificateError(const QWebEngineCertificateError& error) Q_DECL_OVERRIDE;

		void record_info_update(const QUrl& url, const QString& title);
		void record_view_remove(boost::intrusive_ptr<i_t> host_);
		void binder_reset();
		std::function<void(const QUrl& url)> onUrlChanged;        // void onUrlChanged(const QUrl &url);
		std::function<void(const QString& title)> onTitleChanged; // void onTitleChanged(const QString &title);

	    private slots:

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
		void handleUnsupportedContent(QNetworkReply* reply);
#endif
		void authenticationRequired(const QUrl& requestUrl, QAuthenticator* auth);
		void proxyAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth, const QString& proxyHost);

	    private:
		Profile* _profile;
		ts_t* _tree_screen;
		blogger_ref _blogger;
		web::Docker<web::Browser>* _browser_docker;
		browser_ref _browser;
		tabwidget_ref _tab_widget;
		rctrl_ref _rctrl;

		WebView* _view;

		Qt::KeyboardModifiers _keyboardmodifiers = Qt::NoModifier;
		Qt::MouseButtons _pressedbuttons = Qt::NoButton;

		QUrl _loadingurl = QUrl();
		QString _hovered_url; // = Browser::_defaulthome;

		boost::intrusive_ptr<::Binder> _binder;

		bool _certificate_ignored = false;
		bool _activated = false;

		friend class TabWidget;
		friend class WebView;
		friend class r_t;
	};

#ifdef USE_POPUP_WINDOW

	class PopupView : public QWebEngineView {
#if QT_VERSION == 0x050600
		W_OBJECT(PopupView)
#else
		Q_OBJECT
#endif
	    public:
		PopupView(QWidget* parent = 0);
		PopupPage* webPage() const
		{
			return _page;
		}

		void setPage(PopupPage* page_);

		void loadUrl(const QUrl& url);
		QUrl url() const;
		QIcon icon() const;

		QString lastStatusBarText() const;
		inline int progress() const
		{
			return _progress;
		}

	    protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void contextMenuEvent(QContextMenuEvent* event);
		void wheelEvent(QWheelEvent* event);

	    signals:

#if QT_VERSION == 0x050600
		void iconChanged() W_SIGNAL(iconChanged); //
#else
		void iconChanged();
#endif
	    private slots:
		void setProgress(int progress);
		void loadFinished(bool success);
		void setStatusBarText(const QString& string);
		void openLinkInNewTab();
		void onFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature);
		void onIconUrlChanged(const QUrl& url);
		void iconLoaded();

	    private:
		QString _statusbar_text;
		QUrl _initial_url;
		int _progress;
		PopupPage* _page;
		QIcon _icon;
		QNetworkReply* _icon_reply;
	};

#endif // USE_POPUP_WINDOW

	class ToolbarSearch;
	// template Q_ONJECT
	// https://doc.qt.io/archives/qq/qq16-dynamicqobject.html
	// browserview
	// template<typename T>
	class WebView : public QWebEngineView {
#if QT_VERSION == 0x050600
		W_OBJECT(WebView)
#else
		Q_OBJECT
#endif

	    public:
		WebView(boost::intrusive_ptr<i_t> host_,
		    Profile* profile, // , bool openinnewtab
		    ts_t* tree_screen,
		    Blogger* blogger_,
		    web::Docker<web::Browser>* entrance,
		    Browser* browser,
		    web::TabWidget* tabmanager,
		    rctrl_t* rctrl_);

		~WebView();

		static WebView* instance(FindScreen *find_screen_, url_value const & real_url);
		WebPage* page() const;

		bool load_finished() const;
		void page(WebPage* page_);
		void activateWindow();

		WebView* load(boost::intrusive_ptr<i_t> it, bool switch_to = true, bool force_reload = false);
		QUrl url() const = delete;
		QIcon icon() const;

		QString lastStatusBarText() const;
		int progress() const;

		rctrl_t* record_ctrl(); // {return _record_controller;}
					//		void record_ctrl(rctrl_t* rctrl_); // {this->_record_controller =
		// _record_controller;}

		web::TabWidget* tabmanager() const;

		void current_view_global_consistency();

		ToolbarSearch *toolbarsearch() const;
		void toolbarsearch(ToolbarSearch* tbs);

		Browser *browser() const;
	protected:
		void loadUrl(const QUrl& url);

		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void contextMenuEvent(QContextMenuEvent* event);
		void wheelEvent(QWheelEvent* event);
		void focusOutEvent(QFocusEvent* event);
	    signals:

#if QT_VERSION == 0x050600
		void iconChanged() W_SIGNAL(iconChanged); //
#else
		void iconChanged();
#endif

#if QT_VERSION == 0x050600
		void close_requested() W_SIGNAL(close_requested); //
#else
		void close_requested();
#endif
	    public slots:
		// void loadFinished(bool success);
		// void openLinkInNewTab();
		void on_close_requested();
	    private slots:
		void setProgress(int progress);

		void setStatusBarText(const QString& string);
		void openLinkInNewTab();
		void openLinkInNewTab(const QUrl& url);
		void onFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature);
		void onIconUrlChanged(const QUrl& url);
		void iconLoaded();
		void onLoadFinished(bool success);

		// void onCloseTab(int index);
	    private:
		ToolbarSearch* _toolbarsearch;
		browser_ref _browser;
		tabwidget_ref _tab_widget;
		rctrl_ref _rctrl;

		WebPage* _page;
		QString _statusbartext;
		// QUrl _initialurl;
		int _progress;
		QIcon _icon;
		QNetworkReply* _icon_reply;
		bool _load_finished = false;
		//		QMetaObject::Connection _home_connection; // for disconnect
		friend class TabWidget;
		friend class WebPage;
	};

#ifdef USE_POPUP_WINDOW

	class PopupWindow : public QWidget {
#if QT_VERSION == 0x050600
		W_OBJECT(PopupWindow)
#else
		Q_OBJECT
#endif
	    public:
		PopupWindow(web::Browser* browser_, Profile* profile_);

		QWebEnginePage* page() const
		{
			return _view->page();
		}

	    private Q_SLOTS:
		void setUrl(const QUrl& url)
		{
			_addressbar->setText(url.toString());
		}

		void adjustGeometry(const QRect& newGeometry);

	    private:
		browser_ref _browser;
		QLineEdit* _addressbar;
		PopupView* _view;
	};

#endif // USE_POPUP_WINDOW
}

// QT_END_NAMESPACE

#endif
