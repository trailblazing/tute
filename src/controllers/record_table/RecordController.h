#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>
#include "utility/delegate.h"
#include "models/record_table/ItemsFlat.h"
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
class TreeModelKnow;
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
    typedef TreeItem::active_helper active_helper;
    RecordController(QString screen_name
                     , browser::Browser *_browser
                     , TreeScreen       *_tree_screen
                     , FindScreen       *_find_screen
                     , MetaEditor       *_editor_screen
                     , MainWindow       *_main_window
                     , RecordScreen     *_record_screen
                    );
    virtual ~RecordController();

    void init(void);

    RecordView *view(void);
    RecordModel *source_model();    // {return _source_model;}
    RecordScreen *record_screen() {return _record_screen;}
    void click_item(const QModelIndex &index);

    //    bool is_tree_item_exists(void);
    //    void reset_tabledata_test(TableData *rtData);
    //    void tree_item(boost::intrusive_ptr<TreeItem> tree_item);
    //    void reset_tabledata(std::shared_ptr<RecordTable> table_data);

    int row_count(void)const;

    void add_items_to_clipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

    void open_website(QModelIndex proxyIndex);
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

    // Methods of removing records transferred to public access, because through them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop
    void remove_child(QString delId);
    void remove_children(QVector<QString> delIds);



    boost::intrusive_ptr<TreeItem> find(const QUrl &_url);

    boost::intrusive_ptr<TreeItem> request_item(
        boost::intrusive_ptr<TreeItem> item
        , bind_helper generator
        , active_helper activator
    );

    boost::intrusive_ptr<TreeItem> request_item(
        const QUrl &_url
        , bind_helper generator
        , active_helper activator
    );

    //    int addnew_page_record(boost::intrusive_ptr<Record> record, int mode = add_new_record_after);

    //    boost::intrusive_ptr<TreeItem> tree_item();
    void sychronize_metaeditor_to_item(const int pos);
    void sychronize_attachtable_to_item(const int pos);
    void addnew_blank(int mode);

    //    int new_record_from_url(const QUrl &url, const int mode = add_new_record_after);

    int addnew_item_fat(boost::intrusive_ptr<TreeItem> item
                        , const int mode
                        = ADD_NEW_RECORD_AFTER // add_new_record_after
                       );

    //    void init_source_model(boost::intrusive_ptr<TreeItem> item);
    //    void init_source_model(TreeModelKnow *_shadow_branch, MainWindow *main_window, MetaEditor *_editor_screen);
    bool no_view() {return _no_view;}
    boost::intrusive_ptr<TreeItem> register_item_to_browser_source_model(boost::intrusive_ptr<TreeItem> item);

signals:

public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void on_edit_fieldcontext(void);

    void delete_items_selected(void);

    // Вызов действий из контекстного меню для открытия окна с вводом новой записи
    void addnew_to_end(void);
    void addnew_before(void);
    void addnew_after(void);
    //void autoAddNewAfterContext(void);

    // Вызов действий из контекстного меню для удаления конечной записи
    void delete_context(void);

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


    void update_browser(const int source_pos);

    bool                _no_view = true;
    RecordModel         *_source_model; // Class, advanced by QAbstractTableModel   // Класс, расширенный от QAbstractTableModel
    RecordProxyModel    *_proxy_model;
    RecordView          *_view;
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

    friend class browser::Entrance;
    //friend class WebView;

    //    friend boost::intrusive_ptr<Record> register_record(boost::intrusive_ptr<Record> record);

    //    friend Record *register_record(const QUrl &_url);
};

#endif // __RECORDTABLECONTROLLER_H__
