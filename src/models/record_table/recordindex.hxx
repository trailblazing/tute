#ifndef RECORDINDEX_HXX
#define RECORDINDEX_HXX

#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <QModelIndex>

class TreeItem;
class RecordModel;

struct RecordIndex : public boost::intrusive_ref_counter<RecordIndex, boost::thread_safe_counter> {
public:
    RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem>  target_item);
    std::function<RecordModel *()> current_model()const;
    QModelIndex sibling_index()const;
    // QModelIndex current_index()const;
    boost::intrusive_ptr<TreeItem> sibling() const;
    boost::intrusive_ptr<TreeItem> target() const;
private:
    std::function<RecordModel *()>  _current_model;
    boost::intrusive_ptr<TreeItem>  _sibling_item;
    boost::intrusive_ptr<TreeItem>  _target_item;
    // QModelIndex                     _current_index;
};


#endif // RECORDINDEX_HXX
