/***************************************************************************
 * locationlineedit.h - Specialised line-editor for XML-RPC end points
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


#include <QLineEdit>

class QWidget;
class QContextMenuEvent;

class LocationLineEdit : public QLineEdit
{
Q_OBJECT

public:
  LocationLineEdit( QWidget *parent = 0 );

 protected:
  virtual void contextMenuEvent( QContextMenuEvent *event );

private slots:
  void addMT();
  void addWordpress();
  void addTextpattern();
};
