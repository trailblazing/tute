#ifndef __TREENODE_H__
#define __TREENODE_H__

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>

#include "TreeModel.h"

// KnowTreeModel - класс с удобными методами для управления данными дерева
// Внимание! Данный класс наследуется от класса TreeModel


class ClipboardBranch;

class KnowTreeModel : public TreeModel {
    Q_OBJECT

public:
    KnowTreeModel(QObject *parent = 0); // KnowTreeModel(const QStringList &headers, QDomDocument domModel, QObject *parent = 0);
    ~KnowTreeModel();

    void init_from_xml(QString fileName);
    void reload(void);

    QDomElement export_to_dom(boost::intrusive_ptr<TreeItem> root);

    void save(void);

    // Добавление новой подветки к указанной ветке
    void add_new_child_branch(const QModelIndex &_index, QString id, QString name);

    // Добавление новой ветки после указанной ветки
    void add_new_sibling_branch(const QModelIndex &_index, QString id, QString name);

    // Добавление новой подветки к Item элементу
    boost::intrusive_ptr<TreeItem> add_new_branch(boost::intrusive_ptr<TreeItem> parent, QString id, QString name);
    boost::intrusive_ptr<TreeItem> add_child(boost::intrusive_ptr<Record> record, boost::intrusive_ptr<TreeItem> parent);   //        , QString id, QString name
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

    QString paste_new_child_branch(const QModelIndex &_index, ClipboardBranch *subbranch);
    QString paste_new_sibling_branch(const QModelIndex &_index, ClipboardBranch *subbranch);

    void re_encrypt(QString previousPassword, QString currentPassword);

    bool is_contains_crypt_branches(void);

    QStringList record_path(QString recordId);

    // Проверка наличия идентификатора ветки во всем дереве
    bool is_item_id_exists(QString findId);

    // Проверка наличия идентификатора записи во всем дереве
    bool is_record_id_exists(QString findId);
    void record_to_item();
private:


    void init(QDomDocument *domModel);

    // Функция заполнения дерева из DOM-документа
    void setup_modeldata(QDomDocument *dommodel, boost::intrusive_ptr<TreeItem> parent);

    // Преобразование DOM в Item модель. Функция рекурсивного обхода элементов DOM-документа
    void node_from_dom(QDomElement n, boost::intrusive_ptr<TreeItem> parent);

    // Преобразование Item в DOM модель
    void export_to_dom(std::shared_ptr<QDomDocument> doc, QDomElement &xml_data, boost::intrusive_ptr<TreeItem> curr_item);

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

};

#endif // __TREENODE_H__
