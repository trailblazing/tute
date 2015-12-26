#ifndef TREEITEM_H
#define TREEITEM_H


#include <memory>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>

#include "models/record_table/TableData.h"

class TreeItem : public std::enable_shared_from_this<TreeItem> {
public:
    TreeItem(const QMap<QString, QString> &data, std::shared_ptr<TreeItem> parent
            // = nullptr
            );
    ~TreeItem();

    // Возвращение ссылки на потомка, который хранится в списке childItems
    // под указанным номером
    std::shared_ptr<TreeItem> child(int number);

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
    void set_field(QString name, QString value);

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
    std::shared_ptr<TreeItem> parent();

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

    // Получение идентификатора родительской ветки
    QString parent_id();

    // Шифрация данной ветки и всех подветок
    void to_encrypt(void);

    // Расшифровка данной ветки и всех подветок
    void to_decrypt(void);

    // Первичное заполнение таблицы конечных записей, "промежуточный" метод
    void table_init(QDomElement domModel);

    // Взятие количества записей в таблице конечных записей, "промежуточный" метод
    int row_count(void);

    // Удаление всех элементов в таблице конечных записей, "промежуточный" метод
    void table_clear(void);

    // Преобразование таблицы конечных записей в DOM представление, "промежуточный" метод
    QDomElement export_to_dom(QDomDocument *doc);

    // Взятие ссылки на данные конечных записей
    std::shared_ptr<TableData> tabledata(void);

private:
    bool remove_children_link(int position, int count);

    void empty(void);

    // QList<QStringList> get_all_children_path_recurse(TreeItem *item,int mode);
    QList<QStringList> all_children_path_as_field(std::shared_ptr<TreeItem> item, QString fieldName, int mode);

    bool is_fieldname_available(QString name) const;
    QStringList fieldname_availablelist(void) const;
    QStringList fieldname_for_cryptlist(void) const;

    QList<std::shared_ptr<TreeItem>>    _child_items;   // Список ссылок на потомков
    std::shared_ptr<TreeItem>           _parent_item;   // Ссылка на родителя

    // Таблица инфополей данной ветки
    QMap<QString, QString> _fieldtable;

    // Each branch can contain a table of final entries // Каждая ветка может содержать таблицу конечных записей
    std::shared_ptr<TableData> _recordtable = std::make_shared<TableData>();
};

#endif
