#ifndef RECORD_DEF
#define RECORD_DEF
#include "models/record_table/record.h"
#include <QString>
class Record;

template <typename field_key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, field_key_type>::type>::type Record::natural_field_source() const
{ //QString name
// Если имя поля недопустимо
#ifdef TEST
    BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set, concrete>
                                  :
                                  : type::value),
        ==, true); // critical_error("Record::natural_field_source() : get unavailable field " + name);
    BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set, concrete>));
#endif //TEST
    BOOST_MPL_ASSERT((boost::mpl::contains<natural_field_set, field_key_type>));
    //    // Если поле с таким названием есть
    //    if (_field_data.contains(boost::mpl::c_str<concrete>::value))
    //        return _field_data[boost::mpl::c_str<concrete>::value]; // Возвращается значение поля
    //    else
    //        return QString();
    return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<natural_field_set, field_key_type>::type>::type>(_field_data_map);
}

template <typename field_key_type>
void Record::natural_field_source(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, field_key_type>::type>::type& value)
{ //QString name,, concrete cc
//		(void) cc;
// Если имя поля недопустимо
#ifdef TEST
    BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set, concrete>
                                  :
                                  : type::value),
        ==, true); //if(fixedparameters.is_record_field_natural(name) == false) critical_error("In Record::natural_field_source() unavailable field name " + name + " try set to " + value);
    BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set, concrete>));
#endif //TEST
    BOOST_MPL_ASSERT((boost::mpl::contains<natural_field_set, field_key_type>));
    //    // Устанавливается значение поля
    //    _field_data.insert(boost::mpl::c_str<field_type>::value, value);
    boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<natural_field_set, field_key_type>::type>::type>(_field_data_map) = value;
}

template <typename field_type>
struct Record::field_read<field_type, append_to_crypt_set> {
    const Record* _this;
    field_read(const Record* _this)
        : _this(_this)
    {
    }

    typename boost::fusion::result_of::at_key<full_field_map const, typename boost::mpl::at<append_to_crypt_set, field_type>::type>::type const operator()() const
    {
	return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<append_to_crypt_set, field_type>::type>::type>(_this->_field_data_map); //_this->_field_data_map[boost::mpl::c_str < typename boost::mpl::at < append_to_crypt_set, field_type > ::type > ::value];
    }
};

template <typename field_type>
struct Record::field_read<field_type, crypt_field_set> {
    const Record* _this;
    field_read(const Record* _this)
        : _this(_this)
    {
    }

    typename boost::fusion::result_of::at_key<crypt_field_map const, typename boost::mpl::at<crypt_field_set, field_type>::type>::type const operator()() const
    {
	return _this->read_crypt_field<typename boost::remove_reference<typename boost::mpl::at<crypt_field_set, field_type>::type>::type>(); //boost::mpl::c_str<typename boost::mpl::at<crypt_field_set, field_type>::type>::value
    } // {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
};

template <typename field_type>
struct Record::field_read<field_type, calculable_field_set> {
    const Record* _this;
    field_read(const Record* _this)
        : _this(_this)
    {
    }

    typename boost::fusion::result_of::at_key<calculable_field_map const, typename boost::mpl::at<calculable_field_set, field_type>::type>::type const operator()() const
    {
	return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<calculable_field_set, field_type>::type>::type>(_this->_field_data_map); //_this->_field_data[boost::mpl::c_str<typename boost::mpl::at<calculable_field_set, field_type>::type>::value];
    }
};

template <typename field_type>
struct Record::field_write<field_type, append_to_crypt_set> {
    Record* const _this;
    field_write(Record* const _this)
        : _this(_this)
    {
    }

    void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<append_to_crypt_map, field_type>::type>::type& value)
    {
	boost::fusion::at_key<typename boost::mpl::at<append_to_crypt_set, field_type>::type>(_this->_field_data_map) = value; // _this->_field_data[boost::mpl::c_str<typename boost::mpl::at<append_to_crypt_set, field_type>::type>::value] = value;
    }
};

template <typename field_type>
struct Record::field_write<field_type, crypt_field_set> {
    Record* const _this;
    field_write(Record* const _this)
        : _this(_this)
    {
    }

    void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_field_map, field_type>::type>::type& value)
    {
	return _this->write_crypt_field<typename boost::remove_reference<typename boost::mpl::at<crypt_field_set, field_type>::type>::type>(value); //boost::mpl::c_str<typename boost::mpl::at<crypt_field_set, field_type>::type>::value,
    } // {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
};

template <typename field_type>
struct Record::field_write<field_type, calculable_field_set> {
    Record* const _this;
    field_write(Record* const _this)
        : _this(_this)
    {
    }

    void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<calculable_field_map, field_type>::type>::type& value)
    {
	boost::fusion::at_key<typename boost::mpl::at<calculable_field_set, field_type>::type>(_this->_field_data_map) = value; //_this->_field_data[boost::mpl::c_str<typename boost::mpl::at<calculable_field_set, field_type>::type>::value] = value;
    }
};

template <typename field_key_type>
//boost::fusion::extension::at_key_impl::apply::type
//typename boost::fusion::result_of::at_key<full_field_set, field_type>::type
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map const, field_key_type>::type>::type Record::field() const
{
    return //detail::to_string(
	Record::field_read<field_key_type, typename switch_type<field_key_type>::type>(this)()
	//)
	; // natural_field(boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type >::type >::value);
}

template <typename field_key_type>
void Record::field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_key_type>::type>::type& value)
{
    return Record::field_write<field_key_type, typename switch_type<field_key_type>::type>(this)(value);
}

#ifndef EXTERN_NATURAL_FIELD_GET_DECLARATION

#define EXTERN_NATURAL_FIELD_GET_DECLARATION(key_type) \
    extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, key_type>::type>::type Record::natural_field_source<key_type>() const;

#endif //EXTERN_NATURAL_FIELD_GET_DECLARATION

EXTERN_NATURAL_FIELD_GET_DECLARATION(id_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(pin_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(rating_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(name_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(author_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(home_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(url_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(tags_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(ctime_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(dir_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(file_type)
EXTERN_NATURAL_FIELD_GET_DECLARATION(crypt_type)

//extern template decltype(auto) Record::natural_field_source<id_type>() const;
//extern template decltype(auto) Record::natural_field_source<pin_type>() const;
//extern template decltype(auto) Record::natural_field_source<rating_type>() const;
//extern template decltype(auto) Record::natural_field_source<name_type>() const;
//extern template decltype(auto) Record::natural_field_source<author_type>() const;
//extern template decltype(auto) Record::natural_field_source<home_type>() const;
//extern template decltype(auto) Record::natural_field_source<url_type>() const;
//extern template decltype(auto) Record::natural_field_source<tags_type>() const;
//extern template decltype(auto) Record::natural_field_source<ctime_type>() const;
//extern template decltype(auto) Record::natural_field_source<dir_type>() const;
//extern template decltype(auto) Record::natural_field_source<file_type>() const;
//extern template decltype(auto) Record::natural_field_source<crypt_type>() const;
////extern template decltype(auto) Record::natural_field_source<has_attach_type>() const;
////extern template decltype(auto) Record::natural_field_source<attach_size_type>() const;
////extern template decltype(auto) Record::natural_field_source<dynamic_name_type>() const;

#ifndef EXTERN_NATURAL_FIELD_SET_DECLARATION

#define EXTERN_NATURAL_FIELD_SET_DECLARATION(key_type) \
    extern template void Record::natural_field_source<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, key_type>::type>::type& value);

#endif //EXTERN_NATURAL_FIELD_SET_DECLARATION

EXTERN_NATURAL_FIELD_SET_DECLARATION(id_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(pin_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(rating_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(name_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(author_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(home_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(url_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(tags_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(ctime_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(dir_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(file_type)
EXTERN_NATURAL_FIELD_SET_DECLARATION(crypt_type)

//extern template void Record::natural_field_source<id_type>(const typename boost::fusion::result_of::at_key<natural_field_map, id_type>::type& value);
//extern template void Record::natural_field_source<pin_type>(const typename boost::fusion::result_of::at_key<natural_field_set, pin_type>::type& value);
//extern template void Record::natural_field_source<rating_type>(const typename boost::fusion::result_of::at_key<natural_field_set, rating_type>::type& value);
//extern template void Record::natural_field_source<name_type>(const typename boost::fusion::result_of::at_key<natural_field_set, name_type>::type& value);
//extern template void Record::natural_field_source<author_type>(const typename boost::fusion::result_of::at_key<natural_field_set, author_type>::type& value);
//extern template void Record::natural_field_source<home_type>(const typename boost::fusion::result_of::at_key<natural_field_set, home_type>::type& value);
//extern template void Record::natural_field_source<url_type>(const typename boost::fusion::result_of::at_key<natural_field_set, url_type>::type& value);
//extern template void Record::natural_field_source<tags_type>(const typename boost::fusion::result_of::at_key<natural_field_set, tags_type>::type& value);
//extern template void Record::natural_field_source<ctime_type>(const typename boost::fusion::result_of::at_key<natural_field_set, ctime_type>::type& value);
//extern template void Record::natural_field_source<dir_type>(const typename boost::fusion::result_of::at_key<natural_field_set, dir_type>::type& value);
//extern template void Record::natural_field_source<file_type>(const typename boost::fusion::result_of::at_key<natural_field_set, file_type>::type& value);
//extern template void Record::natural_field_source<crypt_type>(const typename boost::fusion::result_of::at_key<natural_field_set, crypt_type>::type& value);
////extern template void Record::natural_field_source<has_attach_type>(const typename boost::fusion::result_of::at_key<natural_field_set, has_attach_type>::type& value);
////extern template void Record::natural_field_source<attach_size_type>(const typename boost::fusion::result_of::at_key<natural_field_set, attach_size_type>::type& value);
////extern template void Record::natural_field_source<dynamic_name_type>(const typename boost::fusion::result_of::at_key<natural_field_set, field_type>::type &value);

extern template struct Record::field_read<pin_type, crypt_field_set>;
extern template struct Record::field_write<pin_type, crypt_field_set>;
extern template struct Record::field_read<name_type, crypt_field_set>;
extern template struct Record::field_write<name_type, crypt_field_set>;
extern template struct Record::field_read<author_type, crypt_field_set>;
extern template struct Record::field_write<author_type, crypt_field_set>;
extern template struct Record::field_read<home_type, crypt_field_set>;
extern template struct Record::field_write<home_type, crypt_field_set>;
extern template struct Record::field_read<url_type, crypt_field_set>;
extern template struct Record::field_write<url_type, crypt_field_set>;
extern template struct Record::field_read<tags_type, crypt_field_set>;
extern template struct Record::field_write<tags_type, crypt_field_set>;

extern template struct Record::field_read<id_type, append_to_crypt_set>;
extern template struct Record::field_write<id_type, append_to_crypt_set>;
extern template struct Record::field_read<rating_type, append_to_crypt_set>;
extern template struct Record::field_write<rating_type, append_to_crypt_set>;
extern template struct Record::field_read<ctime_type, append_to_crypt_set>;
extern template struct Record::field_write<ctime_type, append_to_crypt_set>;
extern template struct Record::field_read<dir_type, append_to_crypt_set>;
extern template struct Record::field_write<dir_type, append_to_crypt_set>;
extern template struct Record::field_read<file_type, append_to_crypt_set>;
extern template struct Record::field_write<file_type, append_to_crypt_set>;
extern template struct Record::field_read<crypt_type, append_to_crypt_set>;
extern template struct Record::field_write<crypt_type, append_to_crypt_set>;

extern template struct Record::field_read<has_attach_type, calculable_field_set>;
extern template struct Record::field_write<has_attach_type, calculable_field_set>;
extern template struct Record::field_read<attach_size_type, calculable_field_set>;
extern template struct Record::field_write<attach_size_type, calculable_field_set>;
extern template struct Record::field_read<dynamic_name_type, calculable_field_set>;
extern template struct Record::field_write<dynamic_name_type, calculable_field_set>;

#ifndef EXTERN_RECORD_GET_DECLARATION
#define EXTERN_RECORD_GET_DECLARATION(key_type) \
    extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type Record::field<key_type>() const;
#endif //EXTERN_RECORD_GET_DECLARATION

EXTERN_RECORD_GET_DECLARATION(id_type)
EXTERN_RECORD_GET_DECLARATION(pin_type)
EXTERN_RECORD_GET_DECLARATION(rating_type)
EXTERN_RECORD_GET_DECLARATION(name_type)
EXTERN_RECORD_GET_DECLARATION(author_type)
EXTERN_RECORD_GET_DECLARATION(home_type)
EXTERN_RECORD_GET_DECLARATION(url_type)
EXTERN_RECORD_GET_DECLARATION(tags_type)
EXTERN_RECORD_GET_DECLARATION(ctime_type)
EXTERN_RECORD_GET_DECLARATION(dir_type)
EXTERN_RECORD_GET_DECLARATION(file_type)
EXTERN_RECORD_GET_DECLARATION(crypt_type)
EXTERN_RECORD_GET_DECLARATION(has_attach_type)
EXTERN_RECORD_GET_DECLARATION(attach_size_type)

//extern template QString Record::field<id_type>() const;
//extern template QString Record::field<pin_type>() const;
//extern template QString Record::field<rating_type>() const;
//extern template QString Record::field<name_type>() const;
//extern template QString Record::field<author_type>() const;
//extern template QString Record::field<home_type>() const;
//extern template QString Record::field<url_type>() const;
//extern template QString Record::field<tags_type>() const;
//extern template QString Record::field<ctime_type>() const;
//extern template QString Record::field<dir_type>() const;
//extern template QString Record::field<file_type>() const;
//extern template QString Record::field<crypt_type>() const;
//extern template QString Record::field<has_attach_type>() const;
//extern template QString Record::field<attach_size_type>() const;
////extern template QString Record::field<dynamic_name_type>() const;

#ifndef EXTERN_RECORD_SET_DECLARATION
#define EXTERN_RECORD_SET_DECLARATION(key_type) \
    extern template void Record::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type& value);
#endif //EXTERN_RECORD_SET_DECLARATION

EXTERN_RECORD_SET_DECLARATION(id_type)
EXTERN_RECORD_SET_DECLARATION(pin_type)
EXTERN_RECORD_SET_DECLARATION(rating_type)
EXTERN_RECORD_SET_DECLARATION(name_type)
EXTERN_RECORD_SET_DECLARATION(author_type)
EXTERN_RECORD_SET_DECLARATION(home_type)
EXTERN_RECORD_SET_DECLARATION(url_type)
EXTERN_RECORD_SET_DECLARATION(tags_type)
EXTERN_RECORD_SET_DECLARATION(ctime_type)
EXTERN_RECORD_SET_DECLARATION(dir_type)
EXTERN_RECORD_SET_DECLARATION(file_type)
EXTERN_RECORD_SET_DECLARATION(crypt_type)
EXTERN_RECORD_SET_DECLARATION(has_attach_type)
EXTERN_RECORD_SET_DECLARATION(attach_size_type)

//extern template void Record::field<id_type>(const QString& value);
//extern template void Record::field<pin_type>(const QString& value);
//extern template void Record::field<rating_type>(const QString& value);
//extern template void Record::field<name_type>(const QString& value);
//extern template void Record::field<author_type>(const QString& value);
//extern template void Record::field<home_type>(const QString& value);
//extern template void Record::field<url_type>(const QString& value);
//extern template void Record::field<tags_type>(const QString& value);
//extern template void Record::field<ctime_type>(const QString& value);
//extern template void Record::field<dir_type>(const QString& value);
//extern template void Record::field<file_type>(const QString& value);
//extern template void Record::field<crypt_type>(const QString& value);
//extern template void Record::field<has_attach_type>(const QString& value);
//extern template void Record::field<attach_size_type>(const QString& value);
////extern template void Record::field<dynamic_name_type>(const QString &value);

///

#ifndef NATURAL_FIELD_GET_DDEFINITION

#define NATURAL_FIELD_GET_DDEFINITION(key_type) \
    template typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, key_type>::type>::type Record::natural_field_source<key_type>() const;

#endif //NATURAL_FIELD_GET_DDEFINITION

NATURAL_FIELD_GET_DDEFINITION(id_type)
NATURAL_FIELD_GET_DDEFINITION(pin_type)
NATURAL_FIELD_GET_DDEFINITION(rating_type)
NATURAL_FIELD_GET_DDEFINITION(name_type)
NATURAL_FIELD_GET_DDEFINITION(author_type)
NATURAL_FIELD_GET_DDEFINITION(home_type)
NATURAL_FIELD_GET_DDEFINITION(url_type)
NATURAL_FIELD_GET_DDEFINITION(tags_type)
NATURAL_FIELD_GET_DDEFINITION(ctime_type)
NATURAL_FIELD_GET_DDEFINITION(dir_type)
NATURAL_FIELD_GET_DDEFINITION(file_type)
NATURAL_FIELD_GET_DDEFINITION(crypt_type)

//template QString Record::natural_field_source<id_type>() const;
//template QString Record::natural_field_source<pin_type>() const;
//template QString Record::natural_field_source<rating_type>() const;
//template QString Record::natural_field_source<name_type>() const;
//template QString Record::natural_field_source<author_type>() const;
//template QString Record::natural_field_source<home_type>() const;
//template QString Record::natural_field_source<url_type>() const;
//template QString Record::natural_field_source<tags_type>() const;
//template QString Record::natural_field_source<ctime_type>() const;
//template QString Record::natural_field_source<dir_type>() const;
//template QString Record::natural_field_source<file_type>() const;
//template QString Record::natural_field_source<crypt_type>() const;
//// template QString Record::natural_field_source<has_attach_type>() const;
//// template QString Record::natural_field_source<attach_size_type>() const;
//// template QString Record::natural_field_source<dynamic_name_type>() const;

#ifndef NATURAL_FIELD_SET_DEFINITION

#define NATURAL_FIELD_SET_DEFINITION(key_type) \
    template void Record::natural_field_source<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_field_map, key_type>::type>::type& value);

#endif //NATURAL_FIELD_SET_DEFINITION

NATURAL_FIELD_SET_DEFINITION(id_type)
NATURAL_FIELD_SET_DEFINITION(pin_type)
NATURAL_FIELD_SET_DEFINITION(rating_type)
NATURAL_FIELD_SET_DEFINITION(name_type)
NATURAL_FIELD_SET_DEFINITION(author_type)
NATURAL_FIELD_SET_DEFINITION(home_type)
NATURAL_FIELD_SET_DEFINITION(url_type)
NATURAL_FIELD_SET_DEFINITION(tags_type)
NATURAL_FIELD_SET_DEFINITION(ctime_type)
NATURAL_FIELD_SET_DEFINITION(dir_type)
NATURAL_FIELD_SET_DEFINITION(file_type)
NATURAL_FIELD_SET_DEFINITION(crypt_type)

//template void Record::natural_field_source<id_type>(const QString& value);
//template void Record::natural_field_source<pin_type>(const QString& value);
//template void Record::natural_field_source<rating_type>(const QString& value);
//template void Record::natural_field_source<name_type>(const QString& value);
//template void Record::natural_field_source<author_type>(const QString& value);
//template void Record::natural_field_source<home_type>(const QString& value);
//template void Record::natural_field_source<url_type>(const QString& value);
//template void Record::natural_field_source<tags_type>(const QString& value);
//template void Record::natural_field_source<ctime_type>(const QString& value);
//template void Record::natural_field_source<dir_type>(const QString& value);
//template void Record::natural_field_source<file_type>(const QString& value);
//template void Record::natural_field_source<crypt_type>(const QString& value);
//// template void Record::natural_field_source<has_attach_type>(const QString &value);
//// template void Record::natural_field_source<attach_size_type>(const QString &value);
//// template void Record::natural_field_source<dynamic_name_type>(const QString &value);

#ifndef RECORD_GET_DEFINITION
#define RECORD_GET_DEFINITION(key_type) \
    template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type Record::field<key_type>() const;
#endif //RECORD_GET_DEFINITION

RECORD_GET_DEFINITION(id_type)
RECORD_GET_DEFINITION(pin_type)
RECORD_GET_DEFINITION(rating_type)
RECORD_GET_DEFINITION(name_type)
RECORD_GET_DEFINITION(author_type)
RECORD_GET_DEFINITION(home_type)
RECORD_GET_DEFINITION(url_type)
RECORD_GET_DEFINITION(tags_type)
RECORD_GET_DEFINITION(ctime_type)
RECORD_GET_DEFINITION(dir_type)
RECORD_GET_DEFINITION(file_type)
RECORD_GET_DEFINITION(crypt_type)
RECORD_GET_DEFINITION(has_attach_type)
RECORD_GET_DEFINITION(attach_size_type)

//template QString Record::field<id_type>() const;
//template QString Record::field<pin_type>() const;
//template QString Record::field<rating_type>() const;
//template QString Record::field<name_type>() const;
//template QString Record::field<author_type>() const;
//template QString Record::field<home_type>() const;
//template QString Record::field<url_type>() const;
//template QString Record::field<tags_type>() const;
//template QString Record::field<ctime_type>() const;
//template QString Record::field<dir_type>() const;
//template QString Record::field<file_type>() const;
//template QString Record::field<crypt_type>() const;
//template QString Record::field<has_attach_type>() const;
//template QString Record::field<attach_size_type>() const;
//// template QString Record::field<dynamic_name_type>() const;

#ifndef RECORD_SET_DEFINITION
#define RECORD_SET_DEFINITION(key_type) \
    template void Record::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, key_type>::type>::type& value);
#endif //RECORD_SET_DEFINITION

RECORD_SET_DEFINITION(id_type)
RECORD_SET_DEFINITION(pin_type)
RECORD_SET_DEFINITION(rating_type)
RECORD_SET_DEFINITION(name_type)
RECORD_SET_DEFINITION(author_type)
RECORD_SET_DEFINITION(home_type)
RECORD_SET_DEFINITION(url_type)
RECORD_SET_DEFINITION(tags_type)
RECORD_SET_DEFINITION(ctime_type)
RECORD_SET_DEFINITION(dir_type)
RECORD_SET_DEFINITION(file_type)
RECORD_SET_DEFINITION(crypt_type)
RECORD_SET_DEFINITION(has_attach_type)
RECORD_SET_DEFINITION(attach_size_type)

//template void Record::field<id_type>(const QString& value);
//template void Record::field<pin_type>(const QString& value);
//template void Record::field<rating_type>(const QString& value);
//template void Record::field<name_type>(const QString& value);
//template void Record::field<author_type>(const QString& value);
//template void Record::field<home_type>(const QString& value);
//template void Record::field<url_type>(const QString& value);
//template void Record::field<tags_type>(const QString& value);
//template void Record::field<ctime_type>(const QString& value);
//template void Record::field<dir_type>(const QString& value);
//template void Record::field<file_type>(const QString& value);
//template void Record::field<crypt_type>(const QString& value);
//template void Record::field<has_attach_type>(const QString& value);
//template void Record::field<attach_size_type>(const QString& value);
//// template void Record::field<dynamic_name_type>(const QString &value);

template struct Record::field_read<pin_type, crypt_field_set>;
template struct Record::field_write<pin_type, crypt_field_set>;
template struct Record::field_read<name_type, crypt_field_set>;
template struct Record::field_write<name_type, crypt_field_set>;
template struct Record::field_read<author_type, crypt_field_set>;
template struct Record::field_write<author_type, crypt_field_set>;
template struct Record::field_read<home_type, crypt_field_set>;
template struct Record::field_write<home_type, crypt_field_set>;
template struct Record::field_read<url_type, crypt_field_set>;
template struct Record::field_write<url_type, crypt_field_set>;
template struct Record::field_read<tags_type, crypt_field_set>;
template struct Record::field_write<tags_type, crypt_field_set>;

template struct Record::field_read<id_type, append_to_crypt_set>;
template struct Record::field_write<id_type, append_to_crypt_set>;
template struct Record::field_read<rating_type, append_to_crypt_set>;
template struct Record::field_write<rating_type, append_to_crypt_set>;
template struct Record::field_read<ctime_type, append_to_crypt_set>;
template struct Record::field_write<ctime_type, append_to_crypt_set>;
template struct Record::field_read<dir_type, append_to_crypt_set>;
template struct Record::field_write<dir_type, append_to_crypt_set>;
template struct Record::field_read<file_type, append_to_crypt_set>;
template struct Record::field_write<file_type, append_to_crypt_set>;
template struct Record::field_read<crypt_type, append_to_crypt_set>;
template struct Record::field_write<crypt_type, append_to_crypt_set>;

template struct Record::field_read<has_attach_type, calculable_field_set>;
template struct Record::field_write<has_attach_type, calculable_field_set>;
template struct Record::field_read<attach_size_type, calculable_field_set>;
template struct Record::field_write<attach_size_type, calculable_field_set>;
template struct Record::field_read<dynamic_name_type, calculable_field_set>;
template struct Record::field_write<dynamic_name_type, calculable_field_set>;

#endif // RECORD_DEF
