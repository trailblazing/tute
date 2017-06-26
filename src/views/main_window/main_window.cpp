
#include <string>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDesktopWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>

//#include "models/tree/tree_item.dec"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/blogger.h"
#include "libraries/qtm/sys_tray_icon.h"
#include "libraries/trash_monitoring.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"
#include "libraries/wyedit/editor_text_area.h"
#include "main.h"
#include "main_window.h"
#include "models/app_config/app_config.h"
#include "models/record_table/linker.hxx"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "views/app_config/app_config_dialog.h"
#include "views/browser/autosaver.h"
#include "views/browser/bookmarks.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/downloadmanager.h"
#include "views/browser/history.h"
#include "views/browser/tabwidget.h"
#include "views/browser/toolbar_search.h"
#include "views/browser/webview.h"
#include "views/console_emulator/execute_command.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/print_preview/print_preview.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<AppConfig> appconfig;
extern TrashMonitoring trashmonitoring;
extern std::shared_ptr<gl_para> gl_paras;
extern WalkHistory walkhistory;
const char* program_title = "Tute";
const QString program_title_qstring = "Tute";
const std::string program_title_string = "Tute";

const char* meta_editor_singleton_name = "editor_screen";
const char* record_screen_multi_instance_name = "record_screen";
const char* tree_screen_singleton_name = "tree_screen";
const char* find_screen_singleton_name = "find_screen";
const char* download_manager_singleton_name = "download_manager";
const char* windowswitcher_singleton_name = "window_switcher";
const char* browser_docker_singleton_name = "browser_docker";
const char* editor_docker_singleton_name = "editor_docker";
const char* record_controller_multi_instance_name = "record_controller"; // std::string(std::string(table_screen_singleton_name)
// + std::string("_controller")).c_str();
const char* record_view_multi_instance_name = "record_view";

extern const char* tree_screen_viewer_name;
extern const char* action_find_in_base;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(wn_t)
#endif

wn_t::wn_t(
    web::Profile* profile, QString style_source // std::shared_ptr<gl_para> gl_paras	// , std::shared_ptr<AppConfig> appconfig_	// , std::shared_ptr<DataBaseConfig> databaseconfig_,
    )
    : QMainWindow()
    // , _gl_paras([&]() -> std::shared_ptr<gl_para> {gl_paras->main_window(this); return gl_paras;} ()) , _appconfig(appconfig_), _databaseconfig(databaseconfig_)
    , _central_widget(new QWidget(this))
    , _profile(profile)
    , _style_source([&]() -> QString {
        gl_paras->main_window(this);
        gl_paras->style_source(style_source);
        this->setAttribute(Qt::WA_InputMethodEnabled);
        return style_source;
    }())
    , _v_right_splitter([&]() -> QSplitter* {
        auto vrs = new QSplitter(Qt::Vertical);
        vrs->setSizes(appconfig->v_right_splitter_sizelist());
        gl_paras->v_right_splitter(vrs);
        return vrs;
    }())
    , _v_find_splitter([&]() -> QSplitter* {
        auto vfs = new QSplitter(Qt::Vertical);
        vfs->setSizes(appconfig->findsplitter_sizelist());
        gl_paras->find_splitter(vfs);
        return vfs;
    }())
    // , _vtab_tree([&](QString style_source_){auto vt = new HidableTabWidget(style_source_, this);gl_paras->vtab_tree(vt);return vt;} (_style))
    , _h_record_splitter([&]() -> QSplitter* {
        auto hrs = new QSplitter(Qt::Horizontal);
        hrs->setSizes(appconfig->h_record_splitter_sizelist());
        gl_paras->h_record_splitter(hrs);
        return hrs;
    }())
    , _h_tree_splitter([&]() -> QSplitter* {
        auto hls = new QSplitter(Qt::Horizontal);
        hls->setSizes(appconfig->h_tree_splitter_sizelist());
        gl_paras->h_tree_splitter(hls);
        return hls;
    }()) // Qt::Vertical
    , _vtab_record([&]() -> HidableTab* {
        _vtab_record = nullptr;
        auto vr = new HidableTab(
            this, _h_record_splitter, std::make_shared<QSettings>(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_conf_filename, QSettings::IniFormat), "General", "h_record_splitter_sizelist", "collapsed", this);
        gl_paras->vtab_record(vr);
        return vr;
    }())
    , _browser_docker([&]() -> web::Docker<web::Browser>* {
        _browser_docker = nullptr;
        auto bd = new web::Docker<web::Browser>(browser_docker_singleton_name, this, Qt::Widget);
        gl_paras->browser_docker(bd);
        return bd;
    }()) // Qt::MaximizeUsingFullscreenGeometryHint
    , _editor_docker([&]() -> web::Docker<Blogger>* {
        _editor_docker = nullptr;
        auto ed = new web::Docker<Blogger>(editor_docker_singleton_name, this, Qt::Widget);
        gl_paras->editor_docker(ed);
        return ed;
    }())
    , _tree_screen([&]() -> ts_t* {
        _tree_screen = nullptr;
        auto ts = new ts_t(tree_screen_singleton_name, _editor_docker, this);
        gl_paras->tree_screen(ts);
        return ts;
    }()) // _vtabwidget
    , _find_screen([&]() -> FindScreen* {
        _find_screen = nullptr;
        auto fs = new FindScreen(find_screen_singleton_name, _tree_screen, this);
        gl_paras->find_screen(fs);
        return fs;
    }())
    , _statusbar([&] {
        auto st = new QStatusBar(this);
        gl_paras->status_bar(st);
        return st;
    }())
    , _switcher([&] {
        auto sw = new WindowSwitcher(windowswitcher_singleton_name, this);
        gl_paras->window_switcher(sw);
        return sw;
    }())
    , _quit_action([&]() -> QAction* {
        auto q = new QAction(tr("&Quit"), this);
        q->setShortcut(Qt::CTRL + Qt::Key_Q);
        connect(q, &QAction::triggered, this, &wn_t::application_exit);
        return q;
    }())
    , _enable_real_close(false)
    // , _h_splitter(new QSplitter(Qt::Horizontal))
    , _filemenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&File"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_file_menu_name] = fm;
    , _editmenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&Edit"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_edit_menu_name] = fm;
    , _viewmenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&View"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_view_menu_name] = fm;
    , _histrymenu([&]() -> web::HistoryMenu* {
        auto fm = new web::HistoryMenu(this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_history_menu_name] = fm;
    , _bookmarkmenu([&]() -> web::BookmarksMenu* {
        auto fm = new web::BookmarksMenu(this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_bookmark_menu_name] = fm;
    , _windowmenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&Window"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_window_menu_name] = fm;
    , _toolsmenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&Tools"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_tools_menu_name] = fm;
    , _helpmenu([&]() -> QMenu* {
        auto fm = new QMenu(tr("&Help"), this);
        fm->setContentsMargins(0, 0, 0, 0);
        return fm;
    }()) //_main_menu_map[gl_para::_help_menu_name] = fm;
    , _autosaver(new web::AutoSaver(this))
#ifdef USE_STRINGLIST_MODEL
    , _stringlistmodel(new QStringListModel(this))
#endif //
    , _tray_icon([&] {
        auto ti = new SysTrayIcon(_vtab_record, _editor_docker, this, _profile, _style_source, true);
        gl_paras->tray_icon(ti);
        return ti;
    }())
// , _download(new web::DownloadManager(download_manager_singleton_name,
// _vtab_record))
{
    // _page_screen->setVisible(false);
    // _page_screen->hide();

    // gl_paras->main_window(this);

    QSettings main_settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_conf_filename, QSettings::IniFormat);

    QStringList line_list;
    QList<int> list;
    main_settings.beginGroup("General");
    line_list = (main_settings.value("h_record_splitter_sizelist", "100,100")).toString().split(",");
    for (int i = 0; i < line_list.size(); ++i) list.append(line_list.at(i).toInt());
    auto collapsed = main_settings.value("collapsed", 0).toBool();
    // HidableTabWidget::_editentry->_blog_editor->_splitter->setSizes(list);
    main_settings.endGroup();

    // int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 100
    // int vtab_g_width = _vtabwidget->geometry().width();         // 100
    // int this_width = geometry().width();                        // 640
    // int download_width = _download->geometry().width();         // 1089
    // int tree_screen_width = _tree_screen->geometry().width();   // 100
    // int find_splitter_width = _v_find_splitter->geometry().width(); // 640
    // int left_splitter_width = _h_left_splitter->geometry().width(); // 640

    // if(_vtabwidget->frameGeometry().width() ==
    // _h_left_splitter->geometry().width()) {
    // _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100,
    // _vtabwidget->geometry().height());
    // }

    _v_right_splitter->setHandleWidth(0);
    _v_find_splitter->setHandleWidth(0);
    _h_tree_splitter->setHandleWidth(0);
    // int wl = _h_left_splitter->width(); // 640
    // int wr = _h_right_splitter->width();// 640   // default handleWidth() == 6
    _h_record_splitter->setHandleWidth(0);
    // _h_splitter->setHandleWidth(0);

    // _vtab_record->tabBar()->hide();

    // _globalparameters.vtab_record(_vtab_record);// in initialize list
    // _globalparameters.vtab_tree(_vtab_tree);	// in initialize list

    extern QObject* mainwindow;
    mainwindow = this;
    setObjectName("mainwindow");

    installEventFilter(this);

    {
        menuBar()->addMenu(_filemenu);
        init_file_menu();
        //		append_quit_menu();
    }
    {
        menuBar()->addMenu(_toolsmenu);
        init_tools_menu();
    }
    // initHelpMenu();
    QMainWindow::menuBar()->hide();

    setup_ui();

    setup_signals();
    assembly();

    // initFileMenu();
    // initToolsMenu();
    // initHelpMenu();

    // setup_icon_actions();
    // create_tray_icon();
    // _tray_icon = [&] {auto ti = new SysTrayIcon(_tree_screen, _browser_dock,
    // _vtab_record, _find_screen, _editor_dock, this, _profile, _style_source);
    // gl_paras->tray_icon(ti); return ti;} ();

    // set_icon(void)
    [&] {
        connect(_tray_icon, &SysTrayIcon::activated, this, &wn_t::icon_activated);

        QIcon icon = QIcon(":/resource/pic/logo.svg");
        _tray_icon->setIcon(icon);
        setWindowIcon(icon);

        // tray_icon->setToolTip(iconComboBox->itemText(index));
    }();
    // set_icon();
    if (QSystemTrayIcon::isSystemTrayAvailable()) _tray_icon->show();
    //// Инициализируется объект слежения за корзиной
    // trashmonitoring.init(appconfig->get_trashdir());
    // trashmonitoring.update();

    // Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
    _enable_real_close = false;

    // Инициализация генератора случайных чисел
    init_random();

    {
        ////	QSettings	qtm_settings(_config_file_name,
        ///QSettings::IniFormat);
        // auto	barwidth_geometry	=
        // _control_panel->tabBar()->geometry().width();
        // auto	whole_width			= _control_panel->width();
        // auto	bar_width			=
        // _control_panel->tabBar()->width();

        auto bar_width = _vtab_record->tabBar()->sizeHint().width();
        if (0 == bar_width) bar_width = _vtab_record->sizeHint().width();
        if (list[0] <= bar_width || collapsed) {
            _vtab_record->collapse_when_true(true);
            // auto list = _splitter->sizes();

            list[1] = width() - bar_width;
            list[0] = bar_width;
            // _splitter->setSizes(list);
            // emit _splitter->splitterMoved(list[0], 0);
        } else {
            _vtab_record->collapse_when_true(false);
            // 284
            // 428
        }
        _vtab_record->resize(list[0], _vtab_record->parentWidget()->geometry().height());
        _h_record_splitter->setSizes(list);

        _browser_docker->resize(list[1], this->geometry().height());

        assert(list[1] == _browser_docker->width());
        emit _h_record_splitter->splitterMoved(list[0], 0);
    }
}

// void wn_t::append_quit_menu(){
//	_quit_action->setShortcut(Qt::CTRL + Qt::Key_Q);
//	connect(_quit_action, &QAction::triggered, this,
//&wn_t::application_exit);
////	_filemenu->addAction(_quit_action);
//}

QAction* wn_t::quit_action() const
{
    return _quit_action;
}

wn_t::~wn_t()
{

    sapp_t::instance()->saveSession(); //save_all_state();
    _autosaver->saveIfNeccessary();
    _browser_docker->deleteLater();
    _browser_docker = nullptr;
    // delete
    _switcher->deleteLater();
    // delete
    _statusbar->deleteLater();
    // delete
    // _editor_screen->deleteLater();
    _editor_docker->deleteLater();
    // delete
    _find_screen->deleteLater();
    // delete  _download;
    // if(_page_screen)delete  _page_screen;
    // delete  _table_screen;
    if (_tree_screen) {
        // delete
        _tree_screen->deleteLater();
        _tree_screen = nullptr;
    }
    // delete  _vtabwidget;

    // delete
    _v_right_splitter->deleteLater();
    // delete
    _v_find_splitter->deleteLater();
    // delete  _vtabwidget;
    // delete
    _h_record_splitter->deleteLater();
    // delete
    _h_tree_splitter->deleteLater();
    // delete
    // _h_splitter->deleteLater();

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

void wn_t::setup_ui(void)
{
    if (gl_paras->target_os() == "android") setWindowState(Qt::WindowMaximized); // Для Андроида окно просто
    // разворачивается на весь экран
    else {
        QRect rect = appconfig->mainwin_geometry();
        resize(rect.size());
        move(rect.topLeft());
    }
    // При создании объектов не указывается parent, так как он буден задан в
    // момент вставки в layout в методе assembly()
    // _tree_screen = new TreeScreen(appconfig, this);
    // _tree_screen->setObjectName(tree_screen_singleton_name);  // "treeScreen"
    // _globalparameters.tree_screen(_tree_screen);
    //// _treetable_hidden = treeScreen->isHidden();
    //// connect(treeScreen, &TreeScreen::hide, this, [this]() {_treetable_hidden
    ///= true;});
    //// connect(treeScreen, &TreeScreen::show, this, [this]() {_treetable_hidden
    ///= false;});
    //// _table_screen = new TableScreen(this);
    //// _table_screen->setObjectName(table_screen_singleton_name); //
    ///"recordTableScreen"
    //// _globalparameters.push_record_screen(_table_screen);
    //// _recordtable_hidden = recordTableScreen->isHidden();
    //// connect(recordTableScreen, &RecordTableScreen::hide, this, [this]()
    ///{_recordtable_hidden = true;});
    //// connect(recordTableScreen, &RecordTableScreen::show, this, [this]()
    ///{_recordtable_hidden = false;});
    //// _page_screen = new TableScreen(this);
    //// _page_screen->setObjectName("page_screen");
    //// _globalparameters.page_screen(_page_screen);
    //// _download = new web::DownloadManager(this);
    //// _download->setObjectName(download_manager_singleton_name);
    // _globalparameters.download_manager(_download);
    // _editor_screen = new MetaEditor();
    // _editor_screen->setObjectName(meta_editor_singleton_name);  //
    // "editorScreen"
    // _globalparameters.meta_editor(_editor_screen);
    // gl_paras->editor_dock(_editor_dock);
    // _find_screen = new FindScreen(this);
    // _find_screen->setObjectName(find_screen_singleton_name);  //
    // "findScreenDisp"
    // gl_paras->find_screen(_find_screen);
    // findScreenDisp->hide();
    if (!appconfig->editor_show()) {
        // _editor_screen->hide();
        _editor_docker->hide();
    }
    // _statusbar = new QStatusBar(this);
    _statusbar->setObjectName("status_bar"); // "statusBar"
    setStatusBar(_statusbar);
    // gl_paras->status_bar(_statusbar);
    // Вспомогательный объект переключения окон, используется в мобильном
    // интерфейсе
    // _switcher = new WindowSwitcher(this);
    // _switcher->setObjectName(windowswitcher_singleton_name); //
    // "windowSwitcher"
    // gl_paras->window_switcher(_switcher);
    if (!_find_screen->isVisible()) _switcher->find_in_base_click();
    // if(_table_screen) {
    // _entrance = new web::Entrance(
    // _record_ontroller
    // = _table_screen->getRecordTableController()
    // , _globalparameters.style_source()
    // , this
    // , Qt::Widget  // Qt::MaximizeUsingFullscreenGeometryHint
    // );
    ////    browsermanager->adjustSize();
    _browser_docker->set_scrollbars(true);
    // _entrance->setObjectName(entrance_singleton_name);
    // gl_paras->browser_dock(_browser_dock);
    // }

    // todo: Для проверки, почему то в этом месте поиск объекта по имени не
    // работает, разобраться.
    // MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name);
}

void wn_t::setup_signals(void)
{
    // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
    connect(sapp_t::instance(), &QApplication::commitDataRequest, this, &wn_t::commit_data);

    connect(sapp_t::instance(), &QApplication::focusChanged, this, &wn_t::on_focus_changed);

    // Связывание сигналов кнопки поиска по базе с действием по открытию виджета
    // поиска по базе
    connect(_tree_screen->_actionlist[action_find_in_base], &QAction::triggered, gl_paras->window_switcher(), &WindowSwitcher::find_in_base_click);
    // connect(_entrance->getactionFreeze(), &QAction::triggered,
    // globalparameters.getWindowSwitcher(), &WindowSwitcher::findInBaseClick);
    // connect(_table_screen->_find_in_base, &QAction::triggered,
    // globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

    // if(_page_screen)connect(_page_screen->actionFindInBase,
    // &QAction::triggered, globalparameters.window_switcher(),
    // &WindowSwitcher::findInBaseClick);

    // auto hide_others = [this](int index) {
    // if(-1 != index) {
    // auto count = _vtabwidget->count();

    // for(int i = 0; i < count; i++) {
    // if(i != index)_vtabwidget->widget(i)->hide();
    // }

    // _vtabwidget->widget(index)->show();
    // }
    // };

    // {
    // connect(_vtab_record, &HidableTabWidget::currentChanged, this, [&](int
    // index){
    // if(- 1 != index){
    // auto current_record_widget = _vtab_record->widget(index);

    //// for(int i = 0; i < _vtab_record->count(); i++) {
    //// if(i != real_index) {
    //// auto current_widget = _vtab_record->widget(i);
    //// if(current_widget) {
    //// if(current_widget->objectName() == record_screen_multi_instance_name) {
    //// auto record_screen = static_cast<RecordScreen *>(current_widget);
    //// record_screen->browser()->lower();
    //// }

    //// current_widget->hide();
    //// }
    //// }
    //// }
    // rs_t *record_screen = nullptr;
    ////		web::DownloadManager *download_manager = nullptr;
    ////		if(current_record_widget->objectName() ==
    ///download_manager_singleton_name){
    ////		    download_manager = dynamic_cast<web::DownloadManager
    ///*>(current_record_widget);
    ////		    if(download_manager){
    ////			;
    ////			;
    ////		    }
    ////		}else
    // if(current_record_widget->objectName() ==
    // record_screen_multi_instance_name){
    // record_screen = dynamic_cast<rs_t *>(current_record_widget);
    // if(record_screen){
    ////		    _vtab_record->setCurrentWidget(record_screen);

    ////		    record_screen->restore_menubar();

    // auto current_brower = record_screen->browser();
    // if(current_brower && ! current_brower->is_under_construction()){
    // current_brower->raise();
    // current_brower->activateWindow();
    // }
    // }
    //// else{
    //// _tree_screen->restore_menubar();
    //// }
    // }
    // for(int i = 0; i < _vtab_tree->count(); i ++){
    // auto tree_viewer = _vtab_tree->widget(i);
    // if(tree_viewer->objectName() == tree_screen_viewer_name){
    // auto tree_viewer_ = dynamic_cast<tsv_t *>(tree_viewer);
    // if(tree_viewer_){
    // if(tree_viewer_->widget_right() == current_record_widget){
    // if(_vtab_tree->currentIndex() != i){
    // if(! tree_viewer_->tree_screen())tree_viewer_->tree_screen(_tree_screen);
    // _vtab_tree->setCurrentIndex(i);
    // }
    // break;
    // }
    // }
    // }
    // }
    // }
    // });

    // auto tree_viewer_integrity = [&, this](wn_t *_this, HidableTabWidget
    // *_vtab_tree, int index) -> void {
    // auto tree_viewer = _vtab_tree->widget(index);
    // if(tree_viewer->objectName() == tree_screen_viewer_name){
    // // if(real_index < _vtab_record->count()) {
    //// for(int i = 0; i < _vtab_record->count(); i++) {
    //// if(i != real_index) {
    //// auto current_widget = _vtab_record->widget(i);
    //// if(current_widget) {
    //// if(current_widget->objectName() == record_screen_multi_instance_name) {
    //// auto record_screen = static_cast<RecordScreen *>(current_widget);
    //// record_screen->browser()->lower();
    //// }

    //// current_widget->hide();
    //// }
    //// }
    //// }
    // QWidget	*current_record_widget	= nullptr;
    // auto	tree_viewer_		= dynamic_cast<tsv_t *>(tree_viewer);
    // // auto current_widget = _vtab_record->widget(real_index);
    // if(tree_viewer_){
    // QWidget *tree_screen = tree_viewer_->tree_screen();
    // if(! tree_screen)tree_viewer_->tree_screen(_this->_tree_screen);
    // current_record_widget = tree_viewer_->widget_right();
    // if(_this->_vtab_record->currentWidget() !=
    // current_record_widget)_this->_vtab_record->setCurrentWidget(current_record_widget);
    // rs_t *record_screen = dynamic_cast<rs_t *>(tree_viewer_->widget_right());
    // if(record_screen){
    ////			if(_this->_vtab_record->currentWidget() !=
    ///record_screen)_this->_vtab_record->setCurrentWidget(record_screen);
    ////			record_screen->restore_menubar();

    // auto current_brower = record_screen->browser();
    // if(current_brower && ! current_brower->is_under_construction()){
    // current_brower->raise();
    // current_brower->activateWindow();
    // auto browser_tab = current_brower->tabmanager();
    // browser_tab->setCurrentIndex(browser_tab->currentIndex());
    // auto v = browser_tab->currentWebView();
    // if(v){
    // v->setFocus();
    // v->activateWindow();
    // v->raise();
    // v->adjustSize();	// v->repaint();
    // }
    ////			    v->layout()->update();	// activate();
    // }
    // }
    ////		    web::DownloadManager *download_manager = nullptr;
    ////		    if(current_record_widget->objectName() ==
    ///download_manager_singleton_name){
    ////			download_manager = dynamic_cast<web::DownloadManager
    ///*>(current_record_widget);
    ////			if(download_manager){
    ////			    if(_this->_vtab_record->currentWidget() !=
    ///download_manager)_this->_vtab_record->setCurrentWidget(download_manager);
    ////			    ;
    ////			}
    ////		    }
    //// else{
    //// _tree_screen->restore_menubar();
    //// }
    // }
    // }
    // };

    // auto empty_recovery = [&, this](wn_t *_this, HidableTabWidget *_vtab_tree,
    // int &index) -> void {
    // (void) _this;
    // int tree_viewer_count = 0;
    // for(int i = 0; i < _vtab_tree->count(); i ++)
    // if(_vtab_tree->widget(i)->objectName() ==
    // tree_screen_viewer_name)tree_viewer_count ++;
    //// auto count = _vtab_tree->count();
    // if(0 == tree_viewer_count){														//
    // if(1 >= count) {    // self and download
    //// _this->_tree_screen->restore_menubar();
    // index = _vtab_tree->insertTab(0, static_cast<QWidget *>(new tsv_t(_this,
    // _this->_tree_screen, nullptr)),
    // QIcon(":/resource/pic/three_leaves_clover.svg"), "Browser");
    // _vtab_tree->setCurrentIndex(index);
    // }
    // };
    // (void) empty_recovery;
    //// hide_others(_vtabwidget->currentIndex());
    // connect(_vtab_tree, &HidableTabWidget::currentChanged, this, [&, this](int
    // index){
    // if(- 1 != index){
    ////		empty_recovery(this, _vtab_tree, index);

    // tree_viewer_integrity(this, _vtab_tree, index);

    ////// deprecated: ignoring Tree Search Area
    //// if(_vtab_tree->widget(index)->objectName() == tree_screen_singleton_name)
    ///{
    //// appconfig->find_screen_tree_search_area(0);
    //// } else if(_vtab_tree->widget(index)->objectName() ==
    ///record_screen_multi_instance_name) {
    //// appconfig->find_screen_tree_search_area(1);
    //// }
    // }
    // });

    // connect(_vtab_tree, &HidableTabWidget::tabBarClicked, this, [&, this](int
    // index){
    //// if(1 <= index) {
    ////	    empty_recovery(this, _vtab_tree, index);
    // tree_viewer_integrity(this, _vtab_tree, index);
    //// }
    // });
    // }
    ////
    connect(_h_tree_splitter, &QSplitter::splitterMoved, [&](int _tree_pos, int index) {
        (void)index;
        (void)_tree_pos;

        // QList<int>	current_tree_sizes = _h_tree_splitter->sizes();
        // QString _hide_tree_text = current_tree_sizes[0] == 0 ? tr("Show
        // tree view") : tr("Hide tree view");
        // QIcon _hide_tree_icon = current_tree_sizes[0] == 0 ?
        // QIcon(":/resource/pic/butterfly-right.svg") :
        // QIcon(":/resource/pic/butterfly-left.svg");
        // auto widget = _vtab_record->currentWidget();
        // auto _record_screen = widget ? widget->objectName() ==
        // record_screen_multi_instance_name ? static_cast<rs_t *>(widget) :
        // nullptr : nullptr;	//
        // globalparameters.entrance()->activated_browser()->record_screen();
        // auto _record_hide = _record_screen ? _record_screen->_record_hide
        // : nullptr;
        //////	    auto _hide_tree_icon =
        ///_record_screen->_hide_tree_icon;
        //////	    auto _hide_tree_text =
        ///_record_screen->_hide_tree_text;
        ////	    auto old_tree_sizes =
        ///appconfig->h_tree_splitter_sizelist();
        //////	    auto _h_record_splitter = this->h_record_splitter();
        ////	    auto record_sizes = _h_record_splitter->sizes();

        //////	    auto _vtab_tree = this->vtab_tree();
        // auto bar_width = 0;	//
        // _vtab_tree->tabBar()->geometry().width();
        // if(_tree_pos <= bar_width){	// if(bar_width >= sizes[0]){//
        // h_left_splitter->widget(0)->width()
        ////		//		vtab_tree->resize(bar_width,
        ///vtab_tree->height());
        ////		//		vtab_tree->setSizePolicy(QSizePolicy::Minimum,
        ///QSizePolicy::Expanding);
        ////		_vtab_tree->setMinimumWidth(bar_width);	//
        ///vtab_tree->setMaximumWidth(bar_width);
        // if(_record_hide){
        // _hide_tree_icon = QIcon(":/resource/pic/butterfly-right.svg");
        // _hide_tree_text = tr("Show tree view");
        // if(!
        // _record_screen->_toolsline->actions().contains(_record_hide))insert_action_as_button<QToolButton>(_record_screen->_toolsline,
        // _record_screen->_pin, _record_hide);
        // }
        // }else{
        //////		vtab_tree->setSizePolicy(QSizePolicy::Expanding,
        ///QSizePolicy::Expanding);
        ////
        ///_vtab_tree->setMaximumWidth(this->maximumWidth());
        // if(_record_hide){
        // _hide_tree_icon = QIcon(":/resource/pic/butterfly-left.svg");
        // _hide_tree_text = tr("Hide tree view");
        // if(_record_screen->_toolsline->actions().contains(_record_hide))_record_screen->_toolsline->removeAction(_record_hide);
        // }
        // }
        // if(_record_hide){
        // _record_hide->setIcon(_hide_tree_icon);
        // _record_hide->setToolTip(_hide_tree_text);
        // _record_hide->setStatusTip(_hide_tree_text);
        // _record_hide->setText(_hide_tree_text);
        // }
        auto tree_siezes = _h_tree_splitter->sizes();
        ////	    emit
        ///_tree_screen->_actionlist[action_show_hide_record_screen]->triggered();
        // auto summary = record_sizes[0] + record_sizes[1];
        // record_sizes[0] = record_sizes[0] + (tree_siezes[0] -
        // old_tree_sizes[0]);
        // record_sizes[1] = summary - record_sizes[0];
        // _h_record_splitter->setSizes(record_sizes);
        // if(0 != pos){
        // auto tree_siezes = _h_tree_splitter->sizes();
        if (tree_siezes != appconfig->h_tree_splitter_sizelist()) appconfig->h_tree_splitter_sizelist(tree_siezes);
        // }
    });

    //// moved to HidableTabWidget
    // connect(_h_record_splitter, &QSplitter::splitterMoved, [&](int record_pos,
    // int index){
    // auto result = _vtab_record->inner_rebuild_on_splitter_moved(record_pos,
    // index);
    // auto bar_width = std::get<0>(result);
    // auto sizes = std::get<1>(result);
    // if(sizes != appconfig->h_record_splitter_sizelist()	// &&
    // h_record_sizes[0] != bar_width
    // ){
    // if(sizes[0] <= bar_width) ;// appconfig->collapsed(true);
    // else appconfig->h_record_splitter_sizelist(sizes);
    // }
    // });

    _vtab_record->tabBar()->setUsesScrollButtons(true);            // _vtab_tree->tabBar()->setUsesScrollButtons(true);
    connect(_vtab_record->tabBar(), &::QTabBar::tabCloseRequested, // this, &wn_t::on_tabCloseRequested);
        [&](int index) {
            auto count = _vtab_record->count();
            assert(index < count);
            if (index != -1) {
                auto w = _vtab_record->widget(index);
                if (w) {
                    if (w->objectName() == record_screen_multi_instance_name) {
                        auto rs = dynamic_cast<rs_t*>(w);
                        //				if (rs == this) {
                        auto _blogger = rs->blogger();
                        if (_blogger) {
#ifdef USE_SIGNAL_CLOSE
                            _blogger->save();
                            rs->close_sender_id(typeid(HidableTab).name());
                            //						    rs->destroy_trigger_from_others()(_blogger); //rs->close(); //
                            //						    //                                                                        rs->deleteLater();
                            //						    //                                                                        auto rctrl_ = rs->record_ctrl();
                            //						    //                                                                        if (rctrl_) rctrl_->close_requested_from_others(nullptr);
                            auto _browser = rs->browser();
                            //						    //						    if (_browser) _browser->close();
                            if (_browser) {
                                _browser->destroy_trigger_from_others()(_blogger); //browser_->close(); // emit browser_->close_request(browser_);
                                                                                   //									browser_->deleteLater();                        //
                                                                                   // if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
                                                                                   //                                                                                _browser->deleteLater();
                            }
////						    _blogger->destroy_trigger_from_others()(nullptr); //_blogger->close(); //
////						    delete _blogger;                              //	_blogger->deleteLater(); //blogger_->~Blogger();

#else
                            rs->close_sender_id(typeid(HidableTab).name());
                            if (_browser) {
                                _browser->close(); //_browser->close_requested_from_others(nullptr); // emit browser_->close_request(browser_);
                                                   //									browser_->deleteLater();                        //
                                                   // if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
                                                   //										_browser->deleteLater();
                            }

//									_blogger->close(); // emit _blogger->close_request(blogger_);
//									_blogger->deleteLater();//delete _blogger; //delete will fall in sapp_t::exec failing to run
#endif //USE_SIGNAL_CLOSE
                        }
                        //				}
                    } else {
                        w->close();
                        //			                w->deleteLater();
                    }
                    //				_vtab_record->removeTab(index);// move
                    //into HidableTab
                }
                //					w = nullptr;
            }
        });
    connect(_vtab_record, &::HidableTab::currentChanged, [](int index) {
        auto _vtab_record = gl_paras->vtab_record();
        if (gl_paras->vtab_record()->updatesEnabled()) { // if(_vtab_record->updatesEnabled()){
            if (index >= 0 && index < _vtab_record->count()) {
                QWidget* w = nullptr;
                w = _vtab_record->widget(index);
                if (w) {
                    if (w->objectName() == record_screen_multi_instance_name) {
                        auto current_rs = dynamic_cast<rs_t*>(w);
                        if (current_rs) {
                            auto blogger_ = current_rs->blogger();
                            if (blogger_) {
                                auto editor_docker_ = gl_paras->editor_docker();
                                if (editor_docker_) {
                                    if (blogger_ != editor_docker_->widget()) {
                                        blogger_->show();
                                        blogger_->activateWindow();
                                        if (!blogger_->isTopLevel()) blogger_->raise();
                                        if (editor_docker_->widget() != blogger_) editor_docker_->prepend(blogger_);
                                        //									_editor_docker->setWidget(blogger_);
                                        //									blogger_->setParent(_editor_docker);
                                        blogger_->adjustSize();
                                    }
                                }
                            }
                            auto current_bro = current_rs->browser();
                            if (current_bro) {
                                if (current_bro != gl_paras->browser_docker()->widget()) {
                                    // if(_record_screens.find(rs) !=
                                    // _record_screens.end())_record_screens.erase(rs);
                                    current_bro->show();
                                    //							if(!browser->isActiveWindow())
                                    current_bro->activateWindow();
                                    if (!current_bro->isTopLevel()) current_bro->raise();
                                    current_bro->adjustSize();
                                    auto _tab_widget = current_bro->tab_widget();
                                    if (_tab_widget->count() > 0) {
                                        auto v = current_bro->currentTab();
                                        if (v) {
                                            //			                                                if(!v->isActiveWindow())
                                            //									v->activateWindow();
                                            auto p = v->page();
                                            if (p) {
                                                emit p->become_current(p->binder());
                                                auto it = p->host();
                                                if (it) {
                                                    auto tree_screen_ = gl_paras->tree_screen();
                                                    if (tree_screen_) {
                                                        if (tree_screen_->view()->current_item()->id() != it->id()) {
                                                            //
                                                            tree_screen_->view()->select_as_current(TreeIndex::item_require_treeindex([&] { return tree_screen_->view()->source_model(); }, it));
                                                        }
                                                    }
                                                    //													auto to_home = gl_paras->find_screen()->historyhome();
                                                    //													to_home->disconnect();
                                                    //													QObject::connect(
                                                    //													    to_home, &QAction::triggered, v,
                                                    //													    [&](bool checked = true) -> void {
                                                    //														    Q_UNUSED(checked)
                                                    //														    if (p->binder()) {
                                                    //															    auto home = it->field<home_key>();
                                                    //															    QUrl homeurl = QUrl(detail::to_qstring(home));
                                                    //															    if (homeurl.isValid() &&
                                                    //															        homeurl != p->url()) {
                                                    //																    it->field<url_key>(url_value(detail::to_qstring(home)));  // "url",
                                                    //																    p->load(it, true);
                                                    //															    }
                                                    //														    }
                                                    //														});
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            // rs->adjustSize();
                            for (int i = 0; i < _vtab_record->count(); i++) {
                                auto w = _vtab_record->widget(i);
                                if (w) {
                                    if (w->objectName() == record_screen_multi_instance_name) {
                                        auto rs = dynamic_cast<rs_t*>(w);
                                        if (rs && rs != current_rs) {
                                            auto blogger_ = rs->blogger();
                                            if (blogger_) {
                                                blogger_->hide();
                                                if (blogger_->isTopLevel()) blogger_->lower();
                                            }
                                            auto browser_ = rs->browser();
                                            if (browser_) {
                                                browser_->hide();
                                                if (browser_->isTopLevel()) browser_->lower();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                w = nullptr;
            }
        }
    });
}

//
//
//

//   // old version
//  ____________________________________________________________________________________________________________________
// |															|
// |    ________________________________________________      ____________________________________________________	|
// |   |						|    |							  |	|
// |   |	 ___________________   _____________    |    |  ________________________________________________  |	|
// |   |	|		    | |		    |	|    | |						| |	|
// |   |	|		    | |		    |	|    | |     _______________________________________	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |		      _entrance		     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |________________________________________|	| |	|
// |   |	|		    | |		    |	|    | |						| |	|
// |   |	|		    | |		    |	|    | |	       _v_right_splitter		| |	|
// |   |	|		    | |		    |	|    | |     ________________________________________	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|_vtab_tree	    | |_vtab_record |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |		   _editor_screen            |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |					     |	| |	|
// |   |	|		    | |		    |	|    | |    |________________________________________|	| |	|
// |   |	|		    | |		    |	|    | |						| |	|
// |   |	|		    | |		    |	|    | |						| |	|
// |   |	|		    | |		    |	|    | |________________________________________________| |	|
// |   |	|		    | |		    |	|    |							  |	|
// |   |	|		    | |		    |	|    |			   _v_find_splitter               |	|
// |   |	|		    | |		    |	|    |  ________________________________________________  |	|
// |   |	|		    | |		    |	|    | |						| |	|
// |   |	|		    | |		    |	|    | |		   _find_screen			| |	|
// |   |	|___________________| |_____________|	|    | |________________________________________________| |	|
// |   |			                        |    |							  |	|
// |   |		_h_tree_splitter		|    |							  |	|
// |   |________________________________________________|    |____________________________________________________|	|
// |															|
// |					    _h_record_splitter								|
// |____________________________________________________________________________________________________________________|

//   // current version diagram
//  ________________________________________________________________________________________________________________________________________
// |																	    |
// |    ________________________________      __________________________________________________________________________________________    |
// |   |				|    |												|   |
// |   |				|    |	  ______________________      ____________________________________________________	|   |
// |   |				|    |	 |			|    |							  |	|   |
// |   |				|    |	 |		        |    |  ________________________________________________  |	|   |
// |   |				|    |	 |		        |    | |						| |	|   |
// |   |				|    |	 |		        |    | |     _______________________________________	| |	|   |
// |   |				|    |	 |		        |    | |    |					    |	| |	|   |
// |   |				|    |	 |		        |    | |    |					    |	| |	|   |
// |   |				|    |	 |		        |    | |    |					    |	| |	|   |
// |   |				|    |   |		        |    | |    |					    |	| |	|   |
// |   |				|    |   |		        |    | |    |	   _browser_dock		    |	| |	|   |
// |   |				|    |   |		        |    | |    |					    |	| |	|   |
// |   |				|    |   |		        |    | |    |					    |	| |	|   |
// |   |				|    |   |		        |    | |    |_______________________________________|	| |	|   |
// |   |				|    |   |		        |    | |						| |	|   |
// |   |				|    |   |		        |    | |	       _v_right_splitter		| |	|   |
// |   |				|    |   |		        |    | |     ________________________________________	| |	|   |
// |   |				|    |   |		        |    | |    |					     |	| |	|   |
// |   |	    _vtab_tree	        |    |   |	_vtab_record	|    | |    |					     |	| |	|   |
// |   |				|    |   |		        |    | |    |					     |	| |	|   |
// |   |				|    |   |		        |    | |    |					     |	| |	|   |
// |   |				|    |   |		        |    | |    |		   _editor_dock		     |	| |	|   |
// |   |				|    |   |		        |    | |    |					     |	| |	|   |
// |   |				|    |   |		        |    | |    |					     |	| |	|   |
// |   |				|    |   |		        |    | |    |________________________________________|	| |	|   |
// |   |				|    |   |		        |    | |						| |	|   |
// |   |				|    |   |		        |    | |						| |	|   |
// |   |				|    |   |		        |    | |________________________________________________| |	|   |
// |   |				|    |   |		        |    |							  |	|   |
// |   |				|    |   |		        |    |			   _v_find_splitter               |	|   |
// |   |				|    |   |		        |    |  ________________________________________________  |	|   |
// |   |				|    |   |		        |    | |						| |	|   |
// |   |				|    |   |		        |    | |	       _find_screen			| |	|   |
// |   |				|    |   |		        |    | |________________________________________________| |	|   |
// |   |			        |    |   |			|    |							  |	|   |
// |   |		                |    |   |                      |    |							  |	|   |
// |   |				|    |	 |______________________|    |____________________________________________________|	|   |
// |   |				|    |												|   |
// |   |				|    |		       _h_record_splitter							|   |
// |   |________________________________|    |__________________________________________________________________________________________|   |																    |
// |																	    |
// |			    _h_tree_splitter												    |
// |________________________________________________________________________________________________________________________________________|


//
//
//
// follow comments above to learn the "assembly function"
void wn_t::assembly(void)
{
    // v_right_splitter = new QSplitter(Qt::Vertical);
    _v_right_splitter->addWidget(_browser_docker);
    // _v_right_splitter->addWidget(_editor_screen);			// Text entries //
    // Текст записи
    _v_right_splitter->addWidget(_editor_docker);
    _v_right_splitter->setCollapsible(0, true); // if true, make editor can
    // overload it    // The list of
    // final entries can not link up
    // // Список конечных записей не
    // может смыкаться
    _v_right_splitter->setCollapsible(1, false); // The contents of the recording
    // can not link up    //
    // Содержимое записи не может
    // смыкаться
    _v_right_splitter->setObjectName("v_right_splitter");

    // find_splitter = new QSplitter(Qt::Vertical);
    _v_find_splitter->addWidget(
        _v_right_splitter); // findSplitter->addWidget(hSplitter);
    _v_find_splitter->addWidget(_find_screen);
    _v_find_splitter->setCollapsible(0, false); // Верхняя часть не должна смыкаться
    _v_find_splitter->setCollapsible(
        1, false); // Часть для поиска не должна смыкаться
    _v_find_splitter->setObjectName("find_splitter");

    //// _qtabwidget = new QTabWidget(this);
    ////    _vtab_tree->hide();
    ////    _vtab_record->setTabPosition(QTabWidget::West);	//
    ///_vtab_tree->setTabPosition(QTabWidget::West);			// sometime make
    ///"QModelIndex TreeModel::parent(const QModelIndex &index) const" failed.

    //// _vtabwidget->addTab(_tree_screen, QIcon(":/resource/pic/leaves.svg"),
    ///"Tree");

    //// _vtabwidget->addTab(_table_screen, QIcon(":/resource/pic/clover.svg"),
    ///"Candidate");

    //// if(_page_screen)_vtabwidget->addTab(_page_screen,
    ///QIcon(":/resource/pic/three_leaves_clover.svg"), "Page");

    ////    auto index = _vtab_tree->addTab(static_cast<QWidget *>(new
    ///tsv_t(_tree_screen, nullptr)),
    ///QIcon(":/resource/pic/three_leaves_clover.svg"), "Browser");
    // _vtab_record->addTab(static_cast<QWidget *>(_download),
    // QIcon(":/resource/pic/apple.svg"), "Download");	//
    // _vtab_tree->addTab(static_cast<QWidget *>(new tsv_t(this, _tree_screen,
    // _download)), QIcon(":/resource/pic/apple.svg"), "Download");			//
    // QIcon(":/resource/pic/holly.svg")

    ////    _vtab_tree->setCurrentIndex(index);

    appconfig->find_screen_tree_search_area(
        0); // force to root_item of global tree

    // v_left_splitter = new QSplitter(
    // Qt::Horizontal  // Qt::Vertical
    // );

    // int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 100
    // int vtab_g_width = _vtabwidget->geometry().width();         // 100
    // int this_width = geometry().width();                        // 640
    // int download_width = _download->geometry().width();         // 1089
    // int tree_screen_width = _tree_screen->geometry().width();   // 100
    // int find_splitter_width = _v_find_splitter->geometry().width(); // 640
    // int left_splitter_width = _h_left_splitter->geometry().width(); // 640

    // if(_vtabwidget->frameGeometry().width() ==
    // _h_left_splitter->geometry().width()) {
    // _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100,
    // _vtabwidget->geometry().height());
    // }

    _h_record_splitter->setObjectName("h_record_splitter");
    _h_record_splitter->addWidget(
        _vtab_record); // _h_record_splitter->addWidget(_h_tree_splitter);
    _h_record_splitter->addWidget(_v_find_splitter);
    _h_record_splitter->setCollapsible(0, false);
    _h_record_splitter->setCollapsible(1, false);

    //
    _h_tree_splitter->addWidget(
        _tree_screen); // _h_tree_splitter->addWidget(_vtab_tree);
    _h_tree_splitter->addWidget(
        _h_record_splitter); // _h_tree_splitter->addWidget(_vtab_record);

    // v_left_splitter->addWidget(treeScreen);
    // v_left_splitter->addWidget(recordTableScreen);
    _h_tree_splitter->setCollapsible(0, true);
    _h_tree_splitter->setCollapsible(1, false);
    // v_left_splitter->setCollapsible(1, false);
    _h_tree_splitter->setObjectName("h_tree_splitter");
    //
    // {
    // auto sizes = _h_right_splitter->sizes();
    // if(_globalparameters.entrance()->browsers().size() == 0) { //
    // h_right_splitter->widget(0)->width()
    // sizes[0] = 0;
    // sizes[1] = sizes[0] + sizes[1];
    // _h_right_splitter->setSizes(sizes);

    // }
    // emit _h_right_splitter->splitterMoved(sizes[0], 1);
    // }

    // gl_paras->v_right_splitter(_v_right_splitter);
    // gl_paras->find_splitter(_v_find_splitter);
    // gl_paras->h_tree_splitter(_h_tree_splitter);
    // gl_paras->h_record_splitter(_h_record_splitter);

    // _vtab_tree->setMinimumWidth(_vtab_tree->tabBar()->geometry().width());
    // _vtab_tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // _h_splitter->addWidget(_tree_screen);

    // hSplitter = new QSplitter(Qt::Horizontal);
    // _h_splitter->addWidget(_h_left_splitter);
    // hSplitter->addWidget(treeScreen);             // Tree branches    // Дерево
    // веток
    // hSplitter->addWidget(recordTableScreen);      // The list of final entries
    // // Список конечных записей

    // _h_splitter->addWidget(_find_splitter); //hSplitter->addWidget(vSplitter);

    // _h_splitter->setCollapsible(0, false);            // Дерево веток не может
    // смыкаться
    // _h_splitter->setCollapsible(1, false);            // Столбец со списком и
    // содержимым записи не может смыкаться
    // _h_splitter->setObjectName("hsplitter");

    // connect(find_splitter, &QSplitter::splitterMoved, browser_entrance,
    // &web::Entrance::on_splitter_moved);
    // connect(find_splitter, &QSplitter::splitterMoved, recordTableScreen,
    // &RecordTableScreen::on_splitter_moved);

    // findSplitter=new QSplitter(Qt::Vertical);
    // findSplitter->addWidget(hSplitter);
    // findSplitter->addWidget(findScreenDisp);
    // findSplitter->setCollapsible(0,false);        // Верхняя часть не должна
    // смыкаться
    // findSplitter->setCollapsible(1,false);        // Часть для поиска не должна
    // смыкаться
    // findSplitter->setObjectName("find_splitter");

    QHBoxLayout* tree_layout = new QHBoxLayout();
    tree_layout->setMargin(0);
    tree_layout->setContentsMargins(0, 0, 0, 0);
    tree_layout->setSpacing(0);
    tree_layout->addWidget(_h_tree_splitter);
    QVBoxLayout* central_ayout = new QVBoxLayout(_central_widget);
    central_ayout->setMargin(0);
    central_ayout->setContentsMargins(0, 0, 0, 0);
    central_ayout->setSpacing(0);

    central_ayout->addLayout(tree_layout);
    _central_widget->setLayout(central_ayout);
    setCentralWidget(_central_widget); // setCentralWidget(_h_record_splitter);//
    // setCentralWidget(_h_splitter);
    // //setCentralWidget(findSplitter);

    _central_widget->adjustSize();
    // auto	record_sizes	= _h_record_splitter->sizes();	//
    // _h_tree_splitter->sizes();
    // auto	bar_width		= _vtab_record->minimumSizeHint().width();	//
    // tabBar()->geometry().width();	//
    // _vtab_tree->tabBar()->geometry().width();
    // auto	bar_width_s		= _vtab_record->sizeHint().width();//24
    // auto	bar_width_t		= _vtab_record->tabBar()->sizeHint().width();//
    // 0
    //// auto ww = _h_left_splitter->widget(0)->width(); // 100 != 0 when sizes[0]
    ///== 0
    // if(bar_width >= record_sizes[0]){			//
    // _h_left_splitter->widget(0)->width()
    // auto vtab_record_min_width = _vtab_record->minimumSizeHint().width();	//
    // auto vtab_tree_min_width = _vtab_tree->minimumSizeHint().width();		//
    // _tree_screen->minimumSizeHint().width();                 //
    // globalparameters.entrance()->activated_browser()->record_screen()->minimumSizeHint().width();
    // // 6xx   // h_right_splitter->widget(0)->width();    // 0    //
    // sizeHint().width();    // 23
    //// auto h = h_right_splitter->handle(1);
    //// h->move(lr + shw, h->rect().top());

    // auto	size_memory	= appconfig->h_record_splitter_sizelist();//
    // appconfig->h_tree_splitter_sizelist();
    // auto	sum			= size_memory[0] > 0 ? size_memory[0] :
    // this->geometry().width() * 15 / 100 + size_memory[1] > 0 ? size_memory[1] :
    // this->geometry().width() * 85 / 100;
    // record_sizes[0]	= size_memory[0] > vtab_record_min_width ? size_memory[0]
    // < sum ? size_memory[0] : sum * 15 / 100 : vtab_record_min_width;
    // record_sizes[1]	= sum - record_sizes[0] > 0 ? sum - record_sizes[0] : sum
    // * 85 / 100;					// sizes[1] > size_memory[1] ? size_memory[1] :
    // sizes[1];
    ////	_vtab_tree->setMaximumWidth(maximumWidth());
    ////	_vtab_tree->setSizePolicy(QSizePolicy::Expanding,
    ///QSizePolicy::Expanding);// h_left_splitter->moveSplitter(sizes[0], 1);   //
    ///protected member
    // _h_record_splitter->setSizes(record_sizes);	//
    // _h_tree_splitter->setSizes(tree_sizes);	// emit
    // _h_tree_splitter->splitterMoved(sizes[0], 1);

    //// auto s_0 = _vtab_tree->minimumSizeHint();   // (146, 146)
    //// auto s_1 = _vtab_record->minimumSizeHint(); // (25, 146)
    //// auto s_2 = _entrance->minimumSizeHint();
    //// auto s_3 = _h_right_splitter->minimumSizeHint();    // (241,146)
    //// auto s_4 = _h_right_splitter->maximumWidth();    // (241,146)
    //// auto sizes_check = _h_left_splitter->sizes();
    //// h_right_splitter->resize(h_right_splitter->sizeHint().width(),
    ///h_right_splitter->height());
    // }
}

void wn_t::save_all_state(void)
{
    Blogger* currrent_blogger = nullptr;
    // Сохранение данных в поле редактирования
    for (int i = 0; i < _vtab_record->count(); i++) {
        auto wd = _vtab_record->widget(i);
        auto rs = dynamic_cast<rs_t*>(wd);
        if (rs) {
            auto blogger_ = rs->blogger();
            if (rs == _vtab_record->currentWidget()) currrent_blogger = blogger_;
            if (blogger_ && !blogger_->close_request_sent() && !blogger_->destroy_request_sent()) {
                blogger_->save_text_context(); // auto bro = rs->browser(); if (bro && !bro->close_request_sent() && !bro->destroy_request_sent()) bro->save();
                blogger_->save_editor_cursor_position();
                blogger_->save_editor_scrollbar_position();
            }
        }
    }
    if (currrent_blogger) {
        auto id = currrent_blogger->misc_field<id_key>(); // static_cast<id_value>(_control_tab->topic());//
        if (detail::to_string(id) == "") {
            auto bro = currrent_blogger->browser();
            if (bro) {
                auto v = bro->currentTab();
                if (v) {
                    auto p = v->page();
                    if (p) {
                        auto h = p->host();
                        if (h) {
                            currrent_blogger->misc_field<id_key>(h->id());
                            id = currrent_blogger->misc_field<id_key>();
                        }
                    }
                }
            }
        }
        assert(detail::to_string(id) != "");
        //		if (detail::to_string(id) == "") {
        //			id = currrent_blogger->browser()->currentTab()->page()->host()->id();
        //			currrent_blogger->misc_field<id_key>(id);
        //			qDebug() << "Blogger::current id : " << id << " _control_tab->topic() : " << currrent_blogger->control_tab()->topic();
        //		}
        auto drop_flag_status = walkhistory.drop_flag();
        walkhistory.drop_flag(true);
        walkhistory.add<WALK_HISTORY_GO_NONE>(
            id,                                      //static_cast<id_value>(_control_tab->topic()), // id
            currrent_blogger->cursor_position(),     // _editor_screen->cursor_position()
            currrent_blogger->scrollbar_position()); // _editor_screen->scrollbar_position());
        walkhistory.drop_flag(drop_flag_status);
    }
    // Сохраняются данные сеанса работы
    save_geometry();
    save_tree_position();
    // save_recordtable_position();

    // Синхронизируется с диском конфиг программы
    appconfig->sync();
}

HidableTab* wn_t::vtab_record() const
{
    return _vtab_record;
}

ts_t* wn_t::tree_screen() const
{
    return _tree_screen;
}

QMenu* wn_t::file_menu() const
{
    return _filemenu;
}

QMenu* wn_t::edit_menu() const
{
    return _editmenu;
}

QMenu* wn_t::view_menu() const
{
    return _viewmenu;
}

web::HistoryMenu* wn_t::histry_menu() const
{
    return _histrymenu;
}

web::BookmarksMenu* wn_t::bookmark_menu() const
{
    return _bookmarkmenu;
}

QMenu* wn_t::window_menu() const
{
    return _windowmenu;
}

QMenu* wn_t::tools_menu() const
{
    return _toolsmenu;
}

QMenu* wn_t::help_menu() const
{
    return _helpmenu;
}

QSplitter* wn_t::find_splitter() const
{
    return _v_find_splitter;
}

QSplitter* wn_t::h_record_splitter() const
{
    return _h_record_splitter;
}

QSplitter* wn_t::h_tree_splitter() const
{
    return _h_tree_splitter;
}

// Слот, срабатывающий когда происходит выход из оконной системы
void wn_t::commit_data(QSessionManager& manager)
{
    Q_UNUSED(manager);
    qDebug() << "Session manager send commit data signal.";

    application_fast_exit();
}

// Восстанавливается геометрия окна и позиции основных разделителей
void wn_t::restore_geometry(void)
{
    if (gl_paras->target_os() == "android") setWindowState(Qt::WindowMaximized); // Для Андроида окно просто
    // разворачивается на весь экран
    else {
        QRect rect = appconfig->mainwin_geometry();
        resize(rect.size());
        move(rect.topLeft());
    }
    // move(rect.topLeft());
    // resize(rect.size());

    _v_right_splitter->setSizes(appconfig->v_right_splitter_sizelist());
    // auto size_v_right_splitter = _v_right_splitter->size();         // (792,
    // 614)
    // auto sizelist_v_right_splitter = _v_right_splitter->sizes();    // (614, 0)

    _v_find_splitter->setSizes(appconfig->findsplitter_sizelist());
    // auto size_v_find_splitter = _v_find_splitter->size();           // (792,
    // 646)
    // auto sizelist_v_find_splitter = _v_find_splitter->sizes();      // (614,
    // 32)

    // _h_splitter->setSizes(appconfig->hspl_sizelist());
    _h_record_splitter->setSizes(appconfig->h_record_splitter_sizelist());
    // auto size_h_right_splitter = _h_right_splitter->size();         // (851,
    // 646)
    // auto sizelist_h_right_splitter = _h_right_splitter->sizes();    // (0, 851)

    auto sizelist_h_left_splitter = appconfig->h_tree_splitter_sizelist(); // (146, 494)?
    _h_tree_splitter->setSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Maximum); // ShrinkFlag  // Minimum
    _h_tree_splitter->setSizes(sizelist_h_left_splitter);
    // auto size_h_left_splitter = _h_left_splitter->size();           // (1366,
    // 646)
    sizelist_h_left_splitter = _h_tree_splitter->sizes(); // (312, 1054)

    // int vtab_base = _vtabwidget->baseSize().width();            // 0
    // int vtab_frame = _vtabwidget->frameSize().width();          // 1118
    // int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 1118

    // auto vtab_size_hint = _vtab_tree->sizeHint();                   // (352,
    // 220)
    // int vtab_tree_max_width = _vtab_tree->geometry().width();       // 312  //
    // 1118

    // int this_width = geometry().width();                        // 1366
    // int download_width = _download->geometry().width();         // 1089
    // int tree_screen_width = _tree_screen->geometry().width();   // 236
    // int find_splitter_width = _v_find_splitter->geometry().width(); // 0
    // int left_splitter_size_width = _h_left_splitter->size().width(); // 1124
    // int left_splitter_geometry_width = _h_left_splitter->geometry().width();
    // // 1124
    // int right_splitter_width = _h_right_splitter->geometry().width(); // 0

    // if(0 == find_splitter_width // _vtabwidget->frameGeometry().width() ==
    // _h_left_splitter->geometry().width()
    // ) {
    // _download->resize(vtab_g_width * 15 / 100,
    // _vtabwidget->geometry().height());
    // _vtabwidget->resize(vtab_g_width * 15 / 100,
    // _vtabwidget->geometry().height());
    // _entrance->resize(vtab_g_width * 85 / 100,
    // _v_find_splitter->geometry().height());
    // _editor_screen->resize(vtab_g_width * 85 / 100,
    // _v_find_splitter->geometry().height());
    // _find_screen->resize(vtab_g_width * 85 / 100,
    // _v_find_splitter->geometry().height());
    // _v_find_splitter->resize(vtab_g_width * 85 / 100,
    // _v_find_splitter->geometry().height());   //
    // _h_right_splitter->resize(vtab_g_width * 85 / 100,
    // _v_find_splitter->geometry().height());   //

    // }

    // download_width = _download->geometry().width();         // 1089
    // vtab_g_width = _vtabwidget->geometry().width();                 // 167
    // find_splitter_width = _v_find_splitter->geometry().width();     // 950
    // right_splitter_width = _h_right_splitter->geometry().width();   // 950
    // _h_right_splitter->move(_h_right_splitter->x() - vtab_g_width * 85 / 100,
    // _h_right_splitter->y());

    // _h_right_splitter->repaint();
    // _h_left_splitter->repaint();

    auto record_sizes = _h_record_splitter->sizes(); // _h_tree_splitter->sizes();
    // QList<int> new_sizes;
    auto sum = record_sizes[0] + record_sizes[1];
    auto vtab_record_min = _vtab_record->minimumSizeHint(); // _vtab_tree->minimumSizeHint();
    // // (146, 146) //
    // _tree_screen->minimumSizeHint().width();
    if (record_sizes[0] == sum) {
        record_sizes[0] = vtab_record_min.width();
        record_sizes[1] = sum - record_sizes[0]; // vtab_tree_max_width
    }
    // for(auto sz : sizes) {
    // if(sz == 0) {
    // sz = vtab_g_width * 85 / 100;
    // } else if(sz == vtab_g_width) {
    // sz = vtab_g_width * 15 / 100;
    // }

    // new_sizes << sz;
    // }

    _h_record_splitter->setSizes(
        record_sizes); // _h_tree_splitter->setSizes(record_sizes);
                       // // new_sizes

    // _v_right_splitter->setSizes(appconfig->vspl_sizelist());
    // _h_splitter->setSizes(appconfig->hspl_sizelist());
    // _h_left_splitter->setSizes(appconfig->v_leftsplitter_sizelist());
    // _v_find_splitter->setSizes(appconfig->findsplitter_sizelist());
    // this->repaint();
}

// Запоминается геометрия окна и позиции основных разделителей
void wn_t::save_geometry(void)
{
    qDebug() << "Save window geometry and splitter sizes";

    QRect geom(pos(), size());

    appconfig->mainwin_geometry(geom.x(), geom.y(), geom.width(), geom.height());

    // tuteconfig.set_mainwingeometry(geometry().x(), geometry().y(),
    // geometry().width(), geometry().height());

    appconfig->v_right_splitter_sizelist(_v_right_splitter->sizes());
    // appconfig->hspl_sizelist(_h_splitter->sizes());
    appconfig->h_record_splitter_sizelist(_h_record_splitter->sizes());
    appconfig->h_tree_splitter_sizelist(_h_tree_splitter->sizes());
    // Запоминается размер сплиттера только при видимом виджете поиска,
    // т.к. если виджета поиска невидно, будет запомнен нуливой размер
    // if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
    // данный метод вызывается из декструктора главного окна, и к этому моменту
    // виджет уже невиден
    if (appconfig->findscreen_show()) appconfig->findsplitter_sizelist(_v_find_splitter->sizes());
}

void wn_t::restore_tree_position(void)
{
    // Путь к последнему выбранному в дереве элементу
    auto pair = appconfig->tree_position();
    QString current_root_id = pair.first;
    QStringList current_item_absolute_path = pair.second; // appconfig->get_tree_position();

    qDebug() << "MainWindow::restoreTreePosition() : "
             << current_item_absolute_path;

    set_tree_position(current_root_id, current_item_absolute_path);
}

// save
void wn_t::save_tree_position(void)
{
    auto _current_source_model = [&]() {
        return _tree_screen->view()->source_model();
    };
    // if(!_tree_screen->sysynchronized())_tree_screen->synchronize();
    auto item = _tree_screen->view()->session_root_auto(); // item([ =
    // ](boost::intrusive_ptr<const
    // TreeItem> t) {return t->id() ==
    // _tree_screen->session_root_id();});
    //// Получение QModelIndex выделенного в дереве элемента
    // const QModelIndex index = _tree_screen->tree_view()->current_index();
    auto current_item = _tree_screen->view()->current_item();
    auto root_id = _current_source_model()->root_item()->id();
    auto conf = appconfig->tree_position();
    if (current_item) {

        auto current_id = current_item->path_list<id_key>();

        if (conf.first != root_id || conf.second != current_id)
            appconfig->tree_position(root_id, current_id); // _tree_screen->know_model_board()->root_item()->id()
    } else if (item) {

        auto current_id = item->path_list<id_key>(); // if(index.isValid()) {

        ////    if(index.isValid()) {   // this line is to be remove
        //// Получаем указатель вида TreeItem
        // auto item = _current_source_model()->item(index);

        // Сохраняем путь к элементу item
        if (conf.first != root_id || conf.second != current_id)
            appconfig->tree_position(root_id, current_id); // _tree_screen->know_model_board()->root_item()->id()

        // }
    }
}

// set
void wn_t::set_tree_position(QString current_root_id, QStringList current_item_absolute_path)
{
    // _tree_screen->session_root_id(current_item_absolute_path.last());

    auto source_model = [&]() { return _tree_screen->view()->source_model(); };
    auto know_model_board = [&]() {
        return _tree_screen->view()->know_model_board();
    };
    if (source_model()->root_item()->id() != current_root_id) {
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
        // TreeIndex(know_model_board, know_model_board()->item([&]
        // (boost::intrusive_ptr<const TreeItem> it) {
        // return it->id() == current_root_id;})); } catch(std::exception &e) {throw
        // e; } return tree_index; } ();
        auto it = know_model_board()->item([&](boost::intrusive_ptr<const i_t> it) {
            return it->id() == current_root_id;
        });
        _tree_screen->view()->intercept(it); // TreeIndex::instance(know_model_board, it, it->parent())
    }
    // if(!_tree_screen->know_model_board()->item(current_item_absolute_path))
    // // on know_root semantic
    // return;

    // Получаем указатель на элемент вида TreeItem, используя путь
    auto it = know_model_board()->item(current_item_absolute_path); // on know_root semantic

    auto _tree_view = _tree_screen->view();
    if (it) {
        if (!static_cast<QModelIndex>(source_model()->index(it)).isValid()) {
            // boost::intrusive_ptr<TreeIndex> tree_index = [&]
            // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
            // TreeIndex(know_model_board, it); } catch(std::exception &e) {throw e; }
            // return tree_index; } ();
            _tree_view->cursor_focus(it);
            _tree_screen->view()->intercept(
                it); // TreeIndex::instance(know_model_board, it, it->parent())
        }
        if (it != know_model_board()->root_item()) {
            qDebug() << "Set tree position to " << it->field<name_key>() << " id "
                     << it->field<id_key>();

            //// Из указателя на элемент TreeItem получаем QModelIndex
            // QModelIndex setto = source_model()->index(item);

            // Курсор устанавливается в нужную позицию

            boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::item_require_treeindex(
                [&] { return _tree_view->source_model(); }, it);
            // try {tree_index = new TreeIndex([&] {return tree_view->source_model();
            // }, it->parent(), it->parent()->sibling_order([&]
            // (boost::intrusive_ptr<const Linker> il) {
            // return il == it->linker() && il->host() == it && it->parent() ==
            // il->host_parent();
            // })); } catch(std::exception &e) {throw e; }

            _tree_view->select_as_current(tree_index);
            _tree_view->source_model()->session_id(tree_index); // TreeIndex(source_model, it)
        } else {
            boost::intrusive_ptr<i_t> r(nullptr);
            if (it->count_direct() == 0)
                r = _tree_view->tree_empty_controll();
            else
                r = it->child_direct(0);
            boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::item_require_treeindex(
                [&] { return _tree_view->source_model(); }, r);
            // try {tree_index = new TreeIndex([&] {return tree_view->source_model();
            // }, it, 0); } catch(std::exception &e) {throw e; }

            _tree_view->select_as_current(tree_index);
            _tree_view->source_model()->session_id(tree_index); // TreeIndex(source_model, it)
        }
    }
}

bool wn_t::is_tree_position_crypt()
{
    bool result = false;
    auto pair = appconfig->tree_position();
    QString id = pair.first;
    QStringList path = pair.second;

    auto know_model_board = [&]() {
        return _tree_screen->view()->know_model_board();
    };
    if (_tree_screen->view()->source_model()->root_item()->id() != id) {
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
        // TreeIndex(know_model_board, know_model_board()->item([&]
        // (boost::intrusive_ptr<const TreeItem> it) {
        // return it->id() == id;})); } catch(std::exception &e) {throw e; } return
        // tree_index; } ();
        auto it = know_model_board()->item(
            [&](boost::intrusive_ptr<const i_t> it) { return it->id() == id; });

        _tree_screen->view()->intercept(
            it); // TreeIndex::instance(know_model_board, it, it->parent())
    }
    // if(_tree_screen->know_model_board()->is_item_valid(path) == false) return
    // false;

    // Получаем указатель на элемент вида TreeItem, используя путь
    auto item = know_model_board()->item(path);
    if (item) {
        if (item->field<crypt_key>() == crypt_value(true)) result = true;
        // else
        // return false;
    }
    return result;
}

// too many _record_screen objects, deprecated
// void MainWindow::restore_recordtable_position(void)
// {
// QString id = appconfig->get_recordtable_selected_record_id();

// if(id.length() > 0)
// select_id(id);
// }

// too many _record_screen objects, deprecated
// void MainWindow::save_recordtable_position(void)
// {
// QString id = _table_screen->first_selection_id();

// appconfig->set_recordtable_selected_record_id(id);
// }

// void MainWindow::select_id(QString id)
// {
// _table_screen->select_id(id);
// }

// void wn_t::restore_editor_cursor_position(void){
// int n = appconfig->editor_cursor_position();

//// _editor_screen->cursor_position(n);
// _editor_dock->blog_editor()->cursor_position(n);
// }

// void wn_t::restore_editor_scrollbar_position(void){
// int n = appconfig->editor_scroll_bar_position();
//// _editor_screen->scrollbar_position(n);
// _editor_dock->blog_editor()->scrollbar_position(n);
// }

void wn_t::restore_find_in_base_visible(void)
{
    bool n = appconfig->findscreen_show();

    // Определяется ссылка на виджет поиска
    FindScreen* find_screen = gl_paras
                                  ->find_screen(); // find_object<FindScreen>(find_screen_singleton_name);
    if (n)
        find_screen->show();
    else
        find_screen->hide();
}

// Создание раздела меню File
void wn_t::init_file_menu(void)
{
    // Создание меню
    // _filemenu = new QMenu(tr("&File"), this);
    _filemenu->clear();
    //	menuBar()->addMenu(_filemenu);

    //// Создание тулбара
    /// *
    // QToolBar *tb = new QToolBar(this);
    // tb->setWindowTitle(tr("File Actions"));
    // addToolBar(tb);
    // */

    // QAction *a;

    /// *
    // a = new QAction(tr("&New"), this);
    // a->setShortcut(QKeySequence::New);
    // connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    //// tb->addAction(a);
    // menu->addAction(a);

    // a = new QAction(tr("&Open..."), this);
    // a->setShortcut(QKeySequence::Open);
    // connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    //// tb->addAction(a);
    // menu->addAction(a);

    // menu->addSeparator();

    // a = new QAction(tr("&Save"), this);
    // a->setShortcut(QKeySequence::Save);
    // connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    // a->setEnabled(false);
    //// tb->addAction(a);
    // menu->addAction(a);

    // a = new QAction(tr("Save &As..."), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    // menu->addAction(a);
    // menu->addSeparator();
    // */
    {
        // move to SuperMenu
        // a = new QAction(tr("&Print..."), this);
        // a->setShortcut(QKeySequence::Print);
        // connect(a, &QAction::triggered, this, &wn_t::file_print);
        //// tb->addAction(a);
        // _filemenu->addAction(a);

        // a = new QAction(tr("Print Preview..."), this);
        // connect(a, &QAction::triggered, this, &wn_t::file_print_preview);
        // _filemenu->addAction(a);

        // a = new QAction(tr("&Export PDF..."), this);
        // a->setShortcut(Qt::CTRL + Qt::Key_D);
        // connect(a, &QAction::triggered, this, &wn_t::file_print_pdf);
        //// tb->addAction(a);
        // _filemenu->addAction(a);

        // _filemenu->addSeparator();
    }
    // a = new QAction(tr("&Quit"), this);
    // a->setShortcut(Qt::CTRL + Qt::Key_Q);
    // connect(a, SIGNAL(triggered()), this, SLOT(applicationExit()));
    // _filemenu->addAction(a);
}

// Создание раздела меню Tools
void wn_t::init_tools_menu(void)
{
    _toolsmenu->clear();
    // Создание меню
    // _toolsmenu = new QMenu(tr("&Tools"), this);
    //	menuBar()->addMenu(_toolsmenu);

    QAction* a;

    a = new QAction(tr("Find in ba&se"), this);
    connect(a, &QAction::triggered, this, &wn_t::find_bar_switch);
    a->setShortcut(QKeySequence(tr("Ctrl+K", "Web Search")));
    _toolsmenu->addAction(a);

    auto b = new QAction(tr("Editor open/close"), this);
    connect(b, &QAction::triggered, [&](bool checked) {
        (void)checked;
        //			if(checked){//
        if (!(_editor_docker->isVisible())) {
            _editor_docker->show();
            // emit _app->editing_win_changed(_editenty->blog_editor());
            // emit this->editing_activated(blog_editor());
            // _app->sendEvent(_editenty->blog_editor(), new
            // QEvent(QEvent::WindowActivate));

            appconfig->editor_show(true);
        } else {
            _editor_docker->hide();
            // _app->sendEvent(_editenty->blog_editor(), new
            // QEvent(QEvent::WindowActivate));

            appconfig->editor_show(false);
        }
    });
    _toolsmenu->addAction(b);

    _toolsmenu->addSeparator();
    if (appconfig->interface_mode() == "desktop") {
        a = new QAction(tr("Main &preferences"), this);
        connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
        _toolsmenu->addAction(a);
    } else {
        // Создание подменю
        QMenu* menu = new QMenu(tr("Main &preferences"), this);
        init_preferences_menu(menu);
    }
    _main_menu_map[gl_para::_tools_menu_name] = _toolsmenu;
}

// Заполнение подраздела меню Preferences
void wn_t::init_preferences_menu(QMenu* menu)
{
    QAction* a;

    a = new QAction(tr("Main"), this);
    connect(a, &QAction::triggered, this, &wn_t::find_bar_switch);
    menu->addAction(a);

    a = new QAction(tr("Crypt"), this);
    connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
    menu->addAction(a);

    a = new QAction(tr("Synchro"), this);
    connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
    menu->addAction(a);

    a = new QAction(tr("RecordTable"), this);
    connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
    menu->addAction(a);

    a = new QAction(tr("Misc"), this);
    connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
    menu->addAction(a);
}

// Создание раздела меню Help
void wn_t::init_help_menu(void)
{
    _helpmenu->clear();
    // Создание меню
    // _helpmenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(_helpmenu);

    QAction* a;

    a = new QAction(tr(std::string("About " + program_title_string).c_str()), this);
    connect(a, &QAction::triggered, this, &wn_t::on_click_help_about_this);
    _helpmenu->addAction(a);

    a = new QAction(tr("About Qt"), this);
    connect(a, &QAction::triggered, this, &wn_t::on_click_help_about_qt);
    _helpmenu->addAction(a);

    _helpmenu->addAction(tr("About &Embedded Browser"), this, &wn_t::slotAboutApplication);
}

// Новая коллекция
void wn_t::file_new(void) {}

// Открыть коллекцию
void wn_t::file_open(void) {}

// Сохранить текущую статью
bool wn_t::file_save(void)
{
    return true;
}

// Сохранить текущую статью как файл
bool wn_t::file_save_as(void)
{
    return true;
}

// EditingWindow *wn_t::current_editing_window(){
// EditingWindow *ew = nullptr;
// auto rs = dynamic_cast<rs_t *>(_vtab_record->currentWidget());
// if(rs) ew = rs->editing_window();
// return ew;
// }
//// Напечатать текущую статью
// void wn_t::file_print(void){
// #ifndef QT_NO_PRINTER
// QPrinter printer(QPrinter::HighResolution);
// printer.setFullPage(true);

// QPrintDialog *dlg = new QPrintDialog(&printer, this);
// dlg->setWindowTitle(tr("Print Document"));
// if(dlg->exec() == QDialog::Accepted){
//// _editor_screen->textarea_document()->print(&printer);
// auto ew = current_editing_window();
// if(ew) ew->textarea_document()->print(&printer);
// }
// delete dlg;
// #endif
// }

//// Предпросмотр печати текущей статьи
// void wn_t::file_print_preview(void){
// auto ew = current_editing_window();
// if(ew){
// PrintPreview *preview = new PrintPreview( //
// _editor_screen->textarea_document()
// ew->textarea_document(), this);

// preview->setModal(true);
// preview->setAttribute(Qt::WA_DeleteOnClose);
// preview->show();
// }
// }

//// Вывод текущей статьи в PDF файл
// void wn_t::file_print_pdf(void){
// #ifndef QT_NO_PRINTER
// QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
// QString(), "*.pdf");
// if(!fileName.isEmpty()){
// if(QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");
// QPrinter printer(QPrinter::HighResolution);
// printer.setOutputFormat(QPrinter::PdfFormat);
// printer.setOutputFileName(fileName);
//// _editor_screen->textarea_document()->print(&printer);
// auto ew = current_editing_window();
// if(ew) ew->textarea_document()->print(&printer);
// }
// #endif
// }

// Слот - Нормальный выход из программы
void wn_t::application_exit(void)
{
    sapp_t::instance()->saveSession(); // save_all_state();
    // Если в конфиге настроено, что нужно синхронизироваться при выходе
    // И задана команда синхронизации
    if (appconfig->synchro_on_exit())
        if (appconfig->synchro_command().trimmed().length() > 0) synchronization();
    // Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}

// Быстрый выход из программы, без возможности синхронизации
void wn_t::application_fast_exit(void)
{
    sapp_t::instance()->saveSession(); // save_all_state();

    // Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}

void wn_t::find_bar_switch(void)
{
    // Определяется ссылка на виджет поиска
    FindScreen* _find_screen = gl_paras->find_screen(); // find_object<FindScreen>(find_screen_singleton_name);
    if (!(_find_screen->isVisible())) {
        _find_screen->show();
        web::ToolbarSearch* toolbarsearch = static_cast<web::ToolbarSearch*>(_find_screen->lineedit_stack()->currentWidget());
        if (toolbarsearch) {
            auto line = toolbarsearch->lineEdit();
            if (line) line->selectAll();
            toolbarsearch->setFocus();
        }
    } else
        _find_screen->hide();
}

void wn_t::tools_preferences(void)
{
    // Создается окно настроек, после выхода из этой функции окно удалится
    AppConfigDialog appconfigdialog(
        "pageMain"); // gl_paras->main_window()->browser(QString(gl_para::_what_ever_topic))->tabmanager()->record_ctrl(),

    appconfigdialog.show();
}

// Слот, срабатывающий при нажатии в редакторе кнопки "распахнуть"
// Флаг показывает, что нужно сделать
// true - распахнуть область отводимую редактору
// false - сделать область, отводимую редактору, обычной
void wn_t::on_expand_edit_area(bool flag)
{
    // static QSize entrance_size = globalparameters.entrance()->size();
    // static QSize tree_size = globalparameters.entrance()->size();
    // static QSize recordtable_size = globalparameters.entrance()->size();
    // static QSize vtab_size = globalparameters.entrance()->size();
    // static bool _treetable_hidden;     // =
    // globalparameters.getTreeScreen()->isHidden();
    // static bool recordtable_hidden; // =
    // globalparameters.getRecordTableScreen()->isHidden();
    if (flag) {
        gl_paras->browser_docker()->hide(); // resize(QSize(0, 0)); //

        // if(!globalparameters.getTreeScreen()->isHidden()) {
        // _treetable_hidden = false;
        ////            globalparameters.getTreeScreen()->resize(QSize(0,
        ///tree_size.height()));
        // globalparameters.getTreeScreen()->hide();    // resize(QSize(0, 0)); //
        // } else {
        // _treetable_hidden = true;
        // }

        // if(!globalparameters.getRecordTableScreen()->isHidden()) {
        // _recordtable_hidden = false;
        ////            globalparameters.getRecordTableScreen()->resize(QSize(0,
        ///recordtable_size.height()));
        // globalparameters.getRecordTableScreen()->hide(); // resize(QSize(0, 0));
        // //
        // } else {
        // _recordtable_hidden = true;
        // }

        emit _vtab_record->_hide_action->setChecked(
            true); // emit _vtab_tree->_hide_action->setChecked(true);
                   // QTabWidget *tab = globalparameters.vtab();
                   // tab->currentWidget()->hide();

        // auto * tab_pane = tab->findChild<QStackedWidget *>();
        // tab_pane->hide();

        // QRect rect = tab->geometry();
        // tab->setGeometry(rect.left(), rect.top(), 0, rect.height());

        // setGeometry(rect.top(), rect.left(), 0, rect.height());

        // resize(QSize(0, v_left_splitter->height())); // hide();
    } else {
        gl_paras->browser_docker()->show(); // resize(entrance_size); //

        // if(!_treetable_hidden) {
        // globalparameters.getTreeScreen()->show();    // resize(tree_size); //
        // }

        // if(!_recordtable_hidden) {
        // globalparameters.getRecordTableScreen()->show(); //
        // resize(recordtable_size); //
        // }

        // emit _vtabwidget->hideAction.toggle();
        emit _vtab_record->_hide_action->setChecked(false); // emit _vtab_tree->_hide_action->setChecked(false);
                                                            // globalparameters.vtab()->resize(vtab_size); // show();
    }
}

void wn_t::on_click_help_about_this(void)
{
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

    infoProgramName = "<b>" + program_title_qstring + "</b> - smart manager<br/>for information collecting<br/><br/>";
    infoVersion = "v." + QString(app_version) + "<br/><br/>";
    infoAuthor = "Author: Hugh Von Young<br/>";
    infoEmail = "Author Email:<i>hughvonyoung@gmail.com</i><br/>";
    infoLicense = "GNU General Public License v.3.0<br/><br/>";
    infoTargetOs = "Target OS: " + gl_paras->target_os() + "<br/>";
    infoProgramFile = "Program file: " + gl_paras->main_program_full_file() + "<br/>";
    infoWorkDirectory = "Work directory: " + gl_paras->root_path() + "<br/>";

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    infoDevicePixelRatio = "Device pixel ratio: " + (QString::number(sapp_t::instance()->devicePixelRatio(), 'f', 8)) + "<br/>";
    infoPhysicalDpi = "Physical DPI (from screen): " + (QString::number(QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 8)) + "<br/>";
#endif

    infoPhysicalDpiX = "Physical DPI X (from desktop): " + (QString::number(sapp_t::instance()->desktop()->physicalDpiX(), 'f', 8)) + "<br/>";
    infoPhysicalDpiY = "Physical DPI Y (from desktop): " + (QString::number(sapp_t::instance()->desktop()->physicalDpiY(), 'f', 8)) + "<br/>";

    QString info = infoProgramName + infoVersion + infoAuthor + infoEmail + infoLicense + infoTargetOs + infoProgramFile + infoWorkDirectory + infoDevicePixelRatio + infoPhysicalDpi + infoPhysicalDpiX + infoPhysicalDpiY;

    auto msgBox = std::make_shared<QMessageBox>(this);
    msgBox->about(this, QString(program_title) + " v." + app_version, info);
}

void wn_t::on_click_help_about_qt(void)
{
    auto msgBox = std::make_shared<QMessageBox>(this);

    msgBox->aboutQt(this);
}

void wn_t::synchronization(void)
{
    // Сохраняются данные в поле редактирования
    for (int i = 0; i < _vtab_record->count(); i++) {
        auto wd = _vtab_record->widget(i);
        auto rs = dynamic_cast<rs_t*>(wd);
        if (rs) {
            auto ew = rs->blogger();
            ew->save_text_context();
            ew->save_editor_cursor_position();
            ew->save_editor_scrollbar_position();
        }
    }
    // Сохраняются данные о курсорах в дереве и таблице конечных записей
    save_tree_position();
    // save_recordtable_position();

    // Считывается команда синхронизации
    QString command = appconfig->synchro_command();
    // Если команда синхронизации пуста
    if (command.trimmed().length() == 0) {
        QMessageBox::warning(this, tr(std::string(std::string(program_title) + ": can't synchronization").c_str()), tr("Do not set synchronization command.<br>Check the "
                                                                                                                       "setting in \"Syncro\" section in \"Tools\" menu"),
            QMessageBox::Close);

        return;
    }
    // Макрос %a заменяется на путь к директории базы данных
    // QString
    // databasePath=globalParameters.getWorkDirectory()+"/"+appconfig->datadir();
    QDir databaseDir(gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName());
    QString databasePath = databaseDir.canonicalPath();

    command.replace("%a", databasePath);

    // Запуск команды синхронизации
    ExecuteCommand cons;
    cons.setWindowTitle(tr(std::string(std::string(program_title) + " synchronization").c_str()));
    cons.setMessageText(tr("Synchronization in progress, please wait..."));

    cons.setCommand(command);
    cons.run();

    bool drop_flag_status = walkhistory.drop_flag();
    // Блокируется история
    walkhistory.drop_flag(true);

    // Заново считываются данные в дерево
    _tree_screen->view()->know_model_reload();
    restore_tree_position();
    // restore_recordtable_position();
    for (int i = 0; i < _vtab_record->count(); i++) {
        auto wd = _vtab_record->widget(i);
        auto rs = dynamic_cast<rs_t*>(wd);
        if (rs) {
            auto blogger_ = rs->blogger();
            blogger_->restore_editor_cursor_position();
            blogger_->restore_editor_scrollbar_position();
        }
    }
    // азблокируется история посещений элементов
    walkhistory.drop_flag(drop_flag_status);
}

// void wn_t::create_tray_icon(void){
////	_tray_icon_menu = new QMenu(this);
////	_tray_icon_menu->addAction(_action_tray_restore);
////	_tray_icon_menu->addAction(_action_tray_maximize);
////	_tray_icon_menu->addAction(_action_tray_minimize);
////	_tray_icon_menu->addSeparator();
////	_tray_icon_menu->addAction(_action_tray_quit);

// _tray_icon = new SysTrayIcon(_tree_screen, _browser_dock, _vtab_record,
// _find_screen, _editor_dock, this, _profile, _style_source);
////	_tray_icon->setContextMenu(_tray_icon_menu);
// }

// void wn_t::set_icon(void){
// connect(_tray_icon, &SysTrayIcon::activated, this, &wn_t::icon_activated);

// QIcon icon = QIcon(":/resource/pic/logo.svg");
// _tray_icon->setIcon(icon);
// setWindowIcon(icon);

//// tray_icon->setToolTip(iconComboBox->itemText(index));
// }

void wn_t::icon_activated(QSystemTrayIcon::ActivationReason reason)
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) return;
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isVisible()) {
            if (isMinimized())
                showNormal();
            else
                hide();
        } else {
            if (isMinimized())
                showNormal();
            else
                show();
        }
        break;
    default:;
    }
}

// Слот закрытия окна
void wn_t::closeEvent(QCloseEvent* event)
{
    if (_enable_real_close == false) {
        if (!QSystemTrayIcon::isSystemTrayAvailable()) return;
        // При приходе события закрыть окно, событие игнорируется
        // и окно просто делается невидимым. Это нужно чтобы при закрытии окна
        // программа не завершала работу
        if (_tray_icon->isVisible()) {
            hide();
            event->ignore();
        }
    }
}

bool wn_t::eventFilter(QObject* o, QEvent* e)
{
    Q_UNUSED(o);
    // qDebug() << "Event: " << e->type();
    // Отлавливание потери фокуса
    // QEvent::ActivationChange
    if (e->type() == QEvent::WindowDeactivate) {
        qDebug() << "Main window focus deactivate, save all state.";
        sapp_t::instance()->saveSession(); // save_all_state();
        return true;
    }
    return false; // Продолжать оработку событий дальше
}

// void wn_t::go_walk_history_previous(void){
//// _editor_screen->save_textarea();
// _editor_dock->blog_editor()->save_textarea();
// id_value id = id_value( // _editor_screen->misc_field("id")
// _editor_dock->blog_editor()->misc_field("id"));
// walkhistory.add<WALK_HISTORY_GO_PREVIOUS>(id
// , _editor_dock->blog_editor()->cursor_position() //
// _editor_screen->cursor_position()
// , _editor_dock->blog_editor()->scrollbar_position() //
// _editor_screen->scrollbar_position()
// );
// walkhistory.set_drop(true);

// go_walk_history();
// }

// void wn_t::go_walk_history_next(void){
//// _editor_screen->save_textarea();
// _editor_dock->blog_editor()->save_textarea();
// id_value id = id_value( // _editor_screen->misc_field("id")
// _editor_dock->blog_editor()->misc_field("id"));
// walkhistory.add<WALK_HISTORY_GO_NEXT>(id
// , _editor_dock->blog_editor()->cursor_position() //
// _editor_screen->cursor_position()
// , _editor_dock->blog_editor()->scrollbar_position() //
// _editor_screen->scrollbar_position()
// );
// walkhistory.set_drop(true);

// go_walk_history();
// }

// void wn_t::go_walk_history(void){
//// QString tree_root_id = walkhistory.tree_root_id();

//// if(_tree_screen->know_root()->root_item()->id() != tree_root_id) {
//// _tree_screen->intercept(tree_root_id);
//// }

//// Выясняется идентификатор записи, на которую надо переключиться
// auto record_id = walkhistory.record_id();

//// if(record_id.length() == 0) {
//// walkhistory.set_drop(false);
//// return;
//// }
// auto _tree_view = _tree_screen->view();
// auto know_model_board = std::bind(&tv_t::know_model_board, _tree_view); //
// [&](){return _tree_screen->view()->know_model_board();};
// if(static_cast<QString>(record_id).length() > 0){
//// Выясняется путь к ветке, где находится данная запись
// QStringList absolute_path = static_cast<tkm_t
// *>(know_model_board())->path_list(record_id); // on know_root semantic

////// Проверяем, есть ли такая ветка
//// if(_tree_screen->know_model_board()->is_item_valid(absolute_path) == false)
///{    // on know_root semantic
//// walkhistory.set_drop(false);
//// return;
//// }

//// Выясняется позицию записи в таблице конечных записей
// auto item = know_model_board()->item(absolute_path); // on know_root semantic
// if(item && item != know_model_board()->root_item()){
////// Проверяем, есть ли такая позиция
//// if(!item->item_direct(record_id)) {  // == false
//// walkhistory.set_drop(false);
//// return;
//// }
//// if(item->child_direct(record_id)){//?
// set_tree_position(global_root_id, absolute_path);
//// select_id(id);
// if(appconfig->remember_cursor_at_history_navigation()){
//// _editor_screen->cursor_position(walkhistory.cursor_position(record_id));
// _editor_dock->blog_editor()->cursor_position(walkhistory.cursor_position(record_id));
////
///_editor_screen->scrollbar_position(walkhistory.scrollbar_position(record_id));
// _editor_dock->blog_editor()->scrollbar_position(walkhistory.scrollbar_position(record_id));
// }
//// }
// auto url_target = item->field<url_type>();
//// walkhistory.set_drop(false);
// TreeIndex::activate([&] {return _tree_view->source_model();},
// _tree_view->current_item(), item->field<url_type>(), std::bind(&tv_t::move,
// _tree_view, std::placeholders::_1, std::placeholders::_2,
// std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const
// TreeItem> it_) -> bool {return
// url_equal(it_->field<home_type>().toStdString(), url_target.toStdString()) ||
// url_equal(it_->field<url_type>().toStdString(), url_target.toStdString());});
// }
//// else {
//// walkhistory.set_drop(false);
//// }
// }
// walkhistory.set_drop(false);
// }

//// Метод, вызываемый из всех точек интерфейса, в которых происходит
//// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
//// текст редактируемой записи
// void wn_t::save_text_context(void){
// id_value id = id_value( // _editor_screen->misc_field("id")
// _editor_dock->blog_editor()->misc_field("id"));

// qDebug() << "MainWindow::saveTextarea() : id :" << id;

//// _editor_screen->save_textarea();
// _editor_dock->blog_editor()->save_textarea();
// walkhistory.add<WALK_HISTORY_GO_NONE>(id
// , _editor_dock->blog_editor()->cursor_position() //
// _editor_screen->cursor_position()
// , _editor_dock->blog_editor()->scrollbar_position()); //
// _editor_screen->scrollbar_position());
// }

// Слот, обрабатывающий смену фокуса на виджетах
void wn_t::on_focus_changed(QWidget* widgetFrom, QWidget* widgetTo)
{
    Q_UNUSED(widgetFrom);
    if (widgetTo == nullptr) return;
    // qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

    return; // Временно ничего не делает
}

web::DownloadManager* wn_t::download_screen() const
{
    // int browser_size_ = 0;
    // for(int i = 0; i < count(); i ++)
    // if(widget(i)->objectName() ==
    // record_screen_multi_instance_name)browser_size_ ++;

    // return browser_size_;
    web::DownloadManager* result = nullptr;
    for (int i = 0; i < _vtab_record->count(); i++) { // for(auto i = _record_screens.begin(); i !=
        // _record_screens.end(); i ++){
        auto w = _vtab_record->widget(i);
        if (w->objectName() == download_manager_singleton_name) {
            auto downloader_ = dynamic_cast<web::DownloadManager*>(w);
            if (downloader_) {
                result = downloader_;
                break;
            }
        }
    }
    return result; // _record_screens;
}

std::set<web::Browser*> wn_t::browsers() const
{
    // int browser_size_ = 0;
    // for(int i = 0; i < count(); i ++)
    // if(widget(i)->objectName() ==
    // record_screen_multi_instance_name)browser_size_ ++;

    // return browser_size_;
    std::set<web::Browser*> result;
    for (int i = 0; i < _vtab_record->count(); i++) { // for(auto i = _record_screens.begin(); i !=
        // _record_screens.end(); i ++){
        auto w = _vtab_record->widget(i);
        if (w->objectName() == record_screen_multi_instance_name) {
            auto rs = dynamic_cast<rs_t*>(w);
            // auto	browser_	= dynamic_cast<rs_t *>(w)->browser();
            if (rs) {
                auto bro = rs->browser();
                if (bro)
                    if (result.find(bro) == result.end()) result.insert(bro); // if(*i){	// && *i != widget()=>for entrance
            }
        }
    }
    return result; // _record_screens;
}

web::Browser* wn_t::dumy_browser() const
{
    web::Browser* result = nullptr;
    for (auto bro : browsers()) {
        auto tabs = bro->tab_widget();
        if (tabs->count() == 1 && tabs->webView(0)->page()->binder()->host()->field<url_key>() == web::Browser::_defaulthome) {
            result = bro;
            break;
        }
    }
    return result;
}

web::WebView* wn_t::find(const std::function<bool(boost::intrusive_ptr<const ::Binder>)>& _equal)
    const
{
    // clean();
    web::WebView* v = nullptr;
    //// new_dockedwindow(record);
    // for(auto i : _record_screens){
    // if(i){
    // v = i->browser()->tabWidget()->find(_equal);
    // if(v != nullptr)break;
    // }
    // }
    for (int i = 0; i < _vtab_record->count(); i++) {
        auto w = _vtab_record->widget(i);
        if (w->objectName() == record_screen_multi_instance_name) {
            // auto	rs		= dynamic_cast<rs_t *>(w);
            auto browser_ = dynamic_cast<rs_t*>(w)->browser();
            if (browser_) {
                v = browser_->tab_widget()->find(_equal);
                if (v != nullptr) break;
            }
        }
    }
    boost::intrusive_ptr<const i_t> found_myself(nullptr);
    if (v) {
        if (v->page()) {
            boost::intrusive_ptr<::Binder> binder = v->page()->binder();
            if (binder) {
                auto _this_item = v->page()->host(); // globalparameters.entrance()->find(_equal);
                if (_this_item) {
                    if (binder->integrity_external(_this_item, v->page())) {
                        // assert(_this_item == v->page()->binder()->item());
                        // if(_this_item && v->load_finished())																			// &&
                        // (v->tabmanager()->currentWebView() == v)
                        found_myself = _this_item;
                    }
                }
            }
        }
    }
    if (!found_myself) v = nullptr;
    return v;
}

// Blogger *wn_t::editing_window(const QString &topic, const QByteArray
// &state_){
//	return new Blogger(_vtab_record, _profile, _editor_docker,
//_style_source, appconfig->hide_editor_tools(), state_, topic);
//}


//// not sure to succeeded if force is false
//template <>
//web::Browser*
//wn_t::browser<QUrl>(const QUrl& url_, bool force)
//{
//	(void)force;

//	return real_url_t<url_value>::instance<web::Browser*>(url_value(url_.toString()),
//	    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) {
//		    return browser<boost::intrusive_ptr<real_url_t<url_value>>>(real_target_url_, force);
//	    });
//}


url_value wn_t::query_internet(const QString& search_text)
{
#ifdef USE_STRINGLIST_MODEL
    QStringList newList = _stringlistmodel->stringList();
    if (newList.contains(search_text))
        newList.removeAt(newList.indexOf(search_text));
    newList.prepend(search_text);
    if (newList.size() >= web::ToolbarSearch::_maxsavedsearches)
        newList.removeLast();
    if (!::sapp_t::instance()->privateBrowsing()) {
        _stringlistmodel->setStringList(newList);
        _autosaver->changeOccurred();
    }
#endif // USE_STRINGLIST_MODEL

    QUrl search_engine(QLatin1String("https://www.google.com/search"));
    QUrlQuery url_query;

    // url_query.addQueryItem(QLatin1String("q"), searchText);
    url_query.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
    url_query.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
    url_query.addQueryItem(
        QLatin1String("client"),
        QLatin1String(gl_paras->application_name().toLatin1())); // globalparameters.main_program_file().toLatin1()

    // urlQuery.addQueryItem();

    search_engine.setQuery(url_query);
    search_engine.setFragment("q=" + search_text);
    // emit search(url, std::bind(&TreeScreen::view_paste_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //                        url_value real_url =
    return url_value(search_engine.toString());
}

// not sure to succeeded if force is false
template <>
web::Browser*
wn_t::browser<QByteArray>(const QByteArray& state_, bool force)
{
    (void)force;
    size_t topic_number = static_cast<size_t>(appconfig->topic_number());
    if (browsers().size() > topic_number) shrink(topic_number > 0 ? topic_number - 1 : topic_number);
    auto state_data = web::Browser::state(state_);
    auto topic = std::get<3>(state_data);
    auto title = std::get<2>(state_data);
    web::Browser* bro = nullptr; //(dumy_browser());

    auto real_topic = (topic != gl_para::_default_topic) ? topic : (title != gl_para::_default_topic) ? title : gl_para::_default_topic;
    //	bro = browser<QString>(topic, false);
    //	if (!bro)

    bro = (new Blogger(real_topic, gl_para::_default_post, appconfig->hide_editor_tools(), state_))->browser();

    assert(bro);
    return bro;
}

// not sure to succeeded if force is false
template <>
web::Browser*
wn_t::browser<QStringList>(const QStringList& tags_list_,
    bool force)
{
    (void)force;
    size_t topic_number = static_cast<size_t>(appconfig->topic_number());
    if (browsers().size() > topic_number) shrink(topic_number > 0 ? topic_number - 1 : topic_number);

    auto tags_list = tags_list_; // QStringList(item->field<tags_key>());
    web::Browser* bro = nullptr; //(dumy_browser());

    if (tags_list.size() > 0) {
        for (int i = 0; i < tags_list.size(); ++i) {
            auto item_topic = (tags_list[i] = tags_list.at(i).trimmed());
            bro = real_url_t<QString>::instance<web::Browser*>(item_topic, [&](boost::intrusive_ptr<real_url_t<QString>> topic) {
                return browser<boost::intrusive_ptr<real_url_t<QString>>>(topic);
            });
            if (bro) break;
        }
        if (!bro) bro = real_url_t<QString>::instance<web::Browser*>(tags_list[0], [&](boost::intrusive_ptr<real_url_t<QString>> topic) {
            return browser<boost::intrusive_ptr<real_url_t<QString>>>(topic);
        });
    }
    if (!bro && force) {
        bro = real_url_t<QString>::instance<web::Browser*>(gl_para::_default_topic, [&](boost::intrusive_ptr<real_url_t<QString>> topic) {
            return browser<boost::intrusive_ptr<real_url_t<QString>>>(topic);
        });
    }

    assert(bro);
    return bro;
}


template <>
web::Browser*
wn_t::browser<boost::intrusive_ptr<i_t>>(const boost::intrusive_ptr<i_t>& it,
    bool force)
{
    (void)force;
    size_t topic_number = static_cast<size_t>(appconfig->topic_number());
    if (browsers().size() > topic_number) shrink(topic_number > 0 ? topic_number - 1 : topic_number);

    boost::intrusive_ptr<i_t> item = it;

    auto tags_list = QStringList(item->field<tags_key>());

    auto view = find(
        [&](boost::intrusive_ptr<const Binder> b) { return b->host() == item; });
    if (item->field<url_key>() == url_value("")) {
        item->field<url_key>(web::Browser::_defaulthome);
    }
    web::Browser* bro = nullptr; //(dumy_browser());
    if (view) {
        bro = view->browser();
    } else if (tags_list.size() > 0) {
        bro = browser(tags_list);
    } else if (item->field<url_key>() != url_value("")) {
        bro = real_url_t<url_value>::instance<web::Browser*>(item->field<url_key>(), [&](boost::intrusive_ptr<real_url_t<url_value>> url_) {
            return browser<boost::intrusive_ptr<real_url_t<url_value>>>(url_);
        });
    }
    assert(bro);
    return bro;
}

// not sure to succeeded if force is false
template <>
web::Browser*
wn_t::browser<boost::intrusive_ptr<real_url_t<url_value>>>(const boost::intrusive_ptr<real_url_t<url_value>>& real_find_url_, bool force)
{
    (void)force;
    size_t topic_number = static_cast<size_t>(appconfig->topic_number());
    if (browsers().size() > topic_number) shrink(topic_number > 0 ? topic_number - 1 : topic_number);
    boost::intrusive_ptr<real_url_t<url_value>> dummy = real_find_url_;
    (void)dummy;
    auto real_url = real_find_url_->value();
    if (real_url == url_value("")) {
        real_url = web::Browser::_defaulthome;
    }

    auto item = TreeIndex::url_require_item_from_tree(
        real_find_url_, std::bind(&tv_t::move, _tree_screen->view(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const i_t> it) -> bool {
            return url_equal(url_value(detail::to_qstring(it->field<home_key>())), real_url) || url_equal(it->field<url_key>(), real_url);
        });
    web::Browser* bro = nullptr; //(dumy_browser());

    if (item) {
        auto tags_list = QStringList(item->field<tags_key>());

        auto view = find(
            [&](boost::intrusive_ptr<const Binder> b) { return b->host() == item; });
        if (item->field<url_key>() == url_value("")) {
            item->field<url_key>(real_url);
        }

        if (view) {
            bro = view->browser();
        } else if (tags_list.size() > 0) {
            bro = browser(tags_list);
        } else if (item->field<url_key>() != url_value("")) {

            auto tab_sate = [&](boost::intrusive_ptr<i_t> it) {
                int version = 1;
                QByteArray data;
                QDataStream stream(&data, QIODevice::WriteOnly);

                stream << qint32(web::TabWidget::TabWidgetMagic);
                stream << qint32(version);

                QStringList tabs_url;
                tabs_url.append(detail::to_qstring(item->field<url_key>()));

                stream << tabs_url;
                stream << it->field<id_key>(); // currentIndex();

                return data;
            };
            auto bro_state = [&](boost::intrusive_ptr<i_t> it) {
                int version = 2;
                QByteArray data;
                QDataStream stream(&data, QIODevice::WriteOnly);
                QString topic = gl_para::_default_topic;
                stream << qint32(web::Browser::browser_magic);
                stream << qint32(version);

                stream << topic;
                stream << detail::to_qstring(it->field<name_key>());
                stream << _browser_docker->size();
                stream << true;  // menuBar()->isVisible();
                stream << true;  //!_find_screen->isHidden();
                stream << false; //!_bookmarkstoolbar->isHidden();
                stream << true;  //!gl_paras->status_bar()->isHidden();
                stream << tab_sate(it);
                return data;
            };
            auto state_ = bro_state(item);
            bro = (new Blogger(gl_para::_default_topic, gl_para::_default_post, appconfig->hide_editor_tools(), state_))->browser();
        }
    }

    assert(bro);
    return bro;
}


// not sure to succeeded if force is false
template <>
web::Browser*
wn_t::browser<boost::intrusive_ptr<real_url_t<QString>>>(const boost::intrusive_ptr<real_url_t<QString>>& topic_, bool force) //const QString& topic_,
{
    size_t topic_number = static_cast<size_t>(appconfig->topic_number());
    if (browsers().size() > topic_number) shrink(topic_number > 0 ? topic_number - 1 : topic_number);
    // clean();
    QString topic = topic_->value();
    if ("" == topic) topic = gl_para::_default_topic;

    // std::pair<Browser *, WebView *> dp = std::make_pair(nullptr, nullptr);
    web::Browser* bro = nullptr; //(dumy_browser());
//	if (!bro) {
#ifdef USE_CURRENT_BROWSER_KEY_WORD
    assert(!((topic == gl_para::_current_browser) && (force)));
    //	auto try_real_url = to_be_url(topic);

    //	if (try_real_url != QUrl() && try_real_url != detail::to_qstring(web::Browser::_defaulthome)) {
    //		browser_ = static_cast<web::ToolbarSearch*>(_find_screen->lineedit_stack()->currentWidget())->search_now(topic);
    //	} else
    if (topic == gl_para::_current_browser) {
        auto rs = _vtab_record->currentWidget();
        if (rs)
            browser_ = dynamic_cast<rs_t*>(rs)->browser();
        else {
            //			int count_browser = 0;
            for (int i = 0; i < _vtab_record->count();
                 i++) { // for(auto i : _record_screens){
                auto rs = _vtab_record->widget(i);
                if (rs->objectName() == record_screen_multi_instance_name) {
                    // auto	rs		= dynamic_cast<rs_t *>(w);
                    auto bro_ = dynamic_cast<rs_t*>(rs)->browser();
                    if (bro_) {
                        if (rs == _vtab_record->currentWidget()) { //_vtab_record->currentWidget()){  //
                            //browser_->isVisible() ||
                            //browser_->isActiveWindow()
                            browser_ = bro_; // .data();

                            break;
                        }
                    }
                    //					count_browser++;
                }
            }
        }
    } else {
#else
    int count_browser = 0;
    for (int i = 0; i < _vtab_record->count(); i++) { // for(auto i : _record_screens){
        auto rs = _vtab_record->widget(i);
        if (rs->objectName() == record_screen_multi_instance_name) {
            // auto	rs		= dynamic_cast<rs_t *>(w);
            auto bro_ = dynamic_cast<rs_t*>(rs)->browser();
            if (bro_) {
                auto blogger_ = bro_->blogger();
                if (blogger_) {
                    if (blogger_->topic() == topic) { //|| topic ==
                        //gl_para::_what_ever_topic//_vtab_record->currentWidget()){
                        //// browser_->isVisible() || browser_->isActiveWindow()
                        bro = bro_; // .data();

                        break;
                    }
                }
            }
            count_browser++;
        }
    }
#endif // USE_CURRENT_BROWSER_KEY_WORD
#ifdef USE_CURRENT_BROWSER_KEY_WORD
    }
#endif // USE_CURRENT_BROWSER_KEY_WORD

    if (!bro) {
        auto state = DiskHelper::get_topic_from_directory(gl_paras->root_path() + "/" + gl_para::_blog_root_dir, topic);
        if (state != QByteArray())
            bro = browser<QByteArray>(state);
    }
    if (!bro && force) {
        auto checked_topic = topic;
        //		if(topic == gl_para::_what_ever_topic) checked_topic =
        //gl_para::_default_topic;
        bro = (new Blogger(checked_topic))->browser();
    }

    assert(bro || !force); //	assert(bro);
    return bro;            // qobject_cast<DockedWindow *>(widget()); //
}


std::map<std::string, QMenu*>& wn_t::main_menu_map()
{
    return _main_menu_map;
}

void wn_t::slotAboutApplication()
{
    QMessageBox::about(
        this, tr("About"), tr("Version %1"
                              "<p>This demo demonstrates the facilities "
                              "of Qt WebEngine in action, providing an note with "
                              "browser for you to experiment with.<p>"
                              "<p>Qt WebEngine is based on the Chromium open source project "
                              "developed at <a "
                              "href=\"http://www.chromium.org/\">http://www.chromium.org/</a>.")
                               .arg(QCoreApplication::applicationVersion()));
}

void wn_t::synchronize_title(const QString& title_)
{
    if (title_ != "") {
        if (!this->windowTitle().contains(title_))
            this->setWindowTitle(
                tr(QString(QString("%1 - ") + program_title).toStdString().c_str())
                    .arg(title_));
    } else
        this->setWindowTitle(program_title);
}


//void wn_t::on_tabCloseRequested(int index)
//{
//	auto count = _vtab_record->count();
//	assert(index < count);
//	if (index != -1) {
//		auto w = _vtab_record->widget(index);
//		if (w) {
//			if (w->objectName() == record_screen_multi_instance_name) {
//				auto rs = dynamic_cast<rs_t*>(w);
//				//				if (rs == this) {
//				auto _blogger = rs->blogger();
//				if (_blogger) {
//#ifdef USE_SIGNAL_CLOSE
//					rs->close_sender_id(typeid(HidableTab).name());
//					//									rs->close_requested_from_others(nullptr); //rs->close(); //
//					//                                                                        rs->deleteLater();
//					//                                                                        auto rctrl_ = rs->record_ctrl();
//					//                                                                        if (rctrl_) rctrl_->close_requested_from_others(nullptr);
//					_blogger->close_requested_from_others(nullptr); //_blogger->close(); //
//					delete _blogger;                              //	_blogger->deleteLater(); //blogger_->~Blogger();
//////                                                                        if (_browser) {
//////                                                                                _browser->close_requested_from_others(_blogger); //browser_->close(); // emit browser_->close_request(browser_);
//////                                                                                                   //									browser_->deleteLater();                        //
//////                                                                                                   // if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
//////                                                                                                   //                                                                                _browser->deleteLater();
//////                                                                        }
//#else
//					rs->close_sender_id(typeid(HidableTab).name());
//					if (_browser) {
//						_browser->close(); //_browser->close_requested_from_others(nullptr); // emit browser_->close_request(browser_);
//								   //									browser_->deleteLater();                        //
//								   // if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
//								   //										_browser->deleteLater();
//					}

////									_blogger->close(); // emit _blogger->close_request(blogger_);
////									_blogger->deleteLater();//delete _blogger; //delete will fall in sapp_t::exec failing to run
//#endif //USE_SIGNAL_CLOSE
//				}
//				//				}
//			} else {
//				w->close();
//				//			                w->deleteLater();
//			}
//			//				_vtab_record->removeTab(index);// move
//			//into HidableTab
//		}
//		//					w = nullptr;
//	}
//}

size_t wn_t::shrink(const size_t bar)
{
    size_t result = bar;
    auto downloader_ = download_screen();
    if (downloader_) _vtab_record->removeTab(_vtab_record->indexOf(downloader_));
    std::set<web::Browser*> browsers = wn_t::browsers();

    //	rs_t* cur_rs = nullptr;
    web::Browser* cur_bro = nullptr;
    boost::intrusive_ptr<i_t> current(nullptr);
    //	for (auto bro : browsers) {
    //		auto _record_screen = bro->record_screen();
    //		auto web_view = bro->currentTab();
    //		if (_record_screen && web_view) {

    //			if (_record_screen == gl_paras->vtab_record()->currentWidget()) {
    //				cur_rs = _record_screen;
    //				auto page = web_view->page();
    //				auto _rctrl = _record_screen->record_ctrl();
    //				if (_rctrl && page) {
    //					auto _record_view = _rctrl->view();
    //					auto host = page->host();
    //					if (_record_view && host) {
    //						//						if (_record_screen == cur_rs)
    //						current = _record_view->current_item();
    //						if (!current) current = host;
    //						assert(current);
    //						break;
    //					}
    //				}
    //			}
    //		}
    //	}
    for (auto bro : browsers) {
        auto _record_screen = bro->record_screen();
        auto web_view = bro->currentTab();
        if (_record_screen && web_view) {

            if (_record_screen == gl_paras->vtab_record()->currentWidget()) {
                //				cur_rs = _record_screen;
                cur_bro = bro;
                auto page = web_view->page();
                auto _rctrl = _record_screen->record_ctrl();
                if (_rctrl && page) {
                    auto _record_view = _rctrl->view();
                    auto host = page->host();
                    if (_record_view && host) {
                        //						if (_record_screen == cur_rs)
                        current = _record_view->current_item();
                        if (!current) current = host;
                        assert(current);
                        break;
                        ////	    auto	_proxy_model	=
                        ///_record_controller->proxy_model();
                        ////	    auto _source_model = _record_controller->source_model();
                        ////	    _view->setSortingEnabled(true);
                        ////	    _proxy_model->setSortRole(SORT_ROLE);
                        // std::vector<web::WebView *> v_list;
                        // for(int index_ = 0; index_ < count(); index_ ++)
                        // v_list.push_back(webView(index_));
                        // std::sort(v_list.begin(), v_list.end(), [&](web::WebView *v0,
                        // web::WebView *v1){
                        // auto p0 = v0 != *v_list.end() ? v0->page() : nullptr;
                        // auto p1 = v1 != *v_list.end() ? v1->page() : nullptr;
                        // return p0 ? p1 ? p0->host()->field<rating_type>().toULongLong() >
                        // p1->host()->field<rating_type>().toULongLong() ||
                        // TreeIndex::is_ancestor_of(current, p0->host()) : true : false;
                        // });
                        ////	    int t = 0;
                        ////	    for(auto v : v_list){
                        ////		_source_model->move(pos_source(webViewIndex(v)),
                        ///pos_source(t));
                        ////		t ++;
                        ////	    }
                        ////	    _view->reset();
                        ////	    _proxy_model->setSourceModel(_source_model);
                        ////	    _view->setModel(_proxy_model);
                        ////	    _view->setSortingEnabled(false);
                        ////	    _proxy_model->setSortRole(Qt::InitialSortOrderRole);
                        ////	    _proxy_model->invalidate();
                        ////
                        ///_record_controller->select_as_current(_record_controller->index<pos_proxy>(_source_model->index(current)));
                        ////		// Сохранение дерева веток
                        ////		// find_object<TreeScreen>(tree_screen_singleton_name)
                        ////	    globalparameters.tree_screen()->view()->know_model_save();
                        ////	    _record_screen->tools_update();
                        //
                        //
                        //
                        // for(int i = 0; i < count(); i ++){	// for(std::vector<web::WebView
                        // *>::size_type i = 20; i < static_cast<std::vector<web::WebView
                        // *>::size_type>(count()); i ++){
                        // auto v = webView(i);	// v_list[i];
                        // if(v){
                        // auto p = v->page();
                        // if(p){
                        // auto h = p->host();
                        // if(  h
                        ////		      && ! TreeIndex::is_ancestor_of(current, p->host())
                        // && (! h->is_ancestor_of(current) || h->distance(current) > 1)
                        // && count() > 30
                        ////			  && h->field<pin_type>() !=
                        ///_string_from_check_state[Qt::Checked]
                        // && v != currentWebView()
                        // && v != webView(0)
                        // ){
                        ////		    v_list[i]->page()->deleteLater();
                        ////		    v_list[i]->page(nullptr);
                        ////
                        ///v_list[i]->page()->setUrl(QUrl(web::Browser::_defaulthome));
                        ////			auto h = p->host();
                        // closeTab(webViewIndex(v));
                        ////			_browser->bind(RecordIndex::instance(record_index->current_model(),
                        ///h, sibling(v)->page()->host()), false);// horrible
                        ////			v = nullptr;
                        // }
                        // }
                        // }
                        // }
                        //
                    }
                }
            }
        }
    }
    assert(current);
    if (browsers.size() > bar) {
        // std::vector<boost::intrusive_ptr<TreeItem> >	list;
        std::vector<web::Browser*> list_v;
        for (auto v : browsers) { // for(std::vector<web::WebView *>::size_type i = 20; i <
            // static_cast<std::vector<web::WebView
            // *>::size_type>(count()); i ++){
            //							auto v = bro->tab_widget()->webView(i); // v_list[i];


            //
            bool found = false;
            // [&] {for(auto &j : list) if(j->id() == h->id()){found =
            // true;break;}return;} ();
            [&] {
                for (auto& j : list_v)
                    if (j == v) {
                        found = true;
                        break;
                    }
                return;
            }();
            if (!found && v != cur_bro) {
                // list.push_back(h);
                list_v.push_back(v);
            }
            // closeTab(webViewIndex(v));
        }
        // std::sort(list.begin(), list.end(), [&](boost::intrusive_ptr<TreeItem>
        // t0, boost::intrusive_ptr<TreeItem> t1){
        ////			auto _e = list.end();
        ////			auto _re = _e.base();
        ////			auto _se = _re->get();
        // return	// &t0 != _re ? &t1 != _re ?
        // t0	// && t0 != *list.end()
        // ? t1	// && t1 != *list.end()
        // ?    (current->distance(t0) != - 1 && current->distance(t0) <
        // current->distance(t1))
        // || (- 1 == current->distance(t1))
        // || (t0->field<rating_type>().toInt() >
        // t1->field<rating_type>().toInt())
        // : true
        // : false	//: true : false
        // ;
        // });

        qSort(list_v.begin(), list_v.end(), [&](web::Browser* b0, web::Browser* b1) -> int {
            // auto _e = list.end();
            // auto _re = _e.base();
            // auto _se = _re->get();
            auto v0 = b0->currentTab();
            auto v1 = b1->currentTab();
            return // &t0 != _re ? &t1 != _re ?
                v0 // && t0 != *list.end()
                ?
                v1 // && t1 != *list.end()
                    ?
                (current->distance(v0->page()->host()) != -1 && current->distance(v0->page()->host()) < current->distance(v1->page()->host())) //
                        || (-1 == current->distance(v1->page()->host()))                                                                       //
                        || (v0->page()->host()->field<rating_key>() > v1->page()->host()->field<rating_key>()) :
                true :
                false // : true : false
                ;
        });
        // while(list.size() > 30) list.pop_back();
        while (list_v.size() > bar) {
            web::Browser* bro = list_v.back();
            if (bro) {
                rs_t* rs = bro->record_screen();
                if (rs) {
                    auto v_tab = gl_paras->vtab_record();
                    auto index = v_tab->indexOf(rs);
                    if (index != -1) { //&& rs != cur_rs
                        emit v_tab->tabBar()->tabCloseRequested(index);
                        list_v.pop_back();
                    }
                }
            }
        }

        //						std::set<web::Browser*> list_v_set;

        //						for (auto i : list_v) list_v_set.insert(i);
        //						for (auto v : list_v_set) { // for(std::vector<web::WebView *>::size_type i = 20; i < static_cast<std::vector<web::WebView*>::size_type>(count()); i ++){
        //							auto p = v->page();
        //							if (p) {
        //								auto h = p->host();
        //								if (h && detail::to_qstring(h->field<pin_key>()) != char_from_check_state[Qt::Checked] && v != currentWebView() && v != webView(0)) {

        //									if (list_v_set.find(v) == list_v_set.end()) closeTab(webViewIndex(v));
        //								}
        //							}
        //						}
        result = list_v.size();
    }
    return result;
}

web::AutoSaver* wn_t::autosaver() const { return _autosaver; }
#ifdef USE_STRINGLIST_MODEL

QStringListModel* wn_t::stringlistmodel() const
{
    return _stringlistmodel;
}

void wn_t::on_topic_changed(const QString& original_topic_, const QString& topic_new, bool append_mode)
{
    (void)append_mode;
    auto topic_old_list = _stringlistmodel->stringList();
    QStringList new_recent_list;

    for (int i = 0; i < topic_old_list.count(); ++i) {
        QString topic_old = topic_old_list.at(i);
        if (original_topic_ == topic_old)
            new_recent_list << topic_new;
        else
            new_recent_list << topic_old;
    }
    _stringlistmodel->setStringList(new_recent_list);
}

#endif // USE_STRINGLIST_MODEL
