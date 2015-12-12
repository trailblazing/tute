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
#include <set>


#include "models/recordTable/Record.h"
#include "controllers/recordTable/RecordTableController.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/recordTable/Record.h"
#include "libraries/GlobalParameters.h"
//#include "tabwidget.h"
#include "views/browser/featurepermissionbar.h"
#include "libraries/qtSingleApplication5/qtsingleapplication.h"
#include "views/recordTable/RecordTableView.h"
//#include "tabwidget.h"
#include "models/recordTable/Record.h"


QT_BEGIN_NAMESPACE
class QAuthenticator;
class QMouseEvent;
class QNetworkProxy;
class QNetworkReply;
class QSslError;
QT_END_NAMESPACE

extern GlobalParameters globalparameters;
extern std::shared_ptr<Record> check_record(const QUrl &_url);

class Record;

QT_BEGIN_NAMESPACE


namespace browser {

    class Browser;
    class PopupWindow;
    class WebView;
    class TabWidget;

    class WebPage : public QWebEnginePage {
        Q_OBJECT

    signals:
        void loadingUrl(const QUrl &url);

    public:
        ~WebPage()
        {
            //            if(_record) {
            //                _record->page(nullptr); // globalparameters.getRecordTableScreen()->previousInFocusChain();    //_record;
            //                _record = nullptr;
            //            }
            std::set<std::shared_ptr<Record> > records = binded_records();

            for(auto i : records) {
                if(i->binded_only_page() == this)
                    i->page_to_nullptr();
            }
        }

        WebPage(QWebEngineProfile *profile
                , const std::shared_ptr<Record> record
                // , bool openinnewtab
                , RecordTableController *_recordtablecontroller
                , WebView *parent = 0
               );
        //        WebView *(*_load_record)(Record *const record);
        Browser *browser();
        WebView *view() {return _pageview;}
        std::set<std::shared_ptr<Record> > binded_records()const;

        void active();
        WebView *load(const std::shared_ptr<Record> record, bool checked = true);
        void load(const QUrl &url) = delete;

        //        void synchronize_title_to_record()
        //        {
        //            this->update_record();
        //            //if(_record)_record->setNaturalFieldSource("name", webPage()->title());
        //        }

        std::shared_ptr<Record> current_record()const {return _record;}

        struct ActiveRecordBinder {
            WebPage *_the;
            bool _make_current;
            //            RecordTableController *_recordtablecontroller;
            ActiveRecordBinder(WebPage *the, bool make_current
                               = true
                                 //                            , RecordTableController *recordtablecontroller
                                 //                          = globalparameters.getRecordTableScreen()->getRecordTableController()
                              )
                : _the(the)
                , _make_current(make_current)
                  //              , _recordtablecontroller(recordtablecontroller)
            {}

            WebView *generator(std::shared_ptr<Record> record)
            {
                return _the->load(record, _make_current);
                //                                    , _recordtablecontroller

            }

            void activator()
            {
                _the->active();
            }
        };

    protected:

        bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) Q_DECL_OVERRIDE;
        QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;
#if !defined(QT_NO_UITOOLS)
        QObject *createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
#endif
        virtual bool certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;

        std::shared_ptr<Record> bind_record(const std::shared_ptr<Record> binded_records);
        void break_record_which_page_point_to_me();    // {if(_record->binded_page() == this)_record->bind_page(nullptr); _record = nullptr;}
        void update_record(const QUrl &url
                           // = ([&](WebPage *const the)->QUrl{return the->url();})(this)
                           , const QString &title
                           // = title()
                           , bool make_current = true
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
        std::set<std::shared_ptr<Record> > _records;
        std::shared_ptr<Record> _record;

        WebView *_pageview;
        // set the webview mousepressedevent
        Qt::KeyboardModifiers _keyboardmodifiers;
        Qt::MouseButtons _pressedbuttons;
        // bool _openinnewtab;
        QUrl    _loadingurl;
        //bool _create_window_generated;
        RecordTableController *_recordtablecontroller;


        void record(std::shared_ptr<Record> record) {_record = record;}
        friend class Record;
        friend void Record::page_to_nullptr();
        friend Record::Record(const Record &obj);
        friend Record::~Record();
        friend Record *Record::bind_page(WebPage *page);
        friend WebPage *Record::binded_only_page();
        friend bool Record::is_holder();
    };


    // template Q_ONJECT
    // https://doc.qt.io/archives/qq/qq16-dynamicqobject.html
    // browserview
    // template<typename T>
    class WebView : public QWebEngineView {
        Q_OBJECT

    public:
        WebView(std::shared_ptr<Record> record
                , QWebEngineProfile *profile    // , bool openinnewtab
                , TabWidget *parent
                , RecordTableController *recordtablecontroller = globalparameters.getRecordTableScreen()->getRecordTableController()
               );

        ~WebView();
        WebPage *page() const { return _page; }
        void page(WebPage *page);


        WebView *load(const std::shared_ptr<Record> record);
        QUrl url() const = delete;
        QIcon icon() const;

        QString lastStatusBarText() const;
        inline int progress() const { return _progress; }

        RecordTableController *recordtablecontroller() {return _recordtablecontroller;}
        void bind_recordtabcontroller(RecordTableController *recordtablecontroller) {_recordtablecontroller  = recordtablecontroller ;}
        //        Record *const &record()const {return _record;}
        //        void record(Record *record) {if(record) {_record = record; _record->view(this);}}

        void switch_show();

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
        TabWidget   *_tabmanager;
        //        Record *_record;
        RecordTableController *_recordtablecontroller;
        WebPage     *_page;
        QString     _statusbartext;
        //        QUrl _initialurl;
        int         _progress;
        QIcon       _icon;
        QNetworkReply   *_iconreply;

        //friend class TabWidget;
    };




}


QT_END_NAMESPACE

#endif


