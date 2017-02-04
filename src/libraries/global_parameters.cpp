// #include <memory>


#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QInputDialog>

#include "main.h"
#include "global_parameters.h"

#include "views/tree/tree_screen.h"
#include "views/record/meta_editor.h"
#include "views/record_table/record_screen.h"
#include "views/find_in_base_screen/find_screen.h"
#include "libraries/window_switcher.h"
#include "views/app_config/app_config_dialog.h"
#include "views/browser/entrance.h"
#include "views/browser/downloadmanager.h"
#include "views/main_window/hidable_tabwidget.h"
// #include "utility/config_ini.h"


extern const char *index_xml_file_name;

extern const char	*standardItem;
extern const char	*portableItem;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(GlobalParameters)
#endif



gl_para::gl_para(QObject *pobj){Q_UNUSED(pobj);}

gl_para::~gl_para(){}


// void GlobalParameters::main_program_full_file(QString file){_main_program_full_file = file;}

QString gl_para::main_program_full_file(void) const {return _main_program_full_file;}

void gl_para::init(const QString &app_name){
//    _tree_screen	= nullptr;
//    _entrance		= nullptr;
//	//    _table_screens = nullptr;
//	//    _page_screen = nullptr;
//    _find_screen	= nullptr;
//    _meta_editor	= nullptr;
//    _statusbar		= nullptr;
//    _window_switcher	= nullptr;
    _main_program_full_file = app_name;
    QFileInfo fi(app_name);
    _application_name = fi.fileName();

//    QFileInfo mainProgramFileInfo(_main_program_full_file);
//    _main_program_path = mainProgramFileInfo.absolutePath();

    _standard_paths = std::make_pair(QDir::homePath() + "/." + _application_name, QDir::homePath() + "/.config/" + _application_name);
    if(QFile(_standard_paths.first).exists()) _standard_path = _standard_paths.first;
    else if(QFile(_standard_paths.second).exists()) _standard_path = _standard_paths.second;
    if("" == _standard_path){	// (! QFile(_main_program_path + "/mode.ini").exists()){
	if(QDir::home().mkdir(_application_name)) _standard_path = _standard_paths.first;
	else if(! QDir(QDir::homePath() + "/.config/").exists() || ! QDir(QDir::homePath() + "/.config/" + _application_name).exists()){
	    if(QDir::root().mkdir(_standard_paths.second)) _standard_path = _standard_paths.second;
	}
//	if(""==_standard_path)
	else critical_error("Can\'t create normal dir for current user.");
	if("" != _standard_path){
	    auto mode_file_location = _standard_path + "/" + _mode_filename;
	    if(! QFile(mode_file_location).exists())
			if(! QFile::copy(QString(":/resource/standardconfig/") + target_os() + "/" + _mode_filename, _standard_path + "/" + _mode_filename)) throw std::runtime_error("Can not copy mode.ini");
//	    else QFile::setPermissions(_standard_path + "/" + _mode_filename, QFile::ReadUser | QFile::WriteUser);
	    if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(mode_file_location) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(mode_file_location, QFile::ReadUser | QFile::WriteUser);
	}
    }
    assert(_standard_path != "");

    QSettings	app_conf(_standard_path + "/" + _mode_filename, QSettings::IniFormat);
    auto	conf_root_dir	= app_conf.value("rootdir").toString();
    QString	temp_root	= "";
    if(conf_root_dir != "./") temp_root = conf_root_dir;
    else if(_standard_path != "") temp_root = _standard_path;
    auto result = coordinate_root(temp_root);		// app_mode() == standardItem,
    assert(std::get<0>(result));
    root_path(std::get<1>(result));
	// Инициализация рабочей директории
}

// static const int mode_initilized = 3;

// QString gl_para::app_mode() const {
////    if(! QFile(_main_program_path + "/mode.ini").exists()){
////	if(! QFile::copy(QString(":/resource/standardconfig/") + target_os() + "/" + "mode.ini", _main_program_path + "/mode.ini")) throw std::runtime_error("Can not copy mode.ini");
////	else QFile::setPermissions(_main_program_path + "/mode.ini", QFile::ReadUser | QFile::WriteUser);
////    }
//    QSettings	setting(_main_program_path + "/mode.ini", QSettings::IniFormat);
//    QString	mode = setting.value("application_mode").toString();

//    return mode;
// }

// void gl_para::app_mode(const QString &mode){
//    QSettings setting(_main_program_path + "/mode.ini", QSettings::IniFormat);

//    setting.setValue("application_mode", mode);
// }


bool gl_para::root_path(QString path_name){
    assert("" != _standard_path);
    auto mode_file_full_name = _standard_path + "/" + _mode_filename;
    if(! QFile(_standard_path + "/" + _mode_filename).exists())
		if(! QFile::copy(QString(":/resource/standardconfig/") + globalparameters.target_os() + "/" + _mode_filename, mode_file_full_name)) critical_error("Can not copy " + _mode_filename);
    if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(mode_file_full_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(mode_file_full_name, QFile::ReadUser | QFile::WriteUser);
    QSettings	mode_ini(mode_file_full_name, QSettings::IniFormat);
    QDir	directory(path_name);
    if(directory.exists() && directory.isReadable()){
	mode_ini.setValue("rootdir", path_name);
	mode_ini.sync();

//	// write
//	ConfigINI *ini = new ConfigINI(mode_file_full.toStdString().c_str());
////	ini->setIntValue("section1", "intValue", 1);
////	ini->setFloatValue("section1", "floatValue", 0.1);
//	ini->setStringValue("General", "rootdir", path_name.toStdString().c_str());
////	ini->setBoolValue("section2", "boolValue", true);
//	ini->writeConfigFile();
//	delete ini;

	return true;
    }else return false;
}


QString gl_para::root_path(void) const {
//    return get_parameter("rootdir");
    assert("" != _standard_path);
    auto	mode_file_full_name = _standard_path + "/" + _mode_filename;
    QSettings	mode_ini(mode_file_full_name, QSettings::IniFormat);
    auto	root_name	= "rootdir";
    QString	result		= "";
    if(mode_ini.contains(root_name)) result = mode_ini.value(root_name).toString();
    else{
	if(QFile(mode_file_full_name).exists()) QFile::remove(mode_file_full_name);
	if(! QFile::copy(QString(":/resource/standardconfig/") + globalparameters.target_os() + "/" + _mode_filename, mode_file_full_name)) critical_error("Can not copy " + _mode_filename);
	if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(mode_file_full_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(mode_file_full_name, QFile::ReadUser | QFile::WriteUser);
	QSettings mode_new(mode_file_full_name, QSettings::IniFormat);
	result = mode_new.value(root_name).toString();
	if(result == "") critical_error("In " + _mode_filename + " not found parameter " + root_name);
    }
    return result;
}


// Инициализация рабочей директории
// Если рабочая директория уже существует, она будет установлена как рабочая
// Если ребочая директория не будет найдена, будут создана новая рабочая директория
// с начальными файлами и она будет установлена как рабочая

// Initialization working directory
// If the working directory already exists, it will be installed as a working directory.
// If the directory is not found, it will create a new working directory with initial files and it will be set as the working directory
std::tuple<const bool, const QString> gl_para::coordinate_root(const QString &root_path_){	// bool standard_mode,
    auto result	= std::tuple<bool, QString>(false, root_path_ == "" ? root_path() : root_path_);
//    if(root_path_ != _root_path && root_path_ != "") _root_path = root_path_;
	// change_root = true;
	////    // you can't do this, because your appconfig is not initialized yet.
	////    AppConfigDialog appconfigdialog(nullptr, "pageMain");
	////    appconfigdialog.show();

	//    // Поиск файла conf.ini в той же директории, где находится бинарник
	//    // Search conf.ini file in the same directory where the binary

	//    // Нужно учесть, что программу могут запускать из другой директории
	//    // QDir::currentPath() - выдает директорию, где была выполнена команда запуска
	//    // mainProgramFile - содержит путь к бинарнику относительно директории запуска

	//    // It is necessary to take into account that the program can be run from a different directory
	//    // QDir :: currentPath () - provides a directory where you have run run
	//    // MainProgramFile - contains the path to the binaries relative to the directory run

	//    // Директория, где была выполнена команда запуска
	//    // Directory where you have performed command launch
	//    QFileInfo	main_program_file_info(_main_program_file);
	//    QString		full_current_path = main_program_file_info.absolutePath();

	//    qDebug() << "Check full current path " << full_current_path;
	//    QSettings	setting(full_current_path + "/mode.ini", QSettings::IniFormat);
	//    QString		mode = setting.value("application_mode").toString();
	//    if(! is_hapnote_ini_config_exist(full_current_path + "/conf.ini")){
	//        if(! QFile::copy(QString(":/resource/standardconfig/") + target_os() + "/conf.ini", full_current_path + "/conf.ini")) throw std::runtime_error("Can not copy conf.ini");
	//        else QFile::setPermissions(full_current_path + "/conf.ini", QFile::ReadUser | QFile::WriteUser);
	//        //	bool succedded = DiskHelper::save_strings_to_directory(full_current_path, config_ini());
	//        //	assert(succedded);
	//    }
	//    if((mode != "Standard") && is_hapnote_ini_config_exist(full_current_path + "/conf.ini")){		// mode == portableItem ||
	//        qDebug() << "Work directory set to path " << full_current_path;

	//        //	throw std::runtime_error("database load failure");

	//        // QDir dir=QDir("./");
	//        // QDir dir=QDir(QDir::currentPath());
	//        // workDirectory=dir.absolutePath();
	//        _work_directory = full_current_path;
	//	}else{
	// Проверка ini-файла
    auto is_hapnote_ini_config_exist = [&](const QString &config_filename) -> bool {
	    qDebug() << "Check config file " << config_filename;
	    bool	result = false;
	    QFileInfo	info(config_filename);
		// Если файл существует, надо определить, от Hapnote он или от другой программы
	    if(info.exists()){
		qDebug() << "Config file " << config_filename << " is exists";

		// Выясняется имя файла без пути к директории
		QString short_filename = info.fileName();
		qDebug() << "Short config file name " << short_filename;

		// Выясняется имя директории из имени файла
		QDir	dir	= info.dir();
		QString	dirname = dir.absolutePath();
		qDebug() << "Config directory name " << dirname;

		// Открывается хранилище настроек
		QSettings app_conf(config_filename, QSettings::IniFormat);
		// conf->setPath(QSettings::IniFormat, QSettings::UserScope, dirName);
		// conf->setPath(QSettings::IniFormat, QSettings::SystemScope, dirName);
		// Если есть переменная version
		if(app_conf.contains("version")){
		    int version = app_conf.value("version").toInt();
			// Если номер версии конфига до 3 включительно
		    if(version <= 3){
			// В этих версиях небыло переменной programm, поэтому проверяется
			// переменная datadir
			if(app_conf.contains("datadir")) result = true;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																					// return true;
			//                else return false;
		    }else{
			// Иначе номер версии больше 3
			if(app_conf.contains("programm")){
			    if(app_conf.value("programm").toString() == application_name()) result = true;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																						// return true;
				//                    else return false;
			}	// else return false;
		    }
		}	// else return false;	// Нет переменной version
	    }		// else return false;	// Нет указанного файла
	    return result;
	};

    auto recover_config = [&](const QString &root_path_local){
	    qDebug() << "Detect home directory " << root_path_local;
	    qDebug() << "Try find conf.ini in home subdirectory " << root_path_local;
	    if(! QFile(root_path_local).exists()) if(! QDir::root().mkpath(root_path_local)) critical_error("Can not make path \"" + root_path_local + "\"");
	    auto config_path = root_path_local + "/" + target_os();
	    if(! QFile(config_path).exists()) if(! QDir::root().mkpath(config_path)) critical_error("Can not make path \"" + config_path + "\"");// throw std::runtime_error("Can not make path \"" + config_path.toStdString() + "\"");
	    auto check_exists_and_permission = [&](const QString &name){
//		    auto	config_path	= root_path + "/" + target_os();
		    auto config_location = config_path + "/" + name;
		    if(! QFile(config_location).exists())
				if(! QFile::copy(QString(":/resource/standardconfig/") + target_os() + "/" + name, config_location)) critical_error("Can not copy " + name);																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																					// throw std::runtime_error("Can not copy " + name.toStdString());
		    if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(config_location) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(config_location, QFile::ReadUser | QFile::WriteUser);
		};
		// Если директория существует и в ней есть настоящий файл конфигурации
	    if(! is_hapnote_ini_config_exist(config_path + _conf_filename)){
		//	qDebug() << "Config init file success find in home subdirectory " << root_dir;
		//	    else
		qDebug() << "File conf.ini can't' find in home subdirectory " << root_path_local;
		//		QFileInfo file(root_dir + "/" + target_os() + "/conf.ini");
		//		if(! (file.exists() && file.isFile())) QFile::remove(root_dir + "/" + target_os() + "/conf.ini");

		check_exists_and_permission("conf.ini");
	    }else if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(config_path + _conf_filename) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(config_path + _conf_filename, QFile::ReadUser | QFile::WriteUser);
//	    if(! QFile(config_path + _browser_conf_filename).exists())
	    check_exists_and_permission(_browser_conf_filename);
//	    if(! QFile(config_path + _browserview_conf_filename).exists())
	    check_exists_and_permission(_browserview_conf_filename);
//	    if(! QFile(config_path + _editor_conf_filename).exists())
	    check_exists_and_permission(_editor_conf_filename);
//	    if(! QFile(config_path + _entrance_conf_filename).exists())
	    check_exists_and_permission(_entrance_conf_filename);
//	    if(! QFile(config_path + _stylesheet_filename).exists())
	    check_exists_and_permission(_stylesheet_filename);
	    std::get<1>(result) = root_path_local;

//	    QSettings app_conf(_main_program_path + "/" + _mode_filename, QSettings::IniFormat);
//	    app_conf.setValue("rootdir", _root_path);
	};

//    auto fix_root_dir_portable = [&] {
////	QString root_oath = "";
////	root_oath = _main_program_path;
//	recover_config(root_path_);
////	return set_current(true);
//    };
    auto figure_out_standard_work_path = [&] {
	QString standard_path_ = _standard_paths.first;

	// Если в текущей директории запуска нет conf.ini

	// Поиск файла conf.ini в домашней директории пользователя
	// в поддиректории ".имя_программы"
//	standard_path_ = QDir::homePath() + "/." + application_name();
	QDir root_dir_path(standard_path_);
	if(root_dir_path.exists()) recover_config(standard_path_);
	else{
		// Иначе директории "~/.имя_программы" нет
		// и нужно пробовать найти данные в "~/.config/имя_программы"
	    qDebug() << "File conf.ini can't' find in home directory " << standard_path_;

	    standard_path_ = _standard_paths.second;	// QDir::homePath() + "/.config/" + application_name();
	    QDir root_dir_path(standard_path_);
	    if(root_dir_path.exists()) recover_config(standard_path_);
	    else{
		if(! QDir::root().mkpath(standard_path_)) critical_error("auto figure_out_standard_work_path = [&] can not make path \"" + standard_path_ + "\"");
		else recover_config(standard_path_);
	    }
	}
//	return set_current(false);
    };
    auto	is_standard_path	= [&](const QString &new_root){return new_root == _standard_paths.first || new_root == _standard_paths.second;};
    auto	non_conflicts		= [&](	// bool mode_std,
	bool path_std){
	    assert(std::get<1>(result) != "");
	    QSettings conf(std::get<1>(result) + "/" + target_os() + "/" + _conf_filename, QSettings::IniFormat);

	    auto	datadir		= conf.value("datadir").toString();
	    auto	trashdir	= conf.value("trashdir").toString();
	    return (  path_std	// mode_std
		   && is_standard_path(QDir::currentPath()) && is_standard_path(QDir(datadir).absolutePath()) && is_standard_path(QDir(trashdir).absolutePath()))
		   || (  ! path_std
		      && ! (is_standard_path(QDir::currentPath()) || is_standard_path(QDir(datadir).absolutePath()) || is_standard_path(QDir(trashdir).absolutePath())));
	};
	//
    auto change_current = [&](const QString &root_path_){
	    auto result_current = std::tuple<bool, QString>(false, root_path_);	// , std::get<2>(result)
		// Устанавливается эта директория как рабочая
	    if(! QDir::setCurrent(root_path_)) critical_error("Can not set work directory as '" + root_path_ + "'. System problem.");
		// std::get<0>(result_current) = true;
//	    else
//	    root_path(root_path_);
	    assert(QDir::currentPath() == root_path_);
	    if(non_conflicts(is_standard_path(std::get<1>(result_current)))) std::get<0>(result_current) = true;
	    return result_current;
	};
    auto check_and_recover_current_root = [&](const QString &root_cur){	// (bool is_standard_mode)
	    auto result_current	= result;	// std::make_pair(false, _root_path);
//	    auto	root_cur	= root_path();
//	    auto	current_path			= QDir::currentPath();
	    auto current_path_is_standard = is_standard_path(root_cur);			// current_path
//	    if(standard_mode && current_path_is_standard) _root_path = current_path;
	    if(! non_conflicts(current_path_is_standard)){
		// Если рабочая директория не определена
		if(current_path_is_standard){
//		if(_root_path == "")
		    figure_out_standard_work_path();
		    if(QDir::currentPath() != root_cur) result_current = change_current(root_cur);																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																												// standardItem,
		    else std::get<0>(result_current) = true;
			// for data and trash folder	// _work_directory.length() == 0 // qDebug() << "Cant find work directory with hapnote data";
		}else{
		    recover_config(root_path_);																																																																																																																		// if(_root_path == "") fix_root_dir_portable();
		    if(QDir::currentPath() != root_cur) result_current = change_current(root_cur);																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				// portableItem,	// app_mode() != portableItem ||
		    else std::get<0>(result_current) = true;
//		}
		}
//
	    }else std::get<0>(result_current) = true;
//	    else{
//		QMessageBox message;
//		message.setText("Root directory and application mode have conflicts.");
//		message.exec();
//	    }

	    return result_current;
	};


    QString dataDirName = ".config/" + application_name();

    QString welcomeText = tr("Welcome to Hapnote v.") + QString::number(APPLICATION_RELEASE_VERSION) + '.' + QString::number(APPLICATION_RELEASE_SUBVERSION) + '.' + QString::number(APPLICATION_RELEASE_MICROVERSION) + "!";

    QString standartText = tr("Create subdirectory  \"%1\"\nin user directory  \"%2\",\nand create application files in it.").arg(dataDirName).arg(QDir::homePath());

    QString portableText = tr("Create application files\nin current directory  \"%1\".").arg(std::get<1>(result));
//	// Если возможно создать только стандартную версию файлового окружения
//    if(! enablePortable){
//	qDebug() << "Can\'t create portable version - cant write data to hapnote bin-file directory";
//	// Если рабочая директория найдена автоматически
//	if(! set_current(enablePortable)){
//	    QString infoText = tr("The following actions will be performed before running this application: \n\n") +
//		standartText + "\n\n" +
//		tr("Do you agree to perform these?");
//	    QMessageBox msgBox;
//	    msgBox.setText(welcomeText);
//	    msgBox.setInformativeText(infoText);
//	    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//	    msgBox.setDefaultButton(QMessageBox::Ok);
//	    int ret = msgBox.exec();
//	    if(ret == QMessageBox::Ok) create_standard_programfiles();
//	    else exit(0);
//	}
//	// Была нажата отмена
//	fix_workdirectory_standard();
//    }else{
	// Если рабочая директория найдена автоматически
//    auto cur_result=set_current_root(enablePortable);
//    while(! std::get<0>(result)){
    while(! std::get<0>(result = check_and_recover_current_root(std::get<1>(result)))){		// app_mode() == standardItem
	// Иначе есть возможность создать как стандартное файловое окружение,
	// так и "переносимое"

	QString infoText = welcomeText + "\n\n" +
	    tr("Please, select application path: \n\n") +
	    tr("Standart:\n") + standartText + "\n\n" +
	    tr("Portable:\n") + portableText + "\n\n";

	QStringList items;
//	QString		standardItem	= tr("Standard");
//	QString		portableItem	= tr("Portable");
	items << root_path() << _standard_paths.first << _standard_paths.second << QFileInfo(_main_program_full_file).absolutePath();	// standardItem << portableItem;

	// QInputDialog inputDialog;
	// inputDialog.setLabelText("<b>"+welcomeText+"</b>\n\n"+infoText);
	// inputDialog.setComboBoxItems(items);
	bool				ok;
	std::unique_ptr<QWidget>	tempWidget(new QWidget());
	QString				return_path = QInputDialog::getItem(tempWidget.get()
									   , welcomeText
									   , infoText
									   , items, 0, false, &ok);

//	auto create_stylesheet_file = [&](QString config_dir){
//		QString target_os_ = target_os();

//		QFile::copy(":/resource/standardconfig/" + target_os_ + "/stylesheet.css", config_dir + "/stylesheet.css");
//		QFile::setPermissions(config_dir + "/stylesheet.css", QFile::ReadUser | QFile::WriteUser);
//	    };

	// Создание первоначального набора файлов в указанной директории
	// Create the initial set of files in the specified directory
	auto initialize_root_child = [&](const QString &root_path_local){
		qDebug() << "Create first program files in directory " << root_path_local;

		QDir dir(root_path_local);

		// Создается дерево директорий в указанной директории
		dir.mkpath("data/base/0000000001");
		dir.mkdir("trash");

//		// Создаются файлы конфигурации
//		QString	target_os_	= target_os();	// "any" или "meego" или "android"
//		auto	config_path	= root_dir + "/" + target_os_;
//		QFile::copy(":/resource/standardconfig/" + target_os_ + "/conf.ini", config_path + "/conf.ini");
//		QFile::setPermissions(config_path + "/conf.ini", QFile::ReadUser | QFile::WriteUser);

//		QFile::copy(":/resource/standardconfig/" + target_os_ + "/editorconf.ini", config_path + "/editorconf.ini");
//		QFile::setPermissions(config_path + "/editorconf.ini", QFile::ReadUser | QFile::WriteUser);

//		QFile::copy(":/resource/standardconfig/" + target_os_ + "/entrance.ini", config_path + "/entrance.ini");
//		QFile::setPermissions(config_path + "/entrance.ini", QFile::ReadUser | QFile::WriteUser);

//		create_stylesheet_file(config_path);
		recover_config(root_path_local);

		// Создается файл базы данных
		QFile::copy(":/resource/standarddata/hapnote.xml", root_path_local + "/data/" + index_xml_file_name);
		QFile::setPermissions(root_path_local + "/data/" + index_xml_file_name, QFile::ReadUser | QFile::WriteUser);

		// Создается файл первой записи
		QFile::copy(":/resource/standarddata/base/0000000001/text.html", root_path_local + "/data/base/0000000001/text.html");
		QFile::setPermissions(root_path_local + "/data/base/0000000001/text.html", QFile::ReadUser | QFile::WriteUser);
	    };

	auto create_things_for_standard_root = [&](void){
		qDebug() << "Create standart program files";

		QDir	userDir		= QDir::home();
		QString	dataDirName	= ".config/" + application_name();
		if(userDir.mkpath(_standard_paths.first)){	// dataDirName
		    qDebug()	<< "Successfull create subdirectory " << _standard_paths.first	// dataDirName
				<< " in directory " << QDir::homePath()	// userDir.absolutePath()
		    ;

//		    QString standard_root_path = userDir.absolutePath() + "/" + dataDirName;		// Ранее использовался QDir::homePath()

		    initialize_root_child(_standard_paths.first);// standard_root_path
		    std::get<1>(result) = _standard_paths.first;	// standard_root_path;
//		    app_mode(standardItem);
		}else if(userDir.mkpath(_standard_paths.second)){
		    qDebug()	<< "Successfull create subdirectory " << _standard_paths.second	// dataDirName
				<< " in directory " << QDir::homePath()		// userDir.absolutePath()
		    ;

		    initialize_root_child(_standard_paths.second);	// standard_root_path
		    std::get<1>(result) = _standard_paths.second;	// standard_root_path;
		}else{
		    critical_error("Can not created directory \"" + _standard_paths.first + "\" or \"" + _standard_paths.second + "\" in user directory \"" + QDir::homePath() + "\"");
		    exit(0);
		}
	    };
	auto create_things_for_portable_root = [&](void){
		qDebug() << "Create portable program files";
		initialize_root_child(std::get<1>(result));
//		temp_root = root_path_;
	    };
	// Если пользователь сделал выбор
	if(ok && ! return_path.isEmpty()){
	    if(is_standard_path(return_path)){	// if(permanent_application_mode() == standardItem){	//
		create_things_for_standard_root();
		figure_out_standard_work_path();
		change_current(return_path);	// standardItem,
//		root_path(QDir(return_path).absolutePath());
		result = check_and_recover_current_root(QDir(return_path).absolutePath());	// STANDARD_MODE
	    }else{
		create_things_for_portable_root();
		recover_config(root_path_);			// fix_root_dir_portable();
		change_current(return_path);	// portableItem,
//		root_path(QDir(return_path).absolutePath());
		result = check_and_recover_current_root(QDir(return_path).absolutePath());	// PORTABLE_MODE
	    }
	}else exit(0);
    }
//    else std::get<0>(result) = true;
//    }
	// Была нажата отмена
//	fix_workdirectory_portable();
//    }


//	// Search restarts working directory    // Заново запускается поиск рабочей директории
//    _work_directory = "";
//    check_workdirectory();
    return result;
}

// QString gl_para::main_program_dir() const {return _main_program_path;}

void gl_para::style_source(const QString &source){_style_source = source;}

QString gl_para::style_source() const {return _style_source;}

void gl_para::download_manager(browser::DownloadManager *dm){_download_manager = dm;}

// QString GlobalParameters::root_path(void) const {return _root_path;}
QString gl_para::config_filename(void) const {return _conf_filename;}

QString gl_para::target_os(void) const {
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
QString gl_para::application_name(void) const {
	// todo: Подумать и заменить этот код на значения, полученные из PRO-файла
    QString app_name = "";
//    auto	to			= target_os();
    if(target_os() == "any") app_name = _application_name;
    if(target_os() == "meego") app_name = QString("ru.webhamster") + "." + _application_name;
    if(target_os() == "android") app_name = QString("ru.webhamster") + "." + _application_name;
	// qDebug() << "In getApplicationName() return \"" << appName << "\"";

    return app_name;
}

browser::Profile *gl_para::profile() const {return _profile;}

void gl_para::profile(browser::Profile *profile_){_profile = profile_;}

QSplitter *gl_para::find_splitter() const {return _find_splitter;}

void gl_para::find_splitter(QSplitter *_find_splitter){this->_find_splitter = _find_splitter;}

void gl_para::h_record_splitter(QSplitter *h_record_splitter){_h_record_splitter = h_record_splitter;}

QSplitter *gl_para::h_record_splitter() const {return _h_record_splitter;}

void gl_para::h_tree_splitter(QSplitter *vleftsplitter){_v_left_splitter = vleftsplitter;}

QSplitter *gl_para::h_tree_splitter() const {return _v_left_splitter;}

void gl_para::v_right_splitter(QSplitter *vrightsplitter){_v_right_splitter = vrightsplitter;}

QSplitter *gl_para::v_right_splitter() const {return _v_right_splitter;}

void gl_para::vtab_record(HidableTabWidget *point){_vtab_record = point;}

HidableTabWidget *gl_para::vtab_record() const {return _vtab_record;}

// void GlobalParameters::vtab_tree(QTabWidget *point)
// {
//    _vtab_tree = point;
// }

// QTabWidget *GlobalParameters::vtab_tree()
// {
//    return _vtab_tree;
// }

browser::DownloadManager *gl_para::request_download_manager(){
    bool found = false;
    for(int i = 0; i < _vtab_record->count(); i ++){
	auto widget_ = _vtab_record->widget(i);
	if(widget_->objectName() == download_manager_singleton_name){
	    _download_manager	= dynamic_cast<browser::DownloadManager *>(widget_);
	    found		= true;
	    break;
	}
    }
    if(! found) _download_manager = new browser::DownloadManager(download_manager_singleton_name, _vtab_record);
//    if(! _download_manager)_download_manager = new browser::DownloadManager(download_manager_singleton_name, _vtab_record);
    if(_vtab_record->currentIndex() != _vtab_record->indexOf(_download_manager)) _vtab_record->setCurrentWidget(static_cast<QWidget *>(_download_manager));
    return _download_manager;
}

browser::DownloadManager *gl_para::download_manager() const {return _download_manager;}

void gl_para::editor_config(EditorConfig *dialog){_editor_config = dialog;}

EditorConfig *gl_para::editor_config() const {return _editor_config;}

AttachTableController *gl_para::attachtable_controller() const {return _attachtable_controller;}

void gl_para::attachtable_controller(AttachTableController *_attachtable_controller){this->_attachtable_controller = _attachtable_controller;}

void gl_para::tree_screen(ts_t *point){_tree_screen = point;}

ts_t *gl_para::tree_screen() const {return _tree_screen;}

browser::Entrance *gl_para::entrance() const {return _entrance;}

void gl_para::entrance(browser::Entrance * &b){_entrance = b;}

void gl_para::push_record_screen(rs_t *point){_table_screens.push_back(point);}

std::vector<rs_t *> gl_para::record_screens() const {return _table_screens;}

// RecordScreen *GlobalParameters::page_screen() {return _page_screen; }

// void GlobalParameters::page_screen(RecordScreen *page) { _page_screen = page; }

void gl_para::find_screen(FindScreen *point){_find_screen = point;}

FindScreen *gl_para::find_screen() const {return _find_screen;}

void gl_para::meta_editor(Editentry *point){_meta_editor = point;}

Editentry *gl_para::meta_editor() const {return _meta_editor;}

void gl_para::status_bar(QStatusBar *point){_statusbar = point;}

QStatusBar *gl_para::status_bar() const {return _statusbar;}

void gl_para::window_switcher(WindowSwitcher *point){_window_switcher = point;}

WindowSwitcher *gl_para::window_switcher() const {return _window_switcher;}

wn_t *gl_para::main_window() const {return _mainwindow;}

void gl_para::main_window(wn_t *mainwindow){_mainwindow = mainwindow;}

void gl_para::crypt_key(QByteArray hash){_password_hash = hash;}

QByteArray gl_para::crypt_key(void) const {return _password_hash;}
//// deprecated
// QMap<QString, QString>  GlobalParameters::config_ini() const {
//    QMap<QString, QString>	result;
//    QString			content =
//	"[General]\r\n"
//	"addnewrecord_expand_info=1\r\n"
//	"application_mode=Portable\r\n"
//	"attachAppendDir=\r\n"
//	"attachSaveAsDir=\r\n"
//	"autoClosePasswordDelay=10\r\n"
//	"autoClosePasswordEnable=false\r\n"
//	"customDateTimeFormat=\r\n"
//	"cutbranchconfirm=true\r\n"
//	"editorCursorPosition=0\r\n"
//	"editorScrollBarPosition=0\r\n"
//	"editor_show=false\r\n"
//	"enableCustomDateTimeFormat=false\r\n"
//	"enableDecryptFileToTrashDirectory=false\r\n"
//	"findInBaseExpand=false\r\n"
//	"findScreenTreeSearchArea=0\r\n"
//	"find_splitter_sizelist=\"657,23\"\r\n"
//	"findscreen_find_in_author=true\r\n"
//	"findscreen_find_in_home=true\r\n"
//	"findscreen_find_in_name=true\r\n"
//	"findscreen_find_in_pin=true\r\n"
//	"findscreen_find_in_tags=true\r\n"
//	"findscreen_find_in_text=false\r\n"
//	"findscreen_find_in_url=true\r\n"
//	"findscreen_find_inauthor=true\r\n"
//	"findscreen_find_inname=true\r\n"
//	"findscreen_find_intags=true\r\n"
//	"findscreen_find_intext=true\r\n"
//	"findscreen_find_inurl=false\r\n"
//	"findscreen_howextract=1\r\n"
//	"findscreen_show=true\r\n"
//	"findscreen_wordregard=0\r\n"
//	"focusWidget=\r\n"
//	"h_left_splitter_sizelist=\"28,161\"\r\n"
//	"h_record_splitter_sizelist=\"245,1121\"\r\n"
//	"h_right_splitter_sizelist=\"189,1177\"\r\n"
//	"h_tree_splitter_sizelist=\"0,1366\"\r\n"
//	"hideEditorTools=\r\n"
//	"horizontal_scrollbar_style_sheet=\"QTabWidget::pane { border: 0 px; } \"\r\n"
//	"howpassrequest=atClickOnCryptBranch\r\n"
//	"interfaceMode=desktop\r\n"
//	"interfacelanguage=en\r\n"
//	"mainwingeometry=\"0,29,1366,702\"\r\n"
//	"passwordMiddleHash=\r\n"
//	"passwordSaveFlag=false\r\n"
//	"printdebugmessages=true\r\n"
//	"programm=hapnote\r\n"
//	"recordTableFieldsWidth=\"25,130,40\"\r\n"
//	"recordTableShowFields=\"pin,name,rating\"\r\n"
//	"recordTableShowHorizontalHeaders=true\r\n"
//	"recordTableShowVerticalHeaders=true\r\n"
//	"recordtableSelectedRecordId=\r\n"
//	"rememberCursorAtHistoryNavigation=true\r\n"
//	"rememberCursorAtOrdinarySelection=true\r\n"
//	"runinminimizedwindow=false\r\n"
//	"showSplashScreen=false\r\n"
//	"synchrocommand=\r\n"
//	"synchroonexit=false\r\n"
//	"synchroonstartup=false\r\n"
//	"syncroConsoleDetails=false\r\n"
//	"datadir=./data\r\n"
//	"trashdir=./trash\r\n"
//	"trashmaxfilecount=200\r\n"
//	"trashsize=5\r\n"
//	"tree_intercept=0\r\n"
//	"tree_position=\"0,1447724759fsuoi32m8z,1469866853dm12nml5fr,1469881278ob68v7s2fr,146986679219xpkvopqs,14697021199iiwq4betd,1469940289r3z03xcy0q,1469941751igm3m3tqrq\"\r\n"
//	"uglyQssReplaceHeightForTableView=0\r\n"
//	"v_left_splitter_sizelist=\"146,1220\"\r\n"
//	"v_right_sizelist=\"614,0\"\r\n"
//	"v_right_splitter_sizelist=\"657,0\"\r\n"
//	"version=27\r\n"
//	"vertical_scrollbar_style_sheet=\"QTabWidget::pane { border: 0 px; } \"\r\n"
//	"vspl_sizelist=\"614,0\"\r\n";
//    result.insert("conf.ini", content);

//    return result;
// }

// QMap<QString, QString>  GlobalParameters::hapnote_xml() const {
//    QMap<QString, QString>	result;
//    QString			content =
//	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
//	"<!DOCTYPE hapnotedoc>\r\n"
//	"<root>\r\n"
//	"   <format subversion=\"2\" version=\"1\"/>\r\n"
//	"   <content>\r\n"
////  <node name="Rename me" id="1">
//	"   <recordtable>\r\n"
//	"	<record name=\"Google\" dir=\"0000000001\" pin=\"1\" tags=\"Hapnote, Qt\" ctime=\"20081210124656\" url=\"https://www.google.com/?gws_rd=ssl\" file=\"text.html\" id=\"1\" author=\"\"/>\r\n"
//	"   </recordtable>\r\n"
////  </node>
//	"   </content>\r\n"
//	"</root>\r\n";
//    result.insert("hapnote.xml", content);

//    return result;
// }

// QMap<QString, QString>  GlobalParameters::editorconf() const {
//    QMap<QString, QString>	result;
//    QString			content =
//	"[General]\r\n"
//	"    code_font=\"Courier New,12,-1,5,50,0,0,0,0,0\"\r\n"
//	"    code_font_color=#6a1009\r\n"
//	"    code_font_size=10\r\n"
//	"    code_font_size_apply=1\r\n"
//	"    code_indent_size=20\r\n"
//	"    code_indent_size_apply=1\r\n"
//	"    default_font=\"DejaVu Sans,12,-1,5,50,0,0,0,0,0\"\r\n"
//	"    default_font_size=10\r\n"
//	"    expand_tools_lines=1\r\n"
//	"    finddialog_geometry=\"286,100,235,154\"\r\n"
//	"    indent_step=10\r\n"
//	"    monospace_font=\"Courier New,12,-1,5,50,0,0,0,0,0\"\r\n"
//	"    monospace_font_size=11\r\n"
//	"    monospace_font_size_apply=1\r\n"
//	"    tools_line_1=\"freeze_browser_view,clear,bold,italic,underline,monospace,code,separator,alignleft,aligncenter,alignright,alignwidth,numericlist,dotlist,indentplus,indentminus,separator,showformatting,findtext,showhtml,insert_image_from_file,settings,expand_edit_area,expand_tools_lines,attach\"\r\n"
//	"    tools_line_2=\"fontselect,fontsize,createtable,table_add_row,table_remove_row,table_add_col,table_remove_col,table_merge_cells,table_split_cell,show_text\"\r\n"
//	"    version=8\r\n";
//    result.insert("editorconf.ini", content);

//    return result;
// }

