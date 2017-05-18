/***************************************************************************
 * SuperMenu.h - Default menu for QTM on Mac OS X
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

//#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/blogger.h"
#include "libraries/qtm/qij_search_widget.h"
#include "libraries/wyedit/editor_context_menu.h"
#include "utility/lease.h"
#include <QList>
#include <QMenu>
#include <QMenuBar>

class QAction;
class QMenu;
class Blogger;
class SysTrayIcon;
class QShowEvent;
class sapp_t;
class EditorContextMenu;
class rs_t;
class Blogger;

namespace web {
	template <typename>
	class Docker;
}


namespace app {
	struct RecentFile;
}

class SuperMenu : public QMenu // EditorContextMenu	// QMenuBar
{
	Q_OBJECT
	typedef QMenu super;

    public:
	SuperMenu(Blogger* bloger_, QWidget* parent = 0, SysTrayIcon* sti = 0);
	void setEditActionsEnabled(bool);
	QAction* getCutAction()
	{
		return cutAction;
	}
	QAction* getCopyAction()
	{
		return copyAction;
	}
	QAction* getPasteAction()
	{
		return pasteAction;
	}
	QAction* getUndoAction()
	{
		return undoAction;
	}
	QAction* getRedoAction()
	{
		return redoAction;
	}
	QAction* getConsoleAction()
	{
		return showConsoleAction;
	}
	QAction* getPreviewAction()
	{
		return previewAction;
	}
	void setConsoleEnabled(bool);
	void setCategoriesEnabled(bool);
	void handleNewWindowAtStartup();

	QMenu *fileMenu, *editMenu, *insMenu, *viewMenu, *categoryMenu, *servicesMenu,
	    *print_menu, *helpMenu;
	QMenu *recentFilesMenu, *pasteAsMenu;

	// File menu actions
	QAction *newEntryAction, *openAction, *openRecent, *saveAction, *saveAsAction,
	    *saveAllAction, *exportAction, *uploadAction, *refreshBlogListAction,
	    *blogThisAction, *abortAction, *accountsAction, *prefsAction,
	    *saveBlogsAction, *qptAction, *clearConsoleAction, *closeAction,
	    *quitAction;

	// Edit menu actions
	QAction *cutAction, *copyAction, *copyURLAction, *pasteAction,
	    *tidyPasteAction, *pasteAsAction, *undoAction, *redoAction, *findAction,
	    *findAgainAction, *boldAction, *italicAction, *underlineAction,
	    *blockquoteAction, *paraAction, *ulAction, *olAction;

	// Paste As sub-menu actions
	QAction *pasteAsMarkedParasAction, *pasteAsBlockquoteAction,
	    *pasteAsMarkdownBlockquoteAction, *pasteAsUnorderedListAction,
	    *pasteAsOrderedListAction;

	// Insert menu actions
	QAction *linkAction, *clipLinkAction, *selfLinkAction, *autoLinkAction,
	    *imageAction, *clipImageAction, *moreAction;

	// View menu actions
	QAction *highlightingAction, *showConsoleAction, *previewAction,
	    *viewBasicsAction, *viewCatsAction, *viewExcerptAction, *viewWPTagsAction,
	    *viewTechTagsAction, *viewPingsAction;
//#ifndef Q_WS_MAC
//	QAction* viewToolbarAction;
//#endif

	// Category menu actions
	QAction *refreshCatsAction, *updateCatsAction, *addCatAction;

	// Services menu actions
	QAction *addWPTagAction, *removeWPTagAction, *refreshWPTagsAction,
	    *addTechTagAction, *addClipTechTagAction, *removeTechTagAction,
	    *addPingAction, *addClipPingAction, *removePingAction;
	QAction *file_print_action, *file_print_preview_action,
	    *file_print_pdf_action;
	QAction *actionSelectAll, *actionEditImageProperties;

	// Help menu actions
	QAction *aboutAction, *whatsThisAction;

	void set_edit_image_properties(bool is_image_select);
    public slots:
	void set_recent_files(const QList<std::shared_ptr<app::RecentFile>>&);
	void openRecentFile();
	void blogger_changed(Blogger* bloger_);
	void setConsoleActionTitle(bool);
	void setPreviewActionTitle(bool);
	void setHighlightingChecked(bool);

    private:
	EditorContextMenu* _editor_context_menu;
	QList<QAction*> _blogger_actions;
	// QList<QMenu *> editingMenus;
	QAction* noRecentFilesAction;
	SysTrayIcon* _sti;
	QList<std::shared_ptr<app::RecentFile>> recentFiles;
	QAction* recentFileActions[10];
	//	sapp_t *_app;
	blogger_ref _blogger;

	ts_t* _tree_screen;
	//	web::Docker<web::Browser>* _browser_docker;
	wn_t* _main_window;
	web::Profile* _profile;
	Qt::WindowFlags _flags;
	QString _style_source;

	FindScreen* _find_screen;
	web::Docker<Blogger>* _editor_docker;
	//	Bloger_blogerer;
	//	HidableTab *_vtab_record;

	// QSplitter *_splitter;
	// std::shared_ptr<QSettings>  _splitter_config;
	// QString _splitter_groupname;
	// QString _splitter_sizelist;

	void updateRecentFileMenu();

    private slots:
	void newEntry();
	void choose(QString fname = QString());
	void quit();
	void about();
	void getPreferences();
	void handleLastWindowClosed();
	// void doPreview( bool );
	void handleConsole(bool);


    protected:
	virtual void showEvent(QShowEvent*);
};
