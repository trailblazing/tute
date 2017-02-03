/***************************************************************************
 * locationlineedit.cc - Specialised line editor for XML-RPC end-points
 *
 * Copyright (C) 2007, 2008 Matthew J Smith
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
#include <QWidget>
#include <QMenu>
#include <QContextMenuEvent>

#include "locationlineedit.h"

LocationLineEdit::LocationLineEdit( QWidget *parent )
  : QLineEdit( parent )
{
}

void LocationLineEdit::contextMenuEvent( QContextMenuEvent *event )
{
  QMenu *menu = new QMenu( this );
  menu->addAction( "mt-xmlrpc.cgi (Movable Type)", this,
		   SLOT( addMT() ) );
  menu->addAction( "xmlrpc.php (WordPress)", this,
		   SLOT( addWordpress() ) );
  menu->addAction( "/textpattern/xmlrpcs.php (TextPattern)", this,
		   SLOT( addTextpattern() ) );
  menu->addSeparator();
  QMenu *cm = createStandardContextMenu();
  cm->setTitle( tr( "Editing" ) );
  menu->addMenu( cm );
  menu->exec( event->globalPos() );
}

void LocationLineEdit::addMT()
{
  insert( "mt-xmlrpc.cgi" );
}

void LocationLineEdit::addWordpress()
{
  insert( "xmlrpc.php" );
}

void LocationLineEdit::addTextpattern()
{
  insert( "/textpattern/xmlrpcs.php" );
}
