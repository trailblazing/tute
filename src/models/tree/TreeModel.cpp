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

QVariant TreeModel::data(const QModelIndex &_index, int role) const
{
    // Если индекс невалиден, возвращается несуществующий элемент
    if(!_index.isValid())return QVariant();

    // Если запрашивается окраска текста элемента
    if(role == Qt::ForegroundRole) {
        boost::intrusive_ptr<TreeItem> it = item(_index);

        if(it->row_count() > 0)
            return QColor(Qt::black);// Если узел содержит таблицу конечных записей
        else
            return QColor(Qt::darkGray); // Ветка без таблицы конечных записей
    }


    if(role == Qt::BackgroundRole) {
        if(_index == _cursor_over_index)
            return QColor(Qt::gray);
    }


    // Если запрашивается содержимое текста элемента
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        boost::intrusive_ptr<TreeItem> it = item(_index);

        return QVariant(it->field("dynamicname"));   // Запрашивается строка имени с количеством элементов
    }

    // Если запрашиваются элементы оформления
    if(role == Qt::DecorationRole) {
        boost::intrusive_ptr<TreeItem> it = item(_index);

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


Qt::ItemFlags TreeModel::flags(const QModelIndex &_index) const
{
    if(!_index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


// Get a pointer to the Item-element associated with the specified QModelIndex  // Получение указателя на Item-злемент связанный с заданным QModelIndex
boost::intrusive_ptr<TreeItem> TreeModel::item(const QModelIndex &_index) const
{
    if(_index.isValid()) {
        boost::intrusive_ptr<TreeItem> item
            = boost::const_pointer_cast<TreeItem>(
                  boost::intrusive_ptr<TreeItem>(static_cast<TreeItem *>(_index.internalPointer()))
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


// Получение индекса элемента, который задан через parent и
// номер строки и столбца относительно parent (нумерация с нуля)
// Загадочный метод, надо еще подумать что он делает на самом деле

// Get the index of the element that is defined with parent and
// Row and column number relative to parent (numbering from zero)
// Mysterious method, we must also think that he does in fact
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0) {
        return QModelIndex();
    }

    boost::intrusive_ptr<TreeItem> parent_item  = this->item(parent);
    boost::intrusive_ptr<TreeItem> child_item   = parent_item->child(row);

    if(child_item) {
        return createIndex(row, column, static_cast<void *>(child_item.get()));
    } else {
        return QModelIndex();
    }
}


QModelIndex TreeModel::index(boost::intrusive_ptr<TreeItem> _item)
{
    QModelIndex result;

    std::function<QModelIndex(QModelIndex, boost::intrusive_ptr<TreeItem>, int)>
    index_recursive = [&](
                          QModelIndex _index, boost::intrusive_ptr<TreeItem> _item, int mode
    ) {
        static QModelIndex find_index;
        static bool is_find = false;

        if(is_find) return find_index;

        if(mode == 0) {
            is_find = false;
            return QModelIndex();
        }

        //        assert(_index.isValid());

        if(mode == 1) { // is the _index valid?
            // If the node pointer coincides with the given item    // Если указатель узла совпадает с заданным item
            if(_item.get() == static_cast<TreeItem *>(_index.internalPointer())) {
                is_find = true;
                find_index = _index;
                return find_index;
            } else if(_index.isValid()) {
                // Иначе указатель узла не совпадает с заданным
                // и нужно рекурсивно искать далее
                // Otherwise, the node pointer does not match with the specified
                // Need to recursively search for and further



                //                    int rows = _index.row();

                //                    for(int i = 0; i < _index.row(); i++) {
                //                        index_recursive(_index.child(i, 0), item, 1);
                //                    }

                //                QModelIndex first_child = index(0, 0, _index);  // _index may be invalid

                //                //                if(first_child.isValid()) {
                //                for(int i = 0; i < item->child_count(); i++) {
                //                    index_recursive(
                //                        index(i, 0, _index) // first_child.sibling(i, 0)
                //                        , item
                //                        , 1
                //                    );
                //                }

                //                //                }


                //                    int rows = _index.row();
                auto it = this->item(_index);

                for(int i = 0; i < it->child_count(); i++) {
                    auto _idx = index(i, 0, _index);  // createIndex(j, 0, static_cast<void *>(_root_item->child(j).get()));
                    //        index_recursive(_idx, item, 1);

                    if(_item.get() == it->child(i).get()) { // index_recursive(_idx, _item, 1);
                        is_find = true;
                        find_index = _idx;
                        return find_index;
                    } else {
                        auto r = index_recursive(_idx, _item, 1);

                        if(r != QModelIndex()) {
                            is_find = true;
                            find_index = r;
                            return find_index;
                        }
                    }
                }

                //                for(int i = 0; i < _index.row(); i++) {
                //                    index_recursive(_index.child(i, 0), _item, 1);
                //                }
            }

        }

        return QModelIndex();
    };


    // Initialize a recursive function  // Инициализация рекурсивной функции
    index_recursive(QModelIndex(), _item, 0);

    QModelIndex _index_root;

    //    _index_root = index(0, 0, QModelIndex());

    for(int j = 0; j < _root_item->child_count(); j++) {
        auto _idx = index(j, 0, _index_root);  // createIndex(j, 0, static_cast<void *>(_root_item->child(j).get()));
        //        index_recursive(_idx, item, 1);

        if(_item.get() == _root_item->child(j).get()) { // index_recursive(_idx, _item, 1);
            result = _idx;
        } else {
            result = index_recursive(_idx, _item, 1);
        }
    }

    //    assert(result.isValid());
    //    return index_recursive(QModelIndex(), _item, 1); // from default index?
    return result;
}


//QModelIndex TreeModel::index_recursive(QModelIndex _index, boost::intrusive_ptr<TreeItem> item, int mode)
//{
//    static QModelIndex find_index;
//    static bool is_find = false;

//    if(is_find) return find_index;

//    if(mode == 0) {
//        is_find = false;
//        return QModelIndex();
//    }

//    if(mode == 1) {
//        // Если указатель узла совпадает с заданным item
//        if(item.get() == static_cast<TreeItem *const>(_index.internalPointer())) {
//            is_find = true;
//            find_index = _index;
//            return find_index;
//        } else {
//            // Иначе указатель узла не совпадает с заданным
//            // и нужно рекурсивно искать далее
//            for(int i = 0; i < _index.row(); i++)
//                index_recursive(_index.child(i, 0), item, 1);
//        }
//    }

//    return QModelIndex();
//}


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


void TreeModel::emit_datachanged_signal(const QModelIndex &_index)
{
    emit dataChanged(_index, _index);
}


// Получение заголовка столбца
// Заголовки хранятся в корневом Item элементе просто в виде значений
// для каждого столбца
// section - для какого столбца возвращается заголовок
QVariant TreeModel::headerData(int section
                               , Qt::Orientation orientation
                               , int role
                              ) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    // Для всех столбцов возвращается одинаковое значение
    // фактически используется только один столбец
    return "";


    //    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    //        return rootItem->data(section);

    //    return QVariant();

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


QModelIndex TreeModel::parent(const QModelIndex &_index) const
{
    if(!_index.isValid())
        return QModelIndex();

    boost::intrusive_ptr<TreeItem> child_item = item(_index);
    boost::intrusive_ptr<TreeItem> parent_item;

    if(child_item) {
        parent_item = child_item->parent();

        if(!parent_item
           || (parent_item == _root_item)) {
            return QModelIndex();
        }
    }

    return createIndex(parent_item->self_index(), 0, static_cast<void *>(parent_item.get()));
}


bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    boost::intrusive_ptr<TreeItem> parent_item = item(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parent_item->remove_children(position, rows);
    endRemoveRows();

    return success;
}


int TreeModel::rowCount(const QModelIndex &itemIndex) const
{
    boost::intrusive_ptr<TreeItem> it = item(itemIndex);
    return it->child_count();
}


// Set the values ​​in the Item element associated with the specified QModelIndex   // Установка значений в Item элементе, связанного с указанным QModelIndex
bool TreeModel::setData(const QModelIndex &_index, const QVariant &value, int role)
{
    // Роль UserRole в настоящий момент используется для задания флага, сообщающего
    // что курсор неаходится над элементом при Drag and Drop
    // The Role UserRole currently used to set a flag informing
    // The cursor over an element in the neahoditsya Drag and Drop
    if(role == Qt::UserRole) {
        QModelIndex previousIndex = _cursor_over_index;

        if(value.toBool())
            _cursor_over_index = _index;
        else
            _cursor_over_index = QModelIndex();

        if(previousIndex.isValid())
            emit_datachanged_signal(previousIndex);

        if(_cursor_over_index.isValid())
            emit_datachanged_signal(_cursor_over_index);

        return true;
    }

    if(role == Qt::EditRole) {

        // Вычисляется указатель на Item элемент по QModelIndex
        // в визуальной модели дерева
        boost::intrusive_ptr<TreeItem> it = item(_index);

        // Устанавливаются данные в Item элемент
        it->field(QString("name"), value.toString());

        return true;
    }

    return false;
}


bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if(role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    Q_UNUSED(section);
    _root_item->field(QString("name"), value.toString());
    return true;
}
