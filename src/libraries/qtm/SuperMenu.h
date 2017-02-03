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


#include <QMenuBar>
#include <QList>
#include "Application.h"
class QAction;
class QMenu;
class EditingWindow;
class SysTrayIcon;
class QShowEvent;

class SuperMenu : public QMenuBar
{
Q_OBJECT

public:
  SuperMenu( QWidget *parent = 0, SysTrayIcon *sti = 0 );
  void setEditActionsEnabled( bool );
  QAction *getCutAction() { return cutAction; }
  QAction *getCopyAction() { return copyAction; }
  QAction *getPasteAction() { return pasteAction; }
  QAction *getUndoAction() { return undoAction; }
  QAction *getRedoAction() { return redoAction; }
  QAction *getConsoleAction() { return showConsoleAction; }
  QAction *getPreviewAction() { return previewAction; }
  void setConsoleEnabled( bool );
  void setCategoriesEnabled( bool );
  void handleNewWindowAtStartup();

  QMenu *fileMenu, *editMenu, *insMenu, *viewMenu, *categoryMenu, 
        *servicesMenu, *helpMenu;
  QMenu *recentFilesMenu, *pasteAsMenu;

  // File menu actions
  QAction *newEntryAction, *openAction, *openRecent, *saveAction, *saveAsAction,
          *saveAllAction, *exportAction, *uploadAction, *refreshBlogListAction,
          *blogThisAction, *abortAction, *accountsAction, *prefsAction, *saveBlogsAction,
          *qptAction, *clearConsoleAction, *closeAction, *quitAction;

  // Edit menu actions
  QAction *cutAction, *copyAction, *copyULAction, *pasteAction, *tidyPasteAction, *pasteAsAction,
          *undoAction, *redoAction, *findAction, *findAgainAction,
          *boldAction, *italicAction, *underlineAction, *blockquoteAction, *paraAction,
          *ulAction, *olAction;
  
  // Paste As sub-menu actions
  QAction *pasteAsMarkedParasAction, *pasteAsBlockquoteAction,
          *pasteAsMarkdownBlockquoteAction,
          *pasteAsUnorderedListAction, *pasteAsOrderedListAction;

  // Insert menu actions
  QAction *linkAction, *clipLinkAction, *selfLinkAction, *autoLinkAction,
          *imageAction, *clipImageAction, *moreAction;

  // View menu actions
  QAction *highlightingAction, *showConsoleAction, *previewAction,
          *viewBasicsAction, *viewCatsAction, *viewExcerptAction, *viewWPTagsAction,
          *viewTechTagsAction, *viewPingsAction;
#ifndef Q_WS_MAC
  QAction *viewToolbarAction;
#endif

  // Category menu actions
  QAction *refreshCatsAction, *updateCatsAction, *addCatAction;

  // Services menu actions
  QAction *addWPTagAction, *removeWPTagAction, *refreshWPTagsAction,
          *addTechTagAction, *addClipTechTagAction, *removeTechTagAction,
          *addPingAction, *addClipPingAction, *removePingAction;

  // Help menu actions
  QAction *aboutAction, *whatsThisAction;

public slots:
  void setRecentFiles( const QList<Application::recentFile> & );
  void openRecentFile();
  void setMainWindow( EditingWindow * );
  void setConsoleActionTitle( bool );
  void setPreviewActionTitle( bool );
  void setHighlightingChecked( bool );

private:
  QList<QAction *> editingWindowActions;
  // QList<QMenu *> editingMenus;
  QAction *noRecentFilesAction;
  SysTrayIcon *_sti;
  QList<Application::recentFile> recentFiles;
  QAction *recentFileActions[10];
  Application *qtm;
  EditingWindow *currentWindow;

  void updateRecentFileMenu();

private slots:
  void newEntry();
  void choose( QString fname = QString() );
  void quit();
  void about();
  void getPreferences();
  void handleLastWindowClosed();
  //void doPreview( bool );
  void handleConsole( bool );

protected:
  virtual void showEvent( QShowEvent * );
};
