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
struct Linker;
//extern const int add_new_record_to_end;

//#define ADD_NEW_RECORD_TO_END 0
//#define ADD_NEW_RECORD_BEFORE 1
//#define ADD_NEW_RECORD_AFTER 2

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

class ItemsFlat {

public:



    //    RecordTable(boost::intrusive_ptr<TreeItem> _tree_item); // a kind of copy constructor
    ItemsFlat(const QDomElement &_dom_element, const bool _is_crypt = false);
    ItemsFlat(const bool _is_crypt = false);

#ifdef _with_record_table
    ItemsFlat(const ItemsFlat &obj);
    ItemsFlat &operator =(const ItemsFlat &obj);
#endif

    virtual ~ItemsFlat();

    // Получение текста указанной записи
    QString text(int pos) const;

    // Установка текста и картинок указанной записи
    void set_text_and_pictures(int pos,
                               const QString &text,
                               const QMap<QString, QByteArray> &picturesFiles = (QMap<QString, QByteArray>()));

    //    // Получение значения указанного поля для указанного элемента
    //    QString field(int pos, QString name) const;

    //    // Установка значения указанного поля для указанного элемента
    //    void field(int pos, QString name, QString value);

    // Получение образа записи
    boost::intrusive_ptr<TreeItem> item_lite(int pos) const;
    boost::intrusive_ptr<TreeItem> item_fat(int pos);

    boost::intrusive_ptr<Linker> linker_direct(int pos)const;

    QList<boost::intrusive_ptr<TreeItem>> items_direct(const QString &name)const;
    QList<boost::intrusive_ptr<TreeItem>> items_direct(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_substitute_condition)const;



    boost::intrusive_ptr<TreeItem> item_direct(int pos) const;
    //    boost::intrusive_ptr<TreeItem> item_direct(int pos) const;
    //    boost::intrusive_ptr<TreeItem> item_direct(const QString &id) const;
    boost::intrusive_ptr<TreeItem> item_direct(const QUrl &url) const;
    //    boost::intrusive_ptr<TreeItem> item_direct(const QUrl &url) const;
    //    boost::intrusive_ptr<TreeItem> item_direct(boost::intrusive_ptr<TreeItem> item) const;
    boost::intrusive_ptr<TreeItem> item_direct(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_substitute_condition)const;
    //    int find_list(boost::intrusive_ptr<TreeItem> item)const;
    //    boost::intrusive_ptr<TreeItem> item_direct(const QString &id)const;


    //    int index_direct(boost::intrusive_ptr<TreeItem> item)const;
    //    void tree_item(boost::intrusive_ptr<TreeItem> tree_item);

    //    // Удаление всех элементов таблицы конечных записей
    //    void delete_list_items(void);

    // Количество записей в таблице данных
    int count_direct(void) const;

    //    // Function to populate a table of DOM-document // Функция заполнения таблицы из DOM-документа
    void dom_to_itemsflat(const QDomElement &dom_model);

    // Функция создания DOM-документа из данных таблицы конечных записей
    //    QDomElement export_to_dom(QDomDocument *doc) const;
    QDomElement dom_from_activated_itemsflat() const;

    QDomElement dom_from_itemsflat() const;
    //    QDomElement dom_from_itemsflat(std::shared_ptr<QDomDocument> doc) const;

    //    // Получение ссылки на объект ветки, которой принадлежит таблица
    //    boost::intrusive_ptr<TreeItem> tree_item(void);




    void fields(int pos, QMap<QString, QString> edit_fields);
    boost::intrusive_ptr<TreeItem> contains_direct(const boost::intrusive_ptr<TreeItem> &&_item)const;
    boost::intrusive_ptr<TreeItem> contains_direct(const boost::intrusive_ptr<const Linker> &&_item_linker)const;

    //    bool remove(int i);
    //    bool remove(QString id);
    boost::intrusive_ptr<TreeItem> delete_permanent_recursive(boost::intrusive_ptr<Linker> remove_linker, std::function<bool(boost::intrusive_ptr<const TreeItem>)> condition);


    //    int sibling_order(QString id) const;
    //    int sibling_order(boost::intrusive_ptr<TreeItem> it)const;
    int sibling_order(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_equal)const;

    void clear(void);
    //    boost::intrusive_ptr<TreeItem> active_subset();

    void move_up(int pos);
    void move_dn(int pos);

    void to_encrypt(void);
    void to_decrypt(void);

    static void editor_load_callback(QObject *editor, QString &loadText);
    static void editor_save_callback(QObject *editor, QString saveText);

    int  work_pos(void) const;
    void work_pos(int pos);

    void check_and_create_textfile(int pos, QString fullFileName);
    //    QList< boost::intrusive_ptr<TreeItem> > &items() {return _child_items;}

    bool crypt() {return _is_crypt;}
    void crypt(const bool _is_crypt);   // {this->_is_crypt = _is_crypt;}




    //    int direct_count()const {return _child_items.size();}
    //    void parent();
    boost::intrusive_ptr<TreeItem> delete_permanent(const std::function<bool(boost::intrusive_ptr<const Linker>)> &_equal);
    void remove_duplicated();
    bool is_empty() const;
    QList<boost::intrusive_ptr<TreeItem> > delete_permanent_recursive(int position, int count);
    void traverse(const std::function< void(boost::intrusive_ptr<Linker>)> &operation);
    QList<boost::intrusive_ptr<Linker>> child_linkers();


protected:

    // Initialize the data table based on the transmitted item DOM- // Первичное заполнение таблицы конечных записей
    //    void init(QDomElement domModel);
    std::function<QDomElement(std::shared_ptr<QDomDocument> doc)> dom_from_itemsflat_impl;

private:



    // Link to the branch that owns the table   // Ссылка на ветку, которой принадлежит данная таблица
    //    boost::intrusive_ptr<TreeItem>      _tree_item;

    //    // Table entries (in the normal form contains only "light" objects records) // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    //    QList<boost::intrusive_ptr<Record>>      _child_items;

    //    QList<boost::intrusive_ptr<TreeItem>>   _child_items;   // Список ссылок на потомков
    QList<boost::intrusive_ptr<Linker>>     _child_linkers;
    //    boost::intrusive_ptr<TreeItem>          _parent_item;   // Ссылка на родителя
    //    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    //    std::shared_ptr<RecordTable>            _record_table;    // = std::make_shared<TableData>();

    // Number of entries with which the user works  // Номер записи, с которой работал пользователь
    bool                                _is_crypt   = false;
    int                                 _workpos    = -1;

    friend class MetaEditor;
    friend class TreeItem;
    friend struct Linker;
};


#endif /* __RECORDTABLEDATA_H__ */
