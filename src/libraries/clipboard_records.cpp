
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QBoxLayout>
#include <QToolButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QMimeData>
#include <QtDebug>

#include "main.h"
#include "models/record_table/record.h"
#include "models/tree/tree_item.h"
#include "models/attach_table/attach_table_data.h"
#include "clipboard_records.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"



#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ClipboardRecords)
#endif




ClipboardRecords::ClipboardRecords(void) : QMimeData(){
    init();
}

ClipboardRecords::~ClipboardRecords(void)
{}

// Подготовка объекта для загрузки данных
void ClipboardRecords::init(void){
    if(_records._child_items.size() > 0)_records._child_items.clear();
    _clipboard_records_format.clear();
    _clipboard_records_format << "hapnote/records";
}

// Очистка объекта
void ClipboardRecords::clear(void){
    init();
    _clipboard_records_format << "";
}

void ClipboardRecords::add_record(boost::intrusive_ptr<TreeItem> record){
    auto get_id = [](QDomElement _dom_element) -> id_value {
	    id_value id("");
		// Получение списка всех атрибутов текущего элемента
	    QDomNamedNodeMap attList;
	    attList = _dom_element.attributes();

		// Перебор атрибутов в списке и добавление их в запись
	    int i;
	    for(i = 0; i < attList.count(); i ++){
		QDomAttr attcurr = attList.item(i).toAttr();

		QString name	= attcurr.name();
		QString value	= attcurr.value();
		if(name == "id"){id = id_value(value);break;}
		//            this->natural_field_source(name, value);

		// Распечатка считанных данных в консоль
		// qDebug() << "Read record attr " << name << value;
	    }
	    assert(id != "");

	    return id;
	};

    bool found = false;
    for(auto el : _records._child_items){
	auto id = get_id(el);
	if(id == record->id()){found = true;break;}
    }
    if(! found)	// if(!_records._child_items.contains(record))
		_records._child_items << record->dom_from_treeitem();	// dom_from_record();
}

// Печать информации о содержимом записи
void ClipboardRecords::print(void) const {
    QList<boost::intrusive_ptr<TreeItem> > source_list;
    for(auto el : _records._child_items){
	boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));	// new TreeItem(nullptr)
	it->dom_to_records(el);
	source_list << it;
    }
    QListIterator< boost::intrusive_ptr<TreeItem> > list(source_list);
	// Перебор записей
    while(list.hasNext()){
	boost::intrusive_ptr<TreeItem> record = list.next();

	qDebug() << record->text_from_fat();

	// Перебор полей в записи
	QMap<QString, QString>		fieldList = record->natural_field_list();
	QMapIterator<QString, QString>	currentField(fieldList);
	while(currentField.hasNext()){
	    currentField.next();
	    qDebug() << currentField.key() << ": " << currentField.value();
	}
	// Перебор информации о праттаченных файлах в записи
	if(record->attach_table()->size() > 0)record->attach_table()->print();
    }
}

// Количество хранимых записей
int ClipboardRecords::size(void) const {
    return _records._child_items.size();
}

boost::intrusive_ptr<TreeItem> ClipboardRecords::record(int n) const {
    if(n < _records._child_items.size()){
	boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));
	it->dom_to_records(_records._child_items.at(n));	// dom_to_record
	return it;
    }else{
	critical_error("In ClipboardRecords::getRecord() unavailable number " + QString::number(n));

	return boost::intrusive_ptr<TreeItem>(nullptr);
    }
}

// Получение текста записи с указанным номером
QString ClipboardRecords::record_text(int n) const {
    if(n < _records._child_items.size()){
	boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));
	it->dom_to_records(_records._child_items.at(n));

	return it->text_from_fat();
    }else{
	critical_error("In ClipboardRecords::getRecordText() unavailable number " + QString::number(n));

	return QString();
    }
}

// Получение полей записи с указанным номером
QMap<QString, QString> ClipboardRecords::record_field_list(int n) const {
    if(n < _records._child_items.size()){
	boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));
	it->dom_to_records(_records._child_items.at(n));

	return it->natural_field_list();
    }else{
	critical_error("In ClipboardRecords::getRecordFieldTable() unavailable number " + QString::number(n));

	return QMap<QString, QString>();
    }
}

// Получение информации о приаттаченных файлах для записи с указанным номером
AttachTableData ClipboardRecords::record_attach_table(int n) const {
    if(n < _records._child_items.size())return *record(n)->attach_table();
    else{
	critical_error("In ClipboardRecords::getRecordAttachTable() unavailable number " + QString::number(n));

	return AttachTableData();
    }
}

// Получение файлов картинок
QMap<QString, QByteArray> ClipboardRecords::record_picture_files(int n) const {
    if(n < _records._child_items.size()){
	boost::intrusive_ptr<TreeItem> it(TreeItem::dangle_instance(QMap<QString, QString>()));
	it->dom_to_records(_records._child_items.at(n));

	return it->picture_files();
    }else{
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

QStringList ClipboardRecords::formats() const {
    return _clipboard_records_format;
}

QVariant ClipboardRecords::retrieveData(const QString &format, QVariant::Type preferredType) const {
    Q_UNUSED(preferredType);
    if(format == _clipboard_records_format[0]){
	QVariant v;
	v.setValue(_records);

	return v;
    }
    return 0;
}

