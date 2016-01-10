#ifndef TREEITEM_H
#define TREEITEM_H


#include <memory>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "models/record_table/RecordTable.h"

class TreeItem
//        : public std::enable_shared_from_this<TreeItem>
    : public boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter> {
public:

    typedef boost::sp_adl_block::thread_safe_counter counter_type; // boost::intrusive_ref_counter<TreeItem>::counter_type

    typedef boost::intrusive_ref_counter<TreeItem, boost::thread_safe_counter> counter;

    TreeItem(QMap<QString, QString> _field_data, boost::intrusive_ptr<TreeItem> _parent_item, std::shared_ptr<RecordTable> _table_data);  // = nullptr


    ~TreeItem();

    // Возвращение ссылки на потомка, который хранится в списке childItems
    // под указанным номером
    boost::intrusive_ptr<TreeItem> child(int number);

    // Возвращение количества потомков (т.е. количество записей в списке childItems)
    int child_count() const;

    // Возвращение количества полей, которые хранит данный элемент.
    int fieldCount() const;

    // Получение значения поля по имени
    QString field(QString name);

    // Получение всех полей данных
    QMap<QString, QString> all_fields();

    // Получение всех полей данных напрямую, без преобразований
    QMap<QString, QString> all_fields_direct();

    // Заполнение указанного поля
    void field(QString name, QString value);

    // Заполнение указанного поля данными напрямую, без преобразований
    void set_field_direct(QString name, QString value);

    // Добавление потомка (потомков) к текущему элементу
    // position - после какой позиции массива childItems вставить
    // count - сколько потомков вставить (обычно 1, но можно и несколько)
    // columns - сколько столбцов должен содержать потомок
    bool insert_children(int position, int count, int columns);

    // Добавление нового подчиненного элемента
    // в конец списка подчиненных элементов
    bool add_children(void);

    // Возвращение ссылки на родительский элемент
    boost::intrusive_ptr<TreeItem> parent();

    // Удаление потомков, начиная с позиции position массива childItems
    bool remove_children(int position, int count);

    // Удаление всех потомков элемента
    void remove_all_children();

    // Возвращает номер, под которым данный объект хранится
    // в массиве childItems своего родителя
    int child_index() const;

    bool move_up(void);
    bool move_dn(void);

    // Возвращает id путь (список идентификаторов от корня до текущего элемента)
    QStringList path(void);

    // Возвращает путь в виде названий веток дерева
    QStringList path_as_name(void);

    QString path_as_name_with_delimiter(QString delimeter);

    // Возвращает набор значений указанного поля для пути от корня к ветке
    QStringList path_as_field(QString fieldName);

    // Возвращает массив путей всех подветок, которые содержит ветка
    QList<QStringList> all_children_path(void);

    // Возвращает набор значений указанного поля для подветок
    QList<QStringList> all_children_path_as_field(QString fieldName);

    // Получение идентификатора элемента
    QString id();
    QString name();

    // Получение идентификатора родительской ветки
    QString parent_id();

    // Шифрация данной ветки и всех подветок
    void to_encrypt(void);

    // Расшифровка данной ветки и всех подветок
    void to_decrypt(void);

    // Взятие количества записей в таблице конечных записей, "промежуточный" метод
    int row_count(void);

    // Удаление всех элементов в таблице конечных записей, "промежуточный" метод
    void clear_tabledata(void);

    // Преобразование таблицы конечных записей в DOM представление, "промежуточный" метод
    // Convert the table of final entries in the DOM representation "intermediate" method
    QDomElement export_to_dom(std::shared_ptr<QDomDocument> doc);
    QDomElement export_to_dom();

    // Взятие ссылки на данные конечных записей
    std::shared_ptr<RecordTable> tabledata(void);
    void tabledata(std::shared_ptr<RecordTable> _table_data);
    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    void tabledata(QDomElement i_dom_element);

private:
    bool remove_children_link(int position, int count);

    void empty(void);

    // QList<QStringList> get_all_children_path_recurse(TreeItem *item,int mode);
    QList<QStringList> all_children_path_as_field(boost::intrusive_ptr<TreeItem> item, QString fieldName, int mode);

    bool is_fieldname_available(QString name) const;
    QStringList fieldname_availablelist(void) const;
    QStringList fieldname_for_cryptlist(void) const;

    QList<boost::intrusive_ptr<TreeItem>>    _child_items;   // Список ссылок на потомков
    boost::intrusive_ptr<TreeItem>           _parent_item;   // Ссылка на родителя

    // Таблица инфополей данной ветки
    QMap<QString, QString>      _field_data;

    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    std::shared_ptr<RecordTable>  _record_data;    // = std::make_shared<TableData>();
    //    friend void intrusive_ptr_add_ref(TreeItem *px);
    //    friend void intrusive_ptr_release(TreeItem *px);
    //    friend void boost::sp_adl_block::intrusive_ptr_add_ref< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;
    //    friend void boost::sp_adl_block::intrusive_ptr_release< TreeItem, counter_type >(const boost::intrusive_ref_counter< TreeItem, counter_type > *p) BOOST_NOEXCEPT;
};

//inline void intrusive_ptr_add_ref(TreeItem *px){boost::sp_adl_block::intrusive_ptr_add_ref(px);}
//inline void intrusive_ptr_release(TreeItem *px){boost::sp_adl_block::intrusive_ptr_release(px);}

#endif
