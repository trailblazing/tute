#ifndef TREEINDEX_HXX
#define TREEINDEX_HXX

#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <QUrl>
#include <QModelIndex>


extern const int	add_new_record_to_end;
extern const int	add_new_record_before;
extern const int	add_new_record_after;

class TreeItem;
struct Binder;
struct Linker;
class tkm_t;
struct TreeIndex;

BOOST_STRONG_TYPEDEF(QModelIndex, index_tree)

struct TreeLevel : public boost::intrusive_ref_counter<TreeLevel, boost::thread_safe_counter> {
    boost::intrusive_ptr<TreeIndex>		tree_index();
    boost::intrusive_ptr<TreeItem>		to_be_operated();
    boost::intrusive_ptr<TreeItem>		move(const int pos = 0, const int mode = add_new_record_after);
    static boost::intrusive_ptr<TreeLevel>	instance(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<TreeItem> _to_be_merge);
    private:
	TreeLevel(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<TreeItem> _to_be_operated);
	boost::intrusive_ptr<TreeIndex> _tree_index;
	boost::intrusive_ptr<TreeItem>	_to_be_operated;
	friend struct TreeIndex;
};

struct TreeIndex : public boost::intrusive_ref_counter<TreeIndex, boost::thread_safe_counter> {
    public:

	typedef std::function<bool (boost::intrusive_ptr<const TreeItem>)>											equal_url;
	typedef std::function<bool (boost::intrusive_ptr<const TreeItem>, const QUrl &)>									equal_url_t;
	typedef std::function<bool (boost::intrusive_ptr<const TreeItem>, boost::intrusive_ptr<const TreeItem>)>						equal_t;
	typedef std::function<bool (boost::intrusive_ptr<const Linker>)>											substitute_condition;	// , boost::intrusive_ptr<const Linker>
	typedef std::function<bool (boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)>							substitute_condition_double;		//
	typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<TreeItem>, const substitute_condition &)>	paste_strategy;


	std::function<tkm_t *()> current_model() const;
//	index_tree host_parent_index() const;
	index_tree host_index() const;

	boost::intrusive_ptr<TreeItem>	host_parent() const;
	boost::intrusive_ptr<TreeItem>	host() const;
	int				sibling_order() const;
//	static boost::intrusive_ptr<TreeIndex> instance(const std::function<km_t *()> &current_model, boost::intrusive_ptr<TreeItem> host_parent, int sibling_order = 0);
	static boost::intrusive_ptr<TreeIndex> instance(const std::function<tkm_t *()> &current_model, boost::intrusive_ptr<TreeItem> host, boost::intrusive_ptr<TreeItem> host_parent = nullptr);

	boost::intrusive_ptr<TreeItem> item_register(const QUrl             &_find_url
						    , const paste_strategy  &_view_paste_strategy
						    , equal_url _equal	// = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
	    );
	//    boost::intrusive_ptr<TreeItem> item_register(boost::intrusive_ptr<TreeItem> target
	//                                                 , const KnowView::paste_strategy &_view_paste_strategy
	//                                                 , equal_t _equal // = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
	//                                                );

	boost::intrusive_ptr<TreeItem> page_instantiate(boost::intrusive_ptr<TreeItem> tab_brother
						       , const QUrl                   &_find_url
						       , const paste_strategy         &_view_paste_strategy
						       , equal_url _equal);

	//    boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<RecordModel::ModelIndex> modelindex
	//                                             , const KnowView::paste_strategy &_view_paste_strategy
	//                                             , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
	//                                            );

    protected:

    private:
//	TreeIndex(const std::function<km_t *()> &current_model, boost::intrusive_ptr<TreeItem> host_parent_, int sibling_order = 0);	// , const QModelIndex &_current_index
	TreeIndex(const std::function<tkm_t *()> &current_model, boost::intrusive_ptr<TreeItem> host, boost::intrusive_ptr<TreeItem> host_parent = nullptr);
	std::function<tkm_t *()>	_current_model;
	boost::intrusive_ptr<TreeItem>	_host;
	boost::intrusive_ptr<TreeItem>	_host_parent;

	int _sibling_order;
//	index_tree _host_parent_index;
	index_tree _host_index;
};


#endif	// TREEINDEX_HXX
