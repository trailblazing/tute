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


//// Возвращение ссылки на потомка, который хранится в списке childItems
//// под указанным номером
//boost::intrusive_ptr<TreeItem> TreeItem::item_direct(int number)
//{
//    return _child_items.value(number);
//}


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
    int result = -1;

    if(_parent_item)
        result = _parent_item->_child_items.indexOf(
                     // boost::const_pointer_cast<TreeItem>(
                     //                   const_cast<TreeItem *>(this) //
                     boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                     // ) // const_cast<TreeItem *>(this)
                 );

    return result;
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
    remove_all();
    //    if(_parent_item) {
    //        _parent_item->remove_child(boost::intrusive_ptr<TreeItem>(this));
    _parent_item.reset();
    //    }
}


int TreeItem::field_count() const
{
    return _field_data.count();
}

void TreeItem::field(QString _name, QString value) {Record::field(_name, value);}

QString TreeItem::field(QString _name)const
{
    QString item_name = "";  // QString item_name(QString::null)
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
        int record_count = this->count_direct();

        // Если конечных элементов нет, возвращатся просто имя
        if(record_count == 0) {
            //            return itemName;
            field_found = true;
        } else {
            // Иначе конечные элементы есть, возвращается имя записи
            // и количество конечных элементов
            QString r, i;
            r = item_name + " [" + i.setNum(record_count) + "]";
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
        if(_name != "") {
            item_name = Record::field(_name);

            if("" != item_name)field_found = true;
        }
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

boost::intrusive_ptr<TreeItem> TreeItem::parent(boost::intrusive_ptr<TreeItem> it, int _pos, int _mode)  // we can use caller temmplate to get caller info
{
    boost::intrusive_ptr<TreeItem> _result(nullptr);




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
    auto child_move_unique = [&](boost::intrusive_ptr<TreeItem> _parent_target, int pos, int mode)->boost::intrusive_ptr<TreeItem> { // child_insert? does not set parent pointer?
        boost::intrusive_ptr<TreeItem> result(nullptr);

        boost::intrusive_ptr<TreeItem> _source_item(this);
        assert(_source_item != _parent_target);

        if(_source_item != _parent_target)
        {

            int found = 0;

            for(auto it = _parent_target->_child_items.begin(); it != _parent_target->_child_items.end(); it++) {
                if(it->get() == _source_item.get()) {
                    found++;

                    if(found == 1)
                        result = *it;
                    else
                        _parent_target->_child_items.erase(it);
                }
            }

            //        if(0 == found) {
            // Запись добавляется в таблицу конечных записей
            int insert_position = -1;


            // get shadow_branch
            //    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

            if(
                0 == found  // !item_direct(_source_item)
            ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))

                //                // deprecated by _parent_target->remove
                //                auto _origin_parent = _source_item->parent();

                //                if(_origin_parent && _origin_parent != _parent_target) {
                //                    //                    if(_parent != this) {
                //                    _origin_parent->remove(_source_item);

                //                    //                    while(_origin_parent->remove_self_as_empty()) {
                //                    //                        _origin_parent = _origin_parent->parent();

                //                    //                        if(!_origin_parent)break;
                //                    //                    }

                //                    //                    }
                //                }

                // !!!, this make item move to new branch, remember to remove orignal one
                _source_item->_parent_item = _parent_target; // _source_item->parent(boost::intrusive_ptr<TreeItem>(_parent_target));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));


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

                if(0 == _source_item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
                    // Создается новая запись (ID был пустой) или
                    // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
                    // Create a new record (ID was empty) or
                    // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

                    QString id = get_unical_id();

                    // Store directory entries and files    // Директория хранения записи и файл
                    if(_source_item->field("dir") == "")_source_item->field("dir", id); // get_unical_id()

                    if(_source_item->field("file") == "")_source_item->field("file", "text.html");

                    // Unique ID of XML records             // Уникальный идентификатор XML записи
                    //            QString id = get_unical_id();
                    _source_item->field("id", id);
                } else {
                    if(_source_item->field("dir") == "")_source_item->field("dir", _source_item->field("id")); // get_unical_id()

                    if(_source_item->field("file") == "")_source_item->field("file", "text.html");
                }

                // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
                if(_source_item->is_lite()) {
                    _source_item->to_fat();
                    //            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
                }

                // В список переданных полей добавляются вычислимые в данном месте поля
                if(_source_item->field("ctime") == "") {
                    // Время создания данной записи
                    QDateTime ctime_dt = QDateTime::currentDateTime();
                    QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
                    _source_item->field("ctime", ctime);

                }

                // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
                bool is_crypt = false;

                //        if(_tree_item) {
                //                if(_is_crypt) { //
                if(_parent_target->field("crypt") == "1") {
                    if(globalparameters.crypt_key().length() > 0)
                        is_crypt = true;
                    else
                        critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
                }

                //        }

                // Запись полновесных данных с учетом шифрации
                if(is_crypt && _source_item->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
                    _source_item->to_encrypt_and_save_fat();
                } else if(!is_crypt && _source_item->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
                    _source_item->to_decrypt_and_save_fat();
                } else {
                    _source_item->push_fat_attributes();
                }

                // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
                _source_item->to_lite();

                //        //        // Запись добавляется в таблицу конечных записей
                //        //        int insertPos = -1;

                //        item->is_registered_to_record_controller_and_tabmanager(true);

                if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
                    _parent_target->_child_items << _source_item;
                    //            insert_position = _child_items.size() - 1;
                } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
                    _parent_target->_child_items.insert(pos, _source_item);
                    //            insert_position = pos;
                } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
                    _parent_target->_child_items.insert(pos + 1, _source_item);
                    //            insert_position = pos + 1;
                }

                insert_position =  _source_item->sibling_order();   // ItemsFlat::
                //                    _parent_target->sibling_order(_source_item);
                qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

                // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

            } else {
                insert_position = _source_item->sibling_order();    // ItemsFlat::
                //                    _parent_target->sibling_order(_source_item);
            }

            assert(_parent_target->_child_items[insert_position] == _source_item);

            if(_parent_target->_child_items.contains(_source_item))result = _source_item;

            //        }
        }

        //    else {
        //        result = boost::intrusive_ptr<TreeItem>(this);
        //    }

        return result;    // insert_position;

    };



    //    if(_parent_item && _parent_item != it) {
    //        _parent_item->remove(this);
    //    }


    if(_parent_item && _parent_item != it) {
        _parent_item->remove(this);
    }


    _parent_item = it;

    if(it) {    //  && _parent_item != it
        //        _parent_item = it;
        bool found = false;

        for(auto i : _parent_item->_child_items) {
            if(this->id() == i->id()) {found = true; break;}
        }

        if(!found) {
            //            _result = _parent_item->child_move_unique(this
            //                                                      , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
            //                                                      , _mode
            //                                                     ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);

            _result = child_move_unique(_parent_item
                                        , _pos  // _parent_item->count_direct() > 0 ? _parent_item->count_direct() - 1 : 0
                                        , _mode
                                       ); // _parent_item->_child_items << boost::intrusive_ptr<TreeItem>(this);
        }
    }

    return _result;
}

boost::intrusive_ptr<TreeItem> TreeItem::parent()const
{
    return _parent_item;
}


QString TreeItem::id()const
{
    if(_field_data.contains("id"))
        return (_field_data["id"]);
    else {
        //        critical_error("In TreeItem data getting field with unavailable name 'id'");
        //        exit(1);
        return "";
    }
}

QString TreeItem::name()const
{
    if(_field_data.contains("name"))
        return (_field_data["name"]);
    else {
        //        critical_error("In TreeItem data getting field with unavailable name 'name'");
        //        exit(1);
        return "";
    }
}

QString TreeItem::parent_id()const
{
    if(_parent_item) {
        return _parent_item->field("id");
    } else
        return "";
}




#ifdef _with_record_table

boost::intrusive_ptr<TreeItem> TreeItem::add_child(boost::intrusive_ptr<Record> record)
{
    boost::intrusive_ptr<TreeItem> item;
    int found = 0;

    for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
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

        //        for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
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

boost::intrusive_ptr<TreeItem> TreeItem::contains_direct(boost::intrusive_ptr<TreeItem> _item)const
{

    boost::intrusive_ptr<TreeItem> result(nullptr);

    if(_item != this) {
        // int pos = -1;
        //        bool found = false;

        for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
            if(it->get() == _item.get()) {
                //                found = true;

                //                if(found == 1)
                result = *it;

                //                else
                //                    _child_items.erase(it);
                break;
            }
        }
    }

    return result;
}



boost::intrusive_ptr<TreeItem> TreeItem::remove(boost::intrusive_ptr<TreeItem> _source_item)
{
    boost::intrusive_ptr<TreeItem> result(nullptr);

    for(auto it : _child_items) { // for(int row = 0; row < _child_items.size(); ++row) {
        //        auto it = _child_items.at(row);

        if(it->id() == _source_item->id()) { //_child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
            _child_items.removeOne(it);
            it->page_break();
            it->parent(nullptr);

            if(it != _source_item)_source_item->merge(it);
            else
                result = _source_item;
        }
    }

    //    self_remove_from_parent_as_empty();

    return result;
}

boost::intrusive_ptr<TreeItem> TreeItem::self_remove_from_parent()
{
    boost::intrusive_ptr<TreeItem> _result(nullptr);

    if(_parent_item) {
        _result = _parent_item->remove(boost::intrusive_ptr<TreeItem>(this));
    }

    return _result;
}

boost::intrusive_ptr<TreeItem> TreeItem::self_remove_from_parent_as_empty()
{
    boost::intrusive_ptr<TreeItem> result(nullptr); // = false;

    if(_parent_item && is_empty()) {
        result = self_remove_from_parent();

        //        if(_parent_item->parent() && _parent_item->is_empty()) {
        _parent_item->self_remove_from_parent_as_empty();
        //            if(_parent_item->remove(boost::intrusive_ptr<TreeItem>(this)))
        //                result = true;
        //        }
    }

    return result;
}

bool TreeItem::remove(int position, int count)
{
    bool result = false;
    //    if(position < 0 || position + count > _child_items.size())
    //        return false;

    if(position >= 0 && position + count <= _child_items.size()) {
        for(int row = 0; row < count; ++row) {
            _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
        }

        result = true;
    }

    return result;  //true;
}


void TreeItem::remove_all()
{
    for(int i = 0; i < _child_items.size(); i++) {
        _child_items[i]->parent(nullptr);
        //        _child_items.removeAt(i);    // _child_items.takeAt(i).reset(); // delete _child_items.takeAt(i);
    }

    _child_items.clear();
    assert(_child_items.size() == 0);
}

//void TreeItem::children_clear(void)
//{
//    ItemsFlat::clear();
//}

// copy introduce duplicated item
boost::intrusive_ptr<TreeItem> TreeItem::child_rent(boost::intrusive_ptr<TreeItem> _item)
{

    boost::intrusive_ptr<TreeItem> result(nullptr);

    if(_item != this) {
        // int pos = -1;
        int found = 0;

        for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
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
//boost::intrusive_ptr<TreeItem> TreeItem::child_move_unique(boost::intrusive_ptr<TreeItem> _source_item, int pos, int mode)  // child_insert? does not set parent pointer?
//{
//    boost::intrusive_ptr<TreeItem> result(nullptr);

//    assert(_source_item != this);

//    if(_source_item != this) {

//        int found = 0;

//        for(auto it = _child_items.begin(); it != _child_items.end(); it++) {
//            if(it->get() == _source_item.get()) {
//                found++;

//                if(found == 1)
//                    result = *it;
//                else
//                    _child_items.erase(it);
//            }
//        }

//        //        if(0 == found) {
//        // Запись добавляется в таблицу конечных записей
//        int insert_position = -1;


//        // get shadow_branch
//        //    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

//        if(
//            0 == found  // !item_direct(_source_item)
//        ) {  //       && !shadow_branch->is_record_id_exists(item->field("id"))

//            // deprecated by this->child_remove
//            auto _parent = _source_item->parent();

//            if(_parent && _parent != this) {
//                //                    if(_parent != this) {
//                _parent->remove(_source_item);

//                while(_parent->remove_self_as_empty()) {
//                    _parent = _parent->parent();

//                    if(!_parent)break;
//                }

//                //                    }
//            }

//            // !!!, this make item move to new branch, remember to remove orignal one
//            _source_item->parent(boost::intrusive_ptr<TreeItem>(this));    //_item->parent(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)));


//            //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();



//            // Выясняется, есть ли в дереве запись с указанным ID
//            // Если есть, то генерируются новые ID для записи и новая директория хранения
//            // Если нет, то это значит что запись была вырезана, но хранится в буфере,
//            // и ее желательно вставить с прежним ID и прежним именем директории
//            // It turns out, is there a record of a tree with specified ID
//            // If there is, then the generated ID for the new record and the new storage directory
//            // If not, then it means that the recording was cut out, but stored in a buffer
//            // And it is desirable to stick with the same ID and the same name directory
//            //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

//            if(0 == _source_item->field("id").length()) { //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
//                // Создается новая запись (ID был пустой) или
//                // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
//                // Create a new record (ID was empty) or
//                // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

//                QString id = get_unical_id();

//                // Store directory entries and files    // Директория хранения записи и файл
//                if(_source_item->field("dir") == "")_source_item->field("dir", id); // get_unical_id()

//                if(_source_item->field("file") == "")_source_item->field("file", "text.html");

//                // Unique ID of XML records             // Уникальный идентификатор XML записи
//                //            QString id = get_unical_id();
//                _source_item->field("id", id);
//            } else {
//                if(_source_item->field("dir") == "")_source_item->field("dir", _source_item->field("id")); // get_unical_id()

//                if(_source_item->field("file") == "")_source_item->field("file", "text.html");
//            }

//            // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
//            if(_source_item->is_lite()) {
//                _source_item->to_fat();
//                //            critical_error("ItemsFlat::insert_new_item() can't insert lite record");
//            }

//            // В список переданных полей добавляются вычислимые в данном месте поля

//            // Время создания данной записи
//            QDateTime ctime_dt = QDateTime::currentDateTime();
//            QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
//            _source_item->field("ctime", ctime);


//            // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
//            bool is_crypt = false;

//            //        if(_tree_item) {
//            //                if(_is_crypt) { //
//            if(field("crypt") == "1") {
//                if(globalparameters.crypt_key().length() > 0)
//                    is_crypt = true;
//                else
//                    critical_error("ItemsFlat::insert_new_item() : Can not insert data to crypt branch. Password not setted.");
//            }

//            //        }

//            // Запись полновесных данных с учетом шифрации
//            if(is_crypt && _source_item->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
//                _source_item->to_encrypt_and_save_fat();
//            } else if(!is_crypt && _source_item->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
//                _source_item->to_decrypt_and_save_fat();
//            } else {
//                _source_item->push_fat_attributes();
//            }

//            // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
//            _source_item->to_lite();

//            //        //        // Запись добавляется в таблицу конечных записей
//            //        //        int insertPos = -1;

//            //        item->is_registered_to_record_controller_and_tabmanager(true);

//            if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
//                _child_items << _source_item;
//                //            insert_position = _child_items.size() - 1;
//            } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
//                _child_items.insert(pos, _source_item);
//                //            insert_position = pos;
//            } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
//                _child_items.insert(pos + 1, _source_item);
//                //            insert_position = pos + 1;
//            }

//            insert_position = ItemsFlat::sibling_order(_source_item);
//            qDebug() << "ItemsFlat::insert_new_item() : New record pos" << QString::number(insert_position);

//            // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

//        } else {
//            insert_position = ItemsFlat::sibling_order(_source_item);
//        }

//        assert(_child_items[insert_position] == _source_item);
//        result = _source_item;


//        assert(_child_items.contains(result));
//        //        }
//    }

//    //    else {
//    //        result = boost::intrusive_ptr<TreeItem>(this);
//    //    }

//    return result;    // insert_position;

//}



// Добавление нового подчиненного элемента
// в конец списка подчиненных элементов
boost::intrusive_ptr<TreeItem> TreeItem::child_add_new(int pos)
{
    //    QMap<QString, QString> data;

    boost::intrusive_ptr<TreeItem> item(new TreeItem(this));
    // new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
    // ); // Создается объект item

    //    _child_items << item; // Добавление item в конец массива childItems
    // int pos
    auto _item = item->parent(this, pos, ADD_NEW_RECORD_AFTER);                      // _child_items.count() - 1

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
        boost::intrusive_ptr<TreeItem> item(new TreeItem(this));
        // new TreeItem(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)))
        // );    // Создается объект item
        item->parent(this, position, ADD_NEW_RECORD_AFTER); //_child_items.insert(position, item);        // Вставка item в нужную позицию массива childItems
    }

    return true;
}





boost::intrusive_ptr<TreeItem> TreeItem::merge(boost::intrusive_ptr<TreeItem> cut)
{
    //    typedef QPair<QString, QString> pair;
    QMap<QString, QString> r = cut->fields_all();

    //    foreach(pair i, r) {
    int field_operation_count = 0;

    for(auto i : r.keys()) {
        if(field(i) == "") {
            field(i, r[i]);
            field_operation_count++;
        }
    }

    if(0 != field_operation_count) {

        QString text = cut->is_lite() ? cut->text_from_lite_direct() : cut->text_from_fat();

        if(is_lite()) {
            if(!text_from_lite_direct() .contains(text))
                text_to_direct(text_from_lite_direct() + text);
        } else {
            if(!text_from_fat() .contains(text))
                text_to_fat(text_from_fat() + text);
        }

        auto attach = cut->attach_table();
        auto this_attach = attach_table();

        if(!attach->is_empty()) {
            auto new_attach = this_attach->merge(attach);
            attach_table(new_attach);
        }


        if(!cut->is_lite() && !is_lite()) {
            auto pic = cut->picture_files();
            auto this_pic = picture_files();

            if(pic.size() > 0) {

                for(auto i : pic.keys()) { // auto new_pic = this_pic.merge(pic);
                    if(this_pic[i].isEmpty())this_pic[i] = pic[i];
                }

                picture_files(this_pic);
            }
        }

        int count = 0;

        for(auto i : cut->_child_items) {
            //        if(!_child_items.contains(i)) {
            bool found = false;
            boost::intrusive_ptr<TreeItem> found_item_keep(nullptr);
            boost::intrusive_ptr<TreeItem> found_item_remove(nullptr);

            for(auto k : _child_items) {
                if(k->id() == i->id()) {
                    found_item_keep = k;
                    found_item_remove = i;
                    found = true;

                    break;
                }
            }

            if(!found) {
                _child_items << i;
                count++;
            } else {
                found_item_keep->merge(found_item_remove);
                count++;
            }

            //            for(auto j : _child_items) {
            //                if(i->id() == j->id()) {
            //                    j->merge(i);
            //                    count++;
            //                } else {

            //                }
            //            }

            assert(count <= cut->count_direct());
            //        }
        }

        cut->remove_all();
        assert(cut->count_direct() == 0);
    }

    return boost::intrusive_ptr<TreeItem>(this);
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


int TreeItem::move_up(void)
{
    int row = 0;
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();

    if(num > 0) {   // if(num == 0)return false;    // Если двигать вверх некуда, ничего делать ненужно
        row = -1;
        // Элемент перемещается вверх по списку
        (_parent_item->_child_items).swap(num, num + row);
    }

    return row;
}


int TreeItem::move_dn(void)
{
    int row = 0;
    // Выясняется номер данного элемента в списке родителя
    int num = sibling_order();

    // Если двигать вниз некуда, ничего делать ненужно
    if(num < (_parent_item->count_direct())) { //if(num >= (_parent_item->count_direct()))return false;

        row = 1;
        // Элемент перемещается вниз по списку
        (_parent_item->_child_items).swap(num, num + row);
    }

    return row;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::path_absolute(void)const
{
    return path_absolute_as_field("id");
}


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::path_absolute_as_name(void)const
{
    return path_absolute_as_field("name");
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::path_absolute_as_name_with_delimiter(QString delimeter)const
{
    QStringList path = path_absolute_as_name();

    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    int emptyStringIndex = path.indexOf("");
    path.removeAt(emptyStringIndex);

    return path.join(delimeter);
}


QStringList TreeItem::path_absolute_as_field(QString field_name)const
{
    QStringList path;
    boost::intrusive_ptr<TreeItem> current_item =
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        ;

    path << current_item->field(field_name);

    while(current_item->parent() != nullptr) {
        current_item = current_item->parent();
        path << current_item->field(field_name);
    }

    // Rotate backwards array identifiers advance   // Поворот массива идентификаторов задом наперед
    int k = path.size() - 1;
    int j = path.size() / 2;

    for(int i = 0; i < j; ++i)path.swap(i, k - i);

    return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::path_children_all(void)const
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


QList<QStringList> TreeItem::path_children_all_as_field(QString fieldName) const
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
    path_children_all_as_field(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)), "", 0);

    // Получение списка путей
    QList<QStringList> pathList = path_children_all_as_field(boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this)), fieldName, 1);

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
QList<QStringList> TreeItem::path_children_all_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)const
{
    static QList<QStringList> pathList;

    // Если дана команда очистить список путей
    if(mode == 0) {
        pathList.clear();
        return QList<QStringList>();
    }

    for(int i = 0; i < (item->count_direct()); i++) {
        QStringList path = (item->item_direct(i))->path_absolute_as_field(fieldName);
        pathList << path;
        path_children_all_as_field(item->item_direct(i), fieldName, 2);
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
        item_direct(i)->to_encrypt();

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
        item_direct(i)->to_decrypt();


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



browser::WebPage *TreeItem::bounded_page()
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

bool TreeItem::is_holder()const
{
    bool is_holder_ = false;

    if(_record_binder->bounded_page())    // _page
        is_holder_ = (_record_binder->bounded_item().get() == this);

    return is_holder_;
}

bool TreeItem::is_ancestor_of(boost::intrusive_ptr<const TreeItem> it) const
{
    return it->path_absolute().contains(this->id());
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

    if(!_record_binder
       || (_record_binder && !_record_binder->bounded_page())
       || (_record_binder && !_record_binder->bounded_item())
       || (_record_binder->bounded_item() && _record_binder->bounded_item().get() != this)
       || (_record_binder->bounded_page() && _record_binder->bounded_page()->record_binder() != _record_binder)
      ) {
        view = _record_binder->binder();    // boost::intrusive_ptr<TreeItem>(this)
    } else {
        view = _record_binder->bounded_page()->view();
    }

    assert(_record_binder);
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


//// deprecated
//boost::intrusive_ptr<TreeItem> TreeItem::active_subset()const
//{
//    //    std::shared_ptr<TableData> result = std::make_shared<TableData>();

//    //    for(auto &i : _tabledata) {
//    //        if(i->unique_page())result->insert_new_record(work_pos(), i);
//    //    }

//    // bypass slite fat switch:

//    //    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
//    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
//    //    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
//    //    //    QMap<QString, QString> data;
//    //    //    boost::intrusive_ptr<TreeItem> tree = new TreeItem(
//    //    //        data
//    //    //        , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))  // _parent_item
//    //    //    );
//    //    //    tree->
//    //    import_from_dom(dommodel);

//    QMap<QString, QString> data;

//    data["id"]      = this->field("id");
//    data["name"]    = this->field("name");

//    boost::intrusive_ptr<TreeItem> result = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

//    //    result->field("id", this->field("id"));
//    //    result->field("name", this->field("name"));
//    //    //    QList<boost::intrusive_ptr<TreeItem>> result;

//    for(int i = 0; i < count_direct(); i++) {
//        if(_child_items.at(i)->page_valid())result->child_insert(i, _child_items.at(i), ADD_NEW_RECORD_TO_END);
//    }

//    //    _child_items.clear();
//    //    _child_items = result;
//    return  result;   // new TreeItem(data, _parent_item);

//    //    return result;
//}

boost::intrusive_ptr<TreeItem> TreeItem::is_activated()const
{
    boost::intrusive_ptr<TreeItem> found(nullptr);

    if(record_binder()) {
        auto _entrance = globalparameters.entrance();
        auto v = _entrance->find([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == this->id();});
        auto test =  is_registered_to_browser();

        if(test && (v->tabmanager()->currentWebView() == v))found = test;
    }

    return found;
}

boost::intrusive_ptr<TreeItem> TreeItem::is_registered_to_browser()const
{
    boost::intrusive_ptr<TreeItem> found(nullptr);

    //    bool re = false;
    if(record_binder()) {
        auto _entrance = globalparameters.entrance();
        auto v = _entrance->find([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == this->id();});

        if(v) {
            if(v->page()->record_binder()) {
                if(v->page()->record_binder()->bounded_item() == boost::intrusive_ptr<const TreeItem>(this)
                   && record_binder()) {
                    found = v->page()->record_binder()->bounded_item();
                }
            }
        }
    }

    //    if(page_valid()) {
    //        auto p = _record_binder->bounded_page();
    //        auto v = p->view();

    //        if(v) {
    //            auto _tabmanager = p->tabmanager();

    //            if(_tabmanager) {
    //                if(_tabmanager->indexOf(v) != -1) {
    //                    re = true;
    //                    found = p->record_binder()->bounded_item();
    //                    assert(found);
    //                }
    //            }

    //            //            if(_record_binder->bounded_page()->view()->record_controller()) {
    //            //                auto _record_controller = _record_binder->bounded_page()->view()->record_controller();
    //            //                found = _record_controller->source_model()->find_current_bound(boost::intrusive_ptr<TreeItem>(this));
    //            //            }
    //        }
    //    }

    //    //    else {
    //    //        browser::Entrance *_entrance = globalparameters.entrance();

    //    //        for(int w = 0; w < _entrance->browsers().size(); w++) {
    //    //            auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

    //    //            for(int i = 0; i < tabmanager->count(); i++) {
    //    //                auto item = tabmanager->webView(i)->page()->current_item();

    //    //                if(item->field("id") == id()) {
    //    //                    found = item;
    //    //                }
    //    //            }
    //    //        }
    //    //    }

    //    assert((found && re) || (!found && !re));
    // assert(found);

    //    if(found && found != boost::intrusive_ptr<TreeItem>(this)) {
    //        found = globalparameters.tree_screen()->duplicated_remove(boost::intrusive_ptr<TreeItem>(this), found);
    //    }

    return found;
}



bool TreeItem::is_empty() const
{
    return static_cast<const Record *>(this)->is_empty()
           && static_cast<const ItemsFlat *>(this)->is_empty();
}



boost::intrusive_ptr<CouplerDelegation> TreeItem::record_binder() const {return _record_binder;}
void TreeItem::record_binder(boost::intrusive_ptr<CouplerDelegation> _record_binder) {this->_record_binder = _record_binder;}


bool TreeItem::page_valid()const
{
    bool result = false;

    if(_record_binder) {
        result = (_record_binder->bounded_page() != nullptr);
    }

    return result;
}


int TreeItem::count_children_all()
{
    std::function<int(boost::intrusive_ptr<const TreeItem>, int)>
    count_all_recursive = [&](boost::intrusive_ptr<const TreeItem> it, int mode)->int {
        static int size = 0;

        if(mode == 0) {size = 0; return size;}

        size += it->count_direct();

        for(auto i : it->_child_items)
        {
            count_all_recursive(i, 1);
        }

        return size;
    };

    count_all_recursive(boost::intrusive_ptr<const TreeItem> (this), 0);

    return count_all_recursive(boost::intrusive_ptr<const TreeItem> (this), 1);
}



