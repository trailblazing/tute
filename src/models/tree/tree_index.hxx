#ifndef TREEINDEX_HXX
#define TREEINDEX_HXX

#include "libraries/global_parameters.h"
#include "utility/lease.h"
#include "utility/strong_typedef.hxx"
#include <QModelIndex>
#include <QUrl>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <functional>

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

class i_t;
struct Binder;
struct Linker;
class tkm_t;
struct TreeIndex;
class tv_t;
class RecordModel;
struct url_value;
template <typename>
struct real_url_t;
namespace web {
	class Browser;
}

BOOST_STRONG_TYPEDEF_EXTENSION(QModelIndex, index_tree)

struct TreeLevel
    : public boost::intrusive_ref_counter<TreeLevel, boost::thread_safe_counter> {
	boost::intrusive_ptr<TreeIndex> tree_index();
	boost::intrusive_ptr<i_t> to_be_operated();
	boost::intrusive_ptr<i_t> merge();
	boost::intrusive_ptr<i_t> move();

	static boost::intrusive_ptr<TreeLevel>
	instance(boost::intrusive_ptr<TreeIndex> _tree_index, boost::intrusive_ptr<i_t> _to_be_operated);

    private:
	TreeLevel(boost::intrusive_ptr<TreeIndex> tree_index, boost::intrusive_ptr<i_t> to_be_merged);
	//	boost::intrusive_ptr<TreeItem>			move_impl(const int pos = 0,
	//const int mode = add_new_record_after);
	boost::intrusive_ptr<TreeIndex> _tree_index;
	boost::intrusive_ptr<i_t> _to_be_operated;
	tv_t* _tree_view;
	friend struct TreeIndex;
	friend class tkm_t;
};

struct TreeIndex
    : public boost::intrusive_ref_counter<TreeIndex, boost::thread_safe_counter> {
    public:
	typedef std::function<bool(boost::intrusive_ptr<const i_t>)> equal_url;
	typedef std::function<bool(boost::intrusive_ptr<const i_t>, const url_value&)> equal_url_t;
	typedef std::function<bool(boost::intrusive_ptr<const i_t>, boost::intrusive_ptr<const i_t>)> equal_t;
	typedef std::function<bool(boost::intrusive_ptr<const Linker>)> substitute_condition;                                            // , boost::intrusive_ptr<const Linker>
	typedef std::function<bool(boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)> substitute_condition_double; //
	typedef std::function<boost::intrusive_ptr<i_t>(boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<i_t>, const substitute_condition&, bool)> insert_strategy;

	std::function<tkm_t*()> current_model() const;
	//	index_tree host_parent_index() const;
	index_tree host_index() const;

	//	boost::intrusive_ptr<TreeItem>	host_parent() const;
	boost::intrusive_ptr<i_t> host() const;
	int sibling_order() const;
	//	static boost::intrusive_ptr<TreeIndex> instance(const std::function<km_t*()> &current_model, boost::intrusive_ptr<TreeItem> host_parent, int sibling_order = 0);
	boost::intrusive_ptr<i_t> url_bind_browser(boost::intrusive_ptr<real_url_t<url_value>> real_find_url_, const insert_strategy& _view_insert_strategy, equal_url _equal, sd::intrusive_ptr<web::Browser> const& brwoser_); // boost::intrusive_ptr<TreeItem>   tab_brother,

	static boost::intrusive_ptr<TreeIndex> item_require_treeindex(const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> host_);
	static boost::intrusive_ptr<i_t> url_require_item_from_tree(boost::intrusive_ptr<real_url_t<url_value>> real_find_url_, const insert_strategy& tree_view_insert_strategy_, equal_url equal_);
	static boost::intrusive_ptr<i_t> url_activate(boost::intrusive_ptr<real_url_t<url_value>> real_find_url_, const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> sibling_host_, const insert_strategy& view_insert_strategy_, equal_url equal_, sd::intrusive_ptr<web::Browser> const& brwoser_) noexcept;
	//    boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<RecordModel::ModelIndex> modelindex, const KnowView::paste_strategy &_view_paste_strategy, equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();});
	static boost::intrusive_ptr<const i_t> is_ancestor_of(boost::intrusive_ptr<const i_t> target, boost::intrusive_ptr<const i_t> reference);

    protected:
    private:
	TreeIndex(const std::function<tkm_t*()>& current_model_, boost::intrusive_ptr<i_t> host_, const index_tree& host_index_, const int sibling_order_);

	std::function<tkm_t*()> _current_model;
	boost::intrusive_ptr<i_t> _host;
	//	boost::intrusive_ptr<TreeItem>		_host_parent;

	int _sibling_order;
	//	index_tree _host_parent_index;
	index_tree _host_index;
};

#endif // TREEINDEX_HXX
