#include "main.h"
#include "Record.h"
#include "RecordModel.h"
#include "ItemsFlat.h"

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
#include "models/tree/TreeModelKnow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"




extern FixedParameters fixedparameters;
extern AppConfig appconfig;
extern GlobalParameters globalparameters;

void RecordModel::init_source_model(RecordController *_record_controller
                                    , RecordScreen *_record_screen
                                    , MainWindow *main_window, MetaEditor *_editor_screen)
{
    qDebug() << "In RecordController init_source_model() start";
    //    shadow_branch(_shadow_branch);



    // Обновление набора данных с последующим выбором первой строки
    // может быть очень длительным, поэтому показывается что процесс выполняется
    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
    // qApp->setOverrideCursor(cursor_wait);
    //    find_object<MainWindow>("mainwindow")
    main_window->setCursor(Qt::BusyCursor);

    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
    //    _source_model->root(tree_item);    // reset_tabledata(table_data);

    RecordView *_view = nullptr; // _record_controller->view();

    if(!_record_controller->no_view()) {
        _view = _record_controller->view();
        // Надо обязательно сбросить selection model
        _view->selectionModel()->clear();
    }

    // Если список конечных записей не пуст
    bool remove_selection = true;

    if(size() > 0) {
        // Нужно выяснить, на какой записи ранее стояло выделение
        int workPos = _browser_pages->work_pos();

        // Если номер записи допустимый
        if(!_record_controller->no_view() && workPos > 0 && workPos < rowCount()) {
            // Выделение устанавливается на нужную запись
            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
            _view->selectRow(workPos);
            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

            remove_selection = false;
        }
    }

    // If the selection does not need to install    // Если выделение устанавливать ненужно
    if(remove_selection) {
        // Надо очистить поля области редактировния
        //        find_object<MetaEditor>(meta_editor_singleton_name)
        _editor_screen->clear_all();

        // При выборе записи обновление инструментов было бы вызвано автоматически
        // в альтернативной ветке (там "эмулируется" клик по записи)
        // А так как записей нет, то обновление инструментов надо
        // вызвать принудительно
        if(_record_screen && _record_screen->inited()) // if(qobject_cast<RecordScreen *>(parent())->inited())
            _record_screen->tools_update();
    }

    // qApp->restoreOverrideCursor();
    //    find_object<MainWindow>("mainwindow")
    main_window->unsetCursor();

    //    globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

    qDebug() << "In RecordTableView set new model stop";

}


void RecordModel::init_source_model(ItemsFlat *_browser_pages
                                    , RecordController *_record_controller
                                    , RecordScreen *_record_screen
                                    , MainWindow *main_window
                                    , MetaEditor *_editor_screen
                                   )
{
    qDebug() << "In RecordController init_source_model() start";

    // Обновление набора данных с последующим выбором первой строки
    // может быть очень длительным, поэтому показывается что процесс выполняется
    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
    // qApp->setOverrideCursor(cursor_wait);
    //    find_object<MainWindow>("mainwindow")
    main_window->setCursor(Qt::BusyCursor);

    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
    this->_browser_pages = _browser_pages;  // reset_tabledata(table_data);

    RecordView *_view = nullptr; // _record_controller->view();

    if(!_record_controller->no_view()) {
        _view = _record_controller->view();
        // Надо обязательно сбросить selection model
        _view->selectionModel()->clear();
    }

    // Если список конечных записей не пуст
    bool remove_selection = true;

    if(size() > 0) {
        // Нужно выяснить, на какой записи ранее стояло выделение
        int workPos = this->_browser_pages->work_pos();

        // Если номер записи допустимый
        if(!_record_controller->no_view() && workPos > 0 && workPos < rowCount()) {
            // Выделение устанавливается на нужную запись
            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
            _view->selectRow(workPos);
            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

            remove_selection = false;
        }
    }

    // If the selection does not need to install    // Если выделение устанавливать ненужно
    if(remove_selection) {
        // Надо очистить поля области редактировния
        //        find_object<MetaEditor>(meta_editor_singleton_name)
        _editor_screen->clear_all();

        // При выборе записи обновление инструментов было бы вызвано автоматически
        // в альтернативной ветке (там "эмулируется" клик по записи)
        // А так как записей нет, то обновление инструментов надо
        // вызвать принудительно
        if(_record_screen && _record_screen->inited()) // if(qobject_cast<RecordScreen *>(parent())->inited())
            _record_screen->tools_update();
    }

    // qApp->restoreOverrideCursor();
    //    find_object<MainWindow>("mainwindow")
    main_window->unsetCursor();

    //    globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

    qDebug() << "In RecordTableView set new model stop";
}

void RecordModel::init_source_model(boost::intrusive_ptr<TreeItem> tree_item
                                    , RecordController *_record_controller
                                    , RecordScreen *_record_screen
                                    , MainWindow *main_window
                                    , MetaEditor *_editor_screen
                                   )
{
    init_source_model(tree_item.get(), _record_controller, _record_screen, main_window, _editor_screen);
}

// Конструктор модели
RecordModel::RecordModel(QString screen_name
                         , TreeScreen   *_tree_screen
                         , FindScreen   *_find_screen
                         , MetaEditor   *_editor_screen
                         , MainWindow   *_main_window
                         , RecordController *_record_controller
                         , RecordScreen *_record_screen  // , RecordController *_record_controller,
                        )
    : QAbstractTableModel(_record_screen)
    , _browser_pages(new ItemsFlat())      //    , _table(new RecordTable(_tree_item, QDomElement()))
    , _reocrd_controller(_record_controller)
{
    //    _browser_pages->init_from_xml(_appconfig.get_tetradir() + "/default_page.xml");
    //    _browser_pages->root_item()->field("id", "0");  // get_unical_id()
    //    _browser_pages->root_item()->field("name", "_shadow_branch");

    _tree_screen->reocrd_controller = std::make_shared<sd::_interface_const<sd::meta_info<void *>, RecordController *>>("", &RecordModel::reocrd_controller, this);
    _find_screen->reocrd_controller = std::make_shared<sd::_interface_const<sd::meta_info<void *>, RecordController *>>("", &RecordModel::reocrd_controller, this);
    init_source_model(_record_controller, _record_screen, _main_window, _editor_screen);

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
    delete _browser_pages;
    return;
}


// Предоставление данных табличной модели
// Provide tabular data model
QVariant RecordModel::data(const QModelIndex &index, int role) const
{
    // Если таблица данных не создана
    if(!_browser_pages)    // if(!_table)
        return QVariant();

    // Если таблица пустая
    if(0 == _browser_pages->direct_children_count()) // if(_table->size() == 0)
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

            QString field = _browser_pages->child(index.row())->field(fieldName);


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
            _browser_pages->child(index.row())->field("id");
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
    if(!_browser_pages)    // if(!_table)
        return false;

    // Если таблица пустая
    if(0 == _browser_pages->direct_children_count())    //if(_table->size() == 0)
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
            _browser_pages->child(index.row())->field(fieldName, cellValue);

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

    if(!_browser_pages)    // if(!_table)
        return 0;

    return _browser_pages->direct_children_count();
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

void RecordModel::remove_child(boost::intrusive_ptr<TreeItem> it)
{
    for(int i = 0; i < size(); i++) {
        if(item(i) == it) {
            removeRows(i, 1);
        }
    }
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
        critical_error("Bad arguments in RecordModel::removeRows(). row: " + QString::number(row) + " count: " + QString::number(count));
        return false;
    }

    //QModelIndex index = createIndex(row, 0);

    auto view = _reocrd_controller->view();

    beginRemoveRows(//index   //
        QModelIndex()
        , row, row + count - 1);


    // Удаляются строки непосредственно в таблице
    for(int i = row; i < row + count; ++i) {
        //        _table
        _browser_pages->remove_child(i);
        //        globalparameters.find_screen()->remove_row(i);  // ?
    }

    view->reset();
    view->setModel(this);
    endRemoveRows();

    return true;
}

//void RecordModel::tree_item(boost::intrusive_ptr<TreeItem> item)
//{
//    beginResetModel();
//    _shadow_branch_root = item;
//    endResetModel();
//}

void RecordModel::shadow_branch(ItemsFlat *_shadow_branch)
{
    beginResetModel();
    this->_browser_pages = _shadow_branch;
    endResetModel();
}

//void RecordModel::root(boost::intrusive_ptr<TreeItem> item)
//{
//    //void RecordModel::shadow_branch(TreeModelKnow *_shadow_branch)

//    beginResetModel();
//    _shadow_branch->root_item(item);
//    endResetModel();
//}

//// Установка данных в таблицу данных
//void RecordModel::reset_tabledata(std::shared_ptr<RecordTable> record_table)
//{
//    beginResetModel();
//    //    _table
//    _tree_item->record_table(record_table);

//    endResetModel();
//}


//// Получение ссылки на таблицу данных
//boost::intrusive_ptr<TreeItem> RecordModel::tree_item(void)
//{
//    // Возвращается ссылка на данные, с которыми в данный момент работает модель
//    return _tree_item;  // ->record_table(); // _table;
//}


// Добавление данных
// Функция возвращает позицию нового добавленного элемента
int RecordModel::insert_new_item(QModelIndex pos_index, boost::intrusive_ptr<TreeItem> item, int mode)
{
    beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows

    // Вставка новых данных в таблицу конечных записей
    int selected_position = _browser_pages->insert_new_item(pos_index.row(), item, mode);   // _table
    assert(_browser_pages->item(selected_position) == item);
    endResetModel(); // Подумать, возможно нужно заменить на endInsertRows

    return selected_position;
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

QString RecordModel::field(int pos, QString name)
{
    QString result = "";

    if(pos >= 0 && pos < _browser_pages->direct_children_count()) {
        result = _browser_pages->child(pos)->field(name);
    }

    return result;
}

void RecordModel::fields(int pos, QMap<QString, QString> data)
{
    if(pos >= 0 && pos < _browser_pages->direct_children_count()) {
        for(QMap<QString, QString>::iterator i = data.begin(); i != data.end(); i++) {
            _browser_pages->child(pos)->field(i.key(), i.value());
        }
    }
}

boost::intrusive_ptr<TreeItem> RecordModel::find(boost::intrusive_ptr<TreeItem> item)
{
    boost::intrusive_ptr<TreeItem> result = nullptr;

    for(int i = 0; i < _browser_pages->direct_children_count(); i++) {
        if(_browser_pages->child(i) == item) {
            result = item; break;
        }
    }

    return result;
}

int RecordModel::is_item_exists(QString find_id)
{
    int pos = -1;

    for(int i = 0; i < _browser_pages->direct_children_count(); i++) {
        if(_browser_pages->child(i)->id() == find_id) {
            pos = i;
            break;
        }
    }

    return pos;
}

bool RecordModel::remove_child(QString find_id)
{
    bool r = false;
    int pos = is_item_exists(find_id);

    if(pos != -1) {
        _browser_pages->remove_child(find_id);
        _reocrd_controller->view()->reset();
        _reocrd_controller->view()->setModel(this);
        r = true;
    }

    return r;
}


boost::intrusive_ptr<TreeItem> RecordModel::child(int pos)
{
    boost::intrusive_ptr<TreeItem> r = nullptr;

    if(pos >= 0 && pos < size()) {
        r = _browser_pages->child(pos);
    }

    return r;
}


void RecordModel::work_pos(int pos)
{
    _browser_pages->work_pos(pos);
}
