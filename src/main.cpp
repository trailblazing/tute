#include <sys/timeb.h>

#include <QObject>
#include <QSplashScreen>
#include <QToolButton>
#include <QTranslator>
#include <boost/date_time/posix_time/posix_time.hpp>       // include all types plus i/o
#include <boost/date_time/posix_time/posix_time_types.hpp> // no i/o just types
#include <boost/locale.hpp>
#include <chrono>
#include <ctime>
#include <sstream>

#ifndef QT_VERSION
// #define QT_VERSION 0x050600
#define QT_VERSION 0x050700
#endif

#if QT_VERSION >= 0x050000
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QScrollerProperties>
#endif

#include <qtwebenginewidgetsglobal.h>

#include "main.h"

#include "views/browser/browser.h"
#include "views/browser/webview.h"
#include "views/record/info_field_enter.h"

#include <qversiontagging.h>

#if QT_VERSION < 0x050000
#include "libraries/qtSingleApplication/qtsingleapplication.h"
#else
#include "libraries/qt_single_application5/qtsingleapplication.h"
#endif

#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_records.h"
#include "libraries/crypt/password.h"
#include "libraries/crypt/rc5simple.h"
#include "libraries/disk_helper.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "libraries/trash_monitoring.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"
#include "models/app_config/app_config.h"
#include "models/database_config/database_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "views/main_window/main_window.h"
#include "views/record_table/record_screen.h"

// const int add_new_record_after = 2;
using namespace std;

// Фиксированные параметры программы (жестко заданные в текущей версии )
FixedParameters fixedparameters;

// Глобальные параметры программы (вычислимые на этапе инициализации, иногда
// меняющиеся в процессе выполнения программы)
std::shared_ptr<gl_para> gl_paras;

// Конфигурация программы (считанная из файла конфигурации)
std::shared_ptr<AppConfig> appconfig; // (gl_paras);

// Конфигурация данных
std::shared_ptr<DataBaseConfig> databaseconfig; // (gl_paras, appconfig);

// Объект слежения за состоянием корзины
TrashMonitoring trashmonitoring;

// Объект с историей посещаемых записей
WalkHistory walkhistory;

// Указатель на основное окно программы
QObject* mainwindow;

extern const char* program_title;
extern const QString program_title_qstring;
extern const std::string program_title_string;

const char* app_version = (std::string("APPLICATION_RELEASE_VERSION") + "." + "APPLICATION_RELEASE_SUBVERSION" + "." + "APPLICATION_RELEASE_MICROVERSION").c_str();

bool url_equal(const std::string& url_compare_stored, const std::string& url_compare_get)
{
	std::string difference = url_difference(url_compare_stored, url_compare_get);

	return difference.size() == 0 || difference == "/";
}

bool url_equal(const QString& u0, const QString& u1)
{
	const std::string& url_compare_stored = u0.toStdString();
	const std::string& url_compare_get = u1.toStdString();
	std::string difference = url_difference(url_compare_stored, url_compare_get);

	return difference.size() == 0 || difference == "/";
}

bool url_equal(const url_value& u0, const url_value& u1)
{
	const std::string& url_compare_stored = detail::to_string(u0);
	const std::string& url_compare_get = detail::to_string(u1);
	std::string difference = url_difference(url_compare_stored, url_compare_get);

	return difference.size() == 0 || difference == "/";
}

std::string url_difference(const std::string& url_compare_stored, const std::string& url_compare_get)
{
	std::string compare = "";
	if (url_compare_stored.size() >= url_compare_get.size()) {
		for (std::string::size_type i = 0; i < url_compare_get.size(); i++)                                 // url_compare_stored.erase(url_compare_get.begin(),
														    // url_compare_get.end());
			if (url_compare_stored.at(i) != url_compare_get.at(i)) compare += url_compare_stored.at(i); // url_compare_stored.erase(i, 1);
		for (std::string::size_type i = url_compare_get.size(); i < url_compare_stored.size(); i++)
			compare += url_compare_stored.at(i);
	} else {
		// url_compare_get.erase(url_compare_stored.begin(),
		// url_compare_stored.end());
		for (std::string::size_type i = 0; i < url_compare_stored.size(); i++)                           // url_compare_stored.erase(url_compare_get.begin(),
														 // url_compare_get.end());
			if (url_compare_stored.at(i) != url_compare_get.at(i)) compare += url_compare_get.at(i); // url_compare_get.erase(i, 1);
		for (std::string::size_type i = url_compare_stored.size(); i < url_compare_get.size(); i++)
			compare += url_compare_get.at(i);
	}
	// std::string::size_type pos;
	// while((pos = compare.find_first_of(" ")) != compare.npos)compare.erase(pos,
	// 1);
	compare.erase(std::remove_if(compare.begin(), compare.end(), ::isspace), compare.end()); // [](char ch){return  // std::isspace<char>(ch,// std::locale::classic());}	//

	return compare;
}

void log_print(char* lpszText, ...)
{
	va_list argList;
	FILE* pFile;

	// инициализируем список аргументов
	va_start(argList, lpszText);
	// открываем лог-файл для добавления данных
	if ((pFile = fopen("tutelog.txt", "a+")) == nullptr) {
		printf("\nLog file not writable\n");

		return;
	}
	// пишем текст в файл
	vfprintf(pFile, lpszText, argList);
	// putc('\n', pFile);

	// пишем текст на экран
	vprintf(lpszText, argList);

	// закрываем файл
	fclose(pFile);
	va_end(argList);

	// успешное завершение
	return;
}

void critical_error(QString message)
{
	qDebug() << " ";
	qDebug() << "---------------";
	qDebug() << "Critical error!";
	qDebug() << "---------------";
	qDebug() << message;
	qDebug() << "---------------";
	qDebug() << " ";

	QMessageBox::critical(qobject_cast<QWidget*>(mainwindow), "Critical error", message + "\n\nProgramm was closed.", QMessageBox::Ok);
	//	throw std::runtime_error(message.toStdString());
	exit(1);
}

// Функция-помощник при отладке генерации XML-кода. Преобразует узел DOM в
// строку
QString xml_node_to_qstring(QDomNode xmlData)
{
	// Если узел представляет собой полностью документ
	if (xmlData.isDocument()) {
		// Значит этот узел можно напрямую преобразовать

		// в документ, а потом в XML строку
		return xmlData.toDocument().toString();
	} else {
		// Иначе узел не является документом, и его нужно обрабатывать по-другому

		// Строка, где будет размещен XML код
		QString xmlcode;

		// Поток, связаный со строкой, в который будет направляться XML код узла
		QTextStream stream(&xmlcode, QIODevice::WriteOnly);

		// XML документ записывается в поток, после чего автоматом окажется в строке
		// xmlcode
		xmlData.save(stream, 1);

		return xmlcode;
	}
}

// Преобразование из QString в обычный char
char* qc_str(const QString& str)
{
	//	char *tmpC = new char[str.size() + 1];
	//	QVariant var;
	//	for(int i = 0; i < str.length(); i++){
	//		var = str.at(i);
	//		tmpC[i] = var.toChar().toAscii();
	//	}
	//	tmpC[str.size()] = 0;

	//	return tmpC;

	return str.toLocal8Bit().data();
}

// екурсивная печать дерева объектов, т.к. dumpObjectInfo() и dumpObjectTree()
// не работают
void print_object_tree_recurse(QObject* pobj)
{
	static int indent = 0;

	QObjectList olist;

	olist = pobj->children();
	for (int i = 0; i < olist.size(); ++i) {
		QObject* currobj;
		currobj = olist.at(i);

		QString indentline = ".";
		for (int j = 0; j < indent; j++)
			indentline = indentline + ".";
		if ((currobj->objectName()).length() == 0)
			qDebug("%s%s", qc_str(indentline), currobj->metaObject()->className());
		else
			qDebug("%s%s, NAME %s", qc_str(indentline), currobj->metaObject()->className(), qc_str(currobj->objectName()));
		indent++;
		print_object_tree_recurse(currobj);
		indent--;
	}
}

// Печать дерева объектов, основная функция
void print_object_tree(void)
{
	qDebug() << "Object tree";

	print_object_tree_recurse(mainwindow);
}

//// Функция для сортировки массива из QStringList исходя из длин списков
// bool compare_qstringlist_length(const QStringList &list1, const QStringList
// &list2)
// {
// return list1.size() < list2.size();
// }

// void insertActionAsButton(QToolBar *tools_line, QAction *action)
// {
// tools_line->addAction(action);
// qobject_cast<QToolButton
// *>(tools_line->widgetForAction(action))->setAutoRaise(
// true    //false
// );
// }

// int imax(int x1, int x2)
// {
// if(x1 > x2)return x1;
// else return x2;
// }

// int imin(int x1, int x2)
// {
// if(x1 < x2)return x1;
// else return x2;
// }

std::string get_time()
{
	std::time_t tt = std::chrono::system_clock::to_time_t(
	    std::chrono::system_clock::now()); // std::time(0);// get time now
	struct tm* now = localtime(&tt);
	std::ostringstream _current_time;
	_current_time << std::setw(4) << std::setfill('0')
		      << (now->tm_year + 1900) // << '-'
		      << std::setw(2) << std::setfill('0')
		      << (now->tm_mon + 1)                                 // << '-'
		      << std::setw(2) << std::setfill('0') << now->tm_mday // << '-'
		      << std::setw(2) << std::setfill('0') << now->tm_hour // << '-'
		      << std::setw(2) << std::setfill('0') << now->tm_min  // << '-'
		      << std::setw(2) << std::setfill('0') << now->tm_sec;

	return _current_time.str();
}

QString get_qtime()
{
	return QString::fromStdString(get_time());
}

void smart_print_debug_message(QString msg)
{
	if (gl_paras->target_os() == "any" || gl_paras->target_os() == "meego") {
		////	QTime	currTime	= QTime::currentTime();
		////	QString timeText	= currTime.toString("hh:mm:ss");
		// QDateTime	ctime_dt	= QDateTime::currentDateTime();
		// QString		ctime		= ctime_dt.toString("yyyyMMddhhmmss");

		std::time_t tt = std::chrono::system_clock::to_time_t(
		    std::chrono::system_clock::now()); // std::time(0);// get time now
		struct tm* now = localtime(&tt);
		std::ostringstream _current_time;
		_current_time << (now->tm_year + 1900) << '-' << std::setw(2)
			      << std::setfill('0') << (now->tm_mon + 1) << '-'
			      << std::setw(2) << std::setfill('0') << now->tm_mday << '-'
			      << std::setw(2) << std::setfill('0') << now->tm_hour << '-'
			      << std::setw(2) << std::setfill('0') << now->tm_min << '-'
			      << std::setw(2) << std::setfill('0') << now->tm_sec;
		std::string c_t(_current_time.str());
		//// auto _end = std::chrono::system_clock::now();	//
		///std::chrono::time_point<std::chrono::system_clock>
		// std::time_t end_time =
		// std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		// namespace pt = boost::posix_time;
		// std::ostringstream msg_;
		// const pt::ptime		now_ = pt::second_clock::local_time();
		// pt::time_facet		f("%H-%M-%S");	// pt::time_facet * const f = new
		// pt::time_facet("%H-%M-%S");
		// msg_.imbue(std::locale(msg_.getloc(), &f));
		// msg_ << now_;
		auto ctime = QString::fromStdString(_current_time.str());
		msg = ctime + " " + msg; // timeText + " " + msg;

		unsigned int messageLen = msg.toLocal8Bit().size();
		// printf("Len of line: %d\n", messageLen);

		fwrite(msg.toLocal8Bit().data(), sizeof(char), messageLen, stderr);
	}
	// В Android пока неясно, как смотреть поток ошибок, для андроида qDebug() не
	// переопределяется
}

// Обработчик (хендлер) вызовов qDebug()
// Внутри этого обработчика нельзя использовать вызовы qDebug(), т. к. получится
// рекурсия
#if QT_VERSION < 0x050000
void my_message_output(QtMsgType type, const char* msg)
#else
void my_message_output(QtMsgType type, const QMessageLogContext& context, const QString& msgText)
#endif
{
#if QT_VERSION >= 0x050000
	Q_UNUSED(context);
#endif

// #if DEBUG_PRINT==1

#if QT_VERSION < 0x050000
	QString msgText(QString::fromUtf8(msg));
#endif
	if (!appconfig->is_init()) {
		smart_print_debug_message("[INF] " + msgText + "\n");

		return;
	}
	// Если в конфигурации запрещен вывод отладочных сообщений
	if (!appconfig->print_debug_messages())
		return;
	switch (type) {
	case QtInfoMsg:
		smart_print_debug_message("[INF] " + msgText + "\n");
		break;

	case QtDebugMsg:
		smart_print_debug_message("[DBG] " + msgText + "\n");
		break;

	case QtWarningMsg:
		smart_print_debug_message("[WRN] " + msgText + "\n");
		break;

	case QtCriticalMsg:
		smart_print_debug_message("[CRERR] " + msgText + "\n");
		break;

	case QtFatalMsg:
		smart_print_debug_message("[FTERR] " + msgText + "\n");
		abort();
	}
	// #endif
}

void set_debug_message_handler()
{
	qDebug() << "Debug message before set message handler for target OS: "
		 << gl_paras->target_os();
	// Для десктопных операционок можно переустановить обработчик qDebug()
	// Для Андроида переустановка qDebug() приводит к невозможности получения
	// отладочных сообщений в удаленном отладчике
	if (gl_paras->target_os() == "any" || gl_paras->target_os() == "meego") {
		qDebug() << "Set alternative handler my_message_output() for debug message";

#if QT_VERSION < 0x050000
		qInstallMsgHandler(my_message_output);
#else
		qInstallMessageHandler(my_message_output);
#endif
	}
	qDebug() << "Debug message after set message handler";
}

// Выдача на экран простого окна с сообщением
void show_message_box(QString message)
{
	QMessageBox msg_box;

	msg_box.setText(message);
	msg_box.exec();
}

int screen_size_y(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
	int size = (QtSingleApplication::instance()->desktop()->availableGeometry())
		       .height();
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
	int size = (QApplication::screens().at(0)->availableGeometry()).height();
#endif

	return size;
}

int screen_size_x(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
	int size =
	    (QtSingleApplication::instance()->desktop()->availableGeometry()).width();
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
	int size = (QApplication::screens().at(0)->availableGeometry()).width();
#endif

	return size;
}

qreal calculate_iconsize_px(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
	qreal dpiX = QtSingleApplication::instance()->desktop()->physicalDpiX();
	qreal dpiY = QtSingleApplication::instance()->desktop()->physicalDpiY();
	qreal dpi = (dpiX + dpiY) / 2;
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
	qreal dpi = QApplication::screens().at(0)->physicalDotsPerInch();
#endif

	qreal iconSizeMm = 6;                   // азмер иконки в миллиметрах (рекомендованный)
	qreal iconSizeInch = iconSizeMm / 25.4; // азмер иконки в дюймах
	qreal iconSizePx = iconSizeInch * dpi;

	return iconSizePx;
}

// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный
// размер иконки в пикселях
// Replacement CSS-style of all occurrences of a substring META_ICON_SIZE
// calculated on the icon size in pixels
QString replace_css_meta_iconsize(QString styleText)
{
	styleText.replace("META_ICON_SIZE", QString::number((int)calculate_iconsize_px()));
	styleText.replace("META_ICON_HALF_SIZE", QString::number((int)calculate_iconsize_px() / 2));
	styleText.replace("META_ICON_TWO_THIRDS_SIZE", QString::number(((int)calculate_iconsize_px() * 2) / 3));
	styleText.replace("META_ICON_QUARTER_SIZE", QString::number((int)calculate_iconsize_px() / 4));
	styleText.replace("META_ICON_FIFTH_SIZE", QString::number((int)calculate_iconsize_px() / 5));
	styleText.replace("META_ICON_SIXTH_SIZE", QString::number((int)calculate_iconsize_px() / 6));

	return styleText;
}


void set_kinetic_scrollarea(QAbstractItemView* object)
{
#if QT_VERSION < 0x050000

	Q_UNUSED(object);

	return;

#else
	if (object == nullptr)
		return;
	if (gl_paras->target_os() == "android") {
		// Настройка жестов прокрутки
		QScroller* scroller = QScroller::scroller(object);

		// For desktop - QScroller::LeftMouseButtonGesture, for Android -
		// QScroller::TouchGesture in doc
		// TouchGesture по факту на Андроиде не работает, а LeftMouseButtonGesture -
		// почему-то работает
		scroller->grabGesture(object, QScroller::LeftMouseButtonGesture);

		// Поведение прокрутки на краях списка (сейчас не пружинит)
		QScrollerProperties properties = scroller->scrollerProperties();
		QVariant overshootPolicy =
		    QVariant::fromValue<QScrollerProperties::OvershootPolicy>(
			QScrollerProperties::OvershootWhenScrollable // OvershootAlwaysOff
			);
		properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
		properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
		scroller->setScrollerProperties(
		    properties); // QScrollerProperties::OvershootAlwaysOff

		// Горизонтальный скроллинг скрывается
		object->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");

		// QScrollBar::add-line:horizontal { border: none; background: none; }
		// QScrollBar::sub-line:horizontal { border: none; background: none; }
		// QScrollBar {width:3px;}
		// QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}
		// QScrollBar::add-line:vertical { height: 0px; }
		// QScrollBar::sub-line:vertical { height: 0px; }
		// QScrollBar::add-line:vertical { border: none; background: none; height:
		// 0px; } QScrollBar::sub-line:vertical { border: none; background: none;
		// height: 0px; }
		// background: transparent; background-color:transparent;
		// "QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}"

		object->verticalScrollBar()->setStyleSheet(
		    "QScrollBar:vertical {width:3px; border: none; background: "
		    "transparent; margin: 0;}"
		    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {width: "
		    "0px; height: 0px; border: none;  background: transparent; image: "
		    "url(:/resource/pic/transparent_dot.png); }"
		    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { "
		    "image: url(:/resource/pic/transparent_dot.png); }");

		// object->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal
		// {border: 2px solid black; background: grey; height: 15px;}"
		// "QScrollBar::add-line:horizontal {border none; background: none;}"
		// "QScrollBar::sub-line:horizontal {border none; background: none;}"
		// );

		object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

		// object->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	} else {
		object->verticalScrollBar()->setStyleSheet(QString::fromUtf8(
		    "QScrollBar:vertical {"
		    "   border: 1px solid #aaaaaa;"
		    "   background: transparent;"          // "   background: #eeeeee;" // white;"
		    "   width:10px;    "                   // "width: 10px;"
		    "   margin: 0px 0px 0px 0px;"          // "margin: 21px 0 21px 0;"
		    "   border-color: rgb(227, 227, 227);" // "   border-width: 1px;"
		    "   border-style: solid;"
		    "   background-color: transparent; " // "   background-color: rgb(240,
							 // 240, 240);"
		    "}"
		    "QScrollBar::handle:vertical {"
		    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
		    "   stop: 0  #666666, stop: 0.5 #666666,  stop:1 #666666);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   min-height: 10px;"                                      // "min-height: 25px;"
		    "   background-color: #AEA89A;"                             // rgb(180, 180, 180)  // 200,200,200
		    "   border: 1px #948B77 solid;"
		    "}"
		    "QScrollBar::add-line:vertical {"
		    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
		    "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   height: 10px;"                                          // "height: 20px;"
		    "   subcontrol-position: bottom;"                           // "subcontrol-position: bottom;"
		    "   subcontrol-origin: margin;"                             // "subcontrol-origin: margin;"
		    "   border: 1px solid grey;"
		    "   background-color: rgb(241, 241, 241);"
		    "}"
		    "QScrollBar::sub-line:vertical {"
		    "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
		    "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   height: 10px;"                                          // "height: 20px;"
		    "   subcontrol-position: top;"                              // "subcontrol-position: top;"
		    "   subcontrol-origin: margin;"                             // "subcontrol-origin: margin;"
		    "   border: 1px solid grey;"
		    "   background-color: rgb(241, 241, 241);"
		    "}"
		    ""
		    "QScrollBar::add-page: vertical, QScrollBar::sub-page: vertical {"
		    "   background: none;"
		    "}"
		    "QScrollBar::up-arrow: vertical {"
		    "   image: url(:/BarIcon/Icons/uparrow.png);"
		    "}"
		    "QScrollBar::down-arrow: vertical {"
		    "   image: url(:/BarIcon/Icons/downarrow.png);"
		    "}"
		    "QScrollBar::handle: hover {"
		    "   background-color: rgb(100, 100, 100);"
		    "   width:10px;"
		    "}"
		    // "QScrollBar: vertical {"
		    // "border-color: rgb(227, 227, 227);"
		    // "border-width: 1px;"
		    // "border-style: solid;"
		    // "background-color: rgb(240, 240, 240);"
		    // "width: 10px;"
		    // "margin: 21px 0 21px 0;"
		    // "}"
		    // "QScrollBar::handle: vertical {"
		    // "background-color: rgb(200, 200, 200);"
		    // "min-height: 25px;"
		    // "}"
		    // "QScrollBar::add-line: vertical {"
		    // "border: 1px solid grey;"
		    // "background-color: rgb(241, 241, 241);"
		    // "height: 20px;"
		    // "subcontrol-position: bottom;"
		    // "subcontrol-origin: margin;"
		    // "}"
		    // "QScrollBar::sub-line: vertical {"
		    // "border: 1px solid grey;"
		    // "background-color: rgb(241, 241, 241);"
		    // "height: 20px;"
		    // "subcontrol-position: top;"
		    // "subcontrol-origin: margin;"
		    // "}"

		    ));

		object->horizontalScrollBar()->setStyleSheet(QString::fromUtf8(
		    "QScrollBar:horizontal {"
		    "   border: 1px solid #aaaaaa;"
		    "   background: transparent;"          // "   background: #eeeeee;"
		    "   height:10px;    "                  // "   height: 10px; "
		    "   margin: 0px 0px 0px 0px;"          // "   margin: 0px 21px 0 21px; "
		    "   border-color: rgb(227, 227, 227);" // "   border-height: 1px;"
		    "   border-style: solid; "
		    "   background-color: transparent; " // "   background-color: rgb(240,
							 // 240, 240); "
		    "}"
		    "QScrollBar::handle:horizontal {"
		    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
		    "   stop: 0  #666666, stop: 0.5 #666666,  stop:1 #666666);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   min-width: 10px;"                                       // "   min-width: 25px; "
		    "   background-color: #AEA89A;"                             // rgb(180, 180, 180)  // 200,200,200
		    "   border: 1px #948B77 solid;"
		    "}"
		    "QScrollBar::add-line:horizontal {"
		    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
		    "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   width: 10px;"                                           // "   width: 20px; "   // "    height: px;"
		    "   subcontrol-position: right;"                            // "   subcontrol-position: right; "
		    "   subcontrol-origin: margin;"                             // "   subcontrol-origin: margin; "
		    "   border: 1px solid grey;"
		    "   background-color: rgb(241, 241, 241); "
		    "}"
		    "QScrollBar::sub-line:horizontal {"
		    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
		    "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);" // "
										// stop: 0
										// rgb(32,
										// 47, 130),
										// stop: 0.5
										// rgb(32,
										// 47, 130),
										// stop:1
										// rgb(32,
										// 47,
										// 130));"
		    "   width: 10px;"                                           // "   width: 20px; "   // "    height: 0px;"
		    "   subcontrol-position: left;"                             // "   subcontrol-position: left; "
		    "   subcontrol-origin: margin;"                             // "   subcontrol-origin: margin; "
		    "   border: 1px solid grey; "
		    "   background-color: rgb(241, 241, 241); "
		    "}"
		    "QScrollBar::left-arrow: horizontal"
		    "{"
		    "   image: url(:/BarIcon/Icons/leftarrow.png); "
		    "}"
		    "QScrollBar::right-arrow: horizontal"
		    "{"
		    "   image: url(:/BarIcon/Icons/rightarrow.png); "
		    "}"
		    "QScrollBar::add-page: horizontal, QScrollBar::sub-page: horizontal {"
		    "   background: none; "
		    "}"
		    "QScrollBar::handle: hover {"
		    "   background-color: rgb(100, 100, 100);"
		    "   height:10px;"
		    "}"
		    // ""
		    // "QScrollBar:horizontal"
		    // "{"
		    // "    border-color: rgb(227, 227, 227);"
		    // "    border-height: 1px;"
		    // "    border-style: solid; "
		    // "    background-color: rgb(240, 240, 240); "
		    // "    height: 10px; "
		    // "    margin: 0px 21px 0 21px; "
		    // "}"
		    // ""
		    // "QScrollBar::handle: horizontal {"
		    // "background-color: rgb(200, 200, 200); "
		    // "min-width: 25px; "
		    // "}"
		    // "QScrollBar::add-line: horizontal {"
		    // "border: 1px solid grey;"
		    // "background-color: rgb(241, 241, 241); "
		    // "width: 20px; "
		    // "subcontrol-position: right; "
		    // "subcontrol-origin: margin; "
		    // "}"
		    // "QScrollBar::sub-line: horizontal {"
		    // "border: 1px solid grey; "
		    // "background-color: rgb(241, 241, 241); "
		    // "width: 20px; "
		    // "subcontrol-position: left; "
		    // "subcontrol-origin: margin; "
		    // "}"

		    ));
		object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

		object->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	}
#endif
}

QString purify(QString text)
{
	text = QObject::tr(
	    text.remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]")).toStdString().c_str()); //QRegExp("[\"/\\\\<>\\?:\\*\\|]+")
	text.replace('"', ' ');
	text.replace("'", " ");
	//	text.replace('.', ' ');
	text.replace(',', ' ');
	text.replace(';', ' ');
	text.replace(':', ' ');
	text.replace('-', ' ');
	text.replace('?', ' ');
	text.replace('!', ' ');
	text = text.simplified();
	//	QStringList list = text.split(QRegExp("\\W + "),
	//QString::SkipEmptyParts);

	return text;
}

QStringList text_delimiter_decompose(QString text)
{
	//	text.replace('"', ' ');
	//	text.replace("'", " ");
	//	text.replace('.', ' ');
	//	text.replace(',', ' ');
	//	text.replace(';', ' ');
	//	text.replace(':', ' ');
	//	text.replace('-', ' ');
	//	text.replace('?', ' ');
	//	text.replace('!', ' ');
	text = purify(text);

	QStringList list = text.split(QRegExp("\\W + "), QString::SkipEmptyParts);

	return list;
}

// Функция всегда возвращает уникальный идентификатор
id_value get_unical_id(void)
{
	// Уникальный идентификатор состоит из 10 цифр количества секунд с эпохи UNIX
	// и 10 случайных символов 0-9 a-z

	// Количество секунд как число
	long seconds;

	seconds = (long)time(nullptr);

	// Количество секунд как строка
	QString secondsLine = QString::number(seconds, 10);
	secondsLine = secondsLine.rightJustified(10, '0');

	// Строка из 10 случайных символов
	QString symbols = "0123456789abcdefghijklmnopqrstuvwxyz";
	QString line;
	for (int i = 0; i < 10; i++)
		line += symbols.mid(rand() % symbols.length(), 1);
	QString result = secondsLine + line;

	return detail::from_qstring<id_key>(result);
}

int get_milli_count(void)
{
	// Something like GetTickCount but portable
	// It rolls over every ~ 12.1 days (0x100000/24/60/60)
	// Use getMilliSpan to correct for rollover
	timeb tb;

	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;

	return nCount;
}

void init_random(void)
{
	qDebug() << "Init random generator";

	unsigned int seed1 = get_milli_count();
	srand(seed1);

	unsigned int delay = rand() % 1000;
	unsigned int r = 0;
	for (unsigned int i = 0; i < delay; i++)
		r = r + rand();
	seed1 = seed1 - get_milli_count() + r;

	unsigned int seed2 = time(nullptr);
	unsigned int seed3 = seed1 + seed2;
	unsigned int seed = seed3;

	srand(seed);
}

#ifdef USE_QTM

#include "libraries/qt_single_application5/qtsingleapplication.h"

#ifdef USE_SYSTRAYICON
#include "libraries/qtm/sys_tray_icon.h"
#ifdef Q_OS_MAC
#include "SuperMenu.h"
#endif
#endif

#endif

int main(int argc, char** argv)
{
#ifdef USE_QTM

// EditingWindow *c;
#ifdef USE_SYSTRAYICON
// SysTrayIcon *sti;
#endif

#ifdef Q_OS_MAC
#if QT_VERSION <= 0x050000
	if (QSysInfo::MacintoshVersion > QSysInfo::MV_10_8) {
		// Fix Mavericks font issue:
		// https://bugreports.qt-project.org/browse/QTBUG-32789
		QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
	}
#endif // QT_VERSION
#endif // Q_OS_MAC
	{
		// Application app(argc, argv);
		// QStringList args = app.arguments();
		// if(args.contains("--delete-sandbox")){
		// app.deleteSandbox();
		// exit(0);
		// }
	}
#ifdef USE_SYSTRAYICON
	{
// if(QSystemTrayIcon::isSystemTrayAvailable()){
// sti = new SysTrayIcon;
// if(! sti->dontStart()){
#ifndef Q_OS_MAC
// sti->show();
#else
// SuperMenu *smenu = new SuperMenu(0, sti);
// smenu->setObjectName("MacGlobalMenu");
// smenu->show();
// smenu->handleNewWindowAtStartup();
#endif
		{
		    // app.setQuitOnLastWindowClosed(false);
		} // }
		// }else{
		{
			// c = new EditingWindow;
			// c->setSTI(0);	// No STI
			// c->setWindowTitle(QObject::tr((program_title_string + " - new entry
			// [*]").c_str()));
			// if(c->handleArguments()) c->show();
			// else c->close();
		}
		// }
	}
#else
	app.setupRecentFiles();
	c = new EditingWindow;
	c->setWindowTitle(QObject::tr((program_title_string + " - new entry [*]").c_str()));
	if (c->handleArguments()) {
#if Q_OS_MAC
		setNoStatusBar(c);
#endif
		c->show();
	} else {
		qDebug("closing");
		c->close();
	}
#endif
	{
		// #ifdef USE_SYSTRAYICON
		// if(QSystemTrayIcon::isSystemTrayAvailable()){
		// if(! sti->dontStart()) return app.exec();
		// }else return app.exec();
		// #else
		// return app.exec();
		// #endif
	}

#endif // USE_QTM

	printf(
	    std::string("\n\rStart " + program_title_string + " v. % d. % d. % d\n\r").c_str(),
	    APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	Q_INIT_RESOURCE(data); // added by hughvonyoung@gmail.com
	Q_INIT_RESOURCE(tute);
	// gl_paras	= std::make_shared<gl_para>();
	// appconfig	= std::make_shared<AppConfig>(gl_paras);
	// databaseconfig = std::make_shared<DataBaseConfig>(gl_paras, appconfig);
	return sapp_t(argc, argv).exec(); // , gl_paras		// , appconfig		// ,
					  // databaseconfig
					  // application.exec();
}

std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(
	    filename, std::ios::ate | std::ios::binary); // std::ifstream in(filename,
							 // std::ifstream::ate |
							 // std::ifstream::binary);
	return in.tellg();
}

std::streampos file_size(const char* file_path)
{
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);
	std::streampos fsize = file.tellg();
	// file.close();

	return fsize;
}

// similar to QString::leftJustified
std::string truncate(std::string str, size_t width, bool show_ellipsis)
{
	if (str.length() > width) {
		if (show_ellipsis)
			return str.substr(0, width - 3) + "...";
		else
			return str.substr(0, width);
	}
	return str;
}

auto to_be_url(const url_value& url_) -> QUrl
{
	return to_be_url(detail::to_qstring(url_));
}

auto to_be_url(const QUrl& url_) -> QUrl
{
	QUrl url = url_;
	QUrl result;
	qDebug() << "\tmain.cpp::to_be_url::url=\t" << url // << "\n"
		 << "\turl.scheme() =\t" << url.scheme();
	if (url.scheme().isEmpty() && !url.topLevelDomain().isNull()) {

		url.setScheme("https"); //QUrl(QString("https://") + url.toString());
					//				result = url;
	}

	qDebug() << "\t!url.isRelative() =\t" << !url.isRelative() << "\n"
		 << "\t!url.topLevelDomain().isNull() =\t" << !url.topLevelDomain().isNull() << "\n"
		 << "\t!url.isEmpty() =\t" << !url.isEmpty() << "\n"
		 << "\t!url.host().isNull() =\t" << !url.host().isNull() << "\n" // flase
		 << "\turl.isValid() =\t" << url.isValid() << "\n"
		 << "\t!url.scheme().isEmpty() =\t" << !url.scheme().isEmpty();
	if (!url.isEmpty() && //&& !url.host().isNull()
	    !url.scheme().isEmpty() &&
	    !url.topLevelDomain().isNull() &&
	    !url.isRelative() &&
	    url != QUrl(web::Browser::_defaulthome) &&
	    url.isValid()) result = url;
	return result;
}
