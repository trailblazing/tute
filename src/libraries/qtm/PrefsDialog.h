/***************************************************************************
 * PrefsDialog.h - Header file for QTM's preferences dialog
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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QtGlobal>

class QDialog;
class QWidget;
class QHostInfo;

#include "ui_PrefsForm.h"

class PrefsDialog : public QDialog, public Ui::PrefsForm
{
Q_OBJECT

public:
  PrefsDialog( QWidget *parent = 0 );
  // bool noValidHost();
  void setLabelText( QLabel *, bool, bool );

private slots:
  void on_pbBrowse_clicked();
  void on_pbWhatsThis_clicked();
  void on_tbEditorBgColor_clicked();
  void on_tbEditorFgColor_clicked();
  void on_tbPreviewBgColor_clicked();
  void on_tbPreviewFgColor_clicked();
  void on_tbConsoleBgColor_clicked();
  void on_tbConsoleFgColor_clicked();
  void on_pbTagColorFore_clicked();
  void on_pbTagColorBack_clicked();
  void on_pbEntityColorFore_clicked();
  void on_pbEntityColorBack_clicked();
  void on_pbCommentColorFore_clicked();
  void on_pbCommentColorBack_clicked();
  void on_chTagBold_toggled( bool );
  void on_chTagItalic_toggled( bool );
  void on_chEntityBold_toggled( bool );
  void on_chEntityItalic_toggled( bool );
  void on_chCommentBold_toggled( bool );
  void on_chCommentItalic_toggled( bool );
  //void on_okButton_clicked();
  //void handleHostInfo( const QHostInfo & );

};

#endif
