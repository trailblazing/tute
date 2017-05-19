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

#include <QLineEdit>
#include <QThread>

// #include "models/tree/TreeItem.h"
// #include "models/tree/TreeModel.h"
#include "libraries/fixed_parameters.h"
#include "searchlineedit.h"
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

class i_t;
class tm_t;
class ts_t;
class tkm_t;
class FindScreen;
template <typename>
struct real_url_t;

namespace web {
	class ChaseWidget;
}

// QT_BEGIN_NAMESPACE

namespace web {
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
		QList<boost::intrusive_ptr<i_t>>& _child_items;

	    public:
		WorkerThread(QWidget* p, QList<boost::intrusive_ptr<i_t>>& l)
		    : QThread(p)
		    , _child_items(l)
		{
		}
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


#if QT_VERSION == 0x050600
		void return_pressed() W_SIGNAL(return_pressed); //
#else
		void return_pressed();
#endif
	    public:
		ToolbarSearch(WebView* view_ = nullptr); // QStackedWidget *lineedits, QLineEdit *findtext,

		~ToolbarSearch();
		//		void text(const QString& text_);
		//		QString text() const;

		//		QLineEdit* findtext() { return _findtext; }

		// void findtext(QLineEdit *findtext) {_findtext = findtext;}

		//		QStackedWidget* lineedit_stack() { return _lineedit_stack; }
		//		void lineedit_stack(QStackedWidget* lineedit_stack_) { _lineedit_stack = lineedit_stack_; }
//		std::function<url_value(const QString&)> query_internet;

	    public slots:
		void clear();
		web::Browser* search_now(boost::intrusive_ptr<real_url_t<QString>> non_url_search_text_); // return browser_ref will lead lease.h need web::Browser defined

	    private slots:
		void save();
		void show_menu();
		void triggered_menu_action(QAction* action);

	    public:
		static int _maxsavedsearches;

	    private:
		void load();

		AutoSaver* _autosaver;

		QStringListModel* _stringlistmodel;
		QCompleter* _completer;
//		QStackedWidget* _lineedit_stack;
#ifdef USE_ADDITIONAL_BUFFER
		QLineEdit* _findtext;
#endif // USE_ADDITIONAL_BUFFER
		// TabManager           *_tab_widget;
		// web::ChaseWidget *_chasewidget;
		web::WebView* _web_view;
	};
}

// QT_END_NAMESPACE

#endif // TOOLBARSEARCH_H
