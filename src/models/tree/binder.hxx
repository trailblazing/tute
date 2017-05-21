#ifndef BINDER_HPP
#define BINDER_HPP

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <functional>
#include <memory>

#include "utility/delegate.h"
#include <QString>

template <typename return_type, typename... Arg>
using _interface = sd::_interface<sd::meta_info<std::shared_ptr<void>>, return_type, Arg...>;

struct Binder;
class i_t;

namespace web {
	class WebPage;
	class WebView;
	class TabWidget;
}

struct Binder
    : public boost::intrusive_ref_counter<Binder, boost::thread_safe_counter> { // : public
										// std::enable_shared_from_this<coupler>
    public:
	typedef _interface<void(boost::intrusive_ptr<i_t>)> item_set;
	typedef _interface<void(web::WebPage*)> page_set;
	typedef _interface<boost::intrusive_ptr<i_t>() const> item_get;
	typedef _interface<web::WebPage*() const> page_get;
	typedef _interface<web::WebView*()> bind_set; // , boost::intrusive_ptr<TreeItem>
						      // (TreeItem::*)(web::WebPage *)
	typedef _interface<web::WebView*(bool)> activate_interface;
	typedef _interface<QString()> binder_type_interface;

	typedef std::shared_ptr<item_get> item_helper;
	typedef std::shared_ptr<page_get> page_helper;
	typedef std::shared_ptr<bind_set> bind_helper; // , boost::intrusive_ptr<TreeItem>
						       // (TreeItem::*)(web::WebPage *)
	typedef std::shared_ptr<activate_interface> activate_helper;

	typedef std::function<boost::intrusive_ptr<i_t>()> item_exist;
	typedef std::function<web::WebPage*()> page_exist;
	typedef std::function<const boost::intrusive_ptr<Binder>()> binder_exist;
	typedef std::function<bool()> binder_self_reference;
	//        typedef std::function<const
	//        boost::intrusive_ptr<coupler>(boost::intrusive_ptr<TreeItem> host)>
	//        item_binder_exist;
	//        typedef std::function<const
	//        boost::intrusive_ptr<coupler>(web::WebPage *page)>
	//        page_binder_exist;
	typedef std::function<bool(boost::intrusive_ptr<const i_t> host)> item_consistency;
	typedef std::function<bool(const web::WebPage* page)> page_consistency;

	typedef std::tuple<
	    item_exist, page_exist, binder_exist, binder_exist, binder_self_reference,
	    binder_self_reference,
	    //        , item_binder_exist     // std::shared_ptr<item_binder_exist>
	    //        , page_binder_exist     // std::shared_ptr<page_binder_exist>
	    item_consistency, page_consistency>
	    status_type;

	typedef std::tuple<
	    std::shared_ptr<item_exist>, std::shared_ptr<page_exist>,
	    std::shared_ptr<binder_exist>, std::shared_ptr<binder_exist>,
	    std::shared_ptr<binder_self_reference>,
	    std::shared_ptr<binder_self_reference>,
	    //        , std::shared_ptr<item_binder_exist>
	    //        , std::shared_ptr<page_binder_exist>
	    std::shared_ptr<item_consistency>, std::shared_ptr<page_consistency>>
	    shared_status_type;

	Binder(item_set _item_linker_set, page_set _page_linker_set, item_get _host_linker, page_get _page_linker, bind_set _bind_helper, activate_interface _activate_helper, binder_type_interface _binder_type);

	// this design make a long time compiling!!!
	template <typename T>
	inline Binder(std::shared_ptr<T> ar)
	    : _item_linker_set(item_set(&T::host, ar))
	    , _page_linker_set(page_set(&T::page, ar))
	    , _host_linker(item_get(&T::host, ar))
	    , _page_linker(page_get(&T::page, ar))
	    , _bind_helper(bind_set(&T::bind, ar))
	    , _activate_helper(activate_interface(&T::activator, ar))
	    , _binder_type(binder_type_interface(&T::binder_type))
	    , _status(state_impl())
	{
		auto is_integratied = make_integrity();
		assert(is_integratied);

		//        //        assert([&]() {return item()->binder();}());
		//        //        _status = state_impl();

		//        assert(std::get<2>(_status)());
		//        assert(std::get<3>(_status)());

		//        //            if(ar->page_link())ar->page_link()->binder(this);
		//        //            if(ar->item_link())ar->item_link()->binder(this);

		//        //        (*_bounded_item)()->record_binder(this);
		//        //        (*_bounded_page)()->record_binder(this);
	}
	~Binder();

    public:
	boost::intrusive_ptr<i_t> host() const;

	web::WebPage* page() const;

	web::WebView* bind();

	web::WebView* activate(bool force_reload = false);

	bool integrity_internal() const;

	bool integrity_external(boost::intrusive_ptr<const i_t> host, const web::WebPage* page) const;

	status_type state() const;

	//        void break_page();

    private:
	void host(boost::intrusive_ptr<i_t>&& it);
	void page(web::WebPage* page);

	status_type state_impl(); // is_closure
	bool make_integrity();

	//        item_helper     _item_linker;
	//        page_helper     _page_linker;
	//        bind_helper     _bind_helper;
	//        activate_helper _activate_helper;

	item_set _item_linker_set;
	page_set _page_linker_set;
	item_get _host_linker;
	page_get _page_linker;
	bind_set _bind_helper;
	activate_interface _activate_helper;
	binder_type_interface _binder_type;
	status_type _status;
	friend class web::TabWidget;
};

#endif // BINDER_HPP
