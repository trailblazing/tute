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

#include <QApplication>
#include "libraries/GlobalParameters.h"
#include "models/appConfig/AppConfig.h"
#include "models/dataBaseConfig/DataBaseConfig.h"
#include "views/browser/dockedwindow.h"
#include <memory>

class QtLocalPeer;

#if defined(Q_WS_WIN) || defined(Q_OS_WIN32)
#  if !defined(QT_QTSINGLEAPPLICATION_EXPORT) && !defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    define QT_QTSINGLEAPPLICATION_EXPORT
#  elif defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    if defined(QT_QTSINGLEAPPLICATION_EXPORT)
#      undef QT_QTSINGLEAPPLICATION_EXPORT
#    endif
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllimport)
#  elif defined(QT_QTSINGLEAPPLICATION_EXPORT)
#    undef QT_QTSINGLEAPPLICATION_EXPORT
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTSINGLEAPPLICATION_EXPORT
#endif

#include <QtWidgets/QApplication>

#include <QtCore/QUrl>
#include <QtCore/QPointer>

#include <QtGui/QIcon>
//#include "qtsingleapplication.h"

QT_BEGIN_NAMESPACE
class QLocalServer;
class QNetworkAccessManager;
class QWebEngineProfile;
QT_END_NAMESPACE

namespace browser {
    class BookmarksManager;
    class DockedWindow;
    class CookieJar;
    class DownloadManager;
    class HistoryManager;
}

class MainWindow;

class QT_QTSINGLEAPPLICATION_EXPORT QtSingleApplication //: public BrowserApplication { //
    : public QApplication {

    Q_OBJECT

public:
    //    typedef BrowserApplication BaseType;
    QtSingleApplication(int &argc
                        , char **argv
                        , GlobalParameters &globalparameters
                        , AppConfig &appconfig
                        , DataBaseConfig &databaseconfig
                        , bool GUIenabled = true);
    QtSingleApplication(const QString &id
                        , int &argc
                        , char **argv
                        , GlobalParameters &globalparameters
                        , AppConfig &appconfig
                        , DataBaseConfig &databaseconfig
                       );
    ~QtSingleApplication();
    // Comment by Xi
    // QtSingleApplication(int &argc, char **argv, Type type);

    //    static QtSingleApplication *instance();


#if defined(Q_WS_X11)
    QtSingleApplication(Display *dpy, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
    QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0);
    QtSingleApplication(Display *dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif

    bool isRunning();
    QString id() const;

    void setActivationWindow(QWidget *aw, bool activateOnMessage = true);
    QWidget *activationWindow() const;

    // Obsolete:
    void initialize(bool dummy = true)
    {
        isRunning();
        Q_UNUSED(dummy)
    }

public Q_SLOTS:
    bool sendMessage(const QString &message, int timeout = 5000);
    void activateWindow();


Q_SIGNALS:
    void messageReceived(const QString &message);


private:
    void sys_init();
    void main_window();
    void browser_init();
    QtLocalPeer *peer;
    QWidget *actWin;
    bool GUIenabled;
    std::shared_ptr<MainWindow> win;
    //private slots:
    //    void newLocalSocketConnection();

    // copy paste

public:

    //    BrowserApplication(int &argc, char **argv);

    //    ~BrowserApplication();

    static QtSingleApplication *instance();    // move to derived class
    void loadSettings();

    //    bool isTheOnlyBrowser() const;
    //    BrowserWindow *mainWindow();
    //    QList<BrowserWindow *> mainWindows();
    QIcon icon(const QUrl &url) const;
    QIcon defaultIcon() const;

    void saveSession();
    bool canRestoreSession() const;
    bool privateBrowsing() const { return _privateBrowsing; }

    static browser::HistoryManager *historyManager();
    static browser::CookieJar *cookieJar();
    static browser::DownloadManager *downloadManager();
    static QNetworkAccessManager *networkAccessManager();
    static browser::BookmarksManager *bookmarksManager();

    void installTranslator(const QString &name);

#if defined(Q_OS_OSX)
    bool event(QEvent *event);
#endif

public slots:
    //    BrowserWindow *newMainWindow();
    void restoreLastSession();
#if defined(Q_OS_OSX)
    void lastWindowClosed();
    void quitBrowser();
#endif
    void setPrivateBrowsing(bool);

signals:
    void privateBrowsingChanged(bool);

private slots:
    void postLaunch();
    //    void openUrl(const QUrl &url);
    void newLocalSocketConnection();

private:
    //    void clean();
    //    void installTranslator(const QString &name);

    static browser::HistoryManager *_historymanager;
    static browser::DownloadManager *_downloadmanager;
    static QNetworkAccessManager *_networkaccessmanager;
    static browser::BookmarksManager *_bookmarksmanager;

    //    QList<QPointer<BrowserWindow> > _mainWindows;
    QLocalServer *_localserver;
    QByteArray _lastSession;
    QWebEngineProfile *_privateProfile;
    bool _privateBrowsing;
    mutable QIcon _defaultIcon;

    GlobalParameters &_globalparameters;
    AppConfig &_appconfig;
    DataBaseConfig &_databaseconfig;
};

#endif // QTSINGLEAPPLICATION_H
