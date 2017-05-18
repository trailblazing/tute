#include <set>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QAbstractItemView>
#include <QAction>
#include <QAction>
#include <QItemSelectionModel>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QScrollArea>
#include <QString>
#include <QWidgetAction>

//#include "models/tree/tree_item.dec"

#include "main.h"

#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_branch.h"
#include "libraries/crypt/password.h"
#include "libraries/global_parameters.h"
#include "libraries/window_switcher.h"
#include "models/app_config/app_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "tree_view.h"
#include "views/browser/docker.h"
#include "views/browser/toolbar_search.h"
#include "views/browser/webview.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record/record_info_fields_editor.h"
#include "views/record_table/record_screen.h"
// #include "models/tree/KnowModel.h"
#include "libraries/qtm/blogger.h"
#include "tree_screen.h"

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;
// extern const char *index_xml_file_name;
extern const char* clipboard_items_root;
extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

// const char *action_edit_switch = "action_edit_switch";
const char* action_hide_tree_screen = "action_hide_tree_screen";
// const char	*action_set_as_session_root	= "set_as_session_root";
const char* action_find_in_base = "find_in_base";
const char* action_cursor_follow_root = "cursor_follow_root";
const char* action_cursor_follow_up = "cursor_follow_up";
const char* action_expand_all_subbranch = "expand_all_subbranch";
const char* action_collapse_all_subbranch = "collapse_all_subbranch";
const char* action_move_up_branch = "move_up_branch";
const char* action_move_dn_branch = "move_dn_branch";
const char* action_insert_sub_branch = "insert_sub_branch";
const char* action_insert_branch = "insert_branch";
const char* action_edit_branch = "edit_branch";
const char* action_delete_branch = "delete_branch";
const char* action_cut_branch = "cut_branch";
const char* action_copy_branch = "copy_branch";
const char* action_paste_branch = "paste_branch";
const char* action_paste_sub_branch = "paste_sub_branch";
const char* action_encrypt_branch = "encrypt_branch";
const char* action_decrypt_branch = "decrypt_branch";
// const char	*action_freeze_browser_view		= "freeze_browser_view";
const char* action_edit_field = "edit_field";
const char* action_editor_switch = "editor_switch";
const char* action_main_menu = "main_menu";

const char* tree_screen_viewer_name = "TreeScreenViewer";

// know_root_holder::know_root_holder(const AppConfig &appconfig, TreeScreen
// *_this)
// : _know_root(new KnowModel(_this))
// {
//// Загрузка данных
//// auto xml =
// _know_root->init_from_xml(appconfig->tetra_dir() + "/" +
// globalparameters.main_program_file() + ".xml");
// int all_count = _know_root->count_records_all();
// assert(all_count > 0);

// }

// know_root_holder::~know_root_holder() {delete _know_root;}

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ts_t)
#endif

ts_t::ts_t(QString object_name, web::Docker<Blogger>* editor_docker_, // , std::shared_ptr<AppConfig> appconfig_ , QMenu *filemenu, QMenu *toolsmenu
    wn_t* main_window)
    : QWidget(main_window)
    , _editor_docker(editor_docker_)
    , _main_window(main_window)
    // , know_root_holder(appconfig, this)
    // , ui(new Ui::MainWindow)
    // , _menubar(new QMenuBar(this))
    // , _tree_controller(new TreeController())
    , _tools_line(new QToolBar(this))
#ifdef USE_MAIN_MENU_IN_BUTTON
    , _extra_tools_line(new QToolBar(this))
#endif // USE_MAIN_MENU_IN_BUTTON
    // , _main_menu_bar(new QToolBar(this))
    // , _main_menu_button(new QPushButton("", _main_menu_bar))
    // , _main_menu_action(new QWidgetAction(_main_menu_button))
    , _main_menu_in_button(new QMenu("buttonmenu"))
    , _context_menu(new QMenu(this))
    , _tree_view(new tv_t(knowtreeview_singleton_name, this))
    // , _know_model_board(new KnowModel(_tree_view))
    , _tools_layout(new QHBoxLayout())
    // , _recordtree_search(new web::ToolbarSearch(this))
    // , _recordtree_searchlayout(new QHBoxLayout())
    , _adjustingscrollarea(new AdjustingScrollArea(_tree_view, this))
    , _treescreenlayout(new QVBoxLayout())
{
	// , _appconfig(appconfig_)
	// , _shadow_branch(new TreeModelKnow(this))
	// _know_model_board->init_from_xml(appconfig->tetra_dir() + "/" +
	// index_xml_file_name);  //
	// _know_branch->intercept(know_root_holder::know_root()->root_item());    //
	// init_from_xml(xml);  //
	// int all_count = _know_model_board->count_records_all();
	// assert(all_count > 0);

	// _know_model_board->synchronized(true);

	////    _selected_branch->init_from_xml(appconfig->datadir() +
	///"/candidate.xml");
	////    _selected_branch->_root_item->field("id", get_unical_id());   // come
	///from _root
	////    _selected_branch->_root_item->field("name", "Candidate");     // come
	///from _root

	////    _shadow_branch->init_from_xml(appconfig->datadir() +
	///"/default_page.xml");
	////    _shadow_branch->_root_item->field("id", "0");  // get_unical_id()   //
	///?

	////    _shadow_branch->_root_item->field("name", "Current Session");

	////    _recordtree_search->setMinimumWidth(100);
	////    _recordtree_search->sizePolicy().setHorizontalStretch(10);

	setObjectName(object_name);
	setup_actions();
	// auto menubar_ =
	setup_ui(); // filemenu, toolsmenu

	// resetup_model(_know_model_board->root_item());

	setup_signals();
	assembly(); // menubar_
	init_context_menu();
}

ts_t::~ts_t()
{
	// appconfig->treescreensize(QSize(this->width(), this->height()));
	// QToolBar        *_menubar; // QMenuBar *_menubar;
	// QPushButton     *_menubutton;
	// QWidgetAction   *_menuaction;
	// QMenu           *_menus_in_button;
	if (_main_menu_in_button) {
		// delete
		_main_menu_in_button->deleteLater();
		_main_menu_in_button = nullptr;
	}
	if (_context_menu) {
		// delete
		_context_menu->deleteLater();
		_context_menu = nullptr;
	}
	// if(_menuaction) {delete _menuaction; _menuaction = nullptr;}
	// if(_menubutton) {delete _menubutton; _menubutton = nullptr;}
	// if(_main_menu_bar) {
	//// delete
	// _main_menu_bar->deleteLater();
	// _main_menu_bar = nullptr;
	// }
	// delete _recordtree_search;
	if (_tools_line) {
		// delete
		_tools_line->deleteLater();
		_tools_line = nullptr;
	}
#ifdef USE_MAIN_MENU_IN_BUTTON
	if (_extra_tools_line) {
		_extra_tools_line->deleteLater();
		_extra_tools_line = nullptr;
	}
#endif // USE_MAIN_MENU_IN_BUTTON
	// delete _treescreenlayout;
	// delete _toolslayout;
	if (_tree_view) {
		// delete
		_tree_view->deleteLater();
		_tree_view = nullptr;
	}
	////    delete _shadow_branch;
	// if(_know_model_board) {
	//// delete
	// _know_model_board->deleteLater();
	// _know_model_board = nullptr;
	// }
}

void ts_t::setup_actions(void)
{
	QAction* ac;

	// void(TreeScreen::*_set_session_id)(bool) = &TreeScreen::session_root_id;

	QAction* _tree_hide = new QAction(tr("Hide tree screen"), this);

	_tree_hide->setStatusTip(tr("Hide tree screen"));
	_tree_hide->setIcon(QIcon(":/resource/pic/butterfly-left.svg")); // globalparameters.h_right_splitter()->sizes()[0]
									 // == 0 ?
									 // QIcon(":/resource/pic/butterfly-right.svg")
									 // :
									 // QIcon(":/resource/pic/butterfly-left.svg")

	// assert(_main_window->h_record_splitter());
	// connect(_main_window->h_record_splitter(), &QSplitter::splitterMoved, [&,
	// _tree_hide](int record_pos = 0, int index = 0) mutable {
	// (void) record_pos;
	// (void) index;

	// auto h_record_splitter = _main_window->h_record_splitter();
	// auto h_record_sizes = h_record_splitter->sizes();
	////	    auto vtab_record = _main_window->vtab_record();
	// auto vtab_tree = _main_window->vtab_tree();
	// auto bar_width = vtab_tree->tabBar()->geometry().width();
	// QIcon icon;
	// QString text = "";
	// if(record_pos <= bar_width){// if(0 == sizes[0]){	// ||
	// globalparameters.entrance()->browsers().size() == 0             //
	// h_right_splitter->widget(0)->width()
	// icon = QIcon(":/resource/pic/butterfly-right.svg");
	// text = tr("Show record screen");

	// h_record_sizes[1] = h_record_sizes[0] + h_record_sizes[1] - bar_width;
	// h_record_sizes[0] = bar_width;
	// h_record_splitter->setSizes(h_record_sizes);	// QList<int>() << bar_width
	// << h_record_sizes[0] + h_record_sizes[1] - bar_width);
	// if(_main_window->vtab_record()->isVisible())_main_window->vtab_record()->hide();
	// }else{
	// icon = QIcon(":/resource/pic/butterfly-left.svg");
	// text = tr("Hide record screen");
	// if(!
	// _main_window->vtab_record()->isVisible())_main_window->vtab_record()->show();
	// }
	// _tree_hide->setIcon(icon);
	// _tree_hide->setText(text);
	// _tree_hide->setToolTip(text);
	// _tree_hide->setStatusTip(text);
	////	    emit
	///globalparameters.entrance()->activated_browser()->record_screen()->tree_hide()->triggered();
	// auto h_tree_splitter = _main_window->h_tree_splitter();
	// emit h_tree_splitter->splitterMoved(h_tree_splitter->sizes()[0], 1);

	////	    if(0 != pos){
	// auto tree_sizes = h_record_splitter->sizes();
	// if(tree_sizes !=
	// appconfig->h_record_splitter_sizelist())appconfig->h_record_splitter_sizelist(tree_sizes);
	////	    }
	// });

	connect(_tree_hide, &QAction::triggered, [&]() mutable {
		auto h_tree_splitter = _main_window->h_tree_splitter();
		// auto h_record_splitter = _main_window->h_record_splitter();
		//// auto ll = h_left_splitter->geometry().left();   // 0 // width();  //
		///1366
		//// auto lr = h_left_splitter->handle(1)->geometry().right();  // 143
		//// auto rl = h_right_splitter->geometry().left();  // 142
		// auto vtab_tree = _main_window->vtab_tree();
		auto bar_width = 0; // vtab_tree->minimumSizeHint().width();	//
				    // tabBar()->geometry().width();

		// auto h_record_sizes = h_record_splitter->sizes();
		auto h_tree_sizes = h_tree_splitter->sizes();
		////	    QList<int> delta;
		//////	    QIcon icon;
		//////	    QString text = "";
		// if(h_tree_sizes[0] <= bar_width){	// if(h_record_sizes[0] <= bar_width ||
		// globalparameters.entrance()->browsers().size() == 0){	//
		// h_right_splitter->widget(0)->width()
		////		auto vtab_record_min_width =
		///globalparameters.entrance()->activated_browser()->record_screen()->minimumSizeHint().width();
		///// 6xx   // h_right_splitter->widget(0)->width();    // 0    //
		///sizeHint().width();    // 23
		////		// auto h = h_right_splitter->handle(1);
		////		// h->move(lr + shw, h->rect().top());

		////		auto record_size_memory =
		///appconfig->h_record_splitter_sizelist();
		////		h_record_sizes[0] = record_size_memory[0] > vtab_record_min_width ?
		///record_size_memory[0] : vtab_record_min_width;
		////		auto record_sum = record_size_memory[0] +
		///record_size_memory[1];
		////		auto pre_size_1 = record_sum - h_record_sizes[0];
		////		if(pre_size_1 <= 0){
		////		    auto pre_size_0 = vtab_record_min_width;	// sizes[1] -
		///sizes[0];
		////		    h_record_sizes[0] = pre_size_0 > 0 ? pre_size_0 : record_sum * 15
		//// 100;
		////		    h_record_sizes[1] = pre_size_0 > 0 ? (record_sum - pre_size_0) > 0
		///? record_sum - pre_size_0 : record_sum * 85 / 100 : record_sum * 85 /
		///100;	// sizes[1] > size_memory[1] ? size_memory[1] : sizes[1];
		////		}else h_record_sizes[1] = pre_size_1;
		//////		icon = QIcon(":/resource/pic/butterfly-left.svg");
		//////		text = tr("Hide record screen");
		////////
		///h_right_splitter->resize(h_right_splitter->sizeHint().width(),
		///h_right_splitter->height());
		// auto vtab_tree_min_width = std::max(vtab_tree->minimumSizeHint().width(),
		// vtab_tree->tabBar()->geometry().width());
		// auto tree_size_memory = appconfig->h_tree_splitter_sizelist();
		// auto tree_sum = tree_size_memory[0] + tree_size_memory[1];
		// h_tree_sizes[0] = tree_size_memory[0] > vtab_tree_min_width ?
		// tree_size_memory[0] < tree_sum ? tree_size_memory[0] : tree_sum * 15 /
		// 100 : vtab_tree_min_width;
		// h_tree_sizes[1] = tree_sum - h_tree_sizes[0] > 0 ? tree_sum -
		// h_tree_sizes[0] : tree_sum * 85 / 100;
		// vtab_tree->setMaximumWidth(_main_window->maximumWidth());	// just a very
		// big number
		// vtab_tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		// }else{
		////		// h_right_splitter->resize(h_left_splitter->sizeHint().width(),
		///h_right_splitter->height());

		////		h_record_sizes[1] = h_record_sizes[0] + h_record_sizes[1] -
		///bar_width;
		////		h_record_sizes[0] = bar_width;

		h_tree_sizes[1] = h_tree_sizes[0] + h_tree_sizes[1] - bar_width;
		h_tree_sizes[0] = bar_width; // 0;
		////		vtab_tree->resize(bar_width, vtab_tree->height());
		////		vtab_tree->setMaximumWidth(bar_width);
		// vtab_tree->setMinimumWidth(bar_width);
		// vtab_tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
		////		icon = QIcon(":/resource/pic/butterfly-right.svg");
		////		text = tr("Show record screen");
		// }
		////	    delta << h_tree_splitter->sizes()[0] - h_tree_sizes[0];
		if (h_tree_sizes != h_tree_splitter->sizes()) {
			h_tree_splitter->setSizes(h_tree_sizes); //
			emit h_tree_splitter->splitterMoved(h_tree_sizes[0], 1);
		}
		// h_record_sizes[0] = h_record_sizes[0] - delta[0];
		// h_record_sizes[1] = h_record_sizes[1] + delta[0];
		// if(h_record_sizes != h_record_splitter->sizes()){
		// h_record_splitter->setSizes(h_record_sizes);
		// emit h_record_splitter->splitterMoved(h_record_sizes[0], 1);
		// }
	});

	_actionlist[action_hide_tree_screen] = _tree_hide;

	// ac = new QAction(tr("Set as session root manually"), this);
	// ac->setStatusTip(tr("Set as session root manually, not necessary"));
	// ac->setIcon(QIcon(":/resource/pic/ginkgo_pure.svg"));
	// connect(ac, &QAction::triggered, _tree_view, &tv_t::session_root_manual);
	// _actionlist[action_set_as_session_root] = ac;

	ac = new QAction(tr("Return to absolute root"), this);
	ac->setStatusTip(tr("Return to absolute root"));
	ac->setIcon(QIcon(":/resource/pic/back_to_top.svg"));
	connect(ac, &QAction::triggered, [&] {
		_tree_view->cursor_focus(_tree_view->know_model_board()->root_item());
	});
	_actionlist[action_cursor_follow_root] = ac;

	ac = new QAction(tr("View follow up"), this);
	ac->setStatusTip(
	    tr("View follow up one level when current item reach the current root"));
	ac->setIcon(QIcon(":/resource/pic/mobile_up.svg")); // ":/resource/pic/streamline_home.svg"
	// style()->standardIcon(QStyle::SP_ArrowUp, 0, this)

	connect(ac, &QAction::triggered, [&]() mutable -> void { _tree_view->cursor_focus(); });
	_actionlist[action_cursor_follow_up] = ac;

	// азворачивание всех подветок
	// a->setShortcut(tr("Ctrl+X"));
	ac = new QAction(tr("Expand all sub items"), this);
	ac->setStatusTip(tr("Expand all sub items"));
	ac->setIcon(QIcon(":/resource/pic/expand_all_subbranch.svg"));
	connect(ac, &QAction::triggered, std::bind(&tv_t::expand_selected, _tree_view, true));
	_actionlist[action_expand_all_subbranch] = ac;

	// Сворачивание всех подветок
	ac = new QAction(tr("Collapse all sub items"), this);
	ac->setStatusTip(tr("Collapse all sub items"));
	ac->setIcon(QIcon(":/resource/pic/collapse_all_subbranch.svg"));
	connect(ac, &QAction::triggered, std::bind(&tv_t::expand_selected, _tree_view,
					     false)); // view_collapse_all_sub
	_actionlist[action_collapse_all_subbranch] = ac;

	// Перемещение ветки вверх
	ac = new QAction(tr("Move item up"), this);
	ac->setStatusTip(tr("Move item up"));
	ac->setIcon(QIcon(":/resource/pic/mobile_up.svg"));
	connect(ac, &QAction::triggered, this, [&]() mutable -> void {
		item_move_up_dn_branch(&i_t::move_up);
	}); // &TreeScreen::item_move_up_branch

	_actionlist[action_move_up_branch] = ac;

	// Перемещение ветки вниз
	ac = new QAction(tr("Move item down"), this);
	ac->setStatusTip(tr("Move item down"));
	ac->setIcon(QIcon(":/resource/pic/mobile_down.svg"));
	connect(ac, &QAction::triggered, this, [&]() mutable -> void {
		item_move_up_dn_branch(&i_t::move_dn);
	}); // &TreeScreen::item_move_dn_branch

	_actionlist[action_move_dn_branch] = ac;

	// Вставка новой подветки
	ac = new QAction(tr("Insert a new sub item"), this);
	ac->setStatusTip(tr("Insert a new sub item into selected"));
	ac->setIcon(QIcon(":/resource/pic/add_subbranch.svg"));
	connect(ac, &QAction::triggered, [&]() mutable -> void {
		_tree_view->new_item(
		    TreeIndex::item_require_treeindex(
			[&]() -> tkm_t* { return _tree_view->source_model(); },
			_tree_view->current_item()),
		    std::bind(&tv_t::new_child, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
		    [&](boost::intrusive_ptr<TreeIndex> _tree_index, QString _id,
			QString _name) -> boost::intrusive_ptr<i_t> {
			    return _tree_index->current_model()()->new_child(_tree_index, _id, _name);
		    });
	});
	_actionlist[action_insert_sub_branch] = ac;

	// Вставка новой ветки
	ac = new QAction(tr("Insert a new sibling item"), this);
	ac->setStatusTip(tr("Insert a new sibling item after selected"));
	ac->setIcon(QIcon(":/resource/pic/add_branch.svg"));
	connect(ac, &QAction::triggered, [&]() mutable {
		_tree_view->new_item(
		    TreeIndex::item_require_treeindex(
			[&]() -> tkm_t* { return _tree_view->source_model(); },
			_tree_view->current_item()) // tree_index
		    ,
		    std::bind(&tv_t::new_child, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
		    [&](boost::intrusive_ptr<TreeIndex> _tree_index, QString _id,
			QString _name) -> boost::intrusive_ptr<i_t> {
			    auto _current_model = _tree_index->current_model();
			    // auto _host_index = _modelindex->parent_index();
			    boost::intrusive_ptr<i_t> result;
			    auto host_parent = _tree_index->host()->parent();
			    assert(host_parent);
			    QList<boost::intrusive_ptr<i_t>> _alternative_items;
			    if (host_parent) {
				    _alternative_items = host_parent->children_direct(
					[&](boost::intrusive_ptr<const Linker> il) {
						return il->host()->name() == _name && il->host()->id() == _id;
					});
			    }
			    // auto _item_has_no_child_first = [&] {boost::intrusive_ptr<TreeItem>
			    // result; for(auto i : _name_same_items) {if(i->count_direct() == 0)
			    // {result = i; break;}} return result;}();
			    if (_alternative_items.size() > 0) { // && _item_has_no_child_first
				    // assert(_item_has_no_child_first->name() == _name);
				    // result = _item_has_no_child_first;

				    QMutableListIterator<boost::intrusive_ptr<i_t>> it(
					_alternative_items);
				    result = it.next();
				    while (it.hasNext())
					    result = TreeLevel::instance(
							 TreeIndex::item_require_treeindex(_current_model, result),
							 it.next())
							 ->merge(); // TreeIndex::instance(_current_model,
								    // result, result->parent()), it.next());
								    // children_transfer(_new_item, _current_model);
			    } else {
				    // Вставка новых данных в модель дерева записей
				    result = _current_model()->new_child(
					TreeIndex::item_require_treeindex(
					    [&]() -> tkm_t* { return _tree_view->source_model(); },
					    _tree_view->current_item()->parent()) // _tree_index
					,
					_id, _name);
			    }
			    return result;
		    });
	});
	_actionlist[action_insert_branch] = ac;

	// едактирование ветки
	ac = new QAction(tr("Edit item name"), this);
	ac->setStatusTip(tr("Edit name of selected item"));
	ac->setIcon(QIcon(":/resource/pic/note_edit.svg"));
	connect(ac, &QAction::triggered, _tree_view, &tv_t::modify);
	_actionlist[action_edit_branch] = ac;

	// Удаление ветки
	ac = new QAction(tr("Delete item"), this);
	ac->setStatusTip(tr("Delete selected item and all sub items"));
	ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
	connect(ac, &QAction::triggered, this, [&](bool checked = false) mutable -> void {
		Q_UNUSED(checked);
		// QModelIndexList _origin_index_list =
		// _tree_view->selectionModel()->selectedIndexes();
		QModelIndexList _selectitems =
		    tv_t::LocalLizeInitializer(_tree_view)()
			.indexes(); // ->is_index_localized(_origin_index_list) ?
				    // _origin_index_list :
				    // _tree_view->index_localize(_origin_index_list);    //
				    // _tree_view->selectionModel()->selectedIndexes(); //
		// view_remove(_selectitems, "cut");
		_tree_view->delete_permanent([&] { return _tree_view->source_model(); }, [&] {
                                   QList<boost::intrusive_ptr<i_t>> r;
                                   for (auto ix : _selectitems)
                                     r << _tree_view->source_model()->child(ix);
                                   return r; }(), &tkm_t::delete_permanent, "cut", false);
	}); // , &TreeScreen::del_branch

	_actionlist[action_delete_branch] = ac;

	// void(TreeScreen::*_cut)(bool) = &TreeScreen::view_cut;
	// Удаление ветки с сохранением копии в буфер обмена
	ac = new QAction(tr("Cut item"), this);
	ac->setStatusTip(tr("Cut item including sub items for paste"));
	ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
	connect(ac, &QAction::triggered, _tree_view, &tv_t::cut);
	_actionlist[action_cut_branch] = ac;

	// Копирование ветки в буфер обмена
	ac = new QAction(tr("Copy / Move item"), this);
	ac->setStatusTip(tr("Copy item including sub items for move"));
	ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
	connect(ac, &QAction::triggered, _tree_view, &tv_t::copy);
	_actionlist[action_copy_branch] = ac;

	// void(TreeScreen::*paste)() = &TreeScreen::branch_paste;
	// Вставка ветки из буфера обмена
	ac = new QAction(tr("Paste as sibling item"), this);
	ac->setStatusTip(tr("Paste sibling item after selected"));
	ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
	connect(ac, &QAction::triggered, [&](bool checked) -> void {
		Q_UNUSED(checked);
		// auto source_model = [&]() {return _tree_view->source_model();};
		// auto _index_child = _tree_view->current_index();
		auto _current_item = _tree_view->current_item();
		auto _parent = _current_item->parent();

		// int sibling_order = _parent->sibling_order([&]
		// (boost::intrusive_ptr<const Linker> il)
		// {
		// return il == _current_item->linker() && il->host() == _current_item &&
		// _current_item->parent() == il->host_parent();
		// });

		// auto _parent_valid = _parent == source_model()->root_item() ?
		// source_model()->root_item() == _know_model_board->root_item() ?
		// _parent
		// : _parent  //
		// cursor_follow_up_one_level()->item_direct(_parent->up_linker())
		// : _parent;

		// _index_parent = source_model()->index(_parent);

		// if(!_index_parent.isValid())
		// {
		// cursor_follow_up_one_level();
		//// _tree_view->select_as_current(_tree_view->current_index().parent());
		// _index_parent = _tree_view->current_index();
		// }

		// assert(_index_parent.isValid());

		// boost::intrusive_ptr<TreeIndex> tree_index;
		// try{tree_index = new TreeIndex([&] () -> KnowModel* {return
		// _tree_view->source_model();}, _parent, sibling_order);}
		// catch(std::exception &e) {throw e; }
		_tree_view->paste_clipboard(TreeIndex::item_require_treeindex([&]() -> tkm_t* { return _tree_view->source_model(); },
		    _parent)); // std::bind(&KnowModel::model_paste_sibling_from_clipboard,
			       // _tree_view->source_model(), std::placeholders::_1,
			       // std::placeholders::_2)
	});                    // , paste

	_actionlist[action_paste_branch] = ac;

	// QString(KnowModel::*_lock_child_paste)(const QModelIndex & _index,
	// ClipboardBranch * subbranch) = &KnowModel::lock_child_paste;
	// Вставка ветки из буфера обмена в виде подветки
	ac = new QAction(tr("Paste as sub item"), this);
	ac->setStatusTip(tr("Paste item as sub item for selected"));
	ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
	connect(ac, &QAction::triggered, [&](bool checked) mutable -> void {
		Q_UNUSED(checked);
		auto _current_item = _tree_view->current_item();
		auto _parent = _current_item->parent();

		// boost::intrusive_ptr<TreeIndex> tree_index;

		// try{
		// tree_index = new TreeIndex([&] () -> KnowModel* {
		// return _tree_view->source_model();
		// }, _current_item, 0);
		// } catch(std::exception &e) {throw e; }
		_tree_view->paste_clipboard(TreeIndex::item_require_treeindex([&]() -> tkm_t* { return _tree_view->source_model(); },
		    _parent)); // std::bind(&KnowModel::model_paste_child_from_clipboard,
			       // _tree_view->source_model(), std::placeholders::_1,
			       // std::placeholders::_2)
	});                    // branch_paste_sub

	_actionlist[action_paste_sub_branch] = ac;

	// Шифрование ветки
	ac = new QAction(tr("Encrypt item"), this);
	ac->setStatusTip(tr("Encrypt item and all subitem"));
	// actionEncryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
	connect(ac, &QAction::triggered, _tree_view, &tv_t::encrypt);
	_actionlist[action_encrypt_branch] = ac;

	// асшифровка ветки (снятие пароля)
	ac = new QAction(tr("Decrypt item"), this);
	ac->setStatusTip(tr("Decrypt item and all subitem"));
	// actionDecryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
	connect(ac, &QAction::triggered, _tree_view, &tv_t::decrypt);
	_actionlist[action_decrypt_branch] = ac;

	// ac = new QAction(tr("Pin / freeze browser view"), this);
	// ac->setStatusTip(tr("Pin / freeze browser view"));	//
	// _pin->setStatusTip(tr("Pin a note"));
	// ac->setIcon(QIcon(":/resource/pic/pentalpha.svg"));	//
	// _pin->setIcon(QIcon(":/resource/pic/pin.svg"));
	// connect(ac, &QAction::triggered
	// , [&](bool checked = false) mutable -> void {
	// Q_UNUSED(checked)
	//// Обновление инфополей в области редактирования записи
	// auto *metaeditor = globalparameters.edit_entry();	// MetaEditor
	// *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);
	// if(metaeditor) metaeditor->switch_pin(checked);
	// });
	// _actionlist[action_freeze_browser_view] = ac;

	// Открытие поиска по базе (связывание клика происходит в MainWindows)
	ac = new QAction(tr("Find in base"), this);
	ac->setStatusTip(tr("Find in base"));
	ac->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
	// connection in void MainWindow::setupSignals(void)
	_actionlist[action_find_in_base] = ac;

	// QAction             *_addnew_to_end = (new QAction(tr("Add note"), this));
	// _addnew_to_end->setIcon(QIcon(":/resource/pic/note_add.svg"));
	// connect(_addnew_to_end, &QAction::triggered, _tab_widget,
	// &web::TabWidget::addnew_to_end);
	// insert_action_as_button<QToolButton>(_toolsline, _addnew_to_end);

	// QAction             *_addnew_before = (new QAction(tr("Add note before"),
	// this));
	// QAction             *_addnew_after = (new QAction(tr("Add note after"),
	// this));

	QAction* _edit_field = (new QAction(tr("Edit properties"), this)); // (name, url, tags...)
	_edit_field->setStatusTip(tr("Edit note properties (pin, name, author, url, tags...)"));
	_edit_field->setIcon(QIcon(":/resource/pic/note_edit.svg"));
	connect(_edit_field, &QAction::triggered, this, [&](bool checked = false) mutable {
		Q_UNUSED(checked);
		edit_field_context(this->_tree_view->current_index());
	});
	_actionlist[action_edit_field] = _edit_field;

	QAction* _editor_switch = (new QAction(tr("&Editor  open/close"), this));
	_editor_switch->setStatusTip(tr("Open/Close editor"));
	_editor_switch->setIcon(QIcon(":/resource/pic/expand_console.svg"));
	connect(_editor_switch, &QAction::triggered, [&](bool checked) {
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
	// insert_action_as_button<QToolButton>(_toolsline, _settings);
	_actionlist[action_editor_switch] = _editor_switch;

	// QAction             *_action_syncro = (new QAction(tr("Synchronization"),
	// this));
	// QAction             *_action_walk_history_previous = (new
	// QAction(tr("Previous viewing note"), this));
	// QAction             *_action_walk_history_next = (new QAction(tr("Next
	// viewing note"), this));
	// QAction             *_back = (new QAction(tr("Back to item tree"), this));
	////    , _sort(new QAction(tr("Toggle sorting"), this))
	// QAction             *_print = (new QAction(tr("Print table"), this));

	// , _delete(new QAction(tr("Delete note"), this))
	// , _cut(new QAction(tr("&Cut note(s)"), this))
	// , _copy(new QAction(tr("&Copy note(s)"), this))
	// , _paste(new QAction(tr("&Paste note(s)"), this))
	// , _action_move_up(new QAction(tr("&Move Up"), this))
	// , _action_move_dn(new QAction(tr("&Move Down"), this))
	// , _find_in_base(new QAction(tr("Find in base"), this))

	QAction* main_menu_action_ = new QAction(tr("Main menu"), this);
	main_menu_action_->setStatusTip(tr("Pull down to get the main menu"));
	main_menu_action_->setIcon(QIcon(":/resource/pic/tableware.svg"));
	main_menu_action_->setToolTip(tr("Main Menu"));
	main_menu_action_->setMenu(_main_menu_in_button);
	_actionlist[action_main_menu] = main_menu_action_;
	// connect(main_menu_action_, &QAction::triggered, [&](bool){
	////			auto _main_menu_in_button = std::make_unique<QMenu>();
	// _main_menu_in_button->clear();
	// for(auto menu : _main_menu_map)
	// _main_menu_in_button->addMenu(menu.second);
	// main_menu_action_->setMenu(_main_menu_in_button);
	// });
}

// Действия при нажатии кнопки редактирования записи
void ts_t::edit_field_context(QModelIndex index_current)
{
	// Функция сохранения отредактированных полей записи в таблицу конечных
	// записей
	auto edit_field = [&](boost::intrusive_ptr<i_t> item,
#ifdef USE_EDITOR_WRAP

	    rctrl_t* _record_controller,
#endif // USE_EDITOR_WRAP
	    QString pin, QString name, QString author, QString home, QString url, QString tags) {
		qDebug() << "In edit_field()";

		// Выясняется ссылка на таблицу конечных данных
		// auto pages = _source_model->browser_pages();

		//		// Переданные отредактированные поля преобразуются в вид имя-значение
		//		QMap<QString, QString> edit_data;
		//		edit_data["pin"] = pin;
		//		edit_data["name"] = name;
		//		edit_data["author"] = author;
		//		edit_data["home"] = home;
		//		edit_data["url"] = url;
		//		edit_data["tags"] = tags;
		// Обновление новых данных в таблице конечных записей
		// auto item = _tree_view->current_item();
		auto fields_data_map = item->fields_data_map();
		boost::fusion::at_key<pin_key>(fields_data_map) = pin_value(detail::from_qstring<pin_key>(pin));
		boost::fusion::at_key<name_key>(fields_data_map) = name_value(detail::from_qstring<name_key>(name));
		boost::fusion::at_key<author_key>(fields_data_map) = author_value(detail::from_qstring<author_key>(author));
		boost::fusion::at_key<home_key>(fields_data_map) = home_value(detail::from_qstring<home_key>(home));
		boost::fusion::at_key<url_key>(fields_data_map) = url_value(detail::from_qstring<url_key>(url));
		boost::fusion::at_key<tags_key>(fields_data_map) = tags_value(detail::from_qstring<tags_key>(tags));
//		for (auto i = edit_data.begin(); i != edit_data.end(); i++)
//			item->field_dynamic(i.key(), i.value());  // field(i.key(), i.value());
//
// Обновление инфополей в области редактирования записи
// auto *editing_window = globalparameters.editor_dock(); //
// find_object<MetaEditor>(meta_editor_singleton_name);
#ifdef USE_EDITOR_WRAP
		auto editing_window = _record_controller->editing_window();
		editing_window->pin(pin);
		editing_window->name(name);
		editing_window->author(author);
		editing_window->home(home);
		editing_window->url(url);
		editing_window->tags(tags);
#endif // USE_EDITOR_WRAP
		// Сохранение дерева веток
		// find_object<TreeScreen>(tree_screen_singleton_name)
		_tree_view->know_model_save();
	};
	// qDebug() << "RecordTableController::editFieldContext()";

	// QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
	// int pos = sourceIndex.row(); // Номер строки в базе

	// Создается окно ввода данных, после выхода из этой функции окно должно
	// удалиться
	InfoFieldsEditor edit_record_dialog;

	// Выясняется ссылка на таблицу конечных данных
	auto item = _tree_view->source_model()->child(index_current);

	// Поля окна заполняются начальными значениями
	edit_record_dialog.setField("pin", detail::to_qstring(item->field<pin_key>()));
	edit_record_dialog.setField("name", detail::to_qstring(item->field<name_key>()));
	edit_record_dialog.setField("author", detail::to_qstring(item->field<author_key>()));
	edit_record_dialog.setField("home", detail::to_qstring(item->field<home_key>()));
	edit_record_dialog.setField("url", detail::to_qstring(item->field<url_key>()));
	edit_record_dialog.setField("tags", detail::to_qstring(item->field<tags_key>()));

	int i = edit_record_dialog.exec();
	if (i == QDialog::Rejected)
		return; // Была нажата отмена, ничего ненужно делать
#ifdef USE_EDITOR_WRAP
	rctrl_t* _record_controller = item->page()->record_ctrl();
#endif // USE_EDITOR_WRAP
	// Измененные данные записываются
	edit_field(
	    item,
#ifdef USE_EDITOR_WRAP

	    _record_controller,
#endif // USE_EDITOR_WRAP

	    edit_record_dialog.getField("pin"),
	    edit_record_dialog.getField("name"),
	    edit_record_dialog.getField("author"),
	    edit_record_dialog.getField("home"),
	    edit_record_dialog.getField("url"),
	    edit_record_dialog.getField("tags"));
}

// void TreeScreen::restore_menubar(){
// if(_tools_layout->indexOf(_main_menu_bar) == -1)
// {_tools_layout->addWidget(_main_menu_bar); }
// }

QMenu* ts_t::menus_in_button()
{
	return _main_menu_in_button;
}

void ts_t::setup_ui()
{ // QMenu *_filemenu, QMenu *_toolsmenu
	// _toolsline = new QToolBar(this);

	// setStyleSheet("border : 0px;");

	// QSize tool_bar_icon_size(16, 16);
	// toolsLine->setIconSize(tool_bar_icon_size);

	add_action<QToolButton>(_tools_line, _actionlist[action_hide_tree_screen]);

	// append_action_as_button<QToolButton>(_tools_line,
	// _actionlist[action_set_as_session_root]);

	// _recordtree_searchlayout = new QHBoxLayout();
	// _recordtree_searchlayout->addWidget(_recordtree_search);

	add_action<QToolButton>(_tools_line, _actionlist[action_cursor_follow_root]);
	add_action<QToolButton>(_tools_line, _actionlist[action_cursor_follow_up]);

	add_action<QToolButton>(_tools_line, _actionlist[action_insert_branch]);
	add_action<QToolButton>(_tools_line, _actionlist[action_insert_sub_branch]);
	if (appconfig->interface_mode() == "desktop") {
		add_action<QToolButton>(_tools_line, _actionlist[action_edit_branch]);
		add_action<QToolButton>(_tools_line, _actionlist[action_delete_branch]);
	}
	_tools_line->addSeparator();

	add_action<QToolButton>(_tools_line, _actionlist[action_expand_all_subbranch]);
	add_action<QToolButton>(_tools_line, _actionlist[action_collapse_all_subbranch]);

	_tools_line->addSeparator();

	add_action<QToolButton>(_tools_line, _actionlist[action_move_up_branch]);
	add_action<QToolButton>(_tools_line, _actionlist[action_move_dn_branch]);
	if (appconfig->interface_mode() == "mobile") {
		_tools_line->addSeparator();
		// add_action<QToolButton>(_tools_line,
		// _actionlist[action_freeze_browser_view]);
		add_action<QToolButton>(
		    _tools_line, _actionlist[action_find_in_base]); // Клик по этой кнопке
								    // связывается с
								    // действием в
								    // MainWindow
	}
// _tools_line->addSeparator();
#ifdef USE_MAIN_MENU_IN_BUTTON
	add_action<QToolButton>(_extra_tools_line, _actionlist[action_main_menu]);
#endif                      // USE_MAIN_MENU_IN_BUTTON
	assembly_menubar(); //_filemenu, _toolsmenu

	// _knowtreeview = new KnowTreeView(this);

	// _tree_view->setObjectName(knowtreeview_singleton_name);

	// _tree_view->setMinimumSize(1, 1);     // 150, 250

	// _tree_view->setWordWrap(true);

	//// Временно сделан одинарный режим выбора пунктов
	//// todo: Множественный режим надо выставить тогда, когда
	//// станет ясно, как удалять несколько произвольных веток так, чтобы
	//// в процессе удаления QModelIndex нижестоящих еще не удаленных
	//// веток не менялся
	//// Time to do single mode select items
	//// todo: Multi Mode should be exposed when
	//// it will be clear how to remove some arbitrary branches so that the
	//// in the removal of the lower QModelIndex not yet deleted
	//// branches has not changed
	//// knowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// _tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
	// //SingleSelection //// MultiSelection  //

	// _tree_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded   //
	// ScrollBarAlwaysOn
	// );

	//// Нужно установить правила показа контекстного самодельного меню
	//// чтобы оно могло вызываться
	// _tree_view->setContextMenuPolicy(Qt::CustomContextMenu);

	//// Представление не должно позволять редактировать элементы обычным путем
	// _tree_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// QSize size = appconfig->treescreensize();
	// resize(size);
}

// void TreeScreen::setup_model(KnowModel *treemodel)
// {
//// Создание и первичная настройка модели
////    _knowtreemodel = new KnowTreeModel(this);

//// Установка заголовка
//// QStringList headers;
//// headers << tr("Info groups");
//// knowTreeModel->setHeaders(headers);

////    // Загрузка данных
////    _treemodelknow->init_from_xml(appconfig->datadir() + "/" +
///index_xml_file_name);
// _tree_view->reset();
//// Модель подключется к виду
// _tree_view->setModel(treemodel);
// }

// void TreeScreen::update_model(KnowModel *_current_know_branch)  // too heavy
// {
// _know_branch->synchronized(false);
// auto source_model = _tree_view->source_model();
// assert(_current_know_branch == source_model);
// _tree_view->reset();
////    _tree_view->
// setup_model(_current_know_branch);
// }

void ts_t::enable_up_action()
{ // bool enable
	auto enable = _tree_view->source_model()->root_item()->id() !=
	    _tree_view->know_model_board()->root_item()->id();
	if (enable) {
		// _actionlist[action_paste_branch]->setEnabled(true);
		// _actionlist[action_paste_sub_branch]->setEnabled(true);
		_actionlist[action_cursor_follow_up]->setEnabled(true);
		_actionlist[action_cursor_follow_root]->setEnabled(true);
	} else {
		// if(
		// static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id() ==
		// _know_branch->root_item()->id() // know_root()->root_item() ==
		// _know_branch->root_item()
		// || !enable)
		// _actionlist[action_paste_branch]->setEnabled(false);
		// _actionlist[action_paste_sub_branch]->setEnabled(false);
		_actionlist[action_cursor_follow_up]->setEnabled(false);
		_actionlist[action_cursor_follow_root]->setEnabled(false);
	}
}

void ts_t::setup_signals(void)
{
	// connect(_recordtree_search, &web::ToolbarSearch::textChanged,
	// globalparameters.getFindScreen(), &FindScreen::enableFindButton);
	// connect(_recordtree_search, &web::ToolbarSearch::returnPressed,
	// globalparameters.getFindScreen(), &FindScreen::findClicked);

	// Соединение сигнал-слот чтобы показать контекстное меню по правому клику на
	// ветке
	connect(_tree_view, &tv_t::customContextMenuRequested, this, &ts_t::on_custom_contextmenu_requested);

	// Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
	connect(_tree_view, &tv_t::tapandhold_gesture_finished, this, &ts_t::on_custom_contextmenu_requested);

	// void pressed(const QModelIndex & index);
	// void clicked(const QModelIndex &index);
	// void doubleClicked(const QModelIndex &index);

	// void activated(const QModelIndex &index);
	// void entered(const QModelIndex &index);

	// Сигнал чтобы открыть на редактирование параметры записи при двойном клике
	// connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)),
	// actionList[action_edit_branch], SLOT(trigger(void)));

	// Сигнал что ветка выбрана мышкой
	// connect(knowTreeView,SIGNAL(pressed(const QModelIndex &)),
	// this,SLOT(on_knowTreeView_clicked(const QModelIndex &)));
	// connect(knowTreeView, SIGNAL(clicked(const QModelIndex &)), this,
	// SLOT(on_knowTreeView_clicked(const QModelIndex &)));
}

// _main_menu_bar [QToolBar *]
// |___main_menu_action [QWidgetAction *] <==> _main_menu_button [QPushButton *]
// |___main_menu_in_button [QMenu *]
// |__sub menu 0  [QMenu *]
// |__sub menu 1  [QMenu *]
void ts_t::assembly_menubar()
{ // QMenu *_filemenu, QMenu *_toolsmenu
	// _menubar->hide();
	// QMenu *_buttonmenu = new QMenu("Menu");
	// QWidgetAction *

	// _menuholder = new QPushButton("", _toolsline);

	// _menuaction = new QWidgetAction(_menuholder);

	// _menuaction->setIcon(QIcon(":/resource/pic/tableware.svg"));  // does not
	// work

	// menuholder->hide();

	// _menubutton->setIcon(QIcon(":/resource/pic/home_blue.svg"));

	// _menubar = globalparameters.mainwindow()->menuBar();    //new
	// QMenuBar(menubutton);
	// _menubar->setParent(_menuaction//menubutton
	// );

	// auto ac = new QAction(tr("Delete item"), this);
	// ac->setStatusTip(tr("Delete selected item and all sub items"));
	// ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
	// connect(ac, SIGNAL(triggered()), this, SLOT(del_branch()));

	// _main_menu_in_button->addMenu(_filemenu); //
	// globalparameters.mainwindow()->filemenu()
	//	_main_menu_map["file_menu"] = _filemenu;
	//	_filemenu->setContentsMargins(0, 0, 0, 0);
	// _main_menu_in_button->addMenu(_toolsmenu); //
	// globalparameters.mainwindow()->toolsmenu()
	//	_main_menu_map["tools_menu"] = _toolsmenu;
	//	_toolsmenu->setContentsMargins(0, 0, 0, 0);
	_main_menu_in_button->setContentsMargins(0, 0, 0, 0);
	// _buttonmenu->addAction(_menuaction);

	// {
	// _main_menu_button->setMenu(_main_menu_in_button);
	// // Ownership of the menu is not transferred to the push button
	////_menuaction
	// _main_menu_button->setContentsMargins(0, 0, 0, 0);

	////connect(_menuaction, &QWidgetAction::triggered, _menubar,
	///&QMenuBar::show);
	////connect(menubutton, &QPushButton::clicked, _menubar, &QMenuBar::show);

	// _main_menu_action->setDefaultWidget(_main_menu_button);
	// // The ownership is transferred to QWidgetAction!!!
	// //_menubar////_buttonmenu//

	// _main_menu_button->setIcon(QIcon(":/resource/pic/tableware.svg"));
	// // must have
	// _main_menu_button->setToolTip(tr("Main Menu"));
	// _main_menu_button->setStatusTip(tr("Pull down to get the main menu"));

	////        style()->standardIcon(
	////            // QStyle::SP_TitleBarMenuButton// "Qt"
	////            // QStyle::SP_ToolBarHorizontalExtensionButton //
	////            QStyle::SP_DirHomeIcon  //
	///QStyle::SP_ToolBarVerticalExtensionButton
	////            , nullptr //new QStyleOption(1, QStyleOption::SO_Frame)
	////            , _menubar
	////        )

	// _main_menu_button->setFlat(true);

	//// insertActionAsButton(_menubar, _menuaction);    //
	// _main_menu_bar->addAction(_main_menu_action);
	// _main_menu_bar->setContentsMargins(0, 0, 0, 0);

	////_toolsline->addWidget(_menubar);

	// return _main_menu_bar;
	// }
}

void ts_t::init_context_menu()
{
	_context_menu->addAction(_actionlist[action_hide_tree_screen]);
	// _context_menu->addAction(_actionlist[action_set_as_session_root]);

	// _context_menu->addAction(_actionlist[action_freeze_browser_view]);
	_context_menu->addAction(_actionlist[action_find_in_base]);
	_context_menu->addAction(_actionlist[action_edit_field]);
	_context_menu->addAction(_actionlist[action_editor_switch]);

	_context_menu->addAction(_actionlist[action_cursor_follow_root]);
	_context_menu->addAction(_actionlist[action_cursor_follow_up]);

	_context_menu->addAction(_actionlist[action_insert_branch]);
	_context_menu->addAction(_actionlist[action_insert_sub_branch]);
	_context_menu->addAction(_actionlist[action_edit_branch]);
	_context_menu->addAction(_actionlist[action_delete_branch]);
	_context_menu->addSeparator();
	_context_menu->addAction(_actionlist[action_expand_all_subbranch]);
	_context_menu->addAction(_actionlist[action_collapse_all_subbranch]);
	_context_menu->addSeparator();
	_context_menu->addAction(_actionlist[action_move_up_branch]);
	_context_menu->addAction(_actionlist[action_move_dn_branch]);
	_context_menu->addSeparator();
	_context_menu->addAction(_actionlist[action_cut_branch]);
	_context_menu->addAction(_actionlist[action_copy_branch]);
	_context_menu->addAction(_actionlist[action_paste_branch]);
	_context_menu->addAction(_actionlist[action_paste_sub_branch]);
	_context_menu->addSeparator();
	_context_menu->addAction(_actionlist[action_encrypt_branch]);
	_context_menu->addAction(_actionlist[action_decrypt_branch]);
}

void ts_t::append_main_menu()
{
	_context_menu->addSeparator();
	// auto main_menu_action = _context_menu->addSection("main menu");
	// main_menu_action->setMenu(_main_menu_in_button);            //
	// _context_menu->addAction(_main_menu_action);
	// _context_menu->addAction(_actionlist[action_main_menu]);
	for (auto menu : _main_window->main_menu_map())
		_context_menu->addMenu(menu.second);
}

void ts_t::assembly()
{
	_tools_layout->addWidget(_tools_line);
	_tools_line->setContentsMargins(0, 0, 0, 0);
	_tools_layout->addStretch();

	// {
	// _tools_layout->addWidget(menubar_);
	// menubar_->setContentsMargins(0,0,0,0);
	// }

	_tools_layout->setSpacing(0);
	_tools_layout->setMargin(0);
	_tools_layout->setContentsMargins(0, 1, 0, 1);
#ifdef USE_MAIN_MENU_IN_BUTTON
	_tools_layout->addWidget(_extra_tools_line);
#endif // USE_MAIN_MENU_IN_BUTTON
	// _treescreenlayout = new QVBoxLayout();
	_treescreenlayout->setObjectName("treescreen_QVBoxLayout");

	// _treescreenlayout->addLayout(_recordtree_searchlayout);
	_treescreenlayout->addLayout(_tools_layout);

	_treescreenlayout->setSpacing(0);
	_treescreenlayout->setMargin(0);
	_treescreenlayout->setContentsMargins(0, 0, 0, 0);

	////    _adjustingscrollarea = new AdjustingScrollArea(_tree_view, this);
	////    //    _treescreenlayout->addWidget(_adjustingscrollarea = new
	///AdjustingScrollArea);  // , 0, 1
	////    _tree_view->setSizePolicy(QSizePolicy::Preferred,
	///QSizePolicy::Preferred);
	// _adjustingscrollarea->setSizePolicy(QSizePolicy::Preferred  //
	// QSizePolicy::MinimumExpanding
	// , QSizePolicy::Preferred
	// );
	// _adjustingscrollarea->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	// _adjustingscrollarea->setWidgetResizable(true);
	// _adjustingscrollarea->setStyleSheet("QScrollArea { border: 0px solid blue
	// }");
	////    _adjustingscrollarea->setWidget(_tree_view);
	////    _tree_view->viewport()->installEventFilter(_adjustingscrollarea);

	// _treescreenlayout->setColumnStretch(1, 1);
	QVBoxLayout* baseLayout = new QVBoxLayout();
	baseLayout->setSpacing(0);
	baseLayout->setMargin(0);
	baseLayout->setContentsMargins(0, 0, 0, 0);
	_adjustingscrollarea->setContentsMargins(0, 0, 0, 0);
	baseLayout->addWidget(_adjustingscrollarea);

	// RecordView *record_view = _record_controller->view(); //    auto
	// record_view = new QWidget(this);
	// record_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	// _vertical_scrollarea->setWidget(record_view);
	// record_view->viewport()->installEventFilter(_vertical_scrollarea);
	// QVBoxLayout *baseLayout = new QVBoxLayout();
	// baseLayout->addWidget(_vertical_scrollarea);

	// _treeviewhelpwidget = new TreeViewHelpWidget(this);

	_treescreenlayout->addLayout(baseLayout); // addWidget(_adjustingscrollarea);
						  // // _tree_view //
						  // _treeviewhelpwidget

	setLayout(_treescreenlayout);

	// The boundaries are removed, as this facility will be used as a widget    //
	// Границы убираются, так как данный объект будет использоваться как виджет
	QLayout* lt;
	lt = layout();
	lt->setMargin(0);
	lt->setContentsMargins(0, 0, 0, 0); // setContentsMargins(0, 2, 0, 0);
	lt->setSpacing(0);
}

tv_t* ts_t::view()
{
	return _tree_view;
}

// void TreeScreen::view_collapse_all_sub(void)
// {
// std::function<void(QModelIndex index, bool mode)> expand_or_collapse_recurse
// = [&](QModelIndex index, bool mode) {
// _tree_view->setExpanded(index, mode);

// int i = 0;

// while((index.child(i, 0)).isValid()) {
// expand_or_collapse_recurse(index.child(i, 0), mode);
// i++;
// }
// };

//// Получение индексов выделенных строк
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();

// for(int i = 0; i < selectitems.size(); ++i)
// expand_or_collapse_recurse(selectitems.at(i), false);
// }

// void TreeScreen::expand_or_collapse_recurse(QModelIndex index, bool mode)
// {
// _tree_view->setExpanded(index, mode);

// int i = 0;

// while((index.child(i, 0)).isValid()) {
// expand_or_collapse_recurse(index.child(i, 0), mode);
// i++;
// }
// }

// void TreeScreen::item_move_up_branch(void)
// {
// item_move_up_dn_branch(
// &TreeItem::move_up  //1
// );
// }

// void TreeScreen::item_move_dn_branch(void)
// {
// item_move_up_dn_branch(
// &TreeItem::move_dn  //-1
// );
// }

void ts_t::item_move_up_dn_branch(int (i_t::*_move)())
{ // int direction
	// Если ветку нельзя перемещать
	if (!move_checkenable())
		return;
	// Получение индекса выделенной строки
	QModelIndex _index = _tree_view->current_index();
	if (_index.isValid()) {
		// Ветка перемещается
		QModelIndex index_after_move;

		// if(direction == 1)
		// index_after_move = _tree_view->source_model()->branch_move_up(index);
		// else
		// index_after_move = _tree_view->source_model()->branch_move_dn(index);

		index_after_move = _tree_view->source_model()->move_up_dn(_index, _move);

		_tree_view->know_model_board()->synchronized(false);
		// Установка курсора на позицию, куда была перенесена ветка
		if (index_after_move.isValid() && index_after_move != _index) {
			auto it = _tree_view->source_model()->child(index_after_move);
			_tree_view->select_as_current(TreeIndex::item_require_treeindex([&] { return _tree_view->source_model(); }, it));
		}
		// Сохранение дерева веток
		// find_object<TreeScreen>(tree_screen_singleton_name)
		_tree_view->know_model_save();
	}
}

// limit to single selection, deprecated
bool ts_t::move_checkenable(void)
{
	//// Получение списка индексов QModelIndex выделенных элементов
	// QModelIndexList selectitems =
	// _tree_view->selectionModel()->selectedIndexes();

	//// Если выбрано более одной ветки
	// if(selectitems.size() > 1) {
	// QMessageBox messageBox(this);
	// messageBox.setWindowTitle(tr("Unavailable action"));
	// messageBox.setText(tr("You've selected ") +
	// QString::number(selectitems.size()) + tr(" items.\nPlease select single
	// item for moving."));
	// messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
	// messageBox.exec();
	// return false;

	// } else
	return true;
}

// boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_sibling(void)
// {
// qDebug() << "In ins_branch()";

// return branch_insert_smart<true>();
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_sub(void)
// {
// qDebug() << "In ins_subbranch()";

// return branch_insert_smart<false>();
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex
// _current_index, QString name, bool insert_sibling_branch)
// {
// return add_branch(_current_index, name, insert_sibling_branch);
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex
// _current_index, boost::intrusive_ptr<TreeItem> it, bool
// insert_sibling_branch, KnowModel *_current_model)
// {

//// Получение ссылки на узел, который соответствует выделенной строке
// auto current_item = _current_model->item(_current_index);

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);

//// Получение уникального идентификатора
// QString id = get_unical_id();
// it->field("id", id);
// QModelIndex setto;

//// Вставка данных и установка курсора

//// Одноранговая ветка
// if(insert_sibling_branch) {
//// Вставка новых данных в модель дерева записей
// _current_model->add_sibling_branch(_current_index, it);

//// Установка курсора на только что созданную позицию

//// Чтобы вычислить позицию, надо синхронно получить parent элемента,
//// на уровне которого должен был создасться новый элемент.
//// Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
//// Затем у объекта Item выяснить количество элементов, и установить
//// засветку через метод index() относительно parent в виде QModelIndex
////        QModelIndex
// setto = static_cast<TreeModel
// *>(_current_model)->index(current_item->parent()->current_count() - 1, 0,
// _current_index.parent());
// _tree_view->selectionModel()->setCurrentIndex(setto,
// QItemSelectionModel::ClearAndSelect);
// } else {
//// Подветка

//// Вставка новых данных в модель дерева записей
// _current_model->add_child_branch(_current_index, it);

//// Установка курсора на только что созданную позицию
////        QModelIndex
// setto = _current_model->index_child(_current_index,
// current_item->current_count() - 1);
// _tree_view->selectionModel()->setCurrentIndex(setto,
// QItemSelectionModel::ClearAndSelect);

//// А можно было установить курсор на нужную позицию и так
////
///knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
//// QItemSelectionModel::ClearAndSelect);
// }

//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// save_knowtree();

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
// return  _current_model->item(setto);
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex
// _current_index, QString name, bool insert_sibling_branch,
// std::shared_ptr<KnowModel> _current_model)
// {
// assert(_current_model);
// return add_branch(_current_index,  name,  insert_sibling_branch,
// _current_model.get());
// }

// void TreeScreen::view_paste_from_search(
// TreeIndex _modelindex  // std::function<KnowModel *()> _current_model,
// QModelIndex _current_index
// , boost::intrusive_ptr<TreeItem> _result_item
// , std::function<bool(boost::intrusive_ptr<TreeItem>)> _substitute_condition
// )
// {
// auto _current_model = _modelindex.current_model();
// auto _current_index = _modelindex.current_index();

// if(_result_item->count_direct() > 0 && _current_index.isValid()) {

////        if(!_result_item->child(0)->unique_page()) {
////
///globalparameters.entrance()->new_browser(_result_item->child(0)->field("url"));
////        }

////        RecordController *_record_controller = //
///resultset_item->unique_page()
////
///_result_item->child(0)->unique_page()->view()->record_ctrl();   //
///globalparameters.record_screens()->record_ctrl();
////        RecordModel *controller_source_model =
///_record_controller->source_model();

////        //        setup_model(_know_branch);

// _actionlist[action_return_to_root]->setEnabled(true);

////        //    assert(controller_source_model->tree_item() ==
///_selected_branch->_root_item);

////        controller_source_model->reset_internal_data();  // delete
///source_model;   // dangerous!!!

////        //    std::shared_ptr<RecordTable> target_data =
///source_model->tree_item()->tabledata();    // resultset_item->tabledata();
////        //    controller->init();

////        //    //    std::shared_ptr<QDomDocument> doc =
///std::make_shared<QDomDocument>();
////        //    auto dommodel = resultset_item->export_to_dom();    //
///source->init(startItem, QDomElement());
////        //    resultset_item->remove_all_children();              //
///dommodel has noting to do with children!

////        //    auto result_data = std::make_shared<RecordTable>(dommodel); //
///resultset_item->tabledata(dommodel);               //
///resultset_item->table_clear();    // already insert into table_init

////        //    //    resultset_data->init(resultset_item, dommodel);     //
///just for init the item bind for reault set record table

////        //    //    _knowtreemodel->save();

////        //        controller_source_model->init_source_model(resultset_item
/////? should we put it to treemodelknow? not the source of browser_pages
////        //                                                   ,
///_record_controller
////        //                                                   ,
///_record_controller->record_screen()
////        //                                                   ,
///globalparameters.mainwindow()
////        //                                                   ,
///globalparameters.meta_editor()
////        //                                                   // ,
///globalparameters.record_screens()
////        //                                                  );

// assert(!_current_model()->index(_result_item).isValid());
// auto it =   // view_paste_as_sibling
// view_paste_from_children(_modelindex, _result_item, _substitute_condition);
// // setup_model(_result_item);   //
// _know_branch->intercept(boost::intrusive_ptr<TreeItem>(new
// TreeItem(QMap<QString, QString>(), nullptr, resultset_item)));

////        auto idx = _tree_view->source_model()->index(it);
// if(it.size() > 0) {
// _tree_view->select_and_current(it[0]);

////    controller_source_model->root(resultset_item); //
///->record_table(result_data);

////    for(int i = 0; i < resultset_item->tabledata()->size(); i++) {

//// _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
////            _shadow_candidate_model->_root_item->tabledata()->size()
////            , resultset_item->tabledata()->record(i)
////        );
////    }

////    reset_tabledata(std::make_shared<RecordTable>(resultset_item,
///dommodel));

////    controller->init();

////    // Устанавливается текстовый путь в таблице конечных записей для
///мобильного варианта интерфейса
////    if(appconfig->getInterfaceMode() == "mobile") {

////        QStringList path = item->getPathAsName();

////        // Убирается пустой элемент, если он есть (это может быть корень, у
///него нет названия)
////        int emptyStringIndex = path.indexOf("");
////        path.removeAt(emptyStringIndex);

////
///find_object<RecordTableScreen>(table_screen_singleton_name)->setTreePath(path.join("
///> "));

////    }

//// Width of the tree column set as to always accommodates data  // Ширина
///колонки дерева устанавливается так чтоб всегда вмещались данные
// _tree_view->resizeColumnToContents(0);

//// Переключаются окна (используется для мобильного интерфейса)
////    globalparameters.window_switcher()->switchFromTreeToRecordtable();
//// globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
// }
// }
// }

////template<>
// boost::intrusive_ptr<TreeItem> TreeScreen::branch_add(QModelIndex
// _current_index, QString name, KnowModel *_current_model)
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);
//// Получение ссылки на узел, который соответствует выделенной строке
////    auto item = _current_model->item(_current_index);

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);

//// Получение уникального идентификатора
// QString id = get_unical_id();
// QModelIndex setto;

//// Вставка данных и установка курсора

//// Одноранговая ветка
////    if(insert_sibling_branch) {

// auto parent = _tree_view->source_model()->item(_current_index)->parent();
// assert(parent);
// auto same = parent->child(parent->is_name_exists(name));

// if(same) {  // && same->is_empty()
// assert(same->name() == name);
// result = same;
////            children_transfer(_new_item, _current_model);
// } else {
//// Вставка новых данных в модель дерева записей
// result = _current_model->lock_sibling_add(_current_index, id, name);
// }

//// Установка курсора на только что созданную позицию

//// Чтобы вычислить позицию, надо синхронно получить parent элемента,
//// на уровне которого должен был создасться новый элемент.
//// Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
//// Затем у объекта Item выяснить количество элементов, и установить
//// засветку через метод index() относительно parent в виде QModelIndex
////        QModelIndex
// setto = _current_model->index(result);  //(item->parent()->current_count() -
// 1, 0, _current_index.parent());
// _tree_view->selectionModel()->setCurrentIndex(setto,
// QItemSelectionModel::ClearAndSelect);

////    }

// _know_model_board->synchronized(false);
//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// knowtree_save();

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
// assert(result->name() == name);
// assert(result == _current_model->item(setto));
// return  result; // _current_model->item(setto);
// }

// QList<boost::intrusive_ptr<TreeItem>> TreeScreen::view_delete_indexes(
// std::function<KnowModel *()> _current_model
// , QModelIndexList            _origin_index_list
// , QString                    _mode
// , bool                       _cut_branch_confirm
// )
// {
// QList<boost::intrusive_ptr<TreeItem>> _result;

// QModelIndexList _index_list = is_index_localized(_origin_index_list) ?
// _origin_index_list : index_localize(_origin_index_list);

// if(_index_list.size() > 0) {

////        auto _source_model = _tree_view->source_model();

//// На время удаления блокируется главное окно
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->blockSignals(true);

////preparations
// auto index_to_be_delete_first = _index_list.first();
// auto _index_common_parent = index_to_be_delete_first.parent();
////    auto index_to_be_delete_last = _index_list.last();
// auto _item_to_be_deleted_first =
// _current_model()->item(index_to_be_delete_first);
// auto deleted_position_first = _item_to_be_deleted_first->sibling_order(); //
// items_update_selected();
////            auto deleted_position_last =
///_source_model->item(index_to_be_delete_last)->sibling_order();
// auto _item_parent = _item_to_be_deleted_first->parent();

//// Список имен веток, которые нужно удалить
// QStringList branches_name;

// for(int i = 0; i < _index_list.size(); ++i) {
// QModelIndex index = _index_list.at(i);

// if(index.isValid() && index.parent() == _index_common_parent) {
// auto it = _current_model()->item(index);
// branches_name << it->field("name");
////                _index_list.push_back(index);
// }
// }

// std::function<bool()> has_encrypt = [&]() {
// bool disableFlag = false;

//// Перебираются удаляемые ветки
// for(int i = 0; i < _index_list.size(); ++i) {
// QModelIndex index = _index_list.at(i);
// auto it = _current_model()->item(index);

//// Если у ветки установлен флаг шифрования
// if(it->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
// disableFlag = true;
// break;
// }

//// Проверяется наличие флага шифрования у всех подветок
// QList<QStringList> cryptFlagsList =
// it->path_children_all_as_field(boost::mpl::c_str < crypt_type > ::value);

// foreach(QStringList cryptFlags, cryptFlagsList) {
// if(cryptFlags.contains("1")) {
// disableFlag = true;
// break;
// }
// }

// if(disableFlag)break;

// } // Закрылся цикл перебора всех выделенных для удаления веток

//// Если в какой-то ветке обнаружено шифрование
// if(disableFlag) {
// QMessageBox messageBox(this);
// messageBox.setWindowTitle(tr("Unavailable action"));
// messageBox.setText(tr("In your selected data found closed item. Action
// canceled."));
// messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
// messageBox.exec();

//// Разблокируется главное окно
////            find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
////            find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->blockSignals(false);

////            return _result;
// }

// return disableFlag;
// };

//// Если системный пароль не установлен, зашифрованные ветки удалять нельзя
// if(!((globalparameters.crypt_key().size() == 0) && has_encrypt())) {
//// Создается окно с вопросом, нужно удалять ветки или нет
// QString title, text, del_button;
// bool enable_question = true;

// if(_mode == "delete") {
// title = tr("Delete item(s)");
// text = tr("Are you sure you wish to delete item(s) <b>") +
// branches_name.join(", ") + tr("</b> and all sub items?");
// del_button = tr("Delete");

// enable_question = true;
// } else if(_mode == "cut") {
// title = tr("Cut item");
// text = tr("Are you sure you wish to cut item <b>") + branches_name.join(", ")
// + tr("</b> and all sub items?");
// del_button = tr("Cut");

// if(appconfig->cut_branch_confirm() && _cut_branch_confirm) enable_question =
// true;
// else enable_question = false;
// }

// bool enable_del = true;

// if(enable_question) {
// QMessageBox message_box(this);
// message_box.setWindowTitle(title);
// message_box.setText(text);
// message_box.addButton(tr("Cancel"), QMessageBox::RejectRole);
// QAbstractButton *delete_button = message_box.addButton(del_button,
// QMessageBox::AcceptRole);
// message_box.exec();

// if(message_box.clickedButton() == delete_button) {
// enable_del = true;
// } else {
// enable_del = false;
// }
// }

//// Если удаление подтверждено
// if(enable_del) {
//// Сохраняется текст в окне редактирования
//// Нужно, чтобы нормально удалилась текущая редактируемая запись,
//// если она находится в удаляемой ветке
////        find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->save_text_area();

//// Сортировка списка индексов по вложенности методом пузырька
//// Индексы с длинным путем перемещаются в начало списка
// for(int i = 0; i < _index_list.size(); i++) {
// for(int j = _index_list.size() - 1; j > i; j--) {
// QStringList path_1 = (_current_model()->item(_index_list.at(j -
// 1)))->path_absolute();
// QStringList path_2 =
// (_current_model()->item(_index_list.at(j)))->path_absolute();

// if(path_1.size() < path_2.size())
// _index_list.swap(j - 1, j);
// }
// }

// qDebug() << "Path for delete";

// for(int i = 0; i < _index_list.size(); ++i)
// qDebug() << (_current_model()->item(_index_list.at(i)))->path_absolute();

//// auto _source_model = _tree_view->source_model();
////_source_model->beginRemoveRows(_index_common_parent,
///_index_list.first().row(), _index_list.last().row());

//// Вызов удаления веток
// for(int i = 0; i < _index_list.size(); ++i) {
// _result.append( // model_delete_one(
// _current_model()->model_delete_index(_index_list.at(i))
// );
// }

//// _source_model->endRemoveRows();
////                _source_model = _tree_view->source_model();
// _current_model()->index_update(_index_common_parent);

// QModelIndex setto;

// if(_item_parent) {
// int new_count = _item_parent->count_direct();

//// Одноранговая ветка
// if(new_count > 0) {
// int new_position = 0;

// if((deleted_position_first - 1) >= 0) {
// new_position = deleted_position_first - 1;
// }

////                    else
////                        // if(deleted_position_last + 1 < new_count)
////                    {
////                        new_position = 0;   //deleted_position_last + 1;
////                    }

////                    int new_pos = (deleted_position_first > new_count - 1) ?
///new_count - 1 : ((deleted_position_first - 1) >= 0) ? deleted_position_first
///: 0;

// setto = _current_model()->index(new_position, 0,
// index_to_be_delete_first.parent());
// _tree_view->select_and_current(setto);
////                        _tree_view->selectionModel()->select(setto,
///current_tree_selection_mode);   // current_tree_selection_mode
////                        _tree_view->selectionModel()->setCurrentIndex(setto,
///current_tree_current_index_mode);   // current_tree_selection_mode    //
///ClearAndSelect

// } else {

////            //        setto = _root_model->index_child(_current_index,
///current_item->direct_children_count() - 1);
////            setto = static_cast<TreeModel
///*>(_current_know_branch)->index(index.parent().row(), 0,
///index.parent().parent());
////            _tree_view->selectionModel()->setCurrentIndex(setto,
///QItemSelectionModel::ClearAndSelect);

// while(_item_parent != _know_model_board->root_item()) {

// if(_item_parent != _current_model()->root_item() &&
// _current_model()->index(_item_parent).isValid()) {
// _tree_view->select_and_current(setto);
////
///_tree_view->selectionModel()->select(_source_model->index(_item_parent),
///current_tree_selection_mode);
////
///_tree_view->selectionModel()->setCurrentIndex(_source_model->index(_item_parent),
///current_tree_current_index_mode);   // ClearAndSelect

// break;
// } else { // if(_parent != _know_model_board->root_item())
// _item_parent = _item_parent->parent();
// cursor_follow_up_one_level();
// }
// }

////            if(index.parent().parent().isValid()) {view_up_one_level();}
////            else {view_return_to_root();}
// }

// }

// qDebug() << "Delete finish";

////        update_model(_current_know_branch);

//// Сохранение дерева веток
////        find_object<TreeScreen>(tree_screen_singleton_name)->
// knowtree_save();
////        reload_knowtree();
// qDebug() << "Save new tree finish";

////            _actionlist[action_paste_branch]->setEnabled(false);
////            _actionlist[action_paste_sub_branch]->setEnabled(false);
// } else {
// _actionlist[action_paste_branch]->setEnabled(true);
// _actionlist[action_paste_sub_branch]->setEnabled(true);
// }

//// Разблокируется главное окно
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->blockSignals(false);

// tree_empty_controll();
// tree_crypt_control();

// } // Закрылось условие что системный пароль не установлен
// }

// return _result;
// }

//// Удаление выбранных веток, вызывается при выборе соотвествущей
//// кнопки или пункта меню
//// Delete the selected branches, called when the corresponding
//// button or menu item
// QList<boost::intrusive_ptr<Linker>> TreeScreen::view_remove(const
// QModelIndexList &_index_list, const QString &mode, const bool
// _cut_branch_confirm)
// {
// qDebug() << "In del_branch()";

//// На время удаления блокируется главное окно
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->blockSignals(true);

// assert(_tree_view->is_index_localized(_index_list));

////    // Получение списка индексов QModelIndex выделенных элементов
////    QModelIndexList _origin_index_list =
///_tree_view->selectionModel()->selectedIndexes();
////    QModelIndexList _index_list = is_index_localized(_origin_index_list) ?
///_origin_index_list : index_localize(_origin_index_list);

////    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
// auto _source_model = [&]() {return _tree_view->source_model();}; //
// std::bind(_source_model_func, _tree_view);
// QList<boost::intrusive_ptr<TreeItem>> delete_items;

// if(_index_list.size() > 0) {
// for(auto _index : _index_list) {
// delete_items << _source_model()->item(_index);
// }

////        auto _source_model = _tree_view->source_model();
// _source_model()->synchronized(false);
// }

// return view_remove(_source_model, delete_items, mode, _cut_branch_confirm);

// }

//// Удаление одной ветки и её подветок
//// Delete one branch and its podvetok / garter
// boost::intrusive_ptr<TreeItem>
// TreeScreen::model_delete_one(std::function<KnowModel *()> _current_model,
// QModelIndex _index_delete)
// {

// boost::intrusive_ptr<TreeItem> _item_deleted(nullptr);

// std::function<void(boost::intrusive_ptr<TreeItem>)> close_tab_recursive =
// [&](boost::intrusive_ptr<TreeItem> it) {
// if(it->is_registered_to_browser())   // item_to_be_deleted->unique_page()
// it->bounded_page()->record_ctrl()->page_remove(it->id()); //
// (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

// if(it->count_direct() > 0) {
// for(int i = 0; i < it->count_direct(); i++) {
// close_tab_recursive(it->item_direct(i));
// }
// }
// };

// if(_index_delete.isValid()) {

// qDebug() << "Label 1";

//// Получение узла, который соответствует обрабатываемому индексу
// _item_deleted = _current_model()->item(_index_delete);

//// try to rewrite, but below is just memory operation which is not we want,
///but but, the original code is also a memory ooperation too? why?
// if( // _index_delete.row() > 0 &&
// _current_model()->root_item()->count_direct() > 0) {  // (index.row() > 0) ==
// index.isValid()

// auto _parent = _item_deleted->parent();

// if(_parent) _parent->child_remove(_item_deleted);

// while(_parent->self_empty_remove()) {
// _parent = _parent->parent();

// if(!_parent)break;
// }

// }

////        int deleted_position = _item_to_be_deleted->sibling_order();
/////item_to_be_deleted->parent()->sibling_order(item_to_be_deleted);

// qDebug() << "Label 2, branch id:" << _item_deleted->field("id") << "name:" <<
// _item_deleted->field("name");

//// Получение пути к элементу
// QStringList _delete_path = _item_deleted->path_absolute();

// qDebug() << "Label 3";

//// Получение путей ко всем подветкам
// QList<QStringList> sub_branches_path = _item_deleted->path_children_all();

// qDebug() << "Label 4";

//// Сортировка массива веток по длине пути
// qSort(sub_branches_path.begin(), sub_branches_path.end()
// , [](const QStringList & list1, const QStringList & list2) {return
// list1.size() < list2.size();}    // compare_qstringlist_length
// );

// qDebug() << "Label 5";

//// Удаление всех таблиц конечных записей для нужных подветок
//// Удаление всех подчиненных элементов для нужных подветок
//// Вначале пробегаются самые длинные ветки а потом более короткие
// for(int i = sub_branches_path.size() - 1; i >= 0; i--) {
// qDebug() << "Delete subbranch, id:" <<
// _current_model()->item(sub_branches_path.at(i))->field("id") << "name:" <<
// _current_model()->item(sub_branches_path.at(i))->field("name");

// (_current_model()->item(sub_branches_path.at(i)))->children_clear();
//// ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
// }

// qDebug() << "Label 6";

//// Удаление таблицы конечных записей для самой удаляемой ветки
//// Удаление подчиненных элементов для самой удаляемой ветки
// qDebug() << "Delete rootbranch, id:" <<
// _current_model()->item(_delete_path)->field("id") << "name:" <<
// _current_model()->item(_delete_path)->field("name");

// (_current_model()->item(_delete_path))->children_clear();
//// ( knowTreeModel->getItem(path) )->removeAllChildren();

// qDebug() << "Label 7";

//// Удаление ветки на экране, при этом удалятся все подветки
// qDebug() << "This branch have row() as" << _index_delete.row();

// if(_index_delete.isValid()) qDebug() << "Index valid";
// else qDebug() << "Index non valid";

////        _source_model->beginRemoveRows(_index_to_be_delete.parent(),
///_index_to_be_delete.row(), _index_to_be_delete.row());
// _current_model()->removeRows(_index_delete.row(), 1, _index_delete.parent());
////        _source_model->endRemoveRows();

////        items_update_selected();

////    // Удаление всех нужных подветок
////    // Пробегаются самые длинные ветки а потом более короткие
////    for(int i = subbranchespath.size() - 1; i >= 0; i--)
////        if(knowTreeModel->isItemValid(subbranchespath.at(i))) {
////            TreeItem *current_item =
///knowTreeModel->getItem(subbranchespath.at(i));
////            delete current_item;
////        }

////    if(!reocrd_controller) {
//// globalparameters.entrance()->new_browser(QUrl(web::Browser::_defaulthome));
////    }

////        if(item_to_be_deleted->is_registered_to_browser())   //
///item_to_be_deleted->unique_page()
////
///item_to_be_deleted->unique_page()->record_ctrl()->remove_child(item_to_be_deleted->id());
///// (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

// close_tab_recursive(_item_deleted);

// qDebug() << "Label 8";

////        // items_update_selected();
////        QModelIndex setto;
////        auto _parent = _item_to_be_deleted->parent();

////        if(_parent) {
////            int new_count = _parent->count_direct();

////            // Одноранговая ветка
////            if(new_count > 0) {
////                int new_pos = (deleted_position > new_count - 1) ? new_count
///- 1 : ((deleted_position - 1) >= 0) ? deleted_position : 0;

////                setto = _current_model->index(new_pos, 0, index.parent());
////                _tree_view->selectionModel()->select(setto,
///current_tree_selection_mode);
////                _tree_view->selectionModel()->setCurrentIndex(setto,
///current_tree_selection_mode);   // ClearAndSelect

////            } else {

////                //            //        setto =
///_root_model->index_child(_current_index,
///current_item->direct_children_count() - 1);
////                //            setto = static_cast<TreeModel
///*>(_current_know_branch)->index(index.parent().row(), 0,
///index.parent().parent());
////                //
///_tree_view->selectionModel()->setCurrentIndex(setto,
///QItemSelectionModel::ClearAndSelect);

////                while(_parent != _know_model_board->root_item()) {

////                    if(_parent != _current_model->root_item() &&
///_current_model->index(_parent).isValid()) {
////
///_tree_view->selectionModel()->select(_current_model->index(_parent),
///current_tree_selection_mode);
////
///_tree_view->selectionModel()->setCurrentIndex(_current_model->index(_parent),
///current_tree_selection_mode);   // ClearAndSelect

////                        break;
////                    } else { // if(_parent !=
///_know_model_board->root_item())
////                        _parent = _parent->parent();
////                        cursor_up_one_level();
////                    }
////                }

////                //            if(index.parent().parent().isValid())
///{view_up_one_level();}
////                //            else {view_return_to_root();}
////            }

////        }

////        assert(item_to_be_deleted == _current_know_branch->item(setto));
// }

// return _item_deleted;
// }

// boost::intrusive_ptr<Linker>
// TreeScreen::view_cut(boost::intrusive_ptr<TreeItem> target)
// {
// QList<boost::intrusive_ptr<Linker>> results;
////    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
// auto _source_model = [&]() {return _tree_view->source_model();}; //
// std::bind(_source_model_func, _tree_view);

// QModelIndex _index = _source_model()->index(target);    // know_root()

// if(_index.isValid()) {
////    QModelIndexList index_list;
////    index_list.append(_index);
// QList<boost::intrusive_ptr<TreeItem>> delete_items;
// delete_items << target;
// results = view_delete_permanent(_source_model, delete_items, "cut"); // ,
// _know_branch // know_root_holder::know_root()

// _actionlist[action_paste_branch]->setEnabled(true);
// _actionlist[action_paste_sub_branch]->setEnabled(true);

// assert(results.size() == 1);
// assert(results.at(0) == target->linker());
// }

// return results.at(0);
// }

// boost::intrusive_ptr<TreeItem>
// TreeScreen::branch_paste(boost::intrusive_ptr<TreeItem> item, KnowModel
// *_current_know_branch)
// {
// return branch_add(_tree_view->index_current(), item, false,
// _current_know_branch);
// }

//// Вспомогательная функция при копировании ветки в буфер
// void TreeScreen::branch_add_to_clipboard(ClipboardBranch
// *branch_clipboard_data, QStringList path, bool is_root)
// {
// boost::intrusive_ptr<TreeItem> curr_item;
// QMap<QString, QString> curr_item_fields;
// QString branch_id;
////    std::shared_ptr<RecordTable> curr_item_record_table;

//// Добавление ветки
// curr_item = _know_model_board->item(path);
// curr_item_fields = curr_item->fields_all(); // Раньше было
// getAllFieldsDirect()
// branch_id = curr_item_fields["id"];

// if(is_root)
// branch_clipboard_data->addBranch("-1", curr_item_fields);
// else
// branch_clipboard_data->addBranch(curr_item->parent_id(), curr_item_fields);

//// Добавление конечных записей
////    auto curr_item_record_table = curr_item->record_table();

// for(int i = 0; i < curr_item->count_direct(); i++) {
//// Полный образ записи (с файлами и текстом)
// boost::intrusive_ptr<TreeItem> record = curr_item->item_fat(i);

// branch_clipboard_data->addRecord(branch_id, record);
// }
// }

//// Вставка ветки из буфера на том же уровне, что и выбранная
// void TreeScreen::branch_paste(void)
// {
// qDebug() << "In paste_branch";

// branch_paste_func(std::bind(&KnowModel::lock_paste_as_sibling,
// _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));
// // branch_paste_smart(true);
// }

//// Вставка ветки из буфера в виде подветки выбранной ветки
// void TreeScreen::branch_paste_sub(void)
// {
// qDebug() << "In paste_subbranch";

// branch_paste_func(std::bind(&KnowModel::lock_paste_as_child,
// _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));
// // branch_paste_smart(false);
// }

// void TreeScreen::branch_paste_smart(bool is_branch)
// {
//// Проверяется, содержит ли буфер обмена данные нужного формата
// const QMimeData *mimeData = QApplication::clipboard()->mimeData();

// if(mimeData == nullptr)
// return;

// if(!(mimeData->hasFormat("tute/branch")))
// return;

//// Получение списка индексов QModelIndex выделенных элементов
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();

//// Если выбрано более одной ветки
// if(selectitems.size() > 1) {
// QMessageBox messageBox(this);
// messageBox.setWindowTitle(tr("Unavailable action"));
// messageBox.setText(tr("You've selected ") +
// QString::number(selectitems.size()) + tr(" items.\nPlease select single item
// for enabling paste operation."));
// messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
// messageBox.exec();
// return;
// }

//// Блокируется главное окно, чтобы при продолжительном выполнении
//// небыло возможности сделать другие действия
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);

//// Создается ссылка на буфер обмена
// QClipboard *cbuf = QApplication::clipboard();

//// Извлечение объекта из буфера обмена
// const ClipboardBranch *branch;
// branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
// branch->print();
// branch->printIdTree();

//// Получение индекса выделенной строки
// QModelIndex index = _tree_view->index_current();

// if(index.isValid()) {
//// Добавление ветки
// QString pasted_branch_id;

// if(is_branch)
// pasted_branch_id = _tree_view->source_model()->lock_paste_as_sibling(index,
// (ClipboardBranch *)branch);
// else
// pasted_branch_id = _tree_view->source_model()->lock_paste_as_child(index,
// (ClipboardBranch *)branch);

// branch_update_on_screen(index.parent());

//// Установка курсора на новую созданную ветку
// auto pasted_branch_item =
// _tree_view->source_model()->item_by_id(pasted_branch_id);
// QStringList pasted_branch_path = pasted_branch_item->path_absolute();
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->set_tree_position(_tree_view->source_model()->root_item()->id(),
// pasted_branch_path);

// _know_model_board->synchronized(false);
//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// knowtree_save();

//// Разблокируется главное окно
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
// }
// }

// void TreeScreen::item_encrypt(void)
// {
// auto _index = _tree_view->current_index();

// if(_index.isValid()) {
// auto _source_model = _tree_view->source_model();
//// Получаем указатель на текущую выбранную ветку дерева
// auto item = _source_model->item(_index);

//// Шифрация ветки и всех подветок
// item->to_encrypt();

//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// know_model_save();

//// Обновляеются на экране ветка и ее подветки
// _source_model->update_index(_index);
// }
// }

// void TreeScreen::item_decrypt(void)
// {
// auto _index = _tree_view->current_index();

// if(_index.isValid()) {
// auto _source_model = _tree_view->source_model();
//// Получаем указатель на текущую выбранную ветку дерева
// auto item = _source_model->item(_index);

//// Расшифровка ветки и всех подветок
// item->to_decrypt();

//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// know_model_save();

//// Обновляеются на экране ветка и ее подветки
// _source_model->update_index(_index);

//// Проверяется, остались ли в дереве зашифрованные данные
//// если зашифрованных данных нет, будет предложено сбросить пароль
// tree_crypt_control();
// }
// }

// void TreeScreen::view_update_selected_indexes(void)
// {
//// Получение списка выделенных Item-элементов
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();

//// Обновление на экране
// for(int i = 0; i < selectitems.size(); ++i) {
// _tree_view->update(selectitems.at(i));
// }
// }

// void TreeScreen::indexes_update_for_selected(void)
// {
//// Получение списка выделенных Item-элементов
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();
// std::set<QModelIndex> index_set;

//// Обновление на экране
// for(int i = 0; i < selectitems.size(); ++i) {
// if(index_set.find(selectitems.at(i)) ==
// index_set.end())index_set.insert(selectitems.at(i));
////        _tree_view->update(selectitems.at(i));
// }

// for(auto &i : index_set) {
// _tree_view->update(i);
// }
// }

//// Обновление на экране ветки и подветок
// void TreeScreen::branch_update_from_model(const QModelIndex &index)
// {
// auto _source_model = _tree_view->source_model();
//// Для корневой ветки дается команда чтобы модель сообщила о своем изменении
// _source_model->emit_datachanged_signal(index);

//// По модельному индексу выясняется указатель на ветку
// auto _item = _source_model->item(index);

//// Перебираются подветки
// QList<QStringList> updatePathts = _item->path_children_all();

// foreach(QStringList currentPath, updatePathts) {
// auto currentItem = _source_model->item(currentPath);

// QModelIndex currentIndex = _source_model->index(currentItem);

//// Для подветки дается команда чтобы модель сообщила о своем изменении
// _source_model->emit_datachanged_signal(currentIndex);
// }
// }

// Открытие контекстного меню в дереве разделов
void ts_t::on_custom_contextmenu_requested(const QPoint& _position)
{
	qDebug() << "In TreeScreen::on_customContextMenuRequested";

	//// Конструирование меню
	// QMenu _context_menu(this);
	// _context_menu->clear();

	auto _current_model = [&]() { return _tree_view->source_model(); };
	////select item at cursor position
	////    QPersistentModelIndex
	// QModelIndex index = _tree_view->indexAt(_position);
	// _tree_view->select_and_current(index);

	// Получение индекса выделенной ветки
	QModelIndex index = _tree_view->current_index();
	if (index.isValid()) {
		// Выясняется, зашифрована ли ветка или нет
		auto cryptFlag = _current_model()->item(index)->field<crypt_key>();
		auto current_item = _current_model()->item(index);
		assert(current_item);
		auto _parent = _current_model()->item(index)->parent();
		assert(_parent);
		// Выясняется, зашифрована ли родительская ветка
		auto parentCryptFlag = _parent ? _parent->field<crypt_key>() : crypt_value(false);
		// Если ветка не зашифрована
		// Или ветка зашифрована, но пароль успешно введен
		if (cryptFlag != crypt_value(true) ||
		    (cryptFlag == crypt_value(true) && gl_paras->crypt_key().length() > 0)) {
			//// If there is a branch, by appropriate points become active in the
			///buffer  // Если в буфере есть ветки, соответсвующие пункты становятся
			///активными
			// bool		is_branch	= false;
			// const QMimeData	*mimeData	=
			// QApplication::clipboard()->mimeData();
			// if(mimeData != nullptr)
			// if(mimeData->hasFormat("tute/branch")) is_branch = true;
			enable_up_action();
			// ----------------------------
			// Подсветка пунктов шифрования
			// ----------------------------
			// Если ветка незашифрована
			if (cryptFlag != crypt_value(true)) {
				// Шифровать можно
				// Дешифровать нельзя
				_actionlist[action_encrypt_branch]->setEnabled(true);
				_actionlist[action_decrypt_branch]->setEnabled(false);
			} else {
				// Ветка зашифрована

				// Шифровать нельзя
				_actionlist[action_encrypt_branch]->setEnabled(false);
				// Дешифровать можно только если верхнележащая ветка незашифрована
				if (parentCryptFlag != crypt_value(true))
					_actionlist[action_decrypt_branch]->setEnabled(true);
				else
					_actionlist[action_decrypt_branch]->setEnabled(false);
			}
		}
	}
	////    _context_menu->addAction(_table_screen->_pin);
	// _context_menu->addAction(_table_screen->_addnew_to_end);
	// _context_menu->addAction(_table_screen->_edit_field);
	// _context_menu->addAction(_table_screen->_delete);

	// _context_menu->addSeparator();

	// _context_menu->addAction(_table_screen->_addnew_before);
	// _context_menu->addAction(_table_screen->_addnew_after);
	////    contextMenu->addAction(parentPointer->actionMoveUp);
	////    contextMenu->addAction(parentPointer->actionMoveDn);
	////    contextMenu->addAction(parentPointer->actionSyncro);

	// _context_menu->addSeparator();

	// _context_menu->addAction(_table_screen->_cut);
	// _context_menu->addAction(_table_screen->_copy);
	// _context_menu->addAction(_table_screen->_paste);

	// _context_menu->addSeparator();

	// _context_menu->addAction(_table_screen->_sort);
	// _context_menu->addAction(_table_screen->_print);
	// _context_menu->addAction(_table_screen->_settings);

	// Включение отображения меню на экране
	// menu.exec(event->globalPos());
	_context_menu->exec(_tree_view->viewport()->mapToGlobal(_position));
}

// bool TreeScreen::is_index_localized(const QModelIndexList
// _origin_index_list)const
// {
// bool result = true;

// if(_origin_index_list.size() > 0) {
////        assert(_origin_index_list.last() == _tree_view->current_index()); //
///means duplicated item found
////preparations
// auto _index_current = _origin_index_list.last();    //
// _tree_view->current_index();    //

// auto _index_common_parent = _index_current.parent();

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// QModelIndex index = _origin_index_list.at(i);

// if(index.isValid() && index.parent() != _index_common_parent) {
// result = false; break;
// }
// }
// }

// return result;
// }

// QModelIndexList TreeScreen::index_localize(const QModelIndexList
// _origin_index_list)
// {
// QModelIndexList _selectitems;

// if(_origin_index_list.size() > 0) {
////        auto _source_model = _tree_view->source_model();
//// На время удаления блокируется главное окно
////        //    find_object<MainWindow>("mainwindow")
////        globalparameters.mainwindow()->setDisabled(true);
////        //    find_object<MainWindow>("mainwindow")
////        globalparameters.mainwindow()->blockSignals(true);

////        assert(_origin_index_list.first() == _tree_view->current_index());

// if(!_origin_index_list.contains(_tree_view->current_index())) {

////            KnowModel *(KnowView::*_source_model_func)() =
///&KnowView::source_model;
// auto _source_model = [&]() {return _tree_view->source_model();}; //
// std::bind(_source_model_func, _tree_view);

////            QModelIndex _current_index;
// boost::intrusive_ptr<TreeItem> duplicated_item = _tree_view->current_item();
// boost::intrusive_ptr<TreeItem> found;

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// auto _index = _origin_index_list.at(i);

// if(duplicated_item->id() == _source_model()->item(_index)->id()) {
////                    _current_index = _index;
// found = _source_model()->item(_index);
// break;
// }
// }

// if(found && duplicated_item && found != duplicated_item) {
// found = _source_model()->model_duplicated_merge_to_left(
// std::bind(&TreeScreen::view_delete_items, this, std::placeholders::_1,
// std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
// , found, duplicated_item
// );
// }
// }

////preparations
// auto _index_current         = _tree_view->current_index();
// //_origin_index_list.first();
// auto _index_common_parent   = _index_current.parent();
////    auto index_to_be_delete_last = _index_list.last();
////        auto _item_to_be_deleted_first =
///_source_model->item(index_to_be_delete_first);
////        auto deleted_position_first =
///_item_to_be_deleted_first->sibling_order(); // items_update_selected();
////            auto deleted_position_last =
///_source_model->item(index_to_be_delete_last)->sibling_order();
////        auto _item_parent = _item_to_be_deleted_first->parent();

////        // Список имен веток, которые нужно удалить
////        QStringList branches_name;

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// QModelIndex index = _origin_index_list.at(i);

// if(index.isValid() && index.parent() == _index_common_parent) {
////                auto it = _source_model->item(index);
////                branches_name << it->field("name");
// _selectitems.push_back(index);
// }
// }
// }

// return _selectitems;
// }

//// deprecated
// void TreeScreen::index_invoke_in_sub_branch(const QModelIndex &_index)
// {
// if(_index.isValid() //       && _tree_view->index_current() != _index
// && !_tree_view->source_model()->item(_index)->is_activated()) {
// view_step_into(_index);

// #ifdef _with_record_table
// _treemodelknow->record_to_item();
// #endif
//// globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
// }
// }

// QString TreeScreen::session_root_id()
// {
// if(_session_id == global_root_id) {
// _session_id = _tree_view->current_item()->id();
// }

// return _session_id;
// }

// void TreeScreen::session_root_id(QString root_id)
// {
// _session_id = root_id;
// }

// void treescreen::on_knowTreeView_repaint(void)
// {
//// Попытка расширить нулевую колонку с деревом, если содержимое слишком
///широкое
// knowTreeView->resizeColumnToContents(0);
// }

// void TreeScreen::cursor_to_index(QModelIndex _index)
// {
// if(_index.isValid() //       && _tree_view->index_current() != _index
// && !_tree_view->source_model()->item(_index)->is_activated()) {
//// Курсор устанавливается на нужный элемент дерева
//// В desktop-варианте на сигнал currentRowChanged() будет вызван слот
///on_knowtree_clicked()
// _tree_view->select_and_current(_index);
////        _tree_view->selectionModel()->select(_index,
///current_tree_selection_mode);
////        _tree_view->selectionModel()->setCurrentIndex(_index,
///current_tree_current_index_mode);   // ClearAndSelect

//// В мобильной версии реакции на выбор ветки нет (не обрабатывается сигнал
///смены строки в модели выбора)
//// Поэтому по ветке должен быть сделан виртуальный клик, чтобы заполнилась
///таблица конечных записей
//// Метод clicked() публичный начиная с Qt5 (мобильный интерфейс возможен
///только в Qt5)
// #if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

// if(appconfig->getInterfaceMode() == "mobile")
// emit _tree_view->clicked(_index);

// #endif
// }
// }

// QItemSelectionModel *TreeScreen::selection_model(void)
// {
// return _tree_view->selectionModel();
// }

//// Получение номера первого выделенного элемента
// int TreeScreen::item_first_selected(void)
// {
//// Получение списка выделенных Item-элементов
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();

// if(selectitems.isEmpty())
// return -1; // Если ничего не выделено
// else
// return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
// }

// void TreeScreen::resetup_model(boost::intrusive_ptr<TreeItem> _item)
// {
////    _tree_view->reset();
//// Модель подключется к виду
// _tree_view->source_model(_item);
// cursor_follow_up(_item);
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::synchronize()
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);

// if(!_know_branch->synchronized())
// result =
// know_root_holder::know_root()->synchronize(_know_branch->root_item());

// return result;
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::item(QStringList path) const
// {
// return know_root_holder::know_root()->item(path);
// }

// boost::intrusive_ptr<TreeItem> TreeScreen::item(TreeModel::delegater _del)
// const
// {
// return know_root_holder::know_root()->item(_del);
// }

void ts_t::resizeEvent(QResizeEvent* e)
{
	// _tree_view->resizeEvent(e); //
	_adjustingscrollarea->resizeEvent(e);

	QWidget::resizeEvent(e);

	// int rw = contentsRect().width();
	// int vw = recordTableController->getView()->contentsRect().width();
	// int vwe = recordTableController->getView()->width();
	// int w = width();
	// int bw = this->baseSize().width();
	// int border = this->geometry().width();

	// _treepath_button->setMaximumWidth(
	////        e->size().width() - 10 // contentsMargins().left() -
	///contentsMargins().right() //
	// contentsRect().width() - 1
	// );

	// _treepath_button->setMinimumWidth(
	////        e->size().width() - 10//contentsMargins().left() -
	///contentsMargins().right() //
	// contentsRect().width() - 1
	// );
}


void ts_t::open(bool activate_browser, bool show_tree)
{
	auto _main_window = gl_paras->main_window();
	auto h_tree_splitter = _main_window->h_tree_splitter();
	auto tree_screen = _main_window->tree_screen();
	if (tree_screen) {
		if (show_tree) {
			auto bar_width = tree_screen->sizeHint().width(); // vtab_tree->minimumSizeHint().width();	//
			// tabBar()->geometry().width();

			// auto h_record_sizes = h_record_splitter->sizes();
			auto h_tree_sizes = h_tree_splitter->sizes();


			h_tree_sizes[1] = h_tree_sizes[0] + h_tree_sizes[1] - bar_width;
			h_tree_sizes[0] = bar_width; // 0;

			if (h_tree_sizes != h_tree_splitter->sizes()) {
				h_tree_splitter->setSizes(h_tree_sizes); //
				emit h_tree_splitter->splitterMoved(h_tree_sizes[0], 1);
			}
		}
		if (activate_browser) {
			auto tree_view = tree_screen->view();
			if (tree_view) {
				auto _url = web::Browser::_defaulthome;
				tree_view->index_invoke(
				    TreeIndex::item_require_treeindex([&] { return tree_view->source_model(); },
					real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(
									  _url,
									  [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
										  return TreeIndex::url_require_item_from_tree(
										      real_target_url_,
										      std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
										      [&](boost::intrusive_ptr<const i_t> it_) -> bool {
											      return url_equal(detail::to_string(it_->field<home_key>()), detail::to_string(real_target_url_->value())) || url_equal(detail::to_string(it_->field<url_key>()), detail::to_string(real_target_url_->value()));
										      });
									  })));
			}
		}
	}
}

#ifdef USE_TREE_SCREEN_VIEW
tsv_t* ts_t::viewer() const
{
	return _viewer;
}

void ts_t::viewer(tsv_t* v)
{
	_viewer = v;
}
#endif // USE_TREE_SCREEN_VIEW

void AdjustingScrollArea::resizeEvent(QResizeEvent* e)
{
	(void)e;
	// auto w = width();
	// auto vw = viewport()->width() ;
	// auto ww = widget()->width();
	// Essential vvv
	resize(_tree_view->_tree_screen->width(), height());
	_tree_view->resize(_tree_view->_tree_screen->width(), height());
	// _tree_view->resizeEvent(e);
}

bool AdjustingScrollArea::eventFilter(QObject* obj, QEvent* ev)
{
	// This works because QScrollArea::setWidget installs an eventFilter on the
	// widget
	if (obj && obj == widget() && ev->type() == QEvent::Resize) {
		// deprecated:
		// setMinimumWidth(widget()->minimumSizeHint().width() +
		// verticalScrollBar()->width());

		// widget()->setMaximumWidth(width() + verticalScrollBar()->width());
		// widget()->repaint();

		// static_cast<RecordView *>(o)
		////        widget()
		// ->resizeEvent(static_cast<QResizeEvent *>(e));

		// auto w = width();
		// auto vw = viewport()->width() ;
		// auto ww = widget()->width();
		resize(_tree_view->_tree_screen->width(), height()); // resize(width() -
								     // viewport()->width()
								     // + widget()->width(),
								     // height());
		_tree_view->resize(_tree_view->_tree_screen->width(), height());
		// _tree_view->resizeEvent(static_cast<QResizeEvent *>(ev));  //
		// (*_interface)(static_cast<QResizeEvent *>(ev));
	}
	if (obj == widget() && ev->type() != QEvent::Resize) {
		// auto w = width();
		// auto vw = viewport()->width() ;
		// auto ww = widget()->width();
		// Essential vvv
		resize(_tree_view->_tree_screen->width(), height());
		_tree_view->resize(_tree_view->_tree_screen->width(),
		    height()); // setMinimumWidth(width() -
			       // viewport()->width() + widget()->width());
			       // // setMaximumWidth(width() -
			       // viewport()->width() + widget()->width());
	}
	return QScrollArea::eventFilter(obj, ev);
}

AdjustingScrollArea::AdjustingScrollArea(tv_t* _tree_view, QWidget* parent)
    : QScrollArea(parent)
    , _tree_view(_tree_view)
{
	// setLayoutDirection(Qt::LayoutDirection::RightToLeft);

	////    _treescreenlayout->addWidget(_adjustingscrollarea = new
	///AdjustingScrollArea);  // , 0, 1
	// _tree_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setSizePolicy(QSizePolicy::Preferred,
	    QSizePolicy::Preferred); // QSizePolicy::MinimumExpanding

	setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	setMinimumSize(_tree_view->size());
	setWidgetResizable(true);
	setStyleSheet("QScrollArea { border: 0px solid blue }");

	// _adjustingscrollarea->setWidget(_tree_view);
	// _tree_view->viewport()->installEventFilter(_adjustingscrollarea);

	_tree_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	// _adjustingscrollarea->setSizePolicy(QSizePolicy::MinimumExpanding,
	// QSizePolicy::Preferred);
	// area->setStyleSheet("QScrollArea { border: 1px solid blue }");
	this->setWidget(_tree_view);
	_tree_view->viewport()->installEventFilter(this);
}

void AdjustingScrollArea::setWidget(tv_t* view)
{
	QScrollArea::setWidget(view);

	// It so happens that QScrollArea already filters widget events,
	// but that's an implementation detail that we shouldn't rely on.
	view->viewport()->installEventFilter(this);
}


// TreeViewHelpWidget::TreeViewHelpWidget(TreeScreen *_tree_screen)
// : QWidget(_tree_screen)
// {
// QVBoxLayout *layout = new QVBoxLayout;
// _scroll_area = new QScrollArea(this);

// _tree_view = new KnowView("", _tree_screen);
// _tree_view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
// QObject::connect(_tree_view, SIGNAL(zoomChanged()), this,
// SLOT(repaintZoom()));
// _scroll_area->setWidget(_tree_view);
// _scroll_area->setWidgetResizable(true);
// _scroll_area->setAlignment(Qt::AlignLeft);
// layout->addWidget(_scroll_area);
// setLayout(layout);
// }

// void TreeViewHelpWidget::repaintZoom()
// {
// _scroll_area->viewport()->updateGeometry();
// _scroll_area->viewport()->update();
// _scroll_area->update();
// }

#ifdef USE_TREE_SCREEN_VIEW

tsv_t::~tsv_t()
{
	_tree_screen->viewer(nullptr);
}

tsv_t::tsv_t(wn_t* main_window, ts_t* tree_screen, QWidget* widget_right)
    : _main_window(main_window)
    , _tree_screen(tree_screen)
    , _widget_right(widget_right)
    , _layout(new QStackedLayout(this))
{
	auto _vtab_record = _main_window->vtab_record();
	if (widget_right->objectName() == record_screen_multi_instance_name)
		_vtab_record->addTab(
		    widget_right, QIcon(":/resource/pic/three_leaves_clover.svg"),
		    QString("Browser")); // QString("Browser ") +
					 // QString::number(vtab_record->count())
	else if (widget_right->objectName() == download_manager_singleton_name)
		_vtab_record->addTab(widget_right, QIcon(":/resource/pic/apple.svg"), QString("Download"));
	// widget_right->setParent(vtab_record);
	_tree_screen->viewer(this);
	setObjectName(tree_screen_viewer_name);
	_layout->addWidget(_tree_screen);
	_layout->setMargin(0);
	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setSpacing(0);
	setLayout(_layout);

	// _tree_screen->setParent(this);
	// _tree_screen->show();
}

int tsv_t::tree_screen(ts_t* tree)
{
	return _layout->addWidget(tree);
}

ts_t* tsv_t::tree_screen() const
{
	return dynamic_cast<ts_t*>(_layout->widget());
}

void tsv_t::widget_right(QWidget* rs)
{
	_widget_right = rs;
	auto _vtab_record = _main_window->vtab_record();
	if (_widget_right->objectName() == record_screen_multi_instance_name)
		_vtab_record->addTab(
		    _widget_right, QIcon(":/resource/pic/three_leaves_clover.svg"),
		    QString("Browser")); // QString("Browser ") +
					 // QString::number(vtab_record->count())
	else if (_widget_right->objectName() == download_manager_singleton_name)
		_vtab_record->addTab(_widget_right, QIcon(":/resource/pic/apple.svg"), QString("Download"));
}

QWidget* tsv_t::widget_right() const
{
	return _widget_right;
}

#endif // USE_TREE_SCREEN_VIEW
