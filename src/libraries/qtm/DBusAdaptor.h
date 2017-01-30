/***************************************************************************
 * DBusAdaptor.h - Header file for QTM's D-Bus interface
 *
 * Copyright (C) 2008, Matthew J Smith
 *
 * This file is part of QTM.
 * QTM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (version 2), as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************************/


#include <QtDBus>
#include <QtCore>

#include "SysTrayIcon.h"

class DBusAdaptor : public QDBusAbstractAdaptor
{
Q_OBJECT
Q_CLASSINFO( "D-Bus Interface", "uk.co.blogistan.catkin" )
  Q_PROPERTY( QString organizationName READ organizationName )
  Q_PROPERTY( QString organizationDomain READ organizationDomain )
  Q_PROPERTY( QString applicationVersion READ applicationVersion )

  public:
  DBusAdaptor( SysTrayIcon *sti );
  QString organizationName() { return QCoreApplication::organizationName(); }
  QString organizationDomain() { return QCoreApplication::organizationDomain(); }
  QString applicationVersion();

public slots:
  Q_NOREPLY void quit();
  Q_NOREPLY void saveAll();
  QStringList getQuickpostTemplates();
  QStringList getQuickpostTemplateTitles();
  Q_NOREPLY void newDocument();
  Q_NOREPLY void newDocumentWithTitleAndText( QString, QString );
  Q_NOREPLY void quickpost( QString, QString );
  Q_NOREPLY void quickpostFromLauncher();
  Q_NOREPLY void choose();
  Q_NOREPLY void chooseRecentFile();
  Q_NOREPLY void chooseQuickpostTemplate();
  bool open( QString );

signals:
  void aboutToQuit();
  void allSaved();
  void quickpostTemplatesUpdated( QStringList );
  void quickpostTemplateTitlesUpdated( QStringList );

private:
  SysTrayIcon *_sti;
};

