#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "models/tree/TreeItem.h"
#include "ClipboardBranch.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "models/tree/KnowModel.h"

extern GlobalParameters globalparameters;


ClipboardBranch::ClipboardBranch(void) : QMimeData()
{
    init();
}


ClipboardBranch::~ClipboardBranch(void)
{

}


void ClipboardBranch::init(void)
{
    _branch_data._branch.clear();
    _branch_data._records.clear();

    _clipboard_branch_format.clear();
    _clipboard_branch_format << "mytetra/branch";
}


void ClipboardBranch::print(void) const
{
    // Перебираются ветки
    typedef QMap<QString, QString> branch_type;

    foreach(branch_type current_branch, _branch_data._branch) {
        // Перебираются и печатаются значения полей текущей ветки
        qDebug() << "Branch:";

        foreach(QString field_name, current_branch.keys()) {
            if(field_name == "id" || field_name == "name") {
                qDebug() << field_name << ":" << current_branch.value(field_name);
            }
        }

        // Запоминается id текущей ветки
        QString branch_id = current_branch.value("id");

        // Находятся все записи, принадлежащие текущей ветке
        foreach(boost::intrusive_ptr<TreeItem> current_record, _branch_data._records.values(branch_id)) {
            qDebug() << "Record:";

            QMap<QString, QString> current_record_fields = current_record->natural_field_list();

            foreach(QString field_name, current_record_fields.keys()) {
                if(field_name == "id" || field_name == "name") {
                    qDebug() << field_name << ":" << current_record_fields.value(field_name);
                }
            }
        }
    }
}


// Получение идентификторов веток в виде, описывающим дерево
// Формат строк списка:
// ID узла 1, список подчиненных узлов
// ID узла 2, список подчиненных узлов и т.д.
QList<CLIPB_TREE_ONE_LINE> ClipboardBranch::id_tree_get(void) const
{
    QList<CLIPB_TREE_ONE_LINE> tree;

    // Перебираются строки с ветками
    typedef QMap<QString, QString> branch_type;

    foreach(branch_type current_branch, _branch_data._branch) {
        QString current_id = current_branch.value("id");

        // Составляется список подветок
        QStringList subbranches;

        foreach(branch_type current_subbranch, _branch_data._branch) {
            if(current_subbranch.value("parent_id") == current_id) {
                subbranches << current_subbranch.value("id");
            }
        }

        CLIPB_TREE_ONE_LINE one_line;
        one_line._branch_id = current_id;
        one_line._subbranches_id = subbranches;
        tree << one_line;
    }

    return tree;
}


void ClipboardBranch::id_tree_print(void) const
{
    QList<CLIPB_TREE_ONE_LINE> tree = id_tree_get();

    foreach(CLIPB_TREE_ONE_LINE one_line, tree)
        qDebug() << one_line._branch_id << one_line._subbranches_id;
}


// Добавление ветки
void ClipboardBranch::branch_fields_add_parent_id(QString parent_id, QMap<QString, QString> _branch_fields)
{
    QMap<QString, QString> line;

    line["parent_id"] = parent_id;
    line.unite(_branch_fields);

    _branch_data._branch << line;
}


// Добавление конечной записи
void ClipboardBranch::record_duplicate_to_parent_id(QString parent_id, boost::intrusive_ptr<TreeItem> record)
{
    // todo: Сделать проверку, есть ли ветка с указанным id
    _branch_data._records.insert(parent_id, record);
}


// Получение полей для указанной ветки
QMap<QString, QString> ClipboardBranch::fields_by_branch_id(QString id)const
{
    //    // Перебираются ветки чтобы найти ветку с нужным идентификатором
    //    typedef QMap<QString, QString> branch_type;
    QMap<QString, QString> result;

    for(auto &current_branch : _branch_data._branch) { //foreach(branch_type current_branch, _branch_data._branch) {
        if(current_branch.contains("id")) {
            if(current_branch["id"] == id) {
                result = current_branch;
            }
        }
    }

    //    critical_error("Can not find id " + id + " in clipboard data");
    //    exit(1);
    return result;  //QMap<QString, QString>();
}


// Получение списка записей для указанной ветки
QList< boost::intrusive_ptr<TreeItem>> ClipboardBranch::records_by_branch_id(QString id)const
{
    //    QList< boost::intrusive_ptr<TreeItem> > records;

    //    // Находятся записи с нужным идентификатором
    //    // Записи добавляются в records в последовательности задом-наперёд
    //    // из-за особенностей реализации foreach для QMultiMap
    //    foreach(boost::intrusive_ptr<TreeItem> current_record, _branch_data._records.values(id))
    //        records.insert(0, current_record);

    return _branch_data._records.values(id);    // records;
}


QStringList ClipboardBranch::formats() const
{
    return _clipboard_branch_format;
}


QVariant ClipboardBranch::retrieveData(const QString &format, QVariant::Type preferredType) const
{
    Q_UNUSED(preferredType);

    if(format == _clipboard_branch_format[0]) {
        QVariant v;
        v.setValue(_branch_data);
        return v;
    }

    return 0;
}

void ClipboardBranch::branch_push(QList<boost::intrusive_ptr<TreeItem>> current_items)
{
    if(current_items.size() > 0) {
        _clip_root_item_parent_id = current_items[0]->parent_id();

        for(auto i : current_items) {
            assert(_clip_root_item_parent_id == i->parent_id());
            branch_push(i);

            //            if(_clip_root_item_parent_id != i->parent_id()) {
            //                current_items[0]->child_move(current_items[0]->count_direct(), i);
            //            }
        }
    }
}

void ClipboardBranch::branch_push(boost::intrusive_ptr<TreeItem> current_item)  //QStringList path
{
    std::function<void (boost::intrusive_ptr<TreeItem>)>branch_copy_recursive = [&](boost::intrusive_ptr<TreeItem> it) {
        branch_fields_add_parent_id(it->parent_id(), it->fields_all());
        record_duplicate_to_parent_id(it->parent_id(), it);

        for(int i = 0; i < it->count_direct(); i++) {
            assert(it->item_fat(i)->parent_id() == it->id());
            branch_copy_recursive(it->item_fat(i));
        }
    };

    //    // boost::intrusive_ptr<TreeItem> curr_item;
    //    // QMap<QString, QString> curr_item_fields;
    //    // QString branch_id;
    //    //    std::shared_ptr<RecordTable> curr_item_record_table;
    //    auto _know_model_board = globalparameters.tree_screen()->know_model_board();
    //    // Добавление ветки
    //    boost::intrusive_ptr<TreeItem> current_item = _know_model_board->item(path);
    //    QMap<QString, QString> current_item_fields = current_item->fields_all(); // Раньше было getAllFieldsDirect()
    _clip_root_item_parent_id = current_item->parent_id();    // current_item_fields["id"];

    //    //        if(is_root)
    //    //            branch_clipboard_data->branch_add(curr_item->parent_id(), curr_item_fields);
    //    //        else
    //    branch_fields(current_item->parent_id(), current_item_fields);
    //    record_duplicate(current_item->parent_id(), current_item);
    //    // Добавление конечных записей
    //    //    auto curr_item_record_table = curr_item->record_table();

    //    for(int i = 0; i < current_item->count_direct(); i++) {
    //        // Полный образ записи (с файлами и текстом)
    //        boost::intrusive_ptr<TreeItem> record = current_item->item_fat(i);
    //        branch_fields(_root_id, record->fields_all());
    //        record_duplicate(_root_id, record);
    //    }
    branch_copy_recursive(current_item);
}




