/*********************************************************************************

    SysTrayIcon.h - Header file for QTM system tray icon
    Copyright (C) 2006-2009 Matthew J Smith

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

#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QtGlobal>

// #include "useSTI.h"
//#ifdef USE_SYSTRAYICON

#include <QSystemTrayIcon>
#include <QByteArray>
#include <QString>
#include <QClipboard>
#include <QList>
#include <QStringList>
#include <QDomElement>
#include "Application.h"
#ifdef UNITY_LAUNCHER
#include "ui_ListDialogBase.h"
#endif

class QMenu;
class QAction;
class QHttpResponseHeader;
class QuickpostTemplate;
class QListWidgetItem;

class QNetworkAccessManager;
class QNetworkReply;

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000  // This function is in QMenu in Qt 5.2
void qt_mac_set_dock_menu( QMenu * );
#endif
#endif

class SysTrayIcon : public STI_SUPERCLASS
{
Q_OBJECT

public:
  SysTrayIcon( bool noWindow = false, QObject *parent = 0 );
  ~SysTrayIcon();
  void setDoubleClickFunction( int );
  bool dontStart() { return _dontStart; }
  QStringList templates();
  QStringList templateTitles();
  void quickpostFromDBus( QString &, QString & );
				    
public slots:
  void configureQuickpostTemplates( QWidget *parent = 0 );
  void setCopyTitle( bool );
  void newDoc();
  void openRecentFile();
  void choose( QString fname = QString() );
  void chooseRecentFile();
  void quickpost( QClipboard::Mode mode = QClipboard::Clipboard );
  void chooseQuickpostTemplate();
  bool handleArguments();
  void doQuit();
  void saveAll();
  void setRecentFiles( const QList<Application::recentFile> & );

private slots:
  void iconActivated( QSystemTrayIcon::ActivationReason );
  void quickpostFromTemplate( int, QString, QString t = QString() );
  void setNewWindowAtStartup( bool );
  void handleDone( QNetworkReply * );
  void abortQP();
  void openRecentFileFromDialog( QListWidgetItem * );
  void actOnChooseQuickpostTemplate( QListWidgetItem * );

signals:
  void quickpostTemplateTitlesUpdated( QStringList );
  void quickpostTemplatesUpdated( QStringList );

private:
  Application *qtm;
  //enum _cbtextIsURL { No, Yes, Untested };
  //enum _cbtextIsURL cbtextIsURL;
  bool _copyTitle;
  int activeTemplate;
  QAction *newWindowAtStartup;
  QAction *abortAction;
  QAction *configureTemplates;
#ifndef SUPERMENU
  QAction *quitAction;
#endif
  QList<QuickpostTemplate *> quickpostTemplateActions;
  QMenu *menu;
  QMenu *templateMenu;
  bool _newWindowAtStartup;
  QNetworkAccessManager *netmgr;
  QNetworkReply *currentReply;
  QByteArray responseData;
  QString cbtext;
  // bool cbtextIsURL;
  int doubleClickFunction;
  bool httpBusy;
  bool templateQPActive;
  bool _dontStart;
  QStringList templateTitleList, templateList;
  QList<int> defaultPublishStatusList;
  QList<bool> copyTitleStatusList;
  QList<QStringList> assocHostLists;
  QList<Application::recentFile> recentFiles;
  QAction *recentFileActions[10];
  QAction *openRecent;
  QAction *noRecentFilesAction;
  QMenu *recentFilesMenu;
  QByteArray userAgentString;
#ifndef DONT_USE_DBUS
  //QDBusConnection *dbus;
#endif

  void setupQuickpostTemplates();
  void doQP( QString );
  QDomElement templateElement( QDomDocument &, QString &, QString &, 
			       int &, bool &, QStringList & );
  void updateRecentFileMenu();
};

//#endif
#endif
