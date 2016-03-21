#ifndef TREEITEM_H
#define TREEITEM_H


#include <memory>
//#include <map>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include "models/record_table/Record.h"
#include "models/record_table/ItemsFlat.h"


//class coupler;
class Record;
class ItemsFlat;
class TreeItem;

// url_type

struct url_full {
    QString operator()(QUrl url) {return url.toString();}
};

struct url_fragment {
    QString operator()(QUrl url) {return url.fragment();}
};


namespace browser {
    class WebPage;
    class WebView;
    class TabWidget;
}


struct CouplerDelegation;

struct CouplerDelegation : public boost::intrusive_ref_counter<CouplerDelegation, boost::thread_safe_counter> { //    : public std::enable_shared_from_this<CouplerDelegation>
public:

    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, boost::intrusive_ptr<TreeItem>>    bounded_item_interface;
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebPage *>                bounded_page_interface;
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebView *>                bind_interface;   // , boost::intrusive_ptr<TreeItem> (TreeItem::*)(browser::WebPage *)
    typedef sd::_interface<sd::meta_info<std::shared_ptr<void>>, browser::WebView *>                activate_interface;

    typedef std::shared_ptr<bounded_item_interface> bounded_item_helper;
    typedef std::shared_ptr<bounded_page_interface> bounded_page_helper;
    typedef std::shared_ptr<bind_interface>         bind_helper;   // , boost::intrusive_ptr<TreeItem> (TreeItem::*)(browser::WebPage *)
    typedef std::shared_ptr<activate_interface>     activate_helper;


    //    template<typename T>
    inline CouplerDelegation(bounded_item_helper      _bounded_item
                             , bounded_page_helper    _bounded_page
                             , bind_helper            _bind_helper
                             , activate_helper        _active_helper
                            ): _bounded_item(_bounded_item), _bounded_page(_bounded_page), _bind_helper(_bind_helper), _activate_helper(_active_helper)
    {}

    template<typename T>
    inline CouplerDelegation(std::shared_ptr<T> ar)    //: std::enable_shared_from_this<CouplerDelegation>()
        : _bounded_item(std::make_shared<bounded_item_interface>("", &T::bounded_item, ar))
        , _bounded_page(std::make_shared<bounded_page_interface>("", &T::bounded_page, ar))
        , _bind_helper(std::make_shared<bind_interface>("", &T::binder, ar))
        , _activate_helper(std::make_shared<activate_interface>("", &T::activator, ar))
    {
        //        (*_bounded_item)()->record_binder(this);
        //        (*_bounded_page)()->record_binder(this);
    }

public:
    boost::intrusive_ptr<TreeItem> bounded_item() {return (*_bounded_item)();}

    browser::WebPage *bounded_page() {return (*_bounded_page)();}

    browser::WebView *binder(/*boost::intrusive_ptr<TreeItem> item*/)    // , boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(browser::WebPage *)
    {
        return (*_bind_helper)();   // , _bind
    }

    browser::WebView *activator() {return (*_activate_helper)();}
private:
    bounded_item_helper _bounded_item;
    bounded_page_helper _bounded_page;
    bind_helper         _bind_helper;
    activate_helper     _activate_helper;

};

// TreeItem = {item |record + items }
// Record = {record | { id, name, url, dir,file, home, pin ...} }
// ItemsFlat = { item[i] | i = 0,1,2,3 ... }
class TreeItem  //        : public std::enable_shared_from_this<TreeItem>
    : public Record
    , public ItemsFlat {
    // boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter>
public:

    typedef typename CouplerDelegation::bind_interface          bind_interface;
    typedef typename CouplerDelegation::activate_interface      activate_interface;
    typedef typename CouplerDelegation::bounded_item_interface  bounded_item_interface;
    typedef typename CouplerDelegation::bounded_page_interface  bounded_page_interface;

    typedef typename CouplerDelegation::bind_helper         bind_helper;
    typedef typename CouplerDelegation::activate_helper     activate_helper;
    typedef typename CouplerDelegation::bounded_item_helper bounded_item_helper;
    typedef typename CouplerDelegation::bounded_page_helper bounded_page_helper;

    typedef boost::sp_adl_block::thread_safe_counter counter_type; // boost::intrusive_ref_counter<TreeItem>::counter_type

    typedef boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter> counter;

    //    TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    // = nullptr
    //             , QMap<QString, QString>           _field_data     // = QMap<QString, QString>()
    //             , boost::intrusive_ptr<TreeItem>   _child_items_root // copy semantic, not good
    //            );

    TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    = nullptr
             , QMap<QString, QString>           _field_data     = QMap<QString, QString>()
             , const QDomElement                &_dom_element   = QDomElement()
            );

#ifdef _with_record_table
    TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    // = nullptr
             , boost::intrusive_ptr<Record>     _record         // = nullptr
             , const QDomElement                &_dom_element   = QDomElement()
            );
#endif

    //    TreeItem(const TreeItem &item);
    //    TreeItem &operator =(const TreeItem &item);

    ~TreeItem();

    //    // Возвращение ссылки на потомка, который хранится в списке childItems
    //    // под указанным номером
    //    boost::intrusive_ptr<TreeItem> item_direct(int number);

    //    int direct_children_count()const;
    //    // Возвращение количества потомков (т.е. количество записей в списке childItems)
    //    int size() const;

    // Возвращение количества полей, которые хранит данный элемент.
    int field_count() const;

    // Получение значения поля по имени
    QString field(QString _name) const;
    void field(QString _name, QString value);   // {Record::field(_name, value);}

    // Получение всех полей данных
    QMap<QString, QString> fields_all();

    // Получение всех полей данных напрямую, без преобразований
    QMap<QString, QString> fields_all_direct();

    // Заполнение указанного поля
    //    void field(QString name, QString value);

    // Заполнение указанного поля данными напрямую, без преобразований
    //    void field_direct(QString name, QString value);

    // Добавление нового подчиненного элемента
    // в конец списка подчиненных элементов
    boost::intrusive_ptr<TreeItem> child_add_new(int pos);

    //    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record> item);
    boost::intrusive_ptr<TreeItem> child_duplicate(boost::intrusive_ptr<TreeItem> item);

    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool children_insert_new(int position, int count, int columns);


    void page_break();
    boost::intrusive_ptr<TreeItem> child_remove(boost::intrusive_ptr<TreeItem> item);

    void parent(boost::intrusive_ptr<TreeItem> it);
    // Возвращение ссылки на родительский элемент
    boost::intrusive_ptr<TreeItem> parent();

    // Удаление потомков, начиная с позиции position массива childItems
    bool children_remove(int position, int count);

    // Удаление всех потомков элемента
    void children_remove_all();

    // Возвращает номер, под которым данный объект хранится
    // в массиве childItems своего родителя
    int sibling_order() const;

    int move_up(void);
    int move_dn(void);
    void move_up(int pos) {ItemsFlat::move_up(pos);}
    void move_dn(int pos) {ItemsFlat::move_dn(pos);}
    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    QStringList path_absolute(void);

    // Возвращает путь в виде названий веток дерева
    QStringList path_absolute_as_name(void);

    QString path_absolute_as_name_with_delimiter(QString delimeter);

    // Возвращает набор значений указанного поля для пути от корня к ветке
    QStringList path_absolute_as_field(QString field_name);

    // Возвращает массив путей всех подветок, которые содержит ветка
    QList<QStringList> path_children_all(void);

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> path_children_all_as_field(QString fieldName);

    // Получение идентификатора элемента
    QString id();
    QString name();

    // Получение идентификатора родительской ветки
    QString parent_id();

    // Шифрация данной ветки и всех подветок
    void to_encrypt(void);

    // Расшифровка данной ветки и всех подветок
    void to_decrypt(void);

    //    // Взятие количества записей в таблице конечных записей, "промежуточный" метод
    //    int size(void);

    //    // Удаление всех элементов в таблице конечных записей, "промежуточный" метод
    //    void clear_tabledata(void);

    //    // Преобразование таблицы конечных записей в DOM представление, "промежуточный" метод
    //    // Convert the table of final entries in the DOM representation "intermediate" method
    QDomElement dom_from_treeitem(std::shared_ptr<QDomDocument> doc);
    QDomElement dom_from_treeitem();
    void dom_to_direct(const QDomElement &_dom_element);

    boost::intrusive_ptr<TreeItem> is_activated();
    boost::intrusive_ptr<TreeItem> is_registered_to_browser();

    boost::intrusive_ptr<CouplerDelegation> record_binder();
    void record_binder(boost::intrusive_ptr<CouplerDelegation> _record_binder);

    //    // Взятие ссылки на данные конечных записей
    //    std::shared_ptr<RecordTable> record_table(void);
    //    void record_table(std::shared_ptr<RecordTable> _table_data);
    //    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    //    void record_table(QDomElement i_dom_element);

#ifdef _with_record_table   // deprecated
    void records_to_children();
#endif

    void children_clear(void);

    browser::WebPage *bounded_page();   // const; // {return _page;}
    browser::WebView *bind();
    //    boost::intrusive_ptr<TreeItem> bind(browser::WebPage *page);  // {_page = page; _page->record(this);}
    browser::WebView *activate();

    //    void binder(bind_helper g); // {_binder = g;}
    //    bind_helper binder() const; // {return _binder;}

    //    void activator(activate_helper a);    // {_activator = a;}
    //    activate_helper activator() const;    // {return _activator;}

    void active_request(int pos, int openLinkIn);
    bool page_valid()const;// {return _page_valid;}
    //    operator ItemsFlat() {return *this;}
    boost::intrusive_ptr<TreeItem> active_subset() const;


    boost::intrusive_ptr<TreeItem> child_move(int pos, boost::intrusive_ptr<TreeItem> _item, int mode = ADD_NEW_RECORD_AFTER);    // ADD_NEW_RECORD_TO_END
    //    int shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> it, int mode = ADD_NEW_RECORD_AFTER);


    bool is_empty() const;
    bool self_remove();
    bool self_empty_remove();
    boost::intrusive_ptr<TreeItem> record_merge(boost::intrusive_ptr<TreeItem> cut);
    int count_records_all();

    template<typename T = url_full>
    inline QString url() const;



protected:

    //    bind_helper     _binder;
    //    activate_helper   _activator;
    //    coupler         _coupler;
private:
    //    bool children_remove_link(int position, int count);

    void isolate(void);

    // QList<QStringList> get_all_children_path_recurse(TreeItem *item,int mode);
    QList<QStringList> path_children_all_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode);

    //    bool is_field_name_available(QString name) const;
    //    QStringList field_name_available_list(void) const;
    QStringList field_name_for_crypt_list(void) const;

    //    QList<boost::intrusive_ptr<TreeItem>>   _child_items;   // Список ссылок на потомков
    boost::intrusive_ptr<TreeItem>  _parent_item;   // Ссылка на родителя

    //    // Таблица инфополей данной ветки
    //    QMap<QString, QString>                  _field_data;

    //    //    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    //    std::shared_ptr<RecordTable>            _record_table;    // = std::make_shared<TableData>();

    //    browser::WebPage    *_page;
    boost::intrusive_ptr<CouplerDelegation>   _record_binder;
    //    bool                _page_valid = false;
    int                 _position = -1;
    int                 _open_link_in_new_window = 0;
    //    friend void intrusive_ptr_add_ref(TreeItem *px);
    //    friend void intrusive_ptr_release(TreeItem *px);
    //    friend void boost::sp_adl_block::intrusive_ptr_add_ref< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;
    //    friend void boost::sp_adl_block::intrusive_ptr_release< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;

    //    void page_to_nullptr();   // {_page->record(nullptr); _page = nullptr; }
    bool is_holder();
    friend class browser::WebPage;

};

//inline void intrusive_ptr_add_ref(TreeItem *px){boost::sp_adl_block::intrusive_ptr_add_ref(px);}
//inline void intrusive_ptr_release(TreeItem *px){boost::sp_adl_block::intrusive_ptr_release(px);}


//template<typename T>
//inline CouplerDelegation::CouplerDelegation(std::shared_ptr<T> ar)
//    : std::enable_shared_from_this<CouplerDelegation>()
//    , _bounded_item(std::make_shared<bounded_item_interface>("", &T::bounded_item, ar))
//    , _bounded_page(std::make_shared<bounded_page_interface>("", &T::bounded_page, ar))
//    , _bind_helper(std::make_shared<bind_interface>("", &T::binder, ar))
//    , _activate_helper(std::make_shared<activate_interface>("", &T::activator, ar))
//{
//    //    if(!(*_bounded_item)()->record_binder())(*_bounded_item)()->record_binder(shared_from_this());

//    //    (*_bounded_page)()->record_binder(shared_from_this());
//}




template<>
inline QString TreeItem::url<url_fragment>()const {return QUrl(field("url")).fragment();}

template<>
inline QString TreeItem::url<url_full>() const    {return field("url");}



#endif
