#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QWidget>
#include <QTextListFormat>
#include <QTextBlockFormat>

#include <QtXml>
#include <QtGui>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QList>
#include <QHBoxLayout>
#include <QSplitter>
#include <QSystemTrayIcon>

#include <QStatusBar>
#include "hidabletabwidget.h"

//#include "libraries/GlobalParameters.h"
//#include "models/app_config/AppConfig.h"
//#include "models/database_config/DataBaseConfig.h"


extern const char *meta_editor_singleton_name;

namespace browser {
    class Entrance;
    class Browser;
    class DownloadManager;
}

class TreeScreen;
class MetaEditor;
class RecordScreen;
class FindScreen;
class WindowSwitcher;
class QtSingleApplication;
class RecordController;
class GlobalParameters;
class AppConfig;
class DataBaseConfig;


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(GlobalParameters &globalparameters
               , AppConfig &appconfig
               , DataBaseConfig &databaseconfig);

    ~MainWindow();

    void restoreGeometry(void);
    void restore_tree_position(void);
    void restore_recordtable_position(void);
    void restoreEditorCursorPosition(void);
    void restoreEditorScrollBarPosition(void);
    void restoreFindOnBaseVisible(void);

    void set_tree_position(QStringList path);
    bool isTreePositionCrypt();

    void select_id(QString id);

    void synchronization(void);

    void goWalkHistoryPrevious(void);
    void goWalkHistoryNext(void);

    void saveTextarea(void);

    void saveAllState(void);
    HidableTabWidget *vtab() {return _vtabwidget;}

    QMenu *filemenu() {return _filemenu;}
    QMenu *toolsmenu() {return _toolsmenu;}
    QMenu *helpmenu() {return _helpmenu;}

public slots:
    void applicationExit(void);
    void applicationFastExit(void);
    void commitData(QSessionManager &manager);
    void editor_switch(void);

private slots:
    void fileNew(void);
    void fileOpen(void);
    bool fileSave(void);
    bool fileSaveAs(void);
    void filePrint(void);
    void filePrintPreview(void);
    void filePrintPdf(void);

    void toolsFind(void);
    //    void editor_switch(void);
    void toolsPreferences(void);

    void onExpandEditArea(bool flag);

    void onClickHelpAboutMyTetra(void);
    void onClickHelpAboutQt(void);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void onFocusChanged(QWidget *, QWidget *);

private:

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);

    void initFileMenu(void);
    void initToolsMenu(void);
    void initPreferencesMenu(QMenu *menu);
    void initHelpMenu(void);

    void initRecordTableActions(void);

    void setupIconActions(void);
    void createTrayIcon(void);
    void setIcon(void);

    void saveGeometry(void);
    void save_tree_position(void);
    void save_recordtable_position(void);
    void saveEditorCursorPosition(void);
    void saveEditorScrollBarPosition(void);

    GlobalParameters    &_globalparameters;
    AppConfig           &_appconfig;
    DataBaseConfig      &_databaseconfig;



    QAction *actionTrayRestore;
    QAction *actionTrayMaximize;
    QAction *actionTrayMinimize;
    QAction *actionTrayQuit;

    QSystemTrayIcon *trayIcon;
    QMenu           *trayIconMenu;

    QSplitter           *_v_right_splitter;
    QSplitter           *_find_splitter;
    HidableTabWidget    *_vtabwidget;
    QSplitter           *_v_left_splitter;
    QSplitter           *_h_splitter;


    QMenu *_filemenu;
    QMenu *_toolsmenu;
    QMenu *_helpmenu;

    TreeScreen          *_tree_screen;
    RecordScreen         *_table_screen;
    //    TableScreen         *_page_screen;
    browser::DownloadManager *_download;
    FindScreen          *_find_screen;
    MetaEditor          *_editor_screen;
    QStatusBar          *_statusbar;
    WindowSwitcher      *_switcher;
    RecordController     *_record_controller;
    //    TableController     *_page_controller;
    browser::Entrance   *_entrance;

    //    bool _treetable_hidden;       // = globalparameters.getTreeScreen()->isHidden();
    //    bool _recordtable_hidden;     // = globalparameters.getRecordTableScreen()->isHidden();

protected:



    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *o, QEvent *e);     // Отслеживание прочих событий

    void goWalkHistory(void);

    bool _enable_real_close;

    friend class browser::Browser;

};
#endif
