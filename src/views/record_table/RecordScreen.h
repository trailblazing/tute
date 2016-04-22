#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_

#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QItemSelection>
#include <QLabel>
//#include "models/tree/TreeItem.h"


class RecordController;
class VerticalScrollArea;
class FlatToolButton;
class TreeItem;
class TreeScreen;
class FindScreen;
class MetaEditor;
class MainWindow;
class AppConfig;
class HidableTabWidget;



namespace browser {
    class Entrance;
    class Browser;
    class ToolbarSearch;
    class TabWidget;
}

class RecordScreen : public QWidget {
    Q_OBJECT

public:
    RecordScreen(TreeScreen *_tree_screen, FindScreen *_find_screen, MetaEditor *_editor_screen, browser::Entrance *_entrance, browser::Browser *_browser, HidableTabWidget *_vtabwidget, MainWindow *_main_window);

    virtual ~RecordScreen();

    int     first_selection_pos(void);
    QString first_selection_id(void);
    void    select_pos(int pos);
    void    select_id(QString id);

    void    tree_path(QString path);
    QString tree_path(void);

    inline RecordController *record_controller() {return _record_controller;}

    bool                inited() {return _inited;}
    browser::TabWidget  *tabmanager() {return _tabmanager;}
    browser::Browser    *browser() {return _browser;}

public slots:

    // Обновление панели инструментов
    void tools_update(void);


protected:
    void resizeEvent(QResizeEvent *e);
private slots:

    void on_syncro_click(void);
    void on_walkhistory_previous_click(void);
    void on_walkhistory_next_click(void);
    void on_back_click(void);

private:
    bool                _inited = false;

    // The steps used on the toolbar and in the context menu entries    // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
    QAction             *_save_in_new_branch;
    QAction             *_pin;
    QAction             *_addnew_to_end;
    QAction             *_addnew_before;
    QAction             *_addnew_after;
    QAction             *_edit_field;
    QAction             *_delete;
    QAction             *_cut;
    QAction             *_copy;
    QAction             *_paste;
    QAction             *_settings;
    QAction             *_action_move_up;
    QAction             *_action_move_dn;
    QAction             *_find_in_base;
    QAction             *_action_syncro;
    QAction             *_action_walk_history_previous;
    QAction             *_action_walk_history_next;
    QAction             *_back;
    QAction             *_sort;
    QAction             *_print;

    QToolBar            *_toolsline;
    QToolBar            *_extra_toolsline;

    QLabel              *_treepathlabel;
    //    FlatToolButton  *_treepath_button;
    QString             _treepath;

    browser::TabWidget  *_tabmanager;
    RecordController    *_record_controller;

    VerticalScrollArea  *_vertical_scrollarea;


    QHBoxLayout         *_records_toolslayout;
    //    browser::ToolbarSearch  *_recordtree_search;
    //    QHBoxLayout             *_recordtree_searchlayout;
    QVBoxLayout         *_records_screenlayout;

    browser::Browser    *_browser;
    MainWindow          *_main_window;

    void setup_ui(void);
    void setup_signals(void);
    void setup_actions(void);
    void assembly(void);

    void disable_all_actions(void);
    void save_in_new_branch(bool checked = false);
    friend class RecordView;
    friend class RecordController;
};

#endif /* RECORDTABLESCREEN_H_ */
