#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>
#include "utility/delegate.h"
#include <boost/serialization/strong_typedef.hpp>
#include "models/record_table/RecordModel.h"
#include "models/tree/TreeItem.h"






extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;


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
    class TabWidget;
    // extern constexpr const char * DockedWindow::_defaulthome;
}

struct PosProxy;
struct PosSource;
struct IndexProxy;
struct IndexSource;
struct IdType;



class RecordController : public QObject {
    Q_OBJECT
public:

    //    typedef TreeItem::bind_helper       bind_helper;
    //    typedef TreeItem::activate_helper   active_helper;
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
    boost::intrusive_ptr<TreeItem> item_click(const IndexProxy &index_proxy_, bool force_update = false);

    //    bool is_tree_item_exists(void);
    //    void reset_tabledata_test(TableData *rtData);
    //    void tree_item(boost::intrusive_ptr<TreeItem> tree_item);
    //    void reset_tabledata(std::shared_ptr<RecordTable> table_data);

    int row_count(void)const;

    void add_items_to_clipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

    //    void url_load(IndexProxy proxyIndex);
    // Действия при редактировании инфополей записи из контекстного меню
    bool edit_field_context(IndexProxy proxyIndex);

    template<typename return_type, typename parameter_type>  inline return_type index(const parameter_type &)const;

    //    IndexProxy  index_proxy(PosProxy pos)const;
    //    IndexProxy  index_proxy(IndexSource sourceIndex)const;
    //    IndexProxy  index_proxy(PosSource pos_source_) const;
    //    IndexProxy  index_proxy(IdType id)const;

    //    IndexSource index_source(IdType id)const;
    //    IndexSource index_source(IndexProxy proxyIndex)const;
    //    IndexSource index_source(PosProxy pos_proxy_)const;
    //    IndexSource index_source(PosSource pos_source_)const;

    //    PosProxy    pos_proxy(IndexProxy index)const;
    //    PosProxy    pos_proxy(PosSource pos_source)const;

    //    PosSource   pos_source(PosProxy pos_proxy_)const;
    //    PosSource   pos_source(IndexSource index)const;


    //    PosProxy first_selectionpos(void) const;
    //    IdType first_selectionid(void)const;
    //    void    select(PosProxy pos_proxy_);
    //    void    select(IdType id);


    void remove(IdType delete_id);
    void remove(QVector<IdType> delete_ids);

    boost::intrusive_ptr<TreeItem> find(const QUrl &_url);

    //    boost::intrusive_ptr<TreeItem> item_request_from_tree(boost::intrusive_ptr<TreeItem> item);

    //    boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url);

    //    int addnew_page_record(boost::intrusive_ptr<Record> record, int mode = add_new_record_after);

    //    boost::intrusive_ptr<TreeItem> tree_item();

    //    void sychronize_metaeditor_to_item(const PosSource pos_source_);
    void sychronize_attachtable_to_item(const PosSource pos);

    void addnew_blank(int mode);

    //    int new_record_from_url(const QUrl &url, const int mode = add_new_record_after);



    //    void init_source_model(boost::intrusive_ptr<TreeItem> item);
    //    void init_source_model(TreeModelKnow *_shadow_branch, MainWindow *main_window, MetaEditor *_editor_screen);
    //    bool no_view() {return _no_view;}
    boost::intrusive_ptr<TreeItem> synchronize_record_view(boost::intrusive_ptr<TreeItem> item);
    RecordScreen *record_screen() {return _record_screen;}
    //    RecordController *reocrd_controller() {return this;}
    //    RecordController *reocrd_controller()const {return const_cast<RecordController *>(this);}


signals:

public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void on_edit_fieldcontext(void);

    //    void pages_remove(void);

    // Вызов действий из контекстного меню для открытия окна с вводом новой записи
    //    void addnew_to_end(void);
    //    void addnew_before(void);
    //    void addnew_after(void);
    //void autoAddNewAfterContext(void);

    // Вызов действий из контекстного меню для удаления конечной записи
    void close_context(void);

    //    void move_up(void);
    //    void move_dn(void);

    // Клик по пункту "Сортировка" в контекстном меню
    void on_sort_click(void);

    // Слот для вызов настроек
    void settings(void);

    // Слот, обновляющий вид если изменились настройки таблицы конечных записей в конфиге программы
    void on_recordtable_configchange(void);

    // Печать таблицы конечных записей
    void on_print_click(void);

    void cursor_to_index(PosProxy pos_proxy_);  // , const int mode = add_new_record_after
    //    void cursor_to_index(boost::intrusive_ptr<TreeItem> it);
protected:


    //    void browser_update(const PosSource pos_source_);

    //    bool                _no_view = true;
    RecordModel         *_source_model; // Class, advanced by QAbstractTableModel   // Класс, расширенный от QAbstractTableModel
    RecordProxyModel    *_proxy_model;
    RecordView          *_view;
    browser::TabWidget  *_tabmanager;   //
    RecordScreen        *_record_screen;
    MetaEditor          *_editor_screen;
    MainWindow          *_main_window;

    PosSource addnew_item_fat(boost::intrusive_ptr<TreeItem> item, const int mode = add_new_record_after); // add_new_record_after
    PosSource addnew_item(boost::intrusive_ptr<TreeItem> item_, const int mode = add_new_record_after);

    void edit_field(PosSource pos
                    , QString pin
                    , QString name
                    , QString author
                    , QString home, QString url
                    , QString tags
                   );

    // Methods of removing records transferred to public access, because through them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop

    //    void pages_remove_from_browser(QVector<QString> del_ids);
    //    void remove_child(int index);

    //    friend class browser::Entrance;

    //friend class WebView;

    //    friend boost::intrusive_ptr<Record> register_record(boost::intrusive_ptr<Record> record);

    //    friend Record *register_record(const QUrl &_url);
};


template<>PosProxy      RecordController::index<PosProxy>(const PosSource &)const;
template<>PosProxy      RecordController::index<PosProxy>(const IndexProxy &)const;
template<>PosProxy      RecordController::index<PosProxy>(const IndexSource &)const;
template<>PosProxy      RecordController::index<PosProxy>(const IdType &)const;
template<>PosProxy      RecordController::index<PosProxy>(const boost::intrusive_ptr<TreeItem> &)const;

template<>PosSource     RecordController::index<PosSource>(const PosProxy &)const;
template<>PosSource     RecordController::index<PosSource>(const IndexProxy &)const;
template<>PosSource     RecordController::index<PosSource>(const IndexSource &)const;
template<>PosSource     RecordController::index<PosSource>(const IdType &)const;
template<>PosSource     RecordController::index<PosSource>(const boost::intrusive_ptr<TreeItem> &)const;

template<>IndexProxy    RecordController::index<IndexProxy>(const PosSource &)const;
template<>IndexProxy    RecordController::index<IndexProxy>(const PosProxy &)const;
template<>IndexProxy    RecordController::index<IndexProxy>(const IndexSource &)const;
template<>IndexProxy    RecordController::index<IndexProxy>(const IdType &)const;
template<>IndexProxy    RecordController::index<IndexProxy>(const boost::intrusive_ptr<TreeItem> &)const;

template<>IndexSource   RecordController::index<IndexSource>(const PosSource &)const;
template<>IndexSource   RecordController::index<IndexSource>(const IndexProxy &)const;
template<>IndexSource   RecordController::index<IndexSource>(const PosProxy &)const;
template<>IndexSource   RecordController::index<IndexSource>(const IdType &)const;
template<>IndexSource   RecordController::index<IndexSource>(const boost::intrusive_ptr<TreeItem> &)const;

template<>IdType        RecordController::index<IdType>(const PosSource &)const;
template<>IdType        RecordController::index<IdType>(const IndexProxy &)const;
template<>IdType        RecordController::index<IdType>(const PosProxy &)const;
template<>IdType        RecordController::index<IdType>(const IndexSource &)const;
template<>IdType        RecordController::index<IdType>(const boost::intrusive_ptr<TreeItem> &)const;

template<>boost::intrusive_ptr<TreeItem> RecordController::index<boost::intrusive_ptr<TreeItem>>(const PosSource &)const;
template<>boost::intrusive_ptr<TreeItem> RecordController::index<boost::intrusive_ptr<TreeItem>>(const IndexProxy &)const;
template<>boost::intrusive_ptr<TreeItem> RecordController::index<boost::intrusive_ptr<TreeItem>>(const PosProxy &)const;
template<>boost::intrusive_ptr<TreeItem> RecordController::index<boost::intrusive_ptr<TreeItem>>(const IndexSource &)const;
template<>boost::intrusive_ptr<TreeItem> RecordController::index<boost::intrusive_ptr<TreeItem>>(const IdType &)const;


#endif // __RECORDTABLECONTROLLER_H__
