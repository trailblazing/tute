#ifndef __RECORD_H__
#define __RECORD_H__

#include <memory>
#include <boost/mpl/map.hpp>
#include <QString>
#include <QMap>
#include <QByteArray>
// #include <QDomElement>
// #include <QDomDocument>

#include "utility/delegate.h"

// #include "views/browser/browser.h"

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "libraries/fixed_parameters.h"

// Класс одной записи в таблице записей
// Class entries in the table of records

// В полях записей с crypt=0 хранятся незашифрованные данные
// В полях записей с crypt=1 хранятся зашифрованные данные
// Это необходимо, чтобы дерево знаний генерировалось в/из XML быстро и без шифрации

// In the fields of records crypt = 0 are stored unencrypted data
// In the fields of records crypt = 1 are stored encrypted data
// It is necessary that the tree of knowledge was generated in / from XML quickly and without encryption



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

namespace browser {
    class Browser;
    class WebView;
    class WebPage;
}

class TreeItem;
class AttachTableData;
class QDomElement;
class QDomDocument;
class StarRating;

class Record
    : public boost::intrusive_ref_counter<Record, boost::thread_safe_counter>
// std::enable_shared_from_this<Record>
{
	// К закрытым функциям может иметь доступ объекты приаттаченного файла
    friend class Attach;
    friend class AttachTableData;

    public:
	Record();
	Record(QMap<QString, QString> _field_data);

#ifdef _with_record_table
	Record(boost::intrusive_ptr<Record> obj);
	Record &operator =(const Record &obj);
#endif

	virtual ~Record();

	// browser::WebPage *unique_page();   // const; // {return _page;}

	void dom_to_record(const QDomElement &_dom_element);
	// QDomElement export_to_dom(QDomDocument *doc) const;
	QDomElement dom_from_record() const;
	// QDomElement dom_from_record(std::shared_ptr<QDomDocument> doc) const;


	QString text_from_fat() const;
	QString text_from_lite_direct() const;
	void	text_to_fat(QString _text_source);

	template < typename field_type, typename field_type_switch>struct field_read;

	template < typename field_type>struct field_read<field_type, append_to_crypt_type> {
	    const Record *_this;
	    field_read(const Record *_this) : _this(_this){}

	    QString operator ()() const {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < append_to_crypt_type, field_type > ::type > ::value];}
	};

	template <typename field_type>struct field_read<field_type, crypt_field_type> {
	    const Record *_this;
	    field_read(const Record *_this) : _this(_this){}

	    QString operator ()(){return _this->read_crypt_field(boost::mpl::c_str < typename boost::mpl::at < crypt_field_type, field_type >::type >::value);}	// {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
	};

	template <typename field_type>struct field_read<field_type, calculable_field_type>{
	    const Record *_this;
	    field_read(const Record *_this) : _this(_this){}

	    QString operator ()();
	};


	template < typename field_type, typename field_type_switch>struct field_write;

	template < typename field_type>struct field_write<field_type, append_to_crypt_type> {
	    Record *const _this;
	    field_write(Record *const _this) : _this(_this){}

	    void operator ()(const QString &value){_this->_field_data[boost::mpl::c_str < typename boost::mpl::at < append_to_crypt_type, field_type > ::type > ::value] = value;}
	};

	template <typename field_type>struct field_write<field_type, crypt_field_type> {
	    Record *const _this;
	    field_write(Record *const _this) : _this(_this){}

	    void operator ()(const QString &value){return _this->write_crypt_field(boost::mpl::c_str < typename boost::mpl::at < crypt_field_type, field_type >::type >::value, value);}// {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
	};

	template <typename field_type>struct field_write<field_type, calculable_field_type> {
	    Record *const _this;
	    field_write(Record *const _this) : _this(_this){}

	    void operator ()(const QString &value);	// {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
	};

	template < typename field_type>	// , typename field_type_switch = typename switch_type<field_type>::type
	QString field() const {
	    return field_read<field_type, typename switch_type<field_type>::type>(this)();	// natural_field(boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type >::type >::value);
	}
	template < typename field_type>	// , typename field_type_switch = typename switch_type<field_type>::type
	void field(const QString &value){
	    return field_write<field_type, typename switch_type<field_type>::type>(this)(value);
	}
	bool is_natural_field_exists(QString name) const;
// QString crypt_field(const QString &name) const;

	// Setting and reading data without transformation. Used to generate / XML readers    // Установка и чтение данных без преобразований. Используется при генерации/чтении XML
	QString natural_field_source(QString name) const;
	void	natural_field_source(QString name, QString value);

	QMap<QString, QString> natural_field_list() const;

	QMap<QString, QByteArray>	picture_files() const;
	void				picture_files(QMap<QString, QByteArray> picture_files);

	std::shared_ptr<AttachTableData>	attach_table() const;
	std::shared_ptr<AttachTableData>	attach_table();
	void					attach_table(std::shared_ptr<AttachTableData> _attach_table_data);

	bool	is_crypt() const;
	bool	is_empty() const;
	bool	is_lite() const;
	void	to_lite();
	void	to_fat();

	// Шифрация
	void	to_encrypt_and_save_lite(void);
	void	to_encrypt_and_save_fat(void);

	// асшифровка
	void	to_decrypt_and_save_lite(void);
	void	to_decrypt_and_save_fat(void);

	void	push_lite_attributes();
	void	push_fat_attributes();
	// bool is_holder();
	// void active_request(int pos, int openLinkIn);
	// Record *active_immediately(bool ai) {_active_immediately = ai; return this;}
	// bool active_immediately() {return _active_immediately;}

	// bool is_registered_to_record_controller_and_tabmanager() {return _is_registered_to_shadow_list;}
	// void is_registered_to_record_controller_and_tabmanager(bool reg) {_is_registered_to_shadow_list = reg;}

	// void binder(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> g) {_binder = g;}
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> binder() const {return _binder;}

	// void activator(std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> a) {_activator = a;}
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> activator() const {return _activator;}

	// browser::WebView *bind();
	// browser::WebView *active();
	bool	dir_exists();
	bool	file_exists();
#ifdef USE_STAR_RATING
	StarRating	*star_rating(){return _star_rating;}
#endif
    protected:

	// browser::WebPage            *_page;
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
// natural_field_map _field_data_static;
	// Light properties // Легкие свойства
	QMap<QString, QString>			_field_data;		//// A list of the properties of records (attributes) ImyaAtributa - Meaning // Перечень свойств записи (атрибутов) ИмяАтрибута - Значение
	QByteArray				_text;		// Содержимое файла с текстом записи// Полновесные свойства
	QMap<QString, QByteArray>		_picture_files;	// Содержимое картинок, используемых в тексте записи (используется при переносе через буфер обмена, при DragAndDrop)
#ifdef USE_STAR_RATING
	StarRating				*_star_rating;
#endif
	std::shared_ptr<AttachTableData>	_attach_table_data;	// Таблица прикрепляемых файлов

	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> _binder;
	// std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> _activator;

	// -----------------
	// Защищенные методы
	// -----------------

	void	text_to_direct(QString _text_source);
	void	create_file_and_save_text();

	QString id_and_name() const;	// Внутренний метод для облегчения печати отладочной информации

	QString full_dir() const;
	QString short_dir() const;

	QString full_text_file_name() const;
	QString full_file_name(QString fileName) const;

	void	to_encrypt_fields(void);
	void	to_decrypt_fields(void);

	void	check_and_fill_file_dir(QString &nameDirFull, QString &i_file_name);
	void	check_and_create_text_file();

// QString natural_field(QString name) const;
// QString calculable_field(QString name) const;

#ifdef _with_record_table
	explicit Record(const Record &obj);	// = delete;
#endif

    private:

	std::function<QDomElement(std::shared_ptr<QDomDocument> doc) >	dom_from_record_impl;
	std::function<QString(const QString &name)>			read_crypt_field;
	std::function<void (const QString &name, const QString &value)> write_crypt_field;
	std::function<QString()>					has_attach_field;
	std::function<QString()>					attach_count_field;

	friend struct Linker;
	friend class ItemsFlat;
	friend class TreeItem;
	friend class FindScreen;
	friend class RecordModel;
	friend class ts_t;
};


// template<> QString Record::field<has_attach_type>() const;
// template<> QString Record::field<attach_count_type>() const;

template <>struct Record::field_read<has_attach_type, calculable_field_type> {
    const Record *_this;
    field_read(const Record *_this) : _this(_this){}

    QString operator ()(){return _this->has_attach_field();}	// {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
};


template <>struct Record::field_read<attach_count_type, calculable_field_type> {
    const Record *_this;
    field_read(const Record *_this) : _this(_this){}

    QString operator ()(){return _this->attach_count_field();}	// {return _this->_field_data[boost::mpl::c_str < typename boost::mpl::at < natural_field_type, field_type_current > ::type > ::value] = value; }
};

// typedef boost::mpl::set<pin_type, name_type, author_type, home_type, url_type, tags_type> crypt_field_type;
// template<typename field_type_current> void Record::field<field_type_current, crypt_field_type>(QString value);
// template<> void field<name_type>(QString value);
// template<> void field<author_type>(QString value);
// template<> void field<home_type>(QString value);
// template<> void field<url_type>(QString value);
// template<> void field<tags_type>(QString value);

#endif	// __RECORD_H__
