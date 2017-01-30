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


#include "bookmarks.h"

#include "autosaver.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "history.h"
#include "xbel.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QMimeData>

#include <QtGui/QDesktopServices>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QIcon>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>

#include "libraries/flat_control.h"

#include <QtCore/QDebug>

#define BOOKMARKBAR	"Bookmarks Bar"
#define BOOKMARKMENU	"Bookmarks Menu"


namespace browser {

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(BookmarksManager)
#endif


    BookmarksManager::BookmarksManager(QObject *parent)
	: QObject(parent)
	  , _loaded(false)
	  , _savetimer(new AutoSaver(this))
	  , _bookmarkrootnode(0)
	  , _bookmarkmodel(0){
	connect(this, &BookmarksManager::entryAdded, _savetimer, &AutoSaver::changeOccurred);
	connect(this, &BookmarksManager::entryRemoved, _savetimer, &AutoSaver::changeOccurred);
	connect(this, &BookmarksManager::entryChanged, _savetimer, &AutoSaver::changeOccurred);
    }

    BookmarksManager::~BookmarksManager(){
	_savetimer->saveIfNeccessary();
    }

    void BookmarksManager::changeExpanded(){
	_savetimer->changeOccurred();
    }

    void BookmarksManager::load(){
	if(_loaded)return;
	_loaded = true;

	QString dir		= QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString bookmarkFile	= dir + QLatin1String("/bookmarks.xbel");
	if(! QFile::exists(bookmarkFile))bookmarkFile = QLatin1String(":defaultbookmarks.xbel");
	XbelReader reader;
	_bookmarkrootnode = reader.read(bookmarkFile);
	if(reader.error() != QXmlStreamReader::NoError){
	    QMessageBox::warning(0, QLatin1String("Loading Bookmark")
				, tr("Error when loading bookmarks on line %1, column %2:\n"
				     "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
	}
	BookmarkNode		*toolbar	= 0;
	BookmarkNode		*menu		= 0;
	QList<BookmarkNode *>	others;
	for(int i = _bookmarkrootnode->children().count() - 1; i >= 0; -- i){
	    BookmarkNode *node = _bookmarkrootnode->children().at(i);
	    if(node->type() == BookmarkNode::Folder){
		// Automatically convert
		if(node->title == tr("Toolbar Bookmarks") && ! toolbar)node->title = tr(BOOKMARKBAR);
		if(node->title == tr(BOOKMARKBAR) && ! toolbar)toolbar = node;
		// Automatically convert
		if(node->title == tr("Menu") && ! menu)node->title = tr(BOOKMARKMENU);
		if(node->title == tr(BOOKMARKMENU) && ! menu)menu = node;
	    }else others.append(node);
	    _bookmarkrootnode->remove(node);
	}
	Q_ASSERT(_bookmarkrootnode->children().count() == 0);
	if(! toolbar){
	    toolbar		= new BookmarkNode(BookmarkNode::Folder, _bookmarkrootnode);
	    toolbar->title	= tr(BOOKMARKBAR);
	}else _bookmarkrootnode->add(toolbar);
	if(! menu){
	    menu	= new BookmarkNode(BookmarkNode::Folder, _bookmarkrootnode);
	    menu->title = tr(BOOKMARKMENU);
	}else _bookmarkrootnode->add(menu);
	for(int i = 0; i < others.count(); ++ i)menu->add(others.at(i));
    }

    void BookmarksManager::save() const {
	if(! _loaded)return;
	XbelWriter	writer;
	QString		dir		= QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString		bookmarkFile	= dir + QLatin1String("/bookmarks.xbel");
	if(! writer.write(bookmarkFile, _bookmarkrootnode))qWarning() << "BookmarkManager: error saving to" << bookmarkFile;
    }

    void BookmarksManager::addBookmark(BookmarkNode *parent, BookmarkNode *node, int row){
	if(! _loaded)return;
	Q_ASSERT(parent);
	InsertBookmarksCommand *command = new InsertBookmarksCommand(this, parent, node, row);
	_commands.push(command);
    }

    void BookmarksManager::removeBookmark(BookmarkNode *node){
	if(! _loaded)return;
	Q_ASSERT(node);
	BookmarkNode		*parent		= node->parent();
	int			row		= parent->children().indexOf(node);
	RemoveBookmarksCommand	*command	= new RemoveBookmarksCommand(this, parent, row);
	_commands.push(command);
    }

    void BookmarksManager::setTitle(BookmarkNode *node, const QString &newTitle){
	if(! _loaded)return;
	Q_ASSERT(node);
	ChangeBookmarkCommand *command = new ChangeBookmarkCommand(this, node, newTitle, true);
	_commands.push(command);
    }

    void BookmarksManager::setUrl(BookmarkNode *node, const QString &newUrl){
	if(! _loaded)return;
	Q_ASSERT(node);
	ChangeBookmarkCommand *command = new ChangeBookmarkCommand(this, node, newUrl, false);
	_commands.push(command);
    }

    BookmarkNode *BookmarksManager::bookmarks(){
	if(! _loaded)load();
	return _bookmarkrootnode;
    }

    BookmarkNode *BookmarksManager::menu(){
	if(! _loaded)load();
	for(int i = _bookmarkrootnode->children().count() - 1; i >= 0; -- i){
	    BookmarkNode *node = _bookmarkrootnode->children().at(i);
	    if(node->title == tr(BOOKMARKMENU))return node;
	}
	Q_ASSERT(false);

	return 0;
    }

    BookmarkNode *BookmarksManager::toolbar(){
	if(! _loaded)load();
	for(int i = _bookmarkrootnode->children().count() - 1; i >= 0; -- i){
	    BookmarkNode *node = _bookmarkrootnode->children().at(i);
	    if(node->title == tr(BOOKMARKBAR))return node;
	}
	Q_ASSERT(false);

	return 0;
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(BookmarksModel)
#endif


    BookmarksModel * BookmarksManager::bookmarksModel(){
	if(! _bookmarkmodel)_bookmarkmodel = new BookmarksModel(this, this);
	return _bookmarkmodel;
    }
    void BookmarksManager::importBookmarks(){
	QString fileName = QFileDialog::getOpenFileName(0, tr("Open File")
						       , QString()
						       , tr("XBEL (*.xbel *.xml)"));
	if(fileName.isEmpty())return;
	XbelReader	reader;
	BookmarkNode	*importRootNode = reader.read(fileName);
	if(reader.error() != QXmlStreamReader::NoError){
	    QMessageBox::warning(0, QLatin1String("Loading Bookmark")
				, tr("Error when loading bookmarks on line %1, column %2:\n"
				     "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
	}
	importRootNode->setType(BookmarkNode::Folder);
	importRootNode->title = (tr("Imported %1").arg(QDate::currentDate().toString(Qt::SystemLocaleShortDate)));
	addBookmark(menu(), importRootNode);
    }

    void BookmarksManager::exportBookmarks(){
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save File")
						       , tr("%1 Bookmarks.xbel").arg(QCoreApplication::applicationName())
						       , tr("XBEL (*.xbel *.xml)"));
	if(fileName.isEmpty())return;
	XbelWriter writer;
	if(! writer.write(fileName, _bookmarkrootnode))QMessageBox::critical(0, tr("Export error"), tr("error saving bookmarks"));
    }

    RemoveBookmarksCommand::RemoveBookmarksCommand(BookmarksManager *bookmarkManagaer, BookmarkNode *parent, int row)
	: QUndoCommand(BookmarksManager::tr("Remove Bookmark"))
	  , _row(row)
	  , _bookmarkmanagaer(bookmarkManagaer)
	  , _node(parent->children().value(row))
	  , _parent(parent)
	  , _done(false)
    {}

    RemoveBookmarksCommand::~RemoveBookmarksCommand(){
	if(_done && ! _node->parent())delete _node;
    }

    void RemoveBookmarksCommand::undo(){
	_parent->add(_node, _row);
	emit _bookmarkmanagaer->entryAdded(_node);
	_done = false;
    }

    void RemoveBookmarksCommand::redo(){
	_parent->remove(_node);
	emit _bookmarkmanagaer->entryRemoved(_parent, _row, _node);
	_done = true;
    }

    InsertBookmarksCommand::InsertBookmarksCommand(BookmarksManager *bookmarkManagaer, BookmarkNode *parent, BookmarkNode *node, int row)
	: RemoveBookmarksCommand(bookmarkManagaer, parent, row){
	setText(BookmarksManager::tr("Insert Bookmark"));
	_node = node;
    }

    ChangeBookmarkCommand::ChangeBookmarkCommand(BookmarksManager *bookmarkManagaer, BookmarkNode *node, const QString &newValue, bool title)
	: QUndoCommand()
	  , _bookmarkmanagaer(bookmarkManagaer)
	  , _title(title)
	  , _newvalue(newValue)
	  , _node(node){
	if(_title){
	    _oldvalue = _node->title;
	    setText(BookmarksManager::tr("Name Change"));
	}else{
	    _oldvalue = _node->url;
	    setText(BookmarksManager::tr("Address Change"));
	}
    }

    void ChangeBookmarkCommand::undo(){
	if(_title)_node->title = _oldvalue;
	else _node->url = _oldvalue;
	emit _bookmarkmanagaer->entryChanged(_node);
    }

    void ChangeBookmarkCommand::redo(){
	if(_title)_node->title = _newvalue;
	else _node->url = _newvalue;
	emit _bookmarkmanagaer->entryChanged(_node);
    }

    BookmarksModel::BookmarksModel(BookmarksManager *bookmarkManager, QObject *parent)
	: QAbstractItemModel(parent)
	  , _endmacro(false)
	  , _bookmarksmanager(bookmarkManager){
	connect(bookmarkManager, &BookmarksManager::entryAdded, this, &BookmarksModel::entryAdded);
	connect(bookmarkManager, &BookmarksManager::entryRemoved, this, &BookmarksModel::entryRemoved);
	connect(bookmarkManager, &BookmarksManager::entryChanged, this, &BookmarksModel::entryChanged);
    }

    QModelIndex BookmarksModel::index(BookmarkNode *node) const {
	BookmarkNode *parent = node->parent();
	if(! parent)return QModelIndex();
	return createIndex(parent->children().indexOf(node), 0, node);
    }

    void BookmarksModel::entryAdded(BookmarkNode *item){
	Q_ASSERT(item && item->parent());
	int		row	= item->parent()->children().indexOf(item);
	BookmarkNode	*parent = item->parent();
	// item was already added so remove beore beginInsertRows is called
	parent->remove(item);
	beginInsertRows(index(parent), row, row);
	parent->add(item, row);
	endInsertRows();
    }

    void BookmarksModel::entryRemoved(BookmarkNode *parent, int row, BookmarkNode *item){
	// item was already removed, re-add so beginRemoveRows works
	parent->add(item, row);
	beginRemoveRows(index(parent), row, row);
	parent->remove(item);
	endRemoveRows();
    }

    void BookmarksModel::entryChanged(BookmarkNode *item){
	QModelIndex	idx = index(item);
	emit		dataChanged(idx, idx);
    }

    bool BookmarksModel::removeRows(int row, int count, const QModelIndex &parent){
	if(row < 0 || count <= 0 || row + count > rowCount(parent))return false;
	BookmarkNode *bookmarkNode = node(parent);
	for(int i = row + count - 1; i >= row; -- i){
	    BookmarkNode *node = bookmarkNode->children().at(i);
	    if(  node == _bookmarksmanager->menu()
	      || node == _bookmarksmanager->toolbar())continue;
	    _bookmarksmanager->removeBookmark(node);
	}
	if(_endmacro){
	    _bookmarksmanager->undoRedoStack()->endMacro();
	    _endmacro = false;
	}
	return true;
    }

    QVariant BookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
	    switch(section){
		case 0: return tr("Title");

		case 1: return tr("Address");
	    }
	}
	return QAbstractItemModel::headerData(section, orientation, role);
    }

    QVariant BookmarksModel::data(const QModelIndex &index, int role) const {
	if(! index.isValid() || index.model() != this)return QVariant();
	const BookmarkNode *bookmarkNode = node(index);
	switch(role){
	    case Qt::EditRole:
	    case Qt::DisplayRole:
		if(bookmarkNode->type() == BookmarkNode::Separator){
		    switch(index.column()){
			case 0: return QString(50, 0xB7);

			case 1: return QString();
		    }
		}
		switch(index.column()){
		    case 0: return bookmarkNode->title;

		    case 1: return bookmarkNode->url;
		}
		break;

	    case BookmarksModel::UrlRole:

		return QUrl(bookmarkNode->url);
		break;

	    case BookmarksModel::UrlStringRole:

		return bookmarkNode->url;
		break;

	    case BookmarksModel::TypeRole:

		return bookmarkNode->type();
		break;

	    case BookmarksModel::SeparatorRole:

		return bookmarkNode->type() == BookmarkNode::Separator;
		break;

	    case Qt::DecorationRole:
		if(index.column() == 0){
		    if(bookmarkNode->type() == BookmarkNode::Folder) return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
		    return sapp_t::instance()->icon(bookmarkNode->url);
		}
	}
	return QVariant();
    }

    int BookmarksModel::columnCount(const QModelIndex &parent) const {
	return (parent.column() > 0) ? 0 : 2;
    }

    int BookmarksModel::rowCount(const QModelIndex &parent) const {
	if(parent.column() > 0)return 0;
	if(! parent.isValid())return _bookmarksmanager->bookmarks()->children().count();
	const BookmarkNode *item = static_cast<BookmarkNode *>(parent.internalPointer());

	return item->children().count();
    }

    QModelIndex BookmarksModel::index(int row, int column, const QModelIndex &parent) const {
	if(row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))return QModelIndex();
	// get the parent node
	BookmarkNode *parentNode = node(parent);

	return createIndex(row, column, parentNode->children().at(row));
    }

    QModelIndex BookmarksModel::parent(const QModelIndex &index) const {
	if(! index.isValid())return QModelIndex();
	BookmarkNode	*itemNode	= node(index);
	BookmarkNode	*parentNode	= (itemNode ? itemNode->parent() : 0);
	if(! parentNode || parentNode == _bookmarksmanager->bookmarks())return QModelIndex();
	// get the parent's row
	BookmarkNode	*grandParentNode	= parentNode->parent();
	int		parentRow		= grandParentNode->children().indexOf(parentNode);
	Q_ASSERT(parentRow >= 0);

	return createIndex(parentRow, 0, parentNode);
    }

    bool BookmarksModel::hasChildren(const QModelIndex &parent) const {
	if(! parent.isValid())return true;
	const BookmarkNode *parentNode = node(parent);

	return parentNode->type() == BookmarkNode::Folder;
    }

    Qt::ItemFlags BookmarksModel::flags(const QModelIndex &index) const {
	if(! index.isValid())return Qt::NoItemFlags;
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	BookmarkNode *bookmarkNode = node(index);
	if(  bookmarkNode != _bookmarksmanager->menu()
	  && bookmarkNode != _bookmarksmanager->toolbar()){
	    flags |= Qt::ItemIsDragEnabled;
	    if(bookmarkNode->type() != BookmarkNode::Separator)flags |= Qt::ItemIsEditable;
	}
	if(hasChildren(index))flags |= Qt::ItemIsDropEnabled;
	return flags;
    }

    Qt::DropActions BookmarksModel::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
    }

#define MIMETYPE QLatin1String("application/bookmarks.xbel")

    QStringList BookmarksModel::mimeTypes() const {
	QStringList types;
	types << MIMETYPE;

	return types;
    }

    QMimeData *BookmarksModel::mimeData(const QModelIndexList &indexes) const {
	QMimeData	*mimeData = new QMimeData();
	QByteArray	data;
	QDataStream	stream(&data, QIODevice::WriteOnly);

	foreach(QModelIndex index, indexes){
	    if(index.column() != 0 || ! index.isValid())continue;
	    QByteArray	encodedData;
	    QBuffer	buffer(&encodedData);
	    buffer.open(QBuffer::ReadWrite);
	    XbelWriter		writer;
	    const BookmarkNode	*parentNode = node(index);
	    writer.write(&buffer, parentNode);
	    stream << encodedData;
	}

	mimeData->setData(MIMETYPE, data);

	return mimeData;
    }

    bool BookmarksModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent){
	if(action == Qt::IgnoreAction)return true;
	if(  ! data->hasFormat(MIMETYPE)
	  || column > 0)return false;
	QByteArray	ba = data->data(MIMETYPE);
	QDataStream	stream(&ba, QIODevice::ReadOnly);
	if(stream.atEnd())return false;
	QUndoStack *undoStack = _bookmarksmanager->undoRedoStack();
	undoStack->beginMacro(QLatin1String("Move Bookmarks"));
	while(! stream.atEnd()){
	    QByteArray encodedData;
	    stream >> encodedData;
	    QBuffer buffer(&encodedData);
	    buffer.open(QBuffer::ReadOnly);

	    XbelReader			reader;
	    BookmarkNode		*rootNode	= reader.read(&buffer);
	    QList<BookmarkNode *>	children	= rootNode->children();
	    for(int i = 0; i < children.count(); ++ i){
		BookmarkNode *bookmarkNode = children.at(i);
		rootNode->remove(bookmarkNode);
		row = qMax(0, row);
		BookmarkNode *parentNode = node(parent);
		_bookmarksmanager->addBookmark(parentNode, bookmarkNode, row);
		_endmacro = true;
	    }
	    delete rootNode;
	}
	return true;
    }

    bool BookmarksModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if(! index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)return false;
	BookmarkNode *item = node(index);
	switch(role){
	    case Qt::EditRole:
	    case Qt::DisplayRole:
		if(index.column() == 0){
		    _bookmarksmanager->setTitle(item, value.toString());
		    break;
		}
		if(index.column() == 1){
		    _bookmarksmanager->setUrl(item, value.toString());
		    break;
		}
		return false;

	    case BookmarksModel::UrlRole:
		_bookmarksmanager->setUrl(item, value.toUrl().toString());
		break;

	    case BookmarksModel::UrlStringRole:
		_bookmarksmanager->setUrl(item, value.toString());
		break;

	    default:
		break;

		return false;
	}
	return true;
    }

    BookmarkNode *BookmarksModel::node(const QModelIndex &index) const {
	BookmarkNode *itemNode = static_cast<BookmarkNode *>(index.internalPointer());
	if(! itemNode)return _bookmarksmanager->bookmarks();
	return itemNode;
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(AddBookmarkProxyModel)
#endif


    AddBookmarkProxyModel::AddBookmarkProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
    {}

    int AddBookmarkProxyModel::columnCount(const QModelIndex &parent) const {
	return qMin(1, QSortFilterProxyModel::columnCount(parent));
    }

    bool AddBookmarkProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
	QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);

	return sourceModel()->hasChildren(idx);
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(AddBookmarkDialog)
#endif


    AddBookmarkDialog::AddBookmarkDialog(const QString &url, const QString &title, QWidget *parent, BookmarksManager *bookmarkManager)
	: QDialog(parent)
	  , _url(url)
	  , _bookmarksmanager(bookmarkManager){
	setWindowFlags(Qt::Sheet);
	if(! _bookmarksmanager)_bookmarksmanager = sapp_t::bookmarksManager();
	setupUi(this);
	QTreeView *view = new QTreeView(this);
	_proxymodel = new AddBookmarkProxyModel(this);
	BookmarksModel *model = _bookmarksmanager->bookmarksModel();
	_proxymodel->setSourceModel(model);
	view->setModel(_proxymodel);
	view->expandAll();
	view->header()->setStretchLastSection(true);
	view->header()->hide();
	view->setItemsExpandable(false);
	view->setRootIsDecorated(false);
	view->setIndentation(10);
	location->setModel(_proxymodel);
	view->show();
	location->setView(view);
	BookmarkNode	*menu	= _bookmarksmanager->menu();
	QModelIndex	idx	= _proxymodel->mapFromSource(model->index(menu));
	view->setCurrentIndex(idx);
	location->setCurrentIndex(idx.row());
	name->setText(title);
    }

    void AddBookmarkDialog::accept(){
	QModelIndex index = location->view()->currentIndex();
	index = _proxymodel->mapToSource(index);
	if(! index.isValid())index = _bookmarksmanager->bookmarksModel()->index(0, 0);
	BookmarkNode	*parent		= _bookmarksmanager->bookmarksModel()->node(index);
	BookmarkNode	*bookmark	= new BookmarkNode(BookmarkNode::Bookmark);
	bookmark->url	= _url;
	bookmark->title = name->text();
	_bookmarksmanager->addBookmark(parent, bookmark);
	QDialog::accept();
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(BookmarksMenu)
#endif


    BookmarksMenu::BookmarksMenu(QWidget *parent)
	: ModelMenu(parent)
	  , _bookmarksmanager(0){
	connect(this, &BookmarksMenu::activated, this, &BookmarksMenu::activated);
	setMaxRows(- 1);
	setHoverRole(BookmarksModel::UrlStringRole);
	setSeparatorRole(BookmarksModel::SeparatorRole);
    }

    void BookmarksMenu::activated(const QModelIndex &index){
	emit openUrl(index.data(BookmarksModel::UrlRole).toUrl());
    }

    bool BookmarksMenu::prePopulated(){
	_bookmarksmanager = sapp_t::bookmarksManager();
	setModel(_bookmarksmanager->bookmarksModel());
	setRootIndex(_bookmarksmanager->bookmarksModel()->index(1, 0));
	// initial actions
	for(int i = 0; i < _initialactions.count(); ++ i)addAction(_initialactions.at(i));
	if(! _initialactions.isEmpty())addSeparator();
	createMenu(model()->index(0, 0), 1, this);

	return true;
    }

    void BookmarksMenu::setInitialActions(QList<QAction *> actions){
	_initialactions = actions;
	for(int i = 0; i < _initialactions.count(); ++ i)addAction(_initialactions.at(i));
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(BookmarksDialog)
#endif



    BookmarksDialog::BookmarksDialog(QWidget *parent, BookmarksManager *manager)
	: QDialog(parent){
	_bookmarksmanager = manager;
	if(! _bookmarksmanager)_bookmarksmanager = sapp_t::bookmarksManager();
	setupUi(this);

	tree->setUniformRowHeights(true);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setSelectionMode(QAbstractItemView::ContiguousSelection);
	tree->setTextElideMode(Qt::ElideMiddle);
	_bookmarksmodel = _bookmarksmanager->bookmarksModel();
	_proxymodel	= new TreeProxyModel(this);
	connect(search, &SearchLineEdit::textChanged, _proxymodel, &TreeProxyModel::setFilterFixedString);
	connect(removeButton, &QPushButton::clicked, tree, &EditTreeView::removeOne);
	_proxymodel->setSourceModel(_bookmarksmodel);
	tree->setModel(_proxymodel);
	tree->setDragDropMode(QAbstractItemView::InternalMove);
	tree->setExpanded(_proxymodel->index(0, 0), true);
	tree->setAlternatingRowColors(true);
	QFontMetrics	fm(font());
	int		header = fm.width(QLatin1Char('m')) * 40;
	tree->header()->resizeSection(0, header);
	tree->header()->setStretchLastSection(true);
	connect(tree, &EditTreeView::activated, this, &BookmarksDialog::open);
	tree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tree, &EditTreeView::customContextMenuRequested, this, &BookmarksDialog::customContextMenuRequested);
	connect(addFolderButton, &QPushButton::clicked, this, &BookmarksDialog::newFolder);
	expandNodes(_bookmarksmanager->bookmarks());
	setAttribute(Qt::WA_DeleteOnClose);
    }

    BookmarksDialog::~BookmarksDialog(){
	if(saveExpandedNodes(tree->rootIndex()))_bookmarksmanager->changeExpanded();
    }

    bool BookmarksDialog::saveExpandedNodes(const QModelIndex &parent){
	bool changed = false;
	for(int i = 0; i < _proxymodel->rowCount(parent); ++ i){
	    QModelIndex		child		= _proxymodel->index(i, 0, parent);
	    QModelIndex		sourceIndex	= _proxymodel->mapToSource(child);
	    BookmarkNode	*childNode	= _bookmarksmodel->node(sourceIndex);
	    bool		wasExpanded	= childNode->expanded;
	    if(tree->isExpanded(child)){
		childNode->expanded	= true;
		changed			|= saveExpandedNodes(child);
	    }else childNode->expanded = false;
	    changed |= (wasExpanded != childNode->expanded);
	}
	return changed;
    }

    void BookmarksDialog::expandNodes(BookmarkNode *node){
	for(int i = 0; i < node->children().count(); ++ i){
	    BookmarkNode *childNode = node->children()[i];
	    if(childNode->expanded){
		QModelIndex idx = _bookmarksmodel->index(childNode);
		idx = _proxymodel->mapFromSource(idx);
		tree->setExpanded(idx, true);
		expandNodes(childNode);
	    }
	}
    }

    void BookmarksDialog::customContextMenuRequested(const QPoint &pos){
	QMenu		menu;
	QModelIndex	index = tree->indexAt(pos);
	index = index.sibling(index.row(), 0);
	if(index.isValid() && ! tree->model()->hasChildren(index)){
	    menu.addAction(tr("Open"), this, &BookmarksDialog::open);
	    menu.addSeparator();
	}
	menu.addAction(tr("Delete"), tree, &browser::EditTreeView::removeOne);
	menu.exec(QCursor::pos());
    }

    void BookmarksDialog::open(){
	QModelIndex index = tree->currentIndex();
	if(! index.parent().isValid())return;
	emit openUrl(index.sibling(index.row(), 1).data(BookmarksModel::UrlRole).toUrl());
    }

    void BookmarksDialog::newFolder(){
	QModelIndex	currentIndex	= tree->currentIndex();
	QModelIndex	idx		= currentIndex;
	if(idx.isValid() && ! idx.model()->hasChildren(idx))idx = idx.parent();
	if(! idx.isValid())idx = tree->rootIndex();
	idx = _proxymodel->mapToSource(idx);
	BookmarkNode	*parent = _bookmarksmanager->bookmarksModel()->node(idx);
	BookmarkNode	*node	= new BookmarkNode(BookmarkNode::Folder);
	node->title = tr("New Folder");
	_bookmarksmanager->addBookmark(parent, node, currentIndex.row() + 1);
    }


#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(BookmarksToolBar)
#endif



    BookmarksToolBar::BookmarksToolBar(BookmarksModel *model, QWidget *parent)
	: QToolBar(tr("Bookmark"), parent)
	  , _bookmarksmodel(model){
	connect(this, &BookmarksToolBar::actionTriggered, this, &BookmarksToolBar::triggered);
	setRootIndex(model->index(0, 0));
	connect(_bookmarksmodel, &BookmarksModel::modelReset, this, &BookmarksToolBar::build);
	connect(_bookmarksmodel, &BookmarksModel::rowsInserted, this, &BookmarksToolBar::build);
	connect(_bookmarksmodel, &BookmarksModel::rowsRemoved, this, &BookmarksToolBar::build);
	connect(_bookmarksmodel, &BookmarksModel::dataChanged, this, &BookmarksToolBar::build);
	setAcceptDrops(true);
	setObjectName("bookmarkstoolbar");	// QObject::setProperty();
    }

    void BookmarksToolBar::dragEnterEvent(QDragEnterEvent *event){
	const QMimeData *mimeData = event->mimeData();
	if(mimeData->hasUrls())event->acceptProposedAction();
	QToolBar::dragEnterEvent(event);
    }

    void BookmarksToolBar::dropEvent(QDropEvent *event){
	const QMimeData *mimeData = event->mimeData();
	if(mimeData->hasUrls() && mimeData->hasText()){
	    QList<QUrl> urls	= mimeData->urls();
	    QAction	*action = actionAt(event->pos());
	    QString	dropText;
	    if(action)dropText = action->text();
	    int		row		= - 1;
	    QModelIndex parentIndex	= _root;
	    for(int i = 0; i < _bookmarksmodel->rowCount(_root); ++ i){
		QModelIndex	idx	= _bookmarksmodel->index(i, 0, _root);
		QString		title	= idx.data().toString();
		if(title == dropText){
		    row = i;
		    if(_bookmarksmodel->hasChildren(idx)){
			parentIndex	= idx;
			row		= - 1;
		    }
		    break;
		}
	    }
	    BookmarkNode *bookmark = new BookmarkNode(BookmarkNode::Bookmark);
	    bookmark->url	= urls.at(0).toString();
	    bookmark->title	= mimeData->text();

	    BookmarkNode	*parent			= _bookmarksmodel->node(parentIndex);
	    BookmarksManager	*bookmarksManager	= _bookmarksmodel->bookmarksManager();
	    bookmarksManager->addBookmark(parent, bookmark, row);
	    event->acceptProposedAction();
	}
	QToolBar::dropEvent(event);
    }

    void BookmarksToolBar::setRootIndex(const QModelIndex &index){
	_root = index;
	build();
    }

    QModelIndex BookmarksToolBar::rootIndex() const {
	return _root;
    }

    void BookmarksToolBar::build(){
	clear();
	for(int i = 0; i < _bookmarksmodel->rowCount(_root); ++ i){
	    QModelIndex idx = _bookmarksmodel->index(i, 0, _root);
	    if(_bookmarksmodel->hasChildren(idx)){
		FlatToolButton *button = new FlatToolButton(this);
		button->setPopupMode(FlatToolButton::InstantPopup);
		button->setArrowType(Qt::DownArrow);
		button->setText(idx.data().toString());
		ModelMenu *menu = new ModelMenu(this);
		connect(menu, &ModelMenu::activated, this, &BookmarksToolBar::activated);
		menu->setModel(_bookmarksmodel);
		menu->setRootIndex(idx);
		menu->addAction(new QAction(menu));
		button->setMenu(menu);
		button->setToolButtonStyle(Qt::ToolButtonTextOnly);
		QAction *a = addWidget(button);
		a->setText(idx.data().toString());
	    }else{
		QAction *action = addAction(idx.data().toString());
		action->setData(idx.data(BookmarksModel::UrlRole));
	    }
	}
    }

    void BookmarksToolBar::triggered(QAction *action){
	QVariant v = action->data();
	if(v.canConvert<QUrl>())emit openUrl(v.toUrl());
    }

    void BookmarksToolBar::activated(const QModelIndex &index){
	emit openUrl(index.data(BookmarksModel::UrlRole).toUrl());
    }
}

