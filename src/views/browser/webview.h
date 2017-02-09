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

// #include "models/record_table/Record.h"
// #include "models/tree/TreeItem.h"
#include "controllers/record_table/record_controller.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "views/browser/featurepermissionbar.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
// #include "models/tree/TreeItem.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


QT_BEGIN_NAMESPACE
class QAuthenticator;
class QMouseEvent;
class QNetworkProxy;
class QNetworkReply;
class QSslError;
QT_END_NAMESPACE

extern gl_para				globalparameters;
extern boost::intrusive_ptr<Record>	check_record(const QUrl &_url);

class Record;
struct Binder;
class TreeItem;
class tm_t;
class ts_t;
class rctrl_t;
class Editentry;

// QT_BEGIN_NAMESPACE

// #define USE_POPUP_WINDOW

namespace browser {
    class Browser;
    class PopupWindow;
    class WebView;
    class TabWidget;

#ifdef USE_POPUP_WINDOW
    class PopupView;
    class PopupPage : public QWebEnginePage {
#if QT_VERSION == 0x050600
	W_OBJECT(PopupPage)
#else
	Q_OBJECT
#endif
	signals:
	    void loadingUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(loadingUrl, (const QUrl &), url)	//
#else
	    ;

#endif
	public:
	    PopupPage(PopupView *view_, Browser  *browser_, Profile *profile_);
	    Browser *browser();

	protected:
	    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
//        QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type)
//        Q_DECL_OVERRIDE;
#if ! defined(QT_NO_UITOOLS)
	    QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
#endif
	    virtual bool certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;

	private slots:
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
	    void handleUnsupportedContent(QNetworkReply *reply);
#endif
	    void authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
	    void proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);

	private:
	    friend class PopupView;

// set the webview mousepressedevent
	    PopupView			*_view;
	    Browser			*_browser;
	    Qt::KeyboardModifiers _keyboard_modifiers;
	    Qt::MouseButtons _pressed_buttons;
	    QUrl _loading_url;
	    bool _certificate_ignored = false;
    };

#endif	// USE_POPUP_WINDOW

    class WebPage : public QWebEnginePage {
#if QT_VERSION == 0x050600
	W_OBJECT(WebPage)
#else
	Q_OBJECT
#endif
	signals:
	    void loadingUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(loadingUrl, (const QUrl &), url)	//
#else
	    ;
#endif
//        void linkHovered(const QString &url, int time_out = 0);
	    void close_requested()
#if QT_VERSION == 0x050600
	    W_SIGNAL(close_requested)	//
#else
	    ;
#endif

	public:
//        typedef Binder coupler_delegation;
//        typedef typename Binder::bind_interface      bind_interface;
//        typedef typename Binder::activate_interface  activate_interface;
//        typedef typename Binder::item_interface      item_interface;
//        typedef typename Binder::page_interface      page_interface;

//        typedef typename Binder::bind_helper     bind_helper;
//        typedef typename Binder::activate_helper activate_helper;
//        typedef typename Binder::item_helper     item_helper;
//        typedef typename Binder::page_helper     page_helper;

	    WebPage(Profile *profile
		   , boost::intrusive_ptr<TreeItem> item
		   , ts_t *tree_screen
		   , Editentry *editentry
		   , Entrance *entrance
		   , Browser *browser
		   , TabWidget *tabmanager
		   , rctrl_t *record_controller
		   , WebView *parent = 0);

	    ~WebPage();

	    TabWidget	*tabmanager();
	    Browser	*browser();
	    WebView	*view();		// {assert(_view); return _view;}


	    rctrl_t	*record_controller();		// {return _record_controller;}
	    WebView	*activate();
	    WebView	*load(boost::intrusive_ptr<TreeItem> item, bool checked = true);


	    void load(const QUrl &url) = delete;

	    boost::intrusive_ptr<TreeItem>	host() const;	// {return _record_binder->bounded_item();}

	    struct Binder : public std::enable_shared_from_this <Binder> {		// boost::intrusive_ref_counter<Binder, boost::thread_safe_counter>    //
		private:
		    boost::intrusive_ptr<TreeItem>	_host;
		    WebPage				*_page;

			//            bool _make_current;
		public:
		    Binder(boost::intrusive_ptr<TreeItem> item_, WebPage *page_);
		    ~Binder();
		    void host(boost::intrusive_ptr<TreeItem> it){_host = it;}

		    void page(WebPage *p){_page = p;}

		    boost::intrusive_ptr<TreeItem> host() const {return _host;}

		    WebPage *page() const {return _page;}

		    WebView	*bind();		// , boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *)
		    WebView	*activator();
		    static QString binder_type(){return "page_binder";}
	    };


	    void metaeditor_sychronize();


	    boost::intrusive_ptr<TreeItem> bind(boost::intrusive_ptr<TreeItem> host_);

	    boost::intrusive_ptr<::Binder>	binder();
	    const boost::intrusive_ptr<::Binder> && binder() const;
	    boost::intrusive_ptr<::Binder> binder(boost::intrusive_ptr<::Binder> &&binder_);

	protected:
//        void setUrl(const QUrl &url);

	    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
	    QWebEnginePage	*createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if ! defined(QT_NO_UITOOLS)
	    QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
#endif
	    virtual bool certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;

	    void record_info_update(const QUrl &url, const QString &title);
	    void record_view_remove(boost::intrusive_ptr<TreeItem> host_);
	    void binder_reset();
	    std::function<void (const QUrl &url)> onUrlChanged;	// void onUrlChanged(const QUrl &url);
	    std::function<void (const QString &title)> onTitleChanged;	// void onTitleChanged(const QString &title);

	private slots:

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
	    void handleUnsupportedContent(QNetworkReply *reply);
#endif
	    void authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
	    void proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);



	private:
	    Profile			*_profile;
	    ts_t			*_tree_screen;
	    Editentry			*_editentry;
	    Entrance			*_entrance;
	    Browser			*_browser;
	    TabWidget			*_tabmanager;
	    rctrl_t			*_record_controller;

	    WebView *_view;

	    Qt::KeyboardModifiers _keyboardmodifiers	= Qt::NoModifier;
	    Qt::MouseButtons _pressedbuttons		= Qt::NoButton;

	    QUrl _loadingurl		= QUrl();
	    QString _hovered_url	= Browser::_defaulthome;



	    boost::intrusive_ptr<::Binder> _binder;


	    bool _certificate_ignored = false;

	    friend class TabWidget;
	    friend class WebView;
	    friend class Record;
    };

#ifdef USE_POPUP_WINDOW

    class PopupView : public QWebEngineView {
#if QT_VERSION == 0x050600
	W_OBJECT(PopupView)
#else
	Q_OBJECT
#endif
	public:
	    PopupView(QWidget *parent = 0);
	    PopupPage	*webPage() const {return _page;}

	    void setPage(PopupPage *page_);

	    void loadUrl(const QUrl &url);
	    QUrl url() const;
	    QIcon icon() const;

	    QString lastStatusBarText() const;
	    inline int progress() const {return _progress;}

	protected:
	    void mousePressEvent(QMouseEvent *event);
	    void mouseReleaseEvent(QMouseEvent *event);
	    void contextMenuEvent(QContextMenuEvent *event);
	    void wheelEvent(QWheelEvent *event);

	signals:
	    void iconChanged()
#if QT_VERSION == 0x050600
	    W_SIGNAL(iconChanged)	//
#else
	    ;
#endif
	private slots:
	    void setProgress(int progress);
	    void loadFinished(bool success);
	    void setStatusBarText(const QString &string);
	    void openLinkInNewTab();
	    void onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature);
	    void onIconUrlChanged(const QUrl &url);
	    void iconLoaded();

	private:
	    QString _statusbar_text;
	    QUrl _initial_url;
	    int	_progress;
	    PopupPage		*_page;
	    QIcon _icon;
	    QNetworkReply	*_icon_reply;
    };

#endif	// USE_POPUP_WINDOW

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
	    WebView(boost::intrusive_ptr<TreeItem> host_
		   , Profile *profile		// , bool openinnewtab
		   , ts_t *tree_screen
		   , Editentry *editentry
		   , Entrance *entrance
		   , Browser *browser
		   , TabWidget *tabmanager
		   , rctrl_t *record_controller);


	    ~WebView();
	    WebPage *page() const;

	    bool load_finished() const;
	    void page(WebPage *page);
	    void activateWindow();

	    WebView	*load(boost::intrusive_ptr<TreeItem> it, bool checked);
	    QUrl url() const = delete;
	    QIcon icon() const;

	    QString lastStatusBarText() const;
	    int	progress() const;

	    rctrl_t	*record_controller();	// {return _record_controller;}
	    void record_controller(rctrl_t *_record_controller);// {this->_record_controller = _record_controller;}

	    TabWidget *const &tabmanager() const;

	    void current_view_global_consistency();
	protected:
	    void loadUrl(const QUrl &url);

	    void mousePressEvent(QMouseEvent *event);
	    void mouseReleaseEvent(QMouseEvent *event);
	    void contextMenuEvent(QContextMenuEvent *event);
	    void wheelEvent(QWheelEvent *event);
	    void focusOutEvent(QFocusEvent *event);
	signals:
	    void iconChanged()
#if QT_VERSION == 0x050600
	    W_SIGNAL(iconChanged)	//
#else
	    ;
#endif
	    void close_requested()
#if QT_VERSION == 0x050600
	    W_SIGNAL(close_requested)	//
#else
	    ;
#endif
	public slots:
//    void loadFinished(bool success);
//    void openLinkInNewTab();
	    void on_close_requested();
	private slots:
	    void setProgress(int progress);

	    void setStatusBarText(const QString &string);
	    void openLinkInNewTab();
	    void openLinkInNewTab(const QUrl &url);
	    void onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature);
	    void onIconUrlChanged(const QUrl &url);
	    void iconLoaded();
	    void onLoadFinished(bool success);

//    void onCloseTab(int index);
	private:
	    Browser	*_browser;
	    TabWidget	*_tabmanager;
	    rctrl_t	*_record_controller;

	    WebPage	*_page;
	    QString _statusbartext;
//        QUrl _initialurl;
	    int	_progress;
	    QIcon _icon;
	    QNetworkReply		*_iconreply;
	    bool _load_finished = false;
	    QMetaObject::Connection _home_connection;			// for disconnect
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
	    PopupWindow(Browser *browser_, Profile *profile_);

	    QWebEnginePage *page() const {return _view->page();}

	private Q_SLOTS:
	    void setUrl(const QUrl &url){_addressbar->setText(url.toString());}

	    void adjustGeometry(const QRect &newGeometry);

	private:
	    Browser	*_browser;
	    QLineEdit	*_addressbar;
	    PopupView	*_view;
    };


#endif	// USE_POPUP_WINDOW
}

// QT_END_NAMESPACE

#endif
