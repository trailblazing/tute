#include "tree_index.hxx"

#include "main.h"
#include "libraries/clipboard_records.h"
#include "libraries/global_parameters.h"
#include "models/tree/binder.hxx"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/tree/tree_know_model.h"
#include "views/tree/tree_view.h"
#include "models/record_table/items_flat.h"
#include "models/tree/tree_item.h"
#include "models/record_table/record_model.h"
#include "views/main_window/main_window.h"
#include "views/record_table/record_screen.h"
#include "views/record/meta_editor.h"
#include "controllers/record_table/record_controller.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"
#include "views/browser/entrance.h"
#include "models/tree/tree_item.h"
#include "libraries/crypt/password.h"
#include "libraries/clipboard_branch.h"


// boost::intrusive_ptr<TreeIndex> TreeIndex::instance(const std::function<km_t *()> &current_model, boost::intrusive_ptr<TreeItem> host_parent, int sibling_order){
//    return new TreeIndex(current_model, host_parent, sibling_order);
// }
boost::intrusive_ptr<TreeIndex> TreeIndex::instance(const std::function<tkm_t *()> &current_model_, boost::intrusive_ptr<TreeItem> host_){
    boost::intrusive_ptr<TreeIndex> result(nullptr);
    if(! host_)throw std::runtime_error("! _host");
    auto	host_parent_	= host_->parent();
    auto	absolute_model	= [&] {return globalparameters.tree_screen()->view()->know_model_board();};
    auto	absolute_root	= absolute_model()->root_item();
    auto	current_root	= [&] {return static_cast<tkm_t *>(current_model_())->root_item();};
    auto	tree_view_	= static_cast<tv_t *>(static_cast<QObject *>(current_model_())->parent());
	//
    auto parent_is_valid = [&](boost::intrusive_ptr<TreeItem> host_parent) -> bool {
	    bool	result		= false;
	    auto	absolute_root	= globalparameters.tree_screen()->view()->know_model_board()->root_item();
	    auto	current_root	= [&](){return static_cast<tkm_t *>(current_model_())->root_item();};
	    if(! host_parent)result = true;
	    else if(TreeIndex::is_ancestor_of(host_parent, current_root())){
		auto new_root = (host_parent == absolute_root) ? host_parent : host_parent->parent();

		auto tv = dynamic_cast<tv_t *>(static_cast<QObject *>(current_model_())->parent());
		assert(tv);
		tv->intercept(new_root);// static_cast<km_t *>(current_model())->intercept(new_root);
		if(host_parent == absolute_root)result = true;
		else if(host_parent == current_root())result = true;
		else{	// if(!current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == parent_item->id();}) && parent_item->id() != "")
		    result = (static_cast<QModelIndex>(current_model_()->index(host_parent)).isValid())
			&& (current_model_()->item([=](boost::intrusive_ptr<const TreeItem> it){return it->id() == host_parent->id();}));
		}
		// _current_index = _current_model()->index(current_item());
		// assert(_current_index.isValid());
	    }else if(host_parent == current_root())result = true;				// assert(_current_index.isValid());
	    else{
		// assert(_current_index.isValid());
		result = (static_cast<QModelIndex>(current_model_()->index(host_parent)).isValid())
		    && (current_model_()->item([=](boost::intrusive_ptr<const TreeItem> it){return it->id() == host_parent->id();}));
	    }
		// assert(_current_index.isValid());
	    return result;
	};
    auto host_index_valid = [&] {
	index_tree index;
	if(host_ == absolute_root){
//		  throw std::runtime_error("operating on absolute item");
////		  boost::intrusive_ptr<TreeItem> default_child;
////		  if(_host->count_direct() > 0){
////		      default_child = _host->child_direct(0);
////		  }else{
////		      default_child = new TreeItem(_host);
////		  }
////		  index = absolute_model()->fake_index(default_child);
	}else if(host_ == current_root() && host_ != absolute_root){
//	    auto tree_view_ = dynamic_cast<tv_t *>(static_cast<QObject *>(current_model_())->parent());
	    assert(tree_view_);
	    if(host_parent_)tree_view_->intercept(host_parent_);	// static_cast<km_t *>(current_model())->intercept(_host->parent());
	    index = static_cast<tkm_t *>(current_model_())->index(host_);
	}else index = static_cast<tkm_t *>(current_model_())->index(host_);
	if(! static_cast<QModelIndex>(index).isValid()){
	    tree_view_->intercept(absolute_root);
	    index = static_cast<tkm_t *>(current_model_())->index(host_);
	}
//	assert(static_cast<QModelIndex>(index).isValid() || (host_ == absolute_root));

	return index;
    };

    index_tree	host_index_;
    int		sibling_order_ = - 1;
//    try{
//	if(! _host_parent){throw std::runtime_error(formatter() << "! _host_parent");}
    if(! parent_is_valid(host_parent_))throw std::runtime_error(formatter() << "! parent_is_valid");			// assert(current_parent_valid);
    host_index_ = host_index_valid();
    if(! static_cast<QModelIndex>(host_index_).isValid()){
	auto target_url	= QUrl(host_->field<home_type>() != "" ? host_->field<home_type>() : host_->field<url_type>());
	if(target_url == QUrl()){
	    target_url = browser::Browser::_defaulthome;
	    host_->field<home_type>(browser::Browser::_defaulthome);
	    host_->field<url_type>(browser::Browser::_defaulthome);
	}
	//	result = new TreeIndex(current_model_, host_, host_index_, sibling_order_);
	host_ = TreeIndex::instance(current_model_, tree_view_->current_item())->register_index(target_url
											       , std::bind(&tv_t::move, tree_view_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)											// std::placeholders::_1
											       , [&](boost::intrusive_ptr<const TreeItem> it_) -> bool {return url_equal(it_->field<home_type>().toStdString(), target_url.toString().toStdString()) || url_equal(it_->field<url_type>().toStdString(), target_url.toString().toStdString());}
		);
	host_index_ = static_cast<tkm_t *>(current_model_())->index(host_);
	assert(static_cast<QModelIndex>(host_index_).isValid() || (host_ == absolute_root));		// throw std::runtime_error(formatter() << "! host_index_valid");
    }
    if(static_cast<QModelIndex>(host_index_).isValid()){
	sibling_order_ = host_->parent() ? host_->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il){return il == host_->linker() && il->host() == host_ && il->host_parent() == host_->parent();}) : 0;
	assert(sibling_order_ >= 0);
	if(host_parent_)if(! host_parent_->contains_direct(host_))throw std::runtime_error("_host_parent does not contains _host");
	if(! static_cast<QModelIndex>(static_cast<tkm_t *>(current_model_())->index(host_)).isValid() && host_ != static_cast<tkm_t *>(current_model_())->root_item())throw std::runtime_error("! current_model()->index(_host).isValid()");
//	if(_sibling_order < 0){throw std::runtime_error(formatter() << "sibling_order < 0");}
//	auto count_direct = _host_parent->count_direct();
//	count_direct = (count_direct == 0) ? 1 : count_direct;
//	// assert(sibling_order < count_direct);
//	if(_sibling_order >= count_direct){throw std::runtime_error(formatter() << "sibling_order >= count_direct");}
//	_host_parent_index = _current_model()->index(_host_parent);
//	auto link = _host->linker();
	if(! host_->linker())throw std::runtime_error(formatter() << "! host_linker");		//	_host = link->host();
//	if(! _host){throw std::runtime_error(formatter() << "! _host");}
//	_host_index = _current_model()->index(_host);
//	if(! _host_index.isValid()){throw std::runtime_error(formatter() << "! _host_index.isValid()");}
	// assert(_current_index.isValid());
//    }catch(std::exception &e){
////	qDebug() << e.what();
//	throw std::runtime_error(e.what());
//    }
	result = new TreeIndex(current_model_, host_, host_index_, sibling_order_);
    }
    assert(static_cast<QModelIndex>(host_index_).isValid() || (host_ == absolute_root));

    return result;	// host_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il){return il->host() == host && il == host->linker() && host->parent() == host_parent;})
}

TreeIndex::TreeIndex(const std::function<tkm_t *()> &current_model_, boost::intrusive_ptr<TreeItem> host_, const index_tree &host_index_, const int sibling_order_)
    : _current_model(current_model_)
      , _host(host_)
      , _sibling_order(sibling_order_)
      , _host_index(host_index_)
{}

//
// TreeIndex::TreeIndex(const std::function<km_t *()> &current_model, boost::intrusive_ptr<TreeItem> host_parent_, int sibling_order)
//    : _current_model(current_model), _host_parent(host_parent_), _host(_host_parent ? _host_parent->child_direct(sibling_order) : nullptr), _sibling_order(sibling_order){	// , _current_index(_current_model()->index(_parent->item_direct(_sibling_order)))
//    bool parent_is_valid = [&](){
//	    bool result = false;
//	    auto absolute_root = globalparameters.tree_screen()->view()->know_model_board()->root_item();
//	    auto current_root = [&](){return static_cast<km_t *>(current_model())->root_item();};
//	    if(host_parent_->is_ancestor_of([&](boost::intrusive_ptr<const TreeItem> it){return it == current_root();})){
//		auto new_root = (host_parent_ == absolute_root) ? host_parent_ : host_parent_->parent();
//		static_cast<km_t *>(current_model())->intercept(new_root);
//		if(host_parent_ == absolute_root){
//		    result = true;
//		}else{	// if(!current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == parent_item->id();}) && parent_item->id() != "")
//		    result = (static_cast<QModelIndex>(current_model()->index(host_parent_)).isValid())
//			&& (current_model()->item([=](boost::intrusive_ptr<const TreeItem> it){return it->id() == host_parent_->id();}));
//		}
//		// _current_index = _current_model()->index(current_item());
//		// assert(_current_index.isValid());
//	    }else if(host_parent_ == current_root()){
//		// assert(_current_index.isValid());
//		result = true;
//	    }else{
//		// assert(_current_index.isValid());
//		result = (static_cast<QModelIndex>(current_model()->index(host_parent_)).isValid())
//		    && (current_model()->item([=](boost::intrusive_ptr<const TreeItem> it){return it->id() == host_parent_->id();}));
//	    }
//		// assert(_current_index.isValid());
//	    return result;
//	} ();

//    try{
//	if(! _host_parent){throw std::runtime_error(formatter() << "! _host_parent");}
//	if(! parent_is_valid){throw std::runtime_error(formatter() << "! parent_is_valid");}		// assert(current_parent_valid);
//	// assert(sibling_order >= 0);
//	if(sibling_order < 0){throw std::runtime_error(formatter() << "sibling_order < 0");}
//	auto count_direct = _host_parent->count_direct();
//	count_direct = (count_direct == 0) ? 1 : count_direct;
//	// assert(sibling_order < count_direct);
//	if(sibling_order >= count_direct){throw std::runtime_error(formatter() << "sibling_order >= count_direct");}
//	_host_parent_index = _current_model()->index(_host_parent);


//	auto link = _host_parent->linker_direct(sibling_order);
//	if(! link){throw std::runtime_error(formatter() << "! host_linker");}
//	_host = link->host();
//	if(! _host){throw std::runtime_error(formatter() << "! _host");}
//	_host_index = _current_model()->index(_host);
//	if(! _host_index.isValid()){throw std::runtime_error(formatter() << "! _host_index.isValid()");}
//	// assert(_current_index.isValid());
//    }catch(std::exception &e){
//	qDebug() << e.what();
//	throw e;
//    }
// }
std::function<tkm_t *()> TreeIndex::current_model() const {return _current_model;}

// boost::intrusive_ptr<TreeItem> TreeIndex::host_parent() const {return _host_parent;}

boost::intrusive_ptr<TreeItem> TreeIndex::host() const {return _host;}	// {return _host_parent ? _host_parent->child_direct(_sibling_order) : nullptr;}

// index_tree TreeIndex::host_parent_index() const {return _host_parent_index;}		// _current_model()->index(_parent);

index_tree TreeIndex::host_index() const {return _host_index;}		// _current_model()->index(current_item())

int TreeIndex::sibling_order() const {return _sibling_order;}

// boost::intrusive_ptr<TreeItem> item_request_from_tree_impl(const QUrl &_url);
boost::intrusive_ptr<TreeItem> TreeIndex::register_index(const QUrl             &find_url_
							, const insert_strategy	&tree_view_insert_strategy_
							, equal_url equal_){
    boost::intrusive_ptr<TreeItem> _result(nullptr);
    if(find_url_ != QUrl())_result = TreeIndex::item_register(find_url_, tree_view_insert_strategy_, equal_);
    return _result;
}

boost::intrusive_ptr<TreeItem> TreeIndex::item_register(const QUrl              &find_url_
						       , const insert_strategy  &tree_view_insert_strategy_
						       , equal_url equal_){
    boost::intrusive_ptr<TreeItem>	_result(nullptr);		// =  _know_model_board->root_item();
    if(find_url_ != QUrl()){
	auto	_tree_view			= globalparameters.tree_screen()->view();// static_cast<tv_t *>(static_cast<QObject *>(_current_model())->parent());
	auto	_current_model			= [&] {return _tree_view->source_model();};
	auto	_current_item			= _tree_view->current_item();
	auto	_treeindex			= TreeIndex::instance(_current_model, _current_item);
	auto	_tree_view_insert_strategy_boot	= [&](boost::intrusive_ptr<TreeItem>                               _result	// boost::intrusive_ptr<TreeIndex>_treeindex,
						     , const substitute_condition                                   &_substitute_condition
						     , const insert_strategy                                        &_tree_view_insert_strategy_impl
						     , const bool _item_is_brand_new
						     , const QUrl                                                   &_find_url
						     , const std::function<bool (boost::intrusive_ptr<TreeItem>)>   &_check_url
		) -> boost::intrusive_ptr<TreeItem> {					// KnowView::
//		auto	_current_model	= _treeindex->current_model();
//		auto _current_item = _treeindex->host();
		assert(_result != _tree_view->know_model_board()->root_item());
		if(_item_is_brand_new || ! _current_model()->item([&](boost::intrusive_ptr<const TreeItem> it){return it->id() == _result->id();})){
		    assert(_check_url(_result));
		    _result = _tree_view_insert_strategy_impl(_treeindex, _result, _substitute_condition, true);	// it->insert_new_item(it->current_count() - 1, _result);
		    assert(_result);
		    assert(_check_url(_result) || _result->field<url_type>() == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);
		    if(_result->field<url_type>() == browser::Browser::_defaulthome && _find_url.toString() != browser::Browser::_defaulthome)_result->field<url_type>(_find_url.toString());		// item->field("url")
		    _tree_view->synchronized(false);
		    assert(equal_(_result));
		}else if(_current_item != _result){
		    if(TreeIndex::is_ancestor_of(_result, _current_item)){
			auto host_new = TreeLevel::instance(TreeIndex::instance(_current_model, _result->parent()), _current_item)->move();	// static_cast<tkm_t *>(current_model())
			assert(host_new == _current_item);
			_current_item = host_new;
//			_tree_screen->session_root_id(_result->id());
			assert(_result != _tree_view->know_model_board()->root_item());
			if(! static_cast<QModelIndex>(_tree_view->source_model()->index(_result)).isValid())_tree_view->cursor_focus(_result);
			assert(_result != _tree_view->know_model_board()->root_item());
			assert(_result);
			assert(static_cast<QModelIndex>(_tree_view->source_model()->index(_result)).isValid());
			_tree_view->synchronized(false);
			assert(equal_(_result));
		    }else if(_tree_view->session_root_auto() != _result->parent()){	// if(session_root_item->is_ancestor_of(_result) && session_root_item != _result->parent())
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



	boost::intrusive_ptr<TreeItem> in_tree = _tree_view->know_model_board()->child(equal_);

	bool item_is_brand_new = false;


	// if(browser_pages) {
	auto browser_view = globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const ::Binder> b) -> bool {return equal_(b->host());});

	boost::intrusive_ptr<TreeItem> in_browser;
	if(browser_view){
	    in_browser = browser_view->page()->binder()->host();
	    assert(equal_(in_browser));		// assert(_result->url<url_type>() == url_type()(_find_url));
	}
	if(in_tree && in_tree != _tree_view->know_model_board()->root_item()){
	    if(! in_browser){
		assert(equal_(in_tree));		// assert(_source_item->url<url_type>() == url_type()(_find_url));
		if(in_tree->is_lite())in_tree->to_fat();
		_result = in_tree;
		assert(equal_(_result));
	    }else{
		assert(equal_(in_browser));			// assert(_result->url<url_type>() == url_type()(_find_url));
		assert(equal_(in_tree));			// assert(_source_item->url<url_type>() == url_type()(_find_url));
		boost::intrusive_ptr<TreeItem> result_(in_browser);
		if(in_browser != in_tree)result_ = TreeLevel::instance(TreeIndex::instance(_current_model, in_browser), in_tree)->merge();
		_result = result_;
		if(_result->is_lite())_result->to_fat();
		if(_result->field<id_type>() == "")_result->field<id_type>(_result->field<dir_type>().length() > 0 ? _result->field<dir_type>() : get_unical_id());
		assert(  globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const ::Binder> b) -> bool {return url_equal((b->host()->field<home_type>()).toStdString(), _result->field<home_type>().toStdString());})
		      || _result->field<url_type>() == browser::Browser::_defaulthome);
		assert(equal_(_result));
	    }
	    assert(! _result->is_lite());
	}else{
	    item_is_brand_new = true;
	    if(! in_browser){
		// record.generator(generator);


		// Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
		QString directory = DiskHelper::create_temp_directory();		//

		QMap<QString, QString> data;
		data["id"]	= get_unical_id();
		data["pin"]	= _string_from_check_state[Qt::Unchecked];
		data["name"]	= "";
		data["author"]	= "";
		data["home"]	= find_url_.toString();
		data["url"]	= find_url_.toString();
		data["tags"]	= "";
		data["dir"]	= data["id"];
		data["file"]	= "text.html";

		boost::intrusive_ptr<TreeItem> new_item = TreeItem::dangle_instance(data);		// boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

		// if(record.isLite())
		new_item->to_fat();
		new_item->text_to_fat("");

		new_item->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));


		// Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
		// Запись должна быть создана, потом можно аттачить файлы.
		// Это ограничение для "ленивого" программинга, но пока так
		// record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

		// Временная директория с картинками и приаттаченными файлами удаляется
		DiskHelper::remove_directory(directory);
		if(new_item->is_lite())new_item->to_fat();
		_result = new_item;		// assert(_item.get() == item.get());
		assert(equal_(_result));
	    }else{
		assert(equal_(in_browser));			// assert(_result->url<url_type>() == url_type()(_find_url));

		_result = in_browser;
		if(_result->is_lite())_result->to_fat();
		if(_result->field<id_type>() == "")_result->field<id_type>(_result->field<dir_type>().length() > 0 ? _result->field<dir_type>() : get_unical_id());
		assert(  globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const ::Binder> b) -> bool {return url_equal((b->host()->field<home_type>()).toStdString(), _result->field<home_type>().toStdString());})
		      || _result->field<url_type>() == browser::Browser::_defaulthome);
		if(_result->field<url_type>() == browser::Browser::_defaulthome && find_url_.toString() != browser::Browser::_defaulthome)_result->field<url_type>(find_url_.toString());
		assert(equal_(_result));
	    }
	}
	assert(_result != _tree_view->know_model_board()->root_item());



	_result = _tree_view_insert_strategy_boot(_result	// this,
						 , [&](boost::intrusive_ptr<const Linker> il) -> bool {return url_equal(_result->field<url_type>().toStdString(), il->host()->field<url_type>().toStdString()) && il->host()->id() == _result->id() && il == _result->linker() && il->host_parent() == _result->parent();}
						 , tree_view_insert_strategy_
						 , item_is_brand_new
						 , find_url_
						 , equal_);


	assert(_result);
	assert(equal_(_result));
	if(_result->is_lite())_result->to_fat();
	assert(_result != _tree_view->know_model_board()->root_item());
	if(_result->field<dir_type>() == "")_result->field<dir_type>(_result->field<id_type>());
	if(_result->field<file_type>() == "")_result->field<file_type>("text.html");
	assert(equal_(_result) || _result->field<url_type>() == browser::Browser::_defaulthome || find_url_.toString() == browser::Browser::_defaulthome);
	if(_result->field<url_type>() == browser::Browser::_defaulthome && find_url_.toString() != browser::Browser::_defaulthome)_result->field<url_type>(find_url_.toString());
	if(_result->field<home_type>() == "")_result->field<home_type>(_result->field<url_type>());		// for history reason
    }
    return _result;
}

// boost::intrusive_ptr<TreeItem> KnowView::item_register(boost::intrusive_ptr<TreeItem> target
// , const KnowView::paste_strategy &_view_paste_strategy
// , equal_t _equal
// )
// {

// boost::intrusive_ptr<TreeItem> _result(nullptr);    // =  _know_model_board->root_item();

// auto find_url = QUrl(target->field("url"));

// auto check_euqal = [&](boost::intrusive_ptr<const TreeItem> it) {return _equal(it, target);};

// TreeScreen *_tree_screen    = static_cast<TreeScreen *>(parent()); //  = globalparameters.tree_screen();
////    auto _know_model_root = tree_screen->know_root();
// auto tree_view = _tree_screen->tree_view();
// auto know_model_board      = tree_view->know_model_board();
////        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model; // std::bind(_source_model_func, _tree_screen->tree_view());
// auto _source_model = [&]() {return tree_view->source_model();};


////        auto session_root_item = [&]() {

////            auto internal_session_root_item  = [&]() {return view_source_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();});};

////            while(!internal_session_root_item() && view_source_model()->root_item() != know_model_board->root_item()) {
////                tree_screen->cursor_follow_up_one_level();
////                tree_screen->session_root_id(view_source_model()->root_item()->id());
////            }

////            if(!know_model_board->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();})) {
////                tree_screen->session_root_id(tree_screen->tree_view()->current_item()->id());
////                tree_screen->know_model_save();
////            }
////            assert(internal_session_root_item());
////            return internal_session_root_item(); // _session_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();});

////        };   // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());


////        auto _current_view_model    = _tree_screen->tree_view()->source_model();

// if(target->is_lite())target->to_fat();

////    item->record_binder(_record_binder);
////    //    item->activator(activator);

////    //    item->is_registered_to_record_controller_and_tabmanager(false);

// if(!target->binder()) {

////        if(item->is_lite())item->to_fat();

////        item->binder(generator);
////        item->activator(activator);
////        item->is_registered_to_record_controller(true);



////    TableController *_record_controller = globalparameters.table_screen()->table_controller();
////    assert(_record_controller);



////            QUrl find_url = QUrl(item->field("url"));

////    boost::intrusive_ptr<TreeItem> _source_root_item = tree_screen->know_branch()->item(TreeModel::delegater(find_url));        // on know_root semantic    // item won't work, for it is not inside _know_model_root if it is come from _know_model_branch
// boost::intrusive_ptr<TreeItem> in_tree = know_model_board->item(check_euqal);

////    if(_source_root_item && !_source_item) {
////        auto result = tree_screen->cut_from_root(_source_root_item);

////        if(result)_source_item = tree_screen->paste_to_branch(result, _know_model_branch);

////        assert(result);
////        assert(_source_item);
////        assert((_source_item == result) && (result == _source_root_item));
////    }

// bool item_is_brand_new = false;
////    if(_record_controller) {
////    auto browser_pages = this->_source_model->browser_pages();
////    assert(browser_pages);

////    if(_source_model->count() > 0) {
// auto view = globalparameters.entrance()->find(check_euqal);

// boost::intrusive_ptr<TreeItem> in_browser;

// if(view) {
// in_browser = view->page()->binder()->item();
// assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
//// assert(_result->fragment() == item->fragment());
// }

// if(in_tree && in_tree != know_model_board->root_item()) {

// if(!in_browser) {
// assert(check_euqal(in_tree));   // assert(_source_item->url() == item->url());
//// assert(item->fragment() == _source_item->fragment());

// if(target != in_tree) {
// _result = in_tree; // assert(_item.get() == _source_item.get());
//// _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(item, _source_item);
// } else {
// _result = in_tree;
// }

////                //                record->binder(generator);
////                //                record->activator(activator);
// if(_result->is_lite())_result->to_fat();

////                _source_item->binder(generator);
////                _source_item->activator(activator);

////                //            _item = register_item_to_browser_source_model(_source_item);
////                _source_item->is_registered_to_record_controller(true);
////                // _source_item->self_bind();
////                //                assert(_record);

////                //                _record->active_immediately(active_immediately);
////                //                _record->generator(generator);



// } else {
// assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
// assert(_equal(in_browser, in_tree));   // assert(_result->url() == _source_item->url());
//// assert(_result->fragment() == _source_item->fragment());

////                if(in_browser != in_tree) {
////                    _result = in_tree; // assert(_item.get() == _source_item.get());
////                    // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, _source_item);
////                }

// _result = in_tree;
// assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);
// }

// assert(!_result->is_lite());
////            assert(_result->is_registered_to_browser());

// } else {

// item_is_brand_new = true;

// if(!in_browser) {


////                if(item->is_lite())item->to_fat();

// if(target->field("id") == "")target->field("id", get_unical_id());

////                item->binder(generator);
////                item->activator(activator);

////                //            _item = register_item_to_browser_source_model(item);
////                item->is_registered_to_record_controller(true);
////                // item->self_bind();
// _result = target; // assert(_item.get() == item.get());
// } else {
// assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
//// assert(_result->fragment() == item->fragment());

// if(in_browser != target) {
// _result = in_browser;   // target; // assert(_item.get() == _source_item.get());
//// _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, item);
// }

// if(_result->is_lite())_result->to_fat();

// if(_result->field("id") == "")_result->field("id", get_unical_id());

// assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

// if(_result->field("url") == browser::Browser::_defaulthome &&  target->field("url") != browser::Browser::_defaulthome) {
// _result->field("url",  target->field("url"));
// }

////                _item->binder(generator);
////                _item->activator(activator);
////                _item->is_registered_to_record_controller(true);
////                // _item->self_bind();
// }


// }

////            {
////                //                    auto _index = _tree_screen->tree_view()->current_index();

////                // if(idx.isValid()) {
////                _current_view_model = _tree_screen->tree_view()->source_model();

////                auto session_root_item = _current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();}); // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
////                auto session_root_index = _tree_screen->tree_view()->source_model()->index(session_root_item); //current_index();
////                assert(session_root_index.isValid());
////                assert(session_root_item);
////                assert(_current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == session_root_item->id();}));

////                //        if(_item->is_lite())_item->to_fat();

////                if(item_is_brand_new) {
////                    assert(_check_url(_result));   // assert(_result->url() == item->url());
////                    // assert(_result->fragment() == item->fragment());
////                    // int pos
////                    _result = _view_paste_strategy(_current_view_model, session_root_index, _result); // it->insert_new_item(it->current_count() - 1, _result);

////                    assert(_check_url(_result) || _result->field("url") == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);

////                    if(_result->field("url") == browser::Browser::_defaulthome) {
////                        _result->field("url"
////                                       , _find_url.toString()  // item->field("url")
////                                      );
////                    }

////                    _tree_screen->synchronized(false);
////                    // assert(_result == it->child(pos));
////                } else if(session_root_item != _result) {
////                    if(_result->is_ancestor_of(session_root_item)) {
////                        if(_result != session_root_item->parent()) {
////                            _result = _tree_screen->view_follow_direct(_result);
////                        }

////                        _tree_screen->session_root(_result->id());
////                        _tree_screen->synchronized(false);
////                    } else if(session_root_item != _result->parent()) { // if(session_root_item->is_ancestor_of(_result) && session_root_item != _result->parent())
////                        _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
////                        _tree_screen->synchronized(false);
////                        // }
////                        // else {
////                        // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
////                    }

////                    // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);

////                }


////                _tree_screen->knowtree_save();
////                // }
////            }



// boost::intrusive_ptr<TreeIndex> tree_index = [&] {
// boost::intrusive_ptr<TreeIndex> tree_index;

// try {
// auto _it = tree_view->current_item();
// tree_index = new TreeIndex(_source_model
// , _it->parent()   // tree_screen->tree_view()->session_root_auto()
// , _it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) ->bool{return il == _it->linker() && _it == il->host() && _it->parent() == il->host_parent(); })
// );
// } catch(std::exception &) {}

// return tree_index;
// }();

// if(tree_index) {
// _result = tree_view->view_paste_strategy(tree_index
// , _result
// , std::bind([&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source)->bool {return target->host()->field("url") == source->host()->field("url");}, std::placeholders::_1, _result->linker())
// , _view_paste_strategy, item_is_brand_new, find_url, check_euqal
// );
// }

// if(_result->is_lite())_result->to_fat();

////        //            else {
////        _item->binder(generator);
////        _item->activator(activator);
////        //                _record->generate();
////        //            }

// assert(_result != know_model_board->root_item());

////        assert(_result->is_registered_to_browser());
// assert(check_euqal(_result));   // assert(_result->url() == item->url());
//// assert(_result->fragment() == item->fragment());
////    }

////    }

////    }

////    assert(_record);
// } else {
// _result = target;
// }

// if(_result->is_lite())_result->to_fat();

// assert(_result != know_model_board->root_item());

// if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

// if(_result->field("file") == "")_result->field("file", "text.html");

// assert(_result->field("url") == target->field("url") || _result->field("url") == browser::Browser::_defaulthome || find_url.toString() == browser::Browser::_defaulthome);


// if((_result->field("url") == browser::Browser::_defaulthome) && (target->field("url") != browser::Browser::_defaulthome)) {
// _result->field("url", target->field("url"));
// }

//// assert(_result->url() == item->url());
//// assert(_result->fragment() == item->fragment());
// return _result;
// }


boost::intrusive_ptr<TreeItem> TreeIndex::bind(const QUrl               &_find_url
					      , const insert_strategy	&_view_insert_strategy
					      , equal_url _equal){
//    boost::intrusive_ptr<TreeItem>   _host = _host;

//    equal_url _equal = [&](boost::intrusive_ptr<const TreeItem> it_){return url_equal(it_->field<home_type>().toStdString(),  _find_url.toString().toStdString()) || url_equal(it_->field<url_type>().toStdString(),  _find_url.toString().toStdString());};
    boost::intrusive_ptr<TreeItem>	result(nullptr);
    auto				_tree_view = globalparameters.tree_screen()->view();	// static_cast<tv_t *>(static_cast<QObject *>(_current_model())->parent());
//    auto				_current_model	= [&] {return _tree_view->source_model();};
    auto	_current_item	= _tree_view->current_item();
    auto	it		= register_index(_find_url, _view_insert_strategy, _equal);
	// assert(tab_brother != it);
    browser::WebView		*view	= nullptr;
    auto			binder	= _current_item ? _current_item->binder() : nullptr;
    if(binder){
	auto host = binder->host();
	view = globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const Binder> b){return b->host() == host && b == binder && host->parent() == b->host()->parent() && b->host()->id() == host->id();});
    }
    auto browser_ = view ? view->page()->browser() : globalparameters.main_window()->vtab_record()->activated_browser();

//    boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] {return browser->record_screen()->record_controller()->source_model();}, tab_brother, it);	// tab_brother ? tab_brother->binder()->page()->record_controller()->source_model() :   // tab_brother does not guarantee the browser exists
//    assert(record_index);

    result = RecordIndex::bind([&] {return browser_->record_screen()->record_controller()->source_model();}, it, _current_item, false);

    return result;
}

boost::intrusive_ptr<TreeItem> TreeIndex::activate(const std::function<tkm_t *()>   &current_model_
						  , boost::intrusive_ptr<TreeItem>  host_
						  , const QUrl                      &find_url_
						  , const insert_strategy           &view_insert_strategy_
						  , equal_url equal_) noexcept {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    result = TreeIndex::instance(current_model_, host_)->bind(find_url_, view_insert_strategy_, equal_);
    if(result)result->activate(std::bind(&HidableTabWidget::find, globalparameters.main_window()->vtab_record(), std::placeholders::_1));
    return result;
}

// boost::intrusive_ptr<TreeItem> KnowView::item_bind(
// boost::intrusive_ptr<RecordModel::ModelIndex> modelindex   //
// , const KnowView::paste_strategy &_view_paste_strategy
// , equal_t _equal
// )
// {
// boost::intrusive_ptr<TreeItem> re;
//// auto _record_model = modelindex.current_model();
// boost::intrusive_ptr<TreeItem> tab_brother = modelindex->sibling();
// boost::intrusive_ptr<TreeItem> target = modelindex->target();

// if(!source_model()->index(target).isValid()) {
// re = item_register(target, _view_paste_strategy, _equal);
// } else
// re = target;

// assert(tab_brother != re);
// assert(re->field("url") == target->field("url"));
////        if( // !it->is_registered_to_browser() &&
////            !it->record_binder())
// re = globalparameters.entrance()->activated_browser()->item_bind(modelindex);
// assert(re->field("url") == target->field("url"));
////        else
////            re = it;

// return re;
////        auto ar = boost::make_shared<TabWidget::Coupler>(this, item, true);

////        //        bind_helper binder = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
////        //                             >>("", &TabWidget::RecordBinder::binder, ar);
////        //        active_helper activator = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &TabWidget::RecordBinder::activator, ar);


////        return _record_controller->item_request_from_tree(item
////                                                          , std::make_shared<CouplerDelegation>(
////                                                              std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
////                                                              , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
////                                                              , std::make_shared<bind_interface>("", &Coupler::binder, ar)
////                                                              , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
////                                                          ));
// }

TreeLevel::TreeLevel(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<TreeItem> _to_be_merged, tv_t *tree_view) : _tree_index(_tree_index)
																   , _to_be_operated(_to_be_merged)
																   , _tree_view(tree_view)
{}

boost::intrusive_ptr<TreeIndex> TreeLevel::tree_index(){return _tree_index;}

boost::intrusive_ptr<TreeItem> TreeLevel::to_be_operated(){return _to_be_operated;}

boost::intrusive_ptr<TreeItem> TreeLevel::merge(){return (_to_be_operated == _tree_index->host()) ? _to_be_operated : _tree_view->merge(this);}

boost::intrusive_ptr<TreeItem> TreeLevel::move(){
    return (_to_be_operated == _tree_index->host()) ? _to_be_operated : static_cast<tkm_t *>(_tree_view->source_model())->move(this);
}

boost::intrusive_ptr<TreeItem> TreeLevel::move_impl(const int pos, const int mode){
//    auto	_parent = _tree_index->host_parent();
    auto _parent = _tree_index->host();
    if(_parent){
	auto _linker = _to_be_operated->linker();// _parent->linker();
	if(! _linker){
	    _to_be_operated->linker(boost::intrusive_ptr<Linker>(new Linker(_parent, _to_be_operated)));								// , pos, mode
	    _linker = _to_be_operated->linker();
	    auto link_result = _linker->parent(_parent, pos, mode);
	    assert(link_result == _linker);
	    assert(_linker->integrity_external(_to_be_operated, _parent));
	}else if(_linker && _linker->host_parent() != _parent){
	    auto parent = _linker->host_parent();
	    if(parent && parent != _parent)parent->release([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == _to_be_operated->id() && il == _linker;});
	    if(_linker->host() != _to_be_operated)_linker->host(std::forward<boost::intrusive_ptr<TreeItem> >(_to_be_operated));							// std::move(boost::intrusive_ptr<TreeItem>(this))
	    auto link_result = _linker->parent(_parent, pos, mode);	// _linker->host_parent()->release(this->linker());
	    assert(link_result == _linker);
	    assert(_linker->integrity_external(_to_be_operated, _parent));
	}
    }
    return _to_be_operated;
}

boost::intrusive_ptr<TreeLevel> TreeLevel::instance(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<TreeItem> _to_be_merged){
    auto	current_model_	= _tree_index->current_model();
    auto	tree_view_	= static_cast<tv_t *>(static_cast<QObject *>(current_model_())->parent());
    auto	host		= _tree_index->host();
    if(TreeIndex::is_ancestor_of(_to_be_merged, host)){
	auto host_new = TreeLevel::instance(TreeIndex::instance(current_model_, _to_be_merged->parent()), host)->move();	// static_cast<tkm_t *>(current_model())
	assert(host_new == host);
//	auto	temp	= host;
//	host		= _to_be_merged;
//	_to_be_merged	= temp;
//	_tree_index	= TreeIndex::instance(_tree_index->current_model(), host, host->parent());
    }
    return new TreeLevel(_tree_index, _to_be_merged, tree_view_);
}

boost::intrusive_ptr<const TreeItem> TreeIndex::is_ancestor_of(boost::intrusive_ptr<const TreeItem> target, boost::intrusive_ptr<const TreeItem> reference){	// const std::function<tkm_t *()> &current_model_, const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal
    boost::intrusive_ptr<const TreeItem>  result(nullptr);
//	//    TreeModel::item(const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal) const {
//    std::function<boost::intrusive_ptr<const TreeItem>(boost::intrusive_ptr<const TreeItem>, const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &, int)>
//    item_recurse	// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem> item, QString name, int mode);
//	= [&](boost::intrusive_ptr<const TreeItem> _it
//	     , const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal
//	     , int mode
//	    ){
//	    static boost::intrusive_ptr<const TreeItem> find_item;
//	    if(mode == 0){
//		find_item = nullptr;
//		return find_item;	// nullptr;
//	    }
//	    if(find_item)return find_item;
//	    if(_equal(_it)){
//		find_item = _it;
//		return find_item;
//	    }else{
//		for(int i = 0; i < _it->count_direct(); i ++)item_recurse(_it->child_direct(i), _equal, 1);
//		return find_item;
//	    }
//	};
//	// Инициализация поиска
//    item_recurse(this, _equal, 0);	// QUrl()
//	// Запуск поиска и возврат результата
//    result = item_recurse(this, _equal, 1);
//	return result == this ? nullptr : result;	// _find_url
//    }
//    if(static_cast<QModelIndex>(current_model_()->index(target)).isValid() && static_cast<QModelIndex>(current_model_()->index(reference)).isValid()){
	//
    if(reference->path_list().contains(target->field<id_type>()) && reference != target){
	//
	result = reference;
    }
//    }
    return result;
}

