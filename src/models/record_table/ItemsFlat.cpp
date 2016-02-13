#include <QDomElement>
#include <QMap>
#include <QString>
#include <QDir>
#include <QMessageBox>

#include "main.h"
#include "Record.h"
#include "ItemsFlat.h"

#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/TreeItem.h"
#include "libraries/WalkHistory.h"
#include "models/tree/TreeModelKnow.h"
#include "views/tree/TreeViewKnow.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/DiskHelper.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"
#include "libraries/wyedit/Editor.h"
#include "views/tree/TreeScreen.h"

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
ItemsFlat::ItemsFlat(boost::intrusive_ptr<TreeItem> parent_item
                     , const bool _is_crypt
                    )
    :
    //    : _tree_item(_tree_item)
    //    , _workpos(-1)
    _parent_item([ & ]()
{
    if(parent_item) {
        QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
        QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
        //        QString crypt_value = "1";
        QString crypt_key(QString::null); crypt_key = QLatin1String("crypt");
        QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
                                                           && parent_item->_field_data.contains(crypt_key)
                                                           && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

        //        field(crypt_key, crypt_value);

        if(crypt_1 == crypt_value
           //           && !table_data->crypt()
          ) {
            this->to_encrypt(); // table_data->to_encrypt();
        } else if(crypt_0 == crypt_value
                  //                  && table_data->crypt()
                 ) {
            this->to_decrypt(); // table_data->to_decrypt();
        }
    }

    return parent_item;
}())
//    , _field_data([ & ]()
//{
//    if(_parent_item)
//        _field_data["crypt"] = (_parent_item->_field_data.contains("crypt") && _parent_item->_field_data["crypt"] == "1") ? "1" : "0";

//    return _field_data;
//}())
//    , _record_table()
, _is_crypt(_is_crypt)
{
    //    treeItem = nullptr;
    //    workPos = -1;

    //    init(i_dom_element);  // i_dom_element

    //    // Создание таблицы
    //    if(!i_dom_element.isNull()) {
    //        //        QDomElement *dom_element = &i_dom_element;
    //        import_from_dom(
    //            i_dom_element  // dom_element
    //        );
    //    }

    //    return;
}
// Деструктор
ItemsFlat::~ItemsFlat()
{
    clear();
    return;
}


// Получение значения указанного поля для указанного имени поля
// Имя поля - все возможные имена полей, кроме text (такого поля теперь вообще нет, текст запрашивается как отдельное свойство)
QString ItemsFlat::field(int pos, QString name) const
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _child_items.size()) {
        QString i;
        i.setNum(pos);
        critical_error("RecordTableData::getField() : get unavailable record index " + i);
    }

    return _child_items.at(pos)->field(name);
}

boost::intrusive_ptr<TreeItem> ItemsFlat::find(const QUrl &url)const
{
    boost::intrusive_ptr<TreeItem> record;

    for(auto &i : _child_items) {
        if(i->field("url") == url.toString()) {
            assert(i->is_registered());
            record = i;
            break;
        }
    }

    return record;
}

int ItemsFlat::locate(boost::intrusive_ptr<TreeItem> item)const
{
    int pos = -1;

    for(int i = 0 ; i < _child_items.size(); i++) {
        auto it = _child_items.value(i);

        if(it->field("id") == item->field("id")) {
            assert(it->is_registered());
            pos = i;
            break;
        }
    }

    return pos;
}

int ItemsFlat::index(boost::intrusive_ptr<TreeItem> item)const
{
    int in = 0;
    int result = 0;

    for(auto &i : _child_items) {

        if(i == item) {
            assert(i->is_registered());
            result = in;
            break;
        }

        in++;
    }

    return result;
}

boost::intrusive_ptr<TreeItem> ItemsFlat::find(boost::intrusive_ptr<TreeItem> item)const
{
    boost::intrusive_ptr<TreeItem> result;

    for(auto &i : _child_items) {
        if(i->field("id") == item->field("id")) {
            assert(i->is_registered());
            result = i;
            break;
        }
    }

    return result;
}

// Установка значения указанного поля для указанного элемента
void ItemsFlat::field(int pos, QString name, QString value)
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _child_items.size()) {
        QString i;
        i.setNum(pos);
        critical_error("In RecordTableData::setField() unavailable record index " + i + " in table while field " + name + " try set to " + value);
    }

    _child_items[pos]->field(name, value);
}


// Получение значения текста указанной записи
// Метод возвращает расшифрованные данные
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
// Get the value of the text of the specified record
// Method returns the decrypted data
// If there is a problem that file with text entries, will create an empty file
QString ItemsFlat::text(int pos)const
{
    // Если индекс недопустимый, возвращается пустая строка
    if(pos < 0 || pos >= size())
        return QString();

    if(_child_items[pos]->is_lite())
        return _child_items[pos]->text_direct_from_lite();
    else
        return _child_items[pos]->text_from_fat();
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
void ItemsFlat::editor_load_callback(QObject *editor, QString &loadText)
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
void ItemsFlat::editor_save_callback(QObject *editor, QString saveText)
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
boost::intrusive_ptr<TreeItem> ItemsFlat::item_lite(int pos)const
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return boost::intrusive_ptr<TreeItem>(nullptr);

    // Хранимая в дереве запись не может быть "тяжелой"
    if(!_child_items.at(pos)->is_lite())
        critical_error("In RecordTableData::getRecordLite() try get fat record");

    return _child_items.at(pos);
}


// Получение копии полного образа записи
// Возвращается запись с "сырыми" данными. Если запись была зашифрована, метод вернет зашифрованные данные
// Get a copy of the full image recording
// Returns the record with "raw" data. If the record was encrypted, the method returns the encrypted data
boost::intrusive_ptr<TreeItem> ItemsFlat::item_fat(int pos)
{
    // Копия записи из дерева
    boost::intrusive_ptr<TreeItem> result = item(pos);  //boost::intrusive_ptr<Record> resultRecord = getRecordLite(pos);

    // original
    // Переключение копии записи на режим с хранением полного содержимого
    if(result->is_lite())result->to_fat();

    // Добавление текста записи
    result->text_to_fat(text(pos));

    // Добавление бинарных образов файлов картинок
    QString directory = appconfig.get_tetradir() + "/base/" + result->field("dir");
    result->picture_files(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    return result;
}


boost::intrusive_ptr<TreeItem> ItemsFlat::item(int pos)const
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return nullptr;

    return _child_items.at(pos);    // _child_items[pos];
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


//// Разбор DOM модели и преобразование ее в таблицу
//void RecordTable::import_from_dom(const QDomElement &dom_model)
//{
//    // QDomElement n = dommodel.documentElement();
//    // QDomElement n = dommodel;

//    // qDebug() << "In recordtabledata setup_data_from_dom() start";

//    // Если принятый элемент не является таблицей
//    if(dom_model.tagName() != "recordtable")
//        return;

//    // Определяется указатель на первый элемент с записью
//    // Define a pointer to the first element of the recording
//    QDomElement current_record_dom = dom_model.firstChildElement("record");

//    while(!current_record_dom.isNull()) {
//        QMap<QString, QString> data;
//        // Структура, куда будет помещена текущая запись
//        // The structure, which will put the current record
//        boost::intrusive_ptr<TreeItem> current_item = boost::intrusive_ptr<TreeItem>(
//                                                          new TreeItem(
//                                                              data
//                                                              , boost::intrusive_ptr<TreeItem>(const_cast<TreeItem>(this))    // _parent_item
//                                                          )
//                                                      );
//        current_item->is_registered(true);

//        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
//        // The current record is added to the final table of records (and located at a certain address in memory)
//        _child_items << current_item;

//        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
//        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
//        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
//        // Order in subordinate objects have registered a valid pointer to this entry
//        (_child_items.last())->import_from_dom(current_record_dom);

//        current_record_dom = current_record_dom.nextSiblingElement("record");
//    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



//    return;
//}

QDomElement ItemsFlat::export_to_dom() const
{
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    return export_to_dom(doc);
}

// Преобразование таблицы конечных записей в Dom документ
QDomElement ItemsFlat::export_to_dom(std::shared_ptr<QDomDocument> doc) const
{
    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_child_items.size() == 0)
        return QDomElement();

    QDomElement item_flat_dom = doc->createElement("recordtable");

    // Пробегаются все записи в таблице
    for(int i = 0; i < _child_items.size(); i++) {
        //        assert(boost::static_pointer_cast<ItemsFlat>(_child_items.at(i).get()) != this);

        if(boost::static_pointer_cast<ItemsFlat>(_child_items.at(i).get()) != this)
            item_flat_dom.appendChild(
                boost::static_pointer_cast<ItemsFlat>(_child_items.at(i).get())->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи
    }

    // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

    return item_flat_dom;
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
QDomElement ItemsFlat::export_activated_dom(std::shared_ptr<QDomDocument> doc) const
{
    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_child_items.size() == 0)
        return QDomElement();

    QDomElement record_dom_data = doc->createElement("recordtable");

    // Пробегаются все записи в таблице
    for(int i = 0; i < _child_items.size(); i++) {
        if(_child_items.at(i)->page_valid() //unique_page()
          ) {
            record_dom_data.appendChild(boost::static_pointer_cast<ItemsFlat>(_child_items.at(i).get())->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи
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
int ItemsFlat::insert_new_item(int pos, boost::intrusive_ptr<TreeItem> item, int mode)
{
    // Запись добавляется в таблицу конечных записей
    int insert_position = -1;

    // get shadow_branch
    //    TreeModelKnow *shadow_branch = globalparameters.tree_screen()->_shadow_branch;  // static_cast<TreeModelKnow *>(find_object<TreeViewKnow>(knowtreeview_singleton_name)->model());

    if(!find(item)
       //       && !shadow_branch->is_record_id_exists(item->field("id"))
      ) {

        //        if(_tree_item) qDebug() << "RecordTable::insert_new_record() : Insert new record to branch " << _tree_item->all_fields();

        // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
        if(item->is_lite() == true)
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

        if(0 == item->field("id").length()  //           || dataModel->isRecordIdExists(record->getField("id"))  // ? Is this a correct policy? can we only create a pure view?
          ) {
            // Создается новая запись (ID был пустой) или
            // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)
            // Create a new record (ID was empty) or
            // Record with ID in a tree there, so stand the new ID and a new storage directory (not to overwrite existing)

            QString id = get_unical_id();
            // Store directory entries and files    // Директория хранения записи и файл
            item->field("dir", id);   // get_unical_id()

            item->field("file", "text.html");

            // Unique ID of XML records             // Уникальный идентификатор XML записи
            //            QString id = get_unical_id();
            item->field("id", id);
        }


        // В список переданных полей добавляются вычислимые в данном месте поля

        // Время создания данной записи
        QDateTime ctime_dt = QDateTime::currentDateTime();
        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
        item->field("ctime", ctime);


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
        if(is_crypt && item->field("crypt") != "1") {       // В зашифрованную ветку незашифрованную запись
            item->to_encrypt_and_save_fat();
        } else if(!is_crypt && item->field("crypt") == "1") { // В незашифрованную ветку зашифрованную запись
            item->to_decrypt_and_save_fat();
        } else {
            item->push_fat_attributes();
        }

        // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
        item->to_lite();

        //        // Запись добавляется в таблицу конечных записей
        //        int insertPos = -1;

        item->is_registered(true);

        if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
            _child_items << item;
            //            insert_position = _child_items.size() - 1;
        } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
            _child_items.insert(pos, item);
            //            insert_position = pos;
        } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
            _child_items.insert(pos + 1, item);
            //            insert_position = pos + 1;
        }

        insert_position = index(item);
        qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insert_position);

        // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

    }

    assert(_child_items[insert_position] == item);
    return insert_position;

}

int ItemsFlat::shadow_item_lite(int pos, boost::intrusive_ptr<TreeItem> record, int mode)
{
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
            _child_items << record;
            insert_position = _child_items.size() - 1;
        } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
            _child_items.insert(pos, record);
            insert_position = pos;
        } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
            _child_items.insert(pos + 1, record);
            insert_position = pos + 1;
        }

        qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insert_position);

        // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода

    }

    return insert_position;
}


// Замена в указанной записи переданных полей на новые значения
void ItemsFlat::fields(int pos, QMap<QString, QString> edit_fields)
{
    qDebug() << "In RecordTable method edit_record_fields()";

    QMapIterator<QString, QString> i(edit_fields);

    while(i.hasNext()) {
        i.next();
        field(pos, i.key(), i.value());
    }

    // changePersistentIndex(QModelIndex(), QModelIndex());
}


// Удаление записи с указанным индексом
// todo: добавить удаление приаттаченных файлов и очистку таблицы приаттаченных файлов
void ItemsFlat::delete_item_by_position(int i)
{
    qDebug() << "Try delete record num " << i << " table count " << _child_items.size();

    // Нельзя удалять с недопустимым индексом
    if(i >= _child_items.size())
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
    _child_items.removeAt(i); // Было takeAt
    qDebug() << "Delete record succesfull";

    //    //
    //    browser::TabManager *tabmanager = view->tabmanager();
    //    tabmanager->closeTab(tabmanager->webViewIndex(view));
    //    //


    // Удаление записи закончено
    // endRemoveRows();
}


void ItemsFlat::delete_item_by_id(QString id)
{
    for(int i = 0; i < size(); i++) {
        if(field(i, "id") == id) {
            delete_item_by_position(i); // Так как id уникальный, удаляться будет только одна запись
            break;
        }
    }
}


// Удаление всех элементов таблицы конечных записей
void ItemsFlat::delete_all_items(void)
{
    int tableSize = size(); // Запоминается размер таблицы, так как он при удалении меняется

    for(int i = 0; i < tableSize; i++)
        delete_item_by_position(0);   // Deleted very first record many times   // Удаляется самая первая запись много раз
}


// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
void ItemsFlat::clear(void)
{
    _child_items.clear();
    //    _tree_item = nullptr;
}

//boost::intrusive_ptr<TreeItem> RecordTable::active_subset(
//    //    boost::intrusive_ptr<TreeItem> start_item
//)
//{
//    //    std::shared_ptr<TableData> result = std::make_shared<TableData>();

//    //    for(auto &i : _tabledata) {
//    //        if(i->unique_page())result->insert_new_record(work_pos(), i);
//    //    }

//    // bypass slite fat switch:

//    //    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
//    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
//    auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
//    QMap<QString, QString> data;
//    boost::intrusive_ptr<TreeItem> tree = new TreeItem(
//        data
//        , boost::intrusive_ptr<TreeItem>(this)   // _parent_item
//    );
//    tree->import_from_dom(dommodel);

//    return  tree;   // new TreeItem(data, _parent_item);

//    //    return result;
//}


bool ItemsFlat::is_item_exists(const QString &id)const
{
    for(int i = 0; i < size(); i++)
        if(field(i, "id") == id)
            return true;

    return false;
}

bool ItemsFlat::is_item_exists(const QUrl &url)const
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

int ItemsFlat::get_pos_by_id(QString id)const
{
    for(int i = 0; i < size(); i++)
        if(field(i, "id") == id)
            return i;

    return -1;
}


// Количество записей в таблице данных
int ItemsFlat::size(void) const
{
    return _child_items.size();
}


// Перемещение записи вверх на одну строку
void ItemsFlat::move_up(int pos)
{
    if(pos > 0) {
        // Данные перемещаются
        _child_items.move(pos, pos - 1);

        // Обновляется экран
        // QModelIndex from=index(pos-1);
        // QModelIndex to=index(pos);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Перемещение записи вниз на одну строку
// Move write down one line
void ItemsFlat::move_dn(int pos)
{
    if(pos < _child_items.count()) {
        // Данные перемещаются
        _child_items.move(pos, pos + 1);

        // Обновляется экран
        // QModelIndex from=index(pos);
        // QModelIndex to=index(pos+1);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Переключение таблицы в зашифрованное состояние
// todo: Добавить шифрацию имени приаттаченных файлов и содержимого файлов
void ItemsFlat::to_encrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись уже зашифрована, ее шифровать ненужно
        if(field(i, "crypt") == "1")
            continue;

        // Шифрация записи
        _child_items[i]->to_encrypt_and_save_lite(); // В таблице конечных записей хранятся легкие записи
    }
}


// Переключение таблицы в расшифрованное состояние
// todo: добавить расшифрацию имени приаттаченных файлов и содержимого файлов
void ItemsFlat::to_decrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись не зашифрована, ее не нужно расшифровывать
        if(field(i, "crypt") == "" || field(i, "crypt") == "0")
            continue;

        // Расшифровка записи
        _child_items[i]->to_decrypt_and_save_lite(); // В таблице конечных записей хранятся легкие записи
    }
}


//// Получение ссылки на объект ветки, которой принадлежит таблица
//boost::intrusive_ptr<TreeItem> RecordTable::tree_item(void)
//{
//    return _tree_item;
//}


int ItemsFlat::work_pos(void)const
{
    return _workpos;
}


void ItemsFlat::work_pos(int pos)
{
    _workpos = pos;
}

void ItemsFlat::crypt(const bool _is_crypt)
{
    if(_is_crypt && !this->_is_crypt) {
        for(int i = 0; i < size(); i++) {
            item(i)->to_encrypt_fields();
        }
    }

    if(!_is_crypt && this->_is_crypt) {
        for(int i = 0; i < size(); i++) {
            item(i)->to_decrypt_fields();
        }
    }

    if(_is_crypt != this->_is_crypt)this->_is_crypt = _is_crypt;

}
