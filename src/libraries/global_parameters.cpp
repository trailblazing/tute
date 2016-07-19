// #include <memory>

#include <wobjectimpl.h>
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
#include "views/browser/entrance.h"
#include "views/browser/downloadmanager.h"
#include "views/main_window/hidable_tabwidget.h"



extern const char *index_xml_file_name;

W_OBJECT_IMPL(GlobalParameters)

GlobalParameters::GlobalParameters(QObject *pobj){Q_UNUSED(pobj);}

GlobalParameters::~GlobalParameters(){}


void GlobalParameters::main_program_file(QString file){_main_program_file = file;}

QString GlobalParameters::main_program_file(void){return _main_program_file;}

void GlobalParameters::init(void){
//    _tree_screen	= nullptr;
//    _entrance		= nullptr;
//	//    _table_screens = nullptr;
//	//    _page_screen = nullptr;
//    _find_screen	= nullptr;
//    _meta_editor	= nullptr;
//    _statusbar		= nullptr;
//    _window_switcher	= nullptr;

    init_workdirectory();	// Инициализация рабочей директории
}

// Инициализация рабочей директории
// Если рабочая директория уже существует, она будет установлена как рабочая
// Если ребочая директория не будет найдена, будут создана новая рабочая директория
// с начальными файлами и она будет установлена как рабочая

// Initialization working directory
// If the working directory already exists, it will be installed as a working directory.
// If the directory is not found, it will create a new working directory with initial files and it will be set as the working directory
void GlobalParameters::init_workdirectory(void){
	// Если рабочая директория найдена автоматически
    if(find_workdirectory())return;
	// Рабочая директория не найдена, и нужно создать начальные данные

	// Проверяется, можно ли создавать файлы в директории бинарника
	// То есть, возможно ли создание "портируемой" версии

    bool enablePortable = false;

	// Путь к директории, где лежит бинарник
    QFileInfo	mainProgramFileInfo(_main_program_file);
    QString	fullCurrentPath = mainProgramFileInfo.absolutePath();

	// Проверяется, можно ли читать и писать файлы в этой директории
    QDir currentBinaryDir(fullCurrentPath);
    if(currentBinaryDir.isReadable()){
	QFile file(currentBinaryDir.filePath("seed.txt"));
	if(file.open(QIODevice::ReadWrite)){
	    enablePortable = true;	// Портируемую версию можно создавать
	    file.close();
	    file.remove();
	}
    }
    QString dataDirName = ".config/" + application_name();

    QString welcomeText = tr("Welcome to MyTetra v.") + QString::number(APPLICATION_RELEASE_VERSION) + '.' + QString::number(APPLICATION_RELEASE_SUBVERSION) + '.' + QString::number(APPLICATION_RELEASE_MICROVERSION) + "!";

    QString standartText = tr("Create subdirectory  \"%1\"\nin user directory  \"%2\",\nand create application files in it.").arg(dataDirName).arg(QDir::homePath());

    QString portableText = tr("Create application files\nin current directory  \"%1\".").arg(fullCurrentPath);
	// Если возможно создать только стандартную версию файлового окружения
    if(enablePortable == false){
	qDebug() << "Cant create portable version - cant write data to mytetra bin-file directory";

	QString infoText = tr("The following actions will be performed before running this application: \n\n") +
	    standartText + "\n\n" +
	    tr("Do you agree to perform these?");

	QMessageBox msgBox;
	msgBox.setText(welcomeText);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Ok)create_standard_programfiles();
	else exit(0);	// Была нажата отмена
    }else{
	// Иначе есть возможность создать как стандартное файловое окружение,
	// так и "переносимое"

	QString infoText = welcomeText + "\n\n" +
	    tr("Please, select application mode: \n\n") +
	    tr("Standart:\n") + standartText + "\n\n" +
	    tr("Portable:\n") + portableText + "\n\n";

	QStringList	items;
	QString		standartItem	= tr("Standard");
	QString		portableItem	= tr("Portable");
	items << standartItem << portableItem;

	bool ok;

	// QInputDialog inputDialog;
	// inputDialog.setLabelText("<b>"+welcomeText+"</b>\n\n"+infoText);
	// inputDialog.setComboBoxItems(items);

	QWidget *tempWidget	= new QWidget();
	QString item		= QInputDialog::getItem(tempWidget
						       , welcomeText
						       , infoText
						       , items, 0, false, &ok);
	delete tempWidget;
	// Если пользователь сделал выбор
	if(ok && ! item.isEmpty()){
	    if(item == standartItem)create_standard_programfiles();
	    else create_portable_programfiles();
	}else exit(0);	// Была нажата отмена
    }
	// Search restarts working directory    // Заново запускается поиск рабочей директории
    _work_directory = "";
    find_workdirectory();
}

void GlobalParameters::create_standard_programfiles(void){
    qDebug() << "Create standart program files";

    QDir	userDir		= QDir::home();
    QString	dataDirName	= ".config/" + application_name();
    if(userDir.mkpath(dataDirName)){
	qDebug() << "Successfull create subdirectory " << dataDirName << " in directory " << userDir.absolutePath();

	QString createFilePath = userDir.absolutePath() + "/" + dataDirName;	// Ранее использовался QDir::homePath()

	create_first_programfiles(createFilePath);
    }else{
	critical_error("Can not created directory \"" + dataDirName + "\" in user directory \"" + QDir::homePath() + "\"");
	exit(0);
    }
}

void GlobalParameters::create_portable_programfiles(void){
    qDebug() << "Create portable program files";

	// Путь к директории, где лежит бинарник
    QFileInfo	mainProgramFileInfo(_main_program_file);
    QString	createFilePath = mainProgramFileInfo.absolutePath();

    create_first_programfiles(createFilePath);
}

// Создание первоначального набора файлов в указанной директории
// Create the initial set of files in the specified directory
void GlobalParameters::create_first_programfiles(QString dirName){
    qDebug() << "Create first program files in directory " << dirName;

    QDir dir(dirName);

	// Создается дерево директорий в указанной директории
    dir.mkpath("data/base/0000000001");
    dir.mkdir("trash");

	// Создаются файлы конфигурации
    QString targetOs = target_os();	// "any" или "meego" или "android"

    QFile::copy(":/resource/standartconfig/" + targetOs + "/conf.ini", dirName + "/conf.ini");
    QFile::setPermissions(dirName + "/conf.ini", QFile::ReadUser | QFile::WriteUser);

    QFile::copy(":/resource/standartconfig/" + targetOs + "/editorconf.ini", dirName + "/editorconf.ini");
    QFile::setPermissions(dirName + "/editorconf.ini", QFile::ReadUser | QFile::WriteUser);

    QFile::copy(":/resource/standartconfig/" + targetOs + "/entrance.ini", dirName + "/entrance.ini");
    QFile::setPermissions(dirName + "/entrance.ini", QFile::ReadUser | QFile::WriteUser);

    create_stylesheet_file(dirName);

	// Создается файл базы данных
    QFile::copy(":/resource/standartdata/mytetra.xml", dirName + "/data/" + index_xml_file_name);
    QFile::setPermissions(dirName + "/data/" + index_xml_file_name, QFile::ReadUser | QFile::WriteUser);

	// Создается файл первой записи
    QFile::copy(":/resource/standartdata/base/0000000001/text.html", dirName + "/data/base/0000000001/text.html");
    QFile::setPermissions(dirName + "/data/base/0000000001/text.html", QFile::ReadUser | QFile::WriteUser);
}

void GlobalParameters::create_stylesheet_file(QString dirname){
    QString targetOs = target_os();
    QFile::copy(":/resource/standartconfig/" + targetOs + "/stylesheet.css", dirname + "/stylesheet.css");
    QFile::setPermissions(dirname + "/stylesheet.css", QFile::ReadUser | QFile::WriteUser);
}

void GlobalParameters::style_source(const QString &source){_style_source = source;}

QString GlobalParameters::style_source() const {return _style_source;}

void GlobalParameters::download_manager(browser::DownloadManager *dm){_download_manager = dm;}

QString GlobalParameters::application_mode() const {
    QSettings	setting(_work_directory + "/mode.ini", QSettings::IniFormat);
    QString	mode = setting.value("application_mode").toString();

    return mode;
}

void GlobalParameters::application_mode(const QString &mode){
    QSettings setting(_work_directory + "/mode.ini", QSettings::IniFormat);

    setting.setValue("application_mode", mode);
}

// Автоопределение рабочей директории
// Auto working directory
bool GlobalParameters::find_workdirectory(void){
	// Поиск файла conf.ini в той же директории, где находится бинарник
	// Search conf.ini file in the same directory where the binary

	// Нужно учесть, что программу могут запускать из другой директории
	// QDir::currentPath() - выдает директорию, где была выполнена команда запуска
	// mainProgramFile - содержит путь к бинарнику относительно директории запуска

	// It is necessary to take into account that the program can be run from a different directory
	// QDir :: currentPath () - provides a directory where you have run run
	// MainProgramFile - contains the path to the binaries relative to the directory run

	// Директория, где была выполнена команда запуска
	// Directory where you have performed command launch
    QFileInfo	main_program_file_info(_main_program_file);
    QString	full_current_path = main_program_file_info.absolutePath();

    qDebug() << "Check full current path " << full_current_path;
    QSettings	setting(full_current_path + "/mode.ini", QSettings::IniFormat);
    QString	mode = setting.value("application_mode").toString();
    if((mode != "Standard") && is_mytetra_ini_config(full_current_path + "/conf.ini")){		// mode == "Portable"||
	qDebug() << "Work directory set to path " << full_current_path;

	// QDir dir=QDir("./");
	// QDir dir=QDir(QDir::currentPath());
	// workDirectory=dir.absolutePath();
	_work_directory = full_current_path;
    }else{
	// Если в текущей директории запуска нет conf.ini

	// Поиск файла conf.ini в домашней директории пользователя
	// в поддиректории ".имя_программы"
	QString dir = QDir::homePath() + "/." + application_name();

	qDebug() << "Detect home directory " << dir;
	// Если директория существует и в ней есть настоящий файл конфигурации
	if(is_mytetra_ini_config(dir + "/conf.ini")){
	    qDebug() << "Config init file success find in home directory " << dir;
	    _work_directory = dir;
	}else{
		// Иначе директории "~/.имя_программы" нет
		// и нужно пробовать найти данные в "~/.config/имя_программы"
	    qDebug() << "File conf.ini can't' find in home directory " << dir;

	    dir = QDir::homePath() + "/.config/" + application_name();

	    qDebug() << "Try find conf.ini in home subdirectory " << dir;
		// Если директория существует и в ней есть настоящий файл конфигурации
	    if(is_mytetra_ini_config(dir + "/conf.ini") == true){
		qDebug() << "Config init file success find in home subdirectory " << dir;
		_work_directory = dir;
	    }else qDebug() << "File conf.ini can't' find in home subdirectory " << dir;
	}
    }
	// Если рабочая директория не определена
    if(_work_directory.length() == 0){
	qDebug() << "Cant find work directory with mytetra data";

	return false;
    }else{
	// Иначе рабочая директория установлена
	qDebug() << "Set work directory to " << _work_directory;
	// Устанавливается эта директория как рабочая
	if(QDir::setCurrent(_work_directory))return true;
	else{
	    critical_error("Can not set work directory as '" + _work_directory + "'. System problem.");

	    return false;
	}
    }
}

// Проверка ini-файла
bool GlobalParameters::is_mytetra_ini_config(QString filename){
    qDebug() << "Check config file " << filename;

    QFileInfo info(filename);
	// Если файл существует, надо определить, от MyTetra он или от другой программы
    if(info.exists()){
	qDebug() << "Config file " << filename << " is exists";

	// Выясняется имя файла без пути к директории
	QString short_filename = info.fileName();
	qDebug() << "Short config file name " << short_filename;

	// Выясняется имя директории из имени файла
	QDir	dir	= info.dir();
	QString dirname = dir.absolutePath();
	qDebug() << "Config directory name " << dirname;

	// Открывается хранилище настроек
	QSettings app_conf(filename, QSettings::IniFormat);
	// conf->setPath(QSettings::IniFormat, QSettings::UserScope, dirName);
	// conf->setPath(QSettings::IniFormat, QSettings::SystemScope, dirName);
	// Если есть переменная version
	if(app_conf.contains("version")){
	    int version = app_conf.value("version").toInt();
		// Если номер версии конфига до 3 включительно
	    if(version <= 3){
		// В этих версиях небыло переменной programm, поэтому проверяется
		// переменная tetradir
		if(app_conf.contains("tetradir"))return true;
		else return false;
	    }else{
		// Иначе номер версии больше 3
		if(app_conf.contains("programm")){
		    if(app_conf.value("programm").toString() == "mytetra")return true;
		    else return false;
		}else return false;
	    }
	}else return false;	// Нет переменной version
    }else return false;	// Нет указанного файла
}

QString GlobalParameters::work_directory(void){return _work_directory;}

QString GlobalParameters::target_os(void){
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
QString GlobalParameters::application_name(void){
	// todo: Подумать и заменить этот код на значения, полученные из PRO-файла
    QString appName = "";
    if(target_os() == "any")appName = "mytetra";
    if(target_os() == "meego")appName = "ru.webhamster.mytetra";
    if(target_os() == "android")appName = "ru.webhamster.mytetra";
	// qDebug() << "In getApplicationName() return \"" << appName << "\"";

    return appName;
}

browser::Profile *GlobalParameters::profile(){return _profile;}

void GlobalParameters::profile(browser::Profile *profile_){_profile = profile_;}

QSplitter *GlobalParameters::find_splitter(){return _find_splitter;}

void GlobalParameters::find_splitter(QSplitter *_find_splitter){this->_find_splitter = _find_splitter;}

void GlobalParameters::h_record_splitter(QSplitter *h_record_splitter){_h_record_splitter = h_record_splitter;}

QSplitter *GlobalParameters::h_record_splitter(){return _h_record_splitter;}

void GlobalParameters::h_tree_splitter(QSplitter *vleftsplitter){_v_left_splitter = vleftsplitter;}

QSplitter *GlobalParameters::h_tree_splitter(){return _v_left_splitter;}

void GlobalParameters::v_right_splitter(QSplitter *vrightsplitter){_v_right_splitter = vrightsplitter;}

QSplitter *GlobalParameters::v_right_splitter(){return _v_right_splitter;}

void GlobalParameters::vtab_record(HidableTabWidget *point){_vtab_record = point;}

HidableTabWidget *GlobalParameters::vtab_record(){return _vtab_record;}

// void GlobalParameters::vtab_tree(QTabWidget *point)
// {
//    _vtab_tree = point;
// }

// QTabWidget *GlobalParameters::vtab_tree()
// {
//    return _vtab_tree;
// }

browser::DownloadManager *GlobalParameters::download_manager(){
    if(! _download_manager)_download_manager = new browser::DownloadManager(download_manager_singleton_name, _vtab_record);
    if(_vtab_record->indexOf(_download_manager) == - 1){
	_vtab_record->addTab(static_cast<QWidget *>(_download_manager), QIcon(":/resource/pic/apple.svg"), "Download");
//	connect(_vtab_record, &::HidableTabWidget::tabCloseRequested, [&](int index){_vtab_record->removeTab(index);});
    }
    if(_vtab_record->currentIndex() != _vtab_record->indexOf(_download_manager))_vtab_record->setCurrentWidget(static_cast<QWidget *>(_download_manager));
    return _download_manager;
}

void GlobalParameters::editor_config(EditorConfig *dialog){_editor_config = dialog;}

EditorConfig *GlobalParameters::editor_config(){return _editor_config;}

AttachTableController *GlobalParameters::attachtable_controller(){return _attachtable_controller;}

void GlobalParameters::attachtable_controller(AttachTableController *_attachtable_controller){this->_attachtable_controller = _attachtable_controller;}

void GlobalParameters::tree_screen(ts_t *point){_tree_screen = point;}

ts_t *GlobalParameters::tree_screen(){return _tree_screen;}

browser::Entrance *GlobalParameters::entrance(){return _entrance;}

void GlobalParameters::entrance(browser::Entrance * &b){_entrance = b;}

void GlobalParameters::push_record_screen(rs_t *point){_table_screens.push_back(point);}

std::vector<rs_t *> GlobalParameters::record_screens(){return _table_screens;}

// RecordScreen *GlobalParameters::page_screen()
// {
//    return _page_screen;
// }


// void GlobalParameters::page_screen(RecordScreen *page)
// {
//    _page_screen = page;
// }



void GlobalParameters::find_screen(FindScreen *point){_find_screen = point;}

FindScreen *GlobalParameters::find_screen(){return _find_screen;}

void GlobalParameters::meta_editor(MetaEditor *point){_meta_editor = point;}

MetaEditor *GlobalParameters::meta_editor(){return _meta_editor;}

void GlobalParameters::status_bar(QStatusBar *point){_statusbar = point;}

QStatusBar *GlobalParameters::status_bar(){return _statusbar;}

void GlobalParameters::window_switcher(WindowSwitcher *point){_window_switcher = point;}

WindowSwitcher *GlobalParameters::window_switcher(){return _window_switcher;}

wn_t *GlobalParameters::main_window(){return _mainwindow;}

void GlobalParameters::main_window(wn_t *mainwindow){_mainwindow = mainwindow;}

void GlobalParameters::crypt_key(QByteArray hash){_password_hash = hash;}

QByteArray GlobalParameters::crypt_key(void){return _password_hash;}

