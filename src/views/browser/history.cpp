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



#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif




#include "history.h"

#include "autosaver.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>

#include <QtCore/QtAlgorithms>

#include <QtGui/QClipboard>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStyle>

#include <QWebEngineSettings>

#include <QtCore/QDebug>




namespace browser {
//    W_OBJECT_IMPL(HistoryItem)

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryManager)
#endif


    static const unsigned int HISTORY_VERSION = 23;

    HistoryManager::HistoryManager(QObject *parent)
	: QObject(parent)
	  , _savetimer(new AutoSaver(this))
	  , _historylimit(30)
	  , _historymodel(new HistoryModel(this, this))
	  , _historyfiltermodel(new HistoryFilterModel(_historymodel, this))
	  , _historytreemodel(new HistoryTreeModel(_historyfiltermodel, this)){
	_expiredtimer.setSingleShot(true);
	connect(&_expiredtimer, &QTimer::timeout, this, &HistoryManager::checkForExpired);
	connect(this, &HistoryManager::entryAdded, _savetimer, &AutoSaver::changeOccurred);
	connect(this, &HistoryManager::entryRemoved, _savetimer, &AutoSaver::changeOccurred);
	load();

	//        _historymodel = new HistoryModel(this, this);
	//        _historyfiltermodel = new HistoryFilterModel(_historymodel, this);
	//        _historytreemodel = new HistoryTreeModel(_historyfiltermodel, this);
    }

    HistoryManager::~HistoryManager(){
	delete _historytreemodel;
	delete _historyfiltermodel;
	delete _historymodel;

	_savetimer->saveIfNeccessary();
	delete _savetimer;
    }

    QList<HistoryItem> HistoryManager::history() const {
	return _history;
    }

    bool HistoryManager::historyContains(const QString &url) const {
	return _historyfiltermodel->historyContains(url);
    }

    void HistoryManager::addHistoryEntry(const QString &url){
	QUrl cleanUrl(url);
	cleanUrl.setPassword(QString());
	cleanUrl.setHost(cleanUrl.host().toLower());
	HistoryItem item(cleanUrl.toString(), QDateTime::currentDateTime());
	addHistoryItem(item);
    }

    void HistoryManager::setHistory(const QList<HistoryItem> &history, bool loadedAndSorted){
	_history = history;
	// verify that it is sorted by date
	if(! loadedAndSorted)qSort(_history.begin(), _history.end());
	checkForExpired();
	if(loadedAndSorted)_lastsavedurl = _history.value(0)._url;
	else{
	    _lastsavedurl = QString();
	    _savetimer->changeOccurred();
	}
	emit historyReset();
    }

    HistoryModel *HistoryManager::historyModel() const {
	return _historymodel;
    }

    HistoryFilterModel *HistoryManager::historyFilterModel() const {
	return _historyfiltermodel;
    }

    HistoryTreeModel *HistoryManager::historyTreeModel() const {
	return _historytreemodel;
    }

    void HistoryManager::checkForExpired(){
	if(_historylimit < 0 || _history.isEmpty())return;
	QDateTime	now		= QDateTime::currentDateTime();
	int		nextTimeout	= 0;
	while(! _history.isEmpty()){
	    QDateTime checkForExpired = _history.last()._datetime;
	    checkForExpired.setDate(checkForExpired.date().addDays(_historylimit));
	    if(now.daysTo(checkForExpired) > 7){
		// check at most in a week to prevent int overflows on the timer
		nextTimeout = 7 * 86400;
	    }else nextTimeout = now.secsTo(checkForExpired);
	    if(nextTimeout > 0)break;
	    HistoryItem item = _history.takeLast();
		// remove from saved file also
	    _lastsavedurl = QString();
	    emit entryRemoved(item);
	}
	if(nextTimeout > 0)_expiredtimer.start(nextTimeout * 1000);
    }

    void HistoryManager::addHistoryItem(const HistoryItem &item){
	if(sapp_t::instance()->privateBrowsing())return;
	_history.prepend(item);
	emit entryAdded(item);
	if(_history.count() == 1)checkForExpired();
    }

    void HistoryManager::updateHistoryItem(const QUrl &url, const QString &title){
	for(int i = 0; i < _history.count(); ++ i){
	    if(url == _history.at(i)._url){
		_history[i]._title = title;
		_savetimer->changeOccurred();
		if(_lastsavedurl.isEmpty())_lastsavedurl = _history.at(i)._url;
		emit entryUpdated(i);
		break;
	    }
	}
    }

    int HistoryManager::historyLimit() const {
	return _historylimit;
    }

    void HistoryManager::setHistoryLimit(int limit){
	if(_historylimit == limit)return;
	_historylimit = limit;
	checkForExpired();
	_savetimer->changeOccurred();
    }

    void HistoryManager::clear(){
	_history.clear();
	_lastsavedurl = QString();
	_savetimer->changeOccurred();
	_savetimer->saveIfNeccessary();
	historyReset();
    }

    void HistoryManager::loadSettings(){
	// load settings
	QSettings settings(globalparameters.root_path() + "/" + globalparameters.target_os() + "/"+globalparameters._browser_conf_filename, QSettings::IniFormat);
	settings.beginGroup(QLatin1String("history"));
	_historylimit = settings.value(QLatin1String("historyLimit"), 30).toInt();
    }

    void HistoryManager::load(){
	loadSettings();

	QFile historyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation)
	    + QLatin1String("/history"));
	if(! historyFile.exists())return;
	if(! historyFile.open(QFile::ReadOnly)){
	    qWarning() << "Unable to open history file" << historyFile.fileName();

	    return;
	}
	QList<HistoryItem>	list;
	QDataStream		in(&historyFile);
	// Double check that the history file is sorted as it is read in
	bool		needToSort = false;
	HistoryItem	lastInsertedItem;
	QByteArray	data;
	QDataStream	stream;
	QBuffer		buffer;
	stream.setDevice(&buffer);
	while(! historyFile.atEnd()){
	    in >> data;
	    buffer.close();
	    buffer.setBuffer(&data);
	    buffer.open(QIODevice::ReadOnly);
	    quint32 ver;
	    stream >> ver;
	    if(ver != HISTORY_VERSION)continue;
	    HistoryItem item;
	    stream >> item._url;
	    stream >> item._datetime;
	    stream >> item._title;
	    if(! item._datetime.isValid())continue;
	    if(item == lastInsertedItem){
		if(lastInsertedItem._title.isEmpty() && ! list.isEmpty())list[0]._title = item._title;
		continue;
	    }
	    if(! needToSort && ! list.isEmpty() && lastInsertedItem < item)needToSort = true;
	    list.prepend(item);
	    lastInsertedItem = item;
	}
	if(needToSort)qSort(list.begin(), list.end());
	setHistory(list, true);
	// If we had to sort re-write the whole history sorted
	if(needToSort){
	    _lastsavedurl = QString();
	    _savetimer->changeOccurred();
	}
    }

    void HistoryManager::save(){
	QSettings settings(globalparameters.root_path() + "/" + globalparameters.target_os() + "/"+globalparameters._browser_conf_filename, QSettings::IniFormat);
	settings.beginGroup(QLatin1String("history"));
	settings.setValue(QLatin1String("historyLimit"), _historylimit);

	bool	saveAll = _lastsavedurl.isEmpty();
	int	first	= _history.count() - 1;
	if(! saveAll){
		// find the first one to save
	    for(int i = 0; i < _history.count(); ++ i){
		if(_history.at(i)._url == _lastsavedurl){
		    first = i - 1;
		    break;
		}
	    }
	}
	if(first == _history.count() - 1)saveAll = true;
	QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	if(directory.isEmpty())directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
	if(! QFile::exists(directory)){
	    QDir dir;
	    dir.mkpath(directory);
	}
	QFile historyFile(directory + QLatin1String("/history"));
	// When saving everything use a temporary file to prevent possible data loss.
	QTemporaryFile tempFile;
	tempFile.setAutoRemove(false);
	bool open = false;
	if(saveAll)open = tempFile.open();
	else open = historyFile.open(QFile::Append);
	if(! open){
	    qWarning()	<< "Unable to open history file for saving"
			<< (saveAll ? tempFile.fileName() : historyFile.fileName());

	    return;
	}
	QDataStream out(saveAll ? &tempFile : &historyFile);
	for(int i = first; i >= 0; -- i){
	    QByteArray	data;
	    QDataStream stream(&data, QIODevice::WriteOnly);
	    HistoryItem item = _history.at(i);
	    stream << HISTORY_VERSION << item._url << item._datetime << item._title;
	    out << data;
	}
	tempFile.close();
	if(saveAll){
	    if(historyFile.exists() && ! historyFile.remove())qWarning() << "History: error removing old history." << historyFile.errorString();
	    if(! tempFile.rename(historyFile.fileName()))qWarning() << "History: error moving new history over old." << tempFile.errorString() << historyFile.fileName();
	}
	_lastsavedurl = _history.value(0)._url;
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryModel)
#endif

    HistoryModel::HistoryModel(HistoryManager *history, QObject *parent)
	: QAbstractTableModel(parent)
	  , _history(history){
	Q_ASSERT(_history);
	connect(_history, &HistoryManager::historyReset, this, &HistoryModel::historyReset);
	connect(_history, &HistoryManager::entryRemoved, this, &HistoryModel::historyReset);

	connect(_history, &HistoryManager::entryAdded, this, &HistoryModel::entryAdded);
	connect(_history, &HistoryManager::entryUpdated, this, &HistoryModel::entryUpdated);
    }

    void HistoryModel::historyReset(){
	beginResetModel();
	endResetModel();
    }

    void HistoryModel::entryAdded(){
	beginInsertRows(QModelIndex(), 0, 0);
	endInsertRows();
    }

    void HistoryModel::entryUpdated(int offset){
	QModelIndex	idx = index(offset, 0);
	emit		dataChanged(idx, idx);
    }

    QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(  orientation == Qt::Horizontal
	  && role == Qt::DisplayRole){
	    switch(section){
		case 0: return tr("Title");

		case 1: return tr("Address");
	    }
	}
	return QAbstractTableModel::headerData(section, orientation, role);
    }

    QVariant HistoryModel::data(const QModelIndex &index, int role) const {
	QList<HistoryItem> lst = _history->history();
	if(index.row() < 0 || index.row() >= lst.size())return QVariant();
	const HistoryItem &item = lst.at(index.row());
	switch(role){
	    case DateTimeRole:

		return item._datetime;

	    case DateRole:

		return item._datetime.date();

	    case UrlRole:

		return QUrl(item._url);

	    case UrlStringRole:

		return item._url;

	    case Qt::DisplayRole:
	    case Qt::EditRole: {
		switch(index.column()){
		    case 0:
			// when there is no title try to generate one from the url
			if(item._title.isEmpty()){
			    QString page = QFileInfo(QUrl(item._url).path()).fileName();
			    if(! page.isEmpty())return page;
			    return item._url;
			}
			return item._title;

		    case 1:

			return item._url;
		}
	    }

	    case Qt::DecorationRole:
		if(index.column() == 0)return sapp_t::instance()->icon(item._url);
	}
	return QVariant();
    }

    int HistoryModel::columnCount(const QModelIndex &parent) const {
	return (parent.isValid()) ? 0 : 2;
    }

    int HistoryModel::rowCount(const QModelIndex &parent) const {
	return (parent.isValid()) ? 0 : _history->history().count();
    }

    bool HistoryModel::removeRows(int row, int count, const QModelIndex &parent){
	if(parent.isValid())return false;
	int lastRow = row + count - 1;
	beginRemoveRows(parent, row, lastRow);
	QList<HistoryItem> lst = _history->history();
	for(int i = lastRow; i >= row; -- i)lst.removeAt(i);
	disconnect(_history, &HistoryManager::historyReset, this, &HistoryModel::historyReset);
	_history->setHistory(lst);
	connect(_history, &HistoryManager::historyReset, this, &HistoryModel::historyReset);
	endRemoveRows();

	return true;
    }

#define MOVEDROWS 15


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryMenuModel)
#endif

	/*
	    Maps the first bunch of items of the source model to the root
	*/
    HistoryMenuModel::HistoryMenuModel(HistoryTreeModel *sourceModel, QObject *parent)
	: QAbstractProxyModel(parent)
	  , _treemodel(sourceModel){
	setSourceModel(sourceModel);
    }

    int HistoryMenuModel::bumpedRows() const {
	QModelIndex first = _treemodel->index(0, 0);
	if(! first.isValid())return 0;
	return qMin(_treemodel->rowCount(first), MOVEDROWS);
    }

    int HistoryMenuModel::columnCount(const QModelIndex &parent) const {
	return _treemodel->columnCount(mapToSource(parent));
    }

    int HistoryMenuModel::rowCount(const QModelIndex &parent) const {
	if(parent.column() > 0)return 0;
	if(! parent.isValid()){
	    int folders		= sourceModel()->rowCount();
	    int bumpedItems	= bumpedRows();
	    if(  bumpedItems <= MOVEDROWS
	      && bumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))-- folders;
	    return bumpedItems + folders;
	}
	if(parent.internalId() == quintptr(- 1))
		if(parent.row() < bumpedRows())return 0;
	QModelIndex	idx		= mapToSource(parent);
	int		defaultCount	= sourceModel()->rowCount(idx);
	if(idx == sourceModel()->index(0, 0))return defaultCount - bumpedRows();
	return defaultCount;
    }

    QModelIndex HistoryMenuModel::mapFromSource(const QModelIndex &sourceIndex) const {
	// currently not used or autotested
	Q_ASSERT(false);
	int sr = _treemodel->mapToSource(sourceIndex).row();

	return createIndex(sourceIndex.row(), sourceIndex.column(), sr);
    }

    QModelIndex HistoryMenuModel::mapToSource(const QModelIndex &proxyIndex) const {
	if(! proxyIndex.isValid())return QModelIndex();
	if(proxyIndex.internalId() == quintptr(- 1)){
	    int bumpedItems = bumpedRows();
	    if(proxyIndex.row() < bumpedItems)return _treemodel->index(proxyIndex.row(), proxyIndex.column(), _treemodel->index(0, 0));
	    if(bumpedItems <= MOVEDROWS && bumpedItems == sourceModel()->rowCount(_treemodel->index(0, 0)))-- bumpedItems;
	    return _treemodel->index(proxyIndex.row() - bumpedItems, proxyIndex.column());
	}
	QModelIndex	historyIndex	= _treemodel->sourceModel()->index(proxyIndex.internalId(), proxyIndex.column());
	QModelIndex	treeIndex	= _treemodel->mapFromSource(historyIndex);

	return treeIndex;
    }

    QModelIndex HistoryMenuModel::index(int row, int column, const QModelIndex &parent) const {
	if(  row < 0
	  || column < 0 || column >= columnCount(parent)
	  || parent.column() > 0)return QModelIndex();
	if(! parent.isValid())return createIndex(row, column, quintptr(- 1));
	QModelIndex treeIndexParent = mapToSource(parent);

	int bumpedItems = 0;
	if(treeIndexParent == _treemodel->index(0, 0))bumpedItems = bumpedRows();
	QModelIndex	treeIndex	= _treemodel->index(row + bumpedItems, column, treeIndexParent);
	QModelIndex	historyIndex	= _treemodel->mapToSource(treeIndex);
	int		historyRow	= historyIndex.row();
	if(historyRow == - 1)historyRow = treeIndex.row();
	return createIndex(row, column, historyRow);
    }

    QModelIndex HistoryMenuModel::parent(const QModelIndex &index) const {
	int offset = index.internalId();
	if(offset == - 1 || ! index.isValid())return QModelIndex();
	QModelIndex	historyIndex	= _treemodel->sourceModel()->index(index.internalId(), 0);
	QModelIndex	treeIndex	= _treemodel->mapFromSource(historyIndex);
	QModelIndex	treeIndexParent = treeIndex.parent();

	int	sr		= _treemodel->mapToSource(treeIndexParent).row();
	int	bumpedItems	= bumpedRows();
	if(bumpedItems <= MOVEDROWS && bumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))-- bumpedItems;
	return createIndex(bumpedItems + treeIndexParent.row(), treeIndexParent.column(), sr);
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryMenu)
#endif


    HistoryMenu::HistoryMenu(QWidget *parent)
	: ModelMenu(parent)
	  , _history(0){
	connect(static_cast<ModelMenu *const>(this), &ModelMenu::activated, this, &HistoryMenu::activated);
	setHoverRole(HistoryModel::UrlStringRole);
    }

    void HistoryMenu::activated(const QModelIndex &index){
	emit openUrl(index.data(HistoryModel::UrlRole).toUrl());
    }

    bool HistoryMenu::prePopulated(){
	if(! _history){
	    _history		= sapp_t::historyManager();
	    _historymenumodel	= new HistoryMenuModel(_history->historyTreeModel(), this);
	    setModel(_historymenumodel);
	}
	// initial actions
	for(int i = 0; i < _initialactions.count(); ++ i)addAction(_initialactions.at(i));
	if(! _initialactions.isEmpty())addSeparator();
	setFirstSeparator(_historymenumodel->bumpedRows());

	return false;
    }

    void HistoryMenu::postPopulated(){
	if(_history->history().count() > 0)addSeparator();
	QAction *showAllAction = new QAction(tr("Show All History"), this);
	connect(showAllAction, &QAction::triggered, this, &HistoryMenu::showHistoryDialog);
	addAction(showAllAction);

	QAction *clearAction = new QAction(tr("Clear History"), this);
	connect(clearAction, &QAction::triggered, _history, &HistoryManager::clear);
	addAction(clearAction);
    }

    void HistoryMenu::showHistoryDialog(){
	HistoryDialog *dialog = new HistoryDialog(this);
	connect(dialog, &HistoryDialog::openUrl, this, &HistoryMenu::openUrl);
	dialog->show();
    }

    void HistoryMenu::setInitialActions(QList<QAction *> actions){
	_initialactions = actions;
	for(int i = 0; i < _initialactions.count(); ++ i)addAction(_initialactions.at(i));
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(TreeProxyModel)
#endif


    TreeProxyModel::TreeProxyModel(QObject *parent) : QSortFilterProxyModel(parent){
	setSortRole(HistoryModel::DateTimeRole);
	setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    bool TreeProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
	if(! source_parent.isValid())return true;
	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryDialog)
#endif


    HistoryDialog::HistoryDialog(QWidget *parent, HistoryManager *setHistory) : QDialog(parent){
	HistoryManager *history = setHistory;
	if(! history)history = sapp_t::historyManager();
	setupUi(this);
	tree->setUniformRowHeights(true);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setTextElideMode(Qt::ElideMiddle);
	QAbstractItemModel	*model		= history->historyTreeModel();
	TreeProxyModel		*proxyModel	= new TreeProxyModel(this);
	connect(search, &SearchLineEdit::textChanged, proxyModel, &TreeProxyModel::setFilterFixedString);
	connect(removeButton, &QPushButton::clicked, tree, &EditTreeView::removeOne);
	connect(removeAllButton, &QPushButton::clicked, history, &HistoryManager::clear);
	proxyModel->setSourceModel(model);
	tree->setModel(proxyModel);
	tree->setExpanded(proxyModel->index(0, 0), true);
	tree->setAlternatingRowColors(true);
	QFontMetrics	fm(font());
	int		header = fm.width(QLatin1Char('m')) * 40;
	tree->header()->resizeSection(0, header);
	tree->header()->setStretchLastSection(true);
	connect(tree, &EditTreeView::activated, this, &HistoryDialog::open);
	tree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tree, &EditTreeView::customContextMenuRequested, this, &HistoryDialog::customContextMenuRequested);
    }

    void HistoryDialog::customContextMenuRequested(const QPoint &pos){
	QMenu		menu;
	QModelIndex	index = tree->indexAt(pos);
	index = index.sibling(index.row(), 0);
	if(index.isValid() && ! tree->model()->hasChildren(index)){
	    menu.addAction(tr("Open"), this, &HistoryDialog::open);
	    menu.addSeparator();
	    menu.addAction(tr("Copy"), this, &HistoryDialog::copy);
	}
	menu.addAction(tr("Delete"), tree, &browser::EditTreeView::removeOne);
	menu.exec(QCursor::pos());
    }

    void HistoryDialog::open(){
	QModelIndex index = tree->currentIndex();
	if(! index.parent().isValid())return;
	emit openUrl(index.data(HistoryModel::UrlRole).toUrl());
    }

    void HistoryDialog::copy(){
	QModelIndex index = tree->currentIndex();
	if(! index.parent().isValid())return;
	QString url = index.data(HistoryModel::UrlStringRole).toString();

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(url);
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryFilterModel)
#endif


    HistoryFilterModel::HistoryFilterModel(QAbstractItemModel *sourceModel, QObject *parent)
	: QAbstractProxyModel(parent)
	  , _loaded(false){
	setSourceModel(sourceModel);
    }

    int HistoryFilterModel::historyLocation(const QString &url) const {
	load();
	if(! _historyhash.contains(url))return 0;
	return sourceModel()->rowCount() - _historyhash.value(url);
    }

    QVariant HistoryFilterModel::data(const QModelIndex &index, int role) const {
	return QAbstractProxyModel::data(index, role);
    }

    void HistoryFilterModel::setSourceModel(QAbstractItemModel *newSourceModel){
	if(sourceModel()){
	    disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryFilterModel::sourceReset);
	    disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryFilterModel::dataChanged);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryFilterModel::sourceRowsInserted);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	}
	QAbstractProxyModel::setSourceModel(newSourceModel);
	if(sourceModel()){
	    _loaded = false;
	    connect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryFilterModel::sourceReset);
	    connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &HistoryFilterModel::sourceDataChanged);
	    connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryFilterModel::sourceRowsInserted);
	    connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	}
    }

    void HistoryFilterModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight){
	emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
    }

    QVariant HistoryFilterModel::headerData(int section, Qt::Orientation orientation, int role) const {
	return sourceModel()->headerData(section, orientation, role);
    }

    void HistoryFilterModel::sourceReset(){
	_loaded = false;
	beginResetModel();
	endResetModel();
    }

    int HistoryFilterModel::rowCount(const QModelIndex &parent) const {
	load();
	if(parent.isValid())return 0;
	return _historyhash.count();
    }

    int HistoryFilterModel::columnCount(const QModelIndex &parent) const {
	return (parent.isValid()) ? 0 : 2;
    }

    QModelIndex HistoryFilterModel::mapToSource(const QModelIndex &proxyIndex) const {
	load();
	int sourceRow = sourceModel()->rowCount() - proxyIndex.internalId();

	return sourceModel()->index(sourceRow, proxyIndex.column());
    }

    QModelIndex HistoryFilterModel::mapFromSource(const QModelIndex &sourceIndex) const {
	load();
	QString url = sourceIndex.data(HistoryModel::UrlStringRole).toString();
	if(! _historyhash.contains(url))return QModelIndex();
	// This can be done in a binary search, but we can't use qBinary find
	// because it can't take: qBinaryFind(_sourcerow.end(), _sourcerow.begin(), v);
	// so if this is a performance bottlneck then convert to binary search, until then
	// the cleaner/easier to read code wins the day.
	int	realRow		= - 1;
	int	sourceModelRow	= sourceModel()->rowCount() - sourceIndex.row();
	for(int i = 0; i < _sourcerow.count(); ++ i){
	    if(_sourcerow.at(i) == sourceModelRow){
		realRow = i;
		break;
	    }
	}
	if(realRow == - 1)return QModelIndex();
	return createIndex(realRow, sourceIndex.column(), sourceModel()->rowCount() - sourceIndex.row());
    }

    QModelIndex HistoryFilterModel::index(int row, int column, const QModelIndex &parent) const {
	load();
	if(  row < 0 || row >= rowCount(parent)
	  || column < 0 || column >= columnCount(parent))return QModelIndex();
	return createIndex(row, column, _sourcerow[row]);
    }

    QModelIndex HistoryFilterModel::parent(const QModelIndex &) const {
	return QModelIndex();
    }

    void HistoryFilterModel::load() const {
	if(_loaded)return;
	_sourcerow.clear();
	_historyhash.clear();
	_historyhash.reserve(sourceModel()->rowCount());
	for(int i = 0; i < sourceModel()->rowCount(); ++ i){
	    QModelIndex idx	= sourceModel()->index(i, 0);
	    QString	url	= idx.data(HistoryModel::UrlStringRole).toString();
	    if(! _historyhash.contains(url)){
		_sourcerow.append(sourceModel()->rowCount() - i);
		_historyhash[url] = sourceModel()->rowCount() - i;
	    }
	}
	_loaded = true;
    }

    void HistoryFilterModel::sourceRowsInserted(const QModelIndex &parent, int start, int end){
	Q_ASSERT(start == end && start == 0);
	Q_UNUSED(end);
	if(! _loaded)return;
	QModelIndex	idx	= sourceModel()->index(start, 0, parent);
	QString		url	= idx.data(HistoryModel::UrlStringRole).toString();
	if(_historyhash.contains(url)){
	    int sourceRow	= sourceModel()->rowCount() - _historyhash[url];
	    int realRow		= mapFromSource(sourceModel()->index(sourceRow, 0)).row();
	    beginRemoveRows(QModelIndex(), realRow, realRow);
	    _sourcerow.removeAt(realRow);
	    _historyhash.remove(url);
	    endRemoveRows();
	}
	beginInsertRows(QModelIndex(), 0, 0);
	_historyhash.insert(url, sourceModel()->rowCount() - start);
	_sourcerow.insert(0, sourceModel()->rowCount());
	endInsertRows();
    }

    void HistoryFilterModel::sourceRowsRemoved(const QModelIndex &, int start, int end){
	Q_UNUSED(start);
	Q_UNUSED(end);
	sourceReset();
    }

	/*
	    Removing a continuous block of rows will remove filtered rows too as this is
	    the users intention.
	*/
    bool HistoryFilterModel::removeRows(int row, int count, const QModelIndex &parent){
	if(row < 0 || count <= 0 || row + count > rowCount(parent) || parent.isValid())return false;
	int lastRow = row + count - 1;
	disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	beginRemoveRows(parent, row, lastRow);
	int	oldCount	= rowCount();
	int	start		= sourceModel()->rowCount() - _sourcerow.value(row);
	int	end		= sourceModel()->rowCount() - _sourcerow.value(lastRow);
	sourceModel()->removeRows(start, end - start + 1);
	endRemoveRows();
	connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryFilterModel::sourceRowsRemoved);
	_loaded = false;
	if(oldCount - count != rowCount()){
	    beginResetModel();
	    endResetModel();
	}
	return true;
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryCompletionModel)
#endif

    HistoryCompletionModel::HistoryCompletionModel(QObject *parent)
	: QAbstractProxyModel(parent)
    {}

    QVariant HistoryCompletionModel::data(const QModelIndex &index, int role) const {
	if(  sourceModel()
	  && (role == Qt::EditRole || role == Qt::DisplayRole)
	  && index.isValid()){
	    QModelIndex idx = mapToSource(index);
	    idx = idx.sibling(idx.row(), 1);
	    QString urlString = idx.data(HistoryModel::UrlStringRole).toString();
	    if(index.row() % 2){
		QUrl	url	= urlString;
		QString s	= url.toString(QUrl::RemoveScheme
			| QUrl::RemoveUserInfo
			| QUrl::StripTrailingSlash);

		return s.mid(2);	// strip // from the front
	    }
	    return urlString;
	}
	return QAbstractProxyModel::data(index, role);
    }

    int HistoryCompletionModel::rowCount(const QModelIndex &parent) const {
	return (parent.isValid() || ! sourceModel()) ? 0 : sourceModel()->rowCount(parent) * 2;
    }

    int HistoryCompletionModel::columnCount(const QModelIndex &parent) const {
	return (parent.isValid()) ? 0 : 1;
    }

    QModelIndex HistoryCompletionModel::mapFromSource(const QModelIndex &sourceIndex) const {
	int row = sourceIndex.row() * 2;

	return index(row, sourceIndex.column());
    }

    QModelIndex HistoryCompletionModel::mapToSource(const QModelIndex &proxyIndex) const {
	if(! sourceModel())return QModelIndex();
	int row = proxyIndex.row() / 2;

	return sourceModel()->index(row, proxyIndex.column());
    }

    QModelIndex HistoryCompletionModel::index(int row, int column, const QModelIndex &parent) const {
	if(  row < 0 || row >= rowCount(parent)
	  || column < 0 || column >= columnCount(parent))return QModelIndex();
	return createIndex(row, column);
    }

    QModelIndex HistoryCompletionModel::parent(const QModelIndex &) const {
	return QModelIndex();
    }

    void HistoryCompletionModel::setSourceModel(QAbstractItemModel *newSourceModel){
	if(sourceModel()){
	    disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryCompletionModel::sourceReset);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryCompletionModel::sourceReset);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryCompletionModel::sourceReset);
	}
	QAbstractProxyModel::setSourceModel(newSourceModel);
	if(newSourceModel){
	    connect(newSourceModel, &QAbstractItemModel::modelReset, this, &HistoryCompletionModel::sourceReset);
	    connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryCompletionModel::sourceReset);
	    connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryCompletionModel::sourceReset);
	}
	beginResetModel();
	endResetModel();
    }

    void HistoryCompletionModel::sourceReset(){
	beginResetModel();
	endResetModel();
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(HistoryTreeModel)
#endif


    HistoryTreeModel::HistoryTreeModel(QAbstractItemModel *sourceModel, QObject *parent)
	: QAbstractProxyModel(parent){
	setSourceModel(sourceModel);
    }

    QVariant HistoryTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
	return sourceModel()->headerData(section, orientation, role);
    }

    QVariant HistoryTreeModel::data(const QModelIndex &index, int role) const {
	if((role == Qt::EditRole || role == Qt::DisplayRole)){
	    int start = index.internalId();
	    if(start == 0){
		int offset = sourceDateRow(index.row());
		if(index.column() == 0){
		    QModelIndex idx	= sourceModel()->index(offset, 0);
		    QDate	date	= idx.data(HistoryModel::DateRole).toDate();
		    if(date == QDate::currentDate())return tr("Earlier Today");
		    return date.toString(QLatin1String("dddd, MMMM d, yyyy"));
		}
		if(index.column() == 1)return tr("%1 items").arg(rowCount(index.sibling(index.row(), 0)));
	    }
	}
	if(role == Qt::DecorationRole && index.column() == 0 && ! index.parent().isValid())return QIcon(QLatin1String(":history.png"));
	if(role == HistoryModel::DateRole && index.column() == 0 && index.internalId() == 0){
	    int		offset	= sourceDateRow(index.row());
	    QModelIndex idx	= sourceModel()->index(offset, 0);

	    return idx.data(HistoryModel::DateRole);
	}
	return QAbstractProxyModel::data(index, role);
    }

    int HistoryTreeModel::columnCount(const QModelIndex &parent) const {
	return sourceModel()->columnCount(mapToSource(parent));
    }

    int HistoryTreeModel::rowCount(const QModelIndex &parent) const {
	if(  parent.internalId() != 0
	  || parent.column() > 0
	  || ! sourceModel())return 0;
	// row count OF dates
	if(! parent.isValid()){
	    if(! _sourcerowcache.isEmpty())return _sourcerowcache.count();
	    QDate	currentDate;
	    int		rows		= 0;
	    int		totalRows	= sourceModel()->rowCount();
	    for(int i = 0; i < totalRows; ++ i){
		QDate rowDate = sourceModel()->index(i, 0).data(HistoryModel::DateRole).toDate();
		if(rowDate != currentDate){
		    _sourcerowcache.append(i);
		    currentDate = rowDate;
		    ++ rows;
		}
	    }
	    Q_ASSERT(_sourcerowcache.count() == rows);

	    return rows;
	}
	// row count FOR a date
	int	start	= sourceDateRow(parent.row());
	int	end	= sourceDateRow(parent.row() + 1);

	return end - start;
    }

	// Translate the top level date row into the offset where that date starts
    int HistoryTreeModel::sourceDateRow(int row) const {
	if(row <= 0)return 0;
	if(_sourcerowcache.isEmpty())rowCount(QModelIndex());
	if(row >= _sourcerowcache.count()){
	    if(! sourceModel())return 0;
	    return sourceModel()->rowCount();
	}
	return _sourcerowcache.at(row);
    }

    QModelIndex HistoryTreeModel::mapToSource(const QModelIndex &proxyIndex) const {
	int offset = proxyIndex.internalId();
	if(offset == 0)return QModelIndex();
	int startDateRow = sourceDateRow(offset - 1);

	return sourceModel()->index(startDateRow + proxyIndex.row(), proxyIndex.column());
    }

    QModelIndex HistoryTreeModel::index(int row, int column, const QModelIndex &parent) const {
	if(  row < 0
	  || column < 0 || column >= columnCount(parent)
	  || parent.column() > 0)return QModelIndex();
	if(! parent.isValid())return createIndex(row, column);
	return createIndex(row, column, parent.row() + 1);
    }

    QModelIndex HistoryTreeModel::parent(const QModelIndex &index) const {
	int offset = index.internalId();
	if(offset == 0 || ! index.isValid())return QModelIndex();
	return createIndex(offset - 1, 0);
    }

    bool HistoryTreeModel::hasChildren(const QModelIndex &parent) const {
	QModelIndex grandparent = parent.parent();
	if(! grandparent.isValid())return true;
	return false;
    }

    Qt::ItemFlags HistoryTreeModel::flags(const QModelIndex &index) const {
	if(! index.isValid())return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
    }

    bool HistoryTreeModel::removeRows(int row, int count, const QModelIndex &parent){
	if(row < 0 || count <= 0 || row + count > rowCount(parent))return false;
	if(parent.isValid()){
		// removing pages
	    int offset = sourceDateRow(parent.row());

	    return sourceModel()->removeRows(offset + row, count);
	}else{
		// removing whole dates
	    for(int i = row + count - 1; i >= row; -- i){
		QModelIndex	dateParent	= index(i, 0);
		int		offset		= sourceDateRow(dateParent.row());
		if(! sourceModel()->removeRows(offset, rowCount(dateParent)))return false;
	    }
	}
	return true;
    }

    void HistoryTreeModel::setSourceModel(QAbstractItemModel *newSourceModel){
	if(sourceModel()){
	    disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryTreeModel::sourceReset);
	    disconnect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &HistoryTreeModel::sourceReset);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryTreeModel::sourceRowsInserted);
	    disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryTreeModel::sourceRowsRemoved);
	}
	QAbstractProxyModel::setSourceModel(newSourceModel);
	if(newSourceModel){
	    connect(sourceModel(), &QAbstractItemModel::modelReset, this, &HistoryTreeModel::sourceReset);
	    connect(sourceModel(), &QAbstractItemModel::layoutChanged, this, &HistoryTreeModel::sourceReset);
	    connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &HistoryTreeModel::sourceRowsInserted);
	    connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &HistoryTreeModel::sourceRowsRemoved);
	}
	beginResetModel();
	endResetModel();
    }

    void HistoryTreeModel::sourceReset(){
	beginResetModel();
	_sourcerowcache.clear();
	endResetModel();
    }

    void HistoryTreeModel::sourceRowsInserted(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent);	// Avoid warnings when compiling release
	Q_ASSERT(! parent.isValid());
	if(start != 0 || start != end){
	    beginResetModel();
	    _sourcerowcache.clear();
	    endResetModel();

	    return;
	}
	_sourcerowcache.clear();
	QModelIndex	treeIndex	= mapFromSource(sourceModel()->index(start, 0));
	QModelIndex	treeParent	= treeIndex.parent();
	if(rowCount(treeParent) == 1){
	    beginInsertRows(QModelIndex(), 0, 0);
	    endInsertRows();
	}else{
	    beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
	    endInsertRows();
	}
    }

    QModelIndex HistoryTreeModel::mapFromSource(const QModelIndex &sourceIndex) const {
	if(! sourceIndex.isValid())return QModelIndex();
	if(_sourcerowcache.isEmpty())rowCount(QModelIndex());
	QList<int>::iterator it;
	it = qLowerBound(_sourcerowcache.begin(), _sourcerowcache.end(), sourceIndex.row());
	if(*it != sourceIndex.row())-- it;
	int	dateRow = qMax(0, it - _sourcerowcache.begin());
	int	row	= sourceIndex.row() - _sourcerowcache.at(dateRow);

	return createIndex(row, sourceIndex.column(), dateRow + 1);
    }

    void HistoryTreeModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent);	// Avoid warnings when compiling release
	Q_ASSERT(! parent.isValid());
	if(_sourcerowcache.isEmpty())return;
	for(int i = end; i >= start; ){
	    QList<int>::iterator it;
	    it = qLowerBound(_sourcerowcache.begin(), _sourcerowcache.end(), i);
		// playing it safe
	    if(it == _sourcerowcache.end()){
		beginResetModel();
		_sourcerowcache.clear();
		endResetModel();

		return;
	    }
	    if(*it != i)-- it;
	    int		row		= qMax(0, it - _sourcerowcache.begin());
	    int		offset		= _sourcerowcache[row];
	    QModelIndex dateParent	= index(row, 0);
		// If we can remove all the rows in the date do that and skip over them
	    int rc = rowCount(dateParent);
	    if(i - rc + 1 == offset && start <= i - rc + 1){
		beginRemoveRows(QModelIndex(), row, row);
		_sourcerowcache.removeAt(row);
		i -= rc + 1;
	    }else{
		beginRemoveRows(dateParent, i - offset, i - offset);
		++ row;
		-- i;
	    }
	    for(int j = row; j < _sourcerowcache.count(); ++ j)-- _sourcerowcache[j];
	    endRemoveRows();
	}
    }

    HistoryItem::HistoryItem(const QString &u, const QDateTime &d, const QString &t)
	: _title(t), _url(u), _datetime(d){}

    bool HistoryItem::operator ==(const HistoryItem &other) const {
	return other._title == _title
	       && other._url == _url && other._datetime == _datetime;
    }
}



