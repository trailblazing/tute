#ifndef _CLIPBRECORDS_H_
#define _CLIPBRECORDS_H_

#include <QWidget>
#include <QMimeData>
#include <QMap>
#include <QList>

// class Record;
#include "models/record_table/Record.h"
#include "models/attach_table/AttachTableData.h"

// Определяется структура данных набора записей
// которая будет передаваться через буфер обмена
struct CLIPB_RECORDS_STRUCT {
    QList< boost::intrusive_ptr<Record> > _record_table;
};

Q_DECLARE_METATYPE(CLIPB_RECORDS_STRUCT);


class ClipboardRecords : public QMimeData {
    Q_OBJECT

public:
    ClipboardRecords(void);
    ~ClipboardRecords(void);

    void init(void);
    void clear(void);
    void add_record(boost::intrusive_ptr<Record> record);
    void print(void) const;
    int size(void) const;

    boost::intrusive_ptr<Record> record(int n) const;
    QString record_text(int n) const;
    QMap<QString, QString> record_field_list(int n) const;
    QMap<QString, QByteArray > record_picture_files(int n) const;
    AttachTableData record_attach_table(int n) const;
    // AttachTableData *getRecordAttachFiles(int n) const;

    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QStringList formats() const;

protected:
    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QVariant retrieveData(const QString &format,QVariant::Type preferredType) const;

private:
    CLIPB_RECORDS_STRUCT    _records; // Данные, которые передаются через буфер обмена
    QStringList             _clipboard_records_format; // Строка с идентификатором формата

};

#endif // _CLIPBRECORDS_H_
