
//#include "models/tree/tree_item.dec"

#include "record_index.hxx"

#include "items_flat.h"
#include "main.h"
#include "record.h"
#include "record_model.h"

#include "controllers/record_table/record_controller.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "models/tree/tree_model.h"
#include "views/browser/browser.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"

boost::intrusive_ptr<i_t>
RecordIndex::bind( // const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem>  host_, boost::intrusive_ptr<TreeItem> sibling_item_,
    bool make_current) noexcept
{
	boost::intrusive_ptr<i_t> result(nullptr);
	auto rctrl_ = _current_model()->record_ctrl();
	assert(rctrl_);
	if (rctrl_) {
		auto tab = rctrl_->tab_widget();

		web::Browser* browser_ = nullptr;

		if (tab) browser_ = tab->browser();
		assert(browser_); //browser_ is a weak_ptr?
		assert(_host);
		if (_host && browser_) {
			auto url = _host->field<url_key>();
			if (url != url_value("")) { // && url != web::Browser::_defaulthome
				// if(sibling_item_ != _host){
				// index_source sibling_index_ = sibling_item_ ?
				// current_model_()->index(sibling_item_) :
				// current_model_()->current_index();
				// if(! static_cast<QModelIndex>(sibling_index_).isValid()){
				// sibling_index_ = index_source(current_model_()->fake_index(_host));
				// assert(!
				// static_cast<QModelIndex>(current_model_()->index(_host)).isValid());
				// assert(static_cast<QModelIndex>(sibling_index_).isValid());
				// }	// index(0, 0, QModelIndex())
				// if(static_cast<QModelIndex>(sibling_index_).isValid())result =
				// browser_->bind(new RecordIndex(current_model_, _host, sibling_item_),
				// make_current);
				// }else result = sibling_item_;

				result = browser_->bind(this, make_current);
				assert(result->binder());
				assert(result->page());
			}
		}
	}
	assert(result);
	return result;
}

boost::intrusive_ptr<RecordIndex>
RecordIndex::instance(const std::function<RecordModel*()>& current_model_, boost::intrusive_ptr<i_t> host_, boost::intrusive_ptr<i_t> sibling_item_)
{
	boost::intrusive_ptr<RecordIndex> result(nullptr);
	assert(host_);
	if (sibling_item_ == host_) {
		auto rctrl_ = current_model_()->record_ctrl();
		if (rctrl_) {
			auto tab = rctrl_->tab_widget();
			if (tab) {
				sibling_item_ = tab->sibling(sibling_item_); // sibling_s(sibling_item_);
									     // if(! sibling_item_)throw std::runtime_error(formatter() << "sibling_item_
									     // == host_; host_ has already been inside the record view");
			}
		}
	}
	index_source sibling_index_;
	if (sibling_item_)
		sibling_index_ = current_model_()->index(sibling_item_);
	if (!static_cast<QModelIndex>(sibling_index_).isValid()) {
		auto parent = host_->linker()->host_parent();
		if (parent)
			sibling_index_ = current_model_()->index(parent);
	}
	if (!static_cast<QModelIndex>(sibling_index_).isValid()) {
		auto sibling_ = host_->sibling();
		if (sibling_)
			sibling_index_ = current_model_()->index(sibling_);
	}
	if (!static_cast<QModelIndex>(sibling_index_).isValid())
		sibling_index_ = current_model_()->current_index();
	if (!static_cast<QModelIndex>(sibling_index_).isValid()) {
		sibling_index_ = index_source(static_cast<RecordModel*>(current_model_())->fake_index(host_));
		assert(!static_cast<QModelIndex>(current_model_()->index(host_)).isValid());
		assert(static_cast<QModelIndex>(sibling_index_).isValid());
	} // index(0, 0, QModelIndex())
	if (!static_cast<QModelIndex>(sibling_index_).isValid())
		throw std::runtime_error(formatter() << "sibling_index_ isn\'t valid");
	result = new RecordIndex(current_model_, host_, sibling_index_);
	if (!result)
		throw std::runtime_error(formatter() << "construct record index failed");
	return result;
}
// boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const
// std::function<RecordModel *()> &current_model_,
// boost::intrusive_ptr<TreeItem>  host_, const index_source &sibling_index){
// boost::intrusive_ptr<RecordIndex>	result(nullptr);
// boost::intrusive_ptr<TreeItem>	sibling_item =
// current_model_()->item(pos_source(static_cast<const
// QModelIndex>(sibling_index).row()));
// if(sibling_item == host_)throw std::runtime_error(formatter() <<
// "_sibling_item == _target_item; _target_item has already been inside the
// record view");
// index_source sibling_index_ = sibling_index;
// if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ =
// index_source(current_model_()->fake_index(host_));;assert(!
// static_cast<QModelIndex>(current_model_()->index(host_)).isValid());}	//
// index(0, 0, QModelIndex())
// if(! static_cast<QModelIndex>(sibling_index_).isValid())throw
// std::runtime_error(formatter() << "_sibling_index isn\'t valid");
// result = new RecordIndex(current_model_, host_, sibling_index_);
// if(! result)throw std::runtime_error(formatter() << "construct record index
// failed");
// return result;
// }

RecordIndex::RecordIndex(const std::function<RecordModel*()>& current_model, boost::intrusive_ptr<i_t> target_item, const index_source& sibling_index)
    : _current_model(current_model)
    , _host(target_item)
    , _sibling_index(sibling_index)
{
}

// RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model,
// boost::intrusive_ptr<TreeItem> target_item, boost::intrusive_ptr<TreeItem>
// sibling_item)
// : _current_model(current_model)
// , _host(target_item)
// , _sibling_index([&] {
// QModelIndex sibling_index =
// static_cast<QModelIndex>(current_model()->index(sibling_item));
// if(! sibling_index.isValid()) sibling_index = current_model()->index(0, 0,
// QModelIndex());
// return sibling_index;
// }
// ()){}

std::function<RecordModel*()> RecordIndex::current_model() const
{
	return _current_model;
}

index_source RecordIndex::sibling_index() const
{
	return _sibling_index;
}
void RecordIndex::sibling_index(const QModelIndex& si)
{
	_sibling_index = si;
}

// QModelIndex RecordModel::ModelIndex::current_index() const {}

// boost::intrusive_ptr<TreeItem> RecordIndex::target_sibling() const {return
// _target_sibling;}

boost::intrusive_ptr<i_t> RecordIndex::host() const
{
	return _host;
}

boost::intrusive_ptr<i_t> RecordIndex::synchronize(
    boost::intrusive_ptr<i_t> host_) noexcept
{ // const
	// std::function<RecordModel
	// *()> &current_model_,
	boost::intrusive_ptr<i_t> _found_item(nullptr);
	if (host_) {
		auto v = gl_paras->main_window()->find(
		    [&](boost::intrusive_ptr<const ::Binder> b) {
			    return url_equal(detail::to_string(b->host()->field<home_key>()), detail::to_string(host_->field<home_key>()));
		    });
		if (v) {
			auto rctrl = v->tabmanager()->record_screen()->record_ctrl();
			if (rctrl) {
				auto current_model_ = [&] { return rctrl->source_model(); };
				auto alternative_item = v->page()->host();
				auto url = detail::to_string(alternative_item->field<url_key>()); // host_->field<url_type>();
				if (url != "") {                                                  // && url != web::Browser::_defaulthome
					// rctrl_t *rctrl = current_model_()->reocrd_controller();
					_found_item = rctrl->synchronize(RecordIndex::instance(current_model_, alternative_item, nullptr)); // host_
				}
			}
		}
	}
	return _found_item; // _record;
}
