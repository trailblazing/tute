
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QMutableMapIterator>
#include <QString>
#include <QStringList>

#include "fixed_parameters.h"
#include "views/record/info_field_enter.h"

// const char *id_field = "id";
// const char *pin_field = "pin";
// const char *rating_field = "rating";
// const char *name_field = "name";
// const char *author_field = "author";
// const char *home_field = "home";
// const char *url_field = "url";
// const char *tags_field = "tags";
// const char *ctime_field = "ctime";
// const char *dir_field = "dir";
// const char *file_field = "file";
// const char *crypt_field = "crypt";
// const char *has_attach_field = "hstch";     // "hasAttach";
// const char *attach_count_field = "tchcnt";  // "attachCount";

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(FixedParameters)
#endif

//constexpr const natural_field_type FixedParameters::_record_natural_field_static = natural_field_type();

constexpr const natural_field_tuple FixedParameters::_record_natural_field_tuple; // = natural_field_tuple();
extern QMap<Qt::CheckState, QString> char_from_check_state;
extern QMap<QString, Qt::CheckState> check_state_from_char;

// Получение описаний набора полей
QMap<QString, QString> FixedParameters::record_field_description(QStringList list) const
{
    QMap<QString, QString> description;

    // _description[boost::mpl::c_str < id_type > ::value]			= tr("ID");
    // _description[boost::mpl::c_str < pin_type > ::value]		= tr("Pin");
    // _description[boost::mpl::c_str < rating_type > ::value]		= tr("Rating");
    // _description[boost::mpl::c_str < name_type > ::value]		= tr("Title");
    // _description[boost::mpl::c_str < author_type > ::value]		= tr("Author");
    // _description[boost::mpl::c_str < home_type > ::value]		= tr("Home");
    // _description[boost::mpl::c_str < url_type > ::value]		= tr("Url");
    // _description[boost::mpl::c_str < tags_type > ::value]		= tr("Tags");
    // _description[boost::mpl::c_str < ctime_type > ::value]		= tr("Create time");
    // _description[boost::mpl::c_str < dir_type > ::value]		= tr("Directory name");
    // _description[boost::mpl::c_str < file_type > ::value]		= tr("File name");
    // _description[boost::mpl::c_str < crypt_type > ::value]		= tr("Is crypt");
    // _description[boost::mpl::c_str < has_attach_type > ::value]		= tr("Has attachs");
    // _description[boost::mpl::c_str < attach_count_type > ::value]	= tr("Attachs count");
    // _description[boost::mpl::c_str < dynamic_name_type > ::value]	= tr("Dynamic name");

    // Удаляются строчки, которых нет в переданном списке
    QMapIterator<QString, QString> iterator(_description); // QMutableMapIterator<QString, QString> iterator(_description);
    while (iterator.hasNext()) {
        iterator.next();
        if (list.contains(iterator.key()))
            description[iterator.key()] = _description[iterator.key()]; // if(list.contains(iterator.key()) == false)iterator.remove();
    }
    return description;
}

FixedParameters::FixedParameters(QObject* parent)
    : QObject(parent)
{
    Q_UNUSED(parent);
    // QMap<QString, QString> _description;

    _description[boost::mpl::c_str<id_type>::value] = tr("ID");
    _description[boost::mpl::c_str<pin_type>::value] = tr("Pin");
    _description[boost::mpl::c_str<rating_type>::value] = tr("Rating");
    _description[boost::mpl::c_str<name_type>::value] = tr("Title");
    _description[boost::mpl::c_str<author_type>::value] = tr("Author");
    _description[boost::mpl::c_str<home_type>::value] = tr("Home");
    _description[boost::mpl::c_str<url_type>::value] = tr("Url");
    _description[boost::mpl::c_str<tags_type>::value] = tr("Tags");
    _description[boost::mpl::c_str<ctime_type>::value] = tr("Create time");
    _description[boost::mpl::c_str<dir_type>::value] = tr("Directory name");
    _description[boost::mpl::c_str<file_type>::value] = tr("File name");
    _description[boost::mpl::c_str<crypt_type>::value] = tr("Is crypt");
    _description[boost::mpl::c_str<has_attach_type>::value] = tr("Has attachs");
    _description[boost::mpl::c_str<attach_size_type>::value] = tr("Attachs count");
    _description[boost::mpl::c_str<dynamic_name_type>::value] = tr("Dynamic name");

    _record_field = (QStringList()
        << boost::mpl::c_str<id_type>::value
        << boost::mpl::c_str<pin_type>::value
        << boost::mpl::c_str<rating_type>::value
        << boost::mpl::c_str<name_type>::value
        << boost::mpl::c_str<author_type>::value
        << boost::mpl::c_str<home_type>::value
        << boost::mpl::c_str<url_type>::value
        << boost::mpl::c_str<tags_type>::value
        << boost::mpl::c_str<ctime_type>::value
        << boost::mpl::c_str<dir_type>::value
        << boost::mpl::c_str<file_type>::value
        << boost::mpl::c_str<crypt_type>::value
        << boost::mpl::c_str<has_attach_type>::value
        << boost::mpl::c_str<attach_size_type>::value
        << boost::mpl::c_str<dynamic_name_type>::value);

    //	_record_natural_field = (QStringList()
    //	                                << boost::mpl::c_str<id_type>::value
    //	                                << boost::mpl::c_str<pin_type>::value
    //	                                << boost::mpl::c_str<rating_type>::value
    //	                                << boost::mpl::c_str<name_type>::value
    //	                                << boost::mpl::c_str<author_type>::value
    //	                                << boost::mpl::c_str<home_type>::value
    //	                                << boost::mpl::c_str<url_type>::value
    //	                                << boost::mpl::c_str<tags_type>::value
    //	                                << boost::mpl::c_str<ctime_type>::value
    //	                                << boost::mpl::c_str<dir_type>::value
    //	                                << boost::mpl::c_str<file_type>::value
    //	                                << boost::mpl::c_str<crypt_type>::value);

    _record_calculable_field = (QStringList()
        << boost::mpl::c_str<has_attach_type>::value
        << boost::mpl::c_str<attach_size_type>::value
        << boost::mpl::c_str<dynamic_name_type>::value);

    _record_field_crypted = (QStringList()
        << boost::mpl::c_str<pin_type>::value
        << boost::mpl::c_str<name_type>::value
        << boost::mpl::c_str<author_type>::value
        << boost::mpl::c_str<home_type>::value
        << boost::mpl::c_str<url_type>::value
        << boost::mpl::c_str<tags_type>::value);
}

FixedParameters::~FixedParameters()
{
}

/*

   //// Перечень всех допустимых полей - натуральных и вычислимых
   //QStringList FixedParameters::record_field_availableList(void) const
   //{
   //    // Для скорости задаются напрямую, не вызывая функции формирования из натурального и вычислимого списка полей

   //    return (QStringList() << "id" \
   //            << name_field \
   //            << author_field \
   //            << url_field \
   //            << tags_field \
   //            << ctime_field \
   //            << dir_field \
   //            << file_field \
   //            << crypt_field \
   //            \
   //            << has_attach_field \
   //            << attach_count_field);
   //}


   //// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся в XML тегах
   //QStringList FixedParameters::record_natural_field_available_list(void) const
   //{
   //    return (QStringList() << "id" \
   //            << name_field \
   //            << author_field \
   //            << url_field \
   //            << tags_field \
   //            << ctime_field \
   //            << dir_field \
   //            << file_field \
   //            << crypt_field);
   //}


   //// Перечень всех вычислимых полей - такие поля нигде не сохраняются
   //QStringList FixedParameters::record_calculable_field_available_list(void) const
   //{
   //    return (QStringList() << has_attach_field \
   //            << attach_count_field);
   //}

 */

bool FixedParameters::is_record_field_available(QString name) const
{
    // if(_record_field.contains(name))
    // return true;
    // else
    // return false;
    return _record_field.contains(name);
}

//template<typename concrete>
//bool FixedParameters::is_record_field_natural() const {//QString name
//	// if(_record_natural_field.contains(name))
//	// return true;
//	// else
//	// return false;
//	return boost::mpl::contains<natural_field_type, concrete>::type::value;
////	return _record_natural_field.contains(name);
//}

bool FixedParameters::is_record_field_calculable(QString name) const
{
    // if(_record_calculable_field.contains(name))
    // return true;
    // else
    // return false;

    return _record_calculable_field.contains(name);
}

// QStringList FixedParameters::record_field_crypted_list(void) const
// {
// QStringList names;

// names << name_field;
// names << author_field;
// names << url_field;
// names << tags_field;

// return names;
// }

//BOOST_STRONG_TYPEDEF(QString, id_value)
//BOOST_STRONG_TYPEDEF(bool, pin_value)
//BOOST_STRONG_TYPEDEF(QString, name_value)
//BOOST_STRONG_TYPEDEF(QString, author_value)
//BOOST_STRONG_TYPEDEF(QString, home_value)
//BOOST_STRONG_TYPEDEF(QString, url_value)
//BOOST_STRONG_TYPEDEF(QString, tags_value)
//BOOST_STRONG_TYPEDEF(int, rating_value)
//BOOST_STRONG_TYPEDEF(QString, ctime_value)
//BOOST_STRONG_TYPEDEF(QString, dir_value)
//BOOST_STRONG_TYPEDEF(QString, file_value)
//BOOST_STRONG_TYPEDEF(bool, crypt_value)
//BOOST_STRONG_TYPEDEF(bool, has_attach_value)
//BOOST_STRONG_TYPEDEF(int, attach_size_value)
//BOOST_STRONG_TYPEDEF(QString, dynamic_name_value)
namespace detail {
template <typename value_type>
QString to_string(const value_type& input) { return input; }

#ifndef DECLARATION_TO_STRING
#define DECLARATION_TO_STRING(id_value) \
    extern template QString to_string<id_value>(const id_value& input);
#endif //DECLARATION_TO_STRING

DECLARATION_TO_STRING(id_value)
DECLARATION_TO_STRING(name_value)
DECLARATION_TO_STRING(author_value)
DECLARATION_TO_STRING(home_value)
DECLARATION_TO_STRING(url_value)
DECLARATION_TO_STRING(ctime_value)
DECLARATION_TO_STRING(dir_value)
DECLARATION_TO_STRING(file_value)
DECLARATION_TO_STRING(dynamic_name_value)

#ifndef DEFINITION_TO_STRING
#define DEFINITION_TO_STRING(id_value) \
    template QString to_string<id_value>(const id_value& input);
#endif //DEFINITION_TO_STRING

DEFINITION_TO_STRING(id_value)
DEFINITION_TO_STRING(name_value)
DEFINITION_TO_STRING(author_value)
DEFINITION_TO_STRING(home_value)
DEFINITION_TO_STRING(url_value)
DEFINITION_TO_STRING(ctime_value)
DEFINITION_TO_STRING(dir_value)
DEFINITION_TO_STRING(file_value)
DEFINITION_TO_STRING(dynamic_name_value)

template <>
QString to_string<rating_value>(const rating_value& input) { return QString::number(input); }

template <>
QString to_string<attach_size_value>(const attach_size_value& input) { return QString::number(input); }

template <>
QString to_string<crypt_value>(const crypt_value& input) { return input ? "1" : ""; }

template <>
QString to_string<has_attach_value>(const has_attach_value& input) { return input ? "1" : ""; }

template <>
QString to_string<pin_value>(const pin_value& input) { return input ? char_from_check_state[Qt::CheckState::Checked] : char_from_check_state[Qt::CheckState::Unchecked]; }

template <>
QString to_string<tags_value>(const tags_value& input)
{
    QString result;
    auto rvsl = static_cast<QStringList>(input);
    for (int i = 0; i < rvsl.count(); i++) {
	result += (i == 0) ? "" : "," + rvsl.at(i);
    }
    return result;
}

template <typename field_type>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_type>::type>::type from_string(const QString& input)
{
    typedef typename boost::fusion::result_of::at_key<full_field_map, field_type>::type value_type;
    return value_type(input);
}

#ifndef DECLARATION_FROM_STRING
#define DECLARATION_FROM_STRING(field_type) \
    extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_type>::type>::type from_string<field_type>(const QString& input);
#endif //DECLARATION_FROM_STRING

DECLARATION_FROM_STRING(id_type)
DECLARATION_FROM_STRING(name_type)
DECLARATION_FROM_STRING(author_type)
DECLARATION_FROM_STRING(home_type)
DECLARATION_FROM_STRING(url_type)
DECLARATION_FROM_STRING(ctime_type)
DECLARATION_FROM_STRING(dir_type)
DECLARATION_FROM_STRING(file_type)
DECLARATION_FROM_STRING(dynamic_name_type)

#ifndef DEFINITION_FROM_STRING
#define DEFINITION_FROM_STRING(field_type) \
    template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, field_type>::type>::type from_string<field_type>(const QString& input);
#endif //DEFINITION_FROM_STRING

DEFINITION_FROM_STRING(id_type)
DEFINITION_FROM_STRING(name_type)
DEFINITION_FROM_STRING(author_type)
DEFINITION_FROM_STRING(home_type)
DEFINITION_FROM_STRING(url_type)
DEFINITION_FROM_STRING(ctime_type)
DEFINITION_FROM_STRING(dir_type)
DEFINITION_FROM_STRING(file_type)
DEFINITION_FROM_STRING(dynamic_name_type)

template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, rating_type>::type>::type from_string<rating_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, rating_type>::type>::type value_type;
    return value_type(input.toInt());
}
template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, attach_size_type>::type>::type from_string<attach_size_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, attach_size_type>::type>::type value_type;
    return value_type(input.toInt());
}
template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, pin_type>::type>::type from_string<pin_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, pin_type>::type>::type value_type;
    return value_type(bool_from_char[input]);
}
template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, crypt_type>::type>::type from_string<crypt_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, crypt_type>::type>::type value_type;
    return value_type(input == "" ? false : true);
}
template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, has_attach_type>::type>::type from_string<has_attach_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, has_attach_type>::type>::type value_type;
    return value_type(input == "" ? false : true);
}

template <>
typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, tags_type>::type>::type from_string<tags_type>(const QString& input)
{
    typedef typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_field_map, tags_type>::type>::type value_type;
    return value_type(input.split(QRegExp("[,:]+"), QString::SkipEmptyParts));
}
}
