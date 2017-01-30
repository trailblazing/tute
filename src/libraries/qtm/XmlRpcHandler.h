/*******************************************************************************
 
    XmlRpcHandler.h - Header for QTM's SAX parser for XML-RPC returns
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

// XmlRpcHandler.h - Header for QTM XML-RPC routines

#ifndef XMLRPCHANDLER_H
#define XMLRPCHANDLER_H

#include <QtXml>
#include <QtCore>

#include "EditingWindow.h"

class XmlRpcHandler : public QXmlDefaultHandler
{
public:
  XmlRpcHandler( EditingWindow::HttpBusinessType x );
  XmlRpcHandler();

  void setProtocol( EditingWindow::HttpBusinessType x );

  bool startElement( const QString &namespaceURI,
		     const QString &localName,
		     const QString &qName,
		     const QXmlAttributes &attr );
  bool endElement( const QString &nu,
		   const QString &localName,
		   const QString &qName );
  bool characters( const QString &str );
  bool fatalError( const QXmlParseException &exc );
  bool isMethodResponseFinished();
  QStringList returnList( const QString & );
  // QString returnFirstEntry();
  bool fault();
  QString faultString();
  QDomDocumentFragment returnXml();

private:
  EditingWindow::HttpBusinessType reqType;
  QString currentString;
  QString currentRpcArgumentName;
  QString _faultString;
  QDomDocument doc;
  QDomElement returnElement, currentSuperElement, currentElement;
  // QList<QString> returnData;
  QMap<QString, QStringList> returnDataList;
  bool _fault;
  bool methodResponseFinished;
  bool receivingArgumentName, insideStruct, receivingData, 
    receivingFaultString;
};

#endif
