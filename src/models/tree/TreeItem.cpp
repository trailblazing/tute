#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"
#include "views/browser/entrance.h"
#include "views/tree/TreeScreen.h"
#include "models/attach_table/AttachTableData.h"
#include "models/record_table/recordindex.hxx"
#include "models/record_table/RecordModel.h"
#include "binder.hxx"
#include "models/record_table/linker.hxx"



// #include "models/record_table/Record.h"
// #include "models/record_table/ItemsFlat.h"


extern GlobalParameters globalparameters;

#ifdef _with_record_table

TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _parent_item
		  , boost::intrusive_ptr<Record>   _record
		  , const QDomElement              &_dom_element)
    : Record(_record)
      , ItemsFlat(_dom_element, _parent_item)
      , _parent_item([&](){
	      if(_parent_item){
		  if(_parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){
		      this->to_encrypt();
		  }else{
		      this->to_decrypt();
		  }
	      }
	      return _parent_item;
	  } ())
{
	// record_to_item();
}

#endif

// TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _parent_item
// , QMap<QString, QString>         _field_data
// , boost::intrusive_ptr<TreeItem> _child_items_root
// )
// : Record(_field_data)
// , ItemsFlat(*_child_items_root)
// , _parent_item([ & ]()
// {
// if(_parent_item) {
// if(_parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
// this->to_encrypt();
// } else {
// this->to_decrypt();
// }

// ItemsFlat::parent(_parent_item);
// }

// return _parent_item;
// }())
// , _page(nullptr)
// {
////    record_to_item();
// }

TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _host_parent
		  , QMap<QString, QString>          _field_data
		  , const QDomElement               &_dom_element
		  , int pos, int mode)
    : Record(_field_data)
      , ItemsFlat(_dom_element, _host_parent ? _host_parent->is_crypt() : false)
      , dom_from_treeitem_impl([&](std::shared_ptr<QDomDocument> doc) -> QDomElement {
		// QDomElement node = doc->createElement("node");
	      QDomElement record = Record::dom_from_record_impl(doc);
	      if(ItemsFlat::count_direct() > 0){
		  QDomElement children = ItemsFlat::dom_from_itemsflat_impl(doc);
		  record.appendChild(children);
	      }
		// node.appendChild(record);
	      return record;				// node;    //
	  })
      , children_parent_as_this([&]() -> void {				// boost::intrusive_ptr<TreeItem> parent_item
		////    assert(static_cast<ItemsFlat *>(parent_item.get()) == this);
		// auto _this = static_cast<TreeItem *const>(this);
		// assert(_this);
		////    QList<boost::intrusive_ptr<TreeItem>> candidates;
		////    for(auto il : _child_linkers) {
		////        if(!il->parent_item() || il->parent_item() != _this) {
		////            candidates << il->host(); // il->host()->parent(parent_item);   // this operation will remove il from _child_linkers and try to insert new one
		////        }
		////    }
		////    for(auto it : candidates) {
		////        it->parent(_this);
		////    }
	      for(int i = 0; i < _child_linkers.size(); i ++){
		  auto il = _child_linkers.at(i);
		  if(! il->host_parent() || il->host_parent() != this){
			// candidates << il->host(); //
		      il->parent(this, i, add_new_record_after);				// this operation will remove il from _child_linkers and try to insert new one
		  }
	      }
	  })

      , _linker(new Linker(_host_parent, this)){															// , pos, mode
	// , _position(PosSource(-1))
    assert(_linker->host().get() == this);
    assert(_linker->host_parent().get() != this);
	// record_to_item();
	// if(_parent_item->_record_linker != _record_linker)_parent_item->_record_linker = _record_linker;







	// boost::intrusive_ptr<Linker> _result(nullptr);

	// if(_host_parent) {
	// if(!_host_parent->contains_direct(std::forward < const boost::intrusive_ptr<Linker> > (_up_linker)) // _host->up_linker()   // _host is under contruction at this time
	// && _host_parent->id() != this->_field_data.value("id")) {







	////        this->child_move_unique =



	////    if(_parent_item && _parent_item != _new_parent) {
	////        _parent_item->remove(_child_item);
	////    }




	////    if(_parent_item && _parent_item != _parent) {
	////        _parent_item->remove(_child_item);
	////    }

	////        auto _child_item_parent = _child_item->parent();  // form ItemsFlat, incomplete

	////        if(_child_item_parent && _child_item_parent != _parent_item) {
	////            _child_item_parent->remove(_child_item);
	////        }


	////        if(_parent) {

	////        assert(_parent_item->id() != _child_item->_field_data.value("id"));   //        assert(_parent_item->name() != _field_data.value("name"));

	//// _parent_item->add_child(boost::intrusive_ptr<TreeItem>(this));



	////            //        }

	////            //    _parent_item = _new_parent;

	////            //            if(_parent) {    //  && _parent_item != it
	////            //        _parent_item = it;
	////            bool found = false;

	////            for(auto i : _parent_item->_child_items) {
	////                if(_child_item->id() == i->id()) {found = true; break;}
	////            }

	////            if(!found) {
	////                //            _result = _parent_item->child_move_unique(this
	////                //                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
	////                //                                                      , _mode
	////                //                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);

	// _result = _up_linker->child_move_unique(_host_parent, pos, mode);    // , _host

	//// _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
	//// _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
	////            }

	// if(_host_parent->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
	// this->to_encrypt();
	// } else {
	// this->to_decrypt();
	// }

	////        }


	////        }

	////        if(_parent_item && _parent_item->up_linker() != this)_parent_item->up_linker(this); // deprecated ItemsFlat::_child_items move to ItemsFlat::_child_linkers
	// assert(_up_linker->integrity());
	// assert(_up_linker->integrity_external(this, _host_parent));
	// }
	// }

    _linker->parent(_host_parent, pos, mode);
}

TreeItem::TreeItem(QMap<QString, QString>   _field_data
		  , const QDomElement       &_dom_element)
    : Record(_field_data)
      , ItemsFlat(_dom_element, false)
      , dom_from_treeitem_impl([&](std::shared_ptr<QDomDocument> doc) -> QDomElement {
		// QDomElement node = doc->createElement("node");
	      QDomElement record = Record::dom_from_record_impl(doc);
	      if(ItemsFlat::count_direct() > 0){
		  QDomElement children = ItemsFlat::dom_from_itemsflat_impl(doc);
		  record.appendChild(children);
	      }
		// node.appendChild(record);
	      return record;				// node;    //
	  })
      , children_parent_as_this([&]() -> void {				// boost::intrusive_ptr<TreeItem> parent_item
	      for(int i = 0; i < _child_linkers.size(); i ++){
		  auto il = _child_linkers.at(i);
		  if(! il->host_parent() || il->host_parent() != this){
			// candidates << il->host(); //
		      il->parent(this, i, add_new_record_after);				// this operation will remove il from _child_linkers and try to insert new one
		  }
	      }
	  })
      , _linker(new Linker(nullptr, this)){
	//
	// , _position(PosSource(-1))
    assert(_linker->host().get() == this);
    assert(_linker->host_parent().get() != this);


    _linker->parent(nullptr);
}


boost::intrusive_ptr<TreeItem> TreeItem::dangle_instance(QMap<QString, QString> _field_data, const QDomElement &_dom_element){
    return new TreeItem(_field_data, _dom_element);
}
// TreeItem::TreeItem(const TreeItem &item)
// : Record(static_cast<const Record &>(item))
// , ItemsFlat(static_cast<const ItemsFlat &>(item))
// , _parent_item([ & ]()
// {
// auto parent_item = const_cast<TreeItem &>(item).parent();

// if(item._parent_item) {
////        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
////        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
////        //        QString crypt_value = "1";
////        QString crypt_key(QString::null); crypt_key = QLatin1String(boost::mpl::c_str < crypt_type > ::value);
////        QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
////                                                           && parent_item->_field_data.contains(crypt_key)
////                                                           && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

////        if(crypt_1 == crypt_value) {
////            this->to_encrypt(); // table_data->to_encrypt();
////        } else if(crypt_0 == crypt_value) {
////            this->to_decrypt(); // table_data->to_decrypt();
////        }

// if(item._parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
// this->to_encrypt();
// } else {
// this->to_decrypt();
// }
// }

// return parent_item; // boost::intrusive_ptr<TreeItem>(new TreeItem(*item._parent_item));
// }())
// {
// if(item.page_valid()    // item._page != nullptr
// ) {

// _page = item._page;
// _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
// const_cast<TreeItem &>(item).page_to_nullptr();

////        obj.breakpage();

////        obj._page->record(nullptr);   // dangerous
////        obj._page = nullptr;          // readonly


// }

// *static_cast<Record *>(const_cast<TreeItem *>(this)) = *static_cast<const Record *>(&item);

////    _parent_item = item._parent_item;
// _position = item._position;

// _open_link_in_new_window = item._open_link_in_new_window;

// _binder = item._binder;

// _activator = item._activator;

// *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);

//// *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);
// }

// TreeItem &TreeItem::operator =(const TreeItem &item)
// {
// if(this != &item) {

// auto parent_item = const_cast<TreeItem &>(item).parent();

// if(item._parent_item) {
////            QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
////            QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
////            //        QString crypt_value = "1";
////            QString crypt_key(QString::null); crypt_key = QLatin1String(boost::mpl::c_str < crypt_type > ::value);
////            QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
////                                                               && parent_item->_field_data.contains(crypt_key)
////                                                               && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

////            if(crypt_1 == crypt_value) {
////                this->to_encrypt(); // table_data->to_encrypt();
////            } else if(crypt_0 == crypt_value) {
////                this->to_decrypt(); // table_data->to_decrypt();
////            }

// if(item._parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
// this->to_encrypt();
// } else {
// this->to_decrypt();
// }
// }

// this->_parent_item = parent_item;   // boost::intrusive_ptr<TreeItem>(new TreeItem(*item._parent_item));

// if(item.page_valid()    // item._page != nullptr
// ) {
// _page = item._page;
// _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
// const_cast<TreeItem &>(item).page_to_nullptr();

////        obj.breakpage();

////        obj._page->record(nullptr);   // dangerous
////        obj._page = nullptr;          // readonly


// }

// *static_cast<Record *>(const_cast<TreeItem *>(this)) = *static_cast<const Record *>(&item);

// this->_position = item._position;

// this->_open_link_in_new_window = item._open_link_in_new_window;

// this->_binder = item._binder;

// this->_activator = item._activator;

// *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);
// }

// return *this;
// }

#ifdef _with_record_table

void TreeItem::records_to_children(){
    for(int i = 0; i < _record_table->size(); i ++){
	this->add_child(_record_table->record(i));
    }
}
#endif

// void TreeItem::record_table(QDomElement i_dom_element)
// {
// _record_table->delete_all_records();
// _record_table.reset();
////    QDomElement *dom_element = &i_dom_element;
// _record_table = std::make_shared<RecordTable>(
// i_dom_element  // boost::intrusive_ptr<TreeItem>(this)   // shared_from_this()
// );

////    _record_data->import(dom_element);
// }



TreeItem::~TreeItem(){
	// В родительском объекте ссылка на данный объект удаляется
	// Подумать, нужно ли это действие
	// Вроде ненужно, но может понадобится вдальнейшем
	// if(parentItem)parentItem->removeChildrenLink(childNumber(),1);

	// Вызывается процедура очищения ветки без физического удаления данных на диске
    isolate();

	//    if(page_valid()    // _page != nullptr
	//      ) {
	//        //
	//        browser::WebView *view = _page->view();
	//        browser::TabWidget *tabmanager = nullptr;

	//        if(view) {
	//            tabmanager = view->tabmanager();
	//        }

	//        if(_page->_item) {


	//            // multi record to one page:
	//            // assert(_page->record()->getNaturalFieldSource("id") == this->getNaturalFieldSource("id"));
	//            // assert(_page->record()->getNaturalFieldSource("url") == this->getNaturalFieldSource("url"));
	//            // assert(_page->record().get() == this);

	//            bool is_holder = (_page->_item.get() == this);     // _page->record() may mean some other record

	//            //            page_to_nullptr();

	//            //        _page->record(nullptr);
	//            //        _page = nullptr;

	//            if(view && tabmanager && is_holder
	//               // && check_register_record(QUrl(browser::DockedWindow::_defaulthome)) != this
	//              ) {
	//                assert(_page == _page->_item->unique_page());   // _page->rebind_record() make sure of this statement
	//                tabmanager->closeTab(tabmanager->webViewIndex(view));
	//            }

	//            page_to_nullptr();
	//        }

	//        //
	//    }

    binder_reset();
}
//// Возвращение ссылки на потомка, который хранится в списке childItems
//// под указанным номером
// boost::intrusive_ptr<TreeItem> TreeItem::item_direct(int number)
// {
// return _child_items.value(number);
// }


// int TreeItem::direct_children_count()const
// {
// return _child_items.count();
// }

//// Возвращение количества потомков (т.е. количество записей в списке childItems)
// int TreeItem::size() const
// {
// return _child_items.count();
// }


// int TreeItem::sibling_order(boost::intrusive_ptr<TreeItem> it)const
// {
// return ItemsFlat::sibling_order(it);
// }

int TreeItem::sibling_order(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_equal) const {
    return ItemsFlat::sibling_order(_equal);
}
// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя

//// Returns the number under which the object is stored
//// Array of the parent child items
// int TreeItem::sibling_order() const
// {
// int result = -1;

////    if(_parent_item)
////        result = _parent_item->_child_items.indexOf(
////                     // boost::const_pointer_cast<TreeItem>(
////                     //                   const_cast<TreeItem *>(this) //
////                     boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
////                     // ) // const_cast<TreeItem *>(this)
////                 );

// if(_record_linker)result = _record_linker->sibling_order();

// return result;
// }




// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел,
// а физические данные на диске не затрагиваются
void TreeItem::isolate(void){
    _field_data.clear();

	// _record_table->
    ItemsFlat::clear();
	////    // Удаляются все подветки
	////    qDeleteAll(_child_items);
	// clear();
	////    //    if(_parent_item) {
	////    //        _parent_item->remove_child(boost::intrusive_ptr<TreeItem>(this));
	////    _parent_item.reset();
	////    //    }
    if(_linker)_linker->remove();
}
int TreeItem::field_count() const {
    return _field_data.count();
}
// void TreeItem::field(QString value){
// Record::field(value);
// }

template<> QString TreeItem::field<dynamic_name_type>() const {
// QString _name = boost::mpl::c_str<typename boost::mpl::at<field_type, dynamic_name>::type>::value;
    QString _field_content = "";				// QString item_name(QString::null)
// bool field_found = false;
	// Если запрашивается динамическое имя из имени и количества потомков
// if(_name == boost::mpl::c_str<typename boost::mpl::at<field_type, dynamic_name>::type>::value){ // "dynamicname"
	// Имя ветки
	// QString
    _field_content = _field_data["name"];
	// Если есть шифрование в этой ветке
	// и поле является зашифрованным
    if(_field_data.contains(boost::mpl::c_str < crypt_type >::value)){
	if(_field_data[boost::mpl::c_str < crypt_type > ::value] == "1"){
	    if(globalparameters.crypt_key().length() > 0){
		_field_content = CryptService::decryptString(globalparameters.crypt_key(), _field_content);
// field_found = true;
	    }else{
		_field_content = QString(QObject::tr("Closed"));
// field_found = true;
	    }
	}
    }
	// Выясняется, есть ли у текущего элемента конечные записи
    int record_count = this->count_direct();
	// Если конечных элементов нет, возвращатся просто имя
    if(record_count != 0){
	// return itemName;
// field_found = true;
// }else{
	// Иначе конечные элементы есть, возвращается имя записи
	// и количество конечных элементов
	QString r, i;
	r = _field_content + " [" + i.setNum(record_count) + "]";
	_field_content = r;										// return r;
// field_found = true;
    }
// }// _name == boost::mpl::c_str<typename boost::mpl::at<field_type, dynamic_name>::type>::value
	//// Если имя поля допустимо
	// if(is_field_name_available(name) && field_found == false) {
	//// Если поле с таким именем существует
	// if(_field_data.contains(name)) {
	// QString value = _field_data[name];
	//// Если есть шифрование
	//// и поле является зашифрованным
	// if(_field_data.contains(boost::mpl::c_str < crypt_type > ::value)) {
	// if(_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
	// if(field_name_for_crypt_list().contains(name)) {
	// if(globalparameters.crypt_key().length() > 0
	// && value != ""
	// )
	// value = CryptService::decryptString(globalparameters.crypt_key(), value);
	// else
	// value = "";
	// }
	// }
	// }
	// item_name = value;   // return value;
	// field_found = true;
	// }
	////        else {
	////            //            return QString(""); // Если поле не существует, возвращается пустая строка
	////        }
	// }
	// else {
	// critical_error("In TreeItem::get_field() unavailable name '" + name + "'");
	// exit(1);
	////        return "";
	// }
// if("" == _field_content && field_found == false){
// if(_name != ""){
// _field_content = Record::field(_name);
// if("" != _field_content)field_found = true;
// }

// }
    return _field_content;
}
// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> TreeItem::fields_all(){
    qDebug() << "TreeItem::fields_all() : Fields data " << _field_data;

    QMap<QString, QString> result;

    QList<QString> names = _field_data.keys();

    foreach(QString name, names){
	// В результат добаляются только параметры с разрешенным именем
	// if(is_field_name_available(name))
	result[name] = _field_data[name];
    }

    return result;
}
// Получение всех установленных полей "имя_поля"->"значение"
// Напрямую, без преобразований, без расшифровки
QMap<QString, QString> TreeItem::fields_all_direct(){
    return _field_data;
}
//// Установка данных
//// Первый параметр - имя поля
//// Второй параметр - устанавливаемое значение
// void TreeItem::field(QString name, QString value)
// {
// if(is_field_name_available(name)) {
//// Если поле нужно шифровать
//// и поле является зашифрованным
// if(_field_data.contains(boost::mpl::c_str < crypt_type > ::value))
// if(_field_data[boost::mpl::c_str < crypt_type > ::value] == "1")
// if(field_name_for_crypt_list().contains(name)) {
//// Если установлен пароль
// if(globalparameters.crypt_key().length() > 0) {
//// Если поле непустое, поле зашифровывается
// if(value != "")
// value = CryptService::encryptString(globalparameters.crypt_key(), value);
// } else // Иначе пароль не установлен
// critical_error("TreeItem::setField() : Can not encrypt field \"" + name + "\". Password not setted.");
// }

//// qDebug() << "Set to item data " << name << value;
// _field_data[name] = value;
// } else
// critical_error("TreeItem::setField() : Set unavailable field \"" + name + "\" to item of branch tree");
// }


//// Установка данных напрямую - какие данные переданы, те и запомнятся
//// без всяких преобразований, без шифрации
// void TreeItem::field_direct(QString name, QString value)
// {
// if(is_field_name_available(name)) {
// _field_data[name] = value;
// } else
// critical_error("TreeItem::setFieldDirect() : Set unavailable field \"" + name + "\" to item of branch tree");
// }


// bool TreeItem::is_field_name_available(QString name) const
// {
////    if(field_name_available_list().contains(name))
////        return true;
////    else
////        return false;
// return field_name_available_list().contains(name);  // || is_natural_field_exists(name);
// }


// QStringList TreeItem::field_name_available_list(void) const
// {
// return (QStringList()
// << "id"
// << "pin"
// << "name"
// << "ctime"
// << boost::mpl::c_str < crypt_type > ::value);
////    return names;
// }


QStringList TreeItem::field_name_for_crypt_list(void) const {
    return(QStringList() << "name");

	// return names;
}
// Linker::~linker()
// {


////    boost::intrusive_ptr<TreeItem> result(nullptr);

// if(_child_item) {
// auto _p = _child_item->_record_linker->_parent_item;

// if(_p && _p != _parent_item) {
////        _child_item->self_remove_from_parent();
////        if(_parent_item) {
////            result =
// _p->remove(_child_item);
////        }

// }

////        _child_item->_record_linker = nullptr;
// _child_item = nullptr;
// }

// if(_parent_item) {
// for(auto it : _parent_item->_child_items) { // for(int row = 0; row < _child_items.size(); ++row) {
////        auto it = _child_items.at(row);

// if(it->id() == _child_item->id()) { //_child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
// _parent_item->_child_items.removeOne(it);
// it->page_break();
// it->record_linker().reset();;   //                it->parent(nullptr);

// if(it != _child_item)_child_item->merge(it);

////                else
////                    result = _child_item;
// }
// }
// }

////    _parent_item->_record_linker = nullptr;
// _parent_item = nullptr;

////    self_remove_from_parent_as_empty();

////    return result;
// }

// Linker::linker(boost::intrusive_ptr<TreeItem>  parentitem, boost::intrusive_ptr<TreeItem>  childitem, int _pos, int _mode)
// : _parent_item([ & ]()->boost::intrusive_ptr<TreeItem> {assert(parentitem != childitem); return parentitem;}()) // _new_parent
// , _child_item(childitem)
// {
////    [ & ]()->boost::intrusive_ptr<TreeItem> {
////        this->_parent_item = _parent;
////        this->_child_item = _child_item;

// boost::intrusive_ptr<TreeItem> _result(nullptr);

// if(_parent_item
// && !_parent_item->contains_direct(_child_item)
// && _parent_item->id() != _child_item->_field_data.value("id")) {






//// Добавление новой записи
//// Метод только добавляет во внутреннее представление новые данные,
//// сохраняет текст файла и обновляет данные на экране.
//// Сохранения дерева XML-данных на диск в этом методе нет.
//// Допустимые режимы:
//// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
//// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
//// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
//// Метод принимает "тяжелый" объект записи
//// Объект для вставки приходит как незашифрованным, так и зашифрованным
// auto child_move_unique = [&](boost::intrusive_ptr<TreeItem>     _parent
// , boost::intrusive_ptr<TreeItem>   _self
// , int pos, int mode
// )->boost::intrusive_ptr<TreeItem> { // child_insert? does not set parent pointer?
// boost::intrusive_ptr<TreeItem> result(nullptr);

////        boost::intrusive_ptr<TreeItem> _child_item(this);

////            assert(_self != _parent);

// if(_self != _parent)
// {

// int found = 0;

// for(auto it = _parent->_child_items.begin(); it != _parent->_child_items.end(); it++) {
// if(it->get() == _self.get()) {
// found++;

// if(found == 1)
// result = *it;
// else
// _parent->_child_items.erase(it);
// }
// }

////        if(0 == found) {
//// Запись добавляется в таблицу конечных записей
// int insert_position = -1;


//// get shadow_branch
////    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

// if(
// 0 == found  // !item_direct(_source_item)
// ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))

////                // deprecated by _parent_target->remove
////                auto _origin_parent = _source_item->parent();

////                if(_origin_parent && _origin_parent != _parent_target) {
////                    //                    if(_parent != this) {
////                    _origin_parent->remove(_source_item);

////                    //                    while(_origin_parent->remove_self_as_empty()) {
////                    //                        _origin_parent = _origin_parent->parent();

////                    //                        if(!_origin_parent)break;
////                    //                    }

////                    //                    }
////                }

//// !!!, this make item move to new branch, remember to remove orignal one
////                    _child_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));








////                    if(_child_item->_record_linker != this)_child_item->_record_linker = this;









////        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();



//// Выясняется, есть ли в дереве запись с указанным ID
//// Если есть, то генерируются новые ID для записи и новая директория хранения
//// Если нет, то это значит что запись была вырезана, но хранится в буфере,
//// и ее желательно вставить с прежним ID и прежним именем директории
//// It turns out, is there a record of a tree with specified ID
//// If there is, then the generated ID for the new record and the new storage directory
//// If not, then it means that the recording was cut out, but stored in a buffer
//// And it is desirable to stick with the same ID and the same name directory
////        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

// if(0 == _self->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
//// Создается новая запись (ID был пустой) или
//// Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
//// Create a new record (ID was empty) or
//// Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

// QString id = get_unical_id();

//// Store directory entries and files    // Директория хранения записи и файл
// if(_self->field("dir") == "")_self->field("dir", id); // get_unical_id()

// if(_self->field("file") == "")_self->field("file", "text.html");

//// Unique ID of XML records             // Уникальный идентификатор XML записи
////            QString id = get_unical_id();
// _self->field("id", id);
// } else {
// if(_self->field("dir") == "")_self->field("dir", _self->field("id")); // get_unical_id()

// if(_self->field("file") == "")_self->field("file", "text.html");
// }

//// The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
// if(_self->is_lite()) {
// _self->to_fat();
////            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
// }

//// В список переданных полей добавляются вычислимые в данном месте поля
// if(_self->field("ctime") == "") {
//// Время создания данной записи
// QDateTime ctime_dt = QDateTime::currentDateTime();
// QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
// _self->field("ctime", ctime);

// }

//// Выясняется в какой ветке вставляется запись - в зашифрованной или нет
// bool is_crypt = false;

////        if(_tree_item) {
////                if(_is_crypt) { //
// if(_parent->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
// if(globalparameters.crypt_key().length() > 0)
// is_crypt = true;
// else
// critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
// }

////        }

//// Запись полновесных данных с учетом шифрации
// if(is_crypt && _self->field(boost::mpl::c_str < crypt_type > ::value) != "1") {       // В зашифрованную ветку незашифрованную запись
// _self->to_encrypt_and_save_fat();
// } else if(!is_crypt && _self->field(boost::mpl::c_str < crypt_type > ::value) == "1") { // В незашифрованную ветку зашифрованную запись
// _self->to_decrypt_and_save_fat();
// } else {
// _self->push_fat_attributes();
// }

//// Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
// _self->to_lite();

////        //        // Запись добавляется в таблицу конечных записей
////        //        int insertPos = -1;

////        item->is_registered_to_record_controller_and_tabmanager(true);

// if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
// _parent->_child_items << _self;
////            insert_position = _child_items.size() - 1;
// } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
// _parent->_child_items.insert(pos, _self);
////            insert_position = pos;
// } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
// _parent->_child_items.insert(pos + 1, _self);
////            insert_position = pos + 1;
// }

////                    insert_position =  _parent->sibling_order(_self);  //_child_item->record_linker()->sibling_order();   // ItemsFlat::
////                    _parent_target->sibling_order(_source_item);
////                    qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

//// Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

// }

////                else {
////                    insert_position = _parent->sibling_order(_self);  // _child_item->sibling_order();    // ItemsFlat::
////                    //                    _parent_target->sibling_order(_source_item);
////                }

// insert_position = _parent->sibling_order([&](boost::intrusive_ptr<TreeItem> it) {return it->id() == _self->id();});
// qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);
// assert(_parent->_child_items[insert_position] == _self);

// if(_parent->_child_items.contains(_self))result = _self;

////        }
// }

////    else {
////        result = boost::intrusive_ptr<TreeItem>(this);
////    }

// return result;    // insert_position;

// };


////    if(_parent_item && _parent_item != _new_parent) {
////        _parent_item->remove(_child_item);
////    }




////    if(_parent_item && _parent_item != _parent) {
////        _parent_item->remove(_child_item);
////    }

////        auto _child_item_parent = _child_item->parent();  // form ItemsFlat, incomplete

////        if(_child_item_parent && _child_item_parent != _parent_item) {
////            _child_item_parent->remove(_child_item);
////        }


////        if(_parent) {

////        assert(_parent_item->id() != _child_item->_field_data.value("id"));   //        assert(_parent_item->name() != _field_data.value("name"));

//// _parent_item->add_child(boost::intrusive_ptr<TreeItem>(this));



////            //        }

////            //    _parent_item = _new_parent;

////            //            if(_parent) {    //  && _parent_item != it
////            //        _parent_item = it;
////            bool found = false;

////            for(auto i : _parent_item->_child_items) {
////                if(_child_item->id() == i->id()) {found = true; break;}
////            }

////            if(!found) {
////                //            _result = _parent_item->child_move_unique(this
////                //                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
////                //                                                      , _mode
////                //                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);

// _result = child_move_unique(_parent_item, _child_item, _pos, _mode);
//// _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
//// _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
////            }

// if(_parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") {
// _child_item->to_encrypt();
// } else {
// _child_item->to_decrypt();
// }

////        }


////        }

// if(_parent_item && _parent_item->_record_linker != this)_parent_item->_record_linker = this;

// }

////    //    return // _parent_item =
////    //        _parent;
////    //    }();

////    //    [&]()->boost::intrusive_ptr<TreeItem> {
////    _child_item->record_linker(this);
////    //        return _child_item;
////    //    }();  //

////    //    return _result;
// }


boost::intrusive_ptr<Linker> TreeItem::parent(boost::intrusive_ptr<TreeItem> it, int pos, int mode){	// we can use caller temmplate to get caller info
	// boost::intrusive_ptr<TreeItem> _result(nullptr);
	//// Добавление новой записи
	//// Метод только добавляет во внутреннее представление новые данные,
	//// сохраняет текст файла и обновляет данные на экране.
	//// Сохранения дерева XML-данных на диск в этом методе нет.
	//// Допустимые режимы:
	//// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
	//// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
	//// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
	//// Метод принимает "тяжелый" объект записи
	//// Объект для вставки приходит как незашифрованным, так и зашифрованным
	// auto child_move_unique = [&](boost::intrusive_ptr<TreeItem> _parent_target, int pos, int mode)->boost::intrusive_ptr<TreeItem> { // child_insert? does not set parent pointer?
	// boost::intrusive_ptr<TreeItem> result(nullptr);
	// boost::intrusive_ptr<TreeItem> _source_item(this);
	// assert(_source_item != _parent_target);
	// if(_source_item != _parent_target)
	// {
	// int found = 0;
	// for(auto it = _parent_target->_child_items.begin(); it != _parent_target->_child_items.end(); it++) {
	// if(it->get() == _source_item.get()) {
	// found++;
	// if(found == 1)
	// result = *it;
	// else
	// _parent_target->_child_items.erase(it);
	// }
	// }
	////        if(0 == found) {
	//// Запись добавляется в таблицу конечных записей
	// int insert_position = -1;
	//// get shadow_branch
	////    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());
	// if(
	// 0 == found  // !item_direct(_source_item)
	// ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))
	////                // deprecated by _parent_target->remove
	////                auto _origin_parent = _source_item->parent();
	////                if(_origin_parent && _origin_parent != _parent_target) {
	////                    //                    if(_parent != this) {
	////                    _origin_parent->remove(_source_item);
	////                    //                    while(_origin_parent->remove_self_as_empty()) {
	////                    //                        _origin_parent = _origin_parent->parent();
	////                    //                        if(!_origin_parent)break;
	////                    //                    }
	////                    //                    }
	////                }
	//// !!!, this make item move to new branch, remember to remove orignal one
	// _source_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));
	////        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();
	//// Выясняется, есть ли в дереве запись с указанным ID
	//// Если есть, то генерируются новые ID для записи и новая директория хранения
	//// Если нет, то это значит что запись была вырезана, но хранится в буфере,
	//// и ее желательно вставить с прежним ID и прежним именем директории
	//// It turns out, is there a record of a tree with specified ID
	//// If there is, then the generated ID for the new record and the new storage directory
	//// If not, then it means that the recording was cut out, but stored in a buffer
	//// And it is desirable to stick with the same ID and the same name directory
	////        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
	// if(0 == _source_item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
	//// Создается новая запись (ID был пустой) или
	//// Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
	//// Create a new record (ID was empty) or
	//// Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)
	// QString id = get_unical_id();
	//// Store directory entries and files    // Директория хранения записи и файл
	// if(_source_item->field("dir") == "")_source_item->field("dir", id); // get_unical_id()
	// if(_source_item->field("file") == "")_source_item->field("file", "text.html");
	//// Unique ID of XML records             // Уникальный идентификатор XML записи
	////            QString id = get_unical_id();
	// _source_item->field("id", id);
	// } else {
	// if(_source_item->field("dir") == "")_source_item->field("dir", _source_item->field("id")); // get_unical_id()
	// if(_source_item->field("file") == "")_source_item->field("file", "text.html");
	// }
	//// The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
	// if(_source_item->is_lite()) {
	// _source_item->to_fat();
	////            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
	// }
	//// В список переданных полей добавляются вычислимые в данном месте поля
	// if(_source_item->field("ctime") == "") {
	//// Время создания данной записи
	// QDateTime ctime_dt = QDateTime::currentDateTime();
	// QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
	// _source_item->field("ctime", ctime);
	// }
	//// Выясняется в какой ветке вставляется запись - в зашифрованной или нет
	// bool is_crypt = false;
	////        if(_tree_item) {
	////                if(_is_crypt) { //
	// if(_parent_target->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
	// if(globalparameters.crypt_key().length() > 0)
	// is_crypt = true;
	// else
	// critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
	// }
	////        }
	//// Запись полновесных данных с учетом шифрации
	// if(is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) != "1") {       // В зашифрованную ветку незашифрованную запись
	// _source_item->to_encrypt_and_save_fat();
	// } else if(!is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) == "1") { // В незашифрованную ветку зашифрованную запись
	// _source_item->to_decrypt_and_save_fat();
	// } else {
	// _source_item->push_fat_attributes();
	// }
	//// Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
	// _source_item->to_lite();
	////        //        // Запись добавляется в таблицу конечных записей
	////        //        int insertPos = -1;
	////        item->is_registered_to_record_controller_and_tabmanager(true);
	// if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
	// _parent_target->_child_items << _source_item;
	////            insert_position = _child_items.size() - 1;
	// } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
	// _parent_target->_child_items.insert(pos, _source_item);
	////            insert_position = pos;
	// } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
	// _parent_target->_child_items.insert(pos + 1, _source_item);
	////            insert_position = pos + 1;
	// }
	// insert_position =  _source_item->sibling_order();   // ItemsFlat::
	////                    _parent_target->sibling_order(_source_item);
	// qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);
	//// Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода
	// } else {
	// insert_position = _source_item->sibling_order();    // ItemsFlat::
	////                    _parent_target->sibling_order(_source_item);
	// }
	// assert(_parent_target->_child_items[insert_position] == _source_item);
	// if(_parent_target->_child_items.contains(_source_item))result = _source_item;
	////        }
	// }
	////    else {
	////        result = boost::intrusive_ptr<TreeItem>(this);
	////    }
	// return result;    // insert_position;
	// };
	////    if(_parent_item && _parent_item != it) {
	////        _parent_item->remove(this);
	////    }
	// if(_parent_item && _parent_item != it) {
	// _parent_item->remove(this);
	// }
	// _parent_item = it;
	// if(it) {    //  && _parent_item != it
	////        _parent_item = it;
	// bool found = false;
	// for(auto i : _parent_item->_child_items) {
	// if(this->id() == i->id()) {found = true; break;}
	// }
	// if(!found) {
	////            _result = _parent_item->child_move_unique(this
	////                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
	////                                                      , _mode
	////                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
	// _result = child_move_unique(_parent_item
	// , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
	// , _mode
	// ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
	// }
	// }
	// if(!_up_linker || _up_linker->host_parent() != it) {
    if(! _linker){
	_linker = boost::intrusive_ptr<Linker>(new Linker(it, this));							// , pos, mode
	_linker->parent(it, pos, mode);
    }else if(_linker && _linker->host_parent() != it){
	auto parent = _linker->host_parent();
	if(parent && parent != it)parent->release([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == this->id() && il == this->_linker;});
	if(_linker->host() != this)_linker->host(this);								// std::move(boost::intrusive_ptr<TreeItem>(this))
	_linker->parent(it, pos, mode);	// _linker->host_parent()->release(this->linker());
    }
	// }

	// _result = _up_linker->host();


    return _linker;
}
boost::intrusive_ptr<TreeItem> TreeItem::parent() const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    if(_linker)result = _linker->host_parent();
    return result;				// _parent_item;
}
id_value TreeItem::parent_id() const {
    id_value id("");
	// if(_parent_item) {
	// return _parent_item->field("id");
	// } else
	// return "";
    if(_linker)id = _linker->host_parent()->id();
    return id;
}
id_value TreeItem::id() const {
    if(_field_data.contains("id"))return id_value(_field_data["id"]);
    else{
	// critical_error("In TreeItem data getting field with unavailable name 'id'");

	// exit(1);
	return id_value("");
    }
}
QString TreeItem::name() const {
    if(_field_data.contains("name"))return(_field_data["name"]);
    else{
	// critical_error("In TreeItem data getting field with unavailable name 'name'");

	// exit(1);
	return "";
    }
}
#ifdef _with_record_table

boost::intrusive_ptr<TreeItem> TreeItem::add_child(boost::intrusive_ptr<Record> record){
    boost::intrusive_ptr<TreeItem> item;
    int found = 0;
    for(auto it = _child_items.begin(); it != _child_items.end(); it ++){
	if(it->get()->id() == record->field("id")){
	    found ++;
	    if(found == 1)item = *it;
	    else _child_items.erase(it);
	}
    }
    if(0 == found){
	// QMap<QString, QString> data;
	// boost::intrusive_ptr<TreeItem>
	item = boost::intrusive_ptr<TreeItem>(							// std::make_shared<TreeItem>
	    new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)), record)									// shared_from_this()
	    );								// Создается объект item

	//// Определяется, является ли родительская ветка зашифрованной
	// if(this->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
	//// Новая ветка превращается в зашифрованную
	// item->to_encrypt();
	// }

	// bool already_has = false;

	// foreach(boost::intrusive_ptr<TreeItem> it, _child_items) {
	// if(it->id() == item->id()) {
	// already_has = true;
	// _child_items.erase();
	// }
	// }

	// for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
	// if(it->get()->id() == item->id()) {
	// _child_items.erase(it);
	// }
	// }

	// if(!already_has)
	insert_new_item(_child_items.count() - 1, item, ADD_NEW_RECORD_AFTER);							// _child_items << item; // Добавление item в конец массива childItems
    }
    return item;
}
#endif

boost::intrusive_ptr<TreeItem> TreeItem::contains_direct(boost::intrusive_ptr<const TreeItem> &&_item) const {
    return ItemsFlat::contains_direct(std::forward<boost::intrusive_ptr<const TreeItem> >(_item));				// result;


	// boost::intrusive_ptr<TreeItem> result(nullptr);

	// if(_item) {
	// for(auto it : _child_linkers) {
	// if(it
	// && it->host() == _item
	////               && _item->up_linker()->integrity_external(_item, static_cast<const TreeItem *>(this))   // recursive call
	// && it == _item->linker()
	// && _item->_linker->host_parent().get() == this
	// ) {
	////                assert(_item->up_linker()->integrity_external(_item, static_cast<const TreeItem *>(this)));
	// result = it->host();
	// break;
	// }
	// }
	// }

	// return result;
}
boost::intrusive_ptr<TreeItem> TreeItem::contains_direct(boost::intrusive_ptr<const Linker> &&_item_linker) const {
    return ItemsFlat::contains_direct(std::forward<boost::intrusive_ptr<const Linker> >(_item_linker));				//

	// boost::intrusive_ptr<TreeItem> result(nullptr);

	// if(_item_linker) {
	// for(auto it : _child_linkers) {
	// if(it
	// && it == _item_linker
	////               && _item_linker->integrity_external(_item_linker->host(), static_cast<const TreeItem *>(this))  // recursive call
	// && _item_linker->host_parent().get() == this  // _item_linker->host_parent() == this
	// ) {
	////                assert(_item_linker->integrity_external(_item_linker->host(), static_cast<const TreeItem *>(this)));
	// result = it->host();
	// break;
	// }
	// }
	// }

	// return result;
}
boost::intrusive_ptr<const TreeItem> TreeItem::is_ancestor_of(const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal) const {
//    TreeModel::item(const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal) const {
    std::function<boost::intrusive_ptr<const TreeItem>(boost::intrusive_ptr<const TreeItem>, const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &, int)>
    item_recurse	// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem> item, QString name, int mode);
	= [&](boost::intrusive_ptr<const TreeItem> _it
	     , const std::function<bool (boost::intrusive_ptr<const TreeItem>)> &_equal
	     , int mode
	    ){
	    static boost::intrusive_ptr<const TreeItem> find_item;
	    if(mode == 0){
		find_item = nullptr;

		return find_item;	// nullptr;
	    }
	    if(find_item)return find_item;
	    if(_equal(_it)){
		find_item = _it;

		return find_item;
	    }else{
		for(int i = 0; i < _it->count_direct(); i ++)item_recurse(_it->item_direct(i), _equal, 1);
		return find_item;
	    }
	};

	// Инициализация поиска
    item_recurse(this, _equal, 0);	// QUrl()

	// Запуск поиска и возврат результата
    return item_recurse(this, _equal, 1);	// _find_url
//    }

//    return it->path_list().contains(this->id());
}
boost::intrusive_ptr<TreeItem> TreeItem::delete_permanent(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_equal){
    return ItemsFlat::delete_permanent(_equal);
}
boost::intrusive_ptr<TreeItem> TreeItem::delete_permanent_recursive(std::function<bool (boost::intrusive_ptr<const TreeItem>)> condition){
    boost::intrusive_ptr<TreeItem> result(nullptr);
    if(_linker && condition(this)){				// _parent_item
	auto parent = _linker->host_parent();
	if(parent){
	    result = parent->delete_permanent_recursive(_linker, condition);
	    parent->delete_permanent_recursive(condition);
	}
    }
    return result;
}
// boost::intrusive_ptr<TreeItem> TreeItem::delete_permanent_recursive_empty()
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);                         // = false;

// if(_linker && is_empty()) {
// auto parent = _linker->host_parent();

// if(parent) {
// result = delete_permanent_recursive();
// parent->delete_permanent_recursive_empty();
// }
// }

// return result;
// }



boost::intrusive_ptr<TreeItem> TreeItem::delete_permanent_recursive(boost::intrusive_ptr<Linker> _to_be_removed_linker, std::function<bool (boost::intrusive_ptr<const TreeItem>)> condition){
	// if(_source_item->parent() != this)_source_item->self_remove_from_parent();

	// boost::intrusive_ptr<TreeItem> result(nullptr);

	// for(auto it : _child_items) { // for(int row = 0; row < _child_items.size(); ++row) {
	////        auto it = _child_items.at(row);

	// if(it->id() == _source_item->id()) { //_child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
	// _child_items.removeOne(it);
	// it->page_break();
	// it->record_linker().reset();    // it->parent(nullptr);

	// if(it != _source_item)_source_item->merge(it);
	// else
	// result = _source_item;
	// }
	// }

	////    self_remove_from_parent_as_empty();

	// return result;
    return ItemsFlat::delete_permanent_recursive(_to_be_removed_linker, condition);
}
QList<boost::intrusive_ptr<TreeItem> > TreeItem::delete_permanent_recursive(int position, int count){
	////    bool result = false;
	// boost::intrusive_ptr<TreeItem> _result(nullptr);
	////    if(position < 0 || position + count > _child_items.size())
	////        return false;

	// if(position >= 0 && position + count <= _child_items.size()) {
	// for(int row = 0; row < count; ++row) {
	// _result = _child_items.at(position);
	// _result->page_break();
	// _result->up_linker().reset();
	// _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
	// }

	////        result = true;
	// }

	// return _result;  //true;
    return ItemsFlat::delete_permanent_recursive(position, count);
}
void TreeItem::traverse_direct(const std::function< void (boost::intrusive_ptr<Linker>)> &operation){
    return ItemsFlat::traverse(operation);
}
boost::intrusive_ptr<Linker> TreeItem::dangle(){
    auto _parent = _linker->host_parent();
    if(_parent)_parent->release([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == this->id() && il == _linker;});	// delete_permanent([&](boost::intrusive_ptr<const Linker> il){return il->host()->id() == this->id() && il == _linker;});
//	p->_child_linkers.removeOne(_linker);
    if(_linker->host_parent())_linker->host_parent(nullptr);
    return _linker;
}
void TreeItem::clear(){
    ItemsFlat::clear();
}
boost::intrusive_ptr<TreeItem> TreeItem::operator <<(boost::intrusive_ptr<TreeItem> _item){
	// boost::intrusive_ptr<TreeItem> r;
    if(_child_linkers.indexOf(_item->linker()) == - 1){
	_child_linkers << _item->linker();
	// r = _item;
    }
    return this;
}
//// copy introduce duplicated item, does not remove from previous parent
// boost::intrusive_ptr<Linker> TreeItem::child_rent(boost::intrusive_ptr<TreeItem> _item)
// {

// boost::intrusive_ptr<Linker> result(nullptr);

// if(_item != this) {
//// int pos = -1;
// int found = 0;

// for(auto it : _child_linkers) {
// if(it->host() == _item && _item->up_linker()->host_parent().get() == this) {
// found++;

// if(found == 1) {
// result = it;
// } else {
// result->host()->merge(it->host());
// _child_linkers.removeOne(it);
// }
// }
// }

////        if(_item->parent()) {
////            if(_item->parent() != this) {

////                item->parent()->remove_child(item);
////                item->parent(boost::intrusive_ptr<TreeItem>(this));

////                //            _item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));   // no!!!, this make item move to new branch, but does not remove orignal one
////            }
////        }

// if(0 == found) {
//// _child_linkers << _item->up_linker();
//// pos =
// result = _item->parent(this); // get_transfer(_child_items.count() - 1, _item, ADD_NEW_RECORD_AFTER); // _child_items << item; // Добавление item в конец массива childItems
//// result = item(pos);
// }
// }

////    if(static_cast<ItemsFlat *>(result->up_linker()->parent_item().get()) != this) {
////        result->parent(this);
////    }

// return result;

// }


//// deprecated because of conflicting with parent(boost::intrusive_ptr<TreeItem>)
//// Добавление новой записи
//// Метод только добавляет во внутреннее представление новые данные,
//// сохраняет текст файла и обновляет данные на экране.
//// Сохранения дерева XML-данных на диск в этом методе нет.
//// Допустимые режимы:
//// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
//// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
//// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
//// Метод принимает "тяжелый" объект записи
//// Объект для вставки приходит как незашифрованным, так и зашифрованным
// boost::intrusive_ptr<TreeItem> TreeItem::child_move_unique(boost::intrusive_ptr<TreeItem> _source_item, int pos, int mode)  // child_insert? does not set parent pointer?
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);

// assert(_source_item != this);

// if(_source_item != this) {

// int found = 0;

// for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
// if(it->get() == _source_item.get()) {
// found++;

// if(found == 1)
// result = *it;
// else
// _child_items.erase(it);
// }
// }

////        if(0 == found) {
//// Запись добавляется в таблицу конечных записей
// int insert_position = -1;


//// get shadow_branch
////    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

// if(
// 0 == found  // !item_direct(_source_item)
// ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))

//// deprecated by this->child_remove
// auto _parent = _source_item->parent();

// if(_parent && _parent != this) {
////                    if(_parent != this) {
// _parent->remove(_source_item);

// while(_parent->remove_self_as_empty()) {
// _parent = _parent->parent();

// if(!_parent)break;
// }

////                    }
// }

//// !!!, this make item move to new branch, remember to remove orignal one
// _source_item->parent(boost::intrusive_ptr<TreeItem>(this));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));


////        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();



//// Выясняется, есть ли в дереве запись с указанным ID
//// Если есть, то генерируются новые ID для записи и новая директория хранения
//// Если нет, то это значит что запись была вырезана, но хранится в буфере,
//// и ее желательно вставить с прежним ID и прежним именем директории
//// It turns out, is there a record of a tree with specified ID
//// If there is, then the generated ID for the new record and the new storage directory
//// If not, then it means that the recording was cut out, but stored in a buffer
//// And it is desirable to stick with the same ID and the same name directory
////        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

// if(0 == _source_item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
//// Создается новая запись (ID был пустой) или
//// Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
//// Create a new record (ID was empty) or
//// Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

// QString id = get_unical_id();

//// Store directory entries and files    // Директория хранения записи и файл
// if(_source_item->field("dir") == "")_source_item->field("dir", id); // get_unical_id()

// if(_source_item->field("file") == "")_source_item->field("file", "text.html");

//// Unique ID of XML records             // Уникальный идентификатор XML записи
////            QString id = get_unical_id();
// _source_item->field("id", id);
// } else {
// if(_source_item->field("dir") == "")_source_item->field("dir", _source_item->field("id")); // get_unical_id()

// if(_source_item->field("file") == "")_source_item->field("file", "text.html");
// }

//// The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
// if(_source_item->is_lite()) {
// _source_item->to_fat();
////            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
// }

//// В список переданных полей добавляются вычислимые в данном месте поля

//// Время создания данной записи
// QDateTime ctime_dt = QDateTime::currentDateTime();
// QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
// _source_item->field("ctime", ctime);


//// Выясняется в какой ветке вставляется запись - в зашифрованной или нет
// bool is_crypt = false;

////        if(_tree_item) {
////                if(_is_crypt) { //
// if(field(boost::mpl::c_str < crypt_type > ::value) == "1") {
// if(globalparameters.crypt_key().length() > 0)
// is_crypt = true;
// else
// critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
// }

////        }

//// Запись полновесных данных с учетом шифрации
// if(is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) != "1") {       // В зашифрованную ветку незашифрованную запись
// _source_item->to_encrypt_and_save_fat();
// } else if(!is_crypt && _source_item->field(boost::mpl::c_str < crypt_type > ::value) == "1") { // В незашифрованную ветку зашифрованную запись
// _source_item->to_decrypt_and_save_fat();
// } else {
// _source_item->push_fat_attributes();
// }

//// Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
// _source_item->to_lite();

////        //        // Запись добавляется в таблицу конечных записей
////        //        int insertPos = -1;

////        item->is_registered_to_record_controller_and_tabmanager(true);

// if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
// _child_items << _source_item;
////            insert_position = _child_items.size() - 1;
// } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
// _child_items.insert(pos, _source_item);
////            insert_position = pos;
// } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
// _child_items.insert(pos + 1, _source_item);
////            insert_position = pos + 1;
// }

// insert_position = ItemsFlat::sibling_order(_source_item);
// qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

//// Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

// } else {
// insert_position = ItemsFlat::sibling_order(_source_item);
// }

// assert(_child_items[insert_position] == _source_item);
// result = _source_item;


// assert(_child_items.contains(result));
////        }
// }

////    else {
////        result = boost::intrusive_ptr<TreeItem>(this);
////    }

// return result;    // insert_position;

// }



//// Добавление нового подчиненного элемента
//// в конец списка подчиненных элементов
// boost::intrusive_ptr<TreeItem> TreeItem::child_add_new(int pos, QString id, QString name)
// {
////    QMap<QString, QString> data;
// QMap<QString, QString> data;
// data["id"] = id;
// data["name"] = name;
// boost::intrusive_ptr<TreeItem> item(new TreeItem(this, data, QDomElement(), pos));
////    // new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
////    // ); // Создается объект item

////    //    _child_items << item; // Добавление item в конец массива childItems
////    // int pos
////    auto _item = item->parent(this, pos, ADD_NEW_RECORD_AFTER);                      // _child_items.count() - 1

////    // auto _item = child(pos);
// return item;
// }


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
QList<boost::intrusive_ptr<TreeItem> >  TreeItem::children_insert_new(int position, int count, int columns){
    Q_UNUSED(columns);
//    bool result = false;
	// if(position < 0 || position > _child_linkers.size())
	// return false;
    QList<boost::intrusive_ptr<TreeItem> > result_list;
    if(position >= 0 && position < _child_linkers.size()){
	for(int row = 0; row < count; ++ row){
	    QMap<QString, QString> data;
	    data["id"] = get_unical_id();
	    boost::intrusive_ptr<TreeItem> item(new TreeItem(this, data));

	    assert(item->linker()->host_parent().get() == this);
		//// new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
		//// );    // Создается объект item
		// item->parent(this, position, ADD_NEW_RECORD_AFTER); //_child_items.insert(position, item);        // Вставка item в нужную позицию массива childItems
//            result = true;	// not a precise solution
	    result_list << item;
	}
    }
    return result_list;	// result;
}
boost::intrusive_ptr<TreeItem> TreeItem::sibling() const {
    boost::intrusive_ptr<TreeItem> r(nullptr);
    auto p = parent();
    if(p){
	auto sibling_order_this = p->sibling_order([&](boost::intrusive_ptr<const Linker> il){return il == this->linker() && il->host() == this && il->host_parent() == p;});
	if(sibling_order_this != 0 && sibling_order_this != - 1){
	    r = p->item_direct(sibling_order_this - 1);
	}
    }
    return r;
}
boost::intrusive_ptr<TreeItem> TreeItem::merge(boost::intrusive_ptr<TreeItem> cut){
	// typedef QPair<QString, QString> pair;
    QMap<QString, QString> r = cut->fields_all();

	// foreach(pair i, r) {
    int field_operation_count = 0;
    for(auto i : r.keys()){
	if(_field_data[i] == ""){
	    _field_data[i] = r[i];	// field(i, r[i]);
	    field_operation_count ++;
	}
    }
	// if(0 != field_operation_count) {

    QString text = cut->is_lite() ? cut->text_from_lite_direct() : cut->text_from_fat();
    if(is_lite()){
	if(! text_from_lite_direct().contains(text))text_to_direct(text_from_lite_direct() + text);
    }else{
	if(! text_from_fat().contains(text))text_to_fat(text_from_fat() + text);
    }
    auto attach = cut->attach_table();
    auto this_attach = attach_table();
    if(! attach->is_empty()){
	auto new_attach = this_attach->merge(attach);
	attach_table(new_attach);
    }
    if(! cut->is_lite() && ! is_lite()){
	auto pic = cut->picture_files();
	auto this_pic = picture_files();
	if(pic.size() > 0){
	    for(auto i : pic.keys()){										// auto new_pic = this_pic.merge(pic);
		if(this_pic[i].isEmpty())this_pic[i] = pic[i];
	    }
	    picture_files(this_pic);
	}
    }
    int merge_count = 0;
    int linkers_count = cut->count_direct();
    int origin_child_linkers_size = _child_linkers.size();
    int new_count = 0;
    for(auto cut_child_linker : cut->_child_linkers){
	// if(!_child_items.contains(i)) {
	bool found = false;
	boost::intrusive_ptr<TreeItem> found_item_keep(nullptr);
	boost::intrusive_ptr<TreeItem> found_item_remove(nullptr);
	for(auto keep_child : _child_linkers){
	    if(keep_child->host()->id() == cut_child_linker->host()->id()){
		found_item_keep = keep_child->host();
		found_item_remove = cut_child_linker->host();
		found = true;

		break;
	    }
	}
	if(! found){
	    auto r = cut_child_linker->parent(this);										// _child_items << i;
	    if(r){merge_count ++;new_count ++;}
	}else{
	    auto r = found_item_keep->merge(found_item_remove);
	    if(r)merge_count ++;
	}
//	for(auto j : _child_items){
//	    if(i->id() == j->id()){
//		j->merge(i);
//		count ++;
//	    }else{}
//	}
	assert(merge_count <= linkers_count);							// ?
	// }
    }
    if(! _linker){
	if(cut->linker()){
	    _linker = boost::intrusive_ptr<Linker>(new Linker(cut->linker()->host_parent(), boost::intrusive_ptr<TreeItem>(this)));										// cut->linker()

	    _linker->parent(cut->linker()->host_parent(), 0);
	}
    }
    auto try_cut_page_rebind = [&](boost::intrusive_ptr<TreeItem> _this, boost::intrusive_ptr<TreeItem> cut){
	    if(cut->binder()){
		if(cut->binder()->page() && cut->parent()){
		    cut->binder()->page()->item_bind(_this);
		}
	    }
	};
    if(! _binder){
	try_cut_page_rebind(this, cut);
    }else if(! _binder->page()){
	try_cut_page_rebind(this, cut);
    }
    assert(_child_linkers.size() == origin_child_linkers_size + new_count);
    cut->clear();

	// if(cut->parent()) cut->delete_permanent();  // need udate model::beginRemoveRows & endRemoveRows

    assert(cut->count_direct() == 0);
	// }

    return boost::intrusive_ptr<TreeItem>(this);
}
// int TreeItem::shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> it, int mode)
// {
//// Запись добавляется в таблицу конечных записей
// int insert_position = -1;

// it->parent(boost::intrusive_ptr<TreeItem>(this));

////    KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
// assert(it->is_lite());

// if(!find_direct(it)    //            && !dataModel->isRecordIdExists(record->getField("id"))
// ) {

////        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();

////        // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
////        if(record->isLite() == true)
////            critical_error("RecordTable::insert_new_record() can't insert lite record");

//// Выясняется, есть ли в дереве запись с указанным ID
//// Если есть, то генерируются новые ID для записи и новая директория хранения
//// Если нет, то это значит что запись была вырезана, но хранится в буфере,
//// и ее желательно вставить с прежним ID и прежним именем директории
//// It turns out, is there a record of a tree with specified ID
//// If there is, then the generated ID for the new record and the new storage directory
//// If not, then it means that the recording was cut out, but stored in a buffer
//// And it is desirable to stick with the same ID and the same name directory
////        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());


// assert(0 != it->field("id").length());

////        if(0 == record->getField("id").length()
////           //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
////          ) {
////            // Создается новая запись (ID был пустой) или
////            // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
////            // Create a new record (ID was empty) or
////            // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

////            QString id = get_unical_id();
////            // Store directory entries and files    // Директория хранения записи и файл
////            record->setField("dir", id);   // get_unical_id()

////            record->setField("file", "text.html");

////            // Unique ID of XML records             // Уникальный идентификатор XML записи
////            //            QString id = get_unical_id();
////            record->setField("id", id);
////        }


//// В список переданных полей добавляются вычислимые в данном месте поля

////        // Время создания данной записи
////        QDateTime ctime_dt = QDateTime::currentDateTime();
////        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
////        record->field("ctime", ctime);

// assert(it->field("ctime") != "");

//// Выясняется в какой ветке вставляется запись - в зашифрованной или нет
// bool is_crypt = false;

////        if(_tree_item) {
// if(_is_crypt) { //            if(_tree_item->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
// if(globalparameters.crypt_key().length() > 0)
// is_crypt = true;
// else
// critical_error("ItemsFlat::shadow_item_lite() : Can not insert data to crypt branch. Password not setted.");
// }

////        }

//// Запись полновесных данных с учетом шифрации
// if(is_crypt && it->field(boost::mpl::c_str < crypt_type > ::value) != "1") {       // В зашифрованную ветку незашифрованную запись
// it->to_encrypt_and_save_lite();
// } else if(!is_crypt && it->field(boost::mpl::c_str < crypt_type > ::value) == "1") { // В незашифрованную ветку зашифрованную запись
// it->to_decrypt_and_save_lite();
// } else {
// it->push_lite_attributes();
// }

////        // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
////        record->switchToLite();

////        // Запись добавляется в таблицу конечных записей
////        int insertPos = -1;

////        record->is_registered(true);
//// assert(record->is_registered_to_browser());

// if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
// _child_items << it;
// insert_position = _child_items.size() - 1;
// } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
// _child_items.insert(pos, it);
// insert_position = pos;
// } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
// _child_items.insert(pos + 1, it);
// insert_position = pos + 1;
// }

// qDebug() << "ItemsFlat::shadow_item_lite() : New record pos" << QString::number(insert_position);

//// Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

// }

// return insert_position;
// }




// bool TreeItem::children_remove_link(int position, int count)
// {
// if(position < 0 || position + count > _child_items.size())
// return false;

//// Ссылка на удаленный элемент убирается из списка подчиненных элементов
// for(int row = 0; row < count; ++row) {
// _child_items.removeAt(position);
// }

// return true;
// }


int TreeItem::move_up(void){
    int row = 0;
	//// Выясняется номер данного элемента в списке родителя
	// int num = sibling_order();
	// if(num > 0) {   // if(num == 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
	// row = -1;
	//// Элемент перемещается вверх по списку
	// (_parent_item->_child_items).swap(num, num + row);
	// }
    if(_linker)row = _linker->move_up();
    return row;
}
int TreeItem::move_dn(void){
    int row = 0;
	//// Выясняется номер данного элемента в списке родителя
	// int num = sibling_order();
	//// Если двигать вниз некуда, ничего делать ненужно
	// if(num < (_parent_item->count_direct())) { //if(num >= (_parent_item->count_direct()))return false;
	// row = 1;
	//// Элемент перемещается вниз по списку
	// (_parent_item->_child_items).swap(num, num + row);
	// }
    if(_linker)row = _linker->move_dn();
    return row;
}
//// Путь к элементу (список идентификаторов от корня до текущего элемента)
// QStringList TreeItem::path_absolute(void)const
// {
// return path_absolute_as_field("id");
// }


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::path_list(QString field_name) const {
	// auto path_absolute_as_field = [&](QString field_name)->QStringList { // const
    QStringList path;
    boost::intrusive_ptr<const TreeItem> current_item(this);				// = boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()


    path << current_item->_field_data[field_name];
    while(current_item->parent()){				// != nullptr
	current_item = current_item->parent();
	path << current_item->_field_data[field_name];
    }
	// Rotate backwards array identifiers advance   // Поворот массива идентификаторов задом наперед
    int k = path.size() - 1;
    int j = path.size() / 2;
    for(int i = 0; i < j; ++ i)path.swap(i, k - i);
    return path;
	// };
	// return path_absolute_as_field(field_name);  // "id", "name"
}
// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::path_string(QString field_name, QString delimeter) const {
    QStringList path = path_list(field_name);				// "name"

	// Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    int emptyStringIndex = path.indexOf("");
    path.removeAt(emptyStringIndex);

    return path.join(delimeter);
}
//// Возвращает массив путей всех подветок, которые содержит ветка
// QList<QStringList> TreeItem::path_children_all(void)const
// {
//// Очищение списка путей
// path_children_all_as_field_impl(this, "", 0);    // const_cast<TreeItem *>(this)  // shared_from_this()

//// Получение списка путей
// QList<QStringList> path_list = path_children_all_as_field_impl(this, "id", 1);    // boost::intrusive_ptr<const TreeItem>(this)  // const_cast<TreeItem *>(this)   // shared_from_this()

// return path_list;
// }


QList<QStringList> TreeItem::path_children_all(QString field_name) const {
	// std::function<QList<QStringList>(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)> all_children_path_as_field =
	// [&](boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)
	//// Возвращает массив указанных полей всех подветок, которые содержит ветка
	//// Внутренняя рекурсивная функция
	////            QList<QStringList> TreeItem::()
	// {
	// static QList<QStringList> pathList;

	//// Если дана команда очистить список путей
	// if(mode == 0) {
	// pathList.clear();
	// return QList<QStringList>();
	// }

	// for(int i = 0; i < (item->child_count()); i++) {
	// QStringList path = (item->child(i))->path_as_field(fieldName);
	// pathList << path;
	// all_children_path_as_field(item->child(i), fieldName, 2);
	// }

	// if(mode == 1)return pathList;
	// else return QList<QStringList>();
	// };



	// Возвращает массив указанных полей всех подветок, которые содержит ветка
	// Внутренняя рекурсивная функция
    std::function<QList<QStringList> (boost::intrusive_ptr<const TreeItem> item, QString field_name, int mode)> path_children_all_as_field_impl
	= [&](boost::intrusive_ptr<const TreeItem> item, QString field_name, int mode) -> QList<QStringList>  {				// const
	    static QList<QStringList> path_list;
		// Если дана команда очистить список путей
	    if(mode == 0){
		path_list.clear();

		return QList<QStringList>();
	    }
	    for(int i = 0; i < item->count_direct(); i ++){
		auto it = item->item_direct(i);
		QStringList path = it->path_list(field_name);
		path_list << path;
		path_children_all_as_field_impl(it, field_name, 1);													// 2?
	    }
	    if(mode == 1)return path_list;
	    else return QList<QStringList>();
	};



	// Очищение списка путей
    path_children_all_as_field_impl(this, "", 0);

	// Получение списка путей
    QList<QStringList> pathList = path_children_all_as_field_impl(this, field_name, 1);

    return pathList;
}
//// Возвращает массив путей всех подветок, которые содержит ветка
//// Внутренняя рекурсивная функция
// QList<QStringList> TreeItem::get_all_children_path_recurse(TreeItem *item, int mode)
// {
// static QList<QStringList> pathList;

//// Если дана команда очистить список путей
// if(mode == 0) {
// pathList.clear();
// return QList<QStringList>();
// }

// for(int i = 0; i < (item->childCount()); i++) {
// QStringList path = (item->child(i))->get_path();
// pathList << path;
// get_all_children_path_recurse(item->child(i), 2);
// }

// if(mode == 1)return pathList;
// else return QList<QStringList>();
// }




// Переключение ветки и всех подветок в зашифрованное состояние
void TreeItem::to_encrypt(void){
    qDebug()	<< "TreeItem::to_encrypt() : Crypt branch"
		<< _field_data["name"]
		<< QString("id : ")
		<< _field_data["id"]
    ;
	// Если ветка оказалось заашифрованной ее нельзя зашифровывать второй раз
    if(_field_data[boost::mpl::c_str < crypt_type > ::value] == "1")return;
	// Устанавливается поле, что ветка зашифрована
    _field_data[boost::mpl::c_str < crypt_type > ::value] = "1";

	// Шифруется имя ветки
    _field_data["name"] = CryptService::encryptString(globalparameters.crypt_key(), _field_data["name"]);


	// Шифрация конечных записей для этой ветки
	// _record_table->
    ItemsFlat::to_encrypt();
	// Шифрация подветок
    for(int i = 0; i < count_direct(); i ++)item_direct(i)->to_encrypt();
    if(is_lite())Record::to_encrypt_and_save_lite();
    else Record::to_encrypt_and_save_fat();
}
// Переключение ветки и всех подветок в расшифрованное состояние
void TreeItem::to_decrypt(void){
    qDebug()	<< "TreeItem::to_decrypt() : Decrypt branch"
		<<				// field("name")   //
	_field_data["name"]
		<< QString("id : ")
		<<				// field("id")     //
	_field_data["id"]
    ;
	// Если ветка оказалось незашифрованной, нечего расшифровывать
    if(_field_data[boost::mpl::c_str < crypt_type > ::value].length() == 0 ||
	_field_data[boost::mpl::c_str < crypt_type > ::value] == "0")return;
	// Устанавливается поле, что ветка не зашифрована
    _field_data[boost::mpl::c_str < crypt_type > ::value] = "0";

	// асшифровка имени ветки
    _field_data["name"] = CryptService::decryptString(globalparameters.crypt_key(), _field_data["name"]);


	// Дешифрация конечных записей для этой ветки
	// _record_table->
    ItemsFlat::to_decrypt();
	// Дешифрация подветок
    for(int i = 0; i < count_direct(); i ++){
	item_direct(i)->to_decrypt();
    }
    if(is_lite())Record::to_decrypt_and_save_lite();
    else Record::to_decrypt_and_save_fat();
}
// int TreeItem::size(void)
// {
// return _child_items.size(); // _record_table->size();
// }


// void TreeItem::dom_to_direct(const QDomElement &_dom_element)
// {
// QDomElement dom_record;

// QDomElement dom_records;

// if(_up_linker) {
// assert(_up_linker->host().get() == this);
// assert(_up_linker->host_parent().get() != this);
// }

////    QString content = _dom_element.text();

////    if(_dom_element.tagName() == "node") {
////        record = _dom_element.firstChildElement("record");
////        records = _dom_element.firstChildElement("record").firstChildElement("recordtable");
////    } else

// if(_dom_element.tagName() == "record") {
// dom_record = _dom_element;
// dom_records = _dom_element.firstChildElement("recordtable");
// } else {    // _dom_element.tagName() == "recordtable"
// assert(_dom_element.tagName() == "recordtable");
// dom_records = _dom_element;
// }

// if(!dom_record.isNull() && dom_record.tagName() == "record") {
// if(_up_linker) {
// assert(_up_linker->host().get() == this);
// assert(_up_linker->host_parent().get() != this);
// }

// Record::dom_to_record(_dom_element);

// if(_up_linker) {
// assert(_up_linker->host().get() == this);
// assert(_up_linker->host_parent().get() != this);
// }
// }

// if(!dom_records.isNull() && dom_records.tagName() == "recordtable") {   // assert(records.tagName() == "recordtable");
// if(_up_linker) {
// assert(_up_linker->host().get() == this);
// assert(_up_linker->host_parent().get() != this);
// }

// ItemsFlat::dom_to_records(dom_records  // dom_model.firstChildElement("recordtable")
//// , boost::intrusive_ptr<TreeItem>(this)    // _parent_item
// );

// if(_up_linker) {
// assert(_up_linker->host().get() == this);
// assert(_up_linker->host_parent().get() != this);
// }
// }

////    // QDomElement n = dommodel.documentElement();
////    // QDomElement n = dommodel;

////    // qDebug() << "In recordtabledata setup_data_from_dom() start";

////    // Если принятый элемент не является таблицей
////    if(dom_model.tagName() != "recordtable")
////        return;

////    // Определяется указатель на первый элемент с записью
////    // Define a pointer to the first element of the recording
////    QDomElement current_record_dom = dom_model.firstChildElement("record");

////    while(!current_record_dom.isNull()) {
////        QMap<QString, QString> data;
////        // Структура, куда будет помещена текущая запись
////        // The structure, which will put the current record
////        boost::intrusive_ptr<TreeItem> current_item = boost::intrusive_ptr<TreeItem>(
////                                                          new TreeItem(data
////                                                                  , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)) // boost::intrusive_ptr<TreeItem>(reinterpret_cast<TreeItem *>(const_cast<ItemsFlat *>(this)))  // _parent_item
////                                                                      )
////                                                      );

////        current_item->is_registered_to_record_controller(true);

////        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
////        // The current record is added to the final table of records (and located at a certain address in memory)
////        _child_items << current_item;

////        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
////        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
////        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
////        // Order in subordinate objects have registered a valid pointer to this entry
////        (_child_items.last())->import_local_from_dom(current_record_dom);

////        current_record_dom = current_record_dom.nextSiblingElement("record");
////    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



////    return;

// }






void TreeItem::dom_to_records(const QDomElement &_record_dom_element){
    auto dom_to_direct
	= [&](const QDomElement &_dom_element) -> void {
	    QDomElement dom_record;

	    QDomElement dom_records;
	    if(_linker){
		assert(_linker->host().get() == this);
		assert(_linker->host_parent().get() != this);
	    }
		// QString content = _dom_element.text();
		// if(_dom_element.tagName() == "node") {
		// record = _dom_element.firstChildElement("record");
		// records = _dom_element.firstChildElement("record").firstChildElement("recordtable");
		// } else
	    if(_dom_element.tagName() == "record"){
		dom_record = _dom_element;
		dom_records = _dom_element.firstChildElement("recordtable");
	    }else{																				// _dom_element.tagName() == "recordtable"
		assert(_dom_element.tagName() == "recordtable");
		dom_records = _dom_element;
	    }
	    if(! dom_record.isNull() && dom_record.tagName() == "record"){
		if(_linker){
		    assert(_linker->host().get() == this);
		    assert(_linker->host_parent().get() != this);
		}
		Record::dom_to_record(_dom_element);
		if(_linker){
		    assert(_linker->host().get() == this);
		    assert(_linker->host_parent().get() != this);
		}
	    }
	    if(! dom_records.isNull() && dom_records.tagName() == "recordtable"){																				// assert(records.tagName() == "recordtable");
		if(_linker){
		    assert(_linker->host().get() == this);
		    assert(_linker->host_parent().get() != this);
		}
		// ItemsFlat::
		dom_to_itemsflat(dom_records);																							// dom_model.firstChildElement("recordtable")
		// , boost::intrusive_ptr<TreeItem>(this)    // _parent_item
		if(_linker){
		    assert(_linker->host().get() == this);
		    assert(_linker->host_parent().get() != this);
		}
	    }
		//// QDomElement n = dommodel.documentElement();
		//// QDomElement n = dommodel;

		//// qDebug() << "In recordtabledata setup_data_from_dom() start";

		//// Если принятый элемент не является таблицей
		// if(dom_model.tagName() != "recordtable")
		// return;

		//// Определяется указатель на первый элемент с записью
		//// Define a pointer to the first element of the recording
		// QDomElement current_record_dom = dom_model.firstChildElement("record");

		// while(!current_record_dom.isNull()) {
		// QMap<QString, QString> data;
		//// Структура, куда будет помещена текущая запись
		//// The structure, which will put the current record
		// boost::intrusive_ptr<TreeItem> current_item = boost::intrusive_ptr<TreeItem>(
		// new TreeItem(data
		// , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)) // boost::intrusive_ptr<TreeItem>(reinterpret_cast<TreeItem *>(const_cast<ItemsFlat *>(this)))  // _parent_item
		// )
		// );

		// current_item->is_registered_to_record_controller(true);

		//// Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
		//// The current record is added to the final table of records (and located at a certain address in memory)
		// _child_items << current_item;

		//// Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
		//// чтобы в подчиненных объектах прописались правильные указатели на данную запись
		//// Write initialized data. It should initsializirovatsya after placement in the list tableData,
		//// Order in subordinate objects have registered a valid pointer to this entry
		// (_child_items.last())->import_local_from_dom(current_record_dom);

		// current_record_dom = current_record_dom.nextSiblingElement("record");
		// } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



		// return;
	};

    assert(this->linker()->host().get() == this);
    assert(this->linker()->host_parent().get() != this);
	// move to void ItemsFlat::dom_to_records
	// auto // std::function<void(const QDomElement &, boost::intrusive_ptr<TreeItem>)>
	// assembly_record_and_table_to_parent = [&](const QDomElement & _dom_record, boost::intrusive_ptr<TreeItem> current_parent)->void {
	//// Определяются атрибуты узла дерева разделов
	// QDomNamedNodeMap attribute_map = _dom_record.attributes();
	////        // Перебираются атрибуты узла дерева разделов
	////        for(int i = 0; i < attribute_map.count(); ++i) {
	////            QDomNode attribute = attribute_map.item(i);
	////            QString name = attribute.nodeName();
	////            QString value = attribute.nodeValue();
	////            if(name == "id") {
	////                id = value;
	////                break;
	////            }
	////            //                        // В дерево разделов устанавливаются считанные атрибуты
	////            //                        // parent->child(parent->current_count() - 1)
	////            //                        parent->field(name , value);
	////            //                        //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
	////        }
	// QString id = attribute_map.namedItem("id").nodeValue();
	// assert(id != "");
	// int index = current_parent->sibling_order([&](boost::intrusive_ptr<const Linker> it) {return it->host()->id() == id;}); // int index = current_parent->item_direct(id)->sibling_order();
	// boost::intrusive_ptr<TreeItem> child_item(nullptr);
	// if(index != -1)
	// {
	// child_item = current_parent->item_direct(index)->host();
	// } else {
	// child_item = current_parent->child_add_new(current_parent->count_direct(), id, "");
	////            child_item->field("id", id);
	// }
	// child_item->dom_to_records(_dom_record);   // item->self_equipment_from_dom(record);
	////        QDomElement recordtable = record.firstChildElement();
	////        assert(recordtable.tagName() == "recordtable" || recordtable.isNull());
	////        if(recordtable.tagName() == "recordtable") {
	////            //                    auto record = recordtable.firstChildElement();
	////            //                    assert(record.tagName() == "record" || record.isNull());
	////            //                    if(record.tagName() == "record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
	////            model_from_dom(recordtable, item);
	////        }
	// };
	// boost::intrusive_ptr<TreeItem> parent = iParent;
    if(! _record_dom_element.isNull()){
	assert(_record_dom_element.tagName() != "recordtable");
	assert(_record_dom_element.tagName() == "record");

	assert(this->linker()->host().get() == this);
	assert(this->linker()->host_parent().get() != this);
	// У данного Dom-элемента ищется таблица конечных записей
	// и данные заполняются в Item-таблицу конечных записей
	// At this Dom-end table element is searched for records and the data filled in the Item-end table entries
	dom_to_direct(_record_dom_element);							// take ground from the nearest level children


	assert(this->linker()->host().get() == this);
	assert(this->linker()->host_parent().get() != this);

	// QDomElement _dom_record_table = _record_dom_element.firstChildElement();

	// assert(_dom_record_table.tagName() == "recordtable" || _dom_record_table.isNull());

	// if(_dom_record_table.tagName() == "recordtable") {

	// QDomElement _dom_record = _dom_record_table.firstChildElement();
	// assert(_dom_record.tagName() == "record");


	// assert(this->up_linker()->host().get() == this);
	// assert(this->up_linker()->parent_item().get() != this);

	//// Пробегаются все DOM элементы текущего уровня
	//// и рекурсивно вызывается обработка подуровней
	// while(!_dom_record.isNull()) { // traverse brothers



	////            if(_dom_element.tagName() == "node") { // "node"
	////                assert(_dom_element.firstChildElement().tagName() == "record");
	////                QDomElement record = _dom_element.firstChildElement("record");

	////                if(!record.isNull()) {

	////                    //                    //                // Обнаруженый подузел прикрепляется к текущему элементу
	////                    //                    //                auto item = parent->add_child();  // insert_children(parent->current_count(), 1, 1);

	////                    //                    //                if(!_dom_element.firstChildElement("record").isNull())
	////                    //                    //                    boost::static_pointer_cast<Record>(item)->record_from_dom(_dom_element.firstChildElement("record"));

	////                    //                    //                //            QString line1, line_name, line_id;
	////                    //                    //                //            line1 = n.tagName();
	////                    //                    //                //            line_name = n.attribute("name");
	////                    //                    //                //            line_id = n.attribute("id");
	////                    //                    //                //            qDebug() << "Read node " << line1 << " " << line_id << " " << line_name;

	////                    //                    QString id = "";

	////                    //                    // Определяются атрибуты узла дерева разделов
	////                    //                    QDomNamedNodeMap attributeMap = record.attributes();

	////                    //                    // Перебираются атрибуты узла дерева разделов
	////                    //                    for(int i = 0; i < attributeMap.count(); ++i) {
	////                    //                        QDomNode attribute = attributeMap.item(i);

	////                    //                        QString name = attribute.nodeName();
	////                    //                        QString value = attribute.nodeValue();

	////                    //                        if(name == "id") {
	////                    //                            id = value;
	////                    //                            break;
	////                    //                        }

	////                    //                        //                    // В дерево разделов устанавливаются считанные атрибуты
	////                    //                        //                    // parent->child(parent->current_count() - 1)
	////                    //                        //                    item->field(name , value);
	////                    //                        //                    //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
	////                    //                    }

	////                    //                    assert(id != "");
	////                    //                    int index = parent->is_item_exists(id);

	////                    //                    boost::intrusive_ptr<TreeItem> item = nullptr;

	////                    //                    if(index != -1) {
	////                    //                        item = parent->child(index);

	////                    //                        item->record_from_dom(record);
	////                    //                    } else {
	////                    //                        item = parent->add_child();
	////                    //                        item->record_from_dom(record);
	////                    //                    }

	////                    //                    // Вызов перебора оставшегося DOM дерева с прикреплением обнаруженных объектов
	////                    //                    // к только что созданному элементу
	////                    //                    model_from_dom(record.firstChildElement(), item);    //
	////                    record_and_recordtable(record);
	////                }

	////            } else

	// if(_dom_record.tagName() == "record") {

	////                if(!_dom_element.isNull()) {
	////                    // boost::static_pointer_cast<Record>(parent)->record_from_dom(_dom_element);
	////                    //                    boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), parent));
	////                    //                    item->record_from_dom(_dom_element);
	////                    //                    parent->find(item->id())->record_from_dom(_dom_element);

	////                    QString id = "";
	////                    // Определяются атрибуты узла дерева разделов
	////                    QDomNamedNodeMap attributeMap = _dom_element.attributes();

	////                    // Перебираются атрибуты узла дерева разделов
	////                    for(int i = 0; i < attributeMap.count(); ++i) {
	////                        QDomNode attribute = attributeMap.item(i);

	////                        QString name = attribute.nodeName();
	////                        QString value = attribute.nodeValue();

	////                        if(name == "id") {
	////                            id = value;
	////                            break;
	////                        }

	////                        //                        // В дерево разделов устанавливаются считанные атрибуты
	////                        //                        // parent->child(parent->current_count() - 1)
	////                        //                        parent->field(name , value);
	////                        //                        //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
	////                    }

	////                    assert(id != "");
	////                    int index = parent->is_item_exists(id);
	////                    boost::intrusive_ptr<TreeItem> item = nullptr;

	////                    if(index != -1) {
	////                        item = parent->child(index);
	////                        item->record_from_dom(_dom_element);
	////                    } else {
	////                        item = parent->add_child();
	////                        item->record_from_dom(_dom_element);
	////                    }

	////                    QDomElement recordtable = _dom_element.firstChildElement();
	////                    assert(recordtable.tagName() == "recordtable" || recordtable.isNull());


	////                    if(recordtable.tagName() == "recordtable") {
	////                        //                    auto record = recordtable.firstChildElement();
	////                        //                    assert(record.tagName() == "record" || record.isNull());

	////                        //                    if(record.tagName() == "record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
	////                        model_from_dom(recordtable, item);
	////                    }

	//// model_from_dom(_child, self); //
	// assert(this->up_linker()->host().get() == this);
	// assert(this->up_linker()->parent_item().get() != this);
	// assembly_record_and_table_to_parent(_dom_record, this);
	////            }

	// }

	////                else {

	////                    assert(_child.tagName() == "recordtable");

	////                    //                if(!_dom_element.isNull())static_cast<ItemsFlat *>(parent.get())->items_from_dom(_dom_element, parent->parent());

	////                    QDomElement record = _child.firstChildElement();
	////                    assert(record.tagName() == "record" || record.isNull());

	////                    if(record.tagName() == "record") {
	////                        assembly_record_and_table_to_parent(record, self);  // model_from_dom(record, parent);    // get recordtable == ItemsFlat
	////                    }
	////                }

	// _dom_record = _dom_record.nextSiblingElement();   // brother record
	// assert(_dom_record.tagName() == "record" || _dom_record.isNull());
	// }
	// }
    }
}
// bypass record::export_to_dom
QDomElement TreeItem::dom_from_treeitem(){
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
	// QDomElement node = QDomDocument().createElement("node");
    QDomElement record = Record::dom_from_record_impl(doc);
    if(ItemsFlat::count_direct() > 0){
	QDomElement children = ItemsFlat::dom_from_itemsflat_impl(doc);
	record.appendChild(children);
    }
	// node.appendChild(record);
    return record;				// node;    //
}
// QDomElement TreeItem::dom_from_treeitem(std::shared_ptr<QDomDocument> doc)
// {
////    QDomElement node = doc->createElement("node");
// QDomElement record = Record::dom_from_record(doc);

// if(ItemsFlat::count_direct() > 0) {
// QDomElement children = ItemsFlat::dom_from_itemsflat(doc);
// record.appendChild(children);
// }

////    node.appendChild(record);
// return // node;    //
// record;
// }


// void TreeItem::clear_tabledata(void)
// {
// _record_table->delete_all_records();
// }


// std::shared_ptr<RecordTable> TreeItem::record_table(void)
// {
// return _record_table;
// }

// void TreeItem::record_table(std::shared_ptr<RecordTable> _table_data)
// {
// _record_table = _table_data;
// }

// void TreeItem::page_to_nullptr()
// {
////    QSet<Record *> binded_records = _page->binded_records();

////    for(auto i : binded_records) {
////        if(i == this) {
////            i->_page = nullptr;    // _page->break_record();  // _page->bind_record(nullptr);
////        }
////    }

// _page = nullptr;
// _page_valid = false;
// }


boost::intrusive_ptr<TreeItem> TreeItem::item() const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    if(_binder){
	result = _binder->host();
    }
    return result;
}
browser::WebPage *TreeItem::page() const {
    browser::WebPage *page = nullptr;
    if(_binder){
	page = _binder->page();
    }
	////    if(_page) {
	////        if(_page->record() == this)
	////            page = _page;
	////        else
	////            page = _page->record()->_page;
	////    }

	// page = _page;

	// while(page && page->_record && page->_record != this) {
	//// if(page->binded_record())
	// page = page->_record->_page;
	// }

	// return page;

    return page;
}
bool TreeItem::is_holder() const {
    bool is_holder_ = false;
    if(_binder->page())is_holder_ = (_binder->host().get() == this);	// _page
    return is_holder_;
}
// void TreeItem::binder(TreeItem::bind_helper g) {_binder = g;}

// TreeItem::bind_helper TreeItem::binder() const {return _binder;}

// void TreeItem::activator(TreeItem::activate_helper a) {_activator = a;}

// TreeItem::activate_helper TreeItem::activator() const {return _activator;}

// boost::intrusive_ptr<TreeItem> TreeItem::bind(browser::WebPage *page)
// {
// assert(page);

// if(_record_binder->bounded_page() != page) {

// if(_record_binder->bounded_page()) {   // _page

// auto _item = _record_binder->bounded_item();

// if(_item) {
// _record_binder->bounded_page()->break_item(_item);
// _item->page_to_nullptr();
// }

// _page_valid = false;
// }

// _page = page;
// }

// if(!_record_binder->bounded_item() || _record_binder->bounded_item().get() != this) {
// _record_binder->bounded_page()->bind(boost::intrusive_ptr<TreeItem>(this));
// }

// _page_valid = true;
// return this;
// }

browser::WebView *TreeItem::bind(){
    assert(_binder);
    browser::WebView *view = nullptr;
    if(_binder){
	if(	// !_record_binder
	    ! _binder->page()
	  || ! _binder->host()
	  || (_binder->host() && _binder->host().get() != this)
	  || (_binder->page() && _binder->page()->binder() != _binder)
	    ){
	    view = _binder->bind();	// boost::intrusive_ptr<TreeItem>(this)
	}else{
	    view = _binder->page()->view();
	}
    }
    assert(view);

    return view;
}
browser::WebView *TreeItem::activate(const std::function<browser::WebView *(const std::function<bool (boost::intrusive_ptr<const ::Binder>)> &_equal)> &find_activated){
    assert(_binder);				// auto result = globalparameters.entrance()->item_bind(this);  // may be not registered to tree model
    browser::WebView *v = nullptr;
    auto check_view = find_activated([&](boost::intrusive_ptr<const ::Binder> b) -> bool {return b->host()->id() == id();});
    if(check_view){
	if(this != check_view->page()->item() || ! _binder){
	    bind();
	    v = _binder->activate();
	}else if(! check_view->load_finished()){
	    v = _binder->activate();
	}else{
	    v = _binder->page()->view();
	}
    }else{
	bind();
	v = _binder->activate();
    }
    assert(v);
    v->recovery_global_consistency();

    return v;
}
// void TreeItem::active_request(PosSource pos, int openLinkIn)
// {
////    _active_request = true;
// _position = pos;
// _open_link_in_new_window = openLinkIn;
// }


//// deprecated
// boost::intrusive_ptr<TreeItem> TreeItem::active_subset()const
// {
////    std::shared_ptr<TableData> result = std::make_shared<TableData>();

////    for(auto &i : _tabledata) {
////        if(i->unique_page())result->insert_new_record(work_pos(), i);
////    }

//// bypass slite fat switch:

////    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
////    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
////    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
////    //    QMap<QString, QString> data;
////    //    boost::intrusive_ptr<TreeItem> tree = new TreeItem(
////    //        data
////    //        , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))  // _parent_item
////    //    );
////    //    tree->
////    import_from_dom(dommodel);

// QMap<QString, QString> data;

// data["id"]      = this->field("id");
// data["name"]    = this->field("name");

// boost::intrusive_ptr<TreeItem> result = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

////    result->field("id", this->field("id"));
////    result->field("name", this->field("name"));
////    //    QList<boost::intrusive_ptr<TreeItem>> result;

// for(int i = 0; i < count_direct(); i++) {
// if(_child_items.at(i)->page_valid())result->child_insert(i, _child_items.at(i), ADD_NEW_RECORD_TO_END);
// }

////    _child_items.clear();
////    _child_items = result;
// return  result;   // new TreeItem(data, _parent_item);

////    return result;
// }





bool TreeItem::is_empty() const {
    return static_cast<const Record *>(this)->is_empty()
	   && static_cast<const ItemsFlat *>(this)->is_empty();
}
boost::intrusive_ptr<Binder> TreeItem::binder(){return _binder;}

const boost::intrusive_ptr<Binder> && TreeItem::binder() const
{
    return std::forward<const boost::intrusive_ptr<Binder> >(_binder);
}

void TreeItem::binder(boost::intrusive_ptr<Binder> &&binder_){
    if(binder_){
	binder_->host(this);
	if(binder_ != _binder)this->_binder = std::forward<boost::intrusive_ptr<Binder> >(binder_);
    }else if(_binder){
	_binder->page(nullptr);
	_binder = nullptr;
    }
}
boost::intrusive_ptr<Linker> TreeItem::linker(){return _linker;}

const boost::intrusive_ptr<Linker> && TreeItem::linker() const {return std::forward<const boost::intrusive_ptr<Linker> >(_linker);}

void TreeItem::linker(boost::intrusive_ptr<Linker> &&up_linker_){
    if(up_linker_){
	up_linker_->host(this);
	if(up_linker_ != _linker)this->_linker = std::forward<boost::intrusive_ptr<Linker> >(up_linker_);
    }else if(_linker){
	_linker->host_parent(nullptr);
	_linker = nullptr;
    }
}
bool TreeItem::page_valid() const {
    bool result = false;
    if(_binder){
	result = (_binder->page() != nullptr);
    }
    return result;
}
int TreeItem::count_children_all(){
    std::function<int (boost::intrusive_ptr<const TreeItem>, int)>
    count_all_recursive
	= [&](boost::intrusive_ptr<const TreeItem> it, int mode) -> int {
	    static int size = 0;
	    if(mode == 0){size = 0;return size;}
	    size += it->count_direct();
	    for(auto i : it->_child_linkers){
		count_all_recursive(i->host(), 1);
	    }
	    return size;
	};

    count_all_recursive(boost::intrusive_ptr<const TreeItem> (this), 0);

    return count_all_recursive(boost::intrusive_ptr<const TreeItem> (this), 1);
}
void TreeItem::binder_reset(){
    if(_binder){				// _page != nullptr
	// std::function<void(boost::intrusive_ptr<TreeItem>)>
	// close_tab_recursive = [&](boost::intrusive_ptr<TreeItem> it)->void {
	// if(it->is_registered_to_browser())   // item_to_be_deleted->unique_page()
	// it->bounded_page()->record_controller()->page_remove(it->id()); // (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

	// if(it->count_direct() > 0)
	// {
	// for(int i = 0; i < it->count_direct(); i++) {
	// close_tab_recursive(it->item_direct(i)->host());
	// }
	// }
	// };

	////
	// browser::WebView *view = _record_binder->bounded_page()->view();    //_page->view();
	// browser::TabWidget *tabmanager = nullptr;

	// if(view) {
	// tabmanager = view->tabmanager();
	// }

	// if(_record_binder->bounded_item()) {


	//// multi record to one page:
	//// assert(_page->record()->getNaturalFieldSource("id") == this->getNaturalFieldSource("id"));
	//// assert(_page->record()->getNaturalFieldSource("url") == this->getNaturalFieldSource("url"));
	//// assert(_page->record().get() == this);

	// bool is_holder = (_record_binder->bounded_item().get() == this);     // _page->record() may mean some other record

	////            page_to_nullptr();

	////        _page->record(nullptr);
	////        _page = nullptr;

	// if(view && tabmanager && is_holder) {
	//// && check_register_record(QUrl(browser::DockedWindow::_defaulthome)) != this

	////                assert(_record_binder->bounded_page() == _record_binder->bounded_item()->unique_page());   // _page->rebind_record() make sure of this statement

	// close_tab_recursive(this);  // if(tabmanager->webViewIndex(view) != -1)tabmanager->closeTab(tabmanager->webViewIndex(view));

	// }

	// }

	// if(_binder->page_link()) {
	// _binder->page_link() = nullptr;
	// }


	auto _page_binder = _binder->page() ? _binder->page()->binder() : nullptr;
	if(_page_binder){
	    if(_page_binder->page())_page_binder->page(nullptr);
	    if(_page_binder->host())_page_binder->host(nullptr);
//            if(_binder->page()){
//                _binder->page()->binder(nullptr);
//                _binder->page(nullptr);
//            }
	    assert(_page_binder == _binder);
	    _page_binder.reset(nullptr);
	}
//        auto _host_binder = _binder->host() ? _binder->host()->binder() : nullptr;
//        if(_host_binder){
//            assert(_host_binder == _binder);
//            if(_host_binder->page())_host_binder->page(nullptr);
//            if(_host_binder->host()){_host_binder->host(boost::intrusive_ptr<TreeItem>(nullptr));}
////            _binder->host()->binder(nullptr);
////            _binder->host(nullptr);
//            _host_binder = nullptr;
//        }
	_binder.reset(nullptr);

// _binder->page()->binder()->host(std::move(boost::intrusive_ptr<TreeItem>(nullptr)));                                                  // _binder->page()->binder(std::move(boost::intrusive_ptr<Binder> (nullptr)));
// _binder->page()->binder()->page(nullptr);

//// _binder->break_page();
// if(_binder->host()) _binder->host(std::move(boost::intrusive_ptr<TreeItem>(nullptr)));                                                    // _binder.reset();
// if(_binder->page()) _binder->page(nullptr);

	// _activator.reset();

	// page_to_nullptr();
    }
}
