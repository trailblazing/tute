#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "models/record_table/Record.h"
#include "models/attach_table/AttachTableData.h"
#include "ClipboardRecords.h"


ClipboardRecords::ClipboardRecords(void) : QMimeData()
{
    init();
}


ClipboardRecords::~ClipboardRecords(void)
{

}


// Подготовка объекта для загрузки данных
void ClipboardRecords::init(void)
{
    if(_records._record_table.size() > 0)
        _records._record_table.clear();

    _clipboard_records_format.clear();
    _clipboard_records_format << "mytetra/records";
}


// Очистка объекта
void ClipboardRecords::clear(void)
{
    init();
    _clipboard_records_format << "";
}


void ClipboardRecords::add_record(std::shared_ptr<Record> record)
{
    if(!_records._record_table.contains(record)) _records._record_table << record;
}


// Печать информации о содержимом записи
void ClipboardRecords::print(void) const
{
    QListIterator< std::shared_ptr<Record> > list(_records._record_table);

    // Перебор записей
    while(list.hasNext()) {
        std::shared_ptr<Record> record = list.next();

        qDebug() << record->text_from_fat();

        // Перебор полей в записи
        QMap<QString, QString> fieldList = record->natural_field_list();
        QMapIterator<QString, QString> currentField(fieldList);

        while(currentField.hasNext()) {
            currentField.next();
            qDebug() << currentField.key() << ": " << currentField.value();
        }

        // Перебор информации о праттаченных файлах в записи
        if(record->attach_table()->size() > 0)
            record->attach_table()->print();
    }
}


// Количество хранимых записей
int ClipboardRecords::size(void) const
{
    return _records._record_table.size();
}


std::shared_ptr<Record> ClipboardRecords::record(int n) const
{
    if(n < _records._record_table.size())
        return _records._record_table.at(n);
    else {
        critical_error("In ClipboardRecords::getRecord() unavailable number " + QString::number(n));
        return std::make_shared<Record>();
    }
}


// Получение текста записи с указанным номером
QString ClipboardRecords::record_text(int n) const
{
    if(n < _records._record_table.size())
        return _records._record_table.at(n)->text_from_fat();
    else {
        critical_error("In ClipboardRecords::getRecordText() unavailable number " + QString::number(n));
        return QString();
    }
}


// Получение полей записи с указанным номером
QMap<QString, QString> ClipboardRecords::record_field_list(int n) const
{
    if(n < _records._record_table.size())
        return _records._record_table.at(n)->natural_field_list();
    else {
        critical_error("In ClipboardRecords::getRecordFieldTable() unavailable number " + QString::number(n));
        return QMap<QString, QString>();
    }
}


// Получение информации о приаттаченных файлах для записи с указанным номером
AttachTableData ClipboardRecords::record_attach_table(int n) const
{
    if(n < _records._record_table.size())
        return *record(n)->attach_table();
    else {
        critical_error("In ClipboardRecords::getRecordAttachTable() unavailable number " + QString::number(n));
        return AttachTableData();
    }
}


// Получение файлов картинок
QMap<QString, QByteArray> ClipboardRecords::record_picture_files(int n) const
{
    if(n < _records._record_table.size())
        return _records._record_table.at(n)->getPictureFiles();
    else {
        critical_error("In ClipboardRecords::getRecordPictureFiles() unavailable number " + QString::number(n));
        return QMap<QString, QByteArray>();
    }
}


// Получение приаттаченных файлов
/*
AttachTableData *ClipboardRecords::getRecordAttachFiles(int n) const
{
 if(n<records.table.size())
  return records.table.at(n).getAttachFiles();
 else
  {
   critical_error("In ClipboardRecords::getRecordAttachFiles() unavailable number "+QString::number(n));
  }
}
*/

QStringList ClipboardRecords::formats() const
{
    return _clipboard_records_format;
}


QVariant ClipboardRecords::retrieveData(const QString &format, QVariant::Type preferredType) const
{
    Q_UNUSED(preferredType);

    if(format == _clipboard_records_format[0]) {
        QVariant v;
        v.setValue(_records);
        return v;
    }

    return 0;
}

