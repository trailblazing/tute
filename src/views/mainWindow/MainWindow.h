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
//#include "models/appConfig/AppConfig.h"
//#include "models/dataBaseConfig/DataBaseConfig.h"




namespace browser {
    class Entrance;
    class Browser;
}

class TreeScreen;
class MetaEditor;
class TableScreen;
class FindScreen;
class WindowSwitcher;
class QtSingleApplication;
class TableController;
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

    TreeScreen          *treeScreen;
    TableScreen   *recordTableScreen;
    MetaEditor          *editorScreen;
    FindScreen          *findScreenDisp;
    QStatusBar          *statusBar;
    WindowSwitcher      *windowSwitcher;
    browser::Entrance   *browser_entrance;

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

    void setupUI(void);
    void setupSignals(void);
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

    TableController   *_recordtablecontroller;

    QAction *actionTrayRestore;
    QAction *actionTrayMaximize;
    QAction *actionTrayMinimize;
    QAction *actionTrayQuit;

    QSystemTrayIcon *trayIcon;
    QMenu           *trayIconMenu;

    QSplitter   *v_right_splitter;
    QSplitter   *find_splitter;
    HidableTabWidget  *_vtabwidget;
    QSplitter   *v_left_splitter;
    QSplitter   *hSplitter;


    QMenu *_filemenu;
    QMenu *_toolsmenu;
    QMenu *_helpmenu;

    //    bool _treetable_hidden;       // = globalparameters.getTreeScreen()->isHidden();
    //    bool _recordtable_hidden;     // = globalparameters.getRecordTableScreen()->isHidden();

protected:



    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *o, QEvent *e);     // Отслеживание прочих событий

    void goWalkHistory(void);

    bool enableRealClose;

    friend class browser::Browser;

};
#endif
