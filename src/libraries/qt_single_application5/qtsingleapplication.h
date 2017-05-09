/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#ifndef QTSINGLEAPPLICATION_H
#define QTSINGLEAPPLICATION_H

//#include "utility/expose.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "models/database_config/database_config.h"
//#include "views/browser/browser.h"
#include "libraries/qtm/blogger.h"

#include <QApplication>
#include <memory>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QtLocalPeer;

#if defined(Q_WS_WIN) || defined(Q_OS_WIN32)
#if !defined(QT_QTSINGLEAPPLICATION_EXPORT) && \
    !defined(QT_QTSINGLEAPPLICATION_IMPORT)
#define QT_QTSINGLEAPPLICATION_EXPORT
#elif defined(QT_QTSINGLEAPPLICATION_IMPORT)
#if defined(QT_QTSINGLEAPPLICATION_EXPORT)
#undef QT_QTSINGLEAPPLICATION_EXPORT
#endif
#define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllimport)
#elif defined(QT_QTSINGLEAPPLICATION_EXPORT)
#undef QT_QTSINGLEAPPLICATION_EXPORT
#define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllexport)
#endif
#else
#define QT_QTSINGLEAPPLICATION_EXPORT
#endif

#include <QtWidgets/QApplication>

#include <QtCore/QPointer>

#include <QtCore/QUrl>

#include <QtGui/QIcon>
// #include "qtsingleapplication.h"

#include <QtNetwork/QAuthenticator>

#ifdef USE_QTM

// #include <QApplication>
#include <QList>
#include <QStringList>

class SuperMenu;
class QMenuBar;
class Blogger;

#endif

QT_BEGIN_NAMESPACE
class QLocalServer;
class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;
class QWebEngineProfile;
QT_END_NAMESPACE

// extern const char *application_name;

namespace web {
	class BookmarksManager;
	class Browser;
	class CookieJar;
	class DownloadManager;
	class HistoryManager;
	class UrlRequestInterceptor;
	class Profile;
}

class wn_t;
class Blogger;
class sapp_t;
namespace app {
	//	typedef struct _RF {
	//		QString title;
	//		QString filename;
	//	} RecentFile;
	struct RecentFile {
		QString title;
		QString filename;
	};
}

class QT_QTSINGLEAPPLICATION_EXPORT sapp_t // : public BrowserApplication { //
    : public QApplication {
#if QT_VERSION == 0x050600
	W_OBJECT(QT_QTSINGLEAPPLICATION_EXPORT sapp_t)
#else
	Q_OBJECT
#endif

    public:
	// typedef BrowserApplication BaseType;
	sapp_t(int& argc, char** argv, bool gui_enabled = true);
	sapp_t(const QString& id, int& argc, char** argv);
	~sapp_t();
// Comment by Xi
// sapp_t(int &argc, char **argv, Type type);

// static sapp_t *instance();

#if defined(Q_WS_X11)
	sapp_t(Display* dpy, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
	sapp_t(Display* dpy, int& argc, char** argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0);
	sapp_t(Display* dpy, const QString& appId, int argc, char** argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif

	bool isRunning();
	QString id() const;

	void setActivationWindow(QWidget* aw, bool activateOnMessage = true);
	QWidget* activationWindow() const;

	// Obsolete:
	void initialize(bool dummy = true);

    public Q_SLOTS:
	bool sendMessage(const QString& message, int timeout = 5000);
	void activateWindow();

    Q_SIGNALS:

#if QT_VERSION == 0x050600
	void messageReceived(const QString& message) W_SIGNAL(messageReceived, (const QString&), message);
#else
	void messageReceived(const QString& message);
#endif

    private:
	void sys_init(char** argv);
	void main_window();
	void browsers_shared_info_init();
#ifdef USE_QTM
	void blog_init();
#endif
	QtLocalPeer* _peer;
	QWidget* _act_window;
	bool _gui_enabled;
	wn_t* _main_window;
	// private slots:
	// void newLocalSocketConnection();

	// copy paste

    public:
	// BrowserApplication(int &argc, char **argv);

	// ~BrowserApplication();

	static sapp_t* instance(); // move to derived class
	void loadSettings();

	// bool isTheOnlyBrowser() const;
	// BrowserWindow *mainWindow();
	// QList<BrowserWindow *> mainWindows();
	QIcon icon(const QUrl& url) const;
	QIcon defaultIcon() const;

	void saveSession();
	bool canRestoreSession() const;
	bool privateBrowsing() const;

	void setLastAuthenticator(QAuthenticator*);
	void setLastProxyAuthenticator(QAuthenticator*);

	// TODO: Remove these functions (QTBUG-47967)
	static QByteArray authenticationKey(const QUrl&, const QString&);
	static QByteArray proxyAuthenticationKey(const QNetworkProxy&, const QString&);
	static QByteArray proxyAuthenticationKey(const QString&, const QString&, const QString&);

	static web::HistoryManager* historyManager();
	static web::CookieJar* cookieJar();
	//	static web::DownloadManager* request_download_manager();
	static QNetworkAccessManager* networkAccessManager();
	static web::BookmarksManager* bookmarksManager();

	void installTranslator(const QString& name);

#if defined(Q_OS_OSX)
	bool event(QEvent* event);
#endif

    public slots:
	// BrowserWindow *newMainWindow();
	int restoreLastSession();
#if defined(Q_OS_OSX)
	void lastWindowClosed();
	void quitBrowser();
#endif
	void setPrivateBrowsing(bool);

	void authenticationRequired(QNetworkReply*, QAuthenticator*);
	void proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

    signals:

#if QT_VERSION == 0x050600
	void privateBrowsingChanged(bool b) W_SIGNAL(privateBrowsingChanged, (bool), b);
#else
	void privateBrowsingChanged(bool b);
#endif

    private slots:
	void postLaunch();
	// void openUrl(const QUrl &url);
	void newLocalSocketConnection();

    private:
	// void clean();
	// void installTranslator(const QString &name);

	static web::HistoryManager* _historymanager;
	static web::DownloadManager* _downloadmanager;
	static QNetworkAccessManager* _networkaccessmanager;
	static web::BookmarksManager* _bookmarksmanager;

	// QList<QPointer<BrowserWindow> > _mainWindows;
	web::Profile* _profile;
	QLocalServer* _localserver;
	QByteArray _last_session;
	// QWebEngineProfile
	web::Profile* _private_profile;
	bool _private_browsing;
	mutable QIcon _default_icon;

	// std::shared_ptr<gl_para> _gl_paras;
	// std::shared_ptr<AppConfig> _appconfig;
	// std::shared_ptr<DataBaseConfig> _databaseconfig;

	QAuthenticator _last_authenticator;
	QAuthenticator _last_proxy_authenticator;
	QString _style;

#ifdef USE_QTM

	// class Application : public QApplication {
	// Q_OBJECT

    public:
	// Application(int &, char * *);
	void setupRecentFiles();
	QStringList recentFileTitles();
	QStringList recentFilenames();

	QList<std::shared_ptr<app::RecentFile>> recentFiles();
	std::shared_ptr<app::RecentFile> getRecentFile(int);
	QStringList titles();
	QStringList filenames();
	Blogger* blogger();
	void deleteSandbox();
	bool isSandbox();

    public slots:
	void set_recent_files(const QStringList&, const QStringList&);
	void add_recent_file(const QString&, const QString&);
//	void saveAll();
	void blogger(Blogger* blogger_);

    signals:
	void recent_files_updated(QStringList, QStringList);
	void recent_files_updated(const QList<std::shared_ptr<app::RecentFile>>&);
	void blogger_changed(Blogger*);

    private:
	QList<std::shared_ptr<app::RecentFile>> _recent_files;
#ifdef Q_OS_MAC
	SuperMenu* superMenu;
#endif
	blogger_ref _blogger;
	bool _isSandbox = false;

    private slots:
	void saveRecentFiles();
	void handleWindowChange(QWidget*, QWidget*);
	void handleLastWindowClosed();
// };
#endif
};

// typedef app::RecentFile RecentFile;

// EXPOSE_INNER(ExposedSappRecentFile, app::RecentFile)

#endif // QTSINGLEAPPLICATION_H
