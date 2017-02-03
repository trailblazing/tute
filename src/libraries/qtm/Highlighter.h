// Highlighter.h - Syntax highlighter for HTML tags in QTM documents
//
// Written by Matthew J Smith
//
// Copyright 2008 Matthew J Smith
// Distributed under GNU General Public License, version 2 or later

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextCharFormat>

class Highlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
  enum Construct {
    Entity, Tag, Comment,
    LastConstruct = Comment
  };

  Highlighter( QTextDocument *document = 0, QObject *parent = 0 );
  void setFormatFor( Construct construct, const QTextCharFormat &format );
  QTextCharFormat formatFor( Construct construct ) const
  { return m_formats[construct]; }

protected:
  enum State {
    NormalState = -1,
    InComment, InTag
  };

  virtual void highlightBlock( const QString & );

private:
  QMap<Construct, QTextCharFormat> m_formats;

};


#endif
