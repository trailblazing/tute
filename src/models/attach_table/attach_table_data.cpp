#include <QDebug>
#include <QDomElement>
#include <QFileInfo>

#include "attach.h"
#include "attach_table_data.h"
#include "attach_table_model.h"
#include "main.h"
#include "models/record_table/record.h"

AttachTableData::AttachTableData(boost::intrusive_ptr<Record> _record)
	: _lite_flag(true)
	  , _record(_record)
	  , _related_attach_table_model(nullptr){
	_attach_table.clear();
}

// Конструктор копирования

AttachTableData::AttachTableData(const AttachTableData &obj)
	: _lite_flag(obj._lite_flag)
	  , _record(nullptr)
	  , _related_attach_table_model(nullptr){
	_attach_table = obj._attach_table; // "Глубокое" копирование таблицы прикрепляемых файлов
	// _record = obj._record; // А ссылка на запись просто копируется

	// if(obj._related_attach_table_model)_related_attach_table_model = obj._related_attach_table_model;
}

AttachTableData &AttachTableData::operator =(const AttachTableData &obj){
	if(this != &obj){
		if(_lite_flag != obj._lite_flag) _lite_flag = obj._lite_flag;
		_attach_table = obj._attach_table; // "Глубокое" копирование таблицы прикрепляемых файлов

		// if(obj._record)_record = obj._record; // А ссылка на запись просто копируется

		// if(obj._related_attach_table_model)_related_attach_table_model = obj._related_attach_table_model;
	}
	return *this;
}

//// Пустой конструктор, он требуется для Q_DECLARE_METATYPE в QMimeData
// AttachTableData::AttachTableData()
// :
// _lite_flag(true)
// , _record(nullptr)
// , _related_attach_table_model(nullptr)
// {
// _attach_table.clear();
// }

AttachTableData::~AttachTableData()
{}

// На вход этого метода подается тег <files>
void AttachTableData::dom_to_data(QDomElement iDomElement){
	// Первый файл
	QDomElement currentFile = iDomElement.firstChildElement("file");
	// Перебор тегов <file ...>
	while(!currentFile.isNull()){
		Attach attach(this);

		// Аттач добавляется в таблицу приаттаченных файлов и размещается в памяти
		_attach_table.append(attach);

		// После размещения в памяти инициализируется начальными данными
		_attach_table.last().dom_to_data(currentFile);

		currentFile = currentFile.nextSiblingElement("file");
	}
}

QDomElement AttachTableData::dom_from_data(std::shared_ptr<QDomDocument> doc) const {
	// Если у записи нет таблицы приаттаченных файлов
	if(_attach_table.size() == 0) return QDomElement();
	QDomElement attachTableDom = doc->createElement("files");
	// Пробегаются все приаттаченные файлы
	for(int i = 0; i < _attach_table.size(); i++) attachTableDom.appendChild(_attach_table.at(i).dom_from_data(doc)); // К элементу files прикрепляются элементы file

	return attachTableDom;
}

void AttachTableData::related_attach_table_model(AttachTableModel *model){
	// Запоминание указателя на модель
	_related_attach_table_model = model;

	// В модели устанавливается указатель на текущие данные (перекрестная ссылка)
	_related_attach_table_model->setData(QModelIndex(), QVariant::fromValue(this), ATTACHTABLE_ROLE_TABLE_DATA_ONLY);
}

void AttachTableData::related_attach_table_model_only(AttachTableModel *model){
	_related_attach_table_model = model;
}

bool AttachTableData::is_empty() const {
	if(_attach_table.size() == 0) return true;
	else return false;
}

bool AttachTableData::is_lite() const {
	return _lite_flag;
}

void AttachTableData::record(boost::intrusive_ptr<Record> record){
	_record = record; // Запоминается ссылка на запись, которой принадлежит данная таблица файлов
}

void AttachTableData::clear(){
	_attach_table.clear();
	_record = nullptr;
	_lite_flag = true;
}

int AttachTableData::size() const {
	return _attach_table.size();
}

// Получение объекта аттача
Attach AttachTableData::attach(QString id){
	int row = row_by_id(id);
	if(row < 0) critical_error("Attach with ID: " + id + " not found");
	return _attach_table.at(row);
}

// Добавление аттача
void AttachTableData::add_attach(Attach attach){
	if(_related_attach_table_model != nullptr) _related_attach_table_model->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_BEGIN_RESET_MODEL);
	// Аттач добавляется в таблицу приаттаченных файлов
	_attach_table.append(attach);
	if(_related_attach_table_model != nullptr) _related_attach_table_model->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_END_RESET_MODEL);
}

// Изменение данных аттача
void AttachTableData::attach(QString id, Attach iAttach){
	int row = row_by_id(id);
	if(row < 0) return;
	_attach_table[row] = iAttach;
}

// Удаление аттача
void AttachTableData::delete_attach(QString id){
	int row = row_by_id(id);
	if(row < 0) return;
	// Сначала уничтожается файл
	_attach_table[row].removeFile();
	// Если связанной модели нет
	if(_related_attach_table_model == nullptr){
		_attach_table.removeAt(row); // Просто удаляется запись в данных
		return;
	}else{
		// Иначе связанная модель есть

		_related_attach_table_model->setData(QModelIndex(), row, ATTACHTABLE_COMMAND_BEGIN_REMOVE_ROW);
		_attach_table.removeAt(row); // Удаляется запись в данных
		_related_attach_table_model->setData(QModelIndex(), QVariant(), ATTACHTABLE_COMMAND_END_REMOVE_ROW);
	}
}

int AttachTableData::row_by_id(QString id){
	for(int i = 0; i < _attach_table.size(); i++)
		if(_attach_table.at(i).getField("id") == id) return i;
	return -1;
}

QString AttachTableData::id_by_row(int row){
	return _attach_table.at(row).getField("id");
}

// Видимое имя файла без пути
QString AttachTableData::file_name(int row){
	return _attach_table.at(row).getField("fileName");
}

// Видимое имя файла без пути по Id
QString AttachTableData::file_name_by_id(QString id){
	int row = row_by_id(id);
	if(row >= 0) return file_name(row);
	else return "";
}

// Внутреннее имя файла без пути
QString AttachTableData::inner_file_name(int row){
	return _attach_table.at(row).getInnerFileName();
}

// Внутреннее имя файла без пути по Id
QString AttachTableData::inner_file_name_by_id(QString id){
	int row = row_by_id(id);
	if(row >= 0) return inner_file_name(row);
	else return "";
}

// Внутреннее имя файла с путем
QString AttachTableData::full_inner_file_name(int row){
	return _attach_table.at(row).getFullInnerFileName();
}

// Внутреннее имя файла с путем по Id
QString AttachTableData::full_inner_file_name_by_id(QString id){
	int row = row_by_id(id);
	if(row >= 0) return full_inner_file_name(row);
	else return "";
}

// Внутреннее имя файла с абсолютным путем
QString AttachTableData::absolute_inner_file_name(int row){
	return _attach_table.at(row).getAbsoluteInnerFileName();
}

// Внутреннее имя файла с абсолютным путем по Id
QString AttachTableData::absolute_inner_file_name_by_id(QString id){
	int row = row_by_id(id);
	if(row >= 0) return absolute_inner_file_name(row);
	else return "";
}

qint64 AttachTableData::file_size(int row){
	return _attach_table.at(row).getFileSize();
}

// Пачать содержимого таблицы конечных файлов
void AttachTableData::print(){
	for(int i = 0; i < _attach_table.size(); ++i) qDebug() << "File: " << _attach_table.at(i).getField("id") << " Type: " << _attach_table.at(i).getField("type");
}

void AttachTableData::switch_to_lite(){
	// Переключение возможно только из полновесного состояния
	if(_lite_flag == true) critical_error("Can't switch attach table to lite state");
	for(int i = 0; i < _attach_table.size(); ++i){
		// Тяжелые данные сохраняются на диск
		if(_attach_table[i].getField("type") == "file") _attach_table[i].pushFatDataToDisk();
		_attach_table[i].switchToLite();
	}
	_lite_flag = true;
}

void AttachTableData::switch_to_fat(){
	// Переключение возможно только из легкого состояния
	if(_lite_flag != true) critical_error("Unavailable switching attach table to fat state");
	for(int i = 0; i < _attach_table.size(); ++i){
		_attach_table[i].switchToFat();
		// Тяжелые данные засасываются с диска в память
		if(_attach_table[i].getField("type") == "file") _attach_table[i].popFatDataFromDisk();
	}
	_lite_flag = false;
}

bool AttachTableData::is_record_crypt(){
	if(_record->field<crypt_type>() == "1") return true;
	else return false;
}

void AttachTableData::encrypt(unsigned int area){
	for(int i = 0; i < _attach_table.size(); ++i) _attach_table[i].encrypt(area);
}

void AttachTableData::decrypt(unsigned int area){
	for(int i = 0; i < _attach_table.size(); ++i) _attach_table[i].decrypt(area);
}

void AttachTableData::save_attach_files_to_directory(QString dirName){
	for(int i = 0; i < _attach_table.size(); ++i)
		if(_attach_table.at(i).getField("type") == "file")                                                                                                                                  // Сохраняются только файлы, не линки
			if(!_attach_table.at(i).isLite()) _attach_table[i].pushFatDataToDirectory(dirName);
}

// Обновление ссылок на таблицу аттачей внутри объектов-аттачей
void AttachTableData::update_attach_table_back_link(){
	for(int i = 0; i < _attach_table.size(); ++i) _attach_table[i].setParentTable(this);
}

std::shared_ptr<AttachTableData> AttachTableData::merge(std::shared_ptr<AttachTableData> cut){
	std::shared_ptr<AttachTableData> result;
	for(auto i : cut->_attach_table){
		bool found = false;
		for(auto j : _attach_table){
			if(j == i){
				found = true;
				break;
			}
		}
		if(!found) _attach_table.append(i);
	}
	return result;
}
