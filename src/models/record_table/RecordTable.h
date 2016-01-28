#ifndef __RECORDTABLEDATA_H__
#define __RECORDTABLEDATA_H__

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>
#include <QtXml>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <memory>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

class TreeItem;
class Record;
class MetaEditor;

//extern const int add_new_record_to_end;

#define ADD_NEW_RECORD_TO_END 0
#define ADD_NEW_RECORD_BEFORE 1
#define ADD_NEW_RECORD_AFTER 2


class RecordTable {

public:
    //    RecordTable(boost::intrusive_ptr<TreeItem> _tree_item); // a kind of copy constructor
    RecordTable(const QDomElement &i_dom_element, const bool _is_crypt = false);
    virtual ~RecordTable();

    // Получение текста указанной записи
    QString text(int pos);

    // Установка текста и картинок указанной записи
    void set_text_and_pictures(int pos,
                               const QString &text,
                               const QMap<QString, QByteArray> &picturesFiles = (QMap<QString, QByteArray>()));

    // Получение значения указанного поля для указанного элемента
    QString field(int pos, QString name) const;

    // Установка значения указанного поля для указанного элемента
    void field(int pos, QString name, QString value);

    // Получение образа записи
    boost::intrusive_ptr<Record> record_lite(int pos);
    boost::intrusive_ptr<Record> record_fat(int pos);
    boost::intrusive_ptr<Record> record(int pos);
    boost::intrusive_ptr<Record> find(const QUrl &url);
    boost::intrusive_ptr<Record> find(const boost::intrusive_ptr<Record> r);

    //    void tree_item(boost::intrusive_ptr<TreeItem> tree_item);

    // Удаление всех элементов таблицы конечных записей
    void delete_all_records(void);

    // Количество записей в таблице данных
    int size(void) const;

    // Функция создания DOM-документа из данных таблицы конечных записей
    //    QDomElement export_to_dom(QDomDocument *doc) const;
    QDomElement export_activated_dom(std::shared_ptr<QDomDocument> doc) const;
    QDomElement export_to_dom() const;
    QDomElement export_to_dom(std::shared_ptr<QDomDocument> doc) const;

    //    // Получение ссылки на объект ветки, которой принадлежит таблица
    //    boost::intrusive_ptr<TreeItem> tree_item(void);

    int insert_new_record(int pos, boost::intrusive_ptr<Record> record, int mode = ADD_NEW_RECORD_AFTER);    // ADD_NEW_RECORD_TO_END
    int shadow_record_lite(int pos, boost::intrusive_ptr<Record> record, int mode = ADD_NEW_RECORD_AFTER);

    void edit_record_fields(int pos, QMap<QString, QString> edit_fields);

    void delete_record_by_position(int i);
    void delete_record_by_id(QString id);

    bool is_record_exists(const QString &id);
    bool is_record_exists(const QUrl &url);

    int get_pos_by_id(QString id);

    void empty(void);
    std::shared_ptr<RecordTable> active_subset(boost::intrusive_ptr<TreeItem> start_item);

    void move_up(int pos);
    void move_dn(int pos);

    void to_encrypt(void);
    void to_decrypt(void);

    static void editor_load_callback(QObject *editor, QString &loadText);
    static void editor_save_callback(QObject *editor, QString saveText);

    int  work_pos(void);
    void work_pos(int pos);

    void check_and_create_textfile(int pos, QString fullFileName);
    QList< boost::intrusive_ptr<Record> > &records() {return _record_data;}

    bool crypt() {return _is_crypt;}
    void crypt(const bool _is_crypt);   // {this->_is_crypt = _is_crypt;}

    // Function to populate a table of DOM-document // Функция заполнения таблицы из DOM-документа
    void import_from_dom(const QDomElement &dom_model);

protected:

    // Initialize the data table based on the transmitted item DOM- // Первичное заполнение таблицы конечных записей
    //    void init(QDomElement domModel);

private:

    bool                                _is_crypt = false;

    // Link to the branch that owns the table   // Ссылка на ветку, которой принадлежит данная таблица
    //    boost::intrusive_ptr<TreeItem>      _tree_item;

    // Table entries (in the normal form contains only "light" objects records) // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    QList<boost::intrusive_ptr<Record>>      _record_data;

    // Number of entries with which the user works  // Номер записи, с которой работал пользователь
    int                                 _workpos = -1;

    friend class MetaEditor;
    friend class TreeItem;
};


#endif /* __RECORDTABLEDATA_H__ */
