#include "TreeItem.h"
#include "TreeModel.h"
#include "main.h"
#include "models/record_table/RecordTable.h"
#include "libraries/GlobalParameters.h"

extern GlobalParameters globalparameters;


// TreeModel - Это модель данных, которая работает с видом TreeScreen
// От него наследуется класс KnowTreeModel

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    return;
}

TreeModel::~TreeModel(void)
{
    return;
}

int TreeModel::columnCount(const QModelIndex &itemIndex) const
{
    Q_UNUSED(itemIndex);

    // Ранее число столбцов вычислялось исходя из
    // количества полей в корневом элементе
    // return rootItem->fieldCount();

    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    // Если индекс невалиден, возвращается несуществующий элемент
    if(!index.isValid())return QVariant();

    // Если запрашивается окраска текста элемента
    if(role == Qt::ForegroundRole) {
        boost::intrusive_ptr<TreeItem> it = item(index);

        if(it->row_count() > 0)
            return QColor(Qt::black);// Если узел содержит таблицу конечных записей
        else
            return QColor(Qt::darkGray); // Ветка без таблицы конечных записей
    }


    if(role == Qt::BackgroundRole) {
        if(index == cursorOverIndex)
            return QColor(Qt::gray);
    }


    // Если запрашивается содержимое текста элемента
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        boost::intrusive_ptr<TreeItem> it = item(index);

        return QVariant(it->field("dynamicname"));   // Запрашивается строка имени с количеством элементов
    }

    // Если запрашиваются элементы оформления
    if(role == Qt::DecorationRole) {
        boost::intrusive_ptr<TreeItem> it = item(index);

        // Если ветка зашифрована
        if(it->field("crypt") == "1") {
            // Если пароль не введен, доступа к ветке нет
            if(globalparameters.crypt_key().length() == 0)
                return QIcon(":/resource/pic/branch_closed.svg");
            else
                return QIcon(":/resource/pic/branch_opened.svg");
        }
    }


    return QVariant();
}


Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


// Получение указателя на Item-злемент связанный с заданным QModelIndex
boost::intrusive_ptr<TreeItem> TreeModel::item(const QModelIndex &index) const
{
    if(index.isValid()) {
        boost::intrusive_ptr<TreeItem> item = boost::const_pointer_cast<TreeItem>(
                                                  boost::intrusive_ptr<TreeItem>(static_cast<TreeItem *const>(index.internalPointer()))
                                                  //->shared_from_this()
                                              );

        if(item) {
            // qDebug() << "Get tree item " << item->data("name").toString();
            return item;
        } else {
            qDebug() << "Detect bad castind to TreeItem in getItem() method ";
            return nullptr;
        }

    }

    // qDebug() << "Detect bad QModelIndex in getItem() method ";
    return _root_item;
}


QModelIndex TreeModel::index_item(boost::intrusive_ptr<TreeItem> item)
{
    // Инициализация рекурсивной функции
    index_recursive(QModelIndex(), item, 0);

    return index_recursive(QModelIndex(), item, 1);
}


QModelIndex TreeModel::index_recursive(QModelIndex index, boost::intrusive_ptr<TreeItem> item, int mode)
{
    static QModelIndex find_index;
    static bool is_find = false;

    if(is_find) return find_index;

    if(mode == 0) {
        is_find = false;
        return QModelIndex();
    }

    if(mode == 1) {
        // Если указатель узла совпадает с заданным item
        if(item.get() == static_cast<TreeItem *const>(index.internalPointer())) {
            is_find = true;
            find_index = index;
            return find_index;
        } else {
            // Иначе указатель узла не совпадает с заданным
            // и нужно рекурсивно искать далее
            for(int i = 0; i < index.row(); i++)
                index_recursive(index.child(i, 0), item, 1);
        }
    }

    return QModelIndex();
}


// Получение указателя на Item-злемент с указанным путем
boost::intrusive_ptr<TreeItem> TreeModel::item(QStringList path) const
{
    boost::intrusive_ptr<TreeItem> curritem = _root_item;

    // Перебор идентификаторов пути
    for(int i = 1; i < path.size(); i++) {
        int found = 0;

        // Поиск нужного идентификатора в подчиненных узлах текущего узла
        for(int j = 0; j < curritem->child_count(); j++)
            if((curritem->child(j))->field("id") == path.at(i)) {
                // Узел найден, он становится текущим
                curritem = curritem->child(j);
                found = 1;
                break;
            }

        // Если очередной идентификатор пути не был найден
        if(found == 0)
            critical_error("Detect bad path in getItem() method " + path.join(","));
    }

    return curritem;
}


bool TreeModel::is_item_valid(QStringList path) const
{
    if(path.count() == 1 && path[0] == "0")
        return false;

    boost::intrusive_ptr<TreeItem> curritem = _root_item;

    // Перебор идентификаторов пути
    for(int i = 1; i < path.size(); i++) {
        int found = 0;

        // Поиск нужного идентификатора в подчиненных узлах текущего узла
        for(int j = 0; j < curritem->child_count(); j++)
            if((curritem->child(j))->field("id") == path.at(i)) {
                // Узел найден, он становится текущим
                curritem = curritem->child(j);
                found = 1;
                break;
            }

        // Если очередной идентификатор пути не был найден
        if(found == 0)
            return false;
    }

    return true;
}


void TreeModel::emit_datachanged_signal(const QModelIndex &index)
{
    emit dataChanged(index, index);
}


// Получение заголовка столбца
// Заголовки хранятся в корневом Item элементе просто в виде значений
// для каждого столбца
// section - для какого столбца возвращается заголовок
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    // Для всех столбцов возвращается одинаковое значение
    // фактически используется только один столбец
    return "";

    /*
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
          return rootItem->data(section);

      return QVariant();
    */
}


// Получение индекса элемента, который задан через parent и
// номер строки и столбца относительно parent (нумерация с нуля)
// Загадочный метод, надо еще подумать что он делает на самом деле
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
        return QModelIndex();

    boost::intrusive_ptr<TreeItem> parentItem = item(parent);

    boost::intrusive_ptr<TreeItem> childItem = parentItem->child(row);

    if(childItem)
        return createIndex(row, column, static_cast<void *>(childItem.get()));
    else
        return QModelIndex();
}


// Вставка пустых строк с позиции position в количестве rows
bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    boost::intrusive_ptr<TreeItem> parentItem = item(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);

    // Добавляются строки начиная с указанной позиции, в количестве rows
    // с числом столбцов равным единице
    success = parentItem->insert_children(position, rows, 1);

    endInsertRows();

    return success;
}


QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    boost::intrusive_ptr<TreeItem> childItem = item(index);
    boost::intrusive_ptr<TreeItem> parentItem;

    if(childItem) {
        parentItem = childItem->parent();

        if(!parentItem
           || (parentItem == _root_item)) {
            return QModelIndex();
        }
    }

    return createIndex(parentItem->child_index(), 0, static_cast<void *>(parentItem.get()));
}


bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    boost::intrusive_ptr<TreeItem> parentItem = item(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->remove_children(position, rows);
    endRemoveRows();

    return success;
}


int TreeModel::rowCount(const QModelIndex &itemIndex) const
{
    boost::intrusive_ptr<TreeItem> it = item(itemIndex);
    return it->child_count();
}


// Установка значений в Item элементе, связанного с указанным QModelIndex
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Роль UserRole в настоящий момент используется для задания флага, сообщающего
    // что курсор неаходится над элементом при Drag and Drop
    // The Role UserRole currently used to set a flag informing
    // The cursor over an element in the neahoditsya Drag and Drop
    if(role == Qt::UserRole) {
        QModelIndex previousIndex = cursorOverIndex;

        if(value.toBool())
            cursorOverIndex = index;
        else
            cursorOverIndex = QModelIndex();

        if(previousIndex.isValid())
            emit_datachanged_signal(previousIndex);

        if(cursorOverIndex.isValid())
            emit_datachanged_signal(cursorOverIndex);

        return true;
    }

    if(role == Qt::EditRole) {

        // Вычисляется указатель на Item элемент по QModelIndex
        // в визуальной модели дерева
        boost::intrusive_ptr<TreeItem> it = item(index);

        // Устанавливаются данные в Item элемент
        it->field("name", value.toString());

        return true;
    }

    return false;
}


bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if(role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    Q_UNUSED(section);
    _root_item->field("name", value.toString());
    return true;
}
