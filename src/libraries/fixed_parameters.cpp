
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "fixed_parameters.h"
#include "main.h"
#include "views/browser/browser.h"
#include "views/record/info_field_enter.h"
#include <QMutableMapIterator>
#include <QString>
#include <QStringList>

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

// constexpr const natural_field_type
// FixedParameters::_record_natural_field_static = natural_field_type();

constexpr const natural_fields_key_tuple FixedParameters::_record_natural_fields_key_tuple; // = natural_field_tuple();
constexpr const full_fields_key_tuple FixedParameters::_record_full_fields_key_tuple;
extern QMap<Qt::CheckState, QString> char_from_check_state;
extern QMap<QString, Qt::CheckState> check_state_from_char;

// Получение описаний набора полей
QMap<QString, QString>
FixedParameters::record_field_description(QStringList list) const
{
	QMap<QString, QString> description;

	// Удаляются строчки, которых нет в переданном списке
	QMapIterator<QString, QString> iterator(_description); // QMutableMapIterator<QString, QString>
							       // iterator(_description);
	while (iterator.hasNext()) {
		iterator.next();
		if (list.contains(iterator.key()))
			description[iterator.key()] = _description[iterator.key()]; // if(list.contains(iterator.key()) ==
										    // false)iterator.remove();
	}
	return description;
}

FixedParameters::FixedParameters(QObject* parent)
    : QObject(parent)
{
	Q_UNUSED(parent);
	// QMap<QString, QString> _description;

	_description[boost::mpl::c_str<name_key>::value] = tr("Title");
	_description[boost::mpl::c_str<author_key>::value] = tr("Author");
	_description[boost::mpl::c_str<home_key>::value] = tr("Home");
	_description[boost::mpl::c_str<url_key>::value] = tr("Url");
	_description[boost::mpl::c_str<tags_key>::value] = tr("Tags");
	_description[boost::mpl::c_str<id_key>::value] = tr("ID");
	_description[boost::mpl::c_str<pin_key>::value] = tr("Pin");
	_description[boost::mpl::c_str<rating_key>::value] = tr("Rating");
	_description[boost::mpl::c_str<ctime_key>::value] = tr("Create time");
	_description[boost::mpl::c_str<dir_key>::value] = tr("Directory name");
	_description[boost::mpl::c_str<file_key>::value] = tr("File name");
	_description[boost::mpl::c_str<crypt_key>::value] = tr("Is crypt");
	_description[boost::mpl::c_str<has_attach_key>::value] = tr("Has attachs");
	_description[boost::mpl::c_str<attach_size_key>::value] = tr("Attachs count");
	_description[boost::mpl::c_str<dynamic_name_key>::value] = tr("Dynamic name");

	_record_all_fields_key_list =
	    (QStringList() << boost::mpl::c_str<name_key>::value
			   << boost::mpl::c_str<author_key>::value
			   << boost::mpl::c_str<home_key>::value
			   << boost::mpl::c_str<url_key>::value
			   << boost::mpl::c_str<tags_key>::value
			   << boost::mpl::c_str<id_key>::value
			   << boost::mpl::c_str<pin_key>::value
			   << boost::mpl::c_str<rating_key>::value
			   << boost::mpl::c_str<ctime_key>::value
			   << boost::mpl::c_str<dir_key>::value
			   << boost::mpl::c_str<file_key>::value
			   << boost::mpl::c_str<crypt_key>::value
			   << boost::mpl::c_str<has_attach_key>::value
			   << boost::mpl::c_str<attach_size_key>::value
			   << boost::mpl::c_str<dynamic_name_key>::value);

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
	//	                                <<
	//boost::mpl::c_str<crypt_type>::value);

	//	_record_calculable_fields_key_list =
	//	    (QStringList() << boost::mpl::c_str<has_attach_key>::value
	//	                   << boost::mpl::c_str<attach_size_key>::value
	//	                   << boost::mpl::c_str<dynamic_name_key>::value);

	//	_record_crypt_fields_key_list =
	//	    (QStringList() << boost::mpl::c_str<pin_key>::value
	//	                   << boost::mpl::c_str<name_key>::value
	//	                   << boost::mpl::c_str<author_key>::value
	//	                   << boost::mpl::c_str<home_key>::value
	//	                   << boost::mpl::c_str<url_key>::value
	//	                   << boost::mpl::c_str<tags_key>::value);
}

FixedParameters::~FixedParameters()
{
}

/*

   //// Перечень всех допустимых полей - натуральных и вычислимых
   //QStringList FixedParameters::record_field_availableList(void) const
   //{
   //    // Для скорости задаются напрямую, не вызывая функции формирования из
   натурального и вычислимого списка полей

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


   //// Перечень всех натуральных полей - то есть тех, которые напрямую хранятся
   в XML тегах
   //QStringList FixedParameters::record_natural_field_available_list(void)
   const
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
   //QStringList FixedParameters::record_calculable_field_available_list(void)
   const
   //{
   //    return (QStringList() << has_attach_field \
   //            << attach_count_field);
   //}

 */

//bool FixedParameters::is_record_field_available(QString name) const {
//	// if(_record_field.contains(name))
//	// return true;
//	// else
//	// return false;
//	return _record_field.contains(name);
//}

// template<typename concrete>
// bool FixedParameters::is_record_field_natural() const {//QString name
//	// if(_record_natural_field.contains(name))
//	// return true;
//	// else
//	// return false;
//	return boost::mpl::contains<natural_field_type, concrete>::type::value;
////	return _record_natural_field.contains(name);
//}

//bool FixedParameters::is_record_field_calculable(QString name) const {
//	// if(_record_calculable_field.contains(name))
//	// return true;
//	// else
//	// return false;

//	return _record_calculable_fields_key_list.contains(name);
//}

// QStringList FixedParameters::record_field_crypted_list(void) const
// {
// QStringList names;

// names << name_field;
// names << author_field;
// names << url_field;
// names << tags_field;

// return names;
// }

// BOOST_STRONG_TYPEDEF_EXTENSION(QString, id_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(bool, pin_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, name_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, author_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, home_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, url_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, tags_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(int, rating_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, ctime_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, dir_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, file_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(bool, crypt_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(bool, has_attach_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(int, attach_size_value)
// BOOST_STRONG_TYPEDEF_EXTENSION(QString, dynamic_name_value)
namespace detail {

	template <typename value_type>
	QString to_qstring(const value_type& input)
	{
		return input;
	}

#ifndef DEFINITION_TO_QSTRING
#define DEFINITION_TO_QSTRING(id_value) \
	template QString to_qstring<id_value>(const id_value& input);
#endif // DEFINITION_TO_QSTRING

	DEFINITION_TO_QSTRING(id_value)
	DEFINITION_TO_QSTRING(name_value)
	DEFINITION_TO_QSTRING(author_value)
	DEFINITION_TO_QSTRING(home_value)
	DEFINITION_TO_QSTRING(url_value)
	DEFINITION_TO_QSTRING(ctime_value)
	DEFINITION_TO_QSTRING(dir_value)
	DEFINITION_TO_QSTRING(file_value)
	DEFINITION_TO_QSTRING(dynamic_name_value)

	template <>
	QString to_qstring<rating_value>(const rating_value& input)
	{
		return QString::number(input);
	}

	template <>
	QString to_qstring<attach_size_value>(const attach_size_value& input)
	{
		return QString::number(input);
	}

	template <>
	QString to_qstring<crypt_value>(const crypt_value& input)
	{
		return input ? "1" : "";
	}

	template <>
	QString to_qstring<has_attach_value>(const has_attach_value& input)
	{
		return input ? "1" : "";
	}

	template <>
	QString to_qstring<pin_value>(const pin_value& input)
	{
		return input ? char_from_check_state[Qt::CheckState::Checked] : char_from_check_state[Qt::CheckState::Unchecked];
	}

	template <>
	QString to_qstring<tags_value>(const tags_value& input)
	{
		QString result;
		auto rvsl = QStringList(input);
		for (int i = 0; i < rvsl.count(); i++) {
			result += ((i == 0) ? "" : ",") + rvsl.at(i); //(i == 0) ? "" : "," + rvsl.at(i); // this implementation is stupid
		}
		return result;
	}

	template <typename value_type>
	std::string to_string(const value_type& input)
	{
		return QString(input).toStdString();
	}

#ifndef DEFINITION_TO_STRING
#define DEFINITION_TO_STRING(id_value) \
	template std::string to_string<id_value>(const id_value& input);
#endif // DEFINITION_TO_STRING

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
	std::string to_string<rating_value>(const rating_value& input)
	{
		return QString::number(input).toStdString();
	}

	template <>
	std::string to_string<attach_size_value>(const attach_size_value& input)
	{
		return QString::number(input).toStdString();
	}

	template <>
	std::string to_string<crypt_value>(const crypt_value& input)
	{
		return QString(input ? "1" : "").toStdString();
	}

	template <>
	std::string to_string<has_attach_value>(const has_attach_value& input)
	{
		return QString(input ? "1" : "").toStdString();
	}

	template <>
	std::string to_string<pin_value>(const pin_value& input)
	{
		return QString(input ? char_from_check_state[Qt::CheckState::Checked] : char_from_check_state[Qt::CheckState::Unchecked])
		    .toStdString();
	}

	template <>
	std::string to_string<tags_value>(const tags_value& input)
	{
		std::string result;
		auto rvsl = static_cast<QStringList>(input);
		for (int i = 0; i < rvsl.count(); i++) {
			result += std::string((i == 0) ? "" : ",") + rvsl.at(i).toStdString();
		}
		return result;
	}

	template <typename field_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_type>::type>::type
	from_qstring(const QString& input)
	{
		typedef
		    typename boost::fusion::result_of::at_key<full_fields_map, field_type>::type value_type;
		return value_type(input);
	}

#ifndef DEFINITION_FROM_STRING
#define DEFINITION_FROM_STRING(field_type)                             \
	template typename boost::remove_reference<                     \
	    typename boost::fusion::result_of::at_key<full_fields_map, \
		field_type>::type>::type                               \
	from_qstring<field_type>(const QString& input);
#endif // DEFINITION_FROM_STRING

	DEFINITION_FROM_STRING(id_key)
	DEFINITION_FROM_STRING(name_key)
	DEFINITION_FROM_STRING(author_key)
	DEFINITION_FROM_STRING(home_key)
	DEFINITION_FROM_STRING(url_key)
	DEFINITION_FROM_STRING(ctime_key)
	DEFINITION_FROM_STRING(dir_key)
	DEFINITION_FROM_STRING(file_key)
	DEFINITION_FROM_STRING(dynamic_name_key)

	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, rating_key>::type>::type
	from_qstring<rating_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, rating_key>::type>::type value_type;
		return value_type(input.toInt());
	}
	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, attach_size_key>::type>::type
	from_qstring<attach_size_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, attach_size_key>::type>::type value_type;
		return value_type(input.toInt());
	}
	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, pin_key>::type>::type
	from_qstring<pin_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, pin_key>::type>::type value_type;
		return value_type(bool_from_char[input]);
	}
	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, crypt_key>::type>::type
	from_qstring<crypt_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, crypt_key>::type>::type value_type;
		return value_type((input == "" || input == "0") ? false : true);
	}
	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, has_attach_key>::type>::type
	from_qstring<has_attach_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, has_attach_key>::type>::type value_type;
		return value_type(input == "" ? false : true);
	}

	template <>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<
	    full_fields_map, tags_key>::type>::type
	from_qstring<tags_key>(const QString& input)
	{
		typedef typename boost::remove_reference<
		    typename boost::fusion::result_of::at_key<
			full_fields_map, tags_key>::type>::type value_type;
		return value_type(input.split(QRegExp("[,:]+"), QString::SkipEmptyParts));
	}
}

//template <>
//boost::intrusive_ptr<::real_url_t<QString>> real_url_t<QString>::
//    instance(const QString& url_)
//{
//	auto real_url_ = to_be_url(url_);
//	if (!(real_url_ == QUrl() && url_ != web::Browser::_defaulthome // || real_url_ == detail::to_qstring(web::Browser::_defaulthome)
//		)) {
//		critical_error("The url is valid, that\'s not what you want");
//		//		throw std::runtime_error("url is not valid");
//	}
//	return new ::real_url_t<QString>(url_);
//}

//template <>
//boost::intrusive_ptr<::real_url_t<url_value>> real_url_t<url_value>::
//    instance(const url_value& url_)
//{
//	auto real_url_ = to_be_url(url_);
//	if (real_url_ == QUrl() && url_ != web::Browser::_defaulthome // || real_url_ == detail::to_qstring(web::Browser::_defaulthome)
//	    ) {
//		critical_error("The url is not valid");
//		//		throw std::runtime_error("url is not valid");
//	}
//	return new ::real_url_t<url_value>(url_);
//}

template struct real_url_t<QString>;
template struct real_url_t<url_value>;

namespace detail {

	template <typename output_t>
	output_t //boost::intrusive_ptr<::real_url_t<QString>> //real_url_t<QString>::
	    real_url_instance(const QString& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<QString>>)> func_)
	{
		output_t result = nullptr;
		auto real_url_ = to_be_url(url_);
		if (real_url_ == QUrl() && url_ != web::Browser::_defaulthome // || real_url_ == detail::to_qstring(web::Browser::_defaulthome)
		    ) {
			//			critical_error("The url is valid, that\'s not what you want");// throw std::runtime_error("url is not valid");
			//		} else
			result = func_(new ::real_url_t<QString>(url_));
		}
		return result;
	}

	template web::Browser* //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const QString& url_, std::function<web::Browser*(boost::intrusive_ptr<real_url_t<QString>>)>);

	template <typename output_t>
	output_t //boost::intrusive_ptr<::real_url_t<url_value>> //real_url_t<url_value>::
	    real_url_instance(const url_value& url_, std::function<output_t(boost::intrusive_ptr<real_url_t<url_value>>)> func_)
	{
		output_t result = nullptr;
		auto real_url_ = to_be_url(url_);
		if (!(real_url_ == QUrl() && url_ != web::Browser::_defaulthome // || real_url_ == detail::to_qstring(web::Browser::_defaulthome)
			)) {
			//			critical_error("The url is not valid");// throw std::runtime_error("url is not valid");
			result = func_(new ::real_url_t<url_value>(url_));
		}
		return result;
	}

	template boost::intrusive_ptr<i_t> //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const url_value& url_, std::function<boost::intrusive_ptr<i_t>(boost::intrusive_ptr<real_url_t<url_value>>)>);

	template web::Browser* //boost::intrusive_ptr<::real_url_t<url_value>>
	    real_url_instance(const url_value& url_, std::function<web::Browser*(boost::intrusive_ptr<real_url_t<url_value>>)>);

	//	template boost::intrusive_ptr<::real_url_t<QString>> real_url_instance(const QString& url_);
	//	template boost::intrusive_ptr<::real_url_t<url_value>> real_url_instance(const url_value& url_);
}

//template<>
//url_value real_url_t<url_value>::value() const
//{
//	return _url;
//}

//template<>
//real_url_t<url_value>::real_url_t(const url_value& url_)
//    : _url(url_)
//{
//}
