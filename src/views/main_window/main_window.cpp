
#include <string>
#include <wobjectimpl.h>
#include <QString>
#include <QDir>
#include <QString>
#include <QDesktopWidget>
#include <QStackedWidget>

#include "main.h"
#include "models/app_config/app_config.h"
#include "views/browser/entrance.h"
#include "main_window.h"
#include "views/print_preview/print_preview.h"
#include "views/app_config/app_config_dialog.h"
#include "libraries/trash_monitoring.h"
#include "views/tree/tree_screen.h"
#include "views/record/meta_editor.h"
#include "views/record_table/record_screen.h"
#include "models/tree/tree_item.h"
#include "views/find_in_base_screen/find_screen.h"
#include "models/record_table/linker.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "libraries/global_parameters.h"
#include "views/console_emulator/execute_command.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"

#include "libraries/wyedit/editor_text_area.h"
#include "views/browser/browser.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "views/browser/downloadmanager.h"
#include "views/browser/history.h"
#include "views/browser/bookmarks.h"
#include "views/tree/tree_view.h"



extern AppConfig	appconfig;
extern TrashMonitoring	trashmonitoring;
extern GlobalParameters globalparameters;
extern WalkHistory	walkhistory;
const char		*meta_editor_singleton_name = "editor_screen";
const char		*record_screen_multi_instance_name = "record_screen";
const char		*tree_screen_singleton_name = "tree_screen";
const char		*find_screen_singleton_name = "find_screen";
const char		*download_manager_singleton_name = "download_manager";
const char		*windowswitcher_singleton_name = "window_switcher";
const char		*entrance_singleton_name = "entrance";
const char		*record_controller_multi_instance_name = "record_controller";		// std::string(std::string(table_screen_singleton_name) + std::string("_controller")).c_str();
const char		*record_view_multi_instance_name = "record_view";
extern const char	*tree_screen_viewer_name;
extern const char	*action_find_in_base;
W_OBJECT_IMPL(wn_t)
wn_t::wn_t(GlobalParameters     &_globalparameters
	  , AppConfig           &_appconfig
	  , DataBaseConfig      &_databaseconfig
	  , browser::Profile    *_profile
	  , QString style_source)
    : QMainWindow()
      , _style(style_source)
      , _globalparameters(_globalparameters)
      , _appconfig(_appconfig)
      , _databaseconfig(_databaseconfig)
      , _v_right_splitter([&]() -> QSplitter * {auto _v_r_s = new QSplitter(Qt::Vertical);_v_r_s->setSizes(_appconfig.v_right_splitter_sizelist());_globalparameters.v_right_splitter(_v_r_s);return _v_r_s;} ())
      , _v_find_splitter([&]() -> QSplitter * {auto _v_f_s = new QSplitter(Qt::Vertical);_v_f_s->setSizes(_appconfig.findsplitter_sizelist());_globalparameters.find_splitter(_v_f_s);return _v_f_s;} ())
      , _vtab_record([&](QString style_source_){auto vr = new HidableTabWidget(style_source_, this);_globalparameters.vtab_record(vr);return vr;} (_style))
      , _vtab_tree([&](QString style_source_){auto vt = new HidableTabWidget(style_source_, this);_globalparameters.vtab_tree(vt);return vt;} (_style))
      , _h_record_splitter([&]() -> QSplitter * {auto _h_r_s = new QSplitter(Qt::Horizontal);_h_r_s->setSizes(_appconfig.h_record_splitter_sizelist());_globalparameters.h_record_splitter(_h_r_s);return _h_r_s;} ())
      , _h_tree_splitter([&]() -> QSplitter * {
	      if(_globalparameters.target_os() == "android")setWindowState(Qt::WindowMaximized);																					// Для Андроида окно просто разворачивается на весь экран
	      else{
		  QRect rect = _appconfig.mainwin_geometry();
		  resize(rect.size());
		  move(rect.topLeft());
	      }
	      auto _h_l_s = new QSplitter(Qt::Horizontal);_h_l_s->setSizes(_appconfig.h_tree_splitter_sizelist());
	      _globalparameters.h_tree_splitter(_h_l_s);

	      return _h_l_s;
	  } ())								// Qt::Vertical
// , _h_splitter(new QSplitter(Qt::Horizontal))
      , _filemenu(new QMenu(tr("&File"), this))
      , _editmenu(new QMenu(tr("&Edit"), this))
      , _viewmenu(new QMenu(tr("&View"), this))
      , _histrymenu(new browser::HistoryMenu(this))
      , _bookmarkmenu(new browser::BookmarksMenu(this))
      , _windowmenu(new QMenu(tr("&Window"), this))
      , _toolsmenu(new QMenu(tr("&Tools"), this))
      , _helpmenu(new QMenu(tr("&Help"), this))
      , _tree_screen(new ts_t(tree_screen_singleton_name, _appconfig, _filemenu, _toolsmenu, this))		// _vtabwidget
      , _find_screen(new FindScreen(find_screen_singleton_name, _tree_screen, this))
      , _editor_screen(new MetaEditor(meta_editor_singleton_name, _find_screen))		// _find_screen -> for find_text
      , _entrance(new browser::Entrance(entrance_singleton_name, _tree_screen, _find_screen, _editor_screen, this, _appconfig, _globalparameters.style_source(), _profile, Qt::Widget))							// Qt::MaximizeUsingFullscreenGeometryHint
      , _download(new browser::DownloadManager(download_manager_singleton_name, _vtab_tree))
      , _statusbar(new QStatusBar(this))
      , _switcher(new WindowSwitcher(windowswitcher_singleton_name, _editor_screen, this))
      , _enable_real_close(false){
	// _page_screen->setVisible(false);
	// _page_screen->hide();

    _globalparameters.mainwindow(this);



	// int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 100
	// int vtab_g_width = _vtabwidget->geometry().width();         // 100
	// int this_width = geometry().width();                        // 640
	// int download_width = _download->geometry().width();         // 1089
	// int tree_screen_width = _tree_screen->geometry().width();   // 100
	// int find_splitter_width = _v_find_splitter->geometry().width(); // 640
	// int left_splitter_width = _h_left_splitter->geometry().width(); // 640

	// if(_vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()) {
	// _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100, _vtabwidget->geometry().height());
	// }


    _v_right_splitter->setHandleWidth(0);
    _v_find_splitter->setHandleWidth(0);
    _h_tree_splitter->setHandleWidth(0);
// int wl = _h_left_splitter->width(); // 640
// int wr = _h_right_splitter->width();// 640   // default handleWidth() == 6
    _h_record_splitter->setHandleWidth(0);
// _h_splitter->setHandleWidth(0);

    _vtab_record->tabBar()->hide();
//    _globalparameters.vtab_record(_vtab_record);// in initialize list
//    _globalparameters.vtab_tree(_vtab_tree);	// in initialize list

    extern QObject *mainwindow;
    mainwindow = this;
    setObjectName("mainwindow");

    installEventFilter(this);

    init_file_menu();

    append_quit_menu();

    init_tools_menu();
	// initHelpMenu();
    QMainWindow::menuBar()->hide();

    setup_ui();
    setup_signals();
    assembly();

	// initFileMenu();
	// initToolsMenu();
	// initHelpMenu();

    setup_icon_actions();
    create_tray_icon();
    set_icon();
    if(QSystemTrayIcon::isSystemTrayAvailable())_tray_icon->show();
	//// Инициализируется объект слежения за корзиной
	// trashmonitoring.init(_appconfig.get_trashdir());
	// trashmonitoring.update();

	// Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
    _enable_real_close = false;

	// Инициализация генератора случайных чисел
    init_random();
}

void wn_t:: append_quit_menu(){
    QAction *quit = new QAction(tr("&Quit"), this);

    quit->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(quit, &QAction::triggered, this, &wn_t::application_exit);
    _filemenu->addAction(quit);
}

wn_t::~wn_t(){
    save_all_state();

    _entrance->deleteLater();_entrance = nullptr;
	// delete
    _switcher->deleteLater();
	// delete
    _statusbar->deleteLater();
	// delete
    _editor_screen->deleteLater();
	// delete
    _find_screen->deleteLater();
	// delete  _download;
	// if(_page_screen)delete  _page_screen;
	// delete  _table_screen;
    if(_tree_screen){
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

void wn_t:: setup_ui(void){
	// При создании объектов не указывается parent, так как он буден задан в момент вставки в layout в методе assembly()

	// _tree_screen = new TreeScreen(_appconfig, this);
	// _tree_screen->setObjectName(tree_screen_singleton_name);  // "treeScreen"
    _globalparameters.tree_screen(_tree_screen);
	// _treetable_hidden = treeScreen->isHidden();
	// connect(treeScreen, &TreeScreen::hide, this, [this]() {_treetable_hidden = true;});
	// connect(treeScreen, &TreeScreen::show, this, [this]() {_treetable_hidden = false;});

	// _table_screen = new TableScreen(this);
	// _table_screen->setObjectName(table_screen_singleton_name); // "recordTableScreen"


	// _globalparameters.push_record_screen(_table_screen);


	// _recordtable_hidden = recordTableScreen->isHidden();
	// connect(recordTableScreen, &RecordTableScreen::hide, this, [this]() {_recordtable_hidden = true;});
	// connect(recordTableScreen, &RecordTableScreen::show, this, [this]() {_recordtable_hidden = false;});

	// _page_screen = new TableScreen(this);
	// _page_screen->setObjectName("page_screen");
	// _globalparameters.page_screen(_page_screen);

	// _download = new browser::DownloadManager(this);
	// _download->setObjectName(download_manager_singleton_name);
    _globalparameters.download_manager(_download);


	// _editor_screen = new MetaEditor();
	// _editor_screen->setObjectName(meta_editor_singleton_name);  // "editorScreen"
    _globalparameters.meta_editor(_editor_screen);


	// _find_screen = new FindScreen(this);
	// _find_screen->setObjectName(find_screen_singleton_name);  // "findScreenDisp"
    _globalparameters.find_screen(_find_screen);
	// findScreenDisp->hide();
    if(! _appconfig.editor_show())_editor_screen->hide();
	// _statusbar = new QStatusBar(this);
    _statusbar->setObjectName("status_bar");		// "statusBar"
    setStatusBar(_statusbar);
    _globalparameters.status_bar(_statusbar);

	// Вспомогательный объект переключения окон, используется в мобильном интерфейсе
	// _switcher = new WindowSwitcher(this);
	// _switcher->setObjectName(windowswitcher_singleton_name); // "windowSwitcher"
    _globalparameters.window_switcher(_switcher);
	// windowSwitcher->findInBaseClick();

	// if(_table_screen) {
	// _entrance = new browser::Entrance(
	// _record_ontroller
	// = _table_screen->getRecordTableController()
	// , _globalparameters.style_source()
	// , this
	// , Qt::Widget  // Qt::MaximizeUsingFullscreenGeometryHint
	// );
	////    browsermanager->adjustSize();
    _entrance->set_scrollbars(true);
	// _entrance->setObjectName(entrance_singleton_name);
    _globalparameters.entrance(_entrance);
	// }

	// todo: Для проверки, почему то в этом месте поиск объекта по имени не работает, разобраться.
	// MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name);
}

void wn_t:: setup_signals(void){
    connect(_editor_screen, &MetaEditor::send_expand_edit_area, this, &wn_t::on_expand_edit_area);

	// Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
    connect(sapp_t::instance(), &QApplication::commitDataRequest, this, &wn_t::commit_data);

    connect(sapp_t::instance(), &QApplication::focusChanged, this, &wn_t::on_focus_changed);

	// Связывание сигналов кнопки поиска по базе с действием по открытию виджета поиска по базе
    connect(_tree_screen->_actionlist[action_find_in_base], &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);
	// connect(_entrance->getactionFreeze(), &QAction::triggered, globalparameters.getWindowSwitcher(), &WindowSwitcher::findInBaseClick);
	// connect(_table_screen->_find_in_base, &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

	// if(_page_screen)connect(_page_screen->actionFindInBase, &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

    connect(_editor_screen, &MetaEditor::wyedit_find_in_base_clicked, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);

	// auto hide_others = [this](int index) {
	// if(-1 != index) {
	// auto count = _vtabwidget->count();

	// for(int i = 0; i < count; i++) {
	// if(i != index)_vtabwidget->widget(i)->hide();
	// }

	// _vtabwidget->widget(index)->show();
	// }
	// };


    connect(_vtab_record, &HidableTabWidget::currentChanged, this, [&](int index){
	    if(- 1 != index){
		auto record_widget = _vtab_record->widget(index);

		// for(int i = 0; i < _vtab_record->count(); i++) {
		// if(i != real_index) {
		// auto current_widget = _vtab_record->widget(i);
		// if(current_widget) {
		// if(current_widget->objectName() == record_screen_multi_instance_name) {
		// auto record_screen = static_cast<RecordScreen *>(current_widget);
		// record_screen->browser()->lower();
		// }

		// current_widget->hide();
		// }
		// }
		// }
		rs_t *record_screen = nullptr;
		if(record_widget->objectName() == record_screen_multi_instance_name)record_screen = dynamic_cast<rs_t *>(record_widget);
		if(record_screen){
// _vtab_record->setCurrentWidget(record_screen);


// record_screen->restore_menubar();


		    auto current_brower = record_screen->browser();
		    if(current_brower && ! current_brower->is_under_construction()){
			current_brower->raise();
			current_brower->activateWindow();
		    }
		    for(int i = 0; i < _vtab_tree->count(); i ++){
			auto tree_view_curr = _vtab_tree->widget(i);
			if(tree_view_curr->objectName() == tree_screen_viewer_name){
			    auto tree_screen_viewer = dynamic_cast<tsv_t *>(tree_view_curr);
			    if(tree_screen_viewer){
				if(tree_screen_viewer->record_screen() == record_screen){
				    if(_vtab_tree->currentIndex() != i){
					if(! tree_screen_viewer->tree_screen()){
					    tree_screen_viewer->tree_screen(_tree_screen);
					}
					_vtab_tree->setCurrentIndex(i);
				    }
				    break;
				}
			    }
			}
		    }
		}
		// else{
		// _tree_screen->restore_menubar();
		// }
	    }
	});

    auto tree_viewer_integrity = [&, this](wn_t *_this, HidableTabWidget *_vtab_tree, int index) -> void {
	    auto tree_view_curr = _vtab_tree->widget(index);
	    if(tree_view_curr->objectName() == tree_screen_viewer_name){																	// if(real_index < _vtab_record->count()) {
		// for(int i = 0; i < _vtab_record->count(); i++) {
		// if(i != real_index) {
		// auto current_widget = _vtab_record->widget(i);
		// if(current_widget) {
		// if(current_widget->objectName() == record_screen_multi_instance_name) {
		// auto record_screen = static_cast<RecordScreen *>(current_widget);
		// record_screen->browser()->lower();
		// }

		// current_widget->hide();
		// }
		// }
		// }

		auto current_tree_screen_viewer = dynamic_cast<tsv_t *>(tree_view_curr);																			// auto current_widget = _vtab_record->widget(real_index);
		if(current_tree_screen_viewer){
		    QWidget *tree_screen = current_tree_screen_viewer->tree_screen();
		    if(! tree_screen){
			current_tree_screen_viewer->tree_screen(_this->_tree_screen);
		    }
		    rs_t *record_screen = current_tree_screen_viewer->record_screen();
		    if(record_screen){
			if(_this->_vtab_record->currentWidget() != record_screen)_this->_vtab_record->setCurrentWidget(record_screen);
// record_screen->restore_menubar();



			auto current_brower = record_screen->browser();
			if(current_brower && ! current_brower->is_under_construction()){
			    current_brower->raise();
			    current_brower->activateWindow();
			    auto browser_tab = current_brower->tabmanager();
			    browser_tab->setCurrentIndex(browser_tab->currentIndex());
			    auto v = browser_tab->currentWebView();
			    if(v){
				v->setFocus();
				v->activateWindow();
				v->raise();
				v->adjustSize();// v->repaint();
			    }
// v->layout()->update();  // activate();
			}
		    }
			// else{
			// _tree_screen->restore_menubar();
			// }
		}
	    }
	};

    auto empty_recovery = [&, this](wn_t *_this, HidableTabWidget *_vtab_tree, int &index) -> void {
	    (void)_this;
	    int tree_viewer_count = 0;
	    for(int i = 0; i < _vtab_tree->count(); i ++){
		if(_vtab_tree->widget(i)->objectName() == tree_screen_viewer_name)tree_viewer_count ++;
	    }
// auto count = _vtab_tree->count();
	    if(0 == tree_viewer_count){														// if(1 >= count) {    // self and download
// _this->_tree_screen->restore_menubar();
		index = _vtab_tree->insertTab(0, static_cast<QWidget *>(new tsv_t(_this->_tree_screen, nullptr)), QIcon(":/resource/pic/three_leaves_clover.svg"), "Browser");
		_vtab_tree->setCurrentIndex(index);
	    }
	};

	// hide_others(_vtabwidget->currentIndex());
    connect(_vtab_tree, &HidableTabWidget::currentChanged, this, [&, this](int index){
	    if(- 1 != index){
		empty_recovery(this, _vtab_tree, index);

		tree_viewer_integrity(this, _vtab_tree, index);

//// deprecated: ignoring Tree Search Area
// if(_vtab_tree->widget(index)->objectName() == tree_screen_singleton_name) {
// _appconfig.find_screen_tree_search_area(0);
// } else if(_vtab_tree->widget(index)->objectName() == record_screen_multi_instance_name) {
// _appconfig.find_screen_tree_search_area(1);
// }
	    }
	});

    connect(_vtab_tree, &HidableTabWidget::tabBarClicked, this, [&, this](int index){
// if(1 <= index) {
	    empty_recovery(this, _vtab_tree, index);
	    tree_viewer_integrity(this, _vtab_tree, index);
// }
	});

    connect(_h_tree_splitter, &QSplitter::splitterMoved, [&](int pos, int index){
	    (void)index;
	    (void)pos;
//	    if(0 != pos){
	    auto sizes = _h_tree_splitter->sizes();
	    _appconfig.h_tree_splitter_sizelist(sizes);
//	    }
	});


    connect(_h_record_splitter, &QSplitter::splitterMoved, [&](int pos, int index){
	    (void)index;
	    (void)pos;
//	    if(0 != pos){
	    auto sizes = _h_record_splitter->sizes();
	    _appconfig.h_record_splitter_sizelist(sizes);
//	    }
	});





//// deprecated: ignoring Tree Search Area
// connect(_vtab_tree, &QTabWidget::currentChanged,  &_appconfig
// , [this] (int index) {
// if(-1 != index) {
// if(_vtab_tree->widget(index)->objectName() == tree_screen_singleton_name) {
// _appconfig.find_screen_tree_search_area(0);
// } else if(_vtab_tree->widget(index)->objectName() == record_screen_multi_instance_name) {
// _appconfig.find_screen_tree_search_area(1);
// }
// }
// }   // &AppConfig::setFindScreenTreeSearchArea
// );      // , findScreenDisp, &FindScreen::changedTreeSearchArea
    _vtab_tree->tabBar()->setUsesScrollButtons(true);
}

std::vector<tsv_t *> wn_t:: tree_viewers() const {
    std::vector<tsv_t *>	tsvs;
    int				tree_viewer_count = 0;
    for(int i = 0; i < _vtab_tree->count(); i ++){
	if(_vtab_tree->widget(i)->objectName() == tree_screen_viewer_name){
	    tsvs.push_back(dynamic_cast<tsv_t *>(_vtab_tree->widget(i)));
	    tree_viewer_count ++;
	}
    }
    return tsvs;
}

void wn_t:: assembly(void){
	// v_right_splitter = new QSplitter(Qt::Vertical);
    _v_right_splitter->addWidget(_entrance);
    _v_right_splitter->addWidget(_editor_screen);			// Text entries // Текст записи
    _v_right_splitter->setCollapsible(0,  true);			// if true, make editor can overload it    // The list of final entries can not link up    // Список конечных записей не может смыкаться
    _v_right_splitter->setCollapsible(1,  false);				// The contents of the recording can not link up    // Содержимое записи не может смыкаться
    _v_right_splitter->setObjectName("v_right_splitter");

	// find_splitter = new QSplitter(Qt::Vertical);
    _v_find_splitter->addWidget(_v_right_splitter);			// findSplitter->addWidget(hSplitter);
    _v_find_splitter->addWidget(_find_screen);
    _v_find_splitter->setCollapsible(0,  false);			// Верхняя часть не должна смыкаться
    _v_find_splitter->setCollapsible(1,  false);			// Часть для поиска не должна смыкаться
    _v_find_splitter->setObjectName("find_splitter");

	// _qtabwidget = new QTabWidget(this);

    _vtab_tree->setTabPosition(QTabWidget::West);			// sometime make "QModelIndex TreeModel::parent(const QModelIndex &index) const" failed.





	// _vtabwidget->addTab(_tree_screen, QIcon(":/resource/pic/leaves.svg"), "Tree");




	// _vtabwidget->addTab(_table_screen, QIcon(":/resource/pic/clover.svg"), "Candidate");

	// if(_page_screen)_vtabwidget->addTab(_page_screen, QIcon(":/resource/pic/three_leaves_clover.svg"), "Page");

    auto index = _vtab_tree->addTab(static_cast<QWidget *>(new tsv_t(_tree_screen, nullptr)), QIcon(":/resource/pic/three_leaves_clover.svg"), "Browser");
    _vtab_tree->addTab(static_cast<QWidget *>(_download), QIcon(":/resource/pic/apple.svg"), "Download");				// QIcon(":/resource/pic/holly.svg")

    _vtab_tree->setCurrentIndex(index);


    _appconfig.find_screen_tree_search_area(0);			// force to root_item of global tree




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

	// if(_vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()) {
	// _vtabwidget->resize(_vtabwidget->geometry().width() * 15 / 100, _vtabwidget->geometry().height());
	// }

    _h_record_splitter->setObjectName("h_right_splitter");
    _h_record_splitter->addWidget(_vtab_record);
    _h_record_splitter->addWidget(_v_find_splitter);

// {
// auto sizes = _h_right_splitter->sizes();
// if(_globalparameters.entrance()->browsers().size() == 0) { // h_right_splitter->widget(0)->width()
// sizes[0] = 0;
// sizes[1] = sizes[0] + sizes[1];
// _h_right_splitter->setSizes(sizes);

// }
// emit _h_right_splitter->splitterMoved(sizes[0], 1);
// }

    _globalparameters.h_record_splitter(_h_record_splitter);

    _h_tree_splitter->addWidget(_vtab_tree);
    _h_tree_splitter->addWidget(_h_record_splitter);

	// v_left_splitter->addWidget(treeScreen);
	// v_left_splitter->addWidget(recordTableScreen);
    _h_tree_splitter->setCollapsible(0,  true);
    _h_tree_splitter->setCollapsible(1,  false);
	// v_left_splitter->setCollapsible(1, false);
    _h_tree_splitter->setObjectName("h_left_splitter");
//    _vtab_tree->setMinimumWidth(_vtab_tree->tabBar()->geometry().width());
//    _vtab_tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _globalparameters.find_splitter(_v_find_splitter);
    _globalparameters.h_tree_splitter(_h_tree_splitter);
    _globalparameters.v_right_splitter(_v_right_splitter);


// _h_splitter->addWidget(_tree_screen);


	// hSplitter = new QSplitter(Qt::Horizontal);
// _h_splitter->addWidget(_h_left_splitter);
	// hSplitter->addWidget(treeScreen);             // Tree branches    // Дерево веток
	// hSplitter->addWidget(recordTableScreen);      // The list of final entries    // Список конечных записей



	// _h_splitter->addWidget(_find_splitter);             //hSplitter->addWidget(vSplitter);




// _h_splitter->setCollapsible(0, false);            // Дерево веток не может смыкаться
// _h_splitter->setCollapsible(1, false);            // Столбец со списком и содержимым записи не может смыкаться
// _h_splitter->setObjectName("hsplitter");

	// connect(find_splitter, &QSplitter::splitterMoved, browser_entrance, &browser::Entrance::on_splitter_moved);
	// connect(find_splitter, &QSplitter::splitterMoved, recordTableScreen, &RecordTableScreen::on_splitter_moved);

	// findSplitter=new QSplitter(Qt::Vertical);
	// findSplitter->addWidget(hSplitter);
	// findSplitter->addWidget(findScreenDisp);
	// findSplitter->setCollapsible(0,false);        // Верхняя часть не должна смыкаться
	// findSplitter->setCollapsible(1,false);        // Часть для поиска не должна смыкаться
	// findSplitter->setObjectName("find_splitter");

    setCentralWidget(_h_tree_splitter);			// setCentralWidget(_h_splitter);                    //setCentralWidget(findSplitter);



    auto	sizes = _h_tree_splitter->sizes();
    auto	bar_width = _vtab_tree->tabBar()->geometry().width();
// auto ww = _h_left_splitter->widget(0)->width(); // 100 != 0 when sizes[0] == 0
    if(bar_width >= sizes[0]){			// _h_left_splitter->widget(0)->width()
	auto vtab_tree_min_width = _vtab_tree->minimumSizeHint().width();		// _tree_screen->minimumSizeHint().width();                 // globalparameters.entrance()->activated_browser()->record_screen()->minimumSizeHint().width();           // 6xx   // h_right_splitter->widget(0)->width();    // 0    // sizeHint().width();    // 23
	// auto h = h_right_splitter->handle(1);
	// h->move(lr + shw, h->rect().top());

	auto	size_memory = appconfig.h_tree_splitter_sizelist();
	auto	sum = size_memory[0] + size_memory[1];
	sizes[0] = size_memory[0] > vtab_tree_min_width ? size_memory[0] < sum ? size_memory[0] : sum * 15 / 100 : vtab_tree_min_width;
	sizes[1] = sum - sizes[0] > 0 ? sum - sizes[0] : sum * 85 / 100;				// sizes[1] > size_memory[1] ? size_memory[1] : sizes[1];
//	_vtab_tree->setMaximumWidth(maximumWidth());
//	_vtab_tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);// h_left_splitter->moveSplitter(sizes[0], 1);   // protected member
	_h_tree_splitter->setSizes(sizes);	// emit _h_tree_splitter->splitterMoved(sizes[0], 1);

// auto s_0 = _vtab_tree->minimumSizeHint();   // (146, 146)
// auto s_1 = _vtab_record->minimumSizeHint(); // (25, 146)
// auto s_2 = _entrance->minimumSizeHint();
// auto s_3 = _h_right_splitter->minimumSizeHint();    // (241,146)
// auto s_4 = _h_right_splitter->maximumWidth();    // (241,146)
// auto sizes_check = _h_left_splitter->sizes();
	// h_right_splitter->resize(h_right_splitter->sizeHint().width(), h_right_splitter->height());
    }
}

void wn_t:: save_all_state(void){
	// Сохранение данных в поле редактирования
    save_text_area();

	// Сохраняются данные сеанса работы
    save_geometry();
    save_tree_position();
	// save_recordtable_position();
    save_editor_cursor_position();
    save_editor_scrollbar_position();

	// Синхронизируется с диском конфиг программы
    appconfig.sync();
}

HidableTabWidget *wn_t:: vtab_record(){return _vtab_record;}

HidableTabWidget *wn_t:: vtab_tree(){return _vtab_tree;}

QMenu *wn_t:: file_menu(){return _filemenu;}

QMenu *wn_t:: edit_menu(){return _editmenu;}

QMenu *wn_t:: view_menu(){return _viewmenu;}

browser::HistoryMenu *wn_t:: histry_menu(){return _histrymenu;}

browser::BookmarksMenu *wn_t:: bookmark_menu(){return _bookmarkmenu;}

QMenu *wn_t:: window_menu(){return _windowmenu;}

QMenu *wn_t:: tools_menu(){return _toolsmenu;}

QMenu *wn_t:: help_menu(){return _helpmenu;}

QSplitter *wn_t:: find_splitter(){return _v_find_splitter;}

QSplitter *wn_t:: h_record_splitter(){return _h_record_splitter;}

QSplitter *wn_t:: h_tree_splitter(){return _h_tree_splitter;}


// Слот, срабатывающий когда происходит выход из оконной системы
void wn_t:: commit_data(QSessionManager &manager){
    Q_UNUSED(manager);
    qDebug() << "Session manager send commit data signal.";

    application_fast_exit();
}

// Восстанавливается геометрия окна и позиции основных разделителей
void wn_t:: restore_geometry(void){
    if(globalparameters.target_os() == "android")setWindowState(Qt::WindowMaximized);				// Для Андроида окно просто разворачивается на весь экран
    else{
	QRect rect = appconfig.mainwin_geometry();
	resize(rect.size());
	move(rect.topLeft());
    }
	// move(rect.topLeft());
	// resize(rect.size());


    _v_right_splitter->setSizes(appconfig.v_right_splitter_sizelist());
// auto size_v_right_splitter = _v_right_splitter->size();         // (792, 614)
// auto sizelist_v_right_splitter = _v_right_splitter->sizes();    // (614, 0)

    _v_find_splitter->setSizes(appconfig.findsplitter_sizelist());
// auto size_v_find_splitter = _v_find_splitter->size();           // (792, 646)
// auto sizelist_v_find_splitter = _v_find_splitter->sizes();      // (614, 32)

	// _h_splitter->setSizes(appconfig.hspl_sizelist());
    _h_record_splitter->setSizes(appconfig.h_record_splitter_sizelist());
// auto size_h_right_splitter = _h_right_splitter->size();         // (851, 646)
// auto sizelist_h_right_splitter = _h_right_splitter->sizes();    // (0, 851)

    auto sizelist_h_left_splitter = appconfig.h_tree_splitter_sizelist();	// (146, 494)?
    _h_tree_splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);		// ShrinkFlag  // Minimum
    _h_tree_splitter->setSizes(sizelist_h_left_splitter);
// auto size_h_left_splitter = _h_left_splitter->size();           // (1366, 646)
    sizelist_h_left_splitter = _h_tree_splitter->sizes();		// (312, 1054)

	// int vtab_base = _vtabwidget->baseSize().width();            // 0
	// int vtab_frame = _vtabwidget->frameSize().width();          // 1118
	// int vtab_fg_width = _vtabwidget->frameGeometry().width();   // 1118






// auto vtab_size_hint = _vtab_tree->sizeHint();                   // (352, 220)
// int vtab_tree_max_width = _vtab_tree->geometry().width();       // 312  // 1118




	// int this_width = geometry().width();                        // 1366
	// int download_width = _download->geometry().width();         // 1089
	// int tree_screen_width = _tree_screen->geometry().width();   // 236
	// int find_splitter_width = _v_find_splitter->geometry().width(); // 0
	// int left_splitter_size_width = _h_left_splitter->size().width();            // 1124
	// int left_splitter_geometry_width = _h_left_splitter->geometry().width();    // 1124
	// int right_splitter_width = _h_right_splitter->geometry().width();           // 0

	// if(0 == find_splitter_width // _vtabwidget->frameGeometry().width() == _h_left_splitter->geometry().width()
	// ) {
	// _download->resize(vtab_g_width * 15 / 100, _vtabwidget->geometry().height());
	// _vtabwidget->resize(vtab_g_width * 15 / 100, _vtabwidget->geometry().height());
	// _entrance->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
	// _editor_screen->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
	// _find_screen->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());
	// _v_find_splitter->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());   //
	// _h_right_splitter->resize(vtab_g_width * 85 / 100, _v_find_splitter->geometry().height());   //

	// }

	// download_width = _download->geometry().width();         // 1089
	// vtab_g_width = _vtabwidget->geometry().width();                 // 167
	// find_splitter_width = _v_find_splitter->geometry().width();     // 950
	// right_splitter_width = _h_right_splitter->geometry().width();   // 950
	// _h_right_splitter->move(_h_right_splitter->x() - vtab_g_width * 85 / 100, _h_right_splitter->y());


	// _h_right_splitter->repaint();
	// _h_left_splitter->repaint();

    auto sizes = _h_tree_splitter->sizes();
// QList<int> new_sizes;
    auto	sum = sizes[0] + sizes[1];
    auto	vtab_tree_min = _vtab_tree->minimumSizeHint();		// (146, 146) // _tree_screen->minimumSizeHint().width();
    if(sizes[0] == sum){
	sizes[0] = vtab_tree_min.width();sizes[1] = sum				// vtab_tree_max_width
	    - sizes[0];
    }
// for(auto sz : sizes) {
// if(sz == 0) {
// sz = vtab_g_width * 85 / 100;
// } else if(sz == vtab_g_width) {
// sz = vtab_g_width * 15 / 100;
// }

// new_sizes << sz;
// }

    _h_tree_splitter->setSizes(sizes);		// new_sizes

	// _v_right_splitter->setSizes(appconfig.vspl_sizelist());
	// _h_splitter->setSizes(appconfig.hspl_sizelist());
	// _h_left_splitter->setSizes(appconfig.v_leftsplitter_sizelist());
	// _v_find_splitter->setSizes(appconfig.findsplitter_sizelist());
	// this->repaint();
}

// Запоминается геометрия окна и позиции основных разделителей
void wn_t:: save_geometry(void){
    qDebug() << "Save window geometry and splitter sizes";

    QRect geom(pos(), size());

    appconfig.mainwin_geometry(geom.x(), geom.y()
			      ,	geom.width(), geom.height());

	// mytetraconfig.set_mainwingeometry(geometry().x(), geometry().y(),
	// geometry().width(), geometry().height());

    appconfig.v_right_splitter_sizelist(_v_right_splitter->sizes());
// appconfig.hspl_sizelist(_h_splitter->sizes());
    appconfig.h_record_splitter_sizelist(_h_record_splitter->sizes());
    appconfig.h_tree_splitter_sizelist(_h_tree_splitter->sizes());
	// Запоминается размер сплиттера только при видимом виджете поиска,
	// т.к. если виджета поиска невидно, будет запомнен нуливой размер
	// if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
	// данный метод вызывается из декструктора главного окна, и к этому моменту
	// виджет уже невиден
    if(appconfig.findscreen_show())appconfig.findsplitter_sizelist(_v_find_splitter->sizes());
}

void wn_t:: restore_tree_position(void){
	// Путь к последнему выбранному в дереве элементу
    auto	pair = appconfig.tree_position();
    QString	current_root_id = pair.first;
    QStringList current_item_absolute_path = pair.second;		// appconfig.get_tree_position();

    qDebug() << "MainWindow::restoreTreePosition() : " << current_item_absolute_path;

    set_tree_position(current_root_id, current_item_absolute_path);
}

// save
void wn_t:: save_tree_position(void){
    auto _current_source_model = [&](){
	    return _tree_screen->view()->source_model();
	};
	// if(!_tree_screen->sysynchronized())_tree_screen->synchronize();
    auto item = _tree_screen->view()->session_root_auto();		// item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == _tree_screen->session_root_id();});
	//// Получение QModelIndex выделенного в дереве элемента
	// const QModelIndex index = _tree_screen->tree_view()->current_index();
    auto current_item = _tree_screen->view()->current_item();
    if(current_item){
	appconfig.tree_position(
	    _current_source_model()->root_item()->id()					// _tree_screen->know_model_board()->root_item()->id()
			       , current_item->path_list()
	    );
    }else if(item){			// if(index.isValid()) {
	////    if(index.isValid()) {   // this line is to be remove
	//// Получаем указатель вида TreeItem
	// auto item = _current_source_model()->item(index);

	// Сохраняем путь к элементу item
	appconfig.tree_position(
	    _current_source_model()->root_item()->id()					// _tree_screen->know_model_board()->root_item()->id()
			       , item->path_list()
	    );
	// }
    }
}

// set
void wn_t:: set_tree_position(QString current_root_id, QStringList current_item_absolute_path){
	// _tree_screen->session_root_id(current_item_absolute_path.last());

    auto	source_model = [&](){return _tree_screen->view()->source_model();};
    auto	know_model_board = [&](){return _tree_screen->view()->know_model_board();};
    if(source_model()->root_item()->id() != current_root_id){
// boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, know_model_board()->item([&] (boost::intrusive_ptr<const TreeItem> it) {
// return it->id() == current_root_id;})); } catch(std::exception &e) {throw e; } return tree_index; } ();
	auto it = know_model_board()->item([&](boost::intrusive_ptr<const TreeItem> it){return it->id() == current_root_id;});
	_tree_screen->view()->intercept(it);	// TreeIndex::instance(know_model_board, it, it->parent())
    }
	// if(!_tree_screen->know_model_board()->item(current_item_absolute_path))   // on know_root semantic
	// return;

	// Получаем указатель на элемент вида TreeItem, используя путь
    auto it = know_model_board()->item(current_item_absolute_path);				// on know_root semantic
    if(! static_cast<QModelIndex>(source_model()->index(it)).isValid()){
// boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, it); } catch(std::exception &e) {throw e; } return tree_index; } ();
	_tree_screen->view()->intercept(it);	// TreeIndex::instance(know_model_board, it, it->parent())
    }
    auto tree_view = _tree_screen->view();
    if(it){
	if(it != know_model_board()->root_item()){
	    qDebug() << "Set tree position to " << it->field<name_type>() << " id " << it->field<id_type>();

		//// Из указателя на элемент TreeItem получаем QModelIndex
		// QModelIndex setto = source_model()->index(item);

		// Курсор устанавливается в нужную позицию

	    boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::instance([&] {return tree_view->source_model();}, it, it->parent());
// try {tree_index = new TreeIndex([&] {return tree_view->source_model(); }, it->parent(), it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
// return il == it->linker() && il->host() == it && it->parent() == il->host_parent();
// })); } catch(std::exception &e) {throw e; }

	    tree_view->select_as_current(tree_index);
	    tree_view->source_model()->session_id(tree_index);						// TreeIndex(source_model, it)
	}else{
	    boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::instance([&] {return tree_view->source_model();}, it);
// try {tree_index = new TreeIndex([&] {return tree_view->source_model(); }, it, 0); } catch(std::exception &e) {throw e; }

	    tree_view->select_as_current(tree_index);
	    tree_view->source_model()->session_id(tree_index);						// TreeIndex(source_model, it)
	}
    }
}

bool wn_t:: is_tree_position_crypt(){
    bool	result = false;
    auto	pair = appconfig.tree_position();
    QString	id = pair.first;
    QStringList path = pair.second;

    auto know_model_board = [&](){
	    return _tree_screen->view()->know_model_board();
	};
    if(_tree_screen->view()->source_model()->root_item()->id() != id){
// boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(know_model_board, know_model_board()->item([&] (boost::intrusive_ptr<const TreeItem> it) {
// return it->id() == id;})); } catch(std::exception &e) {throw e; } return tree_index; } ();
	auto it = know_model_board()->item([&](boost::intrusive_ptr<const TreeItem> it){return it->id() == id;});

	_tree_screen->view()->intercept(it);	// TreeIndex::instance(know_model_board, it, it->parent())
    }
	// if(_tree_screen->know_model_board()->is_item_valid(path) == false) return false;

	// Получаем указатель на элемент вида TreeItem, используя путь
    auto item = know_model_board()->item(path);
    if(item){
	if(item->field<crypt_type>() == "1")result = true;
	// else
	// return false;
    }
    return result;
}

// too many _record_screen objects, deprecated
// void MainWindow::restore_recordtable_position(void)
// {
// QString id = appconfig.get_recordtable_selected_record_id();

// if(id.length() > 0)
// select_id(id);
// }

// too many _record_screen objects, deprecated
// void MainWindow::save_recordtable_position(void)
// {
// QString id = _table_screen->first_selection_id();

// appconfig.set_recordtable_selected_record_id(id);
// }


// void MainWindow::select_id(QString id)
// {
// _table_screen->select_id(id);
// }


void wn_t:: save_editor_cursor_position(void){
    int n = _editor_screen->cursor_position();

    appconfig.editor_cursor_position(n);
}

void wn_t:: restore_editor_cursor_position(void){
    int n = appconfig.editor_cursor_position();

    _editor_screen->cursor_position(n);
}

void wn_t:: save_editor_scrollbar_position(void){
    int n = _editor_screen->scrollbar_position();

    appconfig.editor_scroll_bar_position(n);
}

void wn_t:: restore_editor_scrollbar_position(void){
    int n = appconfig.editor_scroll_bar_position();

    _editor_screen->scrollbar_position(n);
}

void wn_t:: restore_find_in_base_visible(void){
    bool n = appconfig.findscreen_show();

	// Определяется ссылка на виджет поиска
    FindScreen *find_screen = globalparameters.find_screen();			// find_object<FindScreen>(find_screen_singleton_name);
    if(n)find_screen->show();
    else find_screen->hide();
}

// Создание раздела меню File
void wn_t:: init_file_menu(void){
	// Создание меню
	// _filemenu = new QMenu(tr("&File"), this);
    _filemenu->clear();
    menuBar()->addMenu(_filemenu);

	//// Создание тулбара
	/// *
	// QToolBar *tb = new QToolBar(this);
	// tb->setWindowTitle(tr("File Actions"));
	// addToolBar(tb);
	// */

    QAction *a;

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

    a = new QAction(tr("&Print..."), this);
    a->setShortcut(QKeySequence::Print);
    connect(a, &QAction::triggered, this, &wn_t::file_print);
	// tb->addAction(a);
    _filemenu->addAction(a);

    a = new QAction(tr("Print Preview..."), this);
    connect(a, &QAction::triggered, this, &wn_t::file_print_preview);
    _filemenu->addAction(a);

    a = new QAction(tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, &QAction::triggered, this, &wn_t::file_print_pdf);
	// tb->addAction(a);
    _filemenu->addAction(a);

    _filemenu->addSeparator();

	// a = new QAction(tr("&Quit"), this);
	// a->setShortcut(Qt::CTRL + Qt::Key_Q);
	// connect(a, SIGNAL(triggered()), this, SLOT(applicationExit()));
	// _filemenu->addAction(a);
}

// Создание раздела меню Tools
void wn_t:: init_tools_menu(void){
    _toolsmenu->clear();
	// Создание меню
	// _toolsmenu = new QMenu(tr("&Tools"), this);
    menuBar()->addMenu(_toolsmenu);

    QAction *a;

    a = new QAction(tr("Find in ba&se"), this);
    connect(a, &QAction::triggered, this, &wn_t::tools_find);
    _toolsmenu->addAction(a);

    auto b = new QAction(tr("Editor"), this);
    connect(b, &QAction::triggered, this, &wn_t::editor_switch);
    _toolsmenu->addAction(b);

    _toolsmenu->addSeparator();
    if(appconfig.interface_mode() == "desktop"){
	a = new QAction(tr("Main &preferences"), this);
	connect(a, &QAction::triggered, this, &wn_t::tools_preferences);
	_toolsmenu->addAction(a);
    }else{
	// Создание подменю
	QMenu *menu = new QMenu(tr("Main &preferences"), this);
	init_preferences_menu(menu);
    }
}

// Заполнение подраздела меню Preferences
void wn_t:: init_preferences_menu(QMenu *menu){
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
void wn_t:: init_help_menu(void){
    _helpmenu->clear();
	// Создание меню
	// _helpmenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(_helpmenu);

    QAction *a;

    a = new QAction(tr("About MyTetra"), this);
    connect(a, &QAction::triggered, this, &wn_t::on_click_help_about_mytetra);
    _helpmenu->addAction(a);

    a = new QAction(tr("About Qt"), this);
    connect(a, &QAction::triggered, this, &wn_t::on_click_help_about_qt);
    _helpmenu->addAction(a);
}

// Новая коллекция
void wn_t:: file_new(void){}

// Открыть коллекцию
void wn_t:: file_open(void){}

// Сохранить текущую статью
bool wn_t:: file_save(void){return true;}

// Сохранить текущую статью как файл
bool wn_t:: file_save_as(void){return true;}

// Напечатать текущую статью
void wn_t:: file_print(void){
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));
    if(dlg->exec() == QDialog::Accepted)_editor_screen->textarea_document()->print(&printer);
    delete dlg;
#endif
}

// Предпросмотр печати текущей статьи
void wn_t:: file_print_preview(void){
    PrintPreview *preview = new PrintPreview(_editor_screen->textarea_document(), this);

    preview->setModal(true);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}

// Вывод текущей статьи в PDF файл
void wn_t:: file_print_pdf(void){
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF"
						   , QString(), "*.pdf");
    if(! fileName.isEmpty()){
	if(QFileInfo(fileName).suffix().isEmpty())fileName.append(".pdf");
	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fileName);
	_editor_screen->textarea_document()->print(&printer);
    }
#endif
}

// Слот - Нормальный выход из программы
void wn_t:: application_exit(void){
    save_all_state();
	// Если в конфиге настроено, что нужно синхронизироваться при выходе
	// И задана команда синхронизации
    if(appconfig.synchro_on_exit())
		if(appconfig.synchro_command().trimmed().length() > 0)synchronization();
	// Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}

// Быстрый выход из программы, без возможности синхронизации
void wn_t:: application_fast_exit(void){
    save_all_state();

	// Запуск выхода из программы
    _enable_real_close = true;
    emit close();
}

void wn_t:: tools_find(void){
	// Определяется ссылка на виджет поиска
    FindScreen *findScreenRel = globalparameters.find_screen();			// find_object<FindScreen>(find_screen_singleton_name);
    if(! (findScreenRel->isVisible()))findScreenRel->show();
    else findScreenRel->hide();
}

void wn_t:: editor_switch(void){
    MetaEditor *editorScreen = globalparameters.meta_editor();			// find_object<MetaEditor>(meta_editor_singleton_name);
    if(! (editorScreen->isVisible())){
	editorScreen->show();
	appconfig.editor_show(true);
    }else{
	editorScreen->hide();
	appconfig.editor_show(false);
    }
}

void wn_t:: tools_preferences(void){
	// Создается окно настроек, после выхода из этой функции окно удалится
    AppConfigDialog dialog(_entrance->activated_browser()->record_screen()->record_controller(), "");

    dialog.show();
}

// Слот, срабатывающий при нажатии в редакторе кнопки "распахнуть"
// Флаг показывает, что нужно сделать
// true - распахнуть область отводимую редактору
// false - сделать область, отводимую редактору, обычной
void wn_t:: on_expand_edit_area(bool flag){
	// static QSize entrance_size = globalparameters.entrance()->size();
	// static QSize tree_size = globalparameters.entrance()->size();
	// static QSize recordtable_size = globalparameters.entrance()->size();
	// static QSize vtab_size = globalparameters.entrance()->size();
	// static bool _treetable_hidden;     // = globalparameters.getTreeScreen()->isHidden();
	// static bool recordtable_hidden; // = globalparameters.getRecordTableScreen()->isHidden();
    if(flag){
	globalparameters.entrance()->hide();				// resize(QSize(0, 0)); //

	// if(!globalparameters.getTreeScreen()->isHidden()) {
	// _treetable_hidden = false;
	////            globalparameters.getTreeScreen()->resize(QSize(0, tree_size.height()));
	// globalparameters.getTreeScreen()->hide();    // resize(QSize(0, 0)); //
	// } else {
	// _treetable_hidden = true;
	// }

	// if(!globalparameters.getRecordTableScreen()->isHidden()) {
	// _recordtable_hidden = false;
	////            globalparameters.getRecordTableScreen()->resize(QSize(0, recordtable_size.height()));
	// globalparameters.getRecordTableScreen()->hide(); // resize(QSize(0, 0)); //
	// } else {
	// _recordtable_hidden = true;
	// }

	emit _vtab_tree->_hide_action->setChecked(true);
	// QTabWidget *tab = globalparameters.vtab();
	// tab->currentWidget()->hide();

	// auto * tab_pane = tab->findChild<QStackedWidget *>();
	// tab_pane->hide();

	// QRect rect = tab->geometry();
	// tab->setGeometry(rect.left(), rect.top(), 0, rect.height());

	// setGeometry(rect.top(), rect.left(), 0, rect.height());

	// resize(QSize(0, v_left_splitter->height())); // hide();
    }else{
	globalparameters.entrance()->show();				// resize(entrance_size); //

	// if(!_treetable_hidden) {
	// globalparameters.getTreeScreen()->show();    // resize(tree_size); //
	// }

	// if(!_recordtable_hidden) {
	// globalparameters.getRecordTableScreen()->show(); // resize(recordtable_size); //
	// }

	// emit _vtabwidget->hideAction.toggle();
	emit _vtab_tree->_hide_action->setChecked(false);
	// globalparameters.vtab()->resize(vtab_size); // show();
    }
}

void wn_t:: on_click_help_about_mytetra(void){
    QString version = QString::number(APPLICATION_RELEASE_VERSION) + "." + QString::number(APPLICATION_RELEASE_SUBVERSION) + "." + QString::number(APPLICATION_RELEASE_MICROVERSION);

    QString	infoProgramName;
    QString	infoVersion;
    QString	infoAuthor;
    QString	infoEmail;
    QString	infoLicense;
    QString	infoTargetOs;
    QString	infoProgramFile;
    QString	infoWorkDirectory;
    QString	infoDevicePixelRatio;
    QString	infoPhysicalDpi;
    QString	infoPhysicalDpiX;
    QString	infoPhysicalDpiY;

    infoProgramName = "<b>MyTetra</b> - smart manager<br/>for information collecting<br/><br/>";
    infoVersion = "v." + version + "<br/><br/>";
    infoAuthor = "Author: Sergey M. Stepanov<br/>";
    infoEmail = "Author Email:<i>xintrea@gmail.com</i><br/>Author Email:<i>hughvonyoung@gmail.com</i><br/>";
    infoLicense = "GNU General Public License v.3.0<br/><br/>";
    infoTargetOs = "Target OS: " + globalparameters.target_os() + "<br/>";
    infoProgramFile = "Program file: " + globalparameters.main_program_file() + "<br/>";
    infoWorkDirectory = "Work directory: " + globalparameters.work_directory() + "<br/>";

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    infoDevicePixelRatio = "Device pixel ratio: " + (QString::number(sapp_t::instance()->devicePixelRatio(), 'f', 8)) + "<br/>";
    infoPhysicalDpi = "Physical DPI (from screen): " + (QString::number(QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 8)) + "<br/>";
#endif

    infoPhysicalDpiX = "Physical DPI X (from desktop): " + (QString::number(sapp_t::instance()->desktop()->physicalDpiX(), 'f', 8)) + "<br/>";
    infoPhysicalDpiY = "Physical DPI Y (from desktop): " + (QString::number(sapp_t::instance()->desktop()->physicalDpiY(), 'f', 8)) + "<br/>";

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
    msgBox->about(this
		 , "MyTetra v." + version
		 , info);
}

void wn_t:: on_click_help_about_qt(void){
    QMessageBox *msgBox = new QMessageBox(this);

    msgBox->aboutQt(this);
}

void wn_t:: synchronization(void){
	// Сохраняются данные в поле редактирования
    save_text_area();

	// Сохраняются данные о курсорах в дереве и таблице конечных записей
    save_tree_position();
	// save_recordtable_position();
    save_editor_cursor_position();
    save_editor_scrollbar_position();

	// Считывается команда синхронизации
    QString command = appconfig.synchro_command();
	// Если команда синхронизации пуста
    if(command.trimmed().length() == 0){
	QMessageBox::warning(this
			    , tr("MyTetra: can't synchronization")
			    , tr("Do not set synchronization command.<br>Check the setting in \"Syncro\" section in \"Tools\" menu")
			    , QMessageBox::Close);

	return;
    }
	// Макрос %a заменяется на путь к директории базы данных
	// QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir();
    QDir	databaseDir(appconfig.tetra_dir());
    QString	databasePath = databaseDir.canonicalPath();

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
    _tree_screen->view()->know_model_reload();
    restore_tree_position();
	// restore_recordtable_position();
    restore_editor_cursor_position();
    restore_editor_scrollbar_position();

	// азблокируется история посещений элементов
    walkhistory.set_drop(false);
}

void wn_t:: setup_icon_actions(void){
    _action_tray_restore = new QAction(tr("&Restore window"), this);
    connect(_action_tray_restore,   &QAction::triggered,    this,   &wn_t::showNormal);

    _action_tray_maximize = new QAction(tr("Ma&ximize window"), this);
    connect(_action_tray_maximize,  &QAction::triggered,    this,   &wn_t::showMaximized);

    _action_tray_minimize = new QAction(tr("Mi&nimize window"), this);
    connect(_action_tray_minimize,  &QAction::triggered,    this,   &wn_t::hide);

    _action_tray_quit = new QAction(tr("&Quit"), this);
    connect(_action_tray_quit,      &QAction::triggered,    this,   &wn_t::application_exit);
}

void wn_t:: create_tray_icon(void){
    _tray_icon_menu = new QMenu(this);
    _tray_icon_menu->addAction(_action_tray_restore);
    _tray_icon_menu->addAction(_action_tray_maximize);
    _tray_icon_menu->addAction(_action_tray_minimize);
    _tray_icon_menu->addSeparator();
    _tray_icon_menu->addAction(_action_tray_quit);

    _tray_icon = new QSystemTrayIcon(this);
    _tray_icon->setContextMenu(_tray_icon_menu);
}

void wn_t:: set_icon(void){
    connect(_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
	   , this, SLOT(icon_activated(QSystemTrayIcon::ActivationReason)));

    QIcon icon = QIcon(":/resource/pic/logo.svg");
    _tray_icon->setIcon(icon);
    setWindowIcon(icon);

	// tray_icon->setToolTip(iconComboBox->itemText(index));
}

void wn_t:: icon_activated(QSystemTrayIcon::ActivationReason reason){
    if(QSystemTrayIcon::isSystemTrayAvailable() == false)return;
    switch(reason){
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
	if(isVisible()){
	    if(isMinimized())showNormal();
	    else hide();
	}else{
	    if(isMinimized())showNormal();
	    else show();
	}
    default:
	;
    }
}

// Слот закрытия окна
void wn_t:: closeEvent(QCloseEvent *event){
    if(_enable_real_close == false){
	if(QSystemTrayIcon::isSystemTrayAvailable() == false)return;
	// При приходе события закрыть окно, событие игнорируется
	// и окно просто делается невидимым. Это нужно чтобы при закрытии окна
	// программа не завершала работу
	if(_tray_icon->isVisible()){
	    hide();
	    event->ignore();
	}
    }
}

bool wn_t:: eventFilter(QObject *o, QEvent *e){
    Q_UNUSED(o);
	// qDebug() << "Event: " << e->type();
	// Отлавливание потери фокуса
	// QEvent::ActivationChange
    if(e->type() == QEvent::WindowDeactivate){
	qDebug() << "Main window focus deactivate, save all state.";
	save_all_state();
    }
    return false;		// Продолжать оработку событий дальше
}

void wn_t:: go_walk_history_previous(void){
    _editor_screen->save_textarea();

    id_value id = id_value(_editor_screen->misc_field("id"));
    walkhistory.add(id
		   , _editor_screen->cursor_position()
		   , _editor_screen->scrollbar_position()
		   , WALK_HISTORY_GO_PREVIOUS);
    walkhistory.set_drop(true);

    go_walk_history();
}

void wn_t:: go_walk_history_next(void){
    _editor_screen->save_textarea();

    id_value id = id_value(_editor_screen->misc_field("id"));
    walkhistory.add(id
		   , _editor_screen->cursor_position()
		   , _editor_screen->scrollbar_position()
		   , WALK_HISTORY_GO_NEXT);
    walkhistory.set_drop(true);

    go_walk_history();
}

void wn_t:: go_walk_history(void){
	// QString tree_root_id = walkhistory.tree_root_id();

	// if(_tree_screen->know_root()->root_item()->id() != tree_root_id) {
	// _tree_screen->intercept(tree_root_id);
	// }

	// Выясняется идентификатор записи, на которую надо переключиться
    auto record_id = walkhistory.record_id();

	// if(record_id.length() == 0) {
	// walkhistory.set_drop(false);
	// return;
	// }

    auto know_model_board = [&](){return _tree_screen->view()->know_model_board();};
    if(static_cast<QString>(record_id).length() > 0){
	// Выясняется путь к ветке, где находится данная запись
	QStringList absolute_path = know_model_board()->record_path(record_id);					// on know_root semantic

	//// Проверяем, есть ли такая ветка
	// if(_tree_screen->know_model_board()->is_item_valid(absolute_path) == false) {    // on know_root semantic
	// walkhistory.set_drop(false);
	// return;
	// }


	// Выясняется позицию записи в таблице конечных записей
	auto item = know_model_board()->item(absolute_path);				// on know_root semantic
	if(item){
		//// Проверяем, есть ли такая позиция
		// if(!item->item_direct(record_id)) {  // == false
		// walkhistory.set_drop(false);
		// return;
		// }
	    if(item->child_direct(record_id)){
		set_tree_position(global_root_id, absolute_path);
		// select_id(id);
		if(appconfig.remember_cursor_at_history_navigation()){
		    _editor_screen->cursor_position(walkhistory.cursor_position(record_id));
		    _editor_screen->scrollbar_position(walkhistory.scrollbar_position(record_id));
		}
	    }
		// walkhistory.set_drop(false);
	}
	// else {
	// walkhistory.set_drop(false);
	// }
    }
    walkhistory.set_drop(false);
}

// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void wn_t:: save_text_area(void){
    id_value id = id_value(_editor_screen->misc_field("id"));

    qDebug() << "MainWindow::saveTextarea() : id :" << id;

    _editor_screen->save_textarea();

    walkhistory.add(id
		   , _editor_screen->cursor_position()
		   , _editor_screen->scrollbar_position());
}

// Слот, обрабатывающий смену фокуса на виджетах
void wn_t:: on_focus_changed(QWidget *widgetFrom, QWidget *widgetTo){
    Q_UNUSED(widgetFrom);
    if(widgetTo == nullptr)return;
    qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

    return;		// Временно ничего не делает
}

