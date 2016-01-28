#include "main.h"
#include "Record.h"
#include "RecordModel.h"
#include "RecordTable.h"

#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "views/record_table/RecordView.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/record_table/RecordScreen.h"
#include "controllers/record_table/RecordController.h"



extern FixedParameters fixedparameters;
extern AppConfig appconfig;
extern GlobalParameters globalparameters;

// Конструктор модели
RecordModel::RecordModel(QString screen_name, boost::intrusive_ptr<TreeItem> _tree_item, QObject *pobj)
    : QAbstractTableModel(pobj)
    , _tree_item(_tree_item)
      //    , _table(new RecordTable(_tree_item, QDomElement()))
{
    setObjectName(screen_name + "_source_model");
    // При создании модели она должна брать данные как минимум из
    // пустой таблицы данных
    // When you create a model, it has to take the data from at least
    // Empty data table
    //    table = new RecordTableData();

    //    return;
}


// Деструктор модели
RecordModel::~RecordModel()
{
    return;
}


// Предоставление данных табличной модели
// Provide tabular data model
QVariant RecordModel::data(const QModelIndex &index, int role) const
{
    // Если таблица данных не создана
    if(!_tree_item->record_table())    // if(!_table)
        return QVariant();

    // Если таблица пустая
    if(0 == _tree_item->record_table()->size()) // if(_table->size() == 0)
        return QVariant();

    // Если индекс недопустимый, возвращается пустой объект
    if(!index.isValid())
        return QVariant();

    // qDebug() << "RecordTableModel::data(), row:" << index.row() << " column " << index.column();

    // Если запрашивается текст строки для отрисовки или для редактирования
    if(role == Qt::DisplayRole || role == Qt::EditRole || role == SORT_ROLE) {
        QStringList showFields = appconfig.getRecordTableShowFields();

        // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
        if(index.column() < showFields.size()) {
            QString fieldName = showFields.value(index.column());

            QString field = // _table
                _tree_item->record_table()->field(index.row(), fieldName);


            // Некоторые данные при отрисовке в таблице преобразуются в "экранные" представления
            // Преобразование возможно только для отображаемой в таблице информации

            if(role == Qt::DisplayRole && fieldName == "ctime") {
                // Преобразование временного штампа в дату и время
                QDateTime fieldDateTime = QDateTime::fromString(field, "yyyyMMddhhmmss");

                if(appconfig.getEnableCustomDateTimeFormat() == false)
                    return fieldDateTime.toString(Qt::SystemLocaleDate);
                else
                    return fieldDateTime.toString(appconfig.getCustomDateTimeFormat());
            } else if(role == Qt::DisplayRole && fieldName == "hasAttach") { // Наличие аттачей
                if(field == "0")
                    return ""; // Если аттачей нет, выводится пустая строка. Это повышает читабельность
                else
                    return tr("Yes"); // На русский перевести как "Есть"
            } else if(role == Qt::DisplayRole && fieldName == "attachCount") { // Количество аттачей
                if(field == "0")
                    return ""; // Если количество аттачей нуливое, выводится пустая строка. Это повышает читабельность
                else
                    return field;

            } else
                return field;
        }
    }

    if(role == RECORD_ID_ROLE) {
        return // _table
            _tree_item->record_table()->field(index.row(), "id");
    }

    // Если происходит запрос ссылки на таблицу данных

    //    if(role == TABLE_DATA_ROLE) {
    //        QVariant var;
    //        var.setValue<RecordTableDataPointer>(this->getTableData());
    //        return var;
    //    }


    // Во всех остальных случаях
    return QVariant();
}


// Save the input data at the specified index   // Сохранение вводимых данных по указанному индексу
bool RecordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Если таблица данных не создана
    if(!_tree_item->record_table())    // if(!_table)
        return false;

    // Если таблица пустая
    if(0 == _tree_item->record_table()->size())    //if(_table->size() == 0)
        return false;

    // Если индекс недопустимый
    if(!index.isValid())
        return false;

    // Если происходит редактирование
    if(role == Qt::EditRole) {
        // QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля
        QStringList showFields = appconfig.getRecordTableShowFields();

        // Если длина списка показываемых столбцов меньше или равна номеру запрашиваемого столбца
        if(index.column() < showFields.size()) {
            QString fieldName = showFields.value(index.column());

            // Новое значение ячейки записывается в строковую переменную
            QString cellValue;
            cellValue = value.value<QString>();

            // Изменяется поле в таблице конечных записей
            //            _table
            _tree_item->record_table()->field(index.row(), fieldName, cellValue);

            emit dataChanged(index, index); // Посылается сигнал что данные были изменены

            return true;
        }
    }

    /*
    // Если происходит запись во всю таблицу данных
    if(role==TABLE_DATA_ROLE)
    {
     this->setTableData( qVariantFromValue(value) );
     return true;
    }

    // Если происходит запись одной строки
    if(role==ONE_RECORD_ROLE)
    {
      this->setTableData( qVariantFromValue(value) );
      return true;
    }
    */

    // Во всех остальных случаях
    return false;
}


// Получение заголовков столбцов и строк
QVariant RecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля
    QStringList showFields = appconfig.getRecordTableShowFields();

    QMap<QString, QString> descriptionFields = fixedparameters.record_field_description(showFields);

    // Если ни один столбец не показывается (чего, впринципе не может быть)
    if(showFields.size() == 0)
        return QVariant();

    // Если запрашивается заголовок столбца
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        // Если запрашиваемый номер столбца больше количества показываемых
        if(section > showFields.size())
            return QVariant();

        QString fieldName = showFields.value(section);

        return descriptionFields.value(fieldName);
    }


    // Если запрашивается заголовок строки
    if(orientation == Qt::Vertical && role == Qt::DisplayRole)
        return section + 1;


    return QAbstractTableModel::headerData(section, orientation, role);
}


// Сколько записей в таблице
int RecordModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if(!_tree_item->record_table())    // if(!_table)
        return 0;

    return // _table
        _tree_item->record_table()->size();
}


// Сколько столбцов в таблице
int RecordModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    static int previousColumnCount = 0;

    int currentColumnCount = appconfig.getRecordTableShowFields().size();

    if(currentColumnCount != previousColumnCount) {
        qDebug() << "Column count change. New column count: " << currentColumnCount;
        previousColumnCount = currentColumnCount;
    }

    return currentColumnCount;
}


// Удаление строк в таблице
// note: Переопределение метода removeRows() влияет и на метод removeRow(),
// так как он просто вызывает removeRows() для удаления одной строки
// Delete rows in the table
// Note: Override removeRows () method affects the removeRow (),
// Because it simply calls removeRows () to remove a single line
bool RecordModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);


    if(row < 0 || row >= rowCount() || (row + count - 1) < 0 || (row + count - 1) >= rowCount()) {
        critical_error("Bad arguments in RecordTableModel::removeRows(). row: " + QString::number(row) + " count: " + QString::number(count));
        return false;
    }

    //QModelIndex index = createIndex(row, 0);

    auto view = globalparameters.table_screen()->table_controller()->view();

    beginRemoveRows(//index   //
        QModelIndex()
        , row, row + count - 1);


    // Удаляются строки непосредственно в таблице
    for(int i = row; i < row + count; ++i) {
        //        _table
        _tree_item->record_table()->delete_record_by_position(i);
        globalparameters.find_screen()->remove_row(i);  // ?
    }

    view->reset();
    view->setModel(this);
    endRemoveRows();

    return true;
}

void RecordModel::tree_item(boost::intrusive_ptr<TreeItem>item)
{
    beginResetModel();
    _tree_item = item;
    endResetModel();
}

//// Установка данных в таблицу данных
//void RecordModel::reset_tabledata(std::shared_ptr<RecordTable> record_table)
//{
//    beginResetModel();
//    //    _table
//    _tree_item->record_table(record_table);

//    endResetModel();
//}


// Получение ссылки на таблицу данных
std::shared_ptr<RecordTable> RecordModel::table_data(void)
{
    // Возвращается ссылка на данные, с которыми в данный момент работает модель
    return _tree_item->record_table(); // _table;
}


// Добавление данных
// Функция возвращает позицию нового добавленного элемента
int RecordModel::insert_new_record(int mode, QModelIndex posIndex, boost::intrusive_ptr<Record> record)
{
    beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

    // Вставка новых данных в таблицу конечных записей
    int selPos = // _table
        _tree_item->record_table()->insert_new_record(posIndex.row(), record, mode);

    endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

    return selPos;
}


void RecordModel::on_table_config_changed(void)
{
    beginResetModel();
    endResetModel();
}

void RecordModel::reset_internal_data()
{
    QAbstractTableModel::resetInternalData();
}
