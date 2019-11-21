#ifndef _CLIPBRECORDS_H_
#define _CLIPBRECORDS_H_

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <QList>
#include <QMap>
#include <QMimeData>
#include <QWidget>

// class Record;
#include "models/record_table/record.h"
// #include "models/tree/TreeItem.h"
#include "models/attach_table/attach_table_data.h"

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class i_t;

// Определяется структура данных набора записей
// которая будет передаваться через буфер обмена
struct CLIPB_RECORDS_STRUCT {
	QList<QDomElement> _child_items;  // boost::intrusive_ptr<TreeItem>
};

Q_DECLARE_METATYPE(CLIPB_RECORDS_STRUCT);

class ClipboardRecords : public QMimeData {
#if QT_VERSION == 0x050600
	W_OBJECT(ClipboardRecords)
#else
	Q_OBJECT
#endif

	public:
	ClipboardRecords(void);
	~ClipboardRecords(void);

	void init(void);
	void clear(void);
	void add_record(boost::intrusive_ptr<i_t> record);
	void print(void) const;
	int size(void) const;

	boost::intrusive_ptr<i_t> record(int n) const;
	QString record_text(int n) const;
	QMap<QString, QString> record_field_list(int n) const;
	QMap<QString, QByteArray> record_picture_files(int n) const;
	AttachTableData record_attach_table(int n) const;
	// AttachTableData *getRecordAttachFiles(int n) const;

	// Этот метод QMimeData надо переопределить, так как он виртуальный
	QStringList formats() const;

	protected:
	// Этот метод QMimeData надо переопределить, так как он виртуальный
	QVariant retrieveData(const QString &format, QVariant::Type preferredType) const;

	private:
	CLIPB_RECORDS_STRUCT
	_records;                               // Данные, которые передаются через буфер обмена
	QStringList _clipboard_records_format;  // Строка с идентификатором формата
};

#endif  // _CLIPBRECORDS_H_
