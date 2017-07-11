#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QModelIndexList>


#include "libraries/global_parameters.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/tree_item.h"
#include "utility/delegate.h"
#include "utility/lease.h"
#include <boost/serialization/strong_typedef.hpp>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

class r_t;
class rv_t;
class vector_t;
class RecordModel;
class RecordProxyModel;
class ClipboardRecords;
class rs_t;
class i_t;
class tkm_t;
class wn_t;
class ts_t;
class FindScreen;
// class EditorDock;
class AppConfig;
class Blogger;

namespace web {
    template <typename>
    class Docker;
    class Browser;
    class WebView;
    class TabWidget;
    // extern constexpr const char * DockedWindow::_defaulthome;
}

struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;

class rctrl_t : public QObject
#ifdef USE_SIGNAL_CLOSE
    ,
                //		public boost::intrusive_ref_counter<rctrl_t, boost::thread_safe_counter>,
                public sd::renter //<rctrl_t>
#endif                            //USE_SIGNAL_CLOSE
{
#if QT_VERSION == 0x050600
    W_OBJECT(rctl_t)
#else
    Q_OBJECT
#endif
public:
    typedef QObject super;
    // typedef TreeItem::bind_helper       bind_helper;
    // typedef TreeItem::activate_helper   active_helper;
    rctrl_t(Blogger* blogger_, web::TabWidget* tabmanager_, rs_t* record_screen_);
    virtual ~rctrl_t();

    // void init(void);

    rv_t* view(void) const;
    RecordModel* source_model(); // {return _source_model;}
    RecordProxyModel* proxy_model();
    web::TabWidget* tab_widget();
    //	void tab_widget(web::TabWidget* tab_);
    boost::intrusive_ptr<i_t> index_invoke(const index_proxy& index_proxy_, bool force_update = false);

    // bool is_tree_item_exists(void);
    // void reset_tabledata_test(TableData *rtData);
    // void tree_item(boost::intrusive_ptr<TreeItem> tree_item);
    // void reset_tabledata(std::shared_ptr<RecordTable> table_data);

    int row_count(void) const;

    void add_items_to_clipboard(ClipboardRecords* clipboardRecords, QModelIndexList itemsForCopy);

    // void url_load(IndexProxy proxyIndex);
    // Действия при редактировании инфополей записи из контекстного меню
    bool edit_field_context(index_proxy proxyIndex);

    template <typename return_type, typename parameter_type>
    inline return_type index(const parameter_type&) const;

    // template<typename return_type, typename parameter_type>
    // std::function<return_type(const parameter_type &)> index_object;

    // IndexProxy  index_proxy(PosProxy pos)const;
    // IndexProxy  index_proxy(IndexSource sourceIndex)const;
    // IndexProxy  index_proxy(PosSource pos_source_) const;
    // IndexProxy  index_proxy(IdType id)const;

    // IndexSource index_source(IdType id)const;
    // IndexSource index_source(IndexProxy proxyIndex)const;
    // IndexSource index_source(PosProxy pos_proxy_)const;
    // IndexSource index_source(PosSource pos_source_)const;

    // PosProxy    pos_proxy(IndexProxy index)const;
    // PosProxy    pos_proxy(PosSource pos_source)const;

    // PosSource   pos_source(PosProxy pos_proxy_)const;
    // PosSource   pos_source(IndexSource index)const;

    // PosProxy first_selectionpos(void) const;
    // IdType first_selectionid(void)const;
    // void    select(PosProxy pos_proxy_);
    // void    select(IdType id);

    void remove(id_value delete_id);
    void remove(QVector<id_value> delete_ids);

    // boost::intrusive_ptr<TreeItem> find(const QUrl &_url);

    // boost::intrusive_ptr<TreeItem>
    // item_request_from_tree(boost::intrusive_ptr<TreeItem> item);

    // boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url);

    // int addnew_page_record(boost::intrusive_ptr<Record> record, int mode =
    // add_new_record_after);

    // boost::intrusive_ptr<TreeItem> tree_item();

    // void sychronize_metaeditor_to_item(const PosSource pos_source_);
    void sychronize_attachtable_to_item(const pos_source pos);

    web::WebView* addnew_blank(); // int mode

    // int new_record_from_url(const QUrl &url, const int mode =
    // add_new_record_after);

    // void init_source_model(boost::intrusive_ptr<TreeItem> item);
    // void init_source_model(TreeModelKnow *_shadow_branch, MainWindow
    // *main_window, MetaEditor *_editor_screen);
    // bool no_view() {return _no_view;}

    rs_t* record_screen();

    // RecordController *reocrd_controller() {return this;}
    // RecordController *reocrd_controller()const {return
    // const_cast<RecordController *>(this);}

    Blogger* blogger() const;
    //    signals:
    //	void close_request(QObject*);
    //	sd::shared_ptr<rctrl_t> renter() const;
public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void on_edit_fieldcontext(void);

    // void pages_remove(void);

    // Вызов действий из контекстного меню для открытия окна с вводом новой записи
    // void addnew_to_end(void);
    // void addnew_before(void);
    // void addnew_after(void);
    // void autoAddNewAfterContext(void);

    // Вызов действий из контекстного меню для удаления конечной записи
    void close_context(void);

    // void move_up(void);
    // void move_dn(void);

    // Клик по пункту "Сортировка" в контекстном меню
    void on_sort_click(void);

    // Слот для вызов настроек
    void settings(void);

    // Слот, обновляющий вид если изменились настройки таблицы конечных записей в
    // конфиге программы
    void on_recordtable_configchange(void);

    // Печать таблицы конечных записей
    void on_print_click(void);


    // void cursor_to_index(boost::intrusive_ptr<TreeItem> it);
    void on_sort_requested(int logicalIndex, Qt::SortOrder order);

protected:
    // void browser_update(const PosSource pos_source_);

    // bool                _no_view = true;
    tabwidget_ref _tab_widget; //

    //	sd::shared_ptr<rctrl_t> _renter; //	std::shared_ptr<boost::signals2::signal<void(rctrl_t*)>> _close_request;
    RecordModel* _source_model; // Class, advanced by QAbstractTableModel   //
    // Класс, расширенный от QAbstractTableModel
    RecordProxyModel* _proxy_model;

    rs_ref _record_screen;
    blogger_ref _blogger;
    rv_ref _view;
    //	wn_t *_main_window;
    //	bool _closed = false;

    // web::WebView		*addnew_item_fat(boost::intrusive_ptr<RecordIndex>
    // record_index_, bool make_current = true);	// , const int mode =
    // add_new_record_after	// add_new_record_after
    web::WebView* addnew_item(boost::intrusive_ptr<RecordIndex> record_index_, bool make_current = true); // , const int mode = add_new_record_after
#ifdef USE_RECORD_INDEX_SELECTION
    boost::intrusive_ptr<i_t>
    select_as_current(boost::intrusive_ptr<RecordIndex> record_index_);
#endif // USE_RECORD_INDEX_SELECTION
    void edit_field(pos_source pos, const pin_value& pin, const name_value& name, const author_value& author, const home_value& home, const url_value& url, const tags_value& tags);

    // Methods of removing records transferred to public access, because through
    // them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления
    // записей перенесены в открытый доступ, так как через них удаляются даннные
    // из KnowTreeView при DragAndDrop

    // void pages_remove_from_browser(QVector<QString> del_ids);
    // void remove_child(int index);

    // friend class web::Entrance;

    // friend class WebView;

    // friend boost::intrusive_ptr<Record>
    // register_record(boost::intrusive_ptr<Record> record);

    // friend Record *register_record(const QUrl &_url);
private:
    boost::intrusive_ptr<i_t> select_as_current(pos_proxy pos_proxy_); // , const int mode = add_new_record_after
    friend struct RecordIndex;
    friend class web::TabWidget;
};

// template<typename return_type, typename parameter_type>
// std::function<return_type(const parameter_type &)> index_object;	//
// -std=c++14

template <>
pos_proxy rctrl_t::index<pos_proxy>(const pos_source&) const;
template <>
pos_proxy rctrl_t::index<pos_proxy>(const index_proxy&) const;
template <>
pos_proxy rctrl_t::index<pos_proxy>(const index_source&) const;
template <>
pos_proxy rctrl_t::index<pos_proxy>(const id_value&) const;
template <>
pos_proxy rctrl_t::index<pos_proxy>(const boost::intrusive_ptr<i_t>&) const;

template <>
pos_source rctrl_t::index<pos_source>(const pos_proxy&) const;
template <>
pos_source rctrl_t::index<pos_source>(const index_proxy&) const;
template <>
pos_source rctrl_t::index<pos_source>(const index_source&) const;
template <>
pos_source rctrl_t::index<pos_source>(const id_value&) const;
template <>
pos_source rctrl_t::index<pos_source>(const boost::intrusive_ptr<i_t>&) const;

template <>
index_proxy rctrl_t::index<index_proxy>(const pos_source&) const;
template <>
index_proxy rctrl_t::index<index_proxy>(const pos_proxy&) const;
template <>
index_proxy rctrl_t::index<index_proxy>(const index_source&) const;
template <>
index_proxy rctrl_t::index<index_proxy>(const id_value&) const;
template <>
index_proxy
rctrl_t::index<index_proxy>(const boost::intrusive_ptr<i_t>&) const;

template <>
index_source rctrl_t::index<index_source>(const pos_source&) const;
template <>
index_source rctrl_t::index<index_source>(const index_proxy&) const;
template <>
index_source rctrl_t::index<index_source>(const pos_proxy&) const;
template <>
index_source rctrl_t::index<index_source>(const id_value&) const;
template <>
index_source
rctrl_t::index<index_source>(const boost::intrusive_ptr<i_t>&) const;

template <>
id_value rctrl_t::index<id_value>(const pos_source&) const;
template <>
id_value rctrl_t::index<id_value>(const index_proxy&) const;
template <>
id_value rctrl_t::index<id_value>(const pos_proxy&) const;
template <>
id_value rctrl_t::index<id_value>(const index_source&) const;
template <>
id_value rctrl_t::index<id_value>(const boost::intrusive_ptr<i_t>&) const;

template <>
boost::intrusive_ptr<i_t>
rctrl_t::index<boost::intrusive_ptr<i_t>>(const pos_source&) const;
template <>
boost::intrusive_ptr<i_t>
rctrl_t::index<boost::intrusive_ptr<i_t>>(const index_proxy&) const;
template <>
boost::intrusive_ptr<i_t>
rctrl_t::index<boost::intrusive_ptr<i_t>>(const pos_proxy&) const;
template <>
boost::intrusive_ptr<i_t>
rctrl_t::index<boost::intrusive_ptr<i_t>>(const index_source&) const;
template <>
boost::intrusive_ptr<i_t>
rctrl_t::index<boost::intrusive_ptr<i_t>>(const id_value&) const;

#endif // __RECORDTABLECONTROLLER_H__
