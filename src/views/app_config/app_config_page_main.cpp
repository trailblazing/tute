#include <cassert>
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QString>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QCommonStyle>

#include "app_config_page_main.h"
#include "models/app_config/app_config.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/disk_helper.h"


extern AppConfig	appconfig;
extern GlobalParameters globalparameters;
const char		*standartItem	= "Standard";
const char		*portableItem	= "Portable";

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfigPageMain)
#endif

AppConfigPageMain::AppConfigPageMain(QWidget *parent) : ConfigPage(parent){
    setup_ui();
    setup_signals();
    assembly();
}

void AppConfigPageMain::setup_ui(void){
    qDebug() << "Create main config page";


//    QString	standartItem	= tr("Standard");
//    QString	portableItem	= tr("Portable");


	// Блок работы с выбором языка интерфейса
    _application_mode_label = new QLabel(this);
    _application_mode_label->setText(tr("Application Mode"));

    _application_mode_option = new MtComboBox(this);
    _application_mode_option->setMinimumContentsLength(2);
    _application_mode_option->addItem(standartItem);
    _application_mode_option->addItem(portableItem);
    _application_mode_option->setCurrentIndex(_application_mode_option->findText(globalparameters.application_mode(), Qt::MatchCaseSensitive));

    _rootdir_label = new QLabel(this);
    _rootdir_label->setText(tr("Root directory"));

	// Блок работы с путем до каталога данных
    _datadir_label = new QLabel(this);
    _datadir_label->setText(tr("Data directory"));

    _rootdir_input = new QLineEdit(this);
    _rootdir_input->setMinimumWidth(50);
    _rootdir_input->setText(appconfig.root_dir());

    _rootdir_button = new FlatToolButton(this);
    _rootdir_button->setText(tr("..."));

    _datadir_input = new QLineEdit(this);
    _datadir_input->setMinimumWidth(50);
    _datadir_input->setText(appconfig.data_dir());

    _datadir_button = new FlatToolButton(this);
    _datadir_button->setText(tr("..."));


	// Блок работы с путем до корзины
    _trashdir_label = new QLabel(this);
    _trashdir_label->setText(tr("Trash directory"));

    _trashdir_input = new QLineEdit(this);
    _trashdir_input->setMinimumWidth(50);
    _trashdir_input->setText(appconfig.trash_dir());

    _trashdir_button = new FlatToolButton(this);
    _trashdir_button->setText(tr("..."));


	// Блок работы с размером корзины
    _trashsize_label = new QLabel(this);
    _trashsize_label->setText(tr("Trash size"));

    _trashsize_input = new QSpinBox(this);
    _trashsize_input->setRange(1, 1000);
    _trashsize_input->setValue(appconfig.trash_size());

    _trashsize_flexion = new QLabel(this);
    _trashsize_flexion->setText(tr("Mb"));


	// Блок работы с максимальным количеством файлов в корзине
    _trashmaxfilecount_label = new QLabel(this);
    _trashmaxfilecount_label->setText(tr("Maximum files in trash"));

    _trashmaxfilecount_input = new QSpinBox(this);
    _trashmaxfilecount_input->setRange(1, 3000);
    _trashmaxfilecount_input->setValue(appconfig.trash_max_file_count());

    _trashmaxfilecount_flexion = new QLabel(this);
    _trashmaxfilecount_flexion->setText(tr("files"));


	// Блок работы с выбором языка интерфейса
    _interface_language_label = new QLabel(this);
    _interface_language_label->setText(tr("Language"));

    _interface_language = new MtComboBox(this);
    _interface_language->setMinimumContentsLength(2);
    _interface_language->addItem("en");
    _interface_language->addItem("ru");
    _interface_language->setCurrentIndex(_interface_language->findText(appconfig.interface_language(), Qt::MatchCaseSensitive));


	// Блок работы с отображением даты и времени
    _datetime_format_box = new QGroupBox(this);
    _datetime_format_box->setTitle(tr("Date and time show settings"));

    _disable_custom_datetime_format	= new QRadioButton(tr("Show date and time by OS locale settings"), this);
    _enable_custom_datetime_format	= new QRadioButton(tr("Custom date and time show format"), this);
    custom_datetime_format		= new QLineEdit(this);
    _datetime_format_help_button	= new FlatToolButton(this);
    QCommonStyle styleHelp;
    _datetime_format_help_button->setIcon(styleHelp.standardIcon(QStyle::SP_MessageBoxQuestion));
	// Точка устанавливается возле того пункта, который настроен в конфиге
    if(appconfig.enable_custom_datetime_format() == false){
	_disable_custom_datetime_format->setChecked(true);
	_enable_custom_datetime_format->setChecked(false);
	custom_datetime_format->setEnabled(false);
    }else{
	// Если разрешен пользовательский формат даты и времени
	_disable_custom_datetime_format->setChecked(false);
	_enable_custom_datetime_format->setChecked(true);
	custom_datetime_format->setEnabled(true);
	custom_datetime_format->setText(appconfig.custom_datetime_format());
    }
}

AppConfigPageMain::~AppConfigPageMain(void){
//    delete datadirInput;
//    delete trashdirInput;
//    delete trashsizeInput;
//    delete trashmaxfilecountInput;
//    delete interfaceLanguage;

//    delete dateTimeFormatBox;
//    delete disableCustomDateTimeFormat;
//    delete enableCustomDateTimeFormat;
//    delete customDateTimeFormat;
}

void AppConfigPageMain::setup_signals(void){
    connect(_rootdir_button, &FlatToolButton::clicked, this, &AppConfigPageMain::open_rootdir_select_dialog);
    connect(_datadir_button, &FlatToolButton::clicked, this, &AppConfigPageMain::open_datadir_select_dialog);
    connect(_trashdir_button, &FlatToolButton::clicked, this, &AppConfigPageMain::open_trashdir_select_dialog);

    connect(_disable_custom_datetime_format, &QRadioButton::toggled, this, &AppConfigPageMain::on_disable_custom_datetime_format_toggle);
    connect(_enable_custom_datetime_format, &QRadioButton::toggled, this, &AppConfigPageMain::on_enable_custom_datetime_format_toggle);

    connect(_datetime_format_help_button, &FlatToolButton::clicked, this, &AppConfigPageMain::on_datetime_format_help_button);
}

void AppConfigPageMain::assembly(void){
    QHBoxLayout *application_mode_layout = new QHBoxLayout();

    application_mode_layout->addWidget(_application_mode_label);
    application_mode_layout->addWidget(_application_mode_option);

    QHBoxLayout *rootdirLayout = new QHBoxLayout();
    rootdirLayout->addWidget(_rootdir_input);
    rootdirLayout->addWidget(_rootdir_button);

	// Блок работы с путем до каталога данных
    QHBoxLayout *datadirLayout = new QHBoxLayout();
    datadirLayout->addWidget(_datadir_input);
    datadirLayout->addWidget(_datadir_button);

	// Блок работы с путем до корзины
    QHBoxLayout *trashdirLayout = new QHBoxLayout();
    trashdirLayout->addWidget(_trashdir_input);
    trashdirLayout->addWidget(_trashdir_button);

	// Блок работы с размером корзины
    QHBoxLayout *trashsizeLayout = new QHBoxLayout();
    trashsizeLayout->addWidget(_trashsize_input);
    trashsizeLayout->addWidget(_trashsize_flexion);

	// Блок работы с максимальным количеством файлов в корзине
    QHBoxLayout *trashmaxfilecountLayout = new QHBoxLayout();
    trashmaxfilecountLayout->addWidget(_trashmaxfilecount_input);
    trashmaxfilecountLayout->addWidget(_trashmaxfilecount_flexion);

	// Блок работы с выбором языка интерфейса
    QHBoxLayout *interfaceLanguageLayout = new QHBoxLayout();
    interfaceLanguageLayout->addWidget(_interface_language);


	// Блок работы с отображением даты и времени
    QVBoxLayout *dateTimeFormatVBox	= new QVBoxLayout(this);
    QHBoxLayout *dateTimeFormatHBox	= new QHBoxLayout(this);
    dateTimeFormatVBox->addWidget(_disable_custom_datetime_format);
    dateTimeFormatVBox->addWidget(_enable_custom_datetime_format);

    dateTimeFormatHBox->addWidget(custom_datetime_format);
    dateTimeFormatHBox->addWidget(_datetime_format_help_button);

    dateTimeFormatVBox->addLayout(dateTimeFormatHBox);
    _datetime_format_box->setLayout(dateTimeFormatVBox);


	// Сборка всех блоков
    QGridLayout *dirLayout = new QGridLayout();

    dirLayout->addWidget(_rootdir_label, 0, 0);
    dirLayout->addLayout(rootdirLayout, 0, 1);
    dirLayout->addWidget(_datadir_label, 1, 0);
    dirLayout->addLayout(datadirLayout, 1, 1);
    dirLayout->addWidget(_trashdir_label, 2, 0);
    dirLayout->addLayout(trashdirLayout, 2, 1);


    QGridLayout *otherSettingLayout = new QGridLayout();

    otherSettingLayout->addWidget(_trashsize_label, 0, 0);
    otherSettingLayout->addLayout(trashsizeLayout, 0, 1);

    otherSettingLayout->addWidget(_trashmaxfilecount_label, 1, 0);
    otherSettingLayout->addLayout(trashmaxfilecountLayout, 1, 1);

    otherSettingLayout->addWidget(_interface_language_label, 2, 0);
    otherSettingLayout->addLayout(interfaceLanguageLayout, 2, 1);

    otherSettingLayout->setColumnStretch(1, 100);


    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->addLayout(application_mode_layout);
    centralLayout->addLayout(dirLayout);
    centralLayout->addLayout(otherSettingLayout);
    centralLayout->addWidget(_datetime_format_box);
    centralLayout->addStretch();

    setLayout(centralLayout);
}
// Действия при нажатии кнопки выбора директории данных
void AppConfigPageMain::open_rootdir_select_dialog(void){
    QFileDialog rootdirSelectDialog(this);
    rootdirSelectDialog.setFileMode(QFileDialog::Directory);
    rootdirSelectDialog.setWindowTitle(tr("Select root directory"));
    rootdirSelectDialog.setDirectory(_rootdir_input->text());

    rootdirSelectDialog.exec();
    auto root_path = rootdirSelectDialog.directory().absolutePath();
    _rootdir_input->setText(root_path);
    _datadir_input->setText(root_path + "/" + QDir(_datadir_input->text()).dirName());
    _trashdir_input->setText(root_path + "/" + QDir(_trashdir_input->text()).dirName());
}

// Действия при нажатии кнопки выбора директории данных
void AppConfigPageMain::open_datadir_select_dialog(void){
    QFileDialog datadirSelectDialog(this);
    datadirSelectDialog.setFileMode(QFileDialog::Directory);
    datadirSelectDialog.setWindowTitle(tr("Select data directory"));
    datadirSelectDialog.setDirectory(_datadir_input->text());

    datadirSelectDialog.exec();

    _datadir_input->setText(datadirSelectDialog.directory().absolutePath());
}

// Действия при нажатии кнопки выбора директории корзины
void AppConfigPageMain::open_trashdir_select_dialog(void){
    QFileDialog trashdirSelectDialog(this);
    trashdirSelectDialog.setFileMode(QFileDialog::Directory);
    trashdirSelectDialog.setWindowTitle(tr("Select trash directory"));
    trashdirSelectDialog.setDirectory(_trashdir_input->text());

    trashdirSelectDialog.exec();

    _trashdir_input->setText(trashdirSelectDialog.directory().absolutePath());
}

// Действия при нажатии на радиокнопку системных установок даты и времени
void AppConfigPageMain::on_disable_custom_datetime_format_toggle(bool checked){
    qDebug() << "In onDisableCustomDateTimeFormat(): " << checked;
    if(checked == true){
	custom_datetime_format->setEnabled(false);
	custom_datetime_format->setText("");
    }
}

// Действия при нажатии на радиокнопку пользовательских установок даты и времени
void AppConfigPageMain::on_enable_custom_datetime_format_toggle(bool checked){
    qDebug() << "In onEnableCustomDateTimeFormat(): " << checked;
    if(checked == true) custom_datetime_format->setEnabled(true);
}

void AppConfigPageMain::on_datetime_format_help_button(void){
    QString text = "Format symbols:\n\
\n\
d: day 1-31, dd: day 01-31,\n\
ddd: day short name, dddd: day long name,\n\
M: month 1-12, MM: month 01-12,\n\
MMM: month short name, MMMM: month long name,\n\
yy: year 00-99, yyyy: year 0000-9999,\n\
h: hour 0-23, hh: hour 00-23,\n\
H: hour 0-23 AM/PM, HH: hour 00-23 AM/PM,\n\
n: minute 0-59, mm: minute 00-59,\n\
s: second 0-59, ss: second 00-59,\n\
AP: use AM/PM display, ap: use am/pm display,\n\
t: timezone\n\
\n\
Sample: dd.MM.yyyy - hh:mm:ss";

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageMain::apply_changes(void){
    qDebug() << "Apply changes main";

    int	difficult_changes = 0;

    QString	root_path	= globalparameters.root_path();
    auto	write_root	= [&](){
	    auto	root_path = _rootdir_input->text();
	    QDir	dir(root_path);
		// Проверяется, допустимо ли имя директории
	    if(dir.isReadable() == false){
		QMessageBox::warning(this, tr("Warning")
				    , tr("The root directory does not exists or unavailable for reading.")
				    , QMessageBox::Ok);
	    }else if(dir.exists() == false) DiskHelper::create_directory(QDir::rootPath(), root_path);
	    else{
		// Новое имя запоминается в конфиг
		appconfig.root_dir(root_path);
		difficult_changes = 1;
	    }
	};
    if(appconfig.root_dir() != _rootdir_input->text()) write_root();
    auto write_data = [&](){
	    auto	data_path = _datadir_input->text();
	    QDir	dir(data_path);
		// Проверяется, допустимо ли имя директории
	    if(dir.isReadable() == false){
		QMessageBox::warning(this, tr("Warning")
				    , tr("The data directory does not exists or unavailable for reading.")
				    , QMessageBox::Ok);
	    }else if(dir.exists() == false){
		DiskHelper::create_directory(root_path, dir.dirName());
		DiskHelper::create_directory(root_path + dir.dirName(), "base");
	    }else{
		// Новое имя запоминается в конфиг
		appconfig.data_dir(data_path);
		difficult_changes = 1;
	    }
	};
	// Если был изменен путь к базе, он запоминается в конфигфайл
    if(appconfig.data_dir() != _datadir_input->text()) write_data();
    auto write_trash = [&](){
	    auto	trash_path = _trashdir_input->text();
	    QDir	dir(trash_path);
		// Проверяется, допустимо ли имя директории
	    if(dir.isReadable() == false){
		QMessageBox::warning(this, tr("Warning")
				    , tr("The trash directory does not exists or unavailable for reading.")
				    , QMessageBox::Ok);
	    }else if(dir.exists() == false) DiskHelper::create_directory(root_path, dir.dirName());
	    else{
		// Новое имя запоминается в конфиг
		appconfig.trash_dir(trash_path);
	    }
	};
	// Если был изменен путь к корзине, он запоминается в конфигфайл
    if(appconfig.trash_dir() != _trashdir_input->text()) write_trash();
	// Если был изменен размер корзины
    if((int) appconfig.trash_size() != (int) _trashsize_input->text().toInt()) appconfig.trash_size(_trashsize_input->text().toInt());
	// Если было изменено максимально возможное количество файлов в корзине
    if(appconfig.trash_max_file_count() != _trashmaxfilecount_input->text().toInt()) appconfig.trash_max_file_count(_trashmaxfilecount_input->text().toInt());
	// Если было изменено использование пользовательского формата даты и времени
    if(appconfig.enable_custom_datetime_format() != _enable_custom_datetime_format->isChecked()) appconfig.enable_custom_datetime_format(_enable_custom_datetime_format->isChecked());
	// Если было изменен пользовательский формат даты и времени
    if(appconfig.custom_datetime_format() != custom_datetime_format->text()) appconfig.custom_datetime_format(custom_datetime_format->text());
	// Если был изменен язык
    if(appconfig.interface_language() != _interface_language->currentText()){
	appconfig.interface_language(_interface_language->currentText());
	difficult_changes = 1;
    }
    if((globalparameters.application_mode() != _application_mode_option->currentText()) || (_rootdir_input->text() != globalparameters.root_path())){
//	if(! globalparameters.is_hapnote_ini_config_exist(root_dir + "/" + globalparameters.target_os() + "/conf.ini")){
//	    QFileInfo file(root_dir + "/" + globalparameters.target_os() + "/conf.ini");
//	    if(! (file.exists() && file.isFile())) QFile::remove(root_dir + "/" + globalparameters.target_os() + "/conf.ini");
//	    if(! QFile::copy(QString(":/resource/standardconfig/") + globalparameters.target_os() + "/conf.ini", root_dir + "/" + globalparameters.target_os() + "/conf.ini")) throw std::runtime_error("Can not copy conf.ini");
//	    else QFile::setPermissions(root_dir + "/" + globalparameters.target_os() + "/conf.ini", QFile::ReadUser | QFile::WriteUser);
////	    bool succedded = DiskHelper::save_strings_to_directory(full_current_path, globalparameters.config_ini());
////	    assert(succedded);
//	}
//	globalparameters.application_mode(_application_mode_option->currentText());

	const auto	is_portable	= (_application_mode_option->currentText() == portableItem);
	const auto	original_state	= std::tuple<const bool, const bool, const QString>(true, is_portable, _rootdir_input->text());
	const auto	result		= globalparameters.initialize_root(is_portable, _rootdir_input->text());
	if(original_state != result){
	    if(std::get<1>(result) != is_portable) _application_mode_option->setCurrentText(is_portable ? portableItem : standartItem);
	    if(std::get<2>(result) != _rootdir_input->text()){
		_rootdir_input->setText(std::get<2>(result));
		write_root();
		_datadir_input->setText(std::get<2>(result) + "/" + QDir(_datadir_input->text()).dirName());
		write_data();
		_trashdir_input->setText(std::get<2>(result) + "/" + QDir(_trashdir_input->text()).dirName());
		write_trash();
	    }
	    difficult_changes = 1;
	    if(std::get<0>(result)){
		QMessageBox message;
		message.setText("The changes of application mode will take effect after restart the application.");		// You have to restart Mytetra for the configuration changes to take effect.
		message.exec();
		exit(0);
	    }
	}else difficult_changes = 0;
    }
    return difficult_changes;
}

