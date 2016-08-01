#ifndef RECORDINDEX_HXX
#define RECORDINDEX_HXX

#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <QModelIndex>
// #include "models/record_table/RecordModel.h"

class TreeItem;
class RecordModel;


struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;



BOOST_STRONG_TYPEDEF(QString, id_value)
BOOST_STRONG_TYPEDEF(int, pos_source)
BOOST_STRONG_TYPEDEF(int, pos_proxy)
BOOST_STRONG_TYPEDEF(QModelIndex, index_proxy)
BOOST_STRONG_TYPEDEF(QModelIndex, index_source)

struct RecordIndex : public boost::intrusive_ref_counter<RecordIndex, boost::thread_safe_counter> {
    public:

	std::function<RecordModel *()>	current_model() const;
	index_source			sibling_index() const;
//	QModelIndex current_index() const;
//    boost::intrusive_ptr<TreeItem> target_sibling() const;
	boost::intrusive_ptr<TreeItem> host() const;

	static boost::intrusive_ptr<RecordIndex>		instance(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem> sibling_item_, boost::intrusive_ptr<TreeItem>  host_);
	static boost::intrusive_ptr<RecordIndex>		instance(const std::function<RecordModel *()> &current_model_, const index_source &sibling_index, boost::intrusive_ptr<TreeItem>  host_);
	static boost::intrusive_ptr<TreeItem>			synchronize(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem>  host_) noexcept;
	static boost::intrusive_ptr<TreeItem>			bind(const std::function<RecordModel *()> &current_model_, boost::intrusive_ptr<TreeItem> sibling_item_, boost::intrusive_ptr<TreeItem>  host_) noexcept;
    private:
	RecordIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem>  target_item);
	RecordIndex(const std::function<RecordModel *()> &current_model, const index_source &sibling_index, boost::intrusive_ptr<TreeItem>  target_item);
	std::function<RecordModel *()>  _current_model;
//	boost::intrusive_ptr<TreeItem>  _target_sibling;
	// QModelIndex                     _current_index;
	index_source			_sibling_index;
	boost::intrusive_ptr<TreeItem>	_host;
};


#endif	// RECORDINDEX_HXX
