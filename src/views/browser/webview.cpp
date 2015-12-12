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


#include "libraries/qtSingleApplication5/qtsingleapplication.h"

#include "browser.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "featurepermissionbar.h"
#include "networkaccessmanager.h"
#include "ui_passworddialog.h"
#include "ui_proxy.h"
#include "tabwidget.h"
#include "webview.h"
#include "views/mainWindow/MainWindow.h"
#include "views/findInBaseScreen/FindScreen.h"


#include <QtGui/QClipboard>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtGui/QMouseEvent>


#if defined(QWEBENGINEPAGE_HITTESTCONTENT)
#include <QWebEngineHitTestResult>
#endif

#ifndef QT_NO_UITOOLS
#include <QtUiTools/QUiLoader>
#endif  //QT_NO_UITOOLS

#include <QtCore/QDebug>
#include <QtCore/QBuffer>

#include "main.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "models/recordTable/Record.h"
#include "views/recordTable/RecordTableView.h"
#include "libraries/GlobalParameters.h"
#include "views/browser/entrance.h"
#include "views/recordTable/RecordTableScreen.h"
#include "controllers/recordTable/RecordTableController.h"
//#include "browserview.moc"
#include "libraries/GlobalParameters.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"



#include <iostream>

class GlobalParameters;
extern GlobalParameters globalparameters;

namespace browser {

    WebPage::WebPage(QWebEngineProfile *profile
                     , Record *const record
                     // , bool openinnewtab
                     , RecordTableController *_recordtablecontroller
                     , WebView *parent
                    )
        : QWebEnginePage(profile, parent)
          //        , _load_record(&WebPage::load)
        , _record(nullptr
                  //              url ?
                  //                  [this](Record * const r) {_record = nullptr; return bind_record(r);}(url)
                  //    : nullptr
                 )
        , _pageview(parent)
        , _keyboardmodifiers(Qt::NoModifier)
        , _pressedbuttons(Qt::NoButton)
          //        , _openinnewtab(
          //              openinnewtab  //false
          //          )
          //, _create_window_generated(false)
        , _recordtablecontroller(_recordtablecontroller)
    {
        //        assert(url != nullptr);

        //        if(record != nullptr) {
        //            // record->page(this);

        //            bind_record(record);
        //        }

#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
        setNetworkAccessManager(QtSingleApplication::networkAccessManager());
#endif
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
                this, SLOT(handleUnsupportedContent(QNetworkReply *)));
#endif
        connect(this, &WebPage::authenticationRequired, &WebPage::authenticationRequired);
        connect(this, &WebPage::proxyAuthenticationRequired, &WebPage::proxyAuthenticationRequired);

        connect(this, &WebPage::titleChanged, this, &WebPage::onTitleChanged);
        connect(this, &WebPage::urlChanged, this, &WebPage::onUrlChanged);

        //        if(record)QWebEnginePage::load(record->getNaturalFieldSource("url"));

        //        if(url != nullptr) {
        auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(this, true);
        _record = request_record(
                      record
                      , std::make_shared <
                      sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>
                      > (
                          ""
                          , &WebPage::ActiveRecordBinder::generator
                          , ar  // boost::make_shared<WebPage::active_record>(this, true)
                      )
                      , std::make_shared <
                      sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>
                      > (
                          ""
                          , &WebPage::ActiveRecordBinder::activator
                          , ar  // boost::make_shared<WebPage::active_record>(this, true)
                      )
                  );
        //            load(url);
        //        }

    }

    Browser *WebPage::browser()
    {
        //        QObject *w = this->parent();

        //        while(w) {  // ? why
        //            if(DockedWindow *mw = qobject_cast<DockedWindow *>(w))
        //                return mw;

        //            w = w->parent();
        //        }
        return view()->tabmanager()->browser();
        //        return globalparameters.entrance()->activiated_registered().first;    //QtSingleApplication::instance()->mainWindow();
    }


    //    void WebPage::load(const QUrl &url)
    //    {
    //        //        if(_record) {
    //        //            _record->setNaturalFieldSource("url", url.toString());
    //        //            assert(_record->page() == this);
    //        //        }

    //        //            if(_record->getNaturalFieldSource("url") != DockedWindow::_defaulthome)
    //        setUrl(url);
    //        QWebEnginePage::load(url);
    //    }

    WebView *WebPage::load(Record *const record, bool checked)
    {
        Q_UNUSED(checked)
        assert(record);

        //        if(_record)
        //            if(_record->getNaturalFieldSource("url") != DockedWindow::_defaulthome) {
        // _record = record;
        if(
            (_record != record)
            //            || ((_record == record) && (_record->_page != record->_page))
            //            (!_record)
            //            || (
            //                _record && (_record->getNaturalFieldSource("url") != record->getNaturalFieldSource("url"))
            //            )
        ) {

            bind_record(record);
        }

        assert(_record);
        QUrl _url = QUrl(_record->getNaturalFieldSource("url"));

        if(
            //            _record && (_record->getNaturalFieldSource("url") != record->getNaturalFieldSource("url"))
            url() != _url
        ) {
            triggerAction(QWebEnginePage::Stop);

            // QUrl url = QUrl(_record->getNaturalFieldSource("url"));
            setUrl(_url);    // load(QUrl(_record->getNaturalFieldSource("url")));

            QWebEnginePage::load(_url);
            //            }
            //            update_record(_url, title(), checked);

        }

        active();
        //        _record->active_immediately(true);
        return view();
    }

    void WebPage::active()
    {


        if(_record
           // ->_active_request
          ) {

            if(_record->_open_link_in_new_window == 1) {

            }

            Browser *win = view()->tabmanager()->browser();
            assert(win);

            if(!win->isActiveWindow() || !win->isVisible()) {
                win->raise();
                win->activateWindow();
            }

            view()->tabmanager()->setCurrentWidget(view());
            view()->show();
            //        if(checked) // globalparameters.mainwindow()
            this->view()->setFocus();   // make upate validate
            _recordtablecontroller->select_id(_record->getNaturalFieldSource("id"));
        }
    }

    bool WebPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
    {
        Q_UNUSED(type);

        if(isMainFrame) {
            _loadingurl = url;
            emit loadingUrl(_loadingurl);
        }

        return true;
    }

    bool WebPage::certificateError(const QWebEngineCertificateError &error)
    {
        if(error.isOverridable()) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(error.errorDescription());
            msgBox.setInformativeText(tr("If you wish so, you may continue with an unverified certificate. "
                                         "Accepting an unverified certificate means "
                                         "you may not be connected with the host you tried to connect to.\n"
                                         "Do you wish to override the security check and continue?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            return msgBox.exec() == QMessageBox::Yes;
        }

        QMessageBox::critical(view(), tr("Certificate Error"), error.errorDescription(), QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    //    class PopupWindow : public QWidget {
    //        Q_OBJECT
    //    public:
    //        PopupWindow(QWebEngineProfile *profile)
    //            : _addressbar(new QLineEdit(this))
    //            , _view(new BrowserView(nullptr, profile, globalParameters.getRecordTableScreen()->getRecordTableController(), this))
    //        {
    //            _view->setPage(new BrowserPage(profile, _view));
    //            QVBoxLayout *layout = new QVBoxLayout;
    //            layout->setMargin(0);
    //            setLayout(layout);
    //            layout->addWidget(_addressbar);
    //            layout->addWidget(_view);
    //            _view->setFocus();

    //            connect(_view, &BrowserView::titleChanged, this, &QWidget::setWindowTitle);
    //            connect(_view, &BrowserView::urlChanged, this, &PopupWindow::setUrl);
    //            connect(page(), &BrowserPage::geometryChangeRequested, this, &PopupWindow::adjustGeometry);
    //            connect(page(), &BrowserPage::windowCloseRequested, this, &QWidget::close);
    //        }

    //        QWebEnginePage *page() const { return _view->page(); }

    //    private Q_SLOTS:
    //        void setUrl(const QUrl &url)
    //        {
    //            _addressbar->setText(url.toString());
    //        }

    //        void adjustGeometry(const QRect &newGeometry)
    //        {
    //            const int x1 = frameGeometry().left() - geometry().left();
    //            const int y1 = frameGeometry().top() - geometry().top();
    //            const int x2 = frameGeometry().right() - geometry().right();
    //            const int y2 = frameGeometry().bottom() - geometry().bottom();

    //            setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
    //        }

    //    private:
    //        QLineEdit *_addressbar;
    //        BrowserView *_view;

    //    };

    //#include "webview.moc"

    QWebEnginePage *WebPage::createWindow(QWebEnginePage::WebWindowType type)
    {
        //        Record *blank_record = request_record(QUrl(DockedWindow::_defaulthome));    // QUrl("about:newtab")

        //        QWebEnginePage
        WebPage *page = nullptr;

        if(//_openinnewtab ||
            type == QWebEnginePage::WebBrowserTab) {

            // _openinnewtab = false;  // true


            // assert(_record);

            //            QUrl current = url();
            //            QUrl requestedurl = requestedUrl();
            //            QString previous_url = _record->getNaturalFieldSource("url");    // the parent page that host the new link

            //            return globalparameters.entrance()->activebrowser()->tabWidget()->newTab()->page();

            //assert(url == DockedWindow::_defaulthome);
            //Record *new_record = register_record(QUrl(DockedWindow::_defaulthome));    // do not modify current this->_record!
            //assert(_record->page() == this); //_record->page(this);

            // return nullptr;

            WebView *view = globalparameters.entrance()->find(QUrl(Browser::_defaulthome)).second;

            if(view) {
                //                return view->page();
                auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(view->page(), true);
                //                Record *record =
                request_record(
                    QUrl(Browser::_defaulthome)
                    , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                        ""
                        , &WebPage::ActiveRecordBinder::generator
                        , ar
                    )
                    , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                        ""
                        , &WebPage::ActiveRecordBinder::activator
                        , ar
                    )
                );
                //                record->generate();
                //                record->active();
                page = view->page();
            } else {

                // Page *page = this->dockedwindow()->tabWidget()->new_view(new_record, true)->page();
                // page->bind_record(new_record);  //Record *record = request_record(page->url());
                // page->update_record(page->url(), page->title());

                // already create window, why do this? -- refer to demo browser
                auto arint = boost::make_shared<TabWidget::NewTab>(browser()->tabWidget(), true);
                Record *r = request_record(
                                QUrl(Browser::_defaulthome)
                                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                                    ""
                                    , &TabWidget::NewTab::generator
                                    , arint
                                )
                                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>(
                                    ""
                                    , &TabWidget::NewTab::activator
                                    , arint
                                )
                            );
                //                return //_record->page();
                // globalparameters.entrance()->invoke_view(new_record).second->page();
                //                    this->dockedwindow()->tabWidget()->newTab(blank_record, true)->page();

                //                if(!r->generator())browser()->equip_registered(r);
                assert(r->generator());
                r->generate();
                r->active();
                page = r->binded_only_page();

            }



        } else if(type == QWebEnginePage::WebBrowserWindow) {

            std::pair<Browser *, WebView *> dp = globalparameters.entrance()->new_dockedwindow(QUrl(Browser::_defaulthome));               // QtSingleApplication::instance()->newMainWindow();
            //            DockedWindow *mainWindow = globalparameters.entrance()->activiated_registered().first;  // QtSingleApplication::instance()->mainWindow();
            //            return

            page = dp.second->page();  // mainWindow->currentTab()->page();

        } else {

            PopupWindow *popup = new PopupWindow(profile(), QUrl(Browser::_defaulthome), _recordtablecontroller, view()->tabmanager()->browser());
            popup->setAttribute(Qt::WA_DeleteOnClose);
            popup->show();
            //            return
            page = popup->page();
        }

        assert(page);

        // never called
        connect(page, &QWebEnginePage::load, page, [&](const QUrl & url) {
            Record *record = page->current_record();
            record->setNaturalFieldSource("url", url.toString());
            record->generate();
            record->active();
        });

        // not realy needed for each time
        connect(page, &QWebEnginePage::setUrl, page, [&](const QUrl & url) {
            Record *record = page->_record;
            record->setNaturalFieldSource("url", url.toString());
            page->load(record); // record->generate();
            page->active();     // record->active();
        });

        return page;
    }

#if !defined(QT_NO_UITOOLS)
    QObject *WebPage::createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
    {
        Q_UNUSED(url);
        Q_UNUSED(paramNames);
        Q_UNUSED(paramValues);
        QUiLoader loader;
        return loader.createWidget(classId, view());
    }
#endif // !defined(QT_NO_UITOOLS)

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
    void WebPage::handleUnsupportedContent(QNetworkReply *reply)
    {
        QString errorString = reply->errorString();

        if(_loadingurl != reply->url()) {
            // sub resource of this page
            qWarning() << "Resource" << reply->url().toEncoded() << "has unknown Content-Type, will be ignored.";
            reply->deleteLater();
            return;
        }

        if(reply->error() == QNetworkReply::NoError && !reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            errorString = "Unknown Content-Type";
        }

        QFile file(QLatin1String(":/notfound.html"));
        bool isOpened = file.open(QIODevice::ReadOnly);
        Q_ASSERT(isOpened);
        Q_UNUSED(isOpened)

        QString title = tr("Error loading page: %1").arg(reply->url().toString());
        QString html = QString(QLatin1String(file.readAll()))
                       .arg(title)
                       .arg(errorString)
                       .arg(reply->url().toString());

        QBuffer imageBuffer;
        imageBuffer.open(QBuffer::ReadWrite);
        QIcon icon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
        QPixmap pixmap = icon.pixmap(QSize(32, 32));

        if(pixmap.save(&imageBuffer, "PNG")) {
            html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                         QString(QLatin1String(imageBuffer.buffer().toBase64())));
        }

        QList<QWebEngineFrame *> frames;
        frames.append(mainFrame());

        while(!frames.isEmpty()) {
            QWebEngineFrame *frame = frames.takeFirst();

            if(frame->url() == reply->url()) {
                frame->setHtml(html, reply->url());
                return;
            }

            QList<QWebEngineFrame *> children = frame->childFrames();

            foreach(QWebEngineFrame *frame, children)
                frames.append(frame);
        }

        if(_loadingurl == reply->url()) {
            mainFrame()->setHtml(html, reply->url());
        }
    }
#endif

    void WebPage::authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
    {

        if(globalparameters.entrance()->window_list().count() == 0) {
            globalparameters.entrance()->new_dockedwindow(QUrl(browser::Browser::_defaulthome));
        }

        Browser *mainWindow = globalparameters.entrance()->activiated_registered().first;    //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(mainWindow);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::PasswordDialog passwordDialog;
        passwordDialog.setupUi(&dialog);

        passwordDialog.iconLabel->setText(QString());
        passwordDialog.iconLabel->setPixmap(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, mainWindow).pixmap(32, 32));

        QString introMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
        introMessage = introMessage.arg(auth->realm()).arg(requestUrl.toString().toHtmlEscaped());
        passwordDialog.introLabel->setText(introMessage);
        passwordDialog.introLabel->setWordWrap(true);

        if(dialog.exec() == QDialog::Accepted) {
            auth->setUser(passwordDialog.userNameLineEdit->text());
            auth->setPassword(passwordDialog.passwordLineEdit->text());
        }
    }

    void WebPage::proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost)
    {
        Q_UNUSED(requestUrl);

        if(globalparameters.entrance()->window_list().count() == 0) {
            globalparameters.entrance()->new_dockedwindow(QUrl(browser::Browser::_defaulthome));
        }

        Browser *mainWindow = globalparameters.entrance()->activiated_registered().first;    //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(mainWindow);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::ProxyDialog proxyDialog;
        proxyDialog.setupUi(&dialog);

        proxyDialog.iconLabel->setText(QString());
        proxyDialog.iconLabel->setPixmap(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, mainWindow).pixmap(32, 32));

        QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
        introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
        proxyDialog.introLabel->setText(introMessage);
        proxyDialog.introLabel->setWordWrap(true);

        if(dialog.exec() == QDialog::Accepted) {
            auth->setUser(proxyDialog.userNameLineEdit->text());
            auth->setPassword(proxyDialog.passwordLineEdit->text());
        }
    }

    WebView::~WebView()
    {
        //        if(_page->record()) {
        //            _page->record()->page(nullptr);
        //            //            globalparameters.getRecordTableScreen()->previousInFocusChain();    //_record;
        //            _page->record(nullptr);
        //        }

        if(_page)_page->deleteLater();
    }


    WebView::WebView(Record *const record, QWebEngineProfile *profile
                     // , bool openinnewtab
                     , TabWidget *parent, RecordTableController *recordtablecontroller)
        : QWebEngineView(static_cast<QWidget *>(parent
                                                // ->parent()
                                               )
                        )
        , _tabmanager(parent)
          //        , _record(record)
        , _recordtablecontroller(recordtablecontroller)
        , _page(new WebPage(profile, record // , openinnewtab
                            , recordtablecontroller
                            , this))
          //        , _initialurl(record ? record->getNaturalFieldSource("url") : QUrl())
        , _progress(0)
        , _iconreply(0)
    {
        //        if(record) {record->view(this);}

        connect(this, &WebView::loadProgress, this, &WebView::setProgress);

        //    connect(this->webPage(), &WebPage::loadFinished, this, &WebView::onLoadFinished);
        //    connect(this->webPage(), &WebPage::titleChanged, this, &WebView::onTitleChanged);
        //    connect(this->webPage(), &WebPage::urlChanged, this, &WebView::onUrlChanged);
        //    //    &WebPage::titleChanged(const QString &title);
        //    //    &WebPage::urlChanged(const QUrl &url);

        QWebEngineView::setPage(_page);

#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), SIGNAL(statusBarMessage(QString)),
                SLOT(setStatusBarText(QString)));
#endif
        connect(page(), SIGNAL(loadingUrl(QUrl)), this, SIGNAL(urlChanged(QUrl)));
        connect(page(), SIGNAL(iconUrlChanged(QUrl)), this, SLOT(onIconUrlChanged(QUrl)));
        connect(page(), &WebPage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif


        connect(this->_page, &WebPage::loadFinished, this, &WebView::onLoadFinished);

        //    &WebPage::titleChanged(const QString &title);
        //    &WebPage::urlChanged(const QUrl &url);
        //        _page->load(_record->getField("url"));    // auto  loaded
        setFocus();
    }

    QSet<Record *> WebPage::binded_records()const
    {
        //        Record *record = _record;   // maybe invalid


        //        while(record && record->_page && record->_page != this) {
        //            record = record->_page->_record;

        //        }

        //        //        if(_record && _record->_page)
        //        //            record = _record->_page->_record;

        //        return
        //            records;    // (url().isValid()) ? check_record(url()) : nullptr;
        return _records;
    }

    void WebPage::break_record_which_page_point_to_me()
    {
        // what _record point to is a stack variable, it's address may be not correct! especially when it was destoried
        if(_record) {
            if(_record->binded_only_page()) {
                if(_record->binded_only_page() == this) {
                    _record->page_to_nullptr();   // _record->_page = nullptr; // _record->bind_page(nullptr);
                }
            }

            if(_records.contains(_record)) _records.remove(_record);
        }

        _record = nullptr;
    }

    Record *WebPage::bind_record(Record *const record)
    {
        //        Record *new_record = nullptr;
        //        RecordTableData *data = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();
        //        assert(data);

        //        if(_openinnewtab) {
        //            Record *newtab  = data->record(QUrl("about:newtab"));
        //            new_record = newtab ? newtab : register_record(QUrl("about:newtab"));
        //            _openinnewtab = false;
        //        } else {

        //            Record *record  = data->record(url);
        //            Record *blank   = data->record(QUrl(DockedWindow::_defaulthome));

        //            new_record = record ? record
        //                         : blank ? blank
        //                         : ::register_record(url, view()->recordtablecontroller());
        //        }

        //        if( // !_page->record() ||
        //            this->_openinnewtab
        //            // ||  record != nullptr   // history navigation
        //        ) {
        //            // new_record = record ? record : ::register_record(url, _recordtablecontroller);
        //            //            new_record->page(_page);
        //            //            _page->record(new_record);

        //            // if(_page->_openinnewtab)
        //            this->_openinnewtab = false;
        //        }

        if(_record != record) {
            //            if(_record) {
            //                if(_record->binded_page() != nullptr && _record->binded_page() == this) {
            //                    // assert(_record->linkpage() == this);    // should always be true -> if not move note

            //                    // _record->linkpage(nullptr);
            //                    _record->breakpage();

            //                    // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
            //                }
            //            }

            this->break_record_which_page_point_to_me();

            //            if(record) {
            //                if((!record->binded_page()) || (record->binded_page() != this)) {
            //                    // assert(_record->linkpage() == this);    // should always be true -> if not move note

            //                    // _record->linkpage(nullptr);
            //                    _record = record->bind_page(this);  // record->breakpage();
            //                    // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
            //                }
            //            }

            //        else {
            //            _record->breakpage();
            //        }

            //        _record = record;   // ->bind_page(this);   // just for close tab(==view)

            // _record->linkpage(this);

            // connect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});

            //        WebView   *view = record->page()->view();
            //        view->tabmanager()->setCurrentWidget(view);
            //        view->show();
            //        // _page->record(new_record);
            //        update_record();
            _record = record;
        }

        //        else {
        //            if(_record)_record->bind_page(this);
        //        }

        if(_record) {
            if((!_record->binded_only_page()) || (_record->binded_only_page() != this)) {
                // assert(_record->linkpage() == this);    // should always be true -> if not move note

                // _record->linkpage(nullptr);
                _record->bind_page(this);  // record->breakpage();
                // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
                _records.insert(_record);
                //                MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");
                //                assert(metaeditor);
                //                metaeditor->bind(_record);
            }
        }

        return _record;
    }

    void WebPage::onTitleChanged(const QString &title)
    {
        //        assert(this->url() != QUrl());
        assert(this->title() == title);

        //        Record *record = nullptr;
        //        //        RecordTableData *data = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();
        //        //        record = data->record(this->url());

        //        //        if(!record)::register_record(this->url());
        //        record = request_record(url());

        //        bind_record(record);
        if(_record) {
            if(url() != QUrl()
               && !url().host().isEmpty()
               && !url().scheme().isEmpty()
               && url() != QUrl(Browser::_defaulthome)
               && url() == _loadingurl
               && url().toString() == _record->getNaturalFieldSource("url")
              ) {

                //                if(//_state_check[_record->getNaturalFieldSource("pin")] == Qt::Unchecked &&
                //                    url().toString() == _record->getNaturalFieldSource("url")
                //                ) {
                //                    // if(this->record())

                //                    //        if(url() == _loadingurl)

                //                update_record(url(), title, true);

                MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");
                assert(metaeditor);

                if(title != ""
                   && title != _record->getNaturalFieldSource("name")
                   //                   && !QUrl(title).isValid()
                  ) {

                    _record->setNaturalFieldSource("name", title);
                    metaeditor->setName(title);
                }

                //                }
            }
        }
    }


    void WebView::onLoadFinished(bool success)
    {
        if(success && 100 != _progress) {
            qWarning() << "Received finished signal while progress is still:" << progress()
                       << "Url:" << _page->url();
        }

        if(success) {
            //            assert(_page->url() != QUrl());

            //            if(_page->record()->getNaturalFieldSource("url") != DockedWindow::_defaulthome)

            //            if(_page->record()) {

            //                Record *r = nullptr;

            //                while((r = register_record(QUrl(DockedWindow::_defaulthome)))) {
            //                    //                //r->page(_page);
            //                    //                //_page->record(r);
            //                    globalparameters.getRecordTableScreen()->getRecordTableController()->removeRowById(r->getNaturalFieldSource("id"));
            //                    update_recordtableview(_page->url(), _page->title());
            //                }

            //                //_page->setUrl(_page->url());
            //            }





            //            if(! _page->record()) {
            //                Record *record = nullptr;
            //                //            RecordTableData *data = _recordtablecontroller->getRecordTableModel()->getRecordTableData();
            //                //            record = data->record(_page->url());

            //                //            if(!record)::register_record(_page->url());
            //                record = request_record(_page->url());

            //                if(record)_page->bind_record(record);
            //            } else {

            //                //            if(_page->record()) {
            //                //                _page->update_record(_page->url(), _page->title());
            //                //            }
            _page->update_record(_page->url(), _page->title(), false);
            //            }




        }

        _progress = 0;
    }

    // This signal is emitted with the URL of the main frame when the main frame's title is received. The new URL is specified by url.
    void WebPage::onUrlChanged(const QUrl &url)
    {
        //        assert(_page->url() != QUrl());
        //        assert(url != QUrl());

        //        assert(_page->url() == url);
        if(_record
           //           && url == _loadingurl
          ) {

            if(url != QUrl()
               && !url.host().isEmpty()
               && !url.scheme().isEmpty()
               && url != QUrl(Browser::_defaulthome)
               && url != _loadingurl
               && url.toString() != _record->getNaturalFieldSource("url")
              ) {

                //            if(!_record) {
                //                Record *record = nullptr;
                //                //            RecordTableData *data = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();
                //                //            record = data->record(url);
                //                //            if(!record)::register_record(url);

                //                record = request_record(url);

                //                //if(this->record()) {

                //                if(_state_check[_record->getNaturalFieldSource("pin")] == Qt::Checked
                //                   && url.toString() != _record->getNaturalFieldSource("url")
                //                  ) {
                //                    //                TreeScreen *treescreen = globalparameters.getTreeScreen();

                //                    //                if(treescreen) {
                //                    //                    // Получение индекса выделенной строки
                //                    //                    QModelIndex index = treescreen->getCurrentItemIndex();

                //                    //                    // Введенные данные добавляются
                //                    //                    treescreen->insBranchProcess(index, title(), false);
                //                    //                    treescreen->move_dn_branch();
                //                    //                    //load(request_record(url));  //
                //                    //                    globalparameters.entrance()->invoke(request_record(url));
                //                    //                    // return;
                //                    //                }

                //                    //                    Record *record = request_record(url);
                //                    //                    bind_record(record);

                //                    //                    load(request_record(url));

                //                    request_record(url
                //                                   , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>(
                //                                       ""
                //                                       , &TabWidget::active_record_in_new_tab::operator()
                //                                       , boost::make_shared<TabWidget::active_record_in_new_tab>(this->view()->tabmanager(), true))
                //                                  );

                //                } else {

                //                //}



                //                //            //if(!_page->record()) {
                //                //            bind_record(url);

                //                //            // when Ctrl + hyperlink generated page load new url:
                //                //            if(_page->url() != url && url != QUrl(DockedWindow::_defaulthome)) {
                //                //                //auto dp = globalparameters.entrance()->invoke_view(r);
                //                //                //if(dp.second == this) {
                //                //                _page->setUrl(url);
                //                //                //}
                //                //            }


                //                bind_record(record);
                //            } else {

                //                //if(this->record())
                // setUrl(url); // recursive
                MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");
                assert(metaeditor);

                if(url.toString() != "") {
                    _record->setNaturalFieldSource("url", url.toString());
                    metaeditor->setUrl(url.toString());
                }

                //                    update_record(url, title(), true);


                //                //            //}
                //            }
                //                }

            }
        }

        //        if(//_initialurl == QUrl() &&
        //            url.isValid()
        //            && _page->record() == nullptr
        //        ) {  // never in
        //            Record *record = register_record(url);
        //            _page->load(record);    // _page->load(url) already invoked?
        //            // openLinkInNewTab(url);
        //            update_recordtableview(url, _page->title());
        //        }
    }


    void WebPage::update_record(const QUrl &url
                                , const QString &title
                                , bool make_current)         //int pos = recordTableController->getFirstSelectionPos();
    {
        Q_UNUSED(make_current)

        //        //if(globalparameters.getRecordTableScreen()->getRecordTableController()) {

        //        //        RecordTableController *_recordtablecontroller = globalparameters.getRecordTableScreen()->getRecordTableController();
        //        //            //Record *_record = nullptr;
        //        //            //RecordTableModel *recordtablemodel = _recordtablecontroller->getRecordTableModel();
        //        RecordTableView *recordtableview = _recordtablecontroller->getView();
        //        //            //RecordTableData *recordtabledata = recordtablemodel->getRecordTableData();

        //        //        if(!_record) {
        //        //            //            Record *record = nullptr;
        //        //            RecordTableData *recordtabledata = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();

        //        //            if(recordtabledata)_record = recordtabledata->getRecordByUrl(url());
        //        //        }

        //        //        Record *record = this->record();

        if(_record) {
            //            _record->active_immediately(true);

            //            //            WebView *view = record->page()->view();
            //            //            view->tabmanager()->setCurrentWidget(view);
            //            //            view->show();

            //            //                //MetaEditor *metaeditor = ::globalparameters.getMetaEditor();
            //            QModelIndex proxyindex = _recordtablecontroller->convertIdToProxyIndex(_record->getNaturalFieldSource("id"));
            //            //                //QModelIndex sourceindex = _recordtablecontroller->convertIdToSourceIndex(_page->record()->getField("id"));
            //            int position = _recordtablecontroller->convertProxyIndexToPos(proxyindex);
            //            //                // QString page_title = webPage()->title();    // same as this->title()


            // Выясняется указатель на объект редактирования текста записи
            MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");

            // Выясняется ссылка на таблицу конечных данных
            //                RecordTableData *table = recordSourceModel->getTableData();

            if(title != ""
               && title != _record->getNaturalFieldSource("name")
               //                   && !QUrl(title).isValid()
              ) {

                _record->setNaturalFieldSource("name", title);
                //metaeditor->setName(title);


                //                table->setWorkPos(pos);
                metaeditor->setName(title);
            }

            if(url.toString() != "") {
                _record->setNaturalFieldSource("url", url.toString());
                metaeditor->setUrl(url.toString());

            }

            //metaeditor->setUrl(url.toString());

            //_page->load(url);

            //if(recordtableview) {


            //recordtableview->update();//repaint();    // does not work
            //recordtableview->adjustSize();    // does not work, size weird

            //            if(make_current && recordtableview) {
            //                recordtableview->setSelectionToPos(position); // work






            this->view()->setFocus();   // make upate validate

            if(make_current) // globalparameters.mainwindow()
                _recordtablecontroller->select_id(_record->getNaturalFieldSource("id"));

            //            if(_record->_active_request) {
            //                if(_record->_openlinkinnewwindow == 1) {

            //                }

            //                DockedWindow *win = view()->tabmanager()->window();

            //                if(!win->isActiveWindow() || !win->isVisible()) {
            //                    win->raise();
            //                    win->activateWindow();
            //                }

            //                view()->tabmanager()->setCurrentWidget(view());
            //                view()->show();
            //            }






            //globalParameters.getRecordTableScreen()->update();//repaint();    // does not work
            //emit recordtableview->clicked(proxyindex);  // recordtableview->clickToRecord(proxyindex);    // does not work
            //_recordtablecontroller->clickToRecord(proxyindex);    // does not work
            //recordtablemodel->dataChanged();
            //            }

            //}

            //            int row = sourceindex.row();
            //            int column = sourceindex.column();
            //            int _row = proxyindex.row();
            //            int _column = proxyindex.column();
            //            qDebug() << "qDebug()\t" << row << "\t" << column << "\t" << _row << "\t" << _column << "\t" << "\n";    // does not work?

            //std::cout << "std::cout\t" << row << "\t" << column << "\t" << _row << "\t" << _column << "\t" << "\n";
            //_recordtablecontroller->getRecordTableModel()->setData(sourceindex, QVariant::fromValue(this->title()), Qt::EditRole);

            //            _recordtablecontroller->editField(position
            //                                              , _record->getNaturalFieldSource("name")
            //                                              , _record->getNaturalFieldSource("author")
            //                                              , _record->getNaturalFieldSource("url")
            //                                              , _record->getNaturalFieldSource("tags")
            //                                              );    // does not work
        }

        //        WebView *view = record->page()->view();

        //        if(view->hasFocus()) {
        //            view->tabmanager()->setCurrentWidget(view);
        //            view->show();
        //        }

        //}
    }




    void WebView::page(WebPage *_page)
    {
        if(_page)
            _page->deleteLater();

        this->_page = _page;
        QWebEngineView::setPage(_page);
#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), SIGNAL(statusBarMessage(QString)),
                SLOT(setStatusBarText(QString)));
#endif
        connect(page(), SIGNAL(loadingUrl(QUrl)), this, SIGNAL(urlChanged(QUrl)));
        connect(page(), SIGNAL(iconUrlChanged(QUrl)), this, SLOT(onIconUrlChanged(QUrl)));
        connect(page(), &WebPage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif


        connect(this->_page, &WebPage::loadFinished, this, &WebView::onLoadFinished);
        connect(this->_page, &WebPage::titleChanged, this->_page, &WebPage::onTitleChanged);
        connect(this->_page, &WebPage::urlChanged, this->_page, &WebPage::onUrlChanged);
        //    &WebPage::titleChanged(const QString &title);
        //    &WebPage::urlChanged(const QUrl &url);

    }


    void WebView::contextMenuEvent(QContextMenuEvent *event)
    {
#if defined(QWEBENGINEPAGE_HITTESTCONTENT)
        QWebEngineHitTestResult r = page()->hitTestContent(event->pos());

        if(!r.linkUrl().isEmpty()) {
            QMenu menu(this);
            menu.addAction(pageAction(QWebEnginePage::OpenLinkInNewWindow));
            menu.addAction(tr("Open in New Tab"), this, SLOT(openLinkInNewTab()));
            menu.addSeparator();
            menu.addAction(pageAction(QWebEnginePage::DownloadLinkToDisk));
            // Add link to bookmarks...
            menu.addSeparator();
            menu.addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));

            if(page()->settings()->testAttribute(QWebEngineSettings::DeveloperExtrasEnabled))
                menu.addAction(pageAction(QWebEnginePage::InspectElement));

            menu.exec(mapToGlobal(event->pos()));
            return;
        }

#endif
        QWebEngineView::contextMenuEvent(event);
    }


    void WebView::wheelEvent(QWheelEvent *event)
    {
#if defined(QWEBENGINEPAGE_SETTEXTSIZEMULTIPLIER)

        if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
            int numDegrees = event->delta() / 8;
            int numSteps = numDegrees / 15;
            setTextSizeMultiplier(textSizeMultiplier() + numSteps * 0.1);
            event->accept();
            return;
        }

#endif
        QWebEngineView::wheelEvent(event);
    }

    void WebView::openLinkInNewTab()
    {
        pageAction(QWebEnginePage::OpenLinkInNewTab)->trigger();
    }


    void WebView::openLinkInNewTab(const QUrl &_url)
    {
        Q_UNUSED(_url);

#if defined(QWEBENGINEPAGE_WEBACTION_OPENLINKINNEWWINDOW)
        _page->_openinnewtab = true;
        pageAction(QWebEnginePage::OpenLinkInNewWindow)->trigger();
#endif

        //        //QString url(QString(""));
        //        //QUrl _url = globalParameters.getBrowserView()->mainWindow()->currentTab()->url();

        //        //    if(!_recordtablecontroller) {
        //        //        //RecordTableController *
        //        //        _recordtablecontroller = globalParameters.getRecordTableScreen()->getRecordTableController();
        //        //    }

        //        //    RecordTableData *recordTableData = _recordtablecontroller->getRecordTableModel()->getRecordTableData();

        //        //if(_url.isValid() && _record == nullptr) {
        //        _page->record(bind_record(_url));

        //        //        QModelIndex proxyindex = _recordtablecontroller->convertIdToProxyIndex(_page->record()->getField("id"));
        //        //        int position = _recordtablecontroller->convertProxyIndexToPos(proxyindex);
        //        //        _recordtablecontroller->getView()->setSelectionToPos(position);
        setFocus();
        //        globalparameters.mainwindow()
        _recordtablecontroller->select_id(_page->current_record()->getNaturalFieldSource("id"));

        //}

    }


    void WebView::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
    {
        FeaturePermissionBar *permissionBar = new FeaturePermissionBar(this);
        connect(permissionBar, &FeaturePermissionBar::featurePermissionProvided, page(), &QWebEnginePage::setFeaturePermission);

        // Discard the bar on new loads (if we navigate away or reload).
        connect(page(), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

        permissionBar->requestPermission(securityOrigin, feature);
    }


    void WebView::setProgress(int progress)
    {
        _progress = progress;
    }


    //    void PageView::loadUrl(const QUrl &url)
    //    {
    //        //        _initialurl = url;
    //        _page->load(url);   // load(url);
    //    }


    QString WebView::lastStatusBarText() const
    {
        return _statusbartext;
    }


    //    QUrl PageView::url() const
    //    {
    //        QUrl url = QWebEngineView::url();

    //        if(!url.isEmpty())
    //            return url;

    //        return _initialurl;
    //    }


    QIcon WebView::icon() const
    {
        if(!_icon.isNull())
            return _icon;

        return QtSingleApplication::instance()->defaultIcon();
    }


    void WebView::onIconUrlChanged(const QUrl &url)
    {
        QNetworkRequest iconRequest(url);
        _iconreply = QtSingleApplication::networkAccessManager()->get(iconRequest);
        _iconreply->setParent(this);
        connect(_iconreply, SIGNAL(finished()), this, SLOT(iconLoaded()));
    }


    void WebView::iconLoaded()
    {
        _icon = QIcon();

        if(_iconreply) {
            QByteArray data = _iconreply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);
            _icon.addPixmap(pixmap);
            _iconreply->deleteLater();
            _iconreply = 0;
        }

        emit iconChanged();
    }


    void WebView::mousePressEvent(QMouseEvent *event)
    {
        _page->_pressedbuttons = event->buttons();
        _page->_keyboardmodifiers = event->modifiers();
        QWebEngineView::mousePressEvent(event);
    }


    void WebView::mouseReleaseEvent(QMouseEvent *event)
    {
        QWebEngineView::mouseReleaseEvent(event);

        if(!event->isAccepted() && (_page->_pressedbuttons & Qt::MidButton)) {
            QUrl url(QApplication::clipboard()->text(QClipboard::Selection));

            if(!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
                setUrl(url);
            }
        }
    }


    void WebView::setStatusBarText(const QString &string)
    {
        _statusbartext = string;
    }


    WebView *WebView::load(Record *const record)
    {
        //        _page->record(record);
        return _page->load(record);  //        loadUrl(QUrl(record->getNaturalFieldSource("url")));

    }


    void WebView::switch_show()
    {
        _tabmanager->setCurrentIndex(_tabmanager->webViewIndex(this));
        _page->load(_page->current_record());
        show();
    }

}





