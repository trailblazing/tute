/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QObject>

#include "modelmenu.h"
#include "ui_addbookmarkdialog.h"
#include "ui_bookmarks.h"
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QUndoCommand>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class FlatToolButton;

// QT_BEGIN_NAMESPACE

namespace web {
/*!
            Bookmark manager, owner of the bookmarks, loads, saves and basic
   tasks
 */
class AutoSaver;
class BookmarkNode;
class BookmarksModel;

class BookmarksManager : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(BookmarksManager)
#else
	Q_OBJECT
#endif

	signals:
	void entryAdded(BookmarkNode *item)
#if QT_VERSION == 0x050600
	    W_SIGNAL(entryAdded, (BookmarkNode *), item)  //
#else
	    ;
#endif
	    void entryRemoved(BookmarkNode *parent, int row, BookmarkNode *item)
#if QT_VERSION == 0x050600
	        W_SIGNAL(entryRemoved, (BookmarkNode *, int, BookmarkNode *), parent, row, item)  //
#else
	    ;
#endif

	    void entryChanged(BookmarkNode *item)
#if QT_VERSION == 0x050600
	        W_SIGNAL(entryChanged, (BookmarkNode *), item)  //
#else
	    ;
#endif

	    public : BookmarksManager(QObject *parent = 0);
	~BookmarksManager();

	void addBookmark(BookmarkNode *parent, BookmarkNode *node, int row = -1);
	void removeBookmark(BookmarkNode *node);
	void setTitle(BookmarkNode *node, const QString &newTitle);
	void setUrl(BookmarkNode *node, const QString &newUrl);
	void changeExpanded();

	BookmarkNode *bookmarks();
	BookmarkNode *menu();
	BookmarkNode *toolbar();

	BookmarksModel *bookmarksModel();
	QUndoStack *undoRedoStack() {
		return &_commands;
	};

	public slots:
	void importBookmarks();
	void exportBookmarks();

	private slots:
	void save() const;

	private:
	void load();

	bool _loaded;
	AutoSaver *_savetimer;
	BookmarkNode *_bookmarkrootnode;
	BookmarksModel *_bookmarkmodel;
	QUndoStack _commands;

	friend class RemoveBookmarksCommand;
	friend class ChangeBookmarkCommand;
};

class RemoveBookmarksCommand : public QUndoCommand {
	public:
	RemoveBookmarksCommand(BookmarksManager *_bookmarkmanagaer, BookmarkNode *parent, int row);
	~RemoveBookmarksCommand();
	void undo();
	void redo();

	protected:
	int _row;
	BookmarksManager *_bookmarkmanagaer;
	BookmarkNode *_node;
	BookmarkNode *_parent;
	bool _done;
};

class InsertBookmarksCommand : public RemoveBookmarksCommand {
	public:
	InsertBookmarksCommand(BookmarksManager *_bookmarkmanagaer, BookmarkNode *parent, BookmarkNode *node, int row);
	void undo() {
		RemoveBookmarksCommand::redo();
	}

	void redo() {
		RemoveBookmarksCommand::undo();
	}
};

class ChangeBookmarkCommand : public QUndoCommand {
	public:
	ChangeBookmarkCommand(BookmarksManager *_bookmarkmanagaer, BookmarkNode *node, const QString &newValue, bool title);
	void undo();
	void redo();

	private:
	BookmarksManager *_bookmarkmanagaer;
	bool _title;
	QString _oldvalue;
	QString _newvalue;
	BookmarkNode *_node;
};

/*!
            BookmarksModel is a QAbstractItemModel wrapper around the
   BookmarkManager
 */
class BookmarksModel : public QAbstractItemModel {
#if QT_VERSION == 0x050600
	W_OBJECT(BookmarksModel)
#else
	Q_OBJECT
#endif

	public slots:
	void entryAdded(BookmarkNode *item);
	void entryRemoved(BookmarkNode *parent, int row, BookmarkNode *item);
	void entryChanged(BookmarkNode *item);

	public:
	enum Roles {
		TypeRole = Qt::UserRole + 1,
		UrlRole = Qt::UserRole + 2,
		UrlStringRole = Qt::UserRole + 3,
		SeparatorRole = Qt::UserRole + 4
	};

	BookmarksModel(BookmarksManager *bookmarkManager, QObject *parent = 0);
	inline BookmarksManager *bookmarksManager() const {
		return _bookmarksmanager;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QModelIndex index(int, int, const QModelIndex & = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	QStringList mimeTypes() const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

	BookmarkNode *node(const QModelIndex &index) const;
	QModelIndex index(BookmarkNode *node) const;

	private:
	bool _endmacro;
	BookmarksManager *_bookmarksmanager;
};

// Menu that is dynamically populated from the bookmarks
// #include "modelmenu.h"
class BookmarksMenu : public ModelMenu {
#if QT_VERSION == 0x050600
	W_OBJECT(BookmarksMenu)
#else
	Q_OBJECT
#endif

	signals:
	void openUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(openUrl, (const QUrl &), url)  //
#else
	    ;
#endif

	    public : BookmarksMenu(QWidget *parent = 0);
	void setInitialActions(QList<QAction *> actions);

	protected:
	bool prePopulated();

	private slots:
	void activated(const QModelIndex &index);

	private:
	BookmarksManager *_bookmarksmanager;
	QList<QAction *> _initialactions;
};

/*
            Proxy model that filters out the bookmarks so only the folders
            are left behind.  Used in the add bookmark dialog combobox.
 */
// #include <QtCore/QSortFilterProxyModel>
class AddBookmarkProxyModel : public QSortFilterProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(AddBookmarkProxyModel)
#else
	Q_OBJECT
#endif
	public:
	AddBookmarkProxyModel(QObject *parent = 0);
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

/*!
            Add bookmark dialog
 */
// #include "ui_addbookmarkdialog.h"
class AddBookmarkDialog : public QDialog, public Ui_AddBookmarkDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(AddBookmarkDialog)
#else
	Q_OBJECT
#endif
	public:
	AddBookmarkDialog(const QString &url, const QString &title, QWidget *parent = 0, BookmarksManager *bookmarkManager = 0);

	private slots:
	void accept();

	private:
	QString _url;
	BookmarksManager *_bookmarksmanager;
	AddBookmarkProxyModel *_proxymodel;
};

// #include "ui_bookmarks.h"
class TreeProxyModel;
class BookmarksDialog : public QDialog, public Ui_BookmarksDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(BookmarksDialog)
#else
	Q_OBJECT
#endif
	signals:
	void openUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(openUrl, (const QUrl &), url)  //
#else
	    ;
#endif

	    public
	    : BookmarksDialog(QWidget *parent = 0, BookmarksManager *manager = 0);
	~BookmarksDialog();

	private slots:
	void customContextMenuRequested(const QPoint &pos);
	void open();
	void newFolder();

	private:
	void expandNodes(BookmarkNode *node);
	bool saveExpandedNodes(const QModelIndex &parent);

	BookmarksManager *_bookmarksmanager;
	BookmarksModel *_bookmarksmodel;
	TreeProxyModel *_proxymodel;
};

// #include <QtWidgets/QToolBar>
class BookmarksToolBar : public QToolBar {
#if QT_VERSION == 0x050600
	W_OBJECT(BookmarksToolBar)
#else
	Q_OBJECT
#endif
	signals:
	void openUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(openUrl, (const QUrl &), url)  //
#else
	    ;
#endif

	    public : BookmarksToolBar(BookmarksModel *model, QWidget *parent = 0);
	void setRootIndex(const QModelIndex &index);
	QModelIndex rootIndex() const;

	protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	private slots:
	void triggered(QAction *action);
	void activated(const QModelIndex &index);
	void build();

	private:
	BookmarksModel *_bookmarksmodel;
	QPersistentModelIndex _root;
};
}

// QT_END_NAMESPACE

#endif  // BOOKMARKS_H
