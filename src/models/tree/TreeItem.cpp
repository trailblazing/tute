#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"

extern GlobalParameters globalparameters;


TreeItem::TreeItem(boost::intrusive_ptr<Record>     record
                   , boost::intrusive_ptr<TreeItem> parent_item
                   , std::shared_ptr<RecordTable>   table_data
                  ):
    Record(record)
    , _parent_item(parent_item)
    //    , _field_data([ & ]()
    //{
    //    if(_parent_item)
    //        _field_data["crypt"] = (_parent_item->_field_data.contains("crypt") && _parent_item->_field_data["crypt"] == "1") ? "1" : "0";

    //    return _field_data;
    //}())
    , _record_table([ & ]()
{
    if(_parent_item) {
        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
        //        QString crypt_value = "1";
        QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
        QString crypt_value(QString::null); crypt_value = (_parent_item->_field_data.size() > 0
                                                           && _parent_item->_field_data.contains(crypt_key)
                                                           && (_parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

        //        field(crypt_key, crypt_value);

        if(crypt_1 == crypt_value && !table_data->crypt()) {
            this->to_encrypt(); // table_data->to_encrypt();
        } else if(crypt_0 == crypt_value && table_data->crypt()) {
            this->to_decrypt(); // table_data->to_decrypt();
        }
    }

    return table_data;
}())
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

    //    record_to_item();
}

TreeItem::TreeItem(QMap<QString, QString>           field_data
                   , boost::intrusive_ptr<TreeItem> parent_item
                   , std::shared_ptr<RecordTable>   table_data
                  )
    :
    //    boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter>()  //      std::enable_shared_from_this<TreeItem>()
    //    ,
    _parent_item(parent_item)
    //    , _field_data([ & ]()
    //{
    //    if(_parent_item)
    //        _field_data["crypt"] = (_parent_item->_field_data.contains("crypt") && _parent_item->_field_data["crypt"] == "1") ? "1" : "0";

    //    return _field_data;
    //}())
    , _record_table([ & ]()
{
    if(_parent_item) {
        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
        //        QString crypt_value = "1";
        QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
        QString crypt_value(QString::null); crypt_value = (_parent_item->_field_data.size() > 0
                                                           && _parent_item->_field_data.contains(crypt_key)
                                                           && (_parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

        //        field(crypt_key, crypt_value);

        if(crypt_1 == crypt_value && !table_data->crypt()) {
            this->to_encrypt(); // table_data->to_encrypt();
        } else if(crypt_0 == crypt_value && table_data->crypt()) {
            this->to_decrypt(); // table_data->to_decrypt();
        }
    }

    foreach(auto &i, field_data.keys()) {
        _field_data[i] = field_data[i];
    }

    return table_data;
}())
//       std::make_shared<TableData>(
//           boost::intrusive_ptr<TreeItem>(
//               this // const_cast<TreeItem *>(this)
//           )
//       )


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

    //    record_to_item();
}


void TreeItem::records_to_children()
{
    for(int i = 0; i < _record_table->size(); i++) {
        this->add_child(_record_table->record(i));
    }
}


void TreeItem::record_table(QDomElement i_dom_element)
{
    _record_table->delete_all_records();
    _record_table.reset();
    //    QDomElement *dom_element = &i_dom_element;
    _record_table = std::make_shared<RecordTable>(
                        i_dom_element  // boost::intrusive_ptr<TreeItem>(this)   // shared_from_this()
                    );

    //    _record_data->import(dom_element);
}


TreeItem::~TreeItem()
{
    // В родительском объекте ссылка на данный объект удаляется
    // Подумать, нужно ли это действие
    // Вроде ненужно, но может понадобится вдальнейшем
    // if(parentItem)parentItem->removeChildrenLink(childNumber(),1);

    // Вызывается процедура очищения ветки без физического удаления данных на диске
    empty();
}


// Возвращение ссылки на потомка, который хранится в списке childItems
// под указанным номером
boost::intrusive_ptr<TreeItem> TreeItem::child(int number)
{
    return _child_items.value(number);
}


// Возвращение количества потомков (т.е. количество записей в списке childItems)
int TreeItem::child_count() const
{
    return _child_items.count();
}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя

// Returns the number under which the object is stored
// Array of the parent child items
int TreeItem::self_index() const
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


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел,
// а физические данные на диске не затрагиваются
void TreeItem::empty(void)
{
    _field_data.clear();

    _record_table->empty();

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
        int recordCount = this->row_count();

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
                                                   , std::make_shared<RecordTable>(QDomElement())
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
                                , std::make_shared<RecordTable>(QDomElement())
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
                                                           , std::make_shared<RecordTable>(QDomElement())
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
    for(int row = 0; row < count; ++row)
        _child_items.removeAt(position);

    return true;
}


bool TreeItem::move_up(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = self_index();

    // Если двигать вверх некуда, ничего делать ненужно
    if(num == 0)return false;

    // Элемент перемещается вверх по списку
    (_parent_item->_child_items).swap(num, num - 1);

    return true;
}


bool TreeItem::move_dn(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = self_index();

    // Если двигать вниз некуда, ничего делать ненужно
    if(num >= (_parent_item->child_count() - 1))return false;

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

    for(int i = 0; i < (item->child_count()); i++) {
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
    _record_table->to_encrypt();


    // Шифрация подветок
    for(int i = 0; i < child_count(); i++)
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
    _record_table->to_decrypt();


    // Дешифрация подветок
    for(int i = 0; i < child_count(); i++)
        child(i)->to_decrypt();


    if(is_lite())
        Record::to_decrypt_and_save_lite();
    else
        Record::to_decrypt_and_save_fat();
}


int TreeItem::row_count(void)
{
    return _record_table->size();
}

QDomElement TreeItem::export_to_dom()
{
    return _record_table->export_to_dom();
}

QDomElement TreeItem::export_to_dom(std::shared_ptr<QDomDocument> doc)
{
    return _record_table->export_to_dom(doc);
}


void TreeItem::clear_tabledata(void)
{
    _record_table->delete_all_records();
}


std::shared_ptr<RecordTable> TreeItem::record_table(void)
{
    return _record_table;
}

void TreeItem::record_table(std::shared_ptr<RecordTable> _table_data)
{
    _record_table = _table_data;
}
