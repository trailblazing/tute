#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"


extern GlobalParameters globalparameters;


TreeItem::TreeItem(boost::intrusive_ptr<Record>     record
                   , boost::intrusive_ptr<TreeItem> parent_item
                   , const QDomElement &i_dom_element
                   //                   , std::shared_ptr<RecordTable>   table_data
                  )
    : Record(record)
    , ItemsFlat(
          //          boost::intrusive_ptr<TreeItem>(this)  //
          parent_item
      )
{
    //    if(_parent_item) {
    //        QString crypt_1 = "1";
    //        QString crypt_0 = "0";
    //        QString crypt_key; crypt_key = "crypt";     // QObject::tr("crypt");
    //        QString crypt_value = (_parent_item->_field_data.size() > 0
    //                               && _parent_item->_field_data.contains(crypt_key)
    //                               && (_parent_item->_field_data[crypt_key] == crypt_1)) ? crypt_1 : crypt_0;

    //        field(crypt_key, crypt_value);

    //        if(crypt_1 == field(crypt_key)    // _field_data["crypt"]
    //          ) {
    //            //            Record::to_encrypt_fields();
    //            //            if(this->is_decrypt())
    //            this->to_encrypt();
    //        } else {
    //            //            Record::to_decrypt_fields();
    //            //            if(this->is_crypt())
    //            this->to_decrypt();
    //        }
    //    }

    //    foreach(auto &i, _field_data.keys()) {
    //        Record::_field_data[i] = _field_data[i];
    //    }


    // Создание таблицы
    if(!i_dom_element.isNull()) {
        //        QDomElement *dom_element = &i_dom_element;
        import_from_dom(
            i_dom_element  // dom_element
        );
    }


    //    record_to_item();
}

TreeItem::TreeItem(QMap<QString, QString>           field_data
                   , boost::intrusive_ptr<TreeItem> parent_item
                   , const QDomElement &i_dom_element)
    : Record()
    , ItemsFlat([ & ]()
{
    foreach(auto &i, field_data.keys()) {
        _field_data[i] = field_data[i];
    }

    return // boost::intrusive_ptr<TreeItem>(this);    //
        parent_item;
}())
, _page(nullptr)
//    //    boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter>()  //      std::enable_shared_from_this<TreeItem>()
//    //    ,
//    _parent_item(parent_item)
//    //    , _field_data([ & ]()
//    //{
//    //    if(_parent_item)
//    //        _field_data["crypt"] = (_parent_item->_field_data.contains("crypt") && _parent_item->_field_data["crypt"] == "1") ? "1" : "0";

//    //    return _field_data;
//    //}())
//    , _record_table([ & ]()
//{
//    if(_parent_item) {
//        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
//        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
//        //        QString crypt_value = "1";
//        QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
//        QString crypt_value(QString::null); crypt_value = (_parent_item->_field_data.size() > 0
//                                                           && _parent_item->_field_data.contains(crypt_key)
//                                                           && (_parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

//        //        field(crypt_key, crypt_value);

//        if(crypt_1 == crypt_value && !table_data->crypt()) {
//            this->to_encrypt(); // table_data->to_encrypt();
//        } else if(crypt_0 == crypt_value && table_data->crypt()) {
//            this->to_decrypt(); // table_data->to_decrypt();
//        }
//    }

//    foreach(auto &i, field_data.keys()) {
//        _field_data[i] = field_data[i];
//    }

//    return table_data;
//}())
////       std::make_shared<TableData>(
////           boost::intrusive_ptr<TreeItem>(
////               this // const_cast<TreeItem *>(this)
////           )
////       )


{
    //    , _field_data([ & ]() {

    //    if(_parent_item) {
    //        _field_data["crypt"] = (_parent_item->_field_data.size() > 0
    //                                && _parent_item->_field_data.contains("crypt")
    //                                && (_parent_item->_field_data["crypt"] == "1")) ? "1" : "0";

    //        if("1" == _field_data["crypt"]) {
    //            //            Record::to_encrypt_fields();
    //            //            if(this->is_decrypt())
    //            this->to_encrypt();
    //        } else {
    //            //            Record::to_decrypt_fields();
    //            //            if(this->is_crypt())
    //            this->to_decrypt();
    //        }
    //    }

    //    return _field_data;
    //    }());

    //    foreach(auto &i, field_data.keys()) {
    //        _field_data[i] = field_data[i];
    //    }

    //    parentItem = parent;
    //    fieldsTable = data;


    // Создание таблицы
    if(!i_dom_element.isNull()) {
        //        QDomElement *dom_element = &i_dom_element;
        import_from_dom(
            i_dom_element  // dom_element
        );
    }


    //    record_to_item();
}

TreeItem::TreeItem(const TreeItem &item)
    : Record(static_cast<const Record &>(item))
    , ItemsFlat(const_cast<TreeItem &>(item).parent())
{
    if(item.page_valid()    // item._page != nullptr
      ) {

        _page = item._page;
        _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
        const_cast<TreeItem &>(item).page_to_nullptr();

        //        obj.breakpage();

        //        obj._page->record(nullptr);   // dangerous
        //        obj._page = nullptr;          // readonly


    }

    _position = item._position;
    _open_link_in_new_window = item._open_link_in_new_window;
    _binder = item._binder;
    _activator = item._activator;
}

TreeItem &TreeItem::operator =(const TreeItem &item)
{
    if(this != &item) {
        if(item.page_valid()    // item._page != nullptr
          ) {
            _page = item._page;
            _page->bind(boost::intrusive_ptr<TreeItem>(this)); // does not work?
            const_cast<TreeItem &>(item).page_to_nullptr();

            //        obj.breakpage();

            //        obj._page->record(nullptr);   // dangerous
            //        obj._page = nullptr;          // readonly


        }

        this->_position = item._position;
        this->_open_link_in_new_window = item._open_link_in_new_window;
        this->_binder = item._binder;
        this->_activator = item._activator;
    }

    return *this;
}

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
    clear();

    if(page_valid()    // _page != nullptr
      ) {
        //
        browser::WebView *view = _page->view();
        browser::TabWidget *tabmanager = nullptr;

        if(view) {
            tabmanager = view->tabmanager();
        }

        if(_page->_tree_item) {


            // multi record to one page:
            // assert(_page->record()->getNaturalFieldSource("id") == this->getNaturalFieldSource("id"));
            // assert(_page->record()->getNaturalFieldSource("url") == this->getNaturalFieldSource("url"));
            // assert(_page->record().get() == this);

            bool is_holder = (_page->_tree_item.get() == this);     // _page->record() may mean some other record

            //            page_to_nullptr();

            //        _page->record(nullptr);
            //        _page = nullptr;

            if(view && tabmanager && is_holder
               // && check_register_record(QUrl(browser::DockedWindow::_defaulthome)) != this
              ) {
                assert(_page == _page->_tree_item->unique_page());   // _page->rebind_record() make sure of this statement
                tabmanager->closeTab(tabmanager->webViewIndex(view));
            }

            page_to_nullptr();
        }

        //
    }
}


// Возвращение ссылки на потомка, который хранится в списке childItems
// под указанным номером
boost::intrusive_ptr<TreeItem> TreeItem::child(int number)
{
    return _child_items.value(number);
}

int TreeItem::size()const
{
    return _child_items.count();
}

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


void TreeItem::clear_children(void)
{
    ItemsFlat::clear();
}


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел,
// а физические данные на диске не затрагиваются
void TreeItem::clear(void)
{
    _field_data.clear();

    //    _record_table->
    ItemsFlat::clear();

    //    // Удаляются все подветки
    //    qDeleteAll(_child_items);

    _parent_item.reset();
}


int TreeItem::field_count() const
{
    return _field_data.count();
}


QString TreeItem::field(QString _name)
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
        int recordCount = this->size();

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
QMap<QString, QString> TreeItem::all_fields()
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
QMap<QString, QString> TreeItem::all_fields_direct()
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


boost::intrusive_ptr<TreeItem> TreeItem::parent()
{
    return _parent_item;
}


QString TreeItem::id()
{
    if(_field_data.contains("id"))
        return (_field_data["id"]);
    else {
        critical_error("In TreeItem data getting field with unavailable name 'id'");
        exit(1);
        return "";
    }
}

QString TreeItem::name()
{
    if(_field_data.contains("name"))
        return (_field_data["name"]);
    else {
        critical_error("In TreeItem data getting field with unavailable name 'name'");
        exit(1);
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


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
bool TreeItem::insert_children(int position, int count, int columns)
{
    Q_UNUSED(columns);

    if(position < 0 || position > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        QMap<QString, QString> data;
        boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(
                                                  new TreeItem   //
                                                  (data
                                                   , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))  // shared_from_this()
                                                   // , std::make_shared<RecordTable>(QDomElement())
                                                  )
                                              ); // Создается объект item
        _child_items.insert(position, item); // Вставка item в нужную позицию массива childItems
    }

    return true;
}

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
                   new TreeItem(record
                                , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                                // , std::make_shared<RecordTable>(QDomElement())
                               )
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
        _child_items << item; // Добавление item в конец массива childItems
    }

    return item;
}

boost::intrusive_ptr<TreeItem> TreeItem::add_child(boost::intrusive_ptr<TreeItem> item)
{

    //    // Определяется, является ли родительская ветка зашифрованной
    //    if(this->field("crypt") == "1") {
    //        // Новая ветка превращается в зашифрованную
    //        item->to_encrypt();
    //    }

    _child_items << item; // Добавление item в конец массива childItems

    return item;
}


// Добавление нового подчиненного элемента
// в конец списка подчиненных элементов
boost::intrusive_ptr<TreeItem> TreeItem::add_child(void)
{
    QMap<QString, QString> data;

    boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(   // std::make_shared<TreeItem>
                                              new TreeItem(data
                                                           , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                                                           // , std::make_shared<RecordTable>(QDomElement())
                                                          )
                                          ); // Создается объект item

    _child_items << item; // Добавление item в конец массива childItems

    return item;
}


bool TreeItem::remove_children(int position, int count)
{
    if(position < 0 || position + count > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
    }

    return true;
}


void TreeItem::remove_all_children()
{
    for(int i = 0; i < _child_items.size(); i++)
        _child_items.removeAt(i);    // _child_items.takeAt(i).reset(); // delete _child_items.takeAt(i);
}


bool TreeItem::remove_children_link(int position, int count)
{
    if(position < 0 || position + count > _child_items.size())
        return false;

    // Ссылка на удаленный элемент убирается из списка подчиненных элементов
    for(int row = 0; row < count; ++row) {
        _child_items.removeAt(position);
    }

    return true;
}


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
    if(num >= (_parent_item->size() - 1))return false;

    // Элемент перемещается вниз по списку
    (_parent_item->_child_items).swap(num, num + 1);

    return true;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::path(void)
{
    return path_as_field("id");
}


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::path_as_name(void)
{
    return path_as_field("name");
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::path_as_name_with_delimiter(QString delimeter)
{
    QStringList path = path_as_name();

    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    int emptyStringIndex = path.indexOf("");
    path.removeAt(emptyStringIndex);

    return path.join(delimeter);
}


QStringList TreeItem::path_as_field(QString fieldName)
{
    QStringList path;
    boost::intrusive_ptr<TreeItem> currentItem =
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        ;

    path << currentItem->field(fieldName);

    while(currentItem->parent() != nullptr) {
        currentItem = currentItem->parent();
        path << currentItem->field(fieldName);
    }

    // Поворот массива идентификаторов задом наперед
    int k = path.size() - 1;
    int j = path.size() / 2;

    for(int i = 0; i < j; ++i)path.swap(i, k - i);

    return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::all_children_path(void)
{
    // Очищение списка путей
    all_children_path_as_field(
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        , "", 0);

    // Получение списка путей
    QList<QStringList> pathList = all_children_path_as_field(
                                      boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
                                      , "id", 1);

    return pathList;
}


QList<QStringList> TreeItem::all_children_path_as_field(QString fieldName)
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
    all_children_path_as_field(
        boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // shared_from_this()
        , "", 0);

    // Получение списка путей
    QList<QStringList> pathList = all_children_path_as_field(
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
QList<QStringList> TreeItem::all_children_path_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode)
{
    static QList<QStringList> pathList;

    // Если дана команда очистить список путей
    if(mode == 0) {
        pathList.clear();
        return QList<QStringList>();
    }

    for(int i = 0; i < (item->size()); i++) {
        QStringList path = (item->child(i))->path_as_field(fieldName);
        pathList << path;
        all_children_path_as_field(item->child(i), fieldName, 2);
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
    for(int i = 0; i < size(); i++)
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
             << field("name")   // _field_data["name"]
             << QString("id : ")
             << field("id")     // _field_data["id"]
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
    for(int i = 0; i < size(); i++)
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

void TreeItem::import_from_dom(const QDomElement &dom_model)
{
    //    RecordTable::import_from_dom(dom_model);

    // QDomElement n = dommodel.documentElement();
    // QDomElement n = dommodel;

    // qDebug() << "In recordtabledata setup_data_from_dom() start";

    // Если принятый элемент не является таблицей
    if(dom_model.tagName() != "recordtable")
        return;

    // Определяется указатель на первый элемент с записью
    // Define a pointer to the first element of the recording
    QDomElement current_record_dom = dom_model.firstChildElement("record");

    while(!current_record_dom.isNull()) {
        QMap<QString, QString> data;
        // Структура, куда будет помещена текущая запись
        // The structure, which will put the current record
        boost::intrusive_ptr<TreeItem> current_item = boost::intrusive_ptr<TreeItem>(
                                                          new TreeItem(
                                                              data
                                                              , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))   // _parent_item
                                                          )
                                                      );
        current_item->is_registered(true);

        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
        // The current record is added to the final table of records (and located at a certain address in memory)
        _child_items << current_item;

        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
        // Order in subordinate objects have registered a valid pointer to this entry
        (_child_items.last())->import_from_dom(current_record_dom);

        current_record_dom = current_record_dom.nextSiblingElement("record");
    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



    return;

}

// bypass record::export_to_dom
QDomElement TreeItem::export_to_dom()
{
    return ItemsFlat::export_to_dom();  // _record_table->
}

QDomElement TreeItem::export_to_dom(std::shared_ptr<QDomDocument> doc)
{
    return ItemsFlat::export_to_dom(doc);
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

void TreeItem::page_to_nullptr()
{
    //    QSet<Record *> binded_records = _page->binded_records();

    //    for(auto i : binded_records) {
    //        if(i == this) {
    //            i->_page = nullptr;    // _page->break_record();  // _page->bind_record(nullptr);
    //        }
    //    }

    _page = nullptr;
    _page_valid = false;
}




browser::WebPage *TreeItem::unique_page()
{
    //    browser::WebPage *page = nullptr;

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

    return _page;
}

bool TreeItem::is_holder()
{
    bool is_holder_ = false;

    if(page_valid()    // _page
      ) is_holder_ = _page->_tree_item.get() == this;

    return is_holder_;
}

void TreeItem::binder(TreeItem::bind_helper g) {_binder = g;}

TreeItem::bind_helper TreeItem::binder() const {return _binder;}

void TreeItem::activator(TreeItem::active_helper a) {_activator = a;}

TreeItem::active_helper TreeItem::activator() const {return _activator;}

boost::intrusive_ptr<TreeItem> TreeItem::bind(browser::WebPage *page)
{
    if(_page != page) {

        if(page_valid()    // _page
          ) {
            std::map<QString, boost::intrusive_ptr<TreeItem> > records = _page->binded_records() ;

            for(auto &j : records) {
                if(j.second) {
                    if(j.second.get() == this) {
                        if(j.second->_page) {
                            j.second->_page->break_record(
                                boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
                            );
                            //                        i->_page->_record = nullptr;    // _page->break_record();
                            //                        i = nullptr;    // ?
                            j.second->page_to_nullptr();
                        }


                    }
                }
            }
        }

        //        if(page) {
        //            if((!page->binded_record()) || (page->binded_record() != this)) {
        //                page->bind_record(this);
        //            }
        //        }


        //    else {
        //        _page->break_record();
        //    }

        _page = page;
    }

    if(page_valid()    // _page
      ) {

        if(!_page->_tree_item || _page->_tree_item.get() != this) {
            _page->bind(
                boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
            );
        }

        //        if((!_page->binded_records()) || (_page->binded_records() != this)) {
        //            _page->bind_record(this);
        //        }
    }

    _page_valid = true;
    return this;
}

browser::WebView *TreeItem::self_bind()
{
    browser::WebView *view = nullptr;

    if(!page_valid()    // _page
      ) {
        view = (*binder())(
                   boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
                   , &TreeItem::bind
               );
    } else if(_page->current_item() != boost::intrusive_ptr<TreeItem>(this)) {
        view = (*binder())(
                   boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
                   , &TreeItem::bind
               );
    } else {
        view = _page->view();
    }

    return view;
}

browser::WebView *TreeItem::active()
{
    if(!page_valid()    // _page
      ) {
        (*binder())(
            boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
            , &TreeItem::bind
        );
    } else if(_page->current_item() != boost::intrusive_ptr<TreeItem>(this)) {
        (*binder())(
            boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
            , &TreeItem::bind
        );
    }

    assert(page_valid());
    assert(_page);
    assert(_page->current_item() == boost::intrusive_ptr<TreeItem>(this));
    //    if(_page->url().toString() != getNaturalFieldSource("url"))   // wrong! just activate the wiew
    return (*activator())(
               boost::intrusive_ptr<TreeItem>(this)  // shared_from_this()
           );
    //    else
    //        return _page->view();
}

void TreeItem::active_request(int pos, int openLinkIn)
{
    //    _active_request = true;
    _position = pos;
    _open_link_in_new_window = openLinkIn;
}


boost::intrusive_ptr<TreeItem> TreeItem::active_subset(
    //    boost::intrusive_ptr<TreeItem> start_item
)
{
    //    std::shared_ptr<TableData> result = std::make_shared<TableData>();

    //    for(auto &i : _tabledata) {
    //        if(i->unique_page())result->insert_new_record(work_pos(), i);
    //    }

    // bypass slite fat switch:

    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
    QMap<QString, QString> data;
    boost::intrusive_ptr<TreeItem> tree = new TreeItem(
        data
        , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem *>(this))  // _parent_item
    );
    tree->import_from_dom(dommodel);

    return  tree;   // new TreeItem(data, _parent_item);

    //    return result;
}
