#include <tuple>
#include <utility>

#include <map>

#include <boost/fusion/iterator.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>

//#include "utility/variant.h"
#include "utility/for_each.h"

#include <QDomElement>
#include <QMessageBox>
#include <QObject>
// #include <QDomDocument>

#include "libraries/crypt/crypt_service.h"
#include "libraries/disk_helper.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "models/attach_table/attach.h"
#include "models/attach_table/attach_table_data.h"
#include "models/attach_table/attach_table_data.h"
#include "models/record_table/record_index.hxx"
#include "record.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/record_table/record_view.h"

extern std::shared_ptr<AppConfig> appconfig;
extern FixedParameters fixedparameters;
extern std::shared_ptr<gl_para> gl_paras;

#ifndef USE_CXX_14

namespace detail {
	template <typename i_type>
	struct check_id {
		check_id(full_fields_map&)
		{
		}
	};

	template <>
	struct check_id<id_key> {
		check_id(full_fields_map& full_field_map_)
		{
			auto dir = QString(boost::fusion::at_key<dir_key>(full_field_map_));
			if (id_value("") == boost::fusion::at_key<id_key>(full_field_map_))
				boost::fusion::at_key<id_key>(full_field_map_) = dir.length() > 0 ? dir : get_unical_id();
		}
	};
}

#endif // USE_CXX_14

//	tempalte<typename InputRange>
struct element_fullfill {
	QDomElement& elem;
	Record* const _this;

	element_fullfill(QDomElement& elem_, Record* const this_)
	    : elem(elem_)
	    , _this(this_)
	{
	}

	template <typename key_type>
	int operator()(key_type index_type_value) const
	{
		(void)index_type_value;
//			typedef boost::mpl::at<InputRange, indices>::type
//index_type;
//		const QString &field_name =
//boost::mpl::c_str<index_type>::value;//index_type is mpl_::void_?//
//boost::mpl::c_str<typename boost::mpl::at<natural_field_set,
//index_type>::type>::value;

//		auto internal_field = [&](QString
//name){if(_this->_field_data.contains(name)) return _this->_field_data[name];
//else return QString(); };
//		auto internal_field_write = [&](QString key, QString
//value){_this->_field_data.insert(key, value);};// =
//available_field_list.at(j);
#ifndef USE_CXX_14
		typedef typename sd::static_if<sd::equal<key_type, id_key>::value, detail::check_id<id_key>, detail::check_id<key_type>>::type check;
		check c(_this->_fields_data_map);
#endif // USE_CXX_14
		//		std::enable_if<sd::STATIC_SAME<index_type, id_type>::value,
		//check_id>::type();
		detail::static_if<sd::equal<key_type, id_key>::value>([&]() {
			auto dir = QString(boost::fusion::at_key<dir_key>(_this->_fields_data_map));
			if (id_value("") == boost::fusion::at_key<id_key>(_this->_fields_data_map))
				boost::fusion::at_key<id_key>(_this->_fields_data_map) = dir.length() > 0 ? dir : get_unical_id();
		})();

		// Устанавливается значение поля как атрибут DOM-узла
		//		if(_this->_field_data.contains(field_name))
		elem.setAttribute(boost::mpl::c_str<key_type>::value, detail::to_qstring(boost::fusion::at_key<key_type>(_this->_fields_data_map))); // internal_field(field_name)
		return 0;
	}
};

// Record::Record() // boost::intrusive_ref_counter<Record,
// boost::thread_safe_counter>()  // std::enable_shared_from_this<Record>()
//	: _lite_flag(true)
//	  , _text("")
//#ifdef USE_STAR_RATING
//	  , _star_rating(new StarRating(1, 5))
//#endif
//	  ,
//_attach_table_data(std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record
//*>(this)))) // {
//	  // liteFlag = true;    // By default, the object light // По-умолчанию
//объект легкий
//	  , dom_from_record_impl(
//		  [&](std::shared_ptr<QDomDocument> doc) -> QDomElement {
//			  QDomElement elem = doc->createElement("record");

////			  auto dst =
///FixedParameters::_record_natural_field_static;
//			  // Перебираются допустимые имена полей, доступных для
//сохранения
////			  QStringList available_field_list;//=
///FixedParameters::_record_natural_field_static;
////			  natural_field_type::item_ iti;
////			  boost::mpl::begin<natural_field_type> te;
//			  // int available_field_list_size =
//available_field_list.size();
////			  void (Record::*func)(QString, QString) =
///&Record::natural_field_source;
//			  Apply<boost::mpl::size<natural_field_set>::type::value
//				  , element_fullfill//std::bind(func,
//this)//<boost::mpl::at<natural_field_type, boost::mpl::_1> >
//				  >(element_fullfill(elem,
//this))(FixedParameters::_record_natural_field_tuple);//_record_natural_field_static
////					 for(int j = 0; j < available_field_list.size();
///++j){
////					         QString field_name =
///available_field_list.at(j);
////					         if(field_name ==
///boost::mpl::c_str<id_type>::value && natural_field_source(field_name) == "")
///natural_field_source(field_name,
///natural_field_source(boost::mpl::c_str<dir_type>::value).length() > 0 ?
///natural_field_source(boost::mpl::c_str<dir_type>::value) : get_unical_id());
////					         // Устанавливается значение поля как
///атрибут DOM-узла
////					         if(is_natural_field_exists(field_name))
///elem.setAttribute(field_name, natural_field_source(field_name));
////					 }
//			  // К элементу записи прикрепляется элемент таблицы
//приаттаченных файлов, если таковые есть
//			  if(_attach_table_data->size() > 0)
//elem.appendChild(_attach_table_data->dom_from_data(doc));
//			  return elem;
//		  })
//	  , read_crypt_field(
//		  [&](const QString &name){
//			  // QString name = boost::mpl::c_str < typename
//boost::mpl::at < append_to_crypt_type, field_type >::type >::value;
//			  QString result = "";

//			  auto crypt_field_name =
//boost::mpl::c_str<crypt_type>::value;
//			  if(name == "home" && _field_data[name] == "" &&
//_field_data[boost::mpl::c_str < url_type > ::value] != "") _field_data[name] =
//_field_data[boost::mpl::c_str < url_type > ::value]; // for history reason
//			  if(_field_data.contains(crypt_field_name)){  //
//boost::mpl::c_str < crypt_type > ::value
//				  if(_field_data[crypt_field_name] == "1"){
//					  if(_field_data[name].length() > 0 &&
//gl_paras->crypt_key().length() > 0){
//						  // is_crypt = true;
//						  // Поле расшифровывается
//						  result =
//CryptService::decryptString(gl_paras->crypt_key(), _field_data[name]);
//					  }
//				  }else result = _field_data[name];
//			  }else result = _field_data[name];
//			  return result;
//		  })
//	  , write_crypt_field(
//		  [&](const QString &_name, const QString &value) -> void { //
//template <typename field_type>void Record::field_write<field_type,
//crypt_field_type>::operator()(const QString &value){// template<typename
//field_type> void Record::field(const QString &value){
//			  // QString _name = boost::mpl::c_str < typename
//boost::mpl::at < crypt_field_type, field_type >::type >::value;
//			  //// Если имя поля недопустимо (установить значение можно
//только для натурального поля)
//			  // if(fixedparameters.is_record_field_natural(_name) ==
//false)critical_error("In Record::field() unavailable field name " + _name + "
//try set to " + value);
//			  bool is_crypt = false;
//			  // Если имя поля принадлежит списку полей, которые могут
//шифроваться
//			  // и в наборе полей есть поле crypt
//			  // и поле crypt установлено в 1
//			  // и поле не пустое (пустые данные ненужно шифровать)
//			  //
//if(fixedparameters._record_field_crypted.contains(_name)){
//			  if(_field_data.contains(boost::mpl::c_str<crypt_type>::value)){
//				  if(_field_data[boost::mpl::c_str < crypt_type >
//::value] == "1"){
//					  if(value.length() > 0){
//						  if(gl_paras->crypt_key().length() > 0)
//is_crypt = true;
//						  else critical_error("In Record::field()
//can not set data to crypt field " + _name + ". Password not setted");
//					  }
//				  }
//			  }
//			  // }
//			  QString _value = value;
//			  // Если нужно шифровать, поле шифруется
//			  if(is_crypt == true) _value =
//CryptService::encryptString(gl_paras->crypt_key(), value);
//			  // Устанавливается значение поля

//			  _field_data.insert(_name, _value); // _field_data[_name] =
//value; //

//			  // qDebug() << "Record::set_field : pos" << pos << "name"
//<< name << "value" << value;
//		  })
//	  , has_attach_field([&] {return this->_attach_table_data->size() > 0 ?
//"1" : "0";})
//	  , attach_count_field([&] {return
//QString::number(this->_attach_table_data->size());})
//{}

struct record_to_dom {
	QDomElement* const _elem;
	//	full_fields_map* const _fields_data_map;
	record_to_dom(QDomElement* const elem_) //, full_fields_map* const fields_data_map_
	    : _elem(elem_)                      //, _fields_data_map(fields_data_map_)
	{
	}

	template <typename key_t, typename value_t>
	inline void operator()(const boost::fusion::pair<key_t, value_t>& pair) const
	{
		_elem->setAttribute(boost::mpl::c_str<key_t>::value, detail::to_qstring(pair.second)); //boost::fusion::at_key<key_t>(*_fields_data_map)
	}
};

Record::Record(full_fields_map field_data_static_map_)
    : _lite_flag(true) // boost::intrusive_ref_counter<Record,
		       // boost::thread_safe_counter>()  //
		       // std::enable_shared_from_this<Record>()
    //	  , _field_data(field_data_)
    , _fields_data_map(field_data_static_map_)
    , _text("")
#ifdef USE_STAR_RATING
    , _star_rating(new StarRating(1, 5))
#endif
    , _attach_table_data(std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record*>(this)))) // {
    // liteFlag = true;    // By default, the object light // По-умолчанию
    // объект легкий
    , dom_from_record_impl(
          [&](std::shared_ptr<QDomDocument> doc) -> QDomElement {
		  QDomElement elem = doc->createElement("record");

		  //		  //			  // Перебираются допустимые имена полей, доступных для сохранения
		  //		  //			  QStringList available_field_list = fixedparameters._record_natural_field;
		  //		  //			  // int available_field_list_size = available_field_list.size();
		  //		  //			  for(int j = 0; j < available_field_list.size(); ++j){
		  //		  //				  QString field_name = available_field_list.at(j);
		  //		  //				  if(field_name == "id" && natural_field_source(field_name) == "") natural_field_source(field_name, natural_field_source("dir").length() > 0 ? natural_field_source("dir") : get_unical_id());
		  //		  //				  // Устанавливается значение поля как атрибут DOM-узла
		  //		  //				  if(is_natural_field_exists(field_name)) elem.setAttribute(field_name, natural_field_source(field_name));
		  //		  //			  }

		  //		  Apply<boost::mpl::size<natural_fields_keys>::type::value,
		  //		      element_fullfill // std::bind(func, // this)//<boost::mpl::at<natural_field_type,// boost::mpl::_1> >
		  //		      >(element_fullfill(elem, this))(std::forward<natural_fields_key_tuple const>(FixedParameters::_record_natural_fields_key_tuple));
		  boost::fusion::for_each(_fields_data_map, record_to_dom(&elem)); //, &_fields_data_map
		  // К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если таковые есть
		  if (_attach_table_data->size() > 0)
			  elem.appendChild(_attach_table_data->dom_from_data(doc));
		  return elem;
	  })

    , has_attach_field([&] { return this->_attach_table_data->size() > 0 ? "1" : "0"; })
    , attach_count_field([&] { return QString::number(this->_attach_table_data->size()); })
{
}

#ifdef _with_record_table

Record::Record(const Record& obj)
    : boost::intrusive_ref_counter<Record, boost::thread_safe_counter>()
    , _lite_flag(obj._lite_flag)
    , _attach_table_data(std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record*>(this))))
{
	// if(obj->_page != nullptr) {

	// _page = obj->_page;
	// _page->bind(boost::intrusive_ptr<Record>(this)); // does not work?
	// obj->page_to_nullptr();

	////        obj.breakpage();

	////        obj._page->record(nullptr);   // dangerous
	////        obj._page = nullptr;          // readonly
	// }

	// Скопировать нужно каждый кусочек класса, сами они не копируются
	// _lite_flag = obj._lite_flag;

	_field_data = obj._field_data;
	_text = obj._text;
	_picture_files = obj._picture_files;
	// _attach_table_data =
	// std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record
	// *>(this)));
	assert(obj._attach_table_data);
	// _attach_table_data = obj._attach_table_data; //
	*_attach_table_data = *obj._attach_table_data;

	// Обратный указатель во включенном объекте должен указывать на новый
	// экземпляр
	// _attach_table_data->record(boost::intrusive_ptr<Record>(this));

	_attach_table_data->update_attach_table_back_link();
	// _is_registered_to_shadow_list = obj._is_registered_to_shadow_list;
	// _position = obj->_position;
	// _open_link_in_new_window = obj->_open_link_in_new_window;
	// bool    _active_immediately = false;
	// _binder = obj->_binder;
	// _activator = obj->_activator;
}

Record& Record::operator=(const Record& obj)
{
	// : boost::intrusive_ref_counter<Record, boost::thread_safe_counter>()
	// , _lite_flag(true)
	// ,
	// _attach_table_data(std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record
	// *>(this))))
	if (this != &obj) {
		// if(obj->_page != nullptr) {

		// _page = obj->_page;
		// _page->bind(boost::intrusive_ptr<Record>(this)); // does not work?
		// obj->page_to_nullptr();

		////        obj.breakpage();

		////        obj._page->record(nullptr);   // dangerous
		////        obj._page = nullptr;          // readonly
		// }

		// Скопировать нужно каждый кусочек класса, сами они не копируются
		_lite_flag = obj._lite_flag;

		_field_data = obj._field_data;
		_text = obj._text;
		_picture_files = obj._picture_files;
		// _attach_table_data =
		// std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record
		// *>(this)));
		assert(obj._attach_table_data);
		// _attach_table_data = obj._attach_table_data; //
		if (!_attach_table_data)
			_attach_table_data = std::make_shared<AttachTableData>(
			    boost::intrusive_ptr<Record>(const_cast<Record*>(this)));
		*_attach_table_data = *obj._attach_table_data;

		// Обратный указатель во включенном объекте должен указывать на новый
		// экземпляр
		// _attach_table_data->record(boost::intrusive_ptr<Record>(this));

		_attach_table_data->update_attach_table_back_link();
		// _is_registered_to_shadow_list = obj._is_registered_to_shadow_list;
		// _position = obj->_position;
		// _open_link_in_new_window = obj->_open_link_in_new_window;
		// bool    _active_immediately = false;
		// _binder = obj->_binder;
		// _activator = obj->_activator;
	}
	return *this;
}

// Конструктор копирования
Record::Record(boost::intrusive_ptr<Record> obj)
    : boost::intrusive_ref_counter<Record, boost::thread_safe_counter>()
    , _lite_flag(true)
    , _attach_table_data(std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record*>(this))))
{
	if (obj
	    // && obj.get() != this
	    ) {
		// if(obj->_page != nullptr) {

		// _page = obj->_page;
		// _page->bind(boost::intrusive_ptr<Record>(this)); // does not work?
		// obj->page_to_nullptr();

		////        obj.breakpage();

		////        obj._page->record(nullptr);   // dangerous
		////        obj._page = nullptr;          // readonly
		// }

		// Скопировать нужно каждый кусочек класса, сами они не копируются
		_lite_flag = obj->_lite_flag;
		_field_data = obj->_field_data;
		_text = obj->_text;
		_picture_files = obj->_picture_files;
		// _attach_table_data =
		// std::make_shared<AttachTableData>(boost::intrusive_ptr<Record>(const_cast<Record
		// *>(this)));
		assert(obj->_attach_table_data);
		// _attach_table_data = obj->_attach_table_data; //
		*_attach_table_data = *obj->_attach_table_data;

		// Обратный указатель во включенном объекте должен указывать на новый
		// экземпляр
		// _attach_table_data->record(boost::intrusive_ptr<Record>(this));
		_attach_table_data->update_attach_table_back_link();
		// _is_registered_to_shadow_list = obj->_is_registered_to_shadow_list;
		// _position = obj->_position;
		// _open_link_in_new_window = obj->_open_link_in_new_window;
		// bool    _active_immediately = false;
		// _binder = obj->_binder;
		// _activator = obj->_activator;
	}
}

#endif

// web::WebPage *Record::unique_page()
// {
////    web::WebPage *page = nullptr;

////    //    if(_page) {
////    //        if(_page->record() == this)
////    //            page = _page;
////    //        else
////    //            page = _page->record()->_page;
////    //    }

////    page = _page;

////    while(page && page->_record && page->_record != this) {
////        // if(page->binded_record())
////        page = page->_record->_page;
////    }

////    return page;

// return _page;
// }

// void Record::active_request(int pos, int openLinkIn)
// {
////    _active_request = true;
// _position = pos;
// _open_link_in_new_window = openLinkIn;
// }

// bool Record::is_holder()
// {
// bool is_holder_ = false;

// if(_page) is_holder_ = _page->_tree_item.get() == this;

// return is_holder_;
// }

Record::~Record()
{
	// if(_page != nullptr) {
	////
	// web::WebView *view = _page->view();
	// web::TabWidget *tabmanager = nullptr;

	// if(view) {
	// tabmanager = view->tabmanager();
	// }

	// if(_page->_tree_item) {

	//// multi record to one page:
	//// assert(_page->record()->getNaturalFieldSource("id") ==
	///this->getNaturalFieldSource("id"));
	//// assert(_page->record()->getNaturalFieldSource("url") ==
	///this->getNaturalFieldSource("url"));
	//// assert(_page->record().get() == this);

	// bool is_holder = (_page->_tree_item.get() == this);     // _page->record()
	// may mean some other record

	// page_to_nullptr();

	////        _page->record(nullptr);
	////        _page = nullptr;

	// if(view && tabmanager && is_holder
	//// && check_register_record(QUrl(web::DockedWindow::_defaulthome)) != this
	// ) {
	// assert(_page == _page->_tree_item->unique_page());   //
	// _page->rebind_record() make sure of this statement
	// tabmanager->closeTab(tabmanager->webViewIndex(view));
	// }
	// }

	////
	// }
}
namespace detail {
	struct dom_to_record_impl {
		full_fields_map* const _field_data_map;
		QMap<QString, QString> _field_data;
		dom_to_record_impl(full_fields_map* const field_data_map_, const QMap<QString, QString>& field_data_)
		    : _field_data_map(field_data_map_)
		    , _field_data(field_data_)
		{
		}
		template <typename key_type_, typename value_type_>
		inline void
		operator()(const boost::fusion::pair<key_type_, value_type_>& pair) const
		{
			(void)pair;
			//		pair.second = detail::from_qstring<key_type_>(_field_data[boost::mpl::c_str<key_type_>::value]);
			boost::fusion::at_key<key_type_>(*_field_data_map) = detail::from_qstring<key_type_>(_field_data[boost::mpl::c_str<key_type_>::value]);
		}
	};
}
// На вход этой функции подается элемент <record>
void Record::dom_to_record(const QDomElement& _dom_element)
{
	// QDomElement _dom_element;

	// Получение списка всех атрибутов текущего элемента
	QDomNamedNodeMap attList;
	attList = _dom_element.attributes();
	QMap<QString, QString> field_data;
	// Перебор атрибутов в списке и добавление их в запись
	int i;
	for (i = 0; i < attList.count(); i++) {
		QDomAttr attcurr = attList.item(i).toAttr();

		QString name = attcurr.name();
		QString value = attcurr.value();

		field_data[name] = value; // this->natural_field_source(name, value);

		// аспечатка считанных данных в консоль
		// qDebug() << "Read record attr " << name << value;
	}

	boost::fusion::for_each(_fields_data_map, detail::dom_to_record_impl(&_fields_data_map, field_data));
	// Инициализируется таблица прикрепляемых файлов
	_attach_table_data->clear(); // Подумать, возможно эта команда не нужна
	_attach_table_data->record(boost::intrusive_ptr<Record>(this));
	if (_lite_flag != _attach_table_data->is_lite()) {
		if (_lite_flag && !_attach_table_data->is_lite())
			_attach_table_data->switch_to_lite();
		else if (!_lite_flag && _attach_table_data->is_lite())
			_attach_table_data->switch_to_fat();
	}
	// Проверка, есть ли у переданного DOM-элемента таблица файлов для заполнения
	if (!_dom_element.firstChildElement("files").isNull())
		_attach_table_data->dom_to_data(_dom_element.firstChildElement("files")); // Заполнение таблицы приаттаченных файлов
	if (_lite_flag && natural_field_source<id_key>() != global_root_id)
		to_fat();
}

QDomElement Record::dom_from_record() const
{
	// dom_from_record = [&](std::shared_ptr<QDomDocument> doc) ->QDomElement
	// const {
	// QDomElement elem = doc->createElement("record");

	//// Перебираются допустимые имена полей, доступных для сохранения
	// QStringList available_field_list = fixedparameters._record_natural_field;
	//// int available_field_list_size = available_field_list.size();

	// for(int j = 0; j <  available_field_list.size(); ++j) {
	// QString field_name = available_field_list.at(j);

	//// Устанавливается значение поля как атрибут DOM-узла
	// if(is_natural_field_exists(field_name))
	// elem.setAttribute(field_name, natural_field_source(field_name));
	// }

	//// К элементу записи прикрепляется элемент таблицы приаттаченных файлов,
	///если таковые есть
	// if(_attach_table_data->size() > 0)
	// elem.appendChild(_attach_table_data->dom_from_data(doc));

	// return elem;
	// };

	std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();

	return dom_from_record_impl(doc);
}

// QDomElement Record::dom_from_record(std::shared_ptr<QDomDocument> doc) const
// {
// QDomElement elem = doc->createElement("record");

//// Перебираются допустимые имена полей, доступных для сохранения
// QStringList available_field_list = fixedparameters._record_natural_field;
//// int available_field_list_size = available_field_list.size();

// for(int j = 0; j <  available_field_list.size(); ++j) {
// QString field_name = available_field_list.at(j);

//// Устанавливается значение поля как атрибут DOM-узла
// if(is_natural_field_exists(field_name))
// elem.setAttribute(field_name, natural_field_source(field_name));
// }

//// К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если
///таковые есть
// if(_attach_table_data->size() > 0)
// elem.appendChild(_attach_table_data->dom_from_data(doc));

// return elem;
// }

// QDomElement Record::export_to_dom(QDomDocument *doc) const
// {
// QDomElement elem = doc->createElement("record");

//// Перебираются допустимые имена полей, доступных для сохранения
// QStringList availableFieldList =
// fixedparameters.recordNaturalFieldAvailableList;
// int availableFieldListSize = availableFieldList.size();

// for(int j = 0; j < availableFieldListSize; ++j) {
// QString currentFieldName = availableFieldList.at(j);

//// Устанавливается значение поля как атрибут DOM-узла
// if(isNaturalFieldExists(currentFieldName))
// elem.setAttribute(currentFieldName, getNaturalFieldSource(currentFieldName));
// }

//// К элементу записи прикрепляется элемент таблицы приаттаченных файлов, если
///таковые есть
// if(_attach_table_data.size() > 0)
// elem.appendChild(_attach_table_data.export_to_dom(doc));

// return elem;
// }

bool Record::is_crypt() const
{
	return field<crypt_key>() == crypt_value(true); // "1";
}

namespace detail {
	struct check_value {
		//	const full_fields_map &_field_data_map;
		bool* const _is_empty;
		check_value( //const full_fields_map &field_data_map_,
		    bool* const is_empty_)
		    : // _field_data_map(field_data_map_),
		    _is_empty(is_empty_)
		{
		}
		template <typename key_type_, typename value_type_>
		inline void
		operator()(const boost::fusion::pair<key_type_, value_type_>& pair) const
		{
			(void)pair;
			//		pair.second = detail::from_qstring<key_type_>(_field_data[boost::mpl::c_str<key_type_>::value]);
			//		("" == detail::to_string(boost::fusion::at_key<key_type_>(_field_data_map))) ? *_is_empty = true : false;//work
			("" == detail::to_string(pair.second)) ? * _is_empty |= true : false;
		}
	};
}

bool Record::is_empty() const
{
	// Заполненная запись не может содержать пустые свойства
	// if(_field_data.count() == 0)
	// return true;
	// else
	bool fields_empty = false;
	boost::fusion::for_each(_fields_data_map, detail::check_value(&fields_empty));

	// return false;
	return fields_empty && //(_field_data.count() == 0)
	    ((is_lite()) ? (text_from_lite_direct() == "") : (text_from_fat() == ""));
}

bool Record::is_lite() const
{
	return _lite_flag;
}

void Record::to_lite()
{
	// Переключение возможно только из полновесного состояния
	if (_lite_flag == true)
		critical_error("Record::to_lite() : Record " + id_and_name() + " already lite");
	_text.clear();
	_picture_files.clear();

	_attach_table_data->switch_to_lite();

	_lite_flag = true;
}

// Метод используется только при следующих действиях:
// - добавление новой записи
// - помещение записи в буфер обмена
// This method is used only when the following actions:
// - Adding new entry
// - Space entries to the clipboard
void Record::to_fat()
{
	// Переключение возможно только из легкого состояния
	if (_lite_flag != true || _text.length() > 0 || _picture_files.count() > 0)
		critical_error("Unavailable switching record object to fat state. " + id_and_name());
	if (_text.size() != 0)
		check_and_create_text_file();
	_attach_table_data->switch_to_fat();

	_lite_flag = false;
}

QString Record::id_and_name() const
{
	QString id, name;
	//	if (_field_data.contains(boost::mpl::c_str<id_key>::value))
	id = boost::fusion::at_key<id_key>(_fields_data_map);     // _field_data[boost::mpl::c_str<id_key>::value];
								  //	if (_field_data.contains(boost::mpl::c_str<name_key>::value))
	name = boost::fusion::at_key<name_key>(_fields_data_map); // _field_data[boost::mpl::c_str<name_key>::value];
	return "Record ID: " + id + " Name: " + name;
}

//// Получение значения поля
//// Метод возвращает расшифрованные данные, если запись была зашифрована
// QString Record::field(QString _name) const {
//// Если имя поля недопустимо
// if(!
// fixedparameters.is_record_field_available(_name))critical_error("Record::field()
// : get unavailable field " + _name);
//// Для настоящего поля
// if(fixedparameters.is_record_field_natural(_name))return
// natural_field(_name);
//// Для вычислимого поля
// if(fixedparameters.is_record_field_calculable(_name))return
// calculable_field(_name);
// return "";
// }

//// Защищенная функция
//// Проверка допустимости имени происходит в вызывающем коде
// QString Record::calculable_field(QString name) const {
//// Наличие аттачей
// if(name == "hasAttach"){
// if(this->_attach_table_data->size() > 0)return "1";
// else return "0";
// }
//// Количество аттачей
// if(name == "attachCount")return
// QString::number(this->_attach_table_data->size());
// return "";
// }

// void Record::write_crypt_field(const QString &_name, const QString &value){//
// template <typename field_type>void Record::field_write<field_type,
// crypt_field_type>::operator()(const QString &value){// template<typename
// field_type> void Record::field(const QString &value){
//// QString _name = boost::mpl::c_str < typename boost::mpl::at <
///crypt_field_type, field_type >::type >::value;
////// Если имя поля недопустимо (установить значение можно только для
///натурального поля)
//// if(fixedparameters.is_record_field_natural(_name) ==
///false)critical_error("In Record::field() unavailable field name " + _name + "
///try set to " + value);
// bool is_crypt = false;
//// Если имя поля принадлежит списку полей, которые могут шифроваться
//// и в наборе полей есть поле crypt
//// и поле crypt установлено в 1
//// и поле не пустое (пустые данные ненужно шифровать)
//// if(fixedparameters._record_field_crypted.contains(_name)){
// if(_field_data.contains(boost::mpl::c_str < crypt_type > ::value)){
// if(_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){
// if(value.length() > 0){
// if(globalparameters.crypt_key().length() > 0)is_crypt = true;
// else critical_error("In Record::field() can not set data to crypt field " +
// _name + ". Password not setted");
// }
// }
// }
//// }
// QString _value = value;
//// Если нужно шифровать, поле шифруется
// if(is_crypt == true)_value =
// CryptService::encryptString(globalparameters.crypt_key(), value);
//// Устанавливается значение поля

// _field_data.insert(_name, _value);   // _field_data[_name] = value; //

//// qDebug() << "Record::set_field : pos" << pos << "name" << name << "value"
///<< value;
// }

//bool Record::is_natural_field_exists(QString name) const {
//	return _field_data.contains(name);
//}

//// template<> QString Record::field<has_attach_type>() const {return
///_attach_table_data->size() > 0 ? "1" : "0"; }
// template <>QString Record::field_read<has_attach_type,
// calculable_field_type>::operator()(){// {return
// _this->_field_data[boost::mpl::c_str < typename boost::mpl::at <
// natural_field_type, field_type_current > ::type > ::value] = value; }
// return _this->_attach_table_data->size() > 0 ? "1" : "0";
// }

//// template<> QString Record::field<attach_count_type>() const {return
///QString::number(this->_attach_table_data->size()); }
// template <>QString Record::field_read<attach_count_type,
// calculable_field_type>::operator()(){// {return
// _this->_field_data[boost::mpl::c_str < typename boost::mpl::at <
// natural_field_type, field_type_current > ::type > ::value] = value; }
// return QString::number(_this->_attach_table_data->size());
// }

//// Защищенная функция
//// Проверка допустимости имени происходит в вызывающем коде
//// Protected function
//// Validation comes in the name of the calling code
//// template <typename field_type>QString Record::field_read<field_type,
///crypt_field_type>::operator()(){//
// QString Record::crypt_field(const QString &name) const {
//// QString name = boost::mpl::c_str < typename boost::mpl::at <
///append_to_crypt_type, field_type >::type >::value;
// QString result = "";
//// bool is_crypt = false;
////// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил
///пароль)
////// то расшифровка невозможна
////// if(fixedparameters._record_field_crypted.contains(name)){
//// if(_this->_field_data.contains(boost::mpl::c_str < crypt_type > ::value)){
//// if(_this->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){
//// if(globalparameters.crypt_key().length() == 0)return QString();
//// }
//// }
////// }
//////// Если поле с таким названием есть
////// if(_field_data.contains(name)){  // assert by
///field_static<natural_field_type>
////// Нужно определить, зашифровано поле или нет
////// Если имя поля принадлежит списку полей, которые могут шифроваться
////// и в наборе полей есть поле crypt
////// и поле crypt установлено в 1
////// и запрашиваемое поле не пустое (пустые данные невозможно расшифровать)
////// if(fixedparameters._record_field_crypted.contains(name)){
// auto crypt_field_name = boost::mpl::c_str<crypt_type>::value;
// if(_field_data.contains(crypt_field_name)){  // boost::mpl::c_str <
// crypt_type > ::value
// if(_field_data[crypt_field_name] == "1"){
// if(_field_data[name].length() > 0 && globalparameters.crypt_key().length() >
// 0){
//// is_crypt = true;
//// Поле расшифровывается
// result = CryptService::decryptString(globalparameters.crypt_key(),
// _field_data[name]);
// }
// }else{
// result = _field_data[name];
// }
// }else result = _field_data[name];
////// }
////// Если поле не подлежит шифрованию
//// if(is_crypt == false)result = _this->_field_data[name]; // Возвращается
///значение поля
//// else{
////// Поле расшифровывается
//// result = CryptService::decryptString(globalparameters.crypt_key(),
///_this->_field_data[name]);
//// }
////// }
////// qDebug() << "Record::get_field : pos" << pos
///<<"name"<<name<<"value"<<result;

// return result;
// }

struct full_fill_natural_list {

	QMap<QString, QString>& _result_field_list;
	const full_fields_map& _fields_data_map;
	//	bool _is_crypt;

	full_fill_natural_list(QMap<QString, QString>& resultfieldlist_, const full_fields_map& fields_data_map_) //, bool isCrypt)
	    : _result_field_list(resultfieldlist_),
	      _fields_data_map(fields_data_map_) //	    , _is_crypt(isCrypt)
	{
	}
	template <typename key_type>
	void operator()(key_type field_type_value) const
	{
		(void)field_type_value;
		QString key_name = boost::mpl::c_str<key_type>::value; // fieldNames.at(i);
		QString value = detail::to_qstring(boost::fusion::at_key<key_type>(_fields_data_map));
		// Если поле с таким именем существует
		if ("" != value) { //if (_this->_field_data.contains(currName)) {
			QString result = "";

			struct deal_crypt {
				//					QString &_result;
				//					const full_fields_map &_fields_data_map;

				deal_crypt(QString& result_, const full_fields_map& fields_data_map_)
				//					    : _result(result_)
				//					    , _fields_data_map(fields_data_map_)
				{

					if (boost::fusion::at_key<crypt_key>(fields_data_map_) == crypt_value(false))            //if (_is_crypt == false)
						result_ = detail::to_qstring(boost::fusion::at_key<key_type>(fields_data_map_)); // _this->_field_data[key_name];                                     // Напрямую значение поля
					else if (gl_paras->crypt_key().length() > 0) {
						//				// Присутствует шифрование
						//				// Если поле не подлежит шифрованию (не все поля в зашифрованной ветке
						//				// шифруются. Например, не шифруется ID записи)
						//				if (fixedparameters._record_field_crypted.contains(key_name) == false)
						//					result = _this->_field_data[key_name];  // Напрямую значение поля
						//				else if (gl_paras->crypt_key().length() > 0 && fixedparameters._record_field_crypted.contains(key_name))
						//					result = CryptService::decryptString(gl_paras->crypt_key(), _this->_field_data[key_name]);  // асшифровывается значение поля

						//							_result = CryptService::decryptString(gl_paras->crypt_key(), detail::to_qstring(boost::fusion::at_key<key_type>(_fields_data_map)));  // асшифровывается значение поля
						result_ = CryptService::decryptString(gl_paras->crypt_key(), detail::to_qstring(boost::fusion::at_key<key_type>(fields_data_map_)));
					}
				}
			};
			struct deal_free {
				//					QString &_result;
				//					const full_fields_map &_fields_data_map;
				deal_free(QString& result_, const full_fields_map& fields_data_map_)
				//					    : _result(result_)
				//					    , _fields_data_map(fields_data_map_)
				{
					result_ = detail::to_qstring(boost::fusion::at_key<key_type>(fields_data_map_)); //_result = detail::to_qstring(boost::fusion::at_key<key_type>(_fields_data_map));
				}
			};
			typedef typename sd::static_if<boost::mpl::has_key<crypt_fields_keys, key_type>::value, deal_crypt, deal_free>::type current_type;
			current_type ct(result, _fields_data_map);

			_result_field_list[key_name] = result;
		}
	}
};

// Получение значений всех натуральных (невычислимых) полей
// Поля, которые могут быть у записи, но не заданы, не передаются
// Поля, которые зашифрованы, расшифровываются
QMap<QString, QString> Record::natural_field_list() const
{
	// Список имен инфополей
	//	QStringList fieldNames = fixedparameters._record_natural_field;

	QMap<QString, QString> result_field_list;

	//	// Проверяется, используется ли шифрование
	//	bool isCrypt =  //false;
	//	    //	if (_field_data.contains(boost::mpl::c_str<crypt_key>::value))
	//	    //		if (_field_data[boost::mpl::c_str<crypt_key>::value] == "1")
	//	    isCrypt = boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true);  // true;
	// Перебираются названия полей
	Apply<boost::mpl::size<natural_fields_keys>::type::value, full_fill_natural_list>(full_fill_natural_list(result_field_list, this->_fields_data_map))(std::forward<natural_fields_key_tuple const>(FixedParameters::_record_natural_fields_key_tuple));

	//	for(int i = 0; i < boost::mpl::size<natural_field_tuple>::type::value;
	//++i){//for(int i = 0; i < fieldNames.size(); ++i){
	//		QString currName = fieldNames.at(i);
	//		// Если поле с таким именем существует
	//		if(_field_data.contains(currName)){
	//			QString result = "";
	//			if(isCrypt == false) result = _field_data[currName]; // Напрямую
	//значение поля
	//			else{
	//				// Присутствует шифрование
	//				// Если поле не подлежит шифрованию (не все поля в
	//зашифрованной ветке шифруются. Например, не шифруется ID записи)
	//				if(fixedparameters._record_field_crypted.contains(currName)
	//== false) result = _field_data[currName]; // Напрямую значение поля
	//				else if(gl_paras->crypt_key().length() > 0 &&
	//fixedparameters._record_field_crypted.contains(currName)) result =
	//CryptService::decryptString(gl_paras->crypt_key(), _field_data[currName]);
	//// асшифровывается значение поля
	//			}
	//			resultFieldList[currName] = result;
	//		}
	//	}
	qDebug() << "Record::getFieldList() : " << result_field_list;
	///
	return result_field_list;
}

std::shared_ptr<AttachTableData> Record::attach_table() const
{
	if (this->is_lite() != _attach_table_data->is_lite())
		critical_error("attach_table()const: Unsyncro lite state for record: " + id_and_name());
	return _attach_table_data;
}

std::shared_ptr<AttachTableData> Record::attach_table()
{
	if (this->is_lite() != _attach_table_data->is_lite())
		critical_error("attach_table(): Unsyncro lite state for record: " + id_and_name());
	return _attach_table_data;
}

/*
   AttachTableData *Record::getAttachTable() const
   {
   // У легкого объекта невозможно запросить приаттаченные файлы, если так
   происходит - это ошибка вызывающей логики
   // if(liteFlag==true && attachTable.is)
   //   critical_error("Cant get attach files from lite record object
   "+getIdAndNameAsString());

   if(attachTable.size()==0)
    return nullptr;

   if(this->isLite()!=attachTable.isLite())
    critical_error("Pointer getAttachTable(): Unsyncro lite state for record:
   "+getIdAndNameAsString());

   return &attachTable;
   }
 */

void Record::attach_table(std::shared_ptr<AttachTableData> _attach_table_data)
{
	if (this->is_lite() != _attach_table_data->is_lite())
		critical_error("attach_table(): Unsyncro lite state for record: " + id_and_name());
	this->_attach_table_data = _attach_table_data;
}

/*
   void Record::setAttachTable(AttachTableData *iAttachTable)
   {
   // Легкому объекту невозможно установить таблицу аттачей, если так происходит
   это ошибка вызывающей логики
   // if(liteFlag==true)
   //   critical_error("Cant set attach files for lite record object
   "+getIdAndNameAsString());

   if(this->isLite()!=attachTable.isLite())
    critical_error("setAttachTable(): Unsyncro lite state for record:
   "+getIdAndNameAsString());

   if(iAttachTable==nullptr)
    critical_error("Record::setAttachTable() : Can`t set null attach table. Set
   only real attach table.");

   attachTable=*iAttachTable;
   }
 */

// Получение текста записи из памяти
// Если запись зашифрована, возвращаемый текст будет расшифрован
QString Record::text_from_fat() const
{
	QString result = "";
	// У легкого объекта невозможно запросить текст из памяти, если так происходит
	// - это ошибка вызывающей логики
	if (_lite_flag == true)
		critical_error("Can\'t get text from lite record object" + id_and_name());
	// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил
	// пароль)
	// то расшифровка невозможна
	if (!(boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true) //_field_data.value(boost::mpl::c_str<crypt_key>::value) == "1"
		&& gl_paras->crypt_key().length() == 0)) {                            // return "";
		// Если шифровать ненужно
		if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false))     //(_field_data.value(boost::mpl::c_str<crypt_key>::value).length() == 0 || _field_data.value(boost::mpl::c_str<crypt_key>::value) == "0")
			result = QString(_text);                                                  // Текст просто преобразуется из QByteArray
		else if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true)) // (_field_data.value(boost::mpl::c_str<crypt_key>::value) == "1")  // Если нужно шифровать
			result = CryptService::decryptStringFromByteArray(gl_paras->crypt_key(), _text);
		else
			critical_error("Record::text_from_fat() : Unavailable crypt field value \"" + detail::to_qstring(boost::fusion::at_key<crypt_key>(_fields_data_map)) + "\"");
	}
	return result;
}

// Получение значения текста напрямую из файла, без заполнения свойства text
QString Record::text_from_lite_direct() const
{
	// У тяжелого объекта невозможно получить текст записи из файла (у тяжелого
	// объекта текст записи хранится в памяти)
	if (_lite_flag == false)
		critical_error("Can\'t run Record::text_from_lite_direct() for non lite record " + id_and_name());
	// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил
	// пароль)
	// то расшифровка невозможна
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true) && gl_paras->crypt_key().length() == 0)
		return "";
	// Выясняется полное имя файла с текстом записи
	QString fileName = full_text_file_name();

	const_cast<Record*>(this)->check_and_create_text_file();

	QFile f(fileName);
	// Открывается файл
	if (!f.open(QIODevice::ReadOnly))
		critical_error("File " + fileName + " not readable. Check permission.");
	// Если незашифровано
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false)) {
		qDebug() << "Record::text_from_lite_direct() : return direct data";

		return QString::fromUtf8(f.readAll());
	} else {
		qDebug()
		    << "Record::text_from_lite_direct() : return direct data after decrypt";

		return CryptService::decryptStringFromByteArray(
		    gl_paras->crypt_key(), f.readAll()); // Если зашифровано
	}
	return "";
}

// Установка текста записи как свойства объекта
// Принимает незашифрованные данные, сохраняет их в памяти, при записи шифрует
// если запись зашифрована
void Record::text_to_fat(QString _text_source)
{
	// Легкому объекту невозможно установить текст, если так происходит - это
	// ошибка вызывающей логики
	if (_lite_flag == true)
		critical_error("Can\'t set text for lite record object" + id_and_name());
	// Если шифровать ненужно
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false))
		_text =
		    _text_source.toUtf8();                                                // Текст просто запоминается в виде QByteArray
	else if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true)) // Если нужно шифровать
		_text = CryptService::encryptStringToByteArray(gl_paras->crypt_key(), _text_source);
	else
		critical_error("Record::text_to_fat() : Unavailable crypt field value \"" + detail::to_qstring(boost::fusion::at_key<crypt_key>(_fields_data_map)) + "\"");
}

// Сохранение текста записи на диск без установки текста записи как свойства
// Принимает незашифрованные данные, сохраняет в файл, при записи шифрует если
// запись зашифрована
void Record::text_to_direct(QString _text_source)
{
	QString file_name = full_text_file_name();
	// Если шифровать ненужно
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false)) {
		// Текст сохраняется в файл
		QFile _file(file_name);
		if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
			critical_error("Can\'t open text file " + file_name + " for write.");
		QTextStream out(&_file);
		out.setCodec("UTF-8");
		out << _text_source;
	} else if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true)) {
		// Текст шифруется
		QByteArray encryptData = CryptService::encryptStringToByteArray(
		    gl_paras->crypt_key(), _text_source);

		// В файл сохраняются зашифрованные данные
		QFile wfile(file_name);
		if (!wfile.open(QIODevice::WriteOnly))
			critical_error("Cant open binary file " + file_name + " for write.");
		wfile.write(encryptData);
	} else
		critical_error("Record::text_to_direct() : Unavailable crypt field value \"" + detail::to_qstring(boost::fusion::at_key<crypt_key>(_fields_data_map)) + "\"");
}

// Запись текста записи, хранимого в памяти, на диск
void Record::create_file_and_save_text()
{
	QString fileName = full_text_file_name();
	assert(fileName != "");
	if (_text.size() != 0) {
		check_and_create_text_file();
		// В файл сохраняются зашифрованные данные
		QFile wfile(fileName);
		if (!wfile.open(QIODevice::WriteOnly))
			critical_error(
			    "Record::create_file_and_save_text() : Can\'t open binary file " +
			    fileName + " for write.");
		// Сохраняется QByteArray с текстом записи (в QByteArray могут быть как
		// зашифрованные, так и не зашифрованные данные)
		wfile.write(_text);
	}
}

QMap<QString, QByteArray> Record::picture_files() const
{
	// У легкого объекта невозможно запросить картинки, если так происходит - это
	// ошибка вызывающей логики
	if (_lite_flag == true)
		critical_error("Can\'t get picture files from lite record object" + id_and_name());
	return _picture_files;
}

// todo: Переделать на копирование по ссылке
void Record::picture_files(QMap<QString, QByteArray> picture_files)
{
	// Легкому объекту невозможно установить картики, если так происходит - это
	// ошибка вызывающей логики
	if (_lite_flag == true)
		critical_error("Cant set picture files for lite record object" + id_and_name());
	_picture_files = picture_files;
}
namespace detail {

	struct encrypt_impl {
		full_fields_map* const _full_fields_map;
		encrypt_impl(full_fields_map* const full_fields_map_)
		    : _full_fields_map(full_fields_map_)
		{
		}
		template <typename key_type_, typename value_type_>
		inline void
		operator()(const boost::fusion::pair<key_type_, value_type_>& pair) const
		{
			(void)pair;
			//		pair.second = detail::from_qstring<key_type_>(_field_data[boost::mpl::c_str<key_type_>::value]);
			//		("" == detail::to_string(boost::fusion::at_key<key_type_>(_field_data_map))) ? *_is_empty = true : false;//work
			detail::static_if<boost::mpl::has_key<crypt_fields_keys, key_type_>::value>([&] {
				if (gl_paras->crypt_key().length() > 0) // QString CryptService::encryptString(QByteArray key, QString line)
					boost::fusion::at_key<key_type_>(*_full_fields_map) = detail::from_qstring<key_type_>(CryptService::encryptString(gl_paras->crypt_key(), detail::to_qstring(boost::fusion::at_key<key_type_>(*_full_fields_map))));

			});
		}
	};
	struct decrypt_impl {
		full_fields_map* const _full_fields_map;
		decrypt_impl(full_fields_map* const full_fields_map_)
		    : _full_fields_map(full_fields_map_)
		{
		}
		template <typename key_type_, typename value_type_>
		inline void
		operator()(const boost::fusion::pair<key_type_, value_type_>& pair) const
		{
			(void)pair;
			//		pair.second = detail::from_qstring<key_type_>(_field_data[boost::mpl::c_str<key_type_>::value]);
			//		("" == detail::to_string(boost::fusion::at_key<key_type_>(_field_data_map))) ? *_is_empty = true : false;//work
			detail::static_if<boost::mpl::has_key<crypt_fields_keys, key_type_>::value>([&] {
				if (gl_paras->crypt_key().length() > 0) // QString CryptService::encryptString(QByteArray key, QString line)
					boost::fusion::at_key<key_type_>(*_full_fields_map) = detail::from_qstring<key_type_>(CryptService::decryptString(gl_paras->crypt_key(), detail::to_qstring(boost::fusion::at_key<key_type_>(*_full_fields_map))));

			});
		}
	};
}

// Privacy feature encrypts only field// Приватная функция, шифрует только поля
void Record::to_encrypt_fields(void)
{
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true))
		return;

	// Для шифрации просто переустанавливаются поля.
	// В момент, когда поле переустанавливается, оно получит зашифрованное
	// значение так как у записи установлен флаг шифрования
	// To encrypt a field reset.
	// At a time when the field is reset, it receives an encrypted
	// Value since records set the encryption flag

	//	// Select field allowed for encryption	// Выбираются поля, разрешенные для шифрования
	//	foreach (QString field_name, FixedParameters::_record_crypt_fields_key_tuple) {
	//		// Если в полях записей присутствует очередное разрешенное имя поля
	//		// И это поле непустое
	//		// Поле шифруется
	//		// If there is another record crop allowed field name
	//		// This is a non-empty field
	//		// Field is encrypted
	//		if (_field_data.contains(field_name))
	//			if (_field_data[field_name].length() > 0)
	//				_field_data[field_name] = _field_data.value(field_name);  // Set the values, the installation will crypting // Устанавливаются значения, при установке произойдет шифрация
	//	}
	boost::fusion::for_each(_fields_data_map, detail::encrypt_impl(&_fields_data_map));
	// Устанавливается поле (флаг) что запись зашифрована
	boost::fusion::at_key<crypt_key>(_fields_data_map) = crypt_value(true);
}

// Приватная функция, расшифровывает только поля
void Record::to_decrypt_fields(void)
{
	// Нельзя расшифровать незашифрованную запись
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false))
		return;
	//	// Выбираются поля, разрешенные для шифрования
	//	foreach (QString field_name, FixedParameters::_record_crypt_fields_key_tuple) {
	//		// Если в полях записей присутствует очередное разрешенное имя поля
	//		// И это поле непустое
	//		// Поле расшифровывается
	//		if (_field_data.contains(field_name))
	//			if (_field_data[field_name].length() > 0)
	//				_field_data[field_name] = _field_data.value(field_name);  // natural_field_source(field_name, _field_data[field_name]); // field(field_name)
	//	}
	boost::fusion::for_each(_fields_data_map, detail::decrypt_impl(&_fields_data_map));

	// Устанавливается поле (флаг) что запись не зашифрована
	boost::fusion::at_key<crypt_key>(_fields_data_map) = crypt_value(false);
}

// Шифрование записи с легкими данными
void Record::to_encrypt_and_save_lite(void)
{
	// Метод обрабатывает только легкий объект
	if (_lite_flag == false)
		critical_error("Cant call to_encrypt_and_save_lite() for non lite record object " + id_and_name());
	// Нельзя шифровать уже зашифрованную запись
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true)) //(_field_data.value(boost::mpl::c_str<crypt_key>::value) == "1")
		critical_error("Cant call to_encrypt_and_save_lite() for crypt record object " + id_and_name());
	// В легком объекте данные не из полей находятся в файлах

	// Шифрация файла с текстом записи на диске
	QString dir_name;
	QString file_name;
	check_and_fill_file_dir(dir_name, file_name);
	CryptService::encryptFile(gl_paras->crypt_key(), file_name);

	// Шифрование приаттаченных файлов на диске
	_attach_table_data->encrypt(Attach::areaFile);

	// Зашифровываются поля записи (здесь же устанавливается флаг crypt)
	to_encrypt_fields();

	push_lite_attributes();
}

// Шифрование записи с полновесными данными
void Record::to_encrypt_and_save_fat(void)
{
	// Метод обрабатывает только тяжелый объект
	if (_lite_flag == true)
		critical_error("Cant call to_encrypt_and_save_fat() for non fat record object " + id_and_name());
	// Нельзя шифровать уже зашифрованную запись
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true))
		critical_error("Cant call to_encrypt_and_save_fat() for crypt record object " + id_and_name());
	// Зашифровывается текст записи в памяти
	_text = CryptService::encryptByteArray(gl_paras->crypt_key(), _text);

	// Зашифровываются аттачи в памяти
	_attach_table_data->encrypt(Attach::areaMemory);

	// Зашифровываются поля записи (здесь же устанавливается флаг crypt)
	to_encrypt_fields();

	// Тяжелые данные записываются в хранилище
	push_fat_attributes();
}

// асшифровка записи с легкими данными
void Record::to_decrypt_and_save_lite(void)
{
	// Метод обрабатывает только легкий объект
	if (_lite_flag == false)
		critical_error(
		    "Cant call to_decrypt_and_save_lite() for non lite record object " +
		    id_and_name());
	// Нельзя расшифровать не зашифрованную запись
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false))
		critical_error("Cant call to_decrypt_and_save_lite() for non crypt record object " + id_and_name());
	// асшифровка файла с текстом записи на диске
	QString dir_name;
	QString file_name;
	check_and_fill_file_dir(dir_name, file_name);
	CryptService::decryptFile(gl_paras->crypt_key(), file_name);

	// асшифровка приаттаченных файлов на диске
	_attach_table_data->decrypt(Attach::areaFile);

	// асшифровка полей (здесь же происходит установка флага crypt в конечное
	// значение)
	to_decrypt_fields(); // анее было fieldList=getNaturalFieldList(); (имеено
			     // так, так как getNaturalFieldList() возвращает
			     // расшифрованные данные)

	// Устанавливается флаг что шифрации нет
	// fieldList[boost::mpl::c_str < crypt_type > ::value]="0"; // Похоже, что
	// команда не нужна, так как в switchToDecryptFields() флаг уже установлен
	push_lite_attributes();
}

void Record::to_decrypt_and_save_fat(void)
{
	// Метод обрабатывает только тяжелый объект
	if (_lite_flag == true)
		critical_error(
		    "Cant call to_decrypt_and_save_fat() for non fat record object " +
		    id_and_name());
	// Нельзя расшифровать не зашифрованную запись
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(false))
		critical_error(
		    "Cant call to_decrypt_and_save_fat() for non crypt record object " +
		    id_and_name());
	// асшифровывается текст записи в памяти
	_text = CryptService::decryptByteArray(gl_paras->crypt_key(), _text);

	// асшифровываются аттачи в памяти
	_attach_table_data->decrypt(Attach::areaMemory);

	// асшифровываются поля записи (здесь же происходит установка флага crypt в
	// конечное значение)
	to_decrypt_fields();

	// Тяжелые данные записываются в хранилище
	push_fat_attributes();
}

// Запись "тяжелых" атрибутов (текста, картинок и приаттаченных файлов) на диск
// Запись происходит в явном виде, то, что хранится в Fat-атрибутах, без
// шифрации-дешифрации
void Record::push_lite_attributes()
{
	// Легкий объект невозможно сбросить на диск, потому что он не содержит
	// данных, сбрасываемых в файлы
	assert(_lite_flag == true);
	// critical_error("Cant push lite record object" + id_and_name());
	// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил
	// пароль)
	// то зашифровать текст невозможно
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true) && gl_paras->crypt_key().length() == 0)
		critical_error("Record::push_lite_attributes() : Try save text for crypt record while password not setted.");
	// Заполняются имена директории и полей
	// Директория при проверке создается если ее небыло
	QString dir_name;
	QString file_name;
	if (!dir_exists())
		check_and_fill_file_dir(dir_name, file_name);
	//// Запись файла с текстом записи
	// create_file_and_save_text();
	//// Если есть файлы картинок, они вставляются в конечную директорию
	// if(_picture_files.size() > 0)
	// DiskHelper::saveFilesToDirectory(dir_name, _picture_files);
	// Если есть приаттаченные файлы, они вставляются в конечную директорию
	if (_attach_table_data->size() > 0)
		_attach_table_data->save_attach_files_to_directory(dir_name);
}

// Запись "тяжелых" атрибутов (текста, картинок и приаттаченных файлов) на диск
// Запись происходит в явном виде, то, что хранится в Fat-атрибутах, без
// шифрации-дешифрации
void Record::push_fat_attributes()
{
	// Легкий объект невозможно сбросить на диск, потому что он не содержит
	// данных, сбрасываемых в файлы
	if (_lite_flag == true)
		critical_error("Cant push lite record object" + id_and_name());
	// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил
	// пароль)
	// то зашифровать текст невозможно
	if (boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true) && gl_paras->crypt_key().length() == 0)
		critical_error("Record::push_fat_attributes() : Try save text for crypt record while password not setted.");
	// Заполняются имена директории и полей
	// Директория при проверке создается если ее небыло
	QString dir_name;
	QString file_name;
	if (!dir_exists())
		check_and_fill_file_dir(dir_name, file_name);
	// Запись файла с текстом записи
	create_file_and_save_text();
	// Если есть файлы картинок, они вставляются в конечную директорию
	if (_picture_files.size() > 0)
		DiskHelper::save_files_to_directory(dir_name, _picture_files);
	// Если есть приаттаченные файлы, они вставляются в конечную директорию
	if (_attach_table_data->size() > 0)
		_attach_table_data->save_attach_files_to_directory(dir_name);
}

// Полное имя директории записи
QString Record::full_dir() const
{
	if (boost::fusion::at_key<dir_key>(_fields_data_map) == dir_value(""))
		boost::fusion::at_key<dir_key>(const_cast<Record*>(this)->_fields_data_map) = boost::fusion::at_key<id_key>(const_cast<Record*>(this)->_fields_data_map); // critical_error("Record::full_dir() : Not
																					  // present dir field");
	return gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/base/" + boost::fusion::at_key<dir_key>(this->_fields_data_map);
}

// Короткое имя директории записи
QString Record::short_dir() const
{
	if (boost::fusion::at_key<dir_key>(_fields_data_map) == dir_value(""))
		boost::fusion::at_key<dir_key>(const_cast<Record*>(this)->_fields_data_map) = boost::fusion::at_key<id_key>(const_cast<Record*>(this)->_fields_data_map); // critical_error("Record::short_dir() : Not
																					  // present dir field");
	return boost::fusion::at_key<dir_key>(this->_fields_data_map);
}

// Полное имя файла с текстом записи
QString Record::full_text_file_name() const
{

	if (boost::fusion::at_key<file_key>(this->_fields_data_map) == file_value("")) {
		// critical_error("Record::getFullDirName() : Not present file field");
		return full_dir() + "/" + "text.html";
	}
	return full_dir() + "/" + boost::fusion::at_key<file_key>(this->_fields_data_map);
}

// Full name of any file in the directory entry// Полное имя произвольного файла в каталоге записи
QString Record::full_file_name(QString file_name) const
{
	return full_dir() + "/" + file_name;
}

// Функция проверяет наличие полей dir и file (они используются для текста
// записи)
// проверяет их правильность и заполняет в переданных параметрах полные имена
// директории и файла
void Record::check_and_fill_file_dir(QString& idir_name, QString& i_file_name)
{
	// Полные имена директории и файла
	idir_name = full_dir();
	i_file_name = full_text_file_name();

	QDir recordDir(idir_name);
	// ПРоверки на чтение и запись в этом месте бессмысленны, так как директории
	// просто может не существовать
	// if(!recordDir.isReadable())
	// critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is
	// not readable");
	// Оказывается, что у QDir нет возможности проверить доступность на запись в
	// директорию
	// if(!recordDir.isWritable())
	// critical_error("Record::checkAndFillFileDir() : Directory "+iDirName+" is
	// not writable");
	// Проверяется наличие директории, куда будет вставляться файл с текстом
	// записи
	if (!recordDir.exists()) {
		// Создается новая директория в директории base
		QDir directory(gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/base");
		if (!directory.exists())
			QDir(gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName()).mkdir("base");
		bool result = directory.mkdir(short_dir());
		if (!result)
			critical_error("Record::check_and_fill_file_dir() : Can't create directory '" + short_dir() + "'");
	}
}

bool Record::dir_exists()
{
	bool dir_exists = false;
	QString fileName = full_text_file_name();

	QFile f(fileName);
	QFileInfo fileInfo(f);
	if (fileInfo.absoluteDir().exists())
		dir_exists = true;
	// && f.exists()

	return dir_exists;
}

bool Record::file_exists()
{
	bool file_exists = false;
	QString fileName = full_text_file_name();
	if ("" != fileName) {
		QFile f(fileName);
		QFileInfo fileInfo(f);
		if (fileInfo.absoluteDir().exists() && f.exists())
			file_exists = true;
	}
	return file_exists;
}

// В функцию должно передаваться полное имя файла
void Record::check_and_create_text_file()
{
	QString file_name = full_text_file_name();
	if (file_name == "") {
		//		_field_data[boost::mpl::c_str<file_key>::value]
		boost::fusion::at_key<file_key>(_fields_data_map) = file_value("text.html");
		file_name = full_text_file_name(); // _field_data["file"];
	}
	QFile f(file_name);
	QFileInfo fileInfo(f);
	// Если директория с файлом не существует
	if (!fileInfo.absoluteDir().exists()) {
		// QString messageText = QObject::tr("Database consistency was broken.\n
		// Directory %1 not found.\n Tute will try to create a blank entry for the
		// corrections.").arg(fileInfo.absoluteDir().absolutePath());

		// qWarning() << messageText;

		//// Выводится уведомление что будет создана пустая директория записи
		// QMessageBox msgBox;
		// msgBox.setWindowTitle(QObject::tr("Warning!"));
		// msgBox.setText(messageText);
		// msgBox.setIcon(QMessageBox::Information);
		// msgBox.exec();

		// Создается пустая директория записи
		QDir tempDir("/");
		tempDir.mkpath(fileInfo.absoluteDir().absolutePath());
	}
	// Если файл записи не существует
	if (!f.exists()) {
		// QString messageText = QObject::tr("Database consistency was broken.\n
		// File %1 not found.\n Tute will try to create a blank entry for the
		// corrections.").arg(fileName);

		// qWarning() << messageText;

		//// Выводится уведомление что будет создан пустой текст записи
		// QMessageBox msgBox;
		// msgBox.setWindowTitle(QObject::tr("Warning!"));
		// msgBox.setText(messageText);
		// msgBox.setIcon(QMessageBox::Information);
		// msgBox.exec();

		// Создается пустой текст записи
		text_to_direct(QString());
	}
}

// web::WebView *Record::bind()
// {
// if(!_page)
// return (*binder())(
// boost::intrusive_ptr<Record>(this)  // shared_from_this()
// );
// else
// return _page->view();
// }

// web::WebView *Record::active()
// {
// if(!_page)
// (*binder())(
// boost::intrusive_ptr<Record>(this)  // shared_from_this()
// );

// assert(_page);
////    if(_page->url().toString() != getNaturalFieldSource("url"))   // wrong!
///just activate the wiew
// return (*activator())(
// boost::intrusive_ptr<Record>(this)  // shared_from_this()
// );
////    else
////        return _page->view();
// }

id_value Record::id() const
{
	id_value result;
	if (boost::fusion::at_key<id_key>(_fields_data_map) != id_value(""))
		result = boost::fusion::at_key<id_key>(_fields_data_map); // return id_value(_field_data["id"]);
	else {
		// critical_error("In TreeItem data getting field with unavailable name
		// 'id'");

		// exit(1);
		auto dir = detail::to_qstring(boost::fusion::at_key<dir_key>(_fields_data_map));
		boost::fusion::at_key<id_key>(const_cast<Record*>(this)->_fields_data_map) = detail::from_qstring<id_key>(dir.length() > 0 ? dir : get_unical_id());

		result = boost::fusion::at_key<id_key>(_fields_data_map); // return id_value(_field_data["id"]);
	}
	return result;
}

name_value Record::name() const
{
	return boost::fusion::at_key<name_key>(_fields_data_map);
}

rating_value Record::rating() const
{
	//	auto rating_ = _field_data[boost::mpl::c_str<rating_key>::value];
	return boost::fusion::at_key<rating_key>(_fields_data_map); //rating_.toULongLong();
}

size_t Record::add_rating()
{
	//	auto rating_ = _field_data[boost::mpl::c_str<rating_key>::value];
	//	auto number = rating_.toULongLong();
	//	number++;
	//	_field_data[boost::mpl::c_str<rating_key>::value] = QString::number(number);
	return ++boost::fusion::at_key<rating_key>(_fields_data_map);
	//	return number;
}


void Record::clear_content(void)
{
	//	_misc_fields.clear();
	boost::fusion::for_each(_fields_data_map, clear_full_fields_map(&_fields_data_map));
}

struct field_count_impl {
	size_t* _count;
	field_count_impl(size_t* count)
	    : _count(count)
	{
	}
	template <typename key_t, typename value_t>
	void operator()(const boost::fusion::pair<key_t, value_t>& pair) const
	{
		//		(void)pair;
		if (pair.second != value_t()) (*_count)++; // bool value can't judge!
	}
};

size_t Record::fields_likely_count() const
{
	size_t count = 0;
	boost::fusion::for_each(_fields_data_map, field_count_impl(&count));
	return count;
}

struct dump_decode_impl {
	QMap<QString, QString>* const _result;
	bool _is_crypted = false;
	dump_decode_impl(QMap<QString, QString>* const result_, bool is_crypted_)
	    : _result(result_)
	    , _is_crypted(is_crypted_)
	{
	}

	struct dump_crypt {
		//		dump_decode_impl *const _this;
		template <typename key, typename value>
		dump_crypt(dump_decode_impl const* const this_, const boost::fusion::pair<key, value>& pair)
		//		    : _this(this_)
		{
			if (this_->_is_crypted) {
				if (gl_paras->crypt_key().length() > 0) {
					(*(this_->_result))[boost::mpl::c_str<key>::value] = detail::from_qstring<dynamic_name_key>(CryptService::decryptString(gl_paras->crypt_key(), detail::to_qstring(pair.second)));
				} else {
					critical_error(QString("Dump decode field \"") + boost::mpl::c_str<key>::value + "\" without password"); //(*_result)[boost::mpl::c_str<key>::value] = detail::to_qstring(pair.second);
				}
			} else
				(*(this_->_result))[boost::mpl::c_str<key>::value] = detail::to_qstring(pair.second);
		}

		//		void operator()() {
		//		}
	};

	struct dump_direct {
		//		dump_decode_impl* const _this;
		template <typename key, typename value>
		dump_direct(dump_decode_impl const* const this_, const boost::fusion::pair<key, value>& pair)
		//		    : _this(this_)
		{
			(*(this_->_result))[boost::mpl::c_str<key>::value] = detail::to_qstring(pair.second);
		}

		//		void operator()() {
		//		}
	};
	template <typename key, typename value>
	inline void operator()(const boost::fusion::pair<key, value>& pair) const
	{
		typedef typename sd::static_if<boost::mpl::has_key<crypt_fields_keys, key>::value, dump_crypt, dump_direct>::type dump;
		dump(this, pair);
	}
};
// Get all the established fields "fieldname" -> "to"// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> Record::dump_decode() const
{
	//	qDebug() << "TreeItem::fields_all() : Fields data " << _field_data;

	QMap<QString, QString> result;

	//	QList<QString> names = _field_data.keys();

	//	foreach (QString name, names) {
	//		// В результат добаляются только параметры с разрешенным именем
	//		// if(is_field_name_available(name))
	//		result[name] = _field_data[name];
	//	}
	boost::fusion::for_each(_fields_data_map, dump_decode_impl(&result, boost::fusion::at_key<crypt_key>(_fields_data_map) == crypt_value(true)));
	return result;
}

struct dump_direct_impl {
	QMap<QString, QString>* const _result;
	dump_direct_impl(QMap<QString, QString>* const result_)
	    : _result(result_)
	{
	}
	template <typename key, typename value>
	inline void operator()(const boost::fusion::pair<key, value>& pair) const
	{

		(*_result)[boost::mpl::c_str<key>::value] = detail::to_qstring(pair.second);
	}
};

// Получение всех установленных полей "имя_поля"->"значение"
// Напрямую, без преобразований, без расшифровки
// Get all the established fields "fieldname" -> "to"
// Directly without transformation, without decoding
QMap<QString, QString> Record::dump_direct() const
{
	QMap<QString, QString> result;
	//	return _field_data;
	boost::fusion::for_each(_fields_data_map, dump_direct_impl(&result));
	return result;
}

#include "models/record_table/record.inl"
