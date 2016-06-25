#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "models/tree/tree_item.h"
#include "clipboard_branch.h"
#include "libraries/global_parameters.h"
#include "views/tree/tree_screen.h"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"


extern GlobalParameters globalparameters;
const char *clipboard_items_root = "clipboard_items_root";

ClipboardBranch::ClipboardBranch(void) : QMimeData(){
    init();
}
ClipboardBranch::~ClipboardBranch(void)
{}
void ClipboardBranch::init(void){
    _branch_data._branch.clear();
    _branch_data._records.clear();

    _clipboard_branch_format.clear();
    _clipboard_branch_format << "mytetra/branch";
}
void ClipboardBranch::print(void) const {
        // Перебираются ветки
    typedef QMap<QString, QString> branch_type;

    foreach(branch_type current_branch, _branch_data._branch){
        // Перебираются и печатаются значения полей текущей ветки
        qDebug() << "Branch:";

        foreach(QString field_name, current_branch.keys()){
            if(field_name == "id" || field_name == "name"){
                qDebug() << field_name << ":" << current_branch.value(field_name);
            }
        }

        // Запоминается id текущей ветки
        QString branch_id = current_branch.value("id");

        // Находятся все записи, принадлежащие текущей ветке
        foreach(	// boost::intrusive_ptr<TreeItem>
            QDomElement current_record_dom, _branch_data._records.values(branch_id)){
            qDebug() << "Record:";
            boost::intrusive_ptr<Record> record = new Record();
            record->dom_to_record(current_record_dom);
            QMap<QString, QString> current_record_fields = record->natural_field_list();

            foreach(QString field_name, current_record_fields.keys()){
                if(field_name == "id" || field_name == "name"){
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
QList<CLIPB_TREE_ONE_LINE> ClipboardBranch::id_tree_get(void) const {
    QList<CLIPB_TREE_ONE_LINE> tree;

        // Перебираются строки с ветками
    typedef QMap<QString, QString> branch_type;

    foreach(branch_type current_branch, _branch_data._branch){
        QString current_id = current_branch.value("id");

        // Составляется список подветок
        QStringList subbranches;

        foreach(branch_type current_subbranch, _branch_data._branch){
            if(current_subbranch.value("parent_id") == current_id){
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
void ClipboardBranch::id_tree_print(void) const {
    QList<CLIPB_TREE_ONE_LINE> tree = id_tree_get();

    foreach(CLIPB_TREE_ONE_LINE one_line, tree)
    qDebug() << one_line._branch_id << one_line._subbranches_id;
}
//// Добавление ветки
// void ClipboardBranch::branch_fields_add_parent_id(boost::intrusive_ptr<TreeItem> it)
// {
//    QString parent_id = it->parent_id();
//    QMap<QString, QString> _branch_fields = it->fields_all();

//    QMap<QString, QString> line;

//    line["parent_id"] = parent_id;
//    line.unite(_branch_fields);

//    _branch_data._branch << line;
// }


//// Добавление конечной записи
// void ClipboardBranch::record_duplicate_to_parent_id(boost::intrusive_ptr<TreeItem> record)
// {
//    // todo: Сделать проверку, есть ли ветка с указанным id
//    _branch_data._records.insert(record->parent_id(), record);
// }


// Получение полей для указанной ветки
QMap<QString, QString> ClipboardBranch::fields_by_parent_id(QString id) const {
        //    // Перебираются ветки чтобы найти ветку с нужным идентификатором
        //    typedef QMap<QString, QString> branch_type;
    QMap<QString, QString> result;
    for(auto &current_branch : _branch_data._branch){	// foreach(branch_type current_branch, _branch_data._branch) {
        if(current_branch.contains("id")){
            if(current_branch["id"] == id){
                result = current_branch;
            }
        }
    }
        //    critical_error("Can not find id " + id + " in clipboard data");
        //    exit(1);
    return result;	// QMap<QString, QString>();
}
// Получение списка записей для указанной ветки
QList<boost::intrusive_ptr<TreeItem> > ClipboardBranch::records_by_parent_id(QString id) const {
    QList< boost::intrusive_ptr<TreeItem> > records;

        //    // Находятся записи с нужным идентификатором
        //    // Записи добавляются в records в последовательности задом-наперёд
        //    // из-за особенностей реализации foreach для QMultiMap
        //    foreach(boost::intrusive_ptr<TreeItem> current_record, _branch_data._records.values(id))
        //        records.insert(0, current_record);
    auto dom_list = _branch_data._records.values(id);
    for(auto dom : dom_list){
        boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));	// new TreeItem(nullptr)
        it->dom_to_record(dom);
        records << it;
    }
    return records;
}
//// Получение списка записей для указанной ветки
// QList< boost::intrusive_ptr<TreeItem>> ClipboardBranch::records_by_parent_id_list(std::set<QString> id_list)const
// {
//    QList< boost::intrusive_ptr<TreeItem> > records;

//    //    // Находятся записи с нужным идентификатором
//    //    // Записи добавляются в records в последовательности задом-наперёд
//    //    // из-за особенностей реализации foreach для QMultiMap
//    //    foreach(boost::intrusive_ptr<TreeItem> current_record, _branch_data._records.values(id))
//    for(auto id : id_list) {
//        for(auto record : _branch_data._records.values(id)) {
//            records << record;
//        }
//    }

//    //        records.insert(0, current_record);

//    return // _branch_data._records.values(id);    //
//        records;
// }



QStringList ClipboardBranch::formats() const {
    return _clipboard_branch_format;
}
QVariant ClipboardBranch::retrieveData(const QString &format, QVariant::Type preferredType) const {
    Q_UNUSED(preferredType);
    if(format == _clipboard_branch_format[0]){
        QVariant v;
        v.setValue(_branch_data);
        return v;
    }
    return 0;
}
void ClipboardBranch::branch_push(boost::intrusive_ptr<TreeIndex> _modelindex	// , const QString &_clip_blank_header_id
                                 , QList<boost::intrusive_ptr<TreeItem> > current_items){

        //    auto _current_model     = _modelindex.current_model();
    auto _current_parent = _modelindex->host_parent();
        //    auto _current_index     = _modelindex.current_index();  // _current_model()->index(_modelindex.parent());

    this->_clip_blank_header_id = get_unical_id();	// _clip_blank_header_id;
    if(current_items.size() > 0){
        //        _clip_root_items_id_list = current_items[0]->parent_id();
        for(auto item : current_items){
                //            if(_clip_root_items_parent_id_list.find(item->parent_id()) != _clip_root_items_parent_id_list.end())
                //                _clip_root_items_parent_id_list.insert(item->parent_id());


                //            // Получение ссылки на узел, который соответствует выделенной ветке
                //            auto item = _source_model->item(_index_current);

                //            //        // Получение пути к выделенной ветке
                //            //        QStringList _current_item_absolute_path = item->path_absolute();

                //            //        // Получение путей ко всем подветкам
                //            //        QList<QStringList> sub_branches_absolute_path = item->path_children_all();


                // Проверка, содержит ли данная ветка как шифрованные
                // так и незашифрованные данные
            bool nocrypt_presence = false;
            bool encrypt_presence = false;
                // Флаги на основе состояния текущей ветки
            if(_current_parent->field<crypt_type>() == "1")	// boost::mpl::c_str < crypt_type > ::value //_current_item_absolute_path
                        encrypt_presence = true;
            else nocrypt_presence = true;
                // Флаги на основе состояния подветок
            for(int i = 0; i < item->count_direct(); i ++){	// foreach(QStringList curr_absolute_path, sub_branches_absolute_path)
                if(	// _know_model_board->item(curr_absolute_path)
                    item->child_direct(i)->field<crypt_type>() == "1"){	// boost::mpl::c_str < crypt_type > ::value
                    encrypt_presence = true;
                }else{
                    nocrypt_presence = true;
                }
            }
                // Если ветка содержит как шифрованные так и нешифрованные данные
                // то такую ветку копировать в буфер нельзя
            if(nocrypt_presence == true && encrypt_presence == true){
                //                QMessageBox messageBox(this);
                //                messageBox.setWindowTitle(tr("Unavailable action"));
                //                messageBox.setText(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."));
                //                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
                //                messageBox.exec();
                globalparameters.status_bar()->showMessage(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."), 2000);
                break;	// return copy_result;
            }
                //            // -------------------
                //            // Копирование в буфер
                //            // -------------------

                //            qDebug() << "Branch copy to buffer";

                //            //            // Создается ссылка на буфер обмена
                //            //            QClipboard *cbuf = QApplication::clipboard();

                //            // Создается объект с данными для заполнения буфера обмена
                //            // Если в буфере есть какие-то старые данные, они удаляются
                //            static int fillflag = 0;

                //            if(fillflag == 1) {
                //                const ClipboardBranch *branch_clipboard_data_previos;
                //                branch_clipboard_data_previos = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());

                //                if(branch_clipboard_data_previos != nullptr)delete branch_clipboard_data_previos;

                //                fillflag = 0;
                //            }

                //            ClipboardBranch *branch_clipboard_data = new ClipboardBranch();

                //            fillflag = 1;








                //            assert(_clip_root_items_id_list == i->parent_id());

                //            _clip_root_items_id_list.push_back(i->id());

            branch_push(_clip_blank_header_id, item);

                //            if(_clip_root_item_parent_id != i->parent_id()) {
                //                current_items[0]->child_move(current_items[0]->count_direct(), i);
                //            }
        }
    }
}
void ClipboardBranch::branch_push(const QString &_clip_parent_id, boost::intrusive_ptr<TreeItem> current_item){	// QStringList path

        // Добавление ветки
    auto branch_fields_add_parent_id = [&](const QString &_clip_parent_id, boost::intrusive_ptr<TreeItem> it) -> void {
            QString parent_id = _clip_parent_id;	// it->parent_id();
            QMap<QString, QString> _branch_fields = it->fields_all();

            QMap<QString, QString> line;

            line["parent_id"] = parent_id;
            line.unite(_branch_fields);

            _branch_data._branch << line;
        };


        // Добавление конечной записи
    auto record_duplicate_to_parent_id = [&](const QString &_clip_parent_id, boost::intrusive_ptr<TreeItem> record) -> void {
                // todo: Сделать проверку, есть ли ветка с указанным id
            _branch_data._records.insert(_clip_parent_id, record->dom_from_record());	// record->parent_id()
        };


    int global_items_count = 0;

    std::function<void (const QString &_clip_parent_id, boost::intrusive_ptr<TreeItem>)>
    branch_copy_recursive = [&](const QString &_clip_parent_id, boost::intrusive_ptr<TreeItem> it) -> void {
            branch_fields_add_parent_id(_clip_parent_id, it);
            record_duplicate_to_parent_id(_clip_parent_id, it);
            global_items_count ++;
            for(int i = it->count_direct() - 1; i >= 0; i --){
                auto child_it = it->item_fat(i);
                assert(child_it->parent_id() == it->id());
                branch_copy_recursive(it->id(), child_it);
            }
        };

        //    //    // boost::intrusive_ptr<TreeItem> curr_item;
        //    //    // QMap<QString, QString> curr_item_fields;
        //    //    // QString branch_id;
        //    //    //    std::shared_ptr<RecordTable> curr_item_record_table;
        //    //    auto _know_model_board = globalparameters.tree_screen()->know_model_board();
        //    //    // Добавление ветки
        //    //    boost::intrusive_ptr<TreeItem> current_item = _know_model_board->item(path);
        //    //    QMap<QString, QString> current_item_fields = current_item->fields_all(); // Раньше было getAllFieldsDirect()



        //    if(_clip_blank_header_id.find(current_item->parent_id()) == _clip_blank_header_id.end())
        //        _clip_blank_header_id.insert(current_item->parent_id());



        //    //    _clip_root_items_id_list.push_back(current_item->id()); // = current_item->parent_id();    // current_item_fields["id"];


        //    //    //        if(is_root)
        //    //    //            branch_clipboard_data->branch_add(curr_item->parent_id(), curr_item_fields);
        //    //    //        else
        //    //    branch_fields(current_item->parent_id(), current_item_fields);
        //    //    record_duplicate(current_item->parent_id(), current_item);
        //    //    // Добавление конечных записей
        //    //    //    auto curr_item_record_table = curr_item->record_table();

        //    //    for(int i = 0; i < current_item->count_direct(); i++) {
        //    //        // Полный образ записи (с файлами и текстом)
        //    //        boost::intrusive_ptr<TreeItem> record = current_item->item_fat(i);
        //    //        branch_fields(_root_id, record->fields_all());
        //    //        record_duplicate(_root_id, record);
        //    //    }

    branch_copy_recursive(_clip_parent_id, current_item);
    assert(global_items_count == current_item->count_children_all() + 1);
}
