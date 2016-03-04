#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <memory>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include "TreeItem.h"

// TreeModel - Это вспомогательный класс! От него наследуется KnowTreeModel

class TreeItem;

class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    struct  delegater {
        boost::intrusive_ptr<TreeItem> _item;
        QUrl _find_url;
        QString _id = "";
        delegater(boost::intrusive_ptr<TreeItem> _item): _item(_item) {_equal = [&](TreeItem * it) {return _item.get() == it;};}
        delegater(const QUrl &_find_url):  _find_url(_find_url) {_equal = [&](TreeItem * it) {return _find_url.toString() == it->field("url");};}
        delegater(const QString &_id): _id(_id) {_equal = [&](TreeItem * it) {return _id == it->field("id");};}

        //        bool (*equal)(TreeItem * it);
        std::function<bool(TreeItem *)> _equal;

    };


    TreeModel(QObject *parent = 0);
    TreeModel(boost::intrusive_ptr<TreeItem> _root_item, QObject *parent = 0);
    ~TreeModel(void);

    QVariant data(const QModelIndex &_index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &current_index = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &_index) const;

    int rowCount(const QModelIndex &_index = QModelIndex()) const;
    int columnCount(const QModelIndex &itemIndex = QModelIndex()) const;


    Qt::ItemFlags flags(const QModelIndex &_index) const;
    bool setData(const QModelIndex &_index, const QVariant &value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());


    bool is_item_valid(QStringList path) const;

    QModelIndex index(delegater _del) const;
    QModelIndex index(boost::intrusive_ptr<TreeItem> _item) const;
    //    QModelIndex index(const QUrl &find_url)const;
    //    QModelIndex index(const QString &id)const;

    // Возвращение указателя на Item-элемент с указанным index
    // Где index - это индекс объекта в терминах структуры модель-вид
    boost::intrusive_ptr<TreeItem> item(const QModelIndex &_index) const;
    // Возвращение указателя на Item-элемент с указанным путем
    // в виде последовательности идентификаторов
    boost::intrusive_ptr<TreeItem> item(QStringList path) const;

    boost::intrusive_ptr<TreeItem> item(const delegater &_del)const;
    //    boost::intrusive_ptr<TreeItem> find_recursive(const QUrl &find_url) const;
    //    boost::intrusive_ptr<TreeItem> find_recursive(const QString &id)const;

    void emit_datachanged_signal(const QModelIndex &_index);

    // void root_item(boost::intrusive_ptr<TreeItem> it) {_root_item = it;}
    boost::intrusive_ptr<TreeItem> root_item()const {return _root_item;}

protected:
    boost::intrusive_ptr<TreeItem>  _root_item; // Ссылка на первый (корневой) item-объект

private:

    //    QModelIndex index_recursive(QModelIndex _index, boost::intrusive_ptr<TreeItem> item, int mode);

    // Element over which the cursor will carry. Used in the Drag And Drop.    // Элемент, над которым проносят курсор. Используется при Drag And Drop.
    QModelIndex                     _cursor_over_index;
};

#endif
