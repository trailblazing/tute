#include <QAbstractItemModel>
#include <QMap>
#include <QDomNamedNodeMap>

#include "main.h"
#include "KnowModel.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "XmlTree.h"

#include "libraries/ClipboardRecords.h"
#include "libraries/ClipboardBranch.h"
#include "models/app_config/AppConfig.h"
#include "views/tree/TreeScreen.h"
#include "libraries/crypt/Password.h"
#include "libraries/DiskHelper.h"
#include "views/browser/browser.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/KnowView.h"



extern GlobalParameters globalparameters;

extern AppConfig appconfig;
const char *global_root_id = "0";

// Конструктор модели дерева, состоящего из Item элементов
KnowModel::KnowModel(QObject *parent)
    : TreeModel(parent)
      //    , _root_item(nullptr)
{
    //    xmlFileName = "";
    //    rootItem = nullptr;

    QMap<QString, QString> root_data;

    // Определяется одно поле в корневом объекте
    // то есть на экране будет один столбец
    root_data["id"] = global_root_id;
    root_data["name"] = "";

    // Создание корневого Item объекта
    if(_root_item) _root_item.reset();

    _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, root_data)); // ?
}


KnowModel::KnowModel(boost::intrusive_ptr<TreeItem> _root_item, QObject *parent)
    : TreeModel(_root_item, parent)
{}

// Деструктор Item модели.
// По-хорошему деструктор перед удалением корневого элемента должен пробежать по
// дереву элементов и удалить их
KnowModel::~KnowModel()
{
    //    delete rootItem;
}


std::shared_ptr<XmlTree> KnowModel::init_from_xml(QString file_name)
{
    _xml_file_name = file_name;

    // Загрузка файла и преобразование его в DOM модель
    std::shared_ptr<XmlTree> xmlt = std::make_shared<XmlTree>();

    if(!xmlt->load(_xml_file_name))
        return xmlt;

    init(xmlt->dom_model());
    return xmlt;
}

std::shared_ptr<XmlTree> KnowModel::init_from_xml(std::shared_ptr<XmlTree> xmlt)
{
    init(xmlt->dom_model());
    return xmlt;
}

void KnowModel::init(QDomDocument *dom_model)
{
    // Проверка формата XML-файла
    if(!format_check(dom_model->documentElement().firstChildElement("format"))) {
        critical_error(tr("Unsupported format version for data base.\nPlease upgrade your MyTetra application."));
        return;
    }

    assert(_root_item);

    //    QMap<QString, QString> root_data;

    //    // Определяется одно поле в корневом объекте
    //    // то есть на экране будет один столбец
    //    root_data["id"] = global_root_id;
    //    root_data["name"] = "";

    beginResetModel();

    //    // Создание корневого Item объекта
    //    if(_root_item) _root_item.reset();

    //    _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(root_data, nullptr)); // ?
    //    // , std::make_shared<RecordTable>(QDomElement())

    //    // Динамическое создание дерева из Item объектов на основе DOM модели
    //    setup_modeldata(dom_model, _root_item);
    assert(dom_model);
    QDomElement content_root_record = dom_model->documentElement().firstChildElement("content").firstChildElement("record");   // "node"


    if(content_root_record.isNull()) {
        qDebug() << "Unable load xml tree, first content node not found.";
        return;
    }

    dom_to_records(content_root_record, _root_item);

    endResetModel();
    //    save(); // temporary
}



//// Разбор DOM модели и преобразование ее в Item модель
//void TreeKnowModel::setup_modeldata(QDomDocument *dommodel, boost::intrusive_ptr<TreeItem> self)
//{
//    assert(dommodel);
//    QDomElement content_root_record = dommodel->documentElement().firstChildElement("content").firstChildElement("record");   // "node"


//    if(content_root_record.isNull()) {
//        qDebug() << "Unable load xml tree, first content node not found.";
//        return;
//    }

//    dom_to_records(content_root_record, self);

//    return;
//}


// Рекурсивный обход DOM дерева и извлечение из него узлов
void KnowModel::dom_to_records(QDomElement _record_dom_element, boost::intrusive_ptr<TreeItem> self)
{

    std::function<void(const QDomElement &, boost::intrusive_ptr<TreeItem>)>
    assembly_record_and_table_to_parent = [&](const QDomElement & _dom_record, boost::intrusive_ptr<TreeItem> current_parent) {


        // Определяются атрибуты узла дерева разделов
        QDomNamedNodeMap attribute_map = _dom_record.attributes();

        //        // Перебираются атрибуты узла дерева разделов
        //        for(int i = 0; i < attribute_map.count(); ++i) {
        //            QDomNode attribute = attribute_map.item(i);

        //            QString name = attribute.nodeName();
        //            QString value = attribute.nodeValue();

        //            if(name == "id") {
        //                id = value;
        //                break;
        //            }

        //            //                        // В дерево разделов устанавливаются считанные атрибуты
        //            //                        // parent->child(parent->current_count() - 1)
        //            //                        parent->field(name , value);
        //            //                        //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
        //        }

        QString id = attribute_map.namedItem("id").nodeValue();

        assert(id != "");

        int index = current_parent->is_id_exists(id);

        boost::intrusive_ptr<TreeItem> item = nullptr;

        if(index != -1) {
            item = current_parent->child(index);
        } else {
            item = current_parent->child_add_new(current_parent->count_direct());
        }

        dom_to_records(_dom_record, item);   // item->self_equipment_from_dom(record);

        //        QDomElement recordtable = record.firstChildElement();
        //        assert(recordtable.tagName() == "recordtable" || recordtable.isNull());


        //        if(recordtable.tagName() == "recordtable") {
        //            //                    auto record = recordtable.firstChildElement();
        //            //                    assert(record.tagName() == "record" || record.isNull());
        //            //                    if(record.tagName() == "record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
        //            model_from_dom(recordtable, item);
        //        }
    };
    //    boost::intrusive_ptr<TreeItem> parent = iParent;

    if(!_record_dom_element.isNull()) {
        assert(_record_dom_element.tagName() != "recordtable");
        assert(_record_dom_element.tagName() == "record");
        // У данного Dom-элемента ищется таблица конечных записей
        // и данные заполняются в Item-таблицу конечных записей
        // At this Dom-end table element is searched for records and the data filled in the Item-end table entries
        self->dom_to_direct(_record_dom_element);    // take ground from the nearest level children

        QDomElement _child = _record_dom_element.firstChildElement();

        assert(_child.tagName() == "recordtable" || _child.isNull());

        if(_child.tagName() == "recordtable") {

            QDomElement _record_child = _child.firstChildElement();
            assert(_record_child.tagName() == "record");

            // Пробегаются все DOM элементы текущего уровня
            // и рекурсивно вызывается обработка подуровней
            while(!_record_child.isNull()) { // traverse brothers



                //            if(_dom_element.tagName() == "node") { // "node"
                //                assert(_dom_element.firstChildElement().tagName() == "record");
                //                QDomElement record = _dom_element.firstChildElement("record");

                //                if(!record.isNull()) {

                //                    //                    //                // Обнаруженый подузел прикрепляется к текущему элементу
                //                    //                    //                auto item = parent->add_child();  // insert_children(parent->current_count(), 1, 1);

                //                    //                    //                if(!_dom_element.firstChildElement("record").isNull())
                //                    //                    //                    boost::static_pointer_cast<Record>(item)->record_from_dom(_dom_element.firstChildElement("record"));

                //                    //                    //                //            QString line1, line_name, line_id;
                //                    //                    //                //            line1 = n.tagName();
                //                    //                    //                //            line_name = n.attribute("name");
                //                    //                    //                //            line_id = n.attribute("id");
                //                    //                    //                //            qDebug() << "Read node " << line1 << " " << line_id << " " << line_name;

                //                    //                    QString id = "";

                //                    //                    // Определяются атрибуты узла дерева разделов
                //                    //                    QDomNamedNodeMap attributeMap = record.attributes();

                //                    //                    // Перебираются атрибуты узла дерева разделов
                //                    //                    for(int i = 0; i < attributeMap.count(); ++i) {
                //                    //                        QDomNode attribute = attributeMap.item(i);

                //                    //                        QString name = attribute.nodeName();
                //                    //                        QString value = attribute.nodeValue();

                //                    //                        if(name == "id") {
                //                    //                            id = value;
                //                    //                            break;
                //                    //                        }

                //                    //                        //                    // В дерево разделов устанавливаются считанные атрибуты
                //                    //                        //                    // parent->child(parent->current_count() - 1)
                //                    //                        //                    item->field(name , value);
                //                    //                        //                    //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
                //                    //                    }

                //                    //                    assert(id != "");
                //                    //                    int index = parent->is_item_exists(id);

                //                    //                    boost::intrusive_ptr<TreeItem> item = nullptr;

                //                    //                    if(index != -1) {
                //                    //                        item = parent->child(index);

                //                    //                        item->record_from_dom(record);
                //                    //                    } else {
                //                    //                        item = parent->add_child();
                //                    //                        item->record_from_dom(record);
                //                    //                    }

                //                    //                    // Вызов перебора оставшегося DOM дерева с прикреплением обнаруженных объектов
                //                    //                    // к только что созданному элементу
                //                    //                    model_from_dom(record.firstChildElement(), item);    //
                //                    record_and_recordtable(record);
                //                }

                //            } else

                if(_record_child.tagName() == "record") {

                    //                if(!_dom_element.isNull()) {
                    //                    // boost::static_pointer_cast<Record>(parent)->record_from_dom(_dom_element);
                    //                    //                    boost::intrusive_ptr<TreeItem> item = boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), parent));
                    //                    //                    item->record_from_dom(_dom_element);
                    //                    //                    parent->find(item->id())->record_from_dom(_dom_element);

                    //                    QString id = "";
                    //                    // Определяются атрибуты узла дерева разделов
                    //                    QDomNamedNodeMap attributeMap = _dom_element.attributes();

                    //                    // Перебираются атрибуты узла дерева разделов
                    //                    for(int i = 0; i < attributeMap.count(); ++i) {
                    //                        QDomNode attribute = attributeMap.item(i);

                    //                        QString name = attribute.nodeName();
                    //                        QString value = attribute.nodeValue();

                    //                        if(name == "id") {
                    //                            id = value;
                    //                            break;
                    //                        }

                    //                        //                        // В дерево разделов устанавливаются считанные атрибуты
                    //                        //                        // parent->child(parent->current_count() - 1)
                    //                        //                        parent->field(name , value);
                    //                        //                        //                parent->child(parent->child_count() - 1)->record_to_item(); // temporary
                    //                    }

                    //                    assert(id != "");
                    //                    int index = parent->is_item_exists(id);
                    //                    boost::intrusive_ptr<TreeItem> item = nullptr;

                    //                    if(index != -1) {
                    //                        item = parent->child(index);
                    //                        item->record_from_dom(_dom_element);
                    //                    } else {
                    //                        item = parent->add_child();
                    //                        item->record_from_dom(_dom_element);
                    //                    }

                    //                    QDomElement recordtable = _dom_element.firstChildElement();
                    //                    assert(recordtable.tagName() == "recordtable" || recordtable.isNull());


                    //                    if(recordtable.tagName() == "recordtable") {
                    //                        //                    auto record = recordtable.firstChildElement();
                    //                        //                    assert(record.tagName() == "record" || record.isNull());

                    //                        //                    if(record.tagName() == "record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
                    //                        model_from_dom(recordtable, item);
                    //                    }

                    // model_from_dom(_child, self); //
                    assembly_record_and_table_to_parent(_record_child, self);
                    //            }

                }

                //                else {

                //                    assert(_child.tagName() == "recordtable");

                //                    //                if(!_dom_element.isNull())static_cast<ItemsFlat *>(parent.get())->items_from_dom(_dom_element, parent->parent());

                //                    QDomElement record = _child.firstChildElement();
                //                    assert(record.tagName() == "record" || record.isNull());

                //                    if(record.tagName() == "record") {
                //                        assembly_record_and_table_to_parent(record, self);  // model_from_dom(record, parent);    // get recordtable == ItemsFlat
                //                    }
                //                }

                _record_child = _record_child.nextSiblingElement();   // brother record
                assert(_record_child.tagName() == "record" || _record_child.isNull());
            }
        }
    }

}


bool KnowModel::format_check(QDomElement elementFormat)
{
    int baseVersion = 0;
    int baseSubVersion = 0;

    // Если DOM-элемент с версией и подверсией существует
    if(!elementFormat.isNull()) {
        baseVersion = elementFormat.attribute("version", "0").toInt(); // Считывается номер версии
        baseSubVersion = elementFormat.attribute("subversion", "0").toInt(); // Считывается номер подверсии
    }

    // Если номер версии или подверсии выше чем поддерживаемые программой
    if(baseVersion > CURRENT_FORMAT_VERSION ||
       baseSubVersion > CURRENT_FORMAT_SUBVERSION)
        return false;

    // В настоящий момент поддерживается формат 1.2
    // В настоящий момент предполагается, что номер версии всегда 1, поэтому вся работа идет по номеру подверсии
    if(baseSubVersion <= 1)
        if(update_sub_version_from_1_to_2() == false) // Смена формата с 1.1 на 1.2
            return false;

    // На будущее, для перехода с подверии 2 на подверсию 3, эти строки надо добавлять к существующим (а не заменять)
    // if(baseSubVersion<=2)
    //  if(updateSubVersionFrom2To3()==false)
    //   return false;

    return true;
}


bool KnowModel::update_sub_version_from_1_to_2(void)
{


    return true;
}


void KnowModel::reload(void)
{
    if(_xml_file_name != "")
        init_from_xml(_xml_file_name);
}




// Запись всех данных в XML файл
void KnowModel::save()
{
#ifdef _with_record_table
    record_to_item();
#endif

    // Если имя файла небыло проинициализировано
    if(_xml_file_name == "")
        critical_error(tr("In KnowTreeModel can't set file name for XML file"));

    // Коструирование DOM документа для записи в файл
    QDomDocument doc("mytetradoc");

    // Установка заголовка
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

    // Создание корневого элемента
    QDomElement rootelement = doc.createElement("root");

    // Добавление формата версии к корневому элементу
    QDomElement formvers = doc.createElement("format");
    formvers.setAttribute("version", CURRENT_FORMAT_VERSION);
    formvers.setAttribute("subversion", CURRENT_FORMAT_SUBVERSION);
    rootelement.appendChild(formvers);

    QDomElement content = doc.createElement("content");

    // Получение полного DOM дерева хранимых данных
    QDomElement elmdomtree = dom_from_treeitem(_root_item);

    content.appendChild(elmdomtree);

    // Добавление полного дерева DOM хранимых данных к корневому элементу
    rootelement.appendChild(    //
        content // elmdomtree
    );

    // Добавление корневого элемента в DOM документ
    doc.appendChild(rootelement);

    // Распечатка на экран, что будет выводиться в XML файл
    // qDebug() << "Doc document for write " << doc.toString();

    // Перенос текущего файла дерева в корзину
    DiskHelper::remove_file_to_trash(_xml_file_name);

    // Запись DOM данных в файл
    QFile wfile(_xml_file_name);

    if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cant open file " << _xml_file_name << " for write.";
        exit(1);
    }

    QTextStream out(&wfile);
    out.setCodec("UTF-8");
    out << doc.toString();
}


// Генерирование полного DOM дерева хранимых данных
QDomElement KnowModel::dom_from_treeitem(boost::intrusive_ptr<TreeItem> root)   // full modeldata to dom
{
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    //    QDomElement elm = doc->createElement("content");

    // qDebug() << "New element for export" << xmlNodeToString(elm);
    QDomElement records_all_in_root = root->dom_from_treeitem(doc);
    //    dom_from_record(doc, elm, root);

    // qDebug() << "In export_fullmodeldata_to_dom stop element " << xmlNodeToString(elm);
    //    elm.appendChild(record);

    return records_all_in_root;  // elm;
}


// Рекурсивное преобразование Item-элементов в Dom дерево
void KnowModel::dom_from_treeitem(std::shared_ptr<QDomDocument> doc, QDomElement &xml_data, boost::intrusive_ptr<TreeItem> curr_item)
{


    // Если в ветке присутсвует таблица конечных записей
    // В первую очередь добавляется она
    if(curr_item->count_direct() > 0) {
        // Обработка таблицы конечных записей

        // Получение Dom дерева таблицы конечных записей
        // В метод передается QDomDocument, на основе кторого будут создаваться элементы
        QDomElement item_flat_dom = curr_item->dom_from_treeitem(doc);

        // Dom дерево таблицы конечных записей добавляется
        // как подчиненный элемент к текущему элементу
        if(!item_flat_dom.isNull()) {
            xml_data.appendChild(item_flat_dom);  // .cloneNode()
        }
    }

    //    // Обработка каждой подчиненной ветки
    //    int i;

    for(int i = 0; i < curr_item->count_direct(); i++) {
        //        assert(curr_item->child(i).get() != curr_item);

        if(curr_item->child(i) != curr_item) {
            //            // Временный элемент, куда будет внесена текущая перебираемая ветка
            //            QDomElement  tempElement = doc->createElement("node");

            //            // Получение всех полей для данной ветки
            //            QMap<QString, QString> fields = curr_item->child(i)->all_fields_direct();

            //            // Перебираются поля элемента ветки
            //            QMapIterator<QString, QString> fields_iterator(fields);

            //            while(fields_iterator.hasNext()) {
            //                fields_iterator.next();

            //                // Установка для временного элемента значения перебираемого поля как атрибута
            //                tempElement.setAttribute(fields_iterator.key(), fields_iterator.value());
            //            }

            //            // tempElement.appendChild(curr_item->export_local_to_dom(doc));

            //            // Добавление временного элемента к основному документу
            //            xml_data.appendChild(tempElement);

            //            // qDebug() << "In parsetreetodom() current construct doc " << xmlNodeToString(*xmldata);

            // Рекурсивная обработка
            QDomElement workElement = xml_data.lastChildElement();
            dom_from_treeitem(doc, workElement, curr_item->child(i));
        }
    }

}


// Добавление новой ветки после указанной ветки
boost::intrusive_ptr<TreeItem> KnowModel::lock_sibling_add(const QModelIndex &_index, QString id, QString name)
{
    // Получение ссылки на родительский Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(_index);
    boost::intrusive_ptr<TreeItem> parent = current->parent();
    assert(parent);
    boost::intrusive_ptr<TreeItem> _result(nullptr);

    if(parent) {
        //        beginInsertRows(_index.parent()
        //                        , _index  // parent->count_direct()
        //                        , _index  // parent->count_direct()
        //                       );
        _result = lock_child_add_new(parent, _index.row(), id, name);
        //        endInsertRows();
    }

    return _result;
}

// Добавление новой ветки после указанной ветки
boost::intrusive_ptr<TreeItem> KnowModel::lock_sibling_move(const QModelIndex &_index, boost::intrusive_ptr<TreeItem> it)
{
    // Получение ссылки на родительский Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(_index);
    boost::intrusive_ptr<TreeItem> parent = current->parent();
    assert(parent);
    //    assert(globalparameters.tree_screen()->know_model_board()->item_by_id(current->id()));
    //    assert(globalparameters.tree_screen()->know_model_board()->item_by_id(parent->id()));
    //    assert(item_by_id(current->id()));
    //    assert(item_by_id(parent->id()));
    //    assert(is_id_exists(current->id()));
    assert(is_id_exists(parent->id()));
    boost::intrusive_ptr<TreeItem> _result(nullptr);

    if(parent) {
        //        beginInsertRows(_index.parent()
        //                        , _index  // parent->count_direct()
        //                        , _index  // parent->count_direct()
        //                       );
        _result = lock_child_move(parent, _index.row(), it);
        //        endInsertRows();
    }

    assert(is_id_exists(_result->id()));
    return _result;
}


// Добавление новой подветки к указанной ветке
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_add_new(const QModelIndex &_index, QString id, QString name)
{
    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> parent = item(_index);
    boost::intrusive_ptr<TreeItem> _result(nullptr);
    //    beginInsertRows(_index
    //                    , _index.row()  // parent->count_direct()
    //                    , _index.row()  // parent->count_direct()
    //                   );
    _result = lock_child_add_new(parent, _index.row(), id, name);
    //    endInsertRows();
    return _result;
}



// Добавление новой подветки к указанной ветке
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_move(const QModelIndex &_index, boost::intrusive_ptr<TreeItem> it)
{
    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> parent = item(_index);
    boost::intrusive_ptr<TreeItem> _result(nullptr);

    //    beginInsertRows(_index
    //                    , _index.row()  // parent->count_direct()
    //                    , _index.row()  // parent->count_direct()
    //                   );
    _result = lock_child_move(parent, _index.row(), it);
    //    endInsertRows();
    return _result;
}


// Add a new highlight to the Item element  // Добавление новой подветки к Item элементу
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_move(boost::intrusive_ptr<TreeItem> parent, int pos, boost::intrusive_ptr<TreeItem> item) //    , QString id, QString name
{

    beginInsertRows(index(parent)
                    , pos   // parent->count_direct()
                    , (pos + 1 < parent->count_direct()) ? pos + 1 : parent->count_direct()
                   );
    boost::intrusive_ptr<TreeItem> result = parent->child_move(
                                                pos // parent->count_direct()
                                                , item); //add_new_branch(parent, id, name);
    endInsertRows();

    if(parent->field("crypt") == "1") {
        // Новая ветка превращается в зашифрованную
        result->to_encrypt();
    }


    assert(is_id_exists(result->id()));
    // Подузел прикрепляется к указанному элементу
    // в конец списка подчиненных элементов
    //    boost::intrusive_ptr<TreeItem> current =
    return result;  // parent->add_child(item);

    //    // Определяется ссылка на только что созданную ветку
    //    boost::intrusive_ptr<TreeItem> current = parent->child(parent->child_count() - 1);

    //    // Устанавливается идентификатор узла
    //    current->field("id", id);

    //    // Устанавливается навание узла
    //    current->field("name", name);

    //    // Инициализируется таблица конечных записей
    //    current->tabledata(std::make_shared<RecordTable>(current));

    //    // Определяется, является ли родительская ветка зашифрованной
    //    if(parent->field("crypt") == "1") {
    //        // Новая ветка превращается в зашифрованную
    //        current->to_encrypt();
    //    }

    //    return current;
}

//boost::intrusive_ptr<TreeItem> KnowModel::add_new_branch(boost::intrusive_ptr<TreeItem> parent, boost::intrusive_ptr<TreeItem> item)
//{
//    boost::intrusive_ptr<TreeItem> current;

//    if(item->field("url") != browser::Browser::_defaulthome) {
//        // Подузел прикрепляется к указанному элементу
//        // в конец списка подчиненных элементов
//        current = parent->add_child(item);

//        //    // Определяется ссылка на только что созданную ветку
//        //    boost::intrusive_ptr<TreeItem> current = parent->child(parent->child_count() - 1);

//        //    // Инициализируется таблица конечных записей
//        //    current->tabledata(std::make_shared<RecordTable>(current));

//        // Определяется, является ли родительская ветка зашифрованной
//        if(parent->field("crypt") == "1") {
//            // Новая ветка превращается в зашифрованную
//            current->to_encrypt();
//        }
//    }

//    return current;
//}






// Добавление новой подветки к Item элементу
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_add_new(boost::intrusive_ptr<TreeItem> parent, int pos, QString id, QString name)
{
    beginInsertRows(index(parent)
                    , pos   // parent->count_direct()
                    , pos   // parent->count_direct()
                   );
    // Подузел прикрепляется к указанному элементу
    // в конец списка подчиненных элементов
    boost::intrusive_ptr<TreeItem> current = parent->child_add_new(pos);

    //    // Определяется ссылка на только что созданную ветку
    //    boost::intrusive_ptr<TreeItem> current = parent->child(parent->child_count() - 1);

    // Устанавливается идентификатор узла
    current->field("id", id);

    // Устанавливается навание узла
    current->field("name", name);

    //    // Инициализируется таблица конечных записей
    //    current->tabledata(std::make_shared<RecordTable>(current));

    // Определяется, является ли родительская ветка зашифрованной
    if(parent->field("crypt") == "1") {
        // Новая ветка превращается в зашифрованную
        current->to_encrypt();
    }

    endInsertRows();
    return current;
}


#ifdef _with_record_table

// Add a new highlight to the Item element  // Добавление новой подветки к Item элементу
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_add(boost::intrusive_ptr<Record> record, boost::intrusive_ptr<TreeItem> parent) //    , QString id, QString name
{

    beginInsertRows(index(parent), parent->current_count(), parent->current_count());
    boost::intrusive_ptr<TreeItem> result = parent->add_child(record); //add_new_branch(parent, id, name);
    endInsertRows();

    // Подузел прикрепляется к указанному элементу
    // в конец списка подчиненных элементов
    //    boost::intrusive_ptr<TreeItem> current =
    return result;  // parent->add_child(record);

    //    // Определяется ссылка на только что созданную ветку
    //    boost::intrusive_ptr<TreeItem> current = parent->child(parent->child_count() - 1);

    //    // Устанавливается идентификатор узла
    //    current->field("id", id);

    //    // Устанавливается навание узла
    //    current->field("name", name);

    //    // Инициализируется таблица конечных записей
    //    current->tabledata(std::make_shared<RecordTable>(current));

    //    // Определяется, является ли родительская ветка зашифрованной
    //    if(parent->field("crypt") == "1") {
    //        // Новая ветка превращается в зашифрованную
    //        current->to_encrypt();
    //    }

    //    return current;
}
#endif




// Добавление новой подветки к указанной ветке из буфера обмена
QString KnowModel::lock_child_paste(const QModelIndex &_index, ClipboardBranch *subbranch)
{
    QString pasted_branch_id;

    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> parent = item(_index);

    //    beginInsertRows(_index, parent->count_direct(), parent->count_direct());
    pasted_branch_id = lock_child_paste_impl(parent, parent->count_direct(), (ClipboardBranch *)subbranch);
    //    endInsertRows();

    return pasted_branch_id;
}


QString KnowModel::lock_sibling_paste(const QModelIndex &_index, ClipboardBranch *subbranch)
{
    QString pasted_branch_id;

    // Получение ссылки на родительский Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(_index);
    boost::intrusive_ptr<TreeItem> parent = current->parent();
    assert(parent);

    if(parent) {
        //        beginInsertRows(_index.parent(), parent->count_direct(), parent->count_direct());
        pasted_branch_id = lock_child_paste_impl(parent, current->sibling_order(), (ClipboardBranch *)subbranch);
        //        endInsertRows();
    }

    return pasted_branch_id;
}


// Добавление подветки из буфера обмена относительно указанного элемента
// Функция возвращает новый идентификатор стартовой добавленной подветки
QString KnowModel::lock_child_paste_impl(boost::intrusive_ptr<TreeItem> _target_item, int _sibling_order, ClipboardBranch *subbranch)
{
    qDebug() << "In paste_subbranch()";


    std::function<QString(boost::intrusive_ptr<TreeItem>, int, QString, ClipboardBranch *)>
    child_paste_recursive
        = [&](boost::intrusive_ptr<TreeItem>    _clip_parent
              , int                             _sibling_order
              , QString                         _clip_child_parent_id
              , ClipboardBranch *               _sub_branch
    ) {
        QString id = _clip_parent->id();

        //        if(_clip_parent->parent()) {
        qDebug() << "In paste_subbranch_recurse()";

        QModelIndex _index = index(_clip_parent);

        beginInsertRows(_index  // .parent()
                        , _sibling_order    // _index.row()  // item->sibling_order() //item->parent()->position_current(item->id())
                        , _sibling_order    // _index.row()  // item->sibling_order() //item->parent()->position_current(item->id())
                       );




        //            // ---------------------------
        //            // Добавляется стартовая ветка
        //            // ---------------------------

        //            // Выясняются поля стартовой ветки
        //            QMap<QString, QString> subbranch_fields = _sub_branch->branch_fields_by_id(_start_branch_id);

        //            // Выясняется имя ветки
        //            QString subbranch_name = subbranch_fields["name"];

        //            // Получение уникального идентификатора, под которым будет добавляться ветка
        //            id = get_unical_id();

        //            // Стартовая ветка добавляется
        //            auto _new_item = lock_child_add_new(_item, _item->parent()->count_direct(), id, subbranch_name);

        //            //        // Выясняется указатель на эту добавленную ветку
        //            //        auto newitem = item_by_id(id);

        //            qDebug() << "KnowTreeModel::branch_paste_sub_recurse() : create branch with field" << _new_item->fields_all();


        // -----------------------------------------------
        // Для стартовой ветки добавляются конечные записи
        // -----------------------------------------------

        // Выясняются данные конечных записей
        QList< boost::intrusive_ptr<TreeItem> > records = _sub_branch->records_by_branch_id(_clip_child_parent_id);

        foreach(boost::intrusive_ptr<TreeItem> record, records) {
            qDebug() << "Add table record " + record->field("name");
            // _new_item
            auto result = _clip_parent->child_move(// _new_item
                              _clip_parent->count_direct(), record, ADD_NEW_RECORD_TO_END);

            //                // --------------------
            //                // Добавляются подветки
            //                // --------------------

            //                // Выясняется линейная структура добавляемого дерева
            //                QList<CLIPB_TREE_ONE_LINE> tree = _sub_branch->id_tree_get();

            //                // Выясняется список подветок для стартовой ветки
            //                QStringList subbranch_id_list;

            //                foreach(CLIPB_TREE_ONE_LINE one_line, tree) {
            //                    if(one_line._branch_id == _start_branch_id) {
            //                        subbranch_id_list = one_line._subbranches_id;
            //                    }
            //                }

            for(int i; i < result->count_direct(); i++) { //foreach(QString current_subbranch_id, subbranch_id_list) {
                // QList< boost::intrusive_ptr<TreeItem> > records = _sub_branch->records_by_branch_id(current_subbranch_id);
                assert(result->item_fat(i)->parent_id() == result->id());
                // foreach(boost::intrusive_ptr<TreeItem> record, records) {
                child_paste_recursive(result, i, result->item_fat(i)->parent_id(), _sub_branch);
                // }
            }
        }

        endInsertRows();



        //            // --------------------
        //            // Добавляются подветки
        //            // --------------------

        //            // Выясняется линейная структура добавляемого дерева
        //            QList<CLIPB_TREE_ONE_LINE> tree = _sub_branch->id_tree_get();

        //            // Выясняется список подветок для стартовой ветки
        //            QStringList subbranch_id_list;

        //            foreach(CLIPB_TREE_ONE_LINE one_line, tree) {
        //                if(one_line._branch_id == _start_branch_id) {
        //                    subbranch_id_list = one_line._subbranches_id;
        //                }
        //            }

        //            foreach(QString current_subbranch_id, subbranch_id_list) {
        //                QList< boost::intrusive_ptr<TreeItem> > records = _sub_branch->records_by_branch_id(current_subbranch_id);

        //                foreach(boost::intrusive_ptr<TreeItem> record, records) {
        //                    child_paste_recurse(record, current_subbranch_id, _sub_branch);
        //                }
        //            }


        //        }

        assert(id != "");
        return id;
    };

    //    // Выясняется линейная структура добавляемого дерева
    //    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->id_tree_get();

    //    // Идентификатор стартовой ветки лежит в первом элементе списка
    //    QString start_branch_id = tree[0]._branch_id;
    auto _clip_root_item_parent_id = subbranch->clip_root_item_parent_id();

    child_paste_recursive(_target_item, _sibling_order, _clip_root_item_parent_id, subbranch);

    return _target_item->id();
}

//// Перемещение ветки вверх
//QModelIndex KnowModel::branch_move_up(const QModelIndex &_index)
//{
//    return branch_move_up_dn(_index
//                             , &TreeItem::move_up   //, 1
//                            );
//}


//// Перемещение ветки вниз
//QModelIndex KnowModel::branch_move_dn(const QModelIndex &_index)
//{
//    return branch_move_up_dn(_index
//                             , &TreeItem::move_dn   //-1
//                            );
//}


// Перемещение ветки вверх или вниз
QModelIndex KnowModel::branch_move_up_dn(const QModelIndex &_index
                                         , int(TreeItem::*_move)()   //int direction
                                        )
{

    QModelIndex _new_index;
    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(_index);

    // Перемещается ветка
    emit layoutAboutToBeChanged();

    int move_distance = 0;

    //    if(direction == 1) {
    //        moveok = current->move_up(); // Перемещение в Item представлении
    //    } else {
    //        moveok = current->move_dn();
    //    }
    move_distance = ((*current).*(_move))();

    if(move_distance != 0) {
        // Получение QModelIndex расположенного над или под элементом index
        QModelIndex swap_index = _index.sibling(_index.row() + move_distance, 0);   // _index.sibling(_index.row() - direction, 0);

        // Проверяется допустимость индекса элемента, куда будет сделано перемещение
        if(!swap_index.isValid())
            return QModelIndex(); // Возвращается пустой указатель

        // Запоминаются параметры для абсолютного позиционирования засветки
        // после перемещения ветки
        int         swpidx_row      = swap_index.row();
        int         swpidx_column   = swap_index.column();
        QModelIndex swpidx_parent   = swap_index.parent();


        changePersistentIndex(swap_index, _index);  // (from, to)
        changePersistentIndex(_index, swap_index);  // (from, to)


        emit layoutChanged();

        // Возвращается указатель на перемещенную ветку
        if(move_distance != 0) {
            _new_index = TreeModel::index(swpidx_row, swpidx_column, swpidx_parent);
        } else {
            _new_index = _index; // QModelIndex(); // Возвращается пустой указатель
        }
    }

    return _new_index;
}


// Получение индекса подчиненного элемента с указанным номером
// Нумерация элементов считается что идет с нуля
QModelIndex KnowModel::index_direct(const QModelIndex &_parent_index, int n) const
{
    // Проверяется, передан ли правильный QModelIndex
    // Если он неправильный, возвращается пустой индекс
    if(!_parent_index.isValid()) {
        qDebug() << "In indexChildren() unavailable model index";
        return QModelIndex();
    }

    // Выясняется указатель на основной Item элемент
    boost::intrusive_ptr<TreeItem> it = item(_parent_index);

    // Если у основного Item элемента запрашивается индекс
    // несуществующего подэлемента, возвращается пустой индекс
    if(n < 0 || n >= it->count_direct()) {
        qDebug() << "In indexChildren() unavailable children number";
        return QModelIndex();
    }

    // Выясняется указатель на подчиненный Item элемент
    boost::intrusive_ptr<TreeItem> child_item = it->child(n);

    // Если указатель на подчиненный Item элемент непустой
    if(child_item) {
        // return createIndex(0, 0, childitem); // Индекс подчиненного элемента
        // return createIndex(n, 0, parent.internalPointer());

        return TreeModel::index(n, 0, _parent_index);
    } else {
        qDebug() << "In indexChildren() empty child element";
        return QModelIndex(); // Возвращается пустой индекс
    }
}


//// Get QModelIndex of the KnownTreeItem   // Получение QModelIndex по известному TreeItem
//QModelIndex KnowTreeModel::index(boost::intrusive_ptr<TreeItem> item)
//{
//    int itemrow = item->child_index();

//    return this->createIndex(itemrow, 0, static_cast<void *>(item.get()));
//}


// Возвращает общее количество записей, хранимых в дереве
int KnowModel::count_records_all(void)const
{
    //    // Обнуление счетчика
    //    get_all_record_count_recurse(_root_item, 0);

    //    return get_all_record_count_recurse(_root_item, 1);

    return size_of(_root_item);
}


// Возвращает количество записей в ветке и всех подветках
int KnowModel::size_of(boost::intrusive_ptr<TreeItem> item)const
{
    std::function<int (boost::intrusive_ptr<TreeItem>, int)>
    get_all_record_count_recurse
    = [&](boost::intrusive_ptr<TreeItem> item, int mode) {
        static int n = 0;

        if(mode == 0) {
            n = 0;
            return 0;
        }

        n = n + item->count_direct();

        for(int i = 0; i < item->count_direct(); i++)
            get_all_record_count_recurse(item->child(i), 1);

        return n;
    };

    // Обнуление счетчика
    get_all_record_count_recurse(_root_item, 0);

    return get_all_record_count_recurse(item, 1);

}


//int KnowTreeModel::get_all_record_count_recurse(boost::intrusive_ptr<TreeItem> item, int mode)
//{
//    static int n = 0;

//    if(mode == 0) {
//        n = 0;
//        return 0;
//    }

//    n = n + item->row_count();

//    for(int i = 0; i < item->child_count(); i++)
//        get_all_record_count_recurse(item->child(i), 1);

//    return n;
//}


//// Проверка наличия идентификатора ветки во всем дереве
//bool KnowModel::is_item_exists(QString findId)
//{

//    //    std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
//    //    is_item_id_exists_recurse =
//    //        [&](boost::intrusive_ptr<TreeItem> item, QString id_to_find, int mode
//    //    ) {
//    //        static bool is_exists = false;

//    //        // Инициализация
//    //        if(mode == 0) {
//    //            return is_exists = false;
//    //            //            return false;
//    //        }

//    //        // Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
//    //        if(is_exists)
//    //            return true;

//    //        // Если текущая ветка содержит искомый идетнификатор
//    //        if(item->field("id") == id_to_find) {
//    //            return is_exists = true;
//    //            //            return true;
//    //        }

//    //        // Перебираются подветки
//    //        for(int i = 0; i < item->current_count(); i++)
//    //            is_item_id_exists_recurse(item->child(i), id_to_find, 1);

//    //        return is_exists;
//    //    };

//    // Обнуление счетчика
//    is_item_id_exists_recurse(_root_item, findId, 0);

//    return is_item_id_exists_recurse(_root_item, findId, 1);
//}

// Проверка наличия идентификатора записи во всем дереве
bool KnowModel::is_id_exists(QString findId)const
{

    //    std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
    //    is_record_id_exists_recurse
    //        = [&](
    //              boost::intrusive_ptr<TreeItem> item, QString find_id, int mode
    //    ) {

    //        static bool isExists = false;

    //        // Инициализация
    //        if(mode == 0) {
    //            return isExists = false;
    //            // return false;
    //        }

    //        // Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
    //        if(isExists)
    //            return true;

    //        // Если таблица записей текущей ветки содержит искомый идентификатор
    //        if(item->is_item_exists(find_id)) {
    //            return isExists = true;
    //            // return true;
    //        }

    //        // Перебираются подветки
    //        for(int i = 0; i < item->current_count(); i++)
    //            is_record_id_exists_recurse(item->child(i), find_id, 1);

    //        return isExists;
    //    };
    // Обнуление счетчика
    is_item_id_exists_recurse(_root_item, findId, 0);

    return is_item_id_exists_recurse(_root_item, findId, 1);
}

//bool KnowTreeModel::is_item_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode)
//{
//    static bool isExists = false;

//    // Инициализация
//    if(mode == 0) {
//        isExists = false;
//        return false;
//    }

//    // Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
//    if(isExists)
//        return true;

//    // Если текущая ветка содержит искомый идетнификатор
//    if(item->field("id") == findId) {
//        isExists = true;
//        return true;
//    }

//    // Перебираются подветки
//    for(int i = 0; i < item->child_count(); i++)
//        is_item_id_exists_recurse(item->child(i), findId, 1);

//    return isExists;
//}

#ifdef _with_record_table
void TreeModelKnow::record_to_item()
{

    std::function<void (boost::intrusive_ptr<TreeItem>)>  // , boost::intrusive_ptr<TreeItem>
    record_to_item_recurse  = [&](
                                  boost::intrusive_ptr<TreeItem> item
                                  //            , boost::intrusive_ptr<TreeItem> parent
    ) {

        //        if(!is_item_id_exists(item->id())) {
        //            add_child(item, parent);
        //        }

        item->records_to_children();

        // Перебираются подветки
        for(int i = 0; i < item->size(); i++) {
            record_to_item_recurse(item->child(i)); // , item
        }

    };

    _root_item->records_to_children();

    //    if(_root_item->child_count() > 0) {

    for(int i = 0; i < _root_item->size(); i++) {
        record_to_item_recurse(_root_item->child(i));   // , _root_item
    }

    //    }

}
#endif




//bool KnowTreeModel::is_record_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode)
//{
//    static bool isExists = false;

//    // Инициализация
//    if(mode == 0) {
//        isExists = false;
//        return false;
//    }

//    // Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
//    if(isExists)
//        return true;

//    // Если таблица записей текущей ветки содержит искомый идентификатор
//    if(item->is_item_exists(findId)) {
//        isExists = true;
//        return true;
//    }

//    // Перебираются подветки
//    for(int i = 0; i < item->child_count(); i++)
//        is_record_id_exists_recurse(item->child(i), findId, 1);

//    return isExists;
//}





//// Рекурсивное добавление дерева
//// item - элемент главного дерева, к которому добавляется ветка
//// start_branch_id - идентификатор ветки в переданном линейном представлении
////                   добавляемого дерева
//// subbranch - добавляемое дерево
//QString KnowTreeModel::paste_sub_branch_recurse(boost::intrusive_ptr<TreeItem> item
//                                                , QString startBranchId
//                                                , ClipboardBranch *subbranch)
//{
//    qDebug() << "In paste_subbranch_recurse()";

//    // ---------------------------
//    // Добавляется стартовая ветка
//    // ---------------------------

//    // Выясняются поля стартовой ветки
//    QMap<QString, QString> subbranch_fields = subbranch->getBranchFieldsById(startBranchId);

//    // Выясняется имя ветки
//    QString subbranch_name = subbranch_fields["name"];

//    // Получение уникального идентификатора, под которым будет добавляться ветка
//    QString id = get_unical_id();

//    // Стартовая ветка добавляется
//    add_new_branch(item, id, subbranch_name);

//    // Выясняется указатель на эту добавленную ветку
//    auto newitem = item_by_id(id);

//    qDebug() << "KnowTreeModel::paste_subbranch_recurse() : create branch with field" << newitem->all_fields();

//    // -----------------------------------------------
//    // Для стартовой ветки добавляются конечные записи
//    // -----------------------------------------------

//    // Выясняются данные конечных записей
//    QList< boost::intrusive_ptr<Record> > records = subbranch->getBranchRecords(startBranchId);

//    foreach(boost::intrusive_ptr<Record> record, records) {
//        qDebug() << "Add table record " + record->field("name");
//        newitem->record_table()->insert_new_record(0, record, ADD_NEW_RECORD_TO_END);
//    }

//    // --------------------
//    // Добавляются подветки
//    // --------------------

//    // Выясняется линейная структура добавляемого дерева
//    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->getIdTree();

//    // Выясняется список подветок для стартовой ветки
//    QStringList subbranch_list;

//    foreach(CLIPB_TREE_ONE_LINE one_line, tree)
//        if(one_line.branch_id == startBranchId)
//            subbranch_list = one_line.subbranches_id;

//    foreach(QString current_subbranch, subbranch_list)
//        paste_sub_branch_recurse(newitem, current_subbranch, subbranch);

//    return id;
//}


// Перешифрование базы с новым паролем
void KnowModel::re_encrypt(QString previousPassword, QString currentPassword)
{
    // Получение путей ко всем подветкам дерева
    QList<QStringList> subbranchespath = _root_item->path_children_all();

    // Перебираются подветки
    foreach(QStringList currPath, subbranchespath) {
        // Перешифровываются только те подветки, которые имеют
        // флаг шифрования, и у которых родительская ветка нешифрована
        auto currBranch = item(currPath);
        auto currBranchParent = currBranch->parent();

        if(currBranch->field("crypt") == "1" &&
           currBranchParent->field("crypt") != "1") {
            Password pwd;

            pwd.setCryptKeyToMemory(previousPassword);
            currBranch->to_decrypt();

            pwd.setCryptKeyToMemory(currentPassword);
            currBranch->to_encrypt();
        }

    } // Закончился перебор подветок


    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)
    globalparameters.tree_screen()->knowtree_save();
}


//// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде TreeItem *
//// Если ветка с указанным ID не будет найдена, возвращается NULL
//boost::intrusive_ptr<TreeItem> KnowModel::item_by_url(QUrl find_url)const
//{
//    std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>, QUrl, int)>
//    item_by_url_recurse    //    boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem> item, QString name, int mode);
//    = [&](boost::intrusive_ptr<TreeItem> item, QUrl find_url, int mode) {
//        static boost::intrusive_ptr<TreeItem> find_item;

//        if(mode == 0) {
//            find_item = nullptr;
//            return find_item;   // nullptr;
//        }

//        if(find_item) return find_item;

//        if(QUrl(item->field("url")).fragment() == find_url.fragment()) {
//            find_item = item;
//            return find_item;
//        } else {
//            for(int i = 0; i < item->count_direct(); i++)
//                item_by_url_recurse(item->child(i), find_url, 1);

//            return find_item;
//        }
//    };

//    // Инициализация поиска
//    item_by_url_recurse(_root_item, QUrl(), 0);

//    // Запуск поиска и возврат результата
//    return item_by_url_recurse(_root_item, find_url, 1);
//}

// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде TreeItem *
// Если ветка с указанным ID не будет найдена, возвращается NULL
boost::intrusive_ptr<TreeItem> KnowModel::item_by_name(QString name)const
{
    std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>, QString, int)>
    item_by_name_recurse    //    boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem> item, QString name, int mode);
    = [&](boost::intrusive_ptr<TreeItem> item, QString name, int mode) {
        static boost::intrusive_ptr<TreeItem> find_item;

        if(mode == 0) {
            find_item = nullptr;
            return find_item;   // nullptr;
        }

        if(find_item) return find_item;

        if(item->name() == name) {
            find_item = item;
            return find_item;
        } else {
            for(int i = 0; i < item->count_direct(); i++)
                item_by_name_recurse(item->child(i), name, 1);

            return find_item;
        }
    };

    // Инициализация поиска
    item_by_name_recurse(_root_item, "", 0);

    // Запуск поиска и возврат результата
    return item_by_name_recurse(_root_item, name, 1);
}

// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде TreeItem *
// Если ветка с указанным ID не будет найдена, возвращается NULL
boost::intrusive_ptr<TreeItem> KnowModel::item_by_id(QString id)const
{
    std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeItem>, QString, int)>
    item_by_id_recurse
    = [&](boost::intrusive_ptr<TreeItem> item, QString id, int mode) {
        static boost::intrusive_ptr<TreeItem> find_item;

        if(mode == 0) {
            find_item = nullptr;
            return find_item;
        }

        if(find_item) return find_item;

        if(item->id() == id) {
            find_item = item;
            return find_item;
        } else {
            for(int i = 0; i < item->count_direct(); i++)
                item_by_id_recurse(item->child(i), id, 1);

            return find_item;
        }
    };
    // Initialize the search    // Инициализация поиска
    item_by_id_recurse(_root_item, "0", 0);

    // Запуск поиска и возврат результата
    return item_by_id_recurse(_root_item, id, 1);
}


//boost::intrusive_ptr<TreeItem> KnowTreeModel::item_by_id_recurse(boost::intrusive_ptr<TreeItem> item, QString id, int mode)
//{
//    static boost::intrusive_ptr<TreeItem> find_item;

//    if(mode == 0) {
//        find_item = nullptr;
//        return nullptr;
//    }

//    if(find_item != nullptr) return find_item;

//    if(item->id() == id) {
//        find_item = item;
//        return find_item;
//    } else {
//        for(int i = 0; i < item->child_count(); i++)
//            item_by_id_recurse(item->child(i), id, 1);

//        return find_item;
//    }
//}


// Получение пути к ветке, где находится запись
QStringList KnowModel::record_path(QString recordId)const
{
    std::function<QStringList(boost::intrusive_ptr<TreeItem>
                              , QStringList
                              , QString
                              , int)>
    record_path_recurse
        =
            [&](
                boost::intrusive_ptr<TreeItem> item
                , QStringList currentPath
                , QString recordId
                , int mode
    ) {
        static QStringList findPath;
        static bool isFind;

        if(mode == 0) {
            findPath.clear();
            isFind = false;
            return QStringList();
        }

        if(isFind)
            return findPath;

        // Путь дополняется
        currentPath << item->id();

        // Если в данной ветке есть искомая запись
        if(item->is_id_exists(recordId)) {
            isFind = true;
            findPath = currentPath;
        } else {
            // Иначе перебираются подветки
            for(int i = 0; i < item->count_direct(); i++)
                record_path_recurse(item->child(i), currentPath, recordId, 1);
        }

        return findPath;
    };

    record_path_recurse(_root_item, QStringList(), "0", 0);

    return record_path_recurse(_root_item, QStringList(), recordId, 1);
}


//QStringList KnowTreeModel::record_path_recurse(boost::intrusive_ptr<TreeItem> item,
//                                               QStringList currentPath,
//                                               QString recordId,
//                                               int mode)
//{
//    static QStringList findPath;
//    static bool isFind;

//    if(mode == 0) {
//        findPath.clear();
//        isFind = false;
//        return QStringList();
//    }

//    if(isFind)
//        return findPath;

//    // Путь дополняется
//    currentPath << item->id();

//    // Если в данной ветке есть искомая запись
//    if(item->is_item_exists(recordId)) {
//        isFind = true;
//        findPath = currentPath;
//    } else {
//        // Иначе перебираются подветки
//        for(int i = 0; i < item->child_count(); i++)
//            record_path_recurse(item->child(i), currentPath, recordId, 1);
//    }

//    return findPath;
//}


// Метод определяющий есть ли в дереве зашифрованные ветки
bool KnowModel::is_contains_crypt_branches(void)const
{

    std::function<bool (boost::intrusive_ptr<TreeItem>, int)> is_contains_crypt_branches_recurse =
        [&](
            boost::intrusive_ptr<TreeItem> item, int mode
    ) {

        static bool isCrypt = false;

        if(mode == 0) {
            isCrypt = false;
            return isCrypt;
        }

        if(item->field("crypt") == "1") {
            isCrypt = true;
            return isCrypt;
        }

        for(int i = 0; i < item->count_direct(); i++)
            is_contains_crypt_branches_recurse(item->child(i), 1);

        return isCrypt;
    };

    is_contains_crypt_branches_recurse(_root_item, 0);

    return is_contains_crypt_branches_recurse(_root_item, 1);
}


//bool KnowTreeModel::is_contains_crypt_branches_recurse(boost::intrusive_ptr<TreeItem> item, int mode)
//{
//    static bool isCrypt = false;

//    if(mode == 0) {
//        isCrypt = false;
//        return isCrypt;
//    }

//    if(item->field("crypt") == "1") {
//        isCrypt = true;
//        return isCrypt;
//    }

//    for(int i = 0; i < item->child_count(); i++)
//        is_contains_crypt_branches_recurse(item->child(i), 1);

//    return isCrypt;
//}


//// Старый вариант поиска QModelIndex по известному TreeItem закомментирован,
//// но алгоритм может пригодиться для поиска других данных

//// Получение QModelIndex по известному TreeItem
//QModelIndex knowtreemodel::get_item_index(TreeItem *item)
//{
//    // Выясняется начальный QModelIndex дерева
//    QModelIndex rootindex = index(0, 0);

//    // Очищается флаг поиска внутри элементов
//    get_item_index_recurse(rootindex, item, 0);

//    // Перебираются элементы на одном уровне вложения с начальным элементом дерева
//    for(int i = 0; rootindex.sibling(i, 0).isValid(); i++) {
//        // qDebug() << "Sibling current " << (find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(rootindex.sibling(i,0),Qt::EditRole)).toString();

//        // Перебираемый элемент проверяется на соответствие с искомым TreeItem
//        if(item == static_cast<TreeItem *>(rootindex.sibling(i, 0).internalPointer()))
//            return rootindex.sibling(i, 0);
//        else {
//            // Производится поиск внутри элемента
//            QModelIndex idx = get_item_index_recurse(rootindex.sibling(i, 0), item, 1);

//            // Если был найден элемент
//            if(idx.isValid())return idx;
//        }
//    }

//    // Если ничего небыло найдено, возвращается пустой индекс
//    return QModelIndex();
//}


//QModelIndex knowtreemodel::get_item_index_recurse(QModelIndex currindex, TreeItem *finditem, int mode)
//{
//    static QModelIndex findindex;
//    static int findflag = 0;

//    // Из QModelIndex можно всегда получить указатель TreeItem,
//    // поэтому поиск можно вести по QModelIndex

//    // Инициализация поиска
//    if(mode == 0) {
//        findflag = 0;
//        return QModelIndex();
//    }

//    // qDebug() << "Recurse current " << (find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(currindex,Qt::EditRole)).toString();
//    // qDebug() << "Current index have " << currindex.row() << "row";
//    // qDebug() << "Find flag " << findflag;

//    // Если был найден QModelIndex
//    if(findflag == 1)return findindex;

//    for(int i = 0; currindex.child(i, 0).isValid(); i++) {
//        // Проверяется текущий элемент, не соответствует ли
//        // его QModelIndex искомому TreeItem
//        if(findflag == 0 &&
//           finditem == static_cast<TreeItem *>(currindex.child(i, 0).internalPointer())) {
//            findflag = 1;
//            findindex = currindex.child(i, 0);
//            return findindex;
//        }

//        // Рекурсивный вызов поиска в глубину дерева
//        get_item_index_recurse(currindex.child(i, 0), finditem, 1);

//        // Если был найден QModelIndex
//        if(findflag == 1)return findindex;
//    }

//    // Сюда код доходит если на текущем уровне поиска элемент еще не найден
//    return QModelIndex();
//}


void KnowModel::clear()
{
    this->_root_item->delete_list_items();   // = boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr));
}
// if succeeded, id and name changed, deprecated, replaced by KnowView::setup_model(boost::intrusive_ptr<TreeItem> _item)
boost::intrusive_ptr<TreeItem> KnowModel::intercept_self(boost::intrusive_ptr<TreeItem> _item)
{

    boost::intrusive_ptr<TreeItem> result(nullptr);
    //    QMap<QString, QString> root_data;

    //    // Определяется одно поле в корневом объекте
    //    // то есть на экране будет один столбец
    //    root_data["id"] = "0";
    //    root_data["name"] = "";

    beginResetModel();

    // Создание корневого Item объекта
    if(_root_item) _root_item.reset();

    QMap<QString, QString> data;
    data["id"]      = _item->id();
    data["name"]    = _item->name();
    _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data)); // ?

    //    _root_item->field("id", _item->id()); // boost::intrusive_ptr<TreeItem>(new TreeItem(root_data, nullptr)); // ?
    //    _root_item->field("name", _item->name());

    //    //    if(item->id() == global_root_id) {_is_global_root = true;} else {_is_global_root = false;}

    //    // , std::make_shared<RecordTable>(QDomElement())

    //    //    //    // Динамическое создание дерева из Item объектов на основе DOM модели
    //    //    //    setup_modeldata(dom_model, _root_item);
    //    //    assert(dom_model);
    //    //    QDomElement content_root_record = dom_model->documentElement().firstChildElement("content").firstChildElement("record");   // "node"

    //    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();

    auto content_root_record_dom = _item->dom_from_treeitem(std::make_shared<QDomDocument>());

    if(content_root_record_dom.isNull()) {
        qDebug() << "Unable load xml tree, first content node not found.";
        //        return;
    } else {
        result = _root_item;
    }

    dom_to_records(content_root_record_dom, _root_item);

    endResetModel();
    //    save(); // temporary
    _synchronized = false;

    return _root_item;

}

boost::intrusive_ptr<TreeItem> KnowModel::synchronize(boost::intrusive_ptr<TreeItem> source)
{
    boost::intrusive_ptr<TreeItem> result(_root_item);
    result = item_by_id(source->id());
    auto parent = result->parent();

    //    int pos = parent ? parent->list_position(result) : 0;

    if(parent) {
        result.reset(); //->clear_children();
        result = boost::intrusive_ptr<TreeItem>(new TreeItem(parent));
    }

    if(result->is_lite())result->to_fat();

    assert(!result->is_lite());
    assert(!result->attach_table()->is_lite());
    dom_to_records(source->dom_from_treeitem(std::make_shared<QDomDocument>()), result);
    assert(result);
    assert(!result->is_lite());
    assert(!result->attach_table()->is_lite());

    //    if(parent)parent->insert_new_item(pos, result);

    _synchronized = true;

    return result;
}


boost::intrusive_ptr<TreeItem> KnowModel::duplicated_remove(boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source)
{
    boost::intrusive_ptr<TreeItem> keep;
    boost::intrusive_ptr<TreeItem> cut;

    if(target->field("ctime").toULong() > source->field("ctime").toULong()) {
        keep = source;
        cut = target;
    } else {
        keep = target;
        cut = source;
    }

    keep->record_merge(cut);
    record_remove(cut);

    auto model = globalparameters.tree_screen()->tree_view()->source_model();

    if(model->index(source).isValid() || model->index(target).isValid()) {
        globalparameters.tree_screen()->tree_view()->reset();
        globalparameters.tree_screen()->tree_view()->source_model(model->root_item());
    }

    return keep;
}


void KnowModel::record_remove(boost::intrusive_ptr<TreeItem> _item)
{
    if(_item->count_direct() > 0) {
        auto p = _item->parent();
        int pos = p->position_current(_item);
        beginInsertRows(index(p), pos, _item->count_direct() - 1);

        for(int i = 0; i < _item->count_direct(); i++) {
            p->child_move(pos, _item->child(i));
        }

        endInsertRows();
    }

    QModelIndex _index = index(_item);

    beginRemoveRows(parent(_index), _index.row(), _index.row());
    _item->self_remove();
    endRemoveRows();
}
