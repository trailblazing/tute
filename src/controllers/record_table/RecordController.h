#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>
#include "utility/delegate.h"
#include "models/record_table/RecordTable.h"
#include "models/tree/TreeItem.h"

//#include <boost/serialization/strong_typedef.hpp>

//extern const int add_new_record_after;

class Record;
class RecordView;
class RecordTable;
class RecordModel;
class RecordProxyModel;
class ClipboardRecords;
class RecordScreen;
class TreeItem;

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
    RecordController(QString screen_name, boost::intrusive_ptr<TreeItem> _tree_item, RecordScreen *table_screen);
    virtual ~RecordController();

    void init(void);

    RecordView *view(void);
    RecordModel *table_model() {return _source_model;}

    void click_record(const QModelIndex &index);

    bool is_table_notexists(void);
    //    void reset_tabledata_test(TableData *rtData);
    void reset_tabledata(std::shared_ptr<RecordTable> table_data);

    int row_count(void);

    void add_records_to_clipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

    void open_website(QModelIndex proxyIndex);
    // Действия при редактировании инфополей записи из контекстного меню
    void edit_field_context(QModelIndex proxyIndex);

    QModelIndex pos_to_proxyindex(int pos);
    QModelIndex pos_to_sourceindex(int pos);
    int         proxyindex_to_pos(QModelIndex index);
    int         sourceindex_to_pos(QModelIndex index);
    QModelIndex proxyindex_to_sourceindex(QModelIndex proxyIndex);
    QModelIndex sourceindex_to_proxyindex(QModelIndex sourceIndex);
    int         sourcepos_to_proxypos(int sourcePos);
    int         proxypos_to_sourcepos(int proxyPos);
    QModelIndex id_to_sourceindex(QString id);
    QModelIndex id_to_proxyindex(QString id);

    int     first_selectionpos(void);
    QString first_selectionid(void);
    void    select_pos(int pos);
    void    select_id(QString id);

    // Methods of removing records transferred to public access, because through them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop
    void removerow_by_id(QString delId);
    void removerows_by_idlist(QVector<QString> delIds);

    std::shared_ptr<Record> register_record(std::shared_ptr<Record> record);

    std::shared_ptr<Record> check_record(const QUrl &_url);

    std::shared_ptr<Record> request_record(
        std::shared_ptr<Record> record
        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>> generator
        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>> activator
    );

    std::shared_ptr<Record> request_record(
        const QUrl &_url
        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>> generator
        , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>> activator
    );

    //    int addnew_page_record(std::shared_ptr<Record> record, int mode = add_new_record_after);

    std::shared_ptr<RecordTable> table_data();
    void sychronize_metaeditor_to_record(const int pos);
    void sychronize_attachtable_to_record(const int pos);

signals:

public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void on_edit_fieldcontext(void);

    void delete_records_selected(void);

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


    RecordModel        *_source_model; // Class, advanced by QAbstractTableModel   // Класс, расширенный от QAbstractTableModel
    RecordProxyModel   *_proxy_model;
    RecordView         *_view;

    void addnew_blank(int mode);

    //    int new_record_from_url(const QUrl &url, const int mode = add_new_record_after);

    int addnew_record_fat(std::shared_ptr<Record> record
                          , const int mode
                          = ADD_NEW_RECORD_AFTER // add_new_record_after
                         );

    int addnew_record(std::shared_ptr<Record> record, int mode);

    void edit_field(int pos
                    , QString pin
                    , QString name
                    , QString author
                    , QString home, QString url
                    , QString tags
                   );

    friend class browser::Entrance;
    //friend class WebView;

    //    friend std::shared_ptr<Record> register_record(std::shared_ptr<Record> record);

    //    friend Record *register_record(const QUrl &_url);
};

#endif // __RECORDTABLECONTROLLER_H__
