// #include <memory>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "global_parameters.h"
#include "main.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTreeView>

#include "libraries/disk_helper.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/window_switcher.h"
#include "views/app_config/app_config_dialog.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/downloadmanager.h"
#include "views/browser/tabwidget.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/tree/tree_screen.h"
// #include "utility/config_ini.h"

// extern const char *index_xml_file_name;

extern const char* standardItem;
extern const char* portableItem;

extern const char* program_title;
extern const QString program_title_qstring;
extern const std::string program_title_string;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(GlobalParameters)
#endif


constexpr char gl_para::_program_root_dir_name[];      //  = "rootdir";
constexpr char gl_para::_program_instance_name[];      //  = "tute"; // QCoreApplication::applicationName();// won't work // constexpr const char* gl_para::_program_instance_name;
constexpr char gl_para::_index_xml_file_name[];        //  = "index.xml";
constexpr char gl_para::_mode_filename[];              //  = "mode.ini";              // constexpr char gl_para::_mode_filename[]; //	= "mode.ini";
constexpr char gl_para::_conf_filename[];              //  = "conf.ini";              // constexpr char gl_para::_conf_filename[]; //	= "conf.ini";
constexpr char gl_para::_browser_conf_filename[];      //  = "browser.conf";  // constexpr char gl_para::_browser_conf_filename[]; //  = "browser.conf";
constexpr char gl_para::_dock_conf_filename[];         //  = "dock.ini";         // constexpr char gl_para::_dock_conf_filename[]; //  = "dock.ini";
constexpr char gl_para::_dock_settings_section_name[]; //  = "system";   // constexpr char gl_para::_dock_settings_section_name[]; //  = "system";
constexpr char gl_para::_editor_conf_filename[];       //  = "editorconf.ini"; // constexpr char gl_para::_editor_conf_filename[]; //  = "editorconf.ini";
constexpr char gl_para::_data_conf_filename[];         //  = "database.ini";     //	constexpr char gl_para::*_browser_dock_conf_filename; //  = "entrance.ini";//static constexpr char _entrance_conf_filename[]; //  = "entrance.ini";
constexpr char gl_para::_stylesheet_filename[];        //  = "stylesheet.css";  // constexpr char gl_para::_stylesheet_filename[]; //  = "stylesheet.css";
// constexpr char _document_config_name[]; //  = "document.ini";
constexpr char gl_para::_file_menu_name[];     //  = "file_menu";
constexpr char gl_para::_tools_menu_name[];    //  = "tools_menu";
constexpr char gl_para::_edit_menu_name[];     //  = "edit_menu";
constexpr char gl_para::_view_menu_name[];     //  = "view_menu";
constexpr char gl_para::_window_menu_name[];   //  = "window_menu";
constexpr char gl_para::_help_menu_name[];     //  = "help_menu";
constexpr char gl_para::_history_menu_name[];  //  = "history_menu";
constexpr char gl_para::_bookmark_menu_name[]; //  = "bookmark_menu";


constexpr char gl_para::_default_topic[]; // = "undefined";
constexpr char gl_para::_default_post[];  // = "Welcome to \"undefined\" topic";
#ifdef USE_CURRENT_BROWSER_KEY_WORD
constexpr char gl_para::_current_browser[];        // = "current_browser";
#endif                                             // USE_CURRENT_BROWSER_KEY_WORD
constexpr char gl_para::_browser_navigater_name[]; // = "browser_navigater";
constexpr char gl_para::_browser_lineedit_stack_name[];
constexpr char gl_para::_blog_root_dir[];               // = "blog";
constexpr char gl_para::_custom_hidable_tab_style[];    // = "custom_hidable_tab_style.css";
constexpr char gl_para::_custom_browser_tabbar_style[]; // = "custom_browser_tabbar_style.css";
constexpr int gl_para::initialize_priority;             // = 0;


gl_para::gl_para(QObject* pobj)
    : mode_full_name_in_app_data_path_system([&] {
	    return _app_data_path_system + "/" + gl_para::_mode_filename;
    })
    , init([&]() -> std::tuple<const bool, const QString> {
#ifdef TEST_CONFIG_PATH
	    auto recommend_path =
		QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	    auto organization = QCoreApplication::organizationName(); // Unknow
	    auto appname = QCoreApplication::applicationName();
	    QSettings ini(
		QSettings::IniFormat, QSettings::UserScope,
		gl_para::
		    _program_instance_name // QCoreApplication::organizationName()
		,
		QCoreApplication::applicationName());
	    QString dir = QFileInfo(ini.fileName()).absolutePath();
#endif // TEST_CONFIG_PATH
// _main_program_full_file = app_name;
// QFileInfo fi(app_name);
// _program_instance_name = fi.fileName();
// _candidate_paths_by_system = std::make_pair(QDir::homePath() + "/.config/" +
// gl_para::_program_instance_name, QDir::homePath() + "/." +
// gl_para::_program_instance_name);
#ifdef USE_ALTERNATIVE_PATH
	    // if(QFile(_candidate_paths_by_system.first).exists())
	    // _config_path_given_by_system = _candidate_paths_by_system.first;
	    // else if(QFile(_candidate_paths_by_system.second).exists())
	    // _config_path_given_by_system = _candidate_paths_by_system.second;
	    if (!QDir(QDir::homePath() + "/.config/").exists() ||
		!QDir(QDir::homePath() + "/.config/" + gl_para::_program_instance_name)
		     .exists()) {
		    if (QDir::root().mkdir(_candidate_paths_by_system.first))
			    _config_path_given_by_system = _candidate_paths_by_system.first;
	    } else if (QDir::root().mkdir(_candidate_paths_by_system.second))
		    _config_path_given_by_system = _candidate_paths_by_system.second;
	    else
		    critical_error("Can\'t create normal dir for current user.");
#endif // USE_ALTERNATIVE_PATH
	    // assert(_config_path_given_by_system != ".");

	    // mode_file_location = _standard_path + "/" + gl_para::_mode_filename;
	    auto t = mode_full_name_in_app_data_path_system();
	    if (!QFile(t).exists()) {
		    auto s = QString(":/resource/standardconfig/") + target_os() + "/" +
			_mode_filename;
		    // if(!QFile::copy(s, t)) critical_error("Can not copy mode.ini"); //
		    // throw std::runtime_error("Can not copy mode.ini");
		    // else QFile::setPermissions(_standard_path + "/" + _mode_filename,
		    // QFile::ReadUser | QFile::WriteUser);
		    if (!DiskHelper::copy_file_force(s, t))
			    critical_error(
				"Unhandled error encountered when force copy file \"" + s +
				"\" to \"" + t + "\"");
	    }
	    if ((QFile::ReadUser | QFile::WriteUser) !=
		(QFile::permissions(t) & (QFile::ReadUser | QFile::WriteUser)))
		    QFile::setPermissions(t, QFile::ReadUser | QFile::WriteUser);
	    assert(t != (QString("./") + gl_para::_mode_filename));
	    QSettings mode_ini(t, QSettings::IniFormat);
	    auto root_path_from_app_data_path_system =
		mode_ini.value(gl_para::_program_root_dir_name).toString();
	    QString temp_root = ".";
	    if (root_path_from_app_data_path_system != ".")
		    temp_root = root_path_from_app_data_path_system;
	    else
		    temp_root = _app_data_path_system;
	    // auto result =
	    return permanent_coordinate_root(
		temp_root); // app_mode() == standardItem,
			    // assert(std::get<0>(result));
			    // root_path(std::get<1>(result));
			    // Инициализация рабочей директории
    })
{
	Q_UNUSED(pobj);
	init();

	_editors_shared_full_path_name = [&]() -> QString {
		//			  // Handle local directory settings; a default is used if none is
		//specified
		//			  if(gl_paras->editors_shared_full_path_name().isEmpty()){
		QString path =
#ifdef Q_OS_WIN32
		    QString(sapp_t::instance()->isSandbox() ? QString("%1\\") + gl_para::_blog_root_dir + "-sandbox" : QString(QString("%1\\") + gl_para::_blog_root_dir))
			.arg(root_path()); // QDir::homePath()
#else
		    QString(sapp_t::instance()->isSandbox() ? QString("%1/") + gl_para::_blog_root_dir + "-sandbox" : QString(QString("%1/") + gl_para::_blog_root_dir))
			.arg(root_path()); // + "/" + globalparameters.target_os());
#endif
		//			  }
		if (!QDir(path).exists())
			if (!QDir::root().mkpath(path))
				critical_error("Can not create directory: \"" + path + "\"");
		return path;
	}();
}

gl_para::~gl_para()
{
	if (_download_manager) {
		delete _download_manager;
		_download_manager = nullptr;
	}
}

// void GlobalParameters::main_program_full_file(QString
// file){_main_program_full_file = file;}

QString gl_para::main_program_full_file(void) const
{
	return _main_program_full_file;
}

// void gl_para::init(){}// const QString &app_name

// static const int mode_initilized = 3;

// QString gl_para::app_mode() const {
////    if(! QFile(_main_program_path + "/mode.ini").exists()){
////	if(! QFile::copy(QString(":/resource/standardconfig/") + target_os() +
///"/" + "mode.ini", _main_program_path + "/mode.ini")) throw
///std::runtime_error("Can not copy mode.ini");
////	else QFile::setPermissions(_main_program_path + "/mode.ini",
///QFile::ReadUser | QFile::WriteUser);
////    }
// QSettings	setting(_main_program_path + "/mode.ini", QSettings::IniFormat);
// QString	mode = setting.value("application_mode").toString();

// return mode;
// }

// void gl_para::app_mode(const QString &mode){
// QSettings setting(_main_program_path + "/mode.ini", QSettings::IniFormat);

// setting.setValue("application_mode", mode);
// }

QString gl_para::root_path(void) const
{
	// return get_parameter(gl_para::_program_root_dir_name);
	assert(QDir(_app_data_path_system).exists());
	// auto mode_file_location = _standard_path + "/" + _mode_filename;
	QSettings mode_ini(mode_full_name_in_app_data_path_system(), QSettings::IniFormat);
	auto root_name = gl_para::_program_root_dir_name;
	QString result = "";
	if (mode_ini.contains(root_name))
		result = mode_ini.value(root_name).toString();
	else {
		// if(QFile(mode_file_full_name_by_system()).exists())
		// QFile::remove(mode_file_full_name_by_system());
		// if(!QFile::copy(QString(":/resource/standardconfig/") +
		// gl_paras->target_os() + "/" + QString(_mode_filename),
		// mode_file_full_name_by_system())) critical_error("Can not copy " +
		// QString(_mode_filename));
		// if((QFile::ReadUser | QFile::WriteUser) !=
		// (QFile::permissions(mode_file_full_name_by_system()) & (QFile::ReadUser |
		// QFile::WriteUser)))
		// QFile::setPermissions(mode_file_full_name_by_system(), QFile::ReadUser |
		// QFile::WriteUser);
		init();
		QSettings mode_new(mode_full_name_in_app_data_path_system(), QSettings::IniFormat);
		result = mode_new.value(root_name).toString();
		if (result == "")
			critical_error("In " + QString(_mode_filename) + " not found parameter " + root_name);
	}
	if (result.endsWith("/"))
		result.truncate(result.length() - 1);
	result = QDir(result).path();
	return result;
}

// Инициализация рабочей директории
// Если рабочая директория уже существует, она будет установлена как рабочая
// Если ребочая директория не будет найдена, будут создана новая рабочая
// директория
// с начальными файлами и она будет установлена как рабочая

// Initialization working directory
// If the working directory already exists, it will be installed as a working
// directory.
// If the directory is not found, it will create a new working directory with
// initial files and it will be set as the working directory
std::tuple<const bool, const QString>
gl_para::permanent_coordinate_root(const QString& recommend_root_path_,
    bool force)
{ // bool standard_mode,
	auto final_root = std::tuple<bool, QString>(
	    false, recommend_root_path_ == "" ? root_path() : recommend_root_path_);

	////    // you can't do this, because your appconfig is not initialized yet.
	////    AppConfigDialog appconfigdialog(nullptr, "pageMain");
	////    appconfigdialog.show();

	//// Поиск файла conf.ini в той же директории, где находится бинарник
	//// Search conf.ini file in the same directory where the binary

	//// Нужно учесть, что программу могут запускать из другой директории
	//// QDir::currentPath() - выдает директорию, где была выполнена команда
	///запуска
	//// mainProgramFile - содержит путь к бинарнику относительно директории
	///запуска

	//// It is necessary to take into account that the program can be run from a
	///different directory
	//// QDir::currentPath() - provides a directory where you have run run
	//// MainProgramFile - contains the path to the binaries relative to the
	///directory run

	auto recover_config = [&](const QString& root_path_local) {
		auto is_ini_config_exist = [&](const QString& config_filename_) -> bool {
			qDebug() << "Check config file " << config_filename_;
			bool result = false;
			QFileInfo info(config_filename_);
			// Если файл существует, надо определить, от он или от другой программы
			if (info.exists()) {
				qDebug() << "Config file " << config_filename_ << " is exists";

				// Выясняется имя файла без пути к директории
				QString short_filename = info.fileName();
				qDebug() << "Short config file name " << short_filename;

				// Выясняется имя директории из имени файла
				QDir dir = info.dir();
				QString dirname = dir.absolutePath();
				qDebug() << "Config directory name " << dirname;

				// Открывается хранилище настроек
				QSettings app_conf(config_filename_, QSettings::IniFormat);
				// conf->setPath(QSettings::IniFormat, QSettings::UserScope, dirName);
				// conf->setPath(QSettings::IniFormat, QSettings::SystemScope, dirName);
				// Если есть переменная version
				if (app_conf.contains("version")) {
					int version = app_conf.value("version").toInt();
					// Если номер версии конфига до 3 включительно
					if (version <= 3) {
						// В этих версиях небыло переменной programm, поэтому проверяется
						// переменная datadir
						if (app_conf.contains("datadir"))
							result = true; // return true;
								       // else return false;
					} else {
						// Иначе номер версии больше 3
						if (app_conf.contains("programm")) {
							if (app_conf.value("programm").toString() == application_name())
								result = true; // return true;
									       // else return false;
						}                              // else return false;
					}
				} // else return false;	// Нет переменной version
			}         // else return false;	// Нет указанного файла
			return result;
		};
		qDebug() << "Detect home directory " << root_path_local;
		qDebug() << "Try find conf.ini in home subdirectory " << root_path_local;
		if (!QFile(root_path_local).exists())
			if (!QDir::root().mkpath(root_path_local))
				critical_error("Can not make path \"" + root_path_local + "\"");
		auto config_path = root_path_local + "/" + target_os();
		if (!QFile(config_path).exists())
			if (!QDir::root().mkpath(config_path))
				critical_error("Can not make path \"" + config_path + "\""); // throw std::runtime_error("Can not make path \""
											     // + config_path.toStdString() + "\"");
		auto check_exists_and_permission = [&](const QString& name) {
			// auto	config_path	= root_path + "/" + target_os();
			auto config_location = config_path + "/" + name;
			if (!QFile(config_location).exists())
				if (!QFile::copy(QString(":/resource/standardconfig/") + target_os() + "/" + name, config_location))
					critical_error("Can not copy " + name); // throw
										// std::runtime_error("Can not
										// copy " +
										// name.toStdString());
			if ((QFile::ReadUser | QFile::WriteUser) !=
			    (QFile::permissions(config_location) &
				(QFile::ReadUser | QFile::WriteUser)))
				QFile::setPermissions(config_location, QFile::ReadUser | QFile::WriteUser);
		};
		auto config_full_name = config_path + "/" + gl_para::_conf_filename;
		// Если директория существует и в ней есть настоящий файл конфигурации
		if (!is_ini_config_exist(config_full_name)) {
			// qDebug() << "Config init file success find in home subdirectory " <<
			// root_dir;
			// else
			qDebug() << "File conf.ini can\'t be found in configuration path: \""
				 << config_path << "\"";
			// QFileInfo file(root_dir + "/" + target_os() + "/conf.ini");
			// if(! (file.exists() && file.isFile())) QFile::remove(root_dir + "/" +
			// target_os() + "/conf.ini");

			check_exists_and_permission(gl_para::_conf_filename);
		} else if ((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(config_full_name) & (QFile::ReadUser | QFile::WriteUser))) {
			//
			QFile::setPermissions(config_full_name, QFile::ReadUser | QFile::WriteUser);
		}
		// if(! QFile(config_path + _browser_conf_filename).exists())
		check_exists_and_permission(gl_para::_browser_conf_filename);
		// if(! QFile(config_path + _browserview_conf_filename).exists())
		check_exists_and_permission(gl_para::_dock_conf_filename);
		// if(! QFile(config_path + _editor_conf_filename).exists())
		check_exists_and_permission(gl_para::_editor_conf_filename);
		// if(! QFile(config_path + _entrance_conf_filename).exists())
		//			  check_exists_and_permission(gl_para::_browser_dock_conf_filename);
		// if(! QFile(config_path + _stylesheet_filename).exists())
		check_exists_and_permission(gl_para::_stylesheet_filename);
		std::get<1>(final_root) = root_path_local;

		// QSettings app_conf(_main_program_path + "/" + _mode_filename,
		// QSettings::IniFormat);
		// app_conf.setValue(gl_para::_program_root_dir_name, _root_path);
	};
	//
	auto permanent_root_info_to_app_data_path_system =
	    [&](QString target_root) -> bool {
		assert(QDir(_app_data_path_system).exists());
		auto t = mode_full_name_in_app_data_path_system();
		if (!QFile(t).exists()) {
			auto s = QString(":/resource/standardconfig/") + target_os() + "/" +
			    _mode_filename;
			// if(!QFile::copy(s, t)) critical_error("Can not copy mode.ini"); //
			// throw std::runtime_error("Can not copy mode.ini");
			// else QFile::setPermissions(_standard_path + "/" + _mode_filename,
			// QFile::ReadUser | QFile::WriteUser);
			if (!DiskHelper::copy_file_force(s, t))
				critical_error("Unhandled error encountered when force copy file \"" + s + "\" to \"" + t + "\"");
		}
		if ((QFile::ReadUser | QFile::WriteUser) !=
		    (QFile::permissions(t) & (QFile::ReadUser | QFile::WriteUser)))
			QFile::setPermissions(t, QFile::ReadUser | QFile::WriteUser);
		//
		QSettings mode_ini(t, QSettings::IniFormat);
		//
		QDir directory(target_root);
		if (!directory.exists())
			if (DiskHelper::create_directory(QDir::rootPath(), target_root) !=
			    target_root)
				critical_error("What\'s wrong? Can\'t create directory as want");
		if (directory.exists() && directory.isReadable()) {
			if (mode_ini.value(gl_para::_program_root_dir_name).toString() !=
			    target_root)
				mode_ini.setValue(gl_para::_program_root_dir_name, target_root);
			mode_ini.sync();

			//// write
			// ConfigINI *ini = new ConfigINI(mode_file_full.toStdString().c_str());
			////	ini->setIntValue("section1", "intValue", 1);
			////	ini->setFloatValue("section1", "floatValue", 0.1);
			// ini->setStringValue("General", gl_para::_program_root_dir_name,
			// path_name.toStdString().c_str());
			////	ini->setBoolValue("section2", "boolValue", true);
			// ini->writeConfigFile();
			// delete ini;
			assert(root_path() == target_root);
			return true;
		} else
			return false;
	};
//
#ifdef USE_ALTERNATIVE_PATH
	auto determine_standard_mode_path = [&]() -> QString {
		// obtain (platform specific) application's data/settings directory
		QSettings ini(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
		QString dir = QFileInfo(ini.fileName()).absolutePath();
		// on Windows - "%APPDATA%\<Organization Name>" ("<Drive>:\Documents and
		// Settings\<login>\<Application Data>\<Organization Name>")
		// on Unix and Mac OS X - "$HOME/.config/<Organization Name>"
		QString standard_path_ = "";
		// Если в текущей директории запуска нет conf.ini
		// Поиск файла conf.ini в домашней директории пользователя
		// в поддиректории ".имя_программы"
		if (!QDir(_candidate_mode_paths_by_system.first).exists()) {
			// Иначе директории "~/.имя_программы" нет
			// и нужно пробовать найти данные в "~/.config/имя_программы"
			qDebug() << "File conf.ini can\'t find in home directory \""
				 << _candidate_mode_paths_by_system.first << "\"";
			if (!QDir::root().mkpath(_candidate_mode_paths_by_system.first))
				critical_error(
				    "auto figure_out_standard_work_path = [&] {...} can not "
				    "make path \"" +
				    standard_path_ + "\"");
			else
				standard_path_ = _candidate_mode_paths_by_system.first;
		} else if (!QDir(_candidate_mode_paths_by_system.second).exists()) {
			qDebug() << "File conf.ini can\'t find in home directory \""
				 << _candidate_mode_paths_by_system.second << "\"";
			if (!QDir::root().mkpath(_candidate_mode_paths_by_system.second))
				critical_error(
				    "auto figure_out_standard_work_path = [&] {...} can not "
				    "make path \"" +
				    standard_path_ + "\"");
			else
				standard_path_ = _candidate_mode_paths_by_system.second;
		} else if (QDir(_candidate_mode_paths_by_system.first).exists())
			standard_path_ = _candidate_mode_paths_by_system.first;
		else
			standard_path_ = _candidate_mode_paths_by_system.second;
		return _mode_path_given_by_system =
			   standard_path_; // recover_config(standard_path_);
	};
#endif // USE_ALTERNATIVE_PATH
	auto path_could_be_reach_from_system = [&](const QString& new_mode_position) {
		return
#ifdef USE_ALTERNATIVE_PATH
		    (new_mode_position == _candidate_mode_paths_by_system.first ||
			new_mode_position == _candidate_mode_paths_by_system.second) &&
		    QFile(new_mode_position).exists()
#else
		    QFile(new_mode_position).exists()
#endif // USE_ALTERNATIVE_PATH
			;
	};

	auto is_coordinate = [&](QString target_root) { // bool mode_std,
		assert(std::get<1>(final_root) != "");
		bool result_coordinate = false;

#ifdef USE_FILE_PER_TREEITEM
		QSettings conf(std::get<1>(result) + "/" + target_os() + "/" + _conf_filename, QSettings::IniFormat);
		auto datadir = conf.value("datadir").toString();
		auto trashdir = conf.value("trashdir").toString();
#endif // USE_FILE_PER_TREEITEM
		if (path_could_be_reach_from_system(
#ifdef USE_ALTERNATIVE_PATH
			determine_standard_mode_path()
#else
			_app_data_path_system
#endif // USE_ALTERNATIVE_PATH
			    )) {
			if (QFile(mode_full_name_in_app_data_path_system()).exists()) {
				QSettings mode_ini(mode_full_name_in_app_data_path_system(), QSettings::IniFormat);
				if (mode_ini.value(gl_para::_program_root_dir_name).toString() ==
				    target_root) {
					//
					//
					// if(path_could_be_reach_from_system(QDir::currentPath())){
					//
					result_coordinate = true;
					// }
				}
			}
		}
		return result_coordinate
#ifdef USE_FILE_PER_TREEITEM
		    && is_standard_path(QDir(datadir.remove("/data")).absolutePath()) &&
		    is_standard_path(QDir(trashdir.remove("/trash")).absolutePath())
#endif // USE_FILE_PER_TREEITEM
		    ;
	};
	//
	auto change_current_to_portable_root = [&](const QString& root_path_) {
		auto result_current =
		    std::tuple<bool, QString>(false, root_path_); // , std::get<2>(result)
		// Устанавливается эта директория как рабочая
		if (!QDir::setCurrent(root_path_))
			critical_error("Can not set work directory as '" + root_path_ + "'. System problem.");
		// std::get<0>(result_current) = true;
		// else
		// root_path(root_path_);
		assert(QDir::currentPath() == root_path_);
		if (is_coordinate(root_path_))
			std::get<0>(result_current) = true;
		return result_current;
	};
	auto check_current_root =
	    [&](const QString& target_root_current) { // (bool is_standard_mode)
		    auto result_current = final_root; // std::make_pair(false, _root_path);
		    if (!is_coordinate(target_root_current)) {
			    //// Если рабочая директория не определена
			    // if(current_path_is_standard){
			    // if(QDir::currentPath() != target_root_current) result_current =
			    // change_current(target_root_current); // standardItem,
			    // else std::get<0>(result_current) = true;
			    // }else{
			    // recover_config(root_path_);
			    // if(QDir::currentPath() != target_root_current) result_current =
			    // change_current(target_root_current); // portableItem
			    // else std::get<0>(result_current) = true;
			    // }
			    ////
		    } else
			    std::get<0>(result_current) = true;
		    // else{
		    // QMessageBox message;
		    // message.setText("Root directory and application mode have
		    // conflicts.");
		    // message.exec();
		    // }

		    return result_current;
	    };
	//// Если возможно создать только стандартную версию файлового окружения
	// if(! enablePortable){
	// qDebug() << "Can\'t create portable version - cant write data to bin-file
	// directory";
	//// Если рабочая директория найдена автоматически
	// if(! set_current(enablePortable)){
	// QString infoText = tr("The following actions will be performed before
	// running this application: \n\n") +
	// standartText + "\n\n" +
	// tr("Do you agree to perform these?");
	// QMessageBox msgBox;
	// msgBox.setText(welcomeText);
	// msgBox.setInformativeText(infoText);
	// msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	// msgBox.setDefaultButton(QMessageBox::Ok);
	// int ret = msgBox.exec();
	// if(ret == QMessageBox::Ok) create_standard_programfiles();
	// else exit(0);
	// }
	//// Была нажата отмена
	// fix_workdirectory_standard();
	// }else{
	// Если рабочая директория найдена автоматически
	// auto cur_result=set_current_root(enablePortable);
	// while(! std::get<0>(result)){
	while (!std::get<0>(final_root = check_current_root(std::get<1>(final_root)))) { // app_mode() == standardItem
		// Иначе есть возможность создать как стандартное файловое окружение,
		// так и "переносимое"
		QString target_root_path =
		    std::get<1>(final_root); // _root_path_given_by_system;
#ifdef USE_DEFAULT_PATH
		QString dataDirName = ".config/" + application_name() + "/data";

		QString welcomeText =
		    tr(std::string("Welcome to " + program_title_string + " v.").c_str()) +
		    QString::number(APPLICATION_RELEASE_VERSION) + '.' +
		    QString::number(APPLICATION_RELEASE_SUBVERSION) + '.' +
		    QString::number(APPLICATION_RELEASE_MICROVERSION) + "!";

		QString standartText = tr("Create subdirectory  \"%1\"\nin user directory  "
					  "\"%2\",\nand create application files in it.")
					   .arg(dataDirName)
					   .arg(QDir::homePath());

		QString portableText =
		    tr("Create application files\nin current directory  \"%1\".")
			.arg(std::get<1>(result));

		QString infoText = welcomeText + "\n\n" +
		    tr("Please, select application path: \n\n") +
		    tr("Standart:\n") + standartText + "\n\n" +
		    tr("Portable:\n") + portableText + "\n\n";

		QStringList items;
		// QString		standardItem	= tr("Standard");
		// QString		portableItem	= tr("Portable");
		items << root_path() << _candidate_paths_by_system.first
		      << _candidate_paths_by_system.second
		      << QFileInfo(_main_program_full_file)
			     .absolutePath(); // standardItem << portableItem;

		// QInputDialog inputDialog;
		// inputDialog.setLabelText("<b>"+welcomeText+"</b>\n\n"+infoText);
		// inputDialog.setComboBoxItems(items);

		bool ok;
		std::unique_ptr<QWidget> tempWidget(new QWidget());
		target_root_path = QInputDialog::getItem(tempWidget.get(), welcomeText, infoText, items, 0, false, &ok);
#endif // USE_DEFAULT_PATH
		// auto d = new QFileDialog();
		// d->setFileMode(QFileDialog::Directory);
		// d->setOption(QFileDialog::ShowDirsOnly);
		// int result_ = d->exec();
		// if(result_)

		//		AppConfigPageMain::get_root_path();

		target_root_path =
		    force ? std::get<1>(final_root) // d->selectedFiles()[0];//
			    :                       // QFileDialog::getExistingDirectory(new QWidget(), "Specific
		    // the applications\'s data center path", std::get<1>(result),
		    // QFileDialog::ShowDirsOnly);
		    [&] {
			    QFileDialog* fd = new QFileDialog;
			    QTreeView* tree = fd->findChild<QTreeView*>();
			    tree->setRootIsDecorated(true);
			    tree->setItemsExpandable(true);
			    fd->setWindowTitle(
				"Specific the applications\'s data center path");
			    fd->setFileMode(QFileDialog::Directory);
			    fd->setOption(QFileDialog::ShowDirsOnly);
			    fd->setViewMode(QFileDialog::Detail);
			    int result_ = fd->exec();
			    QString directory = std::get<QString>(final_root);
			    if (result_) {
				    directory =
					fd->directory().absolutePath(); // selectedFiles()[0];
				    qDebug() << "Selected root path: \"" << directory << "\"";
			    }
			    delete fd;
			    return directory;
		    }();
// auto create_stylesheet_file = [&](QString config_dir){
// QString target_os_ = target_os();

// QFile::copy(":/resource/standardconfig/" + target_os_ + "/stylesheet.css",
// config_dir + "/stylesheet.css");
// QFile::setPermissions(config_dir + "/stylesheet.css", QFile::ReadUser |
// QFile::WriteUser);
// };

#ifdef USE_FILE_PER_TREEITEM
		// Создание первоначального набора файлов в указанной директории
		// Create the initial set of files in the specified directory
		auto initialize_index = [&](const QString& root_path_local) {
			qDebug() << "Create first program files in directory " << root_path_local;

			QDir dir(root_path_local);

			// Создается дерево директорий в указанной директории
			dir.mkpath("data/base/0000000001");
			dir.mkdir("trash");

			//// Создаются файлы конфигурации
			// QString	target_os_	= target_os();	// "any" или
			// "meego" или "android"
			// auto	config_path	= root_dir + "/" + target_os_;
			// QFile::copy(":/resource/standardconfig/" + target_os_ + "/conf.ini",
			// config_path + "/conf.ini");
			// QFile::setPermissions(config_path + "/conf.ini", QFile::ReadUser |
			// QFile::WriteUser);

			// QFile::copy(":/resource/standardconfig/" + target_os_ +
			// "/editorconf.ini", config_path + "/editorconf.ini");
			// QFile::setPermissions(config_path + "/editorconf.ini", QFile::ReadUser
			// | QFile::WriteUser);

			// QFile::copy(":/resource/standardconfig/" + target_os_ +
			// "/entrance.ini", config_path + "/entrance.ini");
			// QFile::setPermissions(config_path + "/entrance.ini", QFile::ReadUser |
			// QFile::WriteUser);

			// create_stylesheet_file(config_path);
			// recover_config(root_path_local);

			// Создается файл базы данных
			QFile::copy(":/resource/standarddata/index.xml", root_path_local + "/data/" + index_xml_file_name);
			QFile::setPermissions(root_path_local + "/data/" + index_xml_file_name, QFile::ReadUser | QFile::WriteUser);

			// Создается файл первой записи
			QFile::copy(":/resource/standarddata/base/0000000001/text.html", root_path_local + "/data/base/0000000001/text.html");
			QFile::setPermissions(root_path_local + "/data/base/0000000001/text.html", QFile::ReadUser | QFile::WriteUser);
		};
#endif // USE_FILE_PER_TREEITEM

		auto new_root_fullfill = [&](const QString& target_root_path_) {
			qDebug() << "Create standart program files";
			if (!QDir(target_root_path_).exists()) {
				if (QDir::root().mkpath(target_root_path_)) {
					qDebug() << "Successfull create subdirectory "
						 << target_root_path_ // dataDirName
						 << " in directory "
						 << QDir::rootPath() // userDir.absolutePath()
					    ;
					std::get<1>(final_root) = target_root_path_; // standard_root_path;
				} else {
					critical_error("Can not created directory \"" + target_root_path_ + "\" in user directory \"" + QDir::rootPath() + "\"");
					exit(0);
				}
			}
			recover_config(target_root_path_); // standard_root_path

			////				  QString dataDirName = ".config/" +
			///application_name();
			// if(userDir.mkpath(_standard_paths.first)){  // dataDirName
			// qDebug()	<< "Successfull create subdirectory " <<
			// _standard_paths.first // dataDirName
			// << " in directory " << QDir::homePath() // userDir.absolutePath()
			// ;

			////		    QString standard_root_path = userDir.absolutePath()
			///+ "/" + dataDirName;		// Ранее использовался QDir::homePath()

			// initialize_root_child(_standard_paths.first); // standard_root_path
			// std::get<1>(result) = _standard_paths.first; // standard_root_path;
			////		    app_mode(standardItem);
			// }else if(userDir.mkpath(_standard_paths.second)){
			// qDebug()	<< "Successfull create subdirectory " <<
			// _standard_paths.second // dataDirName
			// << " in directory " << QDir::homePath() // userDir.absolutePath()
			// ;

			// initialize_root_child(_standard_paths.second); // standard_root_path
			// std::get<1>(result) = _standard_paths.second; // standard_root_path;
			// }else{
			// critical_error("Can not created directory \"" + _standard_paths.first +
			// "\" or \"" + _standard_paths.second + "\" in user directory \"" +
			// QDir::homePath() + "\"");
			// exit(0);
			// }
		};
		// Если пользователь сделал выбор
		if (
#ifdef USE_DEFAULT_PATH
		    ok &&
#endif // USE_DEFAULT_PATH
		    !target_root_path.isEmpty()) {
			new_root_fullfill(target_root_path); // fix_root_dir_portable();
			permanent_root_info_to_app_data_path_system(target_root_path);
#ifdef USE_FILE_PER_TREEITEM
			initialize_index(
			    target_root_path); // create_things_for_portable_root(target_root_path);
#endif                                         // USE_FILE_PER_TREEITEM

			change_current_to_portable_root(target_root_path); // portableItem,
			// root_path(QDir(return_path).absolutePath());
			final_root = check_current_root(
			    QDir(target_root_path).absolutePath()); // PORTABLE_MODE
								    // }
		}
	}
	// else std::get<0>(result) = true;
	// }
	// Была нажата отмена
	// fix_workdirectory_portable();
	// }

	//// Search restarts working directory    // Заново запускается поиск рабочей
	///директории
	// _work_directory = "";
	// check_workdirectory();

	assert(std::get<0>(final_root));
	// permanent_root_info_to_system_given_path(std::get<1>(result));

	return final_root;
}

// QString gl_para::main_program_dir() const {return _main_program_path;}

void gl_para::style_source(const QString& source)
{
	_style_source = source;
}

QString gl_para::style_source() const
{
	return _style_source;
}

// QString GlobalParameters::root_path(void) const {return _root_path;}
// QString gl_para::config_filename(void) const {return
// gl_para::_conf_filename;}

QString gl_para::target_os(void) const
{
#if TARGET_OS == ANY_OS

	return "any";
#endif

#if TARGET_OS == MEEGO_OS

	return "meego";
#endif

#if TARGET_OS == ANDROID_OS

	return "android";
#endif
}

// Имя программы в системе
// Используется для создания и поиска каталога с данными пользователя
QString gl_para::application_name(void) const
{
	// todo: Подумать и заменить этот код на значения, полученные из PRO-файла
	QString app_name = "";
	// auto	to			= target_os();
	if (target_os() == "any")
		app_name = gl_para::_program_instance_name;
	if (target_os() == "meego")
		app_name = QString("ru.webhamster") + "." + gl_para::_program_instance_name;
	if (target_os() == "android")
		app_name = QString("ru.webhamster") + "." + gl_para::_program_instance_name;
	// qDebug() << "In getApplicationName() return \"" << appName << "\"";

	return app_name;
}

web::Profile* gl_para::profile() const
{
	return _profile;
}

void gl_para::profile(web::Profile* profile_)
{
	_profile = profile_;
}

QSplitter* gl_para::find_splitter() const
{
	return _find_splitter;
}

void gl_para::find_splitter(QSplitter* _find_splitter)
{
	this->_find_splitter = _find_splitter;
}

void gl_para::h_record_splitter(QSplitter* h_record_splitter)
{
	_h_record_splitter = h_record_splitter;
}

QSplitter* gl_para::h_record_splitter() const
{
	return _h_record_splitter;
}

void gl_para::h_tree_splitter(QSplitter* vleftsplitter)
{
	_v_left_splitter = vleftsplitter;
}

QSplitter* gl_para::h_tree_splitter() const
{
	return _v_left_splitter;
}

void gl_para::v_right_splitter(QSplitter* vrightsplitter)
{
	_v_right_splitter = vrightsplitter;
}

QSplitter* gl_para::v_right_splitter() const
{
	return _v_right_splitter;
}

void gl_para::vtab_record(HidableTab* point)
{
	_vtab_record = point;
}

HidableTab* gl_para::vtab_record() const
{
	return _vtab_record;
}

// void GlobalParameters::vtab_tree(QTabWidget *point)
// {
// _vtab_tree = point;
// }

// QTabWidget *GlobalParameters::vtab_tree()
// {
// return _vtab_tree;
// }

web::DownloadManager* gl_para::request_download_manager()
{
	//	bool found = false;
	if (!_download_manager) {
		_download_manager =
		    new web::DownloadManager(download_manager_singleton_name, _vtab_record);
	}

	//	for (int i = 0; i < _vtab_record->count(); i++) {
	//		auto widget_ = _vtab_record->widget(i);
	//		if (widget_->objectName() == download_manager_singleton_name) {
	//			_download_manager = dynamic_cast<web::DownloadManager*>(widget_);
	//			found = true;
	//			break;
	//		}
	//	}
	//	if(!found){}
	if (_vtab_record->indexOf(_download_manager) == -1)
		_vtab_record->addTab(static_cast<QWidget*>(_download_manager), QIcon(":/resource/pic/apple.svg"), "Download");

	// if(! _download_manager)_download_manager = new
	// web::DownloadManager(download_manager_singleton_name, _vtab_record);
	if (_vtab_record->currentIndex() != _vtab_record->indexOf(_download_manager))
		_vtab_record->setCurrentWidget(static_cast<QWidget*>(_download_manager));
	return _download_manager;
}

void gl_para::download_manager(web::DownloadManager* dm)
{
	_download_manager = dm;
}

web::DownloadManager* gl_para::download_manager() const
{
	return _download_manager;
}

//void gl_para::editor_config(std::shared_ptr<EditorConfig> dialog)
//{
//	_editor_config = dialog;
//}

//std::shared_ptr<EditorConfig> gl_para::editor_config() const
//{
//	return _editor_config;
//}

//AttachTableController* gl_para::attachtable_controller() const
//{
//	return _attachtable_controller;
//}

//void gl_para::attachtable_controller(
//    AttachTableController* _attachtable_controller)
//{
//	this->_attachtable_controller = _attachtable_controller;
//}

void gl_para::tree_screen(ts_t* point)
{
	_tree_screen = point;
}

ts_t* gl_para::tree_screen() const
{
	return _tree_screen;
}

web::Docker<web::Browser>* gl_para::browser_docker() const
{
	return _browser_docker;
}

void gl_para::browser_docker(web::Docker<web::Browser>*& b)
{
	_browser_docker = b;
}

void gl_para::push_record_screen(rs_t* point)
{
	_table_screens.push_back(point);
}

//#ifdef USE_SIGNAL_CLOSE
//std::set<sd::intrusive_ptr<rs_t>>
//#else
std::set<rs_t*>
//#endif //USE_SIGNAL_CLOSE
gl_para::record_screens() const
{
//#ifdef USE_SIGNAL_CLOSE
//	std::set<sd::intrusive_ptr<rs_t>>
//#else
	std::set<rs_t*>
//#endif //USE_SIGNAL_CLOSE
	    result;
	for (auto& bro : _mainwindow->browsers()) {
		auto rs = bro->tab_widget()->record_screen();
		if (result.find(rs) != result.end()) result.insert(rs);
	}
	return result;
}

// RecordScreen *GlobalParameters::page_screen() {return _page_screen; }

// void GlobalParameters::page_screen(RecordScreen *page) { _page_screen = page;
// }

void gl_para::find_screen(FindScreen* point)
{
	_find_screen = point;
}

FindScreen* gl_para::find_screen() const
{
	return _find_screen;
}

void gl_para::editor_docker(web::Docker<Blogger>* point)
{
	_editor_docker = point;
}

web::Docker<Blogger>* gl_para::editor_docker() const
{
	return _editor_docker;
}

void gl_para::status_bar(QStatusBar* point)
{
	_statusbar = point;
}

QStatusBar* gl_para::status_bar() const
{
	return _statusbar;
}

void gl_para::window_switcher(WindowSwitcher* point)
{
	_window_switcher = point;
}

WindowSwitcher* gl_para::window_switcher() const
{
	return _window_switcher;
}

wn_t* gl_para::main_window() const
{
	return _mainwindow;
}

void gl_para::main_window(wn_t* mainwindow)
{
	_mainwindow = mainwindow;
}

void gl_para::crypt_key(QByteArray hash)
{
	_password_hash = hash;
}

QByteArray gl_para::crypt_key(void) const
{
	return _password_hash;
}
//// deprecated
// QMap<QString, QString>  GlobalParameters::config_ini() const {
// QMap<QString, QString>	result;
// QString			content =
// "[General]\r\n"
// "addnewrecord_expand_info=1\r\n"
// "application_mode=Portable\r\n"
// "attachAppendDir=\r\n"
// "attachSaveAsDir=\r\n"
// "autoClosePasswordDelay=10\r\n"
// "autoClosePasswordEnable=false\r\n"
// "customDateTimeFormat=\r\n"
// "cutbranchconfirm=true\r\n"
// "editorCursorPosition=0\r\n"
// "editorScrollBarPosition=0\r\n"
// "editor_show=false\r\n"
// "enableCustomDateTimeFormat=false\r\n"
// "enableDecryptFileToTrashDirectory=false\r\n"
// "findInBaseExpand=false\r\n"
// "findScreenTreeSearchArea=0\r\n"
// "find_splitter_sizelist=\"657,23\"\r\n"
// "findscreen_find_in_author=true\r\n"
// "findscreen_find_in_home=true\r\n"
// "findscreen_find_in_name=true\r\n"
// "findscreen_find_in_pin=true\r\n"
// "findscreen_find_in_tags=true\r\n"
// "findscreen_find_in_text=false\r\n"
// "findscreen_find_in_url=true\r\n"
// "findscreen_find_inauthor=true\r\n"
// "findscreen_find_inname=true\r\n"
// "findscreen_find_intags=true\r\n"
// "findscreen_find_intext=true\r\n"
// "findscreen_find_inurl=false\r\n"
// "findscreen_howextract=1\r\n"
// "findscreen_show=true\r\n"
// "findscreen_wordregard=0\r\n"
// "focusWidget=\r\n"
// "h_left_splitter_sizelist=\"28,161\"\r\n"
// "h_record_splitter_sizelist=\"245,1121\"\r\n"
// "h_right_splitter_sizelist=\"189,1177\"\r\n"
// "h_tree_splitter_sizelist=\"0,1366\"\r\n"
// "hideEditorTools=\r\n"
// "horizontal_scrollbar_style_sheet=\"QTabWidget::pane { border: 0 px; }
// \"\r\n"
// "howpassrequest=atClickOnCryptBranch\r\n"
// "interfaceMode=desktop\r\n"
// "interfacelanguage=en\r\n"
// "mainwingeometry=\"0,29,1366,702\"\r\n"
// "passwordMiddleHash=\r\n"
// "passwordSaveFlag=false\r\n"
// "printdebugmessages=true\r\n"
// "programm=tute\r\n"
// "recordTableFieldsWidth=\"25,130,40\"\r\n"
// "recordTableShowFields=\"pin,name,rating\"\r\n"
// "recordTableShowHorizontalHeaders=true\r\n"
// "recordTableShowVerticalHeaders=true\r\n"
// "recordtableSelectedRecordId=\r\n"
// "remember_cursor_at_history_navigation=true\r\n"
// "remember_cursor_at_ordinary_selection=true\r\n"
// "runinminimizedwindow=false\r\n"
// "showSplashScreen=false\r\n"
// "synchrocommand=\r\n"
// "synchroonexit=false\r\n"
// "synchroonstartup=false\r\n"
// "syncroConsoleDetails=false\r\n"
// "datadir=./data\r\n"
// "trashdir=./trash\r\n"
// "trashmaxfilecount=200\r\n"
// "trashsize=5\r\n"
// "tree_intercept=0\r\n"
// "tree_position=\"0,1447724759fsuoi32m8z,1469866853dm12nml5fr,1469881278ob68v7s2fr,146986679219xpkvopqs,14697021199iiwq4betd,1469940289r3z03xcy0q,1469941751igm3m3tqrq\"\r\n"
// "uglyQssReplaceHeightForTableView=0\r\n"
// "v_left_splitter_sizelist=\"146,1220\"\r\n"
// "v_right_sizelist=\"614,0\"\r\n"
// "v_right_splitter_sizelist=\"657,0\"\r\n"
// "version=27\r\n"
// "vertical_scrollbar_style_sheet=\"QTabWidget::pane { border: 0 px; } \"\r\n"
// "vspl_sizelist=\"614,0\"\r\n";
// result.insert("conf.ini", content);

// return result;
// }

// QMap<QString, QString>  GlobalParameters::index_xml() const {
// QMap<QString, QString>	result;
// QString			content =
// "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
// "<!DOCTYPE tutedoc>\r\n"
// "<root>\r\n"
// "   <format subversion=\"2\" version=\"1\"/>\r\n"
// "   <content>\r\n"
////  <node name="Rename me" id="1">
// "   <recordtable>\r\n"
// "	<record name=\"Google\" dir=\"0000000001\" pin=\"1\" tags=\"Tute, Qt\"
// ctime=\"20081210124656\" url=\"https://www.google.com/?gws_rd=ssl\"
// file=\"text.html\" id=\"1\" author=\"\"/>\r\n"
// "   </recordtable>\r\n"
////  </node>
// "   </content>\r\n"
// "</root>\r\n";
// result.insert("index.xml", content);

// return result;
// }

// QMap<QString, QString>  GlobalParameters::editorconf() const {
// QMap<QString, QString>	result;
// QString			content =
// "[General]\r\n"
// "    code_font=\"Courier New,12,-1,5,50,0,0,0,0,0\"\r\n"
// "    code_font_color=#6a1009\r\n"
// "    code_font_size=10\r\n"
// "    code_font_size_apply=1\r\n"
// "    code_indent_size=20\r\n"
// "    code_indent_size_apply=1\r\n"
// "    default_font=\"DejaVu Sans,12,-1,5,50,0,0,0,0,0\"\r\n"
// "    default_font_size=10\r\n"
// "    expand_tools_lines=1\r\n"
// "    finddialog_geometry=\"286,100,235,154\"\r\n"
// "    indent_step=10\r\n"
// "    monospace_font=\"Courier New,12,-1,5,50,0,0,0,0,0\"\r\n"
// "    monospace_font_size=11\r\n"
// "    monospace_font_size_apply=1\r\n"
// "
// tools_line_1=\"freeze_browser_view,clear,bold,italic,underline,monospace,code,separator,alignleft,aligncenter,alignright,alignwidth,numericlist,dotlist,indentplus,indentminus,separator,showformatting,findtext,showhtml,insert_image_from_file,settings,expand_edit_area,expand_tools_lines,attach\"\r\n"
// "
// tools_line_2=\"fontselect,fontsize,createtable,table_add_row,table_remove_row,table_add_col,table_remove_col,table_merge_cells,table_split_cell,show_text\"\r\n"
// "    version=8\r\n";
// result.insert("editorconf.ini", content);

// return result;
// }

SysTrayIcon* gl_para::tray_icon()
{
	return _tray_icon;
}
void gl_para::tray_icon(SysTrayIcon* ti)
{
	_tray_icon = ti;
}
void gl_para::global_home(const QUrl& url_)
{
	_global_home = url_;
}
QUrl gl_para::global_home() const
{
	return _global_home;
}

QString gl_para::editors_shared_full_path_name() const
{
	return _editors_shared_full_path_name;
}

QString gl_para::app_data_path_system() const
{
	return _app_data_path_system;
}
