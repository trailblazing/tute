#include <QObject>
#include <QHeaderView>

#include "main.h"
#include "RecordTableController.h"
#include "controllers/attachTable/AttachTableController.h"
#include "views/record/MetaEditor.h"
#include "views/record/AddNewRecord.h"
#include "views/recordTable/RecordTableView.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/recordTable/RecordTablePrint.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/appConfigWindow/AppConfigDialog.h"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "models/recordTable/Record.h"
#include "models/recordTable/RecordTableData.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableProxyModel.h"
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


RecordTableController::RecordTableController(RecordTableScreen *recordtablescreen)
    : QObject(recordtablescreen)
    , view(new RecordTableView(recordtablescreen, this
                               // , qobject_cast<QWidget * >(RecordTableScreen)
                              ))
    , recordSourceModel(new RecordTableModel(this))
    , recordProxyModel(new RecordTableProxyModel(this))
{
    // Инициализируется область со списком записей
    //    view = new RecordTableView(qobject_cast<QWidget *>(parent));   // Вид размещается внутри виджета Screen
    view->setObjectName("recordTableView");
    //    view->setController(this);

    // Создание модели данных
    //    recordSourceModel = new RecordTableModel(this);
    recordSourceModel->setObjectName("recordSourceModel");

    //    recordProxyModel = new RecordTableProxyModel(this);
    recordProxyModel->setSourceModel(recordSourceModel);
    recordProxyModel->setObjectName("recordProxyModel");

    // Модель данных задается для вида
    view->setModel(recordProxyModel);
}


RecordTableController::~RecordTableController()
{

}


void RecordTableController::init(void)
{
    view->init();
}


RecordTableView *RecordTableController::getView(void)
{
    return view;
}


// Принимает индекс Proxy модели
// Accepts index Proxy models
void RecordTableController::click_record(const QModelIndex &index)
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
void RecordTableController::update_browser(const int source_pos)
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




    std::shared_ptr<Record> record = this->getRecordTableModel()->getRecordTableData()->getRecord(source_pos);
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

        if(!record->generator() && entrance) {
            entrance->equip_registered(record);
        }

        assert(record->generator());
        assert(record->activator());

        if(record->generator() && !record->unique_page())record->generate();
        else if(record->activator() && record->unique_page())record->active();  // if(entrance) entrance->active_record(record);

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

void RecordTableController::sychronize_metaeditor_to_record(const int pos)
{

    std::shared_ptr<Record> record = this->getRecordTableModel()->getRecordTableData()->getRecord(pos);
    assert(record);
    // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь только получить данные из таблицы

    // Выясняется указатель на объект редактирования текста записи
    MetaEditor *metaeditor = find_object<MetaEditor>("editorScreen");
    assert(metaeditor);
    metaeditor->bind(record);

    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // В таблице конечных данных запоминается какая запись была выбрана
    // чтобы затем при выборе этой же подветки засветка автоматически
    // установилась на последнюю рабочую запись
    table->setWorkPos(pos);


    // Устанавливается функция обратного вызова для записи данных
    metaeditor->set_save_callback(table->editorSaveCallback);

    // Сохраняется текст и картинки в окне редактирования
    find_object<MainWindow>("mainwindow")->saveTextarea();


    // Для новой выбраной записи выясняется директория и основной файл
    QString currentDir = table->getField("dir", pos);
    QString currentFile = table->getField("file", pos);
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
    table->getText(pos);

    // Редактору задаются имя файла и директории
    // И дается команда загрузки файла
    metaeditor->set_work_directory(fullDir);
    metaeditor->set_file_name(currentFile);

    // Если идет работа с зашифрованной записью
    // И если имя директории или имя файла пусты, то это означает что
    // запись не была расшифрована, и редактор должен просто показывать пустой текст
    // ничего не сохранять и не считывать
    qDebug() << "RecordTableView::onClickToRecord() : id " << table->getField("id", pos);
    qDebug() << "RecordTableView::onClickToRecord() : name " << table->getField("name", pos);
    qDebug() << "RecordTableView::onClickToRecord() : crypt " << table->getField("crypt", pos);

    if(table->getField("crypt", pos) == "1")
        if(fullDir.length() == 0 || currentFile.length() == 0)
            metaeditor->setDirFileEmptyReaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

    // В редактор заносится информация, идет ли работа с зашифрованным текстом
    metaeditor->setMiscField("crypt", table->getField("crypt", pos));

    // В редакторе устанавливается функция обратного вызова для чтения данных
    metaeditor->set_load_callback(table->editorLoadCallback);

    metaeditor->load_textarea();
    // edView->set_textarea(table->get_text(index.row()));

    // Заполняются прочие инфо-поля
    metaeditor->setName(table->getField("name", pos));
    metaeditor->setAuthor(table->getField("author", pos));
    metaeditor->setHome(table->getField("home", pos));
    metaeditor->setUrl(table->getField("url", pos));
    metaeditor->setTags(table->getField("tags", pos));

    QString id = table->getField("id", pos);
    metaeditor->setMiscField("id", id);

    metaeditor->setMiscField("title", table->getField("name", pos));

    // Устанавливается путь до ветки в которой лежит запись (в виде названий веток)
    QString path = qobject_cast<RecordTableScreen *>(parent())->getTreePath();

    // В мобильном интерфейсе редактор должен показывать путь до записи
    if(appconfig.getInterfaceMode() == "mobile")
        metaeditor->setTreePath(path);

    // В редакторе восстанавливается позиция курсора и прокрутки если это необходимо
    if(appconfig.getRememberCursorAtOrdinarySelection()) {
        metaeditor->setCursorPosition(walkhistory.getCursorPosition(id));
        metaeditor->setScrollBarPosition(walkhistory.getScrollBarPosition(id));
    }

    // Обновление иконки аттачей
    if(table->getRecord(pos)->getAttachTablePointer()->size() == 0)
        metaeditor->toAttach->setIcon(metaeditor->iconAttachNotExists);   // Если нет приаттаченных файлов
    else
        metaeditor->toAttach->setIcon(metaeditor->iconAttachExists);   // Есть приаттаченные файлы
}


void RecordTableController::sychronize_attachtable_to_record(const int pos)
{
    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Устанавливается таблица приаттаченных файлов
    AttachTableController *attachTableController = find_object<AttachTableController>("attachTableController");
    attachTableController->setAttachTableData(table->getRecord(pos)->getAttachTablePointer());
}


bool RecordTableController::isTableNotExists(void)
{
    if(recordSourceModel->getTableData() == NULL)
        return true;
    else
        return false;
}


// Set a new set of data to the list of recordings  // Установка нового набора данных для списка записей
void RecordTableController::set_table_data(RecordTableData *rtData)
{
    qDebug() << "In RecordTableView setTableData() start";

    // Обновление набора данных с последующим выбором первой строки
    // может быть очень длительным, поэтому показывается что процесс выполняется
    // QCursor cursor_wait=QCursor(Qt::BusyCursor);
    // qApp->setOverrideCursor(cursor_wait);
    find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

    // Pointer to the data reported to the data source    // Указатель на данные сообщается источнику данных
    recordSourceModel->setTableData(rtData);

    // Надо обязательно сбросить selection model
    view->selectionModel()->clear();

    // Если список конечных записей не пуст
    bool removeSelection = true;

    if(recordSourceModel->rowCount() > 0) {
        // Нужно выяснить, на какой записи ранее стояло выделение
        int workPos = rtData->getWorkPos();

        // Если номер записи допустимый
        if(workPos > 0 && workPos < recordSourceModel->rowCount()) {
            // Выделение устанавливается на нужную запись
            // selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) , QItemSelectionModel::SelectCurrent);
            view->selectRow(workPos);
            view->scrollTo(view->currentIndex());   // QAbstractItemView::PositionAtCenter

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
        qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
    }

    // qApp->restoreOverrideCursor();
    find_object<MainWindow>("mainwindow")->unsetCursor();

    qDebug() << "In RecordTableView set new model stop";
}


// Заполнение переданного объекта буфера обмена данными из указанных записей
// Индексы QModelIndexList передаются от Proxy модели
// Fill the object passed clipboard data from these records
// Index QModelIndexList transferred from the Proxy pattern
void RecordTableController::addRecordsToClipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy)
{
    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Перебираются записи и вносятся в буфер обмена
    for(int i = 0; i < itemsForCopy.size(); ++i) {
        QModelIndex index = proxyindex_to_sourceindex(itemsForCopy.at(i));

        // The image recording, including all text data (text records, property records list an attached file)        // Образ записи, включающий все текстовые данные (текст записи, свойства записи, перечень приаттаченных файлов)
        std::shared_ptr<Record> record = table->getRecordFat(index.row());

        clipboardRecords->addRecord(record);
    }
}


int RecordTableController::getRowCount(void)
{
    return recordProxyModel->rowCount();
}


// Get the number of the first highlighted item on the screen   // Получение номера первого выделенного элемента на экране
int RecordTableController::getFirstSelectionPos(void)
{
    return view->getFirstSelectionPos();
}


// Получение ID первого выделенного элемента на экране
QString RecordTableController::getFirstSelectionId(void)
{
    return view->getFirstSelectionId();
}


void RecordTableController::select_pos(int pos)
{
    view->setSelectionToPos(pos);
}


void RecordTableController::select_id(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Если таблица конечных данных задана
    // (Не задана таблица может быть по причине если ветка зашифрована и введен неверный пароль, или при вводе пароля была нажата отмена)
    if(table != NULL) {
        // Номер записи в Source данных
        int pos = table->getPosById(id);

        view->setSelectionToPos(sourcepos_to_proxypos(pos));
    }
}


QModelIndex RecordTableController::id_to_sourceindex(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Номер записи в Source данных
    int sourcePos = table->getPosById(id);

    return pos_to_sourceindex(sourcePos);
}


QModelIndex RecordTableController::id_to_proxyindex(QString id)
{
    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Номер записи в Source данных
    int sourcePos = table->getPosById(id);
    int proxyPos = sourcepos_to_proxypos(sourcePos);

    return pos_to_proxyindex(proxyPos);
}


QModelIndex RecordTableController::pos_to_proxyindex(int pos)
{
    if(pos < 0 || pos >= recordProxyModel->rowCount())
        return QModelIndex();

    QModelIndex index = recordProxyModel->index(pos, 0);

    return index;
}


QModelIndex RecordTableController::pos_to_sourceindex(int pos)
{
    if(pos < 0 || pos >= recordProxyModel->rowCount())
        return QModelIndex();

    QModelIndex proxyIndex = pos_to_proxyindex(pos);
    QModelIndex index = recordProxyModel->mapToSource(proxyIndex);

    return index;
}


// Преобразование Proxy индекса в позицию на экране (так, как это будет выглядеть при Proxy модели)
int RecordTableController::proxyindex_to_pos(QModelIndex index)
{
    if(!index.isValid())
        return -1;

    return index.row();
}


// Преобразование Source индекса в позицию на экране (так, как это будет выглядеть при Source модели)
int RecordTableController::sourceindex_to_pos(QModelIndex index)
{
    if(!index.isValid())
        return -1;

    return index.row();
}


QModelIndex RecordTableController::proxyindex_to_sourceindex(QModelIndex proxyIndex)
{
    if(!proxyIndex.isValid())
        return QModelIndex();

    QModelIndex index = recordProxyModel->mapToSource(proxyIndex);

    return index;
}


QModelIndex RecordTableController::sourceindex_to_proxyindex(QModelIndex sourceIndex)
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    QModelIndex index = recordProxyModel->mapFromSource(sourceIndex);

    return index;
}


int RecordTableController::sourcepos_to_proxypos(int sourcePos)
{
    QModelIndex proxyIndex = recordProxyModel->mapFromSource(recordSourceModel->index(sourcePos, 0));
    return proxyIndex.row();
}


int RecordTableController::proxypos_to_sourcepos(int proxyPos)
{
    QModelIndex sourceIndex = recordProxyModel->mapToSource(recordProxyModel->index(proxyPos, 0));
    return sourceIndex.row();
}


// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void RecordTableController::cut(void)
{
    // Надо сохранить запись, так как перед копированием в буфер обмена запись
    // обязательно должна быть сохранена, иначе редактирование,
    // которое было после открытия записи и до нажатия Cut, потеряется
    find_object<MetaEditor>("editorScreen")->save_textarea();

    copy();
    deleteRecords();
}


// Копирование отмеченных записей в буфер обмена
void RecordTableController::copy(void)
{
    // Объект с записями помещается в буфер обмена
    QApplication::clipboard() -> setMimeData(view->getSelectedRecords());
}


// Insert records from the clipboard    // Вставка записей из буфера обмена
void RecordTableController::paste(void)
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
        addNew(ADD_NEW_RECORD_TO_END, clipboardRecords->getRecord(i));

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>("treeScreen")->updateSelectedBranch();
}


// Слот для добавления новой записи в конец таблицы
void RecordTableController::addNewToEndContext(void)
{
    qDebug() << "In slot add_new_toend_context()";

    addNewRecord(ADD_NEW_RECORD_TO_END);
}


// Слот для добавления новой записи перед выделенной строкой
void RecordTableController::addNewBeforeContext(void)
{
    qDebug() << "In slot add_new_before_context()";

    addNewRecord(ADD_NEW_RECORD_BEFORE);
}


// Слот для добавления новой записи после выделенной строки
// Slot to add a new record after the highlighted row
void RecordTableController::addNewAfterContext(void)
{
    qDebug() << "In slot add_new_after_context()";

    addNewRecord(ADD_NEW_RECORD_AFTER);
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
void RecordTableController::addNewRecord(int mode)
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
    addNew(mode, record);
}


// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
int RecordTableController::new_record(std::shared_ptr<Record> record
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
    return addNew(mode, record);
}

// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
int RecordTableController::new_record(
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
    return addNew(mode, record);
}


// Функция добавления новой записи в таблицу конечных записей
// Принимает полный формат записи
int RecordTableController::addNew(int mode, std::shared_ptr<Record> record)
{
    qDebug() << "In add_new()";

    // Получение Source-индекса первой выделенной строки
    QModelIndex posIndex = view->getFirstSelectionSourceIndex();

    //    //    assert(posIndex.isValid());
    //    if(!posIndex.isValid()) {
    //        posIndex = view->getFirstSelectionProxyIndex();
    //    }

    //    if(posIndex.isValid() //   // do not need? yeah, I am sure. hughvonyoung@gmail.com
    //       //       && record.getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome
    //      ) {

    // Вставка новых данных, возвращаемая позиция - это позиция в Source данных
    int selPos = recordSourceModel->addTableData(mode, posIndex, record);

    view->moveCursorToNewRecord(mode, sourcepos_to_proxypos(selPos));

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


void RecordTableController::onEditFieldContext(void)
{
    view->editFieldContext();
    //view->loadUrl();
}

void RecordTableController::openWebsite(QModelIndex proxyIndex)
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

    std::shared_ptr<Record> record = this->getRecordTableModel()->getRecordTableData()->getRecord(pos);

    //    if(record->getNaturalFieldSource("url") != browser::DockedWindow::_defaulthome)
    if(entrance && !record->generator()) entrance->equip_registered(record);

    if(record->generator())record->generate();


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
void RecordTableController::editFieldContext(QModelIndex proxyIndex)
{
    qDebug() << "RecordTableController::editFieldContext()";

    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    int pos = sourceIndex.row(); // Номер строки в базе

    // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    RecordInfoFieldsEditor editRecordWin;

    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Поля окна заполняются начальными значениями
    editRecordWin.setField("pin",       table->getField("pin",   pos));
    editRecordWin.setField("name",      table->getField("name",   pos));
    editRecordWin.setField("author",    table->getField("author", pos));
    editRecordWin.setField("home",      table->getField("home",    pos));
    editRecordWin.setField("url",       table->getField("url",    pos));
    editRecordWin.setField("tags",      table->getField("tags",   pos));


    int i = editRecordWin.exec();

    if(i == QDialog::Rejected)
        return; // Была нажата отмена, ничего ненужно делать

    // Измененные данные записываются
    editField(pos,
              editRecordWin.getField("pin"),
              editRecordWin.getField("name"),
              editRecordWin.getField("author"),
              editRecordWin.getField("home"),
              editRecordWin.getField("url"),
              editRecordWin.getField("tags"));
}


// Функция сохранения отредактированных полей записи в таблицу конечных записей
void RecordTableController::editField(int pos
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
    RecordTableData *table = recordSourceModel->getTableData();

    // Переданные отредактированные поля преобразуются в вид имя-значение
    QMap<QString, QString> editData;
    editData["pin"] = pin;
    editData["name"] = name;
    editData["author"] = author;
    editData["home"] = home;
    editData["url"] = url;
    editData["tags"] = tags;

    // Обновление новых данных в таблице конечных записей
    table->editRecordFields(pos, editData);

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
void RecordTableController::deleteContext(void)
{
    // Создается окно с вопросом нужно удалять запись (записи) или нет
    QMessageBox messageBox(view);
    messageBox.setWindowTitle("Delete");
    messageBox.setText(tr("Are you sure to delete this record(s)?"));
    QAbstractButton *cancelButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    QAbstractButton *deleteButton = messageBox.addButton(tr("Delete"), QMessageBox::AcceptRole);

    Q_UNUSED(cancelButton);

    messageBox.exec();

    if(messageBox.clickedButton() == deleteButton) {
        // Выбранные данные удаляются
        deleteRecords();
    }

}


// Удаление отмеченных записей
void RecordTableController::deleteRecords(void)
{
    qDebug() << "RecordTableView::delete_records()";

    // Получение списка Item-элементов, подлежащих удалению. Индексы Proxy модели
    QModelIndexList itemsForDelete = view->selectionModel()->selectedIndexes();

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
    removeRowsByIdList(delIds);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();

    // Обновление на экране ветки, на которой стоит засветка,
    // так как количество хранимых в ветке записей поменялось
    find_object<TreeScreen>("treeScreen")->updateSelectedBranch();

    // Установка курсора на нужную позицию
    if(selectionRowNum >= 0 && selectionRowNum < recordProxyModel->rowCount())
        view->selectRow(selectionRowNum);

    // Если таблица конечных записей пуста
    if(recordProxyModel->rowCount() == 0) {
        // Нужно очистить поле редактирования чтобы невидно было текста
        // последней удаленной записи
        find_object<MetaEditor>("editorScreen")->clearAll();
    }

    qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


// Удаление одной записи по идентификатору
void RecordTableController::removeRowById(QString delId)
{
    QVector<QString> delIds;
    delIds.append(delId);
    removeRowsByIdList(delIds);
}


// Remove records for the specified list of identifiers // Удаление записей по указанному списку идентификаторов
void RecordTableController::removeRowsByIdList(QVector<QString> delIds)
{
    qDebug() << "Remove rows by ID list: " << delIds;

    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    if(table == NULL)
        return;

    for(int i = 0; i < delIds.count(); i++) {
        QString id = delIds[i];
        QModelIndex idx = id_to_proxyindex(id);

        // Удаляется строка в Proxy модели
        // Proxy модель сама должна уведомить вид о своем изменении, так как именно она подключена к виду
        recordProxyModel->removeRow(idx.row());
        view->reset();
        view->setModel(recordProxyModel);
        globalparameters.getFindScreen()->remove_id(id);
    }
}


// Перемещение записи вверх
void RecordTableController::moveUp(void)
{
    qDebug() << "In moveup()";

    // Получение номера первой выделенной строки
    int pos = view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Перемещение текущей записи вверх
    table->moveUp(pos);

    // Установка засветки на перемещенную запись
    view->setSelectionToPos(pos - 1);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Перемещение записи вниз
void RecordTableController::moveDn(void)
{
    qDebug() << "In movedn()";

    // Получение номера первой выделенной строки
    int pos = view->getFirstSelectionPos();

    // Выясняется ссылка на таблицу конечных данных
    RecordTableData *table = recordSourceModel->getTableData();

    // Перемещение текущей записи вниз
    table->moveDn(pos);

    // Установка засветки на перемещенную запись
    view->setSelectionToPos(pos + 1);

    // Сохранение дерева веток
    find_object<TreeScreen>("treeScreen")->saveKnowTree();
}


// Клик по пункту "Сортировка" в контекстном меню
void RecordTableController::onSortClick(void)
{
    RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen *>(parent());

    // Если сортировка еще не включена
    if(!view->isSortingEnabled()) {
        // Включается сортировка
        view->setSortingEnabled(true);

        recordProxyModel->setSortRole(SORT_ROLE); // Qt::DisplayRole

        // Включается сортировка по нужному столбцу
        int n = parentPointer->actionSort->data().toInt(); // В actionSort хранится номер столбца, по которому нужно сортировать
        qDebug() << "Sort column number " << n;
        recordProxyModel->sort(n);

        // Треугольничек сортировки переставляется на нужный столбец
        view->horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder);

        // Запрещается передвижение заголовков столбцов
        // так как после переноса неправильно устанавливается треугольничек сортировки, он остается на том же по счету столбце
        // horizontalHeader()->setSectionsMovable(false);
    } else {
        // Оменяется сортировка
        view->setSortingEnabled(false);
        recordProxyModel->setSortRole(Qt::InitialSortOrderRole);
        recordProxyModel->invalidate();

        // Разрешается передвижение заголовков столбцов
        // horizontalHeader()->setSectionsMovable(true);
    }

    qobject_cast<RecordTableScreen *>(parent())->toolsUpdate();
}


// Слот, срабатывающий при вызове настроек
void RecordTableController::settings(void)
{
    AppConfigDialog dialog("pageRecordTable");
    dialog.show();

    // Todo: Возвращение фокуса почему-то не работает, надо разбираться
    // (а может просто не выделяется виджет, в Qt5 вделенный виджет не виден в дефолтной схеме)
    // qDebug() << "Set focus to RecordTableView";
    // this->setFocus();
}


void RecordTableController::onRecordTableConfigChange(void)
{
    recordSourceModel->onRecordTableConfigChange();
    view->restoreColumnWidth();
    view->restoreHeaderState();
}


void RecordTableController::onPrintClick(void)
{
    RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen *>(parent());

    RecordTablePrint printDialog(parentPointer);
    printDialog.setModel(recordProxyModel);
    printDialog.generateHtmlTableFromModel();
    printDialog.setTitleToHtml(recordSourceModel->getTableData()->getItem()->getPathAsNameWithDelimeter(" / "));
    printDialog.exec();
}

