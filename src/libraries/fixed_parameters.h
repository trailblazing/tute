#ifndef __FIXEDPARAMETERS_H__
#define __FIXEDPARAMETERS_H__


#include <boost/units/detail/prevent_redefinition.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/at_fwd.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/has_key_fwd.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_back_fwd.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_fwd.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/insert_range_fwd.hpp>
#include <boost/mpl/set.hpp>
// #include <boost/mpl/set/set0.hpp>
// #include <boost/mpl/set/set20.hpp>

#include <wobjectdefs.h>
#include <QObject>


#include <QMap>
#include <QStringList>
#include "utility/delegate.h"

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


typedef boost::mpl::string<'i', 'd'> id_type;
typedef boost::mpl::string<'p', 'i', 'n'> pin_type;
typedef boost::mpl::string<'r', 'a', 't', 'i', 'n', 'g'> rating_type;
typedef boost::mpl::string<'n', 'a', 'm', 'e'> name_type;
typedef boost::mpl::string<'a', 'u', 't', 'h', 'o', 'r'> author_type;
typedef boost::mpl::string<'h', 'o', 'm', 'e'> home_type;
typedef boost::mpl::string<'u', 'r', 'l'> url_type;
typedef boost::mpl::string<'t', 'a', 'g', 's'> tags_type;
typedef boost::mpl::string<'c', 't', 'i', 'm', 'e'> ctime_type;
typedef boost::mpl::string<'d', 'i', 'r'> dir_type;
typedef boost::mpl::string<'f', 'i', 'l', 'e'> file_type;
typedef boost::mpl::string<'c', 'r', 'y', 'p', 't'> crypt_type;
typedef boost::mpl::string<'h', 's', 't', 'c', 'h'> has_attach_type;    // typedef boost::mpl::string<'has_', 'atta', 'ch'> has_attach_type;    // with "-Wmultichar" parameter for make
typedef boost::mpl::string<'t', 'c', 'h', 'c', 'n', 't'> attach_count_type;   // typedef boost::mpl::string<'atta', 'ch_c', 'ount'> attach_count_type;
typedef boost::mpl::string<'d', 'n', 'm', 'c', 'n', 'm'> dynamic_name_type;

typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type> crypt_field_type;
typedef boost::mpl::set<id_type, rating_type, ctime_type, dir_type, file_type, crypt_type> append_to_crypt_type;
// typedef boost::mpl::push_back<crypt_field_type, append_to_crypt_type> natural_field_type;
typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type> natural_field_type;

typedef boost::mpl::set<has_attach_type, attach_count_type, dynamic_name_type> calculable_field_type;

// typedef boost::mpl::push_back<natural_field_type, calculable_field_type> full_field_type;
typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type, id_type, rating_type, ctime_type, dir_type, file_type, crypt_type, has_attach_type, attach_count_type, dynamic_name_type> full_field_type;

struct null_type;
template<typename field_type>struct switch_type {
    typedef typename sd::static_if<boost::mpl::has_key<append_to_crypt_type, field_type>::value
            , append_to_crypt_type
            , typename sd::static_if<boost::mpl::has_key<crypt_field_type, field_type>::value
            , crypt_field_type
            , typename sd::static_if<boost::mpl::has_key<calculable_field_type, field_type>::value
            , calculable_field_type
            , null_type // boost::units::detail::
            >::type >::type >::type type;
};

// Неизменяемые параметры, то есть параметры, которые заданы жестко в текущей версии MyTetra
// Immutable parameters, ie parameters that are hard-coded in the current version MyTetra

class FixedParameters : public QObject {
    W_OBJECT(FixedParameters)

    public:
        FixedParameters(QObject *parent = 0);
        virtual ~FixedParameters();

// boost::mpl::set<id_type, pin_type, rating_type, name_type, author_type, home_type, url_type, tags_type, ctime_type, dir_type,  file_type, crypt_type, has_attach_type, attach_count_type> _record_field_static;
        QStringList _record_field;

// boost::mpl::set<id_type, pin_type, rating_type, name_type, author_type, home_type, url_type, tags_type, ctime_type, dir_type,  file_type, crypt_type> _record_natural_field_static;
        QStringList _record_natural_field;

// boost::mpl::set<has_attach_type, attach_count_type> _record_calculable_field_static;
        QStringList _record_calculable_field;

        boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type> _record_field_crypted_static;
        QStringList _record_field_crypted;

        bool is_record_field_available(QString name) const;
        bool is_record_field_natural(QString name) const;
        bool is_record_field_calculable(QString name) const;

        QMap<QString, QString> record_field_description(QStringList list) const;

    signals:

    public slots:
};

#endif // __FIXEDPARAMETERS_H__
