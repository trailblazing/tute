#include "record_index.hxx"

#include "main.h"
#include "record.h"
#include "record_model.h"
#include "items_flat.h"

#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_model.h"
#include "models/app_config/app_config.h"
#include "views/main_window/main_window.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "views/record_table/record_view.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/record_table/record_screen.h"
#include "controllers/record_table/record_controller.h"
#include "models/tree/tree_know_model.h"
#include "views/tree/tree_screen.h"
#include "views/record/meta_editor.h"
#include "views/browser/tabwidget.h"
#include "views/browser/browser.h"
#include "models/record_table/record_model_proxy.h"


boost::intrusive_ptr<TreeItem> RecordIndex::bind(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem>  host_, boost::intrusive_ptr<TreeItem> sibling_item_, bool make_current) noexcept {
    boost::intrusive_ptr<TreeItem>	result(nullptr);
    rctrl_t				*ctrl		= current_model_()->reocrd_controller();
    browser::Browser			*browser_	= ctrl->tabmanager()->browser();
    if(host_){
	auto url = host_->field<url_type>();
	if(url != "" && url != browser::Browser::_defaulthome){
	    if(sibling_item_ != host_){
		index_source sibling_index_ = sibling_item_ ? current_model_()->index(sibling_item_) : current_model_()->current_index();
		if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ = index_source(current_model_()->fake_index(host_));assert(! static_cast<QModelIndex>(current_model_()->index(host_)).isValid());}	// index(0, 0, QModelIndex())
		if(static_cast<QModelIndex>(sibling_index_).isValid())result = browser_->bind(new RecordIndex(current_model_, host_, sibling_item_), make_current);
	    }else result = sibling_item_;
	}
    }
    return result;
}

boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem>  host_, boost::intrusive_ptr<TreeItem> sibling_item_){
    boost::intrusive_ptr<RecordIndex> result(nullptr);
    if(sibling_item_ == host_)throw std::runtime_error(formatter() << "_sibling_item == _target_item; _target_item has already been inside the record view");
    index_source sibling_index_;
    sibling_index_ = sibling_item_ ? current_model_()->index(sibling_item_) : current_model_()->current_index();
    if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ = index_source(current_model_()->fake_index(host_));assert(! static_cast<QModelIndex>(current_model_()->index(host_)).isValid());}	// index(0, 0, QModelIndex())
    if(! static_cast<QModelIndex>(sibling_index_).isValid())throw std::runtime_error(formatter() << "_sibling_index isn\'t valid");
    result = new RecordIndex(current_model_, host_, sibling_index_);
    if(! result)throw std::runtime_error(formatter() << "construct record index failed");
    return result;
}

// boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem>  host_, const index_source &sibling_index){
//    boost::intrusive_ptr<RecordIndex>	result(nullptr);
//    boost::intrusive_ptr<TreeItem>	sibling_item = current_model_()->item(pos_source(static_cast<const QModelIndex>(sibling_index).row()));
//    if(sibling_item == host_)throw std::runtime_error(formatter() << "_sibling_item == _target_item; _target_item has already been inside the record view");
//    index_source sibling_index_ = sibling_index;
//    if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ = index_source(current_model_()->fake_index(host_));;assert(! static_cast<QModelIndex>(current_model_()->index(host_)).isValid());}	// index(0, 0, QModelIndex())
//    if(! static_cast<QModelIndex>(sibling_index_).isValid())throw std::runtime_error(formatter() << "_sibling_index isn\'t valid");
//    result = new RecordIndex(current_model_, host_, sibling_index_);
//    if(! result)throw std::runtime_error(formatter() << "construct record index failed");
//    return result;
// }

RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem>  target_item, const index_source &sibling_index) : _current_model(current_model), _host(target_item), _sibling_index(sibling_index){}

RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> target_item, boost::intrusive_ptr<TreeItem> sibling_item)
    : _current_model(current_model)
      , _host(target_item)
      , _sibling_index([&] {QModelIndex sibling_index = static_cast<QModelIndex>(current_model()->index(sibling_item));if(! sibling_index.isValid()) sibling_index = current_model()->index(0, 0, QModelIndex());return sibling_index;}

	  ()){}

std::function<RecordModel *()> RecordIndex::current_model() const {return _current_model;}

index_source RecordIndex::sibling_index() const {return _sibling_index;}

// QModelIndex RecordModel::ModelIndex::current_index() const {}

// boost::intrusive_ptr<TreeItem> RecordIndex::target_sibling() const {return _target_sibling;}

boost::intrusive_ptr<TreeItem> RecordIndex::host() const {return _host;}

boost::intrusive_ptr<TreeItem> RecordIndex::synchronize(boost::intrusive_ptr<TreeItem>  host_) noexcept {// const std::function<RecordModel *()> &current_model_,
    boost::intrusive_ptr<TreeItem>	_found_item(nullptr);
    if(host_){
	auto v = globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const ::Binder> b){return url_equal(b->host()->field<home_type>().toStdString(), host_->field<home_type>().toStdString());});
	if(v){
	    auto	ctrl			= v->tabmanager()->record_controller();
	    auto	current_model_		= [&] {return ctrl->source_model();};
	    auto	alternative_item	= v->page()->host();
	    auto	url			= alternative_item->field<url_type>();	// host_->field<url_type>();
	    if(url != "" && url != browser::Browser::_defaulthome){
//		rctrl_t *ctrl = current_model_()->reocrd_controller();
		_found_item = ctrl->synchronize(new RecordIndex(current_model_, alternative_item, nullptr));	// host_
	    }
	}
    }
    return _found_item;	// _record;
}

