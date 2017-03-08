#ifndef RECORD_DEF
#define RECORD_DEF
#include "libraries/crypt/crypt_service.h"
#include "libraries/global_parameters.h"
#include "models/record_table/record.h"
#include <QString>
class Record;

template <typename field_key_type>
const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map, typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::type>::type Record::read_crypt_field() const
{
	typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map, typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::type>::type return_type;
	//    QString name = boost::mpl::c_str<typename
	//    boost::mpl::at<append_to_crypt_set, field_type>::type>::value;
	return_type result = boost::fusion::at_key<field_key_type>(_fields_data_map);

	//    QString crypt_field_name = boost::mpl::c_str<crypt_type>::value;
	detail::static_if<boost::fusion::result_of::has_key<crypt_fields_map const, field_key_type>::value>(
	    [&]() {
		    if (crypt_value(true) == boost::fusion::at_key<crypt_key>(_fields_data_map)) {
			    auto value = detail::to_qstring(boost::fusion::at_key<field_key_type>(_fields_data_map));
			    if (value.length() > 0 && gl_paras->crypt_key().length() > 0) {
				    result = detail::from_qstring<field_key_type>(CryptService::decryptString(gl_paras->crypt_key(), value));
			    }
		    }
	    })();
	//    if (_field_data.contains(crypt_field_name)) { // boost::mpl::c_str <
	//    crypt_type > ::value
	//	if (_field_data[crypt_field_name] == "1") {
	//	    if (_field_data[name].length() > 0 && gl_paras->crypt_key().length()
	//> 0) {
	//		// is_crypt = true;
	//		// Поле расшифровывается
	//		result = CryptService::decryptString(gl_paras->crypt_key(),
	//_field_data[name]);
	//	    }
	//	} else
	//	    result = _field_data[name];
	//    } else
	//	result = _field_data[name];
	return result;
}

template <typename field_key_type>
void Record::write_crypt_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map const, typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::type>::type& value)
{
	//    const QString& _name;
	// template <typename field_type>void Record::field_write<field_type,
	// crypt_field_type>::operator()(const QString &value){// template<typename
	// field_type> void Record::field(const QString &value){
	QString key_name = boost::mpl::c_str<typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::value;
	//// Если имя поля недопустимо (установить значение можно только для
	///натурального поля)
	// if(fixedparameters.is_record_field_natural(_name) ==
	// false)critical_error("In Record::field() unavailable field name " + _name +
	// " try set to " + value);
	bool is_crypt = false;

	detail::static_if<boost::fusion::result_of::has_key<crypt_fields_map const, field_key_type>::value>([&]() {
		if (crypt_value(true) == boost::fusion::at_key<crypt_key>(_fields_data_map)) {
			assert(field<crypt_key>() == crypt_value(true));
			auto value_qstring = detail::to_qstring(value);
			if (value_qstring.length() > 0) {
				if (gl_paras->crypt_key().length() > 0) {
					is_crypt = true;
					boost::fusion::at_key<field_key_type>(_fields_data_map) = detail::from_qstring<field_key_type>(CryptService::encryptString(gl_paras->crypt_key(), value_qstring));

				} else
					critical_error("In Record::field() can not set data to crypt field " + key_name + ". Password not setted");
			}
		} else {
			boost::fusion::at_key<field_key_type>(_fields_data_map) = value;
			assert(field<field_key_type>() == value);
		}
	})();

	//    // Если имя поля принадлежит списку полей, которые могут шифроваться
	//    // и в наборе полей есть поле crypt
	//    // и поле crypt установлено в 1
	//    // и поле не пустое (пустые данные ненужно шифровать)
	//    // if(fixedparameters._record_field_crypted.contains(_name)){
	//    if (_field_data.contains(boost::mpl::c_str<crypt_type>::value)) {
	//	if (_field_data[boost::mpl::c_str<crypt_type>::value] == "1") {
	//	    if (value.length() > 0) {
	//		if (gl_paras->crypt_key().length() > 0)
	//		    is_crypt = true;
	//		else
	//		    critical_error("In Record::field() can not set data to crypt
	//field " + _name + ". Password not setted");
	//	    }
	//	}
	//    }
	//    // }
	//    QString _value = value;
	//    // Если нужно шифровать, поле шифруется
	//    if (is_crypt == true)
	//	_value = CryptService::encryptString(gl_paras->crypt_key(), value);
	//    // Устанавливается значение поля

	//    _field_data.insert(_name, _value); // _field_data[_name] = value; //

	//    // qDebug() << "Record::set_field : pos" << pos << "name" << name <<
	//    "value" << value;
}


template <typename field_key_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map, field_key_type>::type>::type
Record::natural_field_source() const
{ // QString name
// Если имя поля недопустимо
#ifdef TEST
	BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set, concrete>::type::value), ==, true); // critical_error("Record::natural_field_source() : get unavailable field " + name);
	BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set, concrete>));
#endif // TEST
	BOOST_MPL_ASSERT((boost::mpl::contains<natural_fields_keys, field_key_type>));
	//    // Если поле с таким названием есть
	//    if (_field_data.contains(boost::mpl::c_str<concrete>::value))
	//        return _field_data[boost::mpl::c_str<concrete>::value]; //
	//        Возвращается значение поля
	//    else
	//        return QString();
	return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<natural_fields_keys, field_key_type>::type>::type>(_fields_data_map);
}

template <typename field_key_type>
void Record::natural_field_source(
    const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map const, field_key_type>::type>::type& value)
{ // QString name,, concrete cc
//		(void) cc;
// Если имя поля недопустимо
#ifdef TEST
	BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set, concrete>::type::value), ==, true); //if(fixedparameters.is_record_field_natural(name) == false) critical_error("In Record::natural_field_source() unavailable field name " + name + " try set to " + value);
	BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set, concrete>));
#endif // TEST
	BOOST_MPL_ASSERT((boost::mpl::contains<natural_fields_keys, field_key_type>));
	//    // Устанавливается значение поля
	//    _field_data.insert(boost::mpl::c_str<field_type>::value, value);
	boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<natural_fields_keys, field_key_type>::type>::type>(_fields_data_map) = value;
}

template <typename field_type>
struct Record::field_read<field_type, append_to_crypts_keys> {
	const Record* _this;
	field_read(const Record* _this)
	    : _this(_this)
	{
	}

	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, typename boost::mpl::at<append_to_crypts_keys, field_type>::type>::type>::type
	operator()() const
	{
		return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<append_to_crypts_keys, field_type>::type>::type>(_this->_fields_data_map);
	}
};

template <typename field_type>
struct Record::field_read<field_type, crypt_fields_keys> {
	const Record* _this;
	field_read(const Record* _this)
	    : _this(_this)
	{
	}

	typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map, typename boost::mpl::at<crypt_fields_keys, field_type>::type>::type>::type
	operator()() const
	{
		return _this->read_crypt_field<typename boost::remove_reference<typename boost::mpl::at<crypt_fields_keys, field_type>::type>::type>();
	}
};

template <typename field_type>
struct Record::field_read<field_type, calculable_fields_keys> {
	const Record* _this;
	field_read(const Record* _this)
	    : _this(_this)
	{
	}

	typename boost::remove_reference<typename boost::fusion::result_of::at_key<calculable_fields_map, typename boost::mpl::at<calculable_fields_keys, field_type>::type>::type>::type
	operator()() const
	{
		return boost::fusion::at_key<typename boost::remove_reference<typename boost::mpl::at<calculable_fields_keys, field_type>::type>::type>(_this->_fields_data_map);
	}
};

template <typename field_type>
struct Record::field_write<field_type, append_to_crypts_keys> {
	Record* const _this;
	field_write(Record* const _this)
	    : _this(_this)
	{
	}

	void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<append_to_crypts_map const, field_type>::type>::type& value)
	{
		boost::fusion::at_key<typename boost::mpl::at<append_to_crypts_keys, field_type>::type>(_this->_fields_data_map) = value;
	}
};

template <typename field_type>
struct Record::field_write<field_type, crypt_fields_keys> {
	Record* const _this;
	field_write(Record* const _this)
	    : _this(_this)
	{
	}

	void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map const, field_type>::type>::type& value)
	{
		return _this->write_crypt_field<typename boost::remove_reference<typename boost::mpl::at<crypt_fields_keys, field_type>::type>::type>(value);
	}
};

template <typename field_type>
struct Record::field_write<field_type, calculable_fields_keys> {
	Record* const _this;
	field_write(Record* const _this)
	    : _this(_this)
	{
	}

	void operator()(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<calculable_fields_map const, field_type>::type>::type& value)
	{
		boost::fusion::at_key<typename boost::mpl::at<calculable_fields_keys, field_type>::type>(_this->_fields_data_map) = value;
	}
};

template <typename field_key_type> // boost::fusion::extension::at_key_impl::apply::type
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type Record::field() const
{
	return Record::field_read<field_key_type, typename switch_type<field_key_type>::type>(this)(); // detail::to_string()
}

template <typename field_key_type>
void Record::field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type& value)
{
	return Record::field_write<field_key_type, typename switch_type<field_key_type>::type>(this)(value);
}


extern template struct Record::field_read<name_key, crypt_fields_keys>;
extern template struct Record::field_write<name_key, crypt_fields_keys>;
extern template struct Record::field_read<author_key, crypt_fields_keys>;
extern template struct Record::field_write<author_key, crypt_fields_keys>;
extern template struct Record::field_read<home_key, crypt_fields_keys>;
extern template struct Record::field_write<home_key, crypt_fields_keys>;
extern template struct Record::field_read<url_key, crypt_fields_keys>;
extern template struct Record::field_write<url_key, crypt_fields_keys>;
extern template struct Record::field_read<tags_key, crypt_fields_keys>;
extern template struct Record::field_write<tags_key, crypt_fields_keys>;

extern template struct Record::field_read<id_key, append_to_crypts_keys>;
extern template struct Record::field_write<id_key, append_to_crypts_keys>;
extern template struct Record::field_read<pin_key, append_to_crypts_keys>;
extern template struct Record::field_write<pin_key, append_to_crypts_keys>;
extern template struct Record::field_read<rating_key, append_to_crypts_keys>;
extern template struct Record::field_write<rating_key, append_to_crypts_keys>;
extern template struct Record::field_read<ctime_key, append_to_crypts_keys>;
extern template struct Record::field_write<ctime_key, append_to_crypts_keys>;
extern template struct Record::field_read<dir_key, append_to_crypts_keys>;
extern template struct Record::field_write<dir_key, append_to_crypts_keys>;
extern template struct Record::field_read<file_key, append_to_crypts_keys>;
extern template struct Record::field_write<file_key, append_to_crypts_keys>;
extern template struct Record::field_read<crypt_key, append_to_crypts_keys>;
extern template struct Record::field_write<crypt_key, append_to_crypts_keys>;

extern template struct Record::field_read<has_attach_key, calculable_fields_keys>;
extern template struct Record::field_write<has_attach_key, calculable_fields_keys>;
extern template struct Record::field_read<attach_size_key, calculable_fields_keys>;
extern template struct Record::field_write<attach_size_key, calculable_fields_keys>;
extern template struct Record::field_read<dynamic_name_key, calculable_fields_keys>;
extern template struct Record::field_write<dynamic_name_key, calculable_fields_keys>;


#ifndef NATURAL_FIELD_GET_DDEFINITION

#define NATURAL_FIELD_GET_DDEFINITION(key_type) \
	template typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map, key_type>::type>::type Record::natural_field_source<key_type>() const;

#endif // NATURAL_FIELD_GET_DDEFINITION

NATURAL_FIELD_GET_DDEFINITION(id_key)
NATURAL_FIELD_GET_DDEFINITION(pin_key)
NATURAL_FIELD_GET_DDEFINITION(rating_key)
NATURAL_FIELD_GET_DDEFINITION(name_key)
NATURAL_FIELD_GET_DDEFINITION(author_key)
NATURAL_FIELD_GET_DDEFINITION(home_key)
NATURAL_FIELD_GET_DDEFINITION(url_key)
NATURAL_FIELD_GET_DDEFINITION(tags_key)
NATURAL_FIELD_GET_DDEFINITION(ctime_key)
NATURAL_FIELD_GET_DDEFINITION(dir_key)
NATURAL_FIELD_GET_DDEFINITION(file_key)
NATURAL_FIELD_GET_DDEFINITION(crypt_key)

// template QString Record::natural_field_source<id_key>() const;
// template QString Record::natural_field_source<pin_key>() const;
// template QString Record::natural_field_source<rating_key>() const;
// template QString Record::natural_field_source<name_key>() const;
// template QString Record::natural_field_source<author_key>() const;
// template QString Record::natural_field_source<home_key>() const;
// template QString Record::natural_field_source<url_key>() const;
// template QString Record::natural_field_source<tags_key>() const;
// template QString Record::natural_field_source<ctime_key>() const;
// template QString Record::natural_field_source<dir_key>() const;
// template QString Record::natural_field_source<file_key>() const;
// template QString Record::natural_field_source<crypt_key>() const;
//// template QString Record::natural_field_source<has_attach_key>() const;
//// template QString Record::natural_field_source<attach_size_key>() const;
//// template QString Record::natural_field_source<dynamic_name_key>() const;

#ifndef NATURAL_FIELD_SET_DEFINITION

#define NATURAL_FIELD_SET_DEFINITION(key_type) \
	template void Record::natural_field_source<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map const, key_type>::type>::type& value);

#endif // NATURAL_FIELD_SET_DEFINITION

NATURAL_FIELD_SET_DEFINITION(id_key)
NATURAL_FIELD_SET_DEFINITION(pin_key)
NATURAL_FIELD_SET_DEFINITION(rating_key)
NATURAL_FIELD_SET_DEFINITION(name_key)
NATURAL_FIELD_SET_DEFINITION(author_key)
NATURAL_FIELD_SET_DEFINITION(home_key)
NATURAL_FIELD_SET_DEFINITION(url_key)
NATURAL_FIELD_SET_DEFINITION(tags_key)
NATURAL_FIELD_SET_DEFINITION(ctime_key)
NATURAL_FIELD_SET_DEFINITION(dir_key)
NATURAL_FIELD_SET_DEFINITION(file_key)
NATURAL_FIELD_SET_DEFINITION(crypt_key)

// template void Record::natural_field_source<id_key>(const QString& value);
// template void Record::natural_field_source<pin_key>(const QString& value);
// template void Record::natural_field_source<rating_key>(const QString&
// value);
// template void Record::natural_field_source<name_key>(const QString& value);
// template void Record::natural_field_source<author_key>(const QString&
// value);
// template void Record::natural_field_source<home_key>(const QString& value);
// template void Record::natural_field_source<url_key>(const QString& value);
// template void Record::natural_field_source<tags_key>(const QString& value);
// template void Record::natural_field_source<ctime_key>(const QString& value);
// template void Record::natural_field_source<dir_key>(const QString& value);
// template void Record::natural_field_source<file_key>(const QString& value);
// template void Record::natural_field_source<crypt_key>(const QString& value);
//// template void Record::natural_field_source<has_attach_key>(const QString
///&value);
//// template void Record::natural_field_source<attach_size_key>(const QString
///&value);
//// template void Record::natural_field_source<dynamic_name_key>(const QString
///&value);

#ifndef RECORD_GET_DEFINITION
#define RECORD_GET_DEFINITION(key_type) \
	template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Record::field<key_type>() const;
#endif // RECORD_GET_DEFINITION

RECORD_GET_DEFINITION(id_key)
RECORD_GET_DEFINITION(pin_key)
RECORD_GET_DEFINITION(rating_key)
RECORD_GET_DEFINITION(name_key)
RECORD_GET_DEFINITION(author_key)
RECORD_GET_DEFINITION(home_key)
RECORD_GET_DEFINITION(url_key)
RECORD_GET_DEFINITION(tags_key)
RECORD_GET_DEFINITION(ctime_key)
RECORD_GET_DEFINITION(dir_key)
RECORD_GET_DEFINITION(file_key)
RECORD_GET_DEFINITION(crypt_key)
RECORD_GET_DEFINITION(has_attach_key)
RECORD_GET_DEFINITION(attach_size_key)

// template QString Record::field<id_key>() const;
// template QString Record::field<pin_key>() const;
// template QString Record::field<rating_key>() const;
// template QString Record::field<name_key>() const;
// template QString Record::field<author_key>() const;
// template QString Record::field<home_key>() const;
// template QString Record::field<url_key>() const;
// template QString Record::field<tags_key>() const;
// template QString Record::field<ctime_key>() const;
// template QString Record::field<dir_key>() const;
// template QString Record::field<file_key>() const;
// template QString Record::field<crypt_key>() const;
// template QString Record::field<has_attach_key>() const;
// template QString Record::field<attach_size_key>() const;
//// template QString Record::field<dynamic_name_key>() const;

#ifndef RECORD_SET_DEFINITION
#define RECORD_SET_DEFINITION(key_type) \
	template void Record::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type& value);
#endif // RECORD_SET_DEFINITION

RECORD_SET_DEFINITION(id_key)
RECORD_SET_DEFINITION(pin_key)
RECORD_SET_DEFINITION(rating_key)
RECORD_SET_DEFINITION(name_key)
RECORD_SET_DEFINITION(author_key)
RECORD_SET_DEFINITION(home_key)
RECORD_SET_DEFINITION(url_key)
RECORD_SET_DEFINITION(tags_key)
RECORD_SET_DEFINITION(ctime_key)
RECORD_SET_DEFINITION(dir_key)
RECORD_SET_DEFINITION(file_key)
RECORD_SET_DEFINITION(crypt_key)
RECORD_SET_DEFINITION(has_attach_key)
RECORD_SET_DEFINITION(attach_size_key)

// template void Record::field<id_key>(const QString& value);
// template void Record::field<pin_key>(const QString& value);
// template void Record::field<rating_key>(const QString& value);
// template void Record::field<name_key>(const QString& value);
// template void Record::field<author_key>(const QString& value);
// template void Record::field<home_key>(const QString& value);
// template void Record::field<url_key>(const QString& value);
// template void Record::field<tags_key>(const QString& value);
// template void Record::field<ctime_key>(const QString& value);
// template void Record::field<dir_key>(const QString& value);
// template void Record::field<file_key>(const QString& value);
// template void Record::field<crypt_key>(const QString& value);
// template void Record::field<has_attach_key>(const QString& value);
// template void Record::field<attach_size_key>(const QString& value);
//// template void Record::field<dynamic_name_key>(const QString &value);


template struct Record::field_read<name_key, crypt_fields_keys>;
template struct Record::field_write<name_key, crypt_fields_keys>;
template struct Record::field_read<author_key, crypt_fields_keys>;
template struct Record::field_write<author_key, crypt_fields_keys>;
template struct Record::field_read<home_key, crypt_fields_keys>;
template struct Record::field_write<home_key, crypt_fields_keys>;
template struct Record::field_read<url_key, crypt_fields_keys>;
template struct Record::field_write<url_key, crypt_fields_keys>;
template struct Record::field_read<tags_key, crypt_fields_keys>;
template struct Record::field_write<tags_key, crypt_fields_keys>;

template struct Record::field_read<id_key, append_to_crypts_keys>;
template struct Record::field_write<id_key, append_to_crypts_keys>;
template struct Record::field_read<pin_key, append_to_crypts_keys>;
template struct Record::field_write<pin_key, append_to_crypts_keys>;
template struct Record::field_read<rating_key, append_to_crypts_keys>;
template struct Record::field_write<rating_key, append_to_crypts_keys>;
template struct Record::field_read<ctime_key, append_to_crypts_keys>;
template struct Record::field_write<ctime_key, append_to_crypts_keys>;
template struct Record::field_read<dir_key, append_to_crypts_keys>;
template struct Record::field_write<dir_key, append_to_crypts_keys>;
template struct Record::field_read<file_key, append_to_crypts_keys>;
template struct Record::field_write<file_key, append_to_crypts_keys>;
template struct Record::field_read<crypt_key, append_to_crypts_keys>;
template struct Record::field_write<crypt_key, append_to_crypts_keys>;

template struct Record::field_read<has_attach_key, calculable_fields_keys>;
template struct Record::field_write<has_attach_key, calculable_fields_keys>;
template struct Record::field_read<attach_size_key, calculable_fields_keys>;
template struct Record::field_write<attach_size_key, calculable_fields_keys>;
template struct Record::field_read<dynamic_name_key, calculable_fields_keys>;
template struct Record::field_write<dynamic_name_key, calculable_fields_keys>;

#endif // RECORD_DEF
