#ifndef __RECORDTABLEMODEL_H__
#define __RECORDTABLEMODEL_H__

#include <memory>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <boost/serialization/strong_typedef.hpp>

//#include "models/tree/TreeItem.h"

#define TABLE_DATA_ROLE Qt::UserRole+10
#define ONE_RECORD_ROLE Qt::UserRole+11
#define RECORD_ID_ROLE  Qt::UserRole+12
#define SORT_ROLE       Qt::UserRole+13


extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

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

struct PosProxy;
struct PosSource;
struct IndexProxy;
struct IndexSource;
struct IdType;



BOOST_STRONG_TYPEDEF(QString, IdType)
BOOST_STRONG_TYPEDEF(int, PosSource)
BOOST_STRONG_TYPEDEF(int, PosProxy)
BOOST_STRONG_TYPEDEF(QModelIndex, IndexProxy)
BOOST_STRONG_TYPEDEF(QModelIndex, IndexSource)

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

    struct ModelIndex {
    public:
        ModelIndex(const std::function<RecordModel *()> &current_model, boost::intrusive_ptr<TreeItem> sibling_item, boost::intrusive_ptr<TreeItem>  target_item);
        std::function<RecordModel *()> current_model()const;
        QModelIndex sibling_index()const;
        // QModelIndex current_index()const;
        boost::intrusive_ptr<TreeItem> sibling() const;
        boost::intrusive_ptr<TreeItem> target() const;
    private:
        std::function<RecordModel *()>  _current_model;
        boost::intrusive_ptr<TreeItem>  _sibling_item;
        boost::intrusive_ptr<TreeItem>  _target_item;
        //        QModelIndex                     _current_index;
    };

    RecordModel(RecordController        *_record_controller
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

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(boost::intrusive_ptr<TreeItem> it)const;
    // for multi items link with unique page
    boost::intrusive_ptr<TreeItem> item_bounded(boost::intrusive_ptr<TreeItem> it)const;
    boost::intrusive_ptr<TreeItem> item(boost::intrusive_ptr<TreeItem> it)const;

    //    int item_current(QString find_id);
    //    bool remove_child(QString find_id);
    //    bool remove_child(int index);
    //    boost::intrusive_ptr<TreeItem> tree_item() {return _shadow_branch_root;}
    boost::intrusive_ptr<TreeItem> item(const PosSource _index);
    boost::intrusive_ptr<TreeItem> item(const PosSource _index)const;
    boost::intrusive_ptr<TreeItem> item(const IdType &id);
    boost::intrusive_ptr<TreeItem> item(const IdType &id)const;
    boost::intrusive_ptr<TreeItem> item(const QUrl &_url)const;
    boost::intrusive_ptr<TreeItem> item_fat(PosSource index);
    //    boost::intrusive_ptr<TreeItem> item(int pos) {return item(pos);}
    //    boost::intrusive_ptr<TreeItem> item(int pos)const {return item(pos);}



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


    int count()const; // {return _tabmanager->count();}


    boost::intrusive_ptr<TreeItem> current_item()const;

    void position(PosSource _index);
    //    PosSource position()const;
    PosSource position(IdType id)const;
    //    PosSource position(boost::intrusive_ptr<TreeItem> item)const;



public slots:


private:

    void reset_internal_data();

    //    // Установка указателя на таблицу данных, с которой нужно работать модели
    //    void reset_tabledata(std::shared_ptr<RecordTable> record_table);
    //    // Ссылка на данные, с которыми работает модель
    //    boost::intrusive_ptr<TreeItem> tree_item(void);


    //    void tree_item(boost::intrusive_ptr<TreeItem> item);
    //    void browser_pages(ItemsFlat *_browser_pages);
    //    void root(boost::intrusive_ptr<TreeItem> item);
    void on_table_config_changed(void);

    // Добавление записей
    PosSource insert_new_item(IndexSource source_pos_index, boost::intrusive_ptr<TreeItem> _item, int mode = add_new_record_after);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void remove_child(boost::intrusive_ptr<TreeItem> it);

    int move_up(const PosSource pos);
    int move_dn(const PosSource pos);

    //protected:

    // Указатель на таблицу конечных записей
    //    std::shared_ptr<RecordTable>  _table; // flat one
    //    TreeModelKnow *_browser_pages;  // boost::intrusive_ptr<TreeItem> _shadow_branch_root; // keep it flat

    RecordController    *_record_controller;
    friend class RecordScreen;
};

#endif // __RECORDTABLEMODEL_H__
