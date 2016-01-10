#include <QAbstractItemModel>
#include <QMap>
#include <QDomNamedNodeMap>

#include "main.h"
#include "KnowTreeModel.h"
#include "TreeItem.h"
#include "TreeModel.h"
#include "XmlTree.h"

#include "libraries/ClipboardRecords.h"
#include "libraries/ClipboardBranch.h"
#include "models/app_config/AppConfig.h"
#include "views/tree/TreeScreen.h"
#include "libraries/crypt/Password.h"
#include "libraries/DiskHelper.h"

extern AppConfig appconfig;


// Конструктор модели дерева, состоящего из Item элементов
KnowTreeModel::KnowTreeModel(QObject *parent)
    : TreeModel(parent)
      //    , _root_item(nullptr)
{
    //    xmlFileName = "";
    //    rootItem = nullptr;
}


// Деструктор Item модели.
// По-хорошему деструктор перед удалением корневого элемента должен пробежать по
// дереву элементов и удалить их
KnowTreeModel::~KnowTreeModel()
{
    //    delete rootItem;
}


void KnowTreeModel::init_from_xml(QString fileName)
{
    _xml_file_name = fileName;

    // Загрузка файла и преобразование его в DOM модель
    XmlTree xmlt;

    if(!xmlt.load(_xml_file_name))
        return;

    init(xmlt.getDomModel());
}


void KnowTreeModel::init(QDomDocument *domModel)
{
    // Проверка формата XML-файла
    if(!check_format(domModel->documentElement().firstChildElement("format"))) {
        critical_error(tr("Unsupported format version for data base.\nPlease upgrade your MyTetra application."));
        return;
    }


    QMap<QString, QString> root_data;

    // Определяется одно поле в корневом объекте
    // то есть на экране будет один столбец
    root_data["id"] = "0";
    root_data["name"] = "";

    beginResetModel();

    // Создание корневого Item объекта
    if(_root_item) _root_item.reset();

    _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(root_data, nullptr, std::make_shared<RecordTable>(QDomElement())));

    // Динамическое создание дерева из Item объектов на основе DOM модели
    setup_modeldata(domModel, _root_item);

    endResetModel();
}


bool KnowTreeModel::check_format(QDomElement elementFormat)
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


bool KnowTreeModel::update_sub_version_from_1_to_2(void)
{


    return true;
}


void KnowTreeModel::reload(void)
{
    init_from_xml(_xml_file_name);
}


// Разбор DOM модели и преобразование ее в Item модель
void KnowTreeModel::setup_modeldata(QDomDocument *dommodel, boost::intrusive_ptr<TreeItem> parent)
{
    QDomElement contentRootNode = dommodel->documentElement().firstChildElement("content").firstChildElement("node");

    if(contentRootNode.isNull()) {
        qDebug() << "Unable load xml tree, first content node not found.";
        return;
    }

    node_from_dom(contentRootNode, parent);

    return;
}


// Рекурсивный обход DOM дерева и извлечение из него узлов
void KnowTreeModel::node_from_dom(QDomElement domElement, boost::intrusive_ptr<TreeItem> parent)
{
    //    boost::intrusive_ptr<TreeItem> parent = iParent;

    // У данного Dom-элемента ищется таблица конечных записей
    // и данные заполняются в Item-таблицу конечных записей
    parent->tabledata(domElement);

    // Пробегаются все DOM элементы текущего уровня
    // и рекурсивно вызывается обработка подуровней
    while(!domElement.isNull()) {
        if(domElement.tagName() == "node") {
            // Обнаруженый подузел прикрепляется к текущему элементу
            parent->insert_children(parent->child_count(), 1, 1);

            /*
            QString line1,line_name,line_id;
            line1=n.tagName();
            line_name=n.attribute("name");
            line_id=n.attribute("id");
            qDebug() << "Read node " << line1 << " " << line_id<< " " << line_name;
            */

            // Определяются атрибуты узла дерева разделов
            QDomNamedNodeMap attributeMap = domElement.attributes();

            // Перебираются атрибуты узла дерева разделов
            for(int i = 0; i < attributeMap.count(); ++i) {
                QDomNode attribute = attributeMap.item(i);

                QString name = attribute.nodeName();
                QString value = attribute.nodeValue();

                // В дерево разделов устанавливаются считанные атрибуты
                parent->child(parent->child_count() - 1)->set_field_direct(name , value);
            }

            // Вызов перебора оставшегося DOM дерева с прикреплением обнаруженных объектов
            // к только что созданному элементу
            node_from_dom(domElement.firstChildElement(), parent->child(parent->child_count() - 1));

        }

        domElement = domElement.nextSiblingElement();
    }

}


// Генерирование полного DOM дерева хранимых данных
QDomElement KnowTreeModel::export_to_dom(boost::intrusive_ptr<TreeItem> root)   // full modeldata to dom
{
    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    QDomElement elm = doc->createElement("content");

    // qDebug() << "New element for export" << xmlNodeToString(elm);

    export_to_dom(doc, elm, root);

    // qDebug() << "In export_fullmodeldata_to_dom stop element " << xmlNodeToString(elm);

    return elm;
}


// Рекурсивное преобразование Item-элементов в Dom дерево
void KnowTreeModel::export_to_dom(std::shared_ptr<QDomDocument> doc, QDomElement &xml_data, boost::intrusive_ptr<TreeItem> curr_item)
{

    // Если в ветке присутсвует таблица конечных записей
    // В первую очередь добавляется она
    if(curr_item->row_count() > 0) {
        // Обработка таблицы конечных записей

        // Получение Dom дерева таблицы конечных записей
        // В метод передается QDomDocument, на основе кторого будут создаваться элементы
        QDomElement recordTableDom = curr_item->export_to_dom(doc);

        // Dom дерево таблицы конечных записей добавляется
        // как подчиненный элемент к текущему элементу
        if(!recordTableDom.isNull())
            xml_data.appendChild(recordTableDom.cloneNode());
    }

    // Обработка каждой подчиненной ветки
    int i;

    for(i = 0; i < curr_item->child_count(); i++) {
        // Временный элемент, куда будет внесена текущая перебираемая ветка
        QDomElement  tempElement = doc->createElement("node");

        // Получение всех полей для данной ветки
        QMap<QString, QString> fields = curr_item->child(i)->all_fields_direct();

        // Перебираются поля элемента ветки
        QMapIterator<QString, QString> fields_iterator(fields);

        while(fields_iterator.hasNext()) {
            fields_iterator.next();

            // Установка для временного элемента значения перебираемого поля как атрибута
            tempElement.setAttribute(fields_iterator.key(), fields_iterator.value());
        }


        // Добавление временного элемента к основному документу
        xml_data.appendChild(tempElement);

        // qDebug() << "In parsetreetodom() current construct doc " << xmlNodeToString(*xmldata);

        // Рекурсивная обработка
        QDomElement workElement = xml_data.lastChildElement();
        export_to_dom(doc, workElement, curr_item->child(i));
    }

}


// Запись всех данных в XML файл
void KnowTreeModel::save()
{
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

    // Получение полного DOM дерева хранимых данных
    QDomElement elmdomtree = export_to_dom(_root_item);

    // Добавление полного дерева DOM хранимых данных к корневому элементу
    rootelement.appendChild(elmdomtree);

    // Добавление корневого элемента в DOM документ
    doc.appendChild(rootelement);

    // Распечатка на экран, что будет выводиться в XML файл
    // qDebug() << "Doc document for write " << doc.toString();

    // Перенос текущего файла дерева в корзину
    DiskHelper::removeFileToTrash(_xml_file_name);

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


// Добавление новой подветки к указанной ветке
void KnowTreeModel::add_new_child_branch(const QModelIndex &index, QString id, QString name)
{
    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> parent = item(index);

    beginInsertRows(index, parent->child_count(), parent->child_count());
    add_new_branch(parent, id, name);
    endInsertRows();
}


// Добавление новой ветки после указанной ветки
void KnowTreeModel::add_new_sibling_branch(const QModelIndex &index, QString id, QString name)
{
    // Получение ссылки на родительский Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(index);
    boost::intrusive_ptr<TreeItem> parent = current->parent();
    assert(parent);

    if(parent) {
        beginInsertRows(index.parent(), parent->child_count(), parent->child_count());
        add_new_branch(parent, id, name);
        endInsertRows();
    }

    //    else {
    //        beginInsertRows(index, current->childCount(), current->childCount());
    //        addNewBranch(current, id, name);
    //        endInsertRows();
    //    }
}


// Добавление новой подветки к Item элементу
void KnowTreeModel::add_new_branch(boost::intrusive_ptr<TreeItem> parent, QString id, QString name)
{
    // Подузел прикрепляется к указанному элементу
    // в конец списка подчиненных элементов
    parent->add_children();

    // Определяется ссылка на только что созданную ветку
    boost::intrusive_ptr<TreeItem> current = parent->child(parent->child_count() - 1);

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
}


// Добавление новой подветки к указанной ветке из буфера обмена
QString KnowTreeModel::paste_new_child_branch(const QModelIndex &index, ClipboardBranch *subbranch)
{
    QString pasted_branch_id;

    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> parent = item(index);

    beginInsertRows(index, parent->child_count(), parent->child_count());
    pasted_branch_id = paste_sub_branch(parent, (ClipboardBranch *)subbranch);
    endInsertRows();

    return pasted_branch_id;
}


QString KnowTreeModel::paste_new_sibling_branch(const QModelIndex &index, ClipboardBranch *subbranch)
{
    QString pasted_branch_id;

    // Получение ссылки на родительский Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(index);
    boost::intrusive_ptr<TreeItem> parent = current->parent();

    beginInsertRows(index.parent(), parent->child_count(), parent->child_count());
    pasted_branch_id = paste_sub_branch(parent, (ClipboardBranch *)subbranch);
    endInsertRows();

    return pasted_branch_id;
}


// Перемещение ветки вверх
QModelIndex KnowTreeModel::move_up_branch(const QModelIndex &index)
{
    return move_up_dn_branch(index, 1);
}


// Перемещение ветки вниз
QModelIndex KnowTreeModel::move_dn_branch(const QModelIndex &index)
{
    return move_up_dn_branch(index, -1);
}


// Перемещение ветки вверх или вниз
QModelIndex KnowTreeModel::move_up_dn_branch(const QModelIndex &index, int direction)
{
    // Получение QModelIndex расположенного над или под элементом index
    QModelIndex swap_index = index.sibling(index.row() - direction, 0);

    // Проверяется допустимость индекса элемента, куда будет сделано перемещение
    if(!swap_index.isValid())
        return QModelIndex(); // Возвращается пустой указатель

    // Запоминаются параметры для абсолютного позиционирования засветки
    // после перемещения ветки
    int         swpidx_row = swap_index.row();
    int         swpidx_column = swap_index.column();
    QModelIndex swpidx_parent = swap_index.parent();

    // Получение ссылки на Item элемент по QModelIndex
    boost::intrusive_ptr<TreeItem> current = item(index);

    // Перемещается ветка
    emit layoutAboutToBeChanged();

    bool moveok;

    if(direction == 1) moveok = current->move_up(); // Перемещение в Item представлении
    else             moveok = current->move_dn();

    if(moveok) {
        changePersistentIndex(swap_index, index);
        changePersistentIndex(index, swap_index);
    }

    emit layoutChanged();

    // Возвращается указатель на перемещенную ветку
    if(moveok) return this->index(swpidx_row, swpidx_column, swpidx_parent);
    else return QModelIndex(); // Возвращается пустой указатель
}


// Получение индекса подчиненного элемента с указанным номером
// Нумерация элементов считается что идет с нуля
QModelIndex KnowTreeModel::index_children(const QModelIndex &parent, int n) const
{
    // Проверяется, передан ли правильный QModelIndex
    // Если он неправильный, возвращается пустой индекс
    if(!parent.isValid()) {
        qDebug() << "In indexChildren() unavailable model index";
        return QModelIndex();
    }

    // Выясняется указатель на основной Item элемент
    boost::intrusive_ptr<TreeItem> it = item(parent);

    // Если у основного Item элемента запрашивается индекс
    // несуществующего подэлемента, возвращается пустой индекс
    if(n < 0 || n > (it->child_count() - 1)) {
        qDebug() << "In indexChildren() unavailable children number";
        return QModelIndex();
    }

    // Выясняется указатель на подчиненный Item элемент
    boost::intrusive_ptr<TreeItem> childitem = it->child(n);

    // Если указатель на подчиненный Item элемент непустой
    if(childitem) {
        // return createIndex(0, 0, childitem); // Индекс подчиненного элемента
        // return createIndex(n, 0, parent.internalPointer());

        return index(n, 0, parent);
    } else {
        qDebug() << "In indexChildren() empty child element";
        return QModelIndex(); // Возвращается пустой индекс
    }
}


// Получение QModelIndex по известному TreeItem
QModelIndex KnowTreeModel::index_item(boost::intrusive_ptr<TreeItem> item)
{
    int itemrow = item->child_index();

    return this->createIndex(itemrow, 0, static_cast<void *>(item.get()));
}


// Возвращает общее количество записей, хранимых в дереве
int KnowTreeModel::get_all_record_count(void)
{
    // Обнуление счетчика
    get_all_record_count_recurse(_root_item, 0);

    return get_all_record_count_recurse(_root_item, 1);
}


// Возвращает количество записей в ветке и всех подветках
int KnowTreeModel::size_of(boost::intrusive_ptr<TreeItem> item)
{
    // Обнуление счетчика
    get_all_record_count_recurse(_root_item, 0);

    return get_all_record_count_recurse(item, 1);
}


int KnowTreeModel::get_all_record_count_recurse(boost::intrusive_ptr<TreeItem> item, int mode)
{
    static int n = 0;

    if(mode == 0) {
        n = 0;
        return 0;
    }

    n = n + item->row_count();

    for(int i = 0; i < item->child_count(); i++)
        get_all_record_count_recurse(item->child(i), 1);

    return n;
}


// Проверка наличия идентификатора ветки во всем дереве
bool KnowTreeModel::is_item_id_exists(QString findId)
{
    // Обнуление счетчика
    is_item_id_exists_recurse(_root_item, findId, 0);

    return is_item_id_exists_recurse(_root_item, findId, 1);
}


bool KnowTreeModel::is_item_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode)
{
    static bool isExists = false;

    // Инициализация
    if(mode == 0) {
        isExists = false;
        return false;
    }

    // Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
    if(isExists)
        return true;

    // Если текущая ветка содержит искомый идетнификатор
    if(item->field("id") == findId) {
        isExists = true;
        return true;
    }

    // Перебираются подветки
    for(int i = 0; i < item->child_count(); i++)
        is_item_id_exists_recurse(item->child(i), findId, 1);

    return isExists;
}


// Проверка наличия идентификатора записи во всем дереве
bool KnowTreeModel::is_record_id_exists(QString findId)
{
    // Обнуление счетчика
    is_record_id_exists_recurse(_root_item, findId, 0);

    return is_record_id_exists_recurse(_root_item, findId, 1);
}


bool KnowTreeModel::is_record_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode)
{
    static bool isExists = false;

    // Инициализация
    if(mode == 0) {
        isExists = false;
        return false;
    }

    // Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
    if(isExists)
        return true;

    // Если таблица записей текущей ветки содержит искомый идентификатор
    if(item->tabledata()->is_record_exists(findId)) {
        isExists = true;
        return true;
    }

    // Перебираются подветки
    for(int i = 0; i < item->child_count(); i++)
        is_record_id_exists_recurse(item->child(i), findId, 1);

    return isExists;
}



// Добавление подветки из буфера обмена относительно указанного элемента
// Функция возвращает новый идентификатор стартовой добавленной подветки
QString KnowTreeModel::paste_sub_branch(boost::intrusive_ptr<TreeItem> item, ClipboardBranch *subbranch)
{
    qDebug() << "In paste_subbranch()";

    // Выясняется линейная структура добавляемого дерева
    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->getIdTree();

    // Идентификатор стартовой ветки лежит в первом элементе списка
    QString start_branch_id = tree[0].branch_id;

    return paste_sub_branch_recurse(item, start_branch_id, subbranch);
}


// Рекурсивное добавление дерева
// item - элемент главного дерева, к которому добавляется ветка
// start_branch_id - идентификатор ветки в переданном линейном представлении
//                   добавляемого дерева
// subbranch - добавляемое дерево
QString KnowTreeModel::paste_sub_branch_recurse(boost::intrusive_ptr<TreeItem> item,
                                                QString startBranchId,
                                                ClipboardBranch *subbranch)
{
    qDebug() << "In paste_subbranch_recurse()";

    // ---------------------------
    // Добавляется стартовая ветка
    // ---------------------------

    // Выясняются поля стартовой ветки
    QMap<QString, QString> subbranch_fields = subbranch->getBranchFieldsById(startBranchId);

    // Выясняется имя ветки
    QString subbranch_name = subbranch_fields["name"];

    // Получение уникального идентификатора, под которым будет добавляться ветка
    QString id = get_unical_id();

    // Стартовая ветка добавляется
    add_new_branch(item, id, subbranch_name);

    // Выясняется указатель на эту добавленную ветку
    auto newitem = item_by_id(id);

    qDebug() << "KnowTreeModel::paste_subbranch_recurse() : create branch with field" << newitem->all_fields();

    // -----------------------------------------------
    // Для стартовой ветки добавляются конечные записи
    // -----------------------------------------------

    // Выясняются данные конечных записей
    QList< std::shared_ptr<Record> > records = subbranch->getBranchRecords(startBranchId);

    foreach(std::shared_ptr<Record> record, records) {
        qDebug() << "Add table record " + record->field("name");
        newitem->tabledata()->insert_new_record(0, record, ADD_NEW_RECORD_TO_END);
    }

    // --------------------
    // Добавляются подветки
    // --------------------

    // Выясняется линейная структура добавляемого дерева
    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->getIdTree();

    // Выясняется список подветок для стартовой ветки
    QStringList subbranch_list;

    foreach(CLIPB_TREE_ONE_LINE one_line, tree)
        if(one_line.branch_id == startBranchId)
            subbranch_list = one_line.subbranches_id;

    foreach(QString current_subbranch, subbranch_list)
        paste_sub_branch_recurse(newitem, current_subbranch, subbranch);

    return id;
}


// Перешифрование базы с новым паролем
void KnowTreeModel::re_encrypt(QString previousPassword, QString currentPassword)
{
    // Получение путей ко всем подветкам дерева
    QList<QStringList> subbranchespath = _root_item->all_children_path();

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
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
}

// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде TreeItem *
// Если ветка с указанным ID не будет найдена, возвращается NULL
boost::intrusive_ptr<TreeItem> KnowTreeModel::item_by_name(QString name)
{
    std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>, QString, int)> item_by_name_recurse    //    boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem> item, QString name, int mode);
    = [&](boost::intrusive_ptr<TreeItem> item, QString name, int mode) {
        static boost::intrusive_ptr<TreeItem> find_item;

        if(mode == 0) {
            find_item = boost::intrusive_ptr<TreeItem>(nullptr);
            return find_item;   // nullptr;
        }

        if(find_item) return find_item;

        if(item->name() == name) {
            find_item = item;
            return find_item;
        } else {
            for(int i = 0; i < item->child_count(); i++)
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
boost::intrusive_ptr<TreeItem> KnowTreeModel::item_by_id(QString id)
{
    // Initialize the search    // Инициализация поиска
    item_by_id_recurse(_root_item, "0", 0);

    // Запуск поиска и возврат результата
    return item_by_id_recurse(_root_item, id, 1);
}


boost::intrusive_ptr<TreeItem> KnowTreeModel::item_by_id_recurse(boost::intrusive_ptr<TreeItem> item, QString id, int mode)
{
    static boost::intrusive_ptr<TreeItem> find_item;

    if(mode == 0) {
        find_item = nullptr;
        return nullptr;
    }

    if(find_item != nullptr) return find_item;

    if(item->id() == id) {
        find_item = item;
        return find_item;
    } else {
        for(int i = 0; i < item->child_count(); i++)
            item_by_id_recurse(item->child(i), id, 1);

        return find_item;
    }
}


// Получение пути к ветке, где находится запись
QStringList KnowTreeModel::record_path(QString recordId)
{
    record_path_recurse(_root_item, QStringList(), "0", 0);

    return record_path_recurse(_root_item, QStringList(), recordId, 1);
}


QStringList KnowTreeModel::record_path_recurse(boost::intrusive_ptr<TreeItem> item,
                                               QStringList currentPath,
                                               QString recordId,
                                               int mode)
{
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
    if(item->tabledata()->is_record_exists(recordId)) {
        isFind = true;
        findPath = currentPath;
    } else {
        // Иначе перебираются подветки
        for(int i = 0; i < item->child_count(); i++)
            record_path_recurse(item->child(i), currentPath, recordId, 1);
    }

    return findPath;
}


// Метод определяющий есть ли в дереве зашифрованные ветки
bool KnowTreeModel::is_contains_crypt_branches(void)
{
    is_contains_crypt_branches_recurse(_root_item, 0);

    return is_contains_crypt_branches_recurse(_root_item, 1);
}


bool KnowTreeModel::is_contains_crypt_branches_recurse(boost::intrusive_ptr<TreeItem> item, int mode)
{
    static bool isCrypt = false;

    if(mode == 0) {
        isCrypt = false;
        return isCrypt;
    }

    if(item->field("crypt") == "1") {
        isCrypt = true;
        return isCrypt;
    }

    for(int i = 0; i < item->child_count(); i++)
        is_contains_crypt_branches_recurse(item->child(i), 1);

    return isCrypt;
}


// Старый вариант поиска QModelIndex по известному TreeItem закомментирован,
// но алгоритм может пригодиться для поиска других данных
/*
// Получение QModelIndex по известному TreeItem
QModelIndex knowtreemodel::get_item_index(TreeItem *item)
{
 // Выясняется начальный QModelIndex дерева
 QModelIndex rootindex=index( 0, 0 );

 // Очищается флаг поиска внутри элементов
 get_item_index_recurse(rootindex, item, 0);

 // Перебираются элементы на одном уровне вложения с начальным элементом дерева
 for(int i=0;rootindex.sibling(i,0).isValid();i++)
  {
   // qDebug() << "Sibling current " << (find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(rootindex.sibling(i,0),Qt::EditRole)).toString();

   // Перебираемый элемент проверяется на соответствие с искомым TreeItem
   if(item==static_cast<TreeItem*>(rootindex.sibling(i,0).internalPointer()))
    return rootindex.sibling(i,0);
   else
    {
     // Производится поиск внутри элемента
     QModelIndex idx=get_item_index_recurse(rootindex.sibling(i,0), item, 1);

     // Если был найден элемент
     if(idx.isValid())return idx;
    }
  }

 // Если ничего небыло найдено, возвращается пустой индекс
 return QModelIndex();
}


QModelIndex knowtreemodel::get_item_index_recurse(QModelIndex currindex, TreeItem *finditem, int mode)
{
 static QModelIndex findindex;
 static int findflag=0;

 // Из QModelIndex можно всегда получить указатель TreeItem,
 // поэтому поиск можно вести по QModelIndex

 // Инициализация поиска
 if(mode==0)
  {
   findflag=0;
   return QModelIndex();
  }

 // qDebug() << "Recurse current " << (find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(currindex,Qt::EditRole)).toString();
 // qDebug() << "Current index have " << currindex.row() << "row";
 // qDebug() << "Find flag " << findflag;

 // Если был найден QModelIndex
 if(findflag==1)return findindex;

 for(int i=0;currindex.child(i,0).isValid();i++)
  {
   // Проверяется текущий элемент, не соответствует ли
   // его QModelIndex искомому TreeItem
   if(findflag==0 &&
      finditem==static_cast<TreeItem*>(currindex.child(i,0).internalPointer()))
    {
     findflag=1;
     findindex=currindex.child(i,0);
     return findindex;
    }

   // Рекурсивный вызов поиска в глубину дерева
   get_item_index_recurse(currindex.child(i,0), finditem, 1);

   // Если был найден QModelIndex
   if(findflag==1)return findindex;
  }

 // Сюда код доходит если на текущем уровне поиска элемент еще не найден
 return QModelIndex();
}
*/
