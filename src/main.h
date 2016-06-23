#ifndef __MAIN_H__
#define __MAIN_H__

#include <cstdio>
#include <cassert>
#include <QApplication>
#include <QClipboard>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QtGlobal>

#include <QDomNode>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QToolBar>
#include <QAbstractItemView>
#include <QThread>
// #include "utility/delegate.h"
// #include "models/record_table/Record.h"
// #include "libraries/GlobalParameters.h"
// #include "views/record_table/TableScreen.h"

using namespace std;

// ----------------------------------------------------------
// MyTetra - программа для накопления осмысленной информации
// Волгодонск, 2010 - 2015 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензиями:
// - GPL v.3
// - Modified BSD
// © Степанов С. М. и товарищи 2010 - 2015
// ----------------------------------------------------------

// MyTetra - a program to accumulate meaningful information
// Volgodonsk, 2010 - 2015 g.
// Contact: xintrea@gmail.com, www.webhamster.ru
// hughvonyoung@gmail.com (version 1.32.164)
// This source code is licensed under:
// - GPL v.3
// - Modified BSD
// © Stepanov SM and fellow 2010 - 2015

// Версия программы
// Software Version
#define APPLICATION_RELEASE_VERSION 1
#define APPLICATION_RELEASE_SUBVERSION 32
#define APPLICATION_RELEASE_MICROVERSION 164

// Поддерживаемая версия формата базы (хранилища)
// Format supported version of the database (repository)
#define CURRENT_FORMAT_VERSION 1
#define CURRENT_FORMAT_SUBVERSION 2

// #define ADD_NEW_RECORD_TO_END 0
// #define ADD_NEW_RECORD_BEFORE 1
// #define ADD_NEW_RECORD_AFTER 2

// extern const int add_new_record_after;

// Прототипы функций, которые могут использоваться в других файлах
// Function prototypes, which can be used in other files
void	log_print(char *lpszText, ...);
void	critical_error(QString message);
QString xml_node_to_qstring(QDomNode xmlData);

void    print_object_tree(void);
// bool    compare_qstringlist_length(const QStringList &list1, const QStringList &list2);

template<typename ToolButton>
void insert_action_as_button(QToolBar *tools_line, QAction *action){	// void    insert_action_as_button(QToolBar *tools_line, QAction *action);
    tools_line->addAction(action);
    qobject_cast<ToolButton *>(tools_line->widgetForAction(action))->setAutoRaise(true);	// false
}
// int     imax(int x1, int x2);
// int     imin(int x1, int x2);
void		my_message_output(QtMsgType type, const char *msg);
QStringList	text_delimiter_decompose(QString text);
extern QString	get_unical_id(void);
int		get_milli_count(void);
void		init_random(void);

void    set_kinetic_scrollarea(QAbstractItemView *object);

int	screen_size_x(void);
int	screen_size_y(void);
qreal	calculate_iconsize_px(void);

void		show_message_box(QString message);	// Выдача на экран простого окна с сообщением
extern QString	set_css_style();

class WalkHistory;
class GlobalParameters;

extern WalkHistory	walkhistory;
extern GlobalParameters globalparameters;
extern const char	*application_name;
extern const char	*meta_editor_singleton_name;
extern const char	*record_screen_multi_instance_name;
extern const char	*tree_screen_singleton_name;
extern const char	*find_screen_singleton_name;
extern const char	*download_manager_singleton_name;
extern const char	*windowswitcher_singleton_name;
extern const char	*entrance_singleton_name;
extern const char	*record_controller_multi_instance_name;
extern const char	*knowtreeview_singleton_name;
extern const char	*global_root_id;
// extern const char *global_root_parent_id;
extern const char *index_xml_file_name;
// template <class X> inline X *find_object(QString n);

// Поиск объекта от корня по имени
template <class X> inline X *find_object(QString objectName){
    QObject *findObj;

    extern QObject *mainwindow;
	// Если запрошен сам корень
    if(objectName == "mainwindow"){
	QObject *mvp = qobject_cast<X *>(mainwindow);
	if(mvp->metaObject()->className() != mainwindow->metaObject()->className()){
		// Если запрошенный класс объекта не является классом главного окна
	    printf("find_object(): Can't find mainwindow object. Check <type> in "
		   "function call\n");
	    exit(1);

	    return nullptr;
	}else return qobject_cast<X *>(mainwindow);
    }
	// Запрошен обычный объект, надо его найти
	// findObj=qFindChild<X *>(mainwindow, objectName);
    findObj = mainwindow->findChild<X *>(objectName);
    if(findObj == nullptr){
	// Если объекта с указанным именем не найдено
	// print_object_tree();
	printf("find_object(): Can't find object with name %s\n",
	    qPrintable(objectName));

	print_object_tree();

	/*
	QList<QWidget *> widgets = mainwindow->findChildren<QWidget *>();
	foreach (QWidget* b, widgets)
	  printf("Obj: %s\n", qPrintable( b->objectName() ) );
	*/

	exit(1);

	return nullptr;
    }else{
	// ОБъект был найден, и нужно преобразовать указатель на него
	// к указателю c заданным в шаблоне типом
	X *obj = qobject_cast<X *>(findObj);
	if(obj == 0){
		// Если найденный объект не может быть преобразован к заданному в шаблоне
		// типу
	    printf("find_object(): Object %s find, but can't convert type. Check "
		   "<type> in function call\n",
		qPrintable(objectName));
	    exit(1);

	    return nullptr;
	}else return obj;	// Объект найден нормально
    }
}
class Sleeper : public QThread {
    public:
	static void	usleep(unsigned long usecs)	{QThread::usleep(usecs);}
	static void	msleep(unsigned long msecs)	{QThread::msleep(msecs);}
	static void	sleep(unsigned long secs)	{QThread::sleep(secs);}
};

// class Record;

extern std::string	url_difference(const std::string &url_compare_stored, const std::string &url_compare_get);
extern bool		url_equal(const std::string &url_compare_stored, const std::string &url_compare_get);

// extern Record *register_record(const QUrl &_url
//                               //                               , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void> >, browser::WebView *, Record *const> > generator
//                               , RecordTableController *record_controller = globalparameters.getRecordTableScreen()->getRecordTableController());

// extern boost::intrusive_ptr<Record> register_record(boost::intrusive_ptr<Record> record
//                                               //                               , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>> generator
//                                               , TableController *record_controller = globalparameters.table_screen()->table_controller());
// extern boost::intrusive_ptr<Record> check_record(const QUrl &_url);

// namespace browser {
//    class Browser;
//    class WebView;
// }

// template<typename> struct active_record;

// extern boost::intrusive_ptr<Record> request_record(const QUrl &_url
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> generator
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> activator);
// extern boost::intrusive_ptr<Record> request_record(const boost::intrusive_ptr<Record> record
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> generator
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>> activator);

// boost::intrusive_ptr<Record> equip_registered(boost::intrusive_ptr<Record> record, browser::WebPage *page);

#endif	// __MAIN_H__
