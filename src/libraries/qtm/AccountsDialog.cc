/************************************************************************************************
 *
 * AccountsDialog.cc - Multi-account form for QTM
 *
 * Copyright (C) 2007 - 2010, Matthew J Smith
 *
 * This file is part of QTM.
 * QTM program is free software; you can redistribute it and/or modify
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
 *************************************************************************************************/


#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QList>
#include <QWhatsThis>
#include <QMessageBox>
#include <QModelIndex>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTime>
#include <QFocusEvent>
#if QT_VERSION <= 0x040300
#include <QTextDocument>
#endif

#include <QtXml>

#include "AccountsDialog.h"
#include "ui_AccountsForm.h"
#include "return.xpm"

AccountsDialog::AccountsDialog( QList<AccountsDialog::Account> &acctList,
                                int acctIndex,
                                QWidget *parent )
: QDialog( parent )
{
  QString a;
  networkBiz = NoBusiness;
  netmgr = new QNetworkAccessManager;

  setupUi( this );
  leBlogURI->installEventFilter( this );
  //leLocation->installEventFilter( this );
  pbNew->setDefault( false );

  // Set up internal account lists; one for the current contents of the accounts,
  // one for backup
  accountList = acctList;
  originalAccountList = acctList;

  hboxLayout->setStretchFactor( hboxLayout->itemAt( 0 )->layout(), 2 );
  hboxLayout->setStretchFactor( hboxLayout->itemAt( 1 )->layout(), 3 );

  for( int i = 0; i < accountList.count(); i++ ) {
    a = accountList.at( i ).name;
    if( a.isEmpty() )
      lwAccountList->addItem( tr( "(No name)" ) );
    else
      lwAccountList->addItem( a );
  }
  //qDebug( "added the templates" );

  // Set up list of account widgets;
  accountWidgets << leName << cbHostedBlogType << leEndpoint
    << leLogin << lePassword
    << chCategoriesEnabled << chPostDateTime << chAllowComments << chAllowTB << chUseWordpressAPI;

  pbReset->setVisible( false );

  Q_FOREACH( QWidget *w, accountWidgets )
    w->setEnabled( false );

  doingNewAccount = false;
  // setClean();
  currentRow = -1;

  addAccount = new QAction( tr( "&Add account" ), lwAccountList );
  removeAccount = new QAction( tr( "&Remove this account" ), lwAccountList );
  lwAccountList->addAction( addAccount );
  lwAccountList->addAction( removeAccount );
  lwAccountList->setContextMenuPolicy( Qt::ActionsContextMenu );

  connect( addAccount, SIGNAL( triggered( bool ) ),
           this, SLOT( doNewAccount() ) );
  connect( removeAccount, SIGNAL( triggered( bool ) ),
           this, SLOT( removeThisAccount() ) );
  connect( lwAccountList, SIGNAL( currentRowChanged( int ) ),
           this, SLOT( changeListIndex( int ) ) );

  entryDateTime = QDateTime();
  currentRow = acctIndex;
  lwAccountList->setCurrentRow( acctIndex );
}

void AccountsDialog::changeListIndex( int index )
{
  leBlogURI->clear();

  if( doingNewAccount ) {
    if( dirty )
      lwAccountList->item( (lwAccountList->count())-1 )->setText( leName->text().isEmpty() ?
                                                                  tr( "(No name)" ) :
                                                                  leName->text() );
    else {
      accountList.removeAt( currentRow );
      lwAccountList->takeItem( lwAccountList->count()-1 );
    }
  }

  doingNewAccount = false;
  currentRow = index;

  leName->setText( accountList[currentRow].name );
  leEndpoint->setText( accountList[currentRow].endpoint );
  leLogin->setText( accountList[currentRow].login );
  lePassword->setText( accountList[currentRow].password );

  if( accountList[currentRow].hostedBlogType != -1 )
    cbHostedBlogType->setCurrentIndex( accountList[currentRow].hostedBlogType );	  

  chCategoriesEnabled->setCheckState( accountList[currentRow].categoriesEnabled ?
                                      Qt::Checked : Qt::Unchecked );
  chPostDateTime->setCheckState( accountList[currentRow].postDateTime ?
                                 Qt::Checked : Qt::Unchecked );
  chAllowComments->setCheckState( accountList[currentRow].allowComments ? Qt::Checked :
                             Qt::Unchecked );
  chAllowTB->setCheckState( accountList[currentRow].allowTB ? Qt::Checked : Qt::Unchecked );
  chUseWordpressAPI->setCheckState( accountList[currentRow].useWordpressAPI ?
                                    Qt::Checked : Qt::Unchecked );

  Q_FOREACH( QWidget *w, accountWidgets )
    w->setEnabled( true );
}

void AccountsDialog::doNewAccount()
{
  QLineEdit *le;

  entryDateTime = QDateTime::currentDateTime();
  Q_FOREACH( QWidget *w, accountWidgets )
    w->setEnabled( true );

  accountList.append( Account() );

  lwAccountList->disconnect( SIGNAL( currentRowChanged( int ) ),
                             this, SLOT( changeListIndex( int ) ) );
  currentRow = -1;
  Q_FOREACH( QWidget *v, accountWidgets ) {
    le = qobject_cast<QLineEdit *>( v );
    if( le )
      le->clear();
  }
  leBlogURI->clear();

  lwAccountList->addItem( tr( "New account" ) );
  lwAccountList->setCurrentRow( lwAccountList->count()-1 );
  currentRow = lwAccountList->count()-1;
  connect( lwAccountList, SIGNAL( currentRowChanged( int ) ),
           this, SLOT( changeListIndex( int ) ) );
  //qDebug() << "current row:" << currentRow;

  accountList[currentRow].categoriesEnabled = false;
  chCategoriesEnabled->setChecked( false );
  accountList[currentRow].postDateTime = false;
  chPostDateTime->setChecked( false );
  accountList[currentRow].allowComments = false;
  chAllowComments->setChecked( false );
  accountList[currentRow].allowTB = false;
  chAllowTB->setChecked( false );
  accountList[currentRow].useWordpressAPI = false;
  chUseWordpressAPI->setChecked( false );

  doingNewAccount = true;
  leName->setFocus( Qt::OtherFocusReason );
  connect( qApp, SIGNAL( focusChanged( QWidget *, QWidget * ) ),
           this, SLOT( assignSlug() ) );

  setClean();
}

void AccountsDialog::removeThisAccount()
{
  QLineEdit *le;
  QCheckBox *ch;
  int c = lwAccountList->currentRow();

  if( lwAccountList->count() == 1 ) {
    lwAccountList->disconnect( SIGNAL( currentRowChanged( int ) ) );
    lwAccountList->clear();
    accountList.clear();

    Q_FOREACH( QWidget *w, accountWidgets ) {
      le = qobject_cast<QLineEdit *>( w );
      if( le ) {
        le->clear();
        le->setEnabled( false );
      }
      ch = qobject_cast<QCheckBox *>( w );
      if( ch ) {
        ch->setChecked( false );
        ch->setEnabled( false );
      }
    }
    currentRow = -1;
  } else {
    lwAccountList->takeItem( c );
    accountList.removeAt( c );
  }
}

void AccountsDialog::setDirty()
{
  dirty = true;
  Q_FOREACH( QWidget *w, accountWidgets )
    disconnect( w, 0, this, SLOT( setDirty() ) );
}

void AccountsDialog::setClean()
{
  dirty = false;

  Q_FOREACH( QWidget *w, accountWidgets )
    disconnect( w, 0, this, SLOT( setDirty() ) );

  Q_FOREACH( QWidget *v, accountWidgets ) {
    if( qobject_cast<QLineEdit *>( v ) )
      connect( v, SIGNAL( textChanged( const QString & ) ),
               this, SLOT( setDirty() ) );
  }
}

void AccountsDialog::acceptAccount()
{
  Account newAcct;
  if( doingNewAccount ) {
    accountList.append( currentAcct );

    if( lwAccountList->count() == 1 )
      connect( lwAccountList, SIGNAL( currentRowChanged( int ) ),
               this, SLOT( changeListIndex( int ) ) );
  } else {
    if( lwAccountList->count() ) {
      lwAccountList->item( currentRow )->setText( leName->text().isEmpty() ?
                                                  tr( "(No name)" ) : leName->text() );
    }
  }

  doingNewAccount = false;
  setClean();
}

/* Private slot method to assign a unique slug name to identify an account.
 * It's done by the second, which should ensure uniqueness as long as identical
 * accounts aren't created by a program.
 */
void AccountsDialog::assignSlug()
{
  if( accountList.count() != 0 ) {
    currentAccountId = QString( "%1_%2" ).arg( entryDateTime.toString( Qt::ISODate ) )
      .arg( leName->text().toLower()
            .replace( QRegExp( "\\s" ), "_" ) );
    accountList[currentRow].id = currentAccountId;
  }
}

void AccountsDialog::on_pbWhatsThis_clicked()
{
  QWhatsThis::enterWhatsThisMode();
}

void AccountsDialog::on_pbOK_clicked()
{
  if( doingNewAccount && !dirty )
    accountList.removeAt( currentRow );

  accept();
}

void AccountsDialog::on_leBlogURI_returnPressed()
{
  int i;
  QString uriServer, urisLocation;
  QRegExp re( "/.*\\.[shtml|dhtml|phtml|html|htm|php|cgi|pl|py]$" );

  QUrl uri = QUrl( leBlogURI->text(), QUrl::TolerantMode );
  QString uris = uri.toString();

  if( !uri.isValid() ) {
    QMessageBox::information( 0, tr( "QTM: URI not valid" ),
                              tr( "That web location is not valid." ),
                              QMessageBox::Cancel );
    return;
  }

  //bool found = false;
  //  QStringList hostedAccountStrings, hostedAccountServers, hostedAccountLocations;
  QStringList wpmuHosts;
  wpmuHosts << "wordpress.com" << "blogsome.com" << "blogs.ie"
    << "hadithuna.com" << "edublogs.com" << "weblogs.us"
    << "blogvis.com" << "blogates.com";
  /*  hostedAccountEndpoints << "@host@;xmlrpc.php"
      << "www.typepad.com;/t/api"
      << "www.squarespace.com;/do/process/external/PostInterceptor"
      << "@host@;xmlrpc.php"
      << "@host@;xmlrpc.php"
      << "@host@;xmlrpc.php"; */

  // First check for TypePad and SquareSpace
  if( uris.contains( "squarespace.com" ) ) {
    leEndpoint->setText( "http://www.squarespace.com/do/process/external/PostInterceptor" );
    accountList[currentRow].endpoint = "http://www.squarespace.com/do/process/external/PostInterceptor";
    return;
  }

  if( uris.contains( "typepad.com" ) ) {
    leEndpoint->setText( "http://www.typepad.com/t/api" );
    accountList[currentRow].endpoint = "http://www.typepad.com/t/api";
    return;
  }

  // Now check if it's a Wordpress MU host
  for( i = 0; i <= wpmuHosts.count(); i++ ) {
    if( i < wpmuHosts.count() ) {
      if( uris.contains( wpmuHosts.at( i ) ) ) {
        // Wordpress.com is known to support HTTPS posting; others might not
        accountList[currentRow].endpoint = QString( "%1://%2/xmlrpc.php" )
          .arg( uris.contains( "wordpress.com" ) ? "https" : "http" )
          .arg( uri.host() );
        leEndpoint->setText( accountList[currentRow].endpoint );
        chUseWordpressAPI->setChecked( true );
        accountList[currentRow].useWordpressAPI = true;
        leLogin->setFocus( Qt::TabFocusReason );
        return;
      }
    }
  }

  // Is this a self-hosted Wordpress, Textpattern or Drupal site?
  if( cbHostedBlogType->currentIndex() >= 5 &&
      cbHostedBlogType->currentIndex() <= 7 ) {
    QString endpoint, ep;
    if( cbHostedBlogType->currentIndex() == 7 ) // i.e. Textpattern
      endpoint = "rpc/index.php";
    else
      endpoint = "xmlrpc.php";

    ep = uris.section( "//", 1, 1 ).section( "/", 0, 0 );

    urisLocation = uri.path();
    if( re.exactMatch( urisLocation ) )
      urisLocation = urisLocation.section( "/", 0, -2 ).append( endpoint );
    else
      urisLocation.append( urisLocation.endsWith( '/' ) ? endpoint : 
                           QString( "/%1" ).arg( endpoint ) );
    ep.append( urisLocation );
    ep.prepend( "http://" );
    leEndpoint->setText( ep );
    accountList[currentRow].endpoint = ep;
    return;
  }

  // Now test for an rsd.xml file 
  QUrl requestUrl( uri );
  QString loc( uri.path() );
  if( re.exactMatch( loc ) )
    loc = loc.section( "/", -2, 0, QString::SectionIncludeTrailingSep )
      .append( "rsd.xml" );
  else
    loc.append( loc.endsWith( '/' ) ? "rsd.xml" : "/rsd.xml" );
  requestUrl.setPath( loc );

  QNetworkRequest request;
  request.setUrl( requestUrl );
  netmgr->get( request );
/*  http->setHost( uri.host(),
                 uri.port() == -1 ? 80 : uri.port() );
  // http->setPort( uri.port() == -1 ? 80 : uri.port() );
  QString loc( uri.path() );
  if( re.exactMatch( loc ) )
    http->get( loc.section( "/", -2, 0, QString::SectionIncludeTrailingSep )
               .append( "rsd.xml" ) );
  else
    http->get( loc.append( loc.endsWith( '/' ) ? "rsd.xml" : "/rsd.xml" ) );
*/
  if( qApp->overrideCursor() == 0 )
    qApp->setOverrideCursor( QCursor( Qt::BusyCursor ) );

  networkBiz = FindingRsdXml;

  connect( netmgr, SIGNAL( finished( QNetworkReply * ) ),
           this, SLOT( handleHttpDone( QNetworkReply * ) ) );

}


void AccountsDialog::handleHttpDone( QNetworkReply *reply )
{
  QDomDocument rsdXml;
  QDomNodeList apis;
  QDomElement thisApi;
  int i;
  QUrl url;
  QString failure = tr( "QTM failed to auto-configure access to your account. "
                        "Please consult the documentation for your conent management system "
                        "or service." );

  if( reply->error() == QNetworkReply::NoError ) {
    switch( networkBiz ) {
      case FindingRsdXml:
        httpByteArray = reply->readAll();
        rsdXml.setContent( httpByteArray );
        httpByteArray = QByteArray();
        if( rsdXml.documentElement().tagName() == "rsd" ) {
          apis = rsdXml.elementsByTagName( "api" );
          for( i = 0; i < apis.count(); i++ ) {
            if( apis.at( i ).toElement().attribute( "name" ) == "MetaWeblog" ) {
              url = QUrl( apis.at( i ).toElement().attribute( "apiLink" ) );
              if( url.isValid() ) {
                leEndpoint->setText( url.toString() );
                accountList[currentRow].endpoint = url.toString();
                leLogin->setFocus();
                break;
              }
            }
          }
        }
        else 
          // Attempt to find tell-tale files has failed
          QMessageBox::information( this, tr( "QTM: Failure" ), failure, QMessageBox::Cancel );
        netmgr->disconnect();
        reply->disconnect();
        reply->deleteLater();
        currentReply = NULL;
        break;
      case FindingXmlrpcPhp:
        /*
        // If it finds xmlrpc.php, it returns a short string with a successful (200) status code
        if( responseHeader.statusCode() == 200 ) {
        leServer->setText( currentHost );
        currentHost = QString();
        leLocation->setText( QUrl( currentReq.path() ).path() );
        http->close();
        currentReq = QHttpRequestHeader();
        networkBiz = NoBusiness;
        disconnect( http, SIGNAL( requestFinished() ), this, 0 );
        disconnect( http, SIGNAL( done( bool ) ), this, 0 );
        break;
        }
        else {
        http->close();
        http->setHost( QUrl( currentReq.path() ).host() );
        http->get( QUrl( currentReq.path() ).path().replace( "xmlrpc.php", "rsd.xml" ) );
        currentReq = QHttpRequestHeader();
        networkBiz = FindingRsdXml;
        }  */
        break;
    }
  }
  else {
    QMessageBox::information( this, tr( "QTM - Network failure (%1)" ).arg( (int)reply->error() ),
                              tr( "QTM could not contact the site.  Please consult the documentation "
                                  "for your content management system or service and enter the "
                                  "server and location manually." ),
                              QMessageBox::Cancel );
    netmgr->disconnect();
    reply->deleteLater();
    currentReply = NULL;
  }
  if( qApp->overrideCursor() != 0 )
    qApp->restoreOverrideCursor();
}

void AccountsDialog::on_leName_textEdited( const QString &newName )
{
  if( currentRow != -1 ) {
    // qDebug( "title edited" );
    lwAccountList->item( currentRow )->setText( newName.isEmpty() ?
                                                tr( "(No name)" ) : newName );
    accountList[currentRow].name = newName;
  }
}

void AccountsDialog::on_leEndpoint_textEdited( const QString &newEP )
{
  if( currentRow != -1 )
    accountList[currentRow].endpoint = newEP;
}

void AccountsDialog::on_leLogin_textEdited( const QString &text )
{
  if( currentRow != -1 )
    accountList[currentRow].login = text;
}

void AccountsDialog::on_lePassword_textEdited( const QString &text )
{
  if( currentRow != -1 )
    accountList[currentRow].password = text;
}

void AccountsDialog::on_chCategoriesEnabled_clicked( bool state )
{
  //qDebug() << "current row:" << currentRow;
  if( currentRow != -1 )
    accountList[currentRow].categoriesEnabled = state;
  //qDebug() << "new state:" << accountList[currentRow].categoriesEnabled;
}

void AccountsDialog::on_chPostDateTime_clicked( bool state )
{
  if( currentRow != -1 )
    accountList[currentRow].postDateTime = state;
}

void AccountsDialog::on_chAllowComments_clicked( bool state )
{
  if( currentRow != -1 )
    accountList[currentRow].allowComments = state;
}

void AccountsDialog::on_chAllowTB_clicked( bool state )
{
  if( currentRow != -1 )
    accountList[currentRow].allowTB = state;
}

void AccountsDialog::on_chUseWordpressAPI_clicked( bool state )
{
  if( currentRow != -1 )
    accountList[currentRow].useWordpressAPI = state;
}

void AccountsDialog::on_cbHostedBlogType_activated( int newIndex )
{
  switch( newIndex ) {
    case 1: // wordpress.com
#ifdef DONT_USE_SSL
      leEndpoint->setText( "http://yourblog.wordpress.com/xmlrpc.php" );
      accountList[currentRow].endpoint = "http://yourblog.wordpress.com/xmlrpc.php";
#else
      leEndpoint->setText( "https://yourblog.wordpress.com/xmlrpc.php" );
      accountList[currentRow].endpoint = "https://yourblog.wordpress.com/xmlrpc.php";
#endif
      break;
    case 2: // TypePad
      leEndpoint->setText( "http://www.typepad.com/t/api" );
      accountList[currentRow].endpoint = "http://www/typepad.com/t/api";
      break;
    case 3: // SquareSpace
      leEndpoint->setText( "http://www.squarespace.com/do/process/external/PostInterceptor" );
      accountList[currentRow].endpoint = "http://www.squarespace.com/do/process/external/PostInterceptor";
      break;
    case 4: // Movable type
    case 5: // Self-hosted Wordpress
      leEndpoint->clear();
      accountList[currentRow].endpoint = "";
      break;
    case 6: // Drupal  
    case 7: // TextPattern
      chAllowTB->setChecked( false );             // these platforms don't
      accountList[currentRow].allowTB = false;    // support it
      leEndpoint->clear();
      accountList[currentRow].endpoint = "";
      break;
  }
  accountList[currentRow].hostedBlogType = newIndex;
}

bool AccountsDialog::eventFilter( QObject *obj, QEvent *event )
{
  // When the blog URI is typed in and Tab is pressed, the same must happen as when
  // Return is pressed, i.e. QTM must try to find the endpoint.
  if( obj == leBlogURI ) {
    switch( event->type() ) {
      case QEvent::FocusOut:
        if( !leBlogURI->text().isEmpty() ) {
          on_leBlogURI_returnPressed();
        }
      default:
        return QObject::eventFilter( obj, event );
    }
  }

  // The text in the location field must start with /; otherwise, the web server will
  // return an error.
/*  if( obj == leLocation ) {
    switch( event->type() ) {
      case QEvent::FocusOut:
        if( !leLocation->text().isEmpty() && !leLocation->text().startsWith( '/' ) ) {
          QString newLocation = leLocation->text().prepend( '/' );
          leLocation->setText( newLocation );
          if( currentRow != -1 )
            accountList[currentRow].location = newLocation;
        }
      default:
        return QObject::eventFilter( obj, event );
    }
  } */

  return QObject::eventFilter( obj, event );
}

