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

#ifndef HISTORY_H
#define HISTORY_H



#include "modelmenu.h"

#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QtCore/QSortFilterProxyModel>
#include "ui_history.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

// QT_BEGIN_NAMESPACE
namespace browser {
    class HistoryItem;
}

#if QT_VERSION == 0x050600
W_REGISTER_ARGTYPE(browser::HistoryItem)
#endif

namespace browser {
    class HistoryItem {
	public:
	    HistoryItem(){}

	    HistoryItem(const QString &u, const QDateTime &d = QDateTime(), const QString &t = QString());

	    inline bool operator ==(const HistoryItem &other) const;
		// history is sorted in reverse
	    inline bool operator <(const HistoryItem &other) const {return _datetime > other._datetime;}

	    QString	_title;
	    QString	_url;
	    QDateTime	_datetime;
    };

    class AutoSaver;
    class HistoryModel;
    class HistoryFilterModel;
    class HistoryTreeModel;

//    W_REGISTER_ARGTYPE(HistoryModel *)
//    W_REGISTER_ARGTYPE(HistoryFilterModel *)
//    W_REGISTER_ARGTYPE(HistoryTreeModel *)
    class HistoryManager : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryManager)
//	W_PROPERTY(int, historyLimit, &HistoryManager::historyLimit, &HistoryManager::setHistoryLimit)
#else

	Q_OBJECT Q_PROPERTY(int historyLimit READ historyLimit WRITE setHistoryLimit)
#endif
	signals:
	    void historyReset()
#if QT_VERSION == 0x050600
	    W_SIGNAL(historyReset)	//
#else
	    ;
#endif
	    void entryAdded(const HistoryItem &it)
#if QT_VERSION == 0x050600
	    W_SIGNAL(entryAdded, (const HistoryItem &), it)	//
#else
	    ;
#endif
	    void entryRemoved(const HistoryItem &it)
#if QT_VERSION == 0x050600
	    W_SIGNAL(entryRemoved, (const HistoryItem &), it)	//
#else
	    ;
#endif
	    void entryUpdated(int offset)
#if QT_VERSION == 0x050600
	    W_SIGNAL(entryUpdated, (int), offset)	//
#else
	    ;
#endif

	public:
	    HistoryManager(QObject *parent = 0);
	    ~HistoryManager();

	    bool	historyContains(const QString &url) const;
	    void	addHistoryEntry(const QString &url);

	    void updateHistoryItem(const QUrl &url, const QString &title);

	    int		historyLimit() const;
	    void	setHistoryLimit(int limit);

	    QList<HistoryItem>	history() const;
	    void		setHistory(const QList<HistoryItem> &history, bool loadedAndSorted = false);

		// History manager keeps around these models for use by the completer and other classes
	    HistoryModel	*historyModel() const;
	    HistoryFilterModel	*historyFilterModel() const;
	    HistoryTreeModel	*historyTreeModel() const;

	public slots:
	    void	clear();// W_SLOT(clear);
	    void	loadSettings();	// W_SLOT(loadSettings)

	private slots:
	    void	save();	// W_SLOT(save);
	    void	checkForExpired();	// W_SLOT(checkForExpired);

	protected:
	    void addHistoryItem(const HistoryItem &item);

	private:
	    void load();

	    AutoSaver		*_savetimer;
	    int			_historylimit;
	    QTimer		_expiredtimer;
	    QList<HistoryItem>	_history;
	    QString		_lastsavedurl;

	    HistoryModel	*_historymodel;
	    HistoryFilterModel	*_historyfiltermodel;
	    HistoryTreeModel	*_historytreemodel;
#if QT_VERSION == 0x050600
	    W_PROPERTY(int, historyLimit, &HistoryManager::historyLimit, &HistoryManager::setHistoryLimit)
#endif
    };
//    W_REGISTER_ARGTYPE(HistoryManager *)
    class HistoryModel : public QAbstractTableModel {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryModel)
#else
	Q_OBJECT
#endif

	public slots:
	    void	historyReset();
	    void	entryAdded();
	    void	entryUpdated(int offset);

	public:
	    enum Roles {
		DateRole	= Qt::UserRole + 1
		, DateTimeRole	= Qt::UserRole + 2
		, UrlRole	= Qt::UserRole + 3
		, UrlStringRole = Qt::UserRole + 4
	    };

	    HistoryModel(HistoryManager *history, QObject *parent = 0);
	    QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	    int		columnCount(const QModelIndex &parent = QModelIndex()) const;
	    int		rowCount(const QModelIndex &parent = QModelIndex()) const;
	    bool	removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

	private:
	    HistoryManager *_history;
    };

	/*!
	    Proxy model that will remove any duplicate entries.
	    Both _sourcerow and _historyhash store their offsets not from
	    the front of the list, but as offsets from the back.
	  */
    class HistoryFilterModel : public QAbstractProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryFilterModel)
#else
	Q_OBJECT
#endif

	public:
	    HistoryFilterModel(QAbstractItemModel *sourceModel, QObject *parent = 0);

	    inline bool historyContains(const QString &url) const {load();return _historyhash.contains(url);}

	    int	historyLocation(const QString &url) const;

	    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	    void	setSourceModel(QAbstractItemModel *sourceModel);
	    QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	    int		rowCount(const QModelIndex &parent = QModelIndex()) const;
	    int		columnCount(const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex index(int, int, const QModelIndex & = QModelIndex()) const;
	    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
	    bool	removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	private slots:
	    void	sourceReset();
	    void	sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	    void	sourceRowsInserted(const QModelIndex &parent, int start, int end);
	    void	sourceRowsRemoved(const QModelIndex &, int, int);

	private:
	    void load() const;

	    mutable QList<int>		_sourcerow;
	    mutable QHash<QString, int> _historyhash;
	    mutable bool		_loaded;
    };

	/*
	    The history menu
	    - Removes the first twenty entries and puts them as children of the top level.
	    - If there are less then twenty entries then the first folder is also removed.

	    The mapping is done by knowing that HistoryTreeModel is over a table
	    We store that row offset in our index's private data.
	*/
    class HistoryMenuModel : public QAbstractProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryMenuModel)
#else
	Q_OBJECT
#endif

	public:
	    HistoryMenuModel(HistoryTreeModel *sourceModel, QObject *parent = 0);
	    int		columnCount(const QModelIndex &parent) const;
	    int		rowCount(const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

	    int bumpedRows() const;

	private:
	    HistoryTreeModel *_treemodel;
    };

	// Menu that is dynamically populated from the history
    class HistoryMenu : public ModelMenu {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryMenu)
#else
	Q_OBJECT
#endif

	signals:
	    void openUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(openUrl, (const QUrl &), url)	//
#else
	    ;
#endif

	public:
	    HistoryMenu(QWidget *parent = 0);
	    void setInitialActions(QList<QAction *> actions);

	protected:
	    bool	prePopulated();
	    void	postPopulated();

	private slots:
	    void	activated(const QModelIndex &index);
	    void	showHistoryDialog();

	private:
	    HistoryManager	*_history;
	    HistoryMenuModel	*_historymenumodel;
	    QList<QAction *>	_initialactions;
    };

	// proxy model for the history model that
	// exposes each url http://www.foo.com and it url starting at the host www.foo.com
    class HistoryCompletionModel : public QAbstractProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryCompletionModel)
#else
	Q_OBJECT
#endif

	public:
	    HistoryCompletionModel(QObject *parent = 0);
	    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	    int		rowCount(const QModelIndex &parent = QModelIndex()) const;
	    int		columnCount(const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	    QModelIndex index(int, int, const QModelIndex & = QModelIndex()) const;
	    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
	    void	setSourceModel(QAbstractItemModel *sourceModel);

	private slots:
	    void sourceReset();
    };

	// proxy model for the history model that converts the list
	// into a tree, one top level node per day.
	// Used in the HistoryDialog.
    class HistoryTreeModel : public QAbstractProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryTreeModel)
#else
	Q_OBJECT
#endif

	public:
	    HistoryTreeModel(QAbstractItemModel *sourceModel, QObject *parent = 0);
	    QVariant		data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	    int			columnCount(const QModelIndex &parent) const;
	    int			rowCount(const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex		mapFromSource(const QModelIndex &sourceIndex) const;
	    QModelIndex		mapToSource(const QModelIndex &proxyIndex) const;
	    QModelIndex		index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	    QModelIndex		parent(const QModelIndex &index = QModelIndex()) const;
	    bool		hasChildren(const QModelIndex &parent = QModelIndex()) const;
	    Qt::ItemFlags	flags(const QModelIndex &index) const;
	    bool		removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	    QVariant		headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	    void setSourceModel(QAbstractItemModel *sourceModel);

	private slots:
	    void	sourceReset();
	    void	sourceRowsInserted(const QModelIndex &parent, int start, int end);
	    void	sourceRowsRemoved(const QModelIndex &parent, int start, int end);

	private:
	    int			sourceDateRow(int row) const;
	    mutable QList<int>	_sourcerowcache;
    };

	// A modified QSortFilterProxyModel that always accepts the root nodes in the tree
	// so filtering is only done on the children.
	// Used in the HistoryDialog
    class TreeProxyModel : public QSortFilterProxyModel {
#if QT_VERSION == 0x050600
	W_OBJECT(TreeProxyModel)
#else
	Q_OBJECT
#endif

	public:
	    TreeProxyModel(QObject *parent = 0);

	protected:
	    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    };

	// #include "ui_history.h"

    class HistoryDialog : public QDialog, public Ui_HistoryDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(HistoryDialog)
#else
	Q_OBJECT
#endif

	signals:
	    void openUrl(const QUrl &url)
#if QT_VERSION == 0x050600
	    W_SIGNAL(openUrl, (const QUrl &), url)	//
#else
	    ;
#endif

	public:
	    HistoryDialog(QWidget *parent = 0, HistoryManager *history = 0);

	private slots:
	    void	customContextMenuRequested(const QPoint &pos);
	    void	open();
	    void	copy();
    };
}


// QT_END_NAMESPACE

#endif	// HISTORY_H



