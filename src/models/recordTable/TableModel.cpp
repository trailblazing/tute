#include "main.h"
#include "Record.h"
#include "TableModel.h"
#include "TableData.h"

#include "models/tree/TreeItem.h"
#include "models/tree/TreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/FixedParameters.h"
#include "views/recordTable/TableView.h"
#include "views/findInBaseScreen/FindScreen.h"

extern FixedParameters fixedparameters;
extern AppConfig appconfig;


// Конструктор модели
TableModel::TableModel(QObject *pobj)
    : QAbstractTableModel(pobj)
    , _table(new TableData())
{
    // При создании модели она должна брать данные как минимум из
    // пустой таблицы данных
    // When you create a model, it has to take the data from at least
    // Empty data table
    //    table = new RecordTableData();

    //    return;
}


// Деструктор модели
TableModel::~TableModel()
{
    return;
}


// Предоставление данных табличной модели
// Provide tabular data model
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    // Если таблица данных не создана
    if(!_table)
        return QVariant();

    // Если таблица пустая
    if(_table->size() == 0)
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

            QString field = _table->field(fieldName, index.row());


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
        return _table->field("id", index.row());
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
bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Если таблица данных не создана
    if(!_table)
        return false;

    // Если таблица пустая
    if(_table->size() == 0)
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
            _table->field(fieldName, cellValue, index.row());

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
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO: Заменить на показываемые поля
    QStringList showFields = appconfig.getRecordTableShowFields();

    QMap<QString, QString> descriptionFields = fixedparameters.recordFieldDescription(showFields);

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
int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if(!_table)
        return 0;

    return _table->size();
}


// Сколько столбцов в таблице
int TableModel::columnCount(const QModelIndex &parent) const
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
bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);


    if(row < 0 || row >= rowCount() || (row + count - 1) < 0 || (row + count - 1) >= rowCount()) {
        criticalError("Bad arguments in RecordTableModel::removeRows(). row: " + QString::number(row) + " count: " + QString::number(count));
        return false;
    }

    //QModelIndex index = createIndex(row, 0);

    auto view = globalparameters.getRecordTableScreen()->getRecordTableController()->view();

    beginRemoveRows(//index   //
        QModelIndex()
        , row, row + count - 1);


    // Удаляются строки непосредственно в таблице
    for(int i = row; i < row + count; ++i) {
        _table->delete_record(i);
        globalparameters.getFindScreen()->remove_row(row);
    }

    view->reset();
    view->setModel(this);
    endRemoveRows();

    return true;
}


// Установка данных в таблицу данных
void TableModel::reset_tabledata(std::shared_ptr<TableData> rtData)
{
    beginResetModel();

    _table = rtData;

    endResetModel();
}


// Получение ссылки на таблицу данных
std::shared_ptr<TableData> TableModel::getTableData(void)
{
    // Возвращается ссылка на данные, с которыми в данный момент работает модель
    return _table;
}


// Добавление данных
// Функция возвращает позицию нового добавленного элемента
int TableModel::addTableData(int mode, QModelIndex posIndex, std::shared_ptr<Record> record)
{
    beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

    // Вставка новых данных в таблицу конечных записей
    int selPos = _table->insert_new_record(mode, posIndex.row(), record);

    endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

    return selPos;
}


void TableModel::onRecordTableConfigChange(void)
{
    beginResetModel();
    endResetModel();
}

void TableModel::resetInternalData()
{
    QAbstractTableModel::resetInternalData();
}
