#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>
#include "utility/delegate.h"
//#include "models/record_table/ItemsFlat.h"
#include "models/tree/TreeItem.h"

//#include <boost/serialization/strong_typedef.hpp>

//extern const int add_new_record_after;

class Record;
class RecordView;
class ItemsFlat;
class RecordModel;
class RecordProxyModel;
class ClipboardRecords;
class RecordScreen;
class TreeItem;
class KnowModel;
class MainWindow;
class TreeScreen;
class FindScreen;
class MetaEditor;
class AppConfig;


namespace browser {
    class Entrance;
    class Browser;
    class WebView;
    // extern constexpr const char * DockedWindow::_defaulthome;
}


//BOOST_STRONG_TYPEDEF(QString, Id_T)


class RecordController : public QObject {
    Q_OBJECT
public:

    typedef TreeItem::bind_helper bind_helper;
    typedef TreeItem::activate_helper active_helper;
    RecordController(MetaEditor             *_editor_screen
                     , browser::TabWidget   *_tabmanager
                     , RecordScreen         *_record_screen
                     , MainWindow           *_main_window
                    );
    virtual ~RecordController();

    //    void init(void);

    RecordView *view(void);
    RecordModel *source_model();    // {return _source_model;}
    RecordProxyModel *proxy_model();
    browser::TabWidget *tabmanager() {return _tabmanager;}
    void item_click(const QModelIndex &index);

    //    bool is_tree_item_exists(void);
    //    void reset_tabledata_test(TableData *rtData);
    //    void tree_item(boost::intrusive_ptr<TreeItem> tree_item);
    //    void reset_tabledata(std::shared_ptr<RecordTable> table_data);

    int row_count(void)const;

    void add_items_to_clipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

    void url_load(QModelIndex proxyIndex);
    // Действия при редактировании инфополей записи из контекстного меню
    void edit_field_context(QModelIndex proxyIndex);

    QModelIndex pos_to_proxyindex(int pos)const;
    QModelIndex pos_to_sourceindex(int pos)const;
    int         proxyindex_to_pos(QModelIndex index)const;
    int         sourceindex_to_pos(QModelIndex index)const;
    QModelIndex proxyindex_to_sourceindex(QModelIndex proxyIndex)const;
    QModelIndex sourceindex_to_proxyindex(QModelIndex sourceIndex)const;
    int         sourcepos_to_proxypos(int sourcePos)const;
    int         proxypos_to_sourcepos(int proxyPos)const;
    QModelIndex id_to_sourceindex(QString id)const;
    QModelIndex id_to_proxyindex(QString id)const;

    int     first_selectionpos(void) const;
    QString first_selectionid(void)const;
    void    select_pos(int pos);
    void    select_id(QString id);


    void page_remove(QString del_id);
    void pages_remove(QVector<QString> del_ids);

    boost::intrusive_ptr<TreeItem> find(const QUrl &_url);

    //    boost::intrusive_ptr<TreeItem> item_request_from_tree(boost::intrusive_ptr<TreeItem> item);

    //    boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url);

    //    int addnew_page_record(boost::intrusive_ptr<Record> record, int mode = add_new_record_after);

    //    boost::intrusive_ptr<TreeItem> tree_item();
    void sychronize_metaeditor_to_item(const int _index);
    void sychronize_attachtable_to_item(const int pos);

    void addnew_blank(int mode);

    //    int new_record_from_url(const QUrl &url, const int mode = add_new_record_after);

    int addnew_item_fat(boost::intrusive_ptr<TreeItem> item, const int mode = ADD_NEW_RECORD_AFTER); // add_new_record_after

    //    void init_source_model(boost::intrusive_ptr<TreeItem> item);
    //    void init_source_model(TreeModelKnow *_shadow_branch, MainWindow *main_window, MetaEditor *_editor_screen);
    bool no_view() {return _no_view;}
    boost::intrusive_ptr<TreeItem> update_record_view(boost::intrusive_ptr<TreeItem> item);
    RecordScreen *record_screen() {return _record_screen;}
    RecordController *reocrd_controller() {return this;}
    // RecordController *reocrd_controller()const {return const_cast<RecordController *>(this);}


signals:

public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void on_edit_fieldcontext(void);

    void pages_remove(void);

    // Вызов действий из контекстного меню для открытия окна с вводом новой записи
    void addnew_to_end(void);
    void addnew_before(void);
    void addnew_after(void);
    //void autoAddNewAfterContext(void);

    // Вызов действий из контекстного меню для удаления конечной записи
    void close_context(void);

    void move_up(void);
    void move_dn(void);

    // Клик по пункту "Сортировка" в контекстном меню
    void on_sort_click(void);

    // Слот для вызов настроек
    void settings(void);

    // Слот, обновляющий вид если изменились настройки таблицы конечных записей в конфиге программы
    void on_recordtable_configchange(void);

    // Печать таблицы конечных записей
    void on_print_click(void);

protected:


    void browser_update(const int source_pos);

    bool                _no_view = true;
    RecordModel         *_source_model; // Class, advanced by QAbstractTableModel   // Класс, расширенный от QAbstractTableModel
    RecordProxyModel    *_proxy_model;
    RecordView          *_view;
    browser::TabWidget  *_tabmanager;   //
    RecordScreen        *_record_screen;
    MetaEditor          *_editor_screen;
    MainWindow          *_main_window;

    int addnew_item(boost::intrusive_ptr<TreeItem> item, const int mode = ADD_NEW_RECORD_AFTER);

    void edit_field(int pos
                    , QString pin
                    , QString name
                    , QString author
                    , QString home, QString url
                    , QString tags
                   );

    // Methods of removing records transferred to public access, because through them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop

    void pages_remove_from_browser(QVector<QString> del_ids);
    //    void remove_child(int index);

    //    friend class browser::Entrance;

    //friend class WebView;

    //    friend boost::intrusive_ptr<Record> register_record(boost::intrusive_ptr<Record> record);

    //    friend Record *register_record(const QUrl &_url);
};

#endif // __RECORDTABLECONTROLLER_H__
