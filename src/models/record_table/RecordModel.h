#ifndef __RECORDTABLEMODEL_H__
#define __RECORDTABLEMODEL_H__

#include <memory>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>


#include "models/tree/TreeItem.h"

#define TABLE_DATA_ROLE Qt::UserRole+10
#define ONE_RECORD_ROLE Qt::UserRole+11
#define RECORD_ID_ROLE  Qt::UserRole+12
#define SORT_ROLE       Qt::UserRole+13

class Record;
class ItemsFlat;
class TreeItem;

class RecordModel : public QAbstractTableModel {
    Q_OBJECT

    // By the closed (private) function models can have access controller   // К закрытым (private) функциям модели может иметь доступ контроллер
    friend class RecordController;

public:
    RecordModel(QString screen_name
                // , boost::intrusive_ptr<TreeItem> _shadow_branch_root
                , QObject *pobj = 0);
    ~RecordModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Интерфейс модели, предоставление данных по указанному индексу
    // Interface models to provide data at the specified index
    QVariant data(const QModelIndex &index, int role) const;

    // Интерфейс модели, сохранение вводимых данных по указанному индексу
    // Interface model, saving data entry at the specified index
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    //    std::shared_ptr<RecordTable> table_data() {return _table;}

    // Интерфейс модели, сколько записей в таблице
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    // Интерфейс модели, сколько столбцов в таблице
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void tree_item(boost::intrusive_ptr<TreeItem> item);

    boost::intrusive_ptr<TreeItem> tree_item() {return _shadow_branch_root;}



public slots:
    void reset_internal_data();

private:

    //    // Установка указателя на таблицу данных, с которой нужно работать модели
    //    void reset_tabledata(std::shared_ptr<RecordTable> record_table);
    //    // Ссылка на данные, с которыми работает модель
    //    boost::intrusive_ptr<TreeItem> tree_item(void);

    // Добавление записей
    int insert_new_item(int mode, QModelIndex pos_index, boost::intrusive_ptr<TreeItem> item);

    void on_table_config_changed(void);

    //protected:

    // Указатель на таблицу конечных записей
    //    std::shared_ptr<RecordTable>  _table; // flat one
    boost::intrusive_ptr<TreeItem> _shadow_branch_root; // keep it flat

};

#endif // __RECORDTABLEMODEL_H__
