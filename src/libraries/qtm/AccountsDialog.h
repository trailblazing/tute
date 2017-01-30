/***************************************************************************
 * AccountsDialog.h - Multi-account form for QTM
 *
 * Copyright (C) 2008-2010, Matthew J Smith
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

#ifndef ACCOUNTSDIALOG_H
#define ACCOUNTSDIALOG_H

#include "ui_AccountsForm.h"

#include <QStringList> // also includes QString
#include <QByteArray>
#include <QList>
#include <QDialog>

#include <QtNetwork>

class QDialog;
class QWidget;
class QAction;
class QNetworkAccessManager;
class QNetworkReply;
class QEvent;

class AccountsDialog : public QDialog, public Ui::AccountsForm
{
  Q_OBJECT

public:
  typedef struct _acct {
    QString id;
    int hostedBlogType;
    QString name;
    QString endpoint;
/*    QString server;
    QString location;
    QString port; */
    QString login;
    QString password;
    // bool useHTTPS;
    bool categoriesEnabled;
    bool postDateTime;
    bool allowComments;
    bool allowTB;
    bool useWordpressAPI;
  } Account;

  AccountsDialog( QList<AccountsDialog::Account> &, int,
		  QWidget *parent = 0 );
  QList<Account> accounts() { return accountList; }
  /*QStringList templateTitles() { return _templateTitles; }
  QStringList templateStrings() { return _templateStrings; }
  QList<int> defaultPublishStates() { return _defaultPublishStates; }
  QList<bool> copyTitleStates() { return _copyTitleStates; }
  QList<QStringList> compileAssocHostLists( QStringList & );
  QStringList compileAssocHostStrings( QList<QStringList> & );
  QList<QStringList> assocHostLists(); */


private:
  void acceptAccount();
  QList<Account> accountList, originalAccountList;
  Account currentAcct;
  QList<QWidget *> accountWidgets;
  QList<QWidget *> boolWidgets;
  bool doingNewAccount;
  bool dirty;
  int currentRow;
  QString currentAccountId;
  QAction *addAccount;
  QAction *removeAccount;
  Account currentAccount;
  QDateTime entryDateTime;
  QNetworkAccessManager *netmgr;
  QNetworkReply *currentReply;
  QByteArray httpByteArray;
  QString currentHost;
  typedef enum _biz {
    NoBusiness,
    FindingRsdXml,
    FindingXmlrpcPhp
  } HttpBusiness;
  HttpBusiness networkBiz;

private slots:
  void changeListIndex( int );
  void doNewAccount();
  void setDirty();
  void setClean();
  void assignSlug();
  void on_leBlogURI_returnPressed();
  // void handleResponseHeader( int, bool );
  void handleHttpDone( QNetworkReply * );
  void on_pbNew_clicked() { doNewAccount(); }
  void removeThisAccount();
  // void on_pbAccept_clicked() { acceptTemplate(); }
  void on_pbWhatsThis_clicked();
  void on_pbOK_clicked();
  void on_pbCancel_clicked() { reject(); }

  // Handle changes to account variables
  void on_leName_textEdited( const QString & );
  void on_leEndpoint_textEdited( const QString & );
  void on_leLogin_textEdited( const QString & );
  void on_lePassword_textEdited( const QString & );
  void on_cbHostedBlogType_activated( int );
  //void on_chUseHTTPS_clicked( bool );
  void on_chCategoriesEnabled_clicked( bool );
  void on_chPostDateTime_clicked( bool );
  void on_chAllowComments_clicked( bool );
  void on_chAllowTB_clicked( bool );
  void on_chUseWordpressAPI_clicked( bool );

  //void handleRequestFinished( int, bool );

 protected:
  virtual bool eventFilter( QObject *, QEvent * );
};

#endif
