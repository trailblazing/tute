/*******************************************************************************
 *
 * Foxhound Search Widget - A Qt document search widget based on the
 * Qt Designer search widget
 *
 * This program is free software; you can redistribute it and/or modify
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
 * Written by Matthew J Smith , 12th Feb 2008
 *
 *******************************************************************************/

#ifndef QIJSEARCHWIDGET_H
#define QIJSEARCHWIDGET_H

#include "ui_QijSearchWidget.h"

class QTextEdit;
class QObject;
class QEvent;

#if QT_VERSION >= 0x040400
class QPlainTextEdit;
#else
#define QPlainTextEdit void
#endif

class QijSearchWidget : public QWidget, private Ui::SearchWidgetBase
{
Q_OBJECT

public:
  QijSearchWidget( QTextEdit *, QWidget *parent = 0 );
  QijSearchWidget( QPlainTextEdit *, QWidget *parent = 0 );
  QPlainTextEdit * plainTextEdit() { return _plainTextEdit; }
  QTextEdit * textEdit() { return _textEdit; }
  bool expertEnabled() { return _expertEnabled; }

public slots:
  void setTextEdit( QWidget * );
/*  void setTextEdit( QTextEdit * );
  void setTextEdit( QPlainTextEdit * ); */
  void find();
  void findAgain();
  void setExpertEnabled( bool );
  void clearSearchText();

protected:
  bool eventFilter( QObject *, QEvent * );
  virtual void closeEvent( QCloseEvent * );

private:
  enum direction { Forward, Backward, Stay };
  void findInTextEdit( const QString &, direction d = Stay );

  bool matchCase, wholeWords, isRegexSearch;
  bool _expertEnabled;
  direction dir;
  QTextEdit *_textEdit;
  QPlainTextEdit *_plainTextEdit;
  QString currentCursorText;

private slots:
  void on_tbClose_clicked();
  void on_leFindText_textChanged( const QString & );
  void on_leFindText_returnPressed();
  void on_tbSearchDown_clicked( bool );
  void on_tbSearchUp_clicked( bool );
  void on_tbSelectAll_clicked( bool );
  void on_chMatchCase_toggled( bool );
  void on_chWholeWords_toggled( bool );
  void on_chRegExp_toggled( bool );

 signals:
  void searchMessage( const QString & );
};

#endif
