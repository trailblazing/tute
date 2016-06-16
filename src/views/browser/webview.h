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
#include "controllers/record_table/RecordController.h"
#include "libraries/GlobalParameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "models/record_table/RecordModel.h"
#include "views/browser/featurepermissionbar.h"
#include "views/record_table/RecordScreen.h"
#include "views/record_table/RecordView.h"
// #include "models/tree/TreeItem.h"

QT_BEGIN_NAMESPACE
class QAuthenticator;
class QMouseEvent;
class QNetworkProxy;
class QNetworkReply;
class QSslError;
QT_END_NAMESPACE

extern GlobalParameters globalparameters;
extern boost::intrusive_ptr<Record> check_record(const QUrl &_url);

class Record;
struct Binder;
class TreeItem;
class TreeModel;
class TreeScreen;
class RecordController;

QT_BEGIN_NAMESPACE

// #define USE_POPUP_WINDOW

namespace browser {

    class Browser;
    class PopupWindow;
    class WebView;
    class TabWidget;

#ifdef USE_POPUP_WINDOW

    class PopupPage : public QWebEnginePage {
        Q_OBJECT

        signals:
            void loadingUrl(const QUrl &url);

        public:
            PopupPage(Profile *profile, QObject *parent = 0);
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
            Qt::KeyboardModifiers m_keyboardModifiers;
            Qt::MouseButtons m_pressedButtons;
            QUrl m_loadingUrl;
            bool _certificate_ignored = false;
    };

#endif	// USE_POPUP_WINDOW

    class WebPage : public QWebEnginePage {
        Q_OBJECT

        signals:
            void loadingUrl(const QUrl &url);
//        void linkHovered(const QString &url, int time_out = 0);
            void close_requested();

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
                   , TreeScreen *tree_screen
                   , MetaEditor *editor_screen
                   , Entrance *entrance
                   , Browser *browser
                   , TabWidget *tabmanager
                   , RecordController *record_controller
                   , WebView *parent = 0);

            ~WebPage();
//        WebView *(*_load_record)(Record *const record);
            TabWidget *tabmanager();
            Browser *browser();
            WebView *view();		// {assert(_view); return _view;}
//        std::map<QString, boost::intrusive_ptr<TreeItem>>
//        binded_items()const;
            RecordController *record_controller();	// {return _record_controller;}
            WebView *activate();
            WebView *load(boost::intrusive_ptr<TreeItem> item, bool checked = true);
//        WebView *bind(boost::intrusive_ptr<TreeItem> item);

            void load(const QUrl &url) = delete;

            boost::intrusive_ptr<TreeItem>item() const;	// {return _record_binder->bounded_item();}
            WebPage *page() const;

            struct Binder : public std::enable_shared_from_this <Binder> {		// boost::intrusive_ref_counter<Binder, boost::thread_safe_counter>    //

                private:
                    boost::intrusive_ptr<TreeItem>  _item;
                    WebPage                         *_page;

                        //            bool _make_current;
                public:
                    Binder(boost::intrusive_ptr<TreeItem> item_, WebPage *page_);
                    ~Binder();
                    void host(boost::intrusive_ptr<TreeItem> it){_item = it;}
                    void page(WebPage *p){_page = p;}
                    boost::intrusive_ptr<TreeItem> host() const {return _item;}
                    WebPage *page() const {return _page;}
                    WebView *bind();		// , boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *)
                    WebView *activator();
                    static QString binder_type(){return "page_binder";}
            };

//        void item_break(boost::intrusive_ptr<TreeItem> item);    //
//        {if(_record->binded_page() == this)_record->bind_page(nullptr);
//        _record = nullptr;}
//        void items_break();
            void sychronize_metaeditor_to_item();

//        boost::intrusive_ptr<TreeItem> item_bind(const QUrl &_url
//                                                              , const
//                                                              TreeScreen::paste_strategy
//                                                              &_view_paste_strategy
//                                                              , equal_url_t
//                                                              _equal =
//                                                              [](boost::intrusive_ptr<const
//                                                              TreeItem> it,
//                                                              const QUrl
//                                                              &_url)->bool
//                                                              {return
//                                                              it->field("url")
//                                                              ==
//                                                              _url.toString();}
//                                                             );

//        boost::intrusive_ptr<TreeItem>
//        item_bind(boost::intrusive_ptr<TreeItem> item
//                                                 , const
//                                                 TreeScreen::paste_strategy
//                                                 &_view_paste_strategy
//                                                 , equal_t _equal =
//                                                 [](boost::intrusive_ptr<const
//                                                 TreeItem> it,
//                                                 boost::intrusive_ptr<const
//                                                 TreeItem> target)->bool
//                                                 {return it->id() ==
//                                                 target->id();}
//                                                );

            boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<TreeItem> item);

            boost::intrusive_ptr<::Binder> binder();
            const boost::intrusive_ptr<::Binder> && binder() const;
            void binder(boost::intrusive_ptr<::Binder> &&binder_);

        protected:
//        void setUrl(const QUrl &url);

            bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
            QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if ! defined(QT_NO_UITOOLS)
            QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
#endif
            virtual bool certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;

            void update_record(const QUrl &url, const QString &title);
            void update_record_view(boost::intrusive_ptr<TreeItem> item);
            void item_remove_from_record_screen(boost::intrusive_ptr<TreeItem> item);
            void binder_reset();

        private slots:

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
            void handleUnsupportedContent(QNetworkReply *reply);
#endif
            void authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
            void proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);

            void onUrlChanged(const QUrl &url);
            void onTitleChanged(const QString &title);

        private:
            TreeScreen *_tree_screen;
            MetaEditor *_editor_screen;
            Entrance *_entrance;
            Browser *_browser;
            TabWidget *_tabmanager;
            RecordController *_record_controller;
//        std::map<QString, boost::intrusive_ptr<TreeItem> > _items;

            WebView *_view;
// set the webview mousepressedevent
            Qt::KeyboardModifiers _keyboardmodifiers = Qt::NoModifier;
            Qt::MouseButtons _pressedbuttons = Qt::NoButton;
// bool _openinnewtab;
            QUrl _loadingurl;
            QString _hovered_url = Browser::_defaulthome;
// bool _create_window_generated;

//        boost::intrusive_ptr<TreeItem>  _item;
//        void item(boost::intrusive_ptr<TreeItem> it) {_item = it;}

            boost::intrusive_ptr<::Binder> _binder;

//        friend class Record;
//        friend class TreeItem;

//        friend void TreeItem::page_to_nullptr();
//        friend Record::Record(const Record &obj);

//        friend Record::~Record();

//        friend Record *TreeItem::bind(WebPage *page);
//        friend WebPage *TreeItem::unique_page();
//        friend bool TreeItem::is_holder();

//        friend TreeItem::~TreeItem();

//        friend boost::intrusive_ptr<TreeItem>
//        TreeItem::bind(browser::WebPage *page);

//        friend bool TreeItem::is_holder();
            bool _certificate_ignored = false;

            friend class TabWidget;
            friend class WebView;
            friend class Record;
    };

#ifdef USE_POPUP_WINDOW

    class PopupView : public QWebEngineView {
        Q_OBJECT

        public:
            PopupView(QWidget *parent = 0);
            PopupPage *webPage() const {
                return m_page;
            }
            void setPage(PopupPage *page);

            void loadUrl(const QUrl &url);
            QUrl url() const;
            QIcon icon() const;

            QString lastStatusBarText() const;
            inline int progress() const {
                return m_progress;
            }
        protected:
            void mousePressEvent(QMouseEvent *event);
            void mouseReleaseEvent(QMouseEvent *event);
            void contextMenuEvent(QContextMenuEvent *event);
            void wheelEvent(QWheelEvent *event);

        signals:
            void iconChanged();

        private slots:
            void setProgress(int progress);
            void loadFinished(bool success);
            void setStatusBarText(const QString &string);
            void openLinkInNewTab();
            void onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature);
            void onIconUrlChanged(const QUrl &url);
            void iconLoaded();

        private:
            QString m_statusBarText;
            QUrl m_initialUrl;
            int m_progress;
            PopupPage *m_page;
            QIcon m_icon;
            QNetworkReply *m_iconReply;
    };

#endif	// USE_POPUP_WINDOW

// template Q_ONJECT
// https://doc.qt.io/archives/qq/qq16-dynamicqobject.html
// browserview
// template<typename T>
    class WebView : public QWebEngineView {
        Q_OBJECT

        public:
            WebView(boost::intrusive_ptr<TreeItem> item
                   , Profile *profile		// , bool openinnewtab
                   , TreeScreen *tree_screen, MetaEditor *editor_screen
                   , Entrance *entrance
                   , Browser *browser
                   , TabWidget *tabmanager
                   , RecordController *record_controller);

//        WebView(const boost::intrusive_ptr<TreeItem> requested_item
//                , Profile *profile    // , bool openinnewtab
//                , TabWidget *tabmanager
//                , QWidget *parent
//                , RecordController *record_controller
//               );

            ~WebView();
            WebPage *page() const {
                return _page;
            }
            bool load_finished() const;
            void page(WebPage *page);
            void activateWindow();

            WebView *load(boost::intrusive_ptr<TreeItem> it, bool checked);
            QUrl url() const = delete;
            QIcon icon() const;

            QString lastStatusBarText() const;
            inline int progress() const {
                return _progress;
            }
            RecordController *record_controller(){
                return _record_controller;
            }
            void record_controller(RecordController *_record_controller){
                this->_record_controller = _record_controller;
            }
//        Record *const &record()const {return _record;}
//        void record(Record *record) {if(record) {_record = record;
//        _record->view(this);}}

//        void switch_show();

            TabWidget *const &tabmanager() const {
                return _tabmanager;
            }
            void recovery_global_consistency();
        protected:
            void loadUrl(const QUrl &url);

            void mousePressEvent(QMouseEvent *event);
            void mouseReleaseEvent(QMouseEvent *event);
            void contextMenuEvent(QContextMenuEvent *event);
            void wheelEvent(QWheelEvent *event);

        signals:
            void iconChanged();
            void close_requested();
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
            Browser *_browser;
            TabWidget *_tabmanager;
            RecordController *_record_controller;

            WebPage *_page;
            QString _statusbartext;
//        QUrl _initialurl;
            int _progress;
            QIcon _icon;
            QNetworkReply *_iconreply;
            bool _load_finished = false;
            QMetaObject::Connection _home_connection;		// for disconnect
            friend class TabWidget;
            friend class WebPage;
    };

#ifdef USE_POPUP_WINDOW

    class PopupWindow : public QWidget {
        Q_OBJECT
        public:
            PopupWindow(Profile *profile)
                : QWidget(), _addressbar(new QLineEdit(this)),
                  _view(new PopupView(this)){
                _view->setPage(new PopupPage(profile, _view));
                QVBoxLayout *layout = new QVBoxLayout;
                layout->setMargin(0);
                setLayout(layout);
                layout->addWidget(_addressbar);
                layout->addWidget(_view);
                _view->setFocus();

                connect(_view, &PopupView::titleChanged, this, &QWidget::setWindowTitle);
                connect(_view, &PopupView::urlChanged, this, &PopupWindow::setUrl);
                connect(page(), &PopupPage::geometryChangeRequested, this,
                    &PopupWindow::adjustGeometry);
                connect(page(), &PopupPage::windowCloseRequested, this, &QWidget::close);
            }
            QWebEnginePage *page() const {
                return _view->page();
            }
        private Q_SLOTS:
            void setUrl(const QUrl &url){
                _addressbar->setText(url.toString());
            }
            void adjustGeometry(const QRect &newGeometry){
                const int x1 = frameGeometry().left() - geometry().left();
                const int y1 = frameGeometry().top() - geometry().top();
                const int x2 = frameGeometry().right() - geometry().right();
                const int y2 = frameGeometry().bottom() - geometry().bottom();

                setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
            }
        private:
            QLineEdit *_addressbar;
            PopupView *_view;
    };

#endif	// USE_POPUP_WINDOW
}

QT_END_NAMESPACE

#endif
