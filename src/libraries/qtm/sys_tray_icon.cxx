/*********************************************************************************

   SysTrayIcon.cc - System tray icon for QTM
   Copyright (C) 2006-2009 Matthew J Smih

   This file is part of QTM.

   QTM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License (version 2), as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *******************************************************************************/

// SysTrayIcon.cc - System tray icon for QTM

#include <QtGlobal>
// #include "useSTI.h"

#ifdef USE_SYSTRAYICON

#ifdef Q_OS_MAC
#include <QSystemTrayIcon>
// #include "macFunctions.h"
#endif

#include <QAction>
#include <QClipboard>
#include <QCoreApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QUrl>
// #include <QHttpRequestHeader>
#include <QFile>
#include <QList>
#include <QRegExp>
#include <QTextStream>

#include <QtDebug>
#include <QtXml>
#include <cstdio>

#ifndef DONT_USE_DBUS
#include "DBusAdaptor.h"
#include <QtDBus>
#endif

#include "blogger.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "main.h"
#include "quickpost_template.h"
#include "quickpost_template_dialog.h"
#include "sys_tray_icon.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

// #include "ui_QuickpostTemplateForm.h"

#if QT_VERSION >= 0x050000
#ifdef Q_OS_UNIX
#if !defined Q_OS_MAC && !defined Q_OS_CYGWIN
#define Q_WS_X11
#endif
#endif
#endif

#ifdef Q_WS_X11
#include "susesystray.xpm"
#else
#ifdef Q_OS_WIN32
#include "winsystray.xpm"
#include "winsystray_busy.xpm"
#endif
#endif

#include "qtm_version.h.in"

SysTrayIcon::SysTrayIcon(HidableTab* vtab_record, web::Docker<Blogger>* editor_docker, wn_t* main_window, web::Profile* profile, QString style_source, bool noWindow, Qt::WindowFlags flags, QObject* parent)
    : STI_SUPERCLASS(parent)
    //	  , _tree_screen(tree_screen)
    //	  , _browser_dock(browser_dock)
    , _vtab_record(vtab_record)
    , _main_window(main_window)
    , _profile(profile)
    , _flags(flags)
    , _style_source(style_source)
    //	  , _find_screen(find_screen)
    , _editor_docker(editor_docker)
{
	//	(void) noWindow;
	bool newWindow;
	bool noNewWindow = noWindow; // false;

	bool isUnityThere = false;
	_dontStart = false;

	QCoreApplication::setOrganizationName("Catkin Project");
	QCoreApplication::setOrganizationDomain("catkin.blogistan.co.uk");
	QCoreApplication::setApplicationName(program_title_qstring);
	QCoreApplication::setApplicationVersion(app_version);
	userAgentString = (program_title_string + "/").c_str();
	userAgentString.append(app_version);

	QSettings settings;
	settings.beginGroup("sysTrayIcon");
	newWindow = settings.value("newWindowAtStartup", true).toBool();
	doubleClickFunction = settings.value("doubleClickFunction", 0).toInt();
	settings.endGroup();
	settings.beginGroup("account");
	_copyTitle = settings.value("copyTitle", true).toBool();
	settings.endGroup();

#ifndef DONT_USE_DBUS
	new DBusAdaptor(this);

	QDBusConnection sessionBus = QDBusConnection::sessionBus();
	sessionBus.registerObject("/MainApplication", this);
	sessionBus.registerService("uk.co.blogistan.catkin");

	// Now check for presence of Unity
	QDBusConnectionInterface* interface = sessionBus.interface();
	isUnityThere = interface->isServiceRegistered("com.canonical.Unity").value();
#endif

	_app = sapp_t::instance(); // qobject_cast<sapp_t *>(sapp_t::instance());

#ifndef Q_OS_MAC
	switch (doubleClickFunction) {
	case 0:
		setToolTip(
		    tr((program_title_string + " - Double click for \nnew entry").c_str()));
		break;
	case 1:
		setToolTip(tr((program_title_string + " - Double click for \nQuick Post").c_str()));
		break;
	}
	setIcon(
	    QIcon(":/resource/pic/logo.svg")); // (QIcon(QPixmap(sysTrayIcon_xpm)));
#endif

	newWindowAtStartup = new QAction(tr("New entry at startup"), this);
	newWindowAtStartup->setCheckable(true);
	newWindowAtStartup->setChecked(newWindow);
	connect(newWindowAtStartup, SIGNAL(toggled(bool)), this, SLOT(setNewWindowAtStartup(bool)));

	// Override the "newWindowAtStartup" setting if Unity has been found
	QStringList args = QApplication::arguments();
	if (isUnityThere && !args.contains("--no-new-window"))
		newWindow = true;
	menu = new QMenu;
	menu->addAction(tr("New entry"), this, SLOT(newDoc()));
	menu->addAction(tr("Open ..."), this, SLOT(choose()));
	openRecent = menu->addAction(tr("Open recent"));
	menu->addAction(tr("Save all ..."), this, SLOT(saveAll()));
	menu->addAction(tr("Quick post"), this, SLOT(quickpost()));
	abortAction = menu->addAction(tr("Abort quick post"), this, SLOT(abortQP()));
	abortAction->setEnabled(false);
	templateMenu = menu->addMenu(tr("Quickpost templates"));
	templateMenu->setObjectName("templateMenu");
	configureTemplates = new QAction(tr("&Configure ..."), 0);
	connect(configureTemplates, SIGNAL(triggered(bool)), this, SLOT(configureQuickpostTemplates()));
	configureTemplates->setObjectName(program_title_qstring + " Configure Templates");
	setupQuickpostTemplates();
	//
	menu->addSeparator();
	setup_main_window_icon_actions();
	menu->addAction(_action_tray_restore);
	menu->addAction(_action_tray_maximize);
	menu->addAction(_action_tray_minimize);
	menu->addSeparator();

#if (!defined Q_OS_MAC) || QT_VERSION <= 0x050000
	// #ifndef Q_OS_MAC              // Always a new window at startup when
	// #if QT_VERSION <= 0x050000   // using Qt 5 on a Mac
	if (!isUnityThere)
		menu->addAction(newWindowAtStartup);
#endif

#ifndef Q_OS_MAC
	menu->addSeparator();
#endif
#ifndef SUPERMENU
	// quitAction = menu->addAction(tr("Quit"), this, SLOT(doQuit()));
	menu->addAction(_action_tray_quit);
#endif
	// quitAction->setMenuRole( QAction::QuitRole );
	menu->setObjectName("iconMenu");

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000 // Qt versions 5.0 and 5.1 have neither of these.
	menu->setAsDockMenu();
#else
	qt_mac_set_dock_menu(menu);
#endif // QT_VERSION
#else  // i.e. not a Mac
	setContextMenu(menu);
#endif
	// show();

	_app->setupRecentFiles();
	recentFilesMenu = new QMenu;
	noRecentFilesAction = new QAction(tr("No recent files"), this);
	noRecentFilesAction->setVisible(true);
	noRecentFilesAction->setEnabled(false);
	for (int i = 0; i < 10; ++i) {
		recentFileActions[i] = new QAction(this);
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
	_recent_files = _app->recentFiles();
	updateRecentFileMenu();
	openRecent->setMenu(recentFilesMenu);
	connect(_app, SIGNAL(recent_files_updated(QList<std::shared_ptr<app::RecentFile>>)), this, SLOT(set_recent_files(QList<std::shared_ptr<app::RecentFile>>)));

#ifndef Q_OS_MAC
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
#endif
	if (handleArguments())
		noNewWindow = true;
#ifndef Q_OS_MAC
	if (newWindow && !noNewWindow) {
		blogger_ref c(new Blogger());
		c->setSTI(this);
		c->setWindowTitle(tr((program_title_string + " - new entry [*]").c_str()));
		c->show();
		c->activateWindow();
	}
#endif

	netmgr = new QNetworkAccessManager;
	httpBusy = false;
	templateQPActive = false;
	activeTemplate = -1;
}

SysTrayIcon::~SysTrayIcon()
{
}

bool SysTrayIcon::handleArguments()
{
	int i;
	bool rv = false;
//	blogger_ref c;
	QStringList failedFiles;
	QStringList args = QApplication::arguments();
	args.removeAll("&");
	for (i = 1; i < args.size(); i++) {
		blogger_ref c (new Blogger());
		if (!args.at(i).startsWith("--")) {
			qDebug() << "Opening:" << args.at(i);
			if (c->load(args.at(i), true)) {
				c->setSTI(this);
#ifdef Q_OS_MAC
// setNoStatusBar( c );
#endif
				c->show();
				rv = true;
			} else
				failedFiles.append(args.at(i));
		}
	}
	if (failedFiles.size()) {
		if (failedFiles.size() == args.size() - 1) {
			if (QMessageBox::question(0, tr("Error"), tr("Could not load the following:\n\n%1").arg(failedFiles.join("\n")), tr("&Continue"), tr("E&xit")))
				_dontStart = true;
			else
				rv = false;
		} else {
			QMessageBox::information(0, tr("Error"), tr("Could not load the following:\n\n%1 ").arg(failedFiles.join("\n")), QMessageBox::Ok);
			rv = true;
		}
	}
	return rv;
}

void SysTrayIcon::updateRecentFileMenu()
{
	QString text, t;
	int j;
	// qDebug() << "Recent files:" << recentFiles.count();
	QMutableListIterator<std::shared_ptr<app::RecentFile>> i(_recent_files);
	while (i.hasNext())
		if (!QFile::exists(i.next()->filename))
			i.remove();
	recentFilesMenu->clear();
	for (j = 0; j < 10; ++j) {
		if (j < _recent_files.count()) {
			t = _recent_files.value(j)->title.section(' ', 0, 5);
			if (t != _recent_files.value(j)->title)
				t.append(tr(" ..."));
#ifdef Q_OS_MAC
			text = recentFiles.value(j).title.isEmpty() ? recentFiles.value(j).filename.section("/", -1, -1) : t.replace('&', "&&");
#else
			if (j == 9)
				text = tr("1&0 %1").arg(
				    _recent_files.value(j)->title.isEmpty() ? _recent_files.value(j)->filename.section("/", -1, -1) : t.replace('&', "&&"));
			else
				text = tr("&%1 %2").arg(j + 1).arg(
				    _recent_files.value(j)->title.isEmpty() ? _recent_files.value(j)->filename.section("/", -1, -1) : t.replace('&', "&&"));
#endif
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(_recent_files.value(j)->filename);
			recentFilesMenu->addAction(recentFileActions[j]);
			recentFilesMenu->removeAction(noRecentFilesAction);
		} else {
			// recentFileActions[j]->setVisible( false );
			if (!_recent_files.count())
				recentFilesMenu->addAction(noRecentFilesAction);
		}
	}
}

void SysTrayIcon::set_recent_files(const QList<std::shared_ptr<app::RecentFile>>& rfs)
{
	_recent_files = rfs;
	updateRecentFileMenu();
}

void SysTrayIcon::setDoubleClickFunction(int func)
{
	doubleClickFunction = func;

#ifndef Q_OS_MAC
	switch (doubleClickFunction) {
	case 0:
		setToolTip(
		    tr((program_title_string + " - Double click for \nnew entry").c_str()));
		break;
	case 1:
		setToolTip(tr((program_title_string + " - Double click for \nQuick Post").c_str()));
		break;
	}
#endif
}

bool SysTrayIcon::dontStart()
{
	return _dontStart;
}

QStringList SysTrayIcon::templateTitles()
{
	QStringList rv;
	int i;
	for (i = 0; i < templateTitleList.count(); i++) {
		rv.append(QString("%1.%2").arg(i).arg(templateTitleList.value(i)));
	}
	return rv;
}

QStringList SysTrayIcon::templates()
{
	QStringList rv;
	QString currentTemplate;
	int i;
	for (i = 0; i < templateTitleList.count(); i++) {
		currentTemplate =
		    templateList.value(i).replace("\n", "\\n").replace("]", "\\]");
		rv.append(QString("%1.[%2].[%3]").arg(i).arg(templateTitleList.value(i)).arg(currentTemplate));
	}
	return rv;
}

void SysTrayIcon::setCopyTitle(bool status)
{
	_copyTitle = status;
}

void SysTrayIcon::newDoc()
{
	blogger_ref c(new Blogger());
	c->setSTI(this);
	c->setWindowTitle(QObject::tr((program_title_string + " - new entry [*]").c_str()));
	c->setPostClean();
#ifdef Q_OS_MAC
// setNoStatusBar( c );
#endif
	Blogger* blogger_ = _app->blogger();
	qDebug() << "got active widget";
	if (blogger_)
		Blogger::positionWidget(c, blogger_);
	qDebug() << "positioned widget";
	c->show();
	c->activateWindow();
	QApplication::alert(c);
}

void SysTrayIcon::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
		choose(action->data().toString());
}

void SysTrayIcon::choose(QString fname)
{
	QString fn;
	QSettings settings;
	QString local_storage_file_ext, localStorageDirectory;
	QStringList filesSelected;

	settings.beginGroup("account");
	local_storage_file_ext =
	    settings.value("local_storage_file_ext", "cqt").toString();
	localStorageDirectory =
	    settings.value("editors_shared_directory", "").toString();
	settings.endGroup();

	QString extn(
	    QString("%1 (*.%2)").arg(tr("Blog entries")).arg(local_storage_file_ext));
	if (fname.isEmpty())
		fn = QFileDialog::getOpenFileName(0, tr("Choose a file to open"), localStorageDirectory, extn);
	else
		fn = fname;
	if (!fn.isEmpty()) {
		blogger_ref e(new Blogger());
		if (!e->load(fn, true)) {
#ifdef Q_OS_MAC
			QMessageBox::warning(0, program_title_string.c_str(), tr("Could not load the file you specified."), QMessageBox::Cancel, QMessageBox::NoButton);
#else
			showMessage(tr("Error"), tr("Could not load the file you specified."), QSystemTrayIcon::Warning);
#endif
			e->deleteLater();
		} else {
			e->setSTI(this);
			_app->add_recent_file(e->postTitle(), fn);
#ifdef Q_OS_MAC
// setNoStatusBar( e );
#endif
			e->show();
			e->activateWindow();
			QApplication::alert(e);
		}
	}
}

void SysTrayIcon::chooseRecentFile()
{
#ifdef UNITY_LAUNCHER
	int i, j;
	QString fileToOpen;
	QList<QListWidgetItem*> recentFilesForDialog;
	QListWidgetItem* thisRecentFile;

	QDialog chooser;
	Ui::ListDialogBase ldui;
	ldui.setupUi(&chooser);

	chooser.setAttribute(Qt::WA_DeleteOnClose, false);
	chooser.setWindowTitle(tr("Choose recent file"));
	ldui.mainLabel->setText(tr("Recent files:"));

	// Load the recent files into the list widget
	recentFiles = qtm->recentFiles();
	j = (recentFiles.count() >= 10) ? 10 : recentFiles.count();
	for (i = 0; i < j; i++) {
		thisRecentFile = new QListWidgetItem(recentFiles.at(i).title);
		thisRecentFile->setData(32, recentFiles.at(i).filename);
		ldui.lwFileTemplateList->addItem(thisRecentFile);
	}
	connect(ldui.lwFileTemplateList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openRecentFileFromDialog(QListWidgetItem*)));
	// Dialog must only accept if OK is clicked, as double-click is handled by
	// the above slot.
	connect(ldui.lwFileTemplateList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), &chooser, SLOT(reject()));
	if (chooser.exec()) {
		fileToOpen = ldui.lwFileTemplateList->currentItem()->data(32).toString();
		choose(fileToOpen);
	}
#endif
}

void SysTrayIcon::openRecentFileFromDialog(QListWidgetItem* item)
{
	(void)item;
#ifdef UNITY_LAUNCHER
	QString fileToOpen = item->data(32).toString();
	choose(fileToOpen);
#endif
}

void SysTrayIcon::iconActivated(QSystemTrayIcon::ActivationReason ar)
{
#ifndef Q_OS_MAC
	if (ar == QSystemTrayIcon::DoubleClick)
		switch (doubleClickFunction) {
		case 0:
			newDoc();
			break;
		case 1:
			quickpost();
			break;
		}
#ifdef Q_WS_X11
	if (ar == QSystemTrayIcon::MiddleClick)
		quickpost(QClipboard::Selection);
#endif
#endif
}

void SysTrayIcon::quickpost(QClipboard::Mode mode)
{
	int i, j;
	bool qpt = false;
	QRegExp regExp;

	_cb_text = QApplication::clipboard()->text(mode);
	if (_cb_text == "") {
#ifdef Q_OS_MAC
		QMessageBox::warning(0, tr("No selection! - QTM"), tr("No web location specified to blog about."), QMessageBox::Cancel);
#else
		showMessage(tr("No selection!"), tr("No web location specified to blog about."), QSystemTrayIcon::Warning);
#endif
	} else {
		if (!_cb_text.startsWith("http://")
#if !defined DONT_USE_SSL
		    && !_cb_text.startsWith("https://")
#endif
			) {
			// If it's not obviously an URL.
			if (_cb_text.startsWith("https")) {
#ifndef Q_OS_MAC
				showMessage(tr("Error"), tr("This version of QTM does not support HTTPS."));
#else
				QMessageBox::information(
				    0, tr("Quickpost Error"),
				    tr("This version of QTM does not support HTTPS."));
#endif
			}
			doQP("");
		} else {
			if (!QUrl(_cb_text, QUrl::StrictMode).isValid()) {
				if (_cb_text.contains(QChar(' ')))
					doQP("");
				else {
#ifdef Q_OS_MAC
					QMessageBox::warning(0, tr("Invalid web location"), tr("Quick Post requires valid web location."), QMessageBox::Cancel);
#else
					showMessage(tr("Invalid web location"), tr("Quick Post requires valid web location."), QSystemTrayIcon::Warning);
#endif
				}
			} else {
				// Otherwise, it's an URL, and has to be downloaded to extract the
				// title.
				if (!httpBusy) {
					if (assocHostLists.count()) {
						for (i = 0; i < assocHostLists.count(); ++i) {
							if (assocHostLists.at(i).count()) {
								for (j = 0; j < assocHostLists.at(i).count(); j++) {
									if ((!assocHostLists.at(i).at(j).isEmpty()) &&
									    _cb_text.contains(
										QRegExp(QString("[/\\.]%1[/\\.]").arg(assocHostLists.at(i).at(j))))) {
										qpt = true;
										quickpostFromTemplate(
										    i, quickpostTemplateActions.value(i)->postTemplate(),
										    _cb_text);
										break;
									}
								}
							}
							if (qpt)
								break;
						}
					}
					if (!qpt) {
						//// qDebug() << "doing quickpost";
						QNetworkRequest request;
						request.setUrl(QUrl(_cb_text));
						request.setRawHeader("User-Agent", userAgentString);
						currentReply = netmgr->get(request);
						qDebug() << "Fetching URL:" << request.url();
						/*
                                                                                    QString withoutHttp = cbtext.section( "://", 1 );
                                                                                    QString host = withoutHttp.section( "/", 0, 0 );
                                                                                    QString loc = withoutHttp.section( '/', 1, -1, QString::SectionIncludeLeadingSep );
                                                                                    QHttpRequestHeader header( "GET", loc );
                                                                                    header.setValue( "Host", host );
                                                                                    header.setValue( "User-Agent", userAgentString );

                                     #if QT_VERSION >= 0x040300 && !defined
               DONT_USE_SSL
                                                                                    http->setHost( host, cbtext.startsWith( "https://" ) ?
                                                                                    QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp );
                                                                                    connect( http, SIGNAL( sslErrors( QList<QSslError> ) ),
                                                                                    http, SLOT( ignoreSslErrors() ) );
                                     #else
                                                                                    http->setHost( host );
                                     #endif
                                                                                    http->request( header ); */
						abortAction->setEnabled(true);
						httpBusy = true;
						templateQPActive = false;
#ifdef Q_OS_WIN32
						setIcon(QIcon(QPixmap(sysTrayIcon_busy_xpm)));
#endif
						connect(netmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleDone(QNetworkReply*)));
					}
				}
			}
		}
	}
}

void SysTrayIcon::quickpostFromTemplate(int id, QString templateString, QString t)
{
	(void)templateString;
	activeTemplate = id;
	if (t.isNull())
		_cb_text = QApplication::clipboard()->text(QClipboard::Clipboard);
	else
		_cb_text = t;
	if (_cb_text.startsWith("https://")) {
#ifndef Q_OS_MAC
		showMessage(tr("Error"), tr("This version of QTM does not support HTTPS."));
#else
		QMessageBox::information(0, tr("Quickpost Error"), tr("This version of QTM does not support HTTPS."));
#endif
	}
	if (!QUrl(_cb_text, QUrl::StrictMode).isValid()) {
#ifdef Q_OS_MAC
		QMessageBox::warning(
		    0, tr("No selection!"),
		    tr("The selection is not a web location, or is invalid."),
		    QMessageBox::Cancel);
#else
		showMessage(tr("No selection!"), tr("The selection is not a web location, or is invalid."), QSystemTrayIcon::Warning);
#endif
	} else {
		if (!httpBusy) {
			QNetworkRequest request;
			request.setUrl(QUrl(_cb_text));
			request.setRawHeader("User-Agent", userAgentString);
			currentReply = netmgr->get(
			    request); /*
                                               QString withoutHttp =
                       cbtext.section( "://", 1 );
                                               QString host =
                       withoutHttp.section( "/", 0, 0 );
                                               QString loc =
                       withoutHttp.section( '/', 1, -1,
                       QString::SectionIncludeLeadingSep );

                                 #if QT_VERSION >= 0x040300 && !defined
                       DONT_USE_SSL
                                               http->setHost( host,
                       cbtext.startsWith( "https://" ) ?
                                               QHttp::ConnectionModeHttps :
                       QHttp::ConnectionModeHttp );
                                               connect( http, SIGNAL( sslErrors(
                       QList<QSslError> ) ),
                                               http, SLOT( ignoreSslErrors() )
                       );
                                 #else
                                               http->setHost( host );
                                 #endif
                                               http->get( loc ); */
			abortAction->setEnabled(true);
			httpBusy = true;
			templateQPActive = true;
#ifdef Q_OS_WIN32
			setIcon(QIcon(QPixmap(sysTrayIcon_busy_xpm)));
#endif
			connect(netmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleDone(QNetworkReply*)));
			/*      connect( http, SIGNAL( readyRead( const QHttpResponseHeader & ) ),
                                 this, SLOT( handleResponseHeader( const
         QHttpResponseHeader & ) ) ); */
		}
	}
}

void SysTrayIcon::quickpostFromDBus(QString& url, QString& content)
{
	int i, j;
	for (i = 0; i < assocHostLists.count(); ++i) {
		if (assocHostLists.at(i).count()) {
			for (j = 0; j < assocHostLists.at(i).count(); j++) {
				if ((!assocHostLists.at(i).at(j).isEmpty()) &&
				    url.contains(QRegExp(QString("[/\\.]%1[/\\.]").arg(assocHostLists.at(i).at(j))))) {
					activeTemplate = i;
					break;
				}
			}
		}
	}
	_cb_text = url;
	doQP(content);
}

void SysTrayIcon::chooseQuickpostTemplate()
{
#ifdef UNITY_LAUNCHER
	QListWidgetItem* thisTemplate;

	QDialog chooser;
	Ui::ListDialogBase ldui;
	ldui.setupUi(&chooser);

	chooser.setAttribute(Qt::WA_DeleteOnClose, false);
	chooser.setWindowTitle(tr("Choose template"));
	ldui.mainLabel->setText(tr("Templates:"));
	Q_FOREACH (QuickpostTemplate* temp, quickpostTemplateActions) {
		if (temp != 0) {
			thisTemplate = new QListWidgetItem(temp->text());
			thisTemplate->setData(32, temp->identifier());
			ldui.lwFileTemplateList->addItem(thisTemplate);
		}
	}
	connect(ldui.lwFileTemplateList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(actOnChooseQuickpostTemplate(QListWidgetItem*)));
	connect(ldui.lwFileTemplateList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), &chooser, SLOT(reject()));
	if (chooser.exec())
		if (quickpostTemplateActions.at(ldui.lwFileTemplateList->currentRow()) != 0)
			quickpostTemplateActions[ldui.lwFileTemplateList->currentRow()]
			    ->trigger();
#endif
}

void SysTrayIcon::actOnChooseQuickpostTemplate(QListWidgetItem* item)
{
	(void)item;
#ifdef UNITY_LAUNCHER
	QuickpostTemplate* temp = quickpostTemplateActions.at(item->data(32).toInt());
	if (temp)
		temp->trigger();
#endif
}

void SysTrayIcon::setNewWindowAtStartup(bool nwas)
{
	QSettings settings;

	settings.beginGroup("sysTrayIcon");
	settings.setValue("newWindowAtStartup", nwas);
	settings.endGroup();
}

// HTTP Quickpost handler routines

void SysTrayIcon::handleDone(QNetworkReply* reply)
{
	QString ddoc, newTitle, errorString;
	QNetworkReply::NetworkError netError;

	abortAction->setEnabled(false);
	httpBusy = false;

	netError = reply->error();
	if (netError != QNetworkReply::NoError) {
		switch (netError) {
		case QNetworkReply::HostNotFoundError:
			errorString = tr("Could not find the host.");
			break;
		case QNetworkReply::ConnectionRefusedError:
			errorString = tr("Connection was refused.");
			break;
		case QNetworkReply::RemoteHostClosedError:
			errorString = tr("Connection closed unexpectedly.");
			break;
		default:
			errorString = tr("Document was not received correctly.");
			break;
		}
#ifdef Q_OS_MAC
		QMessageBox::warning(0, tr("Quickpost Error"), errorString, QMessageBox::Cancel);
#else
		showMessage(tr("Quickpost Error"), errorString, QSystemTrayIcon::Warning);
#endif
	} else {
		responseData = reply->readAll();
		doQP(QString(responseData));
		responseData = "";
	}
	// http->close();
	netmgr->disconnect();
	currentReply->deleteLater();
	currentReply = NULL;
#ifndef Q_OS_MAC
	setIcon(
	    QIcon(":/resource/pic/logo.svg")); // (QIcon(QPixmap(sysTrayIcon_xpm)));
#endif
}

void SysTrayIcon::doQuit()
{
	int edwins = 0;
	int a;

	QWidgetList tlw = sapp_t::instance()->topLevelWidgets();
	for (a = 0; a < tlw.size(); a++)
		if ((QString(tlw[a]->metaObject()->className()) == "EditingWindow") &&
		    tlw[a]->isVisible())
			edwins++;
	if (!edwins)
		QCoreApplication::quit();
	else {
		sapp_t::instance()->setQuitOnLastWindowClosed(true);
		sapp_t::instance()->closeAllWindows();
	}
}

void SysTrayIcon::saveAll()
{
	//	sapp_t *instance = qobject_cast<sapp_t *>(sapp_t::instance());
	sapp_t::instance()->saveSession();
}

void SysTrayIcon::doQP(QString receivedText)
{
	QString newPost, newTitle;
	// QRegExp titleRegExp( "<title(
	// (dir=\"(ltr)|(rtl)\")|((xml:)?lang=\"[a-zA-Z-]+\"){0, 2}>",
	// Qt::CaseInsensitive );
	QRegExp titleRegExp("<title.*</title>", Qt::CaseInsensitive);
	QRegExp openTitleRX("<title", Qt::CaseInsensitive);
	QRegExp closeTitleRX("</title", Qt::CaseInsensitive);
	if (activeTemplate >= 0) {
		if (receivedText == "")
			newPost = _cb_text;
		else {
			newPost = templateList[activeTemplate];
			newPost.replace("%url%", _cb_text);
			newPost.replace("%host%", _cb_text.section("//", 1, 1).section("/", 0, 0));
			newPost.replace(
			    "%domain%",
			    _cb_text.section("//", 1, 1).section("/", 0, 0).remove("www."));
			if (receivedText.contains(titleRegExp) &&
			    receivedText.section(openTitleRX, 1)
				.section(">", 1)
				.contains(closeTitleRX)) {
				newTitle = receivedText.section(openTitleRX, 1)
					       .section(">", 1)
					       .section(closeTitleRX, 0, 0)
					       .simplified();
				newPost.replace("%title%", newTitle);
			}
		}
		newPost.replace("\\n", "\n");
	} else {
		if (receivedText == "")
			newPost = _cb_text;
		else {
			//// // qDebug( receivedText.left( 300 ).toAscii().constData() );
			// The title check will accept flaky 1990s HTML - this isn't a browser
			if (receivedText.contains(titleRegExp)) {
				/*&& receivedText.section( titleRegExp, 1 )
                                                          .section( '\n', 0
           ).contains( "</title>", Qt::CaseInsensitive ) )*/
				newTitle = receivedText.section(openTitleRX, 1)
					       .section(">", 1)
					       .section(closeTitleRX, 0, 0)
					       .simplified();
				if (!templateQPActive)
					newPost = QString("<a title = \"%1\" href=\"%2\">%1</a>\n\n")
						      .arg(newTitle)
						      .arg(_cb_text);
			} else // Post has no valid title
				newPost =
				    QString(tr("<a href=\"%1\">Insert link text here</a>")).arg(_cb_text);
		}
	}
	blogger_ref c(new Blogger(gl_para::_default_topic, newPost));
	c->setSTI(this);
	c->setPostClean();
	if (activeTemplate >= 0) {
		switch (defaultPublishStatusList[activeTemplate]) {
		case 0:
		case 1:
			c->setPublishStatus(defaultPublishStatusList[activeTemplate]);
		}
		// Copy the title, if a quickpost
		if (copyTitleStatusList[activeTemplate]) {
			if (!newTitle.isEmpty())
				c->setWindowTitle(
				    QString(QString("%1 - ") + gl_para::_program_instance_name + " [*]")
					.arg(newTitle));
			c->setPostTitle(newTitle);
		}
	} else {
		if (_copyTitle) {
			// Copy the title, if not a quickpost
			c->setPostTitle(newTitle);
			if (!newTitle.isEmpty())
				c->setWindowTitle(
				    QString(QString("%1 - ") + gl_para::_program_instance_name + " [*]")
					.arg(newTitle));
		}
	}
#ifdef Q_OS_MAC
// setNoStatusBar( c );
#endif
	c->show();
	c->activateWindow();
#ifdef Q_OS_WIN32
	setIcon(QIcon(QPixmap(sysTrayIcon_xpm)));
#endif

	QApplication::alert(c);

	activeTemplate = -1;
	templateQPActive = false;
}

void SysTrayIcon::abortQP()
{
	currentReply->abort();
	currentReply->deleteLater();
	currentReply = NULL;

	netmgr->disconnect();
	abortAction->setEnabled(false);
	httpBusy = false;
}

void SysTrayIcon::setupQuickpostTemplates()
{
	QString templateFile;
	QString errorString;
	QString currentTitle, currentTemplate;
	int currentDefaultPublishStatus;
	int errorLine, errorCol;
	QDomNodeList templateNodes, titles, templateStrings, defaultPublishStates;
	QDomNodeList assocHostsInTemplate;
	QDomElement currentAssocHostListsElement, currentCopyStatusElement;
	QString currentCopyStatusElementText;
	int numTemplates, t, j;
	QTextStream ts(stdout);
	bool useDefaultPublishStatus, ok;

	QSettings settings;
	settings.beginGroup("account");
	templateFile =
	    QString("%1/qptemplates.xml")
		.arg(settings.value("editors_shared_directory", "").toString());
	settings.endGroup();
	if (QFile::exists(templateFile)) {
		QDomDocument domDoc("quickpostTemplates");
		QFile file(templateFile);
		if (!domDoc.setContent(&file, true, &errorString, &errorLine, &errorCol))
			QMessageBox::warning(0, tr("Failed to read templates"), QString(tr("Error: %1\nLine %2, col %3")).arg(errorString).arg(errorLine).arg(errorCol));
		else {
			templateNodes = domDoc.elementsByTagName("template");
			titles = domDoc.elementsByTagName("title");
			templateStrings = domDoc.elementsByTagName("templateString");
			defaultPublishStates = domDoc.elementsByTagName("defaultPublishStatus");

			int tnc = templateNodes.count();
			if (tnc) {
				for (int i = 0; i < templateNodes.count(); i++) {
					currentAssocHostListsElement =
					    templateNodes.at(i).firstChildElement("associatedHosts");
					assocHostLists.append(QStringList());
					if (!currentAssocHostListsElement.isNull()) {
						assocHostsInTemplate =
						    currentAssocHostListsElement.elementsByTagName(
							"associatedHost");
						for (j = 0; j < assocHostsInTemplate.count(); j++)
							assocHostLists[i].append(
							    assocHostsInTemplate.at(j).toElement().text());
					}
					currentCopyStatusElement =
					    templateNodes.at(i).firstChildElement("copyTitleStatus");
					if (currentCopyStatusElement.isNull())
						copyTitleStatusList.append(false);
					else {
						currentCopyStatusElementText = currentCopyStatusElement.text();
						if (currentCopyStatusElementText.startsWith("1"))
							copyTitleStatusList.append(true);
						else
							copyTitleStatusList.append(false);
					}
				}
			}
			//// qDebug() << "Built list of associated hosts";
			if (titles.size()) {
				quickpostTemplateActions.clear();
				numTemplates =
				    (titles.size() >= templateStrings.size() ? titles.size() : templateStrings.size());
				useDefaultPublishStatus =
				    (defaultPublishStates.size() < numTemplates) ? false : true;

				//// qDebug( "Populating template menu" );
				// templateMenu->addSeparator();
				templateTitleList.clear();
				templateList.clear();
				for (t = 0; t < numTemplates; t++) {
					currentTitle = titles.item(t).toElement().text();
					currentTemplate = templateStrings.item(t).toElement().text();
					currentDefaultPublishStatus =
					    useDefaultPublishStatus ? defaultPublishStates.item(t).toElement().text().toInt(&ok) : 2;
					templateTitleList.append(currentTitle);
					templateList.append(currentTemplate);
					if (ok) {
						switch (currentDefaultPublishStatus) {
						case 0:
						case 1:
							defaultPublishStatusList.append(currentDefaultPublishStatus);
							break;
						default:
							defaultPublishStatusList.append(2);
						}
					} else
						defaultPublishStatusList.append(2);
					ts << titles.item(t).nodeValue();
					ts << templateStrings.item(t).nodeValue();
					quickpostTemplateActions.append(new QuickpostTemplate(t, currentTitle, currentTemplate, templateMenu));
					templateMenu->addAction(quickpostTemplateActions[t]);
					connect(quickpostTemplateActions[t], SIGNAL(quickpostRequested(int, QString)), this, SLOT(quickpostFromTemplate(int, QString)));
				}
				templateMenu->addSeparator();
			}
			/*      else
                                          // qDebug( "No templates found." );*/
		}
		file.close();
	}
	templateMenu->addAction(configureTemplates);
}

void SysTrayIcon::configureQuickpostTemplates(QWidget* parent)
{
	QDomDocument templateDocument;
	QDomElement quickpostTemplates;
	QString templateFileName;
	QString templateFile;
	QSettings settings;

	QuickpostTemplateDialog templateDialog(
	    templateTitleList, templateList, defaultPublishStatusList,
	    copyTitleStatusList, assocHostLists, _copyTitle, parent);

	configureTemplates->setEnabled(false);
	configureTemplates->setMenu(0);
	if (templateDialog.exec()) {
		// Set templates menu back up
		templateMenu->clear();
		templateTitleList = templateDialog.templateTitles();
		templateList = templateDialog.templateStrings();
		defaultPublishStatusList = templateDialog.defaultPublishStates();
		copyTitleStatusList = templateDialog.copyTitleStates();
		assocHostLists = templateDialog.assocHostLists();
		quickpostTemplates = templateDocument.createElement("QuickpostTemplates");
		int numTemplates = (templateTitleList.size() <= templateList.size()) ? templateTitleList.size() : templateList.size();
		quickpostTemplateActions.clear();
		for (int i = 0; i < numTemplates; i++) {
			quickpostTemplates.appendChild(
			    templateElement(templateDocument, templateTitleList[i], templateList[i], defaultPublishStatusList[i], copyTitleStatusList[i], assocHostLists[i]));

			quickpostTemplateActions.append(new QuickpostTemplate(i, templateTitleList[i], templateList[i], templateMenu));
			templateMenu->addAction(quickpostTemplateActions[i]);
			connect(quickpostTemplateActions[i], SIGNAL(quickpostRequested(int, QString)), this, SLOT(quickpostFromTemplate(int, QString)));
		}
		templateDocument.appendChild(quickpostTemplates);
		templateMenu->addSeparator();
		configureTemplates->setEnabled(true);
		templateMenu->addAction(configureTemplates);

		// Save template XML file
		templateDocument.insertBefore(
		    templateDocument.createProcessingInstruction("xml", "version=\"1.0\""),
		    templateDocument.firstChild());
		settings.beginGroup("account");
#if defined Q_OS_WIN32
		QString templateFileDir = QDir::toNativeSeparators(
		    settings
			.value("editors_shared_directory", QString(QString("%1\\") + gl_para::_blog_root_dir).arg(QDir::homePath()))
			.toString());
#else
		QString templateFileDir =
		    settings
			.value("editors_shared_directory", QString(QString("%1/") + gl_para::_blog_root_dir).arg(QDir::homePath()))
			.toString();
#endif
		QDir tfd(templateFileDir);
		if (!tfd.exists())
			tfd.mkpath(templateFileDir);
		templateFileName = QString("%1/qptemplates.xml").arg(templateFileDir);
		settings.endGroup();
		QFile* templateFile = new QFile(templateFileName);
		if (templateFile->open(QIODevice::WriteOnly)) {
			QTextStream templateFileStream(templateFile);
			templateDocument.save(templateFileStream, 4);
			templateFile->close();
		} else
#ifdef Q_OS_MAC
			QMessageBox::warning(0, tr("Error"), tr("Could not write to templates file"), QMessageBox::Cancel);
#else
			showMessage(tr("Error"), tr("Could not write to templates file"), Warning);
#endif
		emit quickpostTemplateTitlesUpdated(templateTitles());
		emit quickpostTemplatesUpdated(templates());
		templateFile->deleteLater();
	} else
		configureTemplates->setEnabled(true);
}

QDomElement SysTrayIcon::templateElement(QDomDocument& doc, QString& title, QString& templateString, int& defaultPublishStatus, bool& copyTitleStatus, QStringList& assocHosts)
{
	QDomElement returnElement = doc.createElement("template");
	QDomElement titleElement = doc.createElement("title");
	titleElement.appendChild(QDomText(doc.createTextNode(title)));
	QDomElement templateStringElement = doc.createElement("templateString");
	templateStringElement.appendChild(
	    QDomText(doc.createTextNode(templateString)));
	QDomElement defaultPublishStatusElement(
	    doc.createElement("defaultPublishStatus"));
	defaultPublishStatusElement.appendChild(
	    QDomText(doc.createTextNode(QString::number(defaultPublishStatus))));
	QDomElement copyTitleElement(doc.createElement("copyTitleStatus"));
	copyTitleElement.appendChild(
	    QDomText(doc.createTextNode(QString::number(copyTitleStatus ? 1 : 0))));
	returnElement.appendChild(titleElement);
	returnElement.appendChild(templateStringElement);
	returnElement.appendChild(defaultPublishStatusElement);
	returnElement.appendChild(copyTitleElement);
	if (assocHosts.count()) {
		QDomElement associatedHost;
		QDomElement assocHostListsElement = doc.createElement("associatedHosts");
		for (int i = 0; i < assocHosts.count(); i++) {
			associatedHost = doc.createElement("associatedHost");
			associatedHost.appendChild(QDomText(doc.createTextNode(assocHosts[i])));
			assocHostListsElement.appendChild(associatedHost);
		}
		returnElement.appendChild(assocHostListsElement);
	}
	return returnElement;
}
#endif

void SysTrayIcon::setup_main_window_icon_actions(void)
{
	_action_tray_restore = new QAction(tr("&Restore window"), this);
	connect(_action_tray_restore, &QAction::triggered, _main_window, &wn_t::showNormal);

	_action_tray_maximize = new QAction(tr("Ma&ximize window"), this);
	connect(_action_tray_maximize, &QAction::triggered, _main_window, &wn_t::showMaximized);

	_action_tray_minimize = new QAction(tr("Mi&nimize window"), this);
	connect(_action_tray_minimize, &QAction::triggered, _main_window, &wn_t::hide);

	_action_tray_quit = new QAction(tr("&Quit"), this);
	connect(_action_tray_quit, &QAction::triggered, _main_window, &wn_t::application_exit);
}
