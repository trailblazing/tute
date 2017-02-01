/*******************************************************************************

  QTM - Qt-based blog manager
  Copyright (C) 2006-2011 Matthew J Smith

  This program is free software; you can redistribute it and/or modify
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

// EditingWindow.cc - Main window class for QTM applicaiton

// #define QTM_DEBUG

// #include <QApplication>
// #include <QCoreApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QCursor>
#include <QSettings>
#include <QList>
#include <QMainWindow>
#include <QStatusBar>
#include <QComboBox>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QSize>
#include <QPoint>
#include <QColor>
#include <QPalette>
#include <QTextCharFormat>
#include <QLabel>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QClipboard>
#include <QDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDataStream>
#include <QChar>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDateTime>
#include <QKeySequence>
#include <QFileDialog>
#include <QIODevice>
#include <QMap>
#include <QHash>
#include <QHashIterator>
#include <QWhatsThis>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>
#include <QFontDialog>
#include <QHostInfo>
#include <QProgressDialog>
#include <QTextEdit>
#include <QProcess>
#include <QProgressBar>
#include <QTimer>
#if ! defined DONT_USE_PTE
#include <QPlainTextEdit>
#endif

#include "XmlRpcHandler.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "EditingWindow.h"
#ifdef USE_SYSTRAYICON
#include "SysTrayIcon.h"
#endif
#include "SuperMenu.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#if QT_VERSION >= 0x050000
#ifdef Q_OS_UNIX
#if ! defined Q_OS_MAC && ! defined Q_OS_CYGWIN
#define Q_WS_X11
#endif	// !defined etc
#endif	// Q_OS_UNIX
#ifdef Q_OS_WIN
#ifndef WIN_QTV
#define WIN_QTV
#endif	// WIN_QTV
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QWindow>
#endif	// #Q_OS_WIN
#endif


/*
#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFBundle.h>
#endif
*/
#include "qtm_version.h.in"
#if defined Q_WS_X11 || defined Q_OS_WIN32
#include "markdown_header.h.in"
#endif

#include <QtAlgorithms>
#include <QtNetwork>
#include <QtXml>
#include <cmath>

#include "addimg.xpm"
#include "addlink.xpm"
#include "blog-this.xpm"
#include "fileopen.xpm"
#include "filesave.xpm"
#include "more.xpm"
#include "paragraph.xpm"
#include "preview.xpm"
#include "textbold.xpm"
#include "textital.xpm"
#include "textul.xpm"
#include "bquote.xpm"
#include "addtag.xpm"
#include "addctag.xpm"
#include "remtag.xpm"

#ifdef Q_OS_MAC
// #include "macFunctions.h"
#endif

#define EDITOR ed

EditingWindow::EditingWindow(QString newPost, QWidget *parent)
    : QMainWindow(parent)
      , currentReply(nullptr){
    QDomElement detailElem, attribElem, nameElem, serverElem, locElem, loginElem, pwdElem;
    QSettings	settings;

    setAttribute(Qt::WA_DeleteOnClose);

    qtm = qobject_cast<sapp_t *>(qApp);

#ifdef Q_OS_MAC
    SuperMenu *thisSM;
    Q_FOREACH(QWidget *w, qtm->topLevelWidgets()){
	if(w->objectName() == "MacGlobalMenu"){
	    thisSM = qobject_cast<SuperMenu *>(w);
	    if(thisSM) superMenu = thisSM;
	    break;
	}
    }
#endif

    doUiSetup();

    xmlEntities["quot"] = '"';
    xmlEntities["amp"]	= '&';
    xmlEntities["gt"]	= '>';
    xmlEntities["lt"]	= '<';
    xmlEntities["apos"] = '\'';

    readSettings();
    setEditorColors();
    setTextFonts();

    cw.chAllowComments->setEnabled(true);
    cw.chAllowComments->setCheckState(allowComments ? Qt::Checked
	: Qt::Unchecked);
    cw.chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);

    handleEnableCategories();

    QFile accountsXmlFile(PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(localStorageDirectory)));
    if(accountsDom.setContent(&accountsXmlFile)){
	accountsXmlFile.close();
	setInitialAccount();
    }else{
	accountsXmlFile.close();
	accountsXmlFile.setFileName(PROPERSEPS(QString("%1/qtmaccounts.xml").arg(localStorageDirectory)));
	accountsXmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
	if(accountsDom.setContent(&accountsXmlFile)){
	    accountsXmlFile.close();
	    setInitialAccount();
	}else{
#ifndef NO_DEBUG_OUTPUT
		// qDebug() << "Can't read the XML";
#endif
	    accountsXmlFile.close();
	    accountsElement = accountsDom.createElement("QTMAccounts");

	    readServerSettings();
	    if(! server.isEmpty()){
		// qDebug() << "copying details to new default element";
		currentAccountElement = accountsDom.createElement("account");
		currentAccountElement.setAttribute("id", "default");
		detailElem	= accountsDom.createElement("details");
		nameElem	= accountsDom.createElement("title");
		nameElem.appendChild(accountsDom.createTextNode(tr("Default account")));
		serverElem = accountsDom.createElement("server");
		serverElem.appendChild(accountsDom.createTextNode(server));
		locElem = accountsDom.createElement("location");
		locElem.appendChild(accountsDom.createTextNode(location));
		loginElem = accountsDom.createElement("login");
		loginElem.appendChild(accountsDom.createTextNode(login));
		pwdElem = accountsDom.createElement("password");
		pwdElem.appendChild(accountsDom.createTextNode(password));
		detailElem.appendChild(nameElem);
		detailElem.appendChild(serverElem);
		detailElem.appendChild(locElem);
		detailElem.appendChild(loginElem);
		detailElem.appendChild(pwdElem);
		currentAccountElement.appendChild(detailElem);

		// Delete the old account from the settings
		settings.beginGroup("account");
		settings.remove("server");
		settings.remove("location");
		settings.remove("login");
		settings.remove("password");
		settings.endGroup();

		// Now transfer the attributes to the default accounts
		QStringList attribs(accountAttributes.keys());
		Q_FOREACH(QString s, attribs){
		    if(*(accountAttributes[s])){
			attribElem = accountsDom.createElement("attribute");
			attribElem.setAttribute("name", s);
			detailElem.appendChild(attribElem);
		    }
		}
	    }
	    extractAccountDetails();

	    accountsElement.appendChild(currentAccountElement);
	    accountsDom.appendChild(accountsElement);
	    accountsDom.insertBefore(accountsDom.createProcessingInstruction("xml", "version=\"1.0\"")
				    , accountsDom.firstChild());
	    QHostInfo::lookupHost(server, this, SLOT(handleInitialLookup(QHostInfo)));
	}
    }
    checkForEmptySettings();
    EDITOR->setPlainText(newPost);
    mainStack->setCurrentIndex(edID);

	// setPostClean();
    setDirtySignals(true);
    dirtyIndicator->hide();
}

EditingWindow::~EditingWindow(){
    deleteLater();
}

#ifdef USE_SYSTRAYICON
void EditingWindow::setSTI(SysTrayIcon *_sti){
    sti = _sti;
}
#endif

void EditingWindow::doUiSetup(){
    qApp->setWindowIcon(QIcon(QPixmap(":/images/qtm-logo1.png")));

#ifdef USE_SYSTRAYICON
    sti = 0;
#endif

#ifdef Q_OS_MAC
    setStatusBar(0);
    setMenuBar(0);
#else
    superMenu = new SuperMenu(this);
    setMenuBar(superMenu);
    superMenu->setMainWindow(this);
#endif


    loadedEntryBlog		= 999;
    noAutoSave			= false;
    noAlphaCats			= false;
    networkActionsEnabled	= false;
    setAttribute(Qt::WA_QuitOnClose);

    userAgentString	= QString("QTM/%1").arg(QTM_VERSION);
    userAgentArray	= "QTM/";
    userAgentArray.append(QTM_VERSION);

	/*QCoreApplication::setOrganizationName( "Catkin Project" );
	QCoreApplication::setOrganizationDomain( "catkin.blogistan.co.uk" );
	QCoreApplication::setApplicationName( "QTM" );
	QCoreApplication::setApplicationVersion ( QTM_VERSION ); */

    qtmaccounts_xml << "qtmaccounts2.xml" << "qtmaccounts.xml";

	// ui.setupUi( this );
    currentHttpBusiness = None;
    doingNewPost	= false;
    entryBlogged	= false;
    netmgr		= new QNetworkAccessManager;

	// Setup main signals and slots

	/*ui.actionPrint->setEnabled( false );    // implemented
	ui.actionClose_and_Delete->setEnabled( false ); */

#ifndef USE_SYSTRAYICON
    if(superMenu) superMenu->qptAction->setVisible(false);
#endif

	// Set up toolbar actions

    action_Open = new_action(tr("Open ..."), tr("Open ...")
			    , tr("Load a saved entry from the disk")
			    , this, SLOT(choose()));
    action_Save = new_action(tr("Save"), tr("Save")
			    , tr("Save this entry to disk for editing or publication later")
			    , this, SLOT(save()));
    action_Bold = new_action(tr("Bold"), tr("Bold")
			    , tr("Make the selected text bold")
			    , this, SLOT(makeBold()));
    action_Italic = new_action(tr("Italic"), tr("Italic")
			      , tr("Italicise the selected text")
			      , this, SLOT(makeItalic()));
    action_Underline = new_action(tr("Underline"), tr("Underline")
				 , tr("Underline the selected text")
				 , this, SLOT(makeUnderline()));
    action_Blockquote = new_action(tr("Blockquote"), tr("Blockquote")
				  , tr("Mark the selected text as a block of quoted text")
				  , this, SLOT(makeBlockquote()));
    action_Link = new_action(tr("Link"), tr("Link")
			    , tr("Inserts a link to another page, the location to be specified now")
			    , this, SLOT(insertLink()));
    action_Image = new_action(tr("Image"), tr("Image")
			     , tr("Insert an image into the text, the location, alignment and alternate "
				  "text to be specified now")
			     , this, SLOT(insertImage()));
    action_More = new_action(tr("More"), tr("More")
			    , tr("Set this as the end of the main entry (which appears on the index) "
				 "and the beginning of the extended entry (which appears when you click More, "
				 "Extended entry or view the entry on its own page).")
			    , this, SLOT(insertMore()));
    action_Preview = new_action(tr("Entry in preview"), tr("Entry in preview")
			       , tr("Preview the entry in this window")
			       , this, SLOT(doPreview(bool)));
    action_Preview->setCheckable(true);
    action_Preview->setChecked(false);
    action_Blog_this = new_action(tr("Blog this!"), tr("Blog this!")
				 , tr("Post this entry to the blog")
				 , this, SLOT(blogThis()));

	// Set up toolbar icons

    action_Open->setIcon(QIcon(QPixmap(fileopen)));
    action_Save->setIcon(QIcon(QPixmap(filesave)));
    action_Bold->setIcon(QIcon(QPixmap(mini_bold_xpm)));
    action_Italic->setIcon(QIcon(QPixmap(mini_ital_xpm)));
    action_Underline->setIcon(QIcon(QPixmap(underline_xpm)));
    action_Blockquote->setIcon(QIcon(QPixmap(bquote_xpm)));
    action_Link->setIcon(QIcon(QPixmap(linkIcon_xpm)));
    action_Image->setIcon(QIcon(QPixmap(imgIcon_xpm)));
    action_More->setIcon(QIcon(QPixmap(more_xpm)));
    action_Preview->setIcon(QIcon(QPixmap(previewIcon_xpm)));
    action_Blog_this->setIcon(QIcon(QPixmap(blogThisIcon_xpm)));

	// Add actions to toolbar

    toolBar = addToolBar("");
    toolBar->addAction(action_Open);
    toolBar->addAction(action_Save);
    toolBar->addSeparator();
    toolBar->addAction(action_Bold);
    toolBar->addAction(action_Italic);
    toolBar->addAction(action_Underline);
    toolBar->addAction(action_Blockquote);
    toolBar->addAction(action_Link);
    toolBar->addAction(action_Image);
    toolBar->addAction(action_More);
    toolBar->addSeparator();
    toolBar->addAction(action_Preview);
    toolBar->addAction(action_Blog_this);
    toolBar->addSeparator();
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	// Actions related to Technorati tags

    addTechTagAction = new_action(tr("Add Technorati tag"), tr("Add technorati tag")
				 , tr("")
				 , this, SLOT(addTechTag()));
    addClipTechTagAction = new_action(tr("Add Technorati tag from clipboard")
				     , tr("Add Technorati tag from clipboard"), tr("")
				     , this, SLOT(addClipTag()));
    removeTechTagAction = new_action(tr("Remove Technorati tag")
				    , tr("Remove Technorati tag"), tr("")
				    , this, SLOT(removeTechTag()));
    addTechTagAction->setIcon(QIcon(QPixmap(addtag_xpm)));
    addClipTechTagAction->setIcon(QIcon(QPixmap(addctag_xpm)));
    removeTechTagAction->setIcon(QIcon(QPixmap(remtag_xpm)));

	// Actions related to Trackback pings

    addTBPingAction = new_action(tr("Add trackback ping"), tr("Add trackback ping"), tr("")
				, this, SLOT(addTBPing()));
    addClipPingAction = new_action(tr("Add ping from clipboard"), tr("Add ping from clipboard")
				  , tr("")
				  , this, SLOT(addClipTBPing()));
    removeTBPingAction = new_action(tr("Remove trackback ping"), tr("Remove trackback ping")
				   , tr("")
				   , this, SLOT(removeTBPing()));

	// Set up the main layout

    cWidget = new QWidget(this);

    leftWidget		= new QWidget(cWidget);
    mainWindowLayout	= new QHBoxLayout;
    mainWindowLayout->setMargin(0);
    mainWindowLayout->setMargin(5);
    cw.setupUi(leftWidget);

    connect(cw.lwOtherCats, SIGNAL(itemSelectionChanged())
	   , this, SLOT(changeOtherCatsHeading()));
    cw.cbBlogSelector->setMaxVisibleItems(10);
    cw.cbMainCat->setMaxVisibleItems(10);
    cw.lwTags->addAction(addTechTagAction);
    cw.lwTags->addAction(addClipTechTagAction);
    cw.lwTags->addAction(removeTechTagAction);
    cw.lwTBPings->addAction(addTBPingAction);
    cw.lwTBPings->addAction(addClipPingAction);
    cw.lwTBPings->addAction(removeTBPingAction);

    addKeywordTag_forList	= new QAction(tr("Add this tag"), 0);
    removeKeywordTag_forList	= new QAction(tr("Remove this tag"), 0);
    _refreshKeywordTags		= new QAction(tr("Refresh"), 0);
    connect(addKeywordTag_forList, SIGNAL(triggered(bool))
	   , this, SLOT(addKeywordTagFromAvailTags()));
    connect(removeKeywordTag_forList, SIGNAL(triggered(bool))
	   , this, SLOT(removeKeywordTag()));
    connect(_refreshKeywordTags, SIGNAL(triggered(bool))
	   , this, SLOT(refreshKeywordTags()));
    cw.lwKeywordTags->addAction(removeKeywordTag_forList);
    cw.lwAvailKeywordTags->addAction(addKeywordTag_forList);
    cw.lwAvailKeywordTags->addAction(_refreshKeywordTags);

    connect(cw.lwAvailKeywordTags, SIGNAL(itemDoubleClicked(QListWidgetItem *))
	   , this, SLOT(addKeywordTagFromAvailTags()));
    connect(cw.leAddKeywordTag, SIGNAL(returnPressed())
	   , this, SLOT(addKeywordTagFromLineEdit()));
    connect(cw.tbAddKeywordTag, SIGNAL(clicked(bool))
	   , this, SLOT(addKeywordTagFromLineEdit()));
    cw.tbSelectKeywordTag->setIcon(QIcon(":/images/next.png"));
    cw.tbRemoveKeywordTag->setIcon(QIcon(":/images/previous.png"));

    childCategoryHereAction = new QAction(tr("Add child category"), cw.lwOtherCats);
    connect(childCategoryHereAction, SIGNAL(triggered(bool))
	   , this, SLOT(newChildCategory()));
    cw.lwOtherCats->addAction(childCategoryHereAction);

#ifdef Q_OS_MAC
    cw.lwOtherCats->setWhatsThis(tr("Secondary categories, if your blog system supports "
				    "them.  To highlight more than one category, press "
				    "Command and click the mouse, or the left mouse button."));
#endif

    tagValidator = new QRegExpValidator(QRegExp("([a-zA-Z0-9\\.%]+[\\+ ])*[a-zA-Z0-9\\.%]+"), this);
    cw.leAddTag->setValidator(tagValidator);
    cw.cbDoTB->hide();

	// Set up main stack widget

    mainStack		= new QStackedWidget(cWidget);
    previewWindow	= new QTextBrowser(mainStack);
    previewWindowID	= mainStack->addWidget(previewWindow);
    previewWindow->setOpenExternalLinks(true);
    previewWindow->installEventFilter(this);

	// Set up console
    console	= new TEXTEDIT(mainStack);
    consoleID	= mainStack->addWidget(console);
    console->setReadOnly(true);
    console->installEventFilter(this);

	// Set up editor widget
    ed = new TEXTEDIT(mainStack);
#if defined DONT_USE_PTE
    EDITOR->setAcceptRichText(false);
#endif
    edID = mainStack->addWidget(ed);
    EDITOR->setReadOnly(false);
    previousRaisedLSWidget = edID;
    mainWindowLayout->addWidget(leftWidget, 3);
    mainWindowLayout->addWidget(mainStack, 6);
    mainWindowLayout->setSpacing(1);

	// Set up editor widget's actions
    EDITOR->setContextMenuPolicy(Qt::ActionsContextMenu);
    QList<QAction *>	editorActions;
    QAction		*sep1 = new QAction(EDITOR);
    sep1->setSeparator(true);
    QAction *sep2 = new QAction(EDITOR);
    sep2->setSeparator(true);
    QAction *actionSelectAll = new QAction(tr("Select all"), EDITOR);
    connect(actionSelectAll, SIGNAL(triggered(bool))
	   , EDITOR, SLOT(selectAll()));
    actionSelectAll->setShortcut(QKeySequence::fromString("Ctrl+A"));
    editorActions << superMenu->getUndoAction() << superMenu->getRedoAction() << sep1;
    editorActions	<< superMenu->getCutAction() << superMenu->getCopyAction()
			<< superMenu->getPasteAction() << sep2;
    editorActions << actionSelectAll;
    EDITOR->addActions(editorActions);
    EDITOR->installEventFilter(this);

	// Set up search widget
    mainWindowLayoutWithSearch	= new QVBoxLayout(cWidget);
    searchWidget		= new QijSearchWidget(EDITOR, this);
    searchWidget->hide();
    mainWindowLayoutWithSearch->setSpacing(1);
    mainWindowLayoutWithSearch->setContentsMargins(5, 5, 5, 5);
    mainWindowLayoutWithSearch->addLayout(mainWindowLayout);
    mainWindowLayoutWithSearch->addWidget(searchWidget);
    cWidget->setLayout(mainWindowLayoutWithSearch);
    setCentralWidget(cWidget);

	// Connect side widget signals
    connect(cw.pbRefresh, SIGNAL(clicked(bool))
	   , this, SLOT(refreshCategories()));
    connect(cw.pbRefresh, SIGNAL(clicked(bool))
	   , cw.cbMainCat, SLOT(clear()));
    connect(cw.pbAddCategory, SIGNAL(clicked(bool))
	   , this, SLOT(newCategory()));
    connect(cw.pbRefresh, SIGNAL(clicked(bool))
	   , cw.lwOtherCats, SLOT(clear()));
    connect(cw.leTitle, SIGNAL(editingFinished())
	   , this, SLOT(changeCaptionAfterTitleChanged()));
    connect(cw.leAddTag, SIGNAL(returnPressed())
	   , this, SLOT(addTechTagFromAddButton()));
    connect(cw.tbAddTag, SIGNAL(clicked())
	   , this, SLOT(addTechTagFromLineEdit()));
    connect(cw.leTBPingURL, SIGNAL(returnPressed())
	   , this, SLOT(addTBPingFromLineEdit()));
    connect(cw.tbTBAdd, SIGNAL(clicked())
	   , this, SLOT(addTBPingFromLineEdit()));
    connect(cw.stackedWidget, SIGNAL(currentChanged(int))
	   , this, SLOT(handleSideWidgetPageSwitch(int)));
    connect(cw.tbSelectKeywordTag, SIGNAL(clicked(bool))
	   , this, SLOT(addKeywordTagFromAvailTags()));
    connect(cw.tbRemoveKeywordTag, SIGNAL(clicked(bool))
	   , this, SLOT(removeKeywordTag()));
    connect(cw.pbRefreshKeywordTags, SIGNAL(clicked(bool))
	   , this, SLOT(refreshKeywordTags()));
    connect(cw.chShowPassword, SIGNAL(toggled(bool))
	   , this, SLOT(showPassword(bool)));

    cw.cbPageSelector->setCurrentIndex(0);
    cw.stackedWidget->setCurrentIndex(0);

    statusWidget = statusBar();

    connect(cw.pbCopyURL, SIGNAL(clicked()), this, SLOT(copyURL()));
    cw.copyURLWidget->hide();

    dirtyIndicator = new QLabel(this);
    dirtyIndicator->setPixmap(QPixmap(filesave));
    statusBar()->addPermanentWidget(dirtyIndicator);

    cw.progressWidget->setVisible(false);
    connect(cw.progressBar, SIGNAL(valueChanged(int))
	   , this, SLOT(hideProgressBarIfMaximum(int)));
#ifdef WIN_QTV
    taskbarButton = new QWinTaskbarButton(this);
    taskbarButton->setWindow(QWindow::fromWinId(effectiveWinId()));
    taskbarProgress = taskbarButton->progress();
    taskbarProgress->setVisible(false);
#endif

    highlighter = new Highlighter(0, this);

	// Set up hash of entry attributes
    accountAttributes["useHTTPS"]		= &useHTTPS;
    accountAttributes["categoriesEnabled"]	= &categoriesEnabled;
    accountAttributes["postDateTime"]		= &postDateTime;
    accountAttributes["allowComments"]		= &allowComments;
    accountAttributes["allowTB"]		= &allowTB;
    accountAttributes["useWordpressAPI"]	= &useWordpressAPI;

    accountStrings["server"]	= &server;
    accountStrings["location"]	= &location;
    accountStrings["port"]	= &port;
    accountStrings["login"]	= &login;
    accountStrings["password"]	= &password;

    setWindowModified(false);
    entryEverSaved	= false;
    cleanSave		= false;
    loadAutoLinkDictionary();
}

QAction *EditingWindow::new_action(QString title, QString toolTip, QString whatsThis, QObject *receiver, const char *slot){
    QAction *return_value = new QAction(title, this);
    return_value->setToolTip(toolTip);
    return_value->setWhatsThis(whatsThis);
    if(receiver) connect(return_value, SIGNAL(triggered(bool))
			, receiver, slot);
    return return_value;
}

bool EditingWindow::handleArguments(){
    bool		rv = true;
    int			i;
    EditingWindow	*c	= 0;
    EditingWindow	*d	= 0;
    QStringList		failedFiles;
    QStringList		args = QApplication::arguments();
    if(args.size() > 1){
	for(i = 1; i < args.size(); i ++){
	    if(c)																	// if there is a current new window
			d = c;
	    c = new EditingWindow;
#ifdef Q_OS_MAC
		// setNoStatusBar( c );
#endif
	    if(c->load(args.at(i))){
#ifdef USE_SYSTRAYICON
		c->setSTI(sti);
#endif
		if(d)																		// if there's an old window
			positionWidget(c, d);
		c->show();
		rv = false;
	    }else failedFiles.append(args.at(i));
	    if(failedFiles.size()){
		if(failedFiles.size() < args.size() - 1){
		    QMessageBox::information(this, tr("Error")
					    , tr("Could not load the following:\n\n%1")
			.arg(failedFiles.join("\n"))
					    , QMessageBox::Ok);
		    rv = false;
		}else{
		    if(QMessageBox::question(0, tr("Error")
					    , tr("Could not load the following:\n\n%1")
			.arg(failedFiles.join("\n"))
					    , tr("&Open blank window"), tr("E&xit"), 0)) QApplication::exit();
		    else rv = false;
		}
	    }
	}
    }
    return rv;
}

void EditingWindow::positionWidget(QWidget *w, QWidget *refWidget){
    QDesktopWidget	*dw	= QApplication::desktop();
    QRect		r	= dw->availableGeometry();
    if(qobject_cast<QWidget *>(refWidget)){
	QRect g = refWidget->geometry();// of the reference widget
	if((g.right() + 30) >= (r.right() + 30) &&
	    (g.bottom() + 30) >= (r.bottom() + 30)) w->move(r.left(), r.top());
	else w->move(refWidget->x() + 30, refWidget->y() + 30);
    }
}

void EditingWindow::changeCaptionAfterTitleChanged(){
    if(cw.leTitle->text().isEmpty()) setWindowTitle(tr("QTM - new entry [*]"));
    else setWindowTitle(QString("%1 - QTM [*]").arg(cw.leTitle->text().trimmed()));
}

void EditingWindow::closeEvent(QCloseEvent *event){
    QSettings settings;

#ifndef NO_DEBUG_OUTPUT
	// qDebug( "close event" );
#endif
    if(isWindowModified()){
	if(! saveCheck()){
	    event->ignore();
	    qApp->setQuitOnLastWindowClosed(false);
	}else{
	    settings.setValue("account/lastAccountId", currentAccountId);
	    event->accept();
	}
    }else{
	writeSettings();
	settings.setValue("account/lastAccountId", currentAccountId);
	event->accept();
    }
}

void EditingWindow::showEvent(QShowEvent *event){
	// If the document is empty, the window unedited and the entry never saved,
	// chances are it's new
    if(EDITOR->document()->isEmpty() &&
	! dirtyIndicator->isVisible() &&
	! event->spontaneous() &&
	! entryEverSaved) cw.leTitle->setFocus(Qt::ActiveWindowFocusReason);
    QMainWindow::showEvent(event);
}

void EditingWindow::resizeEvent(QResizeEvent *event){
    writeSettings();

    QMainWindow::resizeEvent(event);
}

void EditingWindow::doQuit(){
#ifndef NO_DEBUG_OUTPUT
    int i = QApplication::topLevelWidgets().size();
	// qDebug() << i << " top level widgets";
#endif

    qApp->setQuitOnLastWindowClosed(true);
    qApp->closeAllWindows();
}

void EditingWindow::checkForEmptySettings(){
	// Check if this is a brand new user
    if(localStorageDirectory.isEmpty() || server.isEmpty()){
	if(QMessageBox::question(0, tr("Welcome to QTM")
				, tr("You do not have any preferences set, and QTM "
				     "needs to know where to find your blog, and where "
				     "to store your data.\n\n"
				     "Set these preferences now?")
				, QMessageBox::Yes | QMessageBox::Default
				, QMessageBox::No) == QMessageBox::Yes){
	    getPreferences(tr("Stage 1 of 2: Preferences"));
	    getAccounts(tr("Stage 2 of 2: Accounts"));
	}
    }
}

void EditingWindow::setEditorColors(){
    QPalette		widgetPalette;
    QTextCharFormat	tagFormat, entityFormat, commentFormat, linkFormat;

	// Set colours
    widgetPalette = EDITOR->palette();
    widgetPalette.setColor(QPalette::Base, editorBgColor);
    widgetPalette.setColor(QPalette::Text, editorFgColor);
    EDITOR->setPalette(widgetPalette);

    widgetPalette = previewWindow->palette();
    widgetPalette.setColor(QPalette::Base, previewBgColor);
    widgetPalette.setColor(QPalette::Text, previewFgColor);
    previewWindow->setPalette(widgetPalette);

    widgetPalette = console->palette();
    widgetPalette.setColor(QPalette::Base, consoleBgColor);
    widgetPalette.setColor(QPalette::Text, consoleFgColor);
    console->setPalette(widgetPalette);

	// setDirtySignals( false );
	// Set HTML tag colours
    tagFormat.setForeground(QColor(tagFgColor));
    tagFormat.setBackground(QColor(tagBgColor));
    tagFormat.setFontWeight(tagBold ? QFont::Bold : QFont::Normal);
    tagFormat.setFontItalic(tagItalic);
    highlighter->setFormatFor(Highlighter::Tag, tagFormat);

	// Set entity colours
    entityFormat.setForeground(QColor(entityFgColor));
    entityFormat.setBackground(QColor(entityBgColor));
    entityFormat.setFontWeight(entityBold ? QFont::Bold : QFont::Normal);
    entityFormat.setFontItalic(entityItalic);
    highlighter->setFormatFor(Highlighter::Entity, entityFormat);

	// Set comment colours
    commentFormat.setForeground(QColor(commentFgColor));
    commentFormat.setBackground(QColor(commentBgColor));
    commentFormat.setFontWeight(commentBold ? QFont::Bold : QFont::Normal);
    commentFormat.setFontItalic(commentItalic);
    highlighter->setFormatFor(Highlighter::Comment, commentFormat);

	// setDirtySignals( true );
}

void EditingWindow::setTextFonts(){
    QFont f, g, h;
    if(editorFontString != ""){
	f.fromString(editorFontString);
	EDITOR->setFont(f);
    }else{
	f			= EDITOR->font();
	editorFontString	= f.toString();
    }
    if(previewFontString != ""){
	g.fromString(previewFontString);
	previewWindow->setFont(g);
    }else{
	g			= previewWindow->font();
	previewFontString	= g.toString();
    }
    if(consoleFontString != ""){
	h.fromString(consoleFontString);
	console->setFont(h);
    }else{
	h			= console->font();
	consoleFontString	= h.toString();
    }
	// Enable syntax highlighting, if applicable
    if(enableHighlighting){
	// setDirtySignals( false );
	if(superMenu) superMenu->setHighlightingChecked(true);
	highlighter->setDocument(EDITOR->document());
	// setDirtySignals( true );
    }else if(superMenu) superMenu->setHighlightingChecked(false);
}

void EditingWindow::setInitialAccount(){
    int i;

	// qDebug() << "server is empty";
    QSettings settings;
    lastAccountID = settings.value("account/lastAccountID", "").toString();
    QDomNodeList accountsList = accountsDom.documentElement()
	.elementsByTagName("account");
    QDomElement thisTitleElem;
    cw.cbAccountSelector->clear();
    if(accountsList.count() == 1 &&
	accountsList.at(0).firstChildElement("details").isNull()){
	// This is if there is just one legacy account
	readServerSettings();
	QDomElement detailElem, titleElem, serverElem, portElem, locElem
	, loginElem, pwdElem;
	detailElem	= accountsDom.createElement("details");
	titleElem	= accountsDom.createElement("title");
	titleElem.appendChild(accountsDom.createTextNode(tr("Default account")));
	serverElem = accountsDom.createElement("server");
	serverElem.appendChild(accountsDom.createTextNode(server));
	locElem = accountsDom.createElement("location");
	locElem.appendChild(accountsDom.createTextNode(location));
	if(! port.isEmpty()){
	    portElem = accountsDom.createElement("port");
	    portElem.appendChild(accountsDom.createTextNode(port));
	}
	loginElem = accountsDom.createElement("login");
	loginElem.appendChild(accountsDom.createTextNode(login));
	pwdElem = accountsDom.createElement("password");
	pwdElem.appendChild(accountsDom.createTextNode(password));
	detailElem.appendChild(titleElem);
	detailElem.appendChild(serverElem);
	if(! portElem.isNull()) detailElem.appendChild(portElem);
	detailElem.appendChild(locElem);
	detailElem.appendChild(loginElem);
	detailElem.appendChild(pwdElem);
	accountsList.at(0).insertBefore(detailElem, QDomNode());// i.e. insert it at the start
	accountsList.at(0).toElement().setAttribute("id", "default");
    }
    for(i = 0; i < accountsList.count(); i ++){
	thisTitleElem = accountsList.at(i).toElement().firstChildElement("details")
	    .firstChildElement("title");
	if(! thisTitleElem.isNull()) cw.cbAccountSelector->addItem(decodeXmlEntities(thisTitleElem.text())
								  , accountsList.at(i).toElement().attribute("id"));
	else
		cw.cbAccountSelector->addItem(tr("Unnamed account")
					     , accountsList.at(i).toElement().attribute("id").isEmpty()
		    ? QString("noid_%1").arg(i)
		    : accountsList.at(i).toElement().attribute("id"));
    }
	// Check if the last account is in the accounts list, and if so, make it current
    for(i = 0; i < accountsList.count(); i ++){
	if(accountsList.at(i).toElement().attribute("id") == lastAccountID){
		// qDebug() << "found it";
	    currentAccountElement	= accountsList.at(i).toElement();
	    currentAccountId		= currentAccountElement.attribute("id");
	    cw.cbAccountSelector->setCurrentIndex(i);
	    extractAccountDetails();
	    populateBlogList();
	    setNetworkActionsEnabled(true);

	    disconnect(cw.cbAccountSelector, SIGNAL(activated(int))
		      , this, SLOT(changeAccount(int)));
	    connect(cw.cbAccountSelector, SIGNAL(activated(int))
		   , this, SLOT(changeAccount(int)));
	    break;
	}
	// If it reaches the end of the loop with no joy
	if(i == accountsList.count() - 1){
		// qDebug() << "using first account";
	    currentAccountElement = accountsDom.documentElement()
		.firstChildElement("account");
	    extractAccountDetails();
	    populateBlogList();
	    setNetworkActionsEnabled(true);
	    disconnect(cw.cbAccountSelector, SIGNAL(activated(int))
		      , this, SLOT(changeAccount(int)));
	    connect(cw.cbAccountSelector, SIGNAL(activated(int))
		   , this, SLOT(changeAccount(int)));
	}
    }
}

void EditingWindow::readSettings(){
    QString		crf;
    sapp_t::recentFile	currentRF;
    QString		defaultMarkdownPath, defaultPerlLocation;
    bool		defaultUseMarkdown;

#ifdef Q_OS_MAC
    QString	mdPathString = QString("%1/../Resources").arg(qApp->applicationDirPath());
    QDir	mdPath(mdPathString);
    defaultMarkdownPath = QString("%1/Markdown.pl").arg(mdPath.canonicalPath());

    defaultUseMarkdown	= true;
    defaultPerlLocation = "/usr/bin/perl";
#else
#ifdef Q_OS_WIN32
    defaultMarkdownPath = QString("%1/Markdown.pl")
	.arg(QString(MARKDOWN_LOCATION).replace("%APPDIRPATH%", qApp->applicationDirPath()));
    defaultUseMarkdown	= false;
    defaultMarkdownPath = PROPERSEPS(defaultMarkdownPath);
    defaultPerlLocation = "C:\\Perl\\bin\\perl.exe";
#else
	// Presumably we're in X11
    defaultMarkdownPath = MARKDOWN_LOCATION;
    defaultUseMarkdown	= true;
    defaultPerlLocation = "/usr/bin/perl";
#endif
#endif

#ifdef Q_OS_WIN32
    QString defaultLocalStorageDir = QString(qtm->isSandbox() ? "%1\\QTMSandbox" : "%1\\QTM blog").arg(QDir::homePath());
#else
    QString defaultLocalStorageDir = QString(qtm->isSandbox() ? "%1/qtm-sandbox" : "%1/qtm-blog").arg(QDir::homePath());
#endif

    QSettings settings;
    applicationVersion = settings.value("application/version", "").toString();

    settings.beginGroup("geometry");
    resize(settings.value("size", QSize(640, 450)).toSize());
// #endif
    move(settings.value("position", QPoint(20, 20)).toPoint());
    qDebug() << "Done resize";

    settings.endGroup();

    settings.beginGroup("account");
    localStorageDirectory = qtm->isSandbox()
	? QString("%1/sandbox").arg(settings.value("localStorageDirectory", defaultLocalStorageDir).toString())
	: settings.value("localStorageDirectory", defaultLocalStorageDir).toString();
    if(localStorageDirectory.contains("~/")){
	localStorageDirectory.replace("~", QDir::homePath());
	settings.setValue("localStorageDirectory", localStorageDirectory);
    }
    localStorageFileExtn	= settings.value("localStorageFileExtn", "cqt").toString();
    useMarkdown			= settings.value("useMarkdown", defaultUseMarkdown).toBool();
    perlPath			= settings.value("perlPath", defaultPerlLocation).toString();
    markdownPath		= settings.value("markdownPath", defaultMarkdownPath).toString();
    doMarkdownWhenPosting	= settings.value("doMarkdownWhenPosting", useMarkdown).toBool();
    stripParaTags		= settings.value("stripParaTags", false).toBool();
    useHTTPS			= settings.value("useHTTPS", true).toBool();
    categoriesEnabled		= settings.value("categoriesEnabled", true).toBool();
    useNewWindows		= settings.value("useNewWindows", true).toBool();
    savePassword		= settings.value("savePassword", false).toBool();
    postAsSave			= settings.value("postAsSave", true).toBool();
    allowComments		= settings.value("allowComments", true).toBool();
    allowTB			= settings.value("allowTB", true).toBool();
    enableHighlighting		= settings.value("enableHighlighting", true).toBool();
    suppressReadMore		= settings.value("suppressReadMore", true).toBool();
    useUtf8			= settings.value("useUtf8", false).toBool();
#ifdef USE_SYSTRAYICON
    copyTitle = settings.value("copyTitle", true).toBool();
#endif
    allowRegexSearch = settings.value("allowRegexSearch", false).toBool();
    settings.endGroup();
    settings.beginGroup("fonts");
    editorFontString	= settings.value("editorFontString", "").toString();
    previewFontString	= settings.value("previewFontString", "").toString();
    consoleFontString	= settings.value("consoleFontString", "").toString();
    editorBgColor	= QColor(settings.value("editorBgColor", "#FFFFFF").toString());
    editorFgColor	= QColor(settings.value("editorFgColor", "#000000").toString());
    previewBgColor	= QColor(settings.value("previewBgColor", "#FFFFFF").toString());
    previewFgColor	= QColor(settings.value("previewFgColor", "#000000").toString());
    consoleBgColor	= QColor(settings.value("consoleBgColor", "#FFFFFF").toString());
    consoleFgColor	= QColor(settings.value("consoleFgColor", "#000000").toString());
    settings.endGroup();

    settings.beginGroup("highlighting");
    tagBgColor		= QColor(settings.value("tagBgColor", "#FFFFFF").toString());
    tagFgColor		= QColor(settings.value("tagFgColor", "#008000").toString());
    entityBgColor	= QColor(settings.value("entityBgColor", "#FFFFFF").toString());
    entityFgColor	= QColor(settings.value("entityFgColor", "#000080").toString());
    commentBgColor	= QColor(settings.value("commentBgColor", "#FFFFFF").toString());
    commentFgColor	= QColor(settings.value("commentFgColor", "#008080").toString());
    linkBgColor		= QColor(settings.value("linkBgColor", "#ffffff").toString());
    linkFgColor		= QColor(settings.value("linkFgColor", "#0000ff").toString());
    tagBold		= settings.value("tagBold", false).toBool();
    tagItalic		= settings.value("tagItalic", false).toBool();
    entityBold		= settings.value("entityBold", false).toBool();
    entityItalic	= settings.value("entityItalic", false).toBool();
    commentBold		= settings.value("commentBold", false).toBool();
    commentItalic	= settings.value("commentItalic", false).toBool();
    linkBold		= settings.value("linkBold", false).toBool();
    linkItalic		= settings.value("linkItalic", false).toBool();
    linkUnderline	= settings.value("linkUnderline", true).toBool();
    settings.endGroup();

#ifdef USE_SYSTRAYICON
    settings.beginGroup("sysTrayIcon");
    STI2ClickFunction = settings.value("doubleClickFunction", 0).toInt();
    settings.endGroup();
#endif
    if(server.isEmpty() || location.isEmpty() || login.isEmpty()){
	// Adequate network details absent, therefore disable all widgets and actions
	// leading to network use.
	setNetworkActionsEnabled(false);
    }
    cw.categoryPage->setEnabled(true);
    cw.chAllowComments->setEnabled(true);
    cw.chAllowTB->setEnabled(true);
    searchWidget->setExpertEnabled(allowRegexSearch);
}

void EditingWindow::readServerSettings(){
	// qDebug() << "getting server settings";
    QSettings settings;
    settings.beginGroup("account");
    server	= settings.value("server", "").toString();
    location	= settings.value("location", "").toString();
    login	= settings.value("login", "").toString();
    password	= settings.value("password", "").toString();
}

void EditingWindow::handleEnableCategories(){
    cw.gbCategory->setEnabled(categoriesEnabled);
    if(superMenu) superMenu->setCategoriesEnabled(categoriesEnabled);
}

void EditingWindow::openRecentFile(){
    QAction *action = qobject_cast<QAction *>(sender());
    if(action) choose(action->data().toString());
}

void EditingWindow::writeSettings(){
    QSettings settings;

    settings.beginGroup("geometry");
    settings.setValue("size", size());
    settings.setValue("position", pos());
    settings.endGroup();
}

void EditingWindow::callRefreshCategories(){
    qDebug() << "Starting refresh";
    if(! currentHttpBusiness){
	cw.cbMainCat->clear();
	cw.lwOtherCats->clear();
	refreshCategories();
    }else{
#ifdef QTM_DEBUG
	if(currentHttpBusiness != _mt_getCategoryList){
	    statusWidget->showMessage(tr("changeBlog: All HTTP requests are blocked")
				     , 2000);
	    addToConsole(QString("changeBlog %1 failed - HTTP job of type %2 ongoing")
		.arg(b)
		.arg((int) currentHttpBusiness));
	}
#else
	if(currentHttpBusiness != _mt_getCategoryList) statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
#ifndef NO_DEBUG_OUTPUT
	// qDebug() << "Finishing changeblog";
#endif
}

void EditingWindow::refreshCategories(){
    QDomElement param, value, integer, string;

    disconnect(SIGNAL(httpBusinessFinished()));
    if(! currentHttpBusiness){
	cw.cbMainCat->clear();
	cw.lwOtherCats->clear();
	qDebug() << "Cleared category lists";
	qDebug() << "Cleared category lists";

	QDomDocument	doc;
	QDomElement	methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "mt.getCategoryList"));

	QDomElement params = doc.createElement("params");
	params.appendChild(XmlValue(doc, "string", currentBlogid));
	params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details")
	    .firstChildElement("login").text()));
	params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details")
	    .firstChildElement("password").text()));

	methodCall.appendChild(params);
	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml"
							, "version=\"1.0\" encoding=\"UTF-8\"")
			, doc.firstChild());
	QByteArray requestArray(doc.toByteArray(2));
	responseData = "";

	placeNetworkRequest(_mt_getCategoryList, requestArray);
    }else{
#ifdef QTM_DEBUG
	statusWidget->showMessage(tr("refreshCategories: All HTTP requests are blocked")
				 , 2000);
#else
	statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
}

QString EditingWindow::processWithMarkdown(const QString &text){
    QString		conversionString = text;
    QString		nullString, conversionStringB, tempFileName, mdPath;
    QTemporaryFile	tf;
    if(tf.open()){
	tempFileName = tf.fileName();
	QTextStream tstream(&tf);
	tstream.setCodec("UTF-8");
	tstream << conversionString;
	tf.close();
    }else return QString();
    QStringList markdownPaths;
    markdownPaths	<< markdownPath
			<< QString("%1/Markdown/Markdown.pl").arg(QCoreApplication::applicationDirPath())
			<< QString("%1/../Markdown/Markdown.pl").arg(QCoreApplication::applicationDirPath())
			<< "nofile";
    Q_FOREACH(QString mdp, markdownPaths){
	if(mdp == "nofile") return QString();
	else if(QFile::exists(mdp)){
	    mdPath = mdp;
	    break;
	}
    }
    QProcess proc;
    proc.start(perlPath, QStringList() << mdPath << tempFileName);
    if(! proc.waitForStarted()){
	statusWidget->showMessage(tr("Failed to start conversion"), 2000);
	return QString();
    }
	// Now wait until the process finishes; use a loop and process events every 10th of a second
    for(int i = 0; i <= 300; ++ i){
	if(i == 300){	// if 30 seconds has elapsed without a finish signal
	    statusWidget->showMessage(tr("Failed to convert"), 2000);
	    return QString();
	}
	if(proc.waitForFinished(100)) break;
	else qApp->processEvents();
    }
    conversionStringB = QString::fromUtf8(proc.readAllStandardOutput());
    if(conversionStringB.length() < conversionString.length() ||
	proc.exitStatus() != QProcess::NormalExit){
	statusWidget->showMessage(tr("Conversion failed"), 2000);
	if(proc.exitStatus() != QProcess::NormalExit) qDebug() << "Bad exit";
	// qDebug() << "Length:" << conversionStringB.length();
	return QString();
    }
    return conversionStringB;
}
void EditingWindow::getAccounts(){
    getAccounts(QString());
}

void EditingWindow::getAccounts(const QString &title){
    QList<AccountsDialog::Account>	acctsList, returnedAccountsList;
    AccountsDialog::Account		acct;
    QDomNodeList			accountsList, thisAccountsAttribs;
    QDomDocument			newAccountsDom;
    QDomElement				newQTMAccounts, newAccount, detailElement, nameElement, serverElement, locationElement
    , portElement, loginElement, pwdElement, blogsElement, boolElement, attribsElement, hbtElement;
    QString	oldCurrentAccountId, oldBlogid, currentTitle, currentPort;
    QStringList thisAccountsAttribStrings;
    QUrl	thisUrl;
    int		i, j;
    bool	ok;

    purgeBlankAccounts();

	// Extract accounts list from account tree
    accountsList = accountsDom.elementsByTagName("account");
    for(i = 0; i < accountsList.count(); i ++){
	thisAccountsAttribs = accountsList.at(i).toElement().firstChildElement("details")
	    .firstChildElement("attributes").elementsByTagName("attribute");
	thisAccountsAttribStrings = QStringList();
	for(j = 0; j < thisAccountsAttribs.count(); j ++) thisAccountsAttribStrings << thisAccountsAttribs.at(j).toElement().attribute("name");
	acct		= AccountsDialog::Account();
	acct.id		= accountsList.at(i).toElement().attribute("id");
	detailElement	= accountsList.at(i).firstChildElement("details");
	currentPort	= detailElement.firstChildElement("port").text();

	acct.name	= detailElement.firstChildElement("title").text();
	acct.endpoint	= QString("%1://%2%3/%4")
	    .arg(thisAccountsAttribStrings.contains("useHTTPS") ? "https" : "http")
	    .arg(detailElement.firstChildElement("server").text())
	    .arg(currentPort.isEmpty() ? "" : QString(":%1").arg(currentPort))
	    .arg(detailElement.firstChildElement("location").text().remove(QRegExp("^/")));
	acct.login	= detailElement.firstChildElement("login").text();
	acct.password	= detailElement.firstChildElement("password").text();

	acct.hostedBlogType = 0;
	int hbt = detailElement.firstChildElement("hostedBlogType").text().toInt(&ok);
	if(ok) acct.hostedBlogType = hbt;
	acct.categoriesEnabled	= false;
	acct.postDateTime	= false;
	acct.allowComments	= false;
	acct.allowTB		= false;
	acct.useWordpressAPI	= false;
	if(thisAccountsAttribStrings.contains("categoriesEnabled")) acct.categoriesEnabled = true;
	if(thisAccountsAttribStrings.contains("postDateTime")) acct.postDateTime = true;
	if(thisAccountsAttribStrings.contains("allowComments") ||
	    thisAccountsAttribStrings.contains("comments")) acct.allowComments = true;
	if(thisAccountsAttribStrings.contains("allowTB") ||
	    thisAccountsAttribStrings.contains("trackback")) acct.allowTB = true;
	if(thisAccountsAttribStrings.contains("useWordpressAPI")) acct.useWordpressAPI = true;
	acctsList.append(acct);
    }
    oldCurrentAccountId = cw.cbAccountSelector->itemData(cw.cbAccountSelector->currentIndex())
	.toString();
    oldBlogid = cw.cbBlogSelector->itemData(cw.cbBlogSelector->currentIndex()).toString();
    int oldCurrentBlog = cw.cbAccountSelector->currentIndex();
	// accountsList = accountsDom.documentElement().elementsByTagName( "account" );

    AccountsDialog acctsDialog(acctsList, oldCurrentBlog, this);
    if(! title.isEmpty()) acctsDialog.setWindowTitle(title);
    if(acctsDialog.exec()){
	returnedAccountsList	= acctsDialog.accounts();
	newQTMAccounts		= newAccountsDom.createElement("QTMAccounts");
	for(i = 0; i < returnedAccountsList.count(); i ++){
	    newAccount = newAccountsDom.createElement("account");
	    newAccount.setAttribute("id", returnedAccountsList.at(i).id);
	    detailElement	= newAccountsDom.createElement("details");
	    nameElement		= newAccountsDom.createElement("title");
	    nameElement.appendChild(newAccountsDom.createTextNode(returnedAccountsList.at(i).name));
	    hbtElement = newAccountsDom.createElement("hostedBlogType");
	    hbtElement.appendChild(newAccountsDom.createTextNode(QString::number(returnedAccountsList.at(i).hostedBlogType)));

	    thisUrl.setUrl(returnedAccountsList.at(i).endpoint);
	    serverElement = newAccountsDom.createElement("server");
	    serverElement.appendChild(newAccountsDom.createTextNode(thisUrl.host()));
	    locationElement = newAccountsDom.createElement("location");
	    locationElement.appendChild(newAccountsDom.createTextNode(thisUrl.path()));
	    portElement = newAccountsDom.createElement("port");
	    portElement.appendChild(newAccountsDom.createTextNode(thisUrl.port() == - 1 ? ""
		: QString::number(thisUrl.port())));
	    loginElement = newAccountsDom.createElement("login");
	    loginElement.appendChild(newAccountsDom.createTextNode(returnedAccountsList.at(i).login));
	    pwdElement = newAccountsDom.createElement("password");
	    pwdElement.appendChild(newAccountsDom.createTextNode(returnedAccountsList.at(i).password));

	    detailElement.appendChild(nameElement);
	    detailElement.appendChild(hbtElement);
	    detailElement.appendChild(serverElement);
	    detailElement.appendChild(locationElement);
	    detailElement.appendChild(portElement);
	    detailElement.appendChild(loginElement);
	    detailElement.appendChild(pwdElement);
	    if(  returnedAccountsList.at(i).categoriesEnabled
	      || returnedAccountsList.at(i).postDateTime
	      || returnedAccountsList.at(i).allowComments
	      || returnedAccountsList.at(i).allowTB) attribsElement = newAccountsDom.createElement("attributes");
	    if(thisUrl.scheme() == "https"){
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "useHTTPS");
		attribsElement.appendChild(boolElement);
	    }
	    if(returnedAccountsList.at(i).categoriesEnabled){
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "categoriesEnabled");
		attribsElement.appendChild(boolElement);
	    }
	    if(returnedAccountsList.at(i).postDateTime){
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "postDateTime");
		attribsElement.appendChild(boolElement);
	    }
	    if(returnedAccountsList.at(i).allowComments){
		// qDebug() << "comments attribute set";
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "allowComments");
		attribsElement.appendChild(boolElement);
	    }
	    if(returnedAccountsList.at(i).allowTB){
		// qDebug() << "TB attribute set";
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "allowTB");
		attribsElement.appendChild(boolElement);
	    }
	    if(returnedAccountsList.at(i).useWordpressAPI){
		boolElement = newAccountsDom.createElement("attribute");
		boolElement.setAttribute("name", "useWordpressAPI");
		attribsElement.appendChild(boolElement);
	    }
	    if(! attribsElement.isNull()) detailElement.appendChild(attribsElement);
	    newAccount.appendChild(detailElement);
		// Check if each account is matched from the old list; if it is, copy the blogs list
	    for(j = 0; j < accountsList.count(); j ++){
		if(accountsList.at(j).toElement().attribute("id") == returnedAccountsList.at(i).id){
		    blogsElement = accountsList.at(j).toElement().firstChildElement("blogs");
		    if(! blogsElement.isNull()) newAccount.appendChild(newAccountsDom.importNode(blogsElement, true));
		    break;
		}
	    }
	    newQTMAccounts.appendChild(newAccount);
	}
	// Now add processing instruction and the whole QTMAccounts element, and replace the old main
	// accounts tree with this one
	newAccountsDom.appendChild(newAccountsDom.createProcessingInstruction("xml"
									     , "version='1.0'"));
	newAccountsDom.appendChild(newQTMAccounts);
	accountsDom = newAccountsDom.cloneNode(true).toDocument();

	cw.cbAccountSelector->clear();
	accountsList = accountsDom.documentElement().elementsByTagName("account");
	for(i = 0; i < accountsList.count(); ++ i){
	    currentTitle = decodeXmlEntities(accountsList.at(i).firstChildElement("details")
		    .firstChildElement("title").text());
	    if(currentTitle.isEmpty()) currentTitle = tr("(Unnamed account)");
	    cw.cbAccountSelector->addItem(currentTitle, accountsList.at(i).toElement().
		attribute("id"));
	}
	// Check if the old current account is in this list; if so, make it current again
	for(i = 0; i < accountsList.count(); ++ i){
	    if(accountsList.at(i).toElement().attribute("id") == oldCurrentAccountId){
		cw.cbAccountSelector->setCurrentIndex(i);
		currentAccountElement = accountsList.at(i).toElement();
		extractAccountDetails();

		QStringList accountStringNames(accountStrings.keys());
		Q_FOREACH(QString s, accountStringNames){
		    *(accountStrings[s]) = currentAccountElement.firstChildElement("details")
			.firstChildElement(s).text();
		}
		// Now check if the current account has any blogs
		if(! currentAccountElement.firstChildElement("blogs").isNull()){
			// qDebug() << "blogs found";
		    populateBlogList();
		}else{
			// qDebug() << "no blogs found";
		    refreshBlogList();
		}
		break;
	    }
	    if(i == accountsList.count() - 1){
		// qDebug() << "reached end of account list";
		cw.cbAccountSelector->setCurrentIndex(0);
		currentAccountElement	= accountsList.at(0).toElement();
		currentAccountId	= currentAccountElement.attribute("id");
		extractAccountDetails();
		refreshBlogList();
		setNetworkActionsEnabled(true);
	    }
	}
	disconnect(cw.cbAccountSelector, SIGNAL(activated(int))
		  , this, SLOT(changeAccount(int)));
	connect(cw.cbAccountSelector, SIGNAL(activated(int))
	       , this, SLOT(changeAccount(int)));
	saveAccountsDom();
    }
}

void EditingWindow::getPreferences(){
    getPreferences(QString());
}

void EditingWindow::getPreferences(const QString &title){
    QSettings	settings;
    QPalette	palette, widgetPalette;

    PrefsDialog prefsDialog(this);
    if(localStorageDirectory.isEmpty()){
#ifdef Q_OS_WIN32
	QString lsd = QString(qtm->isSandbox() ? "%1\\QTMsandbox" : "%1\\QTM blog").arg(QDir::homePath())
	    .replace("/", "\\");
#else
	QString lsd = QString(qtm->isSandbox() ? "%1/qtm-sandbox" : "%1/qtm-blog").arg(QDir::homePath());
#endif
	prefsDialog.leLocalDir->setText(lsd);
    }else prefsDialog.leLocalDir->setText(localStorageDirectory);
    prefsDialog.leFileExtn->setText(localStorageFileExtn);
    prefsDialog.chUseNewWindows->setCheckState(useNewWindows ? Qt::Checked
	: Qt::Unchecked);
    prefsDialog.chPostAsSave->setCheckState(postAsSave ? Qt::Checked
	: Qt::Unchecked);
    prefsDialog.chAllowComments->setCheckState(allowComments ? Qt::Checked
	: Qt::Unchecked);
    prefsDialog.chAllowTB->setCheckState(allowTB ? Qt::Checked
	: Qt::Unchecked);
#if defined USE_SYSTRAYICON
    prefsDialog.chCopyTitle->setCheckState(copyTitle ? Qt::Checked : Qt::Unchecked);
#else
    prefsDialog.chCopyTitle->setVisible(false);
#endif
    prefsDialog.chUseMarkdown->setCheckState(useMarkdown ? Qt::Checked : Qt::Unchecked);
    prefsDialog.lePerlPath->setText(PROPERSEPS(perlPath));
    prefsDialog.leMarkdownPath->setText(PROPERSEPS(markdownPath));
    prefsDialog.chDoMarkdownWhenPosting->setCheckState(doMarkdownWhenPosting ? Qt::Checked
	: Qt::Unchecked);
    prefsDialog.chStripParaTags->setCheckState(stripParaTags ? Qt::Checked : Qt::Unchecked);
    prefsDialog.chStripParaTags->setEnabled(doMarkdownWhenPosting);
    if(! useMarkdown){
	prefsDialog.lineBelowFonts->setVisible(false);
	prefsDialog.gbMarkdown->setVisible(false);
    }
    prefsDialog.chSuppressReadMore->setCheckState(suppressReadMore ? Qt::Checked : Qt::Unchecked);
    prefsDialog.chUseUtf8->setCheckState(useUtf8 ? Qt::Checked : Qt::Unchecked);

    prefsDialog.chAllowRegexSearch->setCheckState(allowRegexSearch ? Qt::Checked : Qt::Unchecked);

#ifdef Q_OS_MAC
    prefsDialog.setWindowFlags(Qt::Sheet);
#endif

    prefsDialog.tabWidget->setTabText(0, tr("General"));
    prefsDialog.tabWidget->setTabText(1, tr("Fonts && Markdown"));
    prefsDialog.tabWidget->setTabText(2, tr("Syntax highlighting"));
    prefsDialog.tabWidget->setCurrentIndex(0);

    QFont	editorFont	= EDITOR->font();
    QFont	previewFont	= previewWindow->font();
    QFont	consoleFont	= console->font();
    prefsDialog.fcbComposer->setCurrentFont(editorFont);
    prefsDialog.sbComposer->setValue(editorFont.pointSize());
    prefsDialog.fcbPreview->setCurrentFont(previewFont);
    prefsDialog.sbPreview->setValue(previewFont.pointSize());
    prefsDialog.fcbConsole->setCurrentFont(consoleFont);
    prefsDialog.sbConsole->setValue(consoleFont.pointSize());

	// Set editor colours
    palette = prefsDialog.lEditorExampleText->palette();
    palette.setColor(QPalette::Window, editorBgColor);
    palette.setColor(QPalette::WindowText, editorFgColor);
    prefsDialog.lEditorExampleText->setPalette(palette);

	// Set preview colours
    palette = prefsDialog.lPreviewExampleText->palette();
    palette.setColor(QPalette::Window, previewBgColor);
    palette.setColor(QPalette::WindowText, previewFgColor);
    prefsDialog.lPreviewExampleText->setPalette(palette);

	// Set console colours
    palette = prefsDialog.lConsoleExampleText->palette();
    palette.setColor(QPalette::Window, consoleBgColor);
    palette.setColor(QPalette::WindowText, consoleFgColor);
    prefsDialog.lConsoleExampleText->setPalette(palette);

	// Set colours for HTML tags
    palette = prefsDialog.lTagColorDemo->palette();
    palette.setColor(QPalette::Window, tagBgColor);
    palette.setColor(QPalette::WindowText, tagFgColor);
    prefsDialog.lTagColorDemo->setPalette(palette);
    prefsDialog.setLabelText(prefsDialog.lTagColorDemo, tagBold, tagItalic);
    prefsDialog.chTagBold->setChecked(tagBold);
    prefsDialog.chTagItalic->setChecked(tagItalic);

	// Set colours for HTML entities
    palette = prefsDialog.lEntityColorDemo->palette();
    palette.setColor(QPalette::Window, entityBgColor);
    palette.setColor(QPalette::WindowText, entityFgColor);
    prefsDialog.lEntityColorDemo->setPalette(palette);
    prefsDialog.setLabelText(prefsDialog.lEntityColorDemo, entityBold, entityItalic);
    prefsDialog.chEntityBold->setChecked(entityBold);
    prefsDialog.chEntityItalic->setChecked(entityItalic);

	// Set colours for comments
    palette = prefsDialog.lCommentColorDemo->palette();
    palette.setColor(QPalette::Window, commentBgColor);
    palette.setColor(QPalette::WindowText, commentFgColor);
    prefsDialog.lCommentColorDemo->setPalette(palette);
    prefsDialog.setLabelText(prefsDialog.lCommentColorDemo, commentBold, commentItalic);
    prefsDialog.chCommentBold->setChecked(commentBold);
    prefsDialog.chCommentItalic->setChecked(commentItalic);

#ifdef USE_SYSTRAYICON
    prefsDialog.cbSTI2ClickFunction->setCurrentIndex(STI2ClickFunction);
#else
    prefsDialog.cbSTI2ClickFunction->hide();
    prefsDialog.label_10->hide();
#endif

    prefsDialog.resize(QSize(prefsDialog.width()
			    , prefsDialog.minimumHeight()));
    if(! title.isEmpty()) prefsDialog.setWindowTitle(title);
	// Disable cbStripParaTags as this feature is not properly implemented
    prefsDialog.chStripParaTags->setVisible(false);
    if(prefsDialog.exec()){
#ifndef NO_DEBUG_OUTPUT
	// qDebug( "Setting account variables" );
#endif
	localStorageDirectory	= PROPERSEPS(prefsDialog.leLocalDir->text());
	localStorageFileExtn	= prefsDialog.leFileExtn->text();
	useNewWindows		= prefsDialog.chUseNewWindows->isChecked();
	postAsSave		= prefsDialog.chPostAsSave->isChecked();
	allowComments		= prefsDialog.chAllowComments->isChecked();
	allowTB			= prefsDialog.chAllowTB->isChecked();
	copyTitle		= prefsDialog.chCopyTitle->isChecked();
	useMarkdown		= prefsDialog.chUseMarkdown->isChecked();
	perlPath		= PROPERSEPS(prefsDialog.lePerlPath->text());
	markdownPath		= PROPERSEPS(prefsDialog.leMarkdownPath->text());
	doMarkdownWhenPosting	= prefsDialog.chDoMarkdownWhenPosting->isChecked();
	stripParaTags		= prefsDialog.chStripParaTags->isChecked();
	allowRegexSearch	= prefsDialog.chAllowRegexSearch->isChecked();
	suppressReadMore	= prefsDialog.chSuppressReadMore->isChecked();
	useUtf8			= prefsDialog.chUseUtf8->isChecked();

#ifndef NO_DEBUG_OUTPUT
	// qDebug( "setting fonts" );
#endif
	QFont ef = prefsDialog.fcbComposer->currentFont();
	ef.setPointSize(prefsDialog.sbComposer->value());
	editorFontString = ef.toString();
	EDITOR->setFont(ef);
	QFont pf = prefsDialog.fcbPreview->currentFont();
	pf.setPointSize(prefsDialog.sbPreview->value());
	previewFontString = pf.toString();
	previewWindow->setFont(pf);
	QFont cf = prefsDialog.fcbConsole->currentFont();
	cf.setPointSize(prefsDialog.sbConsole->value());
	consoleFontString = cf.toString();
	console->setFont(cf);

	// Set colours
	palette		= prefsDialog.lEditorExampleText->palette();
	editorBgColor	= palette.color(QPalette::Window);
	editorFgColor	= palette.color(QPalette::WindowText);

	palette		= prefsDialog.lPreviewExampleText->palette();
	previewBgColor	= palette.color(QPalette::Window);
	previewFgColor	= palette.color(QPalette::WindowText);

	palette		= prefsDialog.lConsoleExampleText->palette();
	consoleBgColor	= palette.color(QPalette::Window);
	consoleFgColor	= palette.color(QPalette::WindowText);

	setDirtySignals(false);
	palette		= prefsDialog.lTagColorDemo->palette();
	tagBgColor	= palette.color(QPalette::Window);
	tagFgColor	= palette.color(QPalette::WindowText);

	palette		= prefsDialog.lEntityColorDemo->palette();
	entityBgColor	= palette.color(QPalette::Window);
	entityFgColor	= palette.color(QPalette::WindowText);

	palette		= prefsDialog.lCommentColorDemo->palette();
	commentBgColor	= palette.color(QPalette::Window);
	commentFgColor	= palette.color(QPalette::WindowText);

	tagBold		= prefsDialog.chTagBold->isChecked();
	tagItalic	= prefsDialog.chTagItalic->isChecked();
	entityBold	= prefsDialog.chEntityBold->isChecked();
	entityItalic	= prefsDialog.chEntityItalic->isChecked();
	commentBold	= prefsDialog.chCommentBold->isChecked();
	commentItalic	= prefsDialog.chCommentItalic->isChecked();

	setEditorColors();
	setDirtySignals(true);

#if defined USE_SYSTRAYICON
	STI2ClickFunction = prefsDialog.cbSTI2ClickFunction->currentIndex();
	if(sti){
#ifndef NO_DEBUG_OUTPUT
		// qDebug( "setting double click function" );
#endif
	    sti->setDoubleClickFunction(STI2ClickFunction);
	    sti->setCopyTitle(copyTitle);
	}
#endif
	// Handle local directory settings; a default is used if none is specified
	if(localStorageDirectory.isEmpty()){
#ifdef Q_OS_WIN32
	    localStorageDirectory = QString(qtm->isSandbox() ? "%1\\QTMsandbox" : "%1\\QTM blog").arg(QDir::homePath());
#else
	    localStorageDirectory = QString(qtm->isSandbox() ? "%1/qtm-sandbox" : "%1/qtm-blog").arg(QDir::homePath());
#endif
	}
	QDir qd(localStorageDirectory);
	if(! qd.exists()){
	    addToConsole(tr("Making directory %1").arg(localStorageDirectory));
	    if(! qd.mkpath(localStorageDirectory)) statusWidget->showMessage(tr("Could not create QTM directory."), 2000);
	    if(! noAutoSave) saveAccountsDom();
	}
	/*setDirtySignals( false );
	setDirtySignals( true ); */
	// Now check to see if this is an old-style Blogger account, which does not
	// support categories.
	if(server.contains("blogger.com")){
		// Blogger no longer supports this API, so all these features are redundant.
	    statusWidget->showMessage(tr("Blogger no longer supports QTM or similar clients."), 2000);
	    server	= "";
	    location	= "";
	}else cw.categoryPage->setEnabled(true);
	handleEnableCategories();

	searchWidget->setExpertEnabled(allowRegexSearch);

	settings.setValue("application/version", QTM_VERSION);
	settings.beginGroup("account");
	settings.setValue("localStorageDirectory"
			 , localStorageDirectory.replace("~", QDir::homePath()));
	settings.setValue("localStorageFileExtn", localStorageFileExtn);
	settings.setValue("useNewWindows", useNewWindows);
	settings.setValue("postAsSave", postAsSave);
	settings.setValue("allowComments", allowComments);
	settings.setValue("allowTB", allowTB);
#if defined USE_SYSTRAYICON
	settings.setValue("copyTitle", copyTitle);
#endif
	settings.setValue("useMarkdown", useMarkdown);
	settings.setValue("perlPath", perlPath);
	settings.setValue("markdownPath", markdownPath);
	settings.setValue("doMarkdownWhenPosting", doMarkdownWhenPosting);
	settings.setValue("stripParaTags", stripParaTags);
	settings.setValue("allowRegexSearch", allowRegexSearch);
	settings.setValue("enableHighlighting", enableHighlighting);
	settings.setValue("suppressReadMore", suppressReadMore);
	settings.setValue("useUtf8", useUtf8);
	settings.endGroup();

	settings.beginGroup("fonts");
	settings.setValue("editorFontString", editorFontString);
	settings.setValue("previewFontString", previewFontString);
	settings.setValue("consoleFontString", consoleFontString);
	settings.setValue("editorBgColor", editorBgColor.name());
	settings.setValue("editorFgColor", editorFgColor.name());
	settings.setValue("previewBgColor", previewBgColor.name());
	settings.setValue("previewFgColor", previewFgColor.name());
	settings.setValue("consoleBgColor", consoleBgColor.name());
	settings.setValue("consoleFgColor", consoleFgColor.name());
	settings.endGroup();

	settings.beginGroup("highlighting");
	settings.setValue("tagBgColor", tagBgColor.name());
	settings.setValue("tagFgColor", tagFgColor.name());
	settings.setValue("entityBgColor", entityBgColor.name());
	settings.setValue("entityFgColor", entityFgColor.name());
	settings.setValue("commentBgColor", commentBgColor.name());
	settings.setValue("commentFgColor", commentFgColor.name());
	settings.setValue("linkBgColor", linkBgColor.name());
	settings.setValue("linkFgColor", linkFgColor.name());
	settings.setValue("tagBold", tagBold);
	settings.setValue("tagItalic", tagItalic);
	settings.setValue("entityBold", entityBold);
	settings.setValue("entityItalic", entityItalic);
	settings.setValue("commentBold", commentBold);
	settings.setValue("commentItalic", commentItalic);
	settings.setValue("linkBold", linkBold);
	settings.setValue("linkItalic", linkItalic);
	settings.setValue("linkUnderline", linkUnderline);
	settings.endGroup();

#ifdef USE_SYSTRAYICON
	settings.beginGroup("sysTrayIcon");
	settings.setValue("doubleClickFunction", STI2ClickFunction);
	settings.endGroup();
#endif
    }
}
void EditingWindow::setHighlighting(bool hl){
    QSettings settings;

    enableHighlighting = hl;
    disconnect(EDITOR->document(), 0, this, SLOT(dirtify()));
    if(enableHighlighting){
	if(highlighter->document() == 0) highlighter->setDocument(EDITOR->document());
    }else highlighter->setDocument(0);
    settings.setValue("account/enableHighlighting", enableHighlighting);
    QTimer::singleShot(250, this, SLOT(reenableDirty()));
	// connect( EDITOR->document(), SIGNAL( contentsChanged() ), this, SLOT( dirtify() ) );
}

void EditingWindow::reenableDirty(){
    connect(EDITOR->document(), SIGNAL(contentsChanged())
	   , this, SLOT(dirtify()));
}

void EditingWindow::saveAccountsDom(){
    purgeBlankAccounts();

    QFile domOut(PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(localStorageDirectory)));
    if(domOut.open(QIODevice::WriteOnly)){
	QTextStream domFileStream(&domOut);
	accountsDom.save(domFileStream, 2);
	domOut.close();
    }else{
	QDir dir(localStorageDirectory);
	if(! dir.exists()){
	    if(QMessageBox::question(0, tr("Cannot find storage directory")
				    , tr("QTM cannot find the directory you specified to "
					 "store your account data and files.\n\n"
					 "Create it?")
				    , QMessageBox::Yes | QMessageBox::Default
				    , QMessageBox::No) == QMessageBox::Yes){
		if(dir.mkpath(localStorageDirectory)){
		    domOut.setFileName(PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(localStorageDirectory)));
		    if(domOut.open(QIODevice::WriteOnly)){
			QTextStream dfs(&domOut);
			accountsDom.save(dfs, 2);
			domOut.close();
		    }else statusWidget->showMessage(tr("Could not write to accounts file."), 2000);
		}else statusWidget->showMessage(tr("Could not create the directory."), 2000);
	    }
	}
    }
}

void EditingWindow::purgeBlankAccounts(){
    int i, j;

	// First, remove blank account elements
    QDomNode		thisNode, parentNode;
    QDomNodeList	accountNodes = accountsDom.elementsByTagName("account");
    i = accountNodes.count();
    for(j = 0; j < i; j ++){
	thisNode = accountNodes.at(j);
	if(! thisNode.hasChildNodes()){
	    parentNode = thisNode.parentNode();
	    parentNode.removeChild(thisNode);
	}
    }
}

void EditingWindow::populateAccountList(){	// slot
    int		i;
    QDomElement ct, detail;
    QString	cid, cname;

    QDomNodeList	accountNodeList = accountsDom.documentElement().elementsByTagName("account");
    int			a		= accountNodeList.count();
    if(a){
	// qDebug () << "populating:" << a << "accounts";
	cw.cbAccountSelector->clear();
	for(i = 0; i < a; i ++){
	    cid = accountNodeList.at(i).toElement().attribute("id");
	    if(cid.isEmpty()) cid = QString("nameless_%1").arg(i + 1);
	    detail = accountNodeList.at(i).firstChildElement("details");
	    if(! detail.isNull()){
		cname = detail.firstChildElement("title").text();
		if(cname.isEmpty()) cname = tr("Unnamed account %1").arg(i + 1);
		cw.cbAccountSelector->addItem(decodeXmlEntities(cname), cid);
	    }
	}
    }
}

void EditingWindow::populateBlogList(){	// slot
    addToConsole(accountsDom.toString(2));

    QDomNodeList blogNodeList = currentAccountElement.firstChildElement("blogs")
	.elementsByTagName("blog");
    QDomNodeList	catNodeList, tagNodeList;
    QDomElement		catsElement, tagsElement;
    QDomElement		ct;
    QString		thisTagString;
    int			a		= blogNodeList.count();
    int			uncategorized	= 0;
#ifndef NO_DEBUG_OUTPUT
	// qDebug() << "Blogs: " << a;
#endif
    int		b, i, j;
    QString	catName, catId;
    if(a){
	cw.cbBlogSelector->clear();
	for(i = 0; i < a; i ++){
	    ct = blogNodeList.at(i).firstChildElement("blogName");
	    cw.cbBlogSelector->addItem(decodeXmlEntities(blogNodeList.at(i).firstChildElement("blogName").text())
				      , QVariant(blogNodeList.at(i).firstChildElement("blogid").text()));
	    currentBlog		= i;
	    currentBlogElement	= currentAccountElement.firstChildElement("blogs")
		.elementsByTagName("blog").at(currentBlog).toElement();
	    currentBlogid = currentBlogElement.firstChildElement("blogid").text();
	}
	currentBlog = cw.cbBlogSelector->currentIndex();

	catsElement = blogNodeList.at(currentBlog).firstChildElement("categories");
	if(! catsElement.isNull()){
	    catNodeList = catsElement.elementsByTagName("category");
	    b		= catNodeList.count();
	    if(b){
		cw.cbMainCat->clear();
		cw.lwOtherCats->clear();
		for(j = 0; j < b; j ++){
		    catName	= decodeXmlEntities(catNodeList.at(j).firstChildElement("categoryName").text());
		    catId	= catNodeList.at(j).firstChildElement("categoryId").text();
		    cw.cbMainCat->addItem(catName, QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
		    cw.lwOtherCats->addItem(catName);
		    if(catId == "1") uncategorized = j;
		}
		if(uncategorized) cw.cbMainCat->setCurrentIndex(uncategorized);
	    }else{
		cw.chNoCats->setEnabled(false);
		cw.cbMainCat->setEnabled(false);
		cw.lwOtherCats->setEnabled(false);
	    }
	}
	cw.lwAvailKeywordTags->clear();
	cw.lwKeywordTags->clear();
	populateTagList();
	disconnect(cw.cbBlogSelector, SIGNAL(activated(int))
		  , this, SLOT(changeBlog(int)));
	connect(cw.cbBlogSelector, SIGNAL(activated(int))
	       , this, SLOT(changeBlog(int)));
    }else{
	cw.cbBlogSelector->setEnabled(false);
	cw.chNoCats->setEnabled(false);
	cw.cbMainCat->setEnabled(false);
	cw.lwOtherCats->setEnabled(false);
    }
}

void EditingWindow::populateTagList(){	// slot
    if(useWordpressAPI){
	cw.lwAvailKeywordTags->setVisible(true);
	cw.tbSelectKeywordTag->setVisible(true);
	cw.tbRemoveKeywordTag->setVisible(true);
	cw.pbRefreshKeywordTags->setVisible(true);
	QDomElement tagsElement = currentBlogElement.firstChildElement("wpTags");
	if(! tagsElement.isNull()){
	    QStringList		tagList;
	    QString		thisTagString;
	    QDomNodeList	tagNodeList = tagsElement.elementsByTagName("tag");
		// qDebug() << "Tag nodes:" << tagNodeList.count();
	    for(int j = 0; j < tagNodeList.count(); j ++){
		thisTagString = tagNodeList.at(j).toElement().text();
		if(! tagList.contains(thisTagString) && ! thisTagString.isEmpty()) tagList += thisTagString;
	    }
		// qDebug() << "Tags:" << tagList.count();
	    cw.lwAvailKeywordTags->addItems(tagList);
	}
    }else{
	cw.lwAvailKeywordTags->setVisible(false);
	cw.tbSelectKeywordTag->setVisible(false);
	cw.tbRemoveKeywordTag->setVisible(false);
	cw.pbRefreshKeywordTags->setVisible(false);
    }
}

void EditingWindow::refreshBlogList(){	// slot
    QDomDocument	doc;
    QDomElement		methodCall = doc.createElement("methodCall");
    methodCall.appendChild(XmlMethodName(doc, "blogger.getUsersBlogs"));

    QDomElement params = doc.createElement("params");
    params.appendChild(XmlValue(doc, "string", "xxx"));
    params.appendChild(XmlValue(doc, "string", login));
    params.appendChild(XmlValue(doc, "string", password));
    methodCall.appendChild(params);

    doc.appendChild(methodCall);
    doc.insertBefore(doc.createProcessingInstruction("xml"
						    , "version=\"1.0\" encoding=\"UTF-8\"")
		    , doc.firstChild());

    responseData = "";
    QByteArray requestArray(doc.toByteArray(2));
    placeNetworkRequest(_blogger_getUsersBlogs, requestArray);
}

void EditingWindow::placeNetworkRequest(HttpBusinessType hbtype, QByteArray &array){
    bool ok;

    int p = port.toInt(&ok);
#ifdef DONT_USE_SSL
    if(! ok) pp = ":80";
#else
    if(! ok) p = 0;
#endif
    QUrl url(QString("%1://%2%3%4")
	.arg(useHTTPS ? "https" : "http")
	.arg(server)
	.arg(ok ? QString(":%1").arg(port) : "")
	.arg(location));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", userAgentArray);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    currentReply = netmgr->post(request, array);

    addToConsole(request.url().toString());
    addToConsole(QString(array));
    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    currentHttpBusiness = hbtype;
    connect(netmgr, SIGNAL(finished(QNetworkReply *))
	   , this, SLOT(handleDone(QNetworkReply *)));
#ifndef DONT_USE_SSL
    connect(currentReply, SIGNAL(sslErrors(const QList<QSslError> &))
	   , this, SLOT(handleSslErrors(const QList<QSslError> &)));
#endif
}

/*void EditingWindow::setHost( QHttp *h, QString &serv, QString &pt, bool cm )
{
  int p;
  bool ok;

  p = pt.toInt( &ok );
#ifdef DONT_USE_SSL
  if( !ok )
    p = 80;
  h->setHost( serv, QHttp::ConnectionModeHttp, p );
#else
  if( !ok )
    p = 0;
  h->setHost( serv, useHTTPS ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp, p );
#endif
} */

/*void EditingWindow::handleResponseHeader( const QHttpResponseHeader &header ) // slot
{
  if( header.statusCode() == 404 ) {
    QMessageBox::warning( this, tr( "Error 404" ),
                          tr( "There is no web service at the location you\n"
                              "specified.  Please change it in the Preferences\n"
                              "window." ) );
    http->disconnect();
    http->abort();
    setNetworkActionsEnabled( false );
  }
  else
    responseData.append( http->readAll() );
} */

void EditingWindow::stopThisJob(){
#ifndef NO_DEBUG_OUTPUT
	// qDebug() << "Aborting.";
#endif

    netmgr->disconnect();
    if(currentReply){
	currentReply->abort();

	currentHttpBusiness = None;
	disconnect(this, SIGNAL(httpBusinessFinished()), 0, 0);
	if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
	currentReply->deleteLater();
	currentReply = nullptr;
    }
    statusWidget->showMessage(tr("Operation cancelled"), 2000);
}

void EditingWindow::handleDone(QNetworkReply *reply){
    if(reply->error() != QNetworkReply::NoError){
	if(reply->error() == QNetworkReply::OperationCanceledError) statusWidget->showMessage(tr("Operation cancelled."), 2000);
	else statusWidget->showMessage(tr("The request failed"), 2000);
	if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
    }else{
	responseData = reply->readAll();
	statusWidget->showMessage(QString(tr("The request succeeded; %1 bytes received"))
	    .arg(responseData.size()), 2000);
	switch(currentHttpBusiness){
	    case _blogger_getUsersBlogs:      blogger_getUsersBlogs(responseData);break;
	    case _metaWeblog_newMediaObject:  metaWeblog_newMediaObject(responseData);break;
	    case _metaWeblog_newPost:         metaWeblog_newPost(responseData);break;
	    case _metaWeblog_editPost:        metaWeblog_editPost(responseData);break;
	    case _mt_getCategoryList:         mt_getCategoryList(responseData);break;
	    case _mt_publishPost:             mt_publishPost(responseData);break;
	    case _mt_setPostCategories:       mt_setPostCategories(responseData);break;
	    case _wp_getTags:                 wp_getTags(responseData);break;
	    case _wp_newCategory:             wp_newCategory(responseData);break;
	    case _wp_newPost:                 wp_newPost(responseData);break;
	    case _wp_editPost:                wp_editPost(responseData);break;
	}
    }
    netmgr->disconnect();
    reply->disconnect();
    reply->deleteLater();
    currentReply = nullptr;
    cw.progressBar->setValue(cw.progressBar->value() + 1);
#ifdef WIN_QTV
    taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
    currentHttpBusiness = None;
    doingNewPost	= false;
    emit httpBusinessFinished();
}

#ifndef DONT_USE_SSL
void EditingWindow::handleSslErrors(const QList<QSslError> &errorList){
    int i = errorList.count();
    if(i) statusWidget->showMessage(tr("%1 SSL errors").arg(i), 2000);
}
#endif

void EditingWindow::changeCurrentBlog(int b){	// slot
    currentBlog = b;
}

void EditingWindow::changeAccount(int a){// slot
    QString currentBlogText;
    if(currentAccount != a){
	qDebug() << "clearing categories etc";
	currentAccount = a;
	cw.cbBlogSelector->clear();
	cw.cbMainCat->clear();
	cw.lwOtherCats->clear();
	cw.lwAvailKeywordTags->clear();
	cw.lwKeywordTags->clear();

	currentAccountElement = accountsDom.documentElement()
	    .elementsByTagName("account").at(a).toElement();
	currentAccountId = currentAccountElement.toElement().attribute("id");
	extractAccountDetails();
	/*qDebug() << "Current account: " << currentAccountElement.firstChildElement( "details" )
	  .firstChildElement( "title" ).text();
	*/
	QStringList accountStringNames(accountStrings.keys());
	Q_FOREACH(QString s, accountStringNames) *(accountStrings[s]) = currentAccountElement.firstChildElement("details")
		    .firstChildElement(s).text();
	extractAccountAttributes();

	QDomElement blogsElement = currentAccountElement.firstChildElement("blogs");
	if(! blogsElement.isNull()){
	    QDomNodeList	blogsList	= blogsElement.elementsByTagName("blog");
	    int			b		= blogsList.count();
	    if(b){
		// qDebug() << "Blogs: " << b;
		cw.cbBlogSelector->clear();
		for(int i = 0; i < b; i ++) cw.cbBlogSelector->addItem(decodeXmlEntities(blogsList.at(i).firstChildElement("blogName").text())
								      , blogsList.at(i).firstChildElement("blogid").text());
		cw.cbBlogSelector->setEnabled(true);
		changeBlog(0, true);
		currentBlogid = cw.cbBlogSelector->itemData(0).toString();
		cw.cbBlogSelector->disconnect(this, SLOT(changeBlog(int)));	// eliminate duplicate connections
		connect(cw.cbBlogSelector, SIGNAL(activated(int))
		       , this, SLOT(changeBlog(int)));
		emit blogRefreshFinished();
		if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
	    }else refreshBlogList();
	}else refreshBlogList();
    }
}

void EditingWindow::extractAccountDetails(){	// slot
    QDomElement caDetails = currentAccountElement.firstChildElement("details").toElement();
    server		= caDetails.firstChildElement("server").text();
    location		= caDetails.firstChildElement("location").text();
    port		= caDetails.firstChildElement("port").text();
    login		= caDetails.firstChildElement("login").text();
    password		= caDetails.firstChildElement("password").text();
    currentAccountId	= currentAccountElement.attribute("id");

    extractAccountAttributes();
}

void EditingWindow::extractAccountAttributes(){
    QStringList		accountAttribNames(accountAttributes.keys());
    QString		attribName;
    QDomNodeList	attribNodes = currentAccountElement.firstChildElement("details")
	.elementsByTagName("attribute");

    QList<QWidget *> wordpressOnlyWidgets;
    wordpressOnlyWidgets	<< cw.lPassword << cw.lePassword << cw.wBlankWidget
				<< cw.chShowPassword;
    Q_FOREACH(QString t, accountAttribNames){
	*(accountAttributes[t]) = false;
	for(int i = 0; i < attribNodes.count(); i ++){
	    attribName = attribNodes.at(i).toElement().attribute("name");
	    if(attribName == "comments") attribName = "allowComments";
	    if(attribName == "trackback") attribName = "allowTB";
	    if(attribName == t) *(accountAttributes[t]) = true;
	}
    }
    cw.chAllowComments->setCheckState(allowComments ? Qt::Checked : Qt::Unchecked);
    cw.chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);
    cw.pbAddCategory->setEnabled(useWordpressAPI);
	// cw.chSticky->setEnabled( useWordpressAPI );
    cw.chSticky->setVisible(false /*useWordpressAPI */);
    Q_FOREACH(QWidget *w, wordpressOnlyWidgets) w->setVisible(useWordpressAPI);
    if(superMenu) superMenu->addCatAction->setEnabled(useWordpressAPI);
}


void EditingWindow::changeBlog(int b, bool fromChangeAccount){	// slot
    QString		currentCategoryText, currentCatId;
    QDomElement		tagsElement;
    QDomNodeList	tagsList;
    int			i;
    int			uncategorized = 0;
	// qDebug() << "Starting changeblog" << b;
    if(currentBlog != b || fromChangeAccount){
	// qDebug() << "changing the blog anyway";
	currentBlog = b;
	cw.cbMainCat->clear();
	cw.lwOtherCats->clear();
	cw.lwAvailKeywordTags->clear();
	cw.lwKeywordTags->clear();

	currentBlogElement	= currentAccountElement.elementsByTagName("blog").at(currentBlog).toElement();
	currentBlogid		= currentBlogElement.firstChildElement("blogid").text();
	// qDebug() << currentBlogid;


	cw.lwAvailKeywordTags->clear();
	cw.lwKeywordTags->clear();
	populateTagList();

	QDomElement catsElement = currentBlogElement.firstChildElement("categories");
	if(! catsElement.isNull()){
	    QDomNodeList	catsList	= catsElement.elementsByTagName("category");
	    int			c		= catsList.count();
	    qDebug() << c << "categories";
	    if(c){
// #ifndef NO_DEBUG_OUTPUT
		// qDebug() << "Categories: " << c;
// #endif
		cw.cbMainCat->clear();
		cw.lwOtherCats->clear();
		for(i = 0; i < c; i ++){
		    currentCategoryText = decodeXmlEntities(catsList.at(i).firstChildElement("categoryName").text());
		    currentCatId	= catsList.at(i).firstChildElement("categoryId").text();
		    cw.cbMainCat->addItem(currentCategoryText
					 , QVariant(currentCatId));
		    cw.lwOtherCats->addItem(currentCategoryText);
		    if(currentCatId == "1") uncategorized = i;
		    cw.chNoCats->setEnabled(true);
		    cw.cbMainCat->setEnabled(true);
		    cw.lwOtherCats->setEnabled(true);
		    statusWidget->clearMessage();	// as otherwise, "there are no categories" would still show
		}
		if(uncategorized) cw.cbMainCat->setCurrentIndex(uncategorized);
		emit categoryRefreshFinished();
		if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
	    }else callRefreshCategories();
	}else callRefreshCategories();
    }
}

bool EditingWindow::caseInsensitiveLessThan(const QString &s1, const QString &s2){
    return s1.toLower() < s2.toLower();
}

void EditingWindow::handleConsole(bool isChecked){
    QAction *menuConsoleAction, *menuPreviewAction;
    if(superMenu){
	menuConsoleAction	= qobject_cast<QAction *>(superMenu->getConsoleAction());
	menuPreviewAction	= qobject_cast<QAction *>(superMenu->getPreviewAction());
	if(menuConsoleAction) menuConsoleAction->setChecked(isChecked);
    }
    if(! isChecked){
	mainStack->setCurrentIndex(edID);
	searchWidget->setTextEdit(EDITOR);
	if(menuConsoleAction) menuConsoleAction->setText(tr("&Console"));
	if(menuPreviewAction) menuPreviewAction->setEnabled(true);
    }else{
	previousRaisedLSWidget = mainStack->currentIndex();
	mainStack->setCurrentIndex(consoleID);
	searchWidget->setTextEdit(console);
	if(menuConsoleAction) menuConsoleAction->setText(tr("Exit &console"));
	if(menuPreviewAction) menuPreviewAction->setEnabled(false);
	console->setFocus(Qt::OtherFocusReason);
    }
}

void EditingWindow::clearConsole(){
    console->clear();
}

void EditingWindow::setToolBarVisible(bool vis){
    toolBar->setVisible(vis);
}

void EditingWindow::makeBold(){
    EDITOR->insertPlainText(QString("<strong>%1</strong>").arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::makeItalic(){
    EDITOR->insertPlainText(QString("<em>%1</em>").arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::makeUnderline(){
    EDITOR->insertPlainText(QString("<u>%1</u>").arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::insertMore(){
    if(! EDITOR->toPlainText().contains("<!--more-->")) EDITOR->insertPlainText("<!--more-->");
    else statusWidget->showMessage(tr("A 'more' tag already exists."), 2000);
}

void EditingWindow::makeBlockquote(){
    EDITOR->insertPlainText(QString("<blockquote>%1</blockquote>")
	.arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::makePara(){
    EDITOR->insertPlainText(QString("<p>%1</p>").arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::insertLink(bool isAutoLink){
    QString		linkString, titleString;
    QString		insertionString = "";
    QString		selectedString	= EDITOR->textCursor().selectedText();
    QString		selectedStringLC;
    QDialog		linkEntry(this);
    Ui::LinkEntry	leui;
#ifdef Q_OS_MAC
    linkEntry.setWindowFlags(Qt::Sheet);
#endif
    leui.setupUi(&linkEntry);
    if(isAutoLink) leui.cbMakeAutoLink->setChecked(Qt::Checked);
    if(! selectedString.isEmpty()) leui.leLinkText->setText(selectedString);
    if(linkEntry.exec()){
	linkString	= leui.leLinkURL->text();
	insertionString += QString("<a href=\"%1\"").arg(linkString);
	if(leui.leLinkTitle->text().size()){
	    titleString		= leui.leLinkTitle->text();
	    insertionString	+= QString(" title=\"%1\"").arg(titleString.replace("\"", "&quot;"));
	}
	if(leui.chLinkTarget->isChecked()){
	    switch(leui.cbLinkTarget->currentIndex()){
		case 0: insertionString += QString(" target=\"_top\"");break;
		case 1: insertionString += QString(" target=\"_blank\"");break;
		case 2: insertionString += QString(" target=\"view_window\"");break;
	    }
	}
	insertionString += ">";
	EDITOR->insertPlainText(QString("%1%2</a>")
	    .arg(insertionString)
	    .arg(leui.leLinkText->text()));
    }
    if(leui.cbMakeAutoLink->isChecked()){
	selectedStringLC = selectedString.toLower().trimmed();
	autoLinkDictionary.insert(selectedStringLC, linkString);
	autoLinkTitleDictionary.insert(selectedStringLC, titleString);
	if(leui.chLinkTarget->isChecked()) autoLinkTargetDictionary.insert(selectedStringLC, leui.cbLinkTarget->currentIndex());
	else autoLinkTargetDictionary.remove(selectedStringLC);
	saveAutoLinkDictionary();
    }
}

void EditingWindow::insertLinkFromClipboard(){
    QString linkString(QApplication::clipboard()->text());
    EDITOR->insertPlainText(QString("<a href=\"%1\">%2</a>")
	.arg(linkString)
	.arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::insertSelfLink(){
    QString linkString(EDITOR->textCursor().selectedText());
    if(QUrl(linkString, QUrl::StrictMode).isValid()) EDITOR->insertPlainText(QString("<a href=\"%1\">%1</a>").arg(linkString));
    else statusWidget->showMessage(tr("The selection is not a valid URL."), 2000);
}

void EditingWindow::insertAutoLink(){
    QString		selectedText = EDITOR->textCursor().selectedText();
    QString		selectedTextLC = selectedText.toLower().trimmed();
    QString		titleString, targetString;
    QList<QString>	targets;
    targets << "_top" << "_blank" << "view_window";
    if(autoLinkDictionary.contains(selectedTextLC)){
	titleString = QString(" title=\"%1\"")
	    .arg(autoLinkTitleDictionary.value(selectedTextLC));
	if(autoLinkTargetDictionary.contains(selectedTextLC)) targetString = QString(" target=\"%1\"")
		    .arg(targets.value(autoLinkTargetDictionary.value(selectedTextLC)));
	EDITOR->insertPlainText(QString("<a href=\"%1\"%2%3>%4</a>")
	    .arg(autoLinkDictionary.value(selectedTextLC))
	    .arg(titleString)
	    .arg(targetString)
	    .arg(selectedText));
    }else insertLink(true);
}

void EditingWindow::insertImage(){
    QString	insertionString, styleString;
    QStringList borderStyles;
    borderStyles << "dotted" << "dashed" << "solid" << "double";

    QDialog		image_entry(this);
    Ui::ImageEntry	ieui;
#ifdef Q_OS_MAC
    image_entry.setWindowFlags(Qt::Sheet);
#endif
    ieui.setupUi(&image_entry);
    ieui.gbMargins->setVisible(false);
    if(image_entry.exec()){
	insertionString = QString("<img src=\"%1\"")
	    .arg(ieui.leImageURL->text());
	if(ieui.leTitle->text().size()) insertionString += QString(" title=\"%1\"")
		    .arg(ieui.leTitle->text().replace("\"", "&quot;"));
	if(ieui.leAltText->text().size()) insertionString += QString(" alt=\"%1\"")
		    .arg(ieui.leAltText->text().replace("\"", "&quot;"));
	// Alignment
	if(ieui.cbAlign->currentIndex()){
	    if(ieui.cbAlign->currentIndex() == 1) styleString += "float: left; ";
	    else// index is 2
			styleString += "float: right; ";
	}
	// Border
	if(ieui.cbBorderStyle->currentIndex() != 0 && ieui.sbBorderWidth->value() != 0){
	    styleString += QString("border: %1px %2; ")
		.arg(ieui.sbBorderWidth->value())
		.arg(borderStyles[ieui.cbBorderStyle->currentIndex() - 1]);
	}
	// Margins
	if(ieui.sbMarginLeft->value() != 0) styleString += QString("margin-left: %1px; ").arg(ieui.sbMarginLeft->value());
	if(ieui.sbMarginRight->value() != 0) styleString += QString("margin-right: %1px; ").arg(ieui.sbMarginRight->value());
	if(ieui.sbMarginTop->value() != 0) styleString += QString("margin-top: %1px; ").arg(ieui.sbMarginTop->value());
	if(ieui.sbMarginBottom->value() != 0) styleString += QString("margin-bottom: %1px").arg(ieui.sbMarginBottom->value());
	// Insert style string into main insertion string
	if(! styleString.isEmpty()){
	    insertionString += QString(" style=\"%1\"")
		.arg(styleString.trimmed());
	}
	insertionString += " />";
	EDITOR->insertPlainText(insertionString);
    }
}

void EditingWindow::insertImageFromClipboard(){
    QString linkString(QApplication::clipboard()->text());
    EDITOR->insertPlainText(QString("<img src=\"%1\">%2</img>")
	.arg(linkString)
	.arg(EDITOR->textCursor().selectedText()));
}

void EditingWindow::cut(){
    EDITOR->cut();
}

void EditingWindow::copy(){
    if(mainStack->currentIndex() == previewWindowID) previewWindow->copy();
    else EDITOR->copy();
}

QString EditingWindow::removeReadMore(QString &source){
	// List of regexes to match "read more" strings. Will keep the list
	// in case other variants are found. (Previously had list of specific
	// websites.)
    QList<QRegExp> regExpList;
    regExpList << QRegExp("\\n\\nRead more:? http://(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\\-]*[A-Za-z0-9]).*$");
    if(suppressReadMore){
	// qDebug() << "suppressing read more";
	QString text = source;
	Q_FOREACH(QRegExp rx, regExpList){
	    if(text.contains(rx)){
		text.remove(rx);
		return text;
	    }
	}
    }
    return source;
}

void EditingWindow::paste(){
    QString pastedText = QApplication::clipboard()->text();
    EDITOR->insertPlainText(removeReadMore(pastedText));
    EDITOR->ensureCursorVisible();
}

void EditingWindow::tidyPaste(){
    QString pastedText = QApplication::clipboard()->text();
    pastedText = removeReadMore(pastedText);
    pastedText.replace(QRegExp(" {2,}"), " ");
    pastedText.replace(QRegExp(" *\n{1} *"), " ");
    EDITOR->insertPlainText(pastedText);
    EDITOR->ensureCursorVisible();
}

void EditingWindow::undo(){
    EDITOR->document()->undo();
}

void EditingWindow::redo(){
    EDITOR->document()->redo();
}

void EditingWindow::makeUnorderedList(){
    QString listString = EDITOR->textCursor().selection().toPlainText();
    if(! listString.isEmpty()) EDITOR->insertPlainText(getHTMLList(QString("ul"), listString));
}

void EditingWindow::makeOrderedList(){
    QString listString = EDITOR->textCursor().selection().toPlainText();
    if(! listString.isEmpty()) EDITOR->insertPlainText(getHTMLList(QString("ol"), listString));
}

QString &EditingWindow::getHTMLList(QString tag, QString &text){
    QString	return_value, workstring;
    QStringList worklist;

    worklist = text.split("\n");
    for(int a = 0; a < worklist.count(); a ++){
	worklist[a].prepend("<li>");
	worklist[a].append("</li>");
    }
    text = worklist.join("\n");
    text.prepend(QString("<%1>").arg(tag));
    text.append(QString("</%1>").arg(tag));

    return text;
}

void EditingWindow::pasteAsMarkedParagraphs(){
    QString insertion = QApplication::clipboard()->text().trimmed();
    insertion = removeReadMore(insertion);
    if(! insertion.isEmpty()){
	insertion.replace(QRegExp("\n+"), "</p>\n<p>");
	insertion.prepend("<p>");
	insertion.append("</p>");
	EDITOR->insertPlainText(insertion);
	EDITOR->ensureCursorVisible();
    }
}

void EditingWindow::pasteAsBlockquote(){
    QString insertion = QApplication::clipboard()->text().trimmed();
    insertion = removeReadMore(insertion);
    if(! insertion.isEmpty()){
	insertion.replace(QRegExp("\n{1,2}"), "\n\n");
	EDITOR->insertPlainText(QString("<blockquote>%1</blockquote>")
	    .arg(insertion));
	EDITOR->ensureCursorVisible();
    }
}

void EditingWindow::pasteAsMarkdownBlockquote(){
    QString insertion = QApplication::clipboard()->text().trimmed();
    insertion = removeReadMore(insertion);
    QString separator = "\n>\n";
    if(! insertion.isEmpty()){
	insertion.prepend(">");
	insertion.replace(QRegExp("\n{1,2}"), "\n>\n>");
	EDITOR->insertPlainText(insertion);
	EDITOR->ensureCursorVisible();
    }
}

void EditingWindow::pasteAsUnorderedList(){
    QString insertion = QApplication::clipboard()->text().trimmed();
    insertion = removeReadMore(insertion);
    insertion.replace(QRegExp("\n{2,}"), "\n");
    if(! insertion.isEmpty()){
	EDITOR->insertPlainText(getHTMLList(QString("ul"), insertion));
	EDITOR->ensureCursorVisible();
    }
}

void EditingWindow::pasteAsOrderedList(){
    QString insertion = QApplication::clipboard()->text().trimmed();
    insertion = removeReadMore(insertion);
    insertion.replace(QRegExp("\n{2,}"), "\n");
    if(! insertion.isEmpty()){
	EDITOR->insertPlainText(getHTMLList(QString("ol"), insertion));
	EDITOR->ensureCursorVisible();
    }
}

void EditingWindow::doPreview(bool isChecked, bool markdownFailed){
    qDebug() << "preview starting";
    QString		line, techTagString;
    QString		conversionString = "", conversionStringB = "";
    QTextDocument	cDoc;
	// bool finished = false;
    bool isPre = false;

    QAction *menuPreviewAction;
    if(superMenu){
	menuPreviewAction = qobject_cast<QAction *>(superMenu->getPreviewAction());
	if(menuPreviewAction) menuPreviewAction->setChecked(isChecked);
	action_Preview->setChecked(isChecked);
    }
    if(isChecked){
	if(useMarkdown && ! markdownFailed){
	    conversionStringB = processWithMarkdown(EDITOR->toPlainText());
	    if(conversionStringB.isNull()){
		qDebug() << "Markdown conversion failed";
		doPreview(isChecked, true);
		return;
	    }
		// Now that the process has done its job, we can add the title and display
		// the preview

	    conversionStringB.prepend(QString("<strong>%1</strong>\n\n")
		.arg(cw.leTitle->text().size()
		? cw.leTitle->text() : "<em>Untitled</em>"));
	    previewWindow->setHtml(conversionStringB);
	}else{				// i.e. if not Markdown or Markdown failed
	    conversionString += QString("<b>%1</b>\n\n")
		.arg(cw.leTitle->text().size()
		    ? cw.leTitle->text() : "<i>Untitled</i>");
	    conversionString += EDITOR->toPlainText();
	    QTextStream a(&conversionString);
	    QRegExp	re("^(<table|thead|tfoot|caption|tbody|tr|td|th|div|dl|dd|dt|ul|ol|li|select|form|blockquote|address|math|p|h[1-6])>");
	    do {
		line = a.readLine();
		if(! line.isEmpty()){
		    if(re.exactMatch(line)) conversionStringB += line;
		    else{
			if(line.startsWith("<pre>")){
			    isPre		= true;
			    conversionStringB	+= line;
			}else{
			    conversionStringB += QString(isPre ? "%1\n" : "<p>%1</p>").arg(line);
			    if(line.contains("</pre>")) isPre = false;
			}
		    }
		}
	    } while(! a.atEnd());
	    previewWindow->setHtml(conversionStringB);
	}
	previousRaisedLSWidget = mainStack->currentIndex();
	connect(previewWindow, SIGNAL(highlighted(const QString&))
	       , this, SLOT(showHighlightedURL(const QString&)));
	mainStack->setCurrentIndex(previewWindowID);
	searchWidget->setTextEdit(previewWindow);
	action_Preview->setText(tr("Exit p&review"));
	action_Preview->setIconText(tr("Exit preview"));
	action_Preview->setToolTip(tr("Exit preview"));
#ifdef Q_OS_MAC
	if(menuPreviewAction) menuPreviewAction->setText(tr("Exit p&review"));
	if(superMenu) superMenu->setConsoleEnabled(false);
#endif
	previewWindow->setFocus(Qt::OtherFocusReason);
	leftWidget->setEnabled(false);
	setEditActionsEnabled(false);
	searchWidget->close();
    }else{				// i.e. if the preview button is being released
	// mainStack->setCurrentIndex( previousRaisedLSWidget );
	mainStack->setCurrentIndex(edID);
	previewWindow->disconnect(SIGNAL(highlighted(const QString&)));
	searchWidget->setTextEdit(EDITOR);
	action_Preview->setText(tr("Entry in p&review"));
	action_Preview->setIconText(tr("Entry in preview"));
	action_Preview->setToolTip(tr("Entry in preview"));
#ifdef Q_OS_MAC
	if(menuPreviewAction) menuPreviewAction->setText(tr("Entry in p&review"));
	if(superMenu) superMenu->setConsoleEnabled(true);
#endif
	leftWidget->setEnabled(true);
	setEditActionsEnabled(true);
	searchWidget->close();
    }
}

void EditingWindow::setEditActionsEnabled(bool state){
    if(superMenu) superMenu->setEditActionsEnabled(state);
    QList<QAction *> editActions;

    editActions << action_Bold << action_Italic << action_Underline
		<< action_Blockquote << action_Link << action_Image << action_More;
    Q_FOREACH(QAction *a, editActions) a->setEnabled(state);
#ifndef Q_OS_MAC
    if(superMenu){
	superMenu->insMenu->setEnabled(state);
	superMenu->pasteAsMenu->setEnabled(state);
	superMenu->categoryMenu->setEnabled(state);
    }
#endif
}

void EditingWindow::showHighlightedURL(const QString &highlightedURL){
    statusWidget->showMessage(highlightedURL, 2000);
}

void EditingWindow::blogThis(){
    if(useWordpressAPI){
	if(entryBlogged) submitWPEdit();
	else newWPPost();
    }else newMTPost();
}

void EditingWindow::newMTPost(){
    QDomDocument	doc;
    QDomElement		methodCall, params, param, member, value, integer
    , string, rpcstruct, rpcarray, actualValue;
    QString	description, extEntry, techTagString, convertedString, keywordTagList;
    bool	takeComms	= cw.chAllowComments->isChecked();
    bool	takeTB		= cw.chAllowTB->isChecked();
	// bool blogidIsInt;
    int			count, tags;
    QList<QString>	tblist;
    if(! currentHttpBusiness){
	if(! entryBlogged){
	    if(EDITOR->toPlainText().contains("<!--more-->")){
		description = QString(EDITOR->toPlainText())
		    .section("<!--more-->", 0, 0);
		extEntry = QString(EDITOR->toPlainText())
		    .section("<!--more-->", - 1, - 1);
	    }else{
		description	= QString(EDITOR->toPlainText());
		extEntry	= "";
	    }
	    if(doMarkdownWhenPosting){
		convertedString = processWithMarkdown(description);
		if(! convertedString.isNull())
			/* description = stripParaTags ?
			   convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
			description = convertedString;
		else statusWidget->showMessage(tr("Markdown conversion failed; posting main entry as is."), 2000);
		if(! extEntry.isEmpty()){
		    convertedString = processWithMarkdown(extEntry);
		    if(! convertedString.isNull())
				/*extEntry = stripParaTags ?
				  convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
				extEntry = convertedString;
		    else statusWidget->showMessage(tr("Markdown conversion failed; posting extension as is."), 2000);
		}
	    }
	    dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
	    dateTime.remove(QChar('-'));
	    if(cw.lwTags->count()){
		tags		= cw.lwTags->count();
		techTagString	= "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
		for(count = 0; count < tags; count ++){
		    techTagString.append(QString("<a href=\"http://technorati.com/tag/%1\""
						 " rel=\"tag\">%2</a>%3")
			.arg(cw.lwTags->item(count)->text().replace(' ', '+'))
			.arg(cw.lwTags->item(count)->text()
			.replace("+", " "))
			.arg((count == tags - 1) ? "</p>\n\n" : ", "));
		}
		if(cw.rbStartOfMainEntry->isChecked()) description.insert(0, techTagString);
		else description.append(techTagString);
	    }
	    QString blogid = cw.cbBlogSelector->itemData(cw.cbBlogSelector->currentIndex()).toString();
/*      QRegExp blogidRegExp( "^[0-9]+$" );
      blogidIsInt = blogidRegExp.exactMatch( blogid ); */

	    methodCall = doc.createElement("methodCall");
	    methodCall.appendChild(XmlMethodName(doc, "metaWeblog.newPost"));

	    params = doc.createElement("params");
	    params.appendChild(XmlValue(doc, "string", blogid));
	    params.appendChild(XmlValue(doc, "string", login));
	    params.appendChild(XmlValue(doc, "string", password));

	    param	= doc.createElement("param");
	    value	= doc.createElement("value");
	    rpcstruct	= doc.createElement("struct");
	    rpcstruct.appendChild(XmlMember(doc, "title", "string"
					   , cw.leTitle->text()));
	    rpcstruct.appendChild(XmlMember(doc, "description", "string", description));
	    if(postDateTime) rpcstruct.appendChild(XmlMember(doc, "dateCreated", "dateTime.iso8601", dateTime));
	    rpcstruct.appendChild(XmlMember(doc, "mt_allow_comments", "boolean"
					   , takeComms ? "1" : "0"));
	    rpcstruct.appendChild(XmlMember(doc, "mt_allow_pings", "boolean"
					   , takeTB ? "1" : "0"));
	    rpcstruct.appendChild(XmlMember(doc, "mt_text_more", "string", extEntry));
	    if(cw.teExcerpt->toPlainText().length()) rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string"
										    , cw.teExcerpt->toPlainText().replace(QChar('&'), "&amp;")));
	    else rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string", ""));
	    if(! cw.lwKeywordTags->count()) rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", ""));
	    else{
		for(count = 0; count < cw.lwKeywordTags->count(); count ++){
		    keywordTagList.append(cw.lwKeywordTags->item(count)->text());
		    if(count != cw.lwKeywordTags->count() - 1) keywordTagList.append(", ");
		}
		rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", keywordTagList));
	    }
	    if(cw.lwTBPings->count()){
		for(count = 0; count < cw.lwTBPings->count(); count ++) tblist.append(cw.lwTBPings->item(count)->text());
		rpcstruct.appendChild(XmlRpcArray(doc, "mt_tb_ping_urls", tblist));
	    }
	    value.appendChild(rpcstruct);
	    param.appendChild(value);
	    params.appendChild(param);
	    if(! useWordpressAPI) params.appendChild(XmlValue(doc, "boolean"
							     , cw.cbStatus->currentIndex() ? "1" : "0"));
	    methodCall.appendChild(params);
	    doc.appendChild(methodCall);
	    doc.insertBefore(doc.createProcessingInstruction("xml"
							    , "version=\"1.0\" encoding=\"UTF-8\"")
			    , doc.firstChild());

	    QByteArray requestArray(doc.toByteArray());
	    responseData	= "";
	    doingNewPost	= true;
	    placeNetworkRequest(_metaWeblog_newPost, requestArray);
	    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	    if(postAsSave && ! entryEverSaved){
		cleanSave = true;
		setDirtySignals(true);
	    }
	    if(location.contains("mt-xmlrpc.cgi") && cw.cbStatus->currentIndex() == 1) cw.progressBar->setMaximum((categoriesEnabled && ! cw.chNoCats->isChecked()) ? 6 : 4);
	    else cw.progressBar->setMaximum((categoriesEnabled && ! cw.chNoCats->isChecked()) ? 4 : 2);
	    cw.progressBar->setValue(1);
	    cw.progressWidget->setVisible(true);

#ifdef WIN_QTV
	    addToConsole("Showing taskbar button");
	    qDebug() << "Showing taskbar button";
	    if((useWordpressAPI || location.contains("mt-xmlrpc.cgi")) && cw.cbStatus->currentIndex() == 1) taskbarProgress->setMaximum(6);
	    else taskbarProgress->setMaximum(4);
	    taskbarProgress->setValue(1);
	    taskbarProgress->show();
#endif

	    cw.cbPageSelector->setCurrentIndex(0);
	    cw.stackedWidget->setCurrentIndex(0);
/*#ifndef Q_OS_MAC
      progressBarAction->setVisible( true );
#endif */
	}else submitMTEdit();
    }else{
#ifdef QTM_DEBUG
	statusWidget->showMessage(tr("newPost: All HTTP requests are blocked")
				 , 2000);
#else
	statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
}

void EditingWindow::newWPPost(){
    QDomDocument	doc;
    QDomElement		methodCall, params, param, member, value, integer
    , string, rpcstruct, rpcarray, actualValue, primCatElem
    , taxStruct, taxValue, taxMember, taxMemberName;
    QString	primCat, blogid, entryText, techTagString, convertedString, keywordTagList, tblist;
    bool	takeComms	= cw.chAllowComments->isChecked();
    bool	takeTB		= cw.chAllowTB->isChecked();
	// bool blogidIsInt;
    int			count, tags;
    QList<QString>	catList, tagList;
    if(! currentHttpBusiness){
	entryText = EDITOR->toPlainText();
	// Do Markdown, if enabled
	if(doMarkdownWhenPosting){
	    convertedString = processWithMarkdown(entryText);
	    if(! convertedString.isNull())
			/* description = stripParaTags ?
			   convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
			entryText = convertedString;
	    else statusWidget->showMessage(tr("Markdown conversion failed; posting main entry as is."), 2000);
	}
	dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
	dateTime.remove(QChar('-'));
	// Make a string of the Technorati tags
	if(cw.lwTags->count()){
	    tags		= cw.lwTags->count();
	    techTagString	= "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
	    for(count = 0; count < tags; count ++){
		techTagString.append(QString("<a href=\"http://technorati.com/tag/%1\""
					     " rel=\"tag\">%2</a>%3")
		    .arg(cw.lwTags->item(count)->text().replace(' ', '+'))
		    .arg(cw.lwTags->item(count)->text()
		    .replace("+", " "))
		    .arg((count == tags - 1) ? "</p>\n\n" : ", "));
	    }
		// Whether to put the Technorati tags at beginning or end
	    if(cw.rbStartOfMainEntry->isChecked()) entryText.insert(0, techTagString);
	    else entryText.append(techTagString);
	}
	// Make a list of the selected categories (WP API has no "primary category")
	if(cw.chAllowComments->isChecked()){
		/*primCatElem = currentBlogElement.firstChildElement( "categories" )
		  .childNodes().at( cw.cbMainCat->currentIndex() ).toElement(); */
	    catList << cw.cbMainCat->itemData(cw.cbMainCat->currentIndex()).toString();
	    for(int a = 0; a < cw.lwOtherCats->count(); a ++){
		if(cw.lwOtherCats->isItemSelected(cw.lwOtherCats->item(a))){
			// cat = currentBlogElement.firstChildElement( "categories" ).childNodes().at( a ).toElement();
		    catList << cw.cbMainCat->itemData(a).toString();
		}
	    }
	}
	// Make a list of selected WP tags
	if(cw.lwKeywordTags->count())
		for(count = 0; count < cw.lwKeywordTags->count(); count ++) tagList << cw.lwKeywordTags->item(count)->text();
	// Make a list of trackback pings to be sent
	if(cw.lwTBPings->count()){
	    for(count = 0; count < cw.lwTBPings->count(); count ++){
		if(count) tblist.append(' ');
		tblist.append(cw.lwTBPings->item(count)->text());
	    }
	}
	blogid = cw.cbBlogSelector->itemData(cw.cbBlogSelector->currentIndex()).toString();

	methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "wp.newPost"));

	params = doc.createElement("params");
	params.appendChild(XmlValue(doc, "string", blogid));
	params.appendChild(XmlValue(doc, "string", login));
	params.appendChild(XmlValue(doc, "string", password));

	param		= doc.createElement("param");
	value		= doc.createElement("value");
	rpcstruct	= doc.createElement("struct");
	rpcstruct.appendChild(XmlMember(doc, "post_type", "string", "post"));
	rpcstruct.appendChild(XmlMember(doc, "post_status", "string"
				       , cw.cbStatus->currentIndex() ? "publish" : "draft"));
	rpcstruct.appendChild(XmlMember(doc, "post_title", "string"
				       , cw.leTitle->text()));
	rpcstruct.appendChild(XmlMember(doc, "post_content", "string", entryText));
	if(postDateTime) rpcstruct.appendChild(XmlMember(doc, "post_date", "dateTime.iso8601", dateTime));
	rpcstruct.appendChild(XmlMember(doc, "comment_status", "string"
				       , takeComms ? "open" : "closed"));
	rpcstruct.appendChild(XmlMember(doc, "ping_status", "string"
				       , takeTB ? "open" : "closed"));
	if(! cw.lePassword->text().isEmpty()) rpcstruct.appendChild(XmlMember(doc, "post_password", "string"
									     , cw.lePassword->text()));
	/* if( cw.chSticky->isChecked() )
	  rpcstruct.appendChild( XmlMember( doc, "sticky", "bool", "true" ) ); */
	if(catList.count()){
	    taxMember		= doc.createElement("member");
	    taxMemberName	= doc.createElement("name");
	    taxMemberName.appendChild(QDomText(doc.createTextNode("terms")));
	    taxMember.appendChild(taxMemberName);
	    taxValue	= doc.createElement("value");
	    taxStruct	= doc.createElement("struct");
	    taxStruct.appendChild(XmlRpcArray(doc, "category", "string", catList));
	    taxValue.appendChild(taxStruct);
	    taxMember.appendChild(taxValue);
	    rpcstruct.appendChild(taxMember);
	}
	if(tagList.count()) rpcstruct.appendChild(XmlRpcArray(doc, "tags_input", "string", tagList));
	if(tblist.length()) rpcstruct.appendChild(XmlMember(doc, "to_ping", "string", tblist));
	value.appendChild(rpcstruct);
	param.appendChild(value);
	params.appendChild(param);
	methodCall.appendChild(params);
	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml"
							, "version=\"1.0\" encoding=\"UTF-8\"")
			, doc.firstChild());

	QByteArray requestArray(doc.toByteArray());
	responseData = "";
	placeNetworkRequest(_wp_newPost, requestArray);
	if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	if(postAsSave && ! entryEverSaved){
	    cleanSave = true;
	    setDirtySignals(true);
	}
	cw.progressBar->setMaximum(2);
	cw.progressBar->setValue(1);
	cw.progressWidget->setVisible(true);
	cw.cbPageSelector->setCurrentIndex(0);
	cw.stackedWidget->setCurrentIndex(0);
    }else{
#ifdef QTM_DEBUG
	statusWidget->showMessage(tr("refreshCategories: All HTTP requests are blocked")
				 , 2000);
#else
	statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
}

void EditingWindow::submitMTEdit(){
    QDomDocument	doc;
    QDomElement		methodCall, params, param, value, rpcstruct, rpcarray;
    QString		description, extEntry, techTagString, convertedString, keywordTagList;
    int			count, tags;
    bool		takeComms	= cw.chAllowComments->isChecked();
    bool		takeTB		= cw.chAllowTB->isChecked();
    QList<QString>	tblist;
    if((! useWordpressAPI) && EDITOR->toPlainText().contains("<!--more-->")){
	description = QString(EDITOR->toPlainText())
	    .section("<!--more-->", 0, 0);
	extEntry = QString(EDITOR->toPlainText())
	    .section("<!--more-->", - 1, - 1);
    }else{
	description	= QString(EDITOR->toPlainText());
	extEntry	= "";
    }
    if(doMarkdownWhenPosting){
	convertedString = processWithMarkdown(description);
	if(! convertedString.isNull())
		/* description = stripParaTags ?
		  convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
		description = convertedString;
	else statusWidget->showMessage(tr("Markdown conversion failed; posting main entry as is."), 2000);
	if(! extEntry.isEmpty()){
	    convertedString = processWithMarkdown(extEntry);
	    if(! convertedString.isNull())
			/* extEntry = stripParaTags ?
			  convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
			extEntry = convertedString;
	    else statusWidget->showMessage(tr("Markdown conversion failed; posting extension as is."), 2000);
	}
    }
    if(cw.lwTags->count()){
	tags		= cw.lwTags->count();
	techTagString	= "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
	for(count = 0; count < tags; count ++){
	    techTagString.append(QString("<a href=\"http://technorati.com/tag/%1\""
					 " rel=\"tag\">%2</a>%3")
		.arg(cw.lwTags->item(count)->text())
		.arg(cw.lwTags->item(count)->text()
		.replace("+", " "))
		.arg((count == tags - 1) ? "</p>" : ", "));
	}
	if(cw.rbStartOfMainEntry->isChecked()) description.insert(0, techTagString);
	else description.append(techTagString);
    }
    methodCall = doc.createElement("methodCall");
    methodCall.appendChild(XmlMethodName(doc, "metaWeblog.editPost"));

    params = doc.createElement("params");
    params.appendChild(XmlValue(doc, "string", entryNumber));
    params.appendChild(XmlValue(doc, "string", login));
    params.appendChild(XmlValue(doc, "string", password));

    param	= doc.createElement("param");
    value	= doc.createElement("value");
    rpcstruct	= doc.createElement("struct");
    rpcstruct.appendChild(XmlMember(doc, "title", "string"
				   , cw.leTitle->text()));
    rpcstruct.appendChild(XmlMember(doc, "description", "string", description));
    if(postDateTime) rpcstruct.appendChild(XmlMember(doc, "dateCreated", "dateTime.iso8601", dateTime));
    rpcstruct.appendChild(XmlMember(doc, "mt_allow_comments", "boolean"
				   , takeComms ? "1" : "0"));
    rpcstruct.appendChild(XmlMember(doc, "mt_allow_pings", "boolean"
				   , takeTB ? "1" : "0"));
    rpcstruct.appendChild(XmlMember(doc, "mt_text_more", "string", extEntry));
    if(cw.teExcerpt->toPlainText().length()) rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string"
									    , cw.teExcerpt->toPlainText().replace(QChar('&'), "&amp;")));
    else rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string", ""));
    if(! cw.lwKeywordTags->count()) rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", ""));
    else{
	for(count = 0; count < cw.lwKeywordTags->count(); count ++){
	    keywordTagList.append(cw.lwKeywordTags->item(count)->text());
	    if(count != cw.lwKeywordTags->count() - 1) keywordTagList.append(", ");
	}
	rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", keywordTagList));
    }
    if(cw.lwTBPings->count()){
	for(count = 0; count < cw.lwTBPings->count(); count ++) tblist.append(cw.lwTBPings->item(count)->text());
	rpcstruct.appendChild(XmlRpcArray(doc, "mt_tb_ping_urls", tblist));
    }
    value.appendChild(rpcstruct);
    param.appendChild(value);
    params.appendChild(param);
    params.appendChild(XmlValue(doc, "boolean"
			       , cw.cbStatus->currentIndex() ? "1" : "0"));
    methodCall.appendChild(params);
    doc.appendChild(methodCall);
    doc.insertBefore(doc.createProcessingInstruction("xml"
						    , "version=\"1.0\" encoding=\"UTF-8\"")
		    , doc.firstChild());

    QByteArray requestArray(doc.toByteArray());
    responseData = "";
    placeNetworkRequest(_metaWeblog_editPost, requestArray);
    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    if(postAsSave && ! entryEverSaved){
	cleanSave = true;
	setDirtySignals(true);
    }
    if(location.contains("mt-xmlrpc.cgi") && cw.cbStatus->currentIndex() == 1) cw.progressBar->setMaximum((categoriesEnabled && cw.chNoCats->isChecked()) ? 6 : 4);
    else cw.progressBar->setMaximum((categoriesEnabled && cw.chNoCats->isChecked()) ? 4 : 2);
    cw.progressBar->setValue(1);
    cw.progressWidget->setVisible(true);

#ifdef WIN_QTV
    if(location.contains("mt-xmlrpc.cgi") && cw.cbStatus->currentIndex() == 1) taskbarProgress->setMaximum(6);
    else taskbarProgress->setMaximum(4);
    taskbarProgress->setValue(1);
    taskbarProgress->setVisible(true);
#endif

    cw.cbPageSelector->setCurrentIndex(0);
    cw.stackedWidget->setCurrentIndex(0);
}

void EditingWindow::submitWPEdit(){
    QDomDocument	doc;
    QDomElement		methodCall, params, param, member, value, integer
    , string, rpcstruct, rpcarray, actualValue, primCatElem
    , taxStruct, taxValue, taxMember, taxMemberName;
    QString	primCat, blogid, entryText, techTagString, convertedString, keywordTagList, tblist;
    bool	takeComms	= cw.chAllowComments->isChecked();
    bool	takeTB		= cw.chAllowTB->isChecked();
	// bool blogidIsInt;
    int			count, tags;
    QList<QString>	catList, tagList;
    if(! currentHttpBusiness){
	entryText = EDITOR->toPlainText();
	// Do Markdown, if enabled
	if(doMarkdownWhenPosting){
	    convertedString = processWithMarkdown(entryText);
	    if(! convertedString.isNull())
			/* description = stripParaTags ?
			   convertedString.remove( "<p>" ).remove( "</p>" ) : convertedString; */
			entryText = convertedString;
	    else statusWidget->showMessage(tr("Markdown conversion failed; posting main entry as is."), 2000);
	}
	dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
	dateTime.remove(QChar('-'));
	// Make a string of the Technorati tags
	if(cw.lwTags->count()){
	    tags		= cw.lwTags->count();
	    techTagString	= "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
	    for(count = 0; count < tags; count ++){
		techTagString.append(QString("<a href=\"http://technorati.com/tag/%1\""
					     " rel=\"tag\">%2</a>%3")
		    .arg(cw.lwTags->item(count)->text().replace(' ', '+'))
		    .arg(cw.lwTags->item(count)->text()
		    .replace("+", " "))
		    .arg((count == tags - 1) ? "</p>\n\n" : ", "));
	    }
		// Whether to put the Technorati tags at beginning or end
	    if(cw.rbStartOfMainEntry->isChecked()) entryText.insert(0, techTagString);
	    else entryText.append(techTagString);
	}
	// Make a list of the selected categories (WP API has no "primary category")
	if(cw.chAllowComments->isChecked()){
		/*primCatElem = currentBlogElement.firstChildElement( "categories" )
		  .childNodes().at( cw.cbMainCat->currentIndex() ).toElement(); */
	    catList << cw.cbMainCat->itemData(cw.cbMainCat->currentIndex()).toString();
	    for(int a = 0; a < cw.lwOtherCats->count(); a ++){
		if(cw.lwOtherCats->isItemSelected(cw.lwOtherCats->item(a))){
			// cat = currentBlogElement.firstChildElement( "categories" ).childNodes().at( a ).toElement();
		    catList << cw.cbMainCat->itemData(a).toString();
		}
	    }
	}
	// Make a list of selected WP tags
	if(cw.lwKeywordTags->count())
		for(count = 0; count < cw.lwKeywordTags->count(); count ++) tagList << cw.lwKeywordTags->item(count)->text();
	// Make a list of trackback pings to be sent
	if(cw.lwTBPings->count()){
	    for(count = 0; count < cw.lwTBPings->count(); count ++){
		if(count) tblist.append(' ');
		tblist.append(cw.lwTBPings->item(count)->text());
	    }
	}
	blogid = cw.cbBlogSelector->itemData(cw.cbBlogSelector->currentIndex()).toString();

	methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "wp.editPost"));

	params = doc.createElement("params");
	params.appendChild(XmlValue(doc, "int", blogid));
	params.appendChild(XmlValue(doc, "string", login));
	params.appendChild(XmlValue(doc, "string", password));
	params.appendChild(XmlValue(doc, "int", entryNumber));

	param		= doc.createElement("param");
	value		= doc.createElement("value");
	rpcstruct	= doc.createElement("struct");
	rpcstruct.appendChild(XmlMember(doc, "post_status", "string"
				       , cw.cbStatus->currentIndex() ? "publish" : "draft"));
	rpcstruct.appendChild(XmlMember(doc, "post_title", "string"
				       , cw.leTitle->text()));
	rpcstruct.appendChild(XmlMember(doc, "post_content", "string", entryText));
	if(postDateTime) rpcstruct.appendChild(XmlMember(doc, "post_date", "dateTime.iso8601", dateTime));
	rpcstruct.appendChild(XmlMember(doc, "comment_status", "string"
				       , takeComms ? "open" : "closed"));
	rpcstruct.appendChild(XmlMember(doc, "ping_status", "string"
				       , takeTB ? "open" : "closed"));
	rpcstruct.appendChild(XmlMember(doc, "post_password", "string"
				       , cw.lePassword->text()));
	/*rpcstruct.appendChild( XmlMember( doc, "sticky", "bool",
					  cw.chSticky->isChecked() ? "true" : "false" ) ); */
	if(catList.count()){
	    taxMember		= doc.createElement("member");
	    taxMemberName	= doc.createElement("name");
	    taxMemberName.appendChild(QDomText(doc.createTextNode("terms")));
	    taxMember.appendChild(taxMemberName);
	    taxValue	= doc.createElement("value");
	    taxStruct	= doc.createElement("struct");
	    taxStruct.appendChild(XmlRpcArray(doc, "category", "string", catList));
	    taxValue.appendChild(taxStruct);
	    taxMember.appendChild(taxValue);
	    rpcstruct.appendChild(taxMember);
	}
	if(tagList.count()) rpcstruct.appendChild(XmlRpcArray(doc, "tags_input", "string", tagList));
	if(tblist.length()) rpcstruct.appendChild(XmlMember(doc, "to_ping", "string", tblist));
	value.appendChild(rpcstruct);
	param.appendChild(value);
	params.appendChild(param);
	methodCall.appendChild(params);
	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml"
							, "version=\"1.0\" encoding=\"UTF-8\"")
			, doc.firstChild());

	QByteArray requestArray(doc.toByteArray());
	responseData = "";
	placeNetworkRequest(_wp_editPost, requestArray);
	if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	if(postAsSave && ! entryEverSaved){
	    cleanSave = true;
	    setDirtySignals(true);
	}
	cw.progressBar->setMaximum(2);
	cw.progressBar->setValue(1);
	cw.progressWidget->setVisible(true);
	cw.cbPageSelector->setCurrentIndex(0);
	cw.stackedWidget->setCurrentIndex(0);
    }else{
#ifdef QTM_DEBUG
	statusWidget->showMessage(tr("refreshCategories: All HTTP requests are blocked")
				 , 2000);
#else
	statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
}

void EditingWindow::updatePostCategories(){
    if(! entryNumber.isEmpty()) setPostCategories();
    else statusWidget->showMessage(tr("This entry has not been posted yet."), 2000);
}

void EditingWindow::setPostCategories(){
    QDomDocument	doc;
    QString		secCatId, secCatName;
    QDomElement		cat, methodCall, params, param, value, array, _data, arrayValue, arrayStruct;
// #ifndef NO_DEBUG_OUTPUT
//  qDebug() << "starting to post categories";
// #endif
    if(categoriesEnabled){
	// qDebug() << "categories enabled";
	if(! currentHttpBusiness){
		// qDebug() << "no biz";
	    methodCall = doc.createElement("methodCall");
	    methodCall.appendChild(XmlMethodName(doc, "mt.setPostCategories"));
	    params = doc.createElement("params");
	    params.appendChild(XmlValue(doc, "string", entryNumber));
	    params.appendChild(XmlValue(doc, "string", login));
	    params.appendChild(XmlValue(doc, "string", password));

	    param	= doc.createElement("param");
	    value	= doc.createElement("value");
	    array	= doc.createElement("array");
	    _data	= doc.createElement("data");
	    arrayValue	= doc.createElement("value");
	    arrayStruct = doc.createElement("struct");
	    if(! cw.chNoCats->isChecked()){
#ifndef NO_DEBUG_OUTPUT
		// qDebug() << "posting prim cat";
#endif
		cat = currentBlogElement.firstChildElement("categories")
		    .childNodes().at(cw.cbMainCat->currentIndex()).toElement();
		QString primCat = cw.cbMainCat->itemData(cw.cbMainCat->currentIndex()).toString();
#ifndef NO_DEBUG_OUTPUT
		// qDebug() << "posted prim cat";
#endif
		arrayStruct.appendChild(XmlMember(doc, "categoryId", "string", primCat.trimmed()));
		arrayStruct.appendChild(XmlMember(doc, "isPrimary", "boolean", "1"));
		arrayValue.appendChild(arrayStruct);
		_data.appendChild(arrayValue);
		for(int a = 0; a < cw.lwOtherCats->count(); a ++){
		    if(cw.lwOtherCats->isItemSelected(cw.lwOtherCats->item(a))){
			cat		= currentBlogElement.firstChildElement("categories").childNodes().at(a).toElement();
			secCatId	= cw.cbMainCat->itemData(a).toString();
			secCatName	= cw.cbMainCat->itemText(a);
			arrayValue	= doc.createElement("value");
			arrayStruct	= doc.createElement("struct");
			arrayStruct.appendChild(XmlMember(doc, "categoryId", "int", secCatId));
			arrayStruct.appendChild(XmlMember(doc, "categoryName", "string"
							 , secCatName));
			arrayStruct.appendChild(XmlMember(doc, "isPrimary", "boolean", "0"));
			arrayValue.appendChild(arrayStruct);
			_data.appendChild(arrayValue);
		    }
		}
	    }
	    array.appendChild(_data);
	    value.appendChild(array);
	    param.appendChild(value);
	    params.appendChild(param);
	    methodCall.appendChild(params);
	    doc.appendChild(methodCall);
	    doc.insertBefore(doc.createProcessingInstruction("xml"
							    , "version=\"1.0\" encoding=\"UTF-8\"")
			    , doc.firstChild());
	    QByteArray requestArray(doc.toByteArray(2));

	    responseData = "";
	    placeNetworkRequest(_mt_setPostCategories, requestArray);
		// if( progressBarAction->isVisible() )
	    if(cw.progressBar->isVisible()) cw.progressBar->setValue(cw.progressBar->value() + 1);
#ifdef WIN_QTV
	    if(taskbarProgress->isVisible()) taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
	    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	}else{
#ifdef QTM_DEBUG
	    statusWidget->showMessage(tr("setPostCategories: All HTTP requests are blocked")
				     , 2000);
#else
	    statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	    QApplication::restoreOverrideCursor();
	}
    }else{
	// An override cursor might have been set when posting an entry in a
	// non-category-enabled blog
	qDebug() << "categories not enabled";
	if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
    }
}

void EditingWindow::publishPost(){	// slot
    QDomDocument doc;
    if(! currentHttpBusiness){
	QDomElement methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "mt.publishPost"));
	QDomElement params = doc.createElement("params");

	params.appendChild(XmlValue(doc, "string", entryNumber));
	params.appendChild(XmlValue(doc, "string", login));
	params.appendChild(XmlValue(doc, "string", password));
	methodCall.appendChild(params);
	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml"
							, "version=\"1.0\" encoding=\"UTF-8\"")
			, doc.firstChild());

	QByteArray requestArray(doc.toByteArray());

	responseData = "";
	placeNetworkRequest(_mt_publishPost, requestArray);
	// if( progressBarAction->isVisible() )
	if(cw.progressBar->isVisible()) cw.progressBar->setValue(cw.progressBar->value() + 1);
#ifdef WIN_QTV
	if(taskbarProgress->isVisible()) taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
    }else statusWidget->showMessage(tr("Cannot publish; HTTP is blocked"), 2000);
}

void EditingWindow::exportEntry(){
    saveAs(true);
}

void EditingWindow::saveAll(){
    qtm->saveAll();
}

void EditingWindow::saveAs(bool exp){
    QString suggestedFilename;
    if(cw.leTitle->text().isEmpty()) suggestedFilename = localStorageDirectory;
    else{
	suggestedFilename = QString("%1/%2")
	    .arg(localStorageDirectory)
	    .arg(cw.leTitle->text().remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")));
    }
    QString extn = QString("%1 (*.%2)").arg(tr("Blog entries"))
	.arg(localStorageFileExtn);
    QString fn = QFileDialog::getSaveFileName(this, tr("Choose a filename")
					     , suggestedFilename, extn);
    if(! fn.isEmpty()){
	if(! localStorageFileExtn.isEmpty())
		if(! fn.endsWith(QString(".%1").arg(localStorageFileExtn))) fn.append(QString(".%1").arg(localStorageFileExtn));
	filename = fn;
	/*
	   This section has been commented out as unnecessary; the getSaveFileName function
	   already checks for the existence of a file.  Keeping in code until tested on other platforms.

	   if( QFile::exists( fn ) ) {
	   if( !QMessageBox::warning( 0, tr( "File exists" ),
	   tr( "A file by this name already exists. Continue?" ),
	   QMessageBox::Yes, QMessageBox::No | QMessageBox::Default ) ) {
	   QFile::remove( fn );
	   save( fn );
	   }
	   }
	   else */
	save(fn, exp);

	qtm->addRecentFile(cw.leTitle->text(), fn);
    }else statusWidget->showMessage(tr("Saving aborted"), 2000);
}


void EditingWindow::save(){
    if(filename.isEmpty()){
	saveAs();
	return;
    }
    save(filename);
}

void EditingWindow::save(const QString &fname, bool exp){
    int		count, tags;
    QString	text = EDITOR->document()->toPlainText();
	// Rename the old file to a back-up file
    if(QFile::exists(fname)) QFile::rename(fname, QString("%1~").arg(fname));
    QFile f(fname);
    if(! f.open(QIODevice::WriteOnly)){
	statusWidget->showMessage(tr("Could not write to %1").arg(fname)
				 , 2000);
	return;
    }
    QTextStream out(&f);
    if(useUtf8) out.setCodec(QTextCodec::codecForName("UTF-8"));
    out << (exp ? "QTM saved blog entry v2.0\n" : "QTM saved blog entry v3.0\n");
    out << QString("Title:%1\n").arg(cw.leTitle->text());
    out << QString("Publish:%1\n").arg(QString::number(cw.cbStatus->currentIndex()));
    if(entryBlogged) out << QString("EntryNumber:%1\n").arg(entryNumber);
    out << QString("Comments:%1\n").arg(cw.chAllowComments->isChecked() ? "1" : "0");
    out << QString("TB:%1\n").arg(cw.chAllowTB->isChecked() ? "1" : "0");
	// out << QString( "Sticky:%1\n" ).arg( cw.chSticky->isChecked() ? "1" : "0" );
    if(cw.lePassword->text().length()) out << QString("PWD:%1\n").arg(cw.lePassword->text());
    if(exp){
	out << QString("Server:%1\n").arg(server);
	out << QString("Location:%1\n").arg(location);
	out << QString("Login:%1\n").arg(login);
	out << QString("Password:%1\n").arg(password);
	out << QString("Blog:%1\n").arg(cw.cbBlogSelector->currentIndex());
    }else{
	out << QString("AcctBlog:%1@%2 (%3)\n")	// Include the blog name so it can be relayed to the user later
	    .arg(currentBlogid)
	    .arg(currentAccountId)
	    .arg(cw.cbBlogSelector->itemText(cw.cbBlogSelector->currentIndex()));
    }
    tags = cw.lwTags->count();
    if(tags){
	out << "Tags:";
	for(count = 0; count < tags; count ++){
	    out << QString(count ? ";%1" : "%1")
		.arg(cw.lwTags->item(count)->text().replace(' ', '+'));
	}
	out << "\n";
    }
    tags = cw.lwKeywordTags->count();
    if(tags){
	out << "Keywords:";
	for(count = 0; count < tags; count ++){
	    out << QString(count ? ",%1" : "%1")
		.arg(cw.lwKeywordTags->item(count)->text());
	}
	out << "\n";
    }
    if(! cw.chNoCats->isChecked()){
	QDomNodeList catNodeList = currentBlogElement.firstChildElement("categories").elementsByTagName("category");
	out << QString("PrimaryID:%1\n").arg(cw.cbMainCat->itemData(cw.cbMainCat->currentIndex()).toString());
	QString catsList;
	int	cats = 0;
	for(int a = 0; a < cw.lwOtherCats->count(); a ++){
	    if(cw.lwOtherCats->isItemSelected(cw.lwOtherCats->item(a))){
		if(cats) catsList.append(QString(";%1").arg(cw.cbMainCat->itemData(a).toString()));
		else catsList.append(cw.cbMainCat->itemData(a).toString());
		cats ++;
	    }
	}
	out << QString("CatIDs:%1\n").arg(catsList);
    }else out << "PrimaryID:none\n";
    if(cw.teExcerpt->toPlainText().length() > 0) out << QString("Excerpt:%1\n")
		    .arg(cw.teExcerpt->toPlainText().replace(QChar('\n'), "\\n"));
    out << QString("Text:\n%1").arg(text);
    f.close();

    dirtyIndicator->hide();
    setWindowModified(false);
    setDirtySignals(true);

    entryEverSaved = true;
}

void EditingWindow::choose(QString fname){
    QString	fn;
    QString	extn(QString("%1 (*.%2)").arg(tr("Blog entries"))
	.arg(localStorageFileExtn));
    if(fname.isEmpty()) fn = QFileDialog::getOpenFileName(this, tr("Choose a file to open")
							 , localStorageDirectory, extn);
    else fn = fname;
    if(! fn.isEmpty()){
	if(! useNewWindows){
	    if(saveCheck(true)){
		if(! load(fn)) statusWidget->showMessage(tr("File could not be loaded."), 2000);
		else{
		    qtm->addRecentFile(cw.leTitle->text(), fn);

		    dirtyIndicator->hide();
		    setWindowModified(false);
		    setDirtySignals(true);
		}
	    }
	}else{
	    EditingWindow *e = new EditingWindow;
	    if(e->load(fn, true)){
#ifdef USE_SYSTRAYICON
		e->setSTI(sti);
#endif

		positionWidget(e, this);
		qtm->addRecentFile(e->postTitle(), fn);

		e->show();
	    }else{
		statusWidget->showMessage("Loading of new window failed", 2000);
		e->deleteLater();
	    }
	}
    }else statusWidget->showMessage(tr("Loading aborted"), 2000);
}

bool EditingWindow::load(const QString &fname, QDomDocument &dd){
    accountsDom = dd;

    return load(fname);
}


bool EditingWindow::load(const QString &fname, bool fromSTI){
    Ui::dPassword pui;
    addToConsole("Starting load");
    QMap<QString, QString>	emap;
    QString			currentLine, key, value, fetchedText, tags, keywords;
    QString			catName;
    QStringList			otherCatStringList;
    QDomNodeList		accts;
	// bool getDetailsAgain = false;
    bool	isOK;
    int		b, c, d, g, h, hh;
    noAutoSave = true;
    QFile			f(fname);
    QDomElement			details;
    QDomProcessingInstruction	testPI;
    QDomElement			testAccountsElement;
    if(superMenu){
	superMenu->setEnabled(true);
#ifndef Q_OS_MAC
	superMenu->setVisible(true);
#endif
    }
    if(fromSTI){
	testPI			= accountsDom.firstChild().toProcessingInstruction();
	testAccountsElement	= accountsDom.firstChildElement("QTMAccounts");
	if(testAccountsElement.isNull()) accountsElement = accountsDom.createElement("QTMAccounts");
	else accountsElement = accountsDom.firstChildElement("QTMAccounts");
	currentAccountElement = accountsDom.createElement("account");
	accountsElement.appendChild(currentAccountElement);
	if(testAccountsElement.isNull()) accountsDom.appendChild(accountsElement);
	if(testPI.isNull()) accountsDom.insertBefore(accountsDom.createProcessingInstruction("xml", "version=\"1.0\"")
						    , accountsDom.firstChild());
    }
    if(! f.open(QIODevice::ReadOnly)){
	addToConsole("Cannot open file");
	statusWidget->showMessage(tr("Cannot open file."), 2000);
	return false;
    }
    QTextStream t(&f);
    if(useUtf8) t.setCodec(QTextCodec::codecForName("UTF-8"));
    if(! t.readLine().startsWith("QTM saved blog entry")){
	addToConsole("Not a QTM blog entry");
	statusWidget->showMessage(tr("This is not a QTM blog entry."), 2000);
	return false;
    }
    do {
	currentLine = t.readLine();
	if(currentLine.startsWith("Text:")) break;
	key		= currentLine.section(QChar(':'), 0, 0);
	value		= currentLine.section(QChar(':'), 1);
	emap[key]	= value;
	addToConsole(currentLine);
    } while(! t.atEnd());
    if(emap.contains("Title")){
	cw.leTitle->setText(emap.value("Title"));
	setWindowTitle(QString("%1 - QTM [*]").arg(emap.value("Title")));
    }
    if(emap.contains("Publish")) cw.cbStatus->setCurrentIndex(emap.value("Publish") == "1" ? 1 : 0);
    if(emap.contains("EntryNumber")){
	entryNumber	= emap.value("EntryNumber");
	entryBlogged	= true;
    }
    if(emap.contains("Comments")) cw.chAllowComments->setChecked(emap.value("Comments") == "1" ? true : false);
    if(emap.contains("TB")) cw.chAllowTB->setChecked(emap.value("TB") == "1" ? true : false);
	/* if( emap.contains( "Sticky" ) )
	  cw.chSticky->setChecked( emap.value( "Sticky" ) == "1" ? true : false ); */
    if(emap.contains("PWD")) cw.lePassword->setText(emap.value("PWD"));
    else cw.lePassword->clear();
    if(emap.contains("Server")){
	if(emap.value("Server") != server){
	    server = emap.value("Server");
		// getDetailsAgain = true;
	}
    }
    if(emap.contains("Location")){
	if(emap.value("Location") != location){
	    location = emap.value("Location");
		// getDetailsAgain = true;
	}
    }
    if(emap.contains("Login")){
	if(emap.value("Login") != login){
	    login = emap.value("Login");
		// getDetailsAgain = true;
	}
    }
    if(emap.contains("Password")){
	if(emap.value("Password") != password){
	    password = emap.value("Password");
		// getDetailsAgain = true;
	}
	noPassword = false;
    }else{
	password	= "";
	noPassword	= true;
    }
    if(emap.contains("Tags")){
	tags	= emap.value("Tags");
	d	= tags.count(QChar(';'));
	if(d)
		for(c = 0; c <= d; c ++) cw.lwTags->addItem(tags.section(QChar(';'), c, c));
	else cw.lwTags->addItem(tags);
    }
    cw.lwKeywordTags->clear();
    if(emap.contains("Keywords")){
	keywords	= emap.value("Keywords");
	d		= keywords.count(QChar(','));
	if(d)
		for(c = 0; c <= d; c ++) cw.lwKeywordTags->addItem(keywords.section(QChar(','), c, c));
	else cw.lwKeywordTags->addItem(keywords);
    }
    if(emap.contains("Blog")){
	b = emap.value("Blog").toInt(&isOK);
	if(isOK){
	    currentBlog		= b;
	    loadedEntryBlog	= b;
	}
    }
    if(emap.contains("AcctBlog")){
	currentBlogid	= emap.value("AcctBlog").section("@", 0, 0);
	loadedAccountId = emap.value("AcctBlog").section("@", 1, 1).section(" (", 0, 0);
	b		= emap.value("AcctBlog").section("@", 0, 0).toInt(&isOK);
	if(isOK){
	    currentBlog		= b;
	    loadedEntryBlog	= b;
	}
    }
	// Get old-style index categories
    if(emap.contains("Primary")){
	b = emap.value("Primary").toInt(&isOK);
	if(isOK){
	    primaryCat	= b;
	    noAlphaCats = true;
		// getDetailsAgain = true;
	}else{
	    primaryCat = 0;
	    addToConsole(tr("Assignation of primary category failed"));
	}
    }
    if(emap.contains("Cats")){
	otherCatsList		= emap.value("Cats");
	otherCatStringList	= otherCatsList.split(';');
	noAlphaCats		= true;
	// getDetailsAgain = true;
    }
	// Get new-style ID categories
    if(emap.contains("PrimaryID")){
	b = emap.value("PrimaryID").toInt(&isOK);
	if(isOK) primaryCat = b;
	else{
	    primaryCat = 0;
	    addToConsole(tr("Assignation of primary category failed"));
	}
    }
    if(emap.contains("CatIDs")){
	otherCatsList		= emap.value("CatIDs");
	otherCatStringList	= otherCatsList.split(';');
    }
    if(emap.contains("Excerpt")) cw.teExcerpt->setPlainText(QString(emap.value("Excerpt")).replace("\\n", "\n"));
    while(! t.atEnd()) fetchedText += QString("%1\n").arg(t.readLine());
    EDITOR->setPlainText(fetchedText);
    f.close();
    setDirtySignals(true);
    dirtyIndicator->hide();
    setWindowModified(false);
	// First of all, deal with entries saved to accounts
    if(! loadedAccountId.isNull()){
	accts = accountsDom.elementsByTagName("account");
	qDebug() << accts.count() << "accounts found";
	for(g = 0; g <= accts.count(); g ++){
	    if(g == accts.count()){
		// qDebug() << accts.count() << "accounts";
		// i.e. if it gets to the end of the accounts tree without finding the account
		QMessageBox::information(0, tr("QTM - No such account")
					, tr("QTM could not find this account (perhaps it was deleted).\n\n"
					     "Will set up a blank default account; you will need to fill in the access "
					     "details by choosing Accounts from the File menu.")
					, QMessageBox::Ok);
		QDomElement newDefaultAccount = accountsDom.createElement("account");
		newDefaultAccount.setAttribute("id", QString("newAccount_%1").arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
		QDomElement	newDetailElement	= accountsDom.createElement("details");
		QDomElement	newNameElement		= accountsDom.createElement("title");
		newNameElement.appendChild(QDomText(accountsDom.createTextNode(tr("New blank element"))));
		newDetailElement.appendChild(newNameElement);
		newDefaultAccount.appendChild(newDetailElement);
		accountsDom.firstChildElement("accounts").appendChild(newDefaultAccount);
		currentAccountElement = newDefaultAccount;
		cw.cbAccountSelector->addItem(tr("New blank element")
					     , newDefaultAccount.attribute("id"));
		cw.cbAccountSelector->setCurrentIndex(cw.cbAccountSelector->count() - 1);
		cw.cbBlogSelector->clear();
		cw.cbBlogSelector->setEnabled(false);
		cw.chNoCats->setEnabled(false);
		cw.cbMainCat->clear();
		cw.cbMainCat->setEnabled(false);
		cw.lwOtherCats->clear();
		cw.lwOtherCats->setEnabled(false);
		setPostClean();
		return true;
	    }
	    if(accts.at(g).toElement().attribute("id") == loadedAccountId){
		// qDebug() << "found the account:" << loadedAccountId;
		populateAccountList();
		currentAccountElement = accts.at(g).toElement();
		extractAccountDetails();

		QString st;
		for(h = 0; h < cw.cbAccountSelector->count(); h ++){
		    st = cw.cbAccountSelector->itemData(h).toString();
		    if(st == loadedAccountId) cw.cbAccountSelector->setCurrentIndex(h);
		}
		// Now populate the blog list
		QDomNodeList blogNodeList = currentAccountElement.elementsByTagName("blog");
		cw.cbBlogSelector->clear();
		for(hh = 0; hh < blogNodeList.count(); hh ++){
		    cw.cbBlogSelector->addItem(decodeXmlEntities(blogNodeList.at(hh).toElement()
			.firstChildElement("blogName").text())
					      , blogNodeList.at(hh).toElement()
			.firstChildElement("blogid").text());
		    if(blogNodeList.at(hh).firstChildElement("blogid").text() == currentBlogid) currentBlogElement = blogNodeList.at(hh).toElement();
		}
		cw.cbBlogSelector->disconnect(SIGNAL(activated(int)), this, 0);
		// qDebug() << "connecting changeBlog";
		connect(cw.cbBlogSelector, SIGNAL(activated(int))
		       , this, SLOT(changeBlog(int)));
		if(! isOK){
		    QMessageBox::information(0, tr("QTM - Invalid blog")
					    , tr("Could not get a valid blog number.  Please set it again.")
					    , QMessageBox::Ok);
		    return true;
		}
		for(hh = 0; hh < cw.cbBlogSelector->count(); hh ++){
		    if(cw.cbBlogSelector->itemData(hh) == currentBlogid){
			cw.cbBlogSelector->setCurrentIndex(hh);
			break;
		    }
		}
		cw.lwAvailKeywordTags->clear();
		populateTagList();

		// Now populate and set the categories
		QDomElement catsElement = currentBlogElement.firstChildElement("categories");
		if(! catsElement.isNull()){
		    cw.cbMainCat->clear();
		    cw.lwOtherCats->clear();

		    QDomNodeList	catNodeList	= catsElement.elementsByTagName("category");
		    int			b		= catNodeList.count();
		    if(b){
			// qDebug() << "categories:" << b;
			cw.cbMainCat->clear();
			cw.lwOtherCats->clear();
			for(int j = 0; j < b; j ++){
			    catName = decodeXmlEntities(catNodeList.at(j).firstChildElement("categoryName").text());
			    cw.cbMainCat->addItem(catName, QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
			    cw.lwOtherCats->addItem(catName);
			}
			// qDebug() << "primaryCat is" << QString::number( primaryCat );
			for(int i = 0; i < catNodeList.size(); i ++){
			    QString cc = catNodeList.at(i).firstChildElement("categoryId").text();
				// qDebug() << "cc is" << cc;
			    if(cc == QString::number(primaryCat)){
				// qDebug() << "found the primary category";
				cw.cbMainCat->setCurrentIndex(i);
			    }else if(otherCatStringList.contains(cc)) cw.lwOtherCats->setItemSelected(cw.lwOtherCats->item(i), true);
			}
		    }else{
			cw.chNoCats->setEnabled(false);
			cw.cbMainCat->setEnabled(false);
			cw.lwOtherCats->setEnabled(false);
		    }
		}
		filename = fname;
		setPostClean();
		return true;
	    }
	}
	filename = fname;
	setPostClean();
	return true;
    }
	// Now we know this isn't an account entry, check whether the saved details actually
	// belong to an account; if it does, there is no need to check for the password

	// qDebug() << "this is an old-style account";
    QDomNodeList blogs;

    accts = accountsDom.documentElement().elementsByTagName("account");
    for(int e = 0; e <= accts.count(); e ++){
	if(e == accts.count()) break;
	details = accts.at(e).toElement().firstChildElement("details");
	// qDebug() << "matching against" << details.firstChildElement( "title" ).text();
	if(details.firstChildElement("server").text() == server &&
	    details.firstChildElement("location").text() == location &&
	    details.firstChildElement("login").text() == login){
		// qDebug() << "match found";
	    currentAccountElement = accts.at(e).toElement();
	    cw.cbAccountSelector->setCurrentIndex(e);
	    extractAccountDetails();
		// First check whether the blog still exists
	    blogs = currentAccountElement.elementsByTagName("blog");
	    if(currentBlog > blogs.count()){
		connect(this, SIGNAL(categoryRefreshFinished())
		       , this, SLOT(setLoadedPostCategories()));
		refreshBlogList();
		setPostClean();
		return true;
	    }else{
		// qDebug() << "now setting categories";
		if(blogs.at(currentBlog).toElement().elementsByTagName("category").count()){
		    cw.cbBlogSelector->clear();
		    for(hh = 0; hh < blogs.count(); hh ++){
			cw.cbBlogSelector->addItem(decodeXmlEntities(blogs.at(hh).firstChildElement("blogName").text())
						  , QVariant(blogs.at(hh).firstChildElement("blogid").text()));
		    }
		    setLoadedPostCategories();
		    setPostClean();
		    return true;
		}else{
		    connect(this, SIGNAL(categoryRefreshFinished())
			   , this, SLOT(setLoadedPostCategories()));
		    changeBlog(currentBlog);
		    setPostClean();
		    return true;
		}
	    }
	}
    }
    if(noPassword){
	QDialog pwd;
	pui.setupUi(&pwd);
	if(pwd.exec()) password = pui.lePassword->text();
	else
		QMessageBox::warning(0, tr("No password")
				    , tr("This entry was saved without a password.\n"
					 "You will need to set one, using the\n"
					 "Preferences window.")
				    , QMessageBox::Ok, QMessageBox::NoButton);
    }
	// This is an old-style account which isn't in the database
	// qDebug() << "old-style, not found";

    QDomElement newAcct, newDetails, newTitle, newServer, newLocation, newLogin, newPwd;
    newAcct		= accountsDom.createElement("account");
    currentAccountId	= tr("newAccount_%1").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    newAcct.setAttribute("id", currentAccountId);
    newDetails	= accountsDom.createElement("details");
    newTitle	= accountsDom.createElement("title");
    newTitle.appendChild(QDomText(accountsDom.createTextNode(tr("New un-named account"))));
    newServer = accountsDom.createElement("server");
    newServer.appendChild(QDomText(accountsDom.createTextNode(server)));
    newLocation = accountsDom.createElement("location");
    newLocation.appendChild(QDomText(accountsDom.createTextNode(location)));
    newLogin = accountsDom.createElement("login");
    newLogin.appendChild(QDomText(accountsDom.createTextNode(login)));
    newPwd = accountsDom.createElement("password");
    newPwd.appendChild(QDomText(accountsDom.createTextNode(password)));
    newDetails.appendChild(newTitle);
    newDetails.appendChild(newServer);
    newDetails.appendChild(newLocation);
    newDetails.appendChild(newLogin);
    newDetails.appendChild(newPwd);
    newAcct.appendChild(newDetails);
    accountsDom.documentElement().appendChild(newAcct);
    currentAccountElement = newAcct;
    cw.cbAccountSelector->addItem(newTitle.text(), newAcct.attribute("id"));
    cw.cbAccountSelector->setCurrentIndex(cw.cbAccountSelector->count() - 1);

    connect(this, SIGNAL(categoryRefreshFinished())
	   , this, SLOT(setLoadedPostCategories()));
    refreshBlogList();
    saveAccountsDom();

    filename		= fname;
    entryEverSaved	= true;
    setPostClean();
    return true;
}

void EditingWindow::setLoadedPostCategories(){	// slot
    int		a, b, j, z;
    int		i = 0;
    QString	c, cc, d;
    bool	isOK;

    disconnect(SIGNAL(categoryRefreshFinished()));
    disconnect(SIGNAL(httpBusinessFinished()));

    QDomNodeList blogNodes = currentAccountElement.firstChildElement("blogs")
	.elementsByTagName("blog");
    if(! cw.cbBlogSelector->count()){
	j = blogNodes.size();
	for(z = 0; z < j; z ++) cw.cbBlogSelector->addItem(decodeXmlEntities(blogNodes.at(z).firstChildElement("blogName").text())
							  , QVariant(blogNodes.at(z).firstChildElement("blogName").text()));
	cw.cbBlogSelector->setCurrentIndex(currentBlog);
    }
    currentBlogElement = blogNodes.at(currentBlog).toElement();
    cw.cbBlogSelector->setCurrentIndex(currentBlog);
    cw.cbMainCat->clear();
    cw.lwOtherCats->clear();
    QDomNodeList catNodeList = currentBlogElement.elementsByTagName("category");
    b = catNodeList.count();
    if(b){
	// qDebug() << "populating cat list";
	cw.cbMainCat->clear();
	cw.lwOtherCats->clear();
	for(j = 0; j < b; j ++){
	    cw.cbMainCat->addItem(catNodeList.at(j).firstChildElement("categoryName").text()
				 , QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
	    cw.lwOtherCats->addItem(catNodeList.at(j).firstChildElement("categoryName").text());
	}
    }else{
	// qDebug() << "no categories found";
	connect(this, SIGNAL(categoryRefreshFinished())
	       , this, SLOT(setLoadedPostCategories()));
	return;
    }
    if(noAlphaCats){
	cw.cbMainCat->setCurrentIndex(primaryCat);

	QString f(otherCatsList.section(QChar(';'), - 1, - 1));
	do {
	    c	= otherCatsList.section(QChar(';'), i, i);
	    a	= c.toInt(&isOK);
	    if(isOK) cw.lwOtherCats->setItemSelected(cw.lwOtherCats->item(a), true);
	    else break;
	    i ++;
	} while(c != f);
	initialChangeBlog = false;
    }else{
	QDomNodeList	catNodes	= currentBlogElement.firstChildElement("categories").elementsByTagName("category");
	QStringList	catStringList	= otherCatsList.split(";");
	// qDebug() << "current Blog is" << currentBlogElement.firstChildElement( "blogName" ).text();
	// qDebug() << "primaryCat is" << primaryCat;
	for(i = 0; i < catNodes.size(); i ++){
	    cc = catNodes.at(i).firstChildElement("categoryId").text();
	    if(cc == QString::number(primaryCat)){
		// qDebug() << "found primary category";
		cw.cbMainCat->setCurrentIndex(i);
	    }else if(catStringList.contains(cc)) cw.lwOtherCats->setItemSelected(cw.lwOtherCats->item(i), true);
	}
	initialChangeBlog = false;
    }
}

void EditingWindow::uploadFile(){
    QString		fileInBase64;
    QByteArray		conversionBuffer;
    QFile		inFile;
    QDomDocument	doc;
    if(! currentHttpBusiness){
	QString uploadFilename = QFileDialog::getOpenFileName(this
							     , tr("Select file to upload")
							     , QDir::homePath());
	if(! uploadFilename.isEmpty()){
	    if(QFile::exists(uploadFilename)){
		inFile.setFileName(uploadFilename);
		if(inFile.open(QIODevice::ReadOnly)){
		    conversionBuffer = inFile.readAll();
		    if(conversionBuffer.isEmpty()) statusWidget->showMessage(tr("This file was empty, or an error occurred."), 2000);
		    else{
			QApplication::processEvents();
			fileInBase64 = toBase64(conversionBuffer);
			if(! fileInBase64.isEmpty()){
			    QDomElement methodCall = doc.createElement("methodCall");
			    methodCall.appendChild(XmlMethodName(doc, "metaWeblog.newMediaObject"));
			    QDomElement params = doc.createElement("params");

			    QString blogid = currentBlogElement.firstChildElement("blogid").text();
/*              QRegExp blogidRegExp( "^[0-9]+$" );
              bool blogidIsInt = blogidRegExp.exactMatch( blogid ); */

			    params.appendChild(XmlValue(doc, "string", blogid));
			    params.appendChild(XmlValue(doc, "string", login));
			    params.appendChild(XmlValue(doc, "string", password));

			    QDomElement param		= doc.createElement("param");
			    QDomElement value		= doc.createElement("value");
			    QDomElement rpcStruct	= doc.createElement("struct");
			    rpcStruct.appendChild(XmlMember(doc, "bits", "base64", fileInBase64));
			    rpcStruct.appendChild(XmlMember(doc, "name", "string"
							   , QFileInfo(uploadFilename).fileName()));
			    value.appendChild(rpcStruct);
			    param.appendChild(value);
			    params.appendChild(param);

			    methodCall.appendChild(params);
			    doc.appendChild(methodCall);
			    doc.insertBefore(doc.createProcessingInstruction("xml"
									    , "version=\"1.0\" encoding=\"UTF-8\"")
					    , doc.firstChild());

			    currentHttpBusiness = _metaWeblog_newMediaObject;

			    QByteArray requestArray(doc.toByteArray());
			    responseData = "";
			    placeNetworkRequest(_metaWeblog_newMediaObject, requestArray);
			    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
			}else statusWidget->showMessage(tr("Upload was cancelled."), 2000);
		    }
		    inFile.close();
		}else statusWidget->showMessage(tr("Could not open the file."), 2000);
	    }else statusWidget->showMessage(tr("That file could not be found."), 2000);
	}
    }else statusWidget->showMessage(tr("HTTP requests are blocked."), 2000);
}

void EditingWindow::doWhatsThis(){
    QWhatsThis::enterWhatsThisMode();
}

void EditingWindow::doViewBasicSettings(){
    cw.cbPageSelector->setCurrentIndex(0);
}

void EditingWindow::doViewCategories(){
    cw.cbPageSelector->setCurrentIndex(1);
}

void EditingWindow::changeOtherCatsHeading(){
    int c = cw.lwOtherCats->selectedItems().count();
    if(c) cw.lOtherCats->setText(tr("Others (%1)").arg(c));
    else cw.lOtherCats->setText(tr("Others"));
}

void EditingWindow::doViewExcerpt(){
    cw.cbPageSelector->setCurrentIndex(2);
    cw.teExcerpt->setFocus();
}

void EditingWindow::doViewKeywordTags(){
    cw.cbPageSelector->setCurrentIndex(3);
}

void EditingWindow::doViewTechTags(){
    cw.cbPageSelector->setCurrentIndex(4);
}

void EditingWindow::doViewTBPings(){
    cw.cbPageSelector->setCurrentIndex(5);
}

// This slot is to refresh categories automatically if there are none when the user
// switches to the category page
void EditingWindow::handleSideWidgetPageSwitch(int index){
    QDomNodeList	cl;
    QString		currentCatName, currentCatId;
    int			uncategorized = 0;
    if(index == 1){
	if(  cw.cbMainCat->count() == 0 && networkActionsEnabled
	  && categoriesEnabled){
		/*if( currentBlogElement.isNull() )
		  qDebug() << "currentBlogElement is null"; */
	    cl = currentBlogElement.firstChildElement("categories").elementsByTagName("category");
	    if(cl.count() == 0){
		statusWidget->showMessage(tr("Getting categories, please wait"), 2000);
		refreshCategories();
	    }else{
		cw.cbMainCat->clear();
		cw.lwOtherCats->clear();
		for(int i = 0; i < cl.count(); i ++){
		    currentCatName	= cl.at(i).firstChildElement("categoryName").text();
		    currentCatId	= cl.at(i).firstChildElement("categoryId").text();
		    cw.cbMainCat->addItem(currentCatName, currentCatId);
		    if(currentCatId == "1") uncategorized = i;
		    cw.lwOtherCats->addItem(currentCatName);
		}
		if(uncategorized) cw.cbMainCat->setCurrentIndex(uncategorized);
	    }
	}
    }
}

void EditingWindow::newChildCategory(){
    newCategory(cw.lwOtherCats->currentRow() + 1);
}

void EditingWindow::newCategory(int parentCategory){
    QDomElement		catsElement;
    QDomDocument	doc;
    QDomElement		methodCall, params, param, member, value
    , string, rpcstruct, actualValue;
    QDomNodeList	catNodeList;
    int			b, j;
	// This feature uses a Wordpress API call
    if(useWordpressAPI){
	QDialog			newCategoryDialog(this);
	Ui::NewCategoryForm	ncui;
	ncui.setupUi(&newCategoryDialog);

	catsElement = currentBlogElement.firstChildElement("categories");
	if(! catsElement.isNull()){
	    catNodeList = catsElement.elementsByTagName("category");
	    b		= catNodeList.count();
	    if(b){
		ncui.cbParent->clear();
		ncui.cbParent->addItem("(Top level)", QVariant("0"));
		for(j = 0; j < b; j ++) ncui.cbParent->addItem(catNodeList.at(j).firstChildElement("categoryName").text()
							      , QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
		ncui.cbParent->setCurrentIndex(parentCategory);
	    }
	    if(newCategoryDialog.exec()){
		if(! ncui.leName->text().isEmpty()){
		    if(! currentHttpBusiness){
			QString blogid = cw.cbBlogSelector->itemData(cw.cbBlogSelector->currentIndex()).toString();
/*            QRegExp blogidRegExp( "^[0-9]+$" );
            bool blogidIsInt = blogidRegExp.exactMatch( blogid ); */

			methodCall = doc.createElement("methodCall");
			methodCall.appendChild(XmlMethodName(doc, "wp.newCategory"));

			params = doc.createElement("params");
			params.appendChild(XmlValue(doc, "string", blogid));
			params.appendChild(XmlValue(doc, "string", login));
			params.appendChild(XmlValue(doc, "string", password));

			param		= doc.createElement("param");
			value		= doc.createElement("value");
			rpcstruct	= doc.createElement("struct");

			rpcstruct.appendChild(XmlMember(doc, "name", "string"
						       , ncui.leName->text()));
			rpcstruct.appendChild(XmlMember(doc, "slug", "string", ""));
			rpcstruct.appendChild(XmlMember(doc, "parent_id", "int"
						       , ncui.cbParent->itemData(ncui.cbParent->currentIndex()).toString()));
			rpcstruct.appendChild(XmlMember(doc, "description", "string"
						       , ncui.teDescription->toPlainText().simplified()));

			value.appendChild(rpcstruct);
			param.appendChild(value);
			params.appendChild(param);

			methodCall.appendChild(params);
			doc.appendChild(methodCall);
			doc.insertBefore(doc.createProcessingInstruction("xml"
									, "version=\"1.0\" encoding=\"UTF-8\"")
					, doc.firstChild());

			QByteArray requestArray(doc.toByteArray());
			responseData = "";
			placeNetworkRequest(_wp_newCategory, requestArray);
			if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		    }else statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
		}else statusWidget->showMessage(tr("Your category must have a name."), 2000);
	    }
	}
    }
}

void EditingWindow::addKeywordTag(){
    cw.cbPageSelector->setCurrentIndex(3);
    cw.leAddKeywordTag->setFocus();
}

void EditingWindow::addKeywordTagFromLineEdit(){
    QString text(cw.leAddKeywordTag->text());
    if(! text.isEmpty()){
	text.remove(QRegExp("^\""));
	text.remove(QRegExp("\"$"));

	cw.lwKeywordTags->addItem(text);
	cw.leAddKeywordTag->clear();
	if(! isWindowModified()) dirtify();
    }
}

void EditingWindow::addKeywordTagFromAvailTags(){
    if(cw.lwAvailKeywordTags->count() && cw.lwAvailKeywordTags->currentRow() != - 1){
	// Check whether the tag selected is already in use
	QListWidgetItem			*currentItem	= cw.lwAvailKeywordTags->currentItem();
	QList<QListWidgetItem *>	matches		= cw.lwKeywordTags->findItems(currentItem->text()
										     , Qt::MatchFixedString | Qt::MatchCaseSensitive);
	if(! matches.count()) cw.lwKeywordTags->addItem(currentItem->text());
    }
}

void EditingWindow::refreshKeywordTags(){
    if(! currentHttpBusiness){
	if(useWordpressAPI){
	    QDomDocument	doc;
	    QDomElement		methodCall = doc.createElement("methodCall");
	    methodCall.appendChild(XmlMethodName(doc, "wp.getTags"));

	    QDomElement params = doc.createElement("params");
	    params.appendChild(XmlValue(doc, "int", currentBlogid));
	    params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details")
		.firstChildElement("login").text()));
	    params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details")
		.firstChildElement("password").text()));

	    methodCall.appendChild(params);
	    doc.appendChild(methodCall);
	    doc.insertBefore(doc.createProcessingInstruction("xml"
							    , "version=\"1.0\" encoding=\"UTF-8\"")
			    , doc.firstChild());
	    addToConsole(doc.toString());
	    QByteArray requestArray(doc.toByteArray(2));
	    responseData = "";
	    placeNetworkRequest(_wp_getTags, requestArray);
	    if(QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	}else statusWidget->showMessage(tr("This feature only works on Wordpress."), 2000);
    }else{
#ifdef QTM_DEBUG
	statusWidget->showMessage(tr("refreshCategories: All HTTP requests are blocked")
				 , 2000);
#else
	statusWidget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
    }
}

void EditingWindow::removeKeywordTag(){
    int c = cw.lwKeywordTags->currentRow();
    if(c != - 1){
	cw.lwKeywordTags->takeItem(c);
	if(! isWindowModified()) dirtify();
    }
}

void EditingWindow::addTechTag(){
    cw.cbPageSelector->setCurrentIndex(4);
    cw.leAddTag->setFocus();
}

void EditingWindow::addClipTag(){
    int			l;
    QRegExpValidator	tagFormat(QRegExp("^http:\\/\\/(www\\.)?technorati\\.com\\/tag\\/([a-zA-Z0-9\\.%]+[\\+ ])*[a-zA-Z0-9\\.%]+$")
				 , this);
    QString tagText = QApplication::clipboard()->text();

    cw.cbPageSelector->setCurrentIndex(4);
    addToConsole(QString("Validating %1").arg(tagText));
    if(tagFormat.validate(tagText, l) != QValidator::Acceptable) statusWidget->showMessage(tr("This is not a valid tag."), 2000);
    else{
	statusWidget->showMessage(tr("This tag validates OK."), 2000);
	tagText.remove(QRegExp("(http:\\/\\/)?(www\\.)?technorati\\.com\\/tag\\/"));
	cw.lwTags->addItem(tagText);
	if(! isWindowModified()) dirtify();
    }
}

void EditingWindow::addTechTagFromLineEdit(){
    if(! cw.leAddTag->text().isEmpty()){
	cw.lwTags->addItem(cw.leAddTag->text());
	cw.leAddTag->clear();
	if(! isWindowModified()) dirtify();
    }
}

void EditingWindow::addTechTagFromAddButton(){
    int		i;
    QString	lineEditText = cw.leAddTag->text();
    if(! lineEditText.isEmpty()){
	if(tagValidator->validate(lineEditText, i) == QValidator::Acceptable){
	    statusWidget->showMessage(tr("This tag validates."), 2000);
	    cw.lwTags->addItem(cw.leAddTag->text());
	    cw.leAddTag->clear();
	    if(! isWindowModified()) dirtify();
	}else statusWidget->showMessage(tr("This is not a valid tag."), 2000);
    }
}

void EditingWindow::addTBPing(){
    cw.cbPageSelector->setCurrentIndex(5);
    cw.leTBPingURL->setFocus();
}


void EditingWindow::addClipTBPing(){
    QString clipboardText = QApplication::clipboard()->text();
    if(! clipboardText.isEmpty()){
	if(QUrl(clipboardText).isValid()){
	    statusWidget->showMessage(tr("This URL validates."));
	    cw.lwTBPings->addItem(clipboardText);
	    if(! isWindowModified()) dirtify();
	}else statusWidget->showMessage(tr("This is not a valid URL."), 2000);
    }
}

void EditingWindow::addTBPingFromLineEdit(){
    QString lineEditText = cw.leTBPingURL->text();
    if(! lineEditText.isEmpty()){
	if(QUrl(lineEditText).isValid()){
	    statusWidget->showMessage(tr("This URL validates."), 2000);
	    cw.lwTBPings->addItem(lineEditText);
	    cw.leTBPingURL->clear();
	    if(! isWindowModified()) dirtify();
	}else statusWidget->showMessage(tr("This is not a valid URL."), 2000);
    }
}

void EditingWindow::removeTechTag(){
    int c = cw.lwTags->currentRow();
    cw.lwTags->takeItem(c);
    if(! isWindowModified()) dirtify();
}

void EditingWindow::addTBPingFromAddButton(){
    QString lineEditText = cw.leTBPingURL->text();
    if(! lineEditText.isEmpty()){
	if(QUrl(lineEditText).isValid()){
	    statusWidget->showMessage(tr("This URL validates."), 2000);
	    cw.lwTBPings->addItem(cw.leTBPingURL->text());
	    cw.leTBPingURL->clear();
	    if(! isWindowModified()) dirtify();
	}else statusWidget->showMessage(tr("This is not a valid URL."), 2000);
    }
}

void EditingWindow::removeTBPing(){
    int c = cw.lwTBPings->currentRow();
    cw.lwTBPings->takeItem(c);
    if(! isWindowModified()) dirtify();
}

void EditingWindow::showPassword(bool showIt){
    cw.lePassword->setEchoMode(showIt ? QLineEdit::Normal : QLineEdit::Password);
}

void EditingWindow::dirtify(){
    dirtyIndicator->show();
    setWindowModified(true);
    setDirtySignals(false);
    cleanSave = false;
}

void EditingWindow::dirtifyIfText(){
    if(! EDITOR->document()->isEmpty()){
	dirtyIndicator->show();
	setWindowModified(true);
	setDirtySignals(false);
	cleanSave = false;
    }
}

void EditingWindow::setDirtySignals(bool d){
    QList<QWidget *> widgetList;
    widgetList	<< EDITOR << cw.cbAccountSelector << cw.cbBlogSelector << cw.cbStatus
		<< cw.chAllowComments << cw.chAllowTB << cw.cbMainCat << cw.lwOtherCats << cw.teExcerpt;
    if(d){
	connect(EDITOR->document(), SIGNAL(contentsChanged()), this, SLOT(dirtify()));
	connect(cw.leTitle, SIGNAL(textEdited(const QString&)), this, SLOT(dirtifyIfText()));
	connect(cw.cbAccountSelector, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
	connect(cw.cbBlogSelector, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
	connect(cw.cbStatus, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
	connect(cw.chAllowComments, SIGNAL(clicked(bool)), this, SLOT(dirtifyIfText()));
	connect(cw.chAllowTB, SIGNAL(clicked(bool)), this, SLOT(dirtifyIfText()));
	connect(cw.cbMainCat, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
	connect(cw.lwOtherCats, SIGNAL(activated(const QModelIndex&))
	       , this, SLOT(dirtifyIfText()));
	connect(cw.teExcerpt, SIGNAL(textChanged()), this, SLOT(dirtify()));
    }else{
	foreach(QWidget * w, widgetList){
	    disconnect(w, 0, this, SLOT(dirtify()));
	    disconnect(w, 0, this, SLOT(dirtifyIfText()));
	}
	disconnect(EDITOR->document(), 0, this, SLOT(dirtify()));
    }
}

void EditingWindow::setPostClean(){
    dirtyIndicator->hide();
    setWindowModified(false);
    setDirtySignals(true);
    cleanSave = false;
}

void EditingWindow::showStatusBarMessage(const QString &msg){
    statusWidget->showMessage(msg, 2000);
}

bool EditingWindow::saveCheck(bool){
    bool return_value;
    if(! isWindowModified()) return_value = true;
    else return_value = saveCheck();
    return return_value;
}

bool EditingWindow::saveCheck(){
    bool return_value;

    activateWindow();
    int a = QMessageBox::warning(this, tr("Not saved")
				, tr("You have not saved this file.\n"
				     "Save first?")
				, QMessageBox::Yes | QMessageBox::Default
				, QMessageBox::No, QMessageBox::Cancel);
    switch(a){
	case QMessageBox::Cancel:
	    return_value = false;
	    break;
	case QMessageBox::Yes:
	    save();
	default:
	    return_value = true;
    }
    return return_value;
}

void EditingWindow::doInitialChangeBlog(){
#ifdef QTM_DEBUG
    addToConsole(QString("loadedEntryBlog: %1\n").arg(loadedEntryBlog));
#endif
    disconnect(this, SIGNAL(httpBusinessFinished()), 0, 0);
    if(loadedEntryBlog != 999){
	currentBlog	= loadedEntryBlog;
	loadedEntryBlog = 999;
	cw.cbBlogSelector->setCurrentIndex(currentBlog);
	currentBlogElement = currentAccountElement.firstChildElement("blogs")
	    .elementsByTagName("blog").at(currentBlog).toElement();
	currentBlogid = currentBlogElement.firstChildElement("blogid").text();
#ifndef NO_DEBUG_OUTPUT
	// qDebug() << currentBlogid;
#endif
    }
    changeBlog(currentBlog);

    disconnect(cw.cbBlogSelector, SIGNAL(activated(int))
	      , this, SLOT(changeBlog(int)));
    connect(cw.cbBlogSelector, SIGNAL(activated(int))
	   , this, SLOT(changeBlog(int)));
}

void EditingWindow::copyURL(){
    QApplication::clipboard()->setText(remoteFileLocation);
    if(superMenu){
	superMenu->copyULAction->setVisible(false);
	superMenu->copyULAction->setEnabled(false);
    }
    cw.copyURLWidget->hide();
}

void EditingWindow::handleFind(){
    if(searchWidget->isVisible()){
	if(superMenu) superMenu->findAction->setText(tr("&Find ..."));
	searchWidget->close();
    }else{
	if(superMenu) superMenu->findAction->setText(tr("Exit &find"));
	searchWidget->find();
    }
}

void EditingWindow::findAgain(){
#ifdef Q_OS_MAC
    searchWidget->findAgain();
#endif
}

QDomElement EditingWindow::XmlValue(QDomDocument &doc, QString valueType, QString actualValue, QString memberType){
    QDomElement param		= doc.createElement(memberType);
    QDomElement value		= doc.createElement("value");
    QDomElement realValue	= doc.createElement(valueType);
    realValue.appendChild(QDomText(doc.createTextNode(actualValue)));
    value.appendChild(realValue);
    param.appendChild(value);

    return param;
}

QDomElement EditingWindow::XmlMember(QDomDocument &doc, QString valueName, QString valueType, QString actualValue){
    QDomElement mem	= doc.createElement("member");
    QDomElement name	= doc.createElement("name");
    name.appendChild(QDomText(doc.createTextNode(valueName)));
    mem.appendChild(name);
    QDomElement value		= doc.createElement("value");
    QDomElement realValue	= doc.createElement(valueType);
    realValue.appendChild(QDomText(doc.createTextNode(actualValue)));
    value.appendChild(realValue);
    mem.appendChild(value);

    return mem;
}

QDomElement EditingWindow::XmlMethodName(QDomDocument &doc, QString methodName){
    QDomElement methName = doc.createElement("methodName");
    methName.appendChild(QDomText(doc.createTextNode(methodName)));

    return methName;
}

QDomElement EditingWindow::XmlRpcArray(QDomDocument &doc, QString valueName, QString valueType, QList<QString> text){
    QDomElement arrayValue, arrayValueString;
    int		i;

    QDomElement mem	= doc.createElement("member");
    QDomElement name	= doc.createElement("name");
    name.appendChild(QDomText(doc.createTextNode(valueName)));
    mem.appendChild(name);
    QDomElement value		= doc.createElement("value");
    QDomElement array		= doc.createElement("array");
    QDomElement arrayData	= doc.createElement("data");
    for(i = 0; i < text.count(); i ++){
	arrayValue		= doc.createElement("value");
	arrayValueString	= doc.createElement(valueType);
	arrayValueString.appendChild(QDomText(doc.createTextNode(text.value(i))));
	arrayValue.appendChild(arrayValueString);
	arrayData.appendChild(arrayValue);
    }
    array.appendChild(arrayData);
    value.appendChild(array);
    mem.appendChild(value);

    return mem;
}

QDomElement EditingWindow::XmlRpcArray(QDomDocument &doc, QString valueName, QList<QString> text){
    return XmlRpcArray(doc, valueName, "string", text);
}

void EditingWindow::setNetworkActionsEnabled(bool a){
    networkActionsEnabled = a;
    if(superMenu){
	superMenu->refreshBlogListAction->setEnabled(a);
	superMenu->refreshCatsAction->setEnabled(categoriesEnabled ? a : false);
	superMenu->updateCatsAction->setEnabled(categoriesEnabled ? a : false);
	superMenu->blogThisAction->setEnabled(a);
    }
    cw.lBlog->setEnabled(a);
    cw.cbBlogSelector->setEnabled(a);
    cw.pbRefresh->setEnabled(a);
}

void EditingWindow::handleInitialLookup(const QHostInfo &hostInfo){
    if(hostInfo.error() == QHostInfo::NoError){
	refreshBlogList();
	setNetworkActionsEnabled(true);
    }else setNetworkActionsEnabled(false);
}
/*
void EditingWindow::handleHostLookupFailed()
{
  statusBar()->showMessage( tr( "The host specified could not be found." ), 2000 );

  http->disconnect();
  http->abort();

  disconnect( SIGNAL( httpBusinessFinished() ) );
  if( QApplication::overrideCursor() != 0 )
    QApplication::restoreOverrideCursor();
} */

void EditingWindow::configureQuickpostTemplates(){
#if defined USE_SYSTRAYICON
    if(sti) sti->configureQuickpostTemplates(this);
#endif
}

void EditingWindow::setPublishStatus(int publishStatus){
    cw.cbStatus->setCurrentIndex(publishStatus);
}

QString EditingWindow::postTitle(){
    return cw.leTitle->text();
}

void EditingWindow::setPostTitle(const QString &t){
    cw.leTitle->setText(t);
}

void EditingWindow::saveAutoLinkDictionary(){
    QDomDocument	doc;
    QDomElement		currentElement, currentKey, currentValue, currentTitle, currentTarget;
    QDomText		titleText, targetText;

    QDomElement				autoLinkDictionaryElement = doc.createElement("AutoLinkDictionary");
    QHashIterator<QString, QString>	aldIterator(autoLinkDictionary);
    while(aldIterator.hasNext()){
	aldIterator.next();
	currentElement	= doc.createElement("entry");
	currentKey	= doc.createElement("key");
	currentKey.appendChild(QDomText(doc.createTextNode(aldIterator.key())));
	currentValue = doc.createElement("URL");
	currentValue.appendChild(QDomText(doc.createTextNode(aldIterator.value())));
	currentElement.appendChild(currentKey);
	currentElement.appendChild(currentValue);
	if(autoLinkTitleDictionary.contains(aldIterator.key())){
	    currentTitle	= doc.createElement("title");
	    titleText		= doc.createTextNode(autoLinkTitleDictionary.value(aldIterator.key()));
	    currentTitle.appendChild(titleText);
	    currentElement.appendChild(currentTitle);
	}
	if(autoLinkTargetDictionary.contains(aldIterator.key())){
	    currentTarget	= doc.createElement("target");
	    targetText		= doc.createTextNode(QString::number(autoLinkTargetDictionary
		    .value(aldIterator.key())));
	    currentTarget.appendChild(targetText);
	    currentElement.appendChild(currentTarget);
	}
	autoLinkDictionaryElement.appendChild(currentElement);
    }
    doc.appendChild(autoLinkDictionaryElement);
    doc.insertBefore(doc.createProcessingInstruction("xml"
						    , "version=\"1.0\" encoding=\"UTF-8\"")
		    , doc.firstChild());

    QSettings settings;
    settings.beginGroup("account");
    QString dictionaryFileName = QString("%1/qtmautolinkdict.xml")
	.arg(settings.value("localStorageDirectory", "").toString());
    settings.endGroup();
    QFile dictionaryFile(dictionaryFileName);
    if(dictionaryFile.open(QIODevice::WriteOnly)){
	QTextStream dictionaryFileStream(&dictionaryFile);
	doc.save(dictionaryFileStream, 4);
	dictionaryFile.close();
    }else statusWidget->showMessage(tr("Could not write to dictionary file"), 2000);
}

void EditingWindow::loadAutoLinkDictionary(){
    QString	dictionaryFileName, errorString, currentKey, currentURL;
    int		currentTarget;
    QDomElement currentKeyElement, currentURLElement, currentTitleElement, currentTargetElement;
    QSettings	settings;
    int		errorLine, errorCol;
    bool	ok;

    settings.beginGroup("account");
    dictionaryFileName = QString("%1/qtmautolinkdict.xml").
	arg(settings.value("localStorageDirectory", "").toString());
    settings.endGroup();
    if(QFile::exists(dictionaryFileName)){
	QDomDocument	dictDoc("autoLinkDictionary");
	QFile		file(dictionaryFileName);
	if(! dictDoc.setContent(&file, true, &errorString, &errorLine, &errorCol))
		QMessageBox::warning(0, tr("Failed to read templates")
				    , QString(tr("Error: %1\n"
						 "Line %2, col %3"))
		    .arg(errorString).arg(errorLine)
		    .arg(errorCol));
	else{
	    QDomNodeList entryList = dictDoc.elementsByTagName("entry");
	    for(int i = 0; i < entryList.count(); i ++){
		currentKeyElement	= entryList.at(i).firstChildElement("key");
		currentURLElement	= entryList.at(i).firstChildElement("URL");
		currentTitleElement	= entryList.at(i).firstChildElement("title");
		currentTargetElement	= entryList.at(i).firstChildElement("target");
		if(currentKeyElement.isNull() || currentURLElement.isNull()) continue;
		else{
		    currentKey	= currentKeyElement.text();
		    currentURL	= currentURLElement.text();
		    autoLinkDictionary.insert(currentKey, currentURL);
		    if(! currentTitleElement.isNull()) autoLinkTitleDictionary.insert(currentKey, currentTitleElement.text());
		    if(! currentTargetElement.isNull()){
			currentTarget = currentTargetElement.text().toInt(&ok);
			if(ok) autoLinkTargetDictionary.insert(currentKey, currentTarget);
		    }
		}
	    }
	}
    }
}

// This method was adapted from the QByteArray source code in Qt v4.3.1

QByteArray EditingWindow::toBase64(QByteArray &qbarray){
    QProgressDialog pdialog(tr("Converting file to Base64")
			   , tr("Cancel")
			   , 0, qbarray.size(), 0);
    pdialog.setWindowModality(Qt::WindowModal);
    pdialog.setMinimumDuration(2000);
    pdialog.setWindowTitle("QTM");

    const char alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
							     "ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
    const char	padchar = '=';
    int		padlen	= 0;

    char	*data	= qbarray.data();
    int		size	= qbarray.size();

    QByteArray tmp;
    tmp.resize(((size * 4) / 3) + 3);

    int		i	= 0;
    char	*out	= tmp.data();
    while(i < size){
	int chunk = 0;
	chunk |= int (uchar(data[i ++])) << 16;
	if(i == size) padlen = 2;
	else{
	    chunk |= int (uchar(data[i ++])) << 8;
	    if(i == size) padlen = 1;
	    else chunk |= int (uchar(data[i ++]));
	}
	int	j	= (chunk & 0x00fc0000) >> 18;
	int	k	= (chunk & 0x0003f000) >> 12;
	int	l	= (chunk & 0x00000fc0) >> 6;
	int	m	= (chunk & 0x0000003f);
	*out ++ = alphabet[j];
	*out ++ = alphabet[k];
	if(padlen > 1) *out ++ = padchar;
	else *out ++ = alphabet[l];
	if(padlen > 0) *out ++ = padchar;
	else *out ++ = alphabet[m];
	if(pdialog.wasCanceled()) break;
	if(i % 1024 == 0){
	    pdialog.setValue(i);
	    qApp->processEvents();
	}
    }
    tmp.truncate(out - tmp.data());
    if(pdialog.wasCanceled()) return QByteArray();
    else return tmp;
}

void EditingWindow::addToConsole(const QString &t){
    QString text = t;
#ifndef NO_DEBUG_OUTPUT
    if(currentHttpBusiness == _metaWeblog_newMediaObject)
	// qDebug() << "Adding image to console";
#endif
    text.replace(QString("<string>%1</string>").arg(password)
		, tr("<string>(password omitted)</string>"));
    QTextCursor cursor(console->textCursor());

    cursor.movePosition(QTextCursor::End);
    console->setTextCursor(cursor);
    if(text.contains("<base64>"))
		console->insertPlainText(text.section("<base64>", 0, 0, QString::SectionIncludeTrailingSep)
		    .append(tr(" ... base64 encoded file omitted ... "))
		    .append(text.section("</base64>", 1, 1, QString::SectionIncludeLeadingSep)));
    else console->insertPlainText(text);
}

void EditingWindow::hideProgressBarIfMaximum(int val){
    if(val == cw.progressBar->maximum()) QTimer::singleShot(1000, this, SLOT(hideProgressBar()));
}

void EditingWindow::hideProgressBar(){
    cw.progressWidget->setVisible(false);
    cw.progressBar->reset();

#ifdef WIN_QTV
    taskbarProgress->setVisible(false);
    taskbarProgress->reset();
#endif
    if(QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
}

QString EditingWindow::checkBoxName(QString source){
    QChar firstLetter(source.at(1));
    return QString("%1%2%3")
	   .arg("ch")
	   .arg(firstLetter.toUpper())
	   .arg(source.section(1, - 1));
}

QString EditingWindow::decodeXmlEntities(QString source){
    QString	rv;
    int		i, tc;
    int		pos = 0;
    QChar	thisChar;
    QString	key, thisCap;
    bool	ok;
    QStringList lst, caps;

    QRegExp	rx1("&(\\S+);");	// Detects named entities
    QRegExp	rx2("&#(\\d{1-3});");	// Detects only numerically-defined characters
    int		index = rx1.indexIn(source);
    if(index == - 1) return source;
    else{
	lst = source.split(rx1);
	while((pos = rx1.indexIn(source, pos)) != - 1){
	    caps += rx1.cap(0);
		// qDebug() << "Matched" << rx1.cap( 0 );
	    pos += rx1.matchedLength();
	}
	// qDebug() << "Occurrences:" << caps.length();
	for(i = 0; i < lst.count(); ++ i){
	    rv		+= lst.value(i);
	    thisCap	= caps.value(i);
		// qDebug() << "i =" << i;
	    if(! thisCap.isNull()){
		if(rx2.exactMatch(caps.value(i))){
		    tc = rx2.cap(0).toInt(&ok);
		    if(ok && tc < 256) rv += QChar(tc);
		    else rv += caps.value(i);
		}else{
		    rx1 = QRegExp(rx1);
		    rx1.indexIn(thisCap);
		    key = rx1.cap(1);
			// qDebug() << "Using caps value" << i;
			// qDebug() << thisCap << "is not a numerical: caps:" << rx1.numCaptures();
		    if(xmlEntities.contains(key)) rv += xmlEntities.value(key);
		    else rv += rx1.cap(0);
		}
	    }
	}
	return rv;
    }
}

bool EditingWindow::event(QEvent *event){
    if(event->type() == QEvent::WindowActivate){
#ifdef Q_OS_MAC
	superMenu->setMainWindow(this);
#endif
	qtm->setMainWindow(this);
    }
    return QMainWindow::event(event);
}

bool EditingWindow::eventFilter(QObject *obj, QEvent *event){
    if(obj == EDITOR){
	if(event->type() == QEvent::KeyPress){
	    QKeyEvent *kpevent = static_cast<QKeyEvent *>(event);
	    if(kpevent->key() == Qt::Key_V && kpevent->modifiers() == Qt::ControlModifier){
		paste();
		return true;
	    }else return false;
	}else return false;
    }
    if(obj == previewWindow && event->type() == QEvent::KeyPress){
	QKeyEvent *kpevent = static_cast<QKeyEvent *>(event);
	if(kpevent->key() == Qt::Key_Escape && kpevent->modifiers() == 0){
	    doPreview(false);
	    return true;
	}else return false;
    }
    if(obj == console && event->type() == QEvent::KeyPress){
	QKeyEvent *kpevent = static_cast<QKeyEvent *>(event);
	if(kpevent->key() == Qt::Key_Escape && kpevent->modifiers() == 0){
	    handleConsole(false);
	    return true;
	}else return false;
    }
    return QMainWindow::eventFilter(obj, event);
}
