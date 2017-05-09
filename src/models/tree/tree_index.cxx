
//#include "models/tree/tree_item.dec"

#include "tree_index.hxx"

#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_branch.h"
#include "libraries/clipboard_records.h"
#include "libraries/crypt/password.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/tree/tree_view.h"

// boost::intrusive_ptr<TreeIndex> TreeIndex::instance(const std::function<km_t
// *()> &current_model, boost::intrusive_ptr<TreeItem> host_parent, int
// sibling_order){
// return new TreeIndex(current_model, host_parent, sibling_order);
// }
boost::intrusive_ptr<TreeIndex>
TreeIndex::item_require_treeindex(const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> host_)
{
	boost::intrusive_ptr<TreeIndex> result(nullptr);
	if (!host_)
		throw std::runtime_error("! _host");
	auto host_parent_ = host_->parent();
	auto absolute_model = [&] {
		return gl_paras->tree_screen()->view()->know_model_board();
	};
	auto absolute_root = absolute_model()->root_item();
	auto current_root = [&] {
		return static_cast<tkm_t*>(current_model_())->root_item();
	};
	auto tree_view_ = static_cast<tv_t*>(static_cast<QObject*>(current_model_())->parent());
	//
	auto parent_is_valid = [&](boost::intrusive_ptr<i_t> host_parent) -> bool {
		bool result = false;
		auto absolute_root = gl_paras->tree_screen()->view()->know_model_board()->root_item();
		auto current_root = [&]() {
			return static_cast<tkm_t*>(current_model_())->root_item();
		};
		if (!host_parent)
			result = true;
		else if (TreeIndex::is_ancestor_of(host_parent, current_root())) {
			auto new_root = (host_parent == absolute_root) ? host_parent : host_parent->parent();

			auto tv = dynamic_cast<tv_t*>(
			    static_cast<QObject*>(current_model_())->parent());
			assert(tv);
			tv->intercept(new_root); // static_cast<km_t
			// *>(current_model())->intercept(new_root);
			if (host_parent == absolute_root)
				result = true;
			else if (host_parent == current_root())
				result = true;
			else { // if(!current_model()->item([ = ](boost::intrusive_ptr<const
				// TreeItem> it) {return it->id() == parent_item->id();}) &&
				// parent_item->id() != "")
				result = (static_cast<QModelIndex>(current_model_()->index(host_parent))
						 .isValid()) &&
				    (current_model_()->item([=](boost::intrusive_ptr<const i_t> it) {
						 return it->id() == host_parent->id();
					 }));
			}
			// _current_index = _current_model()->index(current_item());
			// assert(_current_index.isValid());
		} else if (host_parent == current_root())
			result = true; // assert(_current_index.isValid());
		else {
			// assert(_current_index.isValid());
			result = (static_cast<QModelIndex>(current_model_()->index(host_parent))
					 .isValid()) &&
			    (current_model_()->item([=](boost::intrusive_ptr<const i_t> it) {
					 return it->id() == host_parent->id();
				 }));
		}
		// assert(_current_index.isValid());
		return result;
	};
	auto host_index_valid = [&] {
		index_tree index;
		if (host_ == absolute_root) {
			// throw std::runtime_error("operating on absolute item");
			////		  boost::intrusive_ptr<TreeItem> default_child;
			////		  if(_host->count_direct() > 0){
			////		      default_child = _host->child_direct(0);
			////		  }else{
			////		      default_child = new TreeItem(_host);
			////		  }
			////		  index = absolute_model()->fake_index(default_child);
		} else if (host_ == current_root() && host_ != absolute_root) {
			// auto tree_view_ = dynamic_cast<tv_t *>(static_cast<QObject
			// *>(current_model_())->parent());
			assert(tree_view_);
			if (host_parent_)
				tree_view_->intercept(
				    host_parent_); // static_cast<km_t
						   // *>(current_model())->intercept(_host->parent());
			index = static_cast<tkm_t*>(current_model_())->index(host_);
		} else
			index = static_cast<tkm_t*>(current_model_())->index(host_);
		if (!static_cast<QModelIndex>(index).isValid()) {
			tree_view_->intercept(absolute_root);
			index = static_cast<tkm_t*>(current_model_())->index(host_);
		}
		// assert(static_cast<QModelIndex>(index).isValid() || (host_ ==
		// absolute_root));

		return index;
	};

	index_tree host_index_;
	int sibling_order_ = -1;
	// try{
	// if(! _host_parent){throw std::runtime_error(formatter() << "!
	// _host_parent");}
	if (!parent_is_valid(host_parent_))
		throw std::runtime_error(
		    formatter() << "! parent_is_valid"); // assert(current_parent_valid);
	host_index_ = host_index_valid();
	if (!static_cast<QModelIndex>(host_index_).isValid()) {
		url_value target_url = host_->field<home_key>() != home_value("") ? url_value(detail::to_qstring(host_->field<home_key>())) : host_->field<url_key>();
		if (QUrl(detail::to_qstring(target_url)) == QUrl() || !QUrl(detail::to_qstring(target_url)).isValid()) {
			target_url = web::Browser::_defaulthome;
			host_->field<home_key>(detail::from_qstring<home_key>(web::Browser::_defaulthome));
			host_->field<url_key>(web::Browser::_defaulthome);
		}
		// result = new TreeIndex(current_model_, host_, host_index_,
		// sibling_order_);
		host_ = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(target_url,
		    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
			    return TreeIndex::url_require_item_from_tree(
				real_target_url_, //std::placeholders::_1,
				std::bind(&tv_t::move, tree_view_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
				[&](boost::intrusive_ptr<const i_t> it_) -> bool {
					return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), target_url) || url_equal(it_->field<url_key>(), target_url);
				});
		    });

		//			TreeIndex::url_require_item_from_tree(real_url_t<url_value>::instance(target_url),
		//							      std::bind(&tv_t::move, tree_view_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), // std::placeholders::_1
		//		    [&](boost::intrusive_ptr<const i_t> it_) -> bool {
		//			    return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), target_url) || url_equal(it_->field<url_key>(), target_url);
		//		    }); // instance(current_model_, tree_view_->current_item())->

		host_index_ = static_cast<tkm_t*>(current_model_())->index(host_);
		assert(static_cast<QModelIndex>(host_index_).isValid() || (host_ == absolute_root)); // throw std::runtime_error(formatter() <<
												     // "! host_index_valid");
	}
	if (static_cast<QModelIndex>(host_index_).isValid()) {
		sibling_order_ = host_->parent() ? host_->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
			return il == host_->linker() && il->host() == host_ && il->host_parent() == host_->parent();
		}) :
						   0;
		assert(sibling_order_ >= 0);
		if (host_parent_) {
			//
			if (!host_parent_->contains_direct(host_)) {
				tree_view_->move(
				    TreeIndex::item_require_treeindex(current_model_, host_parent_),
				    host_,
				    [&](boost::intrusive_ptr<const Linker> il) -> bool {
					    return url_equal(detail::to_qstring(host_->field<url_key>()).toStdString(), detail::to_qstring(il->host()->field<url_key>()).toStdString()) && il->host()->id() == host_->id() && il == host_->linker() && il->host_parent() == host_->parent();
				    },
				    true);
				// throw std::runtime_error("_host_parent does not contains _host");
			}
		}
		if (!static_cast<QModelIndex>(
			 static_cast<tkm_t*>(current_model_())->index(host_))
			 .isValid() &&
		    host_ != static_cast<tkm_t*>(current_model_())->root_item())
			throw std::runtime_error("! current_model()->index(_host).isValid()");
		// if(_sibling_order < 0){throw std::runtime_error(formatter() <<
		// "sibling_order < 0");}
		// auto count_direct = _host_parent->count_direct();
		// count_direct = (count_direct == 0) ? 1 : count_direct;
		//// assert(sibling_order < count_direct);
		// if(_sibling_order >= count_direct){throw std::runtime_error(formatter()
		// << "sibling_order >= count_direct");}
		// _host_parent_index = _current_model()->index(_host_parent);
		// auto link = _host->linker();
		if (!host_->linker())
			throw std::runtime_error(formatter() << "! host_linker"); // _host = link->host();
		// if(! _host){throw std::runtime_error(formatter() << "! _host");}
		// _host_index = _current_model()->index(_host);
		// if(! _host_index.isValid()){throw std::runtime_error(formatter() << "!
		// _host_index.isValid()");}
		// assert(_current_index.isValid());
		// }catch(std::exception &e){
		////	qDebug() << e.what();
		// throw std::runtime_error(e.what());
		// }
		result = new TreeIndex(current_model_, host_, host_index_, sibling_order_);
	}
	assert(static_cast<QModelIndex>(host_index_).isValid() || (host_ == absolute_root));

	return result; // host_parent->sibling_order([&](boost::intrusive_ptr<const
		       // Linker> il){return il->host() == host && il ==
		       // host->linker() && host->parent() == host_parent;})
}

TreeIndex::TreeIndex(const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> host_, const index_tree& host_index_, const int sibling_order_)
    : _current_model(current_model_)
    , _host(host_)
    , _sibling_order(sibling_order_)
    , _host_index(host_index_)
{
}

std::function<tkm_t*()> TreeIndex::current_model() const
{
	return _current_model;
}

// boost::intrusive_ptr<TreeItem> TreeIndex::host_parent() const {return
// _host_parent;}

boost::intrusive_ptr<i_t> TreeIndex::host() const
{
	return _host;
} // {return _host_parent ? _host_parent->child_direct(_sibling_order) :
  // nullptr;}

// index_tree TreeIndex::host_parent_index() const {return _host_parent_index;}
// // _current_model()->index(_parent);

index_tree TreeIndex::host_index() const
{
	return _host_index;
} // _current_model()->index(current_item())

int TreeIndex::sibling_order() const
{
	return _sibling_order;
}

boost::intrusive_ptr<i_t>
TreeIndex::url_require_item_from_tree( //const url_value& find_url_, //
    boost::intrusive_ptr<real_url_t<url_value>> real_find_url_,
    const insert_strategy& tree_view_insert_strategy_,
    equal_url equal_)
{
	auto find_url_ = real_find_url_->value();

	//	assert(to_be_url(find_url_) != QUrl());
	//	assert(find_url_ != web::Browser::_defaulthome);
	boost::intrusive_ptr<i_t> _result(nullptr); // =  _know_model_board->root_item();
	assert(detail::to_qstring(find_url_) != QUrl().toString());
	if (detail::to_qstring(find_url_) != QUrl().toString()) {
		auto _tree_view = gl_paras->tree_screen()->view(); // static_cast<tv_t
								   // *>(static_cast<QObject
								   // *>(_current_model())->parent());
		auto _current_model = [&] { return _tree_view->source_model(); };
		auto _current_item = _tree_view->current_item();
		auto _treeindex = TreeIndex::item_require_treeindex(_current_model, _current_item);
		auto _tree_view_insert_strategy_impl =
		    [&](boost::intrusive_ptr<i_t> _result, // boost::intrusive_ptr<TreeIndex>_treeindex,
			const substitute_condition& _substitute_condition,
			const insert_strategy& tree_view_insert_strategy__,
			const bool _item_is_brand_new,
			const url_value& _find_url,
			const std::function<bool(boost::intrusive_ptr<i_t>)>& _check_url) -> boost::intrusive_ptr<i_t> { // KnowView::
			// auto	_current_model	= _treeindex->current_model();
			// auto _current_item = _treeindex->host();
			assert(_result != _tree_view->know_model_board()->root_item());
			if (_item_is_brand_new) { // || !
				// _current_model()->item([&](boost::intrusive_ptr<const
				// TreeItem> it){return it->id() ==
				// _result->id();})
				assert(_check_url(_result));
				_result = tree_view_insert_strategy__(_treeindex, _result, _substitute_condition, true); // it->insert_new_item(it->current_count() - 1, _result);
				assert(_result);
				assert(_check_url(_result) || _result->field<url_key>() == web::Browser::_defaulthome || _find_url == web::Browser::_defaulthome);
				if (_result->field<url_key>() == web::Browser::_defaulthome && _find_url != web::Browser::_defaulthome)
					_result->field<url_key>(url_value(_find_url)); // item->field("url")
				_tree_view->synchronized(false);
				assert(equal_(_result));
			} else if (_current_item != _result) {
				if (TreeIndex::is_ancestor_of(_result, _current_item)) {
					auto host_new = TreeLevel::instance(TreeIndex::item_require_treeindex(_current_model, _result->parent()), _current_item)->move(); // static_cast<tkm_t *>(current_model())
					assert(host_new == _current_item);
					_current_item = host_new;
					// _tree_screen->session_root_id(_result->id());
					assert(_result != _tree_view->know_model_board()->root_item());
					if (!static_cast<QModelIndex>(_tree_view->source_model()->index(_result)).isValid()) _tree_view->cursor_focus(_result);
					assert(_result != _tree_view->know_model_board()->root_item());
					assert(_result);
					assert(static_cast<QModelIndex>(_tree_view->source_model()->index(_result)).isValid());
					_tree_view->synchronized(false);
					assert(equal_(_result));
				} else if (_tree_view->session_root_auto() != _result->parent()) { // if(session_root_item->is_ancestor_of(_result)
												   // && session_root_item != _result->parent())
					_result = _tree_view->move(_treeindex, _result, _substitute_condition);
					assert(_result != _tree_view->know_model_board()->root_item());
					assert(_result);
					_tree_view->synchronized(false);
					assert(equal_(_result));
				}
			}
			_tree_view->know_model_save();

			assert(_result != _tree_view->know_model_board()->root_item());
			assert(_result);
			assert(equal_(_result));

			return _result;
		};

		boost::intrusive_ptr<i_t> in_tree = _tree_view->know_model_board()->child(equal_);

		bool item_is_brand_new = false;

		// if(browser_pages) {
		auto wev_view_ = gl_paras->main_window()->find(
		    [&](boost::intrusive_ptr<const ::Binder> b) -> bool {
			    return equal_(b->host());
		    });

		boost::intrusive_ptr<i_t> it_in_web(nullptr);
		if (wev_view_) {
			it_in_web = wev_view_->page()->binder()->host();
			assert(equal_(it_in_web)); // assert(_result->url<url_type>() ==
						   // url_type()(_find_url));
		}
		if (in_tree && in_tree != _tree_view->know_model_board()->root_item()) {
			if (!it_in_web) {
				assert(equal_(in_tree)); // assert(_source_item->url<url_type>() ==
				// url_type()(_find_url));
				if (in_tree->is_lite()) in_tree->to_fat();
				_result = in_tree;
				assert(equal_(_result));
			} else {
				assert(equal_(it_in_web)); // assert(_result->url<url_type>() ==
							   // url_type()(_find_url));
				assert(equal_(in_tree));   // assert(_source_item->url<url_type>() ==
							   // url_type()(_find_url));
				boost::intrusive_ptr<i_t> result_(it_in_web);
				if (it_in_web != in_tree)
					result_ = TreeLevel::instance(TreeIndex::item_require_treeindex(_current_model, in_tree), it_in_web)->merge(); //it_in_web disappeared
				// TreeLevel::instance(TreeIndex::treeindex_from_item(_current_model,
				// in_browser), in_tree)->merge();
				_result = result_;
				if (_result->is_lite()) _result->to_fat();
				if (_result->field<id_key>() == id_value(""))
					_result->field<id_key>((detail::to_qstring(_result->field<dir_key>()).length() > 0) ? id_value(detail::to_qstring(_result->field<dir_key>())) : id_value(get_unical_id()));
				//				assert(gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) -> bool { return url_equal(detail::to_string(b->host()->field<home_key>()), detail::to_string(_result->field<home_key>())) || url_equal(b->host()->field<url_key>(), _result->field<url_key>()); }) ||
				//					_result->field<url_key>() == web::Browser::_defaulthome
				//					);
				assert(equal_(_result));
			}
			assert(!_result->is_lite());
		} else {
			item_is_brand_new = true;
			if (!it_in_web) {

				auto view =
				    gl_paras->main_window()->find(
					[&](boost::intrusive_ptr<const ::Binder> b) {
						return url_equal(detail::to_qstring(b->host()->field<home_key>()), detail::to_qstring(web::Browser::_defaulthome)) || url_equal(detail::to_qstring(b->host()->field<url_key>()), detail::to_qstring(web::Browser::_defaulthome));
					});
				if (!view) {
					// record.generator(generator);

					// Имя директории, в которой расположены файлы картинок, используемые в
					// тексте и приаттаченные файлы
					QString directory = DiskHelper::create_temp_directory(); //

					full_fields_map data;
					boost::fusion::at_key<id_key>(data) = get_unical_id();
					boost::fusion::at_key<pin_key>(data) = pin_value(bool_from_check_state[Qt::Unchecked]);
					boost::fusion::at_key<name_key>(data) = "";
					boost::fusion::at_key<author_key>(data) = "";
					boost::fusion::at_key<home_key>(data) = detail::to_qstring(find_url_);
					boost::fusion::at_key<url_key>(data) = find_url_;
					boost::fusion::at_key<tags_key>(data) = tags_value(QStringList());
					boost::fusion::at_key<dir_key>(data) = boost::fusion::at_key<id_key>(data);
					boost::fusion::at_key<file_key>(data) = "text.html";

					boost::intrusive_ptr<i_t> new_item = i_t::dangle_instance(data); // boost::intrusive_ptr<TreeItem>(new
													 // TreeItem(nullptr, data));

					// if(record.isLite())
					new_item->to_fat();
					new_item->text_to_fat("");

					new_item->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));

					// Пока что принята концепция, что файлы нельзя приаттачить в момент
					// создания записи
					// Запись должна быть создана, потом можно аттачить файлы.
					// Это ограничение для "ленивого" программинга, но пока так
					// record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory,
					// "*.bin") );

					// Временная директория с картинками и приаттаченными файлами удаляется
					DiskHelper::remove_directory(directory);
					if (new_item->is_lite())
						new_item->to_fat();
					_result = new_item; // assert(_item.get() == item.get());
				} else {
					auto page = view->page();
					if (page) {
						auto binder = page->binder();
						if (binder) {
							auto host = binder->host();
							if (host) {
								host->field<url_key>(url_value(find_url_));
								host->field<home_key>(home_value(find_url_));
								_result = host;
							}
						}
					}
				}
				assert(equal_(_result));
			} else {
				assert(equal_(it_in_web)); // assert(_result->url<url_type>() == url_type()(_find_url));

				_result = it_in_web;
				if (_result->is_lite()) _result->to_fat();
				if (_result->field<id_key>() == id_value("")) _result->field<id_key>(detail::to_qstring(_result->field<dir_key>()).length() > 0 ? id_value(detail::to_qstring(_result->field<dir_key>())) : get_unical_id());
				assert(gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) -> bool { return url_equal(detail::to_string(b->host()->field<home_key>()), detail::to_string(_result->field<home_key>())); }) || _result->field<url_key>() == web::Browser::_defaulthome);
				if (_result->field<url_key>() == web::Browser::_defaulthome && find_url_ != web::Browser::_defaulthome)
					_result->field<url_key>(find_url_);
				assert(equal_(_result));
			}
		}
		assert(_result != _tree_view->know_model_board()->root_item());

		_result = _tree_view_insert_strategy_impl(
		    _result, // this,
		    [&](boost::intrusive_ptr<const Linker> il) -> bool {
			    return url_equal(_result->field<url_key>(), il->host()->field<url_key>()) &&
				il->host()->id() == _result->id() &&
				il == _result->linker() &&
				il->host_parent() == _result->parent();
		    },
		    tree_view_insert_strategy_,
		    item_is_brand_new,
		    find_url_, equal_);

		assert(_result);
		assert(equal_(_result));
		if (_result->is_lite())
			_result->to_fat();
		assert(_result != _tree_view->know_model_board()->root_item());
		if (_result->field<dir_key>() == dir_value("")) _result->field<dir_key>(dir_value(detail::to_qstring(_result->field<id_key>())));
		if (_result->field<file_key>() == file_value("")) _result->field<file_key>(file_value("text.html"));
		assert(equal_(_result) || _result->field<url_key>() == web::Browser::_defaulthome || find_url_ == web::Browser::_defaulthome);
		if (_result->field<url_key>() == web::Browser::_defaulthome && find_url_ != web::Browser::_defaulthome)
			_result->field<url_key>(find_url_);
		if (_result->field<home_key>() == home_value("")) _result->field<home_key>(home_value(detail::to_qstring(_result->field<url_key>()))); // for history reason
																		       // assert(_result->binder());
	}
	return _result;
}

boost::intrusive_ptr<i_t>
TreeIndex::url_bind_browser(boost::intrusive_ptr<real_url_t<url_value>> real_find_url_, const insert_strategy& view_insert_strategy_, equal_url equal_url_, web::Browser* recommend_browser_)
{
	//	// boost::intrusive_ptr<TreeItem>   _host = _host;
	//	auto find_url_ = real_find_url_->value();
	//	// equal_url _equal = [&](boost::intrusive_ptr<const TreeItem> it_){return
	//	// url_equal(it_->field<home_type>().toStdString(),
	//	// _find_url.toString().toStdString()) ||
	//	// url_equal(it_->field<url_type>().toStdString(),
	//	// _find_url.toString().toStdString());};
	boost::intrusive_ptr<i_t> result(nullptr);
	//	auto _tree_view = gl_paras->tree_screen()->view(); // static_cast<tv_t *>(static_cast<QObject *>(_current_model())->parent());
	// auto				_current_model	= [&] {return
	// _tree_view->source_model();};
	//	auto _host = this->_host; //_tree_view->current_item();
	web::WebView* view = nullptr;
	auto sibling_binder = _host ? _host->binder() : nullptr;
	if (sibling_binder) {
		auto host_ = sibling_binder->host();
		auto _equal = [&](boost::intrusive_ptr<const Binder> b) {
			return b->host() == host_ && b == sibling_binder && host_->parent() == b->host()->parent() && b->host()->id() == host_->id();
		};
		view = recommend_browser_ ? recommend_browser_->find(_equal) : gl_paras->main_window()->find(_equal);
	}
	auto host_browser_ = recommend_browser_ ? recommend_browser_ : view ? view->page()->browser() : gl_paras->main_window()->browser(real_find_url_);
	assert(host_browser_);
	auto _current_item = host_browser_->tab_widget()->current_item(); // _tree_view->current_item();

	auto target_ = TreeIndex::url_require_item_from_tree(real_find_url_, view_insert_strategy_, equal_url_);
	if (target_) {
		// assert(tab_brother != it);

		// boost::intrusive_ptr<RecordIndex> record_index =
		// RecordIndex::instance([&] {return
		// browser->record_screen()->record_ctrl()->source_model();},
		// tab_brother, it);	// tab_brother ?
		// tab_brother->binder()->page()->record_ctrl()->source_model() :   //
		// tab_brother does not guarantee the browser exists
		// assert(record_index);
		auto record_index = RecordIndex::instance([&] {
			RecordModel* rm = nullptr;
			auto _rctrl = host_browser_->record_screen()->record_ctrl();
			if (_rctrl) rm = _rctrl->source_model();
			return rm;
		},
		    target_, _current_item);

		result = record_index->bind(false);
		assert(result);
		if (result) {
			assert(result->binder());
			assert(result->page());
		}
	}
	assert(result);
	return result;
}

boost::intrusive_ptr<i_t>
TreeIndex::url_activate(boost::intrusive_ptr<real_url_t<url_value>> real_find_url_, const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> sibling_host_, const insert_strategy& view_insert_strategy_, equal_url equal_, web::Browser* brwoser_) noexcept
{
	//	auto find_url_ = real_find_url_->value();
	//	assert(to_be_url(detail::to_qstring(find_url_)) != QUrl());
	boost::intrusive_ptr<i_t> result(nullptr);
	auto sibling_tree_index = TreeIndex::item_require_treeindex(current_model_, sibling_host_);
	result = sibling_tree_index->url_bind_browser(real_find_url_, view_insert_strategy_, equal_, brwoser_);
	if (result) {
		if (brwoser_)
			result->activate(std::bind(&web::Browser::find, brwoser_, std::placeholders::_1));
		else
			result->activate(std::bind(&wn_t::find, gl_paras->main_window(), std::placeholders::_1));
	}
	assert(result);
	return result;
}

TreeLevel::TreeLevel(boost::intrusive_ptr<TreeIndex> tree_index, boost::intrusive_ptr<i_t> to_be_merged)
    : _tree_index(tree_index)
    , _to_be_operated(to_be_merged)
    , _tree_view(static_cast<tv_t*>(static_cast<QObject*>(_tree_index->current_model()())->parent()))
{
}

boost::intrusive_ptr<TreeIndex> TreeLevel::tree_index()
{
	return _tree_index;
}

boost::intrusive_ptr<i_t> TreeLevel::to_be_operated()
{
	return _to_be_operated;
}

boost::intrusive_ptr<i_t> TreeLevel::merge()
{
	return (_to_be_operated == _tree_index->host()) ? _to_be_operated : _tree_view->merge(this);
}

boost::intrusive_ptr<i_t> TreeLevel::move()
{
	return (_to_be_operated == _tree_index->host()) ? _to_be_operated : static_cast<tkm_t*>(_tree_view->source_model())->move(this);
}

boost::intrusive_ptr<TreeLevel>
TreeLevel::instance(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<i_t> _to_be_operated)
{
	boost::intrusive_ptr<TreeLevel> result(nullptr);
	auto current_model_ = _tree_index->current_model();
	//	auto tree_view_ = static_cast<tv_t*>(static_cast<QObject*>(current_model_())->parent());
	auto host = _tree_index->host();
	if (_to_be_operated->is_ancestor_of(host)) {
		auto parent_ = _to_be_operated->parent();
		if (parent_) {
			auto host_new = TreeLevel::instance(TreeIndex::item_require_treeindex(current_model_, parent_), host)->move(); // static_cast<tkm_t *>(current_model())
			assert(host_new == host);
			result = //TreeLevel::instance(TreeIndex::item_require_treeindex(current_model_, host), _to_be_operated); //
			    new TreeLevel(TreeIndex::item_require_treeindex(current_model_, host), _to_be_operated);
		} else {
			auto host_new = TreeLevel::instance(TreeIndex::item_require_treeindex(current_model_, _to_be_operated), host)->move(); // static_cast<tkm_t *>(current_model())
			assert(host_new == host);
			result = // TreeLevel::instance(TreeIndex::item_require_treeindex(current_model_, _to_be_operated), host);
			    new TreeLevel(TreeIndex::item_require_treeindex(current_model_, _to_be_operated), host);
		}
	} else {
		result = new TreeLevel(_tree_index, _to_be_operated);
	}
	return result;
}

boost::intrusive_ptr<const i_t> TreeIndex::is_ancestor_of(
    boost::intrusive_ptr<const i_t> target,
    boost::intrusive_ptr<const i_t> reference)
{ // const std::function<tkm_t
	// *()> &current_model_, const
	// std::function<bool
	// (boost::intrusive_ptr<const
	// TreeItem>)> &_equal
	boost::intrusive_ptr<const i_t> result(nullptr);
	////    TreeModel::item(const std::function<bool (boost::intrusive_ptr<const
	///TreeItem>)> &_equal) const {
	// std::function<boost::intrusive_ptr<const
	// TreeItem>(boost::intrusive_ptr<const TreeItem>, const std::function<bool
	// (boost::intrusive_ptr<const TreeItem>)> &, int)>
	// item_recurse	//
	// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
	// item, QString name, int mode);
	// = [&](boost::intrusive_ptr<const TreeItem> _it
	// , const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal
	// , int mode
	// ){
	// static boost::intrusive_ptr<const TreeItem> find_item;
	// if(mode == 0){
	// find_item = nullptr;
	// return find_item;	// nullptr;
	// }
	// if(find_item)return find_item;
	// if(_equal(_it)){
	// find_item = _it;
	// return find_item;
	// }else{
	// for(int i = 0; i < _it->count_direct(); i
	// ++)item_recurse(_it->child_direct(i), _equal, 1);
	// return find_item;
	// }
	// };
	//// Инициализация поиска
	// item_recurse(this, _equal, 0);	// QUrl()
	//// Запуск поиска и возврат результата
	// result = item_recurse(this, _equal, 1);
	// return result == this ? nullptr : result;	// _find_url
	// }
	// if(static_cast<QModelIndex>(current_model_()->index(target)).isValid() &&
	// static_cast<QModelIndex>(current_model_()->index(reference)).isValid()){
	//
	if (reference && target) {
		if (reference->path_list<id_key>().contains(target->field<id_key>()) && reference != target) result = reference;
	}
	// }

	return result;
}
