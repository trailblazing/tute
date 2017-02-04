/*******************************************************************************

    QTM - Qt-based blog manager
    Copyright (C) 2006-2009 Matthew J Smith

    This program is free software; you can redistribute it and/or modify
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

// main.cc - Main window loader for QTM

#include "Application.h"
#include "EditingWindow.h"

#ifdef USE_SYSTRAYICON
#include "SysTrayIcon.h"
#ifdef Q_OS_MAC
#include "SuperMenu.h"
#endif
#endif

int main(int argc, char *argv[]){
    EditingWindow *c;
#ifdef USE_SYSTRAYICON
    SysTrayIcon *sti;
#endif

#ifdef Q_OS_MAC
#if QT_VERSION <= 0x050000
    if(QSysInfo::MacintoshVersion > QSysInfo::MV_10_8){
	// Fix Mavericks font issue: https://bugreports.qt-project.org/browse/QTBUG-32789
	QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
    }
#endif	// QT_VERSION
#endif	// Q_OS_MAC

    Application app(argc, argv);
    QStringList args = app.arguments();
    if(args.contains("--delete-sandbox")){
	app.deleteSandbox();
	exit(0);
    }
#ifdef USE_SYSTRAYICON
    if(QSystemTrayIcon::isSystemTrayAvailable()){
	sti = new SysTrayIcon;
	if(! sti->dontStart()){
#ifndef Q_OS_MAC
	    sti->show();
#else
	    SuperMenu *smenu = new SuperMenu(0, sti);
	    smenu->setObjectName("MacGlobalMenu");
	    smenu->show();
	    smenu->handleNewWindowAtStartup();
#endif
	    app.setQuitOnLastWindowClosed(false);
	}
    }else{
	c = new EditingWindow;
	c->setSTI(0);	// No STI
	c->setWindowTitle(QObject::tr("QTM - new entry [*]"));
	if(c->handleArguments()) c->show();
	else c->close();
    }
#else
    app.setupRecentFiles();
    c = new EditingWindow;
    c->setWindowTitle(QObject::tr("QTM - new entry [*]"));
    if(c->handleArguments()){
#if Q_OS_MAC
	setNoStatusBar(c);
#endif
	c->show();
    }else{
	qDebug("closing");
	c->close();
    }
#endif

#ifdef USE_SYSTRAYICON
    if(QSystemTrayIcon::isSystemTrayAvailable()){
	if(! sti->dontStart()) return app.exec();
    }else return app.exec();
#else
    return app.exec();
#endif
}
