/***************************************************************************
 * SuperMenu.cc - Default menu for QTM on Mac OS X
 *
 * Copyright (C) 2007, Matthew J Smith
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

#include <QAction>
#include <QColor>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>
#include <QShowEvent>
#include <QString>
#include <QStringList>
#ifdef Q_OS_MAC
#include <QSettings>
#endif

#include "blogger.h"
#include "prefs_dialog.h"
#include "qtm_version.h.in"
#include "super_menu.h"
#include "sys_tray_icon.h"
#include "ui_aboutbox.h"
// #include "libraries/wyedit/editor_context_menu.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/wyedit/editor.h"
#include "main.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"

// #include "macFunctions.h"
#define CON_TRIG(a, b) connect(a, SIGNAL(triggered(bool)), _blogger, b)

SuperMenu::SuperMenu(Blogger* bloger_, QWidget* parent, SysTrayIcon* sti)
    : super(parent)
    , _sti(sti)
    //	  , _find_screen(_find_screen)
    , _blogger(bloger_)
    , _editor_docker(gl_paras->editor_docker())
{
	//	  , _vtab_record(vtab_record)
	auto _app = sapp_t::instance(); // qobject_cast<sapp_t *>(sapp_t::instance());
	// _editing_win	= 0;

	// setBackgroundRole(QPalette::Foreground);
	// setForegroundRole(QPalette::Background);
	// Global actions - not related to any editing window
	fileMenu = addMenu(tr("&File"));
	newEntryAction =
	    fileMenu->addAction(tr("&New entry"), this, SLOT(newEntry()));
	newEntryAction->setShortcut(QKeySequence(tr("Ctrl+N")));
	openAction = fileMenu->addAction(tr("&Open ..."), this, SLOT(choose()));
	openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
	openRecent = fileMenu->addAction(tr("Open recent"));
	// _sti	= sti;

	recentFilesMenu = new QMenu;
	noRecentFilesAction = new QAction(tr("No recent files"), this);
	noRecentFilesAction->setVisible(true);
	noRecentFilesAction->setEnabled(false);
	// recentFilesMenu->addAction( noRecentFilesAction );
	for (int i = 0; i < 10; ++i) {
		recentFileActions[i] = new QAction(this);
		// recentFileActions[i]->setVisible( false );
		// recentFilesMenu->addAction( recentFileActions[i] );
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}
	recentFiles = _app->recentFiles();
	updateRecentFileMenu();
	openRecent->setMenu(recentFilesMenu);
	connect(_app, SIGNAL(recent_files_updated(QList<app::RecentFile>)), this, SLOT(set_recent_files(QList<app::RecentFile>)));

	// File menu actions
	saveAction = fileMenu->addAction(tr("&Save"));
	saveAsAction = fileMenu->addAction(tr("Sa&ve as ..."));
	saveAllAction = fileMenu->addAction(tr("Save all ..."));
	exportAction = fileMenu->addAction(tr("&Export ..."));
	uploadAction = fileMenu->addAction(tr("&Upload ..."));
	fileMenu->addSeparator();
	// printAction = fileMenu->addAction( tr( "Print ..." ) );
	refreshBlogListAction = fileMenu->addAction(tr("Re&fresh blog list"));
	blogThisAction = fileMenu->addAction(tr("&Blog this!"));
	abortAction = fileMenu->addAction(tr("Abort this &job"));
	fileMenu->addSeparator();
	accountsAction = fileMenu->addAction(tr("&Accounts ..."));
	prefsAction =
	    fileMenu->addAction(tr("Preferences"), this, SLOT(getPreferences()));
	prefsAction->setMenuRole(QAction::PreferencesRole);
	saveBlogsAction = fileMenu->addAction(tr("Save blogs && categories"));
#ifndef Q_OS_MAC
	saveBlogsAction->setVisible(false);
#endif
	saveBlogsAction->setEnabled(false);
	qptAction = fileMenu->addAction(tr("Quickpost &templates ..."));
	clearConsoleAction = fileMenu->addAction(tr("C&lear console"));
	fileMenu->addSeparator();
	closeAction = fileMenu->addAction(tr("&Close"));
	quitAction = fileMenu->addAction(tr("&Quit"), this, SLOT(quit()));
	quitAction->setMenuRole(QAction::QuitRole);

	// Paste As menu actions
	pasteAsMenu = new QMenu(tr("Paste as ..."));
	pasteAsMarkedParasAction = pasteAsMenu->addAction(tr("Marked paragraphs"));
	pasteAsBlockquoteAction = pasteAsMenu->addAction(tr("Blockquote"));
	pasteAsMarkdownBlockquoteAction =
	    pasteAsMenu->addAction(tr("Markdown blockquote"));
	pasteAsUnorderedListAction = pasteAsMenu->addAction(tr("Unordered list"));
	pasteAsOrderedListAction = pasteAsMenu->addAction(tr("Ordered list"));

	// Создаётся контекстное меню
	// _editor_context_menu = new EditorContextMenu(this);
	// auto original_edit = addMenu("Original Edit");
	// original_edit->addMenu(this);
	// Edit menu actions
	editMenu = addMenu(tr("&Edit"));
	undoAction = editMenu->addAction(tr("&Undo"));
	redoAction = editMenu->addAction(tr("&Redo"));
	editMenu->addSeparator();
	cutAction = editMenu->addAction(tr("&Cut"));
	copyAction = editMenu->addAction(tr("C&opy"));
	copyURLAction = editMenu->addAction(tr("Copy uploaded file's location"));
	copyURLAction->setVisible(false);
	copyURLAction->setEnabled(false);
	pasteAction = editMenu->addAction(tr("Paste"));
	tidyPasteAction = editMenu->addAction(tr("Tidy paste"));
	pasteAsAction = pasteAsMenu->menuAction();
	editMenu->addMenu(pasteAsMenu);

	actionSelectAll = editMenu->addAction(tr("Select all"));
	actionSelectAll->setShortcut(Qt::CTRL + Qt::Key_A);
	actionEditImageProperties = editMenu->addAction(tr("Edit image properties"));
	editMenu->addSeparator();

	findAction = editMenu->addAction(tr("&Find ..."));
	findAgainAction = editMenu->addAction(tr("Find &again"));
	editMenu->addSeparator();
	boldAction = editMenu->addAction(tr("&Bold"));
	italicAction = editMenu->addAction(tr("&Italic"));
	underlineAction = editMenu->addAction(tr("U&nderline"));
	blockquoteAction = editMenu->addAction(tr("Blockquote"));
	editMenu->addSeparator();
	ulAction = editMenu->addAction(tr("Unordered &list"));
	olAction = editMenu->addAction(tr("&Ordered list"));

	// Paste As menu actions

	// Insert menu actions
	insMenu = addMenu(tr("&Insert"));
	linkAction = insMenu->addAction(tr("&Link ..."));
	clipLinkAction = insMenu->addAction(tr("Lin&k from clipboard"));
	selfLinkAction = insMenu->addAction(tr("&Self link"));
	autoLinkAction = insMenu->addAction(tr("&Auto link"));
	imageAction = insMenu->addAction(tr("I&mage ..."));
	clipImageAction = insMenu->addAction(tr("Ima&ge from clipboard"));
	moreAction = insMenu->addAction(tr("&More"));

	// View menu actions
	viewMenu = addMenu(tr("&View"));
	highlightingAction = viewMenu->addAction(tr("HTML High&lighting"));
	highlightingAction->setCheckable(true);
	viewMenu->addSeparator();
#ifndef Q_OS_MAC
	viewToolbarAction = viewMenu->addAction(tr("&Toolbar"));
	viewToolbarAction->setCheckable(true);
#endif
	showConsoleAction = viewMenu->addAction(tr("&Console"));
	showConsoleAction->setCheckable(true);
	previewAction = viewMenu->addAction(tr("Entry in p&review"));
	previewAction->setCheckable(true);
	viewMenu->addSeparator();
	viewBasicsAction = viewMenu->addAction(tr("&Basic entry settings"));
	viewCatsAction = viewMenu->addAction(tr("C&ategories"));
	viewExcerptAction = viewMenu->addAction(tr("E&xcerpt"));
	viewWPTagsAction = viewMenu->addAction(tr("&Wordpress-style tags"));
	viewTechTagsAction = viewMenu->addAction(tr("Tec&hnorati tags"));
	viewPingsAction = viewMenu->addAction(tr("Trackpack &pings"));

	// Category menu actions
	categoryMenu = addMenu(tr("&Category"));
	refreshCatsAction = categoryMenu->addAction(tr("&Refresh categories"));
	updateCatsAction =
	    categoryMenu->addAction(tr("&Update this post's categories"));
	addCatAction = categoryMenu->addAction(tr("&Add category"));

	// Services menu actions
	servicesMenu = addMenu(tr("&Services"));
	addWPTagAction = servicesMenu->addAction(tr("Add Wordpress-style tag ..."));
	removeWPTagAction =
	    servicesMenu->addAction(tr("Remove this Wordpress-style tag"));
	refreshWPTagsAction =
	    servicesMenu->addAction(tr("Refresh Wordpress-style tags"));
	servicesMenu->addSeparator();
	addTechTagAction = servicesMenu->addAction(tr("&Add Technorati tag ..."));
	addClipTechTagAction =
	    servicesMenu->addAction(tr("Add Technorati tag from clipboard"));
	removeTechTagAction = servicesMenu->addAction(tr("&Remove Technorati tag"));
	servicesMenu->addSeparator();
	addPingAction = servicesMenu->addAction(tr("Add trackpack &ping ..."));
	addClipPingAction = servicesMenu->addAction(tr("Add ping from clip&board"));
	removePingAction = servicesMenu->addAction(tr("Re&move ping"));

	print_menu = addMenu(tr("Print"));
	// move form wn_t::init_file_menu()
	file_print_action = print_menu->addAction(tr("&Print..."));
	file_print_action->setShortcut(QKeySequence::Print);

	file_print_preview_action = print_menu->addAction(tr("Print Preview..."));

	file_print_pdf_action = print_menu->addAction(tr("&Export PDF..."));
	file_print_pdf_action->setShortcut(Qt::CTRL + Qt::Key_D);

	// tb->addAction(a);

	// Help menu actions
	helpMenu = addMenu(tr("&Help"));
	aboutAction = helpMenu->addAction(tr("About QTM"), this, SLOT(about()));
	aboutAction->setMenuRole(QAction::AboutRole);
	helpMenu->addSeparator();
	whatsThisAction = helpMenu->addAction(tr("&What's this?"));

	saveAction->setShortcut(QKeySequence::fromString("Ctrl+S"));
	prefsAction->setShortcut(QKeySequence::fromString("Ctrl+,"));
	accountsAction->setShortcut(QKeySequence::fromString("Shift+Ctrl+,"));
	closeAction->setShortcut(QKeySequence::fromString("Ctrl+W"));
	abortAction->setShortcut(QKeySequence::fromString("Ctrl+A"));
	undoAction->setShortcut(QKeySequence::fromString("Ctrl+Z"));
	redoAction->setShortcut(QKeySequence::fromString("Shift+Ctrl+Z"));
	//	cutAction->setShortcut(QKeySequence::fromString("Ctrl+X"));//?, make system shortcut does not work
	//	copyAction->setShortcut(QKeySequence::fromString("Ctrl+C"));//?, make system shortcut does not work
	//	pasteAction->setShortcut(QKeySequence::fromString("Ctrl+V"));//?, make system shortcut does not work
	//	findAction->setShortcut(QKeySequence::fromString("Ctrl+F"));
	findAgainAction->setShortcut(QKeySequence::fromString("Ctrl+G"));
//	boldAction->setShortcut(QKeySequence::fromString("Ctrl+B"));
	italicAction->setShortcut(QKeySequence::fromString("Ctrl+I"));
	imageAction->setShortcut(QKeySequence::fromString("Ctrl+J"));
	linkAction->setShortcut(QKeySequence::fromString("Ctrl+L"));
	autoLinkAction->setShortcut(QKeySequence::fromString("Shift+Ctrl+L"));
	clipLinkAction->setShortcut(QKeySequence::fromString("Ctrl+U"));
	showConsoleAction->setShortcut(QKeySequence::fromString("Shift+Ctrl+V"));
	viewBasicsAction->setShortcut(QKeySequence("Ctrl+1"));
	viewCatsAction->setShortcut(QKeySequence("Ctrl+2"));
	viewExcerptAction->setShortcut(QKeySequence("Ctrl+3"));
	viewWPTagsAction->setShortcut(QKeySequence("Ctrl+4"));
	viewTechTagsAction->setShortcut(QKeySequence("Ctrl+5"));
	viewPingsAction->setShortcut(QKeySequence("Ctrl+6"));
	addWPTagAction->setShortcut(QKeySequence("Ctrl++"));
	removeWPTagAction->setShortcut(QKeySequence("Ctrl+-"));

	_blogger_actions << saveAction << saveAsAction << saveAllAction
			 << saveAllAction << exportAction << uploadAction
			 << refreshBlogListAction << blogThisAction << abortAction
			 << accountsAction << qptAction << clearConsoleAction
			 << closeAction;

	_blogger_actions << undoAction << redoAction << cutAction << copyAction
			 << pasteAction << tidyPasteAction << pasteAsAction
			 << actionSelectAll << actionEditImageProperties << findAction
			 << findAgainAction << boldAction << italicAction
			 << underlineAction << blockquoteAction << ulAction
			 << olAction;

	_blogger_actions << pasteAsMarkedParasAction << pasteAsBlockquoteAction
			 << pasteAsMarkdownBlockquoteAction
			 << pasteAsUnorderedListAction << pasteAsOrderedListAction;

	_blogger_actions << linkAction << clipLinkAction << selfLinkAction
			 << autoLinkAction << imageAction << clipImageAction
			 << moreAction;

	_blogger_actions << highlightingAction << showConsoleAction << previewAction
			 << viewBasicsAction << viewCatsAction << viewExcerptAction
			 << viewWPTagsAction << viewTechTagsAction << viewPingsAction;

	_blogger_actions << refreshCatsAction << updateCatsAction << addCatAction;

	_blogger_actions << addWPTagAction << removeWPTagAction << refreshWPTagsAction
			 << addTechTagAction << addClipTechTagAction
			 << removeTechTagAction << addPingAction << addClipPingAction
			 << removePingAction;

	_blogger_actions << whatsThisAction;
	/*editingMenus << editMenu << insMenu << viewMenu << categoryMenu <<
     servicesMenu
                << helpMenu; */
	Q_FOREACH (QAction* a, _blogger_actions)
		a->setEnabled(false);
	connect(_app, &sapp_t::blogger_changed, this, &SuperMenu::blogger_changed);
	connect(_app, &sapp_t::lastWindowClosed, this, &SuperMenu::handleLastWindowClosed);
	blogger_changed(bloger_);
}

void SuperMenu::setEditActionsEnabled(bool state)
{
	QList<QAction*> editActions;

	editActions << cutAction << copyAction << pasteAction << tidyPasteAction
		    << pasteAsMarkedParasAction << pasteAsBlockquoteAction
		    << pasteAsMarkdownBlockquoteAction << pasteAsUnorderedListAction
		    << pasteAsOrderedListAction << undoAction << redoAction
		    << boldAction << italicAction << underlineAction
		    << blockquoteAction << ulAction << olAction << linkAction
		    << clipLinkAction << selfLinkAction << autoLinkAction
		    << imageAction << clipImageAction << moreAction
		    << refreshCatsAction << updateCatsAction << addCatAction
		    << addWPTagAction << removeWPTagAction << refreshWPTagsAction
		    << addTechTagAction << addClipTechTagAction << removeTechTagAction
		    << addPingAction << addClipPingAction << removePingAction;
	Q_FOREACH (QAction* a, editActions)
		a->setEnabled(state);
}

void SuperMenu::newEntry()
{
	blogger_ref c(new Blogger());
	c->setSTI(_sti);
	c->setWindowTitle(QObject::tr((program_title_string + " - new entry [*]").c_str()));
	if (_blogger)
		Blogger::positionWidget(c, _blogger);
	// setNoStatusBar( c );
	c->show();
	QApplication::alert(c);
}

void SuperMenu::choose(QString fname)
{
	sapp_t* qtm = qobject_cast<sapp_t*>(sapp_t::instance());
	QSettings settings;
	QString local_storage_file_ext, localStorageDirectory;
	QString fn; // filename
	QStringList filesSelected;

	settings.beginGroup("account");
	local_storage_file_ext =
	    settings.value("local_storage_file_ext", "cqt").toString();
	localStorageDirectory =
	    settings.value("editors_shared_directory", "").toString();
	settings.endGroup();

	QString extn(
	    QString("%1 (*.%2)").arg(tr("Blog entries")).arg(local_storage_file_ext));
	if (fname.isEmpty())
		fn = QFileDialog::getOpenFileName(0, tr("Choose a file to open"), localStorageDirectory, extn);
	else
		fn = fname;
	if (!fn.isEmpty()) {
		blogger_ref e(new Blogger());
		if (!e->load(fn, true)) {
			QMessageBox::warning(0, program_title_qstring, tr("Could not load the file you specified."), QMessageBox::Cancel, QMessageBox::NoButton);
			e->deleteLater();
		} else {
			e->setSTI(_sti);
			qtm->add_recent_file(e->postTitle(), fn);
			// setNoStatusBar( e );
			e->show();
			e->activateWindow();
			QApplication::alert(e);
		}
	}
}

void SuperMenu::updateRecentFileMenu()
{
	QString text, t;
	int j;
	QMutableListIterator<std::shared_ptr<app::RecentFile>> i(recentFiles);
	while (i.hasNext())
		if (!QFile::exists(i.next()->filename))
			i.remove();
	recentFilesMenu->clear();
	for (j = 0; j < 10; ++j) {
		if (j < recentFiles.count()) {
			t = recentFiles.value(j)->title.section(' ', 0, 5);
			if (t != recentFiles.value(j)->title)
				t.append(tr(" ..."));
#ifdef Q_OS_MAC
			text = recentFiles.value(j).title.isEmpty() ? recentFiles.value(j).filename.section("/", -1, -1) : t.replace('&', "&&");
#else
			if (j == 9)
				text = tr("1&0 %1").arg(
				    recentFiles.value(j)->title.isEmpty() ? recentFiles.value(j)->filename.section("/", -1, -1) : t.replace('&', "&&"));
			else
				text = tr("&%1 %2").arg(j + 1).arg(
				    recentFiles.value(j)->title.isEmpty() ? recentFiles.value(j)->filename.section("/", -1, -1) : t.replace('&', "&&"));
#endif
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles.value(j)->filename);
			recentFilesMenu->addAction(recentFileActions[j]);
			recentFilesMenu->removeAction(noRecentFilesAction);
		} else {
			// recentFileActions[j]->setVisible( false );
			if (!recentFiles.count())
				recentFilesMenu->addAction(noRecentFilesAction);
		}
	}
}

void SuperMenu::setConsoleEnabled(bool enabled)
{
	showConsoleAction->setEnabled(enabled);
}

void SuperMenu::setCategoriesEnabled(bool enabled)
{
	refreshCatsAction->setEnabled(enabled);
	updateCatsAction->setEnabled(enabled);
}

void SuperMenu::handleNewWindowAtStartup()
{
#ifdef Q_OS_MAC
	bool newWindow;
	QSettings settings;

// Because of a bug in Qt 5.2, the global menu won't show on startup if there's
// no new window. Hence, always a new window on startup in Qt 5.
#if QT_VERSION <= 0x050000
	newWindow = settings.value("sysTrayIcon/newWindowAtStartup", true).toBool();
#else
	newWindow = true;
#endif
	if (newWindow) {
		EditingWindow* c = new EditingWindow(_find_screen, _editentry, Qt::Widget);
		c->setSTI(_sti);
		c->setWindowTitle(tr((program_title_string + " - new entry [*]").c_str()));
		c->show();
		c->activateWindow();
	}
#endif
}

void SuperMenu::set_recent_files(const QList<std::shared_ptr<app::RecentFile>>& rfs)
{
	recentFiles = rfs;
	updateRecentFileMenu();
}

void SuperMenu::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
		choose(action->data().toString());
}

void SuperMenu::quit()
{
	int edwins = 0;
	int a;

	QWidgetList tlw = sapp_t::instance()->topLevelWidgets();
	for (a = 0; a < tlw.size(); a++) {
		if ((QString(tlw[a]->metaObject()->className()) == "EditingWindow") &&
		    tlw[a]->isVisible())
			edwins++;
	}
	if (!edwins)
		QCoreApplication::quit();
	else {
		sapp_t::instance()->setQuitOnLastWindowClosed(true);
		sapp_t::instance()->closeAllWindows();
	}
}

void SuperMenu::about()
{
	QDialog about_box(0);
	Ui::AboutBox abui;

	abui.setupUi(&about_box);

	QString about_string =
	    abui.label->text()
		.replace("(VERSION)", app_version) // QTM_VERSION
		.replace("(QT_VERSION)", QString(qVersion()))
		.replace("(YEAR)", QString::number(QDate::currentDate().year()));
	abui.label->setText(about_string);
	about_box.exec();
}

void SuperMenu::showEvent(QShowEvent* event)
{
	aboutAction->setVisible(true);
	QMenu::showEvent(event); // QMenuBar::showEvent(event);
}

void SuperMenu::getPreferences()
{
#ifdef Q_OS_MAC
	QSettings settings("catkin.blogistan.co.uk", program_title_string.c_str());
	QPalette palette, widgetPalette;

	bool categoriesEnabled, useNewWindows, savePassword, postAsSave,
	    allowComments, allowTB, copyTitle, allowRegexSearch, useMarkdown,
	    doMarkdownWhenPosting, stripParaTags, suppressReadMore, useUtf8;
	QString editorFontString, previewFontString, consoleFontString;
	QString perlPath, markdownPath;
	QColor editorBgColor, editorFgColor, previewBgColor, previewFgColor,
	    consoleBgColor, consoleFgColor;
	QColor tagBgColor, tagFgColor, entityBgColor, entityFgColor, commentBgColor,
	    commentFgColor, linkFgColor, linkBgColor;
	bool tagBold, tagItalic, entityBold, entityItalic, commentBold, commentItalic,
	    linkBold, linkItalic, linkUnderline;
	QString local_storage_file_ext, localStorageDirectory;

	QString mdPathString =
	    QString("%1/../Resources").arg(sapp_t::instance()->applicationDirPath());
	QDir mdPath(mdPathString);
	QString defaultMarkdownPath =
	    QString("%1/Markdown.pl").arg(mdPath.canonicalPath());

	QString defaultPerlLocation("/usr/bin/perl");

	settings.beginGroup("account");
	local_storage_file_ext =
	    settings.value("local_storage_file_ext", "cqt").toString();
	useMarkdown = settings.value("useMarkdown", true).toBool();
	perlPath = settings.value("perlPath", defaultPerlLocation).toString();
	markdownPath = settings.value("markdownPath", defaultMarkdownPath).toString();
	doMarkdownWhenPosting =
	    settings.value("doMarkdownWhenPosting", useMarkdown).toBool();
	stripParaTags = settings.value("stripParaTags", false).toBool();
	categoriesEnabled = settings.value("categoriesEnabled", true).toBool();
	useNewWindows = settings.value("useNewWindows", true).toBool();
	savePassword = settings.value("savePassword", false).toBool();
	postAsSave = settings.value("postAsSave", true).toBool();
	allowComments = settings.value("allowComments", true).toBool();
	allowTB = settings.value("allowTB", true).toBool();
	suppressReadMore = settings.value("suppressReadMore", true).toBool();
	useUtf8 = settings.value("useUtf8", false).toBool();
#ifdef USE_SYSTRAYICON
	copyTitle = settings.value("copyTitle", true).toBool();
#endif
	allowRegexSearch = settings.value("allowRegexSearch", false).toBool();
	settings.endGroup();
	settings.beginGroup("fonts");
	editorFontString = settings.value("editorFontString", "").toString();
	previewFontString = settings.value("previewFontString", "").toString();
	consoleFontString = settings.value("consoleFontString", "").toString();
	editorBgColor = QColor(settings.value("editorBgColor", "#FFFFFF").toString());
	editorFgColor = QColor(settings.value("editorFgColor", "#000000").toString());
	previewBgColor =
	    QColor(settings.value("previewBgColor", "#FFFFFF").toString());
	previewFgColor =
	    QColor(settings.value("previewFgColor", "#000000").toString());
	consoleBgColor =
	    QColor(settings.value("consoleBgColor", "#FFFFFF").toString());
	consoleFgColor =
	    QColor(settings.value("consoleFgColor", "#000000").toString());
	settings.endGroup();

	settings.beginGroup("highlighting");
	tagBgColor = QColor(settings.value("tagBgColor", "#FFFFFF").toString());
	tagFgColor = QColor(settings.value("tagFgColor", "#008000").toString());
	entityBgColor = QColor(settings.value("entityBgColor", "#FFFFFF").toString());
	entityFgColor = QColor(settings.value("entityFgColor", "#000080").toString());
	commentBgColor =
	    QColor(settings.value("commentBgColor", "#FFFFFF").toString());
	commentFgColor =
	    QColor(settings.value("commentFgColor", "#008080").toString());
	linkBgColor = QColor(settings.value("linkBgColor", "#ffffff").toString());
	linkFgColor = QColor(settings.value("linkFgColor", "#0000ff").toString());
	tagBold = settings.value("tagBold", false).toBool();
	tagItalic = settings.value("tagItalic", false).toBool();
	entityBold = settings.value("entityBold", false).toBool();
	entityItalic = settings.value("entityItalic", false).toBool();
	commentBold = settings.value("commentBold", false).toBool();
	commentItalic = settings.value("commentItalic", false).toBool();
	linkBold = settings.value("linkBold", false).toBool();
	linkItalic = settings.value("linkItalic", false).toBool();
	linkUnderline = settings.value("linkUnderline", true).toBool();
	settings.endGroup();

	PrefsDialog prefsDialog;
	if (localStorageDirectory.isEmpty()) {
		QString lsd = QString(QString("%1/") + gl_para::_blog_root_dir).arg(QDir::homePath());
		prefsDialog.leLocalDir->setText(lsd);
	} else
		prefsDialog.leLocalDir->setText(localStorageDirectory);
	prefsDialog.leFileExtn->setText(local_storage_file_ext);
	prefsDialog.chUseNewWindows->setCheckState(useNewWindows ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chPostAsSave->setCheckState(postAsSave ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chAllowComments->setCheckState(allowComments ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);
#if defined USE_SYSTRAYICON
	prefsDialog.chCopyTitle->setCheckState(copyTitle ? Qt::Checked : Qt::Unchecked);
#else
	prefsDialog.chCopyTitle->setVisible(false);
#endif
	prefsDialog.chUseMarkdown->setCheckState(useMarkdown ? Qt::Checked : Qt::Unchecked);
	prefsDialog.lePerlPath->setText(perlPath);
	prefsDialog.leMarkdownPath->setText(markdownPath);
	prefsDialog.chDoMarkdownWhenPosting->setCheckState(
	    doMarkdownWhenPosting ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chStripParaTags->setCheckState(stripParaTags ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chStripParaTags->setEnabled(doMarkdownWhenPosting);
	if (!useMarkdown) {
		prefsDialog.lineBelowFonts->setVisible(false);
		prefsDialog.gbMarkdown->setVisible(false);
	}
	prefsDialog.chSuppressReadMore->setCheckState(
	    suppressReadMore ? Qt::Checked : Qt::Unchecked);
	prefsDialog.chUseUtf8->setCheckState(useUtf8 ? Qt::Checked : Qt::Unchecked);

	prefsDialog.chAllowRegexSearch->setCheckState(
	    allowRegexSearch ? Qt::Checked : Qt::Unchecked);

#ifdef Q_OS_MAC
	prefsDialog.setWindowFlags(Qt::Sheet);
#endif

	prefsDialog.tabWidget->setTabText(0, tr("General"));
	prefsDialog.tabWidget->setTabText(1, tr("Fonts && Markdown"));
	prefsDialog.tabWidget->setTabText(2, tr("Syntax highlighting"));
	prefsDialog.tabWidget->setCurrentIndex(0);

	QFont editorFont, previewFont, consoleFont;
	editorFont.fromString(editorFontString);
	previewFont.fromString(previewFontString);
	consoleFont.fromString(consoleFontString);
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
// prefsDialog.cbSTI2ClickFunction->setCurrentIndex( STI2ClickFunction );
#else
// prefsDialog.cbSTI2ClickFunction->hide();
// prefsDialog.label_10->hide();
#endif

	prefsDialog.resize(QSize(prefsDialog.width(), prefsDialog.minimumHeight()));

	// Disable cbStripParaTags as this feature is not properly implemented
	prefsDialog.chStripParaTags->setVisible(false);
	if (prefsDialog.exec()) {
#ifndef NO_DEBUG_OUTPUT
#endif
		localStorageDirectory = prefsDialog.leLocalDir->text();
		local_storage_file_ext = prefsDialog.leFileExtn->text();
		useNewWindows = prefsDialog.chUseNewWindows->isChecked();
		postAsSave = prefsDialog.chPostAsSave->isChecked();
		allowComments = prefsDialog.chAllowComments->isChecked();
		allowTB = prefsDialog.chAllowTB->isChecked();
		copyTitle = prefsDialog.chCopyTitle->isChecked();
		useMarkdown = prefsDialog.chUseMarkdown->isChecked();
		perlPath = prefsDialog.lePerlPath->text();
		markdownPath = prefsDialog.leMarkdownPath->text();
		doMarkdownWhenPosting = prefsDialog.chDoMarkdownWhenPosting->isChecked();
		stripParaTags = prefsDialog.chStripParaTags->isChecked();
		allowRegexSearch = prefsDialog.chAllowRegexSearch->isChecked();
		suppressReadMore = prefsDialog.chSuppressReadMore->isChecked();
		useUtf8 = prefsDialog.chUseUtf8->isChecked();

#ifndef NO_DEBUG_OUTPUT
// qDebug( "setting fonts" );
#endif
		QFont ef = prefsDialog.fcbComposer->currentFont();
		ef.setPointSize(prefsDialog.sbComposer->value());
		editorFontString = ef.toString();

		QFont pf = prefsDialog.fcbPreview->currentFont();
		pf.setPointSize(prefsDialog.sbPreview->value());
		previewFontString = pf.toString();

		QFont cf = prefsDialog.fcbConsole->currentFont();
		cf.setPointSize(prefsDialog.sbConsole->value());
		consoleFontString = cf.toString();

		// Set colours
		palette = prefsDialog.lEditorExampleText->palette();
		editorBgColor = palette.color(QPalette::Window);
		editorFgColor = palette.color(QPalette::WindowText);

		palette = prefsDialog.lPreviewExampleText->palette();
		previewBgColor = palette.color(QPalette::Window);
		previewFgColor = palette.color(QPalette::WindowText);

		palette = prefsDialog.lConsoleExampleText->palette();
		consoleBgColor = palette.color(QPalette::Window);
		consoleFgColor = palette.color(QPalette::WindowText);

		palette = prefsDialog.lTagColorDemo->palette();
		tagBgColor = palette.color(QPalette::Window);
		tagFgColor = palette.color(QPalette::WindowText);

		palette = prefsDialog.lEntityColorDemo->palette();
		entityBgColor = palette.color(QPalette::Window);
		entityFgColor = palette.color(QPalette::WindowText);

		palette = prefsDialog.lCommentColorDemo->palette();
		commentBgColor = palette.color(QPalette::Window);
		commentFgColor = palette.color(QPalette::WindowText);

		tagBold = prefsDialog.chTagBold->isChecked();
		tagItalic = prefsDialog.chTagItalic->isChecked();
		entityBold = prefsDialog.chEntityBold->isChecked();
		entityItalic = prefsDialog.chEntityItalic->isChecked();
		commentBold = prefsDialog.chCommentBold->isChecked();
		commentItalic = prefsDialog.chCommentItalic->isChecked();
		/*
             #if defined USE_SYSTRAYICON
                       STI2ClickFunction =
       prefsDialog.cbSTI2ClickFunction->currentIndex();
                       if( sti ) {
             #ifndef NO_DEBUG_OUTPUT
                       // qDebug( "setting double click function" );
             #endif
                       sti->setDoubleClickFunction( STI2ClickFunction );
                       sti->setCopyTitle( copyTitle );
                       }
             #endif
             */
		// Handle local directory settings; a default is used if none is specified
		if (localStorageDirectory.isEmpty())
			localStorageDirectory = QString(QString("%1/") + gl_para::_blog_root_dir).arg(QDir::homePath());
		QDir qd(localStorageDirectory);
		if (!qd.exists())
			if (!qd.mkpath(localStorageDirectory))
				QMessageBox::information(0, tr("Warning"), tr("Could not create QTM directory."), QMessageBox::Cancel);
		settings.setValue("application/version", QTM_VERSION);
		settings.beginGroup("account");
		settings.setValue("editors_shared_directory", localStorageDirectory.replace("~", QDir::homePath()));
		settings.setValue("local_storage_file_ext", local_storage_file_ext);
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

		/* #ifdef USE_SYSTRAYICON
                       settings.beginGroup( "sysTrayIcon" );
                       settings.setValue( "doubleClickFunction",
       STI2ClickFunction );
                       settings.endGroup();
             #endif */
	}
#endif
}

void SuperMenu::blogger_changed(Blogger* bloger_)
{
	//	_bloger	= 0;
	//	bloger_	= bloger_;//qobject_cast<Bloger *>(bloger_);
	Q_FOREACH (QAction* a, _blogger_actions)
		disconnect(a, SIGNAL(triggered()), 0, 0);
	if (!bloger_)
		Q_FOREACH (QAction* b, _blogger_actions)
			b->setEnabled(false);
	else {
		/*Q_FOREACH( QMenu *d, editingMenus ) {
                              d->setEnabled( true );
                            } */
		Q_FOREACH (QAction* c, _blogger_actions)
			c->setEnabled(true);
		// Assign file menu actions
		connect(saveAction, SIGNAL(triggered(bool)), bloger_, SLOT(save()));
		connect(saveAsAction, SIGNAL(triggered(bool)), bloger_, SLOT(saveAs()));
		connect(saveAllAction, SIGNAL(triggered(bool)), bloger_, SLOT(saveAll()));
		connect(exportAction, SIGNAL(triggered(bool)), bloger_, SLOT(exportEntry()));
		connect(uploadAction, SIGNAL(triggered(bool)), bloger_, SLOT(uploadFile()));
		connect(refreshBlogListAction, SIGNAL(triggered(bool)), bloger_, SLOT(refreshBlogList()));
		connect(blogThisAction, SIGNAL(triggered()), bloger_, SLOT(newMTPost()));
		connect(abortAction, SIGNAL(triggered()), bloger_, SLOT(stopThisJob()));
		connect(accountsAction, SIGNAL(triggered()), bloger_, SLOT(getAccounts()));
#ifndef Q_OS_MAC
		CON_TRIG(prefsAction, SLOT(getPreferences()));
#endif
		CON_TRIG(saveBlogsAction, SLOT(saveAccountsDom()));
		connect(qptAction, SIGNAL(triggered()), bloger_, SLOT(configureQuickpostTemplates()));
		connect(clearConsoleAction, SIGNAL(triggered()), bloger_, SLOT(clearConsole()));
		connect(closeAction, &QAction::triggered, [&](bool checked) {
			(void)checked;
			auto br = _blogger->browser();
			if (br) {
				//				br->self_close_request();
				br->close();
			}
		});
		// SLOT(close()), temporary polic, current I only have one currentWindow

		connect(undoAction, &QAction::triggered, [&](bool) {
			bloger_->editor()->on_undo();
		}); // _editing_win, &EditingWindow::undo);
		connect(redoAction, &QAction::triggered, [&](bool) {
			bloger_->editor()->on_redo();
		}); // _editing_win, SLOT(redo()));

		// Assign edit menu actions
		connect(cutAction, &QAction::triggered, [&](bool) {
			bloger_->editor()->cut();
		}); // _editing_win, SLOT(cut()));
		connect(copyAction, &QAction::triggered, [&](bool) { bloger_->copy(); });
		CON_TRIG(copyURLAction, SLOT(copyURL()));
		connect(pasteAction, &QAction::triggered, [&](bool) {
			bloger_->paste();
		}); // _editing_win->editor(), &Editor::paste);
		connect(tidyPasteAction, &QAction::triggered, [&](bool) { bloger_->tidyPaste(); });

		//

		// Сигналы контекстного меню

		connect(actionSelectAll, &QAction::triggered, [&](bool) { bloger_->editor()->on_selectAll(); });
		connect(actionEditImageProperties, &QAction::triggered, [&](bool) {
			bloger_->editor()->on_context_menu_edit_image_properties();
		});

		connect(file_print_action, &QAction::triggered, bloger_, &Blogger::file_print);
		connect(file_print_preview_action, &QAction::triggered, bloger_, &Blogger::file_print_preview);
		connect(file_print_pdf_action, &QAction::triggered, bloger_, &Blogger::file_print_pdf);
		//

		CON_TRIG(findAction, SLOT(handleFind()));
		CON_TRIG(findAgainAction, SLOT(findAgain()));
		connect(boldAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeBold()));
		connect(italicAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeItalic()));
		connect(underlineAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeUnderline()));
		connect(blockquoteAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeBlockquote()));
		connect(ulAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeUnorderedList()));
		connect(olAction, SIGNAL(triggered(bool)), bloger_, SLOT(makeOrderedList()));

		// Assign "paste as" sub-menu actions
		CON_TRIG(pasteAsMarkedParasAction, SLOT(pasteAsMarkedParagraphs()));
		CON_TRIG(pasteAsBlockquoteAction, SLOT(pasteAsBlockquote()));
		CON_TRIG(pasteAsMarkdownBlockquoteAction, SLOT(pasteAsMarkdownBlockquote()));
		CON_TRIG(pasteAsUnorderedListAction, SLOT(pasteAsUnorderedList()));
		CON_TRIG(pasteAsOrderedListAction, SLOT(pasteAsOrderedList()));

		// Assign insert menu actions
		connect(linkAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertLink()));
		connect(clipLinkAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertLinkFromClipboard()));
		connect(selfLinkAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertSelfLink()));
		connect(autoLinkAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertAutoLink()));
		connect(imageAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertImage()));
		connect(clipImageAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertImageFromClipboard()));
		connect(moreAction, SIGNAL(triggered(bool)), bloger_, SLOT(insertMore()));

		// Assign view menu actions
		connect(highlightingAction, SIGNAL(triggered(bool)), bloger_, SLOT(setHighlighting(bool)));
#ifndef Q_OS_MAC
		CON_TRIG(viewToolbarAction, SLOT(setToolBarVisible(bool)));
#endif
		connect(showConsoleAction, SIGNAL(triggered(bool)), bloger_, SLOT(handleConsole(bool)));
		/* connect( showConsoleAction, SIGNAL( triggered( bool ) ),
                                this, SLOT( setConsoleActionTitle( bool ) ) );
       */
		connect(previewAction, SIGNAL(triggered(bool)), bloger_, SLOT(doPreview(bool)));
		/* connect( previewAction, SIGNAL( triggered( bool ) ),
                                 this, SLOT( setPreviewActionTitle( bool ) ) );
       */
		connect(viewBasicsAction, SIGNAL(triggered(bool)), bloger_, SLOT(doViewBasicSettings()));
		connect(viewCatsAction, SIGNAL(triggered(bool)), bloger_, SLOT(doViewCategories()));
		connect(viewExcerptAction, SIGNAL(triggered(bool)), bloger_, SLOT(doViewExcerpt()));

		// At this point I defined the CON_TRIG macro because typing all that was
		// getting slow. The following do the same thing as the preceding.

		CON_TRIG(viewWPTagsAction, SLOT(doViewKeywordTags()));
		CON_TRIG(viewTechTagsAction, SLOT(doViewTechTags()));
		CON_TRIG(viewPingsAction, SLOT(doViewTBPings()));

		// Assign category menu actions
		CON_TRIG(refreshCatsAction, SLOT(refreshCategories()));
		CON_TRIG(updateCatsAction, SLOT(updatePostCategories()));
		CON_TRIG(addCatAction, SLOT(newCategory()));

		// Assign services menu actions
		CON_TRIG(addWPTagAction, SLOT(addKeywordTag()));
		CON_TRIG(removeWPTagAction, SLOT(removeKeywordTag()));
		CON_TRIG(refreshWPTagsAction, SLOT(refreshKeywordTags()));
		CON_TRIG(addTechTagAction, SLOT(addTechTag()));
		CON_TRIG(addClipTechTagAction, SLOT(addClipTag()));
		CON_TRIG(removeTechTagAction, SLOT(removeTechTag()));
		CON_TRIG(addPingAction, SLOT(addTBPing()));
		CON_TRIG(addClipPingAction, SLOT(addClipTBPing()));
		CON_TRIG(removePingAction, SLOT(removeTBPing()));

		CON_TRIG(whatsThisAction, SLOT(doWhatsThis()));
	}
}

void SuperMenu::handleConsole(bool isChecked)
{
	if (_blogger)
		_blogger->handleConsole(isChecked);
}

void SuperMenu::setConsoleActionTitle(bool consoleShown)
{
	showConsoleAction->setText(consoleShown ? tr("Exit &console") : tr("&Console"));
	if (showConsoleAction->isChecked() != consoleShown)
		showConsoleAction->setChecked(consoleShown);
}

void SuperMenu::setPreviewActionTitle(bool previewShown)
{
	previewAction->setText(previewShown ? tr("Exit p&review") : tr("Entry in p&review"));
	if (previewAction->isChecked() != previewShown)
		previewAction->setChecked(previewShown);
}

void SuperMenu::setHighlightingChecked(bool state)
{
	highlightingAction->setChecked(state);
}

void SuperMenu::handleLastWindowClosed()
{
	Q_FOREACH (QAction* a, _blogger_actions)
		a->setEnabled(false);
	if (_blogger) _blogger->close_trigger_from_others()(nullptr);
}

// Показывать или нет пункт редактирования свойств изображения
void SuperMenu::set_edit_image_properties(bool is_image_select)
{
	qDebug() << "In SuperMenu::set_edit_image_properties() " << is_image_select;

	actionEditImageProperties->setVisible(is_image_select);
}
