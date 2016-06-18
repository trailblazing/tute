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
#include "models/record_table/recordindex.hxx"
#include "views/record_table/RecordScreen.h"
#include "controllers/record_table/RecordController.h"
#include "models/tree/KnowModel.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/browser/tabwidget.h"
#include "views/browser/browser.h"
#include "models/record_table/RecordProxyModel.h"


boost::intrusive_ptr<RecordIndex> RecordIndex::instance(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem>  target_item){

    return (sibling_item == target_item) ?
           nullptr
           : ((!sibling_item ? false : !sibling_item->binder() ?  false : !((QModelIndex)current_model()->index(sibling_item)).isValid()) ? true : false) ?
           nullptr
           : new RecordIndex(current_model, sibling_item, target_item);

}

RecordIndex::RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem> target_item)
    : _current_model(current_model), _sibling_item(sibling_item), _target_item(target_item)
{
    try {
        if(!_sibling_item ? false : !_sibling_item->binder() ?  false : !((QModelIndex)_current_model()->index(_sibling_item)).isValid()) {throw std::runtime_error(formatter() << "!((QModelIndex)_current_model()->index(_sibling_item)).isValid())"); } // assert(_sibling_item ? _sibling_item->binder() ? _current_model()->index(_sibling_item).isValid() : true : true);

        if(_sibling_item == _target_item) throw std::runtime_error(formatter() << "_sibling_item == _target_item");	// std::exception();   // assert(_sibling_item != _target_item);
    } catch(std::exception &e) {throw e; }
}

std::function<RecordModel *()> RecordIndex::current_model() const {
    return _current_model;
}

QModelIndex RecordIndex::sibling_index() const {
    return _current_model()->index(_sibling_item);
}

// QModelIndex RecordModel::ModelIndex::current_index()const{}

boost::intrusive_ptr<TreeItem> RecordIndex::sibling() const {
    return _sibling_item;
}

boost::intrusive_ptr<TreeItem> RecordIndex::target() const {
    return _target_item;
}
