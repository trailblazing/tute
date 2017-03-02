#ifndef TREE_ITEM_DEF
#define TREE_ITEM_DEF

#include <QString>

#include "models/record_table/record.inl" // very important!!! otherwise Record::field<> is undefined
#include "models/tree/tree_item.h"
class i_t;

template <typename field_key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_key_type>::type>::type i_t::field() const { return Record::field<field_key_type>(); }

template <typename field_key_type>
void i_t::field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_key_type>::type>::type& value) { Record::field<field_key_type>(value); }

#ifndef EXTERN_DECLARATION_GET_FIELD
#define EXTERN_DECLARATION_GET_FIELD(key_type) \
    extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type i_t::field<key_type>() const;
#endif //EXTERN_DECLARATION_GET_FIELD

//extern template typename boost::fusion::result_of::at_key<full_field_map, id_type>::type i_t::field<id_type>() const;

EXTERN_DECLARATION_GET_FIELD(id_type)
EXTERN_DECLARATION_GET_FIELD(pin_type)
EXTERN_DECLARATION_GET_FIELD(rating_type)
EXTERN_DECLARATION_GET_FIELD(name_type)
EXTERN_DECLARATION_GET_FIELD(author_type)
EXTERN_DECLARATION_GET_FIELD(home_type)
EXTERN_DECLARATION_GET_FIELD(url_type)
EXTERN_DECLARATION_GET_FIELD(tags_type)
EXTERN_DECLARATION_GET_FIELD(ctime_type)
EXTERN_DECLARATION_GET_FIELD(dir_type)
EXTERN_DECLARATION_GET_FIELD(file_type)
EXTERN_DECLARATION_GET_FIELD(crypt_type)
EXTERN_DECLARATION_GET_FIELD(has_attach_type)
EXTERN_DECLARATION_GET_FIELD(attach_size_type)

//extern template decltype(auto) i_t::field<pin_type>() const;
//extern template decltype(auto) i_t::field<rating_type>() const;
//extern template decltype(auto) i_t::field<name_type>() const;
//extern template decltype(auto) i_t::field<author_type>() const;
//extern template decltype(auto) i_t::field<home_type>() const;
//extern template decltype(auto) i_t::field<url_type>() const;
//extern template decltype(auto) i_t::field<tags_type>() const;
//extern template decltype(auto) i_t::field<ctime_type>() const;
//extern template decltype(auto) i_t::field<dir_type>() const;
//extern template decltype(auto) i_t::field<file_type>() const;
//extern template decltype(auto) i_t::field<crypt_type>() const;
//extern template decltype(auto) i_t::field<has_attach_type>() const;
//extern template decltype(auto) i_t::field<attach_size_type>() const;
//// extern template decltype(auto) TreeItem::field<dynamic_name_type>() const;

#ifndef EXTERN_DECLARATION_SET_FIELD
#define EXTERN_DECLARATION_SET_FIELD(key_type) \
    extern template void i_t::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type& value);
#endif //EXTERN_DECLARATION_SET_FIELD

EXTERN_DECLARATION_SET_FIELD(id_type)
EXTERN_DECLARATION_SET_FIELD(pin_type)
EXTERN_DECLARATION_SET_FIELD(rating_type)
EXTERN_DECLARATION_SET_FIELD(name_type)
EXTERN_DECLARATION_SET_FIELD(author_type)
EXTERN_DECLARATION_SET_FIELD(home_type)
EXTERN_DECLARATION_SET_FIELD(url_type)
EXTERN_DECLARATION_SET_FIELD(tags_type)
EXTERN_DECLARATION_SET_FIELD(ctime_type)
EXTERN_DECLARATION_SET_FIELD(dir_type)
EXTERN_DECLARATION_SET_FIELD(file_type)
EXTERN_DECLARATION_SET_FIELD(crypt_type)
EXTERN_DECLARATION_SET_FIELD(has_attach_type)
EXTERN_DECLARATION_SET_FIELD(attach_size_type)

//extern template void i_t::field<id_type>(const id_type& value);
//extern template void i_t::field<pin_type>(const pin_type& value);
//extern template void i_t::field<rating_type>(const rating_type& value);
//extern template void i_t::field<name_type>(const name_type& value);
//extern template void i_t::field<author_type>(const author_type& value);
//extern template void i_t::field<home_type>(const home_type& value);
//extern template void i_t::field<url_type>(const url_type& value);
//extern template void i_t::field<tags_type>(const tags_type& value);
//extern template void i_t::field<ctime_type>(const ctime_type& value);
//extern template void i_t::field<dir_type>(const dir_type& value);
//extern template void i_t::field<file_type>(const file_type& value);
//extern template void i_t::field<crypt_type>(const crypt_type& value);
//extern template void i_t::field<has_attach_type>(const has_attach_type& value);
//extern template void i_t::field<attach_size_type>(const attach_size_type& value);
//// extern template void TreeItem::field<dynamic_name_type>(const dynamic_name_type &value);

#ifndef DEFINITION_GET_FIELD
#define DEFINITION_GET_FIELD(key_type) \
    template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type i_t::field<key_type>() const;
#endif //DEFINITION_GET_FIELD

DEFINITION_GET_FIELD(id_type)
DEFINITION_GET_FIELD(pin_type)
DEFINITION_GET_FIELD(rating_type)
DEFINITION_GET_FIELD(name_type)
DEFINITION_GET_FIELD(author_type)
DEFINITION_GET_FIELD(home_type)
DEFINITION_GET_FIELD(url_type)
DEFINITION_GET_FIELD(tags_type)
DEFINITION_GET_FIELD(ctime_type)
DEFINITION_GET_FIELD(dir_type)
DEFINITION_GET_FIELD(file_type)
DEFINITION_GET_FIELD(crypt_type)
DEFINITION_GET_FIELD(has_attach_type)
DEFINITION_GET_FIELD(attach_size_type)

//template decltype(auto) i_t::field<id_type>() const;
//template decltype(auto) i_t::field<pin_type>() const;
//template decltype(auto) i_t::field<rating_type>() const;
//template decltype(auto) i_t::field<name_type>() const;
//template decltype(auto) i_t::field<author_type>() const;
//template decltype(auto) i_t::field<home_type>() const;
//template decltype(auto) i_t::field<url_type>() const;
//template decltype(auto) i_t::field<tags_type>() const;
//template decltype(auto) i_t::field<ctime_type>() const;
//template decltype(auto) i_t::field<dir_type>() const;
//template decltype(auto) i_t::field<file_type>() const;
//template decltype(auto) i_t::field<crypt_type>() const;
//template decltype(auto) i_t::field<has_attach_type>() const;
//template decltype(auto) i_t::field<attach_size_type>() const;
//// template decltype(auto) TreeItem::field<dynamic_name_type>() const;

#ifndef DEFINITION_SET_FIELD
#define DEFINITION_SET_FIELD(key_type) \
    template void i_t::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type& value);
#endif //DEFINITION_SET_FIELD

DEFINITION_SET_FIELD(id_type)
DEFINITION_SET_FIELD(pin_type)
DEFINITION_SET_FIELD(rating_type)
DEFINITION_SET_FIELD(name_type)
DEFINITION_SET_FIELD(author_type)
DEFINITION_SET_FIELD(home_type)
DEFINITION_SET_FIELD(url_type)
DEFINITION_SET_FIELD(tags_type)
DEFINITION_SET_FIELD(ctime_type)
DEFINITION_SET_FIELD(dir_type)
DEFINITION_SET_FIELD(file_type)
DEFINITION_SET_FIELD(crypt_type)
DEFINITION_SET_FIELD(has_attach_type)
DEFINITION_SET_FIELD(attach_size_type)

//template void i_t::field<id_type>(const id_type& value);
//template void i_t::field<pin_type>(const pin_type& value);
//template void i_t::field<rating_type>(const rating_type& value);
//template void i_t::field<name_type>(const name_type& value);
//template void i_t::field<author_type>(const author_type& value);
//template void i_t::field<home_type>(const home_type& value);
//template void i_t::field<url_type>(const url_type& value);
//template void i_t::field<tags_type>(const tags_type& value);
//template void i_t::field<ctime_type>(const ctime_type& value);
//template void i_t::field<dir_type>(const dir_type& value);
//template void i_t::field<file_type>(const file_type& value);
//template void i_t::field<crypt_type>(const crypt_type& value);
//template void i_t::field<has_attach_type>(const has_attach_type& value);
//template void i_t::field<attach_size_type>(const attach_size_type& value);
//// template void TreeItem::field<dynamic_name_type>(const dynamic_name_type &value);

#endif // TREE_ITEM_DEF
