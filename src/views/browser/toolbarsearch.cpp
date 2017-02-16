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

#include <cassert>
#include <functional>
#include <future>
#include <thread>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "autosaver.h"
#include "toolbarsearch.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"
// #include "views/find_in_base_screen/FindTableWidget.h"
#include <QThread>
#include <QWebEngineSettings>
#include <QtCore/QSettings>
#include <QtCore/QStringListModel>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStackedWidget>

// #include "views/browser/chasewidget.h"
#include "controllers/record_table/record_controller.h"
#include "main.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "views/browser/browser_dock.h"
#include "views/browser/tabwidget.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/hidable_tabwidget.h"
#include "views/main_window/main_window.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"
#include "libraries/qtm/editing_window.h"


namespace browser {
#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(ToolbarSearch)
#endif
/*
	ToolbarSearch is a very basic search widget that also contains a small history.
	Searches are turned into urls that use Google to perform search
 */
	ToolbarSearch::ToolbarSearch(QWidget *parent) // QStackedWidget *lineedits, QLineEdit *findtext,
		: SearchLineEdit(parent)
		  , _autosaver(new AutoSaver(this))
		  , _maxsavedsearches(10)
		  , _stringlistmodel(new QStringListModel(this))
		  , _lineedits(nullptr) // new QStackedWidget(this))  // , _lineedits(lineedits)
		  , _findtext(new QLineEdit(this)){ // , _findtext(findtext)
						    //        _lineedits->setVisible(false);
		_findtext->setVisible(false);

		QMenu *m = menu();
		connect(m, &QMenu::aboutToShow, this, &ToolbarSearch::show_menu);
		connect(m, &QMenu::triggered, this, &ToolbarSearch::triggered_menu_action);

		QCompleter *completer = new QCompleter(_stringlistmodel, this);
		completer->setCompletionMode(QCompleter::InlineCompletion);
		lineEdit()->setCompleter(completer);

		assert(lineEdit());
		connect(lineEdit(), &QLineEdit::returnPressed, this, &ToolbarSearch::search_now); // , [&] {std::thread(&ToolbarSearch::searchNow, this).detach();});	//

		connect(this, &SearchLineEdit::textChanged, _findtext, [&](const QString &text){
				_findtext->setText(text);
			});
		// connect(this, &ToolbarSearch::returnPressed, _tabmanager, &TabManager::lineEditReturnPressed);
		connect(this, &ToolbarSearch::return_pressed, _findtext, &QLineEdit::returnPressed);

		setInactiveText(tr("Google"));

		load();
	}

	ToolbarSearch::~ToolbarSearch(){
		_autosaver->saveIfNeccessary();
	}

	void ToolbarSearch::save(){
		QSettings settings(gl_paras.root_path() + "/" + gl_paras.target_os() + "/" + gl_paras._browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("toolbarsearch"));
		settings.setValue(QLatin1String("recentSearches"), _stringlistmodel->stringList());
		settings.setValue(QLatin1String("maximumSaved"), _maxsavedsearches);
		settings.endGroup();
	}

	void ToolbarSearch::load(){
		QSettings settings(gl_paras.root_path() + "/" + gl_paras.target_os() + "/" + gl_paras._browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("toolbarsearch"));
		QStringList list = settings.value(QLatin1String("recentSearches")).toStringList();
		_maxsavedsearches = settings.value(QLatin1String("maximumSaved"), _maxsavedsearches).toInt();
		_stringlistmodel->setStringList(list);
		settings.endGroup();
	}

	void ToolbarSearch::search_now(){
		QString search_text = lineEdit()->text();

		auto result_item = gl_paras.find_screen()->find_clicked();
		ts_t *_tree_screen = gl_paras.tree_screen();
		auto tree_view = _tree_screen->view();
		auto current_item = tree_view->current_item();
		boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::create_treeindex_from_item([&] {return tree_view->source_model();}, current_item);
		auto browser = gl_paras.main_window()->new_editing_window(_findtext->text())->record_screen()->browser();
		if(result_item){
			if(0 >= result_item->count_direct()){
				QUrl url = QUrl(search_text);
				// if(url.host().isSimpleText());
				//        bool url_isRelative = url.isRelative();
				//        bool url_isValid = url.isValid();
				//        bool host_not_null = !url.host().isNull();
				//        bool host_isDetached = url.host().isDetached();
				//        bool host_isEmpty = url.host().isEmpty();
				//        bool host_isSimpleText = url.host().isSimpleText();
				//        bool path_empty = url.path().isEmpty();
				//        bool path_null = url.path().isNull();
				//        QString path = url.path();
				//        if( // !url.host().isEmpty() &&
				//            url.isValid()
				//        ) {
				//            if(url.scheme().isEmpty()    //url.scheme().isNull()
				//               // && url.isRelative() //&& !url.host().isNull()
				//              ) {
				//                //url = QUrl("http://" + searchText);
				//                url.setScheme(QLatin1String("https"));
				//            }
				//            if(url.path().isEmpty()
				//              ) {
				//                url.setPath(QLatin1String("//"));
				//            }
				//        }
				//            auto tree_view = _tree_screen->tree_view();
				if(tree_index){
					// example !url.isEmpty() && url.isValid() && !url.scheme().isEmpty()
					if(  !url.isEmpty()
					  && !url.host().isNull()
					  && url.isValid()
					  && !url.scheme().isEmpty()
					     // && url != QUrl(DockedWindow::_defaulthome) //&& !url.host().isNull()
					  ){
						// QLineEdit *lineedit =

						auto ti = tree_index->bind(url, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const TreeItem> it_) -> bool {return url_equal(it_->field<home_type>().toStdString(), url.toString().toStdString()) || url_equal(it_->field<url_type>().toStdString(), url.toString().toStdString());});
						if(ti) ti->activate(std::bind(&wn_t::find, gl_paras.main_window(), std::placeholders::_1));
						assert(_lineedits);
						if(_lineedits){
							QLineEdit *line_edit = qobject_cast<QLineEdit *>(_lineedits->currentWidget());
							if(line_edit) line_edit->setText(search_text);
						}
						// globalparameters.entrance()->activebrowser()->tabWidget()->currentLineEdit()->setText(searchText);

						// globalparameters.entrance()->activebrowser()->tabWidget()->new_view(register_record(url));

						// currentLineEdit();  // lineEditReturnPressed();
						// assert(lineedit);
						// lineedit->setText(searchText);
						// lineedit->returnPressed();
					}else{
						QStringList newList = _stringlistmodel->stringList();
						if(newList.contains(search_text)) newList.removeAt(newList.indexOf(search_text));
						newList.prepend(search_text);
						if(newList.size() >= _maxsavedsearches) newList.removeLast();
						if(!sapp_t::instance()->privateBrowsing()){
							_stringlistmodel->setStringList(newList);
							_autosaver->changeOccurred();
						}
						QUrl search_engine(QLatin1String("https://www.google.com/search"));
						QUrlQuery url_query;

						//                url_query.addQueryItem(QLatin1String("q"), searchText);
						url_query.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
						url_query.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
						url_query.addQueryItem(QLatin1String("client"), QLatin1String(gl_paras.application_name().toLatin1())); // globalparameters.main_program_file().toLatin1()

						// urlQuery.addQueryItem();

						search_engine.setQuery(url_query);
						search_engine.setFragment("q=" + search_text);
						//		    emit search(url, std::bind(&TreeScreen::view_paste_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
						auto ti = tree_index->bind(search_engine, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const TreeItem> it_) -> bool {return url_equal(it_->field<home_type>().toStdString(), search_engine.toString().toStdString()) || url_equal(it_->field<url_type>().toStdString(), search_engine.toString().toStdString());});
						if(ti) ti->activate(std::bind(&wn_t::find, gl_paras.main_window(), std::placeholders::_1));
					}
				}
			}
			//	else if(result_item != tree_view->current_item()){
			//	    auto index_result = tree_view->source_model()->index(result_item);
			//	    if(static_cast<QModelIndex>(index_result).isValid()){
			//		auto it = tree_view->source_model()->child(index_result);
			//		tree_view->select_as_current(TreeIndex::instance([&] {return tree_view->source_model();}, it, it->parent()));
			//		tree_view->index_invoke(globalparameters.main_window()->vtab_record()->activated_browser()->tabmanager()->currentWebView(), index_result);
			//	    }
			//	}
			else{
				//	    tree_view->select_as_current(TreeIndex::instance([&] {return tree_view->source_model();}, result_item, result_item->parent()));	// tree_view->index_invoke(tree_view->source_model()->index(result_item));
				// move_children is time consuming!
				auto child_items = // tree_view->move(tree_index, result_item, [&](boost::intrusive_ptr<const Linker> target) -> bool {return target->host()->field<url_type>() == current_item->field<url_type>() && target->host()->field<name_type>() == current_item->field<name_type>();});		//
						   tree_view->move_children(tree_index, result_item, [&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source) -> bool {return target->host()->field<url_type>() == source->host()->field<url_type>() && target->host()->field<name_type>() == source->host()->field<name_type>();});
				//
				//	    auto	child_items		= std::async(std::launch::async, &tv_t::move_children, tree_view, tree_index, result_item, [&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source) -> bool {return target->host()->field<url_type>() == source->host()->field<url_type>() && target->host()->field<name_type>() == source->host()->field<name_type>();}).get();
				//            auto _vtab_record = globalparameters.main_window()->vtab_record();
//				auto browser = gl_paras.main_window()->new_editing_window(_findtext->text())->record_screen()->browser();// activated_browser();
				auto ctrl	= browser->record_screen()->record_controller();
				auto last	= ctrl->source_model()->item(pos_source(ctrl->source_model()->count() - 1));
				//	    auto	current_item_	= ctrl->view()->current_item();	// source_model()->item(pos_source(0));
				////	    auto	child_linkers		= result_item->child_linkers();
				//	    auto _total_progress_counter = 0;
				for(auto it : child_items){  //	    for(auto il : result_item->child_linkers()){//	// move to search result
					//		auto it = il->host();
					if(!ctrl->source_model()->item([&](const id_value id){return id == it->id();})){
						boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] {return ctrl->source_model();}, it, last); // current_item_
						last = it; // current_item_ = it;	//
						//                            if(record_index){
						//                            if(  (candidate->parent() != _session_root_item->parent())		// _current_item->parent())
						//                              && ! _session_root_item->item_direct([&](boost::intrusive_ptr<const Linker> il){return il == candidate->linker();})
						//                                ){
						//                                auto result = browser->item_bind(record_index);
						//                                result->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));
						//                                _result_list << result->linker();																												//
						//                            }else{
						// auto previous_item = _source_model()->item(tree_view->previous_index());
						//		    auto result =
						browser->bind(record_index);
#ifdef USE_LOAD_ON_FOUND
						result->activate(std::bind(&HidableTabWidget::find, _vtab_record, std::placeholders::_1));
#else

#endif
						////		std::thread(&TreeItem::activate, result, std::bind(&HidableTabWidget::find, globalparameters.main_window()->vtab_record(), std::placeholders::_1)).join();
						//		globalparameters.status_bar()->showMessage("added node(s) : " + QString::number(++ _total_progress_counter), 1000);	// across thread message
					}
				}
				auto _view = ctrl->view();
				_view->reset();
				ctrl->proxy_model()->setSourceModel(ctrl->source_model());
				_view->setModel(ctrl->proxy_model());
				//	    if(! tab_brother)
				//
				child_items[0]->activate(std::bind(&wn_t::find, gl_paras.main_window(), std::placeholders::_1)); //	    result_item->child_linkers()[0]->host()

				//	    std::function<void()> startWorkInAThread = [&] {
				//		WorkerThread *workerThread = new WorkerThread(this, child_items);
				//		connect(workerThread, &WorkerThread::resultReady, this, [&] {if(child_items.size() > 0)startWorkInAThread();});
				//		connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
				//		workerThread->start();
				//	    };
				//	    startWorkInAThread();
			}
		}
	}

	void ToolbarSearch::show_menu(){
		lineEdit()->selectAll();
		QMenu *m = menu();
		m->clear();
		QStringList list = _stringlistmodel->stringList();
		if(list.isEmpty()){
			m->addAction(tr("No Recent Searches"));

			return;
		}
		QAction *recent = m->addAction(tr("Recent Searches"));
		recent->setEnabled(false);
		for(int i = 0; i < list.count(); ++i){
			QString text = list.at(i);
			m->addAction(text)->setData(text);
		}
		m->addSeparator();
		m->addAction(tr("Clear Recent Searches"), this, &ToolbarSearch::clear);
	}

	void ToolbarSearch::triggered_menu_action(QAction *action){
		QVariant v = action->data();
		if(v.canConvert<QString>()){
			QString text = v.toString();
			lineEdit()->setText(text);
			search_now(); // std::thread(&ToolbarSearch::searchNow, this).detach();
		}
	}

	void ToolbarSearch::clear(){
		_stringlistmodel->setStringList(QStringList());
		_autosaver->changeOccurred();
		;
	}

	void ToolbarSearch::text(const QString &text){_findtext->setText(text);}

	QString ToolbarSearch::text() const {return _findtext->text();}

	void WorkerThread::run(){
		//		    QString result;
		auto element = _child_items.last(); // acrosss thread
		//    auto _vtab_record = globalparameters.main_window()->vtab_record();
		auto browser = gl_paras.main_window()->activated_browser();
		auto record_controller = browser->record_screen()->record_controller();
		//	auto					tab_brother		= record_controller->view()->current_item();	// acrosss thread
		boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] {return record_controller->source_model();}, element); // tab_brother
		//                            if(record_index){
		//                            if(  (candidate->parent() != _session_root_item->parent())		// _current_item->parent())
		//                              && ! _session_root_item->item_direct([&](boost::intrusive_ptr<const Linker> il){return il == candidate->linker();})
		//                                ){
		//                                auto result = browser->item_bind(record_index);
		//                                result->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));
		//                                _result_list << result->linker();																												//
		//                            }else{
		// auto previous_item = _source_model()->item(tree_view->previous_index());
		auto result = browser->bind(record_index);
		result->activate(std::bind(&wn_t::find, gl_paras.main_window(), std::placeholders::_1));
		_child_items.pop_back();
		emit result_ready();
	}
}
