// Highlighter.cc - Syntax highlighter for HTML elements in QTM documents
//
// Copyright 2008 Matthew J Smith
// Distributed under the GNU General Public License, version 2 or later

#include <QtCore>
#include <QtGui>

#include "Highlighter.h"

Highlighter::Highlighter( QTextDocument *document, QObject *parent )
  : QSyntaxHighlighter( parent )
{
  QTextCharFormat format;

  // Formatting for HTML tags
  format.setForeground( Qt::darkGreen );

  m_formats[Tag] = format;

  // Formatting for numerical HTML entities
  format.setForeground( Qt::darkBlue );

  m_formats[Entity] = format;

  // Formatting for other HTML entities
  format.setForeground( Qt::darkCyan );

  m_formats[Comment] = format;
}

void Highlighter::highlightBlock( const QString &text )
{
  QString entityTerminators = " ;";
  int state = previousBlockState();
  int len = text.length();
  int start = 0;
  int pos = 0;

  while( pos < len ) {
    switch( state ) {
      case NormalState:
      default:
        while( pos < len) {
          QChar ch = text.at(pos);
          if( ch == '<') {
            if( text.mid(pos, 4) == "<!--") {
              state = InComment;
            } else {
              state = InTag;
            }
            break;
          } else if( ch == '&') {
            start = pos;
            //while( pos < len && text.at(pos++) != ';' )
            while( pos < len && !entityTerminators.contains( text.at( pos++ ) ) )
              ;
            setFormat(start, pos - start, m_formats[Entity]);
          } else {
            ++pos;
          }
        }
        break;

      case InComment:
        start = pos;
        while( pos < len) {
          if( text.mid( pos, 3 ) == "-->" ) {
            pos += 3;
            state = NormalState;
            break;
          } 
          else {
            ++pos;
          }
        }
        setFormat(start, pos - start, m_formats[Comment] );
        break;

      case InTag:
        QChar quote = QChar::Null;
        start = pos;
        while( pos < len ) {
          QChar ch = text.at( pos );
          if( quote.isNull() ) {
            if( ch == '\'' || ch == '"' ) {
              quote = ch;
            } else if( ch == '>' ) {
              ++pos;
              state = NormalState;
              break;
            }
          } else if( ch == quote ) {
            quote = QChar::Null;
          }
          ++pos;
        }
        setFormat( start, pos - start, m_formats[Tag] );
    }
  }
  setCurrentBlockState( state );
}

void Highlighter::setFormatFor( Construct construct, const QTextCharFormat &format )
{
  m_formats[construct] = format;
  rehighlight();
}
