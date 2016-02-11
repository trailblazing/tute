#include <QObject>
#include <QHeaderView>

#include "main.h"
#include "RecordController.h"
#include "controllers/attach_table/AttachTableController.h"
#include "views/record/MetaEditor.h"
#include "views/record/AddNewRecord.h"
#include "views/record_table/RecordView.h"
#include "views/record_table/RecordScreen.h"
#include "views/record_table/RecordPrint.h"
#include "views/main_window/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/app_config//AppConfigDialog.h"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "models/record_table/Record.h"
#include "models/record_table/ItemsFlat.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/RecordProxyModel.h"
#include "models/app_config/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "libraries/GlobalParameters.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/WalkHistory.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/DiskHelper.h"
#include <cassert>
#include "views/browser/webview.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "libraries/FlatControl.h"
#include "views/tree/TreeViewKnow.h"

extern GlobalParameters globalparameters;
extern AppConfig appconfig;
extern WalkHistory walkhistory;


RecordController::RecordController(QString screen_name, boost::intrusive_ptr<TreeItem> _shadow_branch_root, RecordScreen *table_screen)
    : QObject(table_screen)
    , _source_model(new RecordModel(screen_name, _shadow_branch_root, this))
    , _proxy_model(new RecordProxyModel(screen_name, this))
    , _view(new RecordView(screen_name, table_screen, this))   // , qobject_cast<QWidget * >(RecordTableScreen)
{
    setObjectName(screen_name + "_controller");
    // Инициализируется область со списком записей
    //    view = new RecordTableView(qobject_cast<QWidget *>(parent));   // Вид размещается внутри виджета Screen
    //    _view->setObjectName("recordTableView");
    //    view->setController(this);

    // Создание модели данных
    //    recordSourceModel = new RecordTableModel(this);
    //    _source_model->setObjectName("recordSourceModel");

    //    recordProxyModel = new RecordTableProxyModel(this);
    _proxy_model->setSourceModel(_source_model);
    //    _proxy_model->setObjectName("recordProxyModel");

    // Модель данных задается для вида
    _view->setModel(_proxy_model);
}


RecordController::~RecordController()
{
    delete _view;
    delete _proxy_model;
    delete _source_model;
}


void RecordController::init(void)
{
    _view->init();
}


RecordView *RecordController::view(void)
{
    return _view;
}


// Принимает индекс Proxy модели
// Accepts index Proxy models
void RecordController::click_item(const QModelIndex &index)
{
    // Так как, возможно, включена сортировка, индекс на экране преобразуется в обычный индекс
    QModelIndex sourceIndex = proxyindex_to_sourceindex(index);

    // Позиция записи в списке
    int source_pos = sourceIndex.row();
    qDebug() << "RecordTableView::onClickToRecord() : current item num " << source_pos;

    qobject_cast<RecordScreen *>(parent())->tools_update();
    // sychronize_metaeditor_to_record(source_pos);  // means update editor(source_pos);
    sychronize_attachtable_to_item(source_pos);
    update_browser(source_pos); // if new one, create it? no, you can't click a record which does not exist.


}

void RecordController::open_website(QModelIndex proxyIndex)
{
    qDebug() << "RecordTableController::editFieldContext()";

    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    int pos = sourceIndex.row(); // Номер строки в базе

    update_browser(pos);

    //    // Create input window after exiting the function window should retire  // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    //    //RecordInfoFieldsEditor editRecordWin;
    //    auto entrance = globalparameters.entrance();
    //    // Выясняется ссылка на таблицу конечных данных
    //    //    RecordTableData *table = recordSourceModel->getTableData();

    //    // Fields box filled with initial values    // Поля окна заполняются начальными значениями
    //    //editRecordWin.setField("name",  table->getField("name",   pos) );
    //    //editRecordWin.setField("author",table->getField("author", pos) );
    //    //editRecordWin.setField("url",   table->getField("url",    pos) );
    //    //editRecordWin.setField("tags",  table->getField("tags",   pos) );

    //    //    browser_view->loadUrl(pos);   //table->getField("url", pos)

    //    boost::intrusive_ptr<Record> record = this->table_model()->table_data()->record(pos);
    //    assert(record->is_registered());

    //    //    if(record->getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome)
    //    if(entrance && (!record->binder() || record->activator())) entrance->equip_registered(record);

    //    if(record->binder() && record->activator())record->active();


    //    //    int i = editRecordWin.exec();
    //    //    if(i == QDialog::Rejected)
    //    //        return; // Была нажата отмена, ничего ненужно делать

    //    //// Измененные данные записываются
    //    //    editField(pos,
    //    //              editRecordWin.getField("name"),
    //    //              editRecordWin.getField("author"),
    //    //              editRecordWin.getField("url"),
    //    //              editRecordWin.getField("tags"));

}

// you can't click a record which does not exist.
// you can switch between two already existing record from this
void RecordController::update_browser(const int source_pos)
{
    //RecordTableData *table = recordSourceModel->getTableData();

    browser::Entrance *entrance = globalparameters.entrance();

    //Record *current_record = browser_view->getCurrentRecord();


    //if(current_record) {

    //if(current_record->isLite())current_record->switchToFat();

    //bool existing = table->isRecordExists(current_record->getNaturalFieldSource("id"));

    //    QUrl _url = table->getField("url", pos); //reply->url();
    //    QString url = current_record->getNaturalFieldSource("url");
    //    std::string url_compare_stored = url.toStdString();
    //    std::string url_compare_get = _url.toString().toStdString();
    //    std::string compare = getDifference(url_compare_stored, url_compare_get);

    //    if(//!existing &&
    //        compare.size() != 0 && compare != "/") {
    //        //if(this->current_record->getNaturalFieldSource("url") != url.toString()) {
    //        Record record;

    //        //if(record.isLite())record.switchToFat();

    //        record.setNaturalFieldSource("id",   current_record->getNaturalFieldSource("id"));
    //        record.setNaturalFieldSource("name",   current_record->getNaturalFieldSource("name"));
    //        record.setNaturalFieldSource("author", current_record->getNaturalFieldSource("author"));
    //        record.setNaturalFieldSource("url",    _url.toString());    // only changed
    //        record.setNaturalFieldSource("tags",   current_record->getNaturalFieldSource("tags"));
    //        //record.setText(browser_view->getText());

    //        //this->current_record->setNaturalFieldSource("url", _url.toString());
    //        addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();

    //        browser_view->loadUrl(_url);

    //        //current_record = table->getRecordFat(pos); // wrong pos ! new position created //findRecord();
    //        assert(current_record->getNaturalFieldSource("url") == _url.toString());

    //        //browser_view->loadUrl(pos);

    //        //this->current_record->setNaturalFieldSource("url", url.toString());
    //        //this->loadChanged();
    //    } else {




    boost::intrusive_ptr<TreeItem> record = this->source_model()->tree_item()->item(source_pos);
    //    assert(record->is_registered());
    record->active_request(source_pos, 0);

    //    if(record->generator())record->generate();
    //    else if(entrance) entrance->active_record(record);





    //    entrance->activebrowser()->tabWidget()->view_index();
    //    if(record->page() == nullptr || record->page()->view() == nullptr)

    QString url = record->natural_field_source("url");

    //    browser::WebPage *page = record->binded_only_page();
    //    Record *old_record = nullptr;
    //    if(page)old_record = page->current_record();

    // assert(url != "");

    //    if(record != old_record) {

    if(url != ""
       && url != browser::Browser::_defaulthome
       //       && record->active_immediately()
      ) {

        //        // back compatible
        //        //record->setNaturalFieldSource("url", browser::DockedWindow::_defaulthome);
        //        if(// (!page && url != "")
        //            // || (page && url != "" && url != page->url().toString())  // page redirect to new record already!
        //            // || (page && !page->view()->hasFocus())
        //            (old_record && _state_check[old_record->getNaturalFieldSource("pin")] == Qt::Unchecked)
        //            || (page && !old_record)
        //        ) {
        //            page->load(record);
        //        } else

        if(entrance && !record->page_valid()    // unique_page()
          ) {    // !record->binder() || !record->activator())) {
            entrance->equip_registered(record);
        }

        //        assert(record->unique_page());    // not sure
        assert(record->binder());
        assert(record->activator());

        //        if(record->binder() && !record->unique_page())record->bind();
        //        else if(record->activator() && record->unique_page())record->active();  // if(entrance) entrance->active_record(record);
        if(record->binder() && record->activator())record->active();

        //        else if(entrance)
    }

    //    } else {
    //        browser::TabWidget *tabwidget = entrance->activebrowser()->tabWidget();

    //        if(tabwidget->currentWebView() != page->view()) {
    //            tabwidget->setCurrentIndex(tabwidget->webViewIndex(page->view()));
    //            // page->load(record);
    //            page->view()->show();
    //        }


    //    }

    //    loadUrl(pos);

    //    }

    //    if(browser_view->getCurrentRecord()->getNaturalFieldSource("url") != table->getField("url", pos))
    //        browser_view->loadUrl(QUrl(table->getField("url", pos)));
    //} else {
    //    browser_view->loadUrl(pos);
    //}

    //    Record *_record = getRecordTableModel()->getRecordTableData()->getRecord(pos);
    //    WebView *view = globalParameters.getBrowserView()->getBrowserWindow()->currentTab();
    //    view->bindRecord(_record);

}

// from this function,we would have found that without a full-stack controller, we can't synchronize editor content.
// so if our records are come from different tree path, we must switch the parent node, our give them a sharing parent.
// that's why we need a page_controller,  or we should implement a multi table screen architecture -- but with this design,
// we can not settle the medium results easily.
void RecordController::sychronize_metaeditor_to_item(const int pos)
{

    boost::intrusive_ptr<TreeItem> record = this->source_model()->tree_item()->item(pos);
    assert(record);
    // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь только получить данные из таблицы

    // Выясняется указатель на объект редактирования текста записи
    MetaEditor *meta_editor = find_object<MetaEditor>(meta_editor_singleton_name);
    assert(meta_editor);
    meta_editor->bind(record);

    // Выясняется ссылка на таблицу конечных данных
    auto item = _source_model->tree_item();

    // В таблице конечных данных запоминается какая запись была выбрана
    // чтобы затем при выборе этой же подветки засветка автоматически
    // установилась на последнюю рабочую запись
    item->work_pos(pos);


    // Устанавливается функция обратного вызова для записи данных
    meta_editor->save_callback(item->editor_save_callback);

    // Сохраняется текст и картинки в окне редактирования
    find_object<MainWindow>("mainwindow")->saveTextarea();


    // Для новой выбраной записи выясняется директория и основной файл
    QString currentDir = item->child(pos)->field("dir");
    QString currentFile = item->child(pos)->field("file");
    QString fullDir = appconfig.get_tetradir() + "/base/" + currentDir;
    QString fullFileName = fullDir + "/" + currentFile;
    qDebug() << " File " << fullFileName << "\n";

    // If the window contents of the record is already selected record  // Если в окне содержимого записи уже находится выбираемая запись
    if(meta_editor->work_directory() == fullDir
       && meta_editor->file_name() == currentFile
      ) {
        globalparameters.window_switcher()->switchFromRecordtableToRecord();
        return;
    }

    // Перед открытием редактора происходит попытка получения текста записи
    // Этот вызов создаст файл с текстом записи, если он еще не создан (подумать, переделать)
    // Before the opening of the editor it attempts to get the text records
    // This call will create a text file with the record if it is not already created (think remake)
    item->text(pos);

    // Редактору задаются имя файла и директории
    // И дается команда загрузки файла
    meta_editor->work_directory(fullDir);
    meta_editor->file_name(currentFile);

    // Если идет работа с зашифрованной записью
    // И если имя директории или имя файла пусты, то это означает что
    // запись не была расшифрована, и редактор должен просто показывать пустой текст
    // ничего не сохранять и не считывать
    qDebug() << "RecordTableView::onClickToRecord() : id " << item->child(pos)->field("id");
    qDebug() << "RecordTableView::onClickToRecord() : name " << item->child(pos)->field("name");
    qDebug() << "RecordTableView::onClickToRecord() : crypt " << item->child(pos)->field("crypt");

    if(item->child(pos)->field("crypt") == "1")
        if(fullDir.length() == 0 || currentFile.length() == 0)
            meta_editor->dir_file_empty_reaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

    // В редактор заносится информация, идет ли работа с зашифрованным текстом
    meta_editor->misc_field("crypt", item->child(pos)->field("crypt"));

    // В редакторе устанавливается функция обратного вызова для чтения данных
    meta_editor->load_callback(item->editor_load_callback);

    meta_editor->load_textarea();
    // edView->set_textarea(table->get_text(index.row()));

    // Заполняются прочие инфо-поля
    meta_editor->pin(item->child(pos)->field("pin"));
    meta_editor->name(item->child(pos)->field("name"));
    meta_editor->author(item->child(pos)->field("author"));
    meta_editor->home(item->child(pos)->field("home"));
    meta_editor->url(item->child(pos)->field("url"));
    meta_editor->tags(item->child(pos)->field("tags"));

    QString id = item->child(pos)->field("id");
    meta_editor->misc_field("id", id);

    meta_editor->misc_field("title", item->child(pos)->field("name"));

    // Устанавливается путь до ветки в которой лежит запись (в виде названий веток)
    QString path = qobject_cast<RecordScreen *>(parent())->tree_path();

    // В мобильном интерфейсе редактор должен показывать путь до записи
    if(appconfig.getInterfaceMode() == "mobile")
        meta_editor->tree_path(path);

    // В редакторе восстанавливается позиция курсора и прокрутки если это необходимо
    if(appconfig.getRememberCursorAtOrdinarySelection()) {
        meta_editor->cursor_position(walkhistory.getCursorPosition(id));
        meta_editor->scrollbar_position(walkhistory.getScrollBarPosition(id));
    }

    // Обновление иконки аттачей
    if(item->item(pos)->attach_table()->size() == 0)
        meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_not_exists);   // Если нет приаттаченных файлов
    else
        meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_exists);   // Есть приаттаченные файлы
}


void RecordController::sychronize_attachtable_to_item(const int pos)
{
    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Устанавливается таблица приаттаченных файлов
    AttachTableController *attachTableController = find_object<AttachTableController>("attachTableController");
    attachTableController->attach_table_data(table->item(pos)->attach_table());
}


bool RecordController::is_tree_item_exists(void)
{
    //    if(_source_model->table_data() == nullptr)
    //        return true;
    //    else
    //        return false;
    return _source_model->tree_item() ? true : false;
}

//// Set a new set of data to the list of recordings  // Установка нового набора данных для списка записей
//void TableController::reset_tabledata_test(TableData *rtData)
//{

//    //    _view->deleteLater();
//    _view->reset();

//    recordSourceModel->deleteLater();

//    recordSourceModel = new TableModel(this);
//    //    recordProxyModel->deleteLater();
//    //    recordProxyModel= new RecordTableProxyModel(this);
//    recordProxyModel->clear();
//    recordProxyModel->setSourceModel(recordSourceModel);

//    //    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
//    //    recordSourceModel->reset_tabledata(rtData);

//    _view->setModel(recordProxyModel);

//    reset_tabledata(rtData);
//}

RecordModel *RecordController::source_model() {return _source_model;}

boost::intrusive_ptr<TreeItem> RecordController::tree_item()
{
    return _source_model->tree_item();
}


void RecordController::tree_item(boost::intrusive_ptr<TreeItem> tree_item)
{
    qDebug() << "In RecordController reset_tree_item() start";

    // Обновление набора данных с последующим выбором первой строки
    // может быть очень длительным, поэтому показывается что процесс выполняется
    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
    // qApp->setOverrideCursor(cursor_wait);
    find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
    _source_model->tree_item(tree_item);    // reset_tabledata(table_data);

    // Надо обязательно сбросить selection model
    _view->selectionModel()->clear();

    // Если список конечных записей не пуст
    bool removeSelection = true;

    if(_source_model->rowCount() > 0) {
        // Нужно выяснить, на какой записи ранее стояло выделение
        int workPos = tree_item->work_pos();

        // Если номер записи допустимый
        if(workPos > 0 && workPos < _source_model->rowCount()) {
            // Выделение устанавливается на нужную запись
            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
            _view->selectRow(workPos);
            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

            removeSelection = false;
        }
    }

    // If the selection does not need to install    // Если выделение устанавливать ненужно
    if(removeSelection) {
        // Надо очистить поля области редактировния
        find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();

        // При выборе записи обновление инструментов было бы вызвано автоматически
        // в альтернативной ветке (там "эмулируется" клик по записи)
        // А так как записей нет, то обновление инструментов надо
        // вызвать принудительно
        qobject_cast<RecordScreen *>(parent())->tools_update();
    }

    // qApp->restoreOverrideCursor();
    find_object<MainWindow>("mainwindow")->unsetCursor();

    //    globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

    qDebug() << "In RecordTableView set new model stop";
}


//// Set a new set of data to the list of recordings  // Установка нового набора данных для списка записей
//void RecordController::reset_tabledata(std::shared_ptr<RecordTable> table_data)
//{
//    qDebug() << "In RecordTableView reset_tabledata() start";

//    // Обновление набора данных с последующим выбором первой строки
//    // может быть очень длительным, поэтому показывается что процесс выполняется
//    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
//    // qApp->setOverrideCursor(cursor_wait);
//    find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

//    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
//    _source_model->reset_tabledata(table_data);

//    // Надо обязательно сбросить selection model
//    _view->selectionModel()->clear();

//    // Если список конечных записей не пуст
//    bool removeSelection = true;

//    if(_source_model->rowCount() > 0) {
//        // Нужно выяснить, на какой записи ранее стояло выделение
//        int workPos = table_data->work_pos();

//        // Если номер записи допустимый
//        if(workPos > 0 && workPos < _source_model->rowCount()) {
//            // Выделение устанавливается на нужную запись
//            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
//            _view->selectRow(workPos);
//            _view->scrollTo(_view->currentIndex());   // QAbstractItemView::PositionAtCenter

//            removeSelection = false;
//        }
//    }

//    // If the selection does not need to install    // Если выделение устанавливать ненужно
//    if(removeSelection) {
//        // Надо очистить поля области редактировния
//        find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();

//        // При выборе записи обновление инструментов было бы вызвано автоматически
//        // в альтернативной ветке (там "эмулируется" клик по записи)
//        // А так как записей нет, то обновление инструментов надо
//        // вызвать принудительно
//        qobject_cast<RecordScreen *>(parent())->tools_update();
//    }

//    // qApp->restoreOverrideCursor();
//    find_object<MainWindow>("mainwindow")->unsetCursor();

//    qDebug() << "In RecordTableView set new model stop";
//}


// Заполнение переданного объекта буфера обмена данными из указанных записей
// Индексы QModelIndexList передаются от Proxy модели
// Fill the object passed clipboard data from these records
// Index QModelIndexList transferred from the Proxy pattern
void RecordController::add_items_to_clipboard(ClipboardRecords *clipboardRecords, QModelIndexList items_copy)
{
    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Перебираются записи и вносятся в буфер обмена
    for(int i = 0; i < items_copy.size(); ++i) {
        QModelIndex index = proxyindex_to_sourceindex(items_copy.at(i));

        // The image recording, including all text data (text records, property records list an attached file)        // Образ записи, включающий все текстовые данные (текст записи, свойства записи, перечень приаттаченных файлов)
        boost::intrusive_ptr<TreeItem> record = table->item_fat(index.row());

        clipboardRecords->add_record(record);
    }
}


int RecordController::row_count(void) const
{
    return _proxy_model->rowCount();
}


// Get the number of the first highlighted item on the screen   // Получение номера первого выделенного элемента на экране
int RecordController::first_selectionpos(void)const
{
    return _view->getFirstSelectionPos();
}


// Получение ID первого выделенного элемента на экране
QString RecordController::first_selectionid(void) const
{
    return _view->getFirstSelectionId();
}


void RecordController::select_pos(int pos)
{
    _view->setSelectionToPos(pos);
}


void RecordController::select_id(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Если таблица конечных данных задана
    // (Не задана таблица может быть по причине если ветка зашифрована и введен неверный пароль, или при вводе пароля была нажата отмена)
    if(table) {
        // Номер записи в Source данных
        int pos = table->get_pos_by_id(id);

        _view->setSelectionToPos(sourcepos_to_proxypos(pos));
    }
}


QModelIndex RecordController::id_to_sourceindex(QString id) const
{
    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Номер записи в Source данных
    int sourcePos = table->get_pos_by_id(id);

    return pos_to_sourceindex(sourcePos);
}


QModelIndex RecordController::id_to_proxyindex(QString id) const
{
    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Номер записи в Source данных
    int sourcePos = table->get_pos_by_id(id);
    int proxyPos = sourcepos_to_proxypos(sourcePos);

    return pos_to_proxyindex(proxyPos);
}


QModelIndex RecordController::pos_to_proxyindex(int pos) const
{
    if(pos < 0 || pos >= _proxy_model->rowCount())
        return QModelIndex();

    QModelIndex index = _proxy_model->index(pos, 0);

    return index;
}


QModelIndex RecordController::pos_to_sourceindex(int pos) const
{
    if(pos < 0 || pos >= _proxy_model->rowCount())
        return QModelIndex();

    QModelIndex proxyIndex = pos_to_proxyindex(pos);
    QModelIndex index = _proxy_model->mapToSource(proxyIndex);

    return index;
}


// Преобразование Proxy индекса в позицию на экране (так, как это будет выглядеть при Proxy модели)
int RecordController::proxyindex_to_pos(QModelIndex index) const
{
    if(!index.isValid())
        return -1;

    return index.row();
}


// Преобразование Source индекса в позицию на экране (так, как это будет выглядеть при Source модели)
int RecordController::sourceindex_to_pos(QModelIndex index) const
{
    if(!index.isValid())
        return -1;

    return index.row();
}


QModelIndex RecordController::proxyindex_to_sourceindex(QModelIndex proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();

    QModelIndex index = _proxy_model->mapToSource(proxyIndex);

    return index;
}


QModelIndex RecordController::sourceindex_to_proxyindex(QModelIndex sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    QModelIndex index = _proxy_model->mapFromSource(sourceIndex);

    return index;
}


int RecordController::sourcepos_to_proxypos(int sourcePos) const
{
    QModelIndex proxyIndex = _proxy_model->mapFromSource(_source_model->index(sourcePos, 0));
    return proxyIndex.row();
}


int RecordController::proxypos_to_sourcepos(int proxyPos) const
{
    QModelIndex sourceIndex = _proxy_model->mapToSource(_proxy_model->index(proxyPos, 0));
    return sourceIndex.row();
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void RecordController::cut(void)
{
    // Надо сохранить запись, так как перед копированием в буфер обмена запись
    // обязательно должна быть сохранена, иначе редактирование,
    // которое было после открытия записи и до нажатия Cut, потеряется
    find_object<MetaEditor>(meta_editor_singleton_name)->save_textarea();

    copy();
    delete_items_selected();
}


// Копирование отмеченных записей в буфер обмена
void RecordController::copy(void)
{
    // Объект с записями помещается в буфер обмена
    QApplication::clipboard() -> setMimeData(_view->getSelectedRecords());
}


// Insert records from the clipboard    // Вставка записей из буфера обмена
void RecordController::paste(void)
{
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if(mimeData == nullptr)
        return;

    if(!(mimeData->hasFormat("mytetra/records")))
        return;

    // Создается ссылка на буфер обмена
    QClipboard *clipboardBuf = QApplication::clipboard();

    // Извлечение объекта из буфера обмена
    // const clipboardrecords *rcd=new clipboardrecords();
    const ClipboardRecords *clipboardRecords;
    clipboardRecords = qobject_cast<const ClipboardRecords *>(clipboardBuf->mimeData());
    clipboardRecords->print();

    // Выясняется количество записей в буфере
    int nList = clipboardRecords->size();

    // Пробегаются все записи в буфере
    for(int i = 0; i < nList; i++)
        addnew_item(clipboardRecords->record(i), ADD_NEW_RECORD_TO_END);

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>(tree_screen_singleton_name)->update_selected_branchs();
}


// Слот для добавления новой записи в конец таблицы
void RecordController::addnew_to_end(void)
{
    qDebug() << "In slot add_new_toend_context()";

    addnew_blank(ADD_NEW_RECORD_TO_END);
}


// Слот для добавления новой записи перед выделенной строкой
void RecordController::addnew_before(void)
{
    qDebug() << "In slot add_new_before_context()";

    addnew_blank(ADD_NEW_RECORD_BEFORE);
}


// Слот для добавления новой записи после выделенной строки
// Slot to add a new record after the highlighted row
void RecordController::addnew_after(void)
{
    qDebug() << "In slot add_new_after_context()";

    addnew_blank(ADD_NEW_RECORD_AFTER);
}

//// Слот для добавления новой записи после выделенной строки
//// Slot to add a new record after the highlighted row
//void RecordTableController::autoAddNewAfterContext(void)
//{
//    qDebug() << "In slot add_new_after_context()";

//    autoAddNewRecord(ADD_NEW_RECORD_AFTER);
//}



//// Вызов окна добавления данных в таблицу конечных записей
//// Call window to add data to a table of final entries
//void RecordTableController::autoAddNewRecord(int mode)
//{
//    qDebug() << "In add_new_record()";

//    //// Создается окно ввода данных
//    //// При клике Ok внутри этого окна, будет создана временная директория
//    //// с картинками, содержащимися в тексте
//    //    AddNewRecord addNewRecordWin;
//    //    int i=addNewRecordWin.exec();
//    //    if(i==QDialog::Rejected)
//    //        return; // Была нажата отмена, ничего ненужно делать

//    //// Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
//    //    QString directory=addNewRecordWin.getImagesDirectory();

//    // todo: сделать заполнение таблицы приаттаченных файлов
//    BrowserView *browser_view = globalParameters.getBrowserView();
//    Record *current_view = browser_view->getCurrentRecord();

//    if(browser_view->getCurrentRecord()) {
//        if(!browser_view->getCurrentRecord()->isEmpty()) {

//            RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();
//            int pos = recordTableController->getFirstSelectionPos();

//            //RecordTableView *view = recordTableController->getView();
//            //view->loadUrl();

//            RecordTableModel *recordTableModel = recordTableController->getRecordTableModel();
//            RecordTableData *recordTableData = recordTableModel->getRecordTableData();
//            Record *record = recordTableData->getRecord(pos);

//            if(record->getNaturalFieldSource("url") != current_view->getNaturalFieldSource("url")) {
//                Record record;

//                //if(record.isLite())record.switchToFat();

//                record.setNaturalFieldSource("id",   browser_view->getNaturalFieldSource("id"));
//                record.setNaturalFieldSource("name",   browser_view->getNaturalFieldSource("name"));
//                record.setNaturalFieldSource("author", browser_view->getNaturalFieldSource("author"));
//                record.setNaturalFieldSource("url",    browser_view->getNaturalFieldSource("url"));
//                record.setNaturalFieldSource("tags",   browser_view->getNaturalFieldSource("tags"));
//                //record.setText(browser_view->getText());

//                //record.setPictureFiles( DiskHelper::getFilesFromDirectory(directory, "*.png") );

//                // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
//                // Запись должна быть создана, потом можно аттачить файлы.
//                // Это ограничение для "ленивого" программинга, но пока так
//                // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

//                //// Временная директория с картинками и приаттаченными файлами удаляется
//                //    DiskHelper::removeDirectory(directory);

//                // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
//                addNew(mode, record);
//            }
//        }
//    }
//}



// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
void RecordController::addnew_blank(int mode)
{
    qDebug() << "In add_new_record()";

    //    // Создается окно ввода данных
    //    // При клике Ok внутри этого окна, будет создана временная директория
    //    // с картинками, содержащимися в тексте
    //    AddNewRecord addNewRecordWin;

    //    int i = addNewRecordWin.exec();


    //    if(i == QDialog::Rejected)
    //        return; // Была нажата отмена, ничего ненужно делать

    // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
    QString directory = DiskHelper::createTempDirectory();  //
    // addNewRecordWin.getImagesDirectory();

    // todo: сделать заполнение таблицы приаттаченных файлов

    boost::intrusive_ptr<TreeItem> record = boost::intrusive_ptr<TreeItem>(new TreeItem(boost::intrusive_ptr<Record>(new Record()), _source_model->_shadow_branch_root));
    record->to_fat();
    //    record.setText(addNewRecordWin.getField("text"));
    //    record.setField("pin",   addNewRecordWin.getField("pin"));
    //    record.setField("name",   addNewRecordWin.getField("name"));
    //    record.setField("author", addNewRecordWin.getField("author"));
    //    record.setField("url",    addNewRecordWin.getField("url"));
    //    record.setField("tags",   addNewRecordWin.getField("tags"));
    record->text_to_fat("");
    record->field("pin",   _check_state[Qt::Unchecked]);
    record->field("name",   "");
    record->field("author", "");
    record->field("home",   browser::Browser::_defaulthome);
    record->field("url",    browser::Browser::_defaulthome);
    record->field("tags",   "");

    record->picture_files(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
    // Запись должна быть создана, потом можно аттачить файлы.
    // Это ограничение для "ленивого" программинга, но пока так
    // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

    // Временная директория с картинками и приаттаченными файлами удаляется
    DiskHelper::removeDirectory(directory);

    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
    addnew_item(record, mode);
}



// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
int RecordController::addnew_item_fat(boost::intrusive_ptr<TreeItem> item, const int mode)
{
    qDebug() << "In add_new_record()";

    //    // Создается окно ввода данных
    //    // При клике Ok внутри этого окна, будет создана временная директория
    //    // с картинками, содержащимися в тексте
    //    AddNewRecord addNewRecordWin;

    //    int i = addNewRecordWin.exec();


    //    if(i == QDialog::Rejected)
    //        return; // Была нажата отмена, ничего ненужно делать

    // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
    QString directory = DiskHelper::createTempDirectory();  //
    // addNewRecordWin.getImagesDirectory();

    // todo: сделать заполнение таблицы приаттаченных файлов

    //    Record record;

    //    if(record.isLite())record.switchToFat();

    assert(!item->is_lite());

    //    record.setText(addNewRecordWin.getField("text"));
    //    record.setField("pin",   addNewRecordWin.getField("pin"));
    //    record.setField("name",   addNewRecordWin.getField("name"));
    //    record.setField("author", addNewRecordWin.getField("author"));
    //    record.setField("url",    addNewRecordWin.getField("url"));
    //    record.setField("tags",   addNewRecordWin.getField("tags"));

    //    record.setText("");
    //    record.setField("pin",   _check_state[Qt::Unchecked]);
    //    record.setField("name",   "");
    //    record.setField("author", "");
    //    record.setField("home",   url.toString());
    //    record.setField("url",    url.toString());
    //    record.setField("tags",   "");

    item->picture_files(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    //    record->generator(generator);

    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
    // Запись должна быть создана, потом можно аттачить файлы.
    // Это ограничение для "ленивого" программинга, но пока так
    // record->setAttachFiles(DiskHelper::getFilesFromDirectory(directory, "*.bin"));

    // Временная директория с картинками и приаттаченными файлами удаляется
    DiskHelper::removeDirectory(directory);

    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
    return addnew_item(item, mode);
}


// Функция добавления новой записи в таблицу конечных записей
// Принимает полный формат записи
int RecordController::addnew_item(boost::intrusive_ptr<TreeItem> item, const int mode)
{
    qDebug() << "In add_new()";

    // Получение Source-индекса первой выделенной строки
    QModelIndex position_index = _view->first_selection_source_index();

    if(!position_index.isValid()) {
        position_index = _view->currentIndex();
    }

    if(!position_index.isValid()) {
        position_index = _source_model->createIndex(_source_model->tree_item()->size() - 1, 0, static_cast<void *>(_source_model->tree_item()->child(_source_model->tree_item()->size() - 1).get()));
    }

    //    assert(position_index.isValid());

    //    if(!position_index.isValid()) {
    //        position_index = view->getFirstSelectionProxyIndex();
    //    }

    //    if(position_index.isValid() //   // do not need? but if it is invalid, the sequence will be changed. hughvonyoung@gmail.com
    //       //       && item.getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome
    //      ) {

    // Вставка новых данных, возвращаемая позиция - это позиция в Source данных
    int selected_position = _source_model->insert_new_item(mode, position_index, item);

    assert(_source_model->tree_item()->item(selected_position) == item);

    _view->moveCursorToNewRecord(mode, sourcepos_to_proxypos(selected_position));   // modify _source_model? yeah

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
    //    }

    //    else {
    //        // Add a record in the database // Добавление записи в базу
    //        RecordTableData *table = nullptr;
    //        table = recordSourceModel->getRecordTableData();

    //        //        if(table == nullptr) {}

    //        int selPos = table->insertNewRecord(ADD_NEW_RECORD_TO_END, 0, record);

    //        view->moveCursorToNewRecord(mode, convertSourcePosToProxyPos(selPos));

    //        // Сохранение дерева веток
    //        find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
    //    }

    selected_position = _source_model->tree_item()->locate(item);
    assert(_source_model->tree_item()->item(selected_position) == item);

    return selected_position;
}


//// Функция добавления новой записи в таблицу конечных записей
//// Принимает полный формат записи
//int TableController::addnew_page_record(boost::intrusive_ptr<Record> record, int mode)
//{
//    qDebug() << "In add_new()";

//    // Получение Source-индекса первой выделенной строки
//    QModelIndex posIndex = _view->getFirstSelectionSourceIndex();

//    //    //    assert(posIndex.isValid());
//    //    if(!posIndex.isValid()) {
//    //        posIndex = view->getFirstSelectionProxyIndex();
//    //    }

//    //    if(posIndex.isValid() //   // do not need? yeah, I am sure. hughvonyoung@gmail.com
//    //       //       && record.getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome
//    //      ) {

//    // Вставка новых данных, возвращаемая позиция - это позиция в Source данных
//    int selected_position = _source_model->add_record(mode, posIndex, record);

//    _view->moveCursorToNewRecord(mode, sourcepos_to_proxypos(selected_position));

//    // Сохранение дерева веток
//    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

//    //    }

//    //    else {
//    //        // Add a record in the database // Добавление записи в базу
//    //        RecordTableData *table = nullptr;
//    //        table = recordSourceModel->getRecordTableData();

//    //        //        if(table == nullptr) {}

//    //        int selPos = table->insertNewRecord(ADD_NEW_RECORD_TO_END, 0, record);

//    //        view->moveCursorToNewRecord(mode, convertSourcePosToProxyPos(selPos));

//    //        // Сохранение дерева веток
//    //        find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
//    //    }

//    return selected_position;
//}



//// Вызов окна добавления данных в таблицу конечных записей
//// Call window to add data to a table of final entries
//int TableController::new_record_from_url(   // move to request_record(Url...
//    const QUrl &url
//    , const int mode
//    //    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
//)
//{
//    qDebug() << "In add_new_record()";

//    //    // Создается окно ввода данных
//    //    // При клике Ok внутри этого окна, будет создана временная директория
//    //    // с картинками, содержащимися в тексте
//    //    AddNewRecord addNewRecordWin;

//    //    int i = addNewRecordWin.exec();


//    //    if(i == QDialog::Rejected)
//    //        return; // Была нажата отмена, ничего ненужно делать

//    // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
//    QString directory = DiskHelper::createTempDirectory();  //
//    // addNewRecordWin.getImagesDirectory();

//    // todo: сделать заполнение таблицы приаттаченных файлов

//    boost::intrusive_ptr<Record> record = boost::intrusive_ptr<Record>(new Record());
//    record->switchToFat();
//    //    record.setText(addNewRecordWin.getField("text"));
//    //    record.setField("pin",   addNewRecordWin.getField("pin"));
//    //    record.setField("name",   addNewRecordWin.getField("name"));
//    //    record.setField("author", addNewRecordWin.getField("author"));
//    //    record.setField("url",    addNewRecordWin.getField("url"));
//    //    record.setField("tags",   addNewRecordWin.getField("tags"));
//    record->setTextToFat("");
//    record->setField("pin",   _check_state[Qt::Unchecked]);
//    record->setField("name",   "");
//    record->setField("author", "");
//    record->setField("home",   url.toString());
//    record->setField("url",    url.toString());
//    record->setField("tags",   "");

//    record->setPictureFiles(DiskHelper::getFilesFromDirectory(directory, "*.png"));

//    //    record.generator(generator);

//    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
//    // Запись должна быть создана, потом можно аттачить файлы.
//    // Это ограничение для "ленивого" программинга, но пока так
//    // record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

//    // Временная директория с картинками и приаттаченными файлами удаляется
//    DiskHelper::removeDirectory(directory);

//    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
//    return addnew_record(record, mode);
//}





void RecordController::on_edit_fieldcontext(void)
{
    _view->editFieldContext();
    //view->loadUrl();
}



// Действия при нажатии кнопки редактирования записи
void RecordController::edit_field_context(QModelIndex proxyIndex)
{
    qDebug() << "RecordTableController::editFieldContext()";

    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    int pos = sourceIndex.row(); // Номер строки в базе

    // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    InfoFieldsEditor editRecordWin;

    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Поля окна заполняются начальными значениями
    editRecordWin.setField("pin",       table->child(pos)->field("pin"));
    editRecordWin.setField("name",      table->child(pos)->field("name"));
    editRecordWin.setField("author",    table->child(pos)->field("author"));
    editRecordWin.setField("home",      table->child(pos)->field("home"));
    editRecordWin.setField("url",       table->child(pos)->field("url"));
    editRecordWin.setField("tags",      table->child(pos)->field("tags"));


    int i = editRecordWin.exec();

    if(i == QDialog::Rejected)
        return; // Была нажата отмена, ничего ненужно делать

    // Измененные данные записываются
    edit_field(pos,
               editRecordWin.getField("pin"),
               editRecordWin.getField("name"),
               editRecordWin.getField("author"),
               editRecordWin.getField("home"),
               editRecordWin.getField("url"),
               editRecordWin.getField("tags"));
}


// Функция сохранения отредактированных полей записи в таблицу конечных записей
void RecordController::edit_field(int pos
                                  , QString pin
                                  , QString name
                                  , QString author
                                  , QString home
                                  , QString url
                                  , QString tags
                                 )
{
    qDebug() << "In edit_field()";

    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    // Переданные отредактированные поля преобразуются в вид имя-значение
    QMap<QString, QString> edit_data;
    edit_data["pin"] = pin;
    edit_data["name"] = name;
    edit_data["author"] = author;
    edit_data["home"] = home;
    edit_data["url"] = url;
    edit_data["tags"] = tags;

    // Обновление новых данных в таблице конечных записей
    table->fields(pos, edit_data);

    // Обновление инфополей в области редактирования записи
    MetaEditor *meta_editor = find_object<MetaEditor>(meta_editor_singleton_name);
    meta_editor->pin(pin);
    meta_editor->name(name);
    meta_editor->author(author);
    meta_editor->url(url);
    meta_editor->tags(tags);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
}


// Обработка клика по удалению записи в контекстном меню и по кнопке на панели
void RecordController::delete_context(void)
{
    // Создается окно с вопросом нужно удалять запись (записи) или нет
    QMessageBox messageBox(_view);
    messageBox.setWindowTitle("Delete");
    messageBox.setText(tr("Are you sure to delete this record(s)?"));
    QAbstractButton *cancelButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    QAbstractButton *deleteButton = messageBox.addButton(tr("Delete"), QMessageBox::AcceptRole);

    Q_UNUSED(cancelButton);

    messageBox.exec();

    if(messageBox.clickedButton() == deleteButton) {
        // Выбранные данные удаляются
        delete_items_selected();
    }

}


// Удаление отмеченных записей
void RecordController::delete_items_selected(void)
{
    qDebug() << "RecordTableView::delete_records()";

    // Получение списка Item-элементов, подлежащих удалению. Индексы Proxy модели
    QModelIndexList itemsForDelete = _view->selectionModel()->selectedIndexes();

    // Проверка, выбраны ли записи
    if(itemsForDelete.count() == 0) {
        qDebug() << "Records for delete not selected.";

        QMessageBox msgBox;
        msgBox.setText("Please select at least one record for delete.");
        msgBox.exec();

        return;
    }

    // Сбор в массив всех идентификаторов, которые нужно удалить
    // Напрямую пробегать массив item-элементов и удалять из него нельзя
    // так как итератор начинает указывать на несуществующие элементы
    QVector<QString> delIds;
    QVector<int> delRows;
    QModelIndexList::iterator it;

    for(it = itemsForDelete.begin(); it != itemsForDelete.end(); it++) {
        QModelIndex currIdx;
        currIdx = *it;

        QString appendId = currIdx.data(RECORD_ID_ROLE).toString();

        // Если идентификатор не содержится в перечне удаляемых идентификаторов
        // это может произойти если видно несколько столбцов - у каждой ячейки будет один и тот же идентификатор записи
        if(!delIds.contains(appendId)) {
            qDebug() << "Mark for delete item id " << appendId;
            delIds.append(appendId);
            delRows.append(currIdx.row());
        }
    }

    // Массив удаляемых номеров строк (в Proxy-нумерации) сортируется так чтоб вначале были индексы с наибольшим номером
    qSort(delRows.begin(), delRows.end(), qGreater<int>());
    int lastRowNum = delRows[0]; // Максимальный номер удаляемой строки

    // Номер строки на который надо установить засветку после удаления
    // Засветка устанавливается на запись, следующую после последней удаляемой
    int selectionRowNum = lastRowNum + 1 - delRows.count();
    qDebug() << "After delete cursor set to" << selectionRowNum << "row";

    // Надо очистить поля области редактировния, чтобы редактор не пытался сохранить текущую открытую, но удаленную запись
    find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();

    // Вызывается удаление отмеченных записей
    removerows_by_idlist(delIds);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>(tree_screen_singleton_name)->update_selected_branchs();

    // Установка курсора на нужную позицию
    if(selectionRowNum >= 0 && selectionRowNum < _proxy_model->rowCount())
        _view->selectRow(selectionRowNum);

    // Если таблица конечных записей пуста
    if(_proxy_model->rowCount() == 0) {
        // Нужно очистить поле редактирования чтобы невидно было текста
        // последней удаленной записи
        find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();
    }

    qobject_cast<RecordScreen *>(parent())->tools_update();
}


// Удаление одной записи по идентификатору
void RecordController::removerow_by_id(QString delId)
{
    QVector<QString> delIds;
    delIds.append(delId);
    removerows_by_idlist(delIds);
}


// Remove records for the specified list of identifiers // Удаление записей по указанному списку идентификаторов
void RecordController::removerows_by_idlist(QVector<QString> delIds)
{
    qDebug() << "Remove rows by ID list: " << delIds;

    // Выясняется ссылка на таблицу конечных данных
    auto table = _source_model->tree_item();

    if(!table)
        return;

    for(int i = 0; i < delIds.count(); i++) {
        QString id = delIds[i];
        QModelIndex idx = id_to_proxyindex(id);

        // Удаляется строка в Proxy модели
        // Proxy модель сама должна уведомить вид о своем изменении, так как именно она подключена к виду
        _proxy_model->removeRow(idx.row());
        _view->reset();
        _view->setModel(_proxy_model);
        //        globalparameters.find_screen()->remove_id(id);  // ?
    }
}


// Перемещение записи вверх
void RecordController::move_up(void)
{
    qDebug() << "In moveup()";

    // Получение номера первой выделенной строки
    int pos = _view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    auto item = _source_model->tree_item();

    // Перемещение текущей записи вверх
    item->move_up(pos);

    // Установка засветки на перемещенную запись
    _view->setSelectionToPos(pos - 1);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
}


// Перемещение записи вниз
void RecordController::move_dn(void)
{
    qDebug() << "In movedn()";

    // Получение номера первой выделенной строки
    int pos = _view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    auto item = _source_model->tree_item();

    // Перемещение текущей записи вниз
    item->move_dn(pos);

    // Установка засветки на перемещенную запись
    _view->setSelectionToPos(pos + 1);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
}


// Клик по пункту "Сортировка" в контекстном меню
void RecordController::on_sort_click(void)
{
    RecordScreen *parentPointer = qobject_cast<RecordScreen *>(parent());

    // Если сортировка еще не включена
    if(!_view->isSortingEnabled()) {
        // Включается сортировка
        _view->setSortingEnabled(true);

        _proxy_model->setSortRole(SORT_ROLE); // Qt::DisplayRole

        // Включается сортировка по нужному столбцу
        int n = parentPointer->_sort->data().toInt(); // В actionSort хранится номер столбца, по которому нужно сортировать
        qDebug() << "Sort column number " << n;
        _proxy_model->sort(n);

        // Треугольничек сортировки переставляется на нужный столбец
        _view->horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);

        // Запрещается передвижение заголовков столбцов
        // так как после переноса неправильно устанавливается треугольничек сортировки, он остается на том же по счету столбце
        // horizontalHeader()->setSectionsMovable(false);
    } else {
        // Оменяется сортировка
        _view->setSortingEnabled(false);
        _proxy_model->setSortRole(Qt::InitialSortOrderRole);
        _proxy_model->invalidate();

        // Разрешается передвижение заголовков столбцов
        // horizontalHeader()->setSectionsMovable(true);
    }

    qobject_cast<RecordScreen *>(parent())->tools_update();
}


// Слот, срабатывающий при вызове настроек
void RecordController::settings(void)
{
    AppConfigDialog dialog("pageRecordTable");
    dialog.show();

    // Todo: Возвращение фокуса почему-то не работает, надо разбираться
    // (а может просто не выделяется виджет, в Qt5 вделенный виджет не виден в дефолтной схеме)
    // qDebug() << "Set focus to RecordTableView";
    // this->setFocus();
}


void RecordController::on_recordtable_configchange(void)
{
    _source_model->on_table_config_changed();
    _view->restoreColumnWidth();
    _view->restoreHeaderState();
}


void RecordController::on_print_click(void)
{
    RecordScreen *parentPointer = qobject_cast<RecordScreen *>(parent());

    RecordPrint printDialog(parentPointer);
    printDialog.setModel(_proxy_model);
    printDialog.generateHtmlTableFromModel();
    printDialog.setTitleToHtml(_source_model->tree_item()->path_as_name_with_delimiter(" / "));
    printDialog.exec();
}


boost::intrusive_ptr<TreeItem> RecordController::register_item_to_shadow_branch(boost::intrusive_ptr<TreeItem> item)
{
    //    assert(record_controller);
    auto _shadow_branch_root = this->source_model()->tree_item();
    assert(_shadow_branch_root);

    //    Record record;

    //    if(record.isLite())record.switchToFat();
    //    assert(!item->is_lite());
    if(item->is_lite())item->to_fat();

    int source_position = this->addnew_item_fat(item, ADD_NEW_RECORD_AFTER); //recordTableController->autoAddNewAfterContext();
    assert(source_position != -1);

    //    assert(source_position == source_model()->_shadow_branch_root->size() - 1);

    //    Record *_record = nullptr;
    //    _record = recordtabledata->record(_url);    // does not work every time? still not update now?

    //                int pos = _record_controller->getFirstSelectionPos();

    auto _item = _shadow_branch_root->item(source_position);
    //    int source_position_ = item->sibling_order();   // from treemodelknow->_root_item
    //    auto _item_ = _shadow_branch_root->item(source_position_);
    assert(_item.get() == item.get());
    //    assert(_item_.get() == item.get());
    //assert(record == _record);
    assert(_item->natural_field_source("url") == item->natural_field_source("url"));
    //            }
    //assert(_record);
    return _item; //_record;
}

//Record *register_record(const QUrl &_url
//                        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
//                        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> activator
//                        , RecordTableController *_record_ontroller)
//{

//    assert(_record_ontroller);
//    RecordTableData *recordtabledata = _record_ontroller->getRecordTableModel()->getRecordTableData();
//    assert(recordtabledata);

//    Record record;

//    if(record.isLite())record.switchToFat();

//    //                QString title = _url.toString(); // not ready yet

//    record.setNaturalFieldSource("pin",     "");
//    record.setNaturalFieldSource("name",    "");
//    record.setNaturalFieldSource("author",  "");
//    record.setNaturalFieldSource("home",    _url.toString());
//    record.setNaturalFieldSource("url",     _url.toString());    // only changed
//    record.setNaturalFieldSource("tags",    "");

//    int source_position = _record_ontroller->new_record(_url, ADD_NEW_RECORD_AFTER, generator); //recordTableController->autoAddNewAfterContext();

//    //    Record *_record = nullptr;
//    //    _record = recordtabledata->record(_url);    // does not work every time? still not update now?

//    //                int pos = _record_ontroller->getFirstSelectionPos();
//    auto record_ = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(source_position);

//    //assert(record_ == _record);
//    assert(record_->getNaturalFieldSource("url") == _url.toString());
//    //            }
//    //assert(_record);
//    return record_; //_record;
//}

boost::intrusive_ptr<TreeItem> RecordController::check_item(const QUrl &_url)
{
    boost::intrusive_ptr<TreeItem> _record = nullptr;


    //    TableController *_record_controller = globalparameters.table_screen()->table_controller();
    //    assert(_record_controller);

    //    if(_record_controller) {
    auto recordtabledata = this->source_model()->tree_item();
    assert(recordtabledata);

    if(recordtabledata) {
        _record = recordtabledata->find(_url);
    }

    //    }

    return _record;
}

//namespace browser {
//    class Browser;
//    class WebView;
//}

boost::intrusive_ptr<TreeItem> RecordController::request_item(boost::intrusive_ptr<TreeItem> item
        , bind_helper generator
        , active_helper activator
                                                             )
{
    item->binder(generator);
    item->activator(activator);
    boost::intrusive_ptr<TreeItem> _source_item = nullptr;
    boost::intrusive_ptr<TreeItem> _item = nullptr;
    //    TableController *_record_controller = globalparameters.table_screen()->table_controller();
    //    assert(_record_controller);
    auto _treemodelknow = globalparameters.tree_screen()->_root;
    _source_item = _treemodelknow->_root_item->find(item);
    //    if(_record_controller) {
    auto _shadow_branch_root = this->source_model()->tree_item();
    assert(_shadow_branch_root);

    if(_shadow_branch_root) {
        _item = _shadow_branch_root->find(item);

        if(_source_item) {
            if(!_item) {
                //                record->binder(generator);
                //                record->activator(activator);

                _source_item->binder(generator);
                _source_item->activator(activator);

                _item = register_item_to_shadow_branch(_source_item);
                //                assert(_record);

                //                _record->active_immediately(active_immediately);
                //                _record->generator(generator);
                assert(_item.get() == _source_item.get());
            }
        } else {
            if(!_item) {
                item->binder(generator);
                item->activator(activator);

                _item = register_item_to_shadow_branch(item);

                assert(_item.get() == item.get());
            }

            auto it = _treemodelknow->item(globalparameters.tree_screen()->current_index());
            assert(it);

            if(_item->is_lite())_item->to_fat();

            if(it != _item && !it->find(_item))
                it->insert_new_item(it->size() - 1, _item);

            _treemodelknow->save();
        }

        //        //            else {
        //        _item->binder(generator);
        //        _item->activator(activator);
        //        //                _record->generate();
        //        //            }

        assert(_item);

        assert(_item->is_registered());

    }

    //    }

    //    }

    //    assert(_record);

    return _item;

}

boost::intrusive_ptr<TreeItem> RecordController::request_item(const QUrl &_url
        , bind_helper generator
        , active_helper activator
                                                             )
{
    boost::intrusive_ptr<TreeItem> _source_item = nullptr;
    boost::intrusive_ptr<TreeItem> _item = nullptr;

    //    QString l = _url.toString();

    //    if(_url.toString() == browser::DockedWindow::_defaulthome) {
    //        if(default_record == nullptr) {
    //            default_record = new Record();

    //            if(default_record && default_record->isLite())default_record->switchToFat();

    //            //                QString title = _url.toString(); // not ready yet
    //            if(default_record) {
    //                default_record->setNaturalFieldSource("pin",   "");
    //                default_record->setNaturalFieldSource("name",   "");
    //                default_record->setNaturalFieldSource("author", "");
    //                default_record->setNaturalFieldSource("url",    _url.toString());    // only changed
    //                default_record->setNaturalFieldSource("tags",   "");
    //            }
    //        }

    //        _record = default_record;

    //    } else {

    //    TableController *_record_controller = globalparameters.table_screen()->table_controller();
    //    assert(_record_controller);

    auto _treemodelknow = globalparameters.tree_screen()->_root;
    _source_item = _treemodelknow->_root_item->find(_url);

    //    if(_record_controller) {
    auto _shadow_branch_root = this->source_model()->tree_item();
    assert(_shadow_branch_root);

    if(_shadow_branch_root) {
        _item = _shadow_branch_root->find(_url);

        if(_source_item) {
            if(!_item) {

                _source_item->binder(generator);
                _source_item->activator(activator);

                _item = register_item_to_shadow_branch(_source_item);

            } else {
                _item->binder(generator);
                _item->activator(activator);
            }
        } else {
            if(!_item) {

                //                int pos = _record_ontroller->getFirstSelectionPos();
                //                Record *previous_record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

                //                if(previous_record) {

                //                    Record record;

                //                    if(record.isLite())record.switchToFat();

                //                    //QString title = d->view->title(); // not ready yet
                //                    //record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));   // id concept?
                //                    record.setNaturalFieldSource("pin",   "");
                //                    record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
                //                    record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
                //                    record.setNaturalFieldSource("url",    _url.toString());    // only changed
                //                    record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

                //                    _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
                //                    _record = recordtabledata->getRecordByUrl(_url);
                //                    //                int pos = _record_ontroller->getFirstSelectionPos();
                //                    //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);
                //                } else {




                //    record.generator(generator);


                // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
                QString directory = DiskHelper::createTempDirectory();  //

                boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(new TreeItem(boost::intrusive_ptr<Record>(new Record()), _source_model->_shadow_branch_root));

                //                if(record.isLite())
                item->to_fat();

                //                QString title = _url.toString(); // not ready yet

                item->natural_field_source("pin",     _check_state[Qt::Unchecked]);
                item->natural_field_source("name",    "");
                item->natural_field_source("author",  "");
                item->natural_field_source("home",    _url.toString());    // only changed
                item->natural_field_source("url",     _url.toString());    // only changed
                item->natural_field_source("tags",    "");

                //                _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
                //                _record = recordtabledata->getRecordByUrl(_url);
                //                //                int pos = _record_ontroller->getFirstSelectionPos();
                //                //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

                //                //            }

                //                record->binder(generator);
                //                record->activator(activator);

                item->picture_files(DiskHelper::getFilesFromDirectory(directory, "*.png"));


                // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
                // Запись должна быть создана, потом можно аттачить файлы.
                // Это ограничение для "ленивого" программинга, но пока так
                // record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

                // Временная директория с картинками и приаттаченными файлами удаляется
                DiskHelper::removeDirectory(directory);

                item->binder(generator);
                item->activator(activator);

                _item = register_item_to_shadow_branch(item);

                //                assert(_record);
                //                assert(_record->is_registered());
                //                _record->active_immediately(active_immediately);
                //                _record->generator(generator);

                assert(_item.get() == item.get());
            } else {
                _item->binder(generator);
                _item->activator(activator);
            }

            auto it = _treemodelknow->item(globalparameters.tree_screen()->current_index());
            assert(it);

            if(_item->is_lite())_item->to_fat();

            if(it != _item && !it->find(_item))
                it->insert_new_item(it->size() - 1, _item);

            _treemodelknow->save();
        }

        //        //            else {
        //        //                //                assert(_record->is_registered());
        //        //                _record->binder(generator);
        //        //                _record->activator(activator);
        //        //                //                _record->generate();    // why?
        //        //            }

        //        _item->binder(generator);
        //        _item->activator(activator);

        assert(_item);
        assert(_item->is_registered());
    }

    //    }

    //    }

    //    assert(_record);

    return _item;
}
