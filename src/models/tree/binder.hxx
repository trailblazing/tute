#ifndef BINDER_HPP
#define BINDER_HPP

#include <memory>
#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "utility/delegate.h"


struct Binder;
class TreeItem;

namespace browser {
    class WebPage;
    class WebView;
}

struct Binder : public boost::intrusive_ref_counter<Binder, boost::thread_safe_counter> { // : public std::enable_shared_from_this<coupler>
public:

    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, boost::intrusive_ptr<TreeItem>&()> item_interface;
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebPage *&()>             page_interface;
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebView *()>              bind_interface;  // , boost::intrusive_ptr<TreeItem> (TreeItem::*)(browser::WebPage *)
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebView *()>              activate_interface;

    typedef std::shared_ptr<item_interface>     item_helper;
    typedef std::shared_ptr<page_interface>     page_helper;
    typedef std::shared_ptr<bind_interface>     bind_helper;   // , boost::intrusive_ptr<TreeItem> (TreeItem::*)(browser::WebPage *)
    typedef std::shared_ptr<activate_interface> activate_helper;

    typedef std::function<boost::intrusive_ptr<TreeItem>()>                                         item_exist;
    typedef std::function<browser::WebPage*()>                                                      page_exist;
    typedef std::function<const boost::intrusive_ptr<Binder>()>                                     binder_exist;
    typedef std::function<bool()>                                                                   binder_self_reference;
    //        typedef std::function<const boost::intrusive_ptr<coupler>(boost::intrusive_ptr<TreeItem> host)> item_binder_exist;
    //        typedef std::function<const boost::intrusive_ptr<coupler>(browser::WebPage *page)>              page_binder_exist;
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem> host)>                                item_consistency;
    typedef std::function<bool(const browser::WebPage *page)>                                             page_consistency;

    typedef std::tuple <
    item_exist
    , page_exist
    , binder_exist
    , binder_exist
    , binder_self_reference
    , binder_self_reference
    //        , item_binder_exist     // std::shared_ptr<item_binder_exist>
    //        , page_binder_exist     // std::shared_ptr<page_binder_exist>
    , item_consistency
    , page_consistency
    > status_type;


    typedef std::tuple <
    std::shared_ptr<item_exist>
    , std::shared_ptr<page_exist>
    , std::shared_ptr<binder_exist>
    , std::shared_ptr<binder_exist>
    , std::shared_ptr<binder_self_reference>
    , std::shared_ptr<binder_self_reference>
    //        , std::shared_ptr<item_binder_exist>
    //        , std::shared_ptr<page_binder_exist>
    , std::shared_ptr<item_consistency>
    , std::shared_ptr<page_consistency>
    > shared_status_type;

    Binder(item_interface          _item_linker
           , page_interface        _page_linker
           , bind_interface        _bind_helper
           , activate_interface    _activate_helper
          );

    // this design make a long time compiling!!!
    template<typename T>
    inline Binder(std::shared_ptr<T> ar)    //: std::enable_shared_from_this<CouplerDelegation>()
        : _item_linker(item_interface("", &T::item, ar))  // _item_linker(std::make_shared<item_interface>("", &T::item, ar))
        , _page_linker(page_interface("", &T::page, ar))  // _page_linker(std::make_shared<page_interface>("", &T::page, ar))
        , _bind_helper(bind_interface("", &T::bind, ar))  // _bind_helper(std::make_shared<bind_interface>("", &T::bind, ar))
        , _activate_helper(activate_interface("", &T::activator, ar)) // _activate_helper(std::make_shared<activate_interface>("", &T::activator, ar))
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
    boost::intrusive_ptr<TreeItem> &item() {return _item_linker();}   // (*_item_linker)();

    browser::WebPage *&page() {return _page_linker();}                // (*_page_linker)();

    browser::WebView *bind() {return _bind_helper();}  // , _bind  // (*_bind_helper)();

    browser::WebView *activator() {return _activate_helper();}   // (*_activate_helper)();

    bool integrity_internal()const;

    bool integrity_external(boost::intrusive_ptr<const TreeItem> host, const browser::WebPage *page)const;
    status_type state()const {return _status;}
    //        void break_page();
private:
    status_type state_impl();   // is_closure
    bool make_integrity();

    //        item_helper     _item_linker;
    //        page_helper     _page_linker;
    //        bind_helper     _bind_helper;
    //        activate_helper _activate_helper;

    item_interface      _item_linker;
    page_interface      _page_linker;
    bind_interface      _bind_helper;
    activate_interface  _activate_helper;

    status_type         _status;

};


#endif // BINDER_HPP
