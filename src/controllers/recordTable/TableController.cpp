#include <QObject>
#include <QHeaderView>

#include "main.h"
#include "TableController.h"
#include "controllers/attachTable/AttachTableController.h"
#include "views/record/MetaEditor.h"
#include "views/record/AddNewRecord.h"
#include "views/recordTable/TableView.h"
#include "views/recordTable/TableScreen.h"
#include "views/recordTable/TablePrint.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/appConfigWindow/AppConfigDialog.h"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "models/recordTable/Record.h"
#include "models/recordTable/TableData.h"
#include "models/recordTable/TableModel.h"
#include "models/recordTable/TableProxyModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "libraries/GlobalParameters.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/WalkHistory.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/DiskHelper.h"
#include <cassert>
#include "views/browser/webview.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/FlatControl.h"


extern GlobalParameters globalparameters;
extern AppConfig appconfig;
extern WalkHistory walkhistory;


TableController::TableController(TableScreen *recordtablescreen)
    : QObject(recordtablescreen)
    , _view(new TableView(recordtablescreen, this))    // , qobject_cast<QWidget * >(RecordTableScreen)
    , recordSourceModel(new TableModel(this))
    , recordProxyModel(new TableProxyModel(this))
{
    // Инициализируется область со списком записей
    //    view = new RecordTableView(qobject_cast<QWidget *>(parent));   // Вид размещается внутри виджета Screen
    _view->setObjectName("recordTableView");
    //    view->setController(this);

    // Создание модели данных
    //    recordSourceModel = new RecordTableModel(this);
    recordSourceModel->setObjectName("recordSourceModel");

    //    recordProxyModel = new RecordTableProxyModel(this);
    recordProxyModel->setSourceModel(recordSourceModel);
    recordProxyModel->setObjectName("recordProxyModel");

    // Модель данных задается для вида
    _view->setModel(recordProxyModel);
}


TableController::~TableController()
{

}


void TableController::init(void)
{
    _view->init();
}


TableView *TableController::view(void)
{
    return _view;
}


// Принимает индекс Proxy модели
// Accepts index Proxy models
void TableController::click_record(const QModelIndex &index)
{
    // Так как, возможно, включена сортировка, индекс на экране преобразуется в обычный индекс
    QModelIndex sourceIndex = proxyindex_to_sourceindex(index);

    // Позиция записи в списке
    int source_pos = sourceIndex.row();
    qDebug() << "RecordTableView::onClickToRecord() : current item num " << source_pos;


    sychronize_metaeditor_to_record(source_pos);  // means update editor(source_pos);
    sychronize_attachtable_to_record(source_pos);
    update_browser(source_pos); // if new one, create it? no, you can't click a record which does not exist.


}

// you can't click a record which does not exist.
// you can switch between two already existing record from this
void TableController::update_browser(const int source_pos)
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




    std::shared_ptr<Record> record = this->recordtable_model()->getRecordTableData()->record(source_pos);
    assert(record->is_registered());
    record->active_request(source_pos, 0);

    //    if(record->generator())record->generate();
    //    else if(entrance) entrance->active_record(record);





    //    entrance->activebrowser()->tabWidget()->view_index();
    //    if(record->page() == nullptr || record->page()->view() == nullptr)

    QString url = record->getNaturalFieldSource("url");

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

        if(entrance && (!record->binder() || !record->activator())) {
            entrance->equip_registered(record);
        }

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

void TableController::sychronize_metaeditor_to_record(const int pos)
{

    std::shared_ptr<Record> record = this->recordtable_model()->getRecordTableData()->record(pos);
    assert(record);
    // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь только получить данные из таблицы

    // Выясняется указатель на объект редактирования текста записи
    MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");
    assert(metaeditor);
    metaeditor->bind(record);

    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // В таблице конечных данных запоминается какая запись была выбрана
    // чтобы затем при выборе этой же подветки засветка автоматически
    // установилась на последнюю рабочую запись
    table->work_pos(pos);


    // Устанавливается функция обратного вызова для записи данных
    metaeditor->set_save_callback(table->editor_save_callback);

    // Сохраняется текст и картинки в окне редактирования
    find_object<MainWindow>("mainwindow")->saveTextarea();


    // Для новой выбраной записи выясняется директория и основной файл
    QString currentDir = table->field("dir", pos);
    QString currentFile = table->field("file", pos);
    QString fullDir = appconfig.get_tetradir() + "/base/" + currentDir;
    QString fullFileName = fullDir + "/" + currentFile;
    qDebug() << " File " << fullFileName << "\n";

    // If the window contents of the record is already selected record  // Если в окне содержимого записи уже находится выбираемая запись
    if(metaeditor->get_work_directory() == fullDir
       && metaeditor->get_file_name() == currentFile
      ) {
        globalparameters.getWindowSwitcher()->switchFromRecordtableToRecord();
        return;
    }

    // Перед открытием редактора происходит попытка получения текста записи
    // Этот вызов создаст файл с текстом записи, если он еще не создан (подумать, переделать)
    // Before the opening of the editor it attempts to get the text records
    // This call will create a text file with the record if it is not already created (think remake)
    table->text(pos);

    // Редактору задаются имя файла и директории
    // И дается команда загрузки файла
    metaeditor->set_work_directory(fullDir);
    metaeditor->set_file_name(currentFile);

    // Если идет работа с зашифрованной записью
    // И если имя директории или имя файла пусты, то это означает что
    // запись не была расшифрована, и редактор должен просто показывать пустой текст
    // ничего не сохранять и не считывать
    qDebug() << "RecordTableView::onClickToRecord() : id " << table->field("id", pos);
    qDebug() << "RecordTableView::onClickToRecord() : name " << table->field("name", pos);
    qDebug() << "RecordTableView::onClickToRecord() : crypt " << table->field("crypt", pos);

    if(table->field("crypt", pos) == "1")
        if(fullDir.length() == 0 || currentFile.length() == 0)
            metaeditor->setDirFileEmptyReaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

    // В редактор заносится информация, идет ли работа с зашифрованным текстом
    metaeditor->setMiscField("crypt", table->field("crypt", pos));

    // В редакторе устанавливается функция обратного вызова для чтения данных
    metaeditor->set_load_callback(table->editor_load_callback);

    metaeditor->load_textarea();
    // edView->set_textarea(table->get_text(index.row()));

    // Заполняются прочие инфо-поля
    metaeditor->setName(table->field("name", pos));
    metaeditor->setAuthor(table->field("author", pos));
    metaeditor->setHome(table->field("home", pos));
    metaeditor->setUrl(table->field("url", pos));
    metaeditor->setTags(table->field("tags", pos));

    QString id = table->field("id", pos);
    metaeditor->setMiscField("id", id);

    metaeditor->setMiscField("title", table->field("name", pos));

    // Устанавливается путь до ветки в которой лежит запись (в виде названий веток)
    QString path = qobject_cast<TableScreen *>(parent())->getTreePath();

    // В мобильном интерфейсе редактор должен показывать путь до записи
    if(appconfig.getInterfaceMode() == "mobile")
        metaeditor->setTreePath(path);

    // В редакторе восстанавливается позиция курсора и прокрутки если это необходимо
    if(appconfig.getRememberCursorAtOrdinarySelection()) {
        metaeditor->setCursorPosition(walkhistory.getCursorPosition(id));
        metaeditor->setScrollBarPosition(walkhistory.getScrollBarPosition(id));
    }

    // Обновление иконки аттачей
    if(table->record(pos)->getAttachTablePointer()->size() == 0)
        metaeditor->toAttach->setIcon(metaeditor->iconAttachNotExists);   // Если нет приаттаченных файлов
    else
        metaeditor->toAttach->setIcon(metaeditor->iconAttachExists);   // Есть приаттаченные файлы
}


void TableController::sychronize_attachtable_to_record(const int pos)
{
    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Устанавливается таблица приаттаченных файлов
    AttachTableController *attachTableController = find_object<AttachTableController>("attachTableController");
    attachTableController->setAttachTableData(table->record(pos)->getAttachTablePointer());
}


bool TableController::is_table_notexists(void)
{
    if(recordSourceModel->getTableData() == NULL)
        return true;
    else
        return false;
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

// Set a new set of data to the list of recordings  // Установка нового набора данных для списка записей
void TableController::reset_tabledata(std::shared_ptr<TableData> rtData)
{
    qDebug() << "In RecordTableView setTableData() start";

    // Обновление набора данных с последующим выбором первой строки
    // может быть очень длительным, поэтому показывается что процесс выполняется
    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
    // qApp->setOverrideCursor(cursor_wait);
    find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
    recordSourceModel->reset_tabledata(rtData);

    // Надо обязательно сбросить selection model
    _view->selectionModel()->clear();

    // Если список конечных записей не пуст
    bool removeSelection = true;

    if(recordSourceModel->rowCount() > 0) {
        // Нужно выяснить, на какой записи ранее стояло выделение
        int workPos = rtData->work_pos();

        // Если номер записи допустимый
        if(workPos > 0 && workPos < recordSourceModel->rowCount()) {
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
        find_object<MetaEditor>("editorScreen")->clearAll();

        // При выборе записи обновление инструментов было бы вызвано автоматически
        // в альтернативной ветке (там "эмулируется" клик по записи)
        // А так как записей нет, то обновление инструментов надо
        // вызвать принудительно
        qobject_cast<TableScreen *>(parent())->toolsUpdate();
    }

    // qApp->restoreOverrideCursor();
    find_object<MainWindow>("mainwindow")->unsetCursor();

    qDebug() << "In RecordTableView set new model stop";
}


// Заполнение переданного объекта буфера обмена данными из указанных записей
// Индексы QModelIndexList передаются от Proxy модели
// Fill the object passed clipboard data from these records
// Index QModelIndexList transferred from the Proxy pattern
void TableController::add_records_toclipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy)
{
    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Перебираются записи и вносятся в буфер обмена
    for(int i = 0; i < itemsForCopy.size(); ++i) {
        QModelIndex index = proxyindex_to_sourceindex(itemsForCopy.at(i));

        // The image recording, including all text data (text records, property records list an attached file)        // Образ записи, включающий все текстовые данные (текст записи, свойства записи, перечень приаттаченных файлов)
        std::shared_ptr<Record> record = table->record_fat(index.row());

        clipboardRecords->addRecord(record);
    }
}


int TableController::row_count(void)
{
    return recordProxyModel->rowCount();
}


// Get the number of the first highlighted item on the screen   // Получение номера первого выделенного элемента на экране
int TableController::first_selectionpos(void)
{
    return _view->getFirstSelectionPos();
}


// Получение ID первого выделенного элемента на экране
QString TableController::first_selectionid(void)
{
    return _view->getFirstSelectionId();
}


void TableController::select_pos(int pos)
{
    _view->setSelectionToPos(pos);
}


void TableController::select_id(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Если таблица конечных данных задана
    // (Не задана таблица может быть по причине если ветка зашифрована и введен неверный пароль, или при вводе пароля была нажата отмена)
    if(table) {
        // Номер записи в Source данных
        int pos = table->get_pos_by_id(id);

        _view->setSelectionToPos(sourcepos_to_proxypos(pos));
    }
}


QModelIndex TableController::id_to_sourceindex(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Номер записи в Source данных
    int sourcePos = table->get_pos_by_id(id);

    return pos_to_sourceindex(sourcePos);
}


QModelIndex TableController::id_to_proxyindex(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Номер записи в Source данных
    int sourcePos = table->get_pos_by_id(id);
    int proxyPos = sourcepos_to_proxypos(sourcePos);

    return pos_to_proxyindex(proxyPos);
}


QModelIndex TableController::pos_to_proxyindex(int pos)
{
    if(pos < 0 || pos >= recordProxyModel->rowCount())
        return QModelIndex();

    QModelIndex index = recordProxyModel->index(pos, 0);

    return index;
}


QModelIndex TableController::pos_to_sourceindex(int pos)
{
    if(pos < 0 || pos >= recordProxyModel->rowCount())
        return QModelIndex();

    QModelIndex proxyIndex = pos_to_proxyindex(pos);
    QModelIndex index = recordProxyModel->mapToSource(proxyIndex);

    return index;
}


// Преобразование Proxy индекса в позицию на экране (так, как это будет выглядеть при Proxy модели)
int TableController::proxyindex_to_pos(QModelIndex index)
{
    if(!index.isValid())
        return -1;

    return index.row();
}


// Преобразование Source индекса в позицию на экране (так, как это будет выглядеть при Source модели)
int TableController::sourceindex_to_pos(QModelIndex index)
{
    if(!index.isValid())
        return -1;

    return index.row();
}


QModelIndex TableController::proxyindex_to_sourceindex(QModelIndex proxyIndex)
{
    if(!proxyIndex.isValid())
        return QModelIndex();

    QModelIndex index = recordProxyModel->mapToSource(proxyIndex);

    return index;
}


QModelIndex TableController::sourceindex_to_proxyindex(QModelIndex sourceIndex)
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    QModelIndex index = recordProxyModel->mapFromSource(sourceIndex);

    return index;
}


int TableController::sourcepos_to_proxypos(int sourcePos)
{
    QModelIndex proxyIndex = recordProxyModel->mapFromSource(recordSourceModel->index(sourcePos, 0));
    return proxyIndex.row();
}


int TableController::proxypos_to_sourcepos(int proxyPos)
{
    QModelIndex sourceIndex = recordProxyModel->mapToSource(recordProxyModel->index(proxyPos, 0));
    return sourceIndex.row();
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void TableController::cut(void)
{
    // Надо сохранить запись, так как перед копированием в буфер обмена запись
    // обязательно должна быть сохранена, иначе редактирование,
    // которое было после открытия записи и до нажатия Cut, потеряется
    find_object<MetaEditor>("editorScreen")->save_textarea();

    copy();
    delete_records_selected();
}


// Копирование отмеченных записей в буфер обмена
void TableController::copy(void)
{
    // Объект с записями помещается в буфер обмена
    QApplication::clipboard() -> setMimeData(_view->getSelectedRecords());
}


// Insert records from the clipboard    // Вставка записей из буфера обмена
void TableController::paste(void)
{
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if(mimeData == NULL)
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
    int nList = clipboardRecords->getCount();

    // Пробегаются все записи в буфере
    for(int i = 0; i < nList; i++)
        addnew_record(ADD_NEW_RECORD_TO_END, clipboardRecords->getRecord(i));

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>("treeScreen")->updateSelectedBranch();
}


// Слот для добавления новой записи в конец таблицы
void TableController::addnew_to_end_context(void)
{
    qDebug() << "In slot add_new_toend_context()";

    addnew_record(ADD_NEW_RECORD_TO_END);
}


// Слот для добавления новой записи перед выделенной строкой
void TableController::addnew_before_context(void)
{
    qDebug() << "In slot add_new_before_context()";

    addnew_record(ADD_NEW_RECORD_BEFORE);
}


// Слот для добавления новой записи после выделенной строки
// Slot to add a new record after the highlighted row
void TableController::addnew_after_context(void)
{
    qDebug() << "In slot add_new_after_context()";

    addnew_record(ADD_NEW_RECORD_AFTER);
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
void TableController::addnew_record(int mode)
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

    std::shared_ptr<Record> record = std::make_shared<Record>();
    record->switchToFat();
    //    record.setText(addNewRecordWin.getField("text"));
    //    record.setField("pin",   addNewRecordWin.getField("pin"));
    //    record.setField("name",   addNewRecordWin.getField("name"));
    //    record.setField("author", addNewRecordWin.getField("author"));
    //    record.setField("url",    addNewRecordWin.getField("url"));
    //    record.setField("tags",   addNewRecordWin.getField("tags"));
    record->setTextToFat("");
    record->setField("pin",   _check_state[Qt::Unchecked]);
    record->setField("name",   "");
    record->setField("author", "");
    record->setField("home",   browser::Browser::_defaulthome);
    record->setField("url",    browser::Browser::_defaulthome);
    record->setField("tags",   "");

    record->setPictureFiles(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
    // Запись должна быть создана, потом можно аттачить файлы.
    // Это ограничение для "ленивого" программинга, но пока так
    // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

    // Временная директория с картинками и приаттаченными файлами удаляется
    DiskHelper::removeDirectory(directory);

    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
    addnew_record(mode, record);
}


// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
int TableController::new_record(std::shared_ptr<Record> record
                                , const int mode
                                //    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
                               )
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

    assert(!record->isLite());

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

    //    record.setPictureFiles(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    //    record->generator(generator);

    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
    // Запись должна быть создана, потом можно аттачить файлы.
    // Это ограничение для "ленивого" программинга, но пока так
    // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

    // Временная директория с картинками и приаттаченными файлами удаляется
    DiskHelper::removeDirectory(directory);

    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
    return addnew_record(mode, record);
}

// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
int TableController::new_record_from_url(
    const QUrl &url
    , const int mode
    //    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
)
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

    std::shared_ptr<Record> record = std::make_shared<Record>();
    record->switchToFat();
    //    record.setText(addNewRecordWin.getField("text"));
    //    record.setField("pin",   addNewRecordWin.getField("pin"));
    //    record.setField("name",   addNewRecordWin.getField("name"));
    //    record.setField("author", addNewRecordWin.getField("author"));
    //    record.setField("url",    addNewRecordWin.getField("url"));
    //    record.setField("tags",   addNewRecordWin.getField("tags"));
    record->setTextToFat("");
    record->setField("pin",   _check_state[Qt::Unchecked]);
    record->setField("name",   "");
    record->setField("author", "");
    record->setField("home",   url.toString());
    record->setField("url",    url.toString());
    record->setField("tags",   "");

    record->setPictureFiles(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    //    record.generator(generator);

    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
    // Запись должна быть создана, потом можно аттачить файлы.
    // Это ограничение для "ленивого" программинга, но пока так
    // record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

    // Временная директория с картинками и приаттаченными файлами удаляется
    DiskHelper::removeDirectory(directory);

    // Введенные данные добавляются (все только что введенные данные передаются в функцию addNew() незашифрованными)
    return addnew_record(mode, record);
}


// Функция добавления новой записи в таблицу конечных записей
// Принимает полный формат записи
int TableController::addnew_record(int mode, std::shared_ptr<Record> record)
{
    qDebug() << "In add_new()";

    // Получение Source-индекса первой выделенной строки
    QModelIndex posIndex = _view->getFirstSelectionSourceIndex();

    //    //    assert(posIndex.isValid());
    //    if(!posIndex.isValid()) {
    //        posIndex = view->getFirstSelectionProxyIndex();
    //    }

    //    if(posIndex.isValid() //   // do not need? yeah, I am sure. hughvonyoung@gmail.com
    //       //       && record.getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome
    //      ) {

    // Вставка новых данных, возвращаемая позиция - это позиция в Source данных
    int selPos = recordSourceModel->addTableData(mode, posIndex, record);

    _view->moveCursorToNewRecord(mode, sourcepos_to_proxypos(selPos));

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
    //    }

    //    else {
    //        // Add a record in the database // Добавление записи в базу
    //        RecordTableData *table = nullptr;
    //        table = recordSourceModel->getRecordTableData();

    //        //        if(table == nullptr) {}

    //        int selPos = table->insertNewRecord(ADD_NEW_RECORD_TO_END, 0, record);

    //        view->moveCursorToNewRecord(mode, convertSourcePosToProxyPos(selPos));

    //        // Сохранение дерева веток
    //        find_object<TreeScreen>("treeScreen")->saveKnowTree();
    //    }

    return selPos;
}


void TableController::on_edit_fieldcontext(void)
{
    _view->editFieldContext();
    //view->loadUrl();
}

void TableController::open_website(QModelIndex proxyIndex)
{
    qDebug() << "RecordTableController::editFieldContext()";

    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    int pos = sourceIndex.row(); // Номер строки в базе

    // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    //RecordInfoFieldsEditor editRecordWin;
    auto entrance = globalparameters.entrance();
    // Выясняется ссылка на таблицу конечных данных
    //    RecordTableData *table = recordSourceModel->getTableData();

    // Поля окна заполняются начальными значениями
    //editRecordWin.setField("name",  table->getField("name",   pos) );
    //editRecordWin.setField("author",table->getField("author", pos) );
    //editRecordWin.setField("url",   table->getField("url",    pos) );
    //editRecordWin.setField("tags",  table->getField("tags",   pos) );

    //    browser_view->loadUrl(pos);   //table->getField("url", pos)

    std::shared_ptr<Record> record = this->recordtable_model()->getRecordTableData()->record(pos);
    assert(record->is_registered());

    //    if(record->getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome)
    if(entrance && (!record->binder() || record->activator())) entrance->equip_registered(record);

    if(record->binder() && record->activator())record->active();


    //    int i = editRecordWin.exec();
    //    if(i == QDialog::Rejected)
    //        return; // Была нажата отмена, ничего ненужно делать

    //// Измененные данные записываются
    //    editField(pos,
    //              editRecordWin.getField("name"),
    //              editRecordWin.getField("author"),
    //              editRecordWin.getField("url"),
    //              editRecordWin.getField("tags"));

}

// Действия при нажатии кнопки редактирования записи
void TableController::edit_fieldcontext(QModelIndex proxyIndex)
{
    qDebug() << "RecordTableController::editFieldContext()";

    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    int pos = sourceIndex.row(); // Номер строки в базе

    // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    InfoFieldsEditor editRecordWin;

    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Поля окна заполняются начальными значениями
    editRecordWin.setField("pin",       table->field("pin",   pos));
    editRecordWin.setField("name",      table->field("name",   pos));
    editRecordWin.setField("author",    table->field("author", pos));
    editRecordWin.setField("home",      table->field("home",    pos));
    editRecordWin.setField("url",       table->field("url",    pos));
    editRecordWin.setField("tags",      table->field("tags",   pos));


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
void TableController::edit_field(int pos
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
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Переданные отредактированные поля преобразуются в вид имя-значение
    QMap<QString, QString> editData;
    editData["pin"] = pin;
    editData["name"] = name;
    editData["author"] = author;
    editData["home"] = home;
    editData["url"] = url;
    editData["tags"] = tags;

    // Обновление новых данных в таблице конечных записей
    table->edit_record_fields(pos, editData);

    // Обновление инфополей в области редактирования записи
    MetaEditor *metaEditor = find_object<MetaEditor>("editorScreen");
    metaEditor->setPin(pin);
    metaEditor->setName(name);
    metaEditor->setAuthor(author);
    metaEditor->setUrl(url);
    metaEditor->setTags(tags);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Обработка клика по удалению записи в контекстном меню и по кнопке на панели
void TableController::delete_context(void)
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
        delete_records_selected();
    }

}


// Удаление отмеченных записей
void TableController::delete_records_selected(void)
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
    find_object<MetaEditor>("editorScreen")->clearAll();

    // Вызывается удаление отмеченных записей
    removerows_by_idlist(delIds);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>("treeScreen")->updateSelectedBranch();

    // Установка курсора на нужную позицию
    if(selectionRowNum >= 0 && selectionRowNum < recordProxyModel->rowCount())
        _view->selectRow(selectionRowNum);

    // Если таблица конечных записей пуста
    if(recordProxyModel->rowCount() == 0) {
        // Нужно очистить поле редактирования чтобы невидно было текста
        // последней удаленной записи
        find_object<MetaEditor>("editorScreen")->clearAll();
    }

    qobject_cast<TableScreen *>(parent())->toolsUpdate();
}


// Удаление одной записи по идентификатору
void TableController::removerow_by_id(QString delId)
{
    QVector<QString> delIds;
    delIds.append(delId);
    removerows_by_idlist(delIds);
}


// Remove records for the specified list of identifiers // Удаление записей по указанному списку идентификаторов
void TableController::removerows_by_idlist(QVector<QString> delIds)
{
    qDebug() << "Remove rows by ID list: " << delIds;

    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    if(!table)
        return;

    for(int i = 0; i < delIds.count(); i++) {
        QString id = delIds[i];
        QModelIndex idx = id_to_proxyindex(id);

        // Удаляется строка в Proxy модели
        // Proxy модель сама должна уведомить вид о своем изменении, так как именно она подключена к виду
        recordProxyModel->removeRow(idx.row());
        _view->reset();
        _view->setModel(recordProxyModel);
        globalparameters.getFindScreen()->remove_id(id);
    }
}


// Перемещение записи вверх
void TableController::move_up(void)
{
    qDebug() << "In moveup()";

    // Получение номера первой выделенной строки
    int pos = _view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Перемещение текущей записи вверх
    table->move_up(pos);

    // Установка засветки на перемещенную запись
    _view->setSelectionToPos(pos - 1);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Перемещение записи вниз
void TableController::move_dn(void)
{
    qDebug() << "In movedn()";

    // Получение номера первой выделенной строки
    int pos = _view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    std::shared_ptr<TableData> table = recordSourceModel->getTableData();

    // Перемещение текущей записи вниз
    table->move_dn(pos);

    // Установка засветки на перемещенную запись
    _view->setSelectionToPos(pos + 1);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Клик по пункту "Сортировка" в контекстном меню
void TableController::on_sort_click(void)
{
    TableScreen *parentPointer = qobject_cast<TableScreen *>(parent());

    // Если сортировка еще не включена
    if(!_view->isSortingEnabled()) {
        // Включается сортировка
        _view->setSortingEnabled(true);

        recordProxyModel->setSortRole(SORT_ROLE); // Qt::DisplayRole

        // Включается сортировка по нужному столбцу
        int n = parentPointer->actionSort->data().toInt(); // В actionSort хранится номер столбца, по которому нужно сортировать
        qDebug() << "Sort column number " << n;
        recordProxyModel->sort(n);

        // Треугольничек сортировки переставляется на нужный столбец
        _view->horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);

        // Запрещается передвижение заголовков столбцов
        // так как после переноса неправильно устанавливается треугольничек сортировки, он остается на том же по счету столбце
        // horizontalHeader()->setSectionsMovable(false);
    } else {
        // Оменяется сортировка
        _view->setSortingEnabled(false);
        recordProxyModel->setSortRole(Qt::InitialSortOrderRole);
        recordProxyModel->invalidate();

        // Разрешается передвижение заголовков столбцов
        // horizontalHeader()->setSectionsMovable(true);
    }

    qobject_cast<TableScreen *>(parent())->toolsUpdate();
}


// Слот, срабатывающий при вызове настроек
void TableController::settings(void)
{
    AppConfigDialog dialog("pageRecordTable");
    dialog.show();

    // Todo: Возвращение фокуса почему-то не работает, надо разбираться
    // (а может просто не выделяется виджет, в Qt5 вделенный виджет не виден в дефолтной схеме)
    // qDebug() << "Set focus to RecordTableView";
    // this->setFocus();
}


void TableController::on_recordtable_configchange(void)
{
    recordSourceModel->onRecordTableConfigChange();
    _view->restoreColumnWidth();
    _view->restoreHeaderState();
}


void TableController::on_print_click(void)
{
    TableScreen *parentPointer = qobject_cast<TableScreen *>(parent());

    TablePrint printDialog(parentPointer);
    printDialog.setModel(recordProxyModel);
    printDialog.generateHtmlTableFromModel();
    printDialog.setTitleToHtml(recordSourceModel->getTableData()->item()->getPathAsNameWithDelimeter(" / "));
    printDialog.exec();
}

