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

#include <QThread>
#include <QWebEngineSettings>
#include <QtCore/QSettings>
#include <QtCore/QStringListModel>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStackedWidget>
#include <cassert>
#include <functional>
#include <future>
#include <thread>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

//#include "models/tree/tree_item.dec"

#include "autosaver.h"
#include "toolbar_search.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"
// #include "views/find_in_base_screen/FindTableWidget.h"

// #include "views/browser/chasewidget.h"
#include "controllers/record_table/record_controller.h"
#include "libraries/qtm/blogger.h"
#include "main.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

namespace web {
#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(ToolbarSearch)
#endif
	/*
        ToolbarSearch is a very basic search widget that also contains a small
   history.
        Searches are turned into urls that use Google to perform search
 */
	ToolbarSearch::ToolbarSearch(WebView* view_ // QStackedWidget *lineedits, QLineEdit *findtext,
	    )
	    : SearchLineEdit(view_)
	    , _autosaver(new AutoSaver(this))
	    , _maxsavedsearches(10)
	    , _stringlistmodel(gl_paras->main_window()->stringlistmodel() //new QStringListModel(this)
		  )
	    , _completer(new QCompleter(_stringlistmodel, this))
//	    , _lineedit_stack(nullptr) // new QStackedWidget(this))  // , _lineedits(lineedits)
#ifdef USE_ADDITIONAL_BUFFER
	    , _findtext(new QLineEdit(this))
#else
#endif // USE_ADDITIONAL_BUFFER
	    , _web_view(view_)
	{ // , _findtext(findtext)
	  // _lineedits->setVisible(false);
#ifdef USE_ADDITIONAL_BUFFER
		_findtext->setVisible(false);
#endif // USE_ADDITIONAL_BUFFER

		if (view_) view_->toolbarsearch(this);
		QMenu* menu_ = menu();
		connect(menu_, &QMenu::aboutToShow, this, &ToolbarSearch::show_menu);
		connect(menu_, &QMenu::triggered, this, &ToolbarSearch::triggered_menu_action);

		//		QCompleter* _completer = new QCompleter(_stringlistmodel, this);
		_completer->setCompletionMode(QCompleter::InlineCompletion);

		//		lineEdit()
		_lineedit->setCompleter(_completer);

		assert(_lineedit //lineEdit()
		    );
		connect(_lineedit, &QLineEdit::returnPressed, this, &ToolbarSearch::return_pressed);
		connect( //lineEdit(), &QLineEdit::returnPressed, //this, &ToolbarSearch::search_now); // , [&] {std::thread(&ToolbarSearch::searchNow,
		    this, &ToolbarSearch::return_pressed,
		    [&] {
			    real_url_t<QString>::instance<decltype(static_cast<ToolbarSearch*>(nullptr)->search_now(boost::intrusive_ptr<real_url_t<QString>>()))>(lineEdit()->text(),
				//				[&](boost::intrusive_ptr<real_url_t<QString>> real_target_url_) {
				//					auto bro = search_now(real_target_url_ //, is_an_extend_topic
				//					    );                                 //search_text
				//					bro->activateWindow();
				//					return bro;
				//				}
				std::bind(&ToolbarSearch::search_now, this, std::placeholders::_1));
		    } // .detach();});	//
		    );
#ifdef USE_ADDITIONAL_BUFFER
		connect(this, &SearchLineEdit::textChanged, _findtext, [&](const QString& text) { _findtext->setText(text); });
		// connect(this, &ToolbarSearch::returnPressed, _tab_widget, &TabManager::lineEditReturnPressed);
		connect(this, &ToolbarSearch::return_pressed, _findtext, &QLineEdit::returnPressed);
#else
		//		connect(this, &SearchLineEdit::textChanged, [&](const QString& text_) { lineEdit()->setText(text_); });
		//		connect(this, &ToolbarSearch::return_pressed, [&] {
		//			lineEdit()->returnPressed();
		//			//			_find_screen->find_text(lineEdit()->text());
		//		});
		// При каждом изменении текста в строке запроса
		assert(gl_paras->find_screen());
		connect(this, &ToolbarSearch::textChanged, gl_paras->find_screen(), &FindScreen::enable_findbutton, Qt::UniqueConnection);

#endif // USE_ADDITIONAL_BUFFER

		if (inactiveText() == "") setInactiveText(SearchLineEdit::_default_tip); //tr("Google")


		load();
	}

	ToolbarSearch::~ToolbarSearch()
	{
		_autosaver->saveIfNeccessary();
	}

	void ToolbarSearch::save()
	{
		QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("toolbarsearch"));
		settings.setValue(QLatin1String("recentSearches"), _stringlistmodel->stringList());
		settings.setValue(QLatin1String("maximumSaved"), _maxsavedsearches);
		settings.endGroup();
	}

	void ToolbarSearch::load()
	{
		QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("toolbarsearch"));
		QStringList list =
		    settings.value(QLatin1String("recentSearches")).toStringList();
		_maxsavedsearches =
		    settings.value(QLatin1String("maximumSaved"), _maxsavedsearches).toInt();
		_stringlistmodel->setStringList(list);
		settings.endGroup();
	}

	Browser* ToolbarSearch::search_now(boost::intrusive_ptr<real_url_t<QString>> non_url_search_text_
	    //					   , bool is_an_extend_topic
	    )
	{
		auto topic_new = non_url_search_text_->value();
		setInactiveText(topic_new);
		auto topic_old = _web_view->browser()->blogger()->topic();
		bool is_an_extend_topic = false;
		if (topic_new.contains(topic_old) || topic_old.contains(topic_new)) is_an_extend_topic = true;
		if (is_an_extend_topic) _web_view->browser()->blogger()->on_topic_changed(topic_new);
		auto to_be_url_ = to_be_url(topic_new);
		assert(to_be_url_ == QUrl() // || to_be_url_ == detail::to_qstring(web::Browser::_defaulthome)
		    );
		//move to main.cpp
		//		auto to_be_url = [&](const QUrl& url_) {
		//			QUrl url = url_;
		//			QUrl result;
		//			qDebug()
		//			    << "ToolbarSearch::search_now::to_be_url::url.scheme() =\t" << url.scheme();
		//			if (url.scheme().isEmpty() && !url.topLevelDomain().isNull()) {

		//				url.setScheme("https"); //QUrl(QString("https://") + url.toString());
		//							//				result = url;
		//			}

		//			qDebug()
		//			    << "\t!url.isRelative() =\t" << !url.isRelative()
		//			    << "\t!url.topLevelDomain().isNull() =\t" << !url.topLevelDomain().isNull()
		//			    << "\t!url.isEmpty() =\t" << !url.isEmpty()
		//			    << "\t!url.host().isNull() =\t" << !url.host().isNull() // flase
		//			    << "\turl.isValid() =\t" << url.isValid()
		//			    << "\t!url.scheme().isEmpty() =\t" << !url.scheme().isEmpty();
		//			if (!url.isEmpty() && //&& !url.host().isNull()
		//			    url.isValid() &&
		//			    !url.scheme().isEmpty() &&
		//			    !url.topLevelDomain().isNull() &&
		//			    !url.isRelative() &&
		//			    url != QUrl(web::Browser::_defaulthome)) result = url;
		//			return result;
		//		};

		auto deal_with_url = [&](tv_t* tree_view_, //const QString& search_text,
		    boost::intrusive_ptr<TreeIndex> tree_index, url_value real_url, web::Browser* browser_) {
			// QLineEdit *lineedit =
			auto ti = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
			    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
				    return tree_index->url_bind_browser(
					real_target_url_, //std::placeholders::_1,
					std::bind(&tv_t::move, tree_view_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
					[&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url); },
					browser_);
			    });
			//				tree_index->url_bind_browser(
			//			    real_url_t<url_value>::instance(real_url),
			//			    std::bind(&tv_t::move, tree_view_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			//			    [&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url); },
			//			    browser_);
			if (ti)
				ti->activate(std::bind(&web::Browser::find, browser_, std::placeholders::_1));

			//					assert(_lineedit_stack);
			//					if (_lineedit_stack) {
			//						QLineEdit* line_edit =
			//						    qobject_cast<QLineEdit*>(_lineedit_stack->currentWidget());
			//						if (line_edit)
			//							line_edit->setText(search_text);
			//					}
			lineEdit()->setText(detail::to_qstring(real_url));

			// globalparameters.entrance()->activebrowser()->tabWidget()->currentLineEdit()->setText(searchText);
			// currentLineEdit();  // lineEditReturnPressed();
			// assert(lineedit);
			// lineedit->setText(searchText);
			// lineedit->returnPressed();
			//			left_widget(UrlLineEdit::iconlabel());
			//			_left_widget->show();
			//			SearchLineEdit::searchbutton()->hide();
		};

		auto query_internet_decomposed = [&](tv_t* tree_view_, const QString& search_text, boost::intrusive_ptr<TreeIndex> tree_index, web::Browser* browser_) {
			QStringList newList = _stringlistmodel->stringList();
			if (newList.contains(search_text))
				newList.removeAt(newList.indexOf(search_text));
			newList.prepend(search_text);
			if (newList.size() >= _maxsavedsearches)
				newList.removeLast();
			if (!::sapp_t::instance()->privateBrowsing()) {
				_stringlistmodel->setStringList(newList);
				_autosaver->changeOccurred();
			}
			QUrl search_engine(QLatin1String("https://www.google.com/search"));
			QUrlQuery url_query;

			// url_query.addQueryItem(QLatin1String("q"), searchText);
			url_query.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
			url_query.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
			url_query.addQueryItem(
			    QLatin1String("client"),
			    QLatin1String(gl_paras->application_name().toLatin1())); // globalparameters.main_program_file().toLatin1()

			// urlQuery.addQueryItem();

			search_engine.setQuery(url_query);
			search_engine.setFragment("q=" + search_text);
			// emit search(url, std::bind(&TreeScreen::view_paste_child,
			// _tree_screen, std::placeholders::_1, std::placeholders::_2,
			// std::placeholders::_3));
			url_value real_url = url_value(search_engine.toString());

			deal_with_url(tree_view_, // search_text,
			    tree_index, real_url, browser_);
		};

		QString search_text = non_url_search_text_->value(); //text(); // lineEdit()->text();
		boost::intrusive_ptr<i_t> result_item(nullptr);
		ts_t* _tree_screen = gl_paras->tree_screen();
		auto tree_view_ = _tree_screen->view();
		auto current_item = tree_view_->current_item();
		boost::intrusive_ptr<TreeIndex> sibling_tree_index = TreeIndex::item_require_treeindex([&] { return tree_view_->source_model(); }, current_item);
		//		QUrl url = QUrl(search_text);
		//		url_value real_url = url_value(search_text);
		//		auto topic = Blogger::purify_topic(_findtext->text());
		Browser* activated_browser = nullptr;
		auto browser_ = is_an_extend_topic ?
		    _web_view->browser() :
		    (activated_browser = [&] {
			    Browser* result = nullptr;
			    for (auto bro : gl_paras->main_window()->browsers()) {
				    if (bro->blogger()->topic() == non_url_search_text_->value()) {
					    result = bro;
					    break;
				    }
			    }
			    return result;
		    }()) ?
		    activated_browser :
		    real_url_t<QString>::instance<web::Browser*>(search_text, [&](boost::intrusive_ptr<real_url_t<QString>> topic_) {
			    return gl_paras->main_window()->browser<boost::intrusive_ptr<real_url_t<QString>>>(topic_); // gl_paras->main_window()->browser(search_text);
		    });
		// if(url.host().isSimpleText());
		// bool url_isRelative = url.isRelative();
		// bool url_isValid = url.isValid();
		// bool host_not_null = !url.host().isNull();
		// bool host_isDetached = url.host().isDetached();
		// bool host_isEmpty = url.host().isEmpty();
		// bool host_isSimpleText = url.host().isSimpleText();
		// bool path_empty = url.path().isEmpty();
		// bool path_null = url.path().isNull();
		// QString path = url.path();
		// if( // !url.host().isEmpty() &&
		// url.isValid()
		// ) {
		// if(url.scheme().isEmpty()    //url.scheme().isNull()
		//// && url.isRelative() //&& !url.host().isNull()
		// ) {
		////url = QUrl("http://" + searchText);
		// url.setScheme(QLatin1String("https"));
		// }
		// if(url.path().isEmpty()
		// ) {
		// url.setPath(QLatin1String("//"));
		// }
		// }
		// auto tree_view = _tree_screen->tree_view();
		if (sibling_tree_index) {
			// example !url.isEmpty() && url.isValid() && !url.scheme().isEmpty()
			// move to TabWidget::loadPage
			//			auto real_url_ = to_be_url(url);
			//			if (real_url_ != QUrl() && real_url_ != detail::to_qstring(web::Browser::_defaulthome))
			//				deal_with_url(tree_view_, search_text, tree_index, url_value(url.toString()), browser_);
			//			else
			if ((result_item = gl_paras->find_screen()->find_internal_decomposed(search_text))) {
				if (result_item->count_direct() > 0) {
					// tree_view->select_as_current(TreeIndex::instance([&] {return
					// tree_view->source_model();}, result_item, result_item->parent()));
					// //
					// tree_view->index_invoke(tree_view->source_model()->index(result_item));
					// move_children is time consuming!
					auto child_items = // tree_view->move(tree_index, result_item,
							   // [&](boost::intrusive_ptr<const Linker> target) ->
							   // bool {return target->host()->field<url_type>() ==
							   // current_item->field<url_type>() &&
							   // target->host()->field<name_type>() ==
							   // current_item->field<name_type>();});
							   // //
					    tree_view_->move_children(
						sibling_tree_index, result_item,
						[&](boost::intrusive_ptr<const Linker> target,
						    boost::intrusive_ptr<const Linker> source) -> bool {
							return target->host()->field<url_key>() ==
							    source->host()->field<url_key>() &&
							    target->host()->field<name_key>() ==
							    source->host()->field<name_key>();
						});
					//
					// auto	child_items		= std::async(std::launch::async,
					// &tv_t::move_children, tree_view, tree_index, result_item,
					// [&](boost::intrusive_ptr<const Linker> target,
					// boost::intrusive_ptr<const Linker> source) -> bool {return
					// target->host()->field<url_type>() == source->host()->field<url_type>()
					// && target->host()->field<name_type>() ==
					// source->host()->field<name_type>();}).get();
					// auto _vtab_record = globalparameters.main_window()->vtab_record();
					// auto browser =
					// gl_paras->main_window()->new_editing_window(_findtext->text())->record_screen()->browser();//
					// activated_browser();
					bool changes_happened = false;
					auto _rctrl = browser_->tab_widget()->record_screen()->record_ctrl();
					if (_rctrl) {
						auto last = _rctrl->source_model()->item(pos_source(_rctrl->source_model()->count() - 1));
						// auto	current_item_	= ctrl->view()->current_item();	//
						// source_model()->item(pos_source(0));
						////	    auto	child_linkers		=
						///result_item->child_linkers();
						// auto _total_progress_counter = 0;

						for (auto it : child_items) { // for(auto il :
							// result_item->child_linkers()){//	// move
							// to search result
							// auto it = il->host();
							if (!_rctrl->source_model()->item(
								[&](const id_value id) { return id == it->id(); })) {
								boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] { return _rctrl->source_model(); }, it, last); // current_item_
								last = it;                                                                                                                // current_item_ = it;	//
								// if(record_index){
								// if(  (candidate->parent() != _session_root_item->parent())		//
								// _current_item->parent())
								// && ! _session_root_item->item_direct([&](boost::intrusive_ptr<const
								// Linker> il){return il == candidate->linker();})
								// ){
								// auto result = browser->item_bind(record_index);
								// result->activate(std::bind(&web::Entrance::find,
								// globalparameters.entrance(), std::placeholders::_1));
								// _result_list << result->linker();
								// //
								// }else{
								// auto previous_item =
								// _source_model()->item(tree_view->previous_index());
								// auto result =
								browser_->bind(record_index);
#ifdef USE_LOAD_ON_FOUND
								result->activate(std::bind(&HidableTabWidget::find, _vtab_record, std::placeholders::_1));
#else

#endif
								////		std::thread(&TreeItem::activate, result,
								///std::bind(&HidableTabWidget::find,
								///globalparameters.main_window()->vtab_record(),
								///std::placeholders::_1)).join();
								// globalparameters.status_bar()->showMessage("added node(s) : " +
								// QString::number(++ _total_progress_counter), 1000);	// across
								// thread message
								changes_happened = true;
							}
						}
						if (changes_happened) {
							auto _view = _rctrl->view();
							_view->reset();
							_rctrl->proxy_model()->setSourceModel(_rctrl->source_model());
							_view->setModel(_rctrl->proxy_model());
						}
					}
					// if(! tab_brother)
					//
					if (changes_happened) child_items[0]->activate(std::bind(&web::Browser::find, browser_, std::placeholders::_1)); // result_item->child_linkers()[0]->host()

					// std::function<void()> startWorkInAThread = [&] {
					// WorkerThread *workerThread = new WorkerThread(this, child_items);
					// connect(workerThread, &WorkerThread::resultReady, this, [&]
					// {if(child_items.size() > 0)startWorkInAThread();});
					// connect(workerThread, &WorkerThread::finished, workerThread,
					// &QObject::deleteLater);
					// workerThread->start();
					// };
					// startWorkInAThread();
				} else {
					query_internet_decomposed(tree_view_, search_text, sibling_tree_index, browser_);
				}
			} else {
				query_internet_decomposed(tree_view_, search_text, sibling_tree_index, browser_);
			}
		}

		//		if (result_item) {
		//			if (0 >= result_item->count_direct()) {
		//			}
		//			// else if(result_item != tree_view->current_item()){
		//			// auto index_result = tree_view->source_model()->index(result_item);
		//			// if(static_cast<QModelIndex>(index_result).isValid()){
		//			// auto it = tree_view->source_model()->child(index_result);
		//			// tree_view->select_as_current(TreeIndex::instance([&] {return
		//			// tree_view->source_model();}, it, it->parent()));
		//			// tree_view->index_invoke(globalparameters.main_window()->vtab_record()->activated_browser()->tabmanager()->currentWebView(),
		//			// index_result);
		//			// }
		//			// }
		//			else {
		//			}
		//		}
		browser_->activateWindow();
		return browser_;
	}

	void ToolbarSearch::show_menu()
	{
		lineEdit()->selectAll();
		QMenu* menu_ = menu();
		menu_->clear();
		QStringList recent_list = _stringlistmodel->stringList();
		if (recent_list.isEmpty()) {
			menu_->addAction(tr("No Recent Searches"));

			return;
		}
		QAction* recent = menu_->addAction(tr("Recent Searches"));
		recent->setEnabled(false);
		for (int i = 0; i < recent_list.count(); ++i) {
			QString text = recent_list.at(i);
			menu_->addAction(text)->setData(text);
		}
		menu_->addSeparator();
		menu_->addAction(tr("Clear Recent Searches"), this, &ToolbarSearch::clear);
	}

	void ToolbarSearch::triggered_menu_action(QAction* action)
	{
		QVariant v = action->data();
		if (v.canConvert<QString>()) {
			QString text = v.toString();
			lineEdit()->setText(text);
			//			auto real_url_ =
			real_url_t<QString>::instance<decltype(static_cast<ToolbarSearch*>(nullptr)->search_now(boost::intrusive_ptr<real_url_t<QString>>()))>(lineEdit()->text(),
			    //                            [&](auto real_target_url_) {
			    //                                    //                                auto topic_new = real_target_url_->value(); //
			    //                                    //                                auto topic_old = _web_view->browser()->blogger()->topic();
			    //                                    //                                //				auto topic_old_list = _stringlistmodel->stringList(); //_find_screen->browser()->blogger()->topic();
			    //                                    //                                //				bool is_an_extend_topic = false;
			    //                                    //                                //				for (int i = 0; i < topic_old_list.count(); ++i) {
			    //                                    //                                //					QString topic_old = topic_old_list.at(i);
			    //                                    //                                //					if (topic_new.contains(topic_old) || topic_old.contains(topic_new)) is_an_extend_topic = true;
			    //                                    //                                //				}
			    //                                    //                                bool is_an_extend_topic = false;
			    //                                    //                                if (topic_new.contains(topic_old) || topic_old.contains(topic_new)) is_an_extend_topic = true;
			    //                                    return this->search_now(real_target_url_ //, is_an_extend_topic
			    //                                        );
			    //                            }
			    std::bind(&ToolbarSearch::search_now, this, std::placeholders::_1));
			// std::thread(&ToolbarSearch::searchNow, this).detach();

			//			left_widget(SearchLineEdit::searchbutton());
			//			_left_widget->show();
			//			UrlLineEdit::iconlabel()->hide();
		}
	}

	void ToolbarSearch::clear()
	{
		_stringlistmodel->setStringList(QStringList());
		_autosaver->changeOccurred();
	}

	//	void ToolbarSearch::text(const QString& text_)
	//	{
	//#ifdef USE_ADDITIONAL_BUFFER
	//		_findtext->setText(text_);
	//#else
	//		lineEdit()->setText(text_);
	//#endif // USE_ADDITIONAL_BUFFER
	//	}

	//	QString ToolbarSearch::text() const
	//	{
	//#ifdef USE_ADDITIONAL_BUFFER
	//		return _findtext->text();
	//#else
	//		return lineEdit()->text();
	//#endif // USE_ADDITIONAL_BUFFER
	//	}

	void WorkerThread::run()
	{
		// QString result;
		auto element = _child_items.last(); // acrosss thread
		// auto _vtab_record = globalparameters.main_window()->vtab_record();
		auto browser_ = element->page()->browser(); // gl_paras->main_window()->activated_browser(gl_para::_what_ever_topic);
		auto _rctrl = browser_->tab_widget()->record_screen()->record_ctrl();
		if (_rctrl) {
			// auto					tab_brother		=
			// record_controller->view()->current_item();	// acrosss thread
			boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] { return _rctrl->source_model(); }, element); // tab_brother
			// if(record_index){
			// if(  (candidate->parent() != _session_root_item->parent())		//
			// _current_item->parent())
			// && ! _session_root_item->item_direct([&](boost::intrusive_ptr<const Linker>
			// il){return il == candidate->linker();})
			// ){
			// auto result = browser->item_bind(record_index);
			// result->activate(std::bind(&web::Entrance::find,
			// globalparameters.entrance(), std::placeholders::_1));
			// _result_list << result->linker();
			// //
			// }else{
			// auto previous_item = _source_model()->item(tree_view->previous_index());
			auto result = browser_->bind(record_index);
			result->activate(std::bind(&web::Browser::find, browser_, std::placeholders::_1));
			_child_items.pop_back();
			emit result_ready();
		}
	}
}
