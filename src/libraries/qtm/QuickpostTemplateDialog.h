/***************************************************************************
 * QuickpostTemplateDialog.h - Header for QTM quickpost template dialog
 *
 * Copyright (C) 2007, 2008, Matthew J Smith
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


#include <QtGlobal>
#ifndef QUICKPOSTTEMPLATEDIALOG_H
#define QUICKPOSTTEMPLATEDIALOG_H

#include "ui_QuickpostTemplateForm.h"

#include <QStringList>
#include <QList>

class QDialog;
class QWidget;
class QAction;

class QuickpostTemplateDialog : public QDialog, public Ui::QuickpostTemplateForm
{
  Q_OBJECT

public:
  QuickpostTemplateDialog( QStringList &, QStringList &, 
			   QList<int> &, QList<bool> &,  QList<QStringList> &,
			   bool, QWidget *parent = 0 );
  QStringList templateTitles() { return _templateTitles; }
  QStringList templateStrings() { return _templateStrings; }
  QList<int> defaultPublishStates() { return _defaultPublishStates; }
  QList<bool> copyTitleStates() { return _copyTitleStates; }
  QList<QStringList> compileAssocHostLists( QStringList & );
  QStringList compileAssocHostStrings( QList<QStringList> & );
  QList<QStringList> assocHostLists();

private:
  void acceptTemplate();
  QStringList _templateTitles, originalTitles;
  QStringList _templateStrings, originalStrings;
  QList<int> _defaultPublishStates, originalPublishStates;
  QList<bool> _copyTitleStates, originalCopyTitleStates;
  QList<QStringList> _assocHostLists, originalAssocHostLists;
  QStringList _assocHostStrings;
  // QStringListModel *titlesModel;
  bool doingNewTemplate;
  bool dirty;
  // QModelIndex currentListIndex;
  int currentRow;
  // int lowestNumber; // lowest number of templates in the list in the session
  QAction *addTemplate; 
  QAction *removeTemplate;
  QAction *insertTabIntoTemplate;
  QString currentTemplateTitle, currentTemplateString;
  bool defaultCPStatus;

private slots:
  void changeListIndex( int );
  void doNewTemplate();
  void setDirty();
  void setClean();
  void on_pbNewTemplate_clicked() { doNewTemplate(); }
  void removeThisTemplate();
  void doInsertTabIntoTemplate();
  // void on_pbAccept_clicked() { acceptTemplate(); }
  void on_pbWhatsThis_clicked();
  // void on_pbReset_clicked();
  // void on_buttonBox_accepted();
  void on_pbOK_clicked();
  void on_pbCancel_clicked() { reject(); }
  // void on_lwTemplates_clicked( const QModelIndex & );
  void on_leName_textEdited( const QString & );
  void on_teTemplateText_textChanged();
  void on_teAssocHosts_textChanged();
  void on_cbDefaultPublishStatus_currentIndexChanged( int );
  void on_chCopyTitle_stateChanged( int );
};

#endif
