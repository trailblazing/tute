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
class TreeModelKnow;
class RecordController;
class MainWindow;
class TreeScreen;
class FindScreen;
class MetaEditor;
class AppConfig;
class RecordScreen;

namespace browser {
    class Entrance;
}

class RecordModel : public QAbstractTableModel {
    Q_OBJECT

    // By the closed (private) function models can have access controller   // К закрытым (private) функциям модели может иметь доступ контроллер
    friend class RecordController;
    friend class browser::Entrance;
public:
    RecordModel(QString screen_name
                , TreeScreen *_tree_screen
                , FindScreen *_find_screen   // browser::ToolbarSearch *toolbarsearch
                , MetaEditor *_editor_screen
                , MainWindow *_main_window
                , RecordController *_record_controller, RecordScreen *_record_screen
               );

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
    int size() const {return rowCount();}
    // Интерфейс модели, сколько столбцов в таблице
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void remove_child(boost::intrusive_ptr<TreeItem> it);

    QString field(int pos, QString name);
    void fields(int pos, QMap<QString, QString> data);
    boost::intrusive_ptr<TreeItem> find(boost::intrusive_ptr<TreeItem> item);
    int is_item_exists(QString find_id);
    bool remove_child(QString find_id);
    //    boost::intrusive_ptr<TreeItem> tree_item() {return _shadow_branch_root;}
    boost::intrusive_ptr<TreeItem> child(int pos);
    boost::intrusive_ptr<TreeItem> item(int pos) {return child(pos);}
    // Добавление записей
    int insert_new_item(QModelIndex pos_index, boost::intrusive_ptr<TreeItem> item, int mode = ADD_NEW_RECORD_AFTER);
    void work_pos(int pos);

    void init_source_model(boost::intrusive_ptr<TreeItem> item, RecordController *_record_controller, RecordScreen *_record_screen, MainWindow *main_window, MetaEditor *_editor_screen);
    void init_source_model(ItemsFlat *_browser_pages
                           , RecordController *_record_controller
                           , RecordScreen *_record_screen
                           , MainWindow *main_window
                           , MetaEditor *_editor_screen
                          );
    void init_source_model(RecordController *_record_controller, RecordScreen *_record_screen, MainWindow *main_window, MetaEditor *_editor_screen);
    ItemsFlat *get_shadow_branch()const {return _browser_pages;}
    RecordController *reocrd_controller()const {return _reocrd_controller;}
public slots:
    void reset_internal_data();

private:

    //    // Установка указателя на таблицу данных, с которой нужно работать модели
    //    void reset_tabledata(std::shared_ptr<RecordTable> record_table);
    //    // Ссылка на данные, с которыми работает модель
    //    boost::intrusive_ptr<TreeItem> tree_item(void);


    //    void tree_item(boost::intrusive_ptr<TreeItem> item);
    void shadow_branch(ItemsFlat *_browser_pages);
    //    void root(boost::intrusive_ptr<TreeItem> item);
    void on_table_config_changed(void);

    //protected:

    // Указатель на таблицу конечных записей
    //    std::shared_ptr<RecordTable>  _table; // flat one
    //    TreeModelKnow *_browser_pages;  // boost::intrusive_ptr<TreeItem> _shadow_branch_root; // keep it flat
    ItemsFlat           *_browser_pages;
    RecordController    *_reocrd_controller;
};

#endif // __RECORDTABLEMODEL_H__
