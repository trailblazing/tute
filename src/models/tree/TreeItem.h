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


namespace browser {
    class WebPage;
    class TabWidget;
}

class TreeItem  //        : public std::enable_shared_from_this<TreeItem>
    : public Record
    , public ItemsFlat {
    // boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter>
public:

    typedef std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem> (TreeItem::*)(browser::WebPage *)>> bind_helper;
    typedef std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>> active_helper;

    typedef boost::sp_adl_block::thread_safe_counter counter_type; // boost::intrusive_ref_counter<TreeItem>::counter_type

    typedef boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter> counter;

    TreeItem(
        QMap<QString, QString>              field_data
        , boost::intrusive_ptr<TreeItem>    parent_item
        , const QDomElement                 &i_dom_element
        = QDomElement()
    );

    TreeItem(
        boost::intrusive_ptr<Record>        record
        , boost::intrusive_ptr<TreeItem>    parent_item
        , const QDomElement                 &i_dom_element
        = QDomElement()
    );

    TreeItem(const TreeItem &item);
    TreeItem &operator =(const TreeItem &item);

    ~TreeItem();

    // Возвращение ссылки на потомка, который хранится в списке childItems
    // под указанным номером
    boost::intrusive_ptr<TreeItem> child(int number);

    int size()const;
    //    // Возвращение количества потомков (т.е. количество записей в списке childItems)
    //    int size() const;

    // Возвращение количества полей, которые хранит данный элемент.
    int field_count() const;

    // Получение значения поля по имени
    QString field(QString _name);
    void field(QString _name, QString value) {Record::field(_name, value);}

    // Получение всех полей данных
    QMap<QString, QString> all_fields();

    // Получение всех полей данных напрямую, без преобразований
    QMap<QString, QString> all_fields_direct();

    // Заполнение указанного поля
    //    void field(QString name, QString value);

    // Заполнение указанного поля данными напрямую, без преобразований
    //    void field_direct(QString name, QString value);

    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool insert_children(int position, int count, int columns);

    // Добавление нового подчиненного элемента
    // в конец списка подчиненных элементов
    boost::intrusive_ptr<TreeItem> add_child(void);

    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record> item);
    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<TreeItem> item);

    void unload_page();
    boost::intrusive_ptr<TreeItem> remove_child(boost::intrusive_ptr<TreeItem> item);

    void parent(boost::intrusive_ptr<TreeItem> it);
    // Возвращение ссылки на родительский элемент
    boost::intrusive_ptr<TreeItem> parent();

    // Удаление потомков, начиная с позиции position массива childItems
    bool remove_children(int position, int count);

    // Удаление всех потомков элемента
    void remove_all_children();

    // Возвращает номер, под которым данный объект хранится
    // в массиве childItems своего родителя
    int sibling_order() const;

    bool move_up(void);
    bool move_dn(void);
    void move_up(int pos) {ItemsFlat::move_up(pos);}
    void move_dn(int pos) {ItemsFlat::move_dn(pos);}
    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    QStringList path(void);

    // Возвращает путь в виде названий веток дерева
    QStringList path_as_name(void);

    QString path_as_name_with_delimiter(QString delimeter);

    // Возвращает набор значений указанного поля для пути от корня к ветке
    QStringList path_as_field(QString fieldName);

    // Возвращает массив путей всех подветок, которые содержит ветка
    QList<QStringList> all_children_path(void);

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> all_children_path_as_field(QString fieldName);

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
    QDomElement export_to_dom(std::shared_ptr<QDomDocument> doc);
    QDomElement export_to_dom();
    void import_from_dom(const QDomElement &dom_model);

    boost::intrusive_ptr<TreeItem> active_subset();

    //    // Взятие ссылки на данные конечных записей
    //    std::shared_ptr<RecordTable> record_table(void);
    //    void record_table(std::shared_ptr<RecordTable> _table_data);
    //    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    //    void record_table(QDomElement i_dom_element);

#ifdef _with_record_table   // deprecated
    void records_to_children();
#endif

    void clear_children(void);

    browser::WebPage *unique_page();   // const; // {return _page;}
    browser::WebView *self_bind();
    boost::intrusive_ptr<TreeItem> bind(browser::WebPage *page);  // {_page = page; _page->record(this);}
    browser::WebView *active();

    void binder(bind_helper g); // {_binder = g;}
    bind_helper binder() const; // {return _binder;}

    void activator(active_helper a);    // {_activator = a;}
    active_helper activator() const;    // {return _activator;}

    void active_request(int pos, int openLinkIn);
    bool page_valid()const {return _page_valid;}
protected:

    bind_helper _binder;
    active_helper _activator;

private:
    bool remove_children_link(int position, int count);

    void clear(void);

    // QList<QStringList> get_all_children_path_recurse(TreeItem *item,int mode);
    QList<QStringList> all_children_path_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode);

    //    bool is_field_name_available(QString name) const;
    //    QStringList field_name_available_list(void) const;
    QStringList field_name_for_crypt_list(void) const;

    //    QList<boost::intrusive_ptr<TreeItem>>   _child_items;   // Список ссылок на потомков
    boost::intrusive_ptr<TreeItem>          _parent_item;   // Ссылка на родителя

    //    // Таблица инфополей данной ветки
    //    QMap<QString, QString>                  _field_data;

    //    //    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    //    std::shared_ptr<RecordTable>            _record_table;    // = std::make_shared<TableData>();
    browser::WebPage    *_page;
    bool                _page_valid = false;
    int                 _position = -1;
    int                 _open_link_in_new_window = 0;
    //    friend void intrusive_ptr_add_ref(TreeItem *px);
    //    friend void intrusive_ptr_release(TreeItem *px);
    //    friend void boost::sp_adl_block::intrusive_ptr_add_ref< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;
    //    friend void boost::sp_adl_block::intrusive_ptr_release< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;

    void page_to_nullptr();   // {_page->record(nullptr); _page = nullptr; }
    bool is_holder();
    friend class browser::WebPage;

};

//inline void intrusive_ptr_add_ref(TreeItem *px){boost::sp_adl_block::intrusive_ptr_add_ref(px);}
//inline void intrusive_ptr_release(TreeItem *px){boost::sp_adl_block::intrusive_ptr_release(px);}

#endif
