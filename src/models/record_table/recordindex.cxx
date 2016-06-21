#include "recordindex.hxx"

#include "main.h"
#include "Record.h"
#include "RecordModel.h"
#include "ItemsFlat.h"

#include "models/tree/binder.hxx"
#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "views/record_table/RecordView.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/record_table/RecordScreen.h"
#include "controllers/record_table/RecordController.h"
#include "models/tree/KnowModel.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/browser/tabwidget.h"
#include "views/browser/browser.h"
#include "models/record_table/RecordProxyModel.h"


boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem>  target_item){
    boost::intrusive_ptr<RecordIndex> result(nullptr);
//    if(sibling_item == target_item)throw std::runtime_error(formatter() << "_sibling_item == _target_item");
    index_source sibling_index_ = current_model()->index(sibling_item);
    if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ = index_source(current_model()->fake_index(target_item));assert(! static_cast<QModelIndex>(current_model()->index(target_item)).isValid());}	// index(0, 0, QModelIndex())
    if(! static_cast<QModelIndex>(sibling_index_).isValid())throw std::runtime_error(formatter() << "_sibling_index isn\'t valid");
    result = new RecordIndex(current_model, sibling_index_, target_item);
    if(! result)throw std::runtime_error(formatter() << "construct record index failed");
    return result;
}
boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const std::function<RecordModel *()> &current_model, const index_source &sibling_index, boost::intrusive_ptr<TreeItem>  target_item){
    boost::intrusive_ptr<RecordIndex> result(nullptr);
    boost::intrusive_ptr<TreeItem> sibling_item = current_model()->item(pos_source(static_cast<const QModelIndex>(sibling_index).row()));
    if(sibling_item == target_item)throw std::runtime_error(formatter() << "_sibling_item == _target_item; _target_item has already been inside the record view");
    index_source sibling_index_ = sibling_index;
    if(! static_cast<QModelIndex>(sibling_index_).isValid()){sibling_index_ = index_source(current_model()->fake_index(target_item));;assert(! static_cast<QModelIndex>(current_model()->index(target_item)).isValid());}	// index(0, 0, QModelIndex())
    if(! static_cast<QModelIndex>(sibling_index_).isValid())throw std::runtime_error(formatter() << "_sibling_index isn\'t valid");
    result = new RecordIndex(current_model, sibling_index_, target_item);
    if(! result)throw std::runtime_error(formatter() << "construct record index failed");
    return result;
}
RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model, const index_source &sibling_index, boost::intrusive_ptr<TreeItem>  target_item) : _current_model(current_model), _sibling_index(sibling_index), _target(target_item){}

RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem> target_item)
    : _current_model(current_model), _sibling_index([&] {QModelIndex sibling_index = static_cast<QModelIndex>(current_model()->index(sibling_item));if(! sibling_index.isValid())sibling_index = current_model()->index(0, 0, QModelIndex());return sibling_index;}
          ()), _target(target_item){}
std::function<RecordModel *()> RecordIndex::current_model() const {return _current_model;}

QModelIndex RecordIndex::sibling_index() const {return _sibling_index;}

// QModelIndex RecordModel::ModelIndex::current_index() const {}

// boost::intrusive_ptr<TreeItem> RecordIndex::target_sibling() const {return _target_sibling;}

boost::intrusive_ptr<TreeItem> RecordIndex::target() const {return _target;}
