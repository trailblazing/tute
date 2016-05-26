#ifndef TREEINDEX_HXX
#define TREEINDEX_HXX

#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <QUrl>
#include <QModelIndex>


class TreeItem;
struct Binder;
struct Linker;
class KnowModel;

struct TreeIndex : public boost::intrusive_ref_counter<TreeIndex, boost::thread_safe_counter> {
public:

    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>)> equal_url;
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, const QUrl &)> equal_url_t;
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, boost::intrusive_ptr<const TreeItem>)> equal_t;
    typedef std::function<bool(boost::intrusive_ptr<const Linker>)> substitute_condition; // , boost::intrusive_ptr<const Linker>
    typedef std::function<bool(boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)> substitute_condition_double; //
    typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<TreeItem>, const substitute_condition &)> paste_strategy;

    TreeIndex(const std::function<KnowModel *()> &current_model, boost::intrusive_ptr<TreeItem> parent_item, int sibling_order = 0);   // , const QModelIndex &_current_index
    std::function<KnowModel *()> current_model()const;
    QModelIndex parent_index()const;
    //        QModelIndex current_index()const;
    boost::intrusive_ptr<TreeItem> parent() const;
    boost::intrusive_ptr<TreeItem> current_item() const;
    int sibling_order() const;


    boost::intrusive_ptr<TreeItem> item_register(const QUrl             &_find_url
                                                 , const paste_strategy &_view_paste_strategy
                                                 , equal_url _equal // = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
                                                );
    //    boost::intrusive_ptr<TreeItem> item_register(boost::intrusive_ptr<TreeItem> target
    //                                                 , const KnowView::paste_strategy &_view_paste_strategy
    //                                                 , equal_t _equal // = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
    //                                                );

    boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<TreeItem> tab_brother
                                             , const QUrl                   &_find_url
                                             , const paste_strategy         &_view_paste_strategy
                                             , equal_url _equal // = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
                                            );

    //    boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<RecordModel::ModelIndex> modelindex
    //                                             , const KnowView::paste_strategy &_view_paste_strategy
    //                                             , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
    //                                            );

private:
    std::function<KnowModel *()>    _current_model;
    boost::intrusive_ptr<TreeItem>  _parent;
    int                             _sibling_order;
    //        QModelIndex                     _current_index;
};


#endif // TREEINDEX_HXX
