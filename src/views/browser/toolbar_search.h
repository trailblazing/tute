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

#ifndef TOOLBARSEARCH_H
#define TOOLBARSEARCH_H

#include <functional>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include "searchlineedit.h"
#include <QLineEdit>
#include <QThread>

// #include "models/tree/TreeItem.h"
// #include "models/tree/TreeModel.h"
#include "views/tree/tree_screen.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QUrl;
class QAction;
class QStringListModel;
class QLineEdit;
QT_END_NAMESPACE

        QT_BEGIN_NAMESPACE
class QCompleter;
class QLineEdit;
class QMenu;
class QStackedWidget;
class FindTableWidget;
QT_END_NAMESPACE

class TreeItem;
class tm_t;
class ts_t;
class tkm_t;

namespace browser {
	class ChaseWidget;
}

// QT_BEGIN_NAMESPACE

namespace browser {
	class AutoSaver;

	class WorkerThread : public QThread {
	Q_OBJECT

	void run() Q_DECL_OVERRIDE;
	// {
	////		    QString result;
	// auto e = _child_linkers.last();
	// _child_linkers.pop_back();
	// emit resultReady();
	// }
	signals:
		void result_ready();

	private:
		QList<boost::intrusive_ptr<TreeItem> > &_child_items;

	public:
		WorkerThread(QWidget *p, QList<boost::intrusive_ptr<TreeItem> > &l)
			: QThread(p)
			  , _child_items(l)
		{}
	};

	class ToolbarSearch : public SearchLineEdit {
#if QT_VERSION == 0x050600
	W_OBJECT(ToolbarSearch)
#else
	Q_OBJECT
#endif

	signals:
		// void search(boost::intrusive_ptr<TreeItem>, const QUrl &url
		// , const TreeScreen::paste_strategy &_view_paste_strategy
		// , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url) ->bool {return it ? QUrl(it->field("url")).fragment() == _url.fragment() : false;}
		// );

		void return_pressed()
#if QT_VERSION == 0x050600
		W_SIGNAL(return_pressed) //
#else
		;
#endif
	public: ToolbarSearch( // QStackedWidget *lineedits, QLineEdit *findtext,
			QWidget *parent = 0);
		~ToolbarSearch();
		void text(const QString &text);
		QString text() const;

		QLineEdit *findtext(){return _findtext;}

		// void findtext(QLineEdit *findtext) {_findtext = findtext;}

		QStackedWidget *lineedits(){return _lineedits;}

		void lineedits(QStackedWidget *lineedits){_lineedits = lineedits;}

	public slots:
		void clear();
		void search_now();

	private slots:
		void save();
		void show_menu();
		void triggered_menu_action(QAction *action);

	private:
		void load();

		AutoSaver *_autosaver;
		int _maxsavedsearches;
		QStringListModel *_stringlistmodel;
		QStackedWidget *_lineedits;
		QLineEdit *_findtext;
		// TabManager           *_tabmanager;
		// browser::ChaseWidget *_chasewidget;
	};
}

// QT_END_NAMESPACE

#endif // TOOLBARSEARCH_H
