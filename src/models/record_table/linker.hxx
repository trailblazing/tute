#ifndef LINKER_HXX
#define LINKER_HXX

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <functional>
#include <memory>

#include "utility/delegate.h"
#include "utility/util.hxx"
#include <QList>

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

#define HOST_EXIST 0
#define HOST_PARENT_EXIST 1
#define HOST_LINKER_EXIST 2
#define HOST_PARENT_LINKER_EXIST 3
#define HOST_LINKER_IS_THIS 4
#define HOST_PARENT_LINKERS_HAS_THIS 5
#define EXTERNAL_HOST_IS_THIS_HOST 6
#define EXTERNAL_HOST_PARENT_IS_THIS_HOST_PARENT 7

class i_t;

struct Linker
    : public boost::intrusive_ref_counter<Linker, boost::thread_safe_counter> {
	public:
	typedef std::function<boost::intrusive_ptr<i_t>()> item_exist;
	typedef std::function<const boost::intrusive_ptr<Linker>()> linker_exist;
	typedef std::function<bool()> linker_self_reference;
	typedef std::function<bool(boost::intrusive_ptr<const i_t>)> item_consistency;

	typedef std::tuple<item_exist, item_exist, linker_exist, linker_exist,
	                   linker_self_reference, linker_self_reference,
	                   item_consistency, item_consistency>
	    status_type;

	Linker(boost::intrusive_ptr<i_t> host_parent_item,
	       boost::intrusive_ptr<i_t> host_item);  // , const int pos = 0, const int
	                                              // mode = add_new_record_after

	~Linker();  //        boost::intrusive_ptr<TreeItem>
	            //        remove(boost::intrusive_ptr<TreeItem> _source_item);

	// Возвращение ссылки на родительский элемент
	boost::intrusive_ptr<i_t> host_parent() const;  // {return _parent_item;}
	boost::intrusive_ptr<i_t> host() const;         // {return _child_item;}

	void host_parent(boost::intrusive_ptr<i_t> &&p);
	void host(boost::intrusive_ptr<i_t> &&h);

	boost::intrusive_ptr<Linker> parent(boost::intrusive_ptr<i_t> _parent, const int pos = 0,
	                                    const int mode = add_new_record_after);  //
	//        // Удаление потомков, начиная с позиции position массива childItems
	//        bool remove(int position, int count);

	//        // Удаление всех потомков элемента
	//        void remove_all();

	// Возвращает номер, под которым данный объект хранится
	// в массиве childItems своего родителя
	int sibling_order() const;

	int move_up(void);
	int move_dn(void);
	void dangle();

	bool integrity_internal() const;
	bool integrity_external(boost::intrusive_ptr<const i_t> host_, boost::intrusive_ptr<const i_t> host_parent_) const;
	status_type state() const {
		return *_status;
	}
	static boost::intrusive_ptr<Linker>
	instance(boost::intrusive_ptr<i_t> _host, boost::intrusive_ptr<i_t> _parent, int pos = 0, int mode = add_new_record_before);

	private:
	boost::intrusive_ptr<i_t> _host_parent;
	boost::intrusive_ptr<i_t> _host;

	//	std::function<boost::intrusive_ptr<Linker>
	//(boost::intrusive_ptr<TreeItem>, int, int)> child_move_unique;	// ,
	//boost::intrusive_ptr<TreeItem>   _self_host
	//        status_type state_impl();   // is_closure
	std::unique_ptr<status_type> _status;
	std::function<boost::intrusive_ptr<Linker>(boost::intrusive_ptr<i_t>, int,
	                                           int)>
	    integrity_fix_up;  //        bool integrity();

	friend class i_t;
};

#endif  // LINKER_HXX
