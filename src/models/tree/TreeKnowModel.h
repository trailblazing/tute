#ifndef __TREEKNOWMODEL_H__ // __TREENODE_H__
#define __TREEKNOWMODEL_H__ // __TREENODE_H__

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>

#include "TreeModel.h"

// KnowTreeModel - класс с удобными методами для управления данными дерева
// Внимание! Данный класс наследуется от класса TreeModel

class XmlTree;
class ClipboardBranch;

class TreeKnowModel : public TreeModel {
    Q_OBJECT

public:
    TreeKnowModel(QObject *parent = 0); // KnowTreeModel(const QStringList &headers, QDomDocument domModel, QObject *parent = 0);
    ~TreeKnowModel();

    std::shared_ptr<XmlTree> init_from_xml(QString file_name);
    std::shared_ptr<XmlTree> init_from_xml(std::shared_ptr<XmlTree> xmlt);
    void reload(void);

    QDomElement dom_from_record(boost::intrusive_ptr<TreeItem> root_item);

    void save(void);

    // Добавление новой подветки к указанной ветке
    void add_child_branch(const QModelIndex &_index, QString id, QString name);
    void add_child_branch(const QModelIndex &_index, boost::intrusive_ptr<TreeItem> it);
    // Добавление новой ветки после указанной ветки
    void add_sibling_branch(const QModelIndex &_index, QString id, QString name);
    void add_sibling_branch(const QModelIndex &_index, boost::intrusive_ptr<TreeItem> it);

    // Добавление новой подветки к Item элементу
    boost::intrusive_ptr<TreeItem> add_new_branch(boost::intrusive_ptr<TreeItem> item, boost::intrusive_ptr<TreeItem> parent);
    boost::intrusive_ptr<TreeItem> add_new_branch(boost::intrusive_ptr<TreeItem> parent, QString id, QString name);
    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record> record, boost::intrusive_ptr<TreeItem> parent);
    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<TreeItem> item, boost::intrusive_ptr<TreeItem> parent);

    // Перемещение ветки вверх и вниз
    QModelIndex move_up_branch(const QModelIndex &_index);
    QModelIndex move_dn_branch(const QModelIndex &_index);

    // Получение индекса подчиненного элемента с указанным номером
    QModelIndex index_child(const QModelIndex &parent, int n) const;

    //    QModelIndex index(boost::intrusive_ptr<TreeItem> item);

    // Поиск ветки с указанным ID
    boost::intrusive_ptr<TreeItem> item_by_id(QString id);
    boost::intrusive_ptr<TreeItem> item_by_name(QString name);

    // Возвращает общее количество записей, хранимых в дереве
    int get_all_record_count(void);

    // Возвращает количество записей в ветке и всех подветках
    int size_of(boost::intrusive_ptr<TreeItem> item);

    QString paste_child_branch(const QModelIndex &_index, ClipboardBranch *subbranch);
    QString paste_sibling_branch(const QModelIndex &_index, ClipboardBranch *subbranch);

    void re_encrypt(QString previousPassword, QString currentPassword);

    bool is_contains_crypt_branches(void) const;

    QStringList record_path(QString recordId) const;

    // Проверка наличия идентификатора ветки во всем дереве
    bool is_item_id_exists(QString findId);

    // Проверка наличия идентификатора записи во всем дереве
    bool is_item_id_exists(QString findId) const;

#ifdef _with_record_table
    void record_to_item();
#endif

    void clear();
    void intercept(boost::intrusive_ptr<TreeItem> item);

private:


    void init(QDomDocument *dom_model);

    // Функция заполнения дерева из DOM-документа
    void setup_modeldata(QDomDocument *dommodel, boost::intrusive_ptr<TreeItem> self);

    // Преобразование DOM в Item модель. Функция рекурсивного обхода элементов DOM-документа
    void dom_to_record(QDomElement _record_dom_element, boost::intrusive_ptr<TreeItem> self);

    // Преобразование Item в DOM модель
    void dom_from_record(std::shared_ptr<QDomDocument> doc, QDomElement &xml_data, boost::intrusive_ptr<TreeItem> curr_item);

    // Перемещение ветки вверх или вниз
    QModelIndex move_up_dn_branch(const QModelIndex &_index, int direction);

    //    int get_all_record_count_recurse(boost::intrusive_ptr<TreeItem> item, int mode);

    //    QString paste_sub_branch_recurse(boost::intrusive_ptr<TreeItem> item, QString startBranchId, ClipboardBranch *subbranch);

    //    boost::intrusive_ptr<TreeItem> item_by_id_recurse(boost::intrusive_ptr<TreeItem> item, QString id, int mode);

    //    bool is_contains_crypt_branches_recurse(boost::intrusive_ptr<TreeItem> item, int mode);

    // Добавление подветки из буфера обмена относительно указанного элемента
    QString paste_sub_branch(boost::intrusive_ptr<TreeItem> item, ClipboardBranch *subbranch);

    //    QStringList record_path_recurse(boost::intrusive_ptr<TreeItem> item, QStringList currentPath, QString recordId, int mode);

    //    bool is_item_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode);

    //    bool is_record_id_exists_recurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode);

    bool check_format(QDomElement elementFormat);

    bool update_sub_version_from_1_to_2(void);

    // QModelIndex get_item_index_recurse(QModelIndex currindex, TreeItem *finditem, int mode);
    QString _xml_file_name = "";

    std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
    is_item_id_exists_recurse =
        [&](boost::intrusive_ptr<TreeItem> item, QString id_to_find, int mode)
    {
        static bool is_exists = false;

        // Инициализация
        if(mode == 0) {
            return is_exists = false;
            //            return false;
        }

        // Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет возврат из рекурсии.
        if(is_exists)
            return true;

        // Если текущая ветка содержит искомый идетнификатор
        if(item->field("id") == id_to_find) {
            return is_exists = true;
            //            return true;
        }

        // Перебираются подветки
        for(int i = 0; i < item->current_count(); i++)
            is_item_id_exists_recurse(item->child(i), id_to_find, 1);

        return is_exists;
    };

};

#endif // __TREEKNOWMODEL_H__   // __TREENODE_H__
