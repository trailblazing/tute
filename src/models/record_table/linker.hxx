#ifndef LINKER_HXX
#define LINKER_HXX

#include <memory>
#include <functional>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>


#include <QList>
#include "utility/delegate.h"


extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

class TreeItem;

struct Linker : public boost::intrusive_ref_counter<Linker, boost::thread_safe_counter> {
    public:

        typedef std::function<boost::intrusive_ptr<TreeItem>()>     item_exist;
        typedef std::function<const boost::intrusive_ptr<Linker>()> linker_exist;
        typedef std::function<bool ()>                              linker_self_reference;
        typedef std::function<bool (boost::intrusive_ptr<const TreeItem>)> item_consistency;


        typedef std::tuple <
            item_exist
            , item_exist
            , linker_exist
            , linker_exist
            , linker_self_reference
            , linker_self_reference
            , item_consistency
            , item_consistency
            > status_type;

        Linker(boost::intrusive_ptr<TreeItem> host_parent_item, boost::intrusive_ptr<TreeItem> host_item);	// , int pos = 0, int mode = ADD_NEW_RECORD_BEFORE       boost::intrusive_ptr<TreeItem> parent(boost::intrusive_ptr<TreeItem> it, int _pos = 0, int _mode = ADD_NEW_RECORD_BEFORE);

        ~Linker();	//        boost::intrusive_ptr<TreeItem> remove(boost::intrusive_ptr<TreeItem> _source_item);

        // Возвращение ссылки на родительский элемент
        boost::intrusive_ptr<TreeItem> host_parent() const;	// {return _parent_item;}
        boost::intrusive_ptr<TreeItem> host() const;	// {return _child_item;}


        void host_parent(boost::intrusive_ptr<TreeItem> &&p);
        void host(boost::intrusive_ptr<TreeItem> &&h);

        boost::intrusive_ptr<Linker> parent(boost::intrusive_ptr<TreeItem> _parent, const int pos = 0, const int mode = add_new_record_before);	//
        //        // Удаление потомков, начиная с позиции position массива childItems
        //        bool remove(int position, int count);

        //        // Удаление всех потомков элемента
        //        void remove_all();

        // Возвращает номер, под которым данный объект хранится
        // в массиве childItems своего родителя
        int sibling_order() const;

        int move_up(void);
        int move_dn(void);
        void remove();

        bool integrity_internal() const;
        bool integrity_external(boost::intrusive_ptr<const TreeItem> host_, boost::intrusive_ptr<const TreeItem> host_parent_) const;
        status_type state() const {return *_status;}


    private:





        boost::intrusive_ptr<TreeItem>  _host_parent;
        boost::intrusive_ptr<TreeItem>  _host;

        std::function<boost::intrusive_ptr<Linker> (boost::intrusive_ptr<TreeItem>, int, int)> child_move_unique;	// , boost::intrusive_ptr<TreeItem>   _self_host

        //        status_type state_impl();   // is_closure
        std::function<bool()>   integrity;//        bool integrity();

        std::unique_ptr<status_type>    _status;
        friend class TreeItem;
};

#endif	// LINKER_HXX
