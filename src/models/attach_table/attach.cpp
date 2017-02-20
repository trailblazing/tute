#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "main.h"

#include "attach.h"
#include "attach_table_data.h"
#include "libraries/crypt/crypt_service.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "models/record_table/record.h"

extern std::shared_ptr<gl_para> gl_paras;

// Конструктор прикрепляемого файла
Attach::Attach(AttachTableData *iParentTable){
	init(iParentTable);
}

// Конструктор прикрепляемого файла с указанием типа прикрепления
// в настоящий момент есть два типа прикрепления - просто файл "file" или линк на файл "link"
Attach::Attach(QString iType, AttachTableData *iParentTable){
	if(!typeAvailableList().contains(iType)) critical_error("Incorrect attach type in Attach constructor: " + iType);
	setField("type", iType);

	init(iParentTable);
}

Attach::~Attach(){
	_file_content.clear();
}

void Attach::init(AttachTableData *iParentTable){
	_lite_flag = true; // By default, a light object    // По-умолчанию легкий объект
	_parent_table = iParentTable;
	_file_content.clear();
}

void Attach::setParentTable(AttachTableData *iParentTable){
	_parent_table = iParentTable;
}

// Допустимые имена полей
QStringList Attach::fieldAvailableList(void) const {
	return QStringList()	<< "id"
	                        << "fileName"
	                        << "link"
	                        << "type" << boost::mpl::c_str<crypt_type>::value;
}

// Имена полей, которые шифруются
QStringList Attach::fieldCryptedList(void) const {
	return QStringList()	<< "fileName"
	                        << "link";
}

// Допустимые типы аттачей
QStringList Attach::typeAvailableList(void) const {
	return QStringList()	<< "file"
	                        << "link";
}

// На вход метода подается тег <file>
void Attach::dom_to_data(QDomElement iDomElement){
	QStringList fieldsName = fieldAvailableList();

	foreach(QString fieldName, fieldsName) // Перебираются имена полей (XML-тегов)
	_fields[fieldName] = iDomElement.attribute(fieldName); // Напрямую устанавливаются значения из XML файла
}

QDomElement Attach::dom_from_data(std::shared_ptr<QDomDocument> doc) const {
	QDomElement elem = doc->createElement("file");

	QStringList fieldsName = fieldAvailableList();

	foreach(QString fieldName, fieldsName) // Перебираются имена полей (XML-тегов)
	if(_fields[fieldName].size() > 0) elem.setAttribute(fieldName, _fields[fieldName]);
	return elem;
}

bool Attach::isEmpty() const {
	// Заполненный аттач не может содержать пустой id
	if(_fields.contains("id") == false || getField("id").length() == 0) return true;
	else return false;
}

bool Attach::isLite() const {
	return _lite_flag;
}

void Attach::switchToLite(){
	// Переключение возможно только из полновесного состояния
	if(_lite_flag == true) critical_error("Can't switch attach to lite state. Attach id: " + getField("id") + " File name: " + getField("fileName"));
	_file_content.clear();

	_lite_flag = true;
}

void Attach::switchToFat(){
	// Переключение возможно только из легкого состояния
	if(_lite_flag != true) critical_error("Unavailable switching attach object to fat state. Attach Id: " + getField("id") + " File name: " + getField("fileName"));
	_lite_flag = false;
}

// Получение значения поля
// Метод возвращает расшифрованные данные, если запись была зашифрована
QString Attach::getField(QString name) const {
	// Если имя поля недопустимо
	if(fieldAvailableList().contains(name) == false) critical_error("Attach::getField() : get unavailable field " + name);
	// ------------------------------------------
	// Проверки и действия для разных типов полей
	// ------------------------------------------
	// Если запрашивается линк на файл
	if(name == "link")
		if(_fields["type"] != "link")                                                                                        // И тип аттача не является линком
			critical_error("Attach::getField() : Can't get link from non-link attach.");
	// -----------------------
	// Получение значения поля
	// -----------------------
	// Если запись зашифрована, но ключ не установлен (т.е. человек не вводил пароль)
	// то расшифровка невозможна
	if(fieldCryptedList().contains(name))
		if(_fields.contains(boost::mpl::c_str<crypt_type>::value))
			if(_fields[boost::mpl::c_str < crypt_type > ::value] == "1")
				if(gl_paras->crypt_key().length() == 0) return QString();
	bool isCrypt = false;
	// Если имя поля принадлежит списку полей, которые могут шифроваться
	// и в наборе полей есть поле crypt
	// и поле crypt установлено в 1
	// и запрашиваемое поле не пустое (пустые данные невозможно расшифровать)
	if(fieldCryptedList().contains(name))
		if(_fields.contains(boost::mpl::c_str<crypt_type>::value))
			if(_fields[boost::mpl::c_str < crypt_type > ::value] == "1")
				if(_fields[name].length() > 0) isCrypt = true;
	// Если поле не подлежит шифрованию
	if(isCrypt == false) return _fields[name]; // Возвращается значение поля
	else return CryptService::decryptString(gl_paras->crypt_key(), _fields[name]); // Поле расшифровывается
}

// Установка значения поля
// Метод принимает незашифрованные данные, и шфирует их если запись является зашифрованой
void Attach::setField(QString name, QString value){
	// Если имя поля недопустимо
	if(fieldAvailableList().contains(name) == false) critical_error("Attach::setField() : set unavailable field " + name);
	// ------------------------------------------
	// Проверки и действия для разных типов полей
	// ------------------------------------------
	// Поле с типом аттача
	if(name == "type")
		if(!typeAvailableList().contains(value)) critical_error("Attach::setField() : Incorrect attach type : " + value);
	// Поле с именем файла
	if(name == "fileName")
		if(getField("type") == "link")                                                                                          // Если устанавливается имя файла для линка
			if(getField("fileName").length() > 0 && value.length() > 0){  // Если имя уже было задано (при создании аттача), и новое имя не пустое
				// Имя файла для линка менять нельзя
				show_message_box(QObject::tr("Can't modify file name for link type attach."));
				return;
			}
	// Поле со ссылкой на файл (содержит путь к файлу, на который указывает линк)
	if(name == "link"){
		QFile tempFile(value);
		// Если файла, на который ссылается линк, не существует
		if(!tempFile.exists()){
			show_message_box(QObject::tr("Bad link. File not found."));
			return;
		}
	}
	// -----------------------
	// Установка значения поля
	// -----------------------

	bool isCrypt = false;
	// Если имя поля принадлежит списку полей, которые могут шифроваться
	// и в наборе полей есть поле crypt
	// и поле crypt установлено в 1
	// и поле не пустое (пустые данные ненужно шифровать)
	if(fieldCryptedList().contains(name))
		if(_fields.contains(boost::mpl::c_str<crypt_type>::value))
			if(_fields[boost::mpl::c_str < crypt_type > ::value] == "1")
				if(value.length() > 0){
					if(gl_paras->crypt_key().length() > 0) isCrypt = true;
					else critical_error("In Attach::setField() can not set data to crypt field " + name + ". Password not setted");
				}
	// Если нужно шифровать, значение поля шифруется
	if(isCrypt == true) value = CryptService::encryptString(gl_paras->crypt_key(), value);
	// Устанавливается значение поля
	_fields.insert(name, value);
}

// Защищенный метод - Установка значения поля напрямую
// Используеся при шифрации-дешифрации данных аттача
// todo: подумать, может быть отказаться от этого метода
void Attach::setFieldSource(QString name, QString value){
	// Если имя поля недопустимо
	if(fieldAvailableList().contains(name) == false) critical_error("Attach::setField() : set unavailable field " + name);
	// Устанавливается значение поля
	_fields.insert(name, value);
}

void Attach::pushFatDataToDisk(){
	if(getField("type") != "file") critical_error("Can't push fat data for non-file attach.");
	if(_lite_flag == true) critical_error("Can't push fat data for lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));
	QString innerFileName	= getInnerFileName();
	QString innerDirName	= _parent_table->_record->full_dir();

	QMap<QString, QByteArray> fileList;
	fileList[innerFileName] = _file_content;
	DiskHelper::save_files_to_directory(innerDirName, fileList);
}

// То же самое что и pushFatDataToDisk, только в нужную директорию
void Attach::pushFatDataToDirectory(QString dirName){
	if(getField("type") != "file") critical_error("Can't save to directory " + dirName + " non-file attach.");
	if(_lite_flag == true) critical_error("Can't save to directory lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));
	QMap<QString, QByteArray> fileList;
	fileList[getInnerFileName()] = _file_content;
	DiskHelper::save_files_to_directory(dirName, fileList);
}

// Втаскивание в объект содержимого файла с диска
void Attach::popFatDataFromDisk(){
	// Втаскивание возможно только в полновесном состоянии
	if(_lite_flag == true) critical_error("Can't' pop data for lite attach. Attach id: " + getField("id") + " File name: " + getField("fileName"));
	_file_content.clear();

	QString innerFileName	= getInnerFileName();
	QString innerDirName	= _parent_table->_record->full_dir();

	_file_content.append((DiskHelper::get_files_from_directory(innerDirName, innerFileName)).value(innerFileName));
}

// fullFileName - имя файла с полным путем, который нужно скопировать в каталог записи
bool Attach::copyFileToBase(QString iFileName){
	qDebug() << "Before check file. iFileName: " + iFileName;
	qDebug() << "Inner file name: " + getFullInnerFileName();

	// Проверка наличия исходного файла
	QFile file(iFileName);

	qDebug() << "Check file finish";
	if(!file.exists()){
		show_message_box(QObject::tr("Can't open file %1. File not exists.").arg(iFileName));
		return false;
	}
	qDebug() << "Before real copy file.";

	bool result = file.copy(getFullInnerFileName());

	qDebug() << "After real copy file.";
	if(result == false) show_message_box(QObject::tr("Can't copy file %1. May be directory %2 not writable, or target file %3 already exists.").arg(iFileName).arg(getFullInnerDirName()).arg(getFullInnerFileName()));
	return result;
}

// Удаление файла с диска
void Attach::removeFile(){
	if(getField("type") != "file") return;
	// Проверка наличия файла
	QFile file(getFullInnerFileName());
	if(file.exists() == false){
		show_message_box(QObject::tr("Can't delete file %1 on disk. File not exists.").arg(getFullInnerFileName()));
		return;
	}
	file.setPermissions(QFile::ReadOther | QFile::WriteOther);
	file.remove();
}

// Внутрисистемное имя файла (без пути)
// Внутрисистемное имя складывается из идентификатора аттача и расширения, взятого от имени файла
QString Attach::getInnerFileName() const {
	if(getField("type") == "file"){  // Для файла
		// Выясняется расширение по видимому имени файла
		QFileInfo fileInfo(getField("fileName"));
		QString suffix = fileInfo.suffix();

		QString innerFileName = getField("id") + "." + suffix;

		return innerFileName;
	}
	if(getField("type") == "link")                                                                              // Для линка просто возвращается имя файла, куда указывает линк
		return getField("fileName");
	critical_error("Bad attach type in getInnerFileName():" + getField("type"));

	return "";
}

// Внутрисистемное имя файла с путем
QString Attach::getFullInnerFileName() const {
	if(getField("type") == "file"){  // Для файла
		QString resultFileName = getFullInnerDirName() + "/" + getInnerFileName();
		return resultFileName;
	}
	if(getField("type") == "link")                                                                              // Для линка
		return getField("link");
	critical_error("Bad attach type in getFullInnerFileName():" + getField("type"));

	return "";
}

// Внутрисистемное имя файла с абсолютным путем
QString Attach::getAbsoluteInnerFileName() const {
	QFileInfo fileInfo(getFullInnerFileName());

	return fileInfo.absoluteFilePath();
}

// Внутрисистемный путь к файлу (полный)
QString Attach::getFullInnerDirName() const {
	return _parent_table->_record->full_dir();
}

// азмер аттача в байтах
qint64 Attach::getFileSize() const {
	QString tempFileName;
	if(_fields["type"] == "file"){
		QString recordDir = _parent_table->_record->full_dir();
		tempFileName = recordDir + "/" + getInnerFileName();
	}
	if(_fields["type"] == "link") tempFileName = getField("link");
	QFile tempFile(tempFileName);
	if(!tempFile.exists()) return 0;
	else return tempFile.size();
}

// Шифрация аттача на диске и в памяти
void Attach::encrypt(unsigned int area){
	// В этом методе важна последовательность действий,
	// чтобы не получилась ситуации, когда часть данных зашифрована,
	// а другая пытается их использовать, а флаг шифрации еще не установлен
	// Если аттач уже зашифрован, значит есть какая-то ошибка в логике выше
	if(getField(boost::mpl::c_str<crypt_type>::value) == "1") critical_error("Attach::encrypt() : Cant encrypt already encrypted attach.");
	// Шифруется файл
	if(area & areaFile)
		if(getField("type") == "file") CryptService::encryptFile(gl_paras->crypt_key(), getFullInnerFileName());
	// Шифруется содержимое файла в памяти, если таковое есть
	if(area & areaMemory)
		if(_lite_flag == false && _file_content.length() > 0) _file_content = CryptService::encryptByteArray(gl_paras->crypt_key(), _file_content);
	// Шифруются поля, которые подлежат шифрованию
	foreach(QString fieldName, fieldCryptedList()){
		// У аттача с типом file не должно быть обращений к полю link (оно не должно использоваться)
		if(getField("type") == "file" && fieldName == "link") continue;
		// Если поле с указанным именем существует
		if(getField(fieldName).length() > 0) setFieldSource(fieldName, CryptService::encryptString(gl_paras->crypt_key(), getField(fieldName)));
	}

	// Устанавливается флаг, что запись зашифрована
	setField(boost::mpl::c_str<crypt_type>::value, "1");
}

// асшифровка аттача на диске и в памяти
void Attach::decrypt(unsigned int area){
	// Если аттач не зашифрован, и происходит расшифровка, значит есть какая-то ошибка в логике выше
	if(getField(boost::mpl::c_str<crypt_type>::value) != "1") critical_error("Attach::decrypt() : Cant decrypt unencrypted attach.");
	// асшифровывается файл
	if(area & areaFile)
		if(getField("type") == "file") CryptService::decryptFile(gl_paras->crypt_key(), getFullInnerFileName());
	// асшифровывается содержимое файла в памяти, если таковое есть
	if(area & areaMemory)
		if(_lite_flag == false && _file_content.length() > 0) _file_content = CryptService::decryptByteArray(gl_paras->crypt_key(), _file_content);
	// асшифровываются поля, которые подлежат шифрованию
	foreach(QString fieldName, fieldCryptedList()){
		// У аттача с типом file не должно быть обращений к полю link (оно не должно использоваться)
		if(getField("type") == "file" && fieldName == "link") continue;
		// Если поле с указанным именем существует и содержит данные, оно расшифровывается из исходных зашифрованных данных
		if(getField(fieldName).length() > 0) setFieldSource(fieldName, CryptService::decryptString(gl_paras->crypt_key(), _fields[fieldName]));
	}

	// Устанавливается флаг, что запись не зашифрована
	setField(boost::mpl::c_str<crypt_type>::value, ""); // Отсутсвие значения предпочтительней, так как тогда в XML-данные не будет попадать атрибут crypt="0"
}
