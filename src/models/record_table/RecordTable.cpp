#include <QDomElement>
#include <QMap>
#include <QString>
#include <QDir>
#include <QMessageBox>

#include "main.h"
#include "Record.h"
#include "RecordTable.h"

#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/TreeItem.h"
#include "libraries/WalkHistory.h"
#include "models/tree/KnowTreeModel.h"
#include "views/tree/KnowTreeView.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/DiskHelper.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"
#include "libraries/wyedit/Editor.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern WalkHistory walkhistory;

//const int add_new_record_to_end = 0;

// Это набор данных конечной таблицы, с которыми удобно работать

//// Конструктор
//RecordTable::RecordTable(boost::intrusive_ptr<TreeItem> _tree_item)
//// , QDomElement i_dom_element
////    : _tree_item(_tree_item)
////    , _workpos(-1)
//{
//    //    treeItem = nullptr;
//    //    workPos = -1;

//    init(_tree_item->export_to_dom());  // i_dom_element

//    //    return;
//}

// Конструктор
RecordTable::RecordTable(const QDomElement &i_dom_element, const bool _is_crypt): _is_crypt(_is_crypt)
    //    : _tree_item(_tree_item)
    //    , _workpos(-1)
{
    //    treeItem = nullptr;
    //    workPos = -1;

    //    init(i_dom_element);  // i_dom_element

    // Создание таблицы
    if(!i_dom_element.isNull()) {
        //        QDomElement *dom_element = &i_dom_element;
        import_from_dom(
            i_dom_element  // dom_element
        );
    }

    //    return;
}
// Деструктор
RecordTable::~RecordTable()
{
    empty();
    return;
}


// Получение значения указанного поля для указанного имени поля
// Имя поля - все возможные имена полей, кроме text (такого поля теперь вообще нет, текст запрашивается как отдельное свойство)
QString RecordTable::field(int pos, QString name) const
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _record_data.size()) {
        QString i;
        i.setNum(pos);
        critical_error("RecordTableData::getField() : get unavailable record index " + i);
    }

    return _record_data.at(pos)->field(name);
}

std::shared_ptr<Record> RecordTable::find(const QUrl &url)
{
    std::shared_ptr<Record> record;

    for(auto &i : _record_data) {
        // QString _u = i.getNaturalFieldSource("url") ;

        if(i->natural_field_source("url") == url.toString()) {
            assert(i->is_registered());
            record = i;
            break;
        }
    }

    return record;
}

std::shared_ptr<Record> RecordTable::find(std::shared_ptr<Record> r)
{
    std::shared_ptr<Record> record;

    for(auto &i : _record_data) {
        // QString _u = i.getNaturalFieldSource("url") ;

        if(i->natural_field_source("id") == r->natural_field_source("id")) {
            assert(i->is_registered());
            record = i;
            break;
        }
    }

    return record;
}

// Установка значения указанного поля для указанного элемента
void RecordTable::field(int pos, QString name, QString value)
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _record_data.size()) {
        QString i;
        i.setNum(pos);
        critical_error("In RecordTableData::setField() unavailable record index " + i + " in table while field " + name + " try set to " + value);
    }

    _record_data[pos]->field(name, value);
}


// Получение значения текста указанной записи
// Метод возвращает расшифрованные данные
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
// Get the value of the text of the specified record
// Method returns the decrypted data
// If there is a problem that file with text entries, will create an empty file
QString RecordTable::text(int pos)
{
    // Если индекс недопустимый, возвращается пустая строка
    if(pos < 0 || pos >= size())
        return QString();

    if(_record_data[pos]->is_lite())
        return _record_data[pos]->text_direct_from_lite();
    else
        return _record_data[pos]->text_from_fat();
}


// Функция, которая заменяет стандартную функцию редактора по считыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и ссылку на переменную loadText, которую надо заполнить
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить

// Function that replaces the standard function for reading Editor
// Editable text
// She is editor of passing a pointer to itself
// And variable reference loadText, which must be filled
// Attention! The method does not contain the data recording operation. Think about where to place it
void RecordTable::editor_load_callback(QObject *editor, QString &loadText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли дешифровать данные при чтении
    bool workWithCrypt = false;

    if(currEditor->misc_field("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0) {
            loadText = "";
            return;
        }

        workWithCrypt = true;
    }

    // Файл, с которым работает редактор
    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();

    QFile f(fileName);

    // Если нужный файл не существует
    if(!f.exists())
        critical_error("File " + fileName + " not found");

    // Открывается файл
    if(!f.open(QIODevice::ReadOnly))
        critical_error("File " + fileName + " not readable. Check permission.");

    // Если незашифровано
    if(workWithCrypt == false)
        loadText = QString::fromUtf8(f.readAll());
    else
        loadText = CryptService::decryptStringFromByteArray(globalparameters.crypt_key(), f.readAll()); // Если зашифровано
}


// Функция, которая заменяет стандартную функцию редактора по записыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и текст который надо записать в переменной saveText
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить

// Function that replaces the standard function of the editor to set down
// Editable text
// She is editor of passing a pointer to itself
// And the text to be written in the variable saveText
// Attention! The method does not contain the data recording operation. Think about where to place it
void RecordTable::editor_save_callback(QObject *editor, QString saveText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли шифровать записываемый текст
    bool workWithCrypt = false;

    if(currEditor->misc_field("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0)
            return;

        workWithCrypt = true;
    }

    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();

    // Если шифровать ненужно
    if(workWithCrypt == false) {
        // Текст сохраняется в файл
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
            critical_error("RecordTableData::editor_save_callback() : Cant open text file " + fileName + " for write.");

        QTextStream out(&wfile);
        out.setCodec("UTF-8");
        out << saveText;
    } else {
        // Текст шифруется
        QByteArray encryptData = CryptService::encryptStringToByteArray(globalparameters.crypt_key(), saveText);

        // В файл сохраняются зашифрованные данные
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly))
            critical_error("RecordTableData::editor_save_callback() : Cant open binary file " + fileName + " for write.");

        wfile.write(encryptData);
    }


    // Вызывается сохранение картинок
    // В данной реализации картинки сохраняются незашифрованными
    // Called saving images // In this embodiment, the image is not stored encrypted
    currEditor->save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
}


// Получение копии легкого образа записи
// Эти образы используются для хранения в дереве знаний
// Get a copy of a light image recording
// These images are used to store the tree of knowledge
std::shared_ptr<Record> RecordTable::record_lite(int pos)
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return std::make_shared<Record>();

    // Хранимая в дереве запись не может быть "тяжелой"
    if(!_record_data.at(pos)->is_lite())
        critical_error("In RecordTableData::getRecordLite() try get fat record");

    return _record_data.at(pos);
}


// Получение копии полного образа записи
// Возвращается запись с "сырыми" данными. Если запись была зашифрована, метод вернет зашифрованные данные
// Get a copy of the full image recording
// Returns the record with "raw" data. If the record was encrypted, the method returns the encrypted data
std::shared_ptr<Record> RecordTable::record_fat(int pos)
{
    // Копия записи из дерева
    std::shared_ptr<Record> resultRecord = record(pos);  //std::shared_ptr<Record> resultRecord = getRecordLite(pos);

    // original
    // Переключение копии записи на режим с хранением полного содержимого
    if(resultRecord->is_lite())resultRecord->to_fat();

    // Добавление текста записи
    resultRecord->setTextToFat(text(pos));

    // Добавление бинарных образов файлов картинок
    QString directory = appconfig.get_tetradir() + "/base/" + resultRecord->field("dir");
    resultRecord->picture_files(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    return resultRecord;
}


std::shared_ptr<Record> RecordTable::record(int pos)
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return nullptr;

    return _record_data[pos];
}

//void RecordTable::tree_item(boost::intrusive_ptr<TreeItem> item)
//{
//    // Store a reference to the thread that owns the table  // Запоминается ссылка на ветку, которой принадлежит данная таблица
//    _tree_item = item;
//    init(item->export_to_dom());
//}


//// Initialize the data table based on the transmitted item DOM- // Инициализация таблицы данных на основе переданного DOM-элемента
//void RecordTable::init(// boost::intrusive_ptr<TreeItem> item,
//    QDomElement i_dom_element)
//{
//    // Создание таблицы
//    if(!i_dom_element.isNull()) {
//        QDomElement *dom_element = &i_dom_element;
//        import(dom_element);
//    }

//    // Store a reference to the thread that owns the table  // Запоминается ссылка на ветку, которой принадлежит данная таблица
//    //    _tree_item = item;
//}


// Разбор DOM модели и преобразование ее в таблицу
void RecordTable::import_from_dom(const QDomElement &dom_model)
{
    // QDomElement n = dommodel.documentElement();
    // QDomElement n = dommodel;

    // qDebug() << "In recordtabledata setup_data_from_dom() start";

    // Если принятый элемент не является таблицей
    if(dom_model.tagName() != "recordtable")
        return;

    // Определяется указатель на первый элемент с записью
    // Define a pointer to the first element of the recording
    QDomElement currentRecordDom = dom_model.firstChildElement("record");

    while(!currentRecordDom.isNull()) {
        // Структура, куда будет помещена текущая запись
        // The structure, which will put the current record
        std::shared_ptr<Record> currentRecord = std::make_shared<Record>();
        currentRecord->is_registered(true);

        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
        // The current record is added to the final table of records (and located at a certain address in memory)
        _record_data << currentRecord;

        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
        // Order in subordinate objects have registered a valid pointer to this entry
        (_record_data.last())->import_from_dom(currentRecordDom);

        currentRecordDom = currentRecordDom.nextSiblingElement("record");
    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



    return;
}

QDomElement RecordTable::export_to_dom() const
{
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    return export_to_dom(doc);
}

// Преобразование таблицы конечных записей в Dom документ
QDomElement RecordTable::export_to_dom(std::shared_ptr<QDomDocument> doc) const
{
    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_record_data.size() == 0)
        return QDomElement();

    QDomElement record_dom_data = doc->createElement("recordtable");

    // Пробегаются все записи в таблице
    for(int i = 0; i < _record_data.size(); i++)
        record_dom_data.appendChild(_record_data.at(i)->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи

    // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

    return record_dom_data;
}

//// Преобразование таблицы конечных записей в Dom документ
//QDomElement TableData::export_to_dom(QDomDocument *doc) const
//{
//    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
//    if(_tabledata.size() == 0)
//        return QDomElement();

//    QDomElement record_dom_data = doc->createElement("recordtable");

//    // Пробегаются все записи в таблице
//    for(int i = 0; i < _tabledata.size(); i++)
//        record_dom_data.appendChild(_tabledata.at(i)->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи

//    // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

//    return record_dom_data;
//}


// Преобразование таблицы конечных записей в Dom документ
QDomElement RecordTable::export_activated_dom(std::shared_ptr<QDomDocument> doc) const
{
    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_record_data.size() == 0)
        return QDomElement();

    QDomElement record_dom_data = doc->createElement("recordtable");

    // Пробегаются все записи в таблице
    for(int i = 0; i < _record_data.size(); i++) {
        if(_record_data.at(i)->unique_page()) {
            record_dom_data.appendChild(_record_data.at(i)->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи
        }
    }

    // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

    return record_dom_data;
}




// Добавление новой записи
// Метод только добавляет во внутреннее представление новые данные,
// сохраняет текст файла и обновляет данные на экране.
// Сохранения дерева XML-данных на диск в этом методе нет.
// Допустимые режимы:
// ADD_NEW_RECORD_TO_END - в конец списка, pos игнорируется
// ADD_NEW_RECORD_BEFORE - перед указанной позицией, pos - номер позиции
// ADD_NEW_RECORD_AFTER - после указанной позиции, pos - номер позиции
// Метод принимает "тяжелый" объект записи
// Объект для вставки приходит как незашифрованным, так и зашифрованным
int RecordTable::insert_new_record(int pos, std::shared_ptr<Record> record, int mode)
{
    //    std::shared_ptr<Record> record = std::make_shared<Record>(record_);
    // Запись добавляется в таблицу конечных записей
    int insert_position = -1;
    KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

    if(!find(record) && !dataModel->is_record_id_exists(record->field("id"))) {

        //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();

        // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
        if(record->is_lite() == true)
            critical_error("RecordTable::insert_new_record() can't insert lite record");

        // Выясняется, есть ли в дереве запись с указанным ID
        // Если есть, то генерируются новые ID для записи и новая директория хранения
        // Если нет, то это значит что запись была вырезана, но хранится в буфере,
        // и ее желательно вставить с прежним ID и прежним именем директории
        // It turns out, is there a record of a tree with specified ID
        // If there is, then the generated ID for the new record and the new storage directory
        // If not, then it means that the recording was cut out, but stored in a buffer
        // And it is desirable to stick with the same ID and the same name directory
        //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());

        if(0 == record->field("id").length()
           //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
          ) {
            // Создается новая запись (ID был пустой) или
            // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
            // Create a new record (ID was empty) or
            // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

            QString id = get_unical_id();
            // Store directory entries and files    // Директория хранения записи и файл
            record->field("dir", id);   // get_unical_id()

            record->field("file", "text.html");

            // Unique ID of XML records             // Уникальный идентификатор XML записи
            //            QString id = get_unical_id();
            record->field("id", id);
        }


        // В список переданных полей добавляются вычислимые в данном месте поля

        // Время создания данной записи
        QDateTime ctime_dt = QDateTime::currentDateTime();
        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
        record->field("ctime", ctime);


        // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
        bool is_crypt = false;

        //        if(_tree_item) {
        if(_is_crypt) { //            if(_tree_item->field("crypt") == "1") {
            if(globalparameters.crypt_key().length() > 0)
                is_crypt = true;
            else
                critical_error("RecordTable::insert_new_record() : Can not insert data to crypt branch. Password not setted.");
        }

        //        }

        // Запись полновесных данных с учетом шифрации
        if(is_crypt && record->field("crypt") != "1")        // В зашифрованную ветку незашифрованную запись
            record->to_encrypt_and_save_fat();
        else if(!is_crypt && record->field("crypt") == "1")  // В незашифрованную ветку зашифрованную запись
            record->to_decrypt_and_save_fat();
        else
            record->push_fat_attributes();

        // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
        record->to_lite();

        //        // Запись добавляется в таблицу конечных записей
        //        int insertPos = -1;

        record->is_registered(true);

        if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
            _record_data << record;
            insert_position = _record_data.size() - 1;
        } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
            _record_data.insert(pos, record);
            insert_position = pos;
        } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
            _record_data.insert(pos + 1, record);
            insert_position = pos + 1;
        }

        qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insert_position);

        // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

    }

    return insert_position;

}

int RecordTable::shadow_record_lite(int pos, std::shared_ptr<Record> record, int mode)
{
    //    std::shared_ptr<Record> record = std::make_shared<Record>(record_);
    // Запись добавляется в таблицу конечных записей
    int insert_position = -1;

    //    KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());
    assert(record->is_lite());

    if(!find(record)
       //            && !dataModel->isRecordIdExists(record->getField("id"))
      ) {

        //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();

        //        // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
        //        if(record->isLite() == true)
        //            critical_error("RecordTable::insert_new_record() can't insert lite record");

        // Выясняется, есть ли в дереве запись с указанным ID
        // Если есть, то генерируются новые ID для записи и новая директория хранения
        // Если нет, то это значит что запись была вырезана, но хранится в буфере,
        // и ее желательно вставить с прежним ID и прежним именем директории
        // It turns out, is there a record of a tree with specified ID
        // If there is, then the generated ID for the new record and the new storage directory
        // If not, then it means that the recording was cut out, but stored in a buffer
        // And it is desirable to stick with the same ID and the same name directory
        //        KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>(knowtreeview_singleton_name)->model());


        assert(0 != record->field("id").length());

        //        if(0 == record->getField("id").length()
        //           //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
        //          ) {
        //            // Создается новая запись (ID был пустой) или
        //            // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
        //            // Create a new record (ID was empty) or
        //            // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

        //            QString id = get_unical_id();
        //            // Store directory entries and files    // Директория хранения записи и файл
        //            record->setField("dir", id);   // get_unical_id()

        //            record->setField("file", "text.html");

        //            // Unique ID of XML records             // Уникальный идентификатор XML записи
        //            //            QString id = get_unical_id();
        //            record->setField("id", id);
        //        }


        // В список переданных полей добавляются вычислимые в данном месте поля

        //        // Время создания данной записи
        //        QDateTime ctime_dt = QDateTime::currentDateTime();
        //        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
        //        record->field("ctime", ctime);

        assert(record->field("ctime") != "");

        // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
        bool is_crypt = false;

        //        if(_tree_item) {
        if(_is_crypt) { //            if(_tree_item->field("crypt") == "1") {
            if(globalparameters.crypt_key().length() > 0)
                is_crypt = true;
            else
                critical_error("RecordTable::insert_new_record() : Can not insert data to crypt branch. Password not setted.");
        }

        //        }

        // Запись полновесных данных с учетом шифрации
        if(is_crypt && record->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
            record->to_encrypt_and_save_lite();
        } else if(!is_crypt && record->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
            record->to_decrypt_and_save_lite();
        } else {
            record->push_lite_attributes();
        }

        //        // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
        //        record->switchToLite();

        //        // Запись добавляется в таблицу конечных записей
        //        int insertPos = -1;

        //        record->is_registered(true);
        assert(record->is_registered());

        if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
            _record_data << record;
            insert_position = _record_data.size() - 1;
        } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
            _record_data.insert(pos, record);
            insert_position = pos;
        } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
            _record_data.insert(pos + 1, record);
            insert_position = pos + 1;
        }

        qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insert_position);

        // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

    }

    return insert_position;
}


// Замена в указанной записи переданных полей на новые значения
void RecordTable::edit_record_fields(int pos, QMap<QString, QString> edit_fields)
{
    qDebug() << "In recordtabledata method edit_record()";

    QMapIterator<QString, QString> i(edit_fields);

    while(i.hasNext()) {
        i.next();
        field(pos, i.key(), i.value());
    }

    // changePersistentIndex(QModelIndex(), QModelIndex());
}


// Удаление записи с указанным индексом
// todo: добавить удаление приаттаченных файлов и очистку таблицы приаттаченных файлов
void RecordTable::delete_record_by_position(int i)
{
    qDebug() << "Try delete record num " << i << " table count " << _record_data.size();

    // Нельзя удалять с недопустимым индексом
    if(i >= _record_data.size())
        return;

    // Удаление директории и файлов внутри, с сохранением в резервной директории
    QString dirForDelete = appconfig.get_tetradir() + "/base/" + field(i, "dir");
    qDebug() << "Remove dir " << dirForDelete;
    DiskHelper::removeDirectoryToTrash(dirForDelete);


    // Удаление позиции курсора из истории
    QString id = field(i, "id");

    if(id.length() > 0)
        walkhistory.removeHistoryData(id);


    //    //
    //    Record record = tableData.at(i);
    //    browser::PageView *view = record.view();
    //    //

    // Начинается удаление записи
    // beginRemoveRows(QModelIndex(),i,i);

    // Удаляется элемент
    _record_data.removeAt(i); // Было takeAt
    qDebug() << "Delete record succesfull";

    //    //
    //    browser::TabManager *tabmanager = view->tabmanager();
    //    tabmanager->closeTab(tabmanager->webViewIndex(view));
    //    //


    // Удаление записи закончено
    // endRemoveRows();
}


void RecordTable::delete_record_by_id(QString id)
{
    for(int i = 0; i < size(); i++) {
        if(field(i, "id") == id) {
            delete_record_by_position(i); // Так как id уникальный, удаляться будет только одна запись
            break;
        }
    }
}


// Удаление всех элементов таблицы конечных записей
void RecordTable::delete_all_records(void)
{
    int tableSize = size(); // Запоминается размер таблицы, так как он при удалении меняется

    for(int i = 0; i < tableSize; i++)
        delete_record_by_position(0);   // Deleted very first record many times   // Удаляется самая первая запись много раз
}


// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
void RecordTable::empty(void)
{
    _record_data.clear();
    //    _tree_item = nullptr;
}

std::shared_ptr<RecordTable> RecordTable::active_subset(boost::intrusive_ptr<TreeItem> start_item)
{
    //    std::shared_ptr<TableData> result = std::make_shared<TableData>();

    //    for(auto &i : _tabledata) {
    //        if(i->unique_page())result->insert_new_record(work_pos(), i);
    //    }

    // bypass slite fat switch:

    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
    start_item->record_table(dommodel);
    return start_item->record_table();

    //    return result;
}


bool RecordTable::is_record_exists(const QString &id)
{
    for(int i = 0; i < size(); i++)
        if(field(i, "id") == id)
            return true;

    return false;
}

bool RecordTable::is_record_exists(const QUrl &url)
{
    bool found = false;

    for(int i = 0; i < size(); i++) {
        std::string compare = difference(field(i, "url").toStdString(), url.toString().toStdString());

        if(compare.size() == 0 || compare == "/") {  // if(getField("url", i) == url.toString())
            found = true;
            break;
        }
    }

    return found;
}

int RecordTable::get_pos_by_id(QString id)
{
    for(int i = 0; i < size(); i++)
        if(field(i, "id") == id)
            return i;

    return -1;
}


// Количество записей в таблице данных
int RecordTable::size(void) const
{
    return _record_data.size();
}


// Перемещение записи вверх на одну строку
void RecordTable::move_up(int pos)
{
    if(pos > 0) {
        // Данные перемещаются
        _record_data.move(pos, pos - 1);

        // Обновляется экран
        // QModelIndex from=index(pos-1);
        // QModelIndex to=index(pos);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Перемещение записи вниз на одну строку
// Move write down one line
void RecordTable::move_dn(int pos)
{
    if(pos < _record_data.count()) {
        // Данные перемещаются
        _record_data.move(pos, pos + 1);

        // Обновляется экран
        // QModelIndex from=index(pos);
        // QModelIndex to=index(pos+1);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Переключение таблицы в зашифрованное состояние
// todo: Добавить шифрацию имени приаттаченных файлов и содержимого файлов
void RecordTable::switch_to_encrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись уже зашифрована, ее шифровать ненужно
        if(field(i, "crypt") == "1")
            continue;

        // Шифрация записи
        _record_data[i]->to_encrypt_and_save_lite(); // В таблице конечных записей хранятся легкие записи
    }
}


// Переключение таблицы в расшифрованное состояние
// todo: добавить расшифрацию имени приаттаченных файлов и содержимого файлов
void RecordTable::switch_to_decrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись не зашифрована, ее не нужно расшифровывать
        if(field(i, "crypt") == "" || field(i, "crypt") == "0")
            continue;

        // Расшифровка записи
        _record_data[i]->to_decrypt_and_save_lite(); // В таблице конечных записей хранятся легкие записи
    }
}


//// Получение ссылки на объект ветки, которой принадлежит таблица
//boost::intrusive_ptr<TreeItem> RecordTable::tree_item(void)
//{
//    return _tree_item;
//}


int RecordTable::work_pos(void)
{
    return _workpos;
}


void RecordTable::work_pos(int pos)
{
    _workpos = pos;
}

void RecordTable::crypt(const bool _is_crypt)
{
    if(_is_crypt && !this->_is_crypt) {
        for(int i = 0; i < size(); i++) {
            record(i)->to_encrypt_fields();
        }
    }

    if(!_is_crypt && this->_is_crypt) {
        for(int i = 0; i < size(); i++) {
            record(i)->to_decrypt_fields();
        }
    }

    if(_is_crypt != this->_is_crypt)this->_is_crypt = _is_crypt;

}
