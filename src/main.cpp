#include <sys/timeb.h>

#include <QTranslator>
#include <QToolButton>
#include <QSplashScreen>

#if QT_VERSION >= 0x050000
#include <QScroller>
#include <QScrollerProperties>
#include <QScrollBar>
#include <QScrollArea>
#endif

#include <qtwebenginewidgetsglobal.h>

#include "main.h"

#include "views/browser/browser.h"
#include "views/browser/webview.h"
#include "views/record/InfoFieldEnter.h"


#include <qversiontagging.h>

#ifndef QT_VERSION
#define QT_VERSION 0x050600
#endif

#if QT_VERSION < 0x050000
#include "libraries/qtSingleApplication/qtsingleapplication.h"
#else
#include "libraries/qt_single_application5/qtsingleapplication.h"
#endif

#include "views/main_window/MainWindow.h"
#include "models/app_config/AppConfig.h"
#include "libraries/DiskHelper.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/TrashMonitoring.h"
#include "libraries/FixedParameters.h"
#include "libraries/GlobalParameters.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/crypt/RC5Simple.h"
#include "libraries/crypt/Password.h"
#include "libraries/GlobalParameters.h"
#include "views/record_table/RecordScreen.h"
#include "models/database_config/DataBaseConfig.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "controllers/record_table/RecordController.h"

//const int add_new_record_after = 2;
using namespace std;

// Фиксированные параметры программы (жестко заданные в текущей версии MyTetra)
FixedParameters fixedparameters;

// Глобальные параметры программы (вычислимые на этапе инициализации, иногда меняющиеся в процессе выполнения программы)
GlobalParameters globalparameters;

// Конфигурация программы (считанная из файла конфигурации)
AppConfig appconfig;

// Конфигурация данных
DataBaseConfig databaseconfig;

// Объект слежения за состоянием корзины
TrashMonitoring trashmonitoring;

// Объект с историей посещаемых записей
WalkHistory walkhistory;

// Указатель на основное окно программы
QObject             *mainwindow;



std::string url_difference(const std::string &url_compare_stored, const std::string &url_compare_get)
{
    std::string compare = "";

    if(url_compare_stored.size() >= url_compare_get.size()) {
        for(std::string::size_type i = 0; i < url_compare_get.size(); i++) {                                         //url_compare_stored.erase(url_compare_get.begin(), url_compare_get.end());
            if(url_compare_stored.at(i) != url_compare_get.at(i)) compare += url_compare_stored.at(i);                                                                     //url_compare_stored.erase(i, 1);
        }

        for(std::string::size_type i = url_compare_get.size(); i < url_compare_stored.size(); i++)
            compare += url_compare_stored.at(i);
    } else {
        //url_compare_get.erase(url_compare_stored.begin(), url_compare_stored.end());
        for(std::string::size_type i = 0; i < url_compare_stored.size(); i++) {                                         //url_compare_stored.erase(url_compare_get.begin(), url_compare_get.end());
            if(url_compare_stored.at(i) == url_compare_get.at(i)) compare += url_compare_get.at(i);                                                                     //url_compare_get.erase(i, 1);
        }

        for(std::string::size_type i = url_compare_stored.size(); i < url_compare_get.size(); i++)
            compare += url_compare_get.at(i);
    }

    std::string::size_type pos;

    while((pos = compare.find_first_of(" ")) != compare.npos) compare.erase(pos, 1);

    return compare;
}

void log_print(char *lpszText, ...)
{
    va_list argList;
    FILE *pFile;

    // инициализируем список аргументов
    va_start(argList, lpszText);

    // открываем лог-файл для добавления данных
    if((pFile = fopen("mytetralog.txt", "a+")) == nullptr) {
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

    QMessageBox::critical(qobject_cast<QWidget *>(mainwindow), "Critical error",
        message + "\n\nProgramm was closed.",
        QMessageBox::Ok);

    exit(1);
}


// Функция-помощник при отладке генерации XML-кода. Преобразует узел DOM в строку
QString xml_node_to_qstring(QDomNode xmlData)
{
    // Если узел представляет собой полностью документ
    if(xmlData.isDocument()) {
        // Значит этот узел можно напрямую преобразовать
        // в документ, а потом в XML строку
        return xmlData.toDocument().toString();
    } else {
        // Иначе узел не является документом, и его нужно обрабатывать по-другому

        // Строка, где будет размещен XML код
        QString xmlcode;

        // Поток, связаный со строкой, в который будет направляться XML код узла
        QTextStream stream(&xmlcode, QIODevice::WriteOnly);

        // XML документ записывается в поток, после чего автоматом окажется в строке xmlcode
        xmlData.save(stream, 1);

        return xmlcode;
    }
}


// Преобразование из QString в обычный char
char *qstring_to_char(const QString &str)
{
    /*
       char *tmpC=new char [str.size() + 1];
       QVariant var;

       for(int i=0;i<str.length();i++)
       {
       var=str.at(i);
       tmpC[i] = var.toChar().toAscii();
       }

       tmpC[str.size()] = 0;

       return tmpC;
     */

    return str.toLocal8Bit().data();
}


// Рекурсивная печать дерева объектов, т.к. dumpObjectInfo() и dumpObjectTree() не работают
void print_object_tree_recurse(QObject *pobj)
{
    static int indent = 0;

    QObjectList olist;

    olist = pobj->children();

    for(int i = 0; i < olist.size(); ++i) {
        QObject *currobj;
        currobj = olist.at(i);

        QString indentline = ".";

        for(int j = 0; j < indent; j++) indentline = indentline + ".";

        if((currobj->objectName()).length() == 0)
            qDebug("%s%s", qstring_to_char(indentline), currobj->metaObject()->className());
        else
            qDebug("%s%s, NAME %s", qstring_to_char(indentline), currobj->metaObject()->className(), qstring_to_char(currobj->objectName()));

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
//bool compare_qstringlist_length(const QStringList &list1, const QStringList &list2)
//{
//    return list1.size() < list2.size();
//}


//void insertActionAsButton(QToolBar *tools_line, QAction *action)
//{
//    tools_line->addAction(action);
//    qobject_cast<QToolButton *>(tools_line->widgetForAction(action))->setAutoRaise(
//        true    //false
//    );
//}


//int imax(int x1, int x2)
//{
//    if(x1 > x2)return x1;
//    else return x2;
//}


//int imin(int x1, int x2)
//{
//    if(x1 < x2)return x1;
//    else return x2;
//}


void smart_print_debug_message(QString msg)
{
    if(globalparameters.target_os() == "any" ||
        globalparameters.target_os() == "meego") {
        QTime currTime = QTime::currentTime();
        QString timeText = currTime.toString("hh:mm:ss");
        msg = timeText + " " + msg;

        unsigned int messageLen = msg.toLocal8Bit().size();
        // printf("Len of line: %d\n", messageLen);

        fwrite(msg.toLocal8Bit().data(), sizeof(char), messageLen, stderr);
    }

    // В Android пока неясно, как смотреть поток ошибок, для андроида qDebug() не переопределяется
}


// Обработчик (хендлер) вызовов qDebug()
// Внутри этого обработчика нельзя использовать вызовы qDebug(), т. к. получится рекурсия
#if QT_VERSION < 0x050000
void my_message_output(QtMsgType type, const char *msg)
#else
void my_message_output(QtMsgType type, const QMessageLogContext &context, const QString &msgText)
#endif
{
#if QT_VERSION >= 0x050000
    Q_UNUSED(context);
#endif

    // #if DEBUG_PRINT==1

#if QT_VERSION < 0x050000
    QString msgText(QString::fromUtf8(msg));
#endif


    if(!appconfig.is_init()) {
        smart_print_debug_message("[INF] " + msgText + "\n");
        return;
    }

    // Если в конфигурации запрещен вывод отладочных сообщений
    if(!appconfig.print_debug_messages())
        return;

    switch(type) {
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
    qDebug() << "Debug message before set message handler for target OS: " << globalparameters.target_os();

    // Для десктопных операционок можно переустановить обработчик qDebug()
    // Для Андроида переустановка qDebug() приводит к невозможности получения отладочных сообщений в удаленном отладчике
    if(globalparameters.target_os() == "any" ||
        globalparameters.target_os() == "meego") {
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
    int size = (QtSingleApplication::instance()->desktop()->availableGeometry()).height();
#endif

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    int size = (QApplication::screens().at(0)->availableGeometry()).height();
#endif

    return size;
}


int screen_size_x(void)
{
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
    int size = (QtSingleApplication::instance()->desktop()->availableGeometry()).width();
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

    qreal iconSizeMm = 6;             // Размер иконки в миллиметрах (рекомендованный)
    qreal iconSizeInch = iconSizeMm / 25.4;             // Размер иконки в дюймах
    qreal iconSizePx = iconSizeInch * dpi;

    return iconSizePx;
}


// Замена в CSS-стиле все вхождения подстроки META_ICON_SIZE на вычисленный размер иконки в пикселях
// Replacement CSS-style of all occurrences of a substring META_ICON_SIZE calculated on the icon size in pixels
QString replace_css_meta_iconsize(QString styleText)
{
    styleText.replace("META_ICON_SIZE", QString::number((int) calculate_iconsize_px()));
    styleText.replace("META_ICON_HALF_SIZE", QString::number((int)calculate_iconsize_px() / 2));
    styleText.replace("META_ICON_TWO_THIRDS_SIZE", QString::number(((int)calculate_iconsize_px() * 2) / 3));
    styleText.replace("META_ICON_QUARTER_SIZE", QString::number((int)calculate_iconsize_px() / 4));
    styleText.replace("META_ICON_FIFTH_SIZE", QString::number((int)calculate_iconsize_px() / 5));
    styleText.replace("META_ICON_SIXTH_SIZE", QString::number((int)calculate_iconsize_px() / 6));

    return styleText;
}


QString set_css_style()
{
    QString style;

    QString csspath = globalparameters.work_directory() + "/stylesheet.css";

    QFile css(csspath);

    bool openResult = css.open(QIODevice::ReadOnly | QIODevice::Text);

    // Если файла не существует
    if(!openResult) {
        qDebug() << "Stylesheet not found in " << csspath << ". Create new css file.";
        globalparameters.create_stylesheet_file(globalparameters.work_directory());
    }

    css.close();    // ?

    // Заново открывается файл
    if(css.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Stylesheet success loaded from" << csspath;
//        QString
        style = QTextStream(&css).readAll();

        style = replace_css_meta_iconsize(style);

        QtSingleApplication::instance()->setStyleSheet(style);
        css.close();
    }
    return style;
}


void set_kinetic_scrollarea(QAbstractItemView *object)
{
#if QT_VERSION < 0x050000

    Q_UNUSED(object);
    return;

#else

    if(object == nullptr)
        return;

    if(globalparameters.target_os() == "android") {
        // Настройка жестов прокрутки
        QScroller *scroller = QScroller::scroller(object);

        // For desktop - QScroller::LeftMouseButtonGesture, for Android - QScroller::TouchGesture in doc
        // TouchGesture по факту на Андроиде не работает, а LeftMouseButtonGesture - почему-то работает
        scroller->grabGesture(object, QScroller::LeftMouseButtonGesture);

        // Поведение прокрутки на краях списка (сейчас не пружинит)
        QScrollerProperties properties = scroller->scrollerProperties();
        QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::
                OvershootWhenScrollable                                                           // OvershootAlwaysOff
                                                                                            );
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
        scroller->setScrollerProperties(properties);                                         // QScrollerProperties::OvershootAlwaysOff

        // Горизонтальный скроллинг скрывается
        object->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");

        // QScrollBar::add-line:horizontal { border: none; background: none; } QScrollBar::sub-line:horizontal { border: none; background: none; }
        // QScrollBar {width:3px;}
        // QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}
        // QScrollBar::add-line:vertical { height: 0px; } QScrollBar::sub-line:vertical { height: 0px; }
        // QScrollBar::add-line:vertical { border: none; background: none; height: 0px; } QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }
        // background: transparent; background-color:transparent;
        // "QScrollBar::up-arrow, QScrollBar::down-arrow {width: 0px; height: 0px;}"

        object->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width:3px; border: none; background: transparent; margin: 0;}"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {width: 0px; height: 0px; border: none;  background: transparent; image: url(:/resource/pic/transparent_dot.png); }"
            "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { image: url(:/resource/pic/transparent_dot.png); }");





        //        object->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {border: 2px solid black; background: grey; height: 15px;}"
        //                                                     "QScrollBar::add-line:horizontal {border none; background: none;}"
        //                                                     "QScrollBar::sub-line:horizontal {border none; background: none;}"
        //                                                     );



        object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        //        object->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    } else {

        object->verticalScrollBar()->setStyleSheet(
            QString::fromUtf8(
                "QScrollBar:vertical {"
                "   border: 1px solid #aaaaaa;"
                "   background: transparent;"   // "   background: #eeeeee;"                                         // white;"
                "   width:10px;    "   // "width: 10px;"
                "   margin: 0px 0px 0px 0px;"  // "margin: 21px 0 21px 0;"
                "   border-color: rgb(227, 227, 227);"  //                 "   border-width: 1px;"
                "   border-style: solid;"
                "   background-color: transparent; " // "   background-color: rgb(240, 240, 240);"
                "}"
                "QScrollBar::handle:vertical {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                "   stop: 0  #666666, stop: 0.5 #666666,  stop:1 #666666);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   min-height: 10px;" // "min-height: 25px;"
                "   background-color: #AEA89A;"  // rgb(180, 180, 180)  // 200,200,200
                "   border: 1px #948B77 solid;"
                "}"
                "QScrollBar::add-line:vertical {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   height: 10px;" // "height: 20px;"
                "   subcontrol-position: bottom;"  // "subcontrol-position: bottom;"
                "   subcontrol-origin: margin;"    // "subcontrol-origin: margin;"
                "   border: 1px solid grey;"
                "   background-color: rgb(241, 241, 241);"
                "}"
                "QScrollBar::sub-line:vertical {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   height: 10px;"  // "height: 20px;"
                "   subcontrol-position: top;"  // "subcontrol-position: top;"
                "   subcontrol-origin: margin;" // "subcontrol-origin: margin;"
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
//                "QScrollBar: vertical {"
//                "border-color: rgb(227, 227, 227);"
//                "border-width: 1px;"
//                "border-style: solid;"
//                "background-color: rgb(240, 240, 240);"
//                "width: 10px;"
//                "margin: 21px 0 21px 0;"
//                "}"
//                "QScrollBar::handle: vertical {"
//                "background-color: rgb(200, 200, 200);"
//                "min-height: 25px;"
//                "}"
//                "QScrollBar::add-line: vertical {"
//                "border: 1px solid grey;"
//                "background-color: rgb(241, 241, 241);"
//                "height: 20px;"
//                "subcontrol-position: bottom;"
//                "subcontrol-origin: margin;"
//                "}"
//                "QScrollBar::sub-line: vertical {"
//                "border: 1px solid grey;"
//                "background-color: rgb(241, 241, 241);"
//                "height: 20px;"
//                "subcontrol-position: top;"
//                "subcontrol-origin: margin;"
//                "}"

                             )
                                                  );



        object->horizontalScrollBar()->setStyleSheet(
            QString::fromUtf8(
                "QScrollBar:horizontal {"
                "   border: 1px solid #aaaaaa;"
                "   background: transparent;"   // "   background: #eeeeee;"
                "   height:10px;    "   // "   height: 10px; "
                "   margin: 0px 0px 0px 0px;"   // "   margin: 0px 21px 0 21px; "
                "   border-color: rgb(227, 227, 227);"  // "   border-height: 1px;"
                "   border-style: solid; "
                "   background-color: transparent; " // "   background-color: rgb(240, 240, 240); "
                "}"
                "QScrollBar::handle:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "   stop: 0  #666666, stop: 0.5 #666666,  stop:1 #666666);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   min-width: 10px;"   // "   min-width: 25px; "
                "   background-color: #AEA89A;" // rgb(180, 180, 180)  // 200,200,200
                "   border: 1px #948B77 solid;"
                "}"
                "QScrollBar::add-line:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   width: 10px;"   // "   width: 20px; "   // "    height: px;"
                "   subcontrol-position: right;"    // "   subcontrol-position: right; "
                "   subcontrol-origin: margin;"     // "   subcontrol-origin: margin; "
                "   border: 1px solid grey;"
                "   background-color: rgb(241, 241, 241); "
                "}"
                "QScrollBar::sub-line:horizontal {"
                "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "   stop: 0  #999999, stop: 0.5 #999999,  stop:1 #999999);"                                         // "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                "   width: 10px;"   // "   width: 20px; "   // "    height: 0px;"
                "   subcontrol-position: left;" // "   subcontrol-position: left; "
                "   subcontrol-origin: margin;" // "   subcontrol-origin: margin; "
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
                //                ""
//                "QScrollBar:horizontal"
//                "{"
//                "    border-color: rgb(227, 227, 227);"
//                "    border-height: 1px;"
//                "    border-style: solid; "
//                "    background-color: rgb(240, 240, 240); "
//                "    height: 10px; "
//                "    margin: 0px 21px 0 21px; "
//                "}"
//                ""
//                "QScrollBar::handle: horizontal {"
//                "background-color: rgb(200, 200, 200); "
//                "min-width: 25px; "
//                "}"
//                "QScrollBar::add-line: horizontal {"
//                "border: 1px solid grey;"
//                "background-color: rgb(241, 241, 241); "
//                "width: 20px; "
//                "subcontrol-position: right; "
//                "subcontrol-origin: margin; "
//                "}"
//                "QScrollBar::sub-line: horizontal {"
//                "border: 1px solid grey; "
//                "background-color: rgb(241, 241, 241); "
//                "width: 20px; "
//                "subcontrol-position: left; "
//                "subcontrol-origin: margin; "
//                "}"

                             )
                                                    );
        object->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        object->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    }

#endif
}



QStringList text_delimiter_decompose(QString text)
{
    text.replace('"', ' ');
    text.replace("'", " ");
    text.replace('.', ' ');
    text.replace(',', ' ');
    text.replace(';', ' ');
    text.replace(':', ' ');
    text.replace('-', ' ');
    text.replace('?', ' ');
    text.replace('!', ' ');

    QStringList list = text.split(QRegExp("\\W + "), QString::SkipEmptyParts);

    return list;
}


// Функция всегда возвращает уникальный идентификатор
QString get_unical_id(void)
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

    for(int i = 0; i < 10; i++)
        line += symbols.mid(rand() % symbols.length(), 1);

    QString result = secondsLine + line;

    return result;
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

    for(unsigned int i = 0; i < delay; i++) r = r + rand();

    seed1 = seed1 - get_milli_count() + r;

    unsigned int seed2 = time(nullptr);
    unsigned int seed3 = seed1 + seed2;
    unsigned int seed = seed3;

    srand(seed);
}




int main(int argc, char **argv)
{
    printf("\n\rStart MyTetra v. % d. % d. % d\n\r", APPLICATION_RELEASE_VERSION, APPLICATION_RELEASE_SUBVERSION, APPLICATION_RELEASE_MICROVERSION);

    Q_INIT_RESOURCE(data);              // added by hughvonyoung@gmail.com
    Q_INIT_RESOURCE(mytetra);

    // Начальные инициализации основных объектов

    // Запоминается имя файла запущенного бинарника
    // Файл запущенной программы (нулевой аргумент функции main)
    // Store the file name running binaries
    // File running program (a zero argument to main)
    QString mainProgramFile = QString::fromLatin1(argv[0]);             // Todo: This code must not work correctly with ways to UTF 8   // todo: Этот код наверно некорректно работает с путями в UTF8
    qDebug() << "Set main program file to " << mainProgramFile;
    globalparameters.main_program_file(mainProgramFile);

    // Перехват отладочных сообщений
    set_debug_message_handler();


    //    QtSingleApplication application(argc, argv);

    //    // Do not run another copy    // Не запущен ли другой экземпляр
    //    if(application.isRunning() || !application.isTheOnlyBrowser()) {
    //        QString message = "Another MyTetra exemplar is running.\n";

    //        printf(message.toLocal8Bit());

    //        QMessageBox msgBox;
    //        msgBox.setIcon(QMessageBox::Warning);
    //        msgBox.setText(message);
    //        msgBox.exec();

    //        exit(0);
    //    }

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

    // Установка увеличенного разрешения для дисплеев с большим DPI (Retina)
    // Set the increased resolution for displays with lots DPI (Retina)
    if(QtSingleApplication::instance()->devicePixelRatio() > 1.0)
        QtSingleApplication::instance()->setAttribute(Qt::AA_UseHighDpiPixmaps);

#endif

#if QT_VERSION < 0x050000
    // Установка кодека текстов
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF - 8"));
#endif

    //    // Инициализация глобальных параметров,
    //    // внутри происходит установка рабочей директории
    //    // Initialize global parameters,
    //    // Is being installed inside the working directory
    //    globalparameters.init();

    //    // Initialize the main program of configurable variables    // Инициализация основных конфигурирующих программу переменных
    //    appconfig.init();

    //    // Инициализация переменных, отвечающих за хранилище данных
    //    databaseconfig.init();

    //    QtSingleApplication application(argc, argv, globalparameters, appconfig, databaseconfig);

    //    // Do not run another copy    // Не запущен ли другой экземпляр
    //    if(application.isRunning()) {
    //        QString message = "Another MyTetra exemplar is running.\n";

    //        printf(message.toLocal8Bit());

    //        QMessageBox msgBox;
    //        msgBox.setIcon(QMessageBox::Warning);
    //        msgBox.setText(message);
    //        msgBox.exec();

    //        exit(0);
    //    }

    //    // Установка CSS-оформления
    //    setCssStyle();

    //    // Экран загрузки, показывается только в Андроид версии (так как загрузка идет ~10 сек, и без сплешскрина непонятно что происходит)
    //    QSplashScreen splash(QPixmap(": / resource / pic / mytetra_splash.png"));

    //    if(mytetraConfig.getShowSplashScreen())
    //        splash.show();


    //    //    // Подключение перевода интерфейса
    //    //    // QString langFileName=globalParameters.getWorkDirectory()+" / resource / translations / mytetra_"+mytetraconfig.get_interfacelanguage()+".qm";
    //    //    QString langFileName = ": / resource / translations / mytetra_" + mytetraConfig.get_interfacelanguage() + ".qm";
    //    //    qDebug() << "Use language file " << langFileName;

    //    //    //QTranslator langTranslator;
    //    //    //langTranslator.load(langFileName);
    //    //    application.installTranslator(
    //    //        langFileName    //&langTranslator
    //    //    );


    //    // Создание объекта главного окна
    //    MainWindow win;
    //    win.setWindowTitle("MyTetra");

    //    if(globalParameters.getTargetOs() == "android")
    //        win.show(); // В Андроиде нет десктопа, на нем нельзя сворачивать окно
    //    else {
    //        if(mytetraConfig.get_runinminimizedwindow() == false)
    //            win.show();
    //        else
    //            win.hide();
    //    }

    //    // win.setObjectName("mainwindow");
    //    // pMainWindow=&win; // Запоминается указатель на основное окно

    //    // После создания окна восстанавливается вид окна в предыдущий запуск
    //    // Эти действия нельзя делать в конструкторе главного окна,
    //    // т.к. окно еще не создано
    //    globalParameters.getWindowSwitcher()->disableSwitch();
    //    win.restoreFindOnBaseVisible();
    //    win.restoreGeometry();
    //    win.restoreTreePosition();
    //    win.restoreRecordTablePosition();
    //    win.restoreEditorCursorPosition();
    //    win.restoreEditorScrollBarPosition();
    //    globalParameters.getWindowSwitcher()->enableSwitch();

    //    if(mytetraConfig.getInterfaceMode() == "mobile")
    //        globalParameters.getWindowSwitcher()->restoreFocusWidget();

    //    qDebug() << "Restore session succesfull";

    //    // После восстановления последней редактируемой записи
    //    // история перехода очищается, так как в не может попасть
    //    // первая запись в востаналиваемой ветке и сама восстанавливаемая запись
    //    walkHistory.clear();


    //    // Если в конфиге настроено, что нужно синхронизироваться при старте
    //    // И задана команда синхронизации
    //    if(mytetraConfig.get_synchroonstartup())
    //        if(mytetraConfig.get_synchrocommand().trimmed().length() > 0)
    //            win.synchronization();


    //    // Если настроено в конфиге, сразу запрашивается пароль доступа
    //    // к зашифрованным данным
    //    // И если есть хоть какие-то зашифрованные данные
    //    if(mytetraConfig.get_howpassrequest() == "atStartProgram")
    //        if(globalParameters.getCryptKey().length() == 0)
    //            if(dataBaseConfig.get_crypt_mode() > 0) {
    //                // Запрашивается пароль только в том случае, если ветка,
    //                // на которую установливается курсор при старте, незашифрована
    //                // Если ветка зашифрована, пароль и так будет запрошен автоматически
    //                if(win.isTreePositionCrypt() == false) {
    //                    Password password;
    //                    password.retrievePassword();
    //                }
    //            }


    //    // Если в общем конфиге стоит опция хранения пароля
    //    // И хранимый пароль (точнее его хеш) заполнен
    //    if(globalParameters.getCryptKey().length() == 0)
    //        if(dataBaseConfig.get_crypt_mode() > 0)
    //            if(mytetraConfig.getPasswordSaveFlag())
    //                if(mytetraConfig.getPasswordMiddleHash().length() > 0) {
    //                    // При запросе пароля ключ шифрования будет восстановлен автоматически
    //                    Password password;
    //                    password.retrievePassword();
    //                }

    //    // Распечатывается дерево сгенерированных объектов
    //    // print_object_tree();

    //    // Проверяется наличие системного трея
    //    /*
    //    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
    //     QMessageBox::critical(0, QObject::tr("Systray"),
    //                           QObject::tr("I couldn't detect any system tray on this system."));
    //     exit(1);
    //    }
    //    */

    //    // При закрытии окна не выходить из программы.
    //    // Окно программы может быть снова открыто из трея
    //    QApplication::setQuitOnLastWindowClosed(false);


    //    // win.show();
    //    application.connect(&application, SIGNAL(lastWindowClosed()), &application, SLOT(quit()));

    //    // app.connect(&app, SIGNAL(app.commitDataRequest(QSessionManager)), SLOT(win.commitData(QSessionManager)));

    //    // Окно сплеш-скрина скрывается
    //    if(mytetraConfig.getShowSplashScreen())
    //        splash.finish(&win);

    return QtSingleApplication(argc, argv, globalparameters, appconfig, databaseconfig).exec();                 // application.exec();
}
