#ifndef __RECORD_H__
#define __RECORD_H__

#include <QByteArray>
#include <QMap>
#include <QString>
#include <boost/assert.hpp>
#include <boost/mpl/map.hpp>
#include <memory>

#include <QDomElement>
// #include <QDomDocument>

#include "utility/delegate.h"

// #include "views/browser/browser.h"

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include "libraries/fixed_parameters.h"
#include "main.h"
// Класс одной записи в таблице записей
// Class entries in the table of records

// В полях записей с crypt=0 хранятся незашифрованные данные
// В полях записей с crypt=1 хранятся зашифрованные данные
// Это необходимо, чтобы дерево знаний генерировалось в/из XML быстро и без
// шифрации

// In the fields of records crypt = 0 are stored unencrypted data
// In the fields of records crypt = 1 are stored encrypted data
// It is necessary that the tree of knowledge was generated in / from XML
// quickly and without encryption

// struct id_type;
// struct pin_type;
// struct rating_type;
// struct name_type;
// struct author_type;
// struct home_type;
// struct url_type;
// struct tags_type;
// struct ctime_type;
// struct dir_type;
// struct file_type;
// struct crypt_type;
// struct has_attach_type;
// struct attach_count_type;

// typedef boost::mpl::map<
// boost::mpl::pair<id_type, QString>
// , boost::mpl::pair<pin_type, QString>
// , boost::mpl::pair<rating_type, QString>
// , boost::mpl::pair<name_type, QString>
// , boost::mpl::pair<author_type, QString>
// , boost::mpl::pair<home_type, QString>
// , boost::mpl::pair<url_type, QString>
// , boost::mpl::pair<tags_type, QString>
// , boost::mpl::pair<ctime_type, QString>
// , boost::mpl::pair<dir_type, QString>
// , boost::mpl::pair<file_type, QString>
// , boost::mpl::pair<crypt_type, QString>
// > natural_field_map;

// typedef boost::mpl::map<
// boost::mpl::pair<has_attach_type, QString>
// , boost::mpl::pair<attach_count_type, QString>
// > calculable_field_map;

// class Attach;
// class AttachTableData;

namespace web {
	class Browser;
	class WebView;
	class WebPage;
}

class i_t;
class AttachTableData;
class QDomElement;
class QDomDocument;
class StarRating;
struct id_value;

struct clear_full_fields_map {

	full_fields_map* const _fields_data_map;

	clear_full_fields_map(full_fields_map* const misc_fields_)
	    : _fields_data_map(misc_fields_)
	{
	}

	template <typename key_t, typename value_t>
	void operator()(const boost::fusion::pair<key_t, value_t>&) const
	{
		//		(void)pair;
		//		pair.second = value_t();  //
		boost::fusion::at_key<key_t>(*_fields_data_map) = value_t();
	}
};

class Record
    : public boost::intrusive_ref_counter<Record, boost::thread_safe_counter>
// std::enable_shared_from_this<Record>
{
	// К закрытым функциям может иметь доступ объекты приаттаченного файла
	friend class Attach;
	friend class AttachTableData;

    public:
	//	Record();
	Record(full_fields_map field_data_static_map_ = full_fields_map());

#ifdef _with_record_table
	Record(boost::intrusive_ptr<Record> obj);
	Record& operator=(const Record& obj);
#endif

	virtual ~Record();

	// web::WebPage *unique_page();   // const; // {return _page;}

	void dom_to_record(const QDomElement& _dom_element);
	// QDomElement export_to_dom(QDomDocument *doc) const;
	QDomElement dom_from_record() const;
	// QDomElement dom_from_record(std::shared_ptr<QDomDocument> doc) const;

	QString text_from_fat() const;
	QString text_from_lite_direct() const;
	void text_to_fat(QString _text_source);

    public:
	template <typename field_key_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type field() const; // , typename field_type_switch = typename
	// switch_type<field_type>::type
	//	{
	//		return field_read<field_type, typename switch_type<field_type>::type>(this)(); // natural_field(boost::mpl::c_str <typename boost::mpl::at < natural_field_type, field_type >::type >::value);
	//	}

	template <typename field_key_type>
	void field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type& value); // , typename field_type_switch = typename
																				 // switch_type<field_type>::type
	//	{
	//		return field_write<field_type, typename switch_type<field_type>::type>(this)(value);
	//	}
	//
	//	bool is_natural_field_exists(QString name) const;
	// QString crypt_field(const QString &name) const;
	// Get all of the data fields// Получение всех полей данных
	QMap<QString, QString> dump_decode() const;

	// Get all the data fields directly, without changes// Получение всех полей данных напрямую, без преобразований
	QMap<QString, QString> dump_direct() const;

	QMap<QString, QString> natural_field_list() const;

	QMap<QString, QByteArray> picture_files() const;
	void picture_files(QMap<QString, QByteArray> picture_files);

	std::shared_ptr<AttachTableData> attach_table() const;
	std::shared_ptr<AttachTableData> attach_table();
	void attach_table(std::shared_ptr<AttachTableData> _attach_table_data);

	bool is_crypt() const;
	bool is_empty() const;
	bool is_lite() const;
	void to_lite();
	void to_fat();

	// Шифрация
	void to_encrypt_and_save_lite(void);
	void to_encrypt_and_save_fat(void);

	// асшифровка
	void to_decrypt_and_save_lite(void);
	void to_decrypt_and_save_fat(void);

	void push_lite_attributes();
	void push_fat_attributes();
	// bool is_holder();
	// void active_request(int pos, int openLinkIn);
	// Record *active_immediately(bool ai) {_active_immediately = ai; return
	// this;}
	// bool active_immediately() {return _active_immediately;}

	// bool is_registered_to_record_controller_and_tabmanager() {return
	// _is_registered_to_shadow_list;}
	// void is_registered_to_record_controller_and_tabmanager(bool reg)
	// {_is_registered_to_shadow_list = reg;}

	// void
	// binder(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> g) {_binder = g;}
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> binder() const {return
	// _binder;}

	// void
	// activator(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> a) {_activator = a;}
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> activator() const {return
	// _activator;}

	// web::WebView *bind();
	// web::WebView *active();
	bool dir_exists();
	bool file_exists();
	id_value id() const;
	name_value name() const;
	rating_value rating() const;
	size_t add_rating();
#ifdef USE_STAR_RATING
	StarRating* star_rating()
	{
		return _star_rating;
	}
#endif
	void clear_content();
	// Возвращение количества полей, которые хранит данный элемент.
	size_t fields_likely_count() const;

    protected:
	// web::WebPage            *_page;
	// ---------------------------------------------------------------------
	// Свойства класса (не забыть перечислить все в конструкторе копривания)
	// Class properties (do not forget to list all the constructor koprivaniya)
	// ---------------------------------------------------------------------

	bool _lite_flag;
	// bool                        _is_registered_to_shadow_list = false;
	// bool                        _active_request = false;
	// int                         _position = -1;
	// int                         _open_link_in_new_window = 0;
	// bool                        _active_immediately = false;

	// Установка содержимого свойств происходит в вышестоящем коде
	// Set the properties of the contents occurs in the upstream code
	// Light properties // Легкие свойства
	full_fields_map _fields_data_map; //	QMap<QString, QString> _field_data;  //// A list of the properties of records
	/// The list of recording properties (attributes)///Перечень свойств записи (атрибутов)
	///(attributes) ImyaAtributa - Meaning /////ИмяАтрибута - Значение

	QByteArray _text;                         // Содержимое файла с текстом записи// Полновесные свойства
	QMap<QString, QByteArray> _picture_files; // Содержимое картинок, используемых
// в тексте записи (используется при
// переносе через буфер обмена, при
// DragAndDrop)
#ifdef USE_STAR_RATING
	StarRating* _star_rating;
#endif
	std::shared_ptr<AttachTableData> _attach_table_data; // Таблица прикрепляемых файлов

	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> _binder;
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// web::WebView *, boost::intrusive_ptr<Record>>> _activator;

	// -----------------
	// Защищенные методы
	// -----------------

	void text_to_direct(QString _text_source);
	void create_file_and_save_text();

	QString id_and_name() const; // Внутренний метод для облегчения печати отладочной информации

	QString full_dir() const;
	QString short_dir() const;

	QString full_text_file_name() const;
	QString full_file_name(QString file_name) const;

	void to_encrypt_fields(void);
	void to_decrypt_fields(void);

	void check_and_fill_file_dir(QString& nameDirFull, QString& i_file_name);
	void check_and_create_text_file();

// QString natural_field(QString name) const;
// QString calculable_field(QString name) const;

#ifdef _with_record_table
	explicit Record(const Record& obj); // = delete;
#endif

    private:
	std::function<QDomElement(std::shared_ptr<QDomDocument> doc)> dom_from_record_impl;

	template <typename field_key_type>
	const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map, typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::type>::type read_crypt_field() const;

	//    std::function<void(const QString& name, const QString& value)>
	template <typename field_key_type>
	void write_crypt_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<crypt_fields_map const, typename boost::mpl::at<crypt_fields_keys, field_key_type>::type>::type>::type& value);

	std::function<QString()> has_attach_field;
	std::function<QString()> attach_count_field;

	friend struct Linker;
	friend class ItemsFlat;
	friend class i_t;
	friend class FindScreen;
	friend class RecordModel;
	friend class ts_t;

	friend struct element_fullfill;
	friend struct full_fill_natural_list;

    private:
	// Setting and reading data without transformation. Used to generate / XML
	// readers    // Установка и чтение данных без преобразований. Используется
	// при генерации/чтении XML
	template <typename field_key_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map, field_key_type>::type>::type natural_field_source() const;
	//	{//QString name
	//		// Если имя поля недопустимо
	//#ifdef TEST
	//		BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set,
	//concrete>::type::value), ==, true);//
	//critical_error("Record::natural_field_source() : get unavailable field " +
	//name);
	//		BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set,
	//concrete>));
	//#endif //TEST
	//		BOOST_MPL_ASSERT((boost::mpl::contains<natural_field_set,
	//concrete>));
	//		// Если поле с таким названием есть
	//		if(_field_data.contains(boost::mpl::c_str<concrete>::value)) return
	//_field_data[boost::mpl::c_str < concrete > ::value]; // Возвращается
	//значение поля
	//		else return QString();
	//	}
	template <typename field_key_type>
	void natural_field_source(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map const, field_key_type>::type>::type& value);
	//	{//QString name,, concrete cc
	////		(void) cc;
	//		// Если имя поля недопустимо
	//#ifdef TEST
	//		BOOST_MPL_ASSERT_RELATION((boost::mpl::contains<natural_field_set,
	//concrete>::type::value), ==,
	//true);//if(fixedparameters.is_record_field_natural(name) == false)
	//critical_error("In Record::natural_field_source() unavailable field name " +
	//name + " try set to " + value);
	//		BOOST_MPL_ASSERT((boost::mpl::has_key<natural_field_set,
	//concrete>));
	//#endif //TEST
	//		BOOST_MPL_ASSERT((boost::mpl::contains<natural_field_set,
	//concrete>));
	//		// Устанавливается значение поля
	//		_field_data.insert(boost::mpl::c_str<concrete>::value, value);
	//	}

	// helper struct
	template <typename field_key_type, typename field_type_switch>
	struct field_read;

	//	template <typename field_type>
	//	struct field_read<field_type, append_to_crypt_set> {
	//		const Record *_this;
	//		field_read(const Record *_this)
	//			: _this(_this)
	//		{}

	//		QString operator ()() const {return
	//_this->_field_data[boost::mpl::c_str < typename boost::mpl::at <
	//append_to_crypt_set, field_type > ::type > ::value];}
	//	};

	//	template <typename field_type>
	//	struct field_read<field_type, crypt_field_set> {
	//		const Record *_this;
	//		field_read(const Record *_this)
	//			: _this(_this)
	//		{}

	//		QString operator ()(){return
	//_this->read_crypt_field(boost::mpl::c_str<typename
	//boost::mpl::at<crypt_field_set, field_type>::type>::value);} // {return
	//_this->_field_data[boost::mpl::c_str < typename boost::mpl::at <
	//natural_field_type, field_type_current > ::type > ::value] = value; }
	//	};

	//	template <typename field_type>
	//	struct field_read<field_type, calculable_field_set> {
	//		const Record *_this;
	//		field_read(const Record *_this)
	//			: _this(_this)
	//		{}

	//		QString operator ()();
	//	};

	template <typename field_key_type, typename field_type_switch>
	struct field_write;
};

//#include "models/record_table/record.inl"

#ifndef EXTERN_NATURAL_FIELD_GET_DECLARATION

#define EXTERN_NATURAL_FIELD_GET_DECLARATION(key_type) \
	extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map, key_type>::type>::type Record::natural_field_source<key_type>() const;

#endif // EXTERN_NATURAL_FIELD_GET_DECLARATION

EXTERN_NATURAL_FIELD_GET_DECLARATION(id_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(pin_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(rating_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(name_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(author_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(home_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(url_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(tags_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(ctime_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(dir_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(file_key)
EXTERN_NATURAL_FIELD_GET_DECLARATION(crypt_key)

// extern template decltype(auto) Record::natural_field_source<id_key>() const;
// extern template decltype(auto) Record::natural_field_source<pin_key>() const;
// extern template decltype(auto) Record::natural_field_source<rating_key>() const;
// extern template decltype(auto) Record::natural_field_source<name_key>() const;
// extern template decltype(auto) Record::natural_field_source<author_key>() const;
// extern template decltype(auto) Record::natural_field_source<home_key>() const;
// extern template decltype(auto) Record::natural_field_source<url_key>() const;
// extern template decltype(auto) Record::natural_field_source<tags_key>() const;
// extern template decltype(auto) Record::natural_field_source<ctime_key>() const;
// extern template decltype(auto) Record::natural_field_source<dir_key>() const;
// extern template decltype(auto) Record::natural_field_source<file_key>() const;
// extern template decltype(auto) Record::natural_field_source<crypt_key>() const;
////extern template decltype(auto) Record::natural_field_source<has_attach_key>() const;
////extern template decltype(auto) Record::natural_field_source<attach_size_key>() const;
////extern template decltype(auto) Record::natural_field_source<dynamic_name_key>() const;

#ifndef EXTERN_NATURAL_FIELD_SET_DECLARATION

#define EXTERN_NATURAL_FIELD_SET_DECLARATION(key_type) \
	extern template void Record::natural_field_source<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<natural_fields_map const, key_type>::type>::type& value);

#endif // EXTERN_NATURAL_FIELD_SET_DECLARATION

EXTERN_NATURAL_FIELD_SET_DECLARATION(id_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(pin_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(rating_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(name_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(author_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(home_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(url_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(tags_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(ctime_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(dir_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(file_key)
EXTERN_NATURAL_FIELD_SET_DECLARATION(crypt_key)

// extern template void Record::natural_field_source<id_key>(const typename boost::fusion::result_of::at_key<natural_field_map, id_key>::type& value);
// extern template void Record::natural_field_source<pin_key>(const typename boost::fusion::result_of::at_key<natural_field_set, pin_key>::type& value);
// extern template void Record::natural_field_source<rating_key>(const typename boost::fusion::result_of::at_key<natural_field_set, rating_key>::type& value);
// extern template void Record::natural_field_source<name_key>(const typename boost::fusion::result_of::at_key<natural_field_set, name_key>::type& value);
// extern template void Record::natural_field_source<author_key>(const typename boost::fusion::result_of::at_key<natural_field_set, author_key>::type& value);
// extern template void Record::natural_field_source<home_key>(const typename boost::fusion::result_of::at_key<natural_field_set, home_key>::type& value);
// extern template void Record::natural_field_source<url_key>(const typename boost::fusion::result_of::at_key<natural_field_set, url_key>::type& value);
// extern template void Record::natural_field_source<tags_key>(const typename boost::fusion::result_of::at_key<natural_field_set, tags_key>::type& value);
// extern template void Record::natural_field_source<ctime_key>(const typename boost::fusion::result_of::at_key<natural_field_set, ctime_key>::type& value);
// extern template void Record::natural_field_source<dir_key>(const typename boost::fusion::result_of::at_key<natural_field_set, dir_key>::type& value);
// extern template void Record::natural_field_source<file_key>(const typename boost::fusion::result_of::at_key<natural_field_set, file_key>::type& value);
// extern template void Record::natural_field_source<crypt_key>(const typename boost::fusion::result_of::at_key<natural_field_set, crypt_key>::type& value);
////extern template void Record::natural_field_source<has_attach_key>(const typename boost::fusion::result_of::at_key<natural_field_set, has_attach_key>::type& value);
////extern template void Record::natural_field_source<attach_size_key>(const typename boost::fusion::result_of::at_key<natural_field_set, attach_size_key>::type& value);
////extern template void Record::natural_field_source<dynamic_name_key>(const typename boost::fusion::result_of::at_key<natural_field_set, field_key>::type &value);

#ifndef EXTERN_RECORD_GET_DECLARATION
#define EXTERN_RECORD_GET_DECLARATION(key_type) \
	extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Record::field<key_type>() const;
#endif // EXTERN_RECORD_GET_DECLARATION

EXTERN_RECORD_GET_DECLARATION(id_key)
EXTERN_RECORD_GET_DECLARATION(pin_key)
EXTERN_RECORD_GET_DECLARATION(rating_key)
EXTERN_RECORD_GET_DECLARATION(name_key)
EXTERN_RECORD_GET_DECLARATION(author_key)
EXTERN_RECORD_GET_DECLARATION(home_key)
EXTERN_RECORD_GET_DECLARATION(url_key)
EXTERN_RECORD_GET_DECLARATION(tags_key)
EXTERN_RECORD_GET_DECLARATION(ctime_key)
EXTERN_RECORD_GET_DECLARATION(dir_key)
EXTERN_RECORD_GET_DECLARATION(file_key)
EXTERN_RECORD_GET_DECLARATION(crypt_key)
EXTERN_RECORD_GET_DECLARATION(has_attach_key)
EXTERN_RECORD_GET_DECLARATION(attach_size_key)

// extern template QString Record::field<id_key>() const;
// extern template QString Record::field<pin_key>() const;
// extern template QString Record::field<rating_key>() const;
// extern template QString Record::field<name_key>() const;
// extern template QString Record::field<author_key>() const;
// extern template QString Record::field<home_key>() const;
// extern template QString Record::field<url_key>() const;
// extern template QString Record::field<tags_key>() const;
// extern template QString Record::field<ctime_key>() const;
// extern template QString Record::field<dir_key>() const;
// extern template QString Record::field<file_key>() const;
// extern template QString Record::field<crypt_key>() const;
// extern template QString Record::field<has_attach_key>() const;
// extern template QString Record::field<attach_size_key>() const;
////extern template QString Record::field<dynamic_name_key>() const;

#ifndef EXTERN_RECORD_SET_DECLARATION
#define EXTERN_RECORD_SET_DECLARATION(key_type) \
	extern template void Record::field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type& value);
#endif // EXTERN_RECORD_SET_DECLARATION

EXTERN_RECORD_SET_DECLARATION(id_key)
EXTERN_RECORD_SET_DECLARATION(pin_key)
EXTERN_RECORD_SET_DECLARATION(rating_key)
EXTERN_RECORD_SET_DECLARATION(name_key)
EXTERN_RECORD_SET_DECLARATION(author_key)
EXTERN_RECORD_SET_DECLARATION(home_key)
EXTERN_RECORD_SET_DECLARATION(url_key)
EXTERN_RECORD_SET_DECLARATION(tags_key)
EXTERN_RECORD_SET_DECLARATION(ctime_key)
EXTERN_RECORD_SET_DECLARATION(dir_key)
EXTERN_RECORD_SET_DECLARATION(file_key)
EXTERN_RECORD_SET_DECLARATION(crypt_key)
EXTERN_RECORD_SET_DECLARATION(has_attach_key)
EXTERN_RECORD_SET_DECLARATION(attach_size_key)

// extern template void Record::field<id_key>(const QString& value);
// extern template void Record::field<pin_key>(const QString& value);
// extern template void Record::field<rating_key>(const QString& value);
// extern template void Record::field<name_key>(const QString& value);
// extern template void Record::field<author_key>(const QString& value);
// extern template void Record::field<home_key>(const QString& value);
// extern template void Record::field<url_key>(const QString& value);
// extern template void Record::field<tags_key>(const QString& value);
// extern template void Record::field<ctime_key>(const QString& value);
// extern template void Record::field<dir_key>(const QString& value);
// extern template void Record::field<file_key>(const QString& value);
// extern template void Record::field<crypt_key>(const QString& value);
// extern template void Record::field<has_attach_key>(const QString& value);
// extern template void Record::field<attach_size_key>(const QString& value);
////extern template void Record::field<dynamic_name_key>(const QString &value);

#endif // __RECORD_H__
