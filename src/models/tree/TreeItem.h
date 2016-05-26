#ifndef TREEITEM_H
#define TREEITEM_H


#include <memory>
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
class KnowModel;

struct Binder;
struct PosProxy;
struct PosSource;
struct IndexProxy;
struct IndexSource;
struct IdType;


//// url_type

//struct url_full {
//    QString operator()(QUrl url) {return url.toString();}
//};

//struct url_fragment {
//    QString operator()(QUrl url) {return url.fragment();}
//};



namespace browser {
    class WebPage;
    class WebView;
    class TabWidget;
    class Browser;
}



// TreeItem = {item |record + items }
// Record = {record | { id, name, url, dir,file, home, pin ...} }
// ItemsFlat = { item[i] | i = 0,1,2,3 ... }
class TreeItem  //        : public std::enable_shared_from_this<TreeItem>
    : //boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter>
    public Record
    , public ItemsFlat {

public:


    //    typedef typename Binder::bind_interface        bind_interface;
    //    typedef typename Binder::activate_interface    activate_interface;
    //    typedef typename Binder::item_interface        item_interface;
    //    typedef typename Binder::page_interface        page_interface;

    //    typedef typename Binder::bind_helper           bind_helper;
    //    typedef typename Binder::activate_helper       activate_helper;
    //    typedef typename Binder::item_helper           item_helper;
    //    typedef typename Binder::page_helper           page_helper;



    typedef boost::sp_adl_block::thread_safe_counter counter_type; // boost::intrusive_ref_counter<TreeItem>::counter_type

    typedef boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter> counter;



    //    struct linker : public boost::intrusive_ref_counter<linker, boost::thread_safe_counter> {
    //    public:

    //        linker(boost::intrusive_ptr<TreeItem>  parentitem, boost::intrusive_ptr<TreeItem>  childitem, int _pos = 0, int _mode = ADD_NEW_RECORD_BEFORE);        //        boost::intrusive_ptr<TreeItem> parent(boost::intrusive_ptr<TreeItem> it, int _pos = 0, int _mode = ADD_NEW_RECORD_BEFORE);

    //        ~linker();  //        boost::intrusive_ptr<TreeItem> remove(boost::intrusive_ptr<TreeItem> _source_item);

    //        // Возвращение ссылки на родительский элемент
    //        boost::intrusive_ptr<TreeItem> parent_item()const {return _parent_item;}
    //        boost::intrusive_ptr<TreeItem> child_item()const {return _child_item;}

    //        //        // Удаление потомков, начиная с позиции position массива childItems
    //        //        bool remove(int position, int count);

    //        //        // Удаление всех потомков элемента
    //        //        void remove_all();

    //        // Возвращает номер, под которым данный объект хранится
    //        // в массиве childItems своего родителя
    //        int sibling_order() const
    //        {
    //            int result = -1;

    //            if(_parent_item)
    //                result = _parent_item->_child_items.indexOf(_child_item); // boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()

    //            return result;
    //        }

    //        int move_up(void)
    //        {
    //            int row = 0;
    //            // Выясняется номер данного элемента в списке родителя
    //            int num = sibling_order();

    //            if(num > 0 && num < (_parent_item->count_direct())) {   // if(num == 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
    //                row = -1;
    //                // Элемент перемещается вверх по списку
    //                (_parent_item->_child_items).swap(num, num + row);
    //            }

    //            return row;
    //        }

    //        int move_dn(void)
    //        {
    //            int row = 0;
    //            // Выясняется номер данного элемента в списке родителя
    //            int num = sibling_order();

    //            // Если двигать вниз некуда, ничего делать ненужно
    //            if(num > 0 && num < (_parent_item->count_direct())) { //if(num >= (_parent_item->count_direct()))return false;

    //                row = 1;
    //                // Элемент перемещается вниз по списку
    //                (_parent_item->_child_items).swap(num, num + row);
    //            }

    //            return row;
    //        }

    //    private:
    //        boost::intrusive_ptr<TreeItem>  _parent_item;
    //        boost::intrusive_ptr<TreeItem>  _child_item;
    //    };

    //    TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    // = nullptr
    //             , QMap<QString, QString>           _field_data     // = QMap<QString, QString>()
    //             , boost::intrusive_ptr<TreeItem>   _child_items_root // copy semantic, not good
    //            );

    TreeItem(boost::intrusive_ptr<TreeItem>     _host_parent    // = nullptr
        , QMap<QString, QString>           _field_data = QMap<QString, QString>()
        , const QDomElement                &_dom_element = QDomElement()
        , int pos = 0, int mode = add_new_record_before
        );

#ifdef _with_record_table
    TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    // = nullptr
        , boost::intrusive_ptr<Record>     _record              // = nullptr
        , const QDomElement                &_dom_element = QDomElement()
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

    //    // Добавление нового подчиненного элемента
    //    // в конец списка подчиненных элементов
    //    boost::intrusive_ptr<TreeItem> child_add_new(int pos, QString id, QString name);
    boost::intrusive_ptr<TreeItem> contains_direct(const boost::intrusive_ptr<TreeItem> &&_item) const;
    boost::intrusive_ptr<TreeItem> contains_direct(const boost::intrusive_ptr<const Linker> &&_item_linker) const;
    //    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record> item);
    boost::intrusive_ptr<TreeItem> operator <<(boost::intrusive_ptr<TreeItem> _item);

    //    boost::intrusive_ptr<linker> child_rent(boost::intrusive_ptr<TreeItem> item); // break parent contains semantic, make contians_direct does not work!!!

    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool children_insert_new(int position, int count, int columns);


    void binder_reset();
    boost::intrusive_ptr<Linker> delete_permanent_recursive(boost::intrusive_ptr<Linker> _to_be_removed_linker);

    boost::intrusive_ptr<Linker> parent(boost::intrusive_ptr<TreeItem> it, int pos = 0, int mode = add_new_record_before);
    // Возвращение ссылки на родительский элемент
    boost::intrusive_ptr<TreeItem> parent() const;

    // Удаление потомков, начиная с позиции position массива childItems
    QList<boost::intrusive_ptr<Linker> > delete_permanent_recursive(int position, int count);


    boost::intrusive_ptr<Linker> dangle();
    // Удаление всех потомков элемента
    void clear();

    //    int sibling_order(boost::intrusive_ptr<TreeItem> it)const;
    int sibling_order(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_equal) const;
    //    // Возвращает номер, под которым данный объект хранится
    //    // в массиве childItems своего родителя
    //    int sibling_order() const;

    int move_up(void);
    int move_dn(void);
    void move_up(int pos) {
        ItemsFlat::move_up(pos);
    }
    void move_dn(int pos) {
        ItemsFlat::move_dn(pos);
    }
    //    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    //    QStringList path_absolute(void) const;

    // Возвращает путь в виде названий веток дерева
    QStringList path_list(QString field_name = "id") const;

    QString path_string(QString field_name, QString delimeter) const;



    //    // Возвращает массив путей всех подветок, которые содержит ветка
    //    QList<QStringList> path_children_all(void) const;

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> path_children_all(QString field_name = "id") const;

    // Получение идентификатора элемента
    IdType id() const;
    QString name() const;

    // Получение идентификатора родительской ветки
    IdType parent_id() const;

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

    //    QDomElement dom_from_treeitem(std::shared_ptr<QDomDocument> doc);
    QDomElement dom_from_treeitem();
    //    void dom_to_direct(const QDomElement &_dom_element);
    void dom_to_records(const QDomElement &_record_dom_element);

//    boost::intrusive_ptr<const TreeItem> is_activated() const;
//    boost::intrusive_ptr<const TreeItem> is_registered_to_browser() const;

    boost::intrusive_ptr<Binder> binder();
    const boost::intrusive_ptr<Binder> &&binder() const;
    void binder(boost::intrusive_ptr<Binder> &&binder_);


    boost::intrusive_ptr<Linker> linker();
    const boost::intrusive_ptr<Linker> &&linker() const;
    void linker(boost::intrusive_ptr<Linker> &&up_linker_);

    //    // Взятие ссылки на данные конечных записей
    //    std::shared_ptr<RecordTable> record_table(void);
    //    void record_table(std::shared_ptr<RecordTable> _table_data);
    //    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    //    void record_table(QDomElement i_dom_element);

#ifdef _with_record_table   // deprecated
    void records_to_children();
#endif

    //    void children_clear(void);

    browser::WebPage *page() const;   // const; // {return _page;}
    boost::intrusive_ptr<TreeItem> item() const;
    browser::WebView *bind();
    //    boost::intrusive_ptr<TreeItem> bind(browser::WebPage *page);  // {_page = page; _page->record(this);}
    browser::WebView *activate(const std::function<browser::WebView *(const std::function<bool(boost::intrusive_ptr<const ::Binder>)> &_equal)> &find_activated);

    //    void binder(bind_helper g); // {_binder = g;}
    //    bind_helper binder() const; // {return _binder;}

    //    void activator(activate_helper a);    // {_activator = a;}
    //    activate_helper activator() const;    // {return _activator;}

    //    void active_request(PosSource pos, int openLinkIn);
    bool page_valid() const;// {return _page_valid;}
    //    operator ItemsFlat() {return *this;}

    // deprecated
    //    boost::intrusive_ptr<TreeItem> active_subset() const;



    //    int shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> it, int mode = ADD_NEW_RECORD_AFTER);


    bool is_empty() const;
    bool is_ancestor_of(boost::intrusive_ptr<const TreeItem> it) const;
    boost::intrusive_ptr<Linker> remove(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_equal);
    boost::intrusive_ptr<Linker> delete_permanent_recursive();
    boost::intrusive_ptr<Linker> delete_permanent_empty_recursive();
    boost::intrusive_ptr<TreeItem> sibling() const;
    boost::intrusive_ptr<TreeItem> merge(boost::intrusive_ptr<TreeItem> cut);
    int count_children_all();

    //    template<typename T = url_full>
    //    inline QString url() const;

    // deprecated because of conflicting with parent(boost::intrusive_ptr<TreeItem>)
    //    boost::intrusive_ptr<TreeItem> child_move_unique(boost::intrusive_ptr<TreeItem> _source_item, int pos, int mode = ADD_NEW_RECORD_AFTER);    // ADD_NEW_RECORD_TO_END
    void traverse_direct(const std::function< void(boost::intrusive_ptr<Linker>)> &operation);


    //    // Function to populate a table of DOM-document // Функция заполнения таблицы из DOM-документа
    //    void dom_to_itemsflat(const QDomElement &dom_model);






protected:

    //    bind_helper     _binder;
    //    activate_helper   _activator;
    //    coupler         _coupler;
    std::function<QDomElement(std::shared_ptr<QDomDocument> doc)> dom_from_treeitem_impl;
    std::function<void()> children_parent_as_this;
private:
    //    bool children_remove_link(int position, int count);

    void isolate(void);

    //    // Возвращает набор значений указанного поля для пути от корня к ветке
    //    std::function<QStringList(QString field_name)> path_absolute_as_field;     // std::function<QStringList (QString field_name)const>

    //    QList<QStringList> get_all_children_path_recurse(TreeItem *item, int mode);
    //    std::function<QList<QStringList> (boost::intrusive_ptr<const TreeItem> item, QString field_name, int mode)> path_children_all_as_field_impl;    // std::function<QList<QStringList> (boost::intrusive_ptr<const TreeItem> item, QString field_name, int mode)const>

    //    bool is_field_name_available(QString name) const;
    //    QStringList field_name_available_list(void) const;
    QStringList field_name_for_crypt_list(void) const;

    //    QList<boost::intrusive_ptr<TreeItem>>   _child_items;   // Список ссылок на потомков
    //    boost::intrusive_ptr<TreeItem>  _parent_item;   // Ссылка на родителя
    boost::intrusive_ptr<Linker>    _linker;

    //    // Таблица инфополей данной ветки
    //    QMap<QString, QString>                  _field_data;

    //    //    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    //    std::shared_ptr<RecordTable>            _record_table;    // = std::make_shared<TableData>();

    //    browser::WebPage    *_page;
    boost::intrusive_ptr<Binder>   _binder;
    //    bool                            _page_valid = false;
    //    PosSource                       _position;
    //    int                             _open_link_in_new_window = 0;

    //    friend void intrusive_ptr_add_ref(TreeItem *px);
    //    friend void intrusive_ptr_release(TreeItem *px);
    //    friend void boost::sp_adl_block::intrusive_ptr_add_ref< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;
    //    friend void boost::sp_adl_block::intrusive_ptr_release< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;

    //    void page_to_nullptr();   // {_page->record(nullptr); _page = nullptr; }
    bool is_holder() const;

    //    friend class browser::WebPage;
    friend class ItemsFlat;
    friend class KnowModel;
    //    friend class TreeScreen;
    friend inline boost::intrusive_ptr<TreeItem> operator << (boost::intrusive_ptr<TreeItem> it_left, boost::intrusive_ptr<TreeItem> it_right) {
        return *it_left << it_right;
    }
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




//template<>
//inline QString TreeItem::url<url_fragment>()const {return QUrl(field("url")).fragment();}

//template<>
//inline QString TreeItem::url<url_full>() const    {return field("url");}



#endif
