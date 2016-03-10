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
class KnowModel;
class RecordController;
class MainWindow;
class TreeScreen;
class FindScreen;
class MetaEditor;
class AppConfig;
class RecordScreen;

namespace browser {
    class Entrance;
    class Browser;
    class TabWidget;
}


struct pages_container {
public:
    explicit pages_container(browser::TabWidget *_tabmanager);
    ~pages_container();
    browser::TabWidget *tabmanager() {return _tabmanager;}
    //    void browser_pages(ItemsFlat *_browser_pages);
    //    ItemsFlat *browser_pages();
    //    ItemsFlat *browser_pages()const;

protected:
    browser::TabWidget  *_tabmanager;
    //    ItemsFlat           *_browser_pages;
};

class RecordModel : public QAbstractTableModel
    , public pages_container {
    Q_OBJECT

    // By the closed (private) function models can have access controller   // К закрытым (private) функциям модели может иметь доступ контроллер
    friend class RecordController;
    friend class browser::Entrance;
public:
    RecordModel(RecordController      *_record_controller
                , RecordScreen          *_record_screen
                , browser::TabWidget    *_tabmanager
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
    int size() const;   // {return rowCount();}
    // Интерфейс модели, сколько столбцов в таблице
    int columnCount(const QModelIndex &parent = QModelIndex()) const;





    QString field(int pos, QString name);
    void fields(int pos, QMap<QString, QString> data);
    boost::intrusive_ptr<TreeItem> find_current_bound(boost::intrusive_ptr<TreeItem> item)const;
    boost::intrusive_ptr<TreeItem> find_current(boost::intrusive_ptr<TreeItem> item)const;
    boost::intrusive_ptr<TreeItem> find_current(QUrl _url)const;
    int find_current(QString find_id);
    //    bool remove_child(QString find_id);
    //    bool remove_child(int index);
    //    boost::intrusive_ptr<TreeItem> tree_item() {return _shadow_branch_root;}
    boost::intrusive_ptr<TreeItem> child(int pos);
    boost::intrusive_ptr<TreeItem> child(int pos)const;
    boost::intrusive_ptr<TreeItem> child(QString id);
    boost::intrusive_ptr<TreeItem> child(QString id)const;
    boost::intrusive_ptr<TreeItem> item(int pos) {return child(pos);}
    boost::intrusive_ptr<TreeItem> item(int pos)const {return child(pos);}



    //    void init_source_model(boost::intrusive_ptr<TreeItem> item, RecordController *_record_controller, RecordScreen *_record_screen, MainWindow *main_window, MetaEditor *_editor_screen);

    //    void init_source_model(ItemsFlat *_browser_pages
    //                           , RecordController *_record_controller
    //                           , RecordScreen *_record_screen
    //                           , MainWindow *main_window
    //                           , MetaEditor *_editor_screen
    //                          );

    //    void init_source_model(RecordController *_record_controller, RecordScreen *_record_screen, MainWindow *main_window, MetaEditor *_editor_screen);

    //    ItemsFlat *browser_pages()const {return pages_container::browser_pages();}
    RecordController *reocrd_controller()const {return _record_controller;}
    boost::intrusive_ptr<TreeItem> item_fat(int index);
    int position(QString id);
    int locate(boost::intrusive_ptr<TreeItem> item);
    int count()const; // {return _tabmanager->count();}
    void work_pos(int pos);
    int work_pos()const;




public slots:
    void reset_internal_data();

private:

    //    // Установка указателя на таблицу данных, с которой нужно работать модели
    //    void reset_tabledata(std::shared_ptr<RecordTable> record_table);
    //    // Ссылка на данные, с которыми работает модель
    //    boost::intrusive_ptr<TreeItem> tree_item(void);


    //    void tree_item(boost::intrusive_ptr<TreeItem> item);
    //    void browser_pages(ItemsFlat *_browser_pages);
    //    void root(boost::intrusive_ptr<TreeItem> item);
    void on_table_config_changed(void);

    // Добавление записей
    int insert_new_item(QModelIndex pos_index, boost::intrusive_ptr<TreeItem> _item, int mode = ADD_NEW_RECORD_AFTER);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void remove_child(boost::intrusive_ptr<TreeItem> it);

    int move_up(const int pos);
    int move_dn(const int pos);

    //protected:

    // Указатель на таблицу конечных записей
    //    std::shared_ptr<RecordTable>  _table; // flat one
    //    TreeModelKnow *_browser_pages;  // boost::intrusive_ptr<TreeItem> _shadow_branch_root; // keep it flat

    RecordController    *_record_controller;
};

#endif // __RECORDTABLEMODEL_H__
