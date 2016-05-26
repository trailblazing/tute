
#include <string>
#include <QString>
#include <QDir>
#include <QString>
#include <QDesktopWidget>
#include <QStackedWidget>

#include "main.h"
#include "models/app_config/AppConfig.h"
#include "views/browser/entrance.h"
#include "MainWindow.h"
#include "views/print_preview/PrintPreview.h"
#include "views/app_config/AppConfigDialog.h"
#include "libraries/TrashMonitoring.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/record_table/RecordScreen.h"
#include "models/tree/TreeItem.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "models/record_table/linker.hxx"
#include "models/tree/treeindex.hxx"
#include "models/tree/KnowModel.h"
#include "libraries/GlobalParameters.h"
#include "views/console_emulator/ExecuteCommand.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"

#include "libraries/wyedit/EditorTextArea.h"
#include "views/browser/browser.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "views/browser/downloadmanager.h"
#include "views/browser/history.h"
#include "views/browser/bookmarks.h"
#include "views/tree/KnowView.h"



extern AppConfig appconfig;
extern TrashMonitoring trashmonitoring;
extern GlobalParameters globalparameters;
extern WalkHistory walkhistory;
const char *meta_editor_singleton_name = "editor_screen";
const char *record_screen_multi_instance_name = "record_screen";
const char *tree_screen_singleton_name = "tree_screen";
const char *find_screen_singleton_name = "find_screen";
const char *download_manager_singleton_name = "download_manager";
const char *windowswitcher_singleton_name = "window_switcher";
const char *entrance_singleton_name = "entrance";
const char *record_controller_multi_instance_name = "record_controller";   // std::string(std::string(table_screen_singleton_name) + std::string("_controller")).c_str();
const char *record_view_multi_instance_name = "record_view";
extern const char *tree_screen_viewer_name;
extern const char *action_find_in_base;

MainWindow::MainWindow(GlobalParameters    &_globalparameters
    , AppConfig         &_appconfig
    , DataBaseConfig    &_databaseconfig
    , browser::Profile *_profile)
    : QMainWindow()
    , _globalparameters(_globalparameters)
    , _appconfig(_appconfig)
    , _databaseconfig(_databaseconfig)
    , _v_right_splitter(new QSplitter(Qt::Vertical))
    , _v_find_splitter(new QSplitter(Qt::Vertical))
    , _vtab_record(new HidableTabWidget(this))
    , _vtab_tree(new HidableTabWidget(this))
    , _h_right_splitter(new QSplitter(Qt::Horizontal))
    , _h_left_splitter(new QSplitter(Qt::Horizontal))      // Qt::Vertical
    , _h_splitter(new QSplitter(Qt::Horizontal))
    , _filemenu(new QMenu(tr("&File"), this))
    , _editmenu(new QMenu(tr("&Edit"), this))
    , _viewmenu(new QMenu(tr("&View"), this))
    , _histrymenu(new browser::HistoryMenu(this))
    , _bookmarkmenu(new browser::BookmarksMenu(this))
    , _windowmenu(new QMenu(tr("&Window"), this))
    , _toolsmenu(new QMenu(tr("&Tools"), this))
    , _helpmenu(new QMenu(tr("&Help"), this))
    , _tree_screen(new TreeScreen(tree_screen_singleton_name, _appconfig, _filemenu, _toolsmenu, this))    // _vtabwidget
    , _find_screen(new FindScreen(find_screen_singleton_name, _tree_screen, this))
    , _editor_screen(new MetaEditor(meta_editor_singleton_name, _find_screen))    // _find_screen -> for find_text
    , _entrance(new browser::Entrance(entrance_singleton_name, _tree_screen, _find_screen, _editor_screen, this, _appconfig, _globalparameters.style_source(), _profile, Qt::Widget))                                // Qt::MaximizeUsingFullscreenGeometryHint
    , _download(new browser::DownloadManager(download_manager_singleton_name, _vtab_tree))
    , _statusbar(new QStatusBar(this))
    , _switcher(new WindowSwitcher(windowswitcher_singleton_name, _editor_screen, this))
    , _enable_real_close(false)
{
    //    _page_screen->setVisible(false);
    //    _page_screen->hide();

    _globalparameters.mainwindow(this);



    //    int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 100
    //    int vtab_g_width = _vtabwidget->geometry().width();         // 100
    //    int this_width = geometry().width();                        // 640
    //    int download_width = _download->geometry().width();         // 1089
    //    int tree_screen_width = _tree_screen->geometry().width();   // 100
    //    int find_splitter_width = _v_find_splitter->geometry().width(); // 640
    //    int left_splitter_width = _h_left_splitter->geometry().width(); // 640

    //    if(_vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()) {
    //        _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100, _vtabwidget->geometry().height());
    //    }


    _v_right_splitter->setHandleWidth(0);
    _v_find_splitter->setHandleWidth(0);
    _h_left_splitter->setHandleWidth(0);
//    int wl = _h_left_splitter->width(); // 640
//    int wr = _h_right_splitter->width();// 640   // default handleWidth() == 6
    _h_right_splitter->setHandleWidth(0);
    _h_splitter->setHandleWidth(0);

    _vtab_record->tabBar()->hide();
    _globalparameters.vtab_record(_vtab_record);
    _globalparameters.vtab_tree(_vtab_tree);

    extern QObject *mainwindow;
    mainwindow = this;
    setObjectName("mainwindow");

    installEventFilter(this);

    init_file_menu();

    append_quit_menu();

    init_tools_menu();
    //    initHelpMenu();
    QMainWindow::menuBar()->hide();

    setup_ui();
    setup_signals();
    assembly();

    //    initFileMenu();
    //    initToolsMenu();
    //    initHelpMenu();

    setup_icon_actions();
    create_tray_icon();
    set_icon();

    if(QSystemTrayIcon::isSystemTrayAvailable())
        _tray_icon->show();

    //    // Инициализируется объект слежения за корзиной
    //    trashmonitoring.init(_appconfig.get_trashdir());
    //    trashmonitoring.update();

    // Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
    _enable_real_close = false;

    // Инициализация генератора случайных чисел
    init_random();


}

void MainWindow::append_quit_menu()
{
    QAction *quit = new QAction(tr("&Quit"), this);
    quit->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(quit, &QAction::triggered, this, &MainWindow::application_exit);
    _filemenu->addAction(quit);
}

MainWindow::~MainWindow()
{
    save_all_state();

    _entrance->deleteLater(); _entrance = nullptr;
    // delete
    _switcher->deleteLater();
    // delete
    _statusbar->deleteLater();
    // delete
    _editor_screen->deleteLater();
    // delete
    _find_screen->deleteLater();

    //    delete  _download;

    //    if(_page_screen)delete  _page_screen;

    //    delete  _table_screen;

    if(_tree_screen) {
        // delete
        _tree_screen->deleteLater();
        _tree_screen = nullptr;
    }

    //    delete  _vtabwidget;

    // delete
    _v_right_splitter->deleteLater();
    // delete
    _v_find_splitter->deleteLater();
    //    delete  _vtabwidget;
    // delete
    _h_right_splitter->deleteLater();
    // delete
    _h_left_splitter->deleteLater();
    // delete
    _h_splitter->deleteLater();

    // delete
    _filemenu->deleteLater();
    // delete
    _editmenu->deleteLater();
    // delete
    _viewmenu->deleteLater();
    // delete
    _histrymenu->deleteLater();
    // delete
    _bookmarkmenu->deleteLater();
    // delete
    _windowmenu->deleteLater();
    // delete
    _toolsmenu->deleteLater();
    // delete
    _helpmenu->deleteLater();

}


void MainWindow::setup_ui(void)
{
    // При создании объектов не указывается parent, так как он буден задан в момент вставки в layout в методе assembly()

    //    _tree_screen = new TreeScreen(_appconfig, this);
    //    _tree_screen->setObjectName(tree_screen_singleton_name);  // "treeScreen"
    _globalparameters.tree_screen(_tree_screen);
    //    _treetable_hidden = treeScreen->isHidden();
    //    connect(treeScreen, &TreeScreen::hide, this, [this]() {_treetable_hidden = true;});
    //    connect(treeScreen, &TreeScreen::show, this, [this]() {_treetable_hidden = false;});

    //    _table_screen = new TableScreen(this);
    //    _table_screen->setObjectName(table_screen_singleton_name); // "recordTableScreen"


    //    _globalparameters.push_record_screen(_table_screen);


    //    _recordtable_hidden = recordTableScreen->isHidden();
    //    connect(recordTableScreen, &RecordTableScreen::hide, this, [this]() {_recordtable_hidden = true;});
    //    connect(recordTableScreen, &RecordTableScreen::show, this, [this]() {_recordtable_hidden = false;});

    //    _page_screen = new TableScreen(this);
    //    _page_screen->setObjectName("page_screen");
    //    _globalparameters.page_screen(_page_screen);

    //    _download = new browser::DownloadManager(this);
    //    _download->setObjectName(download_manager_singleton_name);
    _globalparameters.download_manager(_download);


    //    _editor_screen = new MetaEditor();
    //    _editor_screen->setObjectName(meta_editor_singleton_name);  // "editorScreen"
    _globalparameters.meta_editor(_editor_screen);


    //    _find_screen = new FindScreen(this);
    //    _find_screen->setObjectName(find_screen_singleton_name);  // "findScreenDisp"
    _globalparameters.find_screen(_find_screen);
    //findScreenDisp->hide();



    if(!_appconfig.editor_show())
        _editor_screen->hide();

    //    _statusbar = new QStatusBar(this);
    _statusbar->setObjectName("status_bar"); // "statusBar"
    setStatusBar(_statusbar);
    _globalparameters.status_bar(_statusbar);

    // Вспомогательный объект переключения окон, используется в мобильном интерфейсе
    //    _switcher = new WindowSwitcher(this);
    //    _switcher->setObjectName(windowswitcher_singleton_name); // "windowSwitcher"
    _globalparameters.window_switcher(_switcher);
    //    windowSwitcher->findInBaseClick();

    //    if(_table_screen) {
    //        _entrance = new browser::Entrance(
    //            _record_ontroller
    //            = _table_screen->getRecordTableController()
    //              , _globalparameters.style_source()
    //            , this
    //            , Qt::Widget  // Qt::MaximizeUsingFullscreenGeometryHint
    //        );
    //        //    browsermanager->adjustSize();
    _entrance->set_scrollbars(true);
    //    _entrance->setObjectName(entrance_singleton_name);
    _globalparameters.entrance(_entrance);
    //    }

    // todo: Для проверки, почему то в этом месте поиск объекта по имени не работает, разобраться.
    // MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name);
}


void MainWindow::setup_signals(void)
{
    connect(_editor_screen, &MetaEditor::send_expand_edit_area, this, &MainWindow::on_expand_edit_area);

    // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
    connect(qApp, &QApplication::commitDataRequest, this, &MainWindow::commit_data);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::on_focus_changed);

    // Связывание сигналов кнопки поиска по базе с действием по открытию виджета поиска по базе
    connect(_tree_screen->_actionlist[action_find_in_base], &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);
    //    connect(_entrance->getactionFreeze(), &QAction::triggered, globalparameters.getWindowSwitcher(), &WindowSwitcher::findInBaseClick);
    //    connect(_table_screen->_find_in_base, &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

    // if(_page_screen)connect(_page_screen->actionFindInBase, &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

    connect(_editor_screen, &MetaEditor::wyedit_find_in_base_clicked, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

    //    auto hide_others = [this](int index) {
    //        if(-1 != index) {
    //            auto count = _vtabwidget->count();

    //            for(int i = 0; i < count; i++) {
    //                if(i != index)_vtabwidget->widget(i)->hide();
    //            }

    //            _vtabwidget->widget(index)->show();
    //        }
    //    };


    connect(_vtab_record, &HidableTabWidget::currentChanged, [&] (int index) {
        if(-1 != index) {
            auto record_widget = _vtab_record->widget(index);

            //                for(int i = 0; i < _vtab_record->count(); i++) {
            //                    if(i != real_index) {
            //                        auto current_widget = _vtab_record->widget(i);
            //                        if(current_widget) {
            //                            if(current_widget->objectName() == record_screen_multi_instance_name) {
            //                                auto record_screen = static_cast<RecordScreen *>(current_widget);
            //                                record_screen->browser()->lower();
            //                            }

            //                            current_widget->hide();
            //                        }
            //                    }
            //                }
            RecordScreen *record_screen = nullptr;
            if(record_widget->objectName() == record_screen_multi_instance_name)
                record_screen = dynamic_cast<RecordScreen *>(record_widget);
            if(record_screen) {
                _vtab_record->setCurrentWidget(record_screen);
                record_screen->restore_menubar();
                auto current_brower = record_screen->browser();
                if(current_brower && !current_brower->is_under_construction()) {
                    current_brower->raise();
                    current_brower->activateWindow();
                }

                for(int i = 0; i < _vtab_tree->count(); i++) {
                    auto tree_view_curr = _vtab_tree->widget(i);
                    if(tree_view_curr->objectName() == tree_screen_viewer_name) {
                        auto tree_screen_viewer = dynamic_cast<TreeScreenViewer *>(tree_view_curr);
                        if(tree_screen_viewer ) {
                            if(tree_screen_viewer->record_screen() == record_screen) {
                                if(_vtab_tree->currentIndex() != i) {

                                    if(!tree_screen_viewer->tree_screen()) {
                                        tree_screen_viewer->tree_screen(_tree_screen);
                                    }

                                    _vtab_tree->setCurrentIndex(i);
                                }
                            }
                        }
                    }
                }
            }
            //                    else{
            //                        _tree_screen->restore_menubar();
            //                    }




        }
    });

    //    hide_others(_vtabwidget->currentIndex());
    connect(_vtab_tree, &HidableTabWidget::currentChanged, [&] (int index) {
        if(-1 != index) {
            int tree_viewer_count = 0;
            for(int i = 0; i < _vtab_tree->count(); i++) {
                if(_vtab_tree->widget(i)->objectName() == tree_screen_viewer_name) tree_viewer_count++;
            }
//            auto count = _vtab_tree->count();
            if(0 == tree_viewer_count) {// if(1 >= count) {    // self and download
                _tree_screen->restore_menubar();
                if(_tree_screen->isHidden()) {
                    _tree_screen->show();
                }
            }


            //            int tree_viewer_count = 0;
            //            for(int i = 0; i < _vtab_tree->count(); i++) {
            //                if(_vtab_tree->widget(i)->objectName() == tree_screen_viewer_name) tree_viewer_count++;
            //            }

            //            int real_index = index - (_vtab_tree->count() - tree_viewer_count);

            auto tree_view_curr = _vtab_tree->widget(index);
            if(tree_view_curr->objectName() == tree_screen_viewer_name) {                    // if(real_index < _vtab_record->count()) {
                //                for(int i = 0; i < _vtab_record->count(); i++) {
                //                    if(i != real_index) {
                //                        auto current_widget = _vtab_record->widget(i);
                //                        if(current_widget) {
                //                            if(current_widget->objectName() == record_screen_multi_instance_name) {
                //                                auto record_screen = static_cast<RecordScreen *>(current_widget);
                //                                record_screen->browser()->lower();
                //                            }

                //                            current_widget->hide();
                //                        }
                //                    }
                //                }

                auto current_tree_screen_viewer = dynamic_cast<TreeScreenViewer *>(tree_view_curr);              // auto current_widget = _vtab_record->widget(real_index);
                if(current_tree_screen_viewer) {
                    RecordScreen *record_screen = current_tree_screen_viewer->record_screen();
                    if(record_screen) {
                        if(_vtab_record->currentWidget() != record_screen) _vtab_record->setCurrentWidget(record_screen);
                        record_screen->restore_menubar();
                        auto current_brower = record_screen->browser();
                        if(current_brower && !current_brower->is_under_construction()) {
                            current_brower->raise();
                            current_brower->activateWindow();
                        }
                    }
                    //                    else{
                    //                        _tree_screen->restore_menubar();
                    //                    }
                }
            }

//            // deprecated: ignoring Tree Search Area
//            if(_vtab_tree->widget(index)->objectName() == tree_screen_singleton_name) {
//                _appconfig.find_screen_tree_search_area(0);
//            } else if(_vtab_tree->widget(index)->objectName() == record_screen_multi_instance_name) {
//                _appconfig.find_screen_tree_search_area(1);
//            }

        }

    });

    connect(_vtab_tree, &HidableTabWidget::tabBarClicked, _vtab_record, [&] (int index) {
//        if(1 <= index) {

//            int tree_viewer_count = 0;
//            for(int i = 0; i < _vtab_tree->count(); i++) {
//                if(_vtab_tree->widget(i)->objectName() == tree_screen_viewer_name) tree_viewer_count++;
//            }

//            int real_index = index - (_vtab_tree->count() - tree_viewer_count);

        auto tree_curr = _vtab_tree->widget(index);
        if(tree_curr->objectName() == tree_screen_viewer_name) {        // if(real_index < _vtab_record->count()) {

            auto current_tree_screen_viewer = dynamic_cast<TreeScreenViewer *>(tree_curr);      // _vtab_record->widget(real_index);
            if(current_tree_screen_viewer) {
                RecordScreen *record_screen = current_tree_screen_viewer->record_screen();
                QWidget *tree_screen = current_tree_screen_viewer->tree_screen();
                if(!tree_screen) {
                    current_tree_screen_viewer->tree_screen(_tree_screen);
                }
                if(record_screen) {     // curr_record_screen->objectName() == record_screen_multi_instance_name
//                        auto record_screen = static_cast<RecordScreen *>(curr_record_screen);

                    _vtab_record->setCurrentWidget(record_screen);
                    record_screen->restore_menubar();
                    auto current_brower = record_screen->browser();
                    if(current_brower && !current_brower->is_under_construction()) {
                        current_brower->raise();
                        current_brower->activateWindow();
                    }
                }
//                    else{
//                        _tree_screen->restore_menubar();
//                    }

//                    current_tree_screen_viewer->show();
            }
        }

//        }


    });



//    // deprecated: ignoring Tree Search Area
//    connect(_vtab_tree, &QTabWidget::currentChanged,  &_appconfig
//        , [this] (int index) {
//        if(-1 != index) {
//            if(_vtab_tree->widget(index)->objectName() == tree_screen_singleton_name) {
//                _appconfig.find_screen_tree_search_area(0);
//            } else if(_vtab_tree->widget(index)->objectName() == record_screen_multi_instance_name) {
//                _appconfig.find_screen_tree_search_area(1);
//            }
//        }
//    }   // &AppConfig::setFindScreenTreeSearchArea
//        );      // , findScreenDisp, &FindScreen::changedTreeSearchArea


}


void MainWindow::assembly(void)
{
    //    v_right_splitter = new QSplitter(Qt::Vertical);
    _v_right_splitter->addWidget(_entrance);
    _v_right_splitter->addWidget(_editor_screen);           // Text entries // Текст записи
    _v_right_splitter->setCollapsible(0, true);             // if true, make editor can overload it    // The list of final entries can not link up    // Список конечных записей не может смыкаться
    _v_right_splitter->setCollapsible(1, false);             // The contents of the recording can not link up    // Содержимое записи не может смыкаться
    _v_right_splitter->setObjectName("v_right_splitter");

    //    find_splitter = new QSplitter(Qt::Vertical);
    _v_find_splitter->addWidget(_v_right_splitter);         // findSplitter->addWidget(hSplitter);
    _v_find_splitter->addWidget(_find_screen);
    _v_find_splitter->setCollapsible(0, false);             // Верхняя часть не должна смыкаться
    _v_find_splitter->setCollapsible(1, false);             // Часть для поиска не должна смыкаться
    _v_find_splitter->setObjectName("find_splitter");

    //    _qtabwidget = new QTabWidget(this);

    _vtab_tree->setTabPosition(QTabWidget::West);          // sometime make "QModelIndex TreeModel::parent(const QModelIndex &index) const" failed.





    //    _vtabwidget->addTab(_tree_screen, QIcon(":/resource/pic/leaves.svg"), "Tree");




    //    _vtabwidget->addTab(_table_screen, QIcon(":/resource/pic/clover.svg"), "Candidate");

    // if(_page_screen)_vtabwidget->addTab(_page_screen, QIcon(":/resource/pic/three_leaves_clover.svg"), "Page");

    _vtab_tree->addTab(static_cast<QWidget *>(_download), QIcon(":/resource/pic/apple.svg"), "Download");                    // QIcon(":/resource/pic/holly.svg")
    auto index = _vtab_tree->addTab(static_cast<QWidget *>(new TreeScreenViewer(_tree_screen, nullptr)), QIcon(":/resource/pic/apple.svg"), "Browser");
    _vtab_tree->setCurrentIndex(index);


    _appconfig.find_screen_tree_search_area(0);  // force to root_item of global tree




    //    v_left_splitter = new QSplitter(
    //        Qt::Horizontal  // Qt::Vertical
    //    );




    //    int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 100
    //    int vtab_g_width = _vtabwidget->geometry().width();         // 100
    //    int this_width = geometry().width();                        // 640
    //    int download_width = _download->geometry().width();         // 1089
    //    int tree_screen_width = _tree_screen->geometry().width();   // 100
    //    int find_splitter_width = _v_find_splitter->geometry().width(); // 640
    //    int left_splitter_width = _h_left_splitter->geometry().width(); // 640

    //    if(_vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()) {
    //        _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100, _vtabwidget->geometry().height());
    //    }

    _h_right_splitter->addWidget(_vtab_record);
    _h_right_splitter->addWidget(_v_find_splitter);
    globalparameters.h_right_splitter(_h_right_splitter);

    _h_left_splitter->addWidget(_vtab_tree);
    _h_left_splitter->addWidget(_h_right_splitter);



    //    v_left_splitter->addWidget(treeScreen);
    //    v_left_splitter->addWidget(recordTableScreen);
    _h_left_splitter->setCollapsible(0, true);
    _h_left_splitter->setCollapsible(1, false);
    //    v_left_splitter->setCollapsible(1, false);
    _h_left_splitter->setObjectName("v_left_splitter");

    globalparameters.find_splitter(_v_find_splitter);
    globalparameters.v_left_splitter(_h_left_splitter);
    globalparameters.v_right_splitter(_v_right_splitter);




//    _h_splitter->addWidget(_tree_screen);



    //    hSplitter = new QSplitter(Qt::Horizontal);
    _h_splitter->addWidget(_h_left_splitter);
    //hSplitter->addWidget(treeScreen);             // Tree branches    // Дерево веток
    //hSplitter->addWidget(recordTableScreen);      // The list of final entries    // Список конечных записей



    //    _h_splitter->addWidget(_find_splitter);             //hSplitter->addWidget(vSplitter);




    _h_splitter->setCollapsible(0, false);            // Дерево веток не может смыкаться
    _h_splitter->setCollapsible(1, false);            // Столбец со списком и содержимым записи не может смыкаться
    _h_splitter->setObjectName("hsplitter");

    //    connect(find_splitter, &QSplitter::splitterMoved, browser_entrance, &browser::Entrance::on_splitter_moved);
    //    connect(find_splitter, &QSplitter::splitterMoved, recordTableScreen, &RecordTableScreen::on_splitter_moved);

    //    findSplitter=new QSplitter(Qt::Vertical);
    //    findSplitter->addWidget(hSplitter);
    //    findSplitter->addWidget(findScreenDisp);
    //    findSplitter->setCollapsible(0,false);        // Верхняя часть не должна смыкаться
    //    findSplitter->setCollapsible(1,false);        // Часть для поиска не должна смыкаться
    //    findSplitter->setObjectName("find_splitter");

    setCentralWidget(_h_left_splitter); //    setCentralWidget(_h_splitter);                    //setCentralWidget(findSplitter);

}


void MainWindow::save_all_state(void)
{
    // Сохранение данных в поле редактирования
    save_text_area();

    // Сохраняются данные сеанса работы
    save_geometry();
    save_tree_position();
    //    save_recordtable_position();
    save_editor_cursor_position();
    save_editor_scrollbar_position();

    // Синхронизируется с диском конфиг программы
    appconfig.sync();
}


// Слот, срабатывающий когда происходит выход из оконной системы
void MainWindow::commit_data(QSessionManager &manager)
{
    Q_UNUSED(manager);
    qDebug() << "Session manager send commit data signal.";

    application_fast_exit();
}


// Восстанавливается геометрия окна и позиции основных разделителей
void MainWindow::restore_geometry(void)
{
    if(globalparameters.target_os() == "android")
        setWindowState(Qt::WindowMaximized); // Для Андроида окно просто разворачивается на весь экран
    else {
        QRect rect = appconfig.mainwin_geometry();
        resize(rect.size());
        move(rect.topLeft());
    }

    // move(rect.topLeft());
    // resize(rect.size());


    _v_right_splitter->setSizes(appconfig.vspl_sizelist());
    _h_splitter->setSizes(appconfig.hspl_sizelist());
    _h_right_splitter->setSizes(appconfig.h_right_splitter_sizelist());
    _h_left_splitter->setSizes(appconfig.h_left_splitter_sizelist());
    _v_find_splitter->setSizes(appconfig.findsplitter_sizelist());

    //    int vtab_base = _vtabwidget->baseSize().width();            // 0
    //    int vtab_frame = _vtabwidget->frameSize().width();          // 1118
    //    int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 1118
    int vtab_g_width = _vtab_tree->geometry().width();         // 1118
    //    int this_width = geometry().width();                        // 1366
    //    int download_width = _download->geometry().width();         // 1089
    //    int tree_screen_width = _tree_screen->geometry().width();   // 236
    //    int find_splitter_width = _v_find_splitter->geometry().width(); // 0
    //    int left_splitter_size_width = _h_left_splitter->size().width();            // 1124
    //    int left_splitter_geometry_width = _h_left_splitter->geometry().width();    // 1124
    //    int right_splitter_width = _h_right_splitter->geometry().width();           // 0

    //    if(0 == find_splitter_width // _vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()
    //      ) {
    //        _download->resize(vtab_g_width * 15 / 100, _vtabwidget->geometry().height());
    //        _vtabwidget->resize(vtab_g_width * 15 / 100, _vtabwidget->geometry().height());
    //        _entrance->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
    //        _editor_screen->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
    //        _find_screen->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
    //        _v_find_splitter->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());   //
    //        _h_right_splitter->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());   //

    //    }

    //    download_width = _download->geometry().width();         // 1089
    //    vtab_g_width = _vtabwidget->geometry().width();                 // 167
    //    find_splitter_width = _v_find_splitter->geometry().width();     // 950
    //    right_splitter_width = _h_right_splitter->geometry().width();   // 950
    //    _h_right_splitter->move(_h_right_splitter->x() - vtab_g_width * 85 / 100, _h_right_splitter->y());


    //    _h_right_splitter->repaint();
    //    _h_left_splitter->repaint();
    auto sizes = _h_left_splitter->sizes();
    QList<int> new_sizes;

    for(auto sz : sizes) {
        if(sz == 0) {
            sz = vtab_g_width * 85 / 100;
        } else if(sz == vtab_g_width) {
            sz = vtab_g_width * 15 / 100;
        }

        new_sizes << sz;
    }

    _h_left_splitter->setSizes(new_sizes);

    //    _v_right_splitter->setSizes(appconfig.vspl_sizelist());
    //    _h_splitter->setSizes(appconfig.hspl_sizelist());
    //    _h_left_splitter->setSizes(appconfig.v_leftsplitter_sizelist());
    //    _v_find_splitter->setSizes(appconfig.findsplitter_sizelist());
    //    this->repaint();
}


// Запоминается геометрия окна и позиции основных разделителей
void MainWindow::save_geometry(void)
{
    qDebug() << "Save window geometry and splitter sizes";

    QRect geom(pos(), size());

    appconfig.mainwin_geometry(geom.x(), geom.y(),
        geom.width(), geom.height());

    // mytetraconfig.set_mainwingeometry(geometry().x(), geometry().y(),
    //                                   geometry().width(), geometry().height());

    appconfig.vspl_sizelist(_v_right_splitter->sizes());
    appconfig.hspl_sizelist(_h_splitter->sizes());
    appconfig.h_right_splitter_sizelist(_h_right_splitter->sizes());
    appconfig.h_left_splitter_sizelist(_h_left_splitter->sizes());

    // Запоминается размер сплиттера только при видимом виджете поиска,
    // т.к. если виджета поиска невидно, будет запомнен нуливой размер

    // if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
    // данный метод вызывается из декструктора главного окна, и к этому моменту
    // виджет уже невиден

    if(appconfig.findscreen_show())
        appconfig.findsplitter_sizelist(_v_find_splitter->sizes());
}


void MainWindow::restore_tree_position(void)
{
    // Путь к последнему выбранному в дереве элементу
    auto pair = appconfig.tree_position();
    QString current_root_id = pair.first;
    QStringList current_item_absolute_path = pair.second;//appconfig.get_tree_position();

    qDebug() << "MainWindow::restoreTreePosition() : " << current_item_absolute_path;

    set_tree_position(current_root_id, current_item_absolute_path);
}

// save
void MainWindow::save_tree_position(void)
{
    auto _current_source_model = [&] () {
            return _tree_screen->tree_view()->source_model();
        };
    //    if(!_tree_screen->sysynchronized())_tree_screen->synchronize();
    auto item = _tree_screen->tree_view()->session_root_auto();   //item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == _tree_screen->session_root_id();});
    //    // Получение QModelIndex выделенного в дереве элемента
    //    const QModelIndex index = _tree_screen->tree_view()->current_index();
    auto current_item = _tree_screen->tree_view()->current_item();

    if(current_item) {
        appconfig.tree_position(
            _current_source_model()->root_item()->id()    // _tree_screen->know_model_board()->root_item()->id()
            , current_item->path_list()
            );
    } else if(item) { //if(index.isValid()) {

        //        //    if(index.isValid()) {   // this line is to be remove
        //        // Получаем указатель вида TreeItem
        //        auto item = _current_source_model()->item(index);

        // Сохраняем путь к элементу item
        appconfig.tree_position(
            _current_source_model()->root_item()->id()    // _tree_screen->know_model_board()->root_item()->id()
            , item->path_list()
            );
        //    }
    }
}

// set
void MainWindow::set_tree_position(QString current_root_id, QStringList current_item_absolute_path)
{
    //    _tree_screen->session_root_id(current_item_absolute_path.last());

    auto source_model = [&] () {
            return _tree_screen->tree_view()->source_model();
        };
    auto know_model_board = [&] () {
            return _tree_screen->tree_view()->know_model_board();
        };

    if(source_model()->root_item()->id() != current_root_id) {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, know_model_board()->item([&] (boost::intrusive_ptr<const TreeItem> it) {
                    return it->id() == current_root_id;
                })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        _tree_screen->tree_view()->intercept(tree_index);
    }

    //    if(!_tree_screen->know_model_board()->item(current_item_absolute_path))   // on know_root semantic
    //        return;

    // Получаем указатель на элемент вида TreeItem, используя путь
    auto it = know_model_board()->item(current_item_absolute_path);            // on know_root semantic

    if(!source_model()->index(it).isValid()) {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, it); } catch(std::exception &e) {throw e; } return tree_index; } ();
        _tree_screen->tree_view()->intercept(tree_index);
    }

    auto tree_view = _tree_screen->tree_view();

    if(it && it != know_model_board()->root_item()) {
        qDebug() << "Set tree position to " << it->field("name") << " id " << it->field("id");

        //        // Из указателя на элемент TreeItem получаем QModelIndex
        //        QModelIndex setto = source_model()->index(item);

        // Курсор устанавливается в нужную позицию

        boost::intrusive_ptr<TreeIndex> tree_index;
        try {tree_index = new TreeIndex([&] {return tree_view->source_model(); }, it->parent(), it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                return il == it->linker() && il->host() == it && it->parent() == il->host_parent();
            })); } catch(std::exception &e) {throw e; }

        tree_view->select_as_current(tree_index);
        tree_view->source_model()->session_id(tree_index);  // TreeIndex(source_model, it)
    } else if(it) {
        boost::intrusive_ptr<TreeIndex> tree_index;
        try {tree_index = new TreeIndex([&] {return tree_view->source_model(); }, it, 0); } catch(std::exception &e) {throw e; }

        tree_view->select_as_current(tree_index);
        tree_view->source_model()->session_id(tree_index);  // TreeIndex(source_model, it)
    }
}


bool MainWindow::is_tree_position_crypt()
{
    bool result = false;
    auto pair = appconfig.tree_position();
    QString id = pair.first;
    QStringList path = pair.second;

    auto know_model_board = [&] () {
            return _tree_screen->tree_view()->know_model_board();
        };

    if(_tree_screen->tree_view()->source_model()->root_item()->id() != id) {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, know_model_board()->item([&] (boost::intrusive_ptr<const TreeItem> it) {
                    return it->id() == id;
                })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        _tree_screen->tree_view()->intercept(tree_index);
    }

    //    if(_tree_screen->know_model_board()->is_item_valid(path) == false) return false;

    // Получаем указатель на элемент вида TreeItem, используя путь
    auto item = know_model_board()->item(path);

    if(item) {
        if(item->field("crypt") == "1")
            result = true;

        //        else
        //            return false;
    }

    return result;
}

// too many _record_screen objects, deprecated
//void MainWindow::restore_recordtable_position(void)
//{
//    QString id = appconfig.get_recordtable_selected_record_id();

//    if(id.length() > 0)
//        select_id(id);
//}

// too many _record_screen objects, deprecated
//void MainWindow::save_recordtable_position(void)
//{
//    QString id = _table_screen->first_selection_id();

//    appconfig.set_recordtable_selected_record_id(id);
//}


//void MainWindow::select_id(QString id)
//{
//    _table_screen->select_id(id);
//}


void MainWindow::save_editor_cursor_position(void)
{
    int n = _editor_screen->cursor_position();

    appconfig.editor_cursor_position(n);
}


void MainWindow::restore_editor_cursor_position(void)
{
    int n = appconfig.editor_cursor_position();

    _editor_screen->cursor_position(n);
}


void MainWindow::save_editor_scrollbar_position(void)
{
    int n = _editor_screen->scrollbar_position();

    appconfig.editor_scroll_bar_position(n);
}


void MainWindow::restore_editor_scrollbar_position(void)
{
    int n = appconfig.editor_scroll_bar_position();

    _editor_screen->scrollbar_position(n);
}


void MainWindow::restore_find_in_base_visible(void)
{
    bool n = appconfig.findscreen_show();

    // Определяется ссылка на виджет поиска
    FindScreen *find_screen = globalparameters.find_screen(); // find_object<FindScreen>(find_screen_singleton_name);

    if(n)
        find_screen->show();
    else
        find_screen->hide();
}


// Создание раздела меню File
void MainWindow::init_file_menu(void)
{
    // Создание меню
    //    _filemenu = new QMenu(tr("&File"), this);
    _filemenu->clear();
    menuBar()->addMenu(_filemenu);

    //    // Создание тулбара
    //    /*
    //    QToolBar *tb = new QToolBar(this);
    //    tb->setWindowTitle(tr("File Actions"));
    //    addToolBar(tb);
    //    */

    QAction *a;

    //    /*
    //    a = new QAction(tr("&New"), this);
    //    a->setShortcut(QKeySequence::New);
    //    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    //    // tb->addAction(a);
    //    menu->addAction(a);

    //    a = new QAction(tr("&Open..."), this);
    //    a->setShortcut(QKeySequence::Open);
    //    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    //    // tb->addAction(a);
    //    menu->addAction(a);

    //    menu->addSeparator();

    //    a = new QAction(tr("&Save"), this);
    //    a->setShortcut(QKeySequence::Save);
    //    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    //    a->setEnabled(false);
    //    // tb->addAction(a);
    //    menu->addAction(a);

    //    a = new QAction(tr("Save &As..."), this);
    //    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    //    menu->addAction(a);
    //    menu->addSeparator();
    //    */

    a = new QAction(tr("&Print..."), this);
    a->setShortcut(QKeySequence::Print);
    connect(a, &QAction::triggered, this, &MainWindow::file_print);
    // tb->addAction(a);
    _filemenu->addAction(a);

    a = new QAction(tr("Print Preview..."), this);
    connect(a, &QAction::triggered, this, &MainWindow::file_print_preview);
    _filemenu->addAction(a);

    a = new QAction(tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, &QAction::triggered, this, &MainWindow::file_print_pdf);
    // tb->addAction(a);
    _filemenu->addAction(a);

    _filemenu->addSeparator();

    //    a = new QAction(tr("&Quit"), this);
    //    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    //    connect(a, SIGNAL(triggered()), this, SLOT(applicationExit()));
    //    _filemenu->addAction(a);

}


// Создание раздела меню Tools
void MainWindow::init_tools_menu(void)
{
    _toolsmenu->clear();
    // Создание меню
    //_toolsmenu = new QMenu(tr("&Tools"), this);
    menuBar()->addMenu(_toolsmenu);

    QAction *a;

    a = new QAction(tr("Find in ba&se"), this);
    connect(a, &QAction::triggered, this, &MainWindow::tools_find);
    _toolsmenu->addAction(a);

    auto b = new QAction(tr("Editor"), this);
    connect(b, &QAction::triggered, this, &MainWindow::editor_switch);
    _toolsmenu->addAction(b);

    _toolsmenu->addSeparator();


    if(appconfig.interface_mode() == "desktop") {
        a = new QAction(tr("Main &preferences"), this);
        connect(a, &QAction::triggered, this, &MainWindow::tools_preferences);
        _toolsmenu->addAction(a);
    } else {
        // Создание подменю
        QMenu *menu = new QMenu(tr("Main &preferences"), this);
        init_preferences_menu(menu);
    }
}


// Заполнение подраздела меню Preferences
void MainWindow::init_preferences_menu(QMenu *menu)
{
    QAction *a;

    a = new QAction(tr("Main"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsFind()));
    menu->addAction(a);

    a = new QAction(tr("Crypt"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("Synchro"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("RecordTable"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("Misc"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);
}


// Создание раздела меню Help
void MainWindow::init_help_menu(void)
{
    _helpmenu->clear();
    // Создание меню
    //_helpmenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(_helpmenu);

    QAction *a;

    a = new QAction(tr("About MyTetra"), this);
    connect(a, &QAction::triggered, this, &MainWindow::on_click_help_about_mytetra);
    _helpmenu->addAction(a);

    a = new QAction(tr("About Qt"), this);
    connect(a, &QAction::triggered, this, &MainWindow::on_click_help_about_qt);
    _helpmenu->addAction(a);
}


// Новая коллекция
void MainWindow::file_new(void)
{

}

// Открыть коллекцию
void MainWindow::file_open(void)
{

}

// Сохранить текущую статью
bool MainWindow::file_save(void)
{
    return true;
}

// Сохранить текущую статью как файл
bool MainWindow::file_save_as(void)
{
    return true;
}

// Напечатать текущую статью
void MainWindow::file_print(void)
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if(dlg->exec() == QDialog::Accepted)
        _editor_screen->textarea_document()->print(&printer);

    delete dlg;
#endif
}


// Предпросмотр печати текущей статьи
void MainWindow::file_print_preview(void)
{
    PrintPreview *preview = new PrintPreview(_editor_screen->textarea_document(), this);

    preview->setModal(true);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}


// Вывод текущей статьи в PDF файл
void MainWindow::file_print_pdf(void)
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
            QString(), "*.pdf");

    if(!fileName.isEmpty()) {
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        _editor_screen->textarea_document()->print(&printer);
    }

#endif
}


// Слот - Нормальный выход из программы
void MainWindow::application_exit(void)
{
    save_all_state();

    // Если в конфиге настроено, что нужно синхронизироваться при выходе
    // И задана команда синхронизации
    if(appconfig.synchro_on_exit())
        if(appconfig.synchro_command().trimmed().length() > 0)
            synchronization();

    // Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}


// Быстрый выход из программы, без возможности синхронизации
void MainWindow::application_fast_exit(void)
{
    save_all_state();

    // Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}


void MainWindow::tools_find(void)
{
    // Определяется ссылка на виджет поиска
    FindScreen *findScreenRel = globalparameters.find_screen(); // find_object<FindScreen>(find_screen_singleton_name);

    if(!(findScreenRel->isVisible()))
        findScreenRel->show();
    else
        findScreenRel->hide();
}

void MainWindow::editor_switch(void)
{

    MetaEditor *editorScreen = globalparameters.meta_editor();  // find_object<MetaEditor>(meta_editor_singleton_name);

    if(!(editorScreen->isVisible())) {
        editorScreen->show();
        appconfig.editor_show(true);
    } else {
        editorScreen->hide();
        appconfig.editor_show(false);
    }
}

void MainWindow::tools_preferences(void)
{
    // Создается окно настроек, после выхода из этой функции окно удалится
    AppConfigDialog dialog(_entrance->activated_browser()->record_screen()->record_controller(), "");
    dialog.show();
}


// Слот, срабатывающий при нажатии в редакторе кнопки "распахнуть"
// Флаг показывает, что нужно сделать
// true - распахнуть область отводимую редактору
// false - сделать область, отводимую редактору, обычной
void MainWindow::on_expand_edit_area(bool flag)
{
    //    static QSize entrance_size = globalparameters.entrance()->size();
    //    static QSize tree_size = globalparameters.entrance()->size();
    //    static QSize recordtable_size = globalparameters.entrance()->size();
    //    static QSize vtab_size = globalparameters.entrance()->size();

    //    static bool _treetable_hidden;     // = globalparameters.getTreeScreen()->isHidden();
    //    static bool recordtable_hidden; // = globalparameters.getRecordTableScreen()->isHidden();

    if(flag) {
        globalparameters.entrance()->hide();    // resize(QSize(0, 0)); //

        //        if(!globalparameters.getTreeScreen()->isHidden()) {
        //            _treetable_hidden = false;
        //            //            globalparameters.getTreeScreen()->resize(QSize(0, tree_size.height()));
        //            globalparameters.getTreeScreen()->hide();    // resize(QSize(0, 0)); //
        //        } else {
        //            _treetable_hidden = true;
        //        }

        //        if(!globalparameters.getRecordTableScreen()->isHidden()) {
        //            _recordtable_hidden = false;
        //            //            globalparameters.getRecordTableScreen()->resize(QSize(0, recordtable_size.height()));
        //            globalparameters.getRecordTableScreen()->hide(); // resize(QSize(0, 0)); //
        //        } else {
        //            _recordtable_hidden = true;
        //        }

        emit _vtab_tree->_hide_action->setChecked(true);
        //        QTabWidget *tab = globalparameters.vtab();
        //        tab->currentWidget()->hide();

        //        auto * tab_pane = tab->findChild<QStackedWidget *>();
        //        tab_pane->hide();

        //        QRect rect = tab->geometry();
        //        tab->setGeometry(rect.left(), rect.top(), 0, rect.height());

        //                setGeometry(rect.top(), rect.left(), 0, rect.height());

        //                resize(QSize(0, v_left_splitter->height())); // hide();
    } else {
        globalparameters.entrance()->show();    // resize(entrance_size); //

        //        if(!_treetable_hidden) {
        //            globalparameters.getTreeScreen()->show();    // resize(tree_size); //
        //        }

        //        if(!_recordtable_hidden) {
        //            globalparameters.getRecordTableScreen()->show(); // resize(recordtable_size); //
        //        }

        //        emit _vtabwidget->hideAction.toggle();
        emit _vtab_tree->_hide_action->setChecked(false);
        //        globalparameters.vtab()->resize(vtab_size); // show();
    }
}


void MainWindow::on_click_help_about_mytetra(void)
{
    QString version = QString::number(APPLICATION_RELEASE_VERSION) + "." + QString::number(APPLICATION_RELEASE_SUBVERSION) + "." + QString::number(APPLICATION_RELEASE_MICROVERSION);

    QString infoProgramName;
    QString infoVersion;
    QString infoAuthor;
    QString infoEmail;
    QString infoLicense;
    QString infoTargetOs;
    QString infoProgramFile;
    QString infoWorkDirectory;
    QString infoDevicePixelRatio;
    QString infoPhysicalDpi;
    QString infoPhysicalDpiX;
    QString infoPhysicalDpiY;

    infoProgramName = "<b>MyTetra</b> - smart manager<br/>for information collecting<br/><br/>";
    infoVersion = "v." + version + "<br/><br/>";
    infoAuthor = "Author: Sergey M. Stepanov<br/>";
    infoEmail = "Author Email:<i>xintrea@gmail.com</i><br/><br/>";
    infoLicense = "GNU General Public License v.3.0<br/><br/>";
    infoTargetOs = "Target OS: " + globalparameters.target_os() + "<br/>";
    infoProgramFile = "Program file: " + globalparameters.main_program_file() + "<br/>";
    infoWorkDirectory = "Work directory: " + globalparameters.work_directory() + "<br/>";

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    infoDevicePixelRatio = "Device pixel ratio: " + (QString::number(qApp->devicePixelRatio(), 'f', 8)) + "<br/>";
    infoPhysicalDpi = "Physical DPI (from screen): " + (QString::number(QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 8)) + "<br/>";
#endif

    infoPhysicalDpiX = "Physical DPI X (from desktop): " + (QString::number(qApp->desktop()->physicalDpiX(), 'f', 8)) + "<br/>";
    infoPhysicalDpiY = "Physical DPI Y (from desktop): " + (QString::number(qApp->desktop()->physicalDpiY(), 'f', 8)) + "<br/>";

    QString info = infoProgramName +
        infoVersion +
        infoAuthor +
        infoEmail +
        infoLicense +
        infoTargetOs +
        infoProgramFile +
        infoWorkDirectory +
        infoDevicePixelRatio +
        infoPhysicalDpi +
        infoPhysicalDpiX +
        infoPhysicalDpiY;

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->about(this,
        "MyTetra v." + version,
        info);
}


void MainWindow::on_click_help_about_qt(void)
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->aboutQt(this);
}


void MainWindow::synchronization(void)
{
    // Сохраняются данные в поле редактирования
    save_text_area();

    // Сохраняются данные о курсорах в дереве и таблице конечных записей
    save_tree_position();
    //    save_recordtable_position();
    save_editor_cursor_position();
    save_editor_scrollbar_position();

    // Считывается команда синхронизации
    QString command = appconfig.synchro_command();

    // Если команда синхронизации пуста
    if(command.trimmed().length() == 0) {
        QMessageBox::warning(this,
            tr("MyTetra: can't synchronization"),
            tr("Do not set synchronization command.<br>Check the setting in \"Syncro\" section in \"Tools\" menu"),
            QMessageBox::Close);
        return;
    }


    // Макрос %a заменяется на путь к директории базы данных
    // QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir();
    QDir databaseDir(appconfig.tetra_dir());
    QString databasePath = databaseDir.canonicalPath();

    command.replace("%a", databasePath);

    // Запуск команды синхронизации
    ExecuteCommand cons;
    cons.setWindowTitle(tr("MyTetra synchronization"));
    cons.setMessageText(tr("Synchronization in progress, please wait..."));

    cons.setCommand(command);
    cons.run();

    // Блокируется история
    walkhistory.set_drop(true);

    // Заново считываются данные в дерево
    _tree_screen->tree_view()->know_model_reload();
    restore_tree_position();
    //    restore_recordtable_position();
    restore_editor_cursor_position();
    restore_editor_scrollbar_position();

    // Разблокируется история посещений элементов
    walkhistory.set_drop(false);
}


void MainWindow::setup_icon_actions(void)
{
    _action_tray_restore = new QAction(tr("&Restore window"), this);
    connect(_action_tray_restore, &QAction::triggered, this, &MainWindow::showNormal);

    _action_tray_maximize = new QAction(tr("Ma&ximize window"), this);
    connect(_action_tray_maximize, &QAction::triggered, this, &MainWindow::showMaximized);

    _action_tray_minimize = new QAction(tr("Mi&nimize window"), this);
    connect(_action_tray_minimize, &QAction::triggered, this, &MainWindow::hide);

    _action_tray_quit = new QAction(tr("&Quit"), this);
    connect(_action_tray_quit, &QAction::triggered, this, &MainWindow::application_exit);
}


void MainWindow::create_tray_icon(void)
{
    _tray_icon_menu = new QMenu(this);
    _tray_icon_menu->addAction(_action_tray_restore);
    _tray_icon_menu->addAction(_action_tray_maximize);
    _tray_icon_menu->addAction(_action_tray_minimize);
    _tray_icon_menu->addSeparator();
    _tray_icon_menu->addAction(_action_tray_quit);

    _tray_icon = new QSystemTrayIcon(this);
    _tray_icon->setContextMenu(_tray_icon_menu);
}


void MainWindow::set_icon(void)
{
    connect(_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(icon_activated(QSystemTrayIcon::ActivationReason)));

    QIcon icon = QIcon(":/resource/pic/logo.svg");
    _tray_icon->setIcon(icon);
    setWindowIcon(icon);

    // tray_icon->setToolTip(iconComboBox->itemText(index));
}


void MainWindow::icon_activated(QSystemTrayIcon::ActivationReason reason)
{
    if(QSystemTrayIcon::isSystemTrayAvailable() == false) return;

    switch(reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(isVisible()) {
            if(isMinimized()) showNormal();
            else hide();
        } else {
            if(isMinimized()) showNormal();
            else show();
        }

    default:
        ;
    }
}


// Слот закрытия окна
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(_enable_real_close == false) {
        if(QSystemTrayIcon::isSystemTrayAvailable() == false) return;

        // При приходе события закрыть окно, событие игнорируется
        // и окно просто делается невидимым. Это нужно чтобы при закрытии окна
        // программа не завершала работу
        if(_tray_icon->isVisible()) {
            hide();
            event->ignore();
        }
    }
}


bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o);

    // qDebug() << "Event: " << e->type();

    // Отлавливание потери фокуса
    // QEvent::ActivationChange
    if(e->type() == QEvent::WindowDeactivate) {
        qDebug() << "Main window focus deactivate, save all state.";
        save_all_state();
    }

    return false; // Продолжать оработку событий дальше
}


void MainWindow::go_walk_history_previous(void)
{
    _editor_screen->save_textarea();

    QString id = _editor_screen->misc_field("id");
    walkhistory.add(id,
        _editor_screen->cursor_position(),
        _editor_screen->scrollbar_position(),
        WALK_HISTORY_GO_PREVIOUS);
    walkhistory.set_drop(true);

    go_walk_history();
}


void MainWindow::go_walk_history_next(void)
{
    _editor_screen->save_textarea();

    QString id = _editor_screen->misc_field("id");
    walkhistory.add(id,
        _editor_screen->cursor_position(),
        _editor_screen->scrollbar_position(),
        WALK_HISTORY_GO_NEXT);
    walkhistory.set_drop(true);

    go_walk_history();
}


void MainWindow::go_walk_history(void)
{
    //    QString tree_root_id = walkhistory.tree_root_id();

    //    if(_tree_screen->know_root()->root_item()->id() != tree_root_id) {
    //        _tree_screen->intercept(tree_root_id);
    //    }

    // Выясняется идентификатор записи, на которую надо переключиться
    QString record_id = walkhistory.record_id();

    //    if(record_id.length() == 0) {
    //        walkhistory.set_drop(false);
    //        return;
    //    }

    auto know_model_board = [&] () {
            return _tree_screen->tree_view()->know_model_board();
        };

    if(record_id.length() > 0) {
        // Выясняется путь к ветке, где находится данная запись
        QStringList absolute_path = know_model_board()->record_path(record_id);    // on know_root semantic

        //    // Проверяем, есть ли такая ветка
        //    if(_tree_screen->know_model_board()->is_item_valid(absolute_path) == false) {    // on know_root semantic
        //        walkhistory.set_drop(false);
        //        return;
        //    }


        // Выясняется позицию записи в таблице конечных записей
        auto item = know_model_board()->item(absolute_path);    // on know_root semantic

        if(item) {
            //            // Проверяем, есть ли такая позиция
            //            if(!item->item_direct(record_id)) {  // == false
            //                walkhistory.set_drop(false);
            //                return;
            //            }

            if(item->item_direct(record_id)) {
                set_tree_position(global_root_id, absolute_path);
                //    select_id(id);

                if(appconfig.remember_cursor_at_history_navigation()) {
                    _editor_screen->cursor_position(walkhistory.cursor_position(record_id));
                    _editor_screen->scrollbar_position(walkhistory.scrollbar_position(record_id));
                }
            }

            //            walkhistory.set_drop(false);
        }

        //        else {
        //            walkhistory.set_drop(false);
        //        }
    }

    walkhistory.set_drop(false);
}


// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void MainWindow::save_text_area(void)
{
    QString id = _editor_screen->misc_field("id");

    qDebug() << "MainWindow::saveTextarea() : id :" << id;

    _editor_screen->save_textarea();

    walkhistory.add(id,
        _editor_screen->cursor_position(),
        _editor_screen->scrollbar_position());
}


// Слот, обрабатывающий смену фокуса на виджетах
void MainWindow::on_focus_changed(QWidget *widgetFrom, QWidget *widgetTo)
{
    Q_UNUSED(widgetFrom);

    if(widgetTo == nullptr)
        return;

    qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

    return; // Временно ничего не делает
}


