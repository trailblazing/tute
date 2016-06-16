#include <QDir>
#include <QDomElement>
#include <QMap>
#include <QMessageBox>
#include <QString>

#include "ItemsFlat.h"
#include "Record.h"
#include "linker.hxx"
#include "main.h"

#include "libraries/DiskHelper.h"
#include "libraries/GlobalParameters.h"
#include "libraries/WalkHistory.h"
#include "libraries/crypt/CryptService.h"
#include "libraries/wyedit/Editor.h"
#include "models/app_config/AppConfig.h"
#include "models/tree/KnowModel.h"
#include "models/tree/TreeItem.h"
#include "models/tree/binder.hxx"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/MainWindow.h"
#include "views/tree/KnowView.h"
#include "views/tree/TreeScreen.h"
#include "views/browser/entrance.h"


extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern WalkHistory walkhistory;

const int add_new_record_to_end = 0;
const int add_new_record_before = 1;
const int add_new_record_after = 2;

// const int add_new_record_to_end = 0;

// Это набор данных конечной таблицы, с которыми удобно работать

//// Конструктор
// RecordTable::RecordTable(boost::intrusive_ptr<TreeItem> _tree_item)
//// , QDomElement i_dom_element
////    : _tree_item(_tree_item)
////    , _workpos(-1)
// {
////    treeItem = nullptr;
////    workPos = -1;

// init(_tree_item->export_to_dom());  // i_dom_element

////    return;
// }

ItemsFlat::ItemsFlat(const QDomElement &_dom_element, const bool _is_crypt)	// , boost::intrusive_ptr<TreeItem> _parent_item
    : dom_from_itemsflat_impl([&](std::shared_ptr<QDomDocument> doc) -> QDomElement {				// Преобразование таблицы конечных записей в Dom документ
                // Если у ветки нет таблицы конечных записей, возвращается пустой документ
              if(_child_linkers.size() == 0)return QDomElement();
              QDomElement item_flat_dom = doc->createElement("recordtable");
                // Пробегаются все записи в таблице
              for(int i = 0; i < _child_linkers.size(); i ++){
                  if(static_cast<ItemsFlat *>(_child_linkers.at(i)->host().get()) != this){

                      item_flat_dom.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem_impl(doc));					// boost::static_pointer_cast<Record>(_child_items.at(i))->dom_from_record(doc)
                        // К элементу recordtabledata прикрепляются конечные записи
                  }
              }
                // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

              return item_flat_dom;
          })
      , _is_crypt(_is_crypt){
        //
    if(! _dom_element.isNull()){				// QDomElement *dom_element = &i_dom_element;
        dom_to_itemsflat(_dom_element);								// dom_element
        // , _parent_item
    }
    if(_is_crypt){
        this->to_encrypt();								// table_data->to_encrypt();
    }else{
        this->to_decrypt();								// table_data->to_decrypt();
    }
}

// Конструктор
ItemsFlat::ItemsFlat(const bool _is_crypt)
    :

// : _tree_item(_tree_item)
// , _workpos(-1)
// _parent_item([ & ]()
// {
// if(parent_item) {
// QString crypt_1(QString::null); crypt_1 = QLatin1String("1");
// QString crypt_0(QString::null); crypt_0 = QLatin1String("0");
////        QString crypt_value = "1";
// QString crypt_key(QString::null); crypt_key = QLatin1String(boost::mpl::c_str < crypt_type > ::value);
// QString crypt_value(QString::null); crypt_value = (parent_item->_field_data.size() > 0
// && parent_item->_field_data.contains(crypt_key)
// && (parent_item->_field_data[crypt_key] == crypt_value)) ? crypt_1 : crypt_0;

////        field(crypt_key, crypt_value);

// if(crypt_1 == crypt_value
////           && !table_data->crypt()
// ) {
// this->to_encrypt(); // table_data->to_encrypt();
// } else if(crypt_0 == crypt_value
////                  && table_data->crypt()
// ) {
// this->to_decrypt(); // table_data->to_decrypt();
// }
// }

// return parent_item;
// }())

// , _field_data([ & ]()
// {
// if(_parent_item)
// _field_data[boost::mpl::c_str < crypt_type > ::value] = (_parent_item->_field_data.contains(boost::mpl::c_str < crypt_type > ::value) && _parent_item->_field_data[boost::mpl::c_str < crypt_type > ::value] == "1") ? "1" : "0";

// return _field_data;
// }())
// , _record_table()

// ,

      dom_from_itemsflat_impl([&](std::shared_ptr<QDomDocument> doc) -> QDomElement {				// Преобразование таблицы конечных записей в Dom документ
                // Если у ветки нет таблицы конечных записей, возвращается пустой документ
              QDomElement item_flat_dom;
              if(_child_linkers.size() != 0){					// if(_child_linkers.size() == 0)return QDomElement();
                  item_flat_dom = doc->createElement("recordtable");
                        // Пробегаются все записи в таблице
                  for(int i = 0; i < _child_linkers.size(); i ++){
                      if(static_cast<ItemsFlat *>(_child_linkers.at(i)->host().get()) != this){					// history error

                          item_flat_dom.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem_impl(doc));					// boost::static_pointer_cast<Record>(_child_items.at(i))->dom_from_record(doc)
                                // К элементу recordtabledata прикрепляются конечные записи
                      }
                  }
              }
                // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

              return item_flat_dom;
          })
      , _is_crypt(_is_crypt){
        // treeItem = nullptr;
        // workPos = -1;
        // init(i_dom_element);  // i_dom_element
        //// Создание таблицы
        // if(!i_dom_element.isNull()) {
        ////        QDomElement *dom_element = &i_dom_element;
        // import_from_dom(
        // i_dom_element  // dom_element
        // );
        // }
    if(_is_crypt){
        this->to_encrypt();								// table_data->to_encrypt();
    }else{
        this->to_decrypt();								// table_data->to_decrypt();
    }
        // return;
}

#ifdef _with_record_table

ItemsFlat::ItemsFlat(const ItemsFlat &obj)
    : _is_crypt(obj._is_crypt)
      , _workpos(obj._workpos){
        // _child_items = obj._child_items;

    foreach(auto i, obj._child_items){
        _child_items.push_back(i);								// boost::intrusive_ptr<TreeItem>(new TreeItem(*i))
    }
}

ItemsFlat &ItemsFlat::operator=(const ItemsFlat &obj){
    if(&obj != this){
        foreach(auto i, obj._child_items){
            _child_items.push_back(i);												// boost::intrusive_ptr<TreeItem>(new TreeItem(*i))
        }

        _is_crypt = obj._is_crypt;
        _workpos = obj._workpos;
    }
    return *this;
}

#endif

// Деструктор
ItemsFlat::~ItemsFlat(){
    clear();
    return;
}

//// Получение значения указанного поля для указанного имени поля
//// Имя поля - все возможные имена полей, кроме text (такого поля теперь вообще нет, текст запрашивается как отдельное свойство)
// QString ItemsFlat::field(int pos, QString name) const
// {
//// Если индекс недопустимый
// if(pos < 0 || pos >= _child_items.size()) {
// QString i;
// i.setNum(pos);
// critical_error("ItemsFlat::field() : get unavailable record index " + i);
// }

// return _child_items.at(pos)->field(name);
// }

boost::intrusive_ptr<Linker> ItemsFlat::linker_direct(int pos) const {
    boost::intrusive_ptr<Linker> result(nullptr);
        //// Если индекс недопустимый, возвращается пустая запись
        // if(pos < 0 || pos >= count_direct())
        // return nullptr;
    if(pos >= 0 && pos < count_direct())result = _child_linkers.at(pos);
    return result;				// _child_items.at(pos);    // _child_items[pos];
}

QList<boost::intrusive_ptr<TreeItem> > ItemsFlat::items_direct(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_substitute_condition) const {
    QList<boost::intrusive_ptr<TreeItem> > results;
    for(auto il : _child_linkers){
        if(_substitute_condition(il)){
            results.push_back(il->host());											// return i;
        }
    }
    return results;				// -1;
}

QList<boost::intrusive_ptr<TreeItem> > ItemsFlat::items_direct(const QString &name) const {
    QList<boost::intrusive_ptr<TreeItem> > results;
    for(auto il : _child_linkers){				// for(int i = 0; i < count_direct(); i++) {
        if(il->host()->field<name_type>() == name){
            results.push_back(il->host());											// return i;
        }
    }
    return results;				// -1;
}

// boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(const QString &id)const
// {
// boost::intrusive_ptr<TreeItem> result;

// for(int i = 0; i < count_direct(); i++) {
// if(field(i, "id") == id) {
// result = child(i);
// }
// }

// return result;   // -1;
// }

// boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(int pos)const
// {
// return item_direct(pos);
// }

boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(int pos) const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
        //// Если индекс недопустимый, возвращается пустая запись
        // if(pos < 0 || pos >= count_direct())
        // return nullptr;
    if(pos >= 0 && pos < count_direct())result = _child_linkers.at(pos)->host();
    return result;				// _child_items.at(pos);    // _child_items[pos];
}

boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(const QUrl &url) const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    for(int i = 0; i < count_direct(); i ++){
        std::string difference = url_difference((item_direct(i)->field<url_type>()).toStdString(), url.toString().toStdString());
        if(difference.size() == 0 || difference == "/"){
            result = item_direct(i);
            break;
        }
    }
    return result;
}

// boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(const QUrl &url)const
// {
// boost::intrusive_ptr<TreeItem> record;

// for(auto &i : _child_items) {
// if(i->field("url") == url.toString()) {
//// assert(i->is_registered_to_browser());
// record = i;
// break;
// }
// }

// return record;
// }

// int ItemsFlat::find_list(boost::intrusive_ptr<TreeItem> item)const
// {
// int pos = -1;

// for(int i = 0 ; i < _child_items.size(); i++) {
// auto it = _child_items.value(i);

// if(it->field("id") == item->field("id")) {
// assert(it->is_registered_to_record_controller());
// pos = i;
// break;
// }
// }

// return pos;
// }

boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_substitute_condition) const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    for(auto i : _child_linkers){
        if(_substitute_condition(i)){								// if(i->field("id") == item->field("id")) {
                // assert(i->is_registered());
            result = i->host();
            break;
        }
    }
    return result;
}

// boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(boost::intrusive_ptr<TreeItem> item)const
// {
// boost::intrusive_ptr<TreeItem> result;

// for(auto i : _child_items) {
// if(i == item) {   // if(i->field("id") == item->field("id")) {
//// assert(i->is_registered());
// result = i;
// break;
// }
// }

// return result;
// }

// boost::intrusive_ptr<TreeItem> ItemsFlat::item_direct(const QString &id)const
// {
// boost::intrusive_ptr<TreeItem> result;

// for(auto i : _child_items) {
// if(i->id() == id) {   // if(i->field("id") == item->field("id")) {
//// assert(i->is_registered());
// result = i;
// break;
// }
// }

// return result;
// }

// int ItemsFlat::sibling_order(QString id)const
// {
// for(int i = 0; i < count_direct(); i++)
// if(field(i, "id") == id)
// return i;

// return -1;
// }

// int ItemsFlat::sibling_order(boost::intrusive_ptr<TreeItem> it)const
// {
// for(int i = 0; i < count_direct(); i++)
// if(item_direct(i) == it)
// return i;

// return -1;
// }

int ItemsFlat::sibling_order(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_equal) const {
    int r = - 1;
    for(int i = 0; i < _child_linkers.size(); i ++){
        if(_equal(_child_linkers.at(i))){
            r = i;
            break;
        }
    }
    return r;
}

// int ItemsFlat::index_direct(boost::intrusive_ptr<TreeItem> item)const
// {
// int in = 0;
// int result = -1;

// for(auto il : _child_linkers) {

// if(il->host() == item) {
//// assert(i->is_registered_to_browser());
// result = in;
// break;
// }

// in++;
// }

// return result;
// }

//// Установка значения указанного поля для указанного элемента
// void ItemsFlat::field(int pos, QString name, QString value)
// {
//// Если индекс недопустимый
// if(pos < 0 || pos >= _child_items.size()) {
// QString i;
// i.setNum(pos);
// critical_error("In RecordTableData::setField() unavailable record index " + i + " in table while field " + name + " try set to " + value);
// }

// _child_items[pos]->field(name, value);
// }

// Получение значения текста указанной записи
// Метод возвращает расшифрованные данные
// Если возникнет проблема, что файла с текстом записи нет, будет создан пустой файл
// Get the value of the text of the specified record
// Method returns the decrypted data
// If there is a problem that file with text entries, will create an empty file
QString ItemsFlat::text(int pos) const {
        // Если индекс недопустимый, возвращается пустая строка
    if(pos < 0 || pos >= count_direct())return QString();
    if(_child_linkers[pos]->host()->is_lite())return _child_linkers[pos]->host()->text_from_lite_direct();
    else return _child_linkers[pos]->host()->text_from_fat();
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
void ItemsFlat::editor_load_callback(QObject *editor, QString &loadText){
        // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

        // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

        // Нужно ли дешифровать данные при чтении
    bool workWithCrypt = false;
    if(currEditor->misc_field(boost::mpl::c_str < crypt_type >::value) == "1"){
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0){
            loadText = "";
            return;
        }
        workWithCrypt = true;
    }
        // Файл, с которым работает редактор
    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();

    QFile f(fileName);
        // Если нужный файл не существует
    if(! f.exists())critical_error("File " + fileName + " not found");
        // Открывается файл
    if(! f.open(QIODevice::ReadOnly))critical_error("File " + fileName + " not readable. Check permission.");
        // Если незашифровано
    if(workWithCrypt == false)loadText = QString::fromUtf8(f.readAll());
    else loadText = CryptService::decryptStringFromByteArray(globalparameters.crypt_key(), f.readAll());							// Если зашифровано
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
void ItemsFlat::editor_save_callback(QObject *editor, QString saveText){
        // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

        // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

        // Нужно ли шифровать записываемый текст
    bool workWithCrypt = false;
    if(currEditor->misc_field(boost::mpl::c_str < crypt_type >::value) == "1"){
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0)return;
        workWithCrypt = true;
    }
    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();
        // Если шифровать ненужно
    if(workWithCrypt == false){
        // Текст сохраняется в файл
        QFile wfile(fileName);
        if(! wfile.open(QIODevice::WriteOnly | QIODevice::Text))critical_error("ItemsFlat::editor_save_callback() : Can\'t open text file " + fileName + " for write.");
        QTextStream out(&wfile);
        out.setCodec("UTF-8");
        out << saveText;
    }else{
        // Текст шифруется
        QByteArray encryptData = CryptService::encryptStringToByteArray(globalparameters.crypt_key(), saveText);

        // В файл сохраняются зашифрованные данные
        QFile wfile(fileName);
        if(! wfile.open(QIODevice::WriteOnly))critical_error("ItemsFlat::editor_save_callback() : Can\'t open binary file " + fileName + " for write.");
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
boost::intrusive_ptr<TreeItem> ItemsFlat::item_lite(int pos) const {
        // Если индекс недопустимый, возвращается пустая запись
    if(pos < 0 || pos >= count_direct())return boost::intrusive_ptr<TreeItem>(nullptr);
        // Хранимая в дереве запись не может быть "тяжелой"
    if(! _child_linkers.at(pos)->host()->is_lite())critical_error("In ItemsFlat::item_lite() try get fat record");
    return _child_linkers.at(pos)->host();
}

// Получение копии полного образа записи
// Возвращается запись с "сырыми" данными. Если запись была зашифрована, метод вернет зашифрованные данные
// Get a copy of the full image recording
// Returns the record with "raw" data. If the record was encrypted, the method returns the encrypted data
boost::intrusive_ptr<TreeItem> ItemsFlat::item_fat(int pos){
        // Копия записи из дерева
    boost::intrusive_ptr<TreeItem> result = item_direct(pos);					// boost::intrusive_ptr<Record> resultRecord = getRecordLite(pos);
        // original
        // Переключение копии записи на режим с хранением полного содержимого
    if(result->is_lite())result->to_fat();
        // Добавление текста записи
    result->text_to_fat(text(pos));

        // Добавление бинарных образов файлов картинок
    QString directory = appconfig.tetra_dir() + "/base/" + result->field<dir_type>();
    result->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));

    return result;
}

// void RecordTable::tree_item(boost::intrusive_ptr<TreeItem> item)
// {
//// Store a reference to the thread that owns the table  // Запоминается ссылка на ветку, которой принадлежит данная таблица
// _tree_item = item;
// init(item->export_to_dom());
// }

//// Initialize the data table based on the transmitted item DOM- // Инициализация таблицы данных на основе переданного DOM-элемента
// void RecordTable::init(// boost::intrusive_ptr<TreeItem> item,
// QDomElement i_dom_element)
// {
//// Создание таблицы
// if(!i_dom_element.isNull()) {
// QDomElement *dom_element = &i_dom_element;
// import(dom_element);
// }

//// Store a reference to the thread that owns the table  // Запоминается ссылка на ветку, которой принадлежит данная таблица
////    _tree_item = item;
// }

// Преобразование таблицы конечных записей в Dom документ
QDomElement ItemsFlat::dom_from_activated_itemsflat() const {
        // Если у ветки нет таблицы конечных записей, возвращается пустой документ
    if(_child_linkers.size() == 0)return QDomElement();
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    QDomElement record_dom_data = doc->createElement("recordtable");
        // Пробегаются все записи в таблице
    for(int i = 0; i < _child_linkers.size(); i ++){
        if(  _child_linkers.at(i)->host()->page_valid()								// unique_page()
          && static_cast<ItemsFlat *const>(_child_linkers.at(i)->host().get()) != this){
            record_dom_data.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem());											// К элементу recordtabledata прикрепляются конечные записи
        }
    }
        // qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

    return record_dom_data;
}

// азбор DOM модели и преобразование ее в таблицу
void ItemsFlat::dom_to_itemsflat(const QDomElement &dom_model){	// , boost::intrusive_ptr<TreeItem> _parent_item
        // assert(static_cast<ItemsFlat *>(_parent_item.get()) == this);
        // QDomElement n = dommodel.documentElement();
        // QDomElement n = dommodel;
        // qDebug() << "In recordtabledata setup_data_from_dom() start";
        // Если принятый элемент не является таблицей
    if(dom_model.tagName() != "recordtable"){
        return;
    }
        // Определяется указатель на первый элемент с записью
        // Define a pointer to the first element of the recording
    QDomElement dom_record = dom_model.firstChildElement("record");
    while(! dom_record.isNull()){

        QDomNamedNodeMap attribute_map = dom_record.attributes();
        QString id = attribute_map.namedItem("id").nodeValue();

        assert(id != "");

        int index = this->sibling_order([&](boost::intrusive_ptr<const Linker> it){
                    return it->host()->id() == id;
                });								// ->sibling_order();
        QMap<QString, QString> data;
        data["id"] = id;

        // KnowModel::dom_to_records::assembly_record_and_table_to_parent(dom_model, _parent_item);

        boost::intrusive_ptr<TreeItem> child_item(nullptr);
        if(index != - 1){
            child_item = this->item_direct(index);
        }else{
            child_item = new TreeItem(dynamic_cast<TreeItem *const>(this), data, QDomElement(), this->count_direct());												// static_cast<TreeItem *const>(this)->child_add_new(this->count_direct(), id, "");
                // child_item->field("id", id);
        }
        assert(child_item);
        //// Структура, куда будет помещена текущая запись
        //// The structure, which will put the current record
        // boost::intrusive_ptr<TreeItem> child_item = boost::intrusive_ptr<TreeItem>(new TreeItem(_parent_item));

        child_item->dom_to_records(dom_record);
        // current_item->is_registered_to_record_controller_and_tabmanager(true);

        // Текущая запись добавляется в таблицу конечных записей (и располагается по определенному адресу в памяти)
        // The current record is added to the final table of records (and located at a certain address in memory)

        assert(contains_direct(std::forward<const boost::intrusive_ptr<Linker> >(child_item->linker())));								// _child_items << child_item;

        // Запись инициализируется данными. Она должна инициализироватся после размещения в списке tableData,
        // чтобы в подчиненных объектах прописались правильные указатели на данную запись
        // Write initialized data. It should initsializirovatsya after placement in the list tableData,
        // Order in subordinate objects have registered a valid pointer to this entry

        // static_cast<Record *>(_child_items.last().get())->record_from_dom(current_record_dom);

        dom_record = dom_record.nextSiblingElement("record");
    }					// Close the loop iterate tag <record ...>    // Закрылся цикл перебора тегов <record ...>

    return;
}

QDomElement ItemsFlat::dom_from_itemsflat() const {
        //// Преобразование таблицы конечных записей в Dom документ
        // auto dom_from_itemsflat = [&](std::shared_ptr<QDomDocument> doc) ->QDomElement const {
        //// Если у ветки нет таблицы конечных записей, возвращается пустой документ
        // if(_child_linkers.size() == 0)
        // return QDomElement();

        // QDomElement item_flat_dom = doc->createElement("recordtable");

        //// Пробегаются все записи в таблице
        // for(int i = 0; i < _child_linkers.size(); i++) {

        // if(static_cast<ItemsFlat *>(_child_linkers.at(i)->host().get()) != this) {

        // item_flat_dom.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem());                 // boost::static_pointer_cast<Record>(_child_items.at(i))->dom_from_record(doc)
        //// К элементу recordtabledata прикрепляются конечные записи
        // }
        // }

        //// qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

        // return item_flat_dom;
        // };
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    return dom_from_itemsflat_impl(doc);
}

//// Преобразование таблицы конечных записей в Dom документ
// QDomElement ItemsFlat::dom_from_itemsflat(std::shared_ptr<QDomDocument> doc) const
// {
//// Если у ветки нет таблицы конечных записей, возвращается пустой документ
// if(_child_linkers.size() == 0)
// return QDomElement();

// QDomElement item_flat_dom = doc->createElement("recordtable");

//// Пробегаются все записи в таблице
// for(int i = 0; i < _child_linkers.size(); i++) {

// if(static_cast<ItemsFlat *>(_child_linkers.at(i)->host().get()) != this) {

// item_flat_dom.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem(doc));                 // boost::static_pointer_cast<Record>(_child_items.at(i))->dom_from_record(doc)
//// К элементу recordtabledata прикрепляются конечные записи
// }
// }

//// qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

// return item_flat_dom;
// }

//// Преобразование таблицы конечных записей в Dom документ
// QDomElement TableData::export_to_dom(QDomDocument *doc) const
// {
//// Если у ветки нет таблицы конечных записей, возвращается пустой документ
// if(_tabledata.size() == 0)
// return QDomElement();

// QDomElement record_dom_data = doc->createElement("recordtable");

//// Пробегаются все записи в таблице
// for(int i = 0; i < _tabledata.size(); i++)
// record_dom_data.appendChild(_tabledata.at(i)->export_to_dom(doc));     // К элементу recordtabledata прикрепляются конечные записи

//// qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

// return record_dom_data;
// }

//// Преобразование таблицы конечных записей в Dom документ
// QDomElement ItemsFlat::dom_from_activated_itemsflat() const
// {
//// Если у ветки нет таблицы конечных записей, возвращается пустой документ
// if(_child_linkers.size() == 0)
// return QDomElement();

// std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
// QDomElement record_dom_data = doc->createElement("recordtable");

//// Пробегаются все записи в таблице
// for(int i = 0; i < _child_linkers.size(); i++) {
// if(_child_linkers.at(i)->host()->page_valid() //unique_page()
// && (static_cast<ItemsFlat *>(_child_linkers.at(i)->host().get()) != this)
// ) {
// record_dom_data.appendChild(_child_linkers.at(i)->host()->dom_from_treeitem());    // К элементу recordtabledata прикрепляются конечные записи
// }
// }

//// qDebug() << "In export_modeldata_to_dom() recordtabledata " << doc.toString();

// return record_dom_data;
// }

// Замена в указанной записи переданных полей на новые значения
void ItemsFlat::fields(int pos, QMap<QString, QString> edit_fields){
    qDebug() << "In RecordTable method edit_record_fields()";

    QMapIterator<QString, QString> i(edit_fields);
    while(i.hasNext()){
        i.next();
        item_direct(pos)->_field_data[i.key()] = i.value();	// field(pos, i.key(), i.value());
    }
        // changePersistentIndex(QModelIndex(), QModelIndex());
}

//// Удаление записи с указанным индексом
//// todo: добавить удаление приаттаченных файлов и очистку таблицы приаттаченных файлов
// bool ItemsFlat::remove(int i)
// {
// bool result = false;
// qDebug() << "Try delete record num " << i << " table count " << _child_items.size();

//// Нельзя удалять с недопустимым индексом
// if(i >= _child_items.size())
// return result;

//// Удаление директории и файлов внутри, с сохранением в резервной директории
// QString dirForDelete = appconfig.tetra_dir() + "/base/" + field(i, "dir");
// qDebug() << "Remove dir " << dirForDelete;
// DiskHelper::remove_directory_to_trash(dirForDelete);

//// Удаление позиции курсора из истории
// QString id = field(i, "id");

// if(id.length() > 0)
// walkhistory.remove_history_data(id);

////    //
////    Record record = tableData.at(i);
////    browser::PageView *view = record.view();
////    //

//// Начинается удаление записи
//// beginRemoveRows(QModelIndex(),i,i);

//// Удаляется элемент
// _child_items.removeAt(i); // Было takeAt

// result = true;
// qDebug() << "Delete record succesfull";

////    //
////    browser::TabManager *tabmanager = view->tabmanager();
////    tabmanager->closeTab(tabmanager->webViewIndex(view));
////    //

//// Удаление записи закончено
//// endRemoveRows();
// return result;
// }

// bool ItemsFlat::remove(QString id)
// {
// bool result = false;

// for(int i = 0; i < count_direct(); i++) {
// if(field(i, "id") == id) {
// result = remove(i); // Так как id уникальный, удаляться будет только одна запись
// break;
// }
// }

// return result;
// }

boost::intrusive_ptr<TreeItem> ItemsFlat::contains_direct(const boost::intrusive_ptr<TreeItem> &&_item) const {

    boost::intrusive_ptr<TreeItem> result(nullptr);
    if(_item){
        for(auto it : _child_linkers){
            if(  it
              && it->host() == _item											// && _item->up_linker()->integrity_external(_item, static_cast<const TreeItem *>(this))   // recursive call
              && it == _item->linker()
              && static_cast<ItemsFlat *>(_item->_linker->host_parent().get()) == this){
                // assert(_item->up_linker()->integrity_external(_item, static_cast<const TreeItem *>(this)));
                result = it->host();
                break;
            }
        }
    }
    return result;
}

boost::intrusive_ptr<TreeItem> ItemsFlat::contains_direct(const boost::intrusive_ptr<const Linker> &&_item_linker) const {
    boost::intrusive_ptr<TreeItem> result(nullptr);
    if(_item_linker){
        for(auto it : _child_linkers){
            if(  it
              && it == _item_linker											// && _item_linker->integrity_external(_item_linker->host(), static_cast<const TreeItem *>(this))  // recursive call
              && static_cast<ItemsFlat *>(_item_linker->host_parent().get()) == this											// _item_linker->host_parent() == this
                ){
                // assert(_item_linker->integrity_external(_item_linker->host(), static_cast<const TreeItem *>(this)));
                result = it->host();
                break;
            }
        }
    }
    return result;
}

boost::intrusive_ptr<TreeItem> ItemsFlat::delete_permanent(const std::function<bool (boost::intrusive_ptr<const Linker>)> &_equal){
    boost::intrusive_ptr<TreeItem> result;
        // QMutableListIterator<boost::intrusive_ptr<Linker> > it(_child_linkers);
// std::set<boost::intrusive_ptr<Linker>> results;
    for(auto &il : _child_linkers){				// while(it.hasNext()) {
        // auto _it = it.next();
        auto it = il->host();
        if(_equal(il)){
                // Удаление директории и файлов внутри, с сохранением в резервной директории
            QString dir_for_delete = appconfig.tetra_dir() + "/base/" + it->field<dir_type>()												// field(i, "dir")
            ;
            qDebug() << "Remove dir " << dir_for_delete;

            DiskHelper::remove_directory_to_trash(dir_for_delete);

                // Удаление позиции курсора из истории
            QString id = it->id();
            if(id.length() > 0)walkhistory.remove_history_data(id);
                // _it->break_linker();    // recursive calling removeOne!!!
                // result |=
            _child_linkers.removeOne(il);
//	    assert(result);
//	    if(it->binder()){
//		auto page = it->binder()->page();
//		auto tab_manager = page->tabmanager();
//		tab_manager->closeTab(tab_manager->webViewIndex(page->view()));
////		it->binder()->page()->on_close_requested();			// it->binder()->break_page();  // it->page_break();
//	    }
            browser::WebView *web_view = nullptr;
            if((web_view = globalparameters.entrance()->find([&](boost::intrusive_ptr<const ::Binder> b){return url_equal((b->host()->field<home_type>()).toStdString(), it->field<url_type>().toStdString()) && b == it->binder() && b->host()->id() == it->id();}))){
                if(it->binder()){
                    assert(web_view->page() == it->page());
                    it->page()->record_controller()->remove(it->id());
                }
            }
                // if(_it->integrity_internal())


            result = it;
//	    if(results.find(il) == results.end()) results.insert(il);
            if(il){
//		il->host(boost::intrusive_ptr<TreeItem>(nullptr));
//		il->host_parent(boost::intrusive_ptr<TreeItem>(nullptr));
//		il.reset();
            }
            break;
        }
    }
// for(auto &il : results) {
// if (il) {
// il->host(boost::intrusive_ptr<TreeItem>(nullptr));             // il.reset();
// il->host_parent(boost::intrusive_ptr<TreeItem>(nullptr));
// il.reset();
// }

// }


    return result;
}

void ItemsFlat::remove_duplicated(){
    for(auto _item : _child_linkers){					// QList<boost::intrusive_ptr<TreeItem>>::iterator
        int found = 0;
        boost::intrusive_ptr<Linker> keep;
        for(auto il : _child_linkers){								// QList<boost::intrusive_ptr<TreeItem>>::iterator
            if(il->host().get() == _item->host().get()){
                found ++;
                if(1 == found){
                    keep = il;
                }else if(found > 1){
                        // result = *it;
                        // else
                    _child_linkers.removeOne(il);
                    keep->host()->merge(il->host());
                    il->host()->delete_permanent_recursive([&](boost::intrusive_ptr<const TreeItem> it){
                            return it->is_empty();
                        });
                    if(il){
// il->host(boost::intrusive_ptr<TreeItem>(nullptr));
// il->host_parent(boost::intrusive_ptr<TreeItem>(nullptr));
// il.reset();
                    }
                }
            }
        }
    }
}

QList<boost::intrusive_ptr<TreeItem> > ItemsFlat::delete_permanent_recursive(int position, int count){
        // bool result = false;
        // QList<boost::intrusive_ptr<Linker>> candidates;
    QList<boost::intrusive_ptr<TreeItem> > result;
        // if(position < 0 || position + count > _child_items.size())
        // return false;
        // if(position >= 0 && position + count <= _child_linkers.size()) {
        // QMutableListIterator<boost::intrusive_ptr<Linker>> it(_child_linkers);
        // int del_count = 0;
        // while(it.hasNext() && del_count < position + count) {
        // if(del_count >= position) {
        // result << it.next();
        // it.remove();
        // } else {
        // it.next();
        // } // _child_linkers.removeOne(*it.next());
        // del_count++;
        // }
        // }
    if(position >= 0 && position + count <= _child_linkers.size()){
        int del_count = 0;
        for(auto il : _child_linkers){
            auto it = il->host();
            if(del_count >= position && del_count < position + count){

                _child_linkers.removeOne(il);

                result << it;

                delete_permanent_recursive(il, [&](boost::intrusive_ptr<const TreeItem> ){
                        return true;
                    });
            }
// il.reset();
            if(del_count == (position + count))break;
            del_count ++;
        }
    }
        // if(position >= 0 && position + count <= _child_linkers.size()) {
        // for(int row = 0; row < count; ++row) {
        // auto il = _child_linkers.at(position);

        // if(il) {

        ////                result = il->host();
        ////                result->page_break();
        ////                il.reset();
        // candidates << il;
        // }

        ////            _child_linkers.removeOne(il);   //removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
        // }

        ////        result = true;
        // }

        // for(auto it : candidates) {
        // result << remove(it);
        // }

    return result;				// true;
}

boost::intrusive_ptr<TreeItem> ItemsFlat::delete_permanent_recursive(boost::intrusive_ptr<Linker> remove_linker, std::function<bool (boost::intrusive_ptr<const TreeItem>)> condition){
    boost::intrusive_ptr<TreeItem> result(nullptr);				// bool result = false;
        // result = remove(item->id());
        // if(_to_be_removed->parent_item() != this)_to_be_removed->self_remove_from_parent();
    if(remove_linker->host_parent().get() == dynamic_cast<TreeItem *const>(this) && condition(dynamic_cast<TreeItem *const>(this))){
        for(auto il : _child_linkers){								// for(int row = 0; row < _child_items.size(); ++row) {
                // auto it = _child_items.at(row);

            auto it = il->host();
            if(it->id() == remove_linker->host()->id()){											// _child_items.removeAt(position);    // _child_items.takeAt(position).reset(); // delete _child_items.takeAt(position);
                // auto it = it_linker->host();
                if(it && it->count_direct() > 0){
                    for(auto &link : it->_child_linkers){
                        it->delete_permanent_recursive(link, condition);
                    }
                }
                // Удаление директории и файлов внутри, с сохранением в резервной директории
                QString dir_for_delete = appconfig.tetra_dir() + "/base/" + it->field<dir_type>()															// field(i, "dir")
                ;
                qDebug() << "Remove dir " << dir_for_delete;

                DiskHelper::remove_directory_to_trash(dir_for_delete);

                // Удаление позиции курсора из истории
                QString id = it->id();															// field(i, "id");
                if(id.length() > 0)walkhistory.remove_history_data(id);
                ////
                // Record record = tableData.at(i);
                // browser::PageView *view = record.view();
                ////

                // Начинается удаление записи
                // beginRemoveRows(QModelIndex(),i,i);

                // Удаляется элемент
                _child_linkers.removeOne(il);															// removeAt(i); // Было takeAt
                if(it->binder()){
                    auto page = it->binder()->page();
                    auto tab_manager = page->tabmanager();
                    tab_manager->closeTab(tab_manager->webViewIndex(page->view()));
                        // it->binder()->page()->on_close_requested(); // it->binder()->break_page();  // it->page_break();
                }
                ////                //    //
                ////                //    browser::TabManager *tabmanager = view->tabmanager();
                ////                //    tabmanager->closeTab(tabmanager->webViewIndex(view));
                ////                //    //

                ////                // Удаление записи закончено
                ////                // endRemoveRows();

                ////                if(it != _to_be_removed_linker->host()) {
                ////                    result = _to_be_removed_linker->host()->merge(it)->up_linker();
                ////                } else {
                ////                    result = _to_be_removed_linker;
                ////                    //                    _to_be_removed_linker->host()->remove_and_delete();
                ////                }



                result = it;
                if(il){
// il->host(boost::intrusive_ptr<TreeItem>(nullptr));
// il->host_parent(boost::intrusive_ptr<TreeItem>(nullptr));
// il.reset();
                }
                break;
                qDebug() << "Delete record succesfull";
            }
        }
    }
        // self_remove_from_parent_as_empty();


    return result;

        // qDebug() << "Try delete record num " << _child_items.indexOf(item) << " table count " << _child_items.size();

        ////    // Нельзя удалять с недопустимым индексом
        ////    if(i >= _child_items.size())
        ////        return result;
        // auto target = contains(item);

        // if(target) {
        //// Удаление директории и файлов внутри, с сохранением в резервной директории
        // QString dirForDelete = appconfig.tetra_dir() + "/base/" +
        // target->field("dir") // field(i, "dir")
        // ;
        // qDebug() << "Remove dir " << dirForDelete;
        // DiskHelper::remove_directory_to_trash(dirForDelete);

        //// Удаление позиции курсора из истории
        // QString id = target->id();  //field(i, "id");

        // if(id.length() > 0)
        // walkhistory.remove_history_data(id);

        ////    //
        ////    Record record = tableData.at(i);
        ////    browser::PageView *view = record.view();
        ////    //

        //// Начинается удаление записи
        //// beginRemoveRows(QModelIndex(),i,i);

        //// Удаляется элемент
        // _child_items.removeOne(target); //removeAt(i); // Было takeAt
        // target->record_linker().reset();
        // target->page_break();
        // result = target;
        // qDebug() << "Delete record succesfull";

        ////    //
        ////    browser::TabManager *tabmanager = view->tabmanager();
        ////    tabmanager->closeTab(tabmanager->webViewIndex(view));
        ////    //

        //// Удаление записи закончено
        //// endRemoveRows();
        // }

        // return result;
}

//// Удаление всех элементов таблицы конечных записей
// void ItemsFlat::delete_list_items(void)
// {
// int tableSize = count_direct(); // Запоминается размер таблицы, так как он при удалении меняется

// for(int i = 0; i < tableSize; i++)
// remove(0);   // Deleted very first record many times   // Удаляется самая первая запись много раз
// }

// Метод мягкого удаления данных
// Данные очищаются только у объекта
// а физически данные на диске не затрагиваются
// Method of soft data deletion
// These are cleared only object
// And physical data on the disk are not affected
void ItemsFlat::clear(void){
        ////    _child_linkers.clear();
        ////    _tree_item = nullptr;

        ////    for(int i = 0; i < _child_items.size(); i++) {
        ////        auto _result = _child_items.at(i);
        ////        _result->page_break();
        ////        _result->record_linker().reset();
        ////        //        _child_items[i]->parent(nullptr);
        ////        //        _child_items.removeAt(i);    // _child_items.takeAt(i).reset(); // delete _child_items.takeAt(i);
        ////    }

        // for(auto it : _child_linkers) {
        // delete_permanent_recursive(it);
        // }

    _child_linkers.clear();
    assert(_child_linkers.size() == 0);
}

// boost::intrusive_ptr<TreeItem> RecordTable::active_subset(
////    boost::intrusive_ptr<TreeItem> start_item
// )
// {
////    std::shared_ptr<TableData> result = std::make_shared<TableData>();

////    for(auto &i : _tabledata) {
////        if(i->unique_page())result->insert_new_record(work_pos(), i);
////    }

//// bypass slite fat switch:

////    auto start_item = _treeitem;   // std::make_shared<TreeItem>(data, search_model->_root_item);
// std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
// auto dommodel = this->export_activated_dom(doc);    // source->init(startItem, QDomElement());
// QMap<QString, QString> data;
// boost::intrusive_ptr<TreeItem> tree = new TreeItem(
// data
// , boost::intrusive_ptr<TreeItem>(this)   // _parent_item
// );
// tree->import_from_dom(dommodel);

// return  tree;   // new TreeItem(data, _parent_item);

////    return result;
// }

// Количество записей в таблице данных
int ItemsFlat::count_direct(void) const {
    return _child_linkers.size();
}

// Перемещение записи вверх на одну строку
void ItemsFlat::move_up(int pos){
    if(pos > 0 && pos < _child_linkers.count()){
        // Данные перемещаются
        _child_linkers.move(pos, pos - 1);

        // Обновляется экран
        // QModelIndex from=index(pos-1);
        // QModelIndex to=index(pos);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}

// Перемещение записи вниз на одну строку
// Move write down one line
void ItemsFlat::move_dn(int pos){
    if(pos >= 0 && pos < _child_linkers.count() - 1){
        // Данные перемещаются
        _child_linkers.move(pos, pos + 1);

        // Обновляется экран
        // QModelIndex from=index(pos);
        // QModelIndex to=index(pos+1);
        // emit dataChanged(from,to); // Посылается сигнал что данные были изменены
    }
}

// Переключение таблицы в зашифрованное состояние
// todo: Добавить шифрацию имени приаттаченных файлов и содержимого файлов
void ItemsFlat::to_encrypt(void){
        // Перебор записей
    for(int i = 0; i < count_direct(); i ++){
        // Если запись уже зашифрована, ее шифровать ненужно
        if(_child_linkers[i] && _child_linkers[i]->host()->field<crypt_type>() == "1")continue;
        // Шифрация записи
        _child_linkers[i]->host()->to_encrypt_and_save_lite();								// В таблице конечных записей хранятся легкие записи
    }
    this->_is_crypt = true;
}

// Переключение таблицы в расшифрованное состояние
// todo: добавить расшифрацию имени приаттаченных файлов и содержимого файлов
void ItemsFlat::to_decrypt(void){
        // Перебор записей
    for(int i = 0; i < count_direct(); i ++){
        // Если запись не зашифрована, ее не нужно расшифровывать
        if(_child_linkers[i] && (_child_linkers[i]->host()->field<crypt_type>() == "" || _child_linkers[i]->host()->field<crypt_type>() == "0"))continue;
        // асшифровка записи
        _child_linkers[i]->host()->to_decrypt_and_save_lite();								// В таблице конечных записей хранятся легкие записи
    }
    this->_is_crypt = false;
}

//// Получение ссылки на объект ветки, которой принадлежит таблица
// boost::intrusive_ptr<TreeItem> RecordTable::tree_item(void)
// {
// return _tree_item;
// }

int ItemsFlat::work_pos(void) const {
    return _workpos;
}

void ItemsFlat::work_pos(int pos){
    _workpos = pos;
}

void ItemsFlat::crypt(const bool _is_crypt){
    if(_is_crypt && ! this->_is_crypt){
        for(int i = 0; i < count_direct(); i ++){
            item_direct(i)->to_encrypt_fields();
        }
    }
    if(! _is_crypt && this->_is_crypt){
        for(int i = 0; i < count_direct(); i ++){
            item_direct(i)->to_decrypt_fields();
        }
    }
    if(_is_crypt != this->_is_crypt)this->_is_crypt = _is_crypt;
}

bool ItemsFlat::is_empty() const {
    return 0 == _child_linkers.size();
}

QList<boost::intrusive_ptr<Linker> > ItemsFlat::child_linkers(){
    return _child_linkers;
}

void ItemsFlat::traverse(const std::function<void (boost::intrusive_ptr<Linker>)> &operation){
    for(auto il : ItemsFlat::_child_linkers){
        operation(il);
    }
}
