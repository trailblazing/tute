#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__

#include <QObject>
#include <QStatusBar>

//#ifndef QWEBENGINEPAGE_HITTESTCONTENT
//#define QWEBENGINEPAGE_HITTESTCONTENT
//#endif

namespace browser {
    class Entrance;
    class DownloadManager;
}

class TreeScreen;
class MetaEditor;
class RecordScreen;
class FindScreen;
class WindowSwitcher;
class MainWindow;
class QTabWidget;
class QSplitter;


class GlobalParameters : public QObject {
    Q_OBJECT

public:
    GlobalParameters(QObject *pobj = 0);
    virtual ~GlobalParameters();

    void main_program_file(QString file);
    QString main_program_file(void);

    void init(void);

    QString work_directory(void);

    QString target_os(void);
    QString application_name(void);

    void v_left_splitter(QSplitter *vleftsplitter);
    QSplitter *v_left_splitter();

    void vtab(QTabWidget *point);
    QTabWidget *vtab();

    void tree_screen(TreeScreen *point);
    TreeScreen *tree_screen();

    browser::Entrance *entrance();
    void entrance(browser::Entrance *&b);

    void table_screen(RecordScreen *point);
    RecordScreen *table_screen();

    void page_screen(RecordScreen *page);
    RecordScreen *page_screen();

    void find_screen(FindScreen *point);
    FindScreen *find_screen();

    void meta_editor(MetaEditor *point);
    MetaEditor *meta_editor();

    void status_bar(QStatusBar *point);
    QStatusBar *status_bar();

    void window_switcher(WindowSwitcher *point);
    WindowSwitcher *window_switcher();

    MainWindow *mainwindow() {return _mainwindow;}
    void mainwindow(MainWindow *mainwindow) {_mainwindow = mainwindow;}

    void crypt_key(QByteArray hash);
    QByteArray crypt_key(void);

    // Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    // Так как в более старых версиях MyTetra его еще небыло
    void create_stylesheet_file(QString dirName);

    void style_source(const QString &source) {_style_source = source;}
    QString style_source()const {return _style_source;}

    void download_manager(browser::DownloadManager *dm) {_download_manager = dm;}
    browser::DownloadManager *download_manager();
private:

    void init_workdirectory(void);
    bool find_workdirectory(void);
    bool is_mytetra_ini_config(QString fileName);
    void create_standard_programfiles(void);
    void create_portable_programfiles(void);
    void create_first_programfiles(QString dirName);

    TreeScreen          *_tree_screen;
    browser::Entrance   *_browsermanager;
    RecordScreen         *_table_screen;
    RecordScreen         *_page_screen;
    FindScreen          *_find_screen;
    MetaEditor          *_meta_editor;
    QStatusBar          *_statusbar;
    WindowSwitcher      *_window_switcher;
    QSplitter           *_v_left_splitter;
    QTabWidget          *_vtab;
    MainWindow          *_mainwindow;
    browser::DownloadManager *_download_manager;
    QString             _main_program_file;
    QString             _work_directory;

    QByteArray          _password_hash;
    QString             _style_source;
};

#endif  /* __GLOBALPARAMETERS_H__ */

