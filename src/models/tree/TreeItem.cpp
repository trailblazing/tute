#include <QStringList>
#include <QString>
#include <QMap>

#include "TreeItem.h"
#include "main.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/CryptService.h"

extern GlobalParameters globalparameters;



TreeItem::TreeItem(const QMap<QString, QString> &data, std::shared_ptr<TreeItem> parent)
    : std::enable_shared_from_this<TreeItem>()
    , _parent_item(parent), _fieldtable(data)

{
    //    parentItem = parent;
    //    fieldsTable = data;
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
std::shared_ptr<TreeItem> TreeItem::child(int number)
{
    return _child_items.value(number);
}


// Возвращение количества потомков (т.е. количество записей в списке childItems)
int TreeItem::childCount() const
{
    return _child_items.count();
}


// Возвращает номер, под которым данный объект хранится
// в массиве childItems своего родителя
int TreeItem::childNumber() const
{
    if(_parent_item)
        return _parent_item->_child_items.indexOf(
                   std::const_pointer_cast<TreeItem>(shared_from_this()) // const_cast<TreeItem *>(this)
               );

    return 0;
}


// Метод мягкого удаления всех данных из узла
// При таком удалении полностью очищается сам узел,
// а физические данные на диске не затрагиваются
void TreeItem::empty(void)
{
    _fieldtable.clear();

    _recordtable->empty();

    //    // Удаляются все подветки
    //    qDeleteAll(_child_items);

    _parent_item = NULL;
}


int TreeItem::fieldCount() const
{
    return _fieldtable.count();
}


QString TreeItem::getField(QString name)
{

    // Если запрашивается динамическое имя из имени и количества потомков
    if(name == "dynamicname") {
        // Имя ветки
        QString itemName = _fieldtable["name"];

        // Если есть шифрование в этой ветке
        // и поле является зашифрованным
        if(_fieldtable.contains("crypt"))
            if(_fieldtable["crypt"] == "1") {
                if(globalparameters.getCryptKey().length() > 0)
                    itemName = CryptService::decryptString(globalparameters.getCryptKey(), itemName);
                else
                    itemName = QString(QObject::tr("Closed"));
            }

        // Выясняется, есть ли у текущего элемента конечные записи
        int recordCount = this->recordtableGetRowCount();

        // Если конечных элементов нет, возвращатся просто имя
        if(recordCount == 0)
            return itemName;
        else {
            // Иначе конечные элементы есть, возвращается имя записи
            // и количество конечных элементов
            QString r, i;
            r = itemName + " [" + i.setNum(recordCount) + "]";
            return r;
        }
    }


    // Если имя поля допустимо
    if(isFieldNameAvailable(name)) {
        // Если поле с таким именем существует
        if(_fieldtable.contains(name)) {
            QString value = _fieldtable[name];

            // Если есть шифрование
            // и поле является зашифрованным
            if(_fieldtable.contains("crypt"))
                if(_fieldtable["crypt"] == "1")
                    if(fieldNameForCryptList().contains(name)) {
                        if(globalparameters.getCryptKey().length() > 0 &&
                           value != "")
                            value = CryptService::decryptString(globalparameters.getCryptKey(), value);
                        else
                            value = "";
                    }

            return value;
        } else
            return QString(""); // Если поле не существует, возвращается пустая строка
    } else {
        criticalError("In TreeItem::get_field() unavailable name '" + name + "'");
        exit(1);
        return "";
    }
}


// Получение всех установленных полей "имя_поля"->"значение"
QMap<QString, QString> TreeItem::getAllFields()
{
    qDebug() << "TreeItem::getAllFields() : Fields data " << _fieldtable;

    QMap<QString, QString> result;

    QList<QString> names = _fieldtable.keys();

    foreach(QString name, names) {
        // В результат добаляются только параметры с разрешенным именем
        if(isFieldNameAvailable(name))
            result[name] = getField(name);
    }

    return result;
}


// Получение всех установленных полей "имя_поля"->"значение"
// Напрямую, без преобразований, без расшифровки
QMap<QString, QString> TreeItem::getAllFieldsDirect()
{
    return _fieldtable;
}


// Установка данных
// Первый параметр - имя поля
// Второй параметр - устанавливаемое значение
void TreeItem::setField(QString name, QString value)
{
    if(isFieldNameAvailable(name)) {
        // Если поле нужно шифровать
        // и поле является зашифрованным
        if(_fieldtable.contains("crypt"))
            if(_fieldtable["crypt"] == "1")
                if(fieldNameForCryptList().contains(name)) {
                    // Если установлен пароль
                    if(globalparameters.getCryptKey().length() > 0) {
                        // Если поле непустое, поле зашифровывается
                        if(value != "")
                            value = CryptService::encryptString(globalparameters.getCryptKey(), value);
                    } else // Иначе пароль не установлен
                        criticalError("TreeItem::setField() : Can not encrypt field \"" + name + "\". Password not setted.");
                }

        // qDebug() << "Set to item data " << name << value;
        _fieldtable[name] = value;
    } else
        criticalError("TreeItem::setField() : Set unavailable field \"" + name + "\" to item of branch tree");
}


// Установка данных напрямую - какие данные переданы, те и запомнятся
// без всяких преобразований, без шифрации
void TreeItem::setFieldDirect(QString name, QString value)
{
    if(isFieldNameAvailable(name)) {
        _fieldtable[name] = value;
    } else
        criticalError("TreeItem::setFieldDirect() : Set unavailable field \"" + name + "\" to item of branch tree");
}


bool TreeItem::isFieldNameAvailable(QString name) const
{
    if(fieldNameAvailableList().contains(name))
        return true;
    else
        return false;
}


QStringList TreeItem::fieldNameAvailableList(void) const
{
    QStringList names;

    names << "id";
    names << "pin";
    names << "name";
    names << "ctime";
    names << "crypt";

    return names;
}


QStringList TreeItem::fieldNameForCryptList(void) const
{
    QStringList names;

    names << "name";

    return names;
}


std::shared_ptr<TreeItem> TreeItem::parent()
{
    return _parent_item;
}


QString TreeItem::getId()
{
    if(_fieldtable.contains("id"))
        return (_fieldtable["id"]);
    else {
        criticalError("In TreeItem data getting field with unavailable name 'id'");
        exit(1);
        return "";
    }
}


QString TreeItem::getParentId()
{
    if(_parent_item != NULL) {
        return _parent_item->getField("id");
    } else
        return "";
}


// Добавление новых подчиненных элементов
// position - позиция в списке подчиненных элементов для вставки элементов
// count - сколько новых элементов будет вставлено
// columns - сколько колонок содержит добавляемый элемент
bool TreeItem::insertChildren(int position, int count, int columns)
{
    Q_UNUSED(columns);

    if(position < 0 || position > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        QMap<QString, QString> data;
        auto item = std::make_shared<TreeItem>(data); // Создается объект item
        _child_items.insert(position, item); // Вставка item в нужную позицию массива childItems
    }

    return true;
}


// Добавление нового подчиненного элемента
// в конец списка подчиненных элементов
bool TreeItem::addChildren(void)
{
    QMap<QString, QString> data;

    std::shared_ptr<TreeItem> item = std::make_shared<TreeItem>(data, shared_from_this()); // Создается объект item

    _child_items << item; // Добавление item в конец массива childItems

    return true;
}


bool TreeItem::removeChildren(int position, int count)
{
    if(position < 0 || position + count > _child_items.size())
        return false;

    for(int row = 0; row < count; ++row) {
        _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
    }

    return true;
}


void TreeItem::removeAllChildren()
{
    for(int i = 0; i < _child_items.size(); i++)
        _child_items.removeAt(i);    // _child_items.takeAt(i).reset(); // delete _child_items.takeAt(i);
}


bool TreeItem::removeChildrenLink(int position, int count)
{
    if(position < 0 || position + count > _child_items.size())
        return false;

    // Ссылка на удаленный элемент убирается из списка подчиненных элементов
    for(int row = 0; row < count; ++row)
        _child_items.removeAt(position);

    return true;
}


bool TreeItem::moveUp(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = childNumber();

    // Если двигать вверх некуда, ничего делать ненужно
    if(num == 0)return false;

    // Элемент перемещается вверх по списку
    (_parent_item->_child_items).swap(num, num - 1);

    return true;
}


bool TreeItem::moveDn(void)
{
    // Выясняется номер данного элемента в списке родителя
    int num = childNumber();

    // Если двигать вниз некуда, ничего делать ненужно
    if(num >= (_parent_item->childCount() - 1))return false;

    // Элемент перемещается вниз по списку
    (_parent_item->_child_items).swap(num, num + 1);

    return true;
}


// Путь к элементу (список идентификаторов от корня до текущего элемента)
QStringList TreeItem::getPath(void)
{
    return getPathAsField("id");
}


// Путь к элементу (в виде списка названий веток)
QStringList TreeItem::getPathAsName(void)
{
    return getPathAsField("name");
}


// Путь к элементу в виде строки, разделенной указанным разделителем
QString TreeItem::getPathAsNameWithDelimeter(QString delimeter)
{
    QStringList path = getPathAsName();

    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    int emptyStringIndex = path.indexOf("");
    path.removeAt(emptyStringIndex);

    return path.join(delimeter);
}


QStringList TreeItem::getPathAsField(QString fieldName)
{
    QStringList path;
    std::shared_ptr<TreeItem> currentItem = shared_from_this();

    path << currentItem->getField(fieldName);

    while(currentItem->parent() != NULL) {
        currentItem = currentItem->parent();
        path << currentItem->getField(fieldName);
    }

    // Поворот массива идентификаторов задом наперед
    int k = path.size() - 1;
    int j = path.size() / 2;

    for(int i = 0; i < j; ++i)path.swap(i, k - i);

    return path;
}


// Возвращает массив путей всех подветок, которые содержит ветка
QList<QStringList> TreeItem::getAllChildrenPath(void)
{
    // Очищение списка путей
    getAllChildrenPathAsFieldRecurse(shared_from_this(), "", 0);

    // Получение списка путей
    QList<QStringList> pathList = getAllChildrenPathAsFieldRecurse(shared_from_this(), "id", 1);

    return pathList;
}


QList<QStringList> TreeItem::getAllChildrenPathAsField(QString fieldName)
{
    // Очищение списка путей
    getAllChildrenPathAsFieldRecurse(shared_from_this(), "", 0);

    // Получение списка путей
    QList<QStringList> pathList = getAllChildrenPathAsFieldRecurse(shared_from_this(), fieldName, 1);

    return pathList;
}


/*
// Возвращает массив путей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::get_all_children_path_recurse(TreeItem *item,int mode)
{
 static QList<QStringList> pathList;

 // Если дана команда очистить список путей
 if(mode==0)
  {
   pathList.clear();
   return QList<QStringList>();
  }

 for(int i=0;i<(item->childCount());i++)
 {
  QStringList path=(item->child(i))->get_path();
  pathList << path;
  get_all_children_path_recurse(item->child(i),2);
 }

 if(mode==1)return pathList;
 else return QList<QStringList>();
}
*/


// Возвращает массив указанных полей всех подветок, которые содержит ветка
// Внутренняя рекурсивная функция
QList<QStringList> TreeItem::getAllChildrenPathAsFieldRecurse(std::shared_ptr<TreeItem> item, QString fieldName, int mode)
{
    static QList<QStringList> pathList;

    // Если дана команда очистить список путей
    if(mode == 0) {
        pathList.clear();
        return QList<QStringList>();
    }

    for(int i = 0; i < (item->childCount()); i++) {
        QStringList path = (item->child(i))->getPathAsField(fieldName);
        pathList << path;
        getAllChildrenPathAsFieldRecurse(item->child(i), fieldName, 2);
    }

    if(mode == 1)return pathList;
    else return QList<QStringList>();
}


// Переключение ветки и всех подветок в зашифрованное состояние
void TreeItem::switchToEncrypt(void)
{
    qDebug() << "TreeItem::switchToEncrypt() : Crypt branch" << _fieldtable["name"] << "id" << _fieldtable["id"];

    // Если ветка оказалось заашифрованной ее нельзя зашифровывать второй раз
    if(_fieldtable["crypt"] == "1")
        return;

    // Устанавливается поле, что ветка зашифрована
    _fieldtable["crypt"] = "1";

    // Шифруется имя ветки
    _fieldtable["name"] = CryptService::encryptString(globalparameters.getCryptKey(), _fieldtable["name"]);


    // Шифрация конечных записей для этой ветки
    _recordtable->switch_to_encrypt();


    // Шифрация подветок
    for(int i = 0; i < childCount(); i++)
        child(i)->switchToEncrypt();
}


// Переключение ветки и всех подветок в расшифрованное состояние
void TreeItem::switchToDecrypt(void)
{
    qDebug() << "TreeItem::switchToDecrypt() : Decrypt branch" << _fieldtable["name"] << "id" << _fieldtable["id"];

    // Если ветка оказалось незашифрованной, нечего расшифровывать
    if(_fieldtable["crypt"].length() == 0 ||
       _fieldtable["crypt"] == "0")
        return;

    // Устанавливается поле, что ветка не зашифрована
    _fieldtable["crypt"] = "0";

    // Расшифровка имени ветки
    _fieldtable["name"] = CryptService::decryptString(globalparameters.getCryptKey(), _fieldtable["name"]);


    // Дешифрация конечных записей для этой ветки
    _recordtable->switch_to_decrypt();


    // Дешифрация подветок
    for(int i = 0; i < childCount(); i++)
        child(i)->switchToDecrypt();
}


void TreeItem::recordtableInit(QDomElement domModel)
{
    _recordtable->init(shared_from_this(), domModel);
}


int TreeItem::recordtableGetRowCount(void)
{
    return _recordtable->size();
}


QDomElement TreeItem::recordtableExportDataToDom(QDomDocument *doc)
{
    return _recordtable->dom_from_data(doc);
}


void TreeItem::recordtableDeleteAllRecords(void)
{
    _recordtable->delete_all_records();
}


std::shared_ptr<TableData> TreeItem::recordtableGetTableData(void)
{
    return _recordtable;
}


