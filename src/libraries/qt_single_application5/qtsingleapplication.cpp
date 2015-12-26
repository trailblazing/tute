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


#include "qtsingleapplication.h"
#include "qtlocalpeer.h"
#include <QWidget>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include "views/browser/browser.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "libraries/GlobalParameters.h"
#include "views/record_table/TableScreen.h"
#include "models/database_config/DataBaseConfig.h"
#include "models/record_table/TableModel.h"
#include "models/record_table/TableData.h"
#include "controllers/record_table/TableController.h"
#include "main.h"
#include <utility>

//begin from main.cpp

#include <sys/timeb.h>

#include <QTranslator>
#include <QToolButton>
#include <QSplashScreen>

#if QT_VERSION >= 0x050000
#include <QScroller>
#include <QScrollerProperties>
#include <QScrollBar>
#endif

#include <qtwebenginewidgetsglobal.h>

#include "main.h"



#include <qversiontagging.h>

#ifndef QT_VERSION
#define QT_VERSION 0x050600
#endif

#if QT_VERSION < 0x050000
#include "libraries/qtSingleApplication/qtsingleapplication.h"
#else
#include "libraries/qt_single_application5/qtsingleapplication.h"
#endif

#include "views/main_window/MainWindow.h"
#include "models/app_config/AppConfig.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/TrashMonitoring.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/crypt/RC5Simple.h"
#include "libraries/crypt/Password.h"
#include "libraries/GlobalParameters.h"
#include "views/record_table/TableScreen.h"
#include "models/database_config/DataBaseConfig.h"
#include "models/record_table/TableModel.h"
#include "models/record_table/TableData.h"
#include "controllers/record_table/TableController.h"


//using namespace std;

//// Фиксированные параметры программы (жестко заданные в текущей версии MyTetra)
//FixedParameters fixedParameters;

//// Глобальные параметры программы (вычислимые на этапе инициализации, иногда меняющиеся в процессе выполнения программы)
//GlobalParameters globalParameters;

//// Конфигурация программы (считанная из файла конфигурации)
//AppConfig mytetraConfig;

//// Конфигурация данных
//DataBaseConfig dataBaseConfig;

//// Объект слежения за состоянием корзины
//TrashMonitoring trashMonitoring;

//// Объект с историей посещаемых записей
//WalkHistory walkHistory;

//// Указатель на основное окно программы
//QObject *pMainWindow;

//end from main.cpp


/*!
    \class QtSingleApplication qtsingleapplication.h
    \brief The QtSingleApplication class provides an API to detect and
    communicate with running instances of an application.

    This class allows you to create applications where only one
    instance should be running at a time. I.e., if the user tries to
    launch another instance, the already running instance will be
    activated instead. Another usecase is a client-server system,
    where the first started instance will assume the role of server,
    and the later instances will act as clients of that server.

    By default, the full path of the executable file is used to
    determine whether two processes are instances of the same
    application. You can also provide an explicit identifier string
    that will be compared instead.

    The application should create the QtSingleApplication object early
    in the startup phase, and call isRunning() to find out if another
    instance of this application is already running. If isRunning()
    returns false, it means that no other instance is running, and
    this instance has assumed the role as the running instance. In
    this case, the application should continue with the initialization
    of the application user interface before entering the event loop
    with exec(), as normal.

    The messageReceived() signal will be emitted when the running
    application receives messages from another instance of the same
    application. When a message is received it might be helpful to the
    user to raise the application so that it becomes visible. To
    facilitate this, QtSingleApplication provides the
    setActivationWindow() function and the activateWindow() slot.

    If isRunning() returns true, another instance is already
    running. It may be alerted to the fact that another instance has
    started by using the sendMessage() function. Also data such as
    startup parameters (e.g. the name of the file the user wanted this
    new instance to open) can be passed to the running instance with
    this function. Then, the application should terminate (or enter
    client mode).

    If isRunning() returns true, but sendMessage() fails, that is an
    indication that the running instance is frozen.

    Here's an example that shows how to convert an existing
    application to use QtSingleApplication. It is very simple and does
    not make use of all QtSingleApplication's functionality (see the
    examples for that).

    \code
    // Original
    int main(int argc, char **argv)
    {
        QApplication app(argc, argv);

        MyMainWidget mmw;
        mmw.show();
        return app.exec();
    }

    // Single instance
    int main(int argc, char **argv)
    {
        QtSingleApplication app(argc, argv);

        if (app.isRunning())
            return !app.sendMessage(someDataString);

        MyMainWidget mmw;
        app.setActivationWindow(&mmw);
        mmw.show();
        return app.exec();
    }
    \endcode

    Once this QtSingleApplication instance is destroyed (normally when
    the process exits or crashes), when the user next attempts to run the
    application this instance will not, of course, be encountered. The
    next instance to call isRunning() or sendMessage() will assume the
    role as the new running instance.

    For console (non-GUI) applications, QtSingleCoreApplication may be
    used instead of this class, to avoid the dependency on the QtGui
    library.

    \sa QtSingleCoreApplication
*/



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

#include "bookmarks.h"
#include "views/browser/browser.h"
#include "views/browser/entrance.h"
#include "libraries/GlobalParameters.h"

#include "cookiejar.h"
#include "downloadmanager.h"
#include "history.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "webview.h"

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileOpenEvent>
#include <QtWidgets/QMessageBox>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslSocket>

#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include <QtCore/QDebug>

browser::DownloadManager    *QtSingleApplication::_downloadmanager = nullptr;
browser::HistoryManager     *QtSingleApplication::_historymanager = nullptr;
QNetworkAccessManager       *QtSingleApplication::_networkaccessmanager = nullptr;
browser::BookmarksManager   *QtSingleApplication::_bookmarksmanager = nullptr;

static void setUserStyleSheet(QWebEngineProfile *profile
                              , const QString &styleSheet
                              , browser::Entrance *browsemanager  //, browser::BrowserWindow *mainWindow = 0
                             )
{
    Q_ASSERT(profile);
    QString scriptName(QStringLiteral("userStyleSheet"));
    QWebEngineScript script;
    QList<QWebEngineScript> styleSheets = profile->scripts()->findScripts(scriptName);

    if(!styleSheets.isEmpty())
        script = styleSheets.first();

    Q_FOREACH(const QWebEngineScript &s, styleSheets)
        profile->scripts()->remove(s);

    if(script.isNull()) {
        script.setName(scriptName);
        script.setInjectionPoint(QWebEngineScript::DocumentReady);
        script.setRunsOnSubFrames(true);
        script.setWorldId(QWebEngineScript::ApplicationWorld);
    }

    QString source = QString::fromLatin1("(function() {"\
                                         "var css = document.getElementById(\"_qt_testBrowser_userStyleSheet\");"\
                                         "if (css == undefined) {"\
                                         "    css = document.createElement(\"style\");"\
                                         "    css.type = \"text/css\";"\
                                         "    css.id = \"_qt_testBrowser_userStyleSheet\";"\
                                         "    document.head.appendChild(css);"\
                                         "}"\
                                         "css.innerText = \"%1\";"\
                                         "})()").arg(styleSheet);
    script.setSourceCode(source);
    profile->scripts()->insert(script);

    // run the script on the already loaded views
    // this has to be deferred as it could mess with the storage initialization on startup
    if(browsemanager) {
        browsemanager->settingstylesource(source);
        //        QMetaObject::invokeMethod(browsemanager, "runScriptOnOpenViews", Qt::QueuedConnection, Q_ARG(QString, source));
    }
}

void QtSingleApplication::sys_init()
{

    // Инициализация глобальных параметров,
    // внутри происходит установка рабочей директории
    // Initialize global parameters,
    // Is being installed inside the working directory
    _globalparameters.init();

    // Initialize the main program of configurable variables    // Инициализация основных конфигурирующих программу переменных
    _appconfig.init();

    // Инициализация переменных, отвечающих за хранилище данных
    _databaseconfig.init();

    QString version = QString::number(APPLICATION_RELEASE_VERSION) + "." + QString::number(APPLICATION_RELEASE_SUBVERSION) + "." + QString::number(APPLICATION_RELEASE_MICROVERSION);
    QCoreApplication::setOrganizationName(QLatin1String("mytetra"));
    QCoreApplication::setApplicationName(QLatin1String("MyTetra"));
    QCoreApplication::setApplicationVersion(version);   // QLatin1String("0.1")

    QString serverName = QCoreApplication::applicationName() + QString::fromLatin1(QT_VERSION_STR).remove('.') + QLatin1String("webengine");


    QLocalSocket socket;
    socket.connectToServer(serverName);

    if(socket.waitForConnected(500)) {
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();

        if(args.count() > 1)
            stream << args.last();
        else
            stream << QString();

        stream.flush();
        socket.waitForBytesWritten();
        return;
    }

    //    _localserver = new QLocalServer(this);
    connect(_localserver, &QLocalServer::newConnection, this, &QtSingleApplication::newLocalSocketConnection);

    if(!_localserver->listen(serverName)) {
        if(_localserver->serverError() == QAbstractSocket::AddressInUseError
           && QFile::exists(_localserver->serverName())) {
            QFile::remove(_localserver->serverName());
            _localserver->listen(serverName);
        }
    }


#if defined(Q_OS_OSX)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

#ifndef QT_NO_OPENSSL

    if(!QSslSocket::supportsSsl()) {
        QMessageBox::information(0, "Mytetra",
                                 "This system does not support OpenSSL. SSL websites will not be available.");
    }

#endif

    //    actWin = 0;
    //    peer = new QtLocalPeer(this, appId);
    connect(_peer, &QtLocalPeer::messageReceived, &QtLocalPeer::messageReceived);
}

void QtSingleApplication::browser_init()
{

    //QtSingleApplication::QtSingleApplication(int &argc, char **argv)
    //        : QApplication(argc, argv)
    //          //, _localServer(0)
    //        , _privateProfile(0)
    //        , _privateBrowsing(false)
    //{



    QDesktopServices::setUrlHandler(QLatin1String("http"), this, "openUrl");
    QString localSysName = QLocale::system().name();

    installTranslator(QLatin1String("qt_") + localSysName);


    // Подключение перевода интерфейса
    // QString langFileName=globalParameters.getWorkDirectory()+"/resource/translations/mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
    QString langFileName = ":/resource/translations/mytetra_" + _appconfig.get_interfacelanguage() + ".qm";
    qDebug() << "Use language file " << langFileName;

    //QTranslator langTranslator;
    //langTranslator.load(langFileName);
    installTranslator(langFileName);    //&langTranslator


    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));
    _last_session = settings.value(QLatin1String("lastSession")).toByteArray();
    settings.endGroup();

#if defined(Q_OS_OSX)
    connect(this, &QtSingleApplication::lastWindowClosed, this, &QtSingleApplication::lastWindowClosed);
#endif

    QTimer::singleShot(0, this, &QtSingleApplication::postLaunch);


    //}
}

void QtSingleApplication::main_window()
{

    // Do not run another copy    // Не запущен ли другой экземпляр
    if(isRunning()) {
        QString message = "Another MyTetra exemplar is running.\n";

        printf(message.toLocal8Bit());

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(message);
        msgBox.exec();

        exit(0);
    }

    // Установка CSS-оформления
    setCssStyle();

    // Экран загрузки, показывается только в Андроид версии (так как загрузка идет ~10 сек, и без сплешскрина непонятно что происходит)
    QSplashScreen splash(QPixmap(":/resource/pic/mytetra_splash.png"));

    if(_appconfig.getShowSplashScreen())
        splash.show();


    //    // Подключение перевода интерфейса
    //    // QString langFileName=globalParameters.getWorkDirectory()+"/resource/translations/mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
    //    QString langFileName = ":/resource/translations/mytetra_" + mytetraConfig.get_interfacelanguage() + ".qm";
    //    qDebug() << "Use language file " << langFileName;

    //    //QTranslator langTranslator;
    //    //langTranslator.load(langFileName);
    //    application.installTranslator(
    //        langFileName    //&langTranslator
    //    );


    // Создание объекта главного окна
    _window = std::make_shared<MainWindow>(_globalparameters, _appconfig, _databaseconfig);

    _window->setWindowTitle("MyTetra");

    if(_globalparameters.target_os() == "android")
        _window->show(); // В Андроиде нет десктопа, на нем нельзя сворачивать окно
    else {
        if(_appconfig.get_runinminimizedwindow() == false)
            _window->show();
        else
            _window->hide();
    }

    // win->setObjectName("mainwindow");
    // pMainWindow=&win; // Запоминается указатель на основное окно

    // После создания окна восстанавливается вид окна в предыдущий запуск
    // Эти действия нельзя делать в конструкторе главного окна,
    // т.к. окно еще не создано
    _globalparameters.window_switcher()->disableSwitch();
    _window->restoreFindOnBaseVisible();
    _window->restoreGeometry();
    _window->restore_tree_position();
    _window->restore_recordtable_position();
    _window->restoreEditorCursorPosition();
    _window->restoreEditorScrollBarPosition();
    _globalparameters.window_switcher()->enableSwitch();

    if(_appconfig.getInterfaceMode() == "mobile")
        _globalparameters.window_switcher()->restoreFocusWidget();

    qDebug() << "Restore session succesfull";

    // После восстановления последней редактируемой записи
    // история перехода очищается, так как в не может попасть
    // первая запись в востаналиваемой ветке и сама восстанавливаемая запись
    walkhistory.clear();


    // Если в конфиге настроено, что нужно синхронизироваться при старте
    // И задана команда синхронизации
    if(_appconfig.get_synchroonstartup())
        if(_appconfig.get_synchrocommand().trimmed().length() > 0)
            _window->synchronization();


    // Если настроено в конфиге, сразу запрашивается пароль доступа
    // к зашифрованным данным
    // И если есть хоть какие-то зашифрованные данные
    if(_appconfig.get_howpassrequest() == "atStartProgram")
        if(_globalparameters.crypt_key().length() == 0)
            if(_databaseconfig.get_crypt_mode() > 0) {
                // Запрашивается пароль только в том случае, если ветка,
                // на которую установливается курсор при старте, незашифрована
                // Если ветка зашифрована, пароль и так будет запрошен автоматически
                if(_window->isTreePositionCrypt() == false) {
                    Password password;
                    password.retrievePassword();
                }
            }


    // Если в общем конфиге стоит опция хранения пароля
    // И хранимый пароль (точнее его хеш) заполнен
    if(_globalparameters.crypt_key().length() == 0) {
        if(_databaseconfig.get_crypt_mode() > 0) {
            if(_appconfig.getPasswordSaveFlag()) {
                if(_appconfig.getPasswordMiddleHash().length() > 0) {
                    // При запросе пароля ключ шифрования будет восстановлен автоматически
                    Password password;
                    password.retrievePassword();
                }
            }
        }
    }

    // Распечатывается дерево сгенерированных объектов
    // print_object_tree();

    // Проверяется наличие системного трея
    /*
    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
     QMessageBox::critical(0, QObject::tr("Systray"),
                           QObject::tr("I couldn't detect any system tray on this system."));
     exit(1);
    }
    */

    // При закрытии окна не выходить из программы.
    // Окно программы может быть снова открыто из трея
    QApplication::setQuitOnLastWindowClosed(false);


    // win->show();
    connect(this, &QtSingleApplication::lastWindowClosed, this, &QtSingleApplication::quit);

    // app.connect(&app, SIGNAL(app.commitDataRequest(QSessionManager)), SLOT(win->commitData(QSessionManager)));

    // Окно сплеш-скрина скрывается
    if(_appconfig.getShowSplashScreen())
        splash.finish(_window.get());
}

/*!
    Creates a QtSingleApplication object. The application identifier
    will be QCoreApplication::applicationFilePath(). \a argc, \a
    argv, and \a GUIenabled are passed on to the QAppliation constructor.

    If you are creating a console application (i.e. setting \a
    GUIenabled to false), you may consider using
    QtSingleCoreApplication instead.
*/

QtSingleApplication::QtSingleApplication(
    int &argc
    , char **argv
    , GlobalParameters &globalparameters
    , AppConfig &appconfig
    , DataBaseConfig &databaseconfig
    , bool GUIenabled
)
    : QApplication(argc, argv)
    , _peer(new QtLocalPeer(this, QString()))
    , act_window(nullptr)
    , _gui_enabled(GUIenabled)
    , _localserver(new QLocalServer(this))
    , _private_profile(nullptr)
    , _private_browsing(false)
    , _globalparameters(globalparameters)
    , _appconfig(appconfig)
    , _databaseconfig(databaseconfig)

{
    sys_init();

    //    _app = new ThreadedApplication(argc, argv, GUIenabled);
    //    boost::thread appThread(boost::ref(*_app));
    //    _app->waitForApplicationRun();
    main_window();

    browser_init();

}


/*!
    Creates a QtSingleApplication object with the application
    identifier \a appId. \a argc and \a argv are passed on to the
    QAppliation constructor.
*/

QtSingleApplication::QtSingleApplication(
    const QString &appId
    , int &argc
    , char **argv
    , GlobalParameters &globalparameters
    , AppConfig &appconfig
    , DataBaseConfig &databaseconfig
)
    : QApplication(argc, argv)
    , _peer(new QtLocalPeer(this, appId))
    , act_window(0)
    , _gui_enabled(true)
    , _localserver(new QLocalServer(this))
    , _private_profile(0)
    , _private_browsing(false)
    , _globalparameters(globalparameters)
    , _appconfig(appconfig)
    , _databaseconfig(databaseconfig)
{
    sys_init();
    main_window();
    browser_init();
}


/*!
    Creates a QtSingleApplication object. The application identifier
    will be QCoreApplication::applicationFilePath(). \a argc, \a
    argv, and \a type are passed on to the QAppliation constructor.
*/

// Comment by Xi
/*
QtSingleApplication::QtSingleApplication(int &argc, char **argv, Type type)
    : QApplication(argc, argv, type)
{
    sysInit();
}
*/


#if defined(Q_WS_X11)
/*!
  Special constructor for X11, ref. the documentation of
  QApplication's corresponding constructor. The application identifier
  will be QCoreApplication::applicationFilePath(). \a dpy, \a visual,
  and \a cmap are passed on to the QApplication constructor.
*/
QtSingleApplication::QtSingleApplication(Display *dpy, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, visual, cmap)
{
    sysInit();
}

/*!
  Special constructor for X11, ref. the documentation of
  QApplication's corresponding constructor. The application identifier
  will be QCoreApplication::applicationFilePath(). \a dpy, \a argc, \a
  argv, \a visual, and \a cmap are passed on to the QApplication
  constructor.
*/
QtSingleApplication::QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit();
}

/*!
  Special constructor for X11, ref. the documentation of
  QApplication's corresponding constructor. The application identifier
  will be \a appId. \a dpy, \a argc, \a
  argv, \a visual, and \a cmap are passed on to the QApplication
  constructor.
*/
QtSingleApplication::QtSingleApplication(Display *dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit(appId);
}
#endif


/*!
    Returns true if another instance of this application is running;
    otherwise false.

    This function does not find instances of this application that are
    being run by a different user (on Windows: that are running in
    another session).

    \sa sendMessage()
*/

bool QtSingleApplication::isRunning()
{
    return _peer->isClient();
}


/*!
    Tries to send the text \a message to the currently running
    instance. The QtSingleApplication object in the running instance
    will emit the messageReceived() signal when it receives the
    message.

    This function returns true if the message has been sent to, and
    processed by, the current instance. If there is no instance
    currently running, or if the running instance fails to process the
    message within \a timeout milliseconds, this function return false.

    \sa isRunning(), messageReceived()
*/
bool QtSingleApplication::sendMessage(const QString &message, int timeout)
{
    return _peer->sendMessage(message, timeout);
}


/*!
    Returns the application identifier. Two processes with the same
    identifier will be regarded as instances of the same application.
*/
QString QtSingleApplication::id() const
{
    return _peer->applicationId();
}


/*!
  Sets the activation window of this application to \a aw. The
  activation window is the widget that will be activated by
  activateWindow(). This is typically the application's main window.

  If \a activateOnMessage is true (the default), the window will be
  activated automatically every time a message is received, just prior
  to the messageReceived() signal being emitted.

  \sa activateWindow(), messageReceived()
*/

void QtSingleApplication::setActivationWindow(QWidget *aw, bool activateOnMessage)
{
    act_window = aw;

    if(activateOnMessage)
        connect(_peer, &QtLocalPeer::messageReceived, this, &QtSingleApplication::activateWindow);
    else
        disconnect(_peer, &QtLocalPeer::messageReceived, this, &QtSingleApplication::activateWindow);
}


/*!
    Returns the applications activation window if one has been set by
    calling setActivationWindow(), otherwise returns 0.

    \sa setActivationWindow()
*/
QWidget *QtSingleApplication::activationWindow() const
{
    return act_window;
}


/*!
  De-minimizes, raises, and activates this application's activation window.
  This function does nothing if no activation window has been set.

  This is a convenience function to show the user that this
  application instance has been activated when he has tried to start
  another instance.

  This function should typically be called in response to the
  messageReceived() signal. By default, that will happen
  automatically, if an activation window has been set.

  \sa setActivationWindow(), messageReceived(), initialize()
*/
void QtSingleApplication::activateWindow()
{
    if(act_window) {
        act_window->setWindowState(act_window->windowState() & ~Qt::WindowMinimized);
        act_window->raise();
        act_window->activateWindow();
    }
}


/*!
    \fn void QtSingleApplication::messageReceived(const QString& message)

    This signal is emitted when the current instance receives a \a
    message from another instance of this application.

    \sa sendMessage(), setActivationWindow(), activateWindow()
*/


/*!
    \fn void QtSingleApplication::initialize(bool dummy = true)

    \obsolete
*/


QtSingleApplication *QtSingleApplication::instance()
{
    return (static_cast<QtSingleApplication *>(QCoreApplication::instance()));
}



void QtSingleApplication::newLocalSocketConnection()
{
    QLocalSocket *socket = _peer->server()->nextPendingConnection();

    if(!socket)
        return;

    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    QString url;
    stream >> url;

    if(url.isEmpty()) {url = browser::Browser::_defaulthome;}

    //    browser::DockedWindow *w = nullptr;

    //if(!url.isEmpty()) {

    QSettings settings;
    settings.beginGroup(QLatin1String("general"));
    int openLinksIn = settings.value(QLatin1String("openLinksIn"), 0).toInt();
    settings.endGroup();

    auto browser_entrance = globalparameters.entrance();
    //    Record *record = request_record(url);

    std::pair<browser::Browser *, browser::WebView *> dp;

    if(browser_entrance) {
        if(openLinksIn == 1) {
            //browser::BrowserWindow *bw =
            //            dp = browser_entrance->new_dockedwindow(record);




            //            request_record(
            //                url
            //                , sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>(
            //                    ""
            //                    , &browser::Entrance::new_dockedwindow
            //            , boost::shared_ptr<browser::Entrance>(browser_entrance, [ = ](browser::Entrance *) {})
            //                )
            //            //            , [browser_entrance](Record * const record)-> browser::WebView * {   // &Entrance::new_dockedwindow
            //            //                return browser_entrance->new_dockedwindow(record);
            //            //            }
            //            );



            browser_entrance->new_browser(url);

            //bw->tabWidget()->newTabFull(record, globalParameters.getRecordTableScreen()->getRecordTableController());
        } else {
            //Record *record = register_record(url);
            //            dp = browser_entrance->active_record(record);
            auto arb = boost::make_shared<browser::Entrance::ActiveRecordBinder>(browser_entrance);
            auto record = globalparameters.table_screen()->table_controller()->request_record(
                              url
                              , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>(
                                  ""
                                  , &browser::Entrance::ActiveRecordBinder::binder
                                  , arb
                              )
                              , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>(
                                  ""
                                  , &browser::Entrance::ActiveRecordBinder::activator
                                  , arb
                              )
                              //            , [browser_entrance](Record * const record)->browser::WebView * {   // &Entrance::new_dockedwindow
                              //                return browser_entrance->active_record(record);
                              //            }
                          );
            //            record->generate();
            record->active();
            //            dp.second = browser_entrance->active_record()->invoke_page(record); //->tabWidget()->newTabFull(record, globalParameters.getRecordTableScreen()->getRecordTableController());
        }

        //browser_view->mainWindow()->load_record(record);
    }

    //    record->active_immediately(true);

    //}

    delete socket;
    //    dp.first->raise();
    //    dp.first->activateWindow();
}

QtSingleApplication::~QtSingleApplication()
{
    delete _downloadmanager;

    //    for(int i = 0; i < _mainWindows.size(); ++i) {
    //        BrowserWindow *window = _mainWindows.at(i);
    //        delete window;
    //    }

    delete _networkaccessmanager;
    delete _bookmarksmanager;
}

//#if defined(Q_OS_OSX)
//void QtSingleApplication::lastWindowClosed()
//{
//    clean();
//    BrowserWindow *mw = globalParameters.getBrowserView()->getBrowserWindow();  //new BrowserWindow;
//    mw->slotHome();
//    _mainwindows.prepend(mw);
//}
//#endif

//QtSingleApplication *QtSingleApplication::instance()
//{
//    return (static_cast<QtSingleApplication *>(QCoreApplication::instance()));
//}

#if defined(Q_OS_OSX)
#include <QtWidgets/QMessageBox>
void QtSingleApplication::quitBrowser()
{
    clean();
    int tabCount = 0;

    for(int i = 0; i < _mainwindows.count(); ++i) {
        tabCount += _mainwindows.at(i)->tabWidget()->count();
    }

    if(tabCount > 1) {
        int ret = QMessageBox::warning(mainWindow(), QString(),
                                       tr("There are %1 windows and %2 tabs open\n"
                                          "Do you want to quit anyway?").arg(_mainwindows.count()).arg(tabCount),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);

        if(ret == QMessageBox::No)
            return;
    }

    exit(0);
}
#endif

/*!
    Any actions that can be delayed until the window is visible
 */
void QtSingleApplication::postLaunch()
{
    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    if(directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();

#if defined(QWEBENGINESETTINGS_PATHS)
    QWebEngineSettings::setIconDatabasePath(directory);
    QWebEngineSettings::setOfflineStoragePath(directory);
#endif

    setWindowIcon(QIcon(QLatin1String(":browser.svg")));

    loadSettings();

    // newMainWindow() needs to be called in main() for this to happen
    if(_globalparameters.entrance()->window_list().size() > 0) { // _mainWindows.count()
        QStringList args = QCoreApplication::arguments();
        browser::Browser *browser = _globalparameters.entrance()->activiated_registered().first;

        if(args.count() > 1)
            browser->loadPage(args.last());  // mainWindow()->loadPage(args.last());
        else
            browser->slotHome(); // mainWindow()->slotHome();
    }

    QtSingleApplication::historyManager();
}

void QtSingleApplication::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("websettings"));

    QWebEngineSettings *defaultSettings = QWebEngineSettings::globalSettings();
    QWebEngineProfile *defaultProfile = QWebEngineProfile::defaultProfile();

    QString standardFontFamily = defaultSettings->fontFamily(QWebEngineSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
    QFont standardFont = QFont(standardFontFamily, standardFontSize);
    standardFont = qvariant_cast<QFont>(settings.value(QLatin1String("standardFont"), standardFont));
    defaultSettings->setFontFamily(QWebEngineSettings::StandardFont, standardFont.family());
    defaultSettings->setFontSize(QWebEngineSettings::DefaultFontSize, standardFont.pointSize());

    QString fixedFontFamily = defaultSettings->fontFamily(QWebEngineSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
    QFont fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedFont = qvariant_cast<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
    defaultSettings->setFontFamily(QWebEngineSettings::FixedFont, fixedFont.family());
    defaultSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, fixedFont.pointSize());

    defaultSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, settings.value(QLatin1String("enableJavascript"), true).toBool());
    defaultSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, settings.value(QLatin1String("enableScrollAnimator"), true).toBool());

    //#if defined(QTWEBENGINE_PLUGINS)
    defaultSettings->setAttribute(QWebEngineSettings::PluginsEnabled, settings.value(QLatin1String("enablePlugins"), true).toBool());
    //#endif
    defaultSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

    QString css = settings.value(QLatin1String("userStyleSheet")).toString();
    setUserStyleSheet(defaultProfile, css, _globalparameters.entrance()); //->main_window(register_record(QUrl(browser::DockedWindow::_defaulthome)))


    defaultProfile->setHttpUserAgent(settings.value(QLatin1String("httpUserAgent")).toString());
    settings.endGroup();


    //    // this group lead chrome_odthread deadlock? or initialize failure
    //    settings.beginGroup(QLatin1String("cookies"));

    //    QWebEngineProfile::PersistentCookiesPolicy persistentCookiesPolicy = QWebEngineProfile::PersistentCookiesPolicy(
    //                //                QWebEngineProfile::AllowPersistentCookies   //
    //                //                QWebEngineProfile::ForcePersistentCookies  //
    //                settings.value(QLatin1String("persistentCookiesPolicy")).toInt()
    //            );
    //    defaultProfile->setPersistentCookiesPolicy(persistentCookiesPolicy);
    //    QString pdataPath = settings.value(QLatin1String("persistentDataPath")).toString();
    //    QDir cookie_path(pdataPath);

    //    if(!cookie_path.exists()) {
    //        //        QDir dir;
    //        cookie_path.mkpath(pdataPath);
    //    }

    //    defaultProfile->setPersistentStoragePath(pdataPath);

    //    settings.endGroup();

    settings.beginGroup(QLatin1String("proxy"));
    QNetworkProxy proxy;

    if(settings.value(QLatin1String("enabled"), false).toBool()) {
        if(settings.value(QLatin1String("type"), 0).toInt() == 0)
            proxy = QNetworkProxy::Socks5Proxy;
        else
            proxy = QNetworkProxy::HttpProxy;

        proxy.setHostName(settings.value(QLatin1String("hostName")).toString());
        proxy.setPort(settings.value(QLatin1String("port"), 1080).toInt());
        proxy.setUser(settings.value(QLatin1String("userName")).toString());
        proxy.setPassword(settings.value(QLatin1String("password")).toString());
    }

    QNetworkProxy::setApplicationProxy(proxy);
    settings.endGroup();

}

//QList<BrowserWindow *> QtSingleApplication::mainWindows()
//{
//    clean();
//    QList<BrowserWindow *> list;

//    for(int i = 0; i < _mainWindows.count(); ++i)
//        list.append(_mainWindows.at(i));

//    return list;
//}

//void QtSingleApplication::clean()
//{
//    // cleanup any deleted main windows first
//    for(int i = _mainWindows.count() - 1; i >= 0; --i)
//        if(_mainWindows.at(i).isNull())
//            _mainWindows.removeAt(i);
//}

void QtSingleApplication::saveSession()
{
    if(_private_browsing)
        return;

    globalparameters.entrance()->clean();

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));

    QByteArray data;
    QBuffer buffer(&data);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadWrite);

    QList<QPointer<browser::Browser> > mws = globalparameters.entrance()->window_list();

    stream << mws.count();

    for(int i = 0; i < mws.count(); ++i)
        stream << mws.at(i)->save_state();

    settings.setValue(QLatin1String("lastSession"), data);
    settings.endGroup();
}

bool QtSingleApplication::canRestoreSession() const
{
    return !_last_session.isEmpty();
}

void QtSingleApplication::restoreLastSession()
{
    QList<QByteArray> historywindows;
    QBuffer buffer(&_last_session);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadOnly);
    int windowCount;
    stream >> windowCount;

    for(int i = 0; i < windowCount; ++i) {
        QByteArray windowState;
        stream >> windowState;
        historywindows.append(windowState);
    }

    QList<QPointer<browser::Browser > > opened_windows = globalparameters.entrance()->window_list();

    if(opened_windows.count() == 0) {
        globalparameters.entrance()->new_browser(
            QUrl(browser::Browser::_defaulthome)
        );
    }

    for(int i = 0; i < historywindows.count(); ++i) {
        //        browser::BrowserWindow *newWindow = 0;
        //        QList<QPointer<browser::DockedWindow > > opened_windows = globalparameters.entrance()->window_list();

        browser::Browser *current_window = globalparameters.entrance()->activiated_registered().first;

        assert(current_window->currentTab()->page()->url() == QUrl());

        if(opened_windows.count() == 1
           && current_window->tabWidget()->count() == 1
           && current_window->currentTab()->page()->url() == QUrl(browser::Browser::_defaulthome) //?
          ) {
            // newWindow = globalParameters.browsermanager()->main_window();
            globalparameters.entrance()->restore_state(historywindows.at(i));
        } else {
            // newWindow =
            globalparameters.entrance()->new_browser(
                historywindows.at(i)   // register_record(QUrl(browser::DockedWindow::_defaulthome))
            );
        }


        // newWindow->restoreState(windows.at(i));
    }
}

//bool QtSingleApplication::isTheOnlyBrowser() const
//{
//    return (_localserver != 0);
//}

void QtSingleApplication::installTranslator(const QString &name)
{
    QTranslator *translator = new QTranslator(this);
    translator->load(name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(translator);
}

#if defined(Q_OS_OSX)
bool QtSingleApplication::event(QEvent *event)
{
    switch(event->type()) {
        case QEvent::ApplicationActivate: {
            clean();

            if(!_mainwindows.isEmpty()) {
                BrowserWindow *mw = mainWindow();

                if(mw && !mw->isMinimized()) {
                    mainWindow()->show();
                }

                return true;
            }
        }

        case QEvent::FileOpen:
            if(!_mainwindows.isEmpty()) {
                mainWindow()->loadPage(static_cast<QFileOpenEvent *>(event)->file());
                return true;
            }

        default:
            break;
    }

    return QApplication::event(event);
}
#endif

//void QtSingleApplication::openUrl(const QUrl &url)
//{
//    mainWindow()->loadPage(url.toString());
//}

//BrowserWindow *QtSingleApplication::newMainWindow()
//{
//    //BrowserView *browser_view = globalParameters.getBrowserView();
//    //BrowserWindow *browser = nullptr;

//    //if(browser_view) {
//    //        browser = browser_view->getBrowserWindow();     //
//    BrowserWindow *browser = new BrowserWindow();
//    //        if(!browser) {
//    //            browser = new BrowserWindow(browser_view);  //incomplete
//    //        }
//    _mainWindows.prepend(browser);
//    //browser->show();
//    //}

//    //this->exec();
//    return browser;
//}

//BrowserWindow *QtSingleApplication::mainWindow()
//{
//    clean();

//    if(_mainWindows.isEmpty()) {
//        //        BrowserView *browser_view = globalParameters.getBrowserView();
//        //        BrowserWindow *browser = nullptr;

//        //        if(browser_view) {
//        //            browser = browser_view->getBrowserWindow();

//        //            if(nullptr == browser) {
//        //                browser = new BrowserWindow(browser_view);
//        //            }
//        //        }

//        //        _mainwindows.prepend(browser);
//        newMainWindow();
//    }

//    return _mainWindows[0];
//}

//void QtSingleApplication::newLocalSocketConnection()
//{
//    QLocalSocket *socket = _localserver->nextPendingConnection();

//    if(!socket)
//        return;

//    socket->waitForReadyRead(1000);
//    QTextStream stream(socket);
//    QString url;
//    stream >> url;

//    if(!url.isEmpty()) {
//        QSettings settings;
//        settings.beginGroup(QLatin1String("general"));
//        int openLinksIn = settings.value(QLatin1String("openLinksIn"), 0).toInt();
//        settings.endGroup();

//        if(openLinksIn == 1) {
//            //globalParameters.getBrowserView()->getBrowserWindow();  //newMainWindow();
//        } else
//            mainWindow()->tabWidget()->newTab();

//        openUrl(url);
//    }

//    delete socket;
//    mainWindow()->raise();
//    mainWindow()->activateWindow();
//}

browser::CookieJar *QtSingleApplication::cookieJar()
{
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
    return (browser::CookieJar *)networkAccessManager()->cookieJar();
#else
    return 0;
#endif
}

browser::DownloadManager *QtSingleApplication::downloadManager()
{
    //    if(!_downloadmanager) {
    //        _downloadmanager = new browser::DownloadManager();
    //    }

    //    return _downloadmanager;

    return globalparameters.download_manager();
}

QNetworkAccessManager *QtSingleApplication::networkAccessManager()
{
    //#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)

    //    if(!s_networkAccessManager) {
    //        s_networkAccessManager = new browser::NetworkAccessManager();
    //        s_networkAccessManager->setCookieJar(new browser::CookieJar);
    //    }

    //    return s_networkAccessManager;
    //#else

    //    if(!_networkaccessmanager) {
    //        _networkaccessmanager = new QNetworkAccessManager();
    //    }

    //    return _networkaccessmanager;
    //#endif

    if(!_networkaccessmanager) {
        _networkaccessmanager = new QNetworkAccessManager();
        connect(_networkaccessmanager, &QNetworkAccessManager::authenticationRequired,
                QtSingleApplication::instance(), &QtSingleApplication::authenticationRequired);
        connect(_networkaccessmanager, &QNetworkAccessManager::proxyAuthenticationRequired,
                QtSingleApplication::instance(), &QtSingleApplication::proxyAuthenticationRequired);
    }

    return _networkaccessmanager;

}

browser::HistoryManager *QtSingleApplication::historyManager()
{
    if(!_historymanager)
        _historymanager = new browser::HistoryManager();

    return _historymanager;
}

browser::BookmarksManager *QtSingleApplication::bookmarksManager()
{
    if(!_bookmarksmanager) {
        _bookmarksmanager = new browser::BookmarksManager();
    }

    return _bookmarksmanager;
}

QIcon QtSingleApplication::icon(const QUrl &url) const
{
#if defined(QTWEBENGINE_ICONDATABASE)
    QIcon icon = QWebEngineSettings::iconForUrl(url);

    if(!icon.isNull())
        return icon.pixmap(16, 16);

#else
    Q_UNUSED(url);
#endif
    return defaultIcon();
}

QIcon QtSingleApplication::defaultIcon() const
{
    if(_default_icon.isNull())
        _default_icon = QIcon(QLatin1String(":defaulticon.png"));

    return _default_icon;
}

void QtSingleApplication::setPrivateBrowsing(bool privateBrowsing)
{
    if(_private_browsing == privateBrowsing)
        return;

    _private_browsing = privateBrowsing;

    if(privateBrowsing) {
        if(!_private_profile)
            _private_profile = new QWebEngineProfile(this);

        Q_FOREACH(browser::Browser *window, globalparameters.entrance()->window_list()) {
            window->tabWidget()->setProfile(_private_profile);
        }
    } else {
        Q_FOREACH(browser::Browser *window, globalparameters.entrance()->window_list()) {
            window->tabWidget()->setProfile(QWebEngineProfile::defaultProfile());
            window->lastsearch() = QString::null;
            window->tabWidget()->clear();
        }
    }

    emit privateBrowsingChanged(privateBrowsing);
}

// TODO: Remove these functions (QTBUG-47967)
QByteArray QtSingleApplication::authenticationKey(const QUrl &url, const QString &realm)
{
    QUrl copy = url;
    copy.setFragment(realm);
    return "auth:" + copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath | QUrl::RemoveQuery);
}

QByteArray QtSingleApplication::proxyAuthenticationKey(const QNetworkProxy &proxy, const QString &realm)
{
    QString host = QString("%1:%2").arg(proxy.hostName()).arg(proxy.port());
    return QtSingleApplication::proxyAuthenticationKey(proxy.user(), host, realm);
}

QByteArray QtSingleApplication::proxyAuthenticationKey(const QString &user, const QString &host, const QString &realm)
{
    QUrl key;
    key.setScheme(QLatin1String("proxy-http"));
    key.setUserName(user);
    key.setHost(host);
    key.setFragment(realm);
    return "auth:" + key.toEncoded();
}


void QtSingleApplication::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    if(_last_authenticator.isNull())
        return;


    Q_ASSERT(_last_authenticator.option("key").isValid());
    QByteArray lastKey = _last_authenticator.option("key").toByteArray();
    QByteArray key = QtSingleApplication::authenticationKey(reply->url(), authenticator->realm());

    if(lastKey == key)
        *authenticator = _last_authenticator;
}

void QtSingleApplication::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    if(_last_proxy_authenticator.isNull())
        return;

    QNetworkProxy::ProxyType proxyType = proxy.type();

    if(proxyType != QNetworkProxy::HttpProxy || proxyType != QNetworkProxy::HttpCachingProxy)
        return;

    Q_ASSERT(_last_proxy_authenticator.option("host").isValid());
    QByteArray lastKey = _last_proxy_authenticator.option("key").toByteArray();
    QByteArray key = QtSingleApplication::proxyAuthenticationKey(proxy, authenticator->realm());

    if(lastKey == key)
        *authenticator = _last_authenticator;
}

