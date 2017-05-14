
#include <utility>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QRect>

#include "app_config.h"
#include "app_config_updater.h"
#include "main.h"

#include "libraries/global_parameters.h"
#include "views/find_in_base_screen/find_screen.h"

extern std::shared_ptr<gl_para> gl_paras;
extern const char* global_root_id;
extern const char* standardItem;
extern const char* portableItem;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfig)
#endif

// Конструктор объекта настройки программы
AppConfig::AppConfig(std::shared_ptr<gl_para> gl_paras_, QObject* pobj)
{
	Q_UNUSED(pobj);

	// is_init_flag = false;
	auto location = gl_paras_->root_path() + "/" + gl_paras_->target_os();

	// Создается имя файла конфигурации
	_config_file_full_name = location + "/" + gl_para::_conf_filename;
	//// Проверяется, есть ли файл конфигурации
	// QFile conf_file(_config_file_name);
	// if(! conf_file.exists()) critical_error("File " + _config_file_name + " not
	// found.");
	if (!QFile(_config_file_full_name).exists()) {
		if (!QDir(location).exists())
			if (!QDir::root().mkpath(location))
				critical_error("void AppConfig::init(void) can not make path \"" + location + "\"");
		if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras_->target_os() + "/" + gl_para::_conf_filename, _config_file_full_name))
			critical_error(QString("void AppConfig::init(void) can not copy \"") + gl_para::_conf_filename + "\"");
	}
	// throw std::runtime_error("Can not copy " + name.toStdString());
	if ((QFile::ReadUser | QFile::WriteUser) !=
	    (QFile::permissions(_config_file_full_name) &
		(QFile::ReadUser | QFile::WriteUser)))
		QFile::setPermissions(_config_file_full_name, QFile::ReadUser | QFile::WriteUser);
	// Создается указатель на объект хранилища конфигурации
	_app_conf = new QSettings(_config_file_full_name, QSettings::IniFormat);
	// _app_conf->setPath(QSettings::IniFormat, QSettings::UserScope, );
	// _app_conf->setDefaultFormat(QSettings::IniFormat);
	// _app_conf = new QSettings(config_file_name, QSettings::IniFormat);
	update_version_process();

	sync();

	is_init_flag = true;

	// setRecordTableShowVerticalHeaders(true);
}

// Деструктор объекта настройки программы
AppConfig::~AppConfig()
{
	// if(is_init_flag){
	////        qDebug() << "Save config file";
	// if(_app_conf->isWritable()) _app_conf->sync();
	// delete _app_conf;
	// }
}

// void AppConfig::init(const gl_para &gl_paras_){

// }

bool AppConfig::is_init(void)
{
	return is_init_flag;
}

void AppConfig::sync(void)
{
	_app_conf->sync();
}

// Получение параметра по имени в виде строки с проверкой его существования
QString AppConfig::get_parameter(QString name) const
{
	QString result = "";
	if (_app_conf->contains(name))
		result = _app_conf->value(name).toString();
	else {
		AppConfig* _this = const_cast<AppConfig*>(this);
		if (QFile(_config_file_full_name).exists())
			QFile::remove(_config_file_full_name);
		if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + gl_para::_conf_filename, _config_file_full_name))
			critical_error(QString("Can not copy \"") + gl_para::_conf_filename + "\"");
		if ((QFile::ReadUser | QFile::WriteUser) !=
		    (QFile::permissions(_config_file_full_name) &
			(QFile::ReadUser | QFile::WriteUser)))
			QFile::setPermissions(_config_file_full_name, QFile::ReadUser | QFile::WriteUser);
		delete _this->_app_conf;
		_this->_app_conf =
		    new QSettings(_config_file_full_name, QSettings::IniFormat);
		result = _app_conf->value(name).toString();
		if (result == "")
			critical_error("In config not found parameter " + name);
	}
	return result;
}

// bool AppConfig::root_dir(QString dirName){
// QDir directory(dirName);
// if(directory.exists() && directory.isReadable()){
// _app_conf->setValue(gl_para::_program_root_dir_name, dirName);

// return true;
// }else return false;
// }

// QString AppConfig::root_dir(void) const {return
// get_parameter(gl_para::_program_root_dir_name);}

// Установка имени директории с данными (в которой находится index.xml)
bool AppConfig::data_dir(QString dirName)
{
	QDir directory(dirName);
	if (directory.exists() && directory.isReadable()) {
		_app_conf->setValue("datadir", directory.dirName());

		return true;
	} else
		return false;
}

// Получение имени директории с данными (в которой находится index.xml)
QString AppConfig::data_dir(void) const
{
	return get_parameter("datadir");
}

// Установка имени директории с корзиной
bool AppConfig::trash_dir(QString dirName)
{
	QDir directory(dirName);
	if (directory.exists() && directory.isReadable()) {
		_app_conf->setValue("trashdir", directory.dirName());

		return true;
	} else
		return false;
}

// Получение имени директории с корзиной
QString AppConfig::trash_dir(void) const
{
	return get_parameter("trashdir");
}

// Получение максимального размера директории корзины в мегабайтах
unsigned int AppConfig::trash_size(void) const
{
	return get_parameter("trashsize").toInt();
}

// Установка максимального размера директории корзины в мегабайтах
bool AppConfig::trash_size(unsigned int mbSize)
{
	if (mbSize > 0) {
		_app_conf->setValue("trashsize", mbSize);

		return true;
	} else
		return false;
}

// Получение максимально допустимого числа файлов в корзине
int AppConfig::trash_max_file_count(void) const
{
	return get_parameter("trashmaxfilecount").toInt();
}

// Установка максимально допустимого числа файлов в корзине
bool AppConfig::trash_max_file_count(int count)
{
	if (count > 0) {
		_app_conf->setValue("trashmaxfilecount", count);

		return true;
	} else
		return false;
}

bool AppConfig::cut_branch_confirm(void) const
{
	return _app_conf->value("cutbranchconfirm").toBool();
}

bool AppConfig::cut_branch_confirm(bool confirm)
{
	_app_conf->setValue("cutbranchconfirm", confirm);

	return true;
}

bool AppConfig::print_debug_messages(void) const
{
	return _app_conf->value("printdebugmessages").toBool();
}

bool AppConfig::print_debug_messages(bool isPrint)
{
	_app_conf->setValue("printdebugmessages", isPrint);

	return true;
}

QString AppConfig::interface_language(void) const
{
	QString language = get_parameter("interfacelanguage");
	qDebug() << "Get interface language" << language;

	return language;
}

bool AppConfig::interface_language(QString language)
{
	_app_conf->setValue("interfacelanguage", language);

	return true;
}

QString AppConfig::add_new_record_expand_info(void) const
{
	return get_parameter("addnewrecord_expand_info");
}

void AppConfig::add_new_record_expand_info(QString state)
{
	if (state == "0" || state == "1")
		_app_conf->setValue("addnewrecord_expand_info", state);
	else
		critical_error("Set unavailable value for addnewrecord_expand_info " + state);
}

// QSize AppConfig::treescreensize(void)const
// {

// QSize size;
// QString rectString;
// QStringList rectParameter;

// rectString = conf->value("treescreen_size", "500,400").toString();

// rectParameter = rectString.split(",");

// int w = rectParameter[0].toInt();
// int h = rectParameter[1].toInt();

// size.setWidth(w);
// size.setHeight(h);

// return size;
// }

// void AppConfig::treescreensize(const QSize &size) {
// qDebug() << "Save tree screen widget size";

// QString result = QString::number(size.width()) + "," +
// QString::number(size.height());

// conf->setValue("treescreen_size", result);
// }

QRect AppConfig::mainwin_geometry(void) const
{
	QRect rect;
	QString rectString;
	QStringList rectParameter;

	rectString = _app_conf->value("mainwingeometry", "0,0,500,400").toString();

	rectParameter = rectString.split(",");

	int x = rectParameter[0].toInt();
	int y = rectParameter[1].toInt();
	int w = rectParameter[2].toInt();
	int h = rectParameter[3].toInt();

	rect.setRect(x, y, w, h);

	return rect;
}

void AppConfig::mainwin_geometry(int x, int y, int w, int h)
{
	qDebug() << "Save new main window geometry";

	QString result = QString::number(x) + "," + QString::number(y) + "," +
	    QString::number(w) + "," + QString::number(h);

	_app_conf->setValue("mainwingeometry", result);
}

void AppConfig::h_tree_splitter_sizelist(QList<int> list)
{
	splitter_sizelist("h_tree_splitter", list);
}

QList<int> AppConfig::h_tree_splitter_sizelist()
{
	return splitter_sizelist("h_tree_splitter");
}

void AppConfig::h_record_splitter_sizelist(QList<int> list)
{
	splitter_sizelist("h_record_splitter", list);
}

QList<int> AppConfig::h_record_splitter_sizelist()
{
	return splitter_sizelist("h_record_splitter");
}

QList<int> AppConfig::v_right_splitter_sizelist(void) const
{
	return splitter_sizelist("v_right_splitter");
}

void AppConfig::v_right_splitter_sizelist(QList<int> list)
{
	splitter_sizelist("v_right_splitter", list);
}

// QList<int> AppConfig::hspl_sizelist(void)const
// {
// return splitter_sizelist("hspl");
// }

// void AppConfig::hspl_sizelist(QList<int> list)
// {
// splitter_sizelist("hspl", list);
// }

QList<int> AppConfig::findsplitter_sizelist(void) const
{
	return splitter_sizelist("find_splitter");
}

void AppConfig::findsplitter_sizelist(QList<int> list)
{
	qDebug() << "Config set find splitter list to " << list;
	splitter_sizelist("find_splitter", list);
}

QList<int> AppConfig::splitter_sizelist(QString name) const
{
	QStringList line_list;
	QList<int> list;

	line_list =
	    (_app_conf->value(name + "_sizelist", "100,100")).toString().split(",");
	for (int i = 0; i < line_list.size(); ++i)
		list.append(line_list.at(i).toInt());
	return list;
}

void AppConfig::splitter_sizelist(QString name, QList<int> list)
{
	QStringList line_list;
	for (int i = 0; i < list.size(); ++i)
		line_list.append(QString::number(list.at(i)));
	_app_conf->setValue(name + "_sizelist", line_list.join(","));
}

std::pair<QString, QStringList> AppConfig::tree_position(void) const
{
	return std::make_pair(
	    _app_conf->value("tree_intercept", global_root_id).toString(),
	    _app_conf->value("tree_position", "1").toString().split(","));
}

void AppConfig::tree_position(QString view_root_id, QStringList current_item_absolute_path)
{
	qDebug() << "AppConfig::set_tree_position() : id\t= " << view_root_id
		 << "\t= " << current_item_absolute_path;
	_app_conf->setValue("tree_intercept", view_root_id);
	_app_conf->setValue("tree_position", current_item_absolute_path.join(","));
}

// ID записи в таблице конечных записей, которая выделена на экране
QString AppConfig::recordtable_selected_record_id(void) const
{
	return _app_conf->value("recordtableSelectedRecordId", 0).toString();
}

// ID записи в таблице конечных записей, которая выделена на экране
void AppConfig::recordtable_selected_record_id(QString id)
{
	_app_conf->setValue("recordtableSelectedRecordId", id);
}

int AppConfig::findscreen_wordregard(void) const
{
	return _app_conf->value("findscreen_wordregard", 0).toInt();
}

void AppConfig::findscreen_wordregard(int pos)
{
	_app_conf->setValue("findscreen_wordregard", pos);
}

int AppConfig::findscreen_howextract(void) const
{
	return _app_conf->value("findscreen_howextract", 0).toInt();
}

void AppConfig::findscreen_howextract(int pos)
{
	_app_conf->setValue("findscreen_howextract", pos);
}

int AppConfig::find_screen_tree_search_area(void) const
{
	return _app_conf->value("findScreenTreeSearchArea", 0).toInt();
}

void AppConfig::find_screen_tree_search_area(int pos)
{
	_app_conf->setValue("findScreenTreeSearchArea", pos);
}

bool AppConfig::findscreen_find_in_field(QString fieldName) const
{
	return _app_conf->value("findscreen_find_in_" + fieldName, 0).toBool();
}

void AppConfig::findscreen_find_in_field(QString fieldName, bool isChecked)
{
	_app_conf->setValue("findscreen_find_in_" + fieldName, isChecked);
}

bool AppConfig::findscreen_show(void) const
{
	return _app_conf->value("findscreen_show", 0).toBool();
}

void AppConfig::findscreen_show(bool isShow)
{
	_app_conf->setValue("findscreen_show", isShow);
}

bool AppConfig::editor_show(void) const
{
	return _app_conf->value("editor_show", 0).toBool();
}

void AppConfig::editor_show(bool isShow)
{
	_app_conf->setValue("editor_show", isShow);
}

QString AppConfig::howpassrequest(void) const
{
	return get_parameter("howpassrequest");
}

void AppConfig::howpassrequest(QString mode)
{
	if (mode == "atClickOnCryptBranch" || mode == "atStartProgram")
		_app_conf->setValue("howpassrequest", mode);
	else
		critical_error("Set unavailable value for howpassrequest " + mode);
}

bool AppConfig::auto_close_password_enable(void) const
{
	return _app_conf->value("autoClosePasswordEnable", 0).toBool();
}

void AppConfig::auto_close_password_enable(bool flag)
{
	_app_conf->setValue("autoClosePasswordEnable", flag);
}

int AppConfig::auto_close_password_delay(void) const
{
	return _app_conf->value("autoClosePasswordDelay", 0).toInt();
}

void AppConfig::auto_close_password_delay(int delay)
{
	_app_conf->setValue("autoClosePasswordDelay", delay);
}

bool AppConfig::run_in_minimized_window(void) const
{
	return _app_conf->value("runinminimizedwindow").toBool();
}

void AppConfig::run_in_minimized_window(bool flag)
{
	_app_conf->setValue("runinminimizedwindow", flag);
}

QString AppConfig::synchro_command(void) const
{
	return get_parameter("synchrocommand");
}

void AppConfig::synchro_command(QString command)
{
	_app_conf->setValue("synchrocommand", command);
}

bool AppConfig::synchro_on_startup(void) const
{
	return _app_conf->value("synchroonstartup").toBool();
}

void AppConfig::synchro_on_startup(bool flag)
{
	_app_conf->setValue("synchroonstartup", flag);
}

bool AppConfig::synchro_on_exit(void) const
{
	return _app_conf->value("synchroonexit").toBool();
}

void AppConfig::synchro_on_exit(bool flag)
{
	_app_conf->setValue("synchroonexit", flag);
}

bool AppConfig::synchro_console_details(void) const
{
	return _app_conf->value("syncroConsoleDetails").toBool();
}

void AppConfig::synchro_console_details(bool flag)
{
	_app_conf->setValue("syncroConsoleDetails", flag);
}

int AppConfig::editor_cursor_position(void) const
{
	return _app_conf->value("editorCursorPosition", 0).toInt();
}

void AppConfig::editor_cursor_position(int n)
{
	_app_conf->setValue("editorCursorPosition", n);
}

int AppConfig::editor_scroll_bar_position(void) const
{
	return _app_conf->value("editorScrollBarPosition", 0).toInt();
}

void AppConfig::editor_scroll_bar_position(int n)
{
	_app_conf->setValue("editorScrollBarPosition", n);
}

QString AppConfig::password_middle_hash(void) const
{
	return get_parameter("passwordMiddleHash");
}

void AppConfig::password_middle_hash(QString hash)
{
	_app_conf->setValue("passwordMiddleHash", hash);
}

// Нужно ли локально хранить пароль (точнее, промежуточный хеш пароля)
bool AppConfig::password_save_flag(void) const
{
	return _app_conf->value("passwordSaveFlag").toBool();
}

void AppConfig::password_save_flag(bool flag)
{
	_app_conf->setValue("passwordSaveFlag", flag);
}

bool AppConfig::remember_cursor_at_history_navigation(void) const
{
	return _app_conf->value("remember_cursor_at_history_navigation").toBool();
}

void AppConfig::remember_cursor_at_history_navigation(bool flag)
{
	_app_conf->setValue("remember_cursor_at_history_navigation", flag);
}

bool AppConfig::remember_cursor_at_ordinary_selection(void) const
{
	return _app_conf->value("remember_cursor_at_ordinary_selection").toBool();
}

void AppConfig::remember_cursor_at_ordinary_selection(bool flag)
{
	_app_conf->setValue("remember_cursor_at_ordinary_selection", flag);
}

int AppConfig::ugly_qss_replace_height_for_table_view(void) const
{
	return _app_conf->value("uglyQssReplaceHeightForTableView", 0).toInt();
}

void AppConfig::ugly_qss_replace_height_for_table_view(int n)
{
	_app_conf->setValue("uglyQssReplaceHeightForTableView", n);
}

// Перечень полей, отображаемых в таблице конечных записей
QStringList AppConfig::record_table_show_fields(void) const
{
	return (_app_conf->value("recordTableShowFields", "name"))
	    .toString()
	    .split(",");
}

void AppConfig::record_table_show_fields(QStringList fields)
{
	// Не должно быть так, чтобы был пустой список
	if (fields.size() == 0)
		fields << "name";
	_app_conf->setValue("recordTableShowFields", fields.join(","));
}

bool AppConfig::record_table_show_horizontal_headers(void) const
{
	return _app_conf->value("recordTableShowHorizontalHeaders").toBool();
}

void AppConfig::record_table_show_horizontal_headers(bool flag)
{
	_app_conf->setValue("recordTableShowHorizontalHeaders", flag);
}

bool AppConfig::record_table_show_vertical_headers(void) const
{
	return _app_conf->value("recordTableShowVerticalHeaders").toBool();
}

void AppConfig::record_table_show_vertical_headers(bool flag)
{
	_app_conf->setValue("recordTableShowVerticalHeaders", flag);
}

// Ширина полей, отображаемых в таблице конечных записей
QStringList AppConfig::record_table_fields_width(void) const
{
	return (_app_conf->value("recordTableFieldsWidth", "256"))
	    .toString()
	    .split(",");
}

void AppConfig::record_table_fields_width(QStringList fields)
{
	// В списке с одним элементом должна стоять стандартная величина (всеравно она
	// не влияет, и начнет влиять только после появления второй колонки)
	if (fields.size() == 1)
		fields[0] = "256";
	// Не должно быть так, чтобы был пустой список
	if (fields.size() == 0)
		fields << "256";
	_app_conf->setValue("recordTableFieldsWidth", fields.join(","));
}

// Показывать ли сплешскрин при старте программы
bool AppConfig::show_splash_screen(void) const
{
	return _app_conf->value("showSplashScreen").toBool();
}

void AppConfig::show_splash_screen(bool isShow)
{
	_app_conf->setValue("showSplashScreen", isShow);
}

// ежим интерфейса, возможные значения "desktop" и "mobile"
QString AppConfig::interface_mode(void) const
{
	QString mode = get_parameter("interfaceMode");

	return mode;
}

void AppConfig::interface_mode(QString mode)
{
	_app_conf->setValue("interfaceMode", mode);
}

// Имя последнего активного виджета
QString AppConfig::focus_widget(void) const
{
	QString widgetName = get_parameter("focusWidget");

	return widgetName;
}

void AppConfig::focus_widget(QString widgetName)
{
	// qDebug() << "AppConfig::setFocusWidget() : " << widgetName;
	_app_conf->setValue("focusWidget", widgetName);
}

QStringList AppConfig::hide_editor_tools(void) const
{
	return (_app_conf->value("hideEditorTools", "")).toString().split(",");
}

void AppConfig::hide_editor_tools(QStringList toolsNames)
{
	_app_conf->setValue("hideEditorTools", toolsNames.join(","));
}

bool AppConfig::find_in_base_expand(void) const
{
	return _app_conf->value(FindScreen::_find_in_base_expand)
	    .toBool(); // "findInBaseExpand"
}

void AppConfig::find_in_base_expand(bool state)
{
	_app_conf->setValue(FindScreen::_find_in_base_expand,
	    state); // "findInBaseExpand"
}

// азрешено ли использовать собственный формат вывода даты и времени
bool AppConfig::enable_custom_datetime_format(void) const
{
	return _app_conf->value("enableCustomDateTimeFormat").toBool();
}

void AppConfig::enable_custom_datetime_format(bool state)
{
	_app_conf->setValue("enableCustomDateTimeFormat", state);
}

// Строка собственного формата вывода даты и времени
QString AppConfig::custom_datetime_format(void) const
{
	return get_parameter("customDateTimeFormat");
}

void AppConfig::custom_datetime_format(QString format)
{
	_app_conf->setValue("customDateTimeFormat", format);
}

// Путь на диске по которому пользователь открывал файлы чтобы приаттачить их к
// записи
QString AppConfig::attach_append_dir(void) const
{
	return get_parameter("attachAppendDir");
}

void AppConfig::attach_append_dir(QString dir)
{
	_app_conf->setValue("attachAppendDir", dir);
}

// Путь на диске по которому пользователь сохранял (Сохранить
// как...)приаттаченные файлы
QString AppConfig::attach_save_as_dir(void) const
{
	return get_parameter("attachSaveAsDir");
}

void AppConfig::attach_save_as_dir(QString dir)
{
	_app_conf->setValue("attachSaveAsDir", dir);
}

// азрешать ли для просмотра расшифровывать зашифрованные файлы в директорию
// корзины
bool AppConfig::enable_decrypt_file_to_trash_directory(void) const
{
	return _app_conf->value("enableDecryptFileToTrashDirectory").toBool();
}

void AppConfig::enable_decrypt_file_to_trash_directory(bool state)
{
	_app_conf->setValue("enableDecryptFileToTrashDirectory", state);
}


int AppConfig::topic_number() const
{
	return get_parameter("topic_number").toInt();
}

void AppConfig::topic_number(int num)
{
	_app_conf->setValue("topic_number", num);
}

// --------------------
// Номер версии конфига
// --------------------

int AppConfig::config_version(void) const
{
	if (_app_conf->contains("version"))
		return _app_conf->value("version").toInt();
	else
		return 0;
}

void AppConfig::config_version(int i)
{
	_app_conf->setValue("version", i);
}

QStringList AppConfig::remove_parameter_from_table(QString removeName, QStringList table)
{
	// Перебираются параметры в таблице
	for (int i = 0; i < MYTETRA_CONFIG_PARAM_NUM; i++) {
		// Выясняется имя параметра
		QString name = table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
		// Если имя совпадает с удаляемым
		if (name == removeName) {
			// Удаляются данные об элементе (удаляется ячейка с одним и тем же номером
			// столько раз, сколько полей в таблице)
			for (int j = 0; j < MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD; j++)
				table.removeAt(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
			break;
		}
	}
	return table;
}

// Получение типа параметра в виде строки
QString AppConfig::parameter_type_from_table(QString parameterName, QStringList table) const
{
	// Перебираются параметры в таблице
	for (int i = 0; i < MYTETRA_CONFIG_PARAM_NUM; i++) {
		// Выясняется имя параметра
		QString name = table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
		if (name == parameterName)
			return table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD + 1);
	}
	return "";
}

// Получение значения параметра в виде строки
QString AppConfig::parameter_value_from_table(QString parameterName, QStringList table) const
{
	// Перебираются параметры в таблице
	for (int i = 0; i < MYTETRA_CONFIG_PARAM_NUM; i++) {
		// Выясняется имя параметра
		QString name = table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
		if (name == parameterName)
			return table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD + 2);
	}
	return "";
}

// Замена типа и значения параметра
QStringList AppConfig::replace_parameter_in_table(QString replaceName, QString replaceType, QString replaceValue, QStringList table)
{
	// Перебираются параметры в таблице
	for (int i = 0; i < MYTETRA_CONFIG_PARAM_NUM; i++) {
		// Выясняется имя параметра
		QString name = table.at(i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD);
		// Если имя совпадает с заменяемым
		if (name == replaceName) {
			table[i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD + 1] = replaceType;
			table[i * MYTETRA_CONFIG_PARAM_FIELDS_AT_RECORD + 2] = replaceValue;

			break;
		}
	}
	return table;
}

// ------------------------------------
// Методы для обновления версии конфига
// ------------------------------------

void AppConfig::update_version_process(void)
{
	QString configFileName = gl_paras->root_path() + "/" + gl_paras->target_os() +
	    "/" + gl_para::_conf_filename;

	AppConfigUpdater updater;
	updater.set_config_file(configFileName);

	int fromVersion = config_version();
	// Последняя версия на данный момент - 26
	if (fromVersion <= 1)
		updater.update_version(1, 2, parameter_table<0>(), parameter_table<1>());
	if (fromVersion <= 2)
		updater.update_version(2, 3, parameter_table<1>(), parameter_table<2>());
	if (fromVersion <= 3)
		updater.update_version(3, 4, parameter_table<2>(), parameter_table<3>());
	if (fromVersion <= 4)
		updater.update_version(4, 5, parameter_table<3>(), parameter_table<4>());
	if (fromVersion <= 5)
		updater.update_version(5, 6, parameter_table<4>(), parameter_table<5>());
	if (fromVersion <= 6)
		updater.update_version(6, 7, parameter_table<5>(), parameter_table<6>());
	if (fromVersion <= 7)
		updater.update_version(7, 8, parameter_table<6>(), parameter_table<7>());
	if (fromVersion <= 8)
		updater.update_version(8, 9, parameter_table<7>(), parameter_table<8>());
	if (fromVersion <= 9)
		updater.update_version(9, 10, parameter_table<8>(), parameter_table<9>());
	if (fromVersion <= 10)
		updater.update_version(10, 11, parameter_table<9>(), parameter_table<10>());
	if (fromVersion <= 11)
		updater.update_version(11, 12, parameter_table<10>(), parameter_table<11>());
	if (fromVersion <= 12)
		updater.update_version(12, 13, parameter_table<11>(), parameter_table<12>());
	if (fromVersion <= 13)
		updater.update_version(13, 14, parameter_table<12>(), parameter_table<13>());
	if (fromVersion <= 14)
		updater.update_version(14, 15, parameter_table<13>(), parameter_table<14>());
	if (fromVersion <= 15)
		updater.update_version(15, 16, parameter_table<14>(), parameter_table<15>());
	if (fromVersion <= 16)
		updater.update_version(16, 17, parameter_table<15>(), parameter_table<16>());
	if (fromVersion <= 17)
		updater.update_version(17, 18, parameter_table<16>(), parameter_table<17>());
	if (fromVersion <= 18)
		updater.update_version(18, 19, parameter_table<17>(), parameter_table<18>());
	if (fromVersion <= 19)
		updater.update_version(19, 20, parameter_table<18>(), parameter_table<19>());
	if (fromVersion <= 20)
		updater.update_version(20, 21, parameter_table<19>(), parameter_table<20>());
	if (fromVersion <= 21)
		updater.update_version(21, 22, parameter_table<20>(), parameter_table<21>());
	if (fromVersion <= 22)
		updater.update_version(22, 23, parameter_table<21>(), parameter_table<22>());
	if (fromVersion <= 23)
		updater.update_version(23, 24, parameter_table<22>(), parameter_table<23>());
	if (fromVersion <= 24)
		updater.update_version(24, 25, parameter_table<23>(), parameter_table<24>());
	if (fromVersion <= 25)
		updater.update_version(25, 26, parameter_table<24>(), parameter_table<25>());
	if (fromVersion <= 26)
		updater.update_version(26, 27, parameter_table<25>(), parameter_table<26>());
}

template <>
QStringList AppConfig::parameter_table<0>(
    bool withEndSignature)
{ // QStringList AppConfig::parameter_table_1(bool
	// withEndSignature) const
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	table << "addnewrecord_expand_info"
	      << "int"
	      << "1";
	table << "findscreen_find_inname"
	      << "bool"
	      << "true";
	table << "findscreen_find_intags"
	      << "bool"
	      << "true";
	table << "findscreen_find_intext"
	      << "bool"
	      << "true";
	table << "findscreen_howextract"
	      << "int"
	      << "1";
	table << "findscreen_show"
	      << "bool"
	      << "false";
	table << "findscreen_wordregard"
	      << "int"
	      << "1";
	table << "editor_show"
	      << "bool"
	      << "false";
	table << "find_splitter_sizelist"
	      << "QString"
	      << "614, 32";
	// table << "hspl_sizelist"            << "QString" << "237,621";
	table << "lastidnum"
	      << "int"
	      << "3537";
	table << "lastnotenum"
	      << "int"
	      << "3119";
	table << "lastprefixnum"
	      << "int"
	      << "7540";
	table << "mainwingeometry"
	      << "QString"
	      << "155,24,864,711)";
	table << "recordtable_position"
	      << "int"
	      << "0";
	table << "datadir"
	      << "QString"
	      << "./data";
	table << "trashdir"
	      << "QString"
	      << "./trash";
	table << "trashmaxfilecount"
	      << "int"
	      << "200";
	table << "trashsize"
	      << "int"
	      << "5";
	table << "tree_position"
	      << "QString"
	      << "0,1818,1819";
	table << "v_right_splitter_sizelist"
	      << "QString"
	      << "614,0";
	table << "findscreen_find_inauthor"
	      << "bool"
	      << "true";
	table << "findscreen_find_inurl"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<1>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_2(bool
	// withEndSignature) const {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 1
	table << parameter_table<0>(false);

	// Новый параметр
	table << "cutbranchconfirm"
	      << "bool"
	      << "true";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<2>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_3(bool
	// withEndSignature) const {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 2
	table << parameter_table<1>(false);

	// Новый параметр
	table << "printdebugmessages"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<3>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_4(bool
	// withEndSignature) const {
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 3
	table << parameter_table<2>(false);

	// Новые параметры
	table << "interfacelanguage"
	      << "QString"
	      << "en";
	table << "programm"
	      << "QString" << gl_paras->application_name();
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<4>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_5(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 4
	table << parameter_table<3>(false);

	// Исключаются ненужные в новой версии параметры
	table = remove_parameter_from_table("lastidnum", table);
	table = remove_parameter_from_table("lastnotenum", table);
	table = remove_parameter_from_table("lastprefixnum", table);
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<5>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_6(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 5
	table << parameter_table<4>(false);

	// Новые параметры
	table << "howpassrequest"
	      << "QString"
	      << "atClickOnCryptBranch";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<6>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_7(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 6
	table << parameter_table<5>(false);

	// Новые параметры
	table << "runinminimizedwindow"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<7>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_8(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 7
	table << parameter_table<6>(false);

	// Новые параметры
	table << "synchrocommand"
	      << "QString"
	      << "";
	table << "synchroonstartup"
	      << "bool"
	      << "false";
	table << "synchroonexit"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<8>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_9(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 8
	table << parameter_table<7>(false);

	// Новые параметры
	table << "syncroConsoleDetails"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<9>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_10(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 9
	table << parameter_table<8>(false);

	// Новые параметры
	table << "autoClosePasswordEnable"
	      << "bool"
	      << "false";
	table << "autoClosePasswordDelay"
	      << "int"
	      << "10";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<10>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_11(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 10
	table << parameter_table<9>(false);

	// Новые параметры
	table << "editorCursorPosition"
	      << "int"
	      << "0";
	table << "editorScrollBarPosition"
	      << "int"
	      << "0";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<11>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_12(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 11
	table << parameter_table<10>(false);

	// Новые параметры
	table << "passwordMiddleHash"
	      << "QString"
	      << "";
	table << "passwordSaveFlag"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<12>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_13(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 12
	table << parameter_table<11>(false);

	// Новые параметры
	table << "remember_cursor_at_history_navigation"
	      << "bool"
	      << "true";
	table << "remember_cursor_at_ordinary_selection"
	      << "bool"
	      << "true";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<13>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_14(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 13
	table << parameter_table<12>(false);

	// Новые параметры
	table << "findScreenTreeSearchArea"
	      << "int"
	      << "0"; // Область поиска. Искать во всем дереве - 0, искать в текущей
		      // ветке - 1
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<14>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_15(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 14
	table << parameter_table<13>(false);
	// Новые параметры
	if (gl_paras->target_os() == "android")
		table << "uglyQssReplaceHeightForTableView"
		      << "int"
		      << "35"; // Так как не все параметры можно стилизовать через QSS,
			       // здесь задается высота ячейки таблицы
	else
		table << "uglyQssReplaceHeightForTableView"
		      << "int"
		      << "0";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<15>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_16(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 15
	table << parameter_table<14>(false);

	// Новые параметры
	table << "recordTableShowFields"
	      << "QString"
	      << "name,tags";
	table << "recordTableFieldsWidth"
	      << "QString"
	      << "256,128";
	table << "recordTableShowHorizontalHeaders"
	      << "bool"
	      << "true";
	table << "recordTableShowVerticalHeaders"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<16>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_17(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 16
	table << parameter_table<15>(false);
	// Новые параметры
	if (gl_paras->target_os() == "android")
		table << "showSplashScreen"
		      << "bool"
		      << "true"; // В Андроид долгий запуск, нужно показывать сплешскрин
	else
		table << "showSplashScreen"
		      << "bool"
		      << "false"; // На десктопе быстрый запуск, сплешскрин только мешает
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<17>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_18(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 17
	table << parameter_table<16>(false);
	// Новые параметры
	if (gl_paras->target_os() == "android")
		table << "interfaceMode"
		      << "QString"
		      << "mobile"; // В Андроид должен быть мобильный интерфейс
	else
		table << "interfaceMode"
		      << "QString"
		      << "desktop"; // На десктопе должен быть интерфейс адоптированный для
				    // работы на рабочем столе
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<18>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_19(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 18
	table << parameter_table<17>(false);

	// Новые параметры
	table << "focusWidget"
	      << "QString"
	      << "";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<19>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_20(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 19
	table << parameter_table<18>(false);
	// Новые параметры
	if (gl_paras->target_os() == "android")
		table << "hideEditorTools"
		      << "QString"
		      << "italic,underline,monospace,alignleft,aligncenter,alignright,"
			 "alignwidth,numericlist,dotlist,indentplus,indentminus,"
			 "showformatting,showhtml,fontcolor,expand_edit_area,save,"
			 "createtable,table_add_row,table_remove_row,table_add_col,table_"
			 "remove_col,table_merge_cells,table_split_cell"; // В Андроид
									  // прячутся
									  // инструменты
									  // сложного
									  // форматирования
									  // текста
	else
		table << "hideEditorTools"
		      << "QString"
		      << ""; // На десктопе скрываемых кнопок редактора нет
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<20>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_21(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 20
	table << parameter_table<19>(false);

	table << FindScreen::_find_in_base_expand // "findInBaseExpand"
	      << "bool"
	      << "true";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<21>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_22(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 21
	table << parameter_table<20>(false);

	table << "recordtableSelectedRecordId"
	      << "QString"
	      << "";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<22>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_23(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 22
	table << parameter_table<21>(false);

	// Исключаются ненужные в новой версии параметры
	table = remove_parameter_from_table("recordtable_position", table);
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<23>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_24(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 23
	table << parameter_table<22>(false);

	table << "enableCustomDateTimeFormat"
	      << "bool"
	      << "false";
	table << "customDateTimeFormat"
	      << "QString"
	      << "";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<24>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_25(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 24
	table << parameter_table<23>(false);

	table << "attachAppendDir"
	      << "QString"
	      << "";
	table << "attachSaveAsDir"
	      << "QString"
	      << "";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<25>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_26(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 25
	table << parameter_table<24>(false);

	table << "enableDecryptFileToTrashDirectory"
	      << "bool"
	      << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}

template <>
QStringList AppConfig::parameter_table<26>(
    bool withEndSignature)
{ // QStringList AppConfig:: parameter_table_27(bool
	// withEndSignature){
	// Таблица параметров
	// Имя, Тип, Значение на случай когда в конфиге параметра прочему-то нет
	QStringList table;

	// Старые параметры, аналогичные версии 26
	table << parameter_table<25>(false);

	// table << "v_right_splitter_sizelist"        << "QString" << "614, 0";
	table << "h_record_splitter_sizelist"
	      << "QString"
	      << "0, 1220";
	table << "h_tree_splitter_sizelist"
	      << "QString"
	      << "312, 1054";

	table << "vertical_scrollbar_style_sheet"
	      << "QString"
	      << "QTabWidget::pane { border: 0 px; } ";
	table << "horizontal_scrollbar_style_sheet"
	      << "QString"
	      << "QTabWidget::pane { border: 0 px; } ";
	table << "application_mode"
	      << "QString" << portableItem;
	table << "topic_number"
	      << "int"
	      << "0";
	// table << "enableDecryptFileToTrashDirectory" << "bool" << "false";
	if (withEndSignature)
		table << "0"
		      << "0"
		      << "0";
	return table;
}
