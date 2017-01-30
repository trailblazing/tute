/***************************************************************************
 * QuickpostTemplate.h - Header for QTM quickpost template objects
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


#include <QAction>

class QuickpostTemplate : public QAction
{
Q_OBJECT

 public:
  QuickpostTemplate( int, QString, QString, QObject *parent = 0 );
  ~QuickpostTemplate();

  void setIdentifier( int );
  void setPostTemplate( QString );
  int identifier() { return _identifier; }
  QString postTemplate() { return _postTemplate; }

 signals:
  void quickpostRequested( int, QString );

 private:
  int _identifier;
  QString _postTemplate;

 private slots:
  void emitRequestedSignal();
};
