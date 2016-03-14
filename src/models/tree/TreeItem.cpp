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
//#include "models/record_table/Record.h"
//#include "models/record_table/ItemsFlat.h"


extern GlobalParameters globalparameters;

#ifdef _with_record_table

TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _parent_item
                   , boost::intrusive_ptr<Record>   _record
                   , const QDomElement              &_dom_element
                  )
    : Record(_record)
    , ItemsFlat(_dom_element, _parent_item)
    , _parent_item([ & ]()
{
    if(_parent_item) {

        if(_parent_item->_field_data["crypt"] == "1") {
            this->to_encrypt();
        } else {
            this->to_decrypt();
        }
    }

    return _parent_item;
}())
{
    //    record_to_item();
}

#endif

//TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _parent_item
//                   , QMap<QString, QString>         _field_data
//                   , boost::intrusive_ptr<TreeItem> _child_items_root
//                  )
//    : Record(_field_data)
//    , ItemsFlat(*_child_items_root)
//    , _parent_item([ & ]()
//{
//    if(_parent_item) {
//        if(_parent_item->_field_data["crypt"] == "1") {
//            this->to_encrypt();
//        } else {
//            this->to_decrypt();
//        }

//        ItemsFlat::parent(_parent_item);
//    }

//    return _parent_item;
//}())
//, _page(nullptr)
//{
//    //    record_to_item();
//}

TreeItem::TreeItem(boost::intrusive_ptr<TreeItem>   _parent_item
                   , QMap<QString, QString>         _field_data
                   , const QDomElement              &_dom_element
                  )
    : Record(_field_data)
    , ItemsFlat(_dom_element, _parent_item)
    , _parent_item([ & ]()
{
    if(_parent_item) {
        assert(_parent_item->id() != _field_data.value("id"));
        //        assert(_parent_item->name() != _field_data.value("name"));

        // _parent_item->add_child(boost::intrusive_ptr<TreeItem>(this));

        if(_parent_item->_field_data["crypt"] == "1") {
            this->to_encrypt();
        } else {
            this->to_decrypt();
        }
    }

    return _parent_item;
}())
//, _page(nullptr)
{
    //    record_to_item();
}

//TreeItem::TreeItem(const TreeItem &item)
//    : Record(static_cast<const Record &>(item))
//    , ItemsFlat(static_cast<const ItemsFlat &>(item))
//    , _parent_item([ & ]()
//{
//    auto parent_item = const_cast<TreeItem &>(item).parent();

//    if(item._parent_item) {
//        //        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
//        //        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
//        //        //        QString crypt_value = "1";
//        //        QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
//        //        QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
//        //                                                           && parent_item->_field_data.contains(crypt_key)
//        //                                                           && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

//        //        if(crypt_1 == crypt_value) {
//        //            this->to_encrypt(); // table_data->to_encrypt();
//        //        } else if(crypt_0 == crypt_value) {
//        //            this->to_decrypt(); // table_data->to_decrypt();
//        //        }

//        if(item._parent_item->_field_data["crypt"] == "1") {
//            this->to_encrypt();
//        } else {
//            this->to_decrypt();
//        }
//    }

//    return parent_item; // boost::intrusive_ptr<TreeItem>(new TreeItem(*item._parent_item));
//}())
//{
//    if(item.page_valid()    // item._page != nullptr
//      ) {

//        _page = item._page;
//        _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
//        const_cast<TreeItem &>(item).page_to_nullptr();

//        //        obj.breakpage();

//        //        obj._page->record(nullptr);   // dangerous
//        //        obj._page = nullptr;          // readonly


//    }

//    *static_cast<Record *>(const_cast<TreeItem *>(this)) = *static_cast<const Record *>(&item);

//    //    _parent_item = item._parent_item;
//    _position = item._position;

//    _open_link_in_new_window = item._open_link_in_new_window;

//    _binder = item._binder;

//    _activator = item._activator;

//    *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);

//    // *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);
//}

//TreeItem &TreeItem::operator =(const TreeItem &item)
//{
//    if(this != &item) {

//        auto parent_item = const_cast<TreeItem &>(item).parent();

//        if(item._parent_item) {
//            //            QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
//            //            QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
//            //            //        QString crypt_value = "1";
//            //            QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
//            //            QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
//            //                                                               && parent_item->_field_data.contains(crypt_key)
//            //                                                               && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

//            //            if(crypt_1 == crypt_value) {
//            //                this->to_encrypt(); // table_data->to_encrypt();
//            //            } else if(crypt_0 == crypt_value) {
//            //                this->to_decrypt(); // table_data->to_decrypt();
//            //            }

//            if(item._parent_item->_field_data["crypt"] == "1") {
//                this->to_encrypt();
//            } else {
//                this->to_decrypt();
//            }
//        }

//        this->_parent_item = parent_item;   // boost::intrusive_ptr<TreeItem>(new TreeItem(*item._parent_item));

//        if(item.page_valid()    // item._page != nullptr
//          ) {
//            _page = item._page;
//            _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
//            const_cast<TreeItem &>(item).page_to_nullptr();

//            //        obj.breakpage();

//            //        obj._page->record(nullptr);   // dangerous
//            //        obj._page = nullptr;          // readonly


//        }

//        *static_cast<Record *>(const_cast<TreeItem *>(this)) = *static_cast<const Record *>(&item);

//        this->_position = item._position;

//        this->_open_link_in_new_window = item._open_link_in_new_window;

//        this->_binder = item._binder;

//        this->_activator = item._activator;

//        *static_cast<ItemsFlat *>(const_cast<TreeItem *>(this)) = *static_cast<const ItemsFlat *>(&item);
//    }

//    return *this;
//}

#ifdef _with_record_table

void TreeItem::records_to_children()
{
    for(int i = 0; i < _record_table->size(); i++) {
        this->add_child(_record_table->record(i));
    }
}

#endif

//void TreeItem::record_table(QDomElement i_dom_element)
//{
//    _record_table->delete_all_records();
//    _record_table.reset();
//    //    QDomElement *dom_element = &i_dom_element;
//    _record_table = std::make_shared<RecordTable>(
//                        i_dom_element  // boost::intrusive_ptr<TreeItem>(this)   // shared_from_this()
//                    );

//    //    _record_data->import(dom_element);
//}



TreeItem::~TreeItem()
{
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

    page_break();
}


// Возвращение ссылки на потомка, который хранится в списке childItems
// под указанным номером
boost::intrusive_ptr<TreeItem> TreeItem::child(int number)
{
    return _child_items.value(number);
}


//int TreeItem::direct_children_count()const
//{
//    return _child_items.count();
//}

//// Возвращение количества потомков (т.е. количество записей в списке childItems)
//int TreeItem::size() const
//{
//    return _child_items.count();
//}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя

// Returns the number under which the object is stored
// Array of the parent child items
int TreeItem::sibling_order() const
{
    if(_parent_item)
        return _parent_item->_child_items.indexOf(
                   // boost::const_pointer_cast<TreeItem>(
                   //                   const_cast<TreeItem *>(this) //
                   boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                   // ) // const_cast<TreeItem *>(this)
               );

    return 0;
}


void TreeItem::children_clear(void)
{
    ItemsFlat::clear();
}


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел,
// а физические данные на диске не затрагиваются
void TreeItem::isolate(void)
{
    _field_data.clear();

    //    _record_table->
    ItemsFlat::clear();

    //    // Удаляются все подветки
    //    qDeleteAll(_child_items);
    children_remove_all();
    //    if(_parent_item) {
    //        _parent_item->remove_child(boost::intrusive_ptr<TreeItem>(this));
    _parent_item.reset();
    //    }
}


int TreeItem::field_count() const
{
    return _field_data.count();
}


QString TreeItem::field(QString _name)const
{
    QString item_name; item_name = "";  // QString item_name(QString::null)
    bool field_found = false;

    // Если запрашивается динамическое имя из имени и количества потомков
    if(_name == "dynamicname") {
        // Имя ветки
        //        QString
        item_name = _field_data["name"];

        // Если есть шифрование в этой ветке
        // и поле является зашифрованным
        if(_field_data.contains("crypt")) {
            if(_field_data["crypt"] == "1") {
                if(globalparameters.crypt_key().length() > 0) {
                    item_name = CryptService::decryptString(globalparameters.crypt_key(), item_name);
                    field_found = true;
                } else {
                    item_name = QString(QObject::tr("Closed"));
                    field_found = true;
                }
            }
        }

        // Выясняется, есть ли у текущего элемента конечные записи
        int recordCount = this->count_direct();

        // Если конечных элементов нет, возвращатся просто имя
        if(recordCount == 0) {
            //            return itemName;
            field_found = true;
        } else {
            // Иначе конечные элементы есть, возвращается имя записи
            // и количество конечных элементов
            QString r, i;
            r = item_name + " [" + i.setNum(recordCount) + "]";
            item_name = r; //            return r;
            field_found = true;
        }
    }


    //    // Если имя поля допустимо
    //    if(is_field_name_available(name) && field_found == false) {
    //        // Если поле с таким именем существует
    //        if(_field_data.contains(name)) {
    //            QString value = _field_data[name];

    //            // Если есть шифрование
    //            // и поле является зашифрованным
    //            if(_field_data.contains("crypt")) {
    //                if(_field_data["crypt"] == "1") {
    //                    if(field_name_for_crypt_list().contains(name)) {
    //                        if(globalparameters.crypt_key().length() > 0
    //                           && value != ""
    //                          )
    //                            value = CryptService::decryptString(globalparameters.crypt_key(), value);
    //                        else
    //                            value = "";
    //                    }
    //                }
    //            }

    //            item_name = value;   // return value;
    //            field_found = true;
    //        }

    //        //        else {
    //        //            //            return QString(""); // Если поле не существует, возвращается пустая строка
    //        //        }
    //    }

    //    else {
    //        critical_error("In TreeItem::get_field() unavailable name '" + name + "'");
    //        exit(1);
    //        //        return "";
    //    }

    if("" == item_name && field_found == false) {
        item_name = Record::field(_name);

        if("" != item_name)field_found = true;
    }

    return item_name;
}


// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> TreeItem::fields_all()
{
    qDebug() << "TreeItem::getAllFields() : Fields data " << _field_data;

    QMap<QString, QString> result;

    QList<QString> names = _field_data.keys();

    foreach(QString name, names) {
        // В результат добаляются только параметры с разрешенным именем
        //        if(is_field_name_available(name))
        result[name] = field(name);
    }

    return result;
}


// Получение всех установленных полей "имя_поля"->"значение"
// Напрямую, без преобразований, без расшифровки
QMap<QString, QString> TreeItem::fields_all_direct()
{
    return _field_data;
}


//// Установка данных
//// Первый параметр - имя поля
//// Второй параметр - устанавливаемое значение
//void TreeItem::field(QString name, QString value)
//{
//    if(is_field_name_available(name)) {
//        // Если поле нужно шифровать
//        // и поле является зашифрованным
//        if(_field_data.contains("crypt"))
//            if(_field_data["crypt"] == "1")
//                if(field_name_for_crypt_list().contains(name)) {
//                    // Если установлен пароль
//                    if(globalparameters.crypt_key().length() > 0) {
//                        // Если поле непустое, поле зашифровывается
//                        if(value != "")
//                            value = CryptService::encryptString(globalparameters.crypt_key(), value);
//                    } else // Иначе пароль не установлен
//                        critical_error("TreeItem::setField() : Can not encrypt field \"" + name + "\". Password not setted.");
//                }

//        // qDebug() << "Set to item data " << name << value;
//        _field_data[name] = value;
//    } else
//        critical_error("TreeItem::setField() : Set unavailable field \"" + name + "\" to item of branch tree");
//}


//// Установка данных напрямую - какие данные переданы, те и запомнятся
//// без всяких преобразований, без шифрации
//void TreeItem::field_direct(QString name, QString value)
//{
//    if(is_field_name_available(name)) {
//        _field_data[name] = value;
//    } else
//        critical_error("TreeItem::setFieldDirect() : Set unavailable field \"" + name + "\" to item of branch tree");
//}


//bool TreeItem::is_field_name_available(QString name) const
//{
//    //    if(field_name_available_list().contains(name))
//    //        return true;
//    //    else
//    //        return false;
//    return field_name_available_list().contains(name);  // || is_natural_field_exists(name);
//}


//QStringList TreeItem::field_name_available_list(void) const
//{
//    return (QStringList()
//            << "id"
//            << "pin"
//            << "name"
//            << "ctime"
//            << "crypt");
//    //    return names;
//}


QStringList TreeItem::field_name_for_crypt_list(void) const
{
    return (QStringList() << "name");

    //    return names;
}

void TreeItem::parent(boost::intrusive_ptr<TreeItem> it)
{
    if(_parent_item != it)_parent_item = it;
}

boost::intrusive_ptr<TreeItem> TreeItem::parent()
{
    return _parent_item;
}


QString TreeItem::id()
{
    if(_field_data.contains("id"))
        return (_field_data["id"]);
    else {
        //        critical_error("In TreeItem data getting field with unavailable name 'id'");
        //        exit(1);
        return "";
    }
}

QString TreeItem::name()
{
    if(_field_data.contains("name"))
        return (_field_data["name"]);
    else {
        //        critical_error("In TreeItem data getting field with unavailable name 'name'");
        //        exit(1);
        return "";
    }
}

QString TreeItem::parent_id()
{
    if(_parent_item) {
        return _parent_item->field("id");
    } else
        return "";
}

// Добавление нового подчиненного элемента
// в конец списка подчиненных элементов
boost::intrusive_ptr<TreeItem> TreeItem::child_add_new(int pos)
{
    //    QMap<QString, QString> data;

    boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(new TreeItem(this));
    // new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
    // ); // Создается объект item

    //    _child_items << item; // Добавление item в конец массива childItems
    // int pos
    auto _item = child_transfer(pos                      // _child_items.count() - 1
                                , item, ADD_NEW_RECORD_AFTER);
    // auto _item = child(pos);
    return _item;
}


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
bool TreeItem::children_insert_new(int position, int count, int columns)
{
    Q_UNUSED(columns);

    if(position < 0 || position > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        //        QMap<QString, QString> data;
        boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(new TreeItem(this));
        // new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
        // );    // Создается объект item
        child_transfer(position, item, ADD_NEW_RECORD_AFTER); //_child_items.insert(position, item);        // Вставка item в нужную позицию массива childItems
    }

    return true;
}

#ifdef _with_record_table

boost::intrusive_ptr<TreeItem> TreeItem::add_child(boost::intrusive_ptr<Record> record)
{
    boost::intrusive_ptr<TreeItem> item;
    int found = 0;

    for(QList<boost::intrusive_ptr<TreeItem>>::iterator it = _child_items.begin(); it != _child_items.end(); it++) {
        if(it->get()->id() == record->field("id")) {
            found++;

            if(found == 1)
                item = *it;
            else
                _child_items.erase(it);
        }
    }

    if(0 == found) {
        //    QMap<QString, QString> data;
        //        boost::intrusive_ptr<TreeItem>
        item = boost::intrusive_ptr<TreeItem>(   // std::make_shared<TreeItem>
                   new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)), record)  // shared_from_this()
               ); // Создается объект item

        //    // Определяется, является ли родительская ветка зашифрованной
        //    if(this->field("crypt") == "1") {
        //        // Новая ветка превращается в зашифрованную
        //        item->to_encrypt();
        //    }

        //        bool already_has = false;

        //    foreach(boost::intrusive_ptr<TreeItem> it, _child_items) {
        //        if(it->id() == item->id()) {
        //            already_has = true;
        //            _child_items.erase();
        //        }
        //    }

        //        for(QList<boost::intrusive_ptr<TreeItem>>::iterator it = _child_items.begin(); it != _child_items.end(); it++) {
        //            if(it->get()->id() == item->id()) {
        //                _child_items.erase(it);
        //            }
        //        }

        //    if(!already_has)
        insert_new_item(_child_items.count() - 1, item, ADD_NEW_RECORD_AFTER);  // _child_items << item; // Добавление item в конец массива childItems
    }

    return item;
}

#endif



boost::intrusive_ptr<TreeItem> TreeItem::child_clone(boost::intrusive_ptr<TreeItem> _item)
{

    boost::intrusive_ptr<TreeItem> result(nullptr);

    if(_item != this) {
        // int pos = -1;
        int found = 0;

        for(QList<boost::intrusive_ptr<TreeItem>>::iterator it = _child_items.begin(); it != _child_items.end(); it++) {
            if(it->get() == _item.get()) {
                found++;

                if(found == 1)
                    result = *it;
                else
                    _child_items.erase(it);
            }
        }

        //        if(_item->parent()) {
        //            if(_item->parent() != this) {

        //                item->parent()->remove_child(item);
        //                item->parent(boost::intrusive_ptr<TreeItem>(this));

        //                //            _item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));   // no!!!, this make item move to new branch, but does not remove orignal one
        //            }
        //        }

        if(0 == found) {
            _child_items << _item;
            // pos =
            result = _item; // get_transfer(_child_items.count() - 1, _item, ADD_NEW_RECORD_AFTER); // _child_items << item; // Добавление item в конец массива childItems
            // result = item(pos);
        }
    }

    return result;
}



bool TreeItem::remove()
{
    bool result = false;

    if(is_empty()) {
        if(parent()) {
            if(parent()->child_remove(boost::intrusive_ptr<TreeItem>(this)))
                result = true;
        }
    }

    return result;
}

// Добавление новой записи
// Метод только добавляет во внутреннее представление новые данные,
// сохраняет текст файла и обновляет данные на экране.
// Сохранения дерева XML-данных на диск в этом методе нет.
// Допустимые режимы:
// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
// Метод принимает "тяжелый" объект записи
// Объект для вставки приходит как незашифрованным, так и зашифрованным
boost::intrusive_ptr<TreeItem> TreeItem::child_transfer(int pos, boost::intrusive_ptr<TreeItem> _item, int mode)  // does not set parent pointer?
{
    boost::intrusive_ptr<TreeItem> result(nullptr);

    assert(_item != this);

    if(_item != this) {
        // Запись добавляется в таблицу конечных записей
        int insert_position = -1;


        // get shadow_branch
        //    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

        if(!find_direct(_item)) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))

            if(_item->parent()) {
                if(_item->parent() != this) {
                    _item->parent()->child_remove(_item);
                    auto check = _item->parent();

                    while(check->remove()) {
                        check = check->parent();

                        if(!check)break;
                    }
                }
            }

            // !!!, this make item move to new branch, remember to remove orignal one
            _item->parent(boost::intrusive_ptr<TreeItem>(this));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));


            //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();



            // Выясняется, есть ли в дереве запись с указанным ID
            // Если есть, то генерируются новые ID для записи и новая директория хранения
            // Если нет, то это значит что запись была вырезана, но хранится в буфере,
            // и ее желательно вставить с прежним ID и прежним именем директории
            // It turns out, is there a record of a tree with specified ID
            // If there is, then the generated ID for the new record and the new storage directory
            // If not, then it means that the recording was cut out, but stored in a buffer
            // And it is desirable to stick with the same ID and the same name directory
            //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

            if(0 == _item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
                // Создается новая запись (ID был пустой) или
                // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
                // Create a new record (ID was empty) or
                // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

                QString id = get_unical_id();

                // Store directory entries and files    // Директория хранения записи и файл
                if(_item->field("dir") == "")_item->field("dir", id); // get_unical_id()

                if(_item->field("file") == "")_item->field("file", "text.html");

                // Unique ID of XML records             // Уникальный идентификатор XML записи
                //            QString id = get_unical_id();
                _item->field("id", id);
            } else {
                if(_item->field("dir") == "")_item->field("dir", _item->field("id")); // get_unical_id()

                if(_item->field("file") == "")_item->field("file", "text.html");
            }

            // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
            if(_item->is_lite()) {
                _item->to_fat();
                //            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
            }

            // В список переданных полей добавляются вычислимые в данном месте поля

            // Время создания данной записи
            QDateTime ctime_dt = QDateTime::currentDateTime();
            QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
            _item->field("ctime", ctime);


            // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
            bool is_crypt = false;

            //        if(_tree_item) {
            if(_is_crypt) { //            if(_tree_item->field("crypt") == "1") {
                if(globalparameters.crypt_key().length() > 0)
                    is_crypt = true;
                else
                    critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
            }

            //        }

            // Запись полновесных данных с учетом шифрации
            if(is_crypt && _item->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
                _item->to_encrypt_and_save_fat();
            } else if(!is_crypt && _item->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
                _item->to_decrypt_and_save_fat();
            } else {
                _item->push_fat_attributes();
            }

            // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
            _item->to_lite();

            //        //        // Запись добавляется в таблицу конечных записей
            //        //        int insertPos = -1;

            //        item->is_registered_to_record_controller_and_tabmanager(true);

            if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
                _child_items << _item;
                //            insert_position = _child_items.size() - 1;
            } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
                _child_items.insert(pos, _item);
                //            insert_position = pos;
            } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
                _child_items.insert(pos + 1, _item);
                //            insert_position = pos + 1;
            }

            insert_position = position_current(_item);
            qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

            // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

        } else {
            insert_position = position_current(_item);
        }

        assert(_child_items[insert_position] == _item);
        result = _item;
    }

    return result;    // insert_position;

}

//int TreeItem::shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> it, int mode)
//{
//    // Запись добавляется в таблицу конечных записей
//    int insert_position = -1;

//    it->parent(boost::intrusive_ptr<TreeItem>(this));

//    //    KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
//    assert(it->is_lite());

//    if(!find_direct(it)    //            && !dataModel->isRecordIdExists(record->getField("id"))
//      ) {

//        //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();

//        //        // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
//        //        if(record->isLite() == true)
//        //            critical_error("RecordTable::insert_new_record() can't insert lite record");

//        // Выясняется, есть ли в дереве запись с указанным ID
//        // Если есть, то генерируются новые ID для записи и новая директория хранения
//        // Если нет, то это значит что запись была вырезана, но хранится в буфере,
//        // и ее желательно вставить с прежним ID и прежним именем директории
//        // It turns out, is there a record of a tree with specified ID
//        // If there is, then the generated ID for the new record and the new storage directory
//        // If not, then it means that the recording was cut out, but stored in a buffer
//        // And it is desirable to stick with the same ID and the same name directory
//        //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());


//        assert(0 != it->field("id").length());

//        //        if(0 == record->getField("id").length()
//        //           //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
//        //          ) {
//        //            // Создается новая запись (ID был пустой) или
//        //            // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
//        //            // Create a new record (ID was empty) or
//        //            // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

//        //            QString id = get_unical_id();
//        //            // Store directory entries and files    // Директория хранения записи и файл
//        //            record->setField("dir", id);   // get_unical_id()

//        //            record->setField("file", "text.html");

//        //            // Unique ID of XML records             // Уникальный идентификатор XML записи
//        //            //            QString id = get_unical_id();
//        //            record->setField("id", id);
//        //        }


//        // В список переданных полей добавляются вычислимые в данном месте поля

//        //        // Время создания данной записи
//        //        QDateTime ctime_dt = QDateTime::currentDateTime();
//        //        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
//        //        record->field("ctime", ctime);

//        assert(it->field("ctime") != "");

//        // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
//        bool is_crypt = false;

//        //        if(_tree_item) {
//        if(_is_crypt) { //            if(_tree_item->field("crypt") == "1") {
//            if(globalparameters.crypt_key().length() > 0)
//                is_crypt = true;
//            else
//                critical_error("ItemsFlat::shadow_item_lite() : Can not insert data to crypt branch. Password not setted.");
//        }

//        //        }

//        // Запись полновесных данных с учетом шифрации
//        if(is_crypt && it->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
//            it->to_encrypt_and_save_lite();
//        } else if(!is_crypt && it->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
//            it->to_decrypt_and_save_lite();
//        } else {
//            it->push_lite_attributes();
//        }

//        //        // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
//        //        record->switchToLite();

//        //        // Запись добавляется в таблицу конечных записей
//        //        int insertPos = -1;

//        //        record->is_registered(true);
//        // assert(record->is_registered_to_browser());

//        if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
//            _child_items << it;
//            insert_position = _child_items.size() - 1;
//        } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
//            _child_items.insert(pos, it);
//            insert_position = pos;
//        } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
//            _child_items.insert(pos + 1, it);
//            insert_position = pos + 1;
//        }

//        qDebug() << "ItemsFlat::shadow_item_lite() : New record pos" << QString::number(insert_position);

//        // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

//    }

//    return insert_position;
//}


void TreeItem::page_break()
{
    if(_record_binder) {    // _page != nullptr

        //
        browser::WebView *view = _record_binder->bounded_page()->view();    //_page->view();
        browser::TabWidget *tabmanager = nullptr;

        if(view) {
            tabmanager = view->tabmanager();
        }

        if(_record_binder->bounded_item()) {


            // multi record to one page:
            // assert(_page->record()->getNaturalFieldSource("id") == this->getNaturalFieldSource("id"));
            // assert(_page->record()->getNaturalFieldSource("url") == this->getNaturalFieldSource("url"));
            // assert(_page->record().get() == this);

            bool is_holder = (_record_binder->bounded_item().get() == this);     // _page->record() may mean some other record

            //            page_to_nullptr();

            //        _page->record(nullptr);
            //        _page = nullptr;

            if(view && tabmanager && is_holder) {
                // && check_register_record(QUrl(browser::DockedWindow::_defaulthome)) != this

                //                assert(_record_binder->bounded_page() == _record_binder->bounded_item()->unique_page());   // _page->rebind_record() make sure of this statement

                if(tabmanager->webViewIndex(view) != -1)tabmanager->closeTab(tabmanager->webViewIndex(view));
            }

        }

        _record_binder.reset();
        //        _activator.reset();

        //        page_to_nullptr();
    }
}

boost::intrusive_ptr<TreeItem> TreeItem::child_remove(boost::intrusive_ptr<TreeItem> item)
{

    for(int row = 0; row < _child_items.size(); ++row) {
        if(_child_items.at(row) == item) { //_child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
            _child_items.removeOne(item);
            item->page_break();
        }
    }

    return item;
}

bool TreeItem::children_remove(int position, int count)
{
    if(position < 0 || position + count > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
    }

    return true;
}


void TreeItem::children_remove_all()
{
    for(int i = 0; i < _child_items.size(); i++)
        _child_items.removeAt(i);    // _child_items.takeAt(i).reset(); // delete _child_items.takeAt(i);
}


//bool TreeItem::children_remove_link(int position, int count)
//{
//    if(position < 0 || position + count > _child_items.size())
//        return false;

//    // Ссылка на удаленный элемент убирается из списка подчиненных элементов
//    for(int row = 0; row < count; ++row) {
//        _child_items.removeAt(position);
//    }

//    return true;
//}


bool TreeItem::move_up(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();

    // Если двигать вверх некуда, ничего делать ненужно
    if(num == 0)return false;

    // Элемент перемещается вверх по списку
    (_parent_item->_child_items).swap(num, num - 1);

    return true;
}


bool TreeItem::move_dn(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();

    // Если двигать вниз некуда, ничего делать ненужно
    if(num >= (_parent_item->count_direct()))return false;

    // Элемент перемещается вниз по списку
    (_parent_item->_child_items).swap(num, num + 1);

    return true;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::path_absolute(void)
{
    return path_absolute_as_field("id");
}


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::path_absolute_as_name(void)
{
    return path_absolute_as_field("name");
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::path_absolute_as_name_with_delimiter(QString delimeter)
{
    QStringList path = path_absolute_as_name();

    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    int emptyStringIndex = path.indexOf("");
    path.removeAt(emptyStringIndex);

    return path.join(delimeter);
}


QStringList TreeItem::path_absolute_as_field(QString field_name)
{
    QStringList path;
    boost::intrusive_ptr<TreeItem> currentItem =
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        ;

    path << currentItem->field(field_name);

    while(currentItem->parent() != nullptr) {
        currentItem = currentItem->parent();
        path << currentItem->field(field_name);
    }

    // Поворот массива идентификаторов задом наперед
    int k = path.size() - 1;
    int j = path.size() / 2;

    for(int i = 0; i < j; ++i)path.swap(i, k - i);

    return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::path_children_all(void)
{
    // Очищение списка путей
    path_children_all_as_field(
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        , "", 0);

    // Получение списка путей
    QList<QStringList> pathList = path_children_all_as_field(
                                      boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                                      , "id", 1);

    return pathList;
}


QList<QStringList> TreeItem::path_children_all_as_field(QString fieldName)
{

    //    std::function<QList<QStringList>(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)> all_children_path_as_field =
    //        [&](boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)
    //        // Возвращает массив указанных полей всех подветок, которые содержит ветка
    //        // Внутренняя рекурсивная функция
    //        //            QList<QStringList> TreeItem::()
    //    {
    //        static QList<QStringList> pathList;

    //        // Если дана команда очистить список путей
    //        if(mode == 0) {
    //            pathList.clear();
    //            return QList<QStringList>();
    //        }

    //        for(int i = 0; i < (item->child_count()); i++) {
    //            QStringList path = (item->child(i))->path_as_field(fieldName);
    //            pathList << path;
    //            all_children_path_as_field(item->child(i), fieldName, 2);
    //        }

    //        if(mode == 1)return pathList;
    //        else return QList<QStringList>();
    //    };


    // Очищение списка путей
    path_children_all_as_field(
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        , "", 0);

    // Получение списка путей
    QList<QStringList> pathList = path_children_all_as_field(
                                      boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                                      , fieldName, 1);

    return pathList;
}



//// Возвращает массив путей всех подветок, которые содержит ветка
//// Внутренняя рекурсивная функция
//QList<QStringList> TreeItem::get_all_children_path_recurse(TreeItem *item, int mode)
//{
//    static QList<QStringList> pathList;

//    // Если дана команда очистить список путей
//    if(mode == 0) {
//        pathList.clear();
//        return QList<QStringList>();
//    }

//    for(int i = 0; i < (item->childCount()); i++) {
//        QStringList path = (item->child(i))->get_path();
//        pathList << path;
//        get_all_children_path_recurse(item->child(i), 2);
//    }

//    if(mode == 1)return pathList;
//    else return QList<QStringList>();
//}



// Возвращает массив указанных полей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::path_children_all_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)
{
    static QList<QStringList> pathList;

    // Если дана команда очистить список путей
    if(mode == 0) {
        pathList.clear();
        return QList<QStringList>();
    }

    for(int i = 0; i < (item->count_direct()); i++) {
        QStringList path = (item->child(i))->path_absolute_as_field(fieldName);
        pathList << path;
        path_children_all_as_field(item->child(i), fieldName, 2);
    }

    if(mode == 1)return pathList;
    else return QList<QStringList>();
}


// Переключение ветки и всех подветок в зашифрованное состояние
void TreeItem::to_encrypt(void)
{
    qDebug() << "TreeItem::switchToEncrypt() : Crypt branch"
             << _field_data["name"]
             << QString("id : ")
             << _field_data["id"]
             ;

    // Если ветка оказалось заашифрованной ее нельзя зашифровывать второй раз
    if(_field_data["crypt"] == "1")
        return;

    // Устанавливается поле, что ветка зашифрована
    _field_data["crypt"] = "1";

    // Шифруется имя ветки
    _field_data["name"] = CryptService::encryptString(globalparameters.crypt_key(), _field_data["name"]);


    // Шифрация конечных записей для этой ветки
    //    _record_table->
    ItemsFlat::to_encrypt();


    // Шифрация подветок
    for(int i = 0; i < count_direct(); i++)
        child(i)->to_encrypt();

    if(is_lite())
        Record::to_encrypt_and_save_lite();
    else
        Record::to_encrypt_and_save_fat();
}


// Переключение ветки и всех подветок в расшифрованное состояние
void TreeItem::to_decrypt(void)
{
    qDebug() << "TreeItem::switchToDecrypt() : Decrypt branch"
             << // field("name")   //
             _field_data["name"]
             << QString("id : ")
             << // field("id")     //
             _field_data["id"]
             ;

    // Если ветка оказалось незашифрованной, нечего расшифровывать
    if(_field_data["crypt"].length() == 0 ||
       _field_data["crypt"] == "0")
        return;

    // Устанавливается поле, что ветка не зашифрована
    _field_data["crypt"] = "0";

    // Расшифровка имени ветки
    _field_data["name"] = CryptService::decryptString(globalparameters.crypt_key(), _field_data["name"]);


    // Дешифрация конечных записей для этой ветки
    //    _record_table->
    ItemsFlat::to_decrypt();


    // Дешифрация подветок
    for(int i = 0; i < count_direct(); i++)
        child(i)->to_decrypt();


    if(is_lite())
        Record::to_decrypt_and_save_lite();
    else
        Record::to_decrypt_and_save_fat();
}


//int TreeItem::size(void)
//{
//    return _child_items.size(); // _record_table->size();
//}


void TreeItem::dom_to_direct(const QDomElement &_dom_element)
{
    QDomElement record;

    QDomElement records;

    //    QString content = _dom_element.text();

    //    if(_dom_element.tagName() == "node") {
    //        record = _dom_element.firstChildElement("record");
    //        records = _dom_element.firstChildElement("record").firstChildElement("recordtable");
    //    } else

    if(_dom_element.tagName() == "record") {
        record = _dom_element;
        records = _dom_element.firstChildElement("recordtable");
    } else {    // _dom_element.tagName() == "recordtable"
        assert(_dom_element.tagName() == "recordtable");
        records = _dom_element;
    }

    if(!record.isNull() && record.tagName() == "record") {
        Record::dom_to_record(_dom_element);
    }

    if(!records.isNull() && records.tagName() == "recordtable") {   // assert(records.tagName() == "recordtable");
        ItemsFlat::dom_to_items(records  // dom_model.firstChildElement("recordtable")
                                , boost::intrusive_ptr<TreeItem>(this)    // _parent_item
                               );
    }

    //    // QDomElement n = dommodel.documentElement();
    //    // QDomElement n = dommodel;

    //    // qDebug() << "In recordtabledata setup_data_from_dom() start";

    //    // Если принятый элемент не является таблицей
    //    if(dom_model.tagName() != "recordtable")
    //        return;

    //    // Определяется указатель на первый элемент с записью
    //    // Define a pointer to the first element of the recording
    //    QDomElement current_record_dom = dom_model.firstChildElement("record");

    //    while(!current_record_dom.isNull()) {
    //        QMap<QString, QString> data;
    //        // Структура, куда будет помещена текущая запись
    //        // The structure, which will put the current record
    //        boost::intrusive_ptr<TreeItem> current_item = boost::intrusive_ptr<TreeItem>(
    //                                                          new TreeItem(data
    //                                                                  , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)) // boost::intrusive_ptr<TreeItem>(reinterpret_cast<TreeItem *>(const_cast<ItemsFlat *>(this)))  // _parent_item
    //                                                                      )
    //                                                      );

    //        current_item->is_registered_to_record_controller(true);

    //        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
    //        // The current record is added to the final table of records (and located at a certain address in memory)
    //        _child_items << current_item;

    //        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
    //        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
    //        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
    //        // Order in subordinate objects have registered a valid pointer to this entry
    //        (_child_items.last())->import_local_from_dom(current_record_dom);

    //        current_record_dom = current_record_dom.nextSiblingElement("record");
    //    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



    //    return;

}

// bypass record::export_to_dom
QDomElement TreeItem::dom_from_treeitem()
{
    //    QDomElement node = QDomDocument().createElement("node");
    QDomElement record = Record::dom_from_record();

    if(ItemsFlat::count_direct() > 0) {
        QDomElement children = ItemsFlat::dom_from_itemsflat();
        record.appendChild(children);
    }

    //    node.appendChild(record);
    return // node;    //
        record;
}

QDomElement TreeItem::dom_from_treeitem(std::shared_ptr<QDomDocument> doc)
{
    //    QDomElement node = doc->createElement("node");
    QDomElement record = Record::dom_from_record(doc);

    if(ItemsFlat::count_direct() > 0) {
        QDomElement children = ItemsFlat::dom_from_itemsflat(doc);
        record.appendChild(children);
    }

    //    node.appendChild(record);
    return // node;    //
        record;
}


//void TreeItem::clear_tabledata(void)
//{
//    _record_table->delete_all_records();
//}


//std::shared_ptr<RecordTable> TreeItem::record_table(void)
//{
//    return _record_table;
//}

//void TreeItem::record_table(std::shared_ptr<RecordTable> _table_data)
//{
//    _record_table = _table_data;
//}

//void TreeItem::page_to_nullptr()
//{
//    //    QSet<Record *> binded_records = _page->binded_records();

//    //    for(auto i : binded_records) {
//    //        if(i == this) {
//    //            i->_page = nullptr;    // _page->break_record();  // _page->bind_record(nullptr);
//    //        }
//    //    }

//    _page = nullptr;
//    _page_valid = false;
//}




browser::WebPage *TreeItem::unique_page()
{
    browser::WebPage *page = nullptr;

    if(_record_binder) {
        page = _record_binder->bounded_page();
    }

    //    //    if(_page) {
    //    //        if(_page->record() == this)
    //    //            page = _page;
    //    //        else
    //    //            page = _page->record()->_page;
    //    //    }

    //    page = _page;

    //    while(page && page->_record && page->_record != this) {
    //        // if(page->binded_record())
    //        page = page->_record->_page;
    //    }

    //    return page;

    return page;
}

bool TreeItem::is_holder()
{
    bool is_holder_ = false;

    if(_record_binder->bounded_page())    // _page
        is_holder_ = (_record_binder->bounded_item().get() == this);

    return is_holder_;
}

//void TreeItem::binder(TreeItem::bind_helper g) {_binder = g;}

//TreeItem::bind_helper TreeItem::binder() const {return _binder;}

//void TreeItem::activator(TreeItem::activate_helper a) {_activator = a;}

//TreeItem::activate_helper TreeItem::activator() const {return _activator;}

//boost::intrusive_ptr<TreeItem> TreeItem::bind(browser::WebPage *page)
//{
//    assert(page);

//    if(_record_binder->bounded_page() != page) {

//        if(_record_binder->bounded_page()) {   // _page

//            auto _item = _record_binder->bounded_item();

//            if(_item) {
//                _record_binder->bounded_page()->break_item(_item);
//                _item->page_to_nullptr();
//            }

//            _page_valid = false;
//        }

//        _page = page;
//    }

//    if(!_record_binder->bounded_item() || _record_binder->bounded_item().get() != this) {
//        _record_binder->bounded_page()->bind(boost::intrusive_ptr<TreeItem>(this));
//    }

//    _page_valid = true;
//    return this;
//}

browser::WebView *TreeItem::bind()
{
    browser::WebView *view = nullptr;

    if(!_record_binder->bounded_page()
       || !_record_binder->bounded_item()
       || (_record_binder->bounded_item() && _record_binder->bounded_item().get() != this)
       || (_record_binder->bounded_page() && _record_binder->bounded_page()->record_binder() != _record_binder)
      ) {
        view = _record_binder->binder(boost::intrusive_ptr<TreeItem>(this));
    } else {
        view = _record_binder->bounded_page()->view();
    }

    return view;
}

browser::WebView *TreeItem::activate()
{
    bind();

    //    assert(page_valid());
    //    assert(_record_binder->bounded_page());
    //    assert(_record_binder->bounded_item() && _record_binder->bounded_item().get() == this);

    //    if(_page->url().toString() != getNaturalFieldSource("url"))   // wrong! just activate the wiew
    //    if(_page->record_controller()->tabmanager()->currentWebView() != _page->view())

    return _record_binder->activator(); // boost::intrusive_ptr<TreeItem>(this)


    //    else
    //        return _page->view();

    //    else
    //        return _page->view();
}

void TreeItem::active_request(int pos, int openLinkIn)
{
    //    _active_request = true;
    _position = pos;
    _open_link_in_new_window = openLinkIn;
}



boost::intrusive_ptr<TreeItem> TreeItem::active_subset()const
{
    //    std::shared_ptr<TableData> result = std::make_shared<TableData>();

    //    for(auto &i : _tabledata) {
    //        if(i->unique_page())result->insert_new_record(work_pos(), i);
    //    }

    // bypass slite fat switch:

    //    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    //    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
    //    //    QMap<QString, QString> data;
    //    //    boost::intrusive_ptr<TreeItem> tree = new TreeItem(
    //    //        data
    //    //        , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))  // _parent_item
    //    //    );
    //    //    tree->
    //    import_from_dom(dommodel);

    QMap<QString, QString> data;

    data["id"]      = this->field("id");
    data["name"]    = this->field("name");

    boost::intrusive_ptr<TreeItem> result = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

    //    result->field("id", this->field("id"));
    //    result->field("name", this->field("name"));
    //    //    QList<boost::intrusive_ptr<TreeItem>> result;

    for(int i = 0; i < count_direct(); i++) {
        if(_child_items.at(i)->page_valid())result->child_transfer(i, _child_items.at(i), ADD_NEW_RECORD_TO_END);
    }

    //    _child_items.clear();
    //    _child_items = result;
    return  result;   // new TreeItem(data, _parent_item);

    //    return result;
}


boost::intrusive_ptr<TreeItem> TreeItem::is_registered_to_browser()
{
    boost::intrusive_ptr<TreeItem> found(nullptr);

    if(page_valid()) {
        if(_record_binder->bounded_page()->view()) {
            if(_record_binder->bounded_page()->view()->record_controller()) {
                auto _record_controller = _record_binder->bounded_page()->view()->record_controller();
                found = _record_controller->source_model()->find_current_bound(boost::intrusive_ptr<TreeItem>(this));
            }
        }
    }

    //    else {
    //        browser::Entrance *_entrance = globalparameters.entrance();

    //        for(int w = 0; w < _entrance->browsers().size(); w++) {
    //            auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

    //            for(int i = 0; i < tabmanager->count(); i++) {
    //                auto item = tabmanager->webView(i)->page()->current_item();

    //                if(item->field("id") == id()) {
    //                    found = item;
    //                }
    //            }
    //        }
    //    }

    return found;
}



bool TreeItem::is_empty() const
{
    return static_cast<const Record *>(this)->is_empty()
           && static_cast<const ItemsFlat *>(this)->is_empty()
           ;
}

void TreeItem::field(QString _name, QString value) {Record::field(_name, value);}

std::shared_ptr<CouplerDelegation> TreeItem::record_binder() {return _record_binder;}
void TreeItem::record_binder(std::shared_ptr<CouplerDelegation> _record_binder) {this->_record_binder = _record_binder;}


bool TreeItem::page_valid()const
{
    bool result = false;

    if(_record_binder) {
        result = (_record_binder->bounded_page() != nullptr);
    }

    return result;
}



