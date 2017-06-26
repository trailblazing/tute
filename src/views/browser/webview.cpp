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

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QWebChannel>
#include <QWebEnginePage>
#include <QtGui/QClipboard>
#include <QtGui/QMouseEvent>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#if defined(QWEBENGINEPAGE_HITTESTCONTENT)
#include <QWebEngineHitTestResult>
#endif

#ifndef QT_NO_UITOOLS
#include <QtUiTools/QUiLoader>
#endif // QT_NO_UITOOLS

#include <QtCore/QBuffer>
#include <QtCore/QDebug>

//#include "models/tree/tree_item.dec"


#include <thread>

#include "browser.h"
#include "controllers/record_table/record_controller.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "featurepermissionbar.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/blogger.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"
#include "main.h"
#include "models/attach_table/attach_table_data.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "ui_passworddialog.h"
#include "ui_proxy.h"
#include "views/browser/docker.h"
#include "views/browser/toolbar_search.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"
#include "webview.h"

// #include "views/browser/browser.h"

// #include "browserview.moc"

#include <iostream>

class gl_para;
extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<AppConfig> appconfig;
extern bool url_equal(const std::string& url_compare_stored, const std::string& url_compare_get);
extern const char* program_title;

namespace web {
#ifdef USE_POPUP_WINDOW

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(PopupPage)
#endif

    PopupPage::PopupPage(PopupView* view_, web::Browser* browser_, Profile* profile_)
        : QWebEnginePage(profile_, view_)
        , _view(view_)
        , _browser(browser_)
        , _keyboard_modifiers(Qt::NoModifier)
        , _pressed_buttons(Qt::NoButton)
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

    web::Browser* PopupPage::browser()
    {
        return _browser;
        // QObject *w = this->parent();
        // while(w){
        // if(Browser *mw = qobject_cast<Browser *>(w))return mw;
        // w = w->parent();
        // }
        // return _entrance->equip_registered().first;	//
        // BrowserApplication::instance()->mainWindow();
    }

    bool PopupPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
    {
        Q_UNUSED(type);
        if (isMainFrame) {
            _loading_url = url;
            emit loadingUrl(_loading_url);
        }
        return true;
    }

    bool PopupPage::certificateError(const QWebEngineCertificateError& error)
    {
        if (error.isOverridable() && !_certificate_ignored) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(error.errorDescription());
            msgBox.setInformativeText(
                tr("If you wish so, you may continue with an unverified certificate. "
                   "Accepting an unverified certificate means "
                   "you may not be connected with the host you tried to connect to.\n"
                   "Do you wish to override the security check and continue?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            return _certificate_ignored = (msgBox.exec() == QMessageBox::Yes);
        }
        // QMessageBox::critical(view(), tr("Certificate Error"),
        // error.errorDescription(), QMessageBox::Ok, QMessageBox::NoButton);

        // return false;
        return _certificate_ignored;
    }

// #include "webview.moc"

// QWebEnginePage *PopupPage::createWindow(QWebEnginePage::WebWindowType type)
// {
// if(type == QWebEnginePage::WebBrowserTab) {
// return mainWindow()->tabWidget()->newTab()->page();
// } else if(type == QWebEnginePage::WebBrowserWindow) {
// BrowserApplication::instance()->newMainWindow();
// Browser *mainWindow = BrowserApplication::instance()->mainWindow();
// return mainWindow->currentTab()->page();
// } else {
// PopupWindow *popup = new PopupWindow(profile());
// popup->setAttribute(Qt::WA_DeleteOnClose);
// popup->show();
// return popup->page();
// }
// }

#if !defined(QT_NO_UITOOLS)
    QObject* PopupPage::createPlugin(const QString& classId, const QUrl& url, const QStringList& paramNames, const QStringList& paramValues)
    {
        Q_UNUSED(url);
        Q_UNUSED(paramNames);
        Q_UNUSED(paramValues);
        QUiLoader loader;

        return loader.createWidget(classId, view());
    }

#endif // !defined(QT_NO_UITOOLS)

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
    void WebPage::handleUnsupportedContent(QNetworkReply* reply)
    {
        QString errorString = reply->errorString();
        if (m_loadingUrl != reply->url()) {
            // sub resource of this page
            qWarning() << "Resource" << reply->url().toEncoded()
                       << "has unknown Content-Type, will be ignored.";
            reply->deleteLater();

            return;
        }
        if (reply->error() == QNetworkReply::NoError &&
            !reply->header(QNetworkRequest::ContentTypeHeader).isValid())
            errorString = "Unknown Content-Type";
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
        QIcon icon =
            view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
        QPixmap pixmap = icon.pixmap(QSize(32, 32));
        if (pixmap.save(&imageBuffer, "PNG"))
            html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"), QString(QLatin1String(imageBuffer.buffer().toBase64())));
        QList<QWebEngineFrame*> frames;
        frames.append(mainFrame());
        while (!frames.isEmpty()) {
            QWebEngineFrame* frame = frames.takeFirst();
            if (frame->url() == reply->url()) {
                frame->setHtml(html, reply->url());

                return;
            }
            QList<QWebEngineFrame*> children = frame->childFrames();

            foreach (QWebEngineFrame* frame, children)
                frames.append(frame);
        }
        if (m_loadingUrl == reply->url())
            mainFrame()->setHtml(html, reply->url());
    }

#endif

    void PopupPage::authenticationRequired(const QUrl& requestUrl, QAuthenticator* auth)
    {
        // Browser *_browser = _entrance->activiated_registered().first;

        QDialog dialog(_browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::PasswordDialog passwordDialog;
        passwordDialog.setupUi(&dialog);

        passwordDialog.iconLabel->setText(QString());
        passwordDialog.iconLabel->setPixmap(
            _browser->style()
                ->standardIcon(QStyle::SP_MessageBoxQuestion, 0, _browser)
                .pixmap(32, 32));

        QString introMessage =
            tr("<qt>Enter username and password for \"%1\" at %2</qt>");
        introMessage = introMessage.arg(auth->realm())
                           .arg(requestUrl.toString().toHtmlEscaped());
        passwordDialog.introLabel->setText(introMessage);
        passwordDialog.introLabel->setWordWrap(true);
        if (dialog.exec() == QDialog::Accepted) {
            QByteArray key = ::sapp_t::authenticationKey(requestUrl, auth->realm());
            auth->setUser(passwordDialog.userNameLineEdit->text());
            auth->setPassword(passwordDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            ::sapp_t::instance()->setLastAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

    void PopupPage::proxyAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth, const QString& proxyHost)
    {
        Q_UNUSED(requestUrl);
        // Browser *_browser = _entrance->activiated_registered().first;

        QDialog dialog(_browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::ProxyDialog proxyDialog;
        proxyDialog.setupUi(&dialog);

        proxyDialog.iconLabel->setText(QString());
        proxyDialog.iconLabel->setPixmap(
            _browser->style()
                ->standardIcon(QStyle::SP_MessageBoxQuestion, 0, _browser)
                .pixmap(32, 32));

        QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
        introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
        proxyDialog.introLabel->setText(introMessage);
        proxyDialog.introLabel->setWordWrap(true);
        if (dialog.exec() == QDialog::Accepted) {
            QString user = proxyDialog.userNameLineEdit->text();
            QByteArray key =
                ::sapp_t::proxyAuthenticationKey(user, proxyHost, auth->realm());
            auth->setUser(user);
            auth->setPassword(proxyDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            ::sapp_t::instance()->setLastProxyAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

#endif // USE_POPUP_WINDOW

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(WebPage)
#endif

    WebPage::WebPage(Profile* profile,
        boost::intrusive_ptr<i_t> item,
        ts_t* tree_screen,
        Blogger* blogger_,
        web::Docker<web::Browser>* browser_docker_,
        web::Browser* browser,
        web::TabWidget* tabmanager,
        rctrl_t* rctrl_,
        WebView* parent)
        : QWebEnginePage(profile, parent)
        , _profile(profile)
        , _tree_screen(tree_screen)
        , _blogger(blogger_)
        , _browser_docker(browser_docker_)
        , _browser(browser)
        , _tab_widget(tabmanager)
        , _rctrl(rctrl_) //[&] { rctrl_->close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &web::WebView::close_requested, parent)); return rctrl_; }()
        , _view(parent)
        , _hovered_url(Browser::_defaulthome)
    {
        // This signal is emitted with the URL of the main frame when the main frame's
        // title is received. The new URL is specified by url.
        onUrlChanged = [&](const QUrl& url) -> void {
            // assert(_page->url() != QUrl());
            // assert(url != QUrl());
            if (_binder->host()) {
                if (url.toString() != "" && url != QUrl() && !url.host().isEmpty() &&
                    !url.scheme().isEmpty() && url.isValid() &&
                    url != QUrl(web::Browser::_defaulthome) && url != _loadingurl &&
                    url.toString() != _binder->host()->field<url_key>()) {
                    //#ifndef USE_SIGNAL_CLOSE
                    assert(_blogger);
                    //#endif // USE_SIGNAL_CLOSE
                    // if(url.toString() != ""){

                    record_info_update(url, title());
                    ////		    record_view_synchronize(_binder->host());
                    // }
                }
            }
        };
        //
        onTitleChanged = [&](const QString& title_) {
            // assert(this->url() != QUrl());
            assert(this->title() == title_);
            auto host_ = _binder->host();
            auto source_model = [&] { return _rctrl->source_model(); };
            auto title_field_description = fixedparameters.record_field_description(QStringList() << boost::mpl::c_str<name_key>::value)[boost::mpl::c_str<name_key>::value];
            int cc = static_cast<RecordModel*>(source_model())->columnCount();
            int title_column = 0;
            for (int i = 0; i < cc; i++) {
                auto header_title = source_model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(); // DisplayRole?UserRole
                if (header_title == title_field_description) {
                    title_column = i;
                    break;
                }
            }
            auto title_index = source_model()->index(static_cast<QModelIndex>(_rctrl->index<index_source>(host_)).row(), title_column);
            auto record_view_title = static_cast<RecordModel*>(source_model())->data(title_index, Qt::DisplayRole).toString();
            if (host_) {
                //				// if(  url() != QUrl()
                //				// && ! url().host().isEmpty()
                //				// && ! url().scheme().isEmpty()
                //				// && url() != QUrl(web::Browser::_defaulthome)
                //				// && url() == _loadingurl
                //				// && url().toString() == _binder->host()->field<url_type>()
                //				// ){
                //				//#ifndef USE_SIGNAL_CLOSE
                //				assert(_blogger);
                //				//#endif // USE_SIGNAL_CLOSE
                if ((title_ != "" &&
                        title_ != host_->field<name_key>()) // && !QUrl(title).isValid()
                    || (!record_view_title.contains(title_))) {
                    record_info_update(url(), title_);
                    // record_view_synchronize(_binder->host());
                }
                // }


                if (_blogger) {
                    if (_blogger->topic() == gl_para::_default_topic || _blogger->topic() == purify(host_->field<name_key>()) || _blogger->topic() == purify(host_->field<url_key>()))
                        _blogger->topic(title_);

                    auto tags_list = detail::to_qstring(host_->field<tags_key>());
                    bool changed = false;
                    if (tags_list.contains(gl_para::_default_topic)) {
                        tags_list.replace(gl_para::_default_topic, _blogger->topic());
                        changed = true;
                    } else if (tags_list.contains(purify(host_->field<name_key>()))) {
                        tags_list.replace(host_->field<name_key>(), _blogger->topic());
                        changed = true;
                    } else if (tags_list.contains(purify(host_->field<url_key>()))) {
                        tags_list.replace(host_->field<url_key>(), _blogger->topic());
                        changed = true;
                    }
                    if (changed) host_->field<tags_key>(detail::from_qstring<tags_key>(tags_list));
                }
            }
        };
        // , _keyboardmodifiers(Qt::NoModifier)
        // , _pressedbuttons(Qt::NoButton)

        _binder = [&]() -> boost::intrusive_ptr<::Binder> {
            if (item->is_lite())
                item->to_fat();
            item->binder_reset(); // item->binder().reset(); //->break_page();

            //// item_bind(item);  // you can't modify _binder before it's initialize
            ///time!!!

            // auto ar = std::make_shared<WebPage::Binder>(item, this);
            new ::Binder(std::make_shared<WebPage::Binder>(item, this));

            assert(item->binder()->integrity_external(item, this)); // item->binder() = new
                                                                    // TreeItem::coupler(std::make_shared<WebPage::coupler>(item,
                                                                    // this));
            assert(_binder->integrity_external(item, this));
            //			auto topic = _blogger->topic();
            //			if (!QStringList(item->field<tags_key>()).contains(topic))
            item->topic_replace(_blogger->topic()); //topic
            return std::forward<boost::intrusive_ptr<::Binder>>(item->binder());
        }();

        settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, true);
        settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, true);
        settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
        settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, true);
        settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
        settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
        settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
        setNetworkAccessManager(QtSingleApplication::networkAccessManager());
#endif
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        connect(static_cast<QWebEnginePage* const>(this), &QWebEnginePage::unsupportedContent, this, &WebPage::handleUnsupportedContent);
#endif
        connect(static_cast<QWebEnginePage* const>(this), &QWebEnginePage::authenticationRequired, this, &WebPage::authenticationRequired);
        connect(static_cast<QWebEnginePage* const>(this), &QWebEnginePage::proxyAuthenticationRequired, this, &WebPage::proxyAuthenticationRequired);

        connect(static_cast<QWebEnginePage* const>(this), &QWebEnginePage::titleChanged, onTitleChanged);
        connect(static_cast<QWebEnginePage* const>(this), &QWebEnginePage::urlChanged, onUrlChanged);
        connect(this, &WebPage::close_requested, //this, &WebPage::binder_reset);
            [&] {
                //			_blogger.reset();
                //			_rctrl.reset();
                //			binder_reset();
            });
        connect(this, &WebPage::linkHovered, [&](const QString& url) mutable { _hovered_url = url; });

        // update_record_view(item);
        connect(this, &WebPage::become_current, [&](boost::intrusive_ptr<::Binder> _binder) {
            auto host_ = _binder->host();
            if (host_) {
                auto _main_window = gl_paras->main_window();
                if (_main_window)
                    _main_window->synchronize_title(host_->field<name_key>());
            }
        });
        //		connect(this, &WebPage::become_current, [&](boost::intrusive_ptr<::Binder> _binder) {
        //			(void)_binder;
        //			this->metaeditor_sychronize();
        //		});
        auto old_page = static_cast<QWebEngineView*>(_view)->page();
        if (old_page != this)
            _view->page(this);
    }

    WebPage::~WebPage()
    {
        //		// if(_tab_widget->find([&](boost::intrusive_ptr<const ::Binder> b){return b
        //		// == _binder;}))record_view_remove(host());
        //		// if(_binder)binder_reset();	// break_page_shared_items();   // move to
        //		// on_close_requested()
        //		_blogger.reset();
        //		_rctrl.reset();
        //		binder_reset(); //_binder.reset();
    }

    // this will delete record for ever from database if you use real tree_item as
    // source model of record_view
    void WebPage::record_view_remove(boost::intrusive_ptr<i_t> host_)
    {
        if (_tab_widget && _tab_widget->count() > 0) {
            RecordModel* source_model = _rctrl->source_model(); // tab_manager->source_model();
            assert(host_);
            // assert((item->page_valid() && item->unique_page() == this) ||
            // !item->page_valid());
            if (source_model->item([&](const id_value id) {
                    return id == id_value(host_->field<id_key>());
                }))
                _rctrl->remove(
                    host_->id()); // && record->unique_page() == this
        }
    }

    void WebPage::record_info_update(const QUrl& url, const QString& title)
    {
        // Q_UNUSED(make_current)
        if (_binder->host()) {
            // _record->active_immediately(true);

            bool is_current = (_view == _tab_widget->currentWebView());
            if (is_current)
                emit become_current(_binder);
            int index = _tab_widget->webViewIndex(_view);
            _tab_widget->setTabToolTip(index, title);

            bool data_changed = false;
            if (title != "") {
                // &&
                if (title !=
                    _binder->host()->field<name_key>()) { // && ! QUrl(title).isValid()
                    // assert(QUrl(title).isValid());	// always true
                    // assert(title.contains("://"));	// not always true
                    if (detail::to_qstring(_binder->host()->field<name_key>()).length() == 0 ||
                        (detail::to_qstring(_binder->host()->field<name_key>()).length() > 0 &&
                            !title.contains("://")))
                        _binder->host()->field<name_key>(name_value(title)); // "name",

                    data_changed = true;
                }
                // metaeditor->setName(title);
                if (is_current) {
                    //	                                auto _main_window =
                    //gl_paras->main_window();

                    auto it = this->host();
                    if (it)
                        gl_paras->main_window()->synchronize_title(it->field<name_key>());
#ifdef USE_EDITOR_WRAP
                    _blogger->name(title);
#endif // USE_EDITOR_WRAP
                }
            }
            auto url_str = url.toString();
            auto real_url = url_value(url_str);
            if (url_str != "") {
                auto host_ = _binder->host();
                host_->field<url_key>(real_url); // "url",
                if (host_->field<home_key>() == web::Browser::_defaulthome && real_url != web::Browser::_defaulthome)
                    host_->field<home_key>(home_value(url_str));
                _loadingurl = url;
                data_changed = true;
#ifdef USE_EDITOR_WRAP
                if (is_current) {
                    _blogger->url(url_str);
                    _blogger->home(host_->field<home_type>());
                }
#endif // USE_EDITOR_WRAP
            }
            if (data_changed) { // && is_current, without is_current will lead current
                                // view lost editing focus
                                //				RecordIndex::select_as_current(_binder->host());
                auto source_model = [&]() {
                    return _tree_screen->view()->source_model();
                };
                source_model()->emit_datachanged_signal(source_model()->index(_binder->host()));
                _rctrl->on_recordtable_configchange();
            }
            // move to connect();
            //#ifdef USE_EDITOR_WRAP
            //			if(_blogger->item() != _binder->host() && is_current)
            //metaeditor_sychronize(); // metaeditor->bind(_record);

            //#else
            //			if(is_current) metaeditor_sychronize();
            //// metaeditor->bind(_record);
            //#endif //USE_EDITOR_WRAP
            // auto _tree_screen = globalparameters.tree_screen();
            auto it = _binder->host();
            auto tree_view = _tree_screen->view();
            if (is_current) { // globalparameters.mainwindow()
                if (tree_view->current_item() != it)
                    tree_view->select_as_current(TreeIndex::item_require_treeindex([&] { return tree_view->source_model(); }, it));
#ifdef USE_HAS_SELECTION
                auto _record_view = _rctrl->view();
                QItemSelectionModel* item_selection_model = _record_view->selectionModel();
                bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
                if (_rctrl->view()->current_item() != it
#ifdef USE_HAS_SELECTION
                    || !has_selection
#endif // USE_HAS_SELECTION
                    )
                    _rctrl->select_as_current(_rctrl->index<pos_proxy>(it)); // IdType(_binder->item()->field("id"))
            }
            if (is_current)
                _view->setFocus(); // make upate validate
        }
        // WebView *view = record->page()->view();

        // if(view->hasFocus()) {
        // view->tabmanager()->setCurrentWidget(view);
        // view->show();
        // }

        // }
    }


    // moved to Blogger
    //	void WebPage::metaeditor_sychronize()
    //	{
    //		boost::intrusive_ptr<i_t> current_item = _binder->host();
    //		// boost::intrusive_ptr<Record> record =
    //		// this->table_model()->table_data()->record(pos);
    //		assert(current_item);
    //		// assert(record ==
    //		// view()->tabmanager()->currentWebView()->page()->current_record()); // may
    //		// be in backend?

    //		// Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь
    //		// только получить данные из таблицы

    //		// Выясняется указатель на объект редактирования текста записи
    //		// MetaEditor *_editor_screen = globalparameters.meta_editor();    //
    //		// find_object<MetaEditor>(meta_editor_singleton_name);
    //		assert(_blogger);
    //// boost::intrusive_ptr<RecordModel::ModelIndex> record_index;
    //// try {
    //// record_index = new RecordModel::ModelIndex([&] {return
    //// _record_controller->source_model();},
    //// _record_controller->source_model()->sibling(current_item), current_item);
    //// } catch(std::exception &) {}
    //#ifdef USE_FILE_PER_TREEITEM // if(record_index) {
    //		if (_blogger->item() != current_item)
    //			_blogger->bind(current_item);
    //// int pos = indexOf(currentWebView());
    //// Turns the reference to the table of final data   // Выясняется ссылка на
    //// таблицу конечных данных
    //// std::shared_ptr<TableData> table = std::make_shared<TableData>();   //
    //// _source_model->table_data_internal();

    //// for(int i = 0; i < count(); i++) {
    //// table->insert_new_record(table->work_pos(), static_cast<WebView
    //// *>(widget(i))->page()->current_record());
    //// }

    //// В таблице конечных данных запоминается какая запись была выбрана
    //// чтобы затем при выборе этой же подветки засветка автоматически
    //// установилась на последнюю рабочую запись
    //// table->work_pos(pos);
    //#endif // USE_FILE_PER_TREEITEM
    //		// Устанавливается функция обратного вызова для записи данных
    //		_blogger->save_callback(Editor::editor_save_callback); // _editentry->editor_save_callback

    //		// Сохраняется текст и картинки в окне редактирования
    //		// find_object<MainWindow>("mainwindow")
    //		_blogger->save_text_context();
    //#ifdef USE_FILE_PER_TREEITEM
    //		// Для новой выбраной записи выясняется директория и основной файл
    //		if (current_item->field<id_type>().length() == 0)
    //			current_item
    //			    ->field<id_type>(current_item->field<dir_type>().length() > 0 ? current_item->field<dir_type>() : get_unical_id()); // "id",	// ||
    //																		// current_item->field("url")
    //																		// ==
    //																		// web::Browser::_defaulthome
    //		if (current_item->field<url_type>() == "")
    //			current_item->field<dir_type>(current_item->id()); // "dir",
    //		if (current_item->field<file_type>() == "")
    //			current_item->field<file_type>("text.html"); // "file",
    //		QString current_dir =
    //		    current_item->field<dir_type>(); // table->field(pos, "dir");
    //		QString current_file =
    //		    current_item->field<file_type>(); // table->field(pos, "file");
    //		QString full_dir = gl_paras->root_path() + "/" +
    //		    QDir(appconfig->data_dir()).dirName() + "/base/" +
    //		    current_dir;
    //		QString full_file_name = full_dir + "/" + current_file;
    //		qDebug() << " File " << full_file_name << "\n";
    //		// If the window contents of the record is already selected record  // Если в
    //		// окне содержимого записи уже находится выбираемая запись
    //		if (_blogger->work_directory() == full_dir &&
    //		    _blogger->file_name() == current_file) {
    //			gl_paras->window_switcher()->recordtable_ro_record_editor();

    //			return;
    //		}
    //		// Перед открытием редактора происходит попытка получения текста записи
    //		// Этот вызов создаст файл с текстом записи, если он еще не создан (подумать,
    //		// переделать)
    //		// Before the opening of the editor it attempts to get the text records
    //		// This call will create a text file with the record if it is not already
    //		// created (think remake)
    //		_blogger->save_textarea(); // table->text(pos);    // ?

    //		// едактору задаются имя файла и директории
    //		// И дается команда загрузки файла
    //		_blogger->work_directory(full_dir);
    //		_blogger->file_name(current_file);

    //		// Если идет работа с зашифрованной записью
    //		// И если имя директории или имя файла пусты, то это означает что
    //		// запись не была расшифрована, и редактор должен просто показывать пустой
    //		// текст
    //		// ничего не сохранять и не считывать
    //		qDebug() << "void WebPage::metaeditor_sychronize() : id "
    //			 << current_item->field<id_type>(); // table->field(pos, "id");
    //		qDebug() << "void WebPage::metaeditor_sychronize() : name "
    //			 << current_item->field<name_type>(); // table->field(pos, "name");
    //		qDebug() << "void WebPage::metaeditor_sychronize() : crypt "
    //			 << current_item->field<crypt_type>(); // table->field(pos,
    //							       // boost::mpl::c_str <
    //							       // crypt_type > ::value);
    //		if (current_item->field<crypt_type>() ==
    //		    "1") // table->field(pos, boost::mpl::c_str < crypt_type > ::value)
    //			if (full_dir.length() == 0 || current_file.length() == 0)
    //				_blogger->dir_file_empty_reaction(
    //				    EditorWrap::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);
    //		// _editentry->blog_editor()->editor()->clear_all_misc_field();
    //		// В редактор заносится информация, идет ли работа с зашифрованным текстом
    //		_blogger->misc_field(boost::mpl::c_str<crypt_type>::value,
    //		    current_item->field<crypt_type>()); // table->field(pos,
    //							// boost::mpl::c_str
    //							// < crypt_type >
    //							// ::value)

    //		// В редакторе устанавливается функция обратного вызова для чтения данных
    //		_blogger->load_callback(&Editor::editor_load_callback);

    //		_blogger->load_textarea();
    //		// edView->set_textarea(table->get_text(index.row()));

    //		// Заполняются прочие инфо-поля
    //		_blogger->pin(current_item->field<pin_type>()); // table->field(pos, "pin")

    //		_blogger->name(current_item->field<name_type>()); // table->field(pos, "name")

    //		_blogger->author(
    //		    current_item->field<author_type>()); // table->field(pos, "author")

    //		_blogger->home(current_item->field<home_type>()); // table->field(pos, "home")

    //		_blogger->url(current_item->field<url_type>()); // table->field(pos, "url")

    //		_blogger->tags(current_item->field<tags_type>()); // table->field(pos, "tags")

    //		auto id =
    //		    id_value(current_item->field<id_type>()); // table->field(pos, "id");
    //		_blogger->misc_field("id", id);

    //		_blogger->misc_field(
    //		    "title", current_item->field<name_type>()); // table->field(pos, "name")
    //		// should each record carry it's tree path?
    //		//// Set the path to the branch in which lies the record (in the form of the
    //		///names of the branches)   // Устанавливается путь до ветки в которой лежит
    //		///запись (в виде названий веток)
    //		// QString path = qobject_cast<TableScreen *>(parent())->getTreePath();
    //		//// В мобильном интерфейсе редактор должен показывать путь до записи
    //		// if(appconfig->getInterfaceMode() == "mobile")
    //		// meta_editor->setTreePath(path);
    //		// В редакторе восстанавливается позиция курсора и прокрутки если это
    //		// необходимо
    //		if (appconfig->remember_cursor_at_ordinary_selection()) {
    //			_blogger->cursor_position(walkhistory.cursor_position(id));
    //			_blogger->scrollbar_position(walkhistory.scrollbar_position(id));
    //		}
    //#else
    //		_blogger->misc_field<crypt_key>(current_item->field<crypt_key>()); // table->field(pos,
    //										   // boost::mpl::c_str
    //										   // < crypt_type >
    //										   // ::value)
    //		auto id = id_value(current_item->field<id_key>());                 // table->field(pos, "id");
    //		_blogger->misc_field<id_key>(id);
    //		_blogger->misc_field<name_key>(current_item->field<name_key>()); // table->field(pos, "name")
    //		//
    //		//	        auto id = _blogger->topic();
    //		// В редакторе восстанавливается позиция курсора и прокрутки если это
    //		// необходимо
    //		if (appconfig->remember_cursor_at_ordinary_selection()) {
    //			_blogger->cursor_position(walkhistory.cursor_position(static_cast<id_value>(id)));
    //			_blogger->scrollbar_position(walkhistory.scrollbar_position(static_cast<id_value>(id)));
    //		}
    //#endif // USE_FILE_PER_TREEITEM
    //		// Обновление иконки аттачей
    //		if (current_item->attach_table()->size() == 0) // table->record(pos)->getAttachTablePointer()->size()
    //			_blogger->to_attach()->setIcon(_blogger->icon_attach_not_exists());
    //		// Если нет приаттаченных файлов
    //		else
    //			_blogger->to_attach()->setIcon(_blogger->icon_attach_exists()); // Есть приаттаченные файлы

    //		// }
    //	}

    web::Browser* WebPage::browser()
    {
        return _browser;
    } // return _entrance->activiated_registered().first;
      // //QtSingleApplication::instance()->mainWindow();

    WebView* WebPage::load(boost::intrusive_ptr<i_t> item, bool switch_to, bool force_reload)
    {
        // Q_UNUSED(checked)
        assert(item);

        // auto tree_view = _tree_screen->tree_view();

        // boost::intrusive_ptr<TreeIndex> tree_modelindex(nullptr);

        // try {
        // tree_modelindex = new TreeIndex([&] {return tree_view->source_model();},
        // item->parent(),
        // item->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il)
        // {return il->host() == item && il == item->linker() && item->parent() ==
        // il->host_parent();}));
        // } catch(std::exception &e) {}

        // if(record_index) {

        boost::intrusive_ptr<i_t> it;
        if (item->binder()) { // && item->binder() == _binder
            if (!item->binder()->integrity_external(item, this)) {
                // auto it = tree_view->item_register(item,
                // std::bind(&KnowView::view_paste_child, tree_view, tree_modelindex,
                // std::placeholders::_2, std::placeholders::_3));
                it = bind(item);
                // item
                // , std::bind(&TreeScreen::view_paste_as_child, _tree_screen,
                // std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
                // );
            } else {
                if (it != item) it = item;
            }
        } else
            it = bind(item);
        if (switch_to)
            it->activate(std::bind(&wn_t::find, gl_paras->main_window(), std::placeholders::_1), force_reload);
        // }

        return _view;
    }

    WebView* WebPage::activate(bool force_reload)
    {
        QString _url_str = _binder->host()->field<url_key>();
        QUrl _url = QUrl(_url_str);
        if (_view                   // && _loadingurl.isValid()     // && url().isValid()    // &&	           // url().toString() != _url_str	    // && _url_str != web::Browser::_defaulthome		// url() may be	    // nothing
            && (_url != _loadingurl // lead loading stop
                   || !_activated)) {
            if (!_view->_load_finished || force_reload) {
                // triggerAction(QWebEnginePage::Stop);
                QWebEnginePage::setUrl(_url);
                QWebEnginePage::load(_url);


                auto target_ = _binder->host();

                assert(target_);
                if (target_) { // ->_active_request
                    // if(_record_binder->bounded_item()->_open_link_in_new_window == 1) { }
                    assert(_browser);
                    if (!_browser->isActiveWindow() || !_browser->isVisible()) {
                        if (_view) {
                            if (!_view->isTopLevel()) _view->raise();
                            if (!_view->isActiveWindow()) _view->activateWindow();
                        }
                        if (!_browser->isTopLevel()) _browser->raise();
                        if (!_browser->isActiveWindow()) _browser->activateWindow();
                    }
                    auto _vtab_record = gl_paras->vtab_record();
                    auto _record_screen = _browser->record_screen();
                    if (_record_screen) {
                        if (_vtab_record->currentWidget() != _record_screen)
                            _vtab_record->setCurrentWidget(_record_screen);
                    }

                    if (_view) {
                        if (_rctrl) {
                            auto record_view = _rctrl->view();
                            if (record_view) {
                                if (record_view->current_item() != target_ || _tab_widget->currentWebView() != _view || !_activated) {
                                    _tab_widget->setCurrentWidget(_view);
                                    _view->show();
                                    // if(checked) // globalparameters.mainwindow()

                                    //						_view->setFocus(); // make upate validate

                                    target_->add_rating();
                                    // assert(_lineedits);

                                    // if(_lineedits) {
                                    QLineEdit* line_edit = _view->toolbarsearch()->lineEdit(); // _tab_widget->currentLineEdit(); // qobject_cast<QLineEdit
                                                                                               // *>(_lineedits->currentWidget());

                                    std::mutex m;
                                    if (m.try_lock()) {
                                        //                                        std::lock_guard<std::mutex> lock(m);
                                        if (line_edit) line_edit->setText(_url_str);
                                        m.unlock();
                                    }

                                    // }
                                    auto current = [&] {
                                        boost::intrusive_ptr<i_t> r;
                                        auto v = _tab_widget->currentWebView();
                                        if (v) {
                                            auto p = v->page();
                                            if (p) {
                                                auto b = p->binder();
                                                if (b)
                                                    r = b->host();
                                            }
                                        }
                                        return r;
                                    }();
                                    assert(target_ == current);

#ifdef USE_HAS_SELECTION
                                    auto _record_view = _rctrl->view();
                                    QItemSelectionModel* item_selection_model = _record_view->selectionModel();
                                    bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
                                    if (_rctrl->source_model()->current_item() != current
#ifdef USE_HAS_SELECTION
                                        || !has_selection
#endif // USE_HAS_SELECTION
                                        ) {
                                        auto pp = _rctrl->index<pos_proxy>(current);
                                        // auto	index_proxy_	= _record_controller->index<index_proxy>(pp);
                                        _rctrl->select_as_current(pp);
                                        // auto v = _record_controller->view();
                                        // if(v){
                                        // v->setCurrentIndex(index_proxy_);
                                        // v->setFocus();
                                        // v->edit(index_proxy_);
                                        // }
                                        _view->current_view_global_consistency();
                                    }
                                }
                            }
                        }

                        _browser->adjustSize();
                        if (_view == _tab_widget->currentWebView()) {
                            _tab_widget->adjustSize();
                            _view->adjustSize();
                        }
                        _view->current_view_global_consistency();
                    }
                    auto tree_view_ = _tree_screen->view();
                    QList<boost::intrusive_ptr<i_t>> items_similar;
                    auto target_home = detail::to_string(target_->field<home_key>());
                    auto browser_home = detail::to_string(web::Browser::_defaulthome);
                    items_similar = tree_view_->source_model()->children(
                        [&](boost::intrusive_ptr<const i_t> it_) {

                            auto item_home = detail::to_string(it_->field<home_key>());

                            return (url_equal(it_->field<url_key>(), target_->field<url_key>()) //
                                       || it_->id() == target_->id()                            //
                                       || (item_home != ""                                      //
                                              && target_home != ""                              //
                                              && item_home != browser_home                      //
                                              && target_home != browser_home                    //
                                              && url_equal(item_home, target_home)))            //
                                && it_ != target_;                                              // || it_ == target_; // || (it_->field<home_type>() != "" && ti->field<home_type>() != "" && url_equal(it_->field<home_type>().toStdString(), ti->field<home_type>().toStdString()))
                        });

                    if (items_similar.size() > 0) { //1
                        //				QList<boost::intrusive_ptr<i_t>> others_same;
                        //				for (auto it_ : items_similar) {
                        //					if (it_ != target_)
                        //						if (!others_same.contains(it_))
                        //							others_same << it_;
                        //				}
                        for (auto _it : items_similar) { //others_same
                            if (_it->binder()) {
                                if (_it->binder()->page()) {
                                    target_ = TreeLevel::instance(TreeIndex::item_require_treeindex([&] { return tree_view_->source_model(); }, target_), _it)->merge(); // TreeIndex::instance([&] {return v->source_model();}, ti, ti->parent()), _it);
                                                                                                                                                                         // std::thread(&KnowView::view_merge, v, TreeIndex::instance([&] {return v->source_model();}, it->parent(), it), j_).join();
                                }
                            }
                        }
                    }
                    //                        auto target_ = _binder->host();
                    auto tree_view = _tree_screen->view();
                    if (target_ != tree_view->current_item()) tree_view->select_as_current(TreeIndex::item_require_treeindex([&] { return tree_view->source_model(); }, target_));
                }
            }
        }

        // recovery_global_consistency();
        // target_ = _page->binder()->host();


        _activated = true;
        return _view;
    }

    boost::intrusive_ptr<i_t>
    WebPage::bind(boost::intrusive_ptr<i_t> host_)
    { // , web::WebPage *page
        boost::intrusive_ptr<i_t> result(nullptr);

        // auto binder = [](boost::shared_ptr<WebPage::RecordBinder> ar) {
        // return
        // std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
        // web::WebView *, boost::intrusive_ptr<TreeItem>   // ,
        // boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
        // >>("", &WebPage::RecordBinder::binder, ar);
        // };
        // auto activator = [](boost::shared_ptr<WebPage::RecordBinder> ar) {
        // return
        // std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
        // web::WebView *>>("", &WebPage::RecordBinder::activator, ar);
        // };

        // registered record, but have no generator:
        // auto ar =
        // auto _tree_screen = globalparameters.tree_screen();

        // if(!item->is_registered_to_browser() && !item->record_binder()) {
        auto tree_view = _tree_screen->view();
        assert(static_cast<QModelIndex>(tree_view->source_model()->index(host_)).isValid());

        // if(!tree_view->source_model()->index(item).isValid())
        // result = tree_view->item_register(item,
        // std::bind(&KnowView::view_paste_child, tree_view, tree_index,
        // std::placeholders::_2, std::placeholders::_3));
        // else
        result = host_;

        auto create_coupler = [&](boost::intrusive_ptr<i_t> result_item) {
            // auto ar = std::make_shared<WebPage::Binder>(result_item, this);
            auto result_coupler = new ::Binder(std::make_shared<WebPage::Binder>(result_item, this));
            // std::move(::Binder::item_interface("", &WebPage::Binder::item, ar))
            // // std::make_shared<TreeItem::coupler::item_interface>("", &WebPage::coupler::item_link, ar)
            // , std::move(::Binder::page_interface("", &WebPage::Binder::page, ar))
            // // std::make_shared<TreeItem::coupler::page_interface>("", &WebPage::coupler::page_link, ar)
            // , std::move(::Binder::bind_interface("", &WebPage::Binder::bind, ar))
            // // std::make_shared<TreeItem::coupler::bind_interface>("", &WebPage::coupler::bind, ar)
            // , std::move(::Binder::activate_interface("", &WebPage::Binder::activator,
            // ar))    // std::make_shared<TreeItem::coupler::activate_interface>("", &WebPage::coupler::activator, ar) );

            assert(_binder->integrity_external(result_item, this));               // _binder.reset(new TreeItem::coupler(std::make_shared<WebPage::coupler>(result, this)));
            assert(result_item->binder()->integrity_external(result_item, this)); // result->binder(std::forward<boost::intrusive_ptr<TreeItem::coupler>&>(_binder));
                                                                                  //#ifdef USE_SIGNAL_CLOSE                                                                       //			_rctrl->close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &web::WebPage::binder_reset, this));
                                                                                  //			_rctrl->destroy_transfer([&](sd::renter* const r) {
                                                                                  //				(void)r;
                                                                                  //				this->binder_reset();// recursive call _rctrl
                                                                                  //			}); //std::bind(&web::WebPage::binder_reset, this)
                                                                                  //#endif
            return result_coupler;
        };
        if (result->binder() && result->binder() == _binder) {
            if (!result->binder()->integrity_external(result, this)) {
                if (_binder)
                    binder_reset();
                create_coupler(result);
            }
        } else {
            if (_binder)
                binder_reset();
            create_coupler(result);
        }
        // std::make_shared<WebPage::Coupler>(this, item);

        // } else {
        // if(_record_binder != item->record_binder())_record_binder =
        // item->record_binder();
        // }

        ////        it->binder(
        ////            binder(ar)
        ////        );

        ////        it->activator(
        ////            activator(ar)
        ////        );

        // it->record_binder(std::make_shared<CouplerDelegation>(
        // std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
        // , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
        // , std::make_shared<bind_interface>("", &Coupler::binder, ar)
        // , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
        // ));

        assert(result->binder());
        //		auto topic = _blogger->topic();
        //		if (!QStringList(result->field<tags_key>()).contains(topic))
        result->topic_replace(_blogger->topic()); //topic
                                                  //		assert(QStringList(result->field<tags_key>()).contains(topic));
                                                  //		RecordIndex::select_as_current(result);
        return result;
    }

    // deprecated
    void WebPage::binder_reset()
    {
        // {	// if(sychronize_to_record_view)// _tab_widget
        ////	_record_controller->on_recordtable_configchange();
        // }
        //// _record_controller->delete_items_selected();   //
        ///source_model()->on_table_config_changed();
        if (_binder) {
            // auto _host_binder = _binder->host() ? _binder->host()->binder() :
            // nullptr;
            ////	    assert(_host_binder == _binder);
            // if(_host_binder && _host_binder == _binder){
            ////		if(_host_binder->page())
            // _host_binder->page(nullptr);
            ////		if(_host_binder->host())
            // _host_binder->host(nullptr);
            ////		// _binder->break_page();   // item_break(_binder->item_link());  //
            ///break_items();

            ////		_host_binder.reset(nullptr);
            // }else{
            {
                //				//	                        _binder->page(nullptr);

                //				//	                        _binder->host(nullptr);
                //				_binder = nullptr;
            }
            // }
            ////            auto _page_binder = _binder->page() ?
            ///_binder->page()->binder() : nullptr;
            ////            if(_page_binder){
            ////                assert(_page_binder == _binder);
            ////                if(_page_binder->page())_page_binder->page(nullptr);
            ////
            ///if(_page_binder->host()){_page_binder->host(boost::intrusive_ptr<TreeItem>(nullptr));}
            //////		_binder->page()->binder(nullptr);
            //////		_binder->page(nullptr);
            ////                _page_binder = nullptr;
            ////            }
            // _binder.reset(nullptr);
        }
        _rctrl->on_recordtable_configchange();
    }

    bool WebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
    {
        Q_UNUSED(type);
        (void)isMainFrame;
        if (type == QWebEnginePage::NavigationTypeLinkClicked) {
            // retrieve the url here
            _hovered_url = url.toString();
            //			return false;
        }
        return true;

        //                if (isMainFrame) {
        //                        if (_loadingurl != url) {
        //                                _loadingurl = url;
        //                                emit loadingUrl(_loadingurl);
        //                        }
        //                }
        //                return true;
    }

    bool WebPage::certificateError(const QWebEngineCertificateError& error)
    {
        if (error.isOverridable() && !_certificate_ignored) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(error.errorDescription());
            msgBox.setInformativeText(
                tr("If you wish so, you may continue with an unverified certificate. "
                   "Accepting an unverified certificate means "
                   "you may not be connected with the host you tried to connect to.\n"
                   "Do you wish to override the security check and continue?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            return _certificate_ignored = (msgBox.exec() == QMessageBox::Yes);
        }
        // QMessageBox::critical(_view, tr("Certificate Error"),
        // error.errorDescription(), QMessageBox::Ok, QMessageBox::NoButton);

        // return false;
        return _certificate_ignored;
    }

    WebView* WebView::instance(FindScreen* find_screen_, url_value const& real_url)
    {
        gl_paras->find_screen(find_screen_);
        //		WebPage* page = nullptr;
        auto find_binder = [&](boost::intrusive_ptr<const ::Binder> b) {
            return url_equal(b->host()->field<url_key>(), real_url) || url_equal(b->host()->field<home_key>(), real_url);
        };
        auto find_target = [&]() {
            return gl_paras->main_window()->find(find_binder);
        };
        auto tree_view = gl_paras->tree_screen()->view();
        WebView* v = find_target();
        if (v) {
            v->tabmanager()->closeTab(v->tabmanager()->indexOf(v));
        }
        auto _browser_new = real_url_t<url_value>::instance<web::Browser*>(real_url,
            [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) {
                return gl_paras->main_window()->browser(real_target_url_, true); // QtSingleApplication::instance()->newMainWindow();
            });
        auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url, // this_index->
            [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                return TreeIndex::url_require_item_from_tree(
                    real_target_url_, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                    [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                        return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(url_value(detail::to_qstring(it_->field<url_key>())), real_url);
                    }); // return it_->field<url_type>() == target_url.toString();
                        // web::Browser::_defaulthome
            });

        auto new_item = _browser_new->bind(
            RecordIndex::instance([&] {
                RecordModel* rm = nullptr;
                auto rctrl = _browser_new->tab_widget()->record_screen()->record_ctrl();
                if (rctrl)
                    rm = rctrl->source_model();
                return rm;
            },
                it));

        auto view_ = new_item->activate(std::bind(&wn_t::find, gl_paras->main_window(), std::placeholders::_1));
        ////		page = view_->page();
        //		auto url_new = QUrl(new_item->field<name_key>());
        //		auto to_br_url_ = to_be_url(url_new);
        //		auto topic_new = (to_br_url_ == detail::to_qstring(Browser::_defaulthome) || to_br_url_ == QUrl()) ? purify(new_item->field<name_key>()) : purify(url_new.fileName());
        //		if (topic_new == "") topic_new = purify(url_new.path());
        //		if (topic_new == "") topic_new = purify(url_new.topLevelDomain());
        //		if (topic_new == "") topic_new = purify(gl_para::_default_topic);
        //		new_item->topic_append(topic_new);
        //		_browser_new->blogger()->topic(topic_new);
        return view_;
    }
    // #include "webview.moc"

    QWebEnginePage* WebPage::createWindow(QWebEnginePage::WebWindowType type)
    {
        // QString url_	= "";
        // QString script	= "location.href";	// "document.title";
        // //"JavaScript:function(){return this.href;}();";
        // runJavaScript(script, [&](const QVariant &var){url_ =
        // var.toString();assert(url_ != "");});
        // assert(url_ != "");
        QUrl target_url = QUrl(_hovered_url);

        if (!target_url.isValid())
            target_url = web::Browser::_defaulthome;
        auto real_url = url_value(target_url.toString());
        // assert(url != "");

        // QWebChannel *channel = new QWebChannel(this);
        // QVariant _content;
        // channel->registerObject(QStringLiteral("content"), &_content);
        // this->setWebChannel(channel);

        // QWebEnginePage
        WebPage* page = nullptr;

        auto tree_view = _tree_screen->view();

        assert(
            static_cast<QModelIndex>(tree_view->source_model()->index(this->host()))
                .isValid());
        auto parent = this->host() ? this->host()->parent() : nullptr;
        assert(parent);
        // auto parent_parent = parent->parent();

        boost::intrusive_ptr<TreeIndex> this_treeindex = TreeIndex::item_require_treeindex([&] { return tree_view->source_model(); }, this->host()); // parent_parent ? TreeIndex::instance([&] {return
                                                                                                                                                     // tree_view->source_model();}, parent) :
                                                                                                                                                     // TreeIndex::instance([&] {return
                                                                                                                                                     // tree_view->source_model();}, this->host());
        auto find_binder = [&](boost::intrusive_ptr<const ::Binder> b) {
            return url_equal(b->host()->field<url_key>(), real_url) || url_equal(b->host()->field<home_key>(), real_url);
        };
        auto find_target = [&]() {
            return gl_paras->main_window()->find(find_binder);
        };
        if (type == QWebEnginePage::WebBrowserWindow) { // this means a new topic at current design, it is not good.
            WebView* v = find_target();
            if (v)
                v->tabmanager()->closeTab(v->tabmanager()->indexOf(v));
            auto _browser_new = real_url_t<url_value>::instance<web::Browser*>(real_url,
                [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) {
                    return gl_paras->main_window()->browser(real_target_url_, true); // QtSingleApplication::instance()->newMainWindow();
                });
            auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url, // this_index->
                [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                    return TreeIndex::url_require_item_from_tree(
                        real_target_url_, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                        [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                            return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(url_value(detail::to_qstring(it_->field<url_key>())), real_url);
                        }); // return it_->field<url_type>() == target_url.toString();
                            // web::Browser::_defaulthome
                });

            auto new_item = _browser_new->bind(
                RecordIndex::instance([&] {
                    RecordModel* rm = nullptr;
                    auto rctrl = _browser_new->tab_widget()->record_screen()->record_ctrl();
                    if (rctrl)
                        rm = rctrl->source_model();
                    return rm;
                },
                    it));

            auto view_ = new_item->activate(std::bind(&wn_t::find, gl_paras->main_window(), std::placeholders::_1));
            page = view_->page();
            auto url_new = QUrl(new_item->field<name_key>());
            auto to_br_url_ = to_be_url(url_new);
            auto topic_new = (to_br_url_ == detail::to_qstring(Browser::_defaulthome) || to_br_url_ == QUrl()) ? purify(new_item->field<name_key>()) : purify(url_new.fileName());
            if (topic_new == "") topic_new = purify(url_new.path());
            if (topic_new == "") topic_new = purify(url_new.topLevelDomain());
            if (topic_new == "") topic_new = purify(gl_para::_default_topic);
            new_item->topic_replace(topic_new);
            _browser_new->blogger()->topic(topic_new);
            assert(page);
        } else if (type == WebBrowserBackgroundTab) {
            //// should I think about always open new window in new tab
            //// _openinnewtab = false;  // true
            // QUrl current = url();
            // QUrl requestedurl = requestedUrl(); //equal to current page url

            WebView* view = find_target();
            // return b->host()->field<url_type>() == target_url.toString();
            // auto tree_view = _tree_screen->view();
            if (view) {
                page = view->page();
                auto _item = page->host();
                if (_item) {
                    _item->topic_replace(_blogger->topic()); //target_url.toString()
                                                             // auto _index = tree_view->source_model()->index(_item);
                                                             // if(static_cast<QModelIndex>(_index).isValid())_item->activate(std::bind(&HidableTabWidget::find,
                                                             // globalparameters.main_window()->vtab_record(),
                                                             // std::placeholders::_1));	//
                                                             // tree_view->index_invoke(TreeIndex::instance([&] {return
                                                             // tree_view->source_model();}, _item, _item->parent()));	// view,
                }
                assert(page->binder() && page->binder()->integrity_external(page->host(), page));
                assert(static_cast<QModelIndex>(tree_view->source_model()->index(page->host())).isValid());
                assert(page);
            } else {
                // WebPage *page = this->dockedwindow()->tabWidget()->new_view(new_record,
                // true)->page();
                // already create window, why do this? -- refer to demo browser
                assert(static_cast<QModelIndex>(tree_view->source_model()->index(this->_binder->host())).isValid());

                auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
                    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                        return this_treeindex->url_bind_browser(
                            real_target_url_, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), // std::placeholders::_1
                            [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                                return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url);
                            },
                            browser_ref());
                    });
                assert(it);
                // page = it ? it->activate(std::bind(&HidableTabWidget::find,
                // globalparameters.main_window()->vtab_record(),
                // std::placeholders::_1))->page() : nullptr;
                if (it)
                    it->topic_replace(_blogger->topic()); //target_url.toString()
                page = it ? it->page() : nullptr;
                //				if (!page) page = this; //	assert(page || _hovered_url == web::Browser::_defaulthome || _hovered_url == "");
            }
            //			if (!page) page = this; //	assert(page || _hovered_url == web::Browser::_defaulthome || _hovered_url == "");
        } else
#ifdef USE_POPUP_WINDOW
            if (type == WebBrowserTab)
#endif
        {
            //// should I think about always open new window in new tab
            //// _openinnewtab = false;  // true
            // QUrl current = url();
            // QUrl requestedurl = requestedUrl(); //equal to current page url

            WebView* view = find_target();
            // return b->host()->field<url_type>() == target_url.toString();
            // auto tree_view = _tree_screen->view();
            if (view) {
                page = view->page();
                auto _item = page->host();
                if (_item) {
                    auto _index = tree_view->source_model()->index(_item);
                    if (static_cast<QModelIndex>(_index).isValid())
                        _item->activate(std::bind(&wn_t::find, gl_paras->main_window(),
                            std::placeholders::_1)); // tree_view->index_invoke(TreeIndex::instance([&]
                                                     // {return tree_view->source_model();}, _item,
                                                     // _item->parent()));	// view,
                    _item->topic_replace(_blogger->topic());
                }
                assert(page->binder() && page->binder()->integrity_external(page->host(), page));
                assert(static_cast<QModelIndex>(tree_view->source_model()->index(page->host())).isValid());
                assert(page);
            } else {
                // WebPage *page = this->dockedwindow()->tabWidget()->new_view(new_record,
                // true)->page();
                // already create window, why do this? -- refer to demo browser
                assert(static_cast<QModelIndex>(tree_view->source_model()->index(this->_binder->host())).isValid());

                auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
                    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                        return this_treeindex->url_bind_browser(
                            real_target_url_, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), // std::placeholders::_1
                            [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                                return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url);
                            },
                            browser_ref());
                    });
                assert(it);
                if (it)
                    it->topic_replace(_blogger->topic());
                page = it ? it->activate(std::bind(&wn_t::find, gl_paras->main_window(), find_binder))->page() : nullptr; // std::placeholders::_1

                //				if (!page) page = this; //				assert(page || _hovered_url == web::Browser::_defaulthome || _hovered_url == "");
            }
            //			if (!page) page = this; //			assert(page || _hovered_url == web::Browser::_defaulthome || _hovered_url == "");
        }
#ifdef USE_POPUP_WINDOW
        else {                                             // type == WebDialog
            PopupWindow* popup = new PopupWindow(_browser, // view()->tabmanager()
                _profile
                // , QUrl(web::Browser::_defaulthome)
                // , _record_controller
                // , _page_controller
                // , view()->tabmanager()->browser()
                );

            popup->setAttribute(Qt::WA_DeleteOnClose);
            popup->show();
            // return

            // page =
            return popup->page();
        }
#endif // USE_POPUP_WINDOW
        // }
        // if(page) {
        //// not realy needed for each time
        // connect(static_cast<QWebEnginePage *const>(page), &QWebEnginePage::setUrl,
        // [&](const QUrl & url) {
        // boost::intrusive_ptr<TreeItem> _current_item = page->_binder->item();
        // if(_current_item->field("url") != url.toString()) {
        // _current_item->field("url", url.toString());
        // page->load(_current_item); // record->generate();
        // page->activate();     // record->active();
        // } else
        // page->activate();
        // });
        // connect(static_cast<QWebEnginePage *const>(page), &QWebEnginePage::load,
        // [&](const QUrl & url) {
        // boost::intrusive_ptr<TreeItem> _current_item = page->_binder->item();
        // if(_current_item->field("url") != url.toString()) {
        // _current_item->field("url", url.toString());
        // page->load(_current_item); // record->generate();
        // page->activate();     // record->active();
        // } else
        // page->activate();
        // });
        // }
        if (!page) page = this; // because of _hovered_url == "javascript:void(0)" or something else//		assert(page || _hovered_url == web::Browser::_defaulthome || _hovered_url == "" || type == WebDialog);

        return page;
    }

#if !defined(QT_NO_UITOOLS)
    QObject* WebPage::createPlugin(const QString& classId, const QUrl& url, const QStringList& paramNames, const QStringList& paramValues)
    {
        Q_UNUSED(url);
        Q_UNUSED(paramNames);
        Q_UNUSED(paramValues);
        QUiLoader loader;

        return loader.createWidget(classId, _view);
    }

#endif // !defined(QT_NO_UITOOLS)

#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
    void WebPage::handleUnsupportedContent(QNetworkReply* reply)
    {
        QString errorString = reply->errorString();
        if (_loadingurl != reply->url()) {
            // sub resource of this page
            qWarning() << "Resource" << reply->url().toEncoded()
                       << "has unknown Content-Type, will be ignored.";
            reply->deleteLater();

            return;
        }
        if (reply->error() == QNetworkReply::NoError &&
            !reply->header(QNetworkRequest::ContentTypeHeader).isValid())
            errorString = "Unknown Content-Type";
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
        QIcon icon =
            view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
        QPixmap pixmap = icon.pixmap(QSize(32, 32));
        if (pixmap.save(&imageBuffer, "PNG"))
            html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"), QString(QLatin1String(imageBuffer.buffer().toBase64())));
        QList<QWebEngineFrame*> frames;
        frames.append(mainFrame());
        while (!frames.isEmpty()) {
            QWebEngineFrame* frame = frames.takeFirst();
            if (frame->url() == reply->url()) {
                frame->setHtml(html, reply->url());

                return;
            }
            QList<QWebEngineFrame*> children = frame->childFrames();

            foreach (QWebEngineFrame* frame, children)
                frames.append(frame);
        }
        if (_loadingurl == reply->url())
            mainFrame()->setHtml(html, reply->url());
    }

#endif

    void WebPage::authenticationRequired(const QUrl& requestUrl, QAuthenticator* auth)
    {
        // if(_entrance->browsers().count() == 0) {
        // _entrance->new_browser(QUrl(web::Browser::_defaulthome));
        // }

        //	        Browser *_browser =
        //gl_paras->main_window()->activated_browser(); //
        //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(_browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::PasswordDialog passwordDialog;
        passwordDialog.setupUi(&dialog);

        passwordDialog.iconLabel->setText(QString());
        passwordDialog.iconLabel->setPixmap(_browser->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, _browser).pixmap(32, 32));

        QString introMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
        introMessage = introMessage.arg(auth->realm()).arg(requestUrl.toString().toHtmlEscaped());
        passwordDialog.introLabel->setText(introMessage);
        passwordDialog.introLabel->setWordWrap(true);
        if (dialog.exec() == QDialog::Accepted) {
            QByteArray key = ::sapp_t::authenticationKey(requestUrl, auth->realm());
            auth->setUser(passwordDialog.userNameLineEdit->text());
            auth->setPassword(passwordDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            ::sapp_t::instance()->setLastAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

    void WebPage::proxyAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth, const QString& proxyHost)
    {
        Q_UNUSED(requestUrl);

        // if(_entrance->browsers().count() == 0) {
        // _entrance->new_browser(QUrl(web::Browser::_defaulthome));
        // }

        //	        Browser *_browser =
        //gl_paras->main_window()->activated_browser(); //
        //QtSingleApplication::instance()->mainWindow();

        QDialog dialog(_browser);
        dialog.setWindowFlags(Qt::Sheet);

        Ui::ProxyDialog proxyDialog;
        proxyDialog.setupUi(&dialog);

        proxyDialog.iconLabel->setText(QString());
        proxyDialog.iconLabel->setPixmap(
            _browser->style()
                ->standardIcon(QStyle::SP_MessageBoxQuestion, 0, _browser)
                .pixmap(32, 32));

        QString introMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
        introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
        proxyDialog.introLabel->setText(introMessage);
        proxyDialog.introLabel->setWordWrap(true);
        if (dialog.exec() == QDialog::Accepted) {
            QString user = proxyDialog.userNameLineEdit->text();
            QByteArray key =
                ::sapp_t::proxyAuthenticationKey(user, proxyHost, auth->realm());
            auth->setUser(user);
            auth->setPassword(proxyDialog.passwordLineEdit->text());
            auth->setOption("key", key);
            ::sapp_t::instance()->setLastProxyAuthenticator(auth);
        } else {
            // Set authenticator null if dialog is cancelled
            *auth = QAuthenticator();
        }
    }

#ifdef USE_POPUP_WINDOW

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(PopupView)
#endif

    PopupView::PopupView(QWidget* parent)
        : QWebEngineView(parent)
        , _progress(0)
        , _page(0)
        , _icon_reply(0)
    {
        connect(this, &PopupView::loadProgress, this, &PopupView::setProgress);
        connect(this, &QWebEngineView::loadFinished, this, &PopupView::loadFinished);
        connect(this, &QWebEngineView::renderProcessTerminated, [=](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            const char* status = "";
            switch (termStatus) {
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
            qInfo() << "Render process exited with code" << statusCode
                    << status;
            QTimer::singleShot(0, [this] { reload(); });
        });
    }

    void PopupView::setPage(PopupPage* page_)
    {
        auto old_page = QWebEngineView::page();
        if (_page != page_ || old_page != page_) {
            if (_page != old_page) {
                if (old_page) {
                    old_page->deleteLater();
                    old_page = nullptr;
                }
                if (_page) {
                    _page->deleteLater();
                    _page = nullptr;
                }
            } else {
                if (_page) {
                    _page->deleteLater();
                    _page = nullptr;
                }
            }
        }
        if (!_page || _page != page()) {
            _page = page_;
            QWebEngineView::setPage(page_);
#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
            connect(page(), &PopupPage::statusBarMessage, &PopupView::setStatusBarText);
#endif
            connect(_page, &PopupPage::loadingUrl, this, &PopupView::urlChanged);
            connect(page(), &PopupPage::iconUrlChanged, this, &PopupView::onIconUrlChanged);
            connect(page(), &PopupPage::featurePermissionRequested, this, &PopupView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
            page()->setForwardUnsupportedContent(true);
#endif
        }
    }

    void PopupView::contextMenuEvent(QContextMenuEvent* event)
    {
        QMenu* menu = page()->createStandardContextMenu();
        const QList<QAction*> actions = menu->actions();
        QList<QAction*>::const_iterator it =
            qFind(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::OpenLinkInThisWindow));
        if (it != actions.cend()) {
            (*it)->setText(tr("Open Link in This Window"));
            ++it;
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewWindow));
            menu->insertAction(*it, page()->action(QWebEnginePage::OpenLinkInNewTab));
        }
        menu->popup(event->globalPos());
    }

    void PopupView::wheelEvent(QWheelEvent* event)
    {
#if defined(QWEBENGINEPAGE_SETTEXTSIZEMULTIPLIER)
        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
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

    void PopupView::onFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature feature)
    {
        FeaturePermissionBar* permissionBar = new FeaturePermissionBar(this);
        connect(permissionBar, &FeaturePermissionBar::featurePermissionProvided, page(), &QWebEnginePage::setFeaturePermission);

        // Discard the bar on new loads (if we navigate away or reload).
        connect(page(), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

        permissionBar->requestPermission(securityOrigin, feature);
    }

    void PopupView::setProgress(int progress)
    {
        _progress = progress;
    }

    void PopupView::loadFinished(bool success)
    {
        if (success && 100 != _progress) {
            qWarning() << "Received finished signal while progress is still:"
                       << progress() << "Url:" << url();
        }
        _progress = 0;
    }

    void PopupView::loadUrl(const QUrl& url)
    {
        _initial_url = url;
        load(url);
    }

    QString PopupView::lastStatusBarText() const
    {
        return _statusbar_text;
    }

    QUrl PopupView::url() const
    {
        QUrl url = QWebEngineView::url();
        if (!url.isEmpty())
            return url;
        return _initial_url;
    }

    QIcon PopupView::icon() const
    {
        if (!_icon.isNull())
            return _icon;
        return ::sapp_t::instance()->defaultIcon();
    }

    void PopupView::onIconUrlChanged(const QUrl& url)
    {
        QNetworkRequest iconRequest(url);
        _icon_reply = ::sapp_t::networkAccessManager()->get(iconRequest);
        _icon_reply->setParent(this);
        connect(_icon_reply, &QNetworkReply::finished, this, &PopupView::iconLoaded);
    }

    void PopupView::iconLoaded()
    {
        _icon = //qobject_cast<::sapp_t*>(
            ::sapp_t::instance()
                //			    )
                ->defaultIcon(); // QIcon();
        if (_icon_reply) {
            QByteArray data = _icon_reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);
            _icon.addPixmap(pixmap);
            _icon_reply->deleteLater();
            _icon_reply = 0;
        }
        emit iconChanged();
    }

    void PopupView::mousePressEvent(QMouseEvent* event)
    {
        _page->_pressed_buttons = event->buttons();
        _page->_keyboard_modifiers = event->modifiers();
        QWebEngineView::mousePressEvent(event);
    }

    void PopupView::mouseReleaseEvent(QMouseEvent* event)
    {
        QWebEngineView::mouseReleaseEvent(event);
        if (!event->isAccepted() && (_page->_pressed_buttons & Qt::MiddleButton)) {
            QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
            if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty())
                setUrl(url);
        }
    }

    void PopupView::setStatusBarText(const QString& string)
    {
        _statusbar_text = string;
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(PopupWindow)
#endif

    PopupWindow::PopupWindow(web::Browser* browser_, Profile* profile_)
        : QWidget(browser_)
        , _browser(browser_)
        , _addressbar(new QLineEdit(this))
        , _view(new PopupView(this))
    {
        setWindowFlags(Qt::Dialog); // Qt::Window |
        _view->setPage(new PopupPage(_view, browser_, profile_));
        QVBoxLayout* layout = new QVBoxLayout;
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

    void PopupWindow::adjustGeometry(const QRect& newGeometry)
    {
        const int x1 = frameGeometry().left() - geometry().left();
        const int y1 = frameGeometry().top() - geometry().top();
        const int x2 = frameGeometry().right() - geometry().right();
        const int y2 = frameGeometry().bottom() - geometry().bottom();

        setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
    }

#endif // USE_POPUP_WINDOW

    void WebView::onLoadFinished(bool success)
    {
        if (success && 100 != _progress) {
            qWarning() << "Received finished signal while progress is still:"
                       << progress() << "Url:" << _page->url();
        }
        if (success) {
            _page->onUrlChanged(_page->url());
            _page->onTitleChanged(_page->title());

            _load_finished_signal(success);
            //            if (_toolbarsearch) {

            //                auto line_edit = _toolbarsearch->lineEdit();
            //                if (line_edit) {
            //                    auto url_str = _page->url().toString();
            //                    if (line_edit->text() != url_str) line_edit->setText(url_str);
            //                }
            //            }
            _load_finished = true;
            // setFocus();
        }
        _progress = 0;
    }

    bool WebView::load_finished() const
    {
        return _load_finished;
    }

    WebView::~WebView()
    {
        //		//	        if(_page){
        //		//	                // delete _page;
        //		//	                // _page = nullptr; //
        //		//	                // emit _page->close_requested();
        //		//	                _page->deleteLater();
        //		//		}
        //		if (_toolbarsearch) {
        //			_toolbarsearch->deleteLater();
        //			_toolbarsearch = nullptr;
        //		}
    }

    WebPage* WebView::page() const
    {
        return _page;
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(WebView)
#endif

    WebView::WebView(boost::intrusive_ptr<i_t> host_,
        Profile* profile, // , bool openinnewtab
        ts_t* tree_screen,
        Blogger* blogger_,
        web::Docker<web::Browser>* browser_docker_, web::Browser* browser, web::TabWidget* tabmanager,
        rctrl_t* rctrl_)
        : QWebEngineView(static_cast<QWidget*>(tabmanager)) // ->parent()
        , _toolbarsearch(new ToolbarSearch(this))
        , _browser(browser)
        , _tab_widget(tabmanager) // , _record(record)
        , _rctrl(rctrl_)
        , _page([&]() -> WebPage* {
            _page = nullptr;
            auto p = new WebPage(profile, host_, tree_screen, blogger_, browser_docker_, browser, tabmanager, rctrl_, this);
            return p;
        }()) // , openinnewtab
        // , _initialurl(record ? record->getNaturalFieldSource("url") : QUrl())
        , _progress(0)
        , _icon_reply(0)
    {
        settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

        settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, true);
        settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, true);
        settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
        settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, true);
        settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
        settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
        settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

        // if(record) {record->view(this);}

        connect(this, &WebView::loadProgress, this, &WebView::setProgress);

        connect(this, &WebView::loadFinished, this, &WebView::onLoadFinished);
        connect(this, &QWebEngineView::renderProcessTerminated, [=](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            const char* status = "";
            switch (termStatus) {
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
            qInfo() << "Render process exited with code" << statusCode
                    << status;

            QTimer::singleShot(0, [this] { reload(); });
        });

// connect(this->webPage(), &WebPage::loadFinished, this,
// &WebView::onLoadFinished);
// connect(this->webPage(), &WebPage::titleChanged, this,
// &WebView::onTitleChanged);
// connect(this->webPage(), &WebPage::urlChanged, this,
// &WebView::onUrlChanged);
////    &WebPage::titleChanged(const QString &title);
////    &WebPage::urlChanged(const QUrl &url);

//		auto old_page = QWebEngineView::page();
//		if (_page != old_page) {
//			QWebEngineView::setPage(_page);
////			if (old_page) old_page->deleteLater();
//		}

#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
        connect(page(), &WebPage::statusBarMessage, &WebPage::setStatusBarText);
#endif
        connect(this->_page, &WebPage::loadingUrl, this, [&](const QUrl& url) {
            auto _binder = this->_page->binder();
            if (_binder->host()) {
                if (url.toString() != "" && url != QUrl() && !url.host().isEmpty() &&
                    !url.scheme().isEmpty() && url != QUrl(web::Browser::_defaulthome)
                    // && url != _loadingurl
                    && url.toString() != _binder->host()->field<url_key>())
                    if (url.isValid() && url != this->_page->_loadingurl)
                        this->urlChanged(url);
            }
        });
        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::iconUrlChanged, this, &WebView::onIconUrlChanged);
        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
        page()->setForwardUnsupportedContent(true);
#endif

        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::loadFinished, this, &WebView::onLoadFinished);
        connect(this, &WebView::close_requested, &WebView::on_close_requested);
        connect(this, &WebView::close_requested, [&]() { _tab_widget->closeTab(_tab_widget->webViewIndex(this)); });
        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::titleChanged, // , this->_page	// &WebPage::onTitleChanged
            [&](const QString& title) { this->_page->onTitleChanged(title); });
        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::urlChanged, // , this->_page	// &WebPage::onUrlChanged
            [&](const QUrl& url) { this->_page->onUrlChanged(url); });
        // &WebPage::titleChanged(const QString &title);
        // &WebPage::urlChanged(const QUrl &url);
        // _page->load(_record->getField("url"));    // auto  loaded

        // _page = new WebPage(profile
        // , requested_item // , openinnewtab
        // , _browser
        // , _tab_widget
        // , table_controller
        // , this);
        setObjectName("web view");
        setFocus();
// _record_controller->addnew_item_fat(requested_item);
#ifdef USE_QT_DELETE_ON_CLOSE
        setAttribute(Qt::WA_DeleteOnClose, true);
#endif // USE_QT_DELETE_ON_CLOSE
        // set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));    //
        // does not work for base class is not QAbstractItemView
        _load_finished_signal.connect([&](bool success) {

            if (success && _toolbarsearch) {
                auto line_edit = _toolbarsearch->lineEdit();
                if (line_edit) {
                    auto url_str = _page->url().toString();
                    if (line_edit->text() != url_str) line_edit->setText(url_str);
                }
            }
        });
    }

    void WebView::page(WebPage* page_)
    {
        //		assert(_page == QWebEngineView::page());
        if (_page && page_ != _page) { // QWebEngineView::page()
            _page->deleteLater();
            _page = nullptr;
        }
        if (!_page || page_ != QWebEngineView::page()) {
            this->_page = page_;
            QWebEngineView::setPage(page_);
            if (_page) {
#if defined(QWEBENGINEPAGE_STATUSBARMESSAGE)
                connect(page(), &WebPage::statusBarMessage, &WebPage::setStatusBarText);
#endif
                connect(this->_page, &WebPage::loadingUrl, this, // &WebView::urlChanged
                    [&](const QUrl& url) {
                        auto _binder = this->_page->binder();
                        if (_binder->host()) {
                            if (url.toString() != "" && url != QUrl() &&
                                !url.host().isEmpty() && !url.scheme().isEmpty() &&
                                url != QUrl(web::Browser::_defaulthome) &&
                                url.toString() != _binder->host()->field<url_key>())
                                if (url.isValid() && url != this->_page->_loadingurl)
                                    this->urlChanged(url);
                        }
                    });
                connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::iconUrlChanged, this, &WebView::onIconUrlChanged);
                connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::featurePermissionRequested, this, &WebView::onFeaturePermissionRequested);
#if defined(QWEBENGINEPAGE_UNSUPPORTEDCONTENT)
                page()->setForwardUnsupportedContent(true);
#endif

                connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::loadFinished, this, &WebView::onLoadFinished);
                connect(this, &WebView::close_requested, &WebView::on_close_requested);
                connect(this, &WebView::close_requested, [&]() { _tab_widget->closeTab(_tab_widget->webViewIndex(this)); });
                connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::titleChanged, // , this->_page	// &WebPage::onTitleChanged
                    [&](const QString& title) { this->_page->onTitleChanged(title); });
                connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::urlChanged, // , this->_page	// &WebPage::onUrlChanged
                    [&](const QUrl& url) { this->_page->onUrlChanged(url); });
            }
        }
    }

    void WebView::activateWindow()
    {
        if (!_browser->isActiveWindow())
            _browser->activateWindow();
        //	        QObject::disconnect(_home_connection);

        //	        _home_connection
        //
        if (_tab_widget->currentWebView() != this) {
            _tab_widget->select_as_current(this);
            //emit static_cast<QTabWidget*>(_tab_widget)->currentChanged(_tab_widget->webViewIndex(this));
        }

        QWebEngineView::activateWindow();
    }

    void WebView::contextMenuEvent(QContextMenuEvent* event)
    {
#if defined(QWEBENGINEPAGE_HITTESTCONTENT)
        QWebEngineHitTestResult r = page()->hitTestContent(event->pos());
        if (!r.linkUrl().isEmpty()) {
            QMenu menu(this);
            menu.addAction(pageAction(QWebEnginePage::OpenLinkInNewWindow));
            menu.addAction(tr("Open in New Tab"), this, &WebView::openLinkInNewTab);
            menu.addSeparator();
            menu.addAction(pageAction(QWebEnginePage::DownloadLinkToDisk));
            // Add link to bookmarks...
            menu.addSeparator();
            menu.addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));
            if (page()->settings()->testAttribute(
                    QWebEngineSettings::DeveloperExtrasEnabled))
                menu.addAction(pageAction(QWebEnginePage::InspectElement));
            menu.exec(mapToGlobal(event->pos()));

            return;
        }
#endif

        QMenu* menu = _page->createStandardContextMenu();
        const QList<QAction*> actions = menu->actions();
        QList<QAction*>::const_iterator it =
            qFind(actions.cbegin(), actions.cend(), _page->action(QWebEnginePage::OpenLinkInThisWindow));
        if (it != actions.cend()) {
            (*it)->setText(tr("Open Link in This Window"));
            ++it;
            menu->insertAction(*it, _page->action(QWebEnginePage::OpenLinkInNewWindow));
            menu->insertAction(*it, _page->action(QWebEnginePage::OpenLinkInNewTab));
        }
        menu->popup(event->globalPos());

        // QWebEngineView::contextMenuEvent(event);
    }

    void WebView::wheelEvent(QWheelEvent* event)
    {
#if defined(QWEBENGINEPAGE_SETTEXTSIZEMULTIPLIER)
        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
            int numDegrees = event->delta() / 8;
            int numSteps = numDegrees / 15;
            setTextSizeMultiplier(textSizeMultiplier() + numSteps * 0.1);
            event->accept();

            return;
        }
#endif
        QWebEngineView::wheelEvent(event);
    }

    void WebView::focusOutEvent(QFocusEvent* event)
    {
        if (event)
            if (this != _tab_widget->currentWebView())
                QWebEngineView::focusOutEvent(event);
    }

    void WebView::openLinkInNewTab()
    {
        pageAction(QWebEnginePage::OpenLinkInNewTab)->trigger();
    }

    void WebView::openLinkInNewTab(const QUrl& _url)
    {
        Q_UNUSED(_url);

#if defined(QWEBENGINEPAGE_WEBACTION_OPENLINKINNEWWINDOW)
        _page->_openinnewtab = true;
        pageAction(QWebEnginePage::OpenLinkInNewWindow)->trigger();
#endif

        ////QString url(QString(""));
        ////QUrl _url =
        ///globalParameters.getBrowserView()->mainWindow()->currentTab()->url();

        ////    if(!_record_ontroller) {
        ////        //RecordTableController *
        ////        _record_ontroller =
        ///globalParameters.getRecordTableScreen()->getRecordTableController();
        ////    }

        ////    RecordTableData *recordTableData =
        ///_record_ontroller->getRecordTableModel()->getRecordTableData();

        ////if(_url.isValid() && _record == nullptr) {
        // _page->record(bind_record(_url));

        ////        QModelIndex proxyindex =
        ///_record_ontroller->convertIdToProxyIndex(_page->record()->getField("id"));
        ////        int position =
        ///_record_ontroller->convertProxyIndexToPos(proxyindex);
        ////        _record_ontroller->getView()->setSelectionToPos(position);
        setFocus();
        // globalparameters.mainwindow()
        if (_rctrl) {
            auto _record_view = _rctrl->view();
#ifdef USE_HAS_SELECTION
            QItemSelectionModel* item_selection_model = _record_view->selectionModel();
            bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
            if (_record_view->selection_first<id_value>() != _page->host()->field<id_key>()
#ifdef USE_HAS_SELECTION
                || !has_selection
#endif // USE_HAS_SELECTION
                )
                _rctrl->select_as_current(_rctrl->index<pos_proxy>(_page->host())); // IdType(_page->item()->field("id"))
        }
        // }
    }

    void WebView::onFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature feature)
    {
        FeaturePermissionBar* permissionBar = new FeaturePermissionBar(this);
        connect(permissionBar, &FeaturePermissionBar::featurePermissionProvided, _page, &QWebEnginePage::setFeaturePermission);

        // Discard the bar on new loads (if we navigate away or reload).
        connect(static_cast<QWebEnginePage* const>(this->_page), &QWebEnginePage::loadStarted, permissionBar, &QObject::deleteLater);

        permissionBar->requestPermission(securityOrigin, feature);
    }

    // void PageView::loadUrl(const QUrl &url)
    // {
    ////        _initialurl = url;
    // _page->load(url);   // load(url);
    // }

    QString WebView::lastStatusBarText() const
    {
        return _statusbartext;
    }

    int WebView::progress() const
    {
        return _progress;
    }

    void WebView::setProgress(int progress)
    {
        _progress = progress;
    }

    web::TabWidget* WebView::tabmanager() const
    {
        return _tab_widget;
    }

    rctrl_t* WebView::record_ctrl()
    {
        return _rctrl;
    }

    //	void WebView::record_ctrl(rctrl_t* rctrl_)
    //	{
    //		this->_rctrl = rctrl_;
    //	}

    // QUrl PageView::url() const
    // {
    // QUrl url = QWebEngineView::url();

    // if(!url.isEmpty())
    // return url;

    // return _initialurl;
    // }

    QIcon WebView::icon() const
    {
        if (!_icon.isNull())
            return _icon;
        return ::sapp_t::instance()->defaultIcon();
    }

    void WebView::onIconUrlChanged(const QUrl& url)
    {
        QNetworkRequest iconRequest(url);
        _icon_reply = ::sapp_t::networkAccessManager()->get(iconRequest);
        _icon_reply->setParent(this);
        connect(_icon_reply, &QNetworkReply::finished, this, &WebView::iconLoaded);
    }

    void WebView::on_close_requested()
    {
        close();

        emit _page->close_requested();

        // int tab_widget_count = tabmanager()->count();
        // int tab_bar_count = tabmanager()->tabbar()->count();
        // int source_model_size = _record_controller->source_model()->size();
        // assert(tab_bar_count == tab_widget_count);

        // if(source_model_size > tab_widget_count) {
        // bool found = false;

        // for(int i = 0; i < source_model_size; i++) {
        // auto item_maybe_to_removed = _record_controller->source_model()->child(i);

        // if(item_maybe_to_removed->unique_page() == page() ||
        // !item_maybe_to_removed->page_valid()) {
        ////
        ///if(_tab_widget->indexOf(item_maybe_to_removed->unique_page()->view()) !=
        ///-1)
        ////
        ///_tab_widget->closeTab(_tab_widget->indexOf(item_maybe_to_removed->unique_page()->view()));
        /////_record_controller->source_model()->remove_child(item_maybe_to_removed->id());

        // page()->break_page_linked_item(item_maybe_to_removed);  // emit
        // _page->close_requested();
        // found = true;
        // break;
        // }
        // }

        // assert(found == true);
        // }

        // assert(tabmanager()->count() == tabmanager()->tabbar()->count());
        // assert(_record_controller->source_model()->size() ==
        // tabmanager()->tabbar()->count());
        // assert(_record_controller->source_model()->size() ==
        // tabmanager()->count());
    }

    void WebView::iconLoaded()
    {
        _icon = //qobject_cast<sapp_t*>(
            ::sapp_t::instance()
                //					      )
                ->defaultIcon(); // QIcon();
        if (_icon_reply) {
            QByteArray data = _icon_reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(data);
            _icon.addPixmap(pixmap);
            _icon_reply->deleteLater();
            _icon_reply = 0;
        }
        emit iconChanged();
    }

    void WebView::mousePressEvent(QMouseEvent* event)
    {
        _page->_pressedbuttons = event->buttons();
        _page->_keyboardmodifiers = event->modifiers();
        QWebEngineView::mousePressEvent(event);
    }

    void WebView::mouseReleaseEvent(QMouseEvent* event)
    {
        QWebEngineView::mouseReleaseEvent(event);
        if (!event->isAccepted() && (_page->_pressedbuttons & Qt::MiddleButton)) {
            QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
            if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty())
                setUrl(url);
        }
    }

    void WebView::setStatusBarText(const QString& string)
    {
        _statusbartext = string;
    }

    WebView* WebView::load(boost::intrusive_ptr<i_t> it, bool switch_to, bool force_reload)
    {
        WebView* v = nullptr;
        if (it)
            v = _page->load(it, switch_to, force_reload);
        return v;
    }

    bool WebView::current_view_global_consistency()
    {
        bool result = false;
        auto _record_screen = _browser->record_screen();
        wn_t* _main_window = gl_paras->main_window();
        // HidableTabWidget	*_vtab_tree	= _mainwindow->vtab_tree();
        HidableTab* _vtab_record = _main_window->vtab_record();
        ts_t* _tree_screen = gl_paras->tree_screen();
        auto _tree_view = _tree_screen->view();
        // auto _tab_widget = v->tabmanager();
        auto index = _tab_widget->webViewIndex(this);
        //			_rctrl->synchronize_record_view(_page->item());
        if (index != _tab_widget->currentIndex())
            _tab_widget->setCurrentIndex(index); // c->index<PosSource>(c->source_model()->index(_binder->item()))

        auto it = _page->binder()->host();
        if (it) {
            if (it != _tree_view->current_item())
                _tree_view->select_as_current(TreeIndex::item_require_treeindex([&] { return _tree_view->source_model(); }, it));
            auto _record_view = _rctrl->view();
#ifdef USE_HAS_SELECTION
            QItemSelectionModel* item_selection_model = _record_view->selectionModel();
            bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
            if ((_record_view->current_item() != it) ||
                (_tab_widget->currentWebView() != this)
#ifdef USE_HAS_SELECTION
                || !has_selection
#endif // USE_HAS_SELECTION
                )
                _rctrl->select_as_current(_rctrl->index<pos_proxy>(it));
            if (_page->title() != it->field<name_key>())
                _page->record_info_update(_page->url(), _page->title());

            if (!_browser->isVisible()) {
                if (_vtab_record->currentWidget() == _record_screen) {
                    _browser->raise();
                    _browser->activateWindow();
                    gl_paras->main_window()->synchronize_title(it->field<name_key>());
                }
            }

            //			if (_record_screen && _vtab_record->currentWidget() != _record_screen)
            //				_vtab_record->setCurrentWidget(_record_screen);
            result = true;
        }
#ifdef USE_EDITOR_WRAP
        _page->_blogger->name(_page->title());
#endif // USE_EDITOR_WRAP

        return result;
    }
    ToolbarSearch* WebView::toolbarsearch() const { return _toolbarsearch; }
    //	void WebView::toolbarsearch(ToolbarSearch* tbs) { _toolbarsearch = tbs; }

    Browser* WebView::browser() const
    {
        return _browser;
    }
    // void WebView::switch_show()
    // {
    // _tab_widget->setCurrentIndex(_tab_widget->webViewIndex(this));
    // _page->load(_page->current_record());
    // show();
    // }

    WebPage::Binder::Binder(boost::intrusive_ptr<i_t> item_,
        WebPage* page_ // , bool make_current = true
        )
        : _host(item_), _page(page_)
    { // , _make_current(make_current)
      // _bounded_page->binder(new
      // TreeItem::coupler_delegation(shared_from_this()));
      // _bounded_item->binder(_bounded_page->record_binder());
    }

    WebView* WebPage::Binder::bind()
    { // boost::intrusive_ptr<TreeItem> item
        assert(_page);

        WebView* view = nullptr;
        if (_host) {
            if (!_page) {
                auto _browser = gl_paras->main_window()->browser(_host);
                auto record_index = RecordIndex::instance(
                    [&] {
                        RecordModel* rm = nullptr;
                        auto rctrl = _browser->tab_widget()->record_screen()->record_ctrl();
                        if (rctrl) {
                            rm = rctrl->source_model();
                        }
                        return rm;
                    },
                    _host);

                _page = _browser->bind(record_index)->page();
                view = _page->view();
            }
            if (_host->binder() != _page->binder()) {
                assert(_host->binder());
                _page->binder_reset();
            }
            _page->bind(_host);
            if (_page->url().toString() != _host->field<url_key>())
                _page->setUrl(QUrl(_host->field<url_key>()));
#ifdef USE_EDITOR_WRAP
            if (_page->_blogger->item() != _host)
#endif // USE_EDITOR_WRAP
                //				_page->metaeditor_sychronize();
                assert(_page->binder()->integrity_external(_host, _page));
        } else {
            if (_page) {
                view = _page->view();
                if (!_page->binder())
                    new ::Binder(std::make_shared<WebPage::Binder>(_host, _page));
                _host = _page->binder()->host();
            }
        }
        return view; // _the->load(record, _make_current);
    }

    WebPage::Binder::~Binder()
    {
        // if(_bounded_item && _bounded_page) { // || (_item && !_item->page_valid())
        // // bug, they may all are nullptr, conflict with upon
        // if(_bounded_page->record_binder() && _bounded_item->record_binder())
        // _bounded_page->item_break(_bounded_item);
        // }

        // _bounded_page->record_binder().reset();
        // _bounded_item->record_binder().reset();
    }

    WebView* WebPage::Binder::activator(bool force_reload)
    { // boost::intrusive_ptr<TreeItem> item
        // assert(_item_link->binder());
        // assert(_item_link->binder() == _page_link->binder());
        // assert(_item_link->page_link() == _page_link);
        // assert(_item_link == _page_link->item_link());
        ////        WebView *view = nullptr;

        // assert(_item_link);
        // assert(_item_link->page_valid());
        assert(_host->binder()->integrity_external(_host, _page));
        // return _bounded_item->unique_page()->activate();

        ////        if(_page->view() != _page->_tab_widget->currentWebView()) {
        // view = _bounded_page->activate();
        ////        } else {
        ////            view = _page->view();
        ////            view->show();
        ////        }

        return _page->activate(force_reload); // view;
    }

    boost::intrusive_ptr<i_t> WebPage::host() const
    {
        return _binder ? _binder->host() : nullptr;
    }

    bool WebPage::activated() const
    {
        return _activated;
    }

    // WebPage *WebPage::page() const {return _binder ? _binder->page() : nullptr;}

    boost::intrusive_ptr<::Binder> WebPage::binder()
    {
        return _binder;
    }

    //	const boost::intrusive_ptr<::Binder>&& WebPage::binder() const {return
    //std::forward<const boost::intrusive_ptr<::Binder> >(_binder);}

    boost::intrusive_ptr<::Binder>
    WebPage::binder(boost::intrusive_ptr<::Binder>&& binder_)
    {
        boost::intrusive_ptr<::Binder> result(nullptr);
        if (binder_) {
            assert(binder_->page() == this); // binder_->page(this);
            if (binder_ != _binder)
                this->_binder = std::forward<boost::intrusive_ptr<::Binder>>(binder_);
            result = this->_binder;
        } else {
            //	                _binder->host(nullptr);
            //	                _binder->page(nullptr);
            _binder = nullptr;
        }
        return result;
    }

    WebView* WebPage::view()
    {
        return _view;
    } // assert(_view);

    rctrl_t* WebPage::record_ctrl()
    {
        return _rctrl;
    }

    web::TabWidget* WebPage::tabmanager()
    {
        return _tab_widget;
    }
}
