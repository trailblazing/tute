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
 ********************************************************************************/

#include <QIcon>
#include <QKeySequence>
#include <QSize>
#include <QTextEdit>
#include <QLineEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QString>
#include <QRegExp>
#include <QPalette>
#include <QKeyEvent>

#if QT_VERSION >=0x040400
#include <QPlainTextEdit>
#endif

#include "QijSearchWidget.h"

  QijSearchWidget::QijSearchWidget( QTextEdit *te, QWidget *parent )
: QWidget( parent )
{
  setupUi( this );
  tbClose->setIcon( QIcon( ":/images/close.png" ) );
  tbClose->setIconSize( QSize( 20, 20 ) );
  tbSearchUp->setIcon( QIcon( ":/images/previous.png" ) );
  tbSearchUp->setIconSize( QSize( 20, 20 ) );
  tbSearchDown->setIcon( QIcon( ":/images/next.png" ) );
  tbSearchDown->setIconSize( QSize( 20, 20 ) );

  // actionExit_find->setShortcut( QKeySequence( Qt::Key_Escape ) );

  _textEdit = te;
  _plainTextEdit = 0;
  dir = Forward;
  // leFindText->setText( QString() );

  tbSelectAll->hide(); // Will be implemented later

  // RegExp searching disabled on Qt 4.1 as it's not supported by Qt
#if QT_VERSION < 0x040200
  chRegExp->hide();
  chRegExp->setChecked( Qt::Unchecked );
#endif

  leFindText->installEventFilter( this );
}

QijSearchWidget::QijSearchWidget( QPlainTextEdit *te, QWidget *parent )
: QWidget( parent )
{
#if QT_VERSION >= 0x040400
  setupUi( this );
  tbClose->setIcon( QIcon( ":/images/close.png" ) );
  tbClose->setIconSize( QSize( 20, 20 ) );
  tbSearchUp->setIcon( QIcon( ":/images/previous.png" ) );
  tbSearchUp->setIconSize( QSize( 20, 20 ) );
  tbSearchDown->setIcon( QIcon( ":/images/next.png" ) );
  tbSearchDown->setIconSize( QSize( 20, 20 ) );

  // actionExit_find->setShortcut( QKeySequence( Qt::Key_Escape ) );

  _textEdit = 0;
  _plainTextEdit = te;
  dir = Forward;
  // leFindText->setText( QString() );

  tbSelectAll->hide(); // Will be implemented later
#endif
}

void QijSearchWidget::setTextEdit( QWidget *te )
{
  QPlainTextEdit *pted;
  QTextEdit *ted = qobject_cast<QTextEdit *>( te );
  if( ted ) {
    _textEdit = ted;
    _plainTextEdit = 0;

    if( !leFindText->text().isEmpty() )
      findInTextEdit( leFindText->text(), Stay );
  }
  else {
    pted = qobject_cast<QPlainTextEdit *>( te );
    if( pted ) {
      _plainTextEdit = pted;
      _textEdit = 0;

      if( !leFindText->text().isEmpty() )
        findInTextEdit( leFindText->text(), Stay );
    }
    else {
      qDebug( "This widget is not a QTextEdit or a QPlainTextEdit" );
    }
  }
}

/*
void QijSearchWidget::setTextEdit( QTextEdit *te )
{
  _textEdit = te;
  _plainTextEdit = 0;

  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), Stay );
}

void QijSearchWidget::setTextEdit( QPlainTextEdit *te )
{
#if QT_VERSION >= 0x040400
  _plainTextEdit = te;
  _textEdit = 0;

  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), Stay );
#endif
}
*/

void QijSearchWidget::setExpertEnabled( bool a )
{
#if QT_VERSION >= 0x040200
  chRegExp->setVisible( a );

  if( !a )
    chRegExp->setChecked( false );
#endif
}

void QijSearchWidget::clearSearchText()
{
  leFindText->clear();
}

void QijSearchWidget::find()
{
  //qDebug( "find" );
  if( !leFindText->text().length() )
    leFindText->selectAll();

  show();
  leFindText->setFocus( Qt::MenuBarFocusReason );
}

void QijSearchWidget::findAgain()
{
  show();

  if( leFindText->text().isEmpty() ) {
    leFindText->setFocus( Qt::MenuBarFocusReason );
  }
  else {
    findInTextEdit( leFindText->text(), dir );
  }
}

void QijSearchWidget::findInTextEdit( const QString &text, direction currentDirection )
{
  QTextDocument *doc;
  QTextCursor c;

  //qDebug( "starting find" );
  // This method has been adopted from the method "find" in the TabbedBrowser class
  // in Qt Assistant.

#if QT_VERSION < 0x040400
  doc = _textEdit->document();
  c = _textEdit->textCursor();
#else
  if( _textEdit ) {
    doc = _textEdit->document();
    c = _textEdit->textCursor();
  }
  else {
    doc = _plainTextEdit->document();
    c = _plainTextEdit->textCursor();
  }
#endif

  QString oldText = leFindText->text();
  QTextDocument::FindFlags options;
  QPalette p = leFindText->palette();
  p.setColor( QPalette::Active, QPalette::Base, Qt::white );

  if( c.hasSelection() )
    c.setPosition( (currentDirection == Forward) ?
                   //	  c.setPosition( ((dir == Forward) && !stay) ? 
      c.position() : c.anchor(), QTextCursor::MoveAnchor );

                   QTextCursor newCursor = c;

                   if( !text.isEmpty() ) {
                   if( currentDirection == Backward ) 
                   options |= QTextDocument::FindBackward;

                   if ( chMatchCase->isChecked() )
                   options |= QTextDocument::FindCaseSensitively;

                   if ( chWholeWords->isChecked() )
                   options |= QTextDocument::FindWholeWords;

#if QT_VERSION >= 0x040200	  
                   if( chRegExp->isChecked() )
                   newCursor = doc->find( QRegExp( text, chMatchCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive ), c );
                   else
#endif
                     newCursor = doc->find( text, c, options );
                   //ui.labelWrapped->hide();
                   currentCursorText = newCursor.selectedText();

                   if (newCursor.isNull()) {
                     //qDebug( "null cursor" );
                     QTextCursor ac(doc);
                     ac.movePosition(options & QTextDocument::FindBackward
                                     ? QTextCursor::End : QTextCursor::Start);
#if QT_VERSION >= 0x040200
                     if( chRegExp->isChecked() )
                       newCursor = doc->find( QRegExp( text ), ac, options );
                     else
#endif
                       newCursor = doc->find( text, ac, options );
                     if (newCursor.isNull()) {
                       p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
                       newCursor = c;
                     }
                     else
                       emit searchMessage( tr( "Wrapped the search" ) );
                   }
                   }

                   if( !isVisible() )
                     show();
#if QT_VERSION < 0x040400
                   _textEdit->setTextCursor(newCursor);
#else
                   if( _textEdit )
                     _textEdit->setTextCursor(newCursor);
                   else
                     _plainTextEdit->setTextCursor( newCursor );
#endif
                   leFindText->setPalette(p);
                   /*if( !leFindText->hasFocus() )
                     autoHideTimer->start(); */
}

void QijSearchWidget::on_tbClose_clicked()
{
  hide();
}

void QijSearchWidget::on_leFindText_textChanged( const QString &newText )
{
  if( newText.isEmpty() ) {
    tbSearchDown->setEnabled( false );
    tbSearchUp->setEnabled( false );
    tbSelectAll->setEnabled( false );
  }
  else {
    tbSearchDown->setEnabled( true );
    tbSearchUp->setEnabled( true );
    tbSelectAll->setEnabled( true );
  }

  if( (newText != currentCursorText) && !chRegExp->isChecked() ) {
    dir = Forward;
    findInTextEdit( newText, Stay );
  }
}

void QijSearchWidget::on_leFindText_returnPressed()
{
  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), dir );
}

void QijSearchWidget::on_tbSearchDown_clicked( bool )
{
  dir = Forward;
  findInTextEdit( leFindText->text(), Forward );
}

void QijSearchWidget::on_tbSearchUp_clicked( bool )
{
  dir = Backward;
  findInTextEdit( leFindText->text(), Backward );
}

void QijSearchWidget::on_tbSelectAll_clicked( bool )
{

}

void QijSearchWidget::on_chMatchCase_toggled( bool state )
{
  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), Stay );
}

void QijSearchWidget::on_chWholeWords_toggled( bool state )
{
  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), Stay );
}

void QijSearchWidget::on_chRegExp_toggled( bool state )
{
#if QT_VERSION >= 0x040200
  if( !leFindText->text().isEmpty() )
    findInTextEdit( leFindText->text(), Stay );
#endif
}

bool QijSearchWidget::eventFilter( QObject *object, QEvent *event )
{
  if( object == leFindText ) {
    if( event->type() == QEvent::KeyPress ) {
      QKeyEvent *ke = static_cast<QKeyEvent *>( event );
      if( ke->key() == Qt::Key_Escape ) {
        hide();
        return true;
      }
    }
  }

  return QObject::eventFilter( object, event );
}

void QijSearchWidget::closeEvent( QCloseEvent *event )
{
  leFindText->clear();
  QTextCursor c = _textEdit->textCursor();
  c.clearSelection();
  _textEdit->setTextCursor( c );

  event->accept();
}


