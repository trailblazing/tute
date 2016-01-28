#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QList>
#include <QMetaType>
#include <QDomElement>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "models/attach_table/Attach.h"
#include "models/record_table/Record.h"


class Record;
class AttachTableModel;

class AttachTableData {
    friend class Attach;

public:
    AttachTableData(boost::intrusive_ptr<Record> record = nullptr);
    // AttachTableData(const AttachTableData &obj);
    //    AttachTableData();
    virtual ~AttachTableData();

    void setupDataFromDom(QDomElement iDomElement);
    QDomElement export_to_dom(std::shared_ptr<QDomDocument> doc) const;

    void record(boost::intrusive_ptr<Record> record);
    void related_attach_table_model(AttachTableModel *model);
    void related_attach_table_model_only(AttachTableModel *model);

    void clear();
    int size() const;

    Attach attach(QString id); // Получение объекта аттача
    void add_attach(Attach attach); // Добавление аттача в таблицу приаттаченных файлов
    void attach(QString id, Attach iAttach); // Изменение данных аттача
    void delete_attach(QString id); // Удаление аттача по идентификатору

    int row_by_id(QString id);
    QString id_by_row(int row);

    QString file_name(int row);
    QString file_name_by_id(QString id);

    QString inner_file_name(int row);
    QString inner_file_name_by_id(QString id);

    QString full_inner_file_name(int row);
    QString full_inner_file_name_by_id(QString id);

    QString absolute_inner_file_name(int row);
    QString absolute_inner_file_name_by_id(QString id);

    qint64 file_size(int row);

    bool is_empty() const;
    bool is_lite() const;

    void switch_to_lite();
    void switch_to_fat();

    void print();

    bool is_record_crypt();
    void encrypt(unsigned int area);
    void decrypt(unsigned int area);

    void save_attach_files_to_directory(QString dirName);

    // Обновление ссылок на таблицу аттачей внутри аттачей
    void update_attach_table_back_link();

protected:

    bool                            _lite_flag;

    // Перечень файлов и их свойств
    QList< Attach >                 _attach_table;

    // Какой записи принадлежит таблица файлов
    boost::intrusive_ptr<Record>    _record;

    AttachTableModel                *_related_attach_table_model;
};

// Регистрация в QVariant типа AttachTableData
Q_DECLARE_METATYPE(AttachTableData);

// Регистрация в QVariant типа *AttachTableData
typedef AttachTableData *AttachTableDataPointer;
Q_DECLARE_METATYPE(AttachTableDataPointer);

#endif // ATTACHTABLEDATA_H
