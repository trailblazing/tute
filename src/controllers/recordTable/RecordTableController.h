#ifndef __RECORDTABLECONTROLLER_H__
#define __RECORDTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>
#include "utility/delegate.h"

//#include <boost/serialization/strong_typedef.hpp>

extern const int add_new_record_after;

class Record;
class RecordTableView;
class RecordTableData;
class RecordTableModel;
class RecordTableProxyModel;
class ClipboardRecords;

namespace browser {
    class Entrance;
    class DockedWindow;
    class WebView;
    // extern constexpr const char * DockedWindow::_defaulthome;
}


//BOOST_STRONG_TYPEDEF(QString, Id_T)


class RecordTableController : public QObject {
    Q_OBJECT
public:
    RecordTableController(QObject *parent = 0);
    virtual ~RecordTableController();

    void init(void);

    RecordTableView *getView(void);
    RecordTableModel *getRecordTableModel() {return recordSourceModel;}

    void clickToRecord(const QModelIndex &index);

    bool isTableNotExists(void);
    void set_table_data(RecordTableData *rtData);

    int getRowCount(void);

    void addRecordsToClipboard(ClipboardRecords *clipboardRecords, QModelIndexList itemsForCopy);

    void openWebsite(QModelIndex proxyIndex);
    // Действия при редактировании инфополей записи из контекстного меню
    void editFieldContext(QModelIndex proxyIndex);

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

    int     getFirstSelectionPos(void);
    QString getFirstSelectionId(void);
    void    select_pos(int pos);
    void    select_id(QString id);

    // Methods of removing records transferred to public access, because through them removed from Dunn when DragAndDrop KnowTreeView   // Методы удаления записей перенесены в открытый доступ, так как через них удаляются даннные из KnowTreeView при DragAndDrop
    void removeRowById(QString delId);
    void removeRowsByIdList(QVector<QString> delIds);

signals:

public slots:

    // Вызов действий для копирования записей в буфер обмена с удалением
    void cut(void);

    // Вызов действий для копирования записей в буфер обмена
    void copy(void);

    // Вызов действий для вставки записей из буфера
    void paste(void);

    void onEditFieldContext(void);

    void deleteRecords(void);

    // Вызов действий из контекстного меню для открытия окна с вводом новой записи
    void addNewToEndContext(void);
    void addNewBeforeContext(void);
    void addNewAfterContext(void);
    //void autoAddNewAfterContext(void);

    // Вызов действий из контекстного меню для удаления конечной записи
    void deleteContext(void);

    void moveUp(void);
    void moveDn(void);

    // Клик по пункту "Сортировка" в контекстном меню
    void onSortClick(void);

    // Слот для вызов настроек
    void settings(void);

    // Слот, обновляющий вид если изменились настройки таблицы конечных записей в конфиге программы
    void onRecordTableConfigChange(void);

    // Печать таблицы конечных записей
    void onPrintClick(void);

protected:

    void sychronize_metaeditor_to_record(const int pos);
    void sychronize_attachtable_to_record(const int pos);
    void update_browser(const int source_pos);

    RecordTableView *view;
    RecordTableModel *recordSourceModel; // Class, advanced by QAbstractTableModel   // Класс, расширенный от QAbstractTableModel
    RecordTableProxyModel *recordProxyModel;

    void addNewRecord(int mode);

    int new_record(
        const QUrl &url
        , const int mode
        = add_new_record_after
    );

    int new_record(Record const &record
                   , const int mode
                   = add_new_record_after
                  );

    int addNew(int mode, Record record);

    void editField(int pos
                   , QString pin
                   , QString name
                   , QString author
                   , QString home, QString url
                   , QString tags
                  );
    friend class browser::Entrance;
    //friend class WebView;
    //    friend Record *register_record(const QUrl &_url, std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator, RecordTableController *_recordtablecontroller);
    friend Record *register_record(Record const &record
                                   //                                   , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
                                   , RecordTableController *_recordtablecontroller);

    //    friend Record *register_record(const QUrl &_url);
};

#endif // __RECORDTABLECONTROLLER_H__
