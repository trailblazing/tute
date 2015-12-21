#include <QDomElement>
#include <QMap>
#include <QString>
#include <QDir>
#include <QMessageBox>

#include "main.h"
#include "Record.h"
#include "TableData.h"

#include "models/appConfig/AppConfig.h"
#include "views/mainWindow/MainWindow.h"
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


// Это набор данных конечной таблицы, с которыми удобно работать

// Конструктор
TableData::TableData(void): _treeitem(nullptr), _workpos(-1)
{
    //    treeItem = NULL;

    //    workPos = -1;

    //    return;
}


// Деструктор
TableData::~TableData()
{
    empty();
    return;
}


// Получение значения указанного поля для указанного имени поля
// Имя поля - все возможные имена полей, кроме text (такого поля теперь вообще нет, текст запрашивается как отдельное свойство)
QString TableData::field(QString name, int pos) const
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _tabledata.size()) {
        QString i;
        i.setNum(pos);
        criticalError("RecordTableData::getField() : get unavailable record index " + i);
    }

    return _tabledata.at(pos)->getField(name);
}

std::shared_ptr<Record> TableData::find(const QUrl &url)
{
    std::shared_ptr<Record> record;

    for(auto &i : _tabledata) {
        // QString _u = i.getNaturalFieldSource("url") ;

        if(i->getNaturalFieldSource("url") == url.toString()) {
            assert(i->is_registered());
            record = i;
            break;
        }
    }

    return record;
}

std::shared_ptr<Record> TableData::find(std::shared_ptr<Record> r)
{
    std::shared_ptr<Record> record;

    for(auto &i : _tabledata) {
        // QString _u = i.getNaturalFieldSource("url") ;

        if(i->getNaturalFieldSource("id") == r->getNaturalFieldSource("id")) {
            assert(i->is_registered());
            record = i;
            break;
        }
    }

    return record;
}

// Установка значения указанного поля для указанного элемента
void TableData::field(QString name, QString value, int pos)
{
    // Если индекс недопустимый
    if(pos < 0 || pos >= _tabledata.size()) {
        QString i;
        i.setNum(pos);
        criticalError("In RecordTableData::setField() unavailable record index " + i + " in table while field " + name + " try set to " + value);
    }

    _tabledata[pos]->setField(name, value);
}


// Получение значения текста указанной записи
// Метод возвращает расшифрованные данные
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
// Get the value of the text of the specified record
// Method returns the decrypted data
// If there is a problem that file with text entries, will create an empty file
QString TableData::text(int pos)
{
    // Если индекс недопустимый, возвращается пустая строка
    if(pos < 0 || pos >= size())
        return QString();

    if(_tabledata[pos]->isLite())
        return _tabledata[pos]->getTextDirectFromLite();
    else
        return _tabledata[pos]->getTextFromFat();
}


// Функция, которая заменяет стандартную функцию редактора по считыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и ссылку на переменную loadText, которую надо заполнить
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить
void TableData::editor_load_callback(QObject *editor,
                                           QString &loadText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли дешифровать данные при чтении
    bool workWithCrypt = false;

    if(currEditor->getMiscField("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.getCryptKey().length() == 0) {
            loadText = "";
            return;
        }

        workWithCrypt = true;
    }

    // Файл, с которым работает редактор
    QString fileName = currEditor->get_work_directory() + "/" + currEditor->get_file_name();

    QFile f(fileName);

    // Если нужный файл не существует
    if(!f.exists())
        criticalError("File " + fileName + " not found");

    // Открывается файл
    if(!f.open(QIODevice::ReadOnly))
        criticalError("File " + fileName + " not readable. Check permission.");

    // Если незашифровано
    if(workWithCrypt == false)
        loadText = QString::fromUtf8(f.readAll());
    else
        loadText = CryptService::decryptStringFromByteArray(globalparameters.getCryptKey(), f.readAll()); // Если зашифровано
}


// Функция, которая заменяет стандартную функцию редактора по записыванию
// редактируемого текста
// Ее вызывает редактор, передавая указатель на себя
// и текст который надо записать в переменной saveText
// Внимание! Метод не содержит работы с данными записи. Подумать, где его разместить
void TableData::editor_save_callback(QObject *editor,
                                           QString saveText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли шифровать записываемый текст
    bool workWithCrypt = false;

    if(currEditor->getMiscField("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.getCryptKey().length() == 0)
            return;

        workWithCrypt = true;
    }

    QString fileName = currEditor->get_work_directory() + "/" + currEditor->get_file_name();

    // Если шифровать ненужно
    if(workWithCrypt == false) {
        // Текст сохраняется в файл
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
            criticalError("RecordTableData::editor_save_callback() : Cant open text file " + fileName + " for write.");

        QTextStream out(&wfile);
        out.setCodec("UTF-8");
        out << saveText;
    } else {
        // Текст шифруется
        QByteArray encryptData = CryptService::encryptStringToByteArray(globalparameters.getCryptKey(), saveText);

        // В файл сохраняются зашифрованные данные
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly))
            criticalError("RecordTableData::editor_save_callback() : Cant open binary file " + fileName + " for write.");

        wfile.write(encryptData);
    }


    // Вызывается сохранение картинок
    // В данной реализации картинки сохраняются незашифрованными
    currEditor->save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
}


// Получение копии легкого образа записи
// Эти образы используются для хранения в дереве знаний
// Get a copy of a light image recording
// These images are used to store the tree of knowledge
std::shared_ptr<Record> TableData::record_lite(int pos)
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return std::make_shared<Record>();

    // Хранимая в дереве запись не может быть "тяжелой"
    if(!_tabledata.at(pos)->isLite())
        criticalError("In RecordTableData::getRecordLite() try get fat record");

    return _tabledata.at(pos);
}


// Получение копии полного образа записи
// Возвращается запись с "сырыми" данными. Если запись была зашифрована, метод вернет зашифрованные данные
// Get a copy of the full image recording
// Returns the record with "raw" data. If the record was encrypted, the method returns the encrypted data
std::shared_ptr<Record> TableData::record_fat(int pos)
{
    // Копия записи из дерева
    std::shared_ptr<Record> resultRecord = record(pos);  //std::shared_ptr<Record> resultRecord = getRecordLite(pos);

    // original
    // Переключение копии записи на режим с хранением полного содержимого
    if(resultRecord->isLite())resultRecord->switchToFat();

    // Добавление текста записи
    resultRecord->setTextToFat(text(pos));

    // Добавление бинарных образов файлов картинок
    QString directory = appconfig.get_tetradir() + "/base/" + resultRecord->getField("dir");
    resultRecord->setPictureFiles(DiskHelper::getFilesFromDirectory(directory, "*.png"));

    return resultRecord;
}


std::shared_ptr<Record> TableData::record(int pos)
{
    // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= size())
        return nullptr;

    return _tabledata[pos];
}


// Инициализация таблицы данных на основе переданного DOM-элемента
void TableData::init(std::shared_ptr<TreeItem> item, QDomElement iDomElement)
{
    // Создание таблицы
    if(!iDomElement.isNull()) {
        QDomElement *domElement = &iDomElement;
        data_from_dom(domElement);
    }

    // Запоминается ссылка на ветку, которой принадлежит данная таблица
    _treeitem = item;
}


// Разбор DOM модели и преобразование ее в таблицу
void TableData::data_from_dom(QDomElement *domModel)
{
    // QDomElement n = dommodel.documentElement();
    // QDomElement n = dommodel;

    // qDebug() << "In recordtabledata setup_data_from_dom() start";

    // Если принятый элемент не является таблицей
    if(domModel->tagName() != "recordtable")
        return;

    // Определяется указатель на первый элемент с записью
    // Define a pointer to the first element of the recording
    QDomElement currentRecordDom = domModel->firstChildElement("record");

    while(!currentRecordDom.isNull()) {
        // Структура, куда будет помещена текущая запись
        // The structure, which will put the current record
        std::shared_ptr<Record> currentRecord = std::make_shared<Record>();
        currentRecord->is_registered(true);

        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
        // The current record is added to the final table of records (and located at a certain address in memory)
        _tabledata << currentRecord;

        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
        // Order in subordinate objects have registered a valid pointer to this entry
        (_tabledata.last())->setupDataFromDom(currentRecordDom);

        currentRecordDom = currentRecordDom.nextSiblingElement("record");
    } // Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>



    return;
}


// Преобразование таблицы конечных записей в Dom документ
QDomElement TableData::dom_from_data(QDomDocument *doc) const
{
    // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_tabledata.size() == 0)
        return QDomElement();

    QDomElement recordtable_domdata = doc->createElement("recordtable");

    // Пробегаются все записи в таблице
    for(int i = 0; i < _tabledata.size(); i++)
        recordtable_domdata.appendChild(_tabledata.at(i)->exportDataToDom(doc));     // К элементу recordtabledata прикрепляются конечные записи

    // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

    return recordtable_domdata;
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
int TableData::insert_new_record(int mode
                                       , int pos
                                       , std::shared_ptr<Record> record)
{
    //    std::shared_ptr<Record> record = std::make_shared<Record>(record_);


    if(_treeitem != nullptr) qDebug() << "RecordTableData::insert_new_record() : Insert new record to branch " << _treeitem->getAllFields();

    // The method must take a full-fledged object record    // Мотод должен принять полновесный объект записи
    if(record->isLite() == true)
        criticalError("RecordTableData::insertNewRecord() can't insert lite record");

    // Выясняется, есть ли в дереве запись с указанным ID
    // Если есть, то генерируются новые ID для записи и новая директория хранения
    // Если нет, то это значит что запись была вырезана, но хранится в буфере,
    // и ее желательно вставить с прежним ID и прежним именем директории
    KnowTreeModel *dataModel = static_cast<KnowTreeModel *>(find_object<KnowTreeView>("knowTreeView")->model());

    if(record->getField("id").length() == 0 ||
       dataModel->isRecordIdExists(record->getField("id"))) {
        // Создается новая запись (ID был пустой) или
        // Запись с таким ID в дереве есть, поэтому выделяются новый ID и новая директория хранения (чтобы не затереть существующие)

        // Директория хранения записи и файл
        record->setField("dir", get_unical_id());
        record->setField("file", "text.html");

        // Уникальный идентификатор XML записи
        QString id = get_unical_id();
        record->setField("id", id);
    }


    // В список переданных полей добавляются вычислимые в данном месте поля

    // Время создания данной записи
    QDateTime ctime_dt = QDateTime::currentDateTime();
    QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
    record->setField("ctime", ctime);


    // Выясняется в какой ветке вставляется запись - в зашифрованной или нет
    bool isCrypt = false;

    if(_treeitem != nullptr) {
        if(_treeitem->getField("crypt") == "1") {
            if(globalparameters.getCryptKey().length() > 0)
                isCrypt = true;
            else
                criticalError("RecordTableData::insertNewRecord() : Can not insert data to crypt branch. Password not setted.");
        }
    }

    // Запись полновесных данных с учетом шифрации
    if(isCrypt && record->getField("crypt") != "1") // В зашифрованную ветку незашифрованную запись
        record->switchToEncryptAndSaveFat();
    else if(!isCrypt && record->getField("crypt") == "1") // В незашифрованную ветку зашифрованную запись
        record->switchToDecryptAndSaveFat();
    else
        record->pushFatAttributes();

    // Record switch to easy mode to be added to the final table of records ?***    // Запись переключается в легкий режим чтобы быть добавленной в таблицу конечных записей
    record->switchToLite();

    // Запись добавляется в таблицу конечных записей
    int insertPos = -1;

    record->is_registered(true);

    if(mode == ADD_NEW_RECORD_TO_END) {         // В конец списка
        _tabledata << record;
        insertPos = _tabledata.size() - 1;
    } else if(mode == ADD_NEW_RECORD_BEFORE) {  // Перед указанной позицией
        _tabledata.insert(pos, record);
        insertPos = pos;
    } else if(mode == ADD_NEW_RECORD_AFTER) {   // После указанной позиции
        _tabledata.insert(pos + 1, record);
        insertPos = pos + 1;
    }

    qDebug() << "RecordTableData::insert_new_record() : New record pos" << QString::number(insertPos);

    // Возвращается номера строки, на которую должна быть установлена засветка после выхода из данного метода


    return insertPos;
}


// Замена в указанной записи переданных полей на новые значения
void TableData::edit_record_fields(int pos,
                                         QMap<QString, QString> editFields)
{
    qDebug() << "In recordtabledata method edit_record()";

    QMapIterator<QString, QString> i(editFields);

    while(i.hasNext()) {
        i.next();
        field(i.key(), i.value(), pos);
    }

    // changePersistentIndex(QModelIndex(), QModelIndex());
}


// Удаление записи с указанным индексом
// todo: добавить удаление приаттаченных файлов и очистку таблицы приаттаченных файлов
void TableData::delete_record(int i)
{
    qDebug() << "Try delete record num " << i << " table count " << _tabledata.size();

    // Нельзя удалять с недопустимым индексом
    if(i >= _tabledata.size())
        return;

    // Удаление директории и файлов внутри, с сохранением в резервной директории
    QString dirForDelete = appconfig.get_tetradir() + "/base/" + field("dir", i);
    qDebug() << "Remove dir " << dirForDelete;
    DiskHelper::removeDirectoryToTrash(dirForDelete);


    // Удаление позиции курсора из истории
    QString id = field("id", i);

    if(id.length() > 0)
        walkhistory.removeHistoryData(id);


    //    //
    //    Record record = tableData.at(i);
    //    browser::PageView *view = record.view();
    //    //

    // Начинается удаление записи
    // beginRemoveRows(QModelIndex(),i,i);

    // Удаляется элемент
    _tabledata.removeAt(i); // Было takeAt
    qDebug() << "Delete record succesfull";

    //    //
    //    browser::TabManager *tabmanager = view->tabmanager();
    //    tabmanager->closeTab(tabmanager->webViewIndex(view));
    //    //


    // Удаление записи закончено
    // endRemoveRows();
}


void TableData::delete_record_by_id(QString id)
{
    for(int i = 0; i < size(); i++)
        if(field("id", i) == id)
            delete_record(i); // Так как id уникальный, удаляться будет только одна запись
}


// Удаление всех элементов таблицы конечных записей
void TableData::delete_all_records(void)
{
    int tableSize = size(); // Запоминается размер таблицы, так как он при удалении меняется

    for(int i = 0; i < tableSize; i++)
        delete_record(0);   // Deleted very first record many times   // Удаляется самая первая запись много раз
}


// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
void TableData::empty(void)
{
    _tabledata.clear();
    _treeitem = nullptr;
}


bool TableData::is_record_exists(const QString &id)
{
    for(int i = 0; i < size(); i++)
        if(field("id", i) == id)
            return true;

    return false;
}

bool TableData::is_record_exists(const QUrl &url)
{
    bool found = false;

    for(int i = 0; i < size(); i++) {
        std::string compare = getDifference(field("url", i).toStdString(), url.toString().toStdString());

        if(compare.size() == 0 || compare == "/") {  // if(getField("url", i) == url.toString())
            found = true;
            break;
        }
    }

    return found;
}

int TableData::get_pos_by_id(QString id)
{
    for(int i = 0; i < size(); i++)
        if(field("id", i) == id)
            return i;

    return -1;
}


// Количество записей в таблице данных
int TableData::size(void) const
{
    return _tabledata.size();
}


// Перемещение записи вверх на одну строку
void TableData::move_up(int pos)
{
    if(pos > 0) {
        // Данные перемещаются
        _tabledata.move(pos, pos - 1);

        // Обновляется экран
        // QModelIndex from=index(pos-1);
        // QModelIndex to=index(pos);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Перемещение записи вниз на одну строку
// Move write down one line
void TableData::move_dn(int pos)
{
    if(pos < _tabledata.count()) {
        // Данные перемещаются
        _tabledata.move(pos, pos + 1);

        // Обновляется экран
        // QModelIndex from=index(pos);
        // QModelIndex to=index(pos+1);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}


// Переключение таблицы в зашифрованное состояние
// todo: Добавить шифрацию имени приаттаченных файлов и содержимого файлов
void TableData::switch_to_encrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись уже зашифрована, ее шифровать ненужно
        if(field("crypt", i) == "1")
            continue;

        // Шифрация записи
        _tabledata[i]->switchToEncryptAndSaveLite(); // В таблице конечных записей хранятся легкие записи
    }
}


// Переключение таблицы в расшифрованное состояние
// todo: добавить расшифрацию имени приаттаченных файлов и содержимого файлов
void TableData::switch_to_decrypt(void)
{
    // Перебор записей
    for(int i = 0; i < size(); i++) {
        // Если запись не зашифрована, ее не нужно расшифровывать
        if(field("crypt", i) == "" || field("crypt", i) == "0")
            continue;

        // Расшифровка записи
        _tabledata[i]->switchToDecryptAndSaveLite(); // В таблице конечных записей хранятся легкие записи
    }
}


// Получение ссылки на объект ветки, которой принадлежит таблица
std::shared_ptr<TreeItem> TableData::item(void)
{
    return _treeitem;
}


int TableData::work_pos(void)
{
    return _workpos;
}


void TableData::work_pos(int pos)
{
    _workpos = pos;
}
