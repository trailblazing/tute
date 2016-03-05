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
#include "models/tree/KnowModel.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/browser/tabwidget.h"
#include "views/browser/browser.h"
#include "models/record_table/RecordProxyModel.h"


extern FixedParameters fixedparameters;
extern AppConfig appconfig;
extern GlobalParameters globalparameters;

pages_container::pages_container(browser::TabWidget *_tabmanager)
    : _tabmanager(// new browser::TabWidget(_browser, _record_controller)
          _tabmanager)
      //    , _browser_pages(new ItemsFlat())      //    , _table(new RecordTable(_tree_item, QDomElement()))
{}


pages_container::~pages_container()
{
    //    delete _browser_pages;
}

//void pages_container::browser_pages(ItemsFlat *_browser_pages)
//{
//    //    QList<boost::intrusive_ptr<TreeItem>> vw;

//    //    for(int i = 0; i < _tabmanager->count(); i++) {
//    //        vw.push_back(_tabmanager->webView(i)->page()->current_item());
//    //    }

//    for(int j = 0; j < _browser_pages->count(); j++) {
//        if(-1 == _tabmanager->indexOf(_browser_pages->child(j)->unique_page()->view())) {
//            _reocrd_controller->request_item(
//                _browser_pages->child(j)
//                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(browser::WebPage *)>>(
//                    ""
//                    , &browser::TabWidget::ActiveRecordBinder::binder
//                    , _tabmanager
//                )
//                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
//                    ""
//                    , &browser::TabWidget::ActiveRecordBinder::activator
//                    , _tabmanager
//                )
//            );
//            _browser_pages->child(j)->activate();
//            // _tabmanager->newTab(_browser_pages->child(j), false);
//        }
//    }

//}

//ItemsFlat *pages_container::browser_pages() // use as a signal?
//{
//    if(_browser_pages->count() != _tabmanager->count()) {
//        _browser_pages->clear();

//        for(int i = 0; i < _tabmanager->count(); i++) {
//            _browser_pages->insert_new_item(i, _tabmanager->webView(i)->page()->current_item());
//        }
//    }

//    return _browser_pages;
//}

//ItemsFlat *pages_container::browser_pages()const
//{
//    if(_browser_pages->count() != _tabmanager->count()) {
//        _browser_pages->clear();

//        for(int i = 0; i < _tabmanager->count(); i++) {
//            _browser_pages->insert_new_item(i, _tabmanager->webView(i)->page()->current_item());
//        }
//    }

//    return _browser_pages;
//}

//void RecordModel::init_source_model(RecordController    *_record_controller
//                                    , RecordScreen      *_record_screen
//                                    , MainWindow        *main_window
//                                    , MetaEditor        *_editor_screen)
//{
//    qDebug() << "In RecordController init_source_model() start";
//    //    shadow_branch(_shadow_branch);



//    // Обновление набора данных с последующим выбором первой строки
//    // может быть очень длительным, поэтому показывается что процесс выполняется
//    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
//    // qApp->setOverrideCursor(cursor_wait);
//    //    find_object<MainWindow>("mainwindow")
//    main_window->setCursor(Qt::BusyCursor);

//    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
//    //    _source_model->root(tree_item);    // reset_tabledata(table_data);

//    RecordView *_view = nullptr; // _record_controller->view();

//    if(!_record_controller->no_view()) {
//        _view = _record_controller->view();
//        // Надо обязательно сбросить selection model
//        _view->selectionModel()->clear();
//    }

//    // Если список конечных записей не пуст
//    bool remove_selection = true;

//    if(size() > 0) {
//        // Нужно выяснить, на какой записи ранее стояло выделение
//        int _work_pos = work_pos();

//        // Если номер записи допустимый
//        if(!_record_controller->no_view() && _work_pos > 0 && _work_pos < rowCount()) {
//            // Выделение устанавливается на нужную запись
//            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
//            _view->selectRow(_work_pos);
//            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

//            remove_selection = false;
//        }
//    }

//    // If the selection does not need to install    // Если выделение устанавливать ненужно
//    if(remove_selection) {
//        // Надо очистить поля области редактировния
//        //        find_object<MetaEditor>(meta_editor_singleton_name)
//        _editor_screen->clear_all();

//        // При выборе записи обновление инструментов было бы вызвано автоматически
//        // в альтернативной ветке (там "эмулируется" клик по записи)
//        // А так как записей нет, то обновление инструментов надо
//        // вызвать принудительно
//        if(_record_screen && _record_screen->inited()) // if(qobject_cast<RecordScreen *>(parent())->inited())
//            _record_screen->tools_update();
//    }

//    // qApp->restoreOverrideCursor();
//    //    find_object<MainWindow>("mainwindow")
//    main_window->unsetCursor();

//    //    globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

//    qDebug() << "In RecordTableView set new model stop";

//}


//void RecordModel::init_source_model(ItemsFlat *_browser_pages
//                                    , RecordController *_record_controller
//                                    , RecordScreen *_record_screen
//                                    , MainWindow *main_window
//                                    , MetaEditor *_editor_screen
//                                   )
//{
//    qDebug() << "In RecordController init_source_model() start";

//    // Обновление набора данных с последующим выбором первой строки
//    // может быть очень длительным, поэтому показывается что процесс выполняется
//    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
//    // qApp->setOverrideCursor(cursor_wait);
//    //    find_object<MainWindow>("mainwindow")
//    main_window->setCursor(Qt::BusyCursor);

//    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
//    this->browser_pages(_browser_pages);   // ?

//    RecordView *_view = nullptr; // _record_controller->view();

//    if(!_record_controller->no_view()) {
//        _view = _record_controller->view();
//        // Надо обязательно сбросить selection model
//        _view->selectionModel()->clear();
//    }

//    // Если список конечных записей не пуст
//    bool remove_selection = true;

//    if(size() > 0) {
//        // Нужно выяснить, на какой записи ранее стояло выделение
//        int _work_pos = work_pos();

//        // Если номер записи допустимый
//        if(!_record_controller->no_view() && _work_pos > 0 && _work_pos < rowCount()) {
//            // Выделение устанавливается на нужную запись
//            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
//            _view->selectRow(_work_pos);
//            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

//            remove_selection = false;
//        }
//    }

//    // If the selection does not need to install    // Если выделение устанавливать ненужно
//    if(remove_selection) {
//        // Надо очистить поля области редактировния
//        //        find_object<MetaEditor>(meta_editor_singleton_name)
//        _editor_screen->clear_all();

//        // При выборе записи обновление инструментов было бы вызвано автоматически
//        // в альтернативной ветке (там "эмулируется" клик по записи)
//        // А так как записей нет, то обновление инструментов надо
//        // вызвать принудительно
//        if(_record_screen && _record_screen->inited()) // if(qobject_cast<RecordScreen *>(parent())->inited())
//            _record_screen->tools_update();
//    }

//    // qApp->restoreOverrideCursor();
//    //    find_object<MainWindow>("mainwindow")
//    main_window->unsetCursor();

//    //    globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

//    qDebug() << "In RecordTableView set new model stop";
//}


//void RecordModel::init_source_model(boost::intrusive_ptr<TreeItem> tree_item
//                                    , RecordController *_record_controller
//                                    , RecordScreen *_record_screen
//                                    , MainWindow *main_window
//                                    , MetaEditor *_editor_screen
//                                   )
//{
//    init_source_model(tree_item.get(), _record_controller, _record_screen, main_window, _editor_screen);
//}

// Конструктор модели
RecordModel::RecordModel(//TreeScreen             *_tree_screen
    //, FindScreen           *_find_screen
    //,
    RecordController     *_record_controller
    , RecordScreen         *_record_screen
    , browser::TabWidget   *_tabmanager
)
    : QAbstractTableModel(_record_screen)
    , pages_container(_tabmanager)
    , _record_controller(_record_controller)
{
    //    _browser_pages->init_from_xml(_appconfig.get_tetradir() + "/default_page.xml");
    //    _browser_pages->root_item()->field("id", "0");  // get_unical_id()
    //    _browser_pages->root_item()->field("name", "_shadow_branch");

    //    init_source_model(_record_controller, _record_screen, _main_window, _editor_screen);

    //setObjectName(screen_name + "_source_model");

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
    // delete _browser_pages;   // current it created from tabmanager
    return;
}


// Предоставление данных табличной модели
// Provide tabular data model
QVariant RecordModel::data(const QModelIndex &index, int role) const
{
    //    // Если таблица данных не создана
    //    if(count() == 0)    // if(!browser_pages())    // if(!_table)
    //        return QVariant();

    // Если таблица пустая
    if(0 == count()) // if(_table->size() == 0)
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

            QString field = child(index.row())->field(fieldName);


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
            child(index.row())->field("id");
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
    //    // Если таблица данных не создана
    //    if(!browser_pages())    // if(!_table)
    //        return false;

    // Если таблица пустая
    if(0 == count())    //if(_table->size() == 0)
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
            child(index.row())->field(fieldName, cellValue);

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

    //    if(!browser_pages())    // if(!_table)
    //        return 0;

    return count();
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

    //    auto view = _reocrd_controller->view();

    beginRemoveRows(    //index   //
        QModelIndex(), row, row + count - 1);

    //    QVector<QString> ids;

    //    // Удаляются строки непосредственно в таблице
    //    for(int i = row; i < row + count; ++i) {
    //        ids.append(child(i)->id());

    //    }

    //    _record_controller->remove_children(ids); // recursive called

    //    view->reset();
    //    view->setModel(this);   // wrong! this is not the proxy model
    endRemoveRows();

    return true;
}

//void RecordModel::tree_item(boost::intrusive_ptr<TreeItem> item)
//{
//    beginResetModel();
//    _shadow_branch_root = item;
//    endResetModel();
//}

//void RecordModel::browser_pages(ItemsFlat *_browser_pages)
//{
//    beginResetModel();
//    //    pages_container::browser_pages(_shadow_branch);   // this->_browser_pages = _shadow_branch;

//    for(int j = 0; j < _browser_pages->count(); j++) {
//        if(-1 == _tabmanager->indexOf(_browser_pages->child(j)->unique_page()->view())) {
//            boost::shared_ptr<browser::TabWidget> _tabmanager_ptr = boost::make_shared<browser::TabWidget>(_tabmanager);
//            _reocrd_controller->request_item(
//                _browser_pages->child(j)
//                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>(TreeItem::*)(browser::WebPage *)>>(
//                    ""
//                    , &browser::TabWidget::ActiveRecordBinder::binder
//                    , _tabmanager_ptr
//                )
//                , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem>>>(
//                    ""
//                    , &browser::TabWidget::ActiveRecordBinder::activator
//                    , _tabmanager_ptr
//                )
//            );
//            _browser_pages->child(j)->activate();
//            // _tabmanager->newTab(_browser_pages->child(j), false);
//        }
//    }

//    endResetModel();
//}

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
int RecordModel::insert_new_item(QModelIndex pos_index, boost::intrusive_ptr<TreeItem> _item, int mode)
{
    Q_UNUSED(pos_index) // to be used
    Q_UNUSED(mode)      // to be used

    beginResetModel(); // Подумать, возможно нужно заменить на beginInsertRows
    browser::WebView *view = _item->activate();
    // Вставка новых данных в таблицу конечных записей

    // accomplished by TabWidget::addTab in TabWidget::newTab?
    int selected_position =
        _tabmanager->indexOf(view); // _tabmanager->insertTab(pos_index.row(), _item, mode);   // _table

    if(selected_position == -1) {
        view = _tabmanager->newTab(_item); // , _item->field("name")
        //addTab()-> wrong design, demage the function TabWidget::newTab and the function QTabWidget::addTab
    }

    selected_position = _tabmanager->indexOf(view);
    assert(item(selected_position) == _item);

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

    if(pos >= 0 && pos < count()) {
        result = child(pos)->field(name);
    }

    return result;
}

void RecordModel::fields(int pos, QMap<QString, QString> data)
{
    if(pos >= 0 && pos < count()) {
        for(QMap<QString, QString>::iterator i = data.begin(); i != data.end(); i++) {
            child(pos)->field(i.key(), i.value());
        }
    }
}


boost::intrusive_ptr<TreeItem> RecordModel::find_bind_in_list(boost::intrusive_ptr<TreeItem> item)const
{
    boost::intrusive_ptr<TreeItem> result = nullptr;

    for(int i = 0; i < count(); i++) {
        if( item->page_valid() && child(i)->unique_page() == item->unique_page()) {
            result = item; break;
        }
    }

    return result;
}

boost::intrusive_ptr<TreeItem> RecordModel::find_in_list(boost::intrusive_ptr<TreeItem> item)const
{
    boost::intrusive_ptr<TreeItem> result = nullptr;

    for(int i = 0; i < count(); i++) {
        if(child(i) == item) {
            result = item; break;
        }
    }

    return result;
}

boost::intrusive_ptr<TreeItem> RecordModel::find_in_list(QUrl _url)const
{
    boost::intrusive_ptr<TreeItem> result = nullptr;

    for(int i = 0; i < count(); i++) {
        if(child(i)->field("url") == _url.toString()) {
            result = child(i); break;
        }
    }

    return result;
}

int RecordModel::is_item_id_exists_in_list(QString find_id)
{
    int pos = -1;

    for(int i = 0; i < count(); i++) {
        if(child(i)->id() == find_id) {
            pos = i;
            break;
        }
    }

    return pos;
}

//bool RecordModel::remove_child(int index)
//{
//    bool r = false;

//    if(index > 0 && index < count()) {
//        beginRemoveRows(QModelIndex(), index, index);

//        //    if(index > 0 && index < count()) {
//        //        _tabmanager->closeTab(index) ;
//        //        //        _reocrd_controller->view()->reset();
//        //        //        _reocrd_controller->view()->setModel(_reocrd_controller->proxy_model());
//        //        r = true;

//        //    }
//        r = true;
//        endRemoveRows();
//    }

//    return r;
//}

//bool RecordModel::remove_child(QString find_id)
//{
//    bool r = false;
//    int index = is_item_exists(find_id);

//    if(index != -1) {
//        beginRemoveRows(QModelIndex(), index, index);

//        //    if(index != -1) {
//        //        _tabmanager->closeTab(position(find_id)) ;
//        //        //        _reocrd_controller->view()->reset();
//        //        //        _reocrd_controller->view()->setModel(_reocrd_controller->proxy_model());
//        //        r = true;

//        //    }

//        endRemoveRows();
//    }

//    return r;
//}

boost::intrusive_ptr<TreeItem> RecordModel::child(QString id)
{
    boost::intrusive_ptr<TreeItem> r = nullptr;

    for(int pos = 0; pos < _tabmanager->count(); pos++) {
        auto it = _tabmanager->webView(pos)->page()->current_item();

        if(it->id() == id) {
            r = it;
            break;
        }
    }

    return r;

}

boost::intrusive_ptr<TreeItem> RecordModel::child(QString id)const
{
    boost::intrusive_ptr<TreeItem> r = nullptr;

    for(int pos = 0; pos < _tabmanager->count(); pos++) {
        auto it = _tabmanager->webView(pos)->page()->current_item();

        if(it->id() == id) {
            r = it;
            break;
        }
    }

    return r;
}


boost::intrusive_ptr<TreeItem> RecordModel::child(int pos)const
{
    boost::intrusive_ptr<TreeItem> r = nullptr;

    if(pos >= 0 && pos < size()) {
        r = _tabmanager->webView(pos)->page()->current_item();
    }

    return r;
}

boost::intrusive_ptr<TreeItem> RecordModel::child(int pos)
{
    boost::intrusive_ptr<TreeItem> r = nullptr;

    if(pos >= 0 && pos < size()) {
        r = _tabmanager->webView(pos)->page()->current_item();
    }

    return r;
}


void RecordModel::work_pos(int pos)
{
    _tabmanager->setCurrentIndex(pos);
}



boost::intrusive_ptr<TreeItem> RecordModel::item_fat(int index)
{
    boost::intrusive_ptr<TreeItem> item = _tabmanager->webView(index)->page()->current_item();

    if(item->is_lite())item->to_fat();

    return item;
}


int RecordModel::position(QString id)
{
    int result = -1;

    for(int i = 0; i < _tabmanager->count(); i++) {
        if(_tabmanager->webView(i)->page()->current_item()->id() == id) {
            result = i;
            break;
        }
    }

    return result;
}

int RecordModel::locate(boost::intrusive_ptr<TreeItem> item)
{
    int result = -1;

    for(int i = 0; i < _tabmanager->count(); i++) {
        if(_tabmanager->webView(i)->page()->current_item() == item) {
            result = i;
            break;
        }
    }

    return result;
}


int RecordModel::count()const {return _tabmanager->count();}

int RecordModel::size() const {return _tabmanager->count();}

int RecordModel::work_pos()const {return _tabmanager->currentIndex();}

int RecordModel::move_up(const int pos)
{
    beginResetModel();

    int new_pos = pos;

    if(pos > 0) {
        new_pos = pos - 1;
        _tabmanager->moveTab(pos, new_pos);

    }

    endResetModel();
    return new_pos;
}

int RecordModel::move_dn(const int pos)
{
    beginResetModel();

    int new_pos = pos;

    if(pos < count() - 1) {
        new_pos = pos + 1;
        _tabmanager->moveTab(pos, new_pos);

    }

    endResetModel();
    return new_pos;
}
