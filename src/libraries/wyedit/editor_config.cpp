#include <memory>
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QFileInfo>
#include <QtDebug>
#include <QtGlobal>

#include "editor_config.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/app_config/app_config.h"

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorConfig)
#endif

// Конструктор объекта хранения настроек редактора
EditorConfig::EditorConfig(std::shared_ptr<QSettings> topic_editor_config_, QWidget *parent)
    : QWidget(parent)
    , _editor_conf(topic_editor_config_) {
	Q_UNUSED(parent);
	//	auto config_file_name = topic_editor_config_->fileName();
	////	const QString &config_file_name;
	//	assert(config_file_name != "");

	//	QString full_current_path = gl_paras->root_path();
	//	// Информация о файле настроек редактора
	//	std::shared_ptr<QFileInfo> conf_fileinfo =
	//std::make_shared<QFileInfo>(config_file_name);
	//	// Проверяется, есть ли файл конфигурации
	//	if(!conf_fileinfo->exists()){
	//		// critical_error("Editor config file " + config_file_name + " not
	//found.");
	//		auto editor_conf_location = full_current_path + "/" +
	//gl_paras->target_os();
	//		auto editor_conf_file = config_file_name;//editor_conf_location +
	//"/editorconf.ini";
	//		if(!QDir(editor_conf_location).exists())
	//			if(!QDir::root().mkpath(editor_conf_location))
	//critical_error("EditorConfig::EditorConfig(const QString &config_file_name,
	//QWidget *parent) can not make path \"" + editor_conf_location + "\"");
	//		QFileInfo editor_conf(editor_conf_file);
	//		if(!editor_conf.exists())
	//// if(editor_conf.exists() && editor_conf.isFile())
	//QFile::remove(editor_conf_file);
	//			if(!QFile::copy(QString(":/resource/standardconfig/") +
	//gl_paras->target_os() + "/editorconf.ini", editor_conf_file)) throw
	//std::runtime_error("Can not copy editorconf.ini");
	//		QFile::setPermissions(editor_conf_file, QFile::ReadUser |
	//QFile::WriteUser);
	//		// bool succedded =
	//DiskHelper::save_strings_to_directory(full_current_path,
	//globalparameters.editorconf());//editorconf.ini
	//		// assert(succedded);
	//		conf_fileinfo = std::make_shared<QFileInfo>(editor_conf_file);
	//	}
	//	// Проверяется, доступен ли файл конфигурации на чтение или запись
	//	if(conf_fileinfo->isWritable() == false || conf_fileinfo->isReadable()
	//== false) critical_error("Editor config file " + config_file_name + " not
	//writable or readable. Please check file permission.");
	////	// Полное имя файла конфигурации разбивается на путь и имя файла
	////	QString file_name	= conf_fileinfo->fileName();
	////	QString file_dir	= conf_fileinfo->path();

	//	// Создается объект работы с конфигурацией редактора
	//	_editor_conf = std::make_unique<QSettings>(config_file_name,
	//QSettings::IniFormat);
	//	// conf->setPath(QSettings::IniFormat,QSettings::UserScope,file_dir);
	//	// conf->setPath(QSettings::IniFormat,QSettings::SystemScope,file_dir);

	// Вызываются действия для обновления конфига
	update_version_process();
}

// Деструктор объекта настройки редактора
EditorConfig::~EditorConfig() {
	qDebug() << "Save editor config file";
	_editor_conf->sync();
}

// Получение параметра по имени в виде строки с проверкой его существования
QString EditorConfig::get_parameter(QString name) {
	QString t = _editor_conf->value(name).toString();
	if (t.length() == 0) {
		//		DiskHelper::recover_ini_file(std::make_shared<QFileInfo>(_blogger->current_topic_full_config_name()),
		//true);
		critical_error("In editor config not found parameter " + name);
	}
	return t;
}

QString EditorConfig::get_default_font(void) {
	return get_parameter("default_font");
}

void EditorConfig::set_default_font(QString fontName) {
	_editor_conf->setValue("default_font", fontName);
}

int EditorConfig::get_default_font_size(void) {
	int n = get_parameter("default_font_size").toInt();
	if (n == 0)
		return 10;
	else
		return n;
}

void EditorConfig::set_default_font_size(int size) {
	_editor_conf->setValue("default_font_size", size);
}

// ------------------------------
// Настройки моноширинного шрифта
// ------------------------------

QString EditorConfig::get_monospace_font(void) {
	return get_parameter("monospace_font");
}

void EditorConfig::set_monospace_font(QString fontName) {
	_editor_conf->setValue("monospace_font", fontName);
}

int EditorConfig::get_monospace_font_size(void) {
	int n = get_parameter("monospace_font_size").toInt();
	if (n == 0)
		return 10;
	else
		return n;
}

void EditorConfig::set_monospace_font_size(int size) {
	_editor_conf->setValue("monospace_font_size", size);
}

bool EditorConfig::get_monospace_font_size_apply(void) {
	if (get_parameter("monospace_font_size_apply").toInt() == 1)
		return true;
	else
		return false;
}

void EditorConfig::set_monospace_font_size_apply(bool i) {
	if (i == true)
		_editor_conf->setValue("monospace_font_size_apply", 1);
	else
		_editor_conf->setValue("monospace_font_size_apply", 0);
}

// -----------------------------
// Настройки форматирования кода
// -----------------------------

QString EditorConfig::get_code_font(void) {
	return get_parameter("code_font");
}

void EditorConfig::set_code_font(QString fontName) {
	_editor_conf->setValue("code_font", fontName);
}

int EditorConfig::get_code_font_size(void) {
	int n = get_parameter("code_font_size").toInt();
	if (n == 0)
		return 10;
	else
		return n;
}

void EditorConfig::set_code_font_size(int size) {
	_editor_conf->setValue("code_font_size", size);
}

bool EditorConfig::get_code_font_size_apply(void) {
	if (get_parameter("code_font_size_apply").toInt() == 1)
		return true;
	else
		return false;
}

void EditorConfig::set_code_font_size_apply(bool i) {
	if (i == true)
		_editor_conf->setValue("code_font_size_apply", 1);
	else
		_editor_conf->setValue("code_font_size_apply", 0);
}

QString EditorConfig::get_code_font_color(void) {
	return get_parameter("code_font_color");
}

void EditorConfig::set_code_font_color(QString color) {
	QColor saveColor(color);
	// Если сохраняема строка действительно содержит закодированный цвет
	if (saveColor.isValid())
		_editor_conf->setValue("code_font_color", color);
}

int EditorConfig::get_code_indent_size(void) {
	int n = get_parameter("code_indent_size").toInt();
	if (n == 0)
		return 10;
	else
		return n;
}

void EditorConfig::set_code_indent_size(int size) {
	_editor_conf->setValue("code_indent_size", size);
}

bool EditorConfig::get_code_indent_size_apply(void) {
	if (get_parameter("code_indent_size_apply").toInt() == 1)
		return true;
	else
		return false;
}

void EditorConfig::set_code_indent_size_apply(bool i) {
	if (i == true)
		_editor_conf->setValue("code_indent_size_apply", 1);
	else
		_editor_conf->setValue("code_indent_size_apply", 0);
}

// ----------------------
// Настройки шага отступа
// ----------------------

int EditorConfig::get_indent_step(void) {
	int n = get_parameter("indent_step").toInt();
	if (n == 0)
		return 10;
	else
		return n;
}

void EditorConfig::set_indent_step(int i) {
	_editor_conf->setValue("indent_step", i);
}

// -----------------------------
// Координаты поискового диалога
// -----------------------------

QString EditorConfig::get_finddialog_geometry(void) {
	return get_parameter("finddialog_geometry");
}

void EditorConfig::set_finddialog_geometry(QString geometry) {
	_editor_conf->setValue("finddialog_geometry", geometry);
}

// ----------------------------------
// асположение кнопок форматирования
// ----------------------------------

QString EditorConfig::tools_line_0(void) {
	return get_parameter("tools_line_0");
}

void EditorConfig::tools_line_0(QString line) {
	_editor_conf->setValue("tools_line_0", line);
}

QString EditorConfig::tools_line_1(void) {
	return get_parameter("tools_line_1");
}

void EditorConfig::tools_line_1(QString line) {
	_editor_conf->setValue("tools_line_1", line);
}

bool EditorConfig::get_expand_tools_lines(void) {
	if (get_parameter("expand_tools_lines").toInt() == 1)
		return true;
	else
		return false;
}

void EditorConfig::set_expand_tools_lines(bool i) {
	if (i == true)
		_editor_conf->setValue("expand_tools_lines", 1);
	else
		_editor_conf->setValue("expand_tools_lines", 0);
}

// --------------------
// Номер версии конфига
// --------------------

int EditorConfig::get_config_version(void) {
	if (_editor_conf->contains("version"))
		return _editor_conf->value("version").toInt();
	else
		return 0;
}

void EditorConfig::set_config_version(int i) {
	_editor_conf->setValue("version", i);
}

// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void EditorConfig::update_version_process(void) {
	int fromVersion = get_config_version();
	// Последняя версия на данный момент - 7
	if (fromVersion <= 1)
		update_version(1, 2, get_parameter_table_1(), get_parameter_table_2());
	if (fromVersion <= 2)
		update_version(2, 3, get_parameter_table_2(), get_parameter_table_3());
	if (fromVersion <= 3)
		update_version(3, 4, get_parameter_table_3(), get_parameter_table_4());
	if (fromVersion <= 4)
		update_version(4, 5, get_parameter_table_4(), get_parameter_table_5());
	if (fromVersion <= 5)
		update_version(5, 6, get_parameter_table_5(), get_parameter_table_6());
	if (fromVersion <= 6)
		update_version(6, 7, get_parameter_table_6(), get_parameter_table_7());
	if (fromVersion <= 7)
		update_version(7, 8, get_parameter_table_7(), get_parameter_table_8());
}

QStringList EditorConfig::get_parameter_table_1(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	table << "default_font"
	      << "QString"
	      << "DejaVu Sans";
	table << "default_font_size"
	      << "int"
	      << "10";
	table << "monospace_font"
	      << "QString"
	      << "DejaVu Sans Mono";
	table << "monospace_font_size"
	      << "int"
	      << "10";
	table << "monospace_font_size_apply"
	      << "bool"
	      << "1";
	table << "monospace_indent_size"
	      << "int"
	      << "10";
	table << "monospace_indent_size_apply"
	      << "bool"
	      << "1";
	table << "indent_step"
	      << "int"
	      << "10";
	table << "finddialog_geometry"
	      << "QString"
	      << "";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_2(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 1
	table << get_parameter_table_1(false);

	// Удаляются неиспользуемые параметры
	table = remove_option(table, "monospace_indent_size");
	table = remove_option(table, "monospace_indent_size_apply");

	// Добавляются новые параметры
	table << "code_font"
	      << "QString"
	      << "DejaVu Sans Mono";
	table << "code_font_size"
	      << "int"
	      << "10";
	table << "code_font_size_apply"
	      << "bool"
	      << "1";
	table << "code_indent_size"
	      << "int"
	      << "20";
	table << "code_indent_size_apply"
	      << "bool"
	      << "1";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_3(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 2
	table << get_parameter_table_2(false);

	// Добавляются новые параметры
	table << "tools_line_1"
	      << "QString"
	      << "freeze_browser_view,clear,separator,bold,italic,underline,"
	         "separator,monospace,code,alignleft,aligncenter,alignright,"
	         "alignwidth,fontselect,fontsize";
	table << "tools_line_2"
	      << "QString"
	      << " ";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_4(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 3
	table << get_parameter_table_3(false);
	// В параметр tools_line_2 добавляется "insert_image_from_file"
	// см. метод update_version_change_value()
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_5(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 4
	table << get_parameter_table_4(false);

	// Новые параметры
	table << "code_font_color"
	      << "QString"
	      << "#6E120A";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_6(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 5
	table << get_parameter_table_5(false);

	// Новые параметры
	table << "expand_tools_lines"
	      << "bool"
	      << "1";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_7(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 6
	table << get_parameter_table_6(false);
	// В параметр tools_line_2 добавляется "show_text"
	// см. метод update_version_change_value()
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

QStringList EditorConfig::get_parameter_table_8(bool withEndSignature) {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 6
	table << get_parameter_table_7(false);
	// В параметр tools_line_1 добавляется "attach"
	// см. метод update_version_change_value()
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

// Метод разрешения конфликтов если исходные и конечные типы не совпадают
// Должен включать в себя логику обработки только тех параметров
// и только для тех версий конфигов, которые действительно
// должны поменять тип. Если для принятых параметров не будет
// обработки, это значит что что-то сделано программистом не так
// и нужно дорабатывать код
QString EditorConfig::update_version_allowcollision(
    int versionFrom, int versionTo, QString name, QString fromType,
    QString fromValue, QString toType, QString toValue) {
	critical_error("Error while update config version \nFrom: " + (QString::number(versionFrom)) + "\nTo: " + (QString::number(versionTo)) + "\nName: " + name + "\nFrom type: " + fromType + "\nFrom value: " + fromValue + "\nTo type: " + toType + "\nTo value: " + toValue);

	return QString();
}

// Метод изменения значения при неоходимости
// Тут находятся исключительные случаи
// В общем случае метод сохраняет предыдущее значение
QString EditorConfig::update_version_change_value(int versionFrom, int versionTo, QString name, QString fromValue, QString toValue) {
	Q_UNUSED(toValue);

	QString result = fromValue;
	if (versionFrom == 3 && versionTo == 4 && name == "tools_line_2")
		if (!result.contains("insert_image_from_file"))
			result = result + ",insert_image_from_file";
	if (versionFrom == 6 && versionTo == 7 && name == "tools_line_2")
		if (!result.contains("show_text"))
			result = result + ",show_text";
	if (versionFrom == 7 && versionTo == 8 && name == "tools_line_1")
		if (!result.contains("attach"))
			result = result + ",attach";
	return result;
}

// Основной метод обновления версий конфига
void EditorConfig::update_version(int versionFrom, int versionTo, QStringList baseTable, QStringList finalTable) {
	// Таблица исходных параметров преобразуется к более удобному для работы виду
	// И параллельно заполняется значениями из конфига
	QMap<QString, QMap<QString, QString>> fromTable;
	for (int i = 0; i < 100; i++) {
		QString name = baseTable.at(i * 3 + 0);
		QString type = baseTable.at(i * 3 + 1);
		QString defaultValue = baseTable.at(i * 3 + 2);
		// Если достигнут конец массива
		if (name == "0" && type == "0" && defaultValue == "0")
			break;
		// Подготовка массива для текущего параметра
		QMap<QString, QString> line;
		line.clear();
		line["type"] = type;
		if (_editor_conf->contains(name))
			line["value"] =
			    _editor_conf->value(name).toString();  // Значение из конфига
		else
			line["value"] = defaultValue;  // Дефолтное значение

		// Для текущего имени параметра запоминается массив
		fromTable[name] = line;
	}
	// Таблица конечных параметров преобразуется к более удобному для работы виду
	// Параллельно создаётся список контролирующий обработанные параметры
	// Далее из контролирующего списка будут убираться обработанные параметры
	// Контролирующий список нужен для того, чтобы не удалять записи
	// из конечного массива во время обхода через итератор
	QMap<QString, QMap<QString, QString>> toTable;
	QList<QString> controlList;
	for (int i = 0; i < 100; i++) {
		QString name = finalTable.at(i * 3 + 0);
		QString type = finalTable.at(i * 3 + 1);
		QString defaultValue = finalTable.at(i * 3 + 2);
		// Если достигнут конец массива
		if (name == "0" && type == "0" && defaultValue == "0")
			break;
		// Подготовка массива для текущего параметра
		QMap<QString, QString> line;
		line.clear();
		line["type"] = type;
		line["value"] = defaultValue;  // Дефолтное значение

		// Для текущего имени параметра запоминается массив
		toTable[name] = line;
		controlList << name;  // Имя заносится в контролирующий список
	}
	qDebug() << "From table";
	qDebug() << fromTable;
	qDebug() << "To table";
	qDebug() << toTable;

	// Перебирается конечный массив
	QMapIterator<QString, QMap<QString, QString>> i(toTable);
	while (i.hasNext()) {
		i.next();

		// Данные для новой версии конфига
		QString toName = i.key();
		QMap<QString, QString> line = i.value();
		QString toType = line["type"];
		QString toValue = line["value"];

		qDebug() << "To name: " << toName;
		qDebug() << "To type: " << toType;
		qDebug() << "To value: " << toValue;

		// Определяется, есть ли полный аналог параметра в предыдущей версии конфига
		int beforeParamFlag = 0;
		QMap<QString, QString> line2;
		QString fromType;
		QString fromValue;
		if (fromTable.contains(toName)) {
			line2 = fromTable[toName];
			fromType = line2["type"];
			fromValue = line2["value"];

			qDebug() << "Line2: " << line2;
			qDebug() << "From type: " << fromType;
			qDebug() << "From value: " << fromValue;
			if (toType == fromType)
				beforeParamFlag = 1;  // Параметр есть, и типы совпадают
			else
				beforeParamFlag = 2;  // Параметр есть, но типы не совпадают
		}
		// Параметра в предыдущей версии конфига небыло
		if (beforeParamFlag == 0) {
			// Будет просто сохранено новое дефолтное значение
			// Ничего с конечным параметром делать ненужно
			// Параметр из контролирующего массива исключается
			controlList.removeOne(toName);
		}
		// Параметр в предыдущей версии конфига есть, и типы совпадают
		if (beforeParamFlag == 1) {
			// Будет в общем случае сохранено предыдущее значение
			toTable[toName]["value"] = update_version_change_value(
			    versionFrom, versionTo, toName, fromValue, toValue);

			// Параметр из контролирующего массива исключается
			controlList.removeOne(toName);
		}
		// Параметр в предыдущей версии конфига есть, но типы не совпадают
		if (beforeParamFlag == 2) {
			// Будет возвращено высчитанное значение
			toTable[toName]["value"] = update_version_allowcollision(
			    versionFrom, versionTo, toName, fromType, fromValue, toType, toValue);

			// Параметр из контролирующего массива исключается
			controlList.removeOne(toName);
		}
	}  // Закрылся перебор конечного массива
	// Если в контролирующем массиве остались необработанные параметры
	if (controlList.size() > 0) {
		// Программа завершается
		qDebug() << "Can not compute parameter " << controlList;
		critical_error("Error while update config from " + (QString::number(versionFrom)) + " to " + (QString::number(versionTo)));
	}
	// Конфиг обнуляется
	_editor_conf->clear();

	// Конечный массив записывается в конфиг
	QMapIterator<QString, QMap<QString, QString>> j(toTable);
	while (j.hasNext()) {
		j.next();

		QString toName = j.key();

		QMap<QString, QString> line = j.value();
		QString toType = line["type"];
		QString toValue = line["value"];
		if (toType == "QString")
			_editor_conf->setValue(toName, toValue);
		else if (toType == "int")
			_editor_conf->setValue(toName, toValue.toInt());
		else if (toType == "bool")
			_editor_conf->setValue(toName,
			                       toValue.toInt());  // Булевые переменные как 0 или 1
	}
	// Устанавливается новый номер версии
	set_config_version(versionTo);
}

QStringList EditorConfig::remove_option(QStringList table, QString optionName) {
	// Перебираются строки таблицы
	for (int i = 0; i < (table.length() / 3); i++) {
		QString name = table.at(i * 3 + 0);
		QString type = table.at(i * 3 + 1);
		QString defaultValue = table.at(i * 3 + 2);
		// Если достигнут конец массива
		if (name == "0" && type == "0" && defaultValue == "0")
			break;
		if (name == optionName) {
			table.removeAt(i * 3);
			table.removeAt(i * 3);
			table.removeAt(i * 3);
			break;
		}
	}
	return table;
}
