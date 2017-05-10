#ifndef TREEITEM_H
#define TREEITEM_H

#include <QDomDocument>
#include <QDomElement>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <memory>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
// #include "models/record_table/linker.hxx"
// #include "models/tree/binder.hxx"
#include "models/record_table/items_flat.h"
#include "models/record_table/record.h"
// #include "libraries/GlobalParameters.h"

// class coupler;
class r_t;
class vector_t;
class i_t;
class tkm_t;

struct Binder;
struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;

// extern GlobalParameters globalparameters;

//// url_type

// struct url_full {
// QString operator()(QUrl url) {return url.toString();}
// };

// struct url_fragment {
// QString operator()(QUrl url) {return url.fragment();}
// };

namespace web {
	class WebPage;
	class WebView;
	class TabWidget;
	class Browser;
}

// i_t = {item |record + items }
// Record = {record | { id, name, url, dir,file, home, pin ...} }
// vector_t = { item[i] | i = 0,1,2,3 ... }
class i_t // : public std::enable_shared_from_this<TreeItem>
    : public boost::intrusive_ref_counter<i_t, boost::thread_safe_counter>,
      public r_t,
      public vector_t {
    public:
	// typedef typename Binder::bind_interface        bind_interface;
	// typedef typename Binder::activate_interface    activate_interface;
	// typedef typename Binder::item_interface        item_interface;
	// typedef typename Binder::page_interface        page_interface;

	// typedef typename Binder::bind_helper           bind_helper;
	// typedef typename Binder::activate_helper       activate_helper;
	// typedef typename Binder::item_helper           item_helper;
	// typedef typename Binder::page_helper           page_helper;

	typedef boost::sp_adl_block::thread_safe_counter counter_type; // boost::intrusive_ref_counter<TreeItem>::counter_type

	typedef boost::intrusive_ref_counter<i_t, boost::thread_safe_counter> counter;

	// struct linker : public boost::intrusive_ref_counter<linker,
	// boost::thread_safe_counter> {
	// public:

	// linker(boost::intrusive_ptr<TreeItem>  parentitem,
	// boost::intrusive_ptr<TreeItem>  childitem, int _pos = 0, int _mode =
	// ADD_NEW_RECORD_BEFORE);        //        boost::intrusive_ptr<TreeItem>
	// parent(boost::intrusive_ptr<TreeItem> it, int _pos = 0, int _mode =
	// ADD_NEW_RECORD_BEFORE);

	// ~linker();  //        boost::intrusive_ptr<TreeItem>
	// remove(boost::intrusive_ptr<TreeItem> _source_item);

	//// Возвращение ссылки на родительский элемент
	// boost::intrusive_ptr<TreeItem> parent_item()const {return _parent_item;}
	// boost::intrusive_ptr<TreeItem> child_item()const {return _child_item;}

	////        // Удаление потомков, начиная с позиции position массива
	///childItems
	////        bool remove(int position, int count);

	////        // Удаление всех потомков элемента
	////        void remove_all();

	//// Возвращает номер, под которым данный объект хранится
	//// в массиве childItems своего родителя
	// int sibling_order() const
	// {
	// int result = -1;

	// if(_parent_item)
	// result = _parent_item->_child_items.indexOf(_child_item); //
	// boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   //
	// shared_from_this()

	// return result;
	// }

	// int move_up(void)
	// {
	// int row = 0;
	//// Выясняется номер данного элемента в списке родителя
	// int num = sibling_order();

	// if(num > 0 && num < (_parent_item->count_direct())) {   // if(num ==
	// 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
	// row = -1;
	//// Элемент перемещается вверх по списку
	// (_parent_item->_child_items).swap(num, num + row);
	// }

	// return row;
	// }

	// int move_dn(void)
	// {
	// int row = 0;
	//// Выясняется номер данного элемента в списке родителя
	// int num = sibling_order();

	//// Если двигать вниз некуда, ничего делать ненужно
	// if(num > 0 && num < (_parent_item->count_direct())) { //if(num >=
	// (_parent_item->count_direct()))return false;

	// row = 1;
	//// Элемент перемещается вниз по списку
	// (_parent_item->_child_items).swap(num, num + row);
	// }

	// return row;
	// }

	// private:
	// boost::intrusive_ptr<TreeItem>  _parent_item;
	// boost::intrusive_ptr<TreeItem>  _child_item;
	// };

	// TreeItem(boost::intrusive_ptr<TreeItem>     _parent_item    // = nullptr
	// , QMap<QString, QString>           _field_data     // = QMap<QString,
	// QString>()
	// , boost::intrusive_ptr<TreeItem>   _child_items_root // copy semantic, not
	// good
	// );

	i_t(boost::intrusive_ptr<i_t> host_parent, // = nullptr	    //	    , QMap<QString, QString> field_data = QMap<QString,	    //QString>()
	    const full_fields_map& field_data_static_map_ = full_fields_map(),
	    const QDomElement& _dom_element = QDomElement(), int pos = 0,
	    int mode = add_new_record_before);

#ifdef _with_record_table
	TreeItem(boost::intrusive_ptr<TreeItem> _parent_item // = nullptr
	    ,
	    boost::intrusive_ptr<Record> _record // = nullptr
	    ,
	    const QDomElement& _dom_element = QDomElement());
#endif

	// TreeItem(const TreeItem &item);
	// TreeItem &operator =(const TreeItem &item);

	~i_t();

	static boost::intrusive_ptr<i_t>
	dangle_instance(const full_fields_map& field_data_map_ = full_fields_map(), const QDomElement& _dom_element = QDomElement());
	//// Возвращение ссылки на потомка, который хранится в списке childItems
	//// под указанным номером
	// boost::intrusive_ptr<TreeItem> item_direct(int number);

	// int direct_children_count()const;
	//// Возвращение количества потомков (т.е. количество записей в списке
	///childItems)
	// int size() const;

	// Получение значения поля по имени
	template <typename field_key_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type field() const;
	// {return Record::field<field_type>();} // , typename field_type_switch =
	// typename switch_type<field_type>::type

	// , field_type_switch   // field_content;

	template <typename field_key_type>
	void field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type& value);
	//{Record::field<field_type>(value);} // , typename field_type_switch =
	//typename switch_type<field_type>::type

	// , field_type_switch

	// Заполнение указанного поля
	// void field(QString name, QString value);

	// Заполнение указанного поля данными напрямую, без преобразований
	// void field_direct(QString name, QString value);

	//// Добавление нового подчиненного элемента
	//// в конец списка подчиненных элементов
	// boost::intrusive_ptr<TreeItem> child_add_new(int pos, QString id, QString
	// name);
	boost::intrusive_ptr<i_t> contains_direct(const std::function<bool(boost::intrusive_ptr<const Linker>)>&& _equal) const;
	boost::intrusive_ptr<i_t>
	contains_direct(boost::intrusive_ptr<const i_t>&& _item) const;
	boost::intrusive_ptr<i_t>
	contains_direct(boost::intrusive_ptr<const Linker>&& _item_linker) const;
	// boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record>
	// item);
	boost::intrusive_ptr<i_t> operator<<(boost::intrusive_ptr<i_t> _item);

	// boost::intrusive_ptr<linker> child_rent(boost::intrusive_ptr<TreeItem>
	// item); // break parent contains semantic, make contians_direct does not
	// work!!!

	// Добавление потомка (потомков) к текущему элементу
	// position - после какой позиции массива childItems вставить
	// count - сколько потомков вставить (обычно 1, но можно и несколько)
	// columns - сколько столбцов должен содержать потомок
	QList<boost::intrusive_ptr<i_t>> children_insert_new(int position, int count, int columns);

	void binder_reset();
	boost::intrusive_ptr<i_t> delete_permanent_recursive(boost::intrusive_ptr<Linker> _to_be_removed, std::function<bool(boost::intrusive_ptr<const i_t>)> condition);

	// Возвращение ссылки на родительский элемент
	boost::intrusive_ptr<i_t> parent() const;

	// Удаление потомков, начиная с позиции position массива childItems
	QList<boost::intrusive_ptr<i_t>> delete_permanent_recursive(int position, int count);

	boost::intrusive_ptr<Linker> dangle();
	// Удаление всех потомков элемента
	void clear();

	// int sibling_order(boost::intrusive_ptr<TreeItem> it)const;
	int sibling_order(const std::function<bool(boost::intrusive_ptr<const Linker>)>& _equal) const;
	//// Возвращает номер, под которым данный объект хранится
	//// в массиве childItems своего родителя
	// int sibling_order() const;

	int move_up(void);
	int move_dn(void);
	void move_up(int pos);

	void move_dn(int pos);

	//// Возвращает id путь (список идентификаторов от корня до текущего элемента)
	// QStringList path_absolute(void) const;

	// Возвращает путь в виде названий веток дерева
	template <typename key = id_key>
	QStringList path_list() const;
	template <typename key>
	QString path_string(QString delimeter) const;

	//// Возвращает массив путей всех подветок, которые содержит ветка
	// QList<QStringList> path_children_all(void) const;

	// Возвращает набор значений указанного поля для подветок
	template <typename key = id_key>
	QList<QStringList> path_children_all() const; //QString field_name = "id"

	// Получение идентификатора элемента
	// id_value	id() const;
	// QString		name() const;

	// Получение идентификатора родительской ветки
	id_value parent_id() const;

	// Шифрация данной ветки и всех подветок
	void to_encrypt(void);

	// асшифровка данной ветки и всех подветок
	void to_decrypt(void);

	//// Взятие количества записей в таблице конечных записей, "промежуточный"
	///метод
	// int size(void);

	//// Удаление всех элементов в таблице конечных записей, "промежуточный" метод
	// void clear_tabledata(void);

	//// Преобразование таблицы конечных записей в DOM представление,
	///"промежуточный" метод
	//// Convert the table of final entries in the DOM representation
	///"intermediate" method

	// QDomElement dom_from_treeitem(std::shared_ptr<QDomDocument> doc);
	QDomElement dom_from_treeitem();
	// void dom_to_direct(const QDomElement &_dom_element);
	void dom_to_records(const QDomElement& _record_dom_element);

	// boost::intrusive_ptr<const TreeItem> is_activated() const;
	// boost::intrusive_ptr<const TreeItem> is_registered_to_browser() const;

	boost::intrusive_ptr<Binder> binder();
	const boost::intrusive_ptr<Binder>&& binder() const;
	void binder(boost::intrusive_ptr<Binder>&& binder_);

	boost::intrusive_ptr<Linker> linker();
	const boost::intrusive_ptr<Linker>&& linker() const;
	void linker(boost::intrusive_ptr<Linker>&& up_linker_);

//// Взятие ссылки на данные конечных записей
// std::shared_ptr<RecordTable> record_table(void);
// void record_table(std::shared_ptr<RecordTable> _table_data);
//// Первичное заполнение таблицы конечных записей, "промежуточный" метод
// void record_table(QDomElement i_dom_element);

#ifdef _with_record_table // deprecated
	void records_to_children();
#endif

	// void children_clear(void);

	web::WebPage* page() const; // const; // {return _page;}
	boost::intrusive_ptr<i_t> host() const;
	web::WebView* bind(web::WebPage *check_page = nullptr);
	// boost::intrusive_ptr<TreeItem> bind(web::WebPage *page);  // {_page = page;
	// _page->record(this);}
	web::WebView*
	activate(const std::function<web::WebView*(const std::function<bool(boost::intrusive_ptr<const ::Binder>)>& _equal)>& find_activated, bool force_reload = false);

	// void binder(bind_helper g); // {_binder = g;}
	// bind_helper binder() const; // {return _binder;}

	// void activator(activate_helper a);    // {_activator = a;}
	// activate_helper activator() const;    // {return _activator;}

	// void active_request(PosSource pos, int openLinkIn);
	// bool page_valid() const;// {return _page_valid;}
	// operator ItemsFlat() {return *this;}

	// deprecated
	// boost::intrusive_ptr<TreeItem> active_subset() const;

	// int shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> it, int mode =
	// ADD_NEW_RECORD_AFTER);

	bool is_empty() const;

	boost::intrusive_ptr<i_t> delete_permanent(
	    const std::function<bool(boost::intrusive_ptr<const Linker>)>& _equal);
	boost::intrusive_ptr<i_t> delete_permanent_recursive(
	    std::function<bool(boost::intrusive_ptr<const i_t>)> condition);
	// boost::intrusive_ptr<TreeItem> delete_permanent_recursive_empty();
	boost::intrusive_ptr<i_t> sibling() const;
	boost::intrusive_ptr<i_t> merge(boost::intrusive_ptr<i_t> cut);
	int count_children_recursive();

	// template<typename T = url_full>
	// inline QString url() const;

	// deprecated because of conflicting with
	// parent(boost::intrusive_ptr<TreeItem>)
	// boost::intrusive_ptr<TreeItem>
	// child_move_unique(boost::intrusive_ptr<TreeItem> _source_item, int pos, int
	// mode = ADD_NEW_RECORD_AFTER);    // ADD_NEW_RECORD_TO_END
	void traverse_direct(
	    const std::function<void(boost::intrusive_ptr<Linker>)>& operation);

	//// Function to populate a table of DOM-document // Функция заполнения
	///таблицы из DOM-документа
	// void dom_to_itemsflat(const QDomElement &dom_model);
	boost::intrusive_ptr<const i_t>
	is_ancestor_of(boost::intrusive_ptr<const i_t> reference);
	int distance(boost::intrusive_ptr<const i_t> reference);

	full_fields_map& fields_data_map();
	QString field_dynamic(const QString& key_name) const;
	void field_dynamic(const QString& key_name, const QString& value_);
    public slots:
	void topic_append(const QString& new_topic_);

    protected:
	// bind_helper     _binder;
	// activate_helper   _activator;
	// coupler         _coupler;
	std::function<QDomElement(std::shared_ptr<QDomDocument> doc)> dom_from_treeitem_impl;
	std::function<void()> children_parent_as_this;

    private:
	// bool children_remove_link(int position, int count);

	void isolate(void);

	//// Возвращает набор значений указанного поля для пути от корня к ветке
	// std::function<QStringList(QString field_name)> path_absolute_as_field;
	// // std::function<QStringList (QString field_name)const>

	// QList<QStringList> get_all_children_path_recurse(TreeItem *item, int mode);
	// std::function<QList<QStringList> (boost::intrusive_ptr<const TreeItem>
	// item, QString field_name, int mode)> path_children_all_as_field_impl;    //
	// std::function<QList<QStringList> (boost::intrusive_ptr<const TreeItem>
	// item, QString field_name, int mode)const>

	// bool is_field_name_available(QString name) const;
	// QStringList field_name_available_list(void) const;
	QStringList field_name_for_crypt_list(void) const;

	// QList<boost::intrusive_ptr<TreeItem>>   _child_items;   // Список ссылок на
	// потомков
	// boost::intrusive_ptr<TreeItem>  _parent_item;   // Ссылка на родителя
	boost::intrusive_ptr<Linker> _linker; // = nullptr

	//// Таблица инфополей данной ветки
	// QMap<QString, QString>                  _field_data;

	////    // Each branch can contain a table of final entries // Каждая ветка
	///может содержать таблицу конечных записей
	// std::shared_ptr<RecordTable>            _record_table;    // =
	// std::make_shared<TableData>();

	// web::WebPage    *_page;
	boost::intrusive_ptr<Binder> _binder; // = nullptr
	// bool                            _page_valid = false;
	// PosSource                       _position;
	// int                             _open_link_in_new_window = 0;

	// friend void intrusive_ptr_add_ref(TreeItem *px);
	// friend void intrusive_ptr_release(TreeItem *px);
	// friend void boost::sp_adl_block::intrusive_ptr_add_ref< TreeItem,
	// counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type >
	// *p) BOOST_NOEXCEPT;
	// friend void boost::sp_adl_block::intrusive_ptr_release< TreeItem,
	// counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type >
	// *p) BOOST_NOEXCEPT;

	// void page_to_nullptr();   // {_page->record(nullptr); _page = nullptr; }
	// bool is_holder() const;

	// friend class web::WebPage;
	friend class vector_t;
	friend class tkm_t;
	// friend class TreeScreen;

	i_t(const full_fields_map& field_data_static_map_, const QDomElement& _dom_element = QDomElement());
};

template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, dynamic_name_key>::type>::type i_t::field<dynamic_name_key>() const;
// inline void intrusive_ptr_add_ref(TreeItem
// *px){boost::sp_adl_block::intrusive_ptr_add_ref(px);}
// inline void intrusive_ptr_release(TreeItem
// *px){boost::sp_adl_block::intrusive_ptr_release(px);}

// template<>
// inline QString TreeItem::url<url_fragment>()const {return
// QUrl(field("url")).fragment();}

// template<>
// inline QString TreeItem::url<url_full>() const    {return field("url");}

//#include "models/tree/tree_item.inl"

#ifndef EXTERN_DECLARATION_GET_FIELD
#define EXTERN_DECLARATION_GET_FIELD(key_type)                                                                                             \
	extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type \
	i_t::field<key_type>() const;
#endif // EXTERN_DECLARATION_GET_FIELD

// extern template typename boost::fusion::result_of::at_key<full_field_map,
// id_key>::type i_t::field<id_key>() const;

EXTERN_DECLARATION_GET_FIELD(id_key)
EXTERN_DECLARATION_GET_FIELD(pin_key)
EXTERN_DECLARATION_GET_FIELD(rating_key)
EXTERN_DECLARATION_GET_FIELD(name_key)
EXTERN_DECLARATION_GET_FIELD(author_key)
EXTERN_DECLARATION_GET_FIELD(home_key)
EXTERN_DECLARATION_GET_FIELD(url_key)
EXTERN_DECLARATION_GET_FIELD(tags_key)
EXTERN_DECLARATION_GET_FIELD(ctime_key)
EXTERN_DECLARATION_GET_FIELD(dir_key)
EXTERN_DECLARATION_GET_FIELD(file_key)
EXTERN_DECLARATION_GET_FIELD(crypt_key)
EXTERN_DECLARATION_GET_FIELD(has_attach_key)
EXTERN_DECLARATION_GET_FIELD(attach_size_key)

// extern template decltype(auto) i_t::field<pin_key>() const;
// extern template decltype(auto) i_t::field<rating_key>() const;
// extern template decltype(auto) i_t::field<name_key>() const;
// extern template decltype(auto) i_t::field<author_key>() const;
// extern template decltype(auto) i_t::field<home_key>() const;
// extern template decltype(auto) i_t::field<url_key>() const;
// extern template decltype(auto) i_t::field<tags_key>() const;
// extern template decltype(auto) i_t::field<ctime_key>() const;
// extern template decltype(auto) i_t::field<dir_key>() const;
// extern template decltype(auto) i_t::field<file_key>() const;
// extern template decltype(auto) i_t::field<crypt_key>() const;
// extern template decltype(auto) i_t::field<has_attach_key>() const;
// extern template decltype(auto) i_t::field<attach_size_key>() const;
//// extern template decltype(auto) TreeItem::field<dynamic_name_key>() const;

#ifndef EXTERN_DECLARATION_SET_FIELD
#define EXTERN_DECLARATION_SET_FIELD(key_type) \
	extern template void i_t::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type& value);
#endif // EXTERN_DECLARATION_SET_FIELD

EXTERN_DECLARATION_SET_FIELD(id_key)
EXTERN_DECLARATION_SET_FIELD(pin_key)
EXTERN_DECLARATION_SET_FIELD(rating_key)
EXTERN_DECLARATION_SET_FIELD(name_key)
EXTERN_DECLARATION_SET_FIELD(author_key)
EXTERN_DECLARATION_SET_FIELD(home_key)
EXTERN_DECLARATION_SET_FIELD(url_key)
EXTERN_DECLARATION_SET_FIELD(tags_key)
EXTERN_DECLARATION_SET_FIELD(ctime_key)
EXTERN_DECLARATION_SET_FIELD(dir_key)
EXTERN_DECLARATION_SET_FIELD(file_key)
EXTERN_DECLARATION_SET_FIELD(crypt_key)
EXTERN_DECLARATION_SET_FIELD(has_attach_key)
EXTERN_DECLARATION_SET_FIELD(attach_size_key)

// extern template void i_t::field<id_key>(const id_key& value);
// extern template void i_t::field<pin_key>(const pin_key& value);
// extern template void i_t::field<rating_key>(const rating_key& value);
// extern template void i_t::field<name_key>(const name_key& value);
// extern template void i_t::field<author_key>(const author_key& value);
// extern template void i_t::field<home_key>(const home_key& value);
// extern template void i_t::field<url_key>(const url_key& value);
// extern template void i_t::field<tags_key>(const tags_key& value);
// extern template void i_t::field<ctime_key>(const ctime_key& value);
// extern template void i_t::field<dir_key>(const dir_key& value);
// extern template void i_t::field<file_key>(const file_key& value);
// extern template void i_t::field<crypt_key>(const crypt_key& value);
// extern template void i_t::field<has_attach_key>(const has_attach_key& value);
// extern template void i_t::field<attach_size_key>(const attach_size_key& value);
//// extern template void TreeItem::field<dynamic_name_key>(const dynamic_name_key &value);
#ifndef EXTREN_GET_GROUP_ELEMENT
#define EXTREN_GET_GROUP_ELEMENT(return_type, function_name, key, ...) \
	extern template return_type function_name<key>(__VA_ARGS__) const;
#endif

#ifndef EXTREN_GET_GROUP
#define EXTREN_GET_GROUP(return_type, function_name, ...)                                 \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, id_key, __VA_ARGS__)         \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, pin_key, __VA_ARGS__)        \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, rating_key, __VA_ARGS__)     \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, name_key, __VA_ARGS__)       \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, author_key, __VA_ARGS__)     \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, home_key, __VA_ARGS__)       \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, url_key, __VA_ARGS__)        \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, tags_key, __VA_ARGS__)       \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, ctime_key, __VA_ARGS__)      \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, dir_key, __VA_ARGS__)        \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, file_key, __VA_ARGS__)       \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, crypt_key, __VA_ARGS__)      \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, has_attach_key, __VA_ARGS__) \
	EXTREN_GET_GROUP_ELEMENT(return_type, function_name, attach_size_key, __VA_ARGS__)

#endif

EXTREN_GET_GROUP(QStringList, i_t::path_list)
EXTREN_GET_GROUP(QString, i_t::path_string, QString)
EXTREN_GET_GROUP(QList<QStringList>, i_t::path_children_all)

#endif // TREEITEM_H
