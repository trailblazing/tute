#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <QtGlobal>
#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QtGlobal>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <memory>

#include <QAbstractItemView>
#include <QDir>
#include <QDomNode>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTextStream>
#include <QThread>
#include <QToolBar>
#include <QVBoxLayout>
#include <QtDebug>
// #include "utility/delegate.h"
// #include "models/record_table/Record.h"
// #include "libraries/GlobalParameters.h"
// #include "views/record_table/TableScreen.h"
#include "libraries/disk_helper.h"

using namespace std;

// Tute - a program to accumulate meaningful information
// 2015 -
// Contact: hughvonyoung@gmail.com,
// version 0.0.1
// This source code is licensed under:
// - GPL v.3
// - Modified BSD

// Software Version
#define APPLICATION_RELEASE_VERSION 0
#define APPLICATION_RELEASE_SUBVERSION 0
#define APPLICATION_RELEASE_MICROVERSION 1

// Поддерживаемая версия формата базы (хранилища)
// Format supported version of the database (repository)
#define CURRENT_FORMAT_VERSION 1
#define CURRENT_FORMAT_SUBVERSION 2

// #define ADD_NEW_RECORD_TO_END 0
// #define ADD_NEW_RECORD_BEFORE 1
// #define ADD_NEW_RECORD_AFTER 2

class FixedParameters;
class gl_para;
class AppConfig;
class DataBaseConfig;
class TrashMonitoring;
class WalkHistory;
struct id_value;
struct url_value;
// Фиксированные параметры программы (жестко заданные в текущей версии )
extern FixedParameters fixedparameters;

// Глобальные параметры программы (вычислимые на этапе инициализации, иногда
// меняющиеся в процессе выполнения программы)
extern std::shared_ptr<gl_para> gl_paras;

// Конфигурация программы (считанная из файла конфигурации)
extern std::shared_ptr<AppConfig> appconfig;

// Конфигурация данных
extern std::shared_ptr<DataBaseConfig> databaseconfig;

// Объект слежения за состоянием корзины
extern TrashMonitoring trashmonitoring;

// Объект с историей посещаемых записей
extern WalkHistory walkhistory;

// Указатель на основное окно программы
extern QObject* mainwindow;

// extern const int add_new_record_after;

// Прототипы функций, которые могут использоваться в других файлах
// Function prototypes, which can be used in other files
void log_print(char* lpszText, ...);
void critical_error(QString message);
QString xml_node_to_qstring(QDomNode xmlData);
extern const char* app_version;

// bool    compare_qstringlist_length(const QStringList &list1, const
// QStringList &list2);

// int     imax(int x1, int x2);
// int     imin(int x1, int x2);
extern std::string get_time();
extern QString get_qtime();
void my_message_output(QtMsgType type, const char* msg);
extern QString purify(QString text);
extern QStringList text_delimiter_decompose(QString text);
extern id_value get_unical_id(void);
int get_milli_count(void);
void init_random(void);

int screen_size_x(void);
int screen_size_y(void);
qreal calculate_iconsize_px(void);
extern void set_debug_message_handler();
void show_message_box(QString message); // Выдача на экран простого окна с сообщением
//template <typename widget_type>
//extern QString set_css_style(widget_type* widget, const QString& root_path_, const QString& target_os, const QString& file_name);
extern QString replace_css_meta_iconsize(QString styleText);
template <typename widget_type>
QString set_css_style(widget_type* widget, const QString& root_path_, const QString& target_os, const QString& file_name)
{
	QString style;

	//	QString root_path_ = gl_paras->root_path();
	//	auto target_os = gl_paras->target_os();
	auto location = root_path_ + "/" + target_os;
	if (!QDir(location).exists())
		if (!QDir::root().mkpath(location))
			critical_error("QString set_css_style() can not make path \"" + location + "\"");
	QString file_name_to = location + "/" + file_name; //gl_para::_stylesheet_filename
	// QString	file_name_from	= work_directory + "/resource/standardconfig/" +
	// target_os + "/stylesheet.css";
	// qint64	size_from = 0;
	// QFile	css_from(file_name_from);
	// bool result = css_from.open(QIODevice::ReadOnly | QIODevice::Text);
	//// Если файла не существует
	// if(! result){
	// qDebug() << "Stylesheet not found in " << file_name_from;
	////
	///globalparameters.create_stylesheet_file(globalparameters.work_directory());
	// }else size_from = css_from.size();
	// css_from.close();	// ?
	// qint64	size_to = 0;
	QFile css_to(file_name_to);
	// bool openResult = css_to.open(QIODevice::ReadOnly | QIODevice::Text);
	//// Если файла не существует
	// if(! openResult){
	// qDebug() << "Stylesheet not found in " << file_name_from << ". Create new
	// css file.";
	// globalparameters.create_stylesheet_file(globalparameters.work_directory());
	// }else size_to = css_to.size();
	// css_to.close();	// ?
	// if(size_to < size_from){
	if (!css_to.exists())
		// if(! QFile::remove(file_name_to)) critical_error("Can not remove file\n"
		// + file_name_to);
        if (!DiskHelper::file_cover(QResource(QString(":/resource/standardconfig/" + target_os + "/" + file_name)), file_name_to)) { // file_name_from, file_name_to
			// trashmonitoring.add_file(file_name_to_short); // Оповещение что в
			// корзину добавлен файл
			// }else {
			critical_error("Can not copy css file to file\t" + file_name_to);
		}
	// }
	// Заново открывается файл
	if (css_to.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Stylesheet success loaded from" << file_name_to;

		// QString
		style = QTextStream(&css_to).readAll();

		style = replace_css_meta_iconsize(style);

		//		sapp_t::instance()
		widget->setStyleSheet(style);
		css_to.close();
	}
	return style;
}

void set_kinetic_scrollarea(QAbstractItemView* object);

class WalkHistory;
// class gl_para;

extern WalkHistory walkhistory;
// extern std::shared_ptr<gl_para> gl_paras;
// extern const char	*application_name;
extern const char* meta_editor_singleton_name;
extern const char* record_screen_multi_instance_name;
extern const char* tree_screen_singleton_name;
extern const char* find_screen_singleton_name;
extern const char* download_manager_singleton_name;
extern const char* windowswitcher_singleton_name;
extern const char* browser_docker_singleton_name;
extern const char* editor_docker_singleton_name;
extern const char* record_controller_multi_instance_name;
extern const char* knowtreeview_singleton_name;
extern const char* global_root_id;
// extern const char *global_root_parent_id;
// extern const char *index_xml_file_name;
// template <class X> inline X *find_object(QString n);
void print_object_tree(void);
// Поиск объекта от корня по имени
template <class X>
inline X* find_object(QString objectName)
{
	QObject* findObj;

	extern QObject* mainwindow;
	// Если запрошен сам корень
	if (objectName == "mainwindow") {
		QObject* mvp = qobject_cast<X*>(mainwindow);
		if (mvp->metaObject()->className() !=
		    mainwindow->metaObject()->className()) {
			// Если запрошенный класс объекта не является классом главного окна
			printf(
			    "find_object(): Can't find mainwindow object. Check <type> in "
			    "function call\n");
			exit(1);

			return nullptr;
		} else
			return qobject_cast<X*>(mainwindow);
	}
	// Запрошен обычный объект, надо его найти
	// findObj=qFindChild<X *>(mainwindow, objectName);
	findObj = mainwindow->findChild<X*>(objectName);
	if (findObj == nullptr) {
		// Если объекта с указанным именем не найдено
		// print_object_tree();
		printf("find_object(): Can't find object with name %s\n", qPrintable(objectName));

		print_object_tree();

		// QList<QWidget *> widgets = mainwindow->findChildren<QWidget *>();
		// foreach(QWidget * b, widgets) printf("Obj: %s\n",
		// qPrintable(b->objectName()));

		exit(1);

		return nullptr;
	} else {
		// ОБъект был найден, и нужно преобразовать указатель на него
		// к указателю c заданным в шаблоне типом
		X* obj = qobject_cast<X*>(findObj);
		if (obj == 0) {
			// Если найденный объект не может быть преобразован к заданному в шаблоне
			// типу
			printf(
			    "find_object(): Object %s find, but can't convert type. Check "
			    "<type> in function call\n",
			    qPrintable(objectName));
			exit(1);

			return nullptr;
		} else
			return obj; // Объект найден нормально
	}
}

class Sleeper : public QThread {
    public:
	static void usleep(unsigned long usecs)
	{
		QThread::usleep(usecs);
	}

	static void msleep(unsigned long msecs)
	{
		QThread::msleep(msecs);
	}

	static void sleep(unsigned long secs)
	{
		QThread::sleep(secs);
	}
};

// class Record;

extern std::string url_difference(const std::string& url_compare_stored, const std::string& url_compare_get);
extern bool url_equal(const std::string& url_compare_stored, const std::string& url_compare_get);
extern bool url_equal(const QString& u0, const QString& u1);
extern bool url_equal(const url_value& u0, const url_value& u1);

// extern Record *register_record(const QUrl &_url
////                               ,
///std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void> >,
///web::WebView *, Record *const> > generator
// , RecordTableController *record_controller =
// globalparameters.getRecordTableScreen()->getRecordTableController());

// extern boost::intrusive_ptr<Record>
// register_record(boost::intrusive_ptr<Record> record
////                               ,
///std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
///web::WebView *, Record *const>> generator
// , TableController *record_controller =
// globalparameters.table_screen()->table_controller());
// extern boost::intrusive_ptr<r_t> check_record(const QUrl &_url);

// namespace web {
// class Browser;
// class WebView;
// }

// template<typename> struct active_record;

// extern boost::intrusive_ptr<Record> request_record(const QUrl &_url
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<Record>>> generator
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<Record>>> activator);
// extern boost::intrusive_ptr<Record> request_record(const
// boost::intrusive_ptr<Record> record
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<Record>>> generator
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<Record>>> activator);

// boost::intrusive_ptr<Record> equip_registered(boost::intrusive_ptr<Record>
// record, web::WebPage *page);

extern std::ifstream::pos_type filesize_non_qt_resource(const char* filename);
//extern std::streampos filesize_non_qt_resource(const char* file_path);
extern std::string truncate(std::string str, size_t width, bool show_ellipsis = true);
extern auto to_be_url(const url_value& url_) -> QUrl;
extern auto to_be_url(const QUrl& url_) -> QUrl;
#ifndef USE_QTM
#define USE_QTM
#endif

#endif // MAIN_HEADER
