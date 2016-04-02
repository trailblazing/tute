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


#include "libraries/qt_single_application5/qtsingleapplication.h"

#include "browser.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "featurepermissionbar.h"
#include "networkaccessmanager.h"
#include "ui_passworddialog.h"
#include "ui_proxy.h"
#include "tabwidget.h"
#include "webview.h"
#include "views/main_window/MainWindow.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/WalkHistory.h"
#include "models/attach_table/AttachTableData.h"
#include "views/tree/KnowView.h"
#include "models/tree/KnowModel.h"

#include <QtGui/QClipboard>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QWebEnginePage>

#if defined(QWEBENGINEPAGE_HITTESTCONTENT)
#include <QWebEngineHitTestResult>
#endif

#ifndef QT_NO_UITOOLS
#include <QtUiTools/QUiLoader>
#endif  //QT_NO_UITOOLS

#include <QtCore/QDebug>
#include <QtCore/QBuffer>

#include "main.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "models/record_table/Record.h"
#include "views/record_table/RecordView.h"
#include "libraries/GlobalParameters.h"
#include "views/browser/entrance.h"
#include "views/record_table/RecordScreen.h"
#include "controllers/record_table/RecordController.h"
//#include "browserview.moc"
#include "libraries/GlobalParameters.h"
#include "views/record/MetaEditor.h"
#include "models/tree/TreeItem.h"
#include "views/tree/TreeScreen.h"
#include "views/browser/browser.h"



#include <iostream>

class GlobalParameters;
extern GlobalParameters globalparameters;
extern AppConfig appconfig;

namespace browser {


#ifdef USE_POPUP_WINDOW

    PopupPage::PopupPage(QWebEngineProfile *profile, QObject *parent)
        : QWebEnginePage(profile, parent)
        , m_keyboardModifiers(Qt::NoModifier)
        , m_pressedButtons(Qt::NoButton)
    {
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
        setNetworkAccessManager(BrowserApplication::networkAccessManager());
#endif
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        connect(this, &PopupPage::unsupportedContent, this, &PopupPage::handleUnsupportedContent;
#endif
                connect(this, &PopupPage::authenticationRequired, &PopupPage::authenticationRequired);
                connect(this, &PopupPage::proxyAuthenticationRequired, &PopupPage::proxyAuthenticationRequired);
    }

    Browser *PopupPage::browser()
    {
        QObject *w = this->parent();

        while(w) {
            if(Browser *mw = qobject_cast<Browser *>(w))
                return mw;

            w = w->parent();
        }

        return globalparameters.entrance()->equip_registered().first;   // BrowserApplication::instance()->mainWindow();
    }

    bool PopupPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
    {
        Q_UNUSED(type);

        if(isMainFrame) {
            m_loadingUrl = url;
            emit loadingUrl(m_loadingUrl);
        }

        return true;
    }

    bool PopupPage::certificateError(const QWebEngineCertificateError &error)
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


    //#include "webview.moc"

    //    QWebEnginePage *PopupPage::createWindow(QWebEnginePage::WebWindowType type)
    //    {
    //        if(type == QWebEnginePage::WebBrowserTab) {
    //            return mainWindow()->tabWidget()->newTab()->page();
    //        } else if(type == QWebEnginePage::WebBrowserWindow) {
    //            BrowserApplication::instance()->newMainWindow();
    //            Browser *mainWindow = BrowserApplication::instance()->mainWindow();
    //            return mainWindow->currentTab()->page();
    //        } else {
    //            PopupWindow *popup = new PopupWindow(profile());
    //            popup->setAttribute(Qt::WA_DeleteOnClose);
    //            popup->show();
    //            return popup->page();
    //        }
    //    }

#if !defined(QT_NO_UITOOLS)
    QObject *PopupPage::createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
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

        if(m_loadingUrl != reply->url()) {
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

        if(m_loadingUrl == reply->url()) {
            mainFrame()->setHtml(html, reply->url());
        }
    }
#endif

    void PopupPage::authenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
    {
        Browser *mainWindow = globalparameters.entrance()->activiated_registered().first;

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
            QByteArray key = QtSingleApplication::authenticationKey(requestUrl, auth->realm());
            auth->setUser(passwordDialog.userNameLineEdit->text());
            auth->setPassword(passwordDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            QtSingleApplication::instance()->setLastAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

    void PopupPage::proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost)
    {
        Q_UNUSED(requestUrl);
        Browser *mainWindow = globalparameters.entrance()->activiated_registered().first;

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
            QString user = proxyDialog.userNameLineEdit->text();
            QByteArray key = QtSingleApplication::proxyAuthenticationKey(user, proxyHost, auth->realm());
            auth->setUser(user);
            auth->setPassword(proxyDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            QtSingleApplication::instance()->setLastProxyAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

#endif // USE_POPUP_WINDOW



    WebPage::WebPage(QWebEngineProfile *profile
                     , const boost::intrusive_ptr<TreeItem> _item
                     , Browser              *_browser
                     , TabWidget            *_tabmanager
                     , RecordController     *_record_controller
                     , WebView              *parent
                    )
        : QWebEnginePage(profile, parent)
        , _browser(_browser)
        , _tabmanager(_tabmanager)
        , _record_controller(_record_controller)
        , _view(parent)
        , _keyboardmodifiers(Qt::NoModifier)
        , _pressedbuttons(Qt::NoButton)
          //        , _openinnewtab(
          //              openinnewtab  //false
          //          )
          //, _create_window_generated(false)
        , _record_binder(boost::intrusive_ptr<CouplerDelegation>(new CouplerDelegation(std::make_shared<WebPage::Coupler>(this, _item)))
                         //              [ & ]()
                         //    {
                         //        boost::intrusive_ptr<TreeItem> item_result;

                         //        if(!_item->record_binder()) {
                         //            item_result = item_request_from_tree(_item);    // _record_controller->   // may be recursive calling!
                         //        } else {
                         //            item_result = item_equip_registered(_item);
                         //        }

                         //        if(item_result->is_lite())item_result->to_fat();

                         //        return item_result->record_binder();
                         //    }()
                        )
    {
        //        assert(url != nullptr);

        //        if(record != nullptr) {
        //            // record->page(this);

        //            bind_record(record);
        //        }

        settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
        setNetworkAccessManager(QtSingleApplication::networkAccessManager());
#endif
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        connect(static_cast<QWebEnginePage *const>(this), &QWebEnginePage::unsupportedContent, this, &WebPage::handleUnsupportedContent);
#endif
        connect(static_cast<QWebEnginePage *const>(this), &QWebEnginePage::authenticationRequired, this, &WebPage::authenticationRequired);
        connect(static_cast<QWebEnginePage *const>(this), &QWebEnginePage::proxyAuthenticationRequired, this, &WebPage::proxyAuthenticationRequired);

        connect(static_cast<QWebEnginePage *const>(this), &QWebEnginePage::titleChanged, this, &WebPage::onTitleChanged);
        connect(static_cast<QWebEnginePage *const>(this), &QWebEnginePage::urlChanged, this, &WebPage::onUrlChanged);
        connect(this, &WebPage::close_requested, &WebPage::on_close_requested);
        //        if(record)QWebEnginePage::load(record->getNaturalFieldSource("url"));

        //        if(url != nullptr) {

        //        _item->active();

        //            load(url);
        //        }

        //        this->_record_controller->addnew_item_fat(requested_item);

    }

    WebPage::~WebPage()
    {
        //        break_page_shared_items();   // move to on_close_requested()
    }


    void WebPage::update_record_view(boost::intrusive_ptr<TreeItem> item)
    {
        _record_controller->update_record_view(item);

        //        //        auto tab_manager = view()->tabmanager();
        //        auto source_model = _record_controller->source_model(); // tab_manager->source_model();

        //        if(!source_model->is_item_exists(item->field("id"))) {
        //            //        auto records = table_data->records();
        //            //        records.append(_record);    //
        //            if(item->is_lite())item->to_fat();

        //            //            QModelIndex index = _record_controller->pos_to_sourceindex(source_model->size());
        //            _record_controller->addnew_item_fat(item);    //index,
        //            //            tab_manager->reset_tabledata(table_data);
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
        return _browser;
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

    //    void WebPage::setUrl(const QUrl &url)
    //    {
    //        QWebEnginePage::setUrl(url);    // load(QUrl(_record->getNaturalFieldSource("url")));
    //    }


    WebView *WebPage::load(const boost::intrusive_ptr<TreeItem> item, bool checked)
    {
        //        Q_UNUSED(checked)
        assert(item);

        //        //        if(_record)
        //        //            if(_record->getNaturalFieldSource("url") != DockedWindow::_defaulthome) {
        //        // _record = record;
        //        if(
        //            (_item != item)
        //            //            || ((_record == record) && (_record->_page != record->_page))
        //            //            (!_record)
        //            //            || (
        //            //                _record && (_record->getNaturalFieldSource("url") != record->getNaturalFieldSource("url"))
        //            //            )
        //        ) {

        //            bind(item);
        //        }

        //        assert(_item);

        //        //        QUrl _url = QUrl(_record->getNaturalFieldSource("url"));

        //        //        if(_pageview &&
        //        //           //            _record && (_record->getNaturalFieldSource("url") != record->getNaturalFieldSource("url"))
        //        //           url() != _url
        //        //          ) {
        //        //            triggerAction(QWebEnginePage::Stop);

        //        //            // QUrl url = QUrl(_record->getNaturalFieldSource("url"));
        //        //            setUrl(_url);    // load(QUrl(_record->getNaturalFieldSource("url")));

        //        //            QWebEnginePage::load(_url);
        //        //            //            }
        //        //            //            update_record(_url, title(), checked);

        //        //        }

        //        //        return active();

        //        //        _record->active_immediately(true);

        auto it = item_request_from_tree(
                      item
                      , std::bind(&TreeScreen::view_paste_as_child, globalparameters.tree_screen(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
                  );

        if(checked)it->activate();

        return _view;
    }

    WebView *WebPage::activate()
    {
        QString _url_str = _record_binder->bounded_item()->field("url");
        QUrl _url = QUrl(_url_str);

        if(_view    // && _loadingurl.isValid()  // && url().isValid()    // && url().toString() != _url_str
           && _url_str != Browser::_defaulthome  // url() may be nothing
           && _url != _loadingurl
          ) {
            //triggerAction(QWebEnginePage::Stop);

            // QUrl url = QUrl(_record->getNaturalFieldSource("url"));

            QWebEnginePage::setUrl(_url);    // setUrl(_url);   // load(QUrl(_record->getNaturalFieldSource("url")));

            //            QWebEnginePage::load(_url);


            //            }
            //            update_record(_url, title(), checked);

        }

        if(_record_binder->bounded_item()) {  // ->_active_request

            if(_record_binder->bounded_item()->_open_link_in_new_window == 1) {

            }

            //            Browser *_browser = browser();
            assert(_browser);

            if(!_browser->isActiveWindow() || !_browser->isVisible()) {
                _browser->raise();
                _browser->activateWindow();
            }

            auto _tree_screen = globalparameters.tree_screen();
            _tree_screen->tree_view()->select_and_current(_record_binder->bounded_item());

            if(_url_str != Browser::_defaulthome) {    // && _loadingurl.isValid()   // && _loadingurl == _url

                if(_record_controller->first_selectionid() != _record_binder->bounded_item()->field("id") || _view->tabmanager()->currentWebView() != _view) {
                    _tabmanager->setCurrentWidget(_view);
                    _view->show();
                    //        if(checked) // globalparameters.mainwindow()
                    _view->setFocus();   // make upate validate

                    //assert(_lineedits);

                    //if(_lineedits) {
                    QLineEdit *line_edit = _tabmanager->currentLineEdit();  //qobject_cast<QLineEdit *>(_lineedits->currentWidget());

                    if(line_edit)line_edit->setText(_url_str);

                    //}

                    if(_record_controller->view()->selection_first_id() != _record_binder->bounded_item()->field("id")) {
                        _record_controller->select_id(_record_binder->bounded_item()->field("id"));
                    }
                }
            }



        }

        return _view;
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

        QMessageBox::critical(_view, tr("Certificate Error"), error.errorDescription(), QMessageBox::Ok, QMessageBox::NoButton);
        return false;

    }

    //    class PopupWindow : public QWidget {
    //        Q_OBJECT
    //    public:
    //        PopupWindow(QWebEngineProfile *profile)
    //            : m_addressBar(new QLineEdit(this))
    //            , m_view(new PopupView(this))
    //        {
    //            m_view->setPage(new PopupPage(profile, m_view));
    //            QVBoxLayout *layout = new QVBoxLayout;
    //            layout->setMargin(0);
    //            setLayout(layout);
    //            layout->addWidget(m_addressBar);
    //            layout->addWidget(m_view);
    //            m_view->setFocus();

    //            connect(m_view, &PopupView::titleChanged, this, &QWidget::setWindowTitle);
    //            connect(m_view, &PopupView::urlChanged, this, &PopupWindow::setUrl);
    //            connect(page(), &PopupView::geometryChangeRequested, this, &PopupWindow::adjustGeometry);
    //            connect(page(), &PopupView::windowCloseRequested, this, &QWidget::close);
    //        }

    //        QWebEnginePage *page() const { return m_view->page(); }

    //    private Q_SLOTS:
    //        void setUrl(const QUrl &url)
    //        {
    //            m_addressBar->setText(url.toString());
    //        }

    //        void adjustGeometry(const QRect &newGeometry)
    //        {
    //            const int x1 = frameGeometry().left() - geometry().left();
    //            const int y1 = frameGeometry().top() - geometry().top();
    //            const int x2 = frameGeometry().right() - geometry().right();
    //            const int y2 = frameGeometry().bottom() - geometry().bottom();

    //            setGeometry(newGeometry.adjusted(x1, y1 - m_addressBar->height(), x2, y2));
    //        }

    //    private:
    //        QLineEdit *m_addressBar;
    //        PopupView *m_view;

    //    };

    //#include "webview.moc"

    QWebEnginePage *WebPage::createWindow(QWebEnginePage::WebWindowType type)
    {
        //        Record *blank_record = request_record(QUrl(DockedWindow::_defaulthome));    // QUrl("about:newtab")

        //        QWebEnginePage
        WebPage *page = nullptr;


        //        else
        if(type == QWebEnginePage::WebBrowserWindow) {

            Browser *browser = globalparameters.entrance()->new_browser(QUrl(Browser::_defaulthome));                 // QtSingleApplication::instance()->newMainWindow();
            //            DockedWindow *mainWindow = globalparameters.entrance()->activiated_registered().first;  // QtSingleApplication::instance()->mainWindow();
            //            return

            page = browser->tabmanager()->currentWebView()->page();  // mainWindow->currentTab()->page();

        } else

#ifdef USE_POPUP_WINDOW
            if(//_openinnewtab ||
                type == QWebEnginePage::WebBrowserTab)
#endif  // USE_POPUP_WINDOW

            {
                // always open new window in new tab

                // _openinnewtab = false;  // true


                // assert(_record);

                QUrl current = url();
                QUrl requestedurl = requestedUrl(); //requestedUrl
                //            QString previous_url = _record->getNaturalFieldSource("url");    // the parent page that host the new link

                //            return globalparameters.entrance()->activebrowser()->tabWidget()->newTab()->page();

                //assert(url == DockedWindow::_defaulthome);
                //Record *new_record = register_record(QUrl(DockedWindow::_defaulthome));    // do not modify current this->_record!
                //assert(_record->page() == this); //_record->page(this);

                // return nullptr;

                WebView *view = globalparameters.entrance()->find([](boost::intrusive_ptr<const TreeItem> it) {return it->field("url") == Browser::_defaulthome;});

                if(view) {
                    //                return view->page();
                    //                    auto ar = boost::make_shared<WebPage::ActiveRecordBinder>(view->page());

                    auto record = view->page()->item_request_from_tree(
                                      QUrl(Browser::_defaulthome)
                                      , std::bind(&TreeScreen::view_paste_as_sibling, globalparameters.tree_screen(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
                                  );
                    //                record->generate();
                    record->activate();
                    page = view->page();
                } else {

                    // Page *page = this->dockedwindow()->tabWidget()->new_view(new_record, true)->page();
                    // page->bind_record(new_record);  //Record *record = request_record(page->url());
                    // page->update_record(page->url(), page->title());

                    // already create window, why do this? -- refer to demo browser
                    //                    auto arint = boost::make_shared<TabWidget::ActiveRecordBinder>(browser()->tabWidget(), true);
                    boost::intrusive_ptr<TreeItem> r = _browser->tabWidget()->item_request_from_tree(
                                                           QUrl(Browser::_defaulthome)
                                                           , std::bind(&TreeScreen::view_paste_as_sibling, globalparameters.tree_screen(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
                                                       );
                    //                return //_record->page();
                    // globalparameters.entrance()->invoke_view(new_record).second->page();
                    //                    this->dockedwindow()->tabWidget()->newTab(blank_record, true)->page();

                    //                if(!r->generator())browser()->equip_registered(r);
                    assert(r->record_binder());
                    //                r->generate();
                    r->activate();

                    page = r->bounded_page();

                }



            }

#ifdef USE_POPUP_WINDOW
            else {

                PopupWindow *popup = new PopupWindow(
                    //                view()->tabmanager()
                    //                ,
                    profile()
                    //                , QUrl(Browser::_defaulthome)
                    //                , _record_controller
                    //                , _page_controller
                    //                                                 , view()->tabmanager()->browser()
                );

                popup->setAttribute(Qt::WA_DeleteOnClose);
                popup->show();
                //            return
                //            page =
                return popup->page();
            }

#endif // USE_POPUP_WINDOW

        assert(page);

        //        // never called
        //        connect(page, &QWebEnginePage::load, page, [&](const QUrl & url) {
        //            boost::intrusive_ptr<Record> record = page->current_record();
        //            record->setNaturalFieldSource("url", url.toString());
        //            record->generate();
        //            record->active();
        //        });

        // not realy needed for each time
        connect(static_cast<QWebEnginePage *const>(page), &QWebEnginePage::setUrl, [&](const QUrl & url) {
            boost::intrusive_ptr<TreeItem> _current_item = page->_record_binder->bounded_item();
            _current_item->field("url", url.toString());
            page->load(_current_item); // record->generate();
            // page->activate();     // record->active();
        });

        connect(static_cast<QWebEnginePage *const>(page), &QWebEnginePage::load, [&](const QUrl & url) {
            boost::intrusive_ptr<TreeItem> _current_item = page->_record_binder->bounded_item();
            _current_item->field("url", url.toString());
            page->load(_current_item); // record->generate();
            // page->activate();     // record->active();
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
        return loader.createWidget(classId, _view);
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

        //        if(globalparameters.entrance()->browsers().count() == 0) {
        //            globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
        //        }

        Browser *browser = globalparameters.entrance()->activated_browser();     //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::PasswordDialog passwordDialog;
        passwordDialog.setupUi(&dialog);

        passwordDialog.iconLabel->setText(QString());
        passwordDialog.iconLabel->setPixmap(browser->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, browser).pixmap(32, 32));

        QString introMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
        introMessage = introMessage.arg(auth->realm()).arg(requestUrl.toString().toHtmlEscaped());
        passwordDialog.introLabel->setText(introMessage);
        passwordDialog.introLabel->setWordWrap(true);

        if(dialog.exec() == QDialog::Accepted) {
            QByteArray key = QtSingleApplication::authenticationKey(requestUrl, auth->realm());
            auth->setUser(passwordDialog.userNameLineEdit->text());
            auth->setPassword(passwordDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            QtSingleApplication::instance()->setLastAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

    void WebPage::proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost)
    {
        Q_UNUSED(requestUrl);

        //        if(globalparameters.entrance()->browsers().count() == 0) {
        //            globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
        //        }

        Browser *browser = globalparameters.entrance()->activated_browser();     //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::ProxyDialog proxyDialog;
        proxyDialog.setupUi(&dialog);

        proxyDialog.iconLabel->setText(QString());
        proxyDialog.iconLabel->setPixmap(browser->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, browser).pixmap(32, 32));

        QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
        introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
        proxyDialog.introLabel->setText(introMessage);
        proxyDialog.introLabel->setWordWrap(true);

        if(dialog.exec() == QDialog::Accepted) {
            QString user = proxyDialog.userNameLineEdit->text();
            QByteArray key = QtSingleApplication::proxyAuthenticationKey(user, proxyHost, auth->realm());
            auth->setUser(user);
            auth->setPassword(proxyDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            QtSingleApplication::instance()->setLastProxyAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

    boost::intrusive_ptr<TreeItem> WebPage::item_request_from_tree(
        boost::intrusive_ptr<TreeItem> item
        , std::function<boost::intrusive_ptr<TreeItem> (std::function<KnowModel *()>, QModelIndex, boost::intrusive_ptr<TreeItem>)> _view_paste_strategy
        , equal_t _equal
    )
    {
        boost::intrusive_ptr<TreeItem> re;
        auto it = _tabmanager->item_request_from_tree_impl(item, _view_paste_strategy, _equal);

        //        if(// !it->is_registered_to_browser() &&
        //            !it->record_binder())
        re = item_registered_imperative_equip(it);
        //        else
        //            re = it;

        //        auto ar = boost::make_shared<WebPage::Coupler>(this, item);
        //        assert(_record_binder);
        //        //        bind_helper binder = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
        //        //                             >>("", &WebPage::RecordBinder::binder, ar);
        //        //        active_helper activator = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &WebPage::RecordBinder::activator, ar);

        return re;
    }

    boost::intrusive_ptr<TreeItem> WebPage::item_request_from_tree(
        const QUrl &_url
        , std::function<boost::intrusive_ptr<TreeItem> (std::function<KnowModel *()>, QModelIndex, boost::intrusive_ptr<TreeItem>)> _view_paste_strategy
        , equal_url_t _equal
    )
    {
        //        auto ar = boost::make_shared<WebPage::Coupler>(this);

        //        //        bind_helper binder = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
        //        //                             >>("", &WebPage::RecordBinder::binder, ar);
        //        //        active_helper activator = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &WebPage::RecordBinder::activator, ar);
        boost::intrusive_ptr<TreeItem> re;
        auto it = _tabmanager->item_request_from_tree_impl(_url, _view_paste_strategy, _equal);   // [&](boost::intrusive_ptr<TreeItem> it) {return it->field("url") == _url.toString();}

        //        if(// !it->is_registered_to_browser() &&
        //            !it->record_binder())
        re = item_registered_imperative_equip(it);
        //        else
        //            re = it;

        return re;
    }

    boost::intrusive_ptr<TreeItem> WebPage::item_registered_imperative_equip(boost::intrusive_ptr<TreeItem> item)  // , browser::WebPage *page
    {
        //        auto binder = [](boost::shared_ptr<WebPage::RecordBinder> ar) {
        //            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>   // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
        //                   >>("", &WebPage::RecordBinder::binder, ar);
        //        };
        //        auto activator = [](boost::shared_ptr<WebPage::RecordBinder> ar) {
        //            return std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &WebPage::RecordBinder::activator, ar);
        //        };

        // registered record, but have no generator:
        //        auto ar =

        //        if(!item->is_registered_to_browser() && !item->record_binder()) {
        _record_binder = boost::intrusive_ptr<CouplerDelegation>(new CouplerDelegation(std::make_shared<WebPage::Coupler>(this, item)));
        item->record_binder(_record_binder);
        //        } else {
        //            if(_record_binder != item->record_binder())_record_binder = item->record_binder();
        //        }



        //        //        it->binder(
        //        //            binder(ar)
        //        //        );

        //        //        it->activator(
        //        //            activator(ar)
        //        //        );

        //        it->record_binder(std::make_shared<CouplerDelegation>(
        //                              std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
        //                              , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
        //                              , std::make_shared<bind_interface>("", &Coupler::binder, ar)
        //                              , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
        //                          ));
        return item;
    }

#ifdef USE_POPUP_WINDOW

    PopupView::PopupView(QWidget *parent)
        : QWebEngineView(parent)
        , m_progress(0)
        , m_page(0)
        , m_iconReply(0)
    {
        connect(this, &PopupView::loadProgress, this, &PopupView::setProgress);
        connect(this, &QWebEngineView::loadFinished, this, &PopupView::loadFinished;
                connect(this, &QWebEngineView::renderProcessTerminated,
        [ = ](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            const char *status = "";

            switch(termStatus) {
                case QWebEnginePage::NormalTerminationStatus:
                    status = "(normal exit)";
                    break;

                case QWebEnginePage::AbnormalTerminationStatus:
                    status = "(abnormal exit)";
                    break;

                case QWebEnginePage::CrashedTerminationStatus:
                    status = "(crashed)";
                    break;

                case QWebEnginePage::KilledTerminationStatus:
                    status = "(killed)";
                    break;
            }

            qInfo() << "Render process exited with code" << statusCode << status;
            QTimer::singleShot(0, [this] { reload(); });
        });
    }

    void PopupView::setPage(PopupPage *_page)
    {
        if(m_page)
            m_page->deleteLater();

        m_page = _page;
        QWebEngineView::setPage(_page);
#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), &PopupPage::statusBarMessage, &PopupView::setStatusBarText);
#endif
        connect(page(), &PopupPage::loadingUrl, this, &PopupView::urlChanged);
        connect(page(), &PopupPage::iconUrlChanged, this, &PopupView::onIconUrlChanged);
        connect(page(), &PopupPage::featurePermissionRequested, this, &PopupView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif
    }

    void PopupView::contextMenuEvent(QContextMenuEvent *event)
    {
        QMenu *menu = page()->createStandardContextMenu();
        const QList<QAction *> actions = menu->actions();
        QList<QAction *>::const_iterator it = qFind(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::OpenLinkInThisWindow));

        if(it != actions.cend()) {
            (*it)->setText(tr("Open Link in This Window"));
            ++it;
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewWindow));
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewTab));
        }

        menu->popup(event->globalPos());
    }

    void PopupView::wheelEvent(QWheelEvent *event)
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

    void PopupView::openLinkInNewTab()
    {
        pageAction(QWebEnginePage::OpenLinkInNewTab)->trigger();
    }

    void PopupView::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
    {
        FeaturePermissionBar *permissionBar = new FeaturePermissionBar(this);
        connect(permissionBar, &FeaturePermissionBar::featurePermissionProvided, page(), &QWebEnginePage::setFeaturePermission);

        // Discard the bar on new loads (if we navigate away or reload).
        connect(page(), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

        permissionBar->requestPermission(securityOrigin, feature);
    }

    void PopupView::setProgress(int progress)
    {
        m_progress = progress;
    }

    void PopupView::loadFinished(bool success)
    {
        if(success && 100 != m_progress) {
            qWarning() << "Received finished signal while progress is still:" << progress()
                       << "Url:" << url();
        }

        m_progress = 0;
    }

    void PopupView::loadUrl(const QUrl &url)
    {
        m_initialUrl = url;
        load(url);
    }

    QString PopupView::lastStatusBarText() const
    {
        return m_statusBarText;
    }

    QUrl PopupView::url() const
    {
        QUrl url = QWebEngineView::url();

        if(!url.isEmpty())
            return url;

        return m_initialUrl;
    }

    QIcon PopupView::icon() const
    {
        if(!m_icon.isNull())
            return m_icon;

        return QtSingleApplication::instance()->defaultIcon();
    }

    void PopupView::onIconUrlChanged(const QUrl &url)
    {
        QNetworkRequest iconRequest(url);
        m_iconReply = QtSingleApplication::networkAccessManager()->get(iconRequest);
        m_iconReply->setParent(this);
        connect(m_iconReply, &QNetworkReply::finished, this, &PopupView::iconLoaded);
    }

    void PopupView::iconLoaded()
    {
        m_icon = QIcon();

        if(m_iconReply) {
            QByteArray data = m_iconReply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);
            m_icon.addPixmap(pixmap);
            m_iconReply->deleteLater();
            m_iconReply = 0;
        }

        emit iconChanged();
    }

    void PopupView::mousePressEvent(QMouseEvent *event)
    {
        m_page->m_pressedButtons = event->buttons();
        m_page->m_keyboardModifiers = event->modifiers();
        QWebEngineView::mousePressEvent(event);
    }

    void PopupView::mouseReleaseEvent(QMouseEvent *event)
    {
        QWebEngineView::mouseReleaseEvent(event);

        if(!event->isAccepted() && (m_page->m_pressedButtons & Qt::MidButton)) {
            QUrl url(QApplication::clipboard()->text(QClipboard::Selection));

            if(!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
                setUrl(url);
            }
        }
    }

    void PopupView::setStatusBarText(const QString &string)
    {
        m_statusBarText = string;
    }

#endif // USE_POPUP_WINDOW



    //    std::map<QString, boost::intrusive_ptr<TreeItem> > WebPage::binded_items()const
    //    {
    //        //        Record *record = _record;   // maybe invalid


    //        //        while(record && record->_page && record->_page != this) {
    //        //            record = record->_page->_record;

    //        //        }

    //        //        //        if(_record && _record->_page)
    //        //        //            record = _record->_page->_record;

    //        //        return
    //        //            records;    // (url().isValid()) ? check_record(url()) : nullptr;
    //        return _items;
    //    }

    // which_page_point_to_me
    void WebPage::items_break()
    {

        //        for(auto &j : _items) {
        //            if(j.second && j.second->unique_page() == this) {
        //                break_page_linked_item(j.second);
        //            }
        //        }


        // here, remove_child_from_itemsflat, TabWidget::closeTab?
        // page, item, source_model, sychronization, need to be done?


        //            if(_record_controller->source_model()->is_item_exists(this->_item->id())) {
        //                _record_controller->source_model()->remove_child(this->_item);
        //                //            break_records(); // same as _record_controller->source_model()->remove_child()
        //            }



        //            if(_tabmanager->indexOf(_item->unique_page()->view()) != -1)
        //                _tabmanager->closeTab(_tabmanager->indexOf(_item->unique_page()->view()));

        int tab_widget_count = _tabmanager->count();
        int tab_bar_count = _tabmanager->tabbar()->count();
        int source_model_size = _record_controller->source_model()->size();
        assert(tab_bar_count == tab_widget_count);
        assert(source_model_size == tab_widget_count);  // it is done from

        if(source_model_size > tab_widget_count) {      // never in

            bool found = false;

            for(int i = 0; i < source_model_size; i++) {
                auto item_maybe_to_removed = _record_controller->source_model()->item(i);

                if(item_maybe_to_removed->bounded_page() == this || !item_maybe_to_removed->page_valid()) { // others refrer to this
                    //                        if(_tabmanager->indexOf(item_maybe_to_removed->unique_page()->view()) != -1)
                    //                            _tabmanager->closeTab(_tabmanager->indexOf(item_maybe_to_removed->unique_page()->view()));  // _record_controller->source_model()->remove_child(item_maybe_to_removed->id());

                    item_break(item_maybe_to_removed);
                    found = true;
                    break;
                }
            }

            assert(found == true);
        }


        assert(_tabmanager->count() == _tabmanager->tabbar()->count());
        assert(_record_controller->source_model()->size() == _tabmanager->count());
        assert(_record_controller->source_model()->size() == _tabmanager->tabbar()->count());

    }

    // this will delete record for ever from database if you use real tree_item as source model of record_view
    void WebPage::item_remove_from_record_screen(boost::intrusive_ptr<TreeItem> item)
    {

        if(_tabmanager && _tabmanager->count() > 0) {
            RecordModel *source_model = _record_controller->source_model(); // tab_manager->source_model();
            assert(item);
            //            assert((item->page_valid() && item->unique_page() == this) || !item->page_valid());

            if(source_model->item(item->field("id"))) { //                && record->unique_page() == this
                _record_controller->page_remove(item->id());
            }
        }
    }

    // which_page_point_to_me
    void WebPage::item_break(boost::intrusive_ptr<TreeItem> item)
    {
        // what _record point to is a stack variable, it's address may be not correct! especially when it was destoried
        if(item) {


            //            item_remove_from_record_screen(item);

            if(item->page_valid() && item->bounded_page()) {
                if(item->bounded_page() == this) {
                    // remove_item_from_source_model(item);    // this will delete record for ever from database if you use real tree_item as source model of record_view
                    item->page_break();
                    //                    item->binder().reset();
                    //                    item->activator().reset();
                    //                    item->page_to_nullptr();   // _record->_page = nullptr; // _record->bind_page(nullptr);
                    //                    assert(!record->binder());
                    //                    assert(!record->activator());
                    assert(!item->page_valid() && !item->bounded_page());
                }
            }

        }

        //        _record.reset();  // do not reset, you may need it later
    }

    //    WebView *WebPage::bind(boost::intrusive_ptr<TreeItem> item)
    //    {
    //        //        Record *new_record = nullptr;
    //        //        RecordTableData *data = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();
    //        //        assert(data);

    //        //        if(_openinnewtab) {
    //        //            Record *newtab  = data->record(QUrl("about:newtab"));
    //        //            new_record = newtab ? newtab : register_record(QUrl("about:newtab"));
    //        //            _openinnewtab = false;
    //        //        } else {

    //        //            Record *record  = data->record(url);
    //        //            Record *blank   = data->record(QUrl(DockedWindow::_defaulthome));

    //        //            new_record = record ? record
    //        //                         : blank ? blank
    //        //                         : ::register_record(url, view()->recordtablecontroller());
    //        //        }

    //        //        if( // !_page->record() ||
    //        //            this->_openinnewtab
    //        //            // ||  record != nullptr   // history navigation
    //        //        ) {
    //        //            // new_record = record ? record : ::register_record(url, _record_ontroller);
    //        //            //            new_record->page(_page);
    //        //            //            _page->record(new_record);

    //        //            // if(_page->_openinnewtab)
    //        //            this->_openinnewtab = false;
    //        //        }

    //        //        boost::intrusive_ptr<Record> record;
    //        //        TabWidget *_tabmanager = view()->tabmanager();
    //        //        assert(_tabmanager);   // maybe before TabWidget::addTab

    //        if(item) {

    //            if((_item && _item.get() != item.get())
    //               // || (_item && !_item->page_valid())    // bug, they may all are nullptr, conflict with upon
    //              ) {

    //                break_item(_item);
    //                //                //                tabmanager->table_data()->delete_record_by_id(_record->natural_field_source("id"));
    //                //                //                tabmanager->table_data()->shadow_record(tabmanager->table_data()->size(), record);
    //                //                remove_item_from_itemsflat(_tree_item);    // makes ure this will not delete record for ever from database
    //                //                //            if(_record) {
    //                //                //                if(_record->binded_page() != nullptr && _record->binded_page() == this) {
    //                //                //                    // assert(_record->linkpage() == this);    // should always be true -> if not move note

    //                //                //                    // _record->linkpage(nullptr);
    //                //                //                    _record->breakpage();

    //                //                //                    // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
    //                //                //                }
    //                //                //            }

    //                //                //            if(_record && _record->unique_page() != nullptr)

    //                //                // this->break_record_which_page_point_to_me(_record);

    //                //                if(_records.size() > 0
    //                //                   && _records.find(_tree_item->field("id")) != _records.end()
    //                //                  ) _records.erase(_tree_item->field("id"));

    //                //                //            if(record) {
    //                //                //                if((!record->binded_page()) || (record->binded_page() != this)) {
    //                //                //                    // assert(_record->linkpage() == this);    // should always be true -> if not move note

    //                //                //                    // _record->linkpage(nullptr);
    //                //                //                    _record = record->bind_page(this);  // record->breakpage();
    //                //                //                    // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
    //                //                //                }
    //                //                //            }

    //                //                //        else {
    //                //                //            _record->breakpage();
    //                //                //        }

    //                //                //        _record = record;   // ->bind_page(this);   // just for close tab(==view)

    //                //                // _record->linkpage(this);

    //                //                // connect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});

    //                //                //        WebView   *view = record->page()->view();
    //                //                //        view->tabmanager()->setCurrentWidget(view);
    //                //                //        view->show();
    //                //                //        // _page->record(new_record);
    //                //                //        update_record();


    //            }

    //            _item = item;
    //            //        else {
    //            //            if(_record)_record->bind_page(this);
    //            //        }

    //            if(_item) {
    //                if((!_item->page_valid()    // && !_tree_item->unique_page()
    //                   ) || (_item->page_valid() && _item->unique_page() != this)
    //                  ) {
    //                    // assert(_record->linkpage() == this);    // should always be true -> if not move note

    //                    // _record->linkpage(nullptr);
    //                    _record_binder->bounded_page() = const_cast<WebPage *>(this); // record->breakpage();

    //                    // disconnect(_record, &Record::distructed, this, [this]() {_record->linkpage(nullptr); _record = nullptr;});
    //                    // if(_records.find(_record) == _records.end())

    //                    //                    if(_items.find(requested_item->field("id")) == _items.end()) _items[_item->field("id")] = _item;

    //                    //                MetaEditor *metaeditor = find_object<MetaEditor>(meta_editor_singleton_name);
    //                    //                assert(metaeditor);
    //                    //                metaeditor->bind(_record);
    //                }
    //            }

    //            //            update_record_view(_item);

    //            MetaEditor *metaeditor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
    //            assert(metaeditor);

    //            // add_item_to_source_model(_item);    // may lead to recursive call?   // inside record_controller::register_item...

    //            //            TabWidget *tabmanager = view()->tabmanager();

    //            //            std::shared_ptr<TableData> table_data = tab_manager->tree_item()->tabledata();

    //            //            if(!table_data->is_item_exists(_record->getNaturalFieldSource("id"))) {
    //            //                table_data->insert_new_record(table_data->work_pos(), _record);
    //            //            }

    //            //            assert(_tabmanager->count() > 0);   // maybe before TabWidget::addTab

    //            //            if(view() == _tabmanager->currentWebView()) {
    //            if(metaeditor->item() != _item)sychronize_metaeditor_to_item(_item);  // metaeditor->bind(_record);

    //            //            }
    //        }

    //        return _view;
    //    }

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
        if(_record_binder->bounded_item()) {
            if(url() != QUrl()
               && !url().host().isEmpty()
               && !url().scheme().isEmpty()
               && url() != QUrl(Browser::_defaulthome)
               && url() == _loadingurl
               && url().toString() == _record_binder->bounded_item()->field("url")
              ) {

                //                if(//_state_check[_record->getNaturalFieldSource("pin")] == Qt::Unchecked &&
                //                    url().toString() == _record->getNaturalFieldSource("url")
                //                ) {
                //                    // if(this->record())

                //                    //        if(url() == _loadingurl)

                //                update_record(url(), title, true);

                MetaEditor *metaeditor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
                assert(metaeditor);

                if(title != ""
                   && title != _record_binder->bounded_item()->field("name") // && !QUrl(title).isValid()
                  ) {

                    _record_binder->bounded_item()->field("name", title);

                    auto _mainwindow = globalparameters.mainwindow();

                    if(!_mainwindow->windowTitle().contains(title)) {_mainwindow->setWindowTitle(QString(application_name) + " : " + title);}

                    if(_view == _tabmanager->currentWebView()) {
                        if(metaeditor->item() != _record_binder->bounded_item())sychronize_metaeditor_to_item(_record_binder->bounded_item());  // metaeditor->bind(_record);

                        metaeditor->name(title);
                    }

                    int index = _tabmanager->webViewIndex(_view);
                    _tabmanager->setTabToolTip(index, title);
                }

                //                }
            }
        }
    }




    // This signal is emitted with the URL of the main frame when the main frame's title is received. The new URL is specified by url.
    void WebPage::onUrlChanged(const QUrl &url)
    {
        //        assert(_page->url() != QUrl());
        //        assert(url != QUrl());
        if(_record_binder->bounded_item()) {
            //            //        assert(_page->url() == url);
            //            if(_record_binder->bounded_item()->field("pin") != _string_from_check_state[Qt::Checked]
            //               || url.toString() == _record_binder->bounded_item()->field("home")
            //              ) { //           && url == _loadingurl

            if(url != QUrl() && !url.host().isEmpty() && !url.scheme().isEmpty()
               && url != QUrl(Browser::_defaulthome)
               && url != _loadingurl
               && url.toString() != _record_binder->bounded_item()->field("url")
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
                MetaEditor *metaeditor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
                assert(metaeditor);

                if(url.toString() != "") {
                    _record_binder->bounded_item()->field("url", url.toString());

                    if(_view == _tabmanager->currentWebView()) {
                        if(metaeditor->item() != _record_binder->bounded_item())sychronize_metaeditor_to_item(_record_binder->bounded_item());  // metaeditor->bind(_record);

                        metaeditor->url(url.toString());
                    }
                }

                //                    update_record(url, title(), true);


                //                //            //}
                //            }
                //                }

            }

            //            }

            //            else {
            //                WebPage *page = static_cast<WebPage *>(createWindow(QWebEnginePage::WebBrowserTab));
            //                assert(page);

            //                if(page) {
            //                    // auto ar = boost::make_shared<Coupler>(page);
            //                    auto r = page->item_request_from_tree(url);
            //                    r->activate();
            //                }
            //            }

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
    }

    void WebPage::on_close_requested()
    {
        _record_controller->on_recordtable_configchange();
        //        _record_controller->delete_items_selected();   // source_model()->on_table_config_changed();
        item_break(_record_binder->bounded_item());  // break_items();
    }

    void WebPage::update_record(const QUrl &url, const QString &title)
    {
        //        Q_UNUSED(make_current)

        //        //if(globalparameters.getRecordTableScreen()->getRecordTableController()) {

        //        //        RecordTableController *_record_ontroller = globalparameters.getRecordTableScreen()->getRecordTableController();
        //        //            //Record *_record = nullptr;
        //        //            //RecordTableModel *recordtablemodel = _record_ontroller->getRecordTableModel();
        //        RecordTableView *recordtableview = _record_ontroller->getView();
        //        //            //RecordTableData *recordtabledata = recordtablemodel->getRecordTableData();

        //        //        if(!_record) {
        //        //            //            Record *record = nullptr;
        //        //            RecordTableData *recordtabledata = view()->recordtablecontroller()->getRecordTableModel()->getRecordTableData();

        //        //            if(recordtabledata)_record = recordtabledata->getRecordByUrl(url());
        //        //        }

        //        //        Record *record = this->record();

        if(_record_binder->bounded_item()) {
            //            _record->active_immediately(true);

            //            //            WebView *view = record->page()->view();
            //            //            view->tabmanager()->setCurrentWidget(view);
            //            //            view->show();

            //            //                //MetaEditor *metaeditor = ::globalparameters.getMetaEditor();
            //            QModelIndex proxyindex = _record_ontroller->convertIdToProxyIndex(_record->getNaturalFieldSource("id"));
            //            //                //QModelIndex sourceindex = _record_ontroller->convertIdToSourceIndex(_page->record()->getField("id"));
            //            int position = _record_ontroller->convertProxyIndexToPos(proxyindex);
            //            //                // QString page_title = webPage()->title();    // same as this->title()


            // Выясняется указатель на объект редактирования текста записи
            MetaEditor *metaeditor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);

            // Выясняется ссылка на таблицу конечных данных
            //                RecordTableData *table = recordSourceModel->getTableData();

            bool is_current = (_view == _tabmanager->currentWebView()) ;

            if(title != ""
               && title != _record_binder->bounded_item()->field("name")
               //                   && !QUrl(title).isValid()
              ) {

                _record_binder->bounded_item()->field("name", title);
                //metaeditor->setName(title);
                auto _mainwindow = globalparameters.mainwindow();

                if(!_mainwindow->windowTitle().contains(title)) {_mainwindow->setWindowTitle(QString(application_name) + " : " + title);}

                //                table->setWorkPos(pos);
                if(is_current)metaeditor->name(title);
            }

            if(url.toString() != "") {
                _record_binder->bounded_item()->field("url", url.toString());

                if(is_current)metaeditor->url(url.toString());

            }

            if(metaeditor->item() != _record_binder->bounded_item() && is_current)sychronize_metaeditor_to_item(_record_binder->bounded_item());  // metaeditor->bind(_record);

            //metaeditor->setUrl(url.toString());

            //_page->load(url);

            //if(recordtableview) {


            //recordtableview->update();//repaint();    // does not work
            //recordtableview->adjustSize();    // does not work, size weird

            //            if(make_current && recordtableview) {
            //                recordtableview->setSelectionToPos(position); // work






            _view->setFocus();   // make upate validate


            auto _tree_screen = globalparameters.tree_screen();
            _tree_screen->tree_view()->select_and_current(_record_binder->bounded_item());

            if(is_current) {// globalparameters.mainwindow()
                if(_record_controller->view()->selection_first_id() != _record_binder->bounded_item()->field("id"))
                    _record_controller->select_id(_record_binder->bounded_item()->field("id"));

                // _page_controller->select_id(_record->getNaturalFieldSource("id"));
            }






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
            //_record_ontroller->clickToRecord(proxyindex);    // does not work
            //recordtablemodel->dataChanged();
            //            }

            //}

            //            int row = sourceindex.row();
            //            int column = sourceindex.column();
            //            int _row = proxyindex.row();
            //            int _column = proxyindex.column();
            //            qDebug() << "qDebug()\t" << row << "\t" << column << "\t" << _row << "\t" << _column << "\t" << "\n";    // does not work?

            //std::cout << "std::cout\t" << row << "\t" << column << "\t" << _row << "\t" << _column << "\t" << "\n";
            //_record_ontroller->getRecordTableModel()->setData(sourceindex, QVariant::fromValue(this->title()), Qt::EditRole);

            //            _record_ontroller->editField(position
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





    void WebPage::sychronize_metaeditor_to_item(boost::intrusive_ptr<TreeItem> current_item)
    {
        //        boost::intrusive_ptr<Record> record = this->table_model()->table_data()->record(pos);
        assert(current_item);
        //        assert(record == view()->tabmanager()->currentWebView()->page()->current_record()); // may be in backend?

        // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь только получить данные из таблицы

        // Выясняется указатель на объект редактирования текста записи
        MetaEditor *meta_editor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
        assert(meta_editor);
        meta_editor->bind(current_item);

        //        int pos = indexOf(currentWebView());
        // Turns the reference to the table of final data   // Выясняется ссылка на таблицу конечных данных
        //        std::shared_ptr<TableData> table = std::make_shared<TableData>();   // _source_model->table_data_internal();

        //        for(int i = 0; i < count(); i++) {
        //            table->insert_new_record(table->work_pos(), static_cast<WebView *>(widget(i))->page()->current_record());
        //        }

        // В таблице конечных данных запоминается какая запись была выбрана
        // чтобы затем при выборе этой же подветки засветка автоматически
        // установилась на последнюю рабочую запись
        //        table->work_pos(pos);


        // Устанавливается функция обратного вызова для записи данных
        meta_editor->save_callback( // table
            meta_editor->editor_save_callback);

        // Сохраняется текст и картинки в окне редактирования
        //        find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->save_text_area();


        // Для новой выбраной записи выясняется директория и основной файл
        if(current_item->field("id") == "" // || current_item->field("url") == Browser::_defaulthome
          ) {current_item->field("id", get_unical_id());}

        if(current_item->field("dir") == "")current_item->field("dir", current_item->id());

        if(current_item->field("file") == "")current_item->field("file", "text.html");

        QString current_dir = current_item->field("dir");  // table->field(pos, "dir");
        QString current_file = current_item->field("file");  // table->field(pos, "file");
        QString full_dir = appconfig.tetra_dir() + "/base/" + current_dir;
        QString full_file_name = full_dir + "/" + current_file;
        qDebug() << " File " << full_file_name << "\n";

        // If the window contents of the record is already selected record  // Если в окне содержимого записи уже находится выбираемая запись
        if(meta_editor->work_directory() == full_dir
           && meta_editor->file_name() == current_file
          ) {
            globalparameters.window_switcher()->switchFromRecordtableToRecord();
            return;
        }

        // Перед открытием редактора происходит попытка получения текста записи
        // Этот вызов создаст файл с текстом записи, если он еще не создан (подумать, переделать)
        // Before the opening of the editor it attempts to get the text records
        // This call will create a text file with the record if it is not already created (think remake)
        meta_editor->save_textarea();   // table->text(pos);    // ?

        // Редактору задаются имя файла и директории
        // И дается команда загрузки файла
        meta_editor->work_directory(full_dir);
        meta_editor->file_name(current_file);

        // Если идет работа с зашифрованной записью
        // И если имя директории или имя файла пусты, то это означает что
        // запись не была расшифрована, и редактор должен просто показывать пустой текст
        // ничего не сохранять и не считывать
        qDebug() << "RecordTableView::onClickToRecord() : id " << current_item->field("id");       // table->field(pos, "id");
        qDebug() << "RecordTableView::onClickToRecord() : name " << current_item->field("name");   // table->field(pos, "name");
        qDebug() << "RecordTableView::onClickToRecord() : crypt " << current_item->field("crypt"); // table->field(pos, "crypt");

        if(current_item->field("crypt")    // table->field(pos, "crypt")
           == "1")
            if(full_dir.length() == 0 || current_file.length() == 0)
                meta_editor->dir_file_empty_reaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

        // В редактор заносится информация, идет ли работа с зашифрованным текстом
        meta_editor->misc_field("crypt"
                                , current_item->field("crypt") // table->field(pos, "crypt")
                               );

        // В редакторе устанавливается функция обратного вызова для чтения данных
        meta_editor->load_callback( // table
            meta_editor->editor_load_callback);

        meta_editor->load_textarea();
        // edView->set_textarea(table->get_text(index.row()));

        // Заполняются прочие инфо-поля
        meta_editor->pin(
            current_item->field("pin")   // table->field(pos, "pin")
        );
        meta_editor->name(
            current_item->field("name")   // table->field(pos, "name")
        );
        meta_editor->author(
            current_item->field("author")   // table->field(pos, "author")
        );
        meta_editor->home(
            current_item->field("home")   // table->field(pos, "home")
        );
        meta_editor->url(
            current_item->field("url")   // table->field(pos, "url")
        );
        meta_editor->tags(
            current_item->field("tags")   // table->field(pos, "tags")
        );

        QString id = current_item->field("id"); // table->field(pos, "id");
        meta_editor->misc_field("id", id);

        meta_editor->misc_field("title"
                                , current_item->field("name") // table->field(pos, "name")
                               );

        // should each record carry it's tree path?
        //        // Set the path to the branch in which lies the record (in the form of the names of the branches)   // Устанавливается путь до ветки в которой лежит запись (в виде названий веток)
        //        QString path = qobject_cast<TableScreen *>(parent())->getTreePath();

        //        // В мобильном интерфейсе редактор должен показывать путь до записи
        //        if(appconfig.getInterfaceMode() == "mobile")
        //            meta_editor->setTreePath(path);

        // В редакторе восстанавливается позиция курсора и прокрутки если это необходимо
        if(appconfig.remember_cursor_at_ordinary_selection()) {
            meta_editor->cursor_position(walkhistory.cursor_position(id));
            meta_editor->scrollbar_position(walkhistory.scrollbar_position(id));
        }

        // Обновление иконки аттачей
        if(current_item->attach_table()->size()  // table->record(pos)->getAttachTablePointer()->size()
           == 0)
            meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_not_exists);   // Если нет приаттаченных файлов
        else
            meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_exists);   // Есть приаттаченные файлы
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
            //                //            RecordTableData *data = _record_ontroller->getRecordTableModel()->getRecordTableData();
            //                //            record = data->record(_page->url());

            //                //            if(!record)::register_record(_page->url());
            //                record = request_record(_page->url());

            //                if(record)_page->bind_record(record);
            //            } else {

            //                //            if(_page->record()) {
            //                //                _page->update_record(_page->url(), _page->title());
            //                //            }
            _page->update_record(_page->url(), _page->title());
            //            }




        }

        _progress = 0;
    }


    WebView::~WebView()
    {
        //        if(_page->record()) {
        //            _page->record()->page(nullptr);
        //            //            globalparameters.getRecordTableScreen()->previousInFocusChain();    //_record;
        //            _page->record(nullptr);
        //        }

        if(_page) {
            // delete _page;
            // _page = nullptr; //
            // emit _page->close_requested();
            _page->deleteLater();
        }
    }


    //    WebView::WebView(const boost::intrusive_ptr<TreeItem> requested_item
    //                     , QWebEngineProfile *profile   // , bool openinnewtab
    //                     , TabWidget *tabmanager
    //                     , QWidget *parent
    //                     , RecordController *table_controller
    //                    )
    //        : QWebEngineView(static_cast<QWidget *>(parent))   // ->parent()
    //        , _tabmanager(tabmanager)                               //        , _record(record)
    //        , _record_controller(table_controller)
    //        , _page(new WebPage(profile
    //                            , requested_item // , openinnewtab
    //                            , table_controller
    //                            , this))
    //          //        , _initialurl(record ? record->getNaturalFieldSource("url") : QUrl())
    //        , _progress(0)
    //        , _iconreply(0)
    //    {
    //        //        Q_UNUSED(parent)
    //        settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    //        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    //        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    //        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    //        settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

    //        //        if(record) {record->view(this);}

    //        connect(this, &WebView::loadProgress, this, &WebView::setProgress);


    //        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    //        connect(this, &QWebEngineView::renderProcessTerminated
    //        , [ = ](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
    //            const char *status = "";

    //            switch(termStatus) {
    //                case QWebEnginePage::NormalTerminationStatus:
    //                    status = "(normal exit)";
    //                    break;

    //                case QWebEnginePage::AbnormalTerminationStatus:
    //                    status = "(abnormal exit)";
    //                    break;

    //                case QWebEnginePage::CrashedTerminationStatus:
    //                    status = "(crashed)";
    //                    break;

    //                case QWebEnginePage::KilledTerminationStatus:
    //                    status = "(killed)";
    //                    break;
    //            }            qInfo() << "Render process exited with code" << statusCode << status;

    //            QTimer::singleShot(0, [this] { reload(); });
    //        });

    //        //    connect(this->webPage(), &WebPage::loadFinished, this, &WebView::onLoadFinished);
    //        //    connect(this->webPage(), &WebPage::titleChanged, this, &WebView::onTitleChanged);
    //        //    connect(this->webPage(), &WebPage::urlChanged, this, &WebView::onUrlChanged);
    //        //    //    &WebPage::titleChanged(const QString &title);
    //        //    //    &WebPage::urlChanged(const QUrl &url);

    //        QWebEngineView::setPage(_page);

    //#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
    //        connect(page(), &WebPage::statusBarMessage, &WebPage::setStatusBarText);
    //#endif
    //        connect(page(), &WebPage::loadingUrl, this, &WebView::urlChanged);
    //        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::iconUrlChanged, this, &WebView::onIconUrlChanged);
    //        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
    //#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
    //        page()->setForwardUnsupportedContent(true);
    //#endif


    //        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::loadFinished, this, &WebView::onLoadFinished);
    //        connect(this, &WebView::close_view, &WebView::on_close_requested);
    //        //    &WebPage::titleChanged(const QString &title);
    //        //    &WebPage::urlChanged(const QUrl &url);
    //        //        _page->load(_record->getField("url"));    // auto  loaded

    //        setFocus();
    //        //    _record_controller->addnew_item_fat(requested_item);
    //    }

    WebView::WebView(const boost::intrusive_ptr<TreeItem> _item
                     , QWebEngineProfile *profile   // , bool openinnewtab
                     , Browser *_browser, TabWidget *_tabmanager
                     , RecordController *table_controller
                    )
        : QWebEngineView(static_cast<QWidget *>(_tabmanager))    // ->parent()
        , _browser(_browser)
        , _tabmanager(_tabmanager)                               //        , _record(record)
        , _record_controller(table_controller)
        , _page(// nullptr
              new WebPage(profile
                          , _item // , openinnewtab
                          , _browser
                          , _tabmanager
                          , table_controller
                          , this)
          )
          //        , _initialurl(record ? record->getNaturalFieldSource("url") : QUrl())
        , _progress(0)
        , _iconreply(0)
    {

        settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        //        if(record) {record->view(this);}

        connect(this, &WebView::loadProgress, this, &WebView::setProgress);

        connect(this, &WebView::loadFinished, this, &WebView::onLoadFinished);
        connect(this, &QWebEngineView::renderProcessTerminated
        , [ = ](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            const char *status = "";

            switch(termStatus) {
                case QWebEnginePage::NormalTerminationStatus:
                    status = "(normal exit)";
                    break;

                case QWebEnginePage::AbnormalTerminationStatus:
                    status = "(abnormal exit)";
                    break;

                case QWebEnginePage::CrashedTerminationStatus:
                    status = "(crashed)";
                    break;

                case QWebEnginePage::KilledTerminationStatus:
                    status = "(killed)";
                    break;
            }            qInfo() << "Render process exited with code" << statusCode << status;

            QTimer::singleShot(0, [this] { reload(); });
        });

        //    connect(this->webPage(), &WebPage::loadFinished, this, &WebView::onLoadFinished);
        //    connect(this->webPage(), &WebPage::titleChanged, this, &WebView::onTitleChanged);
        //    connect(this->webPage(), &WebPage::urlChanged, this, &WebView::onUrlChanged);
        //    //    &WebPage::titleChanged(const QString &title);
        //    //    &WebPage::urlChanged(const QUrl &url);

        QWebEngineView::setPage(_page);

#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), &WebPage::statusBarMessage, &WebPage::setStatusBarText);
#endif
        connect(page(), &WebPage::loadingUrl, this, &WebView::urlChanged);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::iconUrlChanged, this, &WebView::onIconUrlChanged);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif


        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::loadFinished, this, &WebView::onLoadFinished);
        connect(this, &WebView::close_requested, &WebView::on_close_requested);
        //    &WebPage::titleChanged(const QString &title);
        //    &WebPage::urlChanged(const QUrl &url);
        //        _page->load(_record->getField("url"));    // auto  loaded

        //        _page = new WebPage(profile
        //                            , requested_item // , openinnewtab
        //                            , _browser
        //                            , _tabmanager
        //                            , table_controller
        //                            , this);

        setFocus();
        //        _record_controller->addnew_item_fat(requested_item);
    }


    void WebView::page(WebPage *_page)
    {
        if(_page)
            _page->deleteLater();

        this->_page = _page;
        QWebEngineView::setPage(_page);
#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), &WebPage::statusBarMessage, &WebPage::setStatusBarText);
#endif
        connect(page(), &WebPage::loadingUrl, this, &WebView::urlChanged);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::iconUrlChanged, this, &WebView::onIconUrlChanged);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif


        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::loadFinished, this, &WebView::onLoadFinished);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::titleChanged, this->_page, &WebPage::onTitleChanged);
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::urlChanged, this->_page, &WebPage::onUrlChanged);

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

        QMenu *menu = page()->createStandardContextMenu();
        const QList<QAction *> actions = menu->actions();
        QList<QAction *>::const_iterator it = qFind(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::OpenLinkInThisWindow));

        if(it != actions.cend()) {
            (*it)->setText(tr("Open Link in This Window"));
            ++it;
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewWindow));
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewTab));
        }

        menu->popup(event->globalPos());

        //        QWebEngineView::contextMenuEvent(event);
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

        //        //    if(!_record_ontroller) {
        //        //        //RecordTableController *
        //        //        _record_ontroller = globalParameters.getRecordTableScreen()->getRecordTableController();
        //        //    }

        //        //    RecordTableData *recordTableData = _record_ontroller->getRecordTableModel()->getRecordTableData();

        //        //if(_url.isValid() && _record == nullptr) {
        //        _page->record(bind_record(_url));

        //        //        QModelIndex proxyindex = _record_ontroller->convertIdToProxyIndex(_page->record()->getField("id"));
        //        //        int position = _record_ontroller->convertProxyIndexToPos(proxyindex);
        //        //        _record_ontroller->getView()->setSelectionToPos(position);
        setFocus();

        //        globalparameters.mainwindow()
        if(_record_controller->view()->selection_first_id() != _page->bounded_item()->field("id"))
            _record_controller->select_id(_page->bounded_item()->field("id"));

        //}

    }


    void WebView::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
    {
        FeaturePermissionBar *permissionBar = new FeaturePermissionBar(this);
        connect(permissionBar, &FeaturePermissionBar::featurePermissionProvided, page(), &QWebEnginePage::setFeaturePermission);

        // Discard the bar on new loads (if we navigate away or reload).
        connect(static_cast<QWebEnginePage *const>(this->_page), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

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
        connect(_iconreply, &QNetworkReply::finished, this, &WebView::iconLoaded);
    }

    void WebView::on_close_requested()
    {
        close();

        emit _page->close_requested();

        //        int tab_widget_count = tabmanager()->count();
        //        int tab_bar_count = tabmanager()->tabbar()->count();
        //        int source_model_size = _record_controller->source_model()->size();
        //        assert(tab_bar_count == tab_widget_count);

        //        if(source_model_size > tab_widget_count) {
        //            bool found = false;

        //            for(int i = 0; i < source_model_size; i++) {
        //                auto item_maybe_to_removed = _record_controller->source_model()->child(i);

        //                if(item_maybe_to_removed->unique_page() == page() || !item_maybe_to_removed->page_valid()) {
        //                    //                    if(_tabmanager->indexOf(item_maybe_to_removed->unique_page()->view()) != -1)
        //                    //                        _tabmanager->closeTab(_tabmanager->indexOf(item_maybe_to_removed->unique_page()->view()));  //_record_controller->source_model()->remove_child(item_maybe_to_removed->id());

        //                    page()->break_page_linked_item(item_maybe_to_removed);  // emit _page->close_requested();
        //                    found = true;
        //                    break;
        //                }
        //            }

        //            assert(found == true);
        //        }

        //        assert(tabmanager()->count() == tabmanager()->tabbar()->count());
        //        assert(_record_controller->source_model()->size() == tabmanager()->tabbar()->count());
        //        assert(_record_controller->source_model()->size() == tabmanager()->count());

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


    WebView *WebView::load(boost::intrusive_ptr<TreeItem> record)
    {
        //        _page->record(record);
        return _page->item_registered_imperative_equip(record)->activate();  //        loadUrl(QUrl(record->getNaturalFieldSource("url")));

    }


    //    void WebView::switch_show()
    //    {
    //        _tabmanager->setCurrentIndex(_tabmanager->webViewIndex(this));
    //        _page->load(_page->current_record());
    //        show();
    //    }

    WebPage::Coupler::Coupler(WebPage *_page, boost::intrusive_ptr<TreeItem> _bounded_item)  // , bool make_current = true
        : std::enable_shared_from_this<Coupler>()
        , _bounded_page(_page)            // , _make_current(make_current)
        , _bounded_item(_bounded_item)
    {
        //        _bounded_page->record_binder(boost::intrusive_ptr<CouplerDelegation>(new CouplerDelegation(shared_from_this())));
        //        _bounded_item->record_binder(_bounded_page->record_binder());
    }

    WebView *WebPage::Coupler::binder(/*boost::intrusive_ptr<TreeItem> item*/)
    {

        //        //                boost::intrusive_ptr<TreeItem> result = _the->record_controller()->source_model()->find(item);
        WebView *view = _bounded_page->view();;

        //        //                if(result) {
        //        //                    view = item->bind();    //
        //        //                    // item->unique_page()->view();
        //        //                } else {
        //        WebView *view = _bounded_page->bind(item);
        //        //        (item.get()->*_bind)(_bounded_page);
        //        //        //                }
        //        _bounded_item = item;

        if(_bounded_item) {

            //            if((_bounded_item && _bounded_item.get() != item.get())) { // || (_item && !_item->page_valid())    // bug, they may all are nullptr, conflict with upon
            //                _bounded_page->item_break(_bounded_item);
            //            }

            //            _bounded_item = item;

            if(_bounded_item->record_binder() != _bounded_page->record_binder()) {
                assert(_bounded_item->record_binder());
                _bounded_page->record_binder(_bounded_item->record_binder());
            }

            if(_bounded_page->url().toString() != _bounded_item->field("url")) {
                _bounded_page->setUrl(QUrl(_bounded_item->field("url")));
            }

            //        _bounded_page = view->page();
            MetaEditor *metaeditor = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
            assert(metaeditor);

            if(metaeditor->item() != _bounded_item)_bounded_page->sychronize_metaeditor_to_item(_bounded_item);
        }

        return view;  // _the->load(record, _make_current);
    }

    WebPage::Coupler::~Coupler()
    {
        //        if(_bounded_item && _bounded_page) { // || (_item && !_item->page_valid())    // bug, they may all are nullptr, conflict with upon
        //            if(_bounded_page->record_binder() && _bounded_item->record_binder())
        //                _bounded_page->item_break(_bounded_item);
        //        }

        //        _bounded_page->record_binder().reset();
        //        _bounded_item->record_binder().reset();
    }

    WebView *WebPage::Coupler::activator(
        // boost::intrusive_ptr<TreeItem> item
    )
    {
        assert(_bounded_item->record_binder());
        assert(_bounded_item->record_binder() == _bounded_page->record_binder());
        assert(_bounded_item->bounded_page() == _bounded_page);
        assert(_bounded_item == _bounded_page->bounded_item());
        //        WebView *view = nullptr;

        assert(_bounded_item);
        assert(_bounded_item->page_valid());
        //        return _bounded_item->unique_page()->activate();

        //        //        if(_page->view() != _page->_tabmanager->currentWebView()) {
        //        view = _bounded_page->activate();
        //        //        } else {
        //        //            view = _page->view();
        //        //            view->show();
        //        //        }

        return _bounded_page->activate();   // view;
    }

    boost::intrusive_ptr<TreeItem> WebPage::bounded_item()const {return _record_binder->bounded_item();}

    boost::intrusive_ptr<CouplerDelegation> WebPage::record_binder() {return _record_binder;}

    void WebPage::record_binder(boost::intrusive_ptr<CouplerDelegation> _record_binder) {this->_record_binder = _record_binder;}

    WebView *WebPage::view()
    {
        //        assert(_view);
        return _view;
    }

    RecordController *WebPage::record_controller() {return _record_controller;}

    TabWidget *WebPage::tabmanager() {return _tabmanager;}




}





