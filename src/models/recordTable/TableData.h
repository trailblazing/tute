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

class TreeItem;
class Record;
class MetaEditor;

class TableData {

public:
    TableData(void);
    virtual ~TableData();

    // Получение текста указанной записи
    QString text(int pos);

    // Установка текста и картинок указанной записи
    void set_text_and_pictures(int pos,
                            const QString &text,
                            const QMap<QString, QByteArray> &picturesFiles = (QMap<QString, QByteArray>()));

    // Получение значения указанного поля для указанного элемента
    QString field(QString name, int pos) const;

    // Установка значения указанного поля для указанного элемента
    void field(QString name, QString value, int pos);

    // Получение образа записи
    std::shared_ptr<Record> record_lite(int pos);
    std::shared_ptr<Record> record_fat(int pos);
    std::shared_ptr<Record> record(int pos);
    std::shared_ptr<Record> find(const QUrl &url);
    std::shared_ptr<Record> find(const std::shared_ptr<Record> r);

    // Первичное заполнение таблицы конечных записей
    void init(std::shared_ptr<TreeItem> item, QDomElement domModel);

    // Удаление всех элементов таблицы конечных записей
    void delete_all_records(void);

    // Количество записей в таблице данных
    int size(void) const;

    // Функция создания DOM-документа из данных таблицы конечных записей
    QDomElement dom_from_data(QDomDocument *doc) const;

    // Получение ссылки на объект ветки, которой принадлежит таблица
    std::shared_ptr<TreeItem> item(void);

    int insert_new_record(int mode
                        , int pos
                        , std::shared_ptr<Record> record);

    void edit_record_fields(int pos
                          , QMap<QString, QString> editFields);

    void delete_record(int i);
    void delete_record_by_id(QString id);

    bool is_record_exists(const QString &id);
    bool is_record_exists(const QUrl &url);

    int get_pos_by_id(QString id);

    void empty(void);

    void move_up(int pos);
    void move_dn(int pos);

    void switch_to_encrypt(void);
    void switch_to_decrypt(void);

    static void editor_load_callback(QObject *editor,
                                   QString &loadText);

    static void editor_save_callback(QObject *editor,
                                   QString saveText);

    int  work_pos(void);
    void work_pos(int pos);

    void check_and_create_textfile(int pos, QString fullFileName);


private:

    // Функция заполнения таблицы из DOM-документа
    // Function to populate a table of DOM-document
    void data_from_dom(QDomElement *domModel);

    // Таблица записей (в нормальном виде содержит только "легкие" объекты записей)
    // Table entries (in the normal form contains only "light" objects records)
    QList< std::shared_ptr<Record> > _tabledata;

    // Ссылка на ветку, которой принадлежит данная таблица
    // Link to the branch that owns the table
    std::shared_ptr<TreeItem> _treeitem;

    // Номер записи, с которой работал пользователь
    // Number of entries with which the user works
    int _workpos;
    friend class MetaEditor;
};


#endif /* __RECORDTABLEDATA_H__ */
