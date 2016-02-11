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

#include <QIcon>
#include <QWebEngineView>
#include <QLineEdit>
#include <QNetworkReply>
#include <QClipboard>
#include <map>


#include "models/record_table/Record.h"
#include "controllers/record_table/RecordController.h"
#include "views/record_table/RecordScreen.h"
#include "models/record_table/Record.h"
#include "libraries/GlobalParameters.h"
#include "views/browser/featurepermissionbar.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "views/record_table/RecordView.h"
#include "models/record_table/Record.h"


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
class TreeItem;

QT_BEGIN_NAMESPACE

//#define USE_POPUP_WINDOW

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
        PopupPage(QWebEngineProfile *profile, QObject *parent = 0);
        Browser *browser();

    protected:
        bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
        //        QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if !defined(QT_NO_UITOOLS)
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
    };

#endif // USE_POPUP_WINDOW

    class WebPage : public QWebEnginePage {
        Q_OBJECT

    signals:
        void loadingUrl(const QUrl &url);
        //        void linkHovered(const QString &url, int time_out = 0);
    public:
        ~WebPage()
        {
            //            if(_record) {
            //                _record->page(nullptr); // globalparameters.getRecordTableScreen()->previousInFocusChain();    //_record;
            //                _record = nullptr;
            //            }

            //            std::list<boost::intrusive_ptr<Record> > records = binded_records();

            for(auto &j : _records) {
                //                if(auto i = j.second) {
                if(j.second && j.second->unique_page() == this) {
                    break_records();  //
                    j.second->page_to_nullptr();
                }

                //                }
            }
        }

        WebPage(QWebEngineProfile *profile
                , const boost::intrusive_ptr<TreeItem> record
                // , bool openinnewtab
                , RecordController *_record_controller
                //                , TableController *_page_controller
                , WebView *parent = 0
               );
        //        WebView *(*_load_record)(Record *const record);
        Browser *browser();
        WebView *view() {return _pageview;}
        std::map<QString, boost::intrusive_ptr<TreeItem>> binded_records()const;

        WebView *active();
        WebView *load(const boost::intrusive_ptr<TreeItem> record, bool checked = true);
        WebView *bind(boost::intrusive_ptr<TreeItem> record);

        void load(const QUrl &url) = delete;

        //        void synchronize_title_to_record()
        //        {
        //            this->update_record();
        //            //if(_record)_record->setNaturalFieldSource("name", webPage()->title());
        //        }

        boost::intrusive_ptr<TreeItem> current_item()const {return _tree_item;}

        struct ActiveRecordBinder {
            WebPage *_the;
            //            bool _make_current;
            //            RecordTableController *_record_ontroller;
            ActiveRecordBinder(WebPage *the
                               //                               , bool make_current = true
                              )
            //                            , RecordTableController *recordtablecontroller
            //                          = globalparameters.getRecordTableScreen()->getRecordTableController()

                : _the(the)
                  //                , _make_current(make_current)
                  //              , _record_ontroller(recordtablecontroller)
            {}

            WebView *binder(boost::intrusive_ptr<TreeItem> record, boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *))
            {
                WebView *view = _the->bind(record);
                (record.get()->*_bind)(_the);
                return view;  // _the->load(record, _make_current);
                //                                    , _record_ontroller

            }

            WebView *activator(boost::intrusive_ptr<TreeItem> record)
            {
                assert(record->unique_page() == _the);
                return _the->active();
            }
        };

        boost::intrusive_ptr<TreeItem> equip_registered(boost::intrusive_ptr<TreeItem> record);
        void add_record_to_table_data(boost::intrusive_ptr<TreeItem> item);
        void remove_record_from_table_data(boost::intrusive_ptr<TreeItem> record);
        void break_record(boost::intrusive_ptr<TreeItem> record);    // {if(_record->binded_page() == this)_record->bind_page(nullptr); _record = nullptr;}
        void break_records();
        void sychronize_metaeditor_to_record(boost::intrusive_ptr<TreeItem> record);


    protected:

        bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
        QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if !defined(QT_NO_UITOOLS)
        QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
#endif
        virtual bool certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;

        void update_record(const QUrl &url
                           // = ([&](WebPage *const the)->QUrl{return the->url();})(this)
                           , const QString &title
                           // = title()
                          );


    private slots:

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        void handleUnsupportedContent(QNetworkReply *reply);
#endif
        void authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
        void proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);

        void onUrlChanged(const QUrl &url);
        void onTitleChanged(const QString &title);


    private:
        friend class WebView;
        friend class Record;
        std::map<QString, boost::intrusive_ptr<TreeItem> > _records;
        boost::intrusive_ptr<TreeItem> _tree_item;

        WebView                 *_pageview;
        // set the webview mousepressedevent
        Qt::KeyboardModifiers   _keyboardmodifiers;
        Qt::MouseButtons        _pressedbuttons;
        // bool _openinnewtab;
        QUrl                    _loadingurl;
        //bool _create_window_generated;
        RecordController        *_record_controller;
        //        TableController         *_page_controller;

        void record(boost::intrusive_ptr<TreeItem> item) {_tree_item = item;}
        friend class Record;
        friend class TreeItem;
        //        friend void TreeItem::page_to_nullptr();
        //        friend Record::Record(const Record &obj);
        friend Record::~Record();
        //        friend Record *TreeItem::bind(WebPage *page);
        //        friend WebPage *TreeItem::unique_page();
        //        friend bool TreeItem::is_holder();
        friend TreeItem::~TreeItem();
        friend boost::intrusive_ptr<TreeItem> TreeItem::bind(browser::WebPage *page);
        friend bool TreeItem::is_holder();
    };

#ifdef USE_POPUP_WINDOW

    class PopupView : public QWebEngineView {
        Q_OBJECT

    public:
        PopupView(QWidget *parent = 0);
        PopupPage *webPage() const { return m_page; }
        void setPage(PopupPage *page);

        void loadUrl(const QUrl &url);
        QUrl url() const;
        QIcon icon() const;

        QString lastStatusBarText() const;
        inline int progress() const { return m_progress; }

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


#endif //USE_POPUP_WINDOW


    // template Q_ONJECT
    // https://doc.qt.io/archives/qq/qq16-dynamicqobject.html
    // browserview
    // template<typename T>
    class WebView : public QWebEngineView {
        Q_OBJECT

    public:
        WebView(boost::intrusive_ptr<TreeItem> record
                , QWebEngineProfile *profile    // , bool openinnewtab
                , TabWidget *parent
                , RecordController *table_controller
                = globalparameters.table_screen()->table_controller()
                  //                  , TableController *_page_controller
                  //                = globalparameters.page_screen()->table_controller()
               );

        WebView(const boost::intrusive_ptr<TreeItem> record
                , QWebEngineProfile *profile    // , bool openinnewtab
                , TabWidget *tabmanager
                , QWidget *parent
                , RecordController *table_controller
                = globalparameters.table_screen()->table_controller()
                  //                  , TableController *_page_controller
                  //                = globalparameters.page_screen()->table_controller()
               );

        ~WebView();
        WebPage *page() const { return _page; }
        void page(WebPage *page);


        WebView *load(boost::intrusive_ptr<TreeItem> record);
        QUrl url() const = delete;
        QIcon icon() const;

        QString lastStatusBarText() const;
        inline int progress() const { return _progress; }

        RecordController *table_controller() {return _record_controller;}
        void bind_recordtabcontroller(RecordController *recordtablecontroller) {_record_controller  = recordtablecontroller ;}
        //        Record *const &record()const {return _record;}
        //        void record(Record *record) {if(record) {_record = record; _record->view(this);}}

        //        void switch_show();

        TabWidget *const &tabmanager()const {return _tabmanager;}
    protected:
        void loadUrl(const QUrl &url);

        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void contextMenuEvent(QContextMenuEvent *event);
        void wheelEvent(QWheelEvent *event);

    signals:
        void iconChanged();

    public slots:
        //    void loadFinished(bool success);
        //    void openLinkInNewTab();

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
        TabWidget               *_tabmanager;
        //        Record *_record;
        RecordController         *_record_controller;
        //        TableController         *_page_controller;
        WebPage                 *_page;
        QString                 _statusbartext;
        //        QUrl _initialurl;
        int                     _progress;
        QIcon                   _icon;
        QNetworkReply           *_iconreply;

        friend class TabWidget;
    };

#ifdef USE_POPUP_WINDOW

    class PopupWindow : public QWidget {
        Q_OBJECT
    public:

        PopupWindow(QWebEngineProfile *profile)
            : QWidget()
            , _addressbar(new QLineEdit(this))
            , _view(new PopupView(this))
        {
            _view->setPage(new PopupPage(profile, _view));
            QVBoxLayout *layout = new QVBoxLayout;
            layout->setMargin(0);
            setLayout(layout);
            layout->addWidget(_addressbar);
            layout->addWidget(_view);
            _view->setFocus();

            connect(_view, &PopupView::titleChanged, this, &QWidget::setWindowTitle);
            connect(_view, &PopupView::urlChanged, this, &PopupWindow::setUrl);
            connect(page(), &PopupPage::geometryChangeRequested, this, &PopupWindow::adjustGeometry);
            connect(page(), &PopupPage::windowCloseRequested, this, &QWidget::close);
        }

        QWebEnginePage *page() const { return _view->page(); }

    private Q_SLOTS:
        void setUrl(const QUrl &url)
        {
            _addressbar->setText(url.toString());
        }

        void adjustGeometry(const QRect &newGeometry)
        {
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

#endif // USE_POPUP_WINDOW

}


QT_END_NAMESPACE

#endif


