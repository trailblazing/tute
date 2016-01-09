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

    void initFromXML(QString fileName);
    void reload(void);

    QDomElement export_to_dom(boost::intrusive_ptr<TreeItem> root);

    void save(void);

    // Добавление новой подветки к указанной ветке
    void addNewChildBranch(const QModelIndex &index, QString id, QString name);

    // Добавление новой ветки после указанной ветки
    void addNewSiblingBranch(const QModelIndex &index, QString id, QString name);

    // Добавление новой подветки к Item элементу
    void addNewBranch(boost::intrusive_ptr<TreeItem> parent, QString id, QString name);

    // Перемещение ветки вверх и вниз
    QModelIndex moveUpBranch(const QModelIndex &index);
    QModelIndex moveDnBranch(const QModelIndex &index);

    // Получение индекса подчиненного элемента с указанным номером
    QModelIndex indexChildren(const QModelIndex &parent, int n) const;

    QModelIndex index_item(boost::intrusive_ptr<TreeItem> item);

    // Поиск ветки с указанным ID
    boost::intrusive_ptr<TreeItem> getItemById(QString id);

    // Возвращает общее количество записей, хранимых в дереве
    int getAllRecordCount(void);

    // Возвращает количество записей в ветке и всех подветках
    int size_of(boost::intrusive_ptr<TreeItem> item);

    QString pasteNewChildBranch(const QModelIndex &index, ClipboardBranch *subbranch);
    QString pasteNewSiblingBranch(const QModelIndex &index, ClipboardBranch *subbranch);

    void reEncrypt(QString previousPassword, QString currentPassword);

    bool isContainsCryptBranches(void);

    QStringList getRecordPath(QString recordId);

    // Проверка наличия идентификатора ветки во всем дереве
    bool isItemIdExists(QString findId);

    // Проверка наличия идентификатора записи во всем дереве
    bool isRecordIdExists(QString findId);

private:

    QString xmlFileName = "";

    void init(QDomDocument *domModel);

    // Функция заполнения дерева из DOM-документа
    void setup_modeldata(QDomDocument *dommodel, boost::intrusive_ptr<TreeItem> parent);

    // Преобразование DOM в Item модель. Функция рекурсивного обхода элементов DOM-документа
    void node_from_dom(QDomElement n, boost::intrusive_ptr<TreeItem> parent);

    // Преобразование Item в DOM модель
    void export_to_dom(std::shared_ptr<QDomDocument> doc, QDomElement &xml_data, boost::intrusive_ptr<TreeItem> curr_item);

    // Перемещение ветки вверх или вниз
    QModelIndex moveUpDnBranch(const QModelIndex &index, int direction);

    int getAllRecordCountRecurse(boost::intrusive_ptr<TreeItem> item, int mode);

    QString pasteSubbranchRecurse(boost::intrusive_ptr<TreeItem> item,
                                  QString startBranchId,
                                  ClipboardBranch *subbranch);

    boost::intrusive_ptr<TreeItem> getItemByIdRecurse(boost::intrusive_ptr<TreeItem> item, QString id, int mode);

    bool isContainsCryptBranchesRecurse(boost::intrusive_ptr<TreeItem> item, int mode);

    // Добавление подветки из буфера обмена относительно указанного элемента
    QString pasteSubbranch(boost::intrusive_ptr<TreeItem> item, ClipboardBranch *subbranch);

    QStringList getRecordPathRecurse(boost::intrusive_ptr<TreeItem> item,
                                     QStringList currentPath,
                                     QString recordId,
                                     int mode);

    bool isItemIdExistsRecurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode);

    bool isRecordIdExistsRecurse(boost::intrusive_ptr<TreeItem> item, QString findId, int mode);

    bool checkFormat(QDomElement elementFormat);

    bool updateSubVersionFrom1To2(void);

    // QModelIndex get_item_index_recurse(QModelIndex currindex, TreeItem *finditem, int mode);
};

#endif // __TREENODE_H__
