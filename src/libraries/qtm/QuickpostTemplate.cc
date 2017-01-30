/***************************************************************************
 * QuickpostTemplate.cc
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


#include <QObject>
#include <QAction>
#include <QString>

#include "QuickpostTemplate.h"

QuickpostTemplate::QuickpostTemplate( int id, QString t,
                                      QString pt, QObject *parent )
  : QAction( t, parent )
{
  _postTemplate = pt;
  _identifier = id;

  connect( this, SIGNAL( triggered( bool ) ),
	   this, SLOT( emitRequestedSignal() ) );
}

QuickpostTemplate::~QuickpostTemplate()
{
  deleteLater();
}


void QuickpostTemplate::setIdentifier( int i )
{
  _identifier = i;
}

void QuickpostTemplate::setPostTemplate( QString newTemplate )
{
  _postTemplate = newTemplate;
}

void QuickpostTemplate::emitRequestedSignal()
{
  emit quickpostRequested( _identifier, _postTemplate );
}

/* inline int QuickpostTemplate::identifier()
{
  return _identifier;
}

QString QuickpostTemplate::postTemplate()
{
  return _postTemplate;
} */

