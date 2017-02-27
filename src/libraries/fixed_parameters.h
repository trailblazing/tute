#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__

#include <boost/mpl/at.hpp>
#include <boost/mpl/at_fwd.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/has_key_fwd.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_fwd.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/insert_range_fwd.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_back_fwd.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/mpl/string.hpp>
#include <boost/units/detail/prevent_redefinition.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/assert.hpp>
// #include <boost/mpl/set/set0.hpp>
// #include <boost/mpl/set/set20.hpp>
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
# define BOOST_MPL_LIMIT_STRING_SIZE 256
#endif

typedef boost::mpl::string<'i', 'd'>					id_type;
typedef boost::mpl::string<'p', 'i', 'n'>				pin_type;
typedef boost::mpl::string<'r', 'a', 't', 'i', 'n', 'g'>		rating_type;
typedef boost::mpl::string<'n', 'a', 'm', 'e'>				name_type;
typedef boost::mpl::string<'a', 'u', 't', 'h', 'o', 'r'>		author_type;
typedef boost::mpl::string<'h', 'o', 'm', 'e'>				home_type;
typedef boost::mpl::string<'u', 'r', 'l'>				url_type;
typedef boost::mpl::string<'t', 'a', 'g', 's'>				tags_type;
typedef boost::mpl::string<'c', 't', 'i', 'm', 'e'>			ctime_type;
typedef boost::mpl::string<'d', 'i', 'r'>				dir_type;
typedef boost::mpl::string<'f', 'i', 'l', 'e'>				file_type;
typedef boost::mpl::string<'c', 'r', 'y', 'p', 't'>			crypt_type;
typedef boost::mpl::string<'h', 'a', 's', '_', 'a', 't', 'c', 'h'>	has_attach_type; //
//typedef boost::mpl::string<'has_', 'a','t','t','a', 'c','h'> has_attach_type;
// http://stackoverflow.com/questions/37606999/boostmplstring-size-error-messages
// with "-Wmultichar" parameter for make // -Wno-multichar won't disable it
typedef boost::mpl::string<'a', 't', 'c', 'h', 's', 'i', 'z', 'e'> attach_size_type; //
//typedef boost::mpl::string<'atta', 'c','h','_','c', 'o','u','n','t'>	attach_count_type;// error: too many template arguments for class template 'string'

typedef boost::mpl::string<'d', 'y', 'n', '_', 'n', 'a', 'm', 'e'> dynamic_name_type;
//typedef boost::mpl::string<'dyna', 'm','i','c','_', 'n','a','m','e'>	dynamic_name_type;

typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type>	crypt_field_set;
typedef boost::mpl::map<pin_type, name_type, author_type, home_type, url_type, tags_type>	crypt_field_map;
typedef std::tuple<pin_type, name_type, author_type, home_type, url_type, tags_type>		crypt_field_tuple;

typedef boost::mpl::set<id_type, rating_type, ctime_type, dir_type, file_type, crypt_type>	append_to_crypt_set;
typedef std::tuple<id_type, rating_type, ctime_type, dir_type, file_type, crypt_type>		append_to_crypt_tuple;
// typedef boost::mpl::push_back<crypt_field_type, append_to_crypt_type> natural_field_type;
typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type>	natural_field_set;
typedef std::tuple<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type>		natural_field_tuple;

#ifdef USE_COMPILE_TIME_CACULATION
//using natural_field_fold = boost::mpl::fold <
//			   crypt_field_set
//      , boost::mpl::set0<>
//      , boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>::type;

typedef boost::mpl::copy<crypt_field_set, boost::mpl::back_inserter<append_to_crypt_set> >::type natural_field_concatenated;

typedef boost::mpl::joint_view<
	crypt_field_set
	, append_to_crypt_set
	> natural_field_concatenated_view;
natural_field_concatenated::dog;
natural_field_set::fish;
natural_field_concatenated_view::pig;
BOOST_MPL_ASSERT((boost::mpl::equal<natural_field_concatenated_view, natural_field_concatenated> ));

static_assert(sd::STATIC_SAME<natural_field_concatenated, natural_field_set>::value == true, "types are not euqal");
//
//typedef Unify<
//	std::tuple
//	, natural_field_set
//	, boost::mpl::size<natural_field_set>::type::value
//	>::type natural_field_tuple;
#endif // USE_COMPILE_TIME_CACULATION

typedef boost::mpl::set<has_attach_type, attach_size_type, dynamic_name_type>	calculable_field_set;
typedef std::tuple<has_attach_type, attach_size_type, dynamic_name_type>	calculable_field_tuple;
// typedef boost::mpl::push_back<natural_field_type, calculable_field_type> full_field_type;
typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type, has_attach_type, attach_size_type, dynamic_name_type>	full_field_set;
typedef std::tuple<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type, has_attach_type, attach_size_type, dynamic_name_type>		full_field_tuple;

struct null_type;
template <typename field_type>
struct switch_type {
	typedef typename sd::static_if<boost::mpl::has_key<append_to_crypt_set, field_type>::value
		, append_to_crypt_set
		, typename sd::static_if<boost::mpl::has_key<crypt_field_set, field_type>::value
			, crypt_field_set
			, typename sd::static_if<boost::mpl::has_key<calculable_field_set, field_type>::value
				, calculable_field_set, null_type // boost::units::detail::
				>::type>::type>::type type;
};

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии
// Immutable parameters, ie parameters that are hard-coded in the current version

class FixedParameters : public QObject {
#if QT_VERSION == 0x050600
W_OBJECT(FixedParameters)
#else
Q_OBJECT
#endif

public:
	FixedParameters(QObject *parent = 0);
	virtual ~FixedParameters();

	// boost::mpl::set<id_type, pin_type, rating_type, name_type, author_type, home_type, url_type, tags_type, ctime_type, dir_type,  file_type, crypt_type, has_attach_type, attach_count_type> _record_field_static;
	QStringList _record_field;

//	static constexpr const natural_field_set _record_natural_field_static = natural_field_set();

	static constexpr const natural_field_tuple _record_natural_field_tuple = natural_field_tuple();
//	QStringList _record_natural_field;


	// boost::mpl::set<has_attach_type, attach_count_type> _record_calculable_field_static;
	static constexpr const calculable_field_tuple _record_calculable_field_tuple = calculable_field_tuple();
	QStringList _record_calculable_field;

	boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type> _record_field_crypted_static;
	static constexpr const crypt_field_tuple _record_field_crypted_tuple = crypt_field_tuple();
	QStringList _record_field_crypted;

	bool is_record_field_available(QString name) const;
//	bool is_record_field_natural(QString name) const;
	template<typename concrete>
	static constexpr bool is_record_field_natural(){  //const//QString name
		// if(_record_natural_field.contains(name))
		// return true;
		// else
		// return false;
		return boost::mpl::contains<natural_field_set, concrete>::type::value;
		//	return _record_natural_field.contains(name);
	}

	bool is_record_field_calculable(QString name) const;

	QMap<QString, QString> record_field_description(QStringList list) const;

signals:

public slots:
private:
	QMap<QString, QString> _description;
};

#endif // __FIXEDPARAMETERS_H__
