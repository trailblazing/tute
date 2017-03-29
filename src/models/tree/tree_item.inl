#ifndef TREE_ITEM_DEF
#define TREE_ITEM_DEF

#include <QString>

#include "models/record_table/record.inl"  // very important!!! otherwise Record::field<> is undefined
#include "models/tree/tree_item.h"
class i_t;

template <typename field_key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type i_t::field() const {
	return r_t::field<field_key_type>();
}

template <typename field_key_type>
void i_t::field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type &value) {
	r_t::field<field_key_type>(value);
}

#ifndef DEFINITION_GET_FIELD
#define DEFINITION_GET_FIELD(key_type)                                                                                          \
	template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type \
	i_t::field<key_type>() const;
#endif  // DEFINITION_GET_FIELD

DEFINITION_GET_FIELD(id_key)
DEFINITION_GET_FIELD(pin_key)
DEFINITION_GET_FIELD(rating_key)
DEFINITION_GET_FIELD(name_key)
DEFINITION_GET_FIELD(author_key)
DEFINITION_GET_FIELD(home_key)
DEFINITION_GET_FIELD(url_key)
DEFINITION_GET_FIELD(tags_key)
DEFINITION_GET_FIELD(ctime_key)
DEFINITION_GET_FIELD(dir_key)
DEFINITION_GET_FIELD(file_key)
DEFINITION_GET_FIELD(crypt_key)
DEFINITION_GET_FIELD(has_attach_key)
DEFINITION_GET_FIELD(attach_size_key)

// template decltype(auto) i_t::field<id_key>() const;
// template decltype(auto) i_t::field<pin_key>() const;
// template decltype(auto) i_t::field<rating_key>() const;
// template decltype(auto) i_t::field<name_key>() const;
// template decltype(auto) i_t::field<author_key>() const;
// template decltype(auto) i_t::field<home_key>() const;
// template decltype(auto) i_t::field<url_key>() const;
// template decltype(auto) i_t::field<tags_key>() const;
// template decltype(auto) i_t::field<ctime_key>() const;
// template decltype(auto) i_t::field<dir_key>() const;
// template decltype(auto) i_t::field<file_key>() const;
// template decltype(auto) i_t::field<crypt_key>() const;
// template decltype(auto) i_t::field<has_attach_key>() const;
// template decltype(auto) i_t::field<attach_size_key>() const;
//// template decltype(auto) TreeItem::field<dynamic_name_key>() const;

#ifndef DEFINITION_SET_FIELD
#define DEFINITION_SET_FIELD(key_type) \
	template void i_t::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type &value);
#endif  // DEFINITION_SET_FIELD

DEFINITION_SET_FIELD(id_key)
DEFINITION_SET_FIELD(pin_key)
DEFINITION_SET_FIELD(rating_key)
DEFINITION_SET_FIELD(name_key)
DEFINITION_SET_FIELD(author_key)
DEFINITION_SET_FIELD(home_key)
DEFINITION_SET_FIELD(url_key)
DEFINITION_SET_FIELD(tags_key)
DEFINITION_SET_FIELD(ctime_key)
DEFINITION_SET_FIELD(dir_key)
DEFINITION_SET_FIELD(file_key)
DEFINITION_SET_FIELD(crypt_key)
DEFINITION_SET_FIELD(has_attach_key)
DEFINITION_SET_FIELD(attach_size_key)

// template void i_t::field<id_key>(const id_key& value);
// template void i_t::field<pin_key>(const pin_key& value);
// template void i_t::field<rating_key>(const rating_key& value);
// template void i_t::field<name_key>(const name_key& value);
// template void i_t::field<author_key>(const author_key& value);
// template void i_t::field<home_key>(const home_key& value);
// template void i_t::field<url_key>(const url_key& value);
// template void i_t::field<tags_key>(const tags_key& value);
// template void i_t::field<ctime_key>(const ctime_key& value);
// template void i_t::field<dir_key>(const dir_key& value);
// template void i_t::field<file_key>(const file_key& value);
// template void i_t::field<crypt_key>(const crypt_key& value);
// template void i_t::field<has_attach_key>(const has_attach_key& value);
// template void i_t::field<attach_size_key>(const attach_size_key& value);
//// template void TreeItem::field<dynamic_name_key>(const dynamic_name_key
///&value);

// Путь к элементу (в виде списка названий веток)
template <typename key>
QStringList i_t::path_list() const {
	// auto path_absolute_as_field = [&](QString field_name)->QStringList { //
	// const
	QStringList path;
	boost::intrusive_ptr<const i_t> current_item(this);  // = boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))
	// // shared_from_this()

	path << detail::to_qstring(boost::fusion::at_key<key>(current_item->_fields_data_map));
	while (current_item->parent()) {  // != nullptr
		current_item = current_item->parent();
		path << detail::to_qstring(boost::fusion::at_key<key>(current_item->_fields_data_map));
	}
	// Rotate backwards array identifiers advance   // Поворот массива идентификаторов задом наперед
	int k = path.size() - 1;
	int j = path.size() / 2;
	for (int i = 0; i < j; ++i)
		path.swap(i, k - i);
	return path;
	// };
	// return path_absolute_as_field(field_name);  // "id", "name"
}

// Путь к элементу в виде строки, разделенной указанным разделителем
template <typename key>
QString i_t::path_string(QString delimeter) const {
	QStringList path = path_list<key>();  // "name"

	// Убирается пустой элемент, если он есть (это может быть корень, у него нет
	// названия)
	int emptyStringIndex = path.indexOf("");
	path.removeAt(emptyStringIndex);

	return path.join(delimeter);
}

template <typename key>  //struct
QList<QStringList>
path_children_all_as_field_impl(boost::intrusive_ptr<const i_t> item, int mode) {
	//=
	//	QList<QStringList> operator()(boost::intrusive_ptr<const i_t> item, int mode)  // -> QList<QStringList>
	//	{                                                                              //QString field_name,
	// const
	static QList<QStringList> path_list;
	// Если дана команда очистить список путей
	if (mode == 0) {
		path_list.clear();

		return QList<QStringList>();
	}
	for (int i = 0; i < item->count_direct(); i++) {
		auto it = item->child_direct(i);
		QStringList path = it->path_list<key>();
		path_list << path;
		path_children_all_as_field_impl<key>(it, 1);  // 2?
	}
	if (mode == 1)
		return path_list;
	else
		return QList<QStringList>();
}
//};

template <typename key>
QList<QStringList> i_t::path_children_all() const {  //QString field_name
	// std::function<QList<QStringList>(boost::intrusive_ptr<TreeItem> item,
	// QString fieldName, int mode)> all_children_path_as_field =
	// [&](boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)
	//// Возвращает массив указанных полей всех подветок, которые содержит ветка
	//// Внутренняя рекурсивная функция
	////            QList<QStringList> TreeItem::()
	// {
	// static QList<QStringList> pathList;

	//// Если дана команда очистить список путей
	// if(mode == 0) {
	// pathList.clear();
	// return QList<QStringList>();
	// }

	// for(int i = 0; i < (item->child_count()); i++) {
	// QStringList path = (item->child(i))->path_as_field(fieldName);
	// pathList << path;
	// all_children_path_as_field(item->child(i), fieldName, 2);
	// }

	// if(mode == 1)return pathList;
	// else return QList<QStringList>();
	// };

	// Возвращает массив указанных полей всех подветок, которые содержит ветка
	// Внутренняя рекурсивная функция
	//	std::function<QList<QStringList>(boost::intrusive_ptr<const i_t>, int)>  //QString,

	// Очищение списка путей
	path_children_all_as_field_impl<key>(this, 0);

	// Получение списка путей
	QList<QStringList> pathList = path_children_all_as_field_impl<key>(this, 1);

	return pathList;
}

#ifndef GET_GROUP_ELEMENT
#define GET_GROUP_ELEMENT(return_type, function_name, key, ...) \
	template return_type function_name<key>(__VA_ARGS__) const;
#endif

#ifndef GET_GROUP
#define GET_GROUP(return_type, function_name, ...)                             \
	GET_GROUP_ELEMENT(return_type, function_name, id_key, __VA_ARGS__)         \
	GET_GROUP_ELEMENT(return_type, function_name, pin_key, __VA_ARGS__)        \
	GET_GROUP_ELEMENT(return_type, function_name, rating_key, __VA_ARGS__)     \
	GET_GROUP_ELEMENT(return_type, function_name, name_key, __VA_ARGS__)       \
	GET_GROUP_ELEMENT(return_type, function_name, author_key, __VA_ARGS__)     \
	GET_GROUP_ELEMENT(return_type, function_name, home_key, __VA_ARGS__)       \
	GET_GROUP_ELEMENT(return_type, function_name, url_key, __VA_ARGS__)        \
	GET_GROUP_ELEMENT(return_type, function_name, tags_key, __VA_ARGS__)       \
	GET_GROUP_ELEMENT(return_type, function_name, ctime_key, __VA_ARGS__)      \
	GET_GROUP_ELEMENT(return_type, function_name, dir_key, __VA_ARGS__)        \
	GET_GROUP_ELEMENT(return_type, function_name, file_key, __VA_ARGS__)       \
	GET_GROUP_ELEMENT(return_type, function_name, crypt_key, __VA_ARGS__)      \
	GET_GROUP_ELEMENT(return_type, function_name, has_attach_key, __VA_ARGS__) \
	GET_GROUP_ELEMENT(return_type, function_name, attach_size_key, __VA_ARGS__)

#endif

GET_GROUP(QStringList, i_t::path_list)
GET_GROUP(QString, i_t::path_string, QString)
GET_GROUP(QList<QStringList>, i_t::path_children_all)

#endif  // TREE_ITEM_DEF
