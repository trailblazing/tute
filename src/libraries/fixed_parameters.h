#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__

//#define FUSION_MAX_VECTOR_SIZE 50
//#include <boost/tr1/tuple.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/at_fwd.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/has_key_fwd.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_fwd.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/insert_range_fwd.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_back_fwd.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/string.hpp>
#include <boost/serialization/strong_typedef.hpp>
#include <boost/units/detail/prevent_redefinition.hpp>
// #include <boost/mpl/set/set0.hpp>
// #include <boost/mpl/set/set20.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/map/map_fwd.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/map_fwd.hpp>

#include <type_traits>
#include <typeinfo>

#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/map.hpp>

#include <boost/fusion/include/value_at_key.hpp>
#include <boost/fusion/sequence/intrinsic/value_at_key.hpp>

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>

#include <boost/type_traits.hpp>
#include <boost/type_traits/remove_reference.hpp> //or

#include <tuple>

#include <QMap>
#include <QStringList>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#else
#include <QObject>
#endif

#include "utility/delegate.h"
#include "utility/strong_typedef.hxx"
//#include "libraries/global_parameters.h"
//#include "utility/variant.h" // for Unify template

// extern const char *id_field;
// extern const char *pin_field;
// extern const char *rating_field;
// extern const char *name_field;
// extern const char *author_field;
// extern const char *home_field;
// extern const char *url_field;
// extern const char *tags_field;
// extern const char *ctime_field;
// extern const char *dir_field;
// extern const char *file_field;
// extern const char *crypt_field;
// extern const char *has_attach_field;
// extern const char *attach_count_field;

#ifndef BOOST_MPL_LIMIT_STRING_SIZE
//# define BOOST_MPL_LIMIT_STRING_SIZE 64
#define BOOST_MPL_LIMIT_STRING_SIZE 256
#endif

typedef boost::mpl::string<'i', 'd'> id_key;
typedef boost::mpl::string<'p', 'i', 'n'> pin_key;
typedef boost::mpl::string<'r', 'a', 't', 'i', 'n', 'g'> rating_key;
typedef boost::mpl::string<'n', 'a', 'm', 'e'> name_key;
typedef boost::mpl::string<'a', 'u', 't', 'h', 'o', 'r'> author_key;
typedef boost::mpl::string<'h', 'o', 'm', 'e'> home_key;
typedef boost::mpl::string<'u', 'r', 'l'> url_key;
typedef boost::mpl::string<'t', 'a', 'g', 's'> tags_key;
typedef boost::mpl::string<'c', 't', 'i', 'm', 'e'> ctime_key;
typedef boost::mpl::string<'d', 'i', 'r'> dir_key;
typedef boost::mpl::string<'f', 'i', 'l', 'e'> file_key;
typedef boost::mpl::string<'c', 'r', 'y', 'p', 't'> crypt_key;
typedef boost::mpl::string<'h', 'a', 's', '_', 'a', 't', 'c', 'h'> has_attach_key; //
// typedef boost::mpl::string<'has_', 'a','t','t','a', 'c','h'> has_attach_type;
// http://stackoverflow.com/questions/37606999/boostmplstring-size-error-messages
// with "-Wmultichar" parameter for make // -Wno-multichar won't disable it
typedef boost::mpl::string<'a', 't', 'c', 'h', 's', 'i', 'z', 'e'> attach_size_key; //
// typedef boost::mpl::string<'atta', 'c','h','_','c', 'o','u','n','t'>
// attach_count_type;// error: too many template arguments for class template
// 'string'

typedef boost::mpl::string<'d', 'y', 'n', '_', 'n', 'a', 'm', 'e'> dynamic_name_key;
// typedef boost::mpl::string<'dyna', 'm','i','c','_', 'n','a','m','e'>
// dynamic_name_type;

BOOST_STRONG_TYPEDEF_EXTENSION(QString, id_value)
BOOST_STRONG_TYPEDEF_EXTENSION(bool, pin_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, name_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, author_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, home_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, url_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QStringList, tags_value)
BOOST_STRONG_TYPEDEF_EXTENSION(int, rating_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, ctime_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, dir_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, file_value)
BOOST_STRONG_TYPEDEF_EXTENSION(bool, crypt_value)
BOOST_STRONG_TYPEDEF_EXTENSION(bool, has_attach_value)
BOOST_STRONG_TYPEDEF_EXTENSION(int, attach_size_value)
BOOST_STRONG_TYPEDEF_EXTENSION(QString, dynamic_name_value)

class i_t;
namespace web{
    class Browser;
}

template <typename input_t>
struct real_url_t;

namespace detail {
	template <typename output_t, typename input_t>
	output_t //boost::intrusive_ptr<::real_url_t<input_t>>
	    real_url_instance(const input_t& url_, std::function<output_t(const input_t&)>);


	template <typename output_t>
	output_t //boost::intrusive_ptr<::real_url_t<QString>>
	    real_url_instance(const QString& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<QString>>)>);

	extern template web::Browser* //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const QString& url_, std::function<web::Browser*(boost::intrusive_ptr<real_url_t<QString>>)>);


	template <typename output_t>
	output_t //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const url_value& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<url_value>>)>);

	extern template
	boost::intrusive_ptr<i_t> //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const url_value& url_, std::function<boost::intrusive_ptr<i_t>(boost::intrusive_ptr<real_url_t<url_value>>)>);

	extern template
	web::Browser* //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const url_value& url_, std::function<web::Browser*(boost::intrusive_ptr<real_url_t<url_value>>)>);
}

template <typename input_t>
struct real_url_t : public boost::intrusive_ref_counter<real_url_t<input_t>, boost::thread_safe_counter> {
	template <typename output_t>
	static output_t //boost::intrusive_ptr<::real_url_t<input_t>>
	    instance(const input_t& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<input_t>>)> func_)
	{
		return detail::real_url_instance(url_, func_);
	}
	input_t value() const { return _url; }

    protected:
	real_url_t(const input_t& url_)
	    : _url(url_) {}

	input_t _url;

	template <typename output_t>
	friend output_t //boost::intrusive_ptr<real_url_t<input_t>>
	    detail::real_url_instance(const input_t& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<input_t>>)>);
};


extern template struct real_url_t<QString>;
//	: public boost::intrusive_ref_counter<real_url_t<QString>, boost::thread_safe_counter> {
//	static boost::intrusive_ptr<real_url_t<QString>> instance(const QString& url_);
//	QString value() const;

//    protected:
//	real_url_t(const QString& url_);
//	QString _url;
//};
extern template struct real_url_t<url_value>;
//	: public boost::intrusive_ref_counter<real_url_t<url_value>, boost::thread_safe_counter> {
//	static boost::intrusive_ptr<real_url_t<url_value>> instance(const url_value& url_);
//	url_value value() const;

//    protected:
//	real_url_t(const url_value& url_);
//	url_value _url;
//};


typedef boost::mpl::set<name_key, author_key, home_key, url_key, tags_key> crypt_fields_keys;
// typedef boost::mpl::map<pin_type, name_type, author_type, home_type,
// url_type, tags_type> crypt_field_map;
typedef std::tuple<name_key, author_key, home_key, url_key, tags_key> crypt_fields_key_tuple;
typedef boost::fusion::map<boost::fusion::pair<name_key, name_value>,
    boost::fusion::pair<author_key, author_value>,
    boost::fusion::pair<home_key, home_value>,
    boost::fusion::pair<url_key, url_value>,
    boost::fusion::pair<tags_key, tags_value>>
    crypt_fields_map;

typedef boost::mpl::set<id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key> append_to_crypts_keys;
typedef std::tuple<id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key> append_to_crypts_key_tuple;
typedef boost::fusion::map<boost::fusion::pair<id_key, id_value>,
    boost::fusion::pair<pin_key, pin_value>,
    boost::fusion::pair<rating_key, rating_value>,
    boost::fusion::pair<ctime_key, ctime_value>,
    boost::fusion::pair<dir_key, dir_value>,
    boost::fusion::pair<file_key, file_value>,
    boost::fusion::pair<crypt_key, crypt_value>>
    append_to_crypts_map;

// typedef boost::mpl::push_back<crypt_field_type, append_to_crypt_type>
// natural_field_type;
typedef boost::mpl::set<name_key, author_key, home_key, url_key, tags_key, id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key> natural_fields_keys;
typedef std::tuple<name_key, author_key, home_key, url_key, tags_key, id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key> natural_fields_key_tuple;
typedef boost::fusion::map<boost::fusion::pair<name_key, name_value>,
    boost::fusion::pair<author_key, author_value>,
    boost::fusion::pair<home_key, home_value>,
    boost::fusion::pair<url_key, url_value>,
    boost::fusion::pair<tags_key, tags_value>,
    boost::fusion::pair<id_key, id_value>,
    boost::fusion::pair<pin_key, pin_value>,
    boost::fusion::pair<rating_key, rating_value>,
    boost::fusion::pair<ctime_key, ctime_value>,
    boost::fusion::pair<dir_key, dir_value>,
    boost::fusion::pair<file_key, file_value>,
    boost::fusion::pair<crypt_key, crypt_value>>
    natural_fields_map;

#ifdef USE_COMPILE_TIME_CACULATION
// using natural_field_fold = boost::mpl::fold <
//			   crypt_field_set
//      , boost::mpl::set0<>
//      , boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>::type;

typedef boost::mpl::copy<crypt_field_set,
    boost::mpl::back_inserter<append_to_crypt_set>>::type
    natural_field_concatenated;

typedef boost::mpl::joint_view<crypt_field_set, append_to_crypt_set>
    natural_field_concatenated_view;
natural_field_concatenated::dog;
natural_field_set::fish;
natural_field_concatenated_view::pig;
BOOST_MPL_ASSERT((boost::mpl::equal<natural_field_concatenated_view, natural_field_concatenated>));

static_assert(sd::equal_type<natural_field_concatenated, natural_field_set>::value == true, "types are not euqal");
//
// typedef Unify<
//	std::tuple
//	, natural_field_set
//	, boost::mpl::size<natural_field_set>::type::value
//	>::type natural_field_tuple;
#endif // USE_COMPILE_TIME_CACULATION

typedef boost::mpl::set<has_attach_key, attach_size_key, dynamic_name_key> calculable_fields_keys;
typedef std::tuple<has_attach_key, attach_size_key, dynamic_name_key> calculable_fields_key_tuple;
typedef boost::fusion::map<boost::fusion::pair<has_attach_key, has_attach_value>, boost::fusion::pair<attach_size_key, attach_size_value>, boost::fusion::pair<dynamic_name_key, dynamic_name_value>>
    calculable_fields_map;

// typedef boost::mpl::push_back<natural_field_type, calculable_field_type>
// full_field_type;
typedef boost::mpl::set<name_key, author_key, home_key, url_key, tags_key, id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key, has_attach_key, attach_size_key, dynamic_name_key>
    full_fields_keys;
typedef std::tuple<name_key, author_key, home_key, url_key, tags_key, id_key, pin_key, rating_key, ctime_key, dir_key, file_key, crypt_key, has_attach_key, attach_size_key, dynamic_name_key>
    full_fields_key_tuple;
// typedef boost::fusion::tuple<pin_type, name_type, author_type, home_type,
// url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type,
// crypt_type, has_attach_type, attach_size_type, dynamic_name_type>
// full_field_fusion_tuple;
typedef std::tuple<name_value, author_value, home_value, url_value, tags_value, id_value, pin_value, rating_value, ctime_value, dir_value, file_value, crypt_value, has_attach_value, attach_size_value, dynamic_name_value>
    full_fields_value_tuple;

typedef boost::fusion::map<boost::fusion::pair<name_key, name_value>,
    boost::fusion::pair<author_key, author_value>,
    boost::fusion::pair<home_key, home_value>,
    boost::fusion::pair<url_key, url_value>,
    boost::fusion::pair<tags_key, tags_value>,
    boost::fusion::pair<id_key, id_value>,
    boost::fusion::pair<pin_key, pin_value>,
    boost::fusion::pair<rating_key, rating_value>,
    boost::fusion::pair<ctime_key, ctime_value>,
    boost::fusion::pair<dir_key, dir_value>,
    boost::fusion::pair<file_key, file_value>,
    boost::fusion::pair<crypt_key, crypt_value>,
    boost::fusion::pair<has_attach_key, has_attach_value>,
    boost::fusion::pair<attach_size_key, attach_size_value>,
    boost::fusion::pair<dynamic_name_key, dynamic_name_value>>
    full_fields_map;

namespace detail {
	template <typename value_type>
	QString to_qstring(const value_type& input);
	template <typename value_type>
	std::string to_string(const value_type& input);
	template <typename field_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_type>::type>::type
	from_qstring(const QString& input);
}

struct null_type;
template <typename field_type>
struct switch_type {
	typedef typename sd::static_if<boost::mpl::has_key<append_to_crypts_keys, field_type>::value,
	    append_to_crypts_keys,
	    typename sd::static_if<boost::mpl::has_key<crypt_fields_keys, field_type>::value,
					   crypt_fields_keys,
					   typename sd::static_if<boost::mpl::has_key<calculable_fields_keys, field_type>::value,
					       calculable_fields_keys, null_type // boost::units::detail::
					       >::type>::type>::type type;
};

extern QMap<Qt::CheckState, QString> char_from_check_state;
extern QMap<QString, Qt::CheckState> check_state_from_char;
extern QMap<bool, QString> char_from_bool;
extern QMap<QString, bool> bool_from_char;
extern QMap<Qt::CheckState, bool> bool_from_check_state;
extern QMap<bool, Qt::CheckState> check_state_from_bool;

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей
// версии
// Immutable parameters, ie parameters that are hard-coded in the current
// version

class FixedParameters : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(FixedParameters)
#else
	Q_OBJECT
#endif

    public:
	FixedParameters(QObject* parent = 0);
	virtual ~FixedParameters();

	// boost::mpl::set<id_type, pin_type, rating_type, name_type, author_type,
	// home_type, url_type, tags_type, ctime_type, dir_type,  file_type,
	// crypt_type, has_attach_type, attach_count_type> _record_field_static;
	QStringList _record_all_fields_key_list;
	static constexpr const full_fields_key_tuple _record_full_fields_key_tuple = full_fields_key_tuple();
	//	static constexpr const natural_field_set _record_natural_field_static =
	//natural_field_set();

	static constexpr const natural_fields_key_tuple _record_natural_fields_key_tuple = natural_fields_key_tuple();
	//	QStringList _record_natural_field;

	// boost::mpl::set<has_attach_type, attach_count_type>
	// _record_calculable_field_static;
	static constexpr const calculable_fields_key_tuple _record_calculable_fields_key_tuple = calculable_fields_key_tuple();
	//	QStringList _record_calculable_fields_key_list;

	boost::mpl::set<pin_key, name_key, author_key, home_key, url_key, tags_key> _record_field_crypted_static;
	static constexpr const crypt_fields_key_tuple _record_crypt_fields_key_tuple = crypt_fields_key_tuple();
	//	QStringList _record_crypt_fields_key_list;

	//	bool is_record_field_available(QString name) const;
	//	bool is_record_field_natural(QString name) const;
	template <typename concrete>
	static constexpr bool is_record_field_natural()
	{ // const//QString name
		// if(_record_natural_field.contains(name))
		// return true;
		// else
		// return false;
		return boost::mpl::contains<natural_fields_keys, concrete>::type::value;
		//	return _record_natural_field.contains(name);
	}

	//	bool is_record_field_calculable(QString name) const;

	QMap<QString, QString> record_field_description(QStringList list) const;

    signals:

    public slots:
    private:
	QMap<QString, QString> _description;
};

namespace detail {

#ifndef TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION
#define TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(value_type) \
	template <>                                             \
	QString to_qstring<value_type>(const value_type& input);

#endif // TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION

	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(pin_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(name_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(author_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(home_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(url_value)
	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(tags_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(id_value)
	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(rating_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(ctime_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(dir_value)
	// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(file_value)
	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(crypt_value)
	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(has_attach_value)
	TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(attach_size_value)
// TO_QSTRING_DECLARE_EXPLICIAE_SPECIALIZATION(dynamic_name_value)

#ifndef DECLARATION_TO_QSTRING
#define DECLARATION_TO_QSTRING(id_value) \
	extern template QString to_qstring<id_value>(const id_value& input);
#endif // DECLARATION_TO_QSTRING

	DECLARATION_TO_QSTRING(id_value)
	DECLARATION_TO_QSTRING(name_value)
	DECLARATION_TO_QSTRING(author_value)
	DECLARATION_TO_QSTRING(home_value)
	DECLARATION_TO_QSTRING(url_value)
	DECLARATION_TO_QSTRING(ctime_value)
	DECLARATION_TO_QSTRING(dir_value)
	DECLARATION_TO_QSTRING(file_value)
	DECLARATION_TO_QSTRING(dynamic_name_value)

#ifndef TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION
#define TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(value_type) \
	template <>                                            \
	std::string to_string<value_type>(const value_type& input);

#endif // TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION

	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(pin_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(name_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(author_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(home_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(url_value)
	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(tags_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(id_value)
	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(rating_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(ctime_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(dir_value)
	// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(file_value)
	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(crypt_value)
	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(has_attach_value)
	TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(attach_size_value)
// TO_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(dynamic_name_value)

// template <> QString to_string<rating_value>(rating_value rv);
// template <> QString to_string<attach_size_value>(attach_size_value rv);
// template <> QString to_string<crypt_value>(crypt_value rv);
// template <> QString to_string<has_attach_value>(has_attach_value rv);
// template <> QString to_string<pin_value>(pin_value rv);
// template <> QString to_string<tags_value>(tags_value rv);

#ifndef DECLARATION_TO_STRING
#define DECLARATION_TO_STRING(id_value) \
	extern template std::string to_string<id_value>(const id_value& input);
#endif // DECLARATION_TO_STRING

	DECLARATION_TO_STRING(id_value)
	DECLARATION_TO_STRING(name_value)
	DECLARATION_TO_STRING(author_value)
	DECLARATION_TO_STRING(home_value)
	DECLARATION_TO_STRING(url_value)
	DECLARATION_TO_STRING(ctime_value)
	DECLARATION_TO_STRING(dir_value)
	DECLARATION_TO_STRING(file_value)
	DECLARATION_TO_STRING(dynamic_name_value)

#ifndef FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION
#define FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(key_type)                                                             \
	template <>                                                                                                        \
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type \
	from_qstring<key_type>(const QString& input);

#endif // FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION

	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(pin_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(name_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(author_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(home_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(url_key)
	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(tags_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(id_key)
	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(rating_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(ctime_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(dir_key)
	// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(file_key)
	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(crypt_key)
	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(has_attach_key)
	FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(attach_size_key)
// FROM_STRING_DECLARE_EXPLICIAE_SPECIALIZATION(dynamic_name_key)

#ifndef DECLARATION_FROM_STRING
#define DECLARATION_FROM_STRING(field_type)                            \
	extern template typename boost::remove_reference<              \
	    typename boost::fusion::result_of::at_key<full_fields_map, \
		field_type>::type>::type                               \
	from_qstring<field_type>(const QString& input);
#endif // DECLARATION_FROM_STRING

	DECLARATION_FROM_STRING(id_key)
	DECLARATION_FROM_STRING(name_key)
	DECLARATION_FROM_STRING(author_key)
	DECLARATION_FROM_STRING(home_key)
	DECLARATION_FROM_STRING(url_key)
	DECLARATION_FROM_STRING(ctime_key)
	DECLARATION_FROM_STRING(dir_key)
	DECLARATION_FROM_STRING(file_key)
	DECLARATION_FROM_STRING(dynamic_name_key)
}

#endif // __FIXEDPARAMETERS_H__
