#include <set>
#include <QAbstractItemView>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QString>
#include <QMap>
#include <QAction>
#include <QItemSelectionModel>
#include <QWidgetAction>


#include "main.h"


#include "models/record_table/ItemsFlat.h"
#include "models/record_table/RecordModel.h"
#include "controllers/record_table/RecordController.h"
#include "views/record_table/RecordScreen.h"
#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "models/tree/TreeItem.h"
#include "models/tree/KnowModel.h"
#include "KnowView.h"
#include "libraries/ClipboardBranch.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/Password.h"
#include "libraries/WindowSwitcher.h"
#include "views/browser/toolbarsearch.h"
#include "views/browser/entrance.h"
#include "views/record/RecordInfoFieldsEditor.h"
#include "views/browser/webview.h"
//#include "models/tree/KnowModel.h"


#include "TreeScreen.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern const char *index_xml_file_name;
extern const char *clipboard_items_root;
extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;


know_root_holder::know_root_holder(const AppConfig &_appconfig, TreeScreen *_this)
    : _know_root(new KnowModel(_this))
{
    // Загрузка данных
    // auto xml =
    _know_root->init_from_xml(_appconfig.tetra_dir() + "/" + globalparameters.main_program_file() + ".xml");
    int all_count = _know_root->count_records_all();
    assert(all_count > 0);

}

know_root_holder::~know_root_holder() {delete _know_root;}

TreeScreen::TreeScreen(QString              object_name
                       , const AppConfig    &_appconfig
                       , QMenu              *_filemenu
                       , QMenu              *_toolsmenu
                       , QWidget            *_parent
                      )
    : QWidget(_parent)
      //    , know_root_holder(_appconfig, this)
      //, ui(new Ui::MainWindow)
      //, _menubar(new QMenuBar(this))
    , _know_model_board(new KnowModel(this))
      //    , _tree_controller(new TreeController())
    , _tools_line(new QToolBar(this))
    , _menubar(new QToolBar(this))
    , _menubutton(new QPushButton("", _menubar))
    , _menuaction(new QWidgetAction(_menubutton))
    , _menus_in_button(new QMenu("buttonmenu"))
    , _context_menu(new QMenu(this))
    , _tree_view(new KnowView(knowtreeview_singleton_name, this))
    , _tools_layout(new QHBoxLayout())
      //    , _recordtree_search(new browser::ToolbarSearch(this))
      //    , _recordtree_searchlayout(new QHBoxLayout())
    , _treescreenlayout(new QVBoxLayout())
    , _appconfig(_appconfig)
      //    , _shadow_branch(new TreeModelKnow(this))
{
    _know_model_board->init_from_xml(_appconfig.tetra_dir() + "/" + index_xml_file_name);  // _know_branch->intercept(know_root_holder::know_root()->root_item());    // init_from_xml(xml);  //
    int all_count = _know_model_board->count_records_all();
    assert(all_count > 0);

    _know_model_board->synchronized(true);

    //    _selected_branch->init_from_xml(_appconfig.get_tetradir() + "/candidate.xml");
    //    _selected_branch->_root_item->field("id", get_unical_id());   // come from _root
    //    _selected_branch->_root_item->field("name", "Candidate");     // come from _root

    //    _shadow_branch->init_from_xml(_appconfig.get_tetradir() + "/default_page.xml");
    //    _shadow_branch->_root_item->field("id"
    //                                      , "0"  // get_unical_id()
    //                                     );   // ?
    //    _shadow_branch->_root_item->field("name", "Current Session");

    //    _recordtree_search->setMinimumWidth(100);
    //    _recordtree_search->sizePolicy().setHorizontalStretch(10);
    setObjectName(object_name);
    setup_actions();
    setup_ui(_filemenu, _toolsmenu);
    resetup_model(_know_model_board->root_item());

    setup_signals();
    assembly();
    assembly_context_menu();
}


TreeScreen::~TreeScreen()
{
    //appconfig.treescreensize(QSize(this->width(), this->height()));


    //    QToolBar        *_menubar; // QMenuBar *_menubar;
    //    QPushButton     *_menubutton;
    //    QWidgetAction   *_menuaction;
    //    QMenu           *_menus_in_button;

    if(_menus_in_button) {
        // delete
        _menus_in_button->deleteLater();
        _menus_in_button = nullptr;
    }

    if(_context_menu) {
        // delete
        _context_menu->deleteLater();
        _context_menu = nullptr;
    }

    //    if(_menuaction) {delete _menuaction; _menuaction = nullptr;}

    //    if(_menubutton) {delete _menubutton; _menubutton = nullptr;}

    if(_menubar) {
        // delete
        _menubar->deleteLater();
        _menubar = nullptr;
    }

    //    delete _recordtree_search;
    if(_tools_line) {
        // delete
        _tools_line->deleteLater();
        _tools_line = nullptr;
    }

    //    delete _treescreenlayout;
    //    delete _toolslayout;
    if(_tree_view) {
        // delete
        _tree_view->deleteLater();
        _tree_view = nullptr;
    }

    //    delete _shadow_branch;
    if(_know_model_board) {
        // delete
        _know_model_board->deleteLater();
        _know_model_board = nullptr;
    }

}


void TreeScreen::setup_actions(void)
{
    QAction *ac;

    //    void(TreeScreen::*_set_session_id)(bool) = &TreeScreen::session_root_id;

    ac = new QAction(tr("Set as session root"), this);
    ac->setStatusTip(tr("Set as session root"));
    ac->setIcon(QIcon(":/resource/pic/clover.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::session_root_id);
    _actionlist["set_as_session_root"] = ac;




    ac = new QAction(tr("Return to root"), this);
    ac->setStatusTip(tr("Return to root"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::cursor_follow_up_to_root);
    _actionlist["return_to_root"] = ac;


    ac = new QAction(tr("View up one level"), this);
    ac->setStatusTip(tr("View up one level"));
    //    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));

    ac->setIcon(
        //        QIcon(
        //            ":/resource/pic/up-arrow-circle.svg"    // ":/resource/pic/streamline_home.svg"
        //        )  //
        style()->standardIcon(QStyle::SP_ArrowUp, 0, this)
    );

    connect(ac, &QAction::triggered, this, &TreeScreen::cursor_follow_up_one_level);
    _actionlist["view_up_one_level"] = ac;


    // Разворачивание всех подветок
    // a->setShortcut(tr("Ctrl+X"));
    ac = new QAction(tr("Expand all sub items"), this);
    ac->setStatusTip(tr("Expand all sub items"));
    ac->setIcon(QIcon(":/resource/pic/expand_all_subbranch.svg"));
    connect(ac, &QAction::triggered, std::bind(&TreeScreen::view_expand_selected, this, true));
    _actionlist["expandAllSubbranch"] = ac;

    // Сворачивание всех подветок
    ac = new QAction(tr("Collapse all sub items"), this);
    ac->setStatusTip(tr("Collapse all sub items"));
    ac->setIcon(QIcon(":/resource/pic/collapse_all_subbranch.svg"));
    connect(ac, &QAction::triggered, std::bind(&TreeScreen::view_expand_selected, this, false));   // view_collapse_all_sub
    _actionlist["collapseAllSubbranch"] = ac;

    // Перемещение ветки вверх
    ac = new QAction(tr("Move item up"), this);
    ac->setStatusTip(tr("Move item up"));
    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(ac, &QAction::triggered, this
            , [&]()->void {item_move_up_dn_branch(&TreeItem::move_up);});   // &TreeScreen::item_move_up_branch

    _actionlist["moveUpBranch"] = ac;

    // Перемещение ветки вниз
    ac = new QAction(tr("Move item down"), this);
    ac->setStatusTip(tr("Move item down"));
    ac->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(ac, &QAction::triggered, this
            , [&]()->void {item_move_up_dn_branch(&TreeItem::move_dn);});   // &TreeScreen::item_move_dn_branch

    _actionlist["moveDnBranch"] = ac;

    // Вставка новой подветки
    ac = new QAction(tr("Insert a new sub item"), this);
    ac->setStatusTip(tr("Insert a new sub item into selected"));
    ac->setIcon(QIcon(":/resource/pic/add_subbranch.svg"));
    connect(ac, &QAction::triggered, this, [&]()->void {
        view_insert_new(TreeModel::ModelIndex([&]()->KnowModel* {return _tree_view->source_model();}, _tree_view->current_index())
        , std::bind(&TreeScreen::view_add_new, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        , [&](TreeModel::ModelIndex _modelindex, QString _id, QString _name)->boost::intrusive_ptr<TreeItem> {
            return _modelindex.current_model()()->model_add_child_new(_modelindex.current_index(), _id, _name);
        });
    });
    _actionlist["insSubbranch"] = ac;

    // Вставка новой ветки
    ac = new QAction(tr("Insert a new sibling item"), this);
    ac->setStatusTip(tr("Insert a new sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/add_branch.svg"));
    connect(ac, &QAction::triggered, this
    , [&]() {
        view_insert_new(TreeModel::ModelIndex([&]()->KnowModel* {return _tree_view->source_model();}, _tree_view->current_index())
                        , std::bind(&TreeScreen::view_add_new, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        , [&](TreeModel::ModelIndex _modelindex, QString _id, QString _name)->boost::intrusive_ptr<TreeItem> {
            auto _current_model = _modelindex.current_model();
            auto _current_index = _modelindex.current_index();
            boost::intrusive_ptr<TreeItem> result;
            auto parent = _current_model()->item(_current_index)->parent();
            assert(parent);
            auto _name_same_items = parent->items_direct(_name);
            auto _item_has_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _name_same_items) {if(i->count_direct() == 0) {result = i; break;}} return result;}();

            if(_name_same_items.size() > 0 && _item_has_no_child_first)   //
            {
                assert(_item_has_no_child_first->name() == _name);
                result = _item_has_no_child_first;
                //            children_transfer(_new_item, _current_model);
            } else {
                // Вставка новых данных в модель дерева записей
                result = _current_model()->model_add_sibling(_current_index, _id, _name);
            }

            return result;
        });
    });
    _actionlist["insBranch"] = ac;

    // Редактирование ветки
    ac = new QAction(tr("Edit item name"), this);
    ac->setStatusTip(tr("Edit name of selected item"));
    ac->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::view_edit);
    _actionlist["editBranch"] = ac;

    // Удаление ветки
    ac = new QAction(tr("Delete item"), this);
    ac->setStatusTip(tr("Delete selected item and all sub items"));
    ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(ac, &QAction::triggered, this
    , [this](bool checked = false)->void {
        Q_UNUSED(checked);
        QModelIndexList _origin_index_list = _tree_view->selectionModel()->selectedIndexes();
        QModelIndexList _selectitems = _tree_view->is_index_localized(_origin_index_list) ? _origin_index_list : _tree_view->index_localize(_origin_index_list);    // _tree_view->selectionModel()->selectedIndexes(); //
        // view_remove(_selectitems, "cut");
        view_delete_permantent([&]() {return _tree_view->source_model();}, [&]() {QList<boost::intrusive_ptr<TreeItem>> r; for(auto ix : _selectitems)r << _tree_view->source_model()->item(ix); return r;}(), "cut", false);
    }); //, &TreeScreen::del_branch

    _actionlist["delBranch"] = ac;

    void(TreeScreen::*_cut)(bool) = &TreeScreen::view_cut;
    // Удаление ветки с сохранением копии в буфер обмена
    ac = new QAction(tr("Cut item"), this);
    ac->setStatusTip(tr("Cut item including sub items for paste"));
    ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
    connect(ac, &QAction::triggered, this, _cut);
    _actionlist["cutBranch"] = ac;

    // Копирование ветки в буфер обмена
    ac = new QAction(tr("Copy / Move item"), this);
    ac->setStatusTip(tr("Copy item including sub items for move"));
    ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::view_copy);
    _actionlist["copyBranch"] = ac;

    //    void(TreeScreen::*paste)() = &TreeScreen::branch_paste;
    // Вставка ветки из буфера обмена
    ac = new QAction(tr("Paste as sibling item"), this);
    ac->setStatusTip(tr("Paste sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this
    , [&](bool checked)->void {
        Q_UNUSED(checked);
        auto _index = _tree_view->current_index().parent();

        if(!_index.isValid())
        {
            cursor_follow_up_one_level();
            _tree_view->select_as_current(_tree_view->current_index().parent());
            _index = _tree_view->current_index();
        }

        assert(_index.isValid());

        view_paste_children_from_clipboard(TreeModel::ModelIndex([&]()->KnowModel* {return _tree_view->source_model();}, _index));   // std::bind(&KnowModel::model_paste_sibling_from_clipboard, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2)

    });            //, paste

    _actionlist["pasteBranch"] = ac;


    //    QString(KnowModel::*_lock_child_paste)(const QModelIndex & _index, ClipboardBranch * subbranch) = &KnowModel::lock_child_paste;
    // Вставка ветки из буфера обмена в виде подветки
    ac = new QAction(tr("Paste as sub item"), this);
    ac->setStatusTip(tr("Paste item as sub item for selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this
    , [&](bool checked)->void {
        Q_UNUSED(checked);
        view_paste_children_from_clipboard(TreeModel::ModelIndex([&]()->KnowModel* {return _tree_view->source_model();}, _tree_view->current_index()));         // std::bind(&KnowModel::model_paste_child_from_clipboard, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2)

    });            // branch_paste_sub

    _actionlist["pasteSubbranch"] = ac;

    // Шифрование ветки
    ac = new QAction(tr("Encrypt item"), this);
    ac->setStatusTip(tr("Encrypt item and all subitem"));
    // actionEncryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::view_encrypt);
    _actionlist["encryptBranch"] = ac;

    // Расшифровка ветки (снятие пароля)
    ac = new QAction(tr("Decrypt item"), this);
    ac->setStatusTip(tr("Decrypt item and all subitem"));
    // actionDecryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::view_decrypt);
    _actionlist["decryptBranch"] = ac;

    ac = new QAction(tr("Pin / freeze browser view"), this);
    ac->setStatusTip(tr("Pin / freeze browser view"));  // _pin->setStatusTip(tr("Pin a note"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg")); // _pin->setIcon(QIcon(":/resource/pic/pin.svg"));
    connect(ac, &QAction::triggered
    , [&](bool checked = false)->void {
        Q_UNUSED(checked)
        // Обновление инфополей в области редактирования записи
        MetaEditor *metaeditor = globalparameters.meta_editor();  //MetaEditor *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);

        if(metaeditor)metaeditor->switch_pin();
    });
    _actionlist["freeze_browser_view"] = ac;





    // Открытие поиска по базе (связывание клика происходит в MainWindows)
    ac = new QAction(tr("Find in base"), this);
    ac->setStatusTip(tr("Find in base"));
    ac->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
    // connection in void MainWindow::setupSignals(void)
    _actionlist["find_in_base"] = ac;


    //    QAction             *_addnew_to_end = (new QAction(tr("Add note"), this));
    //    _addnew_to_end->setIcon(QIcon(":/resource/pic/note_add.svg"));
    //    connect(_addnew_to_end, &QAction::triggered, _tabmanager, &browser::TabWidget::addnew_to_end);
    //    insert_action_as_button<QToolButton>(_toolsline, _addnew_to_end);

    //    QAction             *_addnew_before = (new QAction(tr("Add note before"), this));
    //    QAction             *_addnew_after = (new QAction(tr("Add note after"), this));

    QAction *_edit_field = (new QAction(tr("Edit properties (name, url, tags...)"), this));
    _edit_field->setStatusTip(tr("Edit note properties (pin, name, author, url, tags...)"));
    _edit_field->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(_edit_field, &QAction::triggered, this, [&](bool checked = false) {Q_UNUSED(checked); edit_field_context(this->_tree_view->current_index());});
    _actionlist["edit_field"] = _edit_field;




    QAction *_editor_switch = (new QAction(tr("&Editor switch"), this));
    _editor_switch->setStatusTip(tr("Open/Close editor"));
    _editor_switch->setIcon(QIcon(":/resource/pic/expand_console.svg"));
    connect(_editor_switch, &QAction::triggered, []() {globalparameters.mainwindow()->editor_switch();});
    //    insert_action_as_button<QToolButton>(_toolsline, _settings);
    _actionlist["editor_switch"] = _editor_switch;


    //    QAction             *_action_syncro = (new QAction(tr("Synchronization"), this));
    //    QAction             *_action_walk_history_previous = (new QAction(tr("Previous viewing note"), this));
    //    QAction             *_action_walk_history_next = (new QAction(tr("Next viewing note"), this));
    //    QAction             *_back = (new QAction(tr("Back to item tree"), this));
    //    //    , _sort(new QAction(tr("Toggle sorting"), this))
    //    QAction             *_print = (new QAction(tr("Print table"), this));

    //    , _delete(new QAction(tr("Delete note(s)"), this))
    //    , _cut(new QAction(tr("&Cut note(s)"), this))
    //    , _copy(new QAction(tr("&Copy note(s)"), this))
    //    , _paste(new QAction(tr("&Paste note(s)"), this))
    //    , _action_move_up(new QAction(tr("&Move Up"), this))
    //    , _action_move_dn(new QAction(tr("&Move Down"), this))
    //    , _find_in_base(new QAction(tr("Find in base"), this))

}


// Действия при нажатии кнопки редактирования записи
void TreeScreen::edit_field_context(QModelIndex index_current)
{
    //    qDebug() << "RecordTableController::editFieldContext()";

    //    QModelIndex sourceIndex = proxyindex_to_sourceindex(proxyIndex);
    //    int pos = sourceIndex.row(); // Номер строки в базе

    // Создается окно ввода данных, после выхода из этой функции окно должно удалиться
    InfoFieldsEditor edit_record_dialog;

    // Выясняется ссылка на таблицу конечных данных
    auto item = _tree_view->source_model()->item(index_current);

    // Поля окна заполняются начальными значениями
    edit_record_dialog.setField("pin",       item->field("pin"));
    edit_record_dialog.setField("name",      item->field("name"));
    edit_record_dialog.setField("author",    item->field("author"));
    edit_record_dialog.setField("home",      item->field("home"));
    edit_record_dialog.setField("url",       item->field("url"));
    edit_record_dialog.setField("tags",      item->field("tags"));


    int i = edit_record_dialog.exec();

    if(i == QDialog::Rejected)
        return; // Была нажата отмена, ничего ненужно делать

    // Измененные данные записываются
    edit_field(edit_record_dialog.getField("pin"),
               edit_record_dialog.getField("name"),
               edit_record_dialog.getField("author"),
               edit_record_dialog.getField("home"),
               edit_record_dialog.getField("url"),
               edit_record_dialog.getField("tags"));
}


// Функция сохранения отредактированных полей записи в таблицу конечных записей
void TreeScreen::edit_field(QString pin
                            , QString name
                            , QString author
                            , QString home
                            , QString url
                            , QString tags
                           )
{
    qDebug() << "In edit_field()";

    // Выясняется ссылка на таблицу конечных данных
    //    auto pages = _source_model->browser_pages();

    // Переданные отредактированные поля преобразуются в вид имя-значение
    QMap<QString, QString> edit_data;
    edit_data["pin"] = pin;
    edit_data["name"] = name;
    edit_data["author"] = author;
    edit_data["home"] = home;
    edit_data["url"] = url;
    edit_data["tags"] = tags;

    // Обновление новых данных в таблице конечных записей
    auto item = _tree_view->source_model()->item(_tree_view->current_index());

    for(auto i = edit_data.begin(); i != edit_data.end(); i++) {
        item->field(i.key(), i.value());
    }


    // Обновление инфополей в области редактирования записи
    MetaEditor *meta_editor = globalparameters.meta_editor();   //find_object<MetaEditor>(meta_editor_singleton_name);
    meta_editor->pin(pin);
    meta_editor->name(name);
    meta_editor->author(author);
    meta_editor->url(url);
    meta_editor->tags(tags);

    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)
    know_model_save();
}

void TreeScreen::setup_ui(QMenu *_filemenu, QMenu *_toolsmenu)
{
    //_toolsline = new QToolBar(this);


    //    QSize tool_bar_icon_size(16, 16);
    //    toolsLine->setIconSize(tool_bar_icon_size);


    insert_action_as_button<QToolButton>(_tools_line, _actionlist["set_as_session_root"]);

    //    _recordtree_searchlayout = new QHBoxLayout();
    //    _recordtree_searchlayout->addWidget(_recordtree_search);

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["return_to_root"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["view_up_one_level"]);

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["insBranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["insSubbranch"]);

    if(appconfig.interface_mode() == "desktop") {
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["editBranch"]);
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["delBranch"]);
    }

    _tools_line->addSeparator();

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["expandAllSubbranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["collapseAllSubbranch"]);

    _tools_line->addSeparator();

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["moveUpBranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["moveDnBranch"]);

    if(appconfig.interface_mode() == "mobile") {
        _tools_line->addSeparator();
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["freeze_browser_view"]);
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["find_in_base"]); // Клик по этой кнопке связывается с действием в MainWindow
    }

    //_toolsline->addSeparator();


    //_menubar->hide();
    //QMenu *_buttonmenu = new QMenu("Menu");
    //QWidgetAction *

    //    _menuholder = new QPushButton("", _toolsline);

    //    _menuaction = new QWidgetAction(_menuholder);

    //    _menuaction->setIcon(QIcon(":/resource/pic/tableware.svg"));  // does not work

    //menuholder->hide();

    // _menubutton->setIcon(QIcon(":/resource/pic/home_blue.svg"));

    //_menubar = globalparameters.mainwindow()->menuBar();    //new QMenuBar(menubutton);
    //    _menubar->setParent(_menuaction//menubutton
    //                        );

    //    auto ac = new QAction(tr("Delete item"), this);
    //    ac->setStatusTip(tr("Delete selected item and all sub items"));
    //    ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    //    connect(ac, SIGNAL(triggered()), this, SLOT(del_branch()));

    _menus_in_button->addMenu(_filemenu);           // globalparameters.mainwindow()->filemenu()

    _menus_in_button->addMenu(_toolsmenu);           // globalparameters.mainwindow()->toolsmenu()

    //_buttonmenu->addAction(_menuaction);

    _menubutton->setMenu(_menus_in_button);    // Ownership of the menu is not transferred to the push button
    //_menuaction


    //connect(_menuaction, &QWidgetAction::triggered, _menubar, &QMenuBar::show);
    //connect(menubutton, &QPushButton::clicked, _menubar, &QMenuBar::show);

    _menuaction->setDefaultWidget(_menubutton); // The ownership is transferred to QWidgetAction!!!  //_menubar////_buttonmenu//


    _menubutton->setIcon(QIcon(":/resource/pic/tableware.svg"));    // must have
    _menubutton->setToolTip(tr("Main Menu"));
    _menubutton->setStatusTip(tr("Pull down to get the main menu"));

    //        style()->standardIcon(
    //            // QStyle::SP_TitleBarMenuButton// "Qt"
    //            // QStyle::SP_ToolBarHorizontalExtensionButton //
    //            QStyle::SP_DirHomeIcon  // QStyle::SP_ToolBarVerticalExtensionButton
    //            , nullptr //new QStyleOption(1, QStyleOption::SO_Frame)
    //            , _menubar
    //        )

    _menubutton->setFlat(true);

    // insertActionAsButton(_menubar, _menuaction);    //
    _menubar->addAction(_menuaction);


    //_toolsline->addWidget(_menubar);

    //    _knowtreeview = new KnowTreeView(this);





    //    _tree_view->setObjectName(knowtreeview_singleton_name);

    //    _tree_view->setMinimumSize(1, 1);     // 150, 250

    //    _tree_view->setWordWrap(true);

    //    // Временно сделан одинарный режим выбора пунктов
    //    // todo: Множественный режим надо выставить тогда, когда
    //    // станет ясно, как удалять несколько произвольных веток так, чтобы
    //    // в процессе удаления QModelIndex нижестоящих еще не удаленных
    //    // веток не менялся
    //    // Time to do single mode select items
    //    // todo: Multi Mode should be exposed when
    //    // it will be clear how to remove some arbitrary branches so that the
    //    // in the removal of the lower QModelIndex not yet deleted
    //    // branches has not changed
    //    // knowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //    _tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);    //SingleSelection //// MultiSelection  //



    //    _tree_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded   // ScrollBarAlwaysOn
    //                                            );

    //    // Нужно установить правила показа контекстного самодельного меню
    //    // чтобы оно могло вызываться
    //    _tree_view->setContextMenuPolicy(Qt::CustomContextMenu);

    //    // Представление не должно позволять редактировать элементы обычным путем
    //    _tree_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //    QSize size = appconfig.treescreensize();
    //    resize(size);
}

void TreeScreen::resetup_model(boost::intrusive_ptr<TreeItem> _item)
{
    _tree_view->reset();
    // Модель подключется к виду
    _tree_view->source_model(_item);
}

//void TreeScreen::setup_model(KnowModel *treemodel)
//{
//    // Создание и первичная настройка модели
//    //    _knowtreemodel = new KnowTreeModel(this);

//    // Установка заголовка
//    // QStringList headers;
//    // headers << tr("Info groups");
//    // knowTreeModel->setHeaders(headers);

//    //    // Загрузка данных
//    //    _treemodelknow->init_from_xml(_appconfig.get_tetradir() + "/" + index_xml_file_name);
//    _tree_view->reset();
//    // Модель подключется к виду
//    _tree_view->setModel(treemodel);
//}


//void TreeScreen::update_model(KnowModel *_current_know_branch)  // too heavy
//{
//    _know_branch->synchronized(false);
//    auto source_model = _tree_view->source_model();
//    assert(_current_know_branch == source_model);
//    _tree_view->reset();
//    //    _tree_view->
//    setup_model(_current_know_branch);
//}


void TreeScreen::enable_up_action() //        bool enable
{
    auto enable = _tree_view->source_model()->root_item()->id() != _know_model_board->root_item()->id();

    if(enable) {
        //        _actionlist["pasteBranch"]->setEnabled(true);
        //        _actionlist["pasteSubbranch"]->setEnabled(true);
        _actionlist["view_up_one_level"]->setEnabled(true);
        _actionlist["return_to_root"]->setEnabled(true);
    } else
        //        if(
        //        static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id() == _know_branch->root_item()->id() // know_root()->root_item() == _know_branch->root_item()
        //        || !enable)
    {
        //        _actionlist["pasteBranch"]->setEnabled(false);
        //        _actionlist["pasteSubbranch"]->setEnabled(false);
        _actionlist["view_up_one_level"]->setEnabled(false);
        _actionlist["return_to_root"]->setEnabled(false);
    }
}

void TreeScreen::assembly_context_menu()
{
    _context_menu->addAction(_actionlist["set_as_session_root"]);

    _context_menu->addAction(_actionlist["freeze_browser_view"]);
    _context_menu->addAction(_actionlist["find_in_base"]);
    _context_menu->addAction(_actionlist["edit_field"]);
    _context_menu->addAction(_actionlist["editor_switch"]);


    _context_menu->addAction(_actionlist["return_to_root"]);
    _context_menu->addAction(_actionlist["view_up_one_level"]);

    _context_menu->addAction(_actionlist["insBranch"]);
    _context_menu->addAction(_actionlist["insSubbranch"]);
    _context_menu->addAction(_actionlist["editBranch"]);
    _context_menu->addAction(_actionlist["delBranch"]);
    _context_menu->addSeparator();
    _context_menu->addAction(_actionlist["expandAllSubbranch"]);
    _context_menu->addAction(_actionlist["collapseAllSubbranch"]);
    _context_menu->addSeparator();
    _context_menu->addAction(_actionlist["moveUpBranch"]);
    _context_menu->addAction(_actionlist["moveDnBranch"]);
    _context_menu->addSeparator();
    _context_menu->addAction(_actionlist["cutBranch"]);
    _context_menu->addAction(_actionlist["copyBranch"]);
    _context_menu->addAction(_actionlist["pasteBranch"]);
    _context_menu->addAction(_actionlist["pasteSubbranch"]);
    _context_menu->addSeparator();
    _context_menu->addAction(_actionlist["encryptBranch"]);
    _context_menu->addAction(_actionlist["decryptBranch"]);

}



void TreeScreen::setup_signals(void)
{
    //    connect(_recordtree_search, &browser::ToolbarSearch::textChanged, globalparameters.getFindScreen(), &FindScreen::enableFindButton);
    //    connect(_recordtree_search, &browser::ToolbarSearch::returnPressed, globalparameters.getFindScreen(), &FindScreen::findClicked);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
    connect(_tree_view, &KnowView::customContextMenuRequested, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(_tree_view, &KnowView::tapAndHoldGestureFinished, this, &TreeScreen::on_custom_contextmenu_requested);

    //    void pressed(const QModelIndex & index);
    //    void clicked(const QModelIndex &index);
    //    void doubleClicked(const QModelIndex &index);

    //    void activated(const QModelIndex &index);
    //    void entered(const QModelIndex &index);
    connect(_tree_view, &KnowView::pressed, this, &TreeScreen::on_pressed);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if(appconfig.interface_mode() == "desktop")
        connect(_tree_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TreeScreen::on_current_row_changed);   // &TreeScreen::step_into_sub_branch


    if(appconfig.interface_mode() == "mobile")
        connect(_tree_view, &KnowView::clicked, this, &TreeScreen::cursor_step_into);

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)), actionList["editBranch"], SLOT(trigger(void)));
    if(appconfig.interface_mode() == "desktop") {
        connect(_tree_view, &KnowView::clicked, this, &TreeScreen::index_invoke);
        //                [&](const QModelIndex index) {
        //            //            _tree_view->collapse(index);
        //            //            _tree_view->expand(index);
        //            (_tree_view->isExpanded(index)) ? _tree_view->setExpanded(index, false) : _tree_view->setExpanded(index, true);
        //            auto result_item = _tree_view->source_model()->item(index);

        //            if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        //        }//&TreeScreen::candidate_from_knowtree_item)

        connect(_tree_view, &KnowView::doubleClicked, this, &TreeScreen::cursor_step_into);   // index_invoke_in_sub_branch
    }

    // Сигнал что ветка выбрана мышкой
    // connect(knowTreeView,SIGNAL(pressed(const QModelIndex &)), this,SLOT(on_knowTreeView_clicked(const QModelIndex &)));
    // connect(knowTreeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(on_knowTreeView_clicked(const QModelIndex &)));

}


void TreeScreen::assembly(void)
{
    _tools_layout->addWidget(_tools_line);
    _tools_layout->addStretch();
    _tools_layout->addWidget(_menubar);
    //    _treescreenlayout = new QVBoxLayout();
    _treescreenlayout->setObjectName("treescreen_QVBoxLayout");

    //    _treescreenlayout->addLayout(_recordtree_searchlayout);
    _treescreenlayout->addLayout(_tools_layout);
    _treescreenlayout->addWidget(_tree_view);

    setLayout(_treescreenlayout);

    // The boundaries are removed, as this facility will be used as a widget    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);
}


void TreeScreen::view_expand_selected(bool expand)
{
    std::function<void(QModelIndex index, bool mode)> expand_or_collapse_recurse = [&](QModelIndex index, bool mode) {
        _tree_view->setExpanded(index, mode);

        int i = 0;

        while((index.child(i, 0)).isValid()) {
            expand_or_collapse_recurse(index.child(i, 0), mode);
            i++;
        }
    };

    // Получение индексов выделенных строк
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), expand);
}


//void TreeScreen::view_collapse_all_sub(void)
//{
//    std::function<void(QModelIndex index, bool mode)> expand_or_collapse_recurse = [&](QModelIndex index, bool mode) {
//        _tree_view->setExpanded(index, mode);

//        int i = 0;

//        while((index.child(i, 0)).isValid()) {
//            expand_or_collapse_recurse(index.child(i, 0), mode);
//            i++;
//        }
//    };

//    // Получение индексов выделенных строк
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

//    for(int i = 0; i < selectitems.size(); ++i)
//        expand_or_collapse_recurse(selectitems.at(i), false);
//}


//void TreeScreen::expand_or_collapse_recurse(QModelIndex index, bool mode)
//{
//    _tree_view->setExpanded(index, mode);

//    int i = 0;

//    while((index.child(i, 0)).isValid()) {
//        expand_or_collapse_recurse(index.child(i, 0), mode);
//        i++;
//    }
//}


//void TreeScreen::item_move_up_branch(void)
//{
//    item_move_up_dn_branch(
//        &TreeItem::move_up  //1
//    );
//}


//void TreeScreen::item_move_dn_branch(void)
//{
//    item_move_up_dn_branch(
//        &TreeItem::move_dn  //-1
//    );
//}


void TreeScreen::item_move_up_dn_branch(int(TreeItem::*_move)()) // int direction
{
    // Если ветку нельзя перемещать
    if(!move_checkenable()) return;

    // Получение индекса выделенной строки
    QModelIndex _index = _tree_view->current_index();

    if(_index.isValid()) {
        // Ветка перемещается
        QModelIndex index_after_move;

        //        if(direction == 1)
        //            index_after_move = _tree_view->source_model()->branch_move_up(index);
        //        else
        //            index_after_move = _tree_view->source_model()->branch_move_dn(index);

        index_after_move = _tree_view->source_model()->model_move_up_dn(_index, _move);

        _know_model_board->synchronized(false);

        // Установка курсора на позицию, куда была перенесена ветка
        if(index_after_move.isValid() && index_after_move != _index) {
            _tree_view->select_as_current(index_after_move);
            //            _tree_view->selectionModel()->select(index_after_move, current_tree_selection_mode);    // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
            //            _tree_view->selectionModel()->setCurrentIndex(index_after_move, current_tree_current_index_mode);   //  | QItemSelectionModel::Current

        }

        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)
        globalparameters.tree_screen()->know_model_save();
    }
}

// limit to single selection, deprecated
bool TreeScreen::move_checkenable(void)
{
    //    // Получение списка индексов QModelIndex выделенных элементов
    //    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    //    // Если выбрано более одной ветки
    //    if(selectitems.size() > 1) {
    //        QMessageBox messageBox(this);
    //        messageBox.setWindowTitle(tr("Unavailable action"));
    //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for moving."));
    //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    //        messageBox.exec();
    //        return false;
    //    } else
    return true;
}


//boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_sibling(void)
//{
//    qDebug() << "In ins_branch()";

//    return branch_insert_smart<true>();
//}


//boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_sub(void)
//{
//    qDebug() << "In ins_subbranch()";

//    return branch_insert_smart<false>();
//}




//template<>
boost::intrusive_ptr<TreeItem> TreeScreen::view_add_new(
    TreeModel::ModelIndex _modelindex   // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , QString _name
    , const std::function<boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, QString)> &_branch_add_new_impl
)
{
    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<TreeItem> result(nullptr);
    // Получение ссылки на узел, который соответствует выделенной строке
    //    auto item = _current_model->item(_current_index);

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);

    auto current_item = _current_model()->item(_current_index);
    auto current_parent = current_item->parent();
    QList<boost::intrusive_ptr<TreeItem>> _alternative_items;

    if(current_parent) {
        _alternative_items = current_parent->items_direct(_name);
    }

    //        auto _name_same_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _items_with_same_name) {if(i->count_direct() == 0) {result = i; break;}} return result;}();
    //        //    QModelIndex setto;

    if(_alternative_items.size() > 0) {

        //        for(int i = 1; i < _items_with_same_name.size(); i++) {
        //            result = view_merge_to_left(_current_model, _items_with_same_name[0], _items_with_same_name[i]);
        //        }
        QMutableListIterator<boost::intrusive_ptr<TreeItem> > it(_alternative_items);
        result = it.next();

        while(it.hasNext()) {
            result = view_merge_to_left(_current_model, result, it.next());
        }

        //            result = _name_same_no_child_first ? _name_same_no_child_first : _items_with_same_name[0];
    } else {

        // Получение уникального идентификатора
        QString id = get_unical_id();


        // Вставка данных и установка курсора

        // Одноранговая ветка

        // Подветка

        // Вставка новых данных в модель дерева записей
        result = _branch_add_new_impl(_modelindex, id, _name);    // _current_model->lock_child_add(_current_index, id, _name);
    }

    // Установка курсора на только что созданную позицию
    //        QModelIndex
    //    setto = _current_model()->index(result);  //index_child(_current_index, item->current_count() - 1);
    _tree_view->select_as_current(result);
    //    _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
    //    _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

    // А можно было установить курсор на нужную позицию и так
    //    knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount() - 1, 0, index), QItemSelectionModel::ClearAndSelect);


    _know_model_board->synchronized(false);
    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    know_model_save();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    assert(result->name() == _name);
    //    assert(result == _current_model()->item(setto));
    return  result; // _current_model->item(setto);
}


// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
// Helper function is used when you add branches
// Eli is branch = true, the branch is added to the same level
// If is_branch = false, then added podvetka
//template<bool insert_sibling_branch>
boost::intrusive_ptr<TreeItem> TreeScreen::view_insert_new(
    TreeModel::ModelIndex _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , const std::function < boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, const std::function < boost::intrusive_ptr<TreeItem>(TreeModel::ModelIndex, QString, QString) >&) > &_branch_add_new
    , const std::function < boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, QString) > &_branch_add_new_impl
)     // bool insert_sibling_branch
{
    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<TreeItem> result(nullptr);

    //    // Получение списка индексов QModelIndex выделенных элементов
    //    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    //    //    QModelIndex _current_index;

    //    // Если выбрано более одной ветки
    //    if(selectitems.size() > 1) {
    //        //        QMessageBox messageBox(this);
    //        //        messageBox.setWindowTitle(tr("Unavailable action"));
    //        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling insert operation."));
    //        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    //        //        messageBox.exec();
    //        //        //        current_index = selectitems.first();
    //        //        return nullptr;
    //        _current_index = selectitems.last();
    //    } else {

    //        //    else if(selectitems.size() == 0) {
    //        //        current_index = last_index();
    //        //    } else {
    //        _current_index = _tree_view->current_index();
    //    }

    if(_current_index.isValid()) {
        //    }

        //    if(!_current_index.isValid()) {
        //        _tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
        //        assert(_current_index.isValid());
        //    }

        // Создается окно ввода данных
        bool ok;
        QString title = tr("Create new item"), text = tr("Item name:");

        //        if(insert_sibling_branch) {
        //            title = tr("Create new item");
        //            text = tr("Item name:");
        //        } else {
        //            title = tr("Create new sub item");
        //            text = tr("Sub item name:");
        //        }

        QString _name = QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);

        if(ok && !_name.isEmpty()) {   // return nullptr;    // Если была нажата отмена

            auto current_item = _current_model()->item(_current_index);
            auto current_parent = current_item->parent();
            QList<boost::intrusive_ptr<TreeItem> > _alternative_items;

            if(current_parent)
                _alternative_items = current_parent->items_direct(_name);

            //            auto _name_same_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _items_with_same_name) {if(i->count_direct() == 0) {result = i; break;}} return result;}();

            if(_alternative_items.size() > 0) {

                //                result = _name_same_no_child_first ? _name_same_no_child_first : _items_with_same_name[0];

                QMutableListIterator<boost::intrusive_ptr<TreeItem> > it(_alternative_items);
                result = it.next();

                while(it.hasNext()) {
                    view_merge_to_left(_current_model, result, it.next());
                }

            } else {

                // Получение индекса выделенной строки
                //    QModelIndex
                //    current_index = currentitem_index();

                //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
                //                auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);
                result = _branch_add_new(_modelindex
                                         , _name // _tree_view->source_model()->item(_current_index)->name()
                                         , _branch_add_new_impl);   // nonsense
            }

            _tree_view->select_as_current(result);
            // , insert_sibling_branch
        }
    }

    // Введенные данные добавляются
    return result;
}

//boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch)
//{
//    return add_branch(_current_index, name, insert_sibling_branch);
//}

//boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, boost::intrusive_ptr<TreeItem> it, bool insert_sibling_branch, KnowModel *_current_model)
//{

//    // Получение ссылки на узел, который соответствует выделенной строке
//    auto current_item = _current_model->item(_current_index);

//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setDisabled(true);

//    // Получение уникального идентификатора
//    QString id = get_unical_id();
//    it->field("id", id);
//    QModelIndex setto;

//    // Вставка данных и установка курсора

//    // Одноранговая ветка
//    if(insert_sibling_branch) {
//        // Вставка новых данных в модель дерева записей
//        _current_model->add_sibling_branch(_current_index, it);

//        // Установка курсора на только что созданную позицию

//        // Чтобы вычислить позицию, надо синхронно получить parent элемента,
//        // на уровне которого должен был создасться новый элемент.
//        // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
//        // Затем у объекта Item выяснить количество элементов, и установить
//        // засветку через метод index() относительно parent в виде QModelIndex
//        //        QModelIndex
//        setto = static_cast<TreeModel *>(_current_model)->index(current_item->parent()->current_count() - 1, 0, _current_index.parent());
//        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
//    } else {
//        // Подветка

//        // Вставка новых данных в модель дерева записей
//        _current_model->add_child_branch(_current_index, it);

//        // Установка курсора на только что созданную позицию
//        //        QModelIndex
//        setto = _current_model->index_child(_current_index, current_item->current_count() - 1);
//        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

//        // А можно было установить курсор на нужную позицию и так
//        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
//        //                                             QItemSelectionModel::ClearAndSelect);
//    }

//    // Сохранение дерева веток
//    //    find_object<TreeScreen>(tree_screen_singleton_name)->
//    save_knowtree();

//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setEnabled(true);
//    return  _current_model->item(setto);
//}

//boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch, std::shared_ptr<KnowModel> _current_model)
//{
//    assert(_current_model);
//    return add_branch(_current_index,  name,  insert_sibling_branch, _current_model.get());
//}

boost::intrusive_ptr<TreeItem> TreeScreen::view_paste_children_from_children(
    TreeModel::ModelIndex                       _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , boost::intrusive_ptr<TreeItem>            _blank_header
    , const TreeScreen::substitute_condition    &_substitute_condition
)
{
    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<TreeItem>  result(nullptr);

    //    std::vector<boost::intrusive_ptr<TreeItem>> results;
    if(_current_model()->index(_blank_header).isValid()
       && _current_model()->index(_blank_header) != _current_index
       && _substitute_condition(_blank_header->up_linker(), _current_model()->item(_current_index)->up_linker())
       && _blank_header->field("name") != clipboard_items_root
      ) {
        view_merge_to_left(_current_model, _current_model()->item(_current_index), _blank_header);
    }

    if(_blank_header->count_direct() > 0) {
        //    for(int i = 0; i < it->current_count(); i++) {

        //        auto current_item = it->child(i);
        //    if(_current_model->is_item_id_exists(it->id())) {
        //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

        //        auto _current_index = _tree_view->index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

        if(_current_index.isValid()) {
            // Получение ссылки на узел, который соответствует выделенной строке
            //        auto current_item = _current_model->item(_current_index);

            //        if(!_current_index.isValid()) {
            //            _tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
            //            assert(_current_index.isValid());
            //        }

            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->setDisabled(true);

            //            if(new_branch_root->id() == "") {
            //                // Получение уникального идентификатора
            //                QString id = get_unical_id();
            //                new_branch_root->field("id", id);
            //            }

            QModelIndex setto;
            int current_count = 0;
            //            //            auto begin_item = _blank_header->item_direct(0);

            //            int reserved = 0;
            //            //            _current_model->beginInsertRows(_current_index.parent()
            //            //                                            , _current_index.row()   // parent->count_direct()
            //            //                                            , _current_index.row()   // parent->count_direct()
            //            //                                           );

            //            // Вставка данных и установка курсора
            //            while(( // current_count =
            //                      _blank_header->count_direct()) > 0)
            _blank_header->traverse_direct([&](boost::intrusive_ptr<TreeItem::linker> il) { // != reserved

                //            for(int index = 0; index < _blank_root->direct_count(); index++) {
                auto candidate = il->host();    // _blank_header->item_direct(0);

                if(candidate) {
                    auto found_item = // _know_model_board
                        _current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t)->bool {return t->name() == candidate->name();});

                    // Подветка
                    if(!found_item) { // candidate->id()

                        //                    if(_blank_header->item_direct(candidate->id())) {
                        //                        auto _index = _current_model()->index(candidate);

                        //                        //                        if(_index.isValid()) {
                        //                        auto _index_source = _current_model()->index(_blank_header);

                        //                        //                            _current_model()->model_delete_item(candidate);
                        //                        if(_index_source.isValid() && _index.isValid()) {
                        //                            _current_model()->beginRemoveRows(_index.parent(), _index.row(), _index.row());

                        //                            result = _blank_header->delete_permanent(candidate->up_linker())->host();

                        //                            _current_model()->endRemoveRows();
                        //                        } else {
                        //                            result = _blank_header->delete_permanent(candidate->up_linker())->host();
                        //                        }

                        //                        assert(current_count > _blank_header->count_direct());
                        //                        //                        }
                        //                    }

                        // Вставка новых данных в модель дерева записей
                        //                    results.push_back(
                        view_paste_child(_modelindex, candidate, _substitute_condition); // [&](boost::intrusive_ptr<TreeItem> it)->bool {return it->field("name") == candidate->field("name");}
                        // _current_model->add_child_item(_current_model->root_item(), new_branch_root->child(index))
                        //                    );




                    } else {
                        view_merge_to_left(_current_model, found_item, candidate);
                        //                    reserved++;
                    }

                    //                assert(_know_model_board->is_item_exists(candidate->id()));


                    current_count++;


                    //            // Установка курсора на только что созданную позицию
                    //            //        QModelIndex
                    //            setto = _current_model->index_child(_current_index, current_item->current_count() - 1);
                    //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

                    //            // А можно было установить курсор на нужную позицию и так
                    //            // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                    //            //                                             QItemSelectionModel::ClearAndSelect);
                }
            });

            if(current_count > 0)
                result = _current_model()->item(_current_index);    // _blank_header;

            if(result   // && _blank_header->field("name") != clipboard_items_root
              ) {

                //            _current_model->endInsertRows();

                // Установка курсора на только что созданную позицию
                //        QModelIndex
                setto = _current_model()->index(
                            std::bind(_substitute_condition, std::placeholders::_1, result->up_linker()) // [&](boost::intrusive_ptr<const TreeItem> it)->bool{return it->id() == begin_item->id();}
                        );  // TreeModel::delegater(begin_item->id())

                // index_child(_current_index.parent(), _current_index.row() + 1);   // current_item->current_count() - 1

                assert(setto.isValid());
                _tree_view->select_as_current(result);  // setto
                //            _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
                //            _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //        return
                result = _current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == result->id();}); // setto

                assert(result);
                //            assert(results[0] == result);

                //    } else if(_current_model->root_item()->id() == it->id()) {



                //    }

                //    }
            }
        }
    }

    return result;
}

//void TreeScreen::view_paste_from_search(
//    TreeModel::ModelIndex _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
//    , boost::intrusive_ptr<TreeItem> _result_item
//    , std::function<bool(boost::intrusive_ptr<TreeItem>)> _substitute_condition
//)
//{
//    auto _current_model = _modelindex.current_model();
//    auto _current_index = _modelindex.current_index();

//    if(_result_item->count_direct() > 0 && _current_index.isValid()) {

//        //        if(!_result_item->child(0)->unique_page()) {
//        //            globalparameters.entrance()->new_browser(_result_item->child(0)->field("url"));
//        //        }

//        //        RecordController *_record_controller = // resultset_item->unique_page()
//        //            _result_item->child(0)->unique_page()->view()->record_controller();   // globalparameters.record_screens()->record_controller();
//        //        RecordModel *controller_source_model = _record_controller->source_model();

//        //        //        setup_model(_know_branch);

//        _actionlist["return_to_root"]->setEnabled(true);

//        //        //    assert(controller_source_model->tree_item() == _selected_branch->_root_item);

//        //        controller_source_model->reset_internal_data();  // delete source_model;   // dangerous!!!

//        //        //    std::shared_ptr<RecordTable> target_data = source_model->tree_item()->tabledata();    // resultset_item->tabledata();
//        //        //    controller->init();




//        //        //    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
//        //        //    auto dommodel = resultset_item->export_to_dom();    // source->init(startItem, QDomElement());
//        //        //    resultset_item->remove_all_children();              // dommodel has noting to do with children!

//        //        //    auto result_data = std::make_shared<RecordTable>(dommodel); //    resultset_item->tabledata(dommodel);               //    resultset_item->table_clear();    // already insert into table_init

//        //        //    //    resultset_data->init(resultset_item, dommodel);     // just for init the item bind for reault set record table

//        //        //    //    _knowtreemodel->save();

//        //        //        controller_source_model->init_source_model(resultset_item   //? should we put it to treemodelknow? not the source of browser_pages
//        //        //                                                   , _record_controller
//        //        //                                                   , _record_controller->record_screen()
//        //        //                                                   , globalparameters.mainwindow()
//        //        //                                                   , globalparameters.meta_editor()
//        //        //                                                   // , globalparameters.record_screens()
//        //        //                                                  );


//        assert(!_current_model()->index(_result_item).isValid());
//        auto it =   // view_paste_as_sibling
//            view_paste_from_children(_modelindex, _result_item, _substitute_condition); // setup_model(_result_item);   // _know_branch->intercept(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr, resultset_item)));

//        //        auto idx = _tree_view->source_model()->index(it);
//        if(it.size() > 0) {
//            _tree_view->select_and_current(it[0]);


//            //    controller_source_model->root(resultset_item); // ->record_table(result_data);

//            //    for(int i = 0; i < resultset_item->tabledata()->size(); i++) {

//            //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
//            //            _shadow_candidate_model->_root_item->tabledata()->size()
//            //            , resultset_item->tabledata()->record(i)
//            //        );
//            //    }





//            //    reset_tabledata(std::make_shared<RecordTable>(resultset_item, dommodel));

//            //    controller->init();

//            //    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
//            //    if(appconfig.getInterfaceMode() == "mobile") {

//            //        QStringList path = item->getPathAsName();

//            //        // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
//            //        int emptyStringIndex = path.indexOf("");
//            //        path.removeAt(emptyStringIndex);

//            //        find_object<RecordTableScreen>(table_screen_singleton_name)->setTreePath(path.join(" > "));

//            //    }



//            // Width of the tree column set as to always accommodates data  // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
//            _tree_view->resizeColumnToContents(0);

//            // Переключаются окна (используется для мобильного интерфейса)
//            //    globalparameters.window_switcher()->switchFromTreeToRecordtable();
//            //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
//        }
//    }
//}


void TreeScreen::view_paste_children_from_clipboard(TreeModel::ModelIndex _modelindex)  // std::function<QString(const QModelIndex &, ClipboardBranch *)>  _model_paste_from_clipboard
{

    // Добавление подветки из буфера обмена относительно указанного элемента
    // Функция возвращает новый идентификатор стартовой добавленной подветки
    auto paste_children_from_clipboard = [](boost::intrusive_ptr<TreeItem> _blank_header, ClipboardBranch * _sub_branch, int _sibling_order)->boost::intrusive_ptr<TreeItem> {
        qDebug() << "In paste_subbranch()";
        boost::intrusive_ptr<TreeItem> _result(nullptr);
        //    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == _target_item->id();}));  // maybe a dangling pointer, for further operations



        auto move_as_child_from_clipboard_object = [](boost::intrusive_ptr<TreeItem> _parent, boost::intrusive_ptr<TreeItem> _source_item, int _pos, int _mode = add_new_record_after)->boost::intrusive_ptr<TreeItem> {

            //    auto item_from_model = item([&](boost::intrusive_ptr<const TreeItem> it)->bool{return it->id() == _source_item->id();});  // clipboard implementation have no model

            //    if(item_from_model) {
            //        _source_item = item_from_model;
            //    }
            assert(_parent);
            boost::intrusive_ptr<TreeItem> result(nullptr);    // 1-1?

            //        auto _index_parent = index(_parent);

            //        auto _index_origin = index(_source_item);

            //        assert(!_index_origin.isValid());
            if(_parent && !(_source_item->parent() == _parent && _parent->contains_direct(_source_item->up_linker())))
            {
                //                //                _source_item->self_remove_from_parent();    //
                //                _source_item->parent()->remove(_source_item->up_linker());    // -> this content is not must a logic error  1-2? // inside in 1-3 _source_item->parent calling

                //                //        assert(!_index_parent.isValid());

                result = _source_item->parent(_parent, _pos, _mode)->host();    // 1-3?  // *_parent << _source_item; //


                if(_parent->field("crypt") == "1" && result) {
                    // Новая ветка превращается в зашифрованную
                    result->to_encrypt();
                }

                //    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == _parent->id();}));

                //    if(result)
                //        assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == result->id();})); // may be parent is not exist?

                //    // Подузел прикрепляется к указанному элементу
                //    // в конец списка подчиненных элементов
                //    //    boost::intrusive_ptr<TreeItem> current =
            }

            return result;  // return model_move_as_child_impl(_parent, _source, _pos, _mode);
        };

        int record_move_times = 0;

        std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>, ClipboardBranch *, const int)> // , const QString &
        child_assembly_recursive
            = [&](boost::intrusive_ptr<TreeItem>    _current_clip_parent    // , const QString &_current_clip_records_parent_id
                  , ClipboardBranch                 *_sub_branch
                  , const int                       _sibling_order
        )->boost::intrusive_ptr<TreeItem> {
            //            QString id = _current_clip_parent->id();
            boost::intrusive_ptr<TreeItem> contain_current_parent(nullptr);
            contain_current_parent = _blank_header->contains_direct(_current_clip_parent->up_linker());

            if(!contain_current_parent)
            {
                //        if(_clip_parent->parent()) {
                qDebug() << "In child_assembly_recursive()";

                // Выясняются данные конечных записей
                QList< boost::intrusive_ptr<TreeItem> > records = _sub_branch->records_by_parent_id(_current_clip_parent->id());

                for(int i = 0; i < records.size(); i++) { //foreach(boost::intrusive_ptr<TreeItem> record, records) {
                    auto record = records.at(i);


                    if(record && _current_clip_parent != record) {
                        boost::intrusive_ptr<TreeItem> contain_current_record(nullptr);
                        contain_current_record = _current_clip_parent->contains_direct(record->up_linker());

                        // _new_item
                        if(!contain_current_record) {
                            auto record_shadow = move_as_child_from_clipboard_object(_current_clip_parent, record, _sibling_order + i); // _clip_parent->count_direct()
                            qDebug() << "Add table record : " + record->field("name") + " to " + _current_clip_parent->name();
                            // assert(record->parent_id() == _current_clip_parent->id());
                            assert(record_shadow->id() == record->id());
                            assert(record->id() != _current_clip_parent->id());
                            record_move_times++;
                            // _clip_parent->child_move(_clip_parent->count_direct(), record, ADD_NEW_RECORD_TO_END);    // _new_item

                            //                assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == record_shadow->id();})); // dangling parent lead all do not exist in model

                            if(record_shadow) {    // you can not use result->count_direct() because there are no children there
                                //                    QList< boost::intrusive_ptr<TreeItem> > record_children = _sub_branch->records_by_parent_id(record_shadow->id());

                                //                    for(int i = 0; i < record_children.size(); i++) { //foreach(QString current_subbranch_id, subbranch_id_list) {

                                //                    assert(record_children[i]->parent_id() == record_shadow->id());

                                //                    assert(0 == record_shadow->count_direct());
                                child_assembly_recursive(record_shadow, _sub_branch, 0);

                                //                    }
                            }
                        }
                    }
                }
            }

            //        }

            //            assert(id != "");
            return _current_clip_parent;    // id;
        };

        //        //    // Выясняется линейная структура добавляемого дерева
        //        //    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->id_tree_get();

        //        //    // Идентификатор стартовой ветки лежит в первом элементе списка
        //        //    QString start_branch_id = tree[0]._branch_id;

        //        //    auto _clip_root_items_parent_id_list = subbranch->clip_root_items_parent_id_list();
        //        auto _clip_blank_header_id = _sub_branch->clip_blank_header_id();

        //        //        //    int offset = 0;
        //        //        if(_clip_blank_header_id.size() > 0)
        //        //        {
        //        //            size_t calling_count = 0;

        //            for(auto id_iterator = _clip_blank_header_id.begin(); id_iterator != _clip_blank_header_id.end(); id_iterator++) {
        _blank_header->field("id", _sub_branch->clip_blank_header_id());
        _result = child_assembly_recursive(_blank_header    // , _clip_blank_header_id // *id_iterator
                                           , _sub_branch
                                           , _sibling_order  // + offset
                                          );

        //        //        offset++;
        //        calling_count++;
        //        //            }

        //        assert(calling_count == _clip_blank_header_id.size());
        assert(record_move_times <= _blank_header->count_children_all());
        //        _result = _blank_header;
        //        }

        return _result;  // _target_item->id();
    };





    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<TreeItem> _result(nullptr);
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mime_data = QApplication::clipboard()->mimeData();

    if(mime_data == nullptr)
        return;

    if(!(mime_data->hasFormat("mytetra/branch")))
        return;

    //    //    // Получение списка индексов QModelIndex выделенных элементов
    //    //    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    //    QModelIndex _current_index;

    //    //    // Если выбрано более одной ветки
    //    //    if(selectitems.size() > 1) {
    //    //        //        QMessageBox messageBox(this);
    //    //        //        messageBox.setWindowTitle(tr("Unavailable action"));
    //    //        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling paste operation."));
    //    //        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    //    //        //        messageBox.exec();
    //    //        //        return;
    //    //        _current_index = selectitems.first();
    //    //    } else {
    //    _current_index = _tree_view->current_index();
    //    //    }


    if(_current_index.isValid()) {
        // Блокируется главное окно, чтобы при продолжительном выполнении
        // небыло возможности сделать другие действия
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setDisabled(true);


        // Создается ссылка на буфер обмена
        QClipboard *cbuf = QApplication::clipboard();

        // Извлечение объекта из буфера обмена
        const ClipboardBranch *_sub_branch;


        _sub_branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
        _sub_branch->print();
        _sub_branch->id_tree_print();

        //        //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
        //        auto _current_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);




        //        //        // delete items from view
        //        //        //        auto _source_model = _tree_view->source_model();
        //        //        auto _current_item = _tree_view->current_item();

        //        //        assert(_source_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _current_item->id();})); // _current_item






        //        //        //        // ?
        //        //        //        auto _clip_root_items_parent_id_list = _sub_branch->clip_root_items_parent_id_list();
        //        //        //        QModelIndexList _index_list;
        //        //        //        QList<boost::intrusive_ptr<TreeItem>> clip_board_items;

        //        //        //        for(auto _id : _clip_root_items_parent_id_list) {
        //        //        //            QList<boost::intrusive_ptr<TreeItem>>records = _sub_branch->records_by_parent_id(_id);

        //        //        //            for(auto record : records) {
        //        //        //                auto _index = _source_model()->index(record);

        //        //        //                if(_index.isValid()) {
        //        //        //                    _index_list.push_back(_index);
        //        //        //                    clip_board_items << record;
        //        //        //                }
        //        //        //            }
        //        //        //        }

        //        //        //        if(!_index_list.isEmpty()) view_delete_items(_source_model, clip_board_items, "cut");






        //        //        _tree_view->select_and_current(_current_item);

        //        //        assert(_source_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _current_item->id();}));

        QMap<QString, QString> data;
        QDateTime ctime_dt = QDateTime::currentDateTime();
        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
        //        data["id"]      = get_unical_id();
        data["name"]    = clipboard_items_root; // "Clipboard items root";
        data["ctime"]   = ctime;
        data["dir"]     = data["id"];
        data["file"]    = "text.html";

        boost::intrusive_ptr<TreeItem> blank_header(new TreeItem(nullptr, data));
        blank_header = paste_children_from_clipboard(blank_header, (ClipboardBranch *)_sub_branch, 0);  // _current_model()->
        QList<boost::intrusive_ptr<TreeItem>> buffer;
        blank_header->traverse_direct([&](boost::intrusive_ptr<TreeItem::linker> il_in_clipboard) {
            auto index = static_cast<KnowModel *>(_current_model())->index([&](boost::intrusive_ptr<const TreeItem::linker> link_in_tree) {return link_in_tree->host()->id() == il_in_clipboard->host()->id();});

            if(index.isValid()) {
                auto it_on_tree = _current_model()->item(index);

                if(index.parent().isValid()) {
                    auto parent_item = _current_model()->item(index.parent());
                    assert(parent_item->field("name") != clipboard_items_root);

                    if((static_cast<KnowModel *>(_current_model())->item([&](boost::intrusive_ptr<const TreeItem> t) {return t->id() == parent_item->id();}))     // if(it_on_tree->up_linker()->host_parent()->field("name") == clipboard_items_root)it_on_tree->parent(parent_item);
                    && !buffer.contains(it_on_tree)) { buffer << it_on_tree;}
                }
            }
        });

        if(buffer.size() > 0) {   // == blank_header->count_direct()
            blank_header->clear();

            for(auto it_on_tree : buffer) {
                view_paste_child(_modelindex, it_on_tree, [&](boost::intrusive_ptr<const TreeItem::linker> l0, boost::intrusive_ptr<const TreeItem::linker>l1) {
                    return l0->host()->field("name") == l1->host()->field("name");
                }); //it->parent(blank_header);
            }

            _result = _current_model()->item(_current_index);
        } else if(blank_header) {
            _result = view_paste_children_from_children(_modelindex, blank_header, [&](boost::intrusive_ptr<const TreeItem::linker> target, boost::intrusive_ptr<const TreeItem::linker> source)->bool {return target->host()->field("url") == source->host()->field("url") && target->host()->field("name") == source->host()->field("name");});

        }

        // Добавление ветки
        QString pasted_branch_id = _result->id();  // _model_paste_from_clipboard(_current_index, (ClipboardBranch *)_sub_branch);

        //        if(is_branch)
        //            pasted_branch_id = _tree_view->source_model()->lock_paste_as_sibling(index, (ClipboardBranch *)branch);
        //        else
        //            pasted_branch_id = _tree_view->source_model()->lock_paste_as_child(index, (ClipboardBranch *)branch);

        // update indexes
        if(_current_index.parent().isValid())_current_model()->update_index(_current_index.parent());

        _tree_view->setExpanded(_current_index, true);

        // Установка курсора на новую созданную ветку
        auto pasted_branch_item = _current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == pasted_branch_id;});
        QStringList pasted_branch_path = pasted_branch_item->path_absolute();
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->set_tree_position(_tree_view->source_model()->root_item()->id(), pasted_branch_path);

        _know_model_board->synchronized(false);
        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        know_model_save();


        //        }



        // Разблокируется главное окно
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }
}


boost::intrusive_ptr<TreeItem> TreeScreen::view_paste_sibling(
    TreeModel::ModelIndex _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , boost::intrusive_ptr<TreeItem> _source_item
    , const TreeScreen::substitute_condition &_substitute_condition
)
{
    boost::intrusive_ptr<TreeItem>  result(nullptr);
    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    auto current_item = _current_model()->item(_current_index);

    assert(current_item); // make it automatically?
    assert(current_item != _source_item);   // mayb be equal because integrity is false
    auto _current_parent    = current_item->parent();
    //    auto current_parent_    = _current_model()->item(_current_index.parent());
    assert(_current_parent);
    //    assert(_current_parent == current_parent_);

    if(_current_parent && _current_parent->id() != "") {

        bool current_parent_valid = [&]() {
            if(!_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == _current_parent->id();}) && _current_parent->id() != "") {
                intercept(_current_parent->id());
            }
            return (_current_model()->index(_current_parent).isValid())
            && (_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == _current_parent->id();}));
        }();



        //    for(int i = 0; i < it->current_count(); i++) {

        //        auto current_item = it->child(i);
        //    if(_current_model->is_item_id_exists(it->id())) {
        //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

        //    auto _current_index = _tree_view->index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

        if(_current_index.isValid()) {
            // Получение ссылки на узел, который соответствует выделенной строке
            //    auto current_item = _current_model->item(_current_index);

            //    if(!_current_index.isValid()) {
            //        _tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
            //        assert(_current_index.isValid());
            //    }

            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->setDisabled(true);

            if(_source_item->id() == "") {
                // Получение уникального идентификатора
                QString id = get_unical_id();
                _source_item->field("id", id);
            }

            //        QModelIndex setto;

            // Вставка данных и установка курсора

            // Одноранговая ветка
            //        if(insert_sibling_branch) {

            //        auto current_parent = _current_model()->item(_current_index)->parent();
            //        assert(current_parent);

            QList<boost::intrusive_ptr<TreeItem>> _alternative_items;
            //            auto check = _current_model->item(_current_index);
            //            assert(_current_model->item_by_id(check->id()));
            //            assert(_current_model->item_by_name(check->name()));
            //            assert(_know_model_board->item_by_name(check->name()));
            //            assert(_know_model_board->item_by_id(check->id()));

            if(current_parent_valid) {
                _alternative_items = _current_parent->items_direct(std::bind(_substitute_condition, std::placeholders::_1, _source_item->up_linker()));

                //        auto _alternative_item_has_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _alternative_items) {if(i->count_direct() == 0) {result = i; break;}} return result;}();

                if(_alternative_items.size() > 0) {  // && same->is_empty()
                    //            //            if(_alternative_item_has_no_child_first) {
                    //            //                assert(_alternative_item_has_no_child_first->name() == _source_item->name());
                    //            //                result = view_merge_to_left(_current_model, _alternative_item_has_no_child_first, _source_item);
                    //            //            } else
                    //            for(int i = 1; i < _alternative_items.size(); i++) {
                    //                result = view_merge_to_left(_current_model, _alternative_items[0], _alternative_items[i]);
                    //            }

                    QMutableListIterator<boost::intrusive_ptr<TreeItem> > it(_alternative_items);
                    result = it.next();

                    while(it.hasNext()) {
                        result = view_merge_to_left(_current_model, result, it.next());
                        assert(result);
                        assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));
                    }

                    assert(result);

                    if(result) {
                        assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));
                    }

                    if(result != _source_item) {
                        result = view_merge_to_left(_current_model, result, _source_item);
                        assert(result);
                        assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));

                    }

                    _tree_view->select_as_current(result);
                    //            view_paste_from_children_move(_modelindex, _source_item, [&](boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source)->bool {return target->field("url") == source->field("url") && target->field("name") == source->field("name");});  // , _substitute_condition

                    assert(result != _know_model_board->root_item());
                } else {
                    // Вставка новых данных в модель дерева записей
                    result = static_cast<KnowModel *>(_current_model())->model_move_as_sibling(_current_index, _source_item);
                    assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));
                    assert(result != _know_model_board->root_item());
                }


                assert(result);
                // Установка курсора на только что созданную позицию

                // Чтобы вычислить позицию, надо синхронно получить parent элемента,
                // на уровне которого должен был создасться новый элемент.
                // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
                // Затем у объекта Item выяснить количество элементов, и установить
                // засветку через метод index() относительно parent в виде QModelIndex
                //        QModelIndex
                assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));

                //        setto = _current_model()->index(result);    //(current_item->parent()->current_count() - 1, 0, _current_index.parent());
                //        assert(setto.isValid());
                _tree_view->select_as_current(result);

                //        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
                //        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect
                //        }


                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();


                //        return

                //        result = _current_model()->item(setto);
                _tree_view->select_as_current(result);
                assert(
                    (_source_item == result) || (_source_item->name() == result->name())
                );
                //    } else if(_current_model->root_item()->id() == it->id()) {

                //        assert(new_branch_root == result);  // not must, if you already have it

                //    }

                //    }
            }
        } else {

        }

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }

    return result;
}

boost::intrusive_ptr<TreeItem> TreeScreen::view_paste_child(
    TreeModel::ModelIndex _modelindex   // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , boost::intrusive_ptr<TreeItem> _source_item
    , const TreeScreen::substitute_condition &_substitute_condition
)
{
    boost::intrusive_ptr<TreeItem> result(nullptr);
    auto _current_model = _modelindex.current_model();
    auto _current_index = _modelindex.current_index();
    assert(_current_model()->item(_current_index));


    //    for(int i = 0; i < it->current_count(); i++) {

    //        auto current_item = it->child(i);
    //    if(_current_model->is_item_id_exists(it->id())) {
    //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

    //    auto _current_index = _tree_view->index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

    if(_current_index.isValid() && _source_item) {

        boost::intrusive_ptr<TreeItem> current_item = _current_model()->item(_current_index);
        assert(!current_item->contains_direct(_source_item->up_linker()));
        assert(!current_item->contains_direct(std::forward<const boost::intrusive_ptr<TreeItem>>(_source_item)));

        if(!current_item->contains_direct(_source_item->up_linker())) {
            auto last_child = current_item->item_direct(current_item->count_direct() - 1);
            assert(last_child != _source_item);    // may be equal because integrity is false

            if(current_item->count_direct() > 0) {
                //                assert(current_item->item_direct(0) != _source_item);
                result = view_paste_sibling(TreeModel::ModelIndex(_current_model, _current_model()->index(last_child)) //_current_model, _current_model()->index(current_item->item_direct(0))
                                            , _source_item, _substitute_condition);
            } else {

                // Получение ссылки на узел, который соответствует выделенной строке
                //    auto current_item = _current_model->item(_current_index);

                //    if(!_current_index.isValid()) {
                //        _tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
                //        assert(_current_index.isValid());
                //    }

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setDisabled(true);

                if(_source_item->id() == "") {
                    // Получение уникального идентификатора
                    QString id = get_unical_id();
                    _source_item->field("id", id);
                }

                //            QModelIndex setto;

                // Вставка данных и установка курсора

                // Одноранговая ветка

                // Подветка

                // Вставка новых данных в модель дерева записей
                result = static_cast<KnowModel *>(_current_model())->model_move_as_child(_current_index, _source_item);
                assert(result);
                assert(result != _know_model_board->root_item());
                // Установка курсора на только что созданную позицию
                //        QModelIndex
                assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));
                //            setto = _current_model()->index(result);  //index_child(_current_index, current_item->current_count() - 1);
                //            assert(setto.isValid());
                _tree_view->select_as_current(result);   // setto
                //        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
                //        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

                // А можно было установить курсор на нужную позицию и так
                // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                //                                             QItemSelectionModel::ClearAndSelect);


                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //        return
                result = _current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();});
                assert((_source_item == result) || (_source_item->name() == result->name()));
                //    } else if(_current_model->root_item()->id() == it->id()) {

                //        assert(new_branch_root == result);  // not must, if you already have it

                //    }

                //    }
            }
        } else {
            result = _source_item;
        }

        assert(result);
    }

    return result;
}




////template<>
//boost::intrusive_ptr<TreeItem> TreeScreen::branch_add(QModelIndex _current_index, QString name, KnowModel *_current_model)
//{
//    boost::intrusive_ptr<TreeItem> result(nullptr);
//    // Получение ссылки на узел, который соответствует выделенной строке
//    //    auto item = _current_model->item(_current_index);

//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setDisabled(true);

//    // Получение уникального идентификатора
//    QString id = get_unical_id();
//    QModelIndex setto;

//    // Вставка данных и установка курсора

//    // Одноранговая ветка
//    //    if(insert_sibling_branch) {

//    auto parent = _tree_view->source_model()->item(_current_index)->parent();
//    assert(parent);
//    auto same = parent->child(parent->is_name_exists(name));


//    if(same) {  // && same->is_empty()
//        assert(same->name() == name);
//        result = same;
//        //            children_transfer(_new_item, _current_model);
//    } else {
//        // Вставка новых данных в модель дерева записей
//        result = _current_model->lock_sibling_add(_current_index, id, name);
//    }

//    // Установка курсора на только что созданную позицию

//    // Чтобы вычислить позицию, надо синхронно получить parent элемента,
//    // на уровне которого должен был создасться новый элемент.
//    // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
//    // Затем у объекта Item выяснить количество элементов, и установить
//    // засветку через метод index() относительно parent в виде QModelIndex
//    //        QModelIndex
//    setto = _current_model->index(result);  //(item->parent()->current_count() - 1, 0, _current_index.parent());
//    _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

//    //    }



//    _know_model_board->synchronized(false);
//    // Сохранение дерева веток
//    //    find_object<TreeScreen>(tree_screen_singleton_name)->
//    knowtree_save();

//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setEnabled(true);
//    assert(result->name() == name);
//    assert(result == _current_model->item(setto));
//    return  result; // _current_model->item(setto);
//}



QList<boost::intrusive_ptr<TreeItem::linker>> TreeScreen::view_delete_permantent(
                                               const std::function<KnowModel *()>       &_current_model
                                               , QList<boost::intrusive_ptr<TreeItem>>  _items
                                               , const QString                          &_mode
                                               , const bool                             _cut_branch_confirm
                                           )
{
    QList<boost::intrusive_ptr<TreeItem>> items_candidate;
    QList<boost::intrusive_ptr<TreeItem::linker>> result_linkers;

    //    QModelIndexList _index_list = is_index_localized(_origin_index_list) ? _origin_index_list : index_localize(_origin_index_list);

    if(_items.size() > 0) {

        std::function<bool(QList<boost::intrusive_ptr<TreeItem>>)>
        has_encrypt = [&](QList<boost::intrusive_ptr<TreeItem>> _result) {
            bool disableFlag = false;

            // Перебираются удаляемые ветки
            for(auto it : _result) { //for(int i = 0; i < _index_list.size(); ++i) {
                // QModelIndex index = _index_list.at(i);
                // auto it = _current_model()->item(index);

                // Если у ветки установлен флаг шифрования
                if(it->field("crypt") == "1") {
                    disableFlag = true;
                    break;
                }

                // Проверяется наличие флага шифрования у всех подветок
                QList<QStringList> cryptFlagsList = it->path_children_all_as_field("crypt");

                foreach(QStringList cryptFlags, cryptFlagsList) {
                    if(cryptFlags.contains("1")) {
                        disableFlag = true;
                        break;
                    }
                }

                if(disableFlag)break;

            } // Закрылся цикл перебора всех выделенных для удаления веток


            // Если в какой-то ветке обнаружено шифрование
            if(disableFlag) {
                QMessageBox messageBox(this);
                messageBox.setWindowTitle(tr("Unavailable action"));
                messageBox.setText(tr("In your selected data found closed item. Action canceled."));
                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
                messageBox.exec();

                // Разблокируется главное окно
                //            find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //            find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->blockSignals(false);

                //            return _result;
            }

            return disableFlag;
        };

        //        auto _source_model = _tree_view->source_model();

        // На время удаления блокируется главное окно
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setDisabled(true);
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->blockSignals(true);

        //preparations

        std::sort(_items.begin(), _items.end(), [&](boost::intrusive_ptr<TreeItem> t0, boost::intrusive_ptr<TreeItem> t1) {return _current_model()->index(t0).row() < _current_model()->index(t1).row();});

        auto index_to_be_delete_first = _current_model()->index(_items.first());
        auto _index_common_parent = index_to_be_delete_first.parent();

        //    auto index_to_be_delete_last = _index_list.last();
        auto _item_to_be_deleted_first = _items.first();
        auto _item_common_parent = _item_to_be_deleted_first->parent(); // may be a clipboard root item : blank header
        assert(_item_common_parent);

        if(_item_common_parent->field("name") != clipboard_items_root) {
            auto deleted_position_first = _item_common_parent->sibling_order([&](boost::intrusive_ptr<const TreeItem::linker> it) {return it->host()->id() == _item_to_be_deleted_first->id();}); // items_update_selected();
            //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();



            // Список имен веток, которые нужно удалить
            QStringList branches_name;

            for(auto it : _items) {
                // QModelIndex index = _index_list.at(i);

                if(it->parent() == _item_common_parent) { //if(index.isValid() && index.parent() == _index_common_parent) {

                    if(it->id() == _current_model()->session_id()) {
                        //        keep = source; remove = target;
                        _current_model()->session_id(_item_common_parent->id());

                    }

                    // auto it = _current_model()->item(index);
                    branches_name << it->field("name");
                    //                _index_list.push_back(index);
                    items_candidate << it;
                }
            }

            // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
            if(!((globalparameters.crypt_key().size() == 0) && has_encrypt(items_candidate))) {
                // Создается окно с вопросом, нужно удалять ветки или нет
                QString title, text, del_button;
                bool enable_question = true;

                if(_mode == "delete") {
                    title = tr("Delete item(s)");
                    text = tr("Are you sure you wish to delete item(s) <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
                    del_button = tr("Delete");

                    enable_question = true;
                } else if(_mode == "cut") {
                    title = tr("Cut item");
                    text = tr("Are you sure you wish to cut item <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
                    del_button = tr("Cut");

                    if(appconfig.cut_branch_confirm() && _cut_branch_confirm) enable_question = true;
                    else enable_question = false;
                }

                bool enable_del = true;

                if(enable_question) {
                    QMessageBox message_box(this);
                    message_box.setWindowTitle(title);
                    message_box.setText(text);
                    message_box.addButton(tr("Cancel"), QMessageBox::RejectRole);
                    QAbstractButton *delete_button = message_box.addButton(del_button, QMessageBox::AcceptRole);
                    message_box.exec();

                    if(message_box.clickedButton() == delete_button) {
                        enable_del = true;
                    } else {
                        enable_del = false;
                    }
                }


                // Если удаление подтверждено
                if(enable_del) {
                    // Сохраняется текст в окне редактирования
                    // Нужно, чтобы нормально удалилась текущая редактируемая запись,
                    // если она находится в удаляемой ветке
                    //        find_object<MainWindow>("mainwindow")
                    globalparameters.mainwindow()->save_text_area();

                    // Сортировка списка индексов по вложенности методом пузырька
                    // Индексы с длинным путем перемещаются в начало списка
                    for(int i = 0; i < items_candidate.size(); i++) {
                        for(int j = items_candidate.size() - 1; j > i; j--) {
                            QStringList path_1 = items_candidate.at(j - 1)->path_absolute();//(_current_model()->item(  // ))
                            QStringList path_2 = items_candidate.at(j)->path_absolute();    //(_current_model()->item(  // ))

                            if(path_1.size() < path_2.size()) items_candidate.swap(j - 1, j);
                        }
                    }

                    qDebug() << "Path for delete";

                    for(int i = 0; i < items_candidate.size(); ++i)
                        qDebug() << items_candidate.at(i)->path_absolute(); //(_current_model()->item(  //))

                    // auto _source_model = _tree_view->source_model();
                    //_source_model->beginRemoveRows(_index_common_parent, _index_list.first().row(), _index_list.last().row());

                    // Вызов удаления веток
                    for(auto it : items_candidate) { // for(int i = 0; i < _result.size(); ++i) {
                        if(it->id() == static_cast<KnowModel *>(_current_model())->session_id()) {
                            static_cast<KnowModel *>(_current_model())->session_id(it->parent()->id());
                        }

                        auto result = static_cast<KnowModel *>(_current_model())->model_delete_permantent(it->up_linker());

                        // just for counting, 0 is fine
                        //                    if(result) { // if(!result)
                        result_linkers.append(result); // model_delete_one( // just for counting, 0 is fine
                        //                    }

                        // _current_model()->model_delete_item(it)

                    }

                    if(result_linkers.size() > 0) {
                        // _source_model->endRemoveRows();
                        //                _source_model = _tree_view->source_model();
                        _current_model()->update_index(_index_common_parent);


                        QModelIndex setto;
                        boost::intrusive_ptr<TreeItem> left_sibling_item(nullptr);


                        if(_item_common_parent && _item_common_parent->field("name") != clipboard_items_root) {
                            int new_count = _item_common_parent->count_direct();


                            // Одноранговая ветка
                            if(new_count > 0) {
                                int new_position = 0;

                                if((deleted_position_first - 1) >= 0) {
                                    new_position = deleted_position_first - 1;
                                } else {
                                    new_position = deleted_position_first;
                                }

                                //                    else
                                //                        // if(deleted_position_last + 1 < new_count)
                                //                    {
                                //                        new_position = 0;   //deleted_position_last + 1;
                                //                    }

                                //                    int new_pos = (deleted_position_first > new_count - 1) ? new_count - 1 : ((deleted_position_first - 1) >= 0) ? deleted_position_first : 0;

                                setto = _current_model()->index(new_position, 0, index_to_be_delete_first.parent());
                                left_sibling_item = _item_common_parent->item_direct(new_position);
                                _tree_view->select_as_current(left_sibling_item);    // setto


                                //                        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);   // current_tree_selection_mode
                                //                        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // current_tree_selection_mode    // ClearAndSelect

                            } else {

                                //            //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
                                //            setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
                                //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

                                while(_item_common_parent && _item_common_parent != _know_model_board->root_item()) {

                                    if(_item_common_parent != _current_model()->root_item() && _current_model()->index(_item_common_parent).isValid()) {

                                        left_sibling_item = _item_common_parent;
                                        setto = _current_model()->index(left_sibling_item);

                                        //                                _tree_view->select_and_current(setto);

                                        //                                _tree_view->selectionModel()->select(_source_model->index(_item_parent), current_tree_selection_mode);
                                        //                                _tree_view->selectionModel()->setCurrentIndex(_source_model->index(_item_parent), current_tree_current_index_mode);   // ClearAndSelect

                                        break;
                                    } else { // if(_parent != _know_model_board->root_item())
                                        cursor_follow_up_one_level();
                                        _item_common_parent = _item_common_parent->parent();

                                        if(_item_common_parent) {
                                            left_sibling_item = _item_common_parent;
                                            setto = _current_model()->index(left_sibling_item);

                                        } else {
                                            tree_empty_controll();
                                            left_sibling_item = _tree_view->source_model()->root_item()->item_direct(0);
                                            setto = _current_model()->index(left_sibling_item);
                                        }
                                    }
                                }

                                assert(left_sibling_item);
                                setto = _current_model()->index(left_sibling_item);
                                assert(setto.isValid());

                                _tree_view->select_as_current(left_sibling_item);  // setto

                                //            if(index.parent().parent().isValid()) {view_up_one_level();}
                                //            else {view_return_to_root();}
                            }

                        }


                        qDebug() << "Delete finish";

                        //        update_model(_current_know_branch);

                        // Сохранение дерева веток
                        //        find_object<TreeScreen>(tree_screen_singleton_name)->
                        know_model_save();
                        //        reload_knowtree();
                        qDebug() << "Save new tree finish";

                        //            _actionlist["pasteBranch"]->setEnabled(false);
                        //            _actionlist["pasteSubbranch"]->setEnabled(false);
                    }
                } else {
                    _actionlist["pasteBranch"]->setEnabled(true);
                    _actionlist["pasteSubbranch"]->setEnabled(true);
                }

                // Разблокируется главное окно
                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->blockSignals(false);

                tree_empty_controll();
                tree_crypt_control();

            }
        } // Закрылось условие что системный пароль не установлен
    }

    return result_linkers;
}



//QList<boost::intrusive_ptr<TreeItem>> TreeScreen::view_delete_indexes(
//                                       std::function<KnowModel *()> _current_model
//                                       , QModelIndexList            _origin_index_list
//                                       , QString                    _mode
//                                       , bool                       _cut_branch_confirm
//                                   )
//{
//    QList<boost::intrusive_ptr<TreeItem>> _result;

//    QModelIndexList _index_list = is_index_localized(_origin_index_list) ? _origin_index_list : index_localize(_origin_index_list);

//    if(_index_list.size() > 0) {

//        //        auto _source_model = _tree_view->source_model();

//        // На время удаления блокируется главное окно
//        //    find_object<MainWindow>("mainwindow")
//        globalparameters.mainwindow()->setDisabled(true);
//        //    find_object<MainWindow>("mainwindow")
//        globalparameters.mainwindow()->blockSignals(true);

//        //preparations
//        auto index_to_be_delete_first = _index_list.first();
//        auto _index_common_parent = index_to_be_delete_first.parent();
//        //    auto index_to_be_delete_last = _index_list.last();
//        auto _item_to_be_deleted_first = _current_model()->item(index_to_be_delete_first);
//        auto deleted_position_first = _item_to_be_deleted_first->sibling_order(); // items_update_selected();
//        //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();
//        auto _item_parent = _item_to_be_deleted_first->parent();


//        // Список имен веток, которые нужно удалить
//        QStringList branches_name;

//        for(int i = 0; i < _index_list.size(); ++i) {
//            QModelIndex index = _index_list.at(i);

//            if(index.isValid() && index.parent() == _index_common_parent) {
//                auto it = _current_model()->item(index);
//                branches_name << it->field("name");
//                //                _index_list.push_back(index);
//            }
//        }

//        std::function<bool()> has_encrypt = [&]() {
//            bool disableFlag = false;

//            // Перебираются удаляемые ветки
//            for(int i = 0; i < _index_list.size(); ++i) {
//                QModelIndex index = _index_list.at(i);
//                auto it = _current_model()->item(index);

//                // Если у ветки установлен флаг шифрования
//                if(it->field("crypt") == "1") {
//                    disableFlag = true;
//                    break;
//                }

//                // Проверяется наличие флага шифрования у всех подветок
//                QList<QStringList> cryptFlagsList = it->path_children_all_as_field("crypt");

//                foreach(QStringList cryptFlags, cryptFlagsList) {
//                    if(cryptFlags.contains("1")) {
//                        disableFlag = true;
//                        break;
//                    }
//                }

//                if(disableFlag)break;

//            } // Закрылся цикл перебора всех выделенных для удаления веток


//            // Если в какой-то ветке обнаружено шифрование
//            if(disableFlag) {
//                QMessageBox messageBox(this);
//                messageBox.setWindowTitle(tr("Unavailable action"));
//                messageBox.setText(tr("In your selected data found closed item. Action canceled."));
//                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
//                messageBox.exec();

//                // Разблокируется главное окно
//                //            find_object<MainWindow>("mainwindow")
//                globalparameters.mainwindow()->setEnabled(true);
//                //            find_object<MainWindow>("mainwindow")
//                globalparameters.mainwindow()->blockSignals(false);

//                //            return _result;
//            }

//            return disableFlag;
//        };

//        // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
//        if(!((globalparameters.crypt_key().size() == 0) && has_encrypt())) {
//            // Создается окно с вопросом, нужно удалять ветки или нет
//            QString title, text, del_button;
//            bool enable_question = true;

//            if(_mode == "delete") {
//                title = tr("Delete item(s)");
//                text = tr("Are you sure you wish to delete item(s) <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
//                del_button = tr("Delete");

//                enable_question = true;
//            } else if(_mode == "cut") {
//                title = tr("Cut item");
//                text = tr("Are you sure you wish to cut item <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
//                del_button = tr("Cut");

//                if(appconfig.cut_branch_confirm() && _cut_branch_confirm) enable_question = true;
//                else enable_question = false;
//            }

//            bool enable_del = true;

//            if(enable_question) {
//                QMessageBox message_box(this);
//                message_box.setWindowTitle(title);
//                message_box.setText(text);
//                message_box.addButton(tr("Cancel"), QMessageBox::RejectRole);
//                QAbstractButton *delete_button = message_box.addButton(del_button, QMessageBox::AcceptRole);
//                message_box.exec();

//                if(message_box.clickedButton() == delete_button) {
//                    enable_del = true;
//                } else {
//                    enable_del = false;
//                }
//            }


//            // Если удаление подтверждено
//            if(enable_del) {
//                // Сохраняется текст в окне редактирования
//                // Нужно, чтобы нормально удалилась текущая редактируемая запись,
//                // если она находится в удаляемой ветке
//                //        find_object<MainWindow>("mainwindow")
//                globalparameters.mainwindow()->save_text_area();

//                // Сортировка списка индексов по вложенности методом пузырька
//                // Индексы с длинным путем перемещаются в начало списка
//                for(int i = 0; i < _index_list.size(); i++) {
//                    for(int j = _index_list.size() - 1; j > i; j--) {
//                        QStringList path_1 = (_current_model()->item(_index_list.at(j - 1)))->path_absolute();
//                        QStringList path_2 = (_current_model()->item(_index_list.at(j)))->path_absolute();

//                        if(path_1.size() < path_2.size())
//                            _index_list.swap(j - 1, j);
//                    }
//                }

//                qDebug() << "Path for delete";

//                for(int i = 0; i < _index_list.size(); ++i)
//                    qDebug() << (_current_model()->item(_index_list.at(i)))->path_absolute();

//                // auto _source_model = _tree_view->source_model();
//                //_source_model->beginRemoveRows(_index_common_parent, _index_list.first().row(), _index_list.last().row());

//                // Вызов удаления веток
//                for(int i = 0; i < _index_list.size(); ++i) {
//                    _result.append( // model_delete_one(
//                        _current_model()->model_delete_index(_index_list.at(i))
//                    );
//                }

//                // _source_model->endRemoveRows();
//                //                _source_model = _tree_view->source_model();
//                _current_model()->index_update(_index_common_parent);


//                QModelIndex setto;



//                if(_item_parent) {
//                    int new_count = _item_parent->count_direct();


//                    // Одноранговая ветка
//                    if(new_count > 0) {
//                        int new_position = 0;

//                        if((deleted_position_first - 1) >= 0) {
//                            new_position = deleted_position_first - 1;
//                        }

//                        //                    else
//                        //                        // if(deleted_position_last + 1 < new_count)
//                        //                    {
//                        //                        new_position = 0;   //deleted_position_last + 1;
//                        //                    }

//                        //                    int new_pos = (deleted_position_first > new_count - 1) ? new_count - 1 : ((deleted_position_first - 1) >= 0) ? deleted_position_first : 0;

//                        setto = _current_model()->index(new_position, 0, index_to_be_delete_first.parent());
//                        _tree_view->select_and_current(setto);
//                        //                        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);   // current_tree_selection_mode
//                        //                        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // current_tree_selection_mode    // ClearAndSelect

//                    } else {

//                        //            //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
//                        //            setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
//                        //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);


//                        while(_item_parent != _know_model_board->root_item()) {

//                            if(_item_parent != _current_model()->root_item() && _current_model()->index(_item_parent).isValid()) {
//                                _tree_view->select_and_current(setto);
//                                //                                _tree_view->selectionModel()->select(_source_model->index(_item_parent), current_tree_selection_mode);
//                                //                                _tree_view->selectionModel()->setCurrentIndex(_source_model->index(_item_parent), current_tree_current_index_mode);   // ClearAndSelect

//                                break;
//                            } else { // if(_parent != _know_model_board->root_item())
//                                _item_parent = _item_parent->parent();
//                                cursor_follow_up_one_level();
//                            }
//                        }

//                        //            if(index.parent().parent().isValid()) {view_up_one_level();}
//                        //            else {view_return_to_root();}
//                    }

//                }


//                qDebug() << "Delete finish";

//                //        update_model(_current_know_branch);

//                // Сохранение дерева веток
//                //        find_object<TreeScreen>(tree_screen_singleton_name)->
//                knowtree_save();
//                //        reload_knowtree();
//                qDebug() << "Save new tree finish";

//                //            _actionlist["pasteBranch"]->setEnabled(false);
//                //            _actionlist["pasteSubbranch"]->setEnabled(false);
//            } else {
//                _actionlist["pasteBranch"]->setEnabled(true);
//                _actionlist["pasteSubbranch"]->setEnabled(true);
//            }

//            // Разблокируется главное окно
//            //    find_object<MainWindow>("mainwindow")
//            globalparameters.mainwindow()->setEnabled(true);
//            //    find_object<MainWindow>("mainwindow")
//            globalparameters.mainwindow()->blockSignals(false);

//            tree_empty_controll();
//            tree_crypt_control();

//        } // Закрылось условие что системный пароль не установлен
//    }

//    return _result;
//}



//// Удаление выбранных веток, вызывается при выборе соотвествущей
//// кнопки или пункта меню
//// Delete the selected branches, called when the corresponding
//// button or menu item
//QList<boost::intrusive_ptr<TreeItem::linker>> TreeScreen::view_remove(const QModelIndexList &_index_list, const QString &mode, const bool _cut_branch_confirm)
//{
//    qDebug() << "In del_branch()";

//    // На время удаления блокируется главное окно
//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setDisabled(true);
//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->blockSignals(true);

//    assert(_tree_view->is_index_localized(_index_list));

//    //    // Получение списка индексов QModelIndex выделенных элементов
//    //    QModelIndexList _origin_index_list = _tree_view->selectionModel()->selectedIndexes();
//    //    QModelIndexList _index_list = is_index_localized(_origin_index_list) ? _origin_index_list : index_localize(_origin_index_list);

//    //    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
//    auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);
//    QList<boost::intrusive_ptr<TreeItem>> delete_items;

//    if(_index_list.size() > 0) {
//        for(auto _index : _index_list) {
//            delete_items << _source_model()->item(_index);
//        }

//        //        auto _source_model = _tree_view->source_model();
//        _source_model()->synchronized(false);
//    }

//    return view_remove(_source_model, delete_items, mode, _cut_branch_confirm);

//}



//// Удаление одной ветки и её подветок
//// Delete one branch and its podvetok / garter
//boost::intrusive_ptr<TreeItem> TreeScreen::model_delete_one(std::function<KnowModel *()> _current_model, QModelIndex _index_delete)
//{

//    boost::intrusive_ptr<TreeItem> _item_deleted(nullptr);

//    std::function<void(boost::intrusive_ptr<TreeItem>)> close_tab_recursive = [&](boost::intrusive_ptr<TreeItem> it) {
//        if(it->is_registered_to_browser())   // item_to_be_deleted->unique_page()
//            it->bounded_page()->record_controller()->page_remove(it->id()); // (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

//        if(it->count_direct() > 0) {
//            for(int i = 0; i < it->count_direct(); i++) {
//                close_tab_recursive(it->item_direct(i));
//            }
//        }
//    };

//    if(_index_delete.isValid()) {

//        qDebug() << "Label 1";

//        // Получение узла, который соответствует обрабатываемому индексу
//        _item_deleted = _current_model()->item(_index_delete);











//        // try to rewrite, but below is just memory operation which is not we want, but but, the original code is also a memory ooperation too? why?
//        if( // _index_delete.row() > 0 &&
//            _current_model()->root_item()->count_direct() > 0) {  // (index.row() > 0) == index.isValid()

//            auto _parent = _item_deleted->parent();

//            if(_parent) _parent->child_remove(_item_deleted);

//            while(_parent->self_empty_remove()) {
//                _parent = _parent->parent();

//                if(!_parent)break;
//            }

//        }




//        //        int deleted_position = _item_to_be_deleted->sibling_order(); //item_to_be_deleted->parent()->sibling_order(item_to_be_deleted);

//        qDebug() << "Label 2, branch id:" << _item_deleted->field("id") << "name:" << _item_deleted->field("name");

//        // Получение пути к элементу
//        QStringList _delete_path = _item_deleted->path_absolute();

//        qDebug() << "Label 3";

//        // Получение путей ко всем подветкам
//        QList<QStringList> sub_branches_path = _item_deleted->path_children_all();

//        qDebug() << "Label 4";

//        // Сортировка массива веток по длине пути
//        qSort(sub_branches_path.begin(), sub_branches_path.end()
//        , [](const QStringList & list1, const QStringList & list2) {return list1.size() < list2.size();}    // compare_qstringlist_length
//             );

//        qDebug() << "Label 5";

//        // Удаление всех таблиц конечных записей для нужных подветок
//        // Удаление всех подчиненных элементов для нужных подветок
//        // Вначале пробегаются самые длинные ветки а потом более короткие
//        for(int i = sub_branches_path.size() - 1; i >= 0; i--) {
//            qDebug() << "Delete subbranch, id:" << _current_model()->item(sub_branches_path.at(i))->field("id") << "name:" << _current_model()->item(sub_branches_path.at(i))->field("name");

//            (_current_model()->item(sub_branches_path.at(i)))->children_clear();
//            // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
//        }

//        qDebug() << "Label 6";

//        // Удаление таблицы конечных записей для самой удаляемой ветки
//        // Удаление подчиненных элементов для самой удаляемой ветки
//        qDebug() << "Delete rootbranch, id:" << _current_model()->item(_delete_path)->field("id") << "name:" << _current_model()->item(_delete_path)->field("name");

//        (_current_model()->item(_delete_path))->children_clear();
//        // ( knowTreeModel->getItem(path) )->removeAllChildren();

//        qDebug() << "Label 7";

//        // Удаление ветки на экране, при этом удалятся все подветки
//        qDebug() << "This branch have row() as" << _index_delete.row();

//        if(_index_delete.isValid()) qDebug() << "Index valid";
//        else qDebug() << "Index non valid";










//        //        _source_model->beginRemoveRows(_index_to_be_delete.parent(), _index_to_be_delete.row(), _index_to_be_delete.row());
//        _current_model()->removeRows(_index_delete.row(), 1, _index_delete.parent());
//        //        _source_model->endRemoveRows();

//        //        items_update_selected();

//        //    // Удаление всех нужных подветок
//        //    // Пробегаются самые длинные ветки а потом более короткие
//        //    for(int i = subbranchespath.size() - 1; i >= 0; i--)
//        //        if(knowTreeModel->isItemValid(subbranchespath.at(i))) {
//        //            TreeItem *current_item = knowTreeModel->getItem(subbranchespath.at(i));
//        //            delete current_item;
//        //        }

//        //    if(!reocrd_controller) {
//        //        globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
//        //    }

//        //        if(item_to_be_deleted->is_registered_to_browser())   // item_to_be_deleted->unique_page()
//        //            item_to_be_deleted->unique_page()->record_controller()->remove_child(item_to_be_deleted->id()); // (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

//        close_tab_recursive(_item_deleted);

//        qDebug() << "Label 8";

//        //        // items_update_selected();
//        //        QModelIndex setto;
//        //        auto _parent = _item_to_be_deleted->parent();

//        //        if(_parent) {
//        //            int new_count = _parent->count_direct();

//        //            // Одноранговая ветка
//        //            if(new_count > 0) {
//        //                int new_pos = (deleted_position > new_count - 1) ? new_count - 1 : ((deleted_position - 1) >= 0) ? deleted_position : 0;

//        //                setto = _current_model->index(new_pos, 0, index.parent());
//        //                _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
//        //                _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect

//        //            } else {

//        //                //            //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
//        //                //            setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
//        //                //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);


//        //                while(_parent != _know_model_board->root_item()) {

//        //                    if(_parent != _current_model->root_item() && _current_model->index(_parent).isValid()) {
//        //                        _tree_view->selectionModel()->select(_current_model->index(_parent), current_tree_selection_mode);
//        //                        _tree_view->selectionModel()->setCurrentIndex(_current_model->index(_parent), current_tree_selection_mode);   // ClearAndSelect

//        //                        break;
//        //                    } else { // if(_parent != _know_model_board->root_item())
//        //                        _parent = _parent->parent();
//        //                        cursor_up_one_level();
//        //                    }
//        //                }

//        //                //            if(index.parent().parent().isValid()) {view_up_one_level();}
//        //                //            else {view_return_to_root();}
//        //            }

//        //        }

//        //        assert(item_to_be_deleted == _current_know_branch->item(setto));
//    }

//    return _item_deleted;
//}

boost::intrusive_ptr<TreeItem::linker> TreeScreen::view_cut(boost::intrusive_ptr<TreeItem> target)
{
    QList<boost::intrusive_ptr<TreeItem::linker>> results;
    //    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
    auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);

    QModelIndex _index = _source_model()->index(target);    // know_root()

    if(_index.isValid()) {
        //    QModelIndexList index_list;
        //    index_list.append(_index);
        QList<boost::intrusive_ptr<TreeItem>> delete_items;
        delete_items << target;
        results = view_delete_permantent(_source_model, delete_items, "cut"); // , _know_branch // know_root_holder::know_root()

        _actionlist["pasteBranch"]->setEnabled(true);
        _actionlist["pasteSubbranch"]->setEnabled(true);

        assert(results.size() == 1);
        assert(results.at(0) == target->up_linker());
    }

    return results.at(0);
}

void TreeScreen::view_cut(bool _cut_branch_confirm)
{
    //    bool copy_result = false;

    //    copy_result
    auto index_list = view_copy();

    if(index_list.size() > 0) {// copy_result
        view_delete_permantent([&]() {return _tree_view->source_model();}, [&]() {
            QList<boost::intrusive_ptr<TreeItem>> r;

            for(auto ix : index_list)r << _tree_view->source_model()->item(ix);

            return r;
        }(), "cut", _cut_branch_confirm);
        // view_remove(index_list, "cut", _cut_branch_confirm); //this->_know_branch,
    }

    _actionlist["pasteBranch"]->setEnabled(true);
    _actionlist["pasteSubbranch"]->setEnabled(true);

}

//boost::intrusive_ptr<TreeItem> TreeScreen::branch_paste(boost::intrusive_ptr<TreeItem> item, KnowModel *_current_know_branch)
//{
//    return branch_add(_tree_view->index_current(), item, false, _current_know_branch);
//}



QModelIndexList TreeScreen::view_copy(void)    // const
{

    bool copy_result = false;

    //    // Вспомогательная функция при копировании ветки в буфер
    //    std::function<void(ClipboardBranch *, QStringList)> //  , bool    //void TreeScreen::
    //    branch_add_to_clipboard
    //    = [&](ClipboardBranch * branch_clipboard_data, QStringList path) {  // , bool is_root

    //        // boost::intrusive_ptr<TreeItem> curr_item;
    //        // QMap<QString, QString> curr_item_fields;
    //        // QString branch_id;
    //        //    std::shared_ptr<RecordTable> curr_item_record_table;

    //        // Добавление ветки
    //        boost::intrusive_ptr<TreeItem> curr_item = _know_model_board->item(path);
    //        QMap<QString, QString> curr_item_fields = curr_item->fields_all(); // Раньше было getAllFieldsDirect()
    //        QString branch_id = curr_item_fields["id"];

    //        //        if(is_root)
    //        //            branch_clipboard_data->branch_add(curr_item->parent_id(), curr_item_fields);
    //        //        else
    //        branch_clipboard_data->branch_add(curr_item->parent_id(), curr_item_fields);
    //        branch_clipboard_data->record_add(curr_item->parent_id(), curr_item);
    //        // Добавление конечных записей
    //        //    auto curr_item_record_table = curr_item->record_table();

    //        for(int i = 0; i < curr_item->count_direct(); i++) {
    //            // Полный образ записи (с файлами и текстом)
    //            boost::intrusive_ptr<TreeItem> record = curr_item->item_fat(i);
    //            branch_clipboard_data->branch_add(branch_id, record->fields_all());
    //            branch_clipboard_data->record_add(branch_id, record);
    //        }
    //    };

    qDebug() << "In branch_copy()";

    // Сохраняется текст в окне редактирования
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList _origin_index_list = _tree_view->selectionModel()->selectedIndexes();
    QModelIndexList _selectitems = // _tree_view->selectionModel()->selectedIndexes(); //
        _tree_view->is_index_localized(_origin_index_list) ? _origin_index_list : _tree_view->index_localize(_origin_index_list);

    QModelIndex _index_current;
    bool indexs_valid = true;
    QList<boost::intrusive_ptr<TreeItem>> items;
    auto _source_model = [&]() {return _tree_view->source_model();};

    // Если выбрано более одной ветки
    if(_selectitems.size() >= 1) {
        //        QMessageBox messageBox(this);
        //        messageBox.setWindowTitle(tr("Unavailable action"));
        //        messageBox.setText(tr("Please select a single item for copy."));
        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //        messageBox.exec();
        //        return copy_result;
        for(auto i : _selectitems) {
            if(!i.isValid()) {
                indexs_valid = false; break;
            } else {
                auto it = static_cast<KnowModel *>(_source_model())->item(i);

                if(!items.contains(it)) items << it;
            }
        }

        if(indexs_valid)
            _index_current = static_cast<KnowModel *>(_source_model())->index(items[0]); // _selectitems[0];
    }


    // Получение индекса выделенной ветки
    //    _index_current = _tree_view->index_current();

    if(indexs_valid) { // _index_current.isValid()


        // -------------------
        // Копирование в буфер
        // -------------------

        qDebug() << "Branch copy to buffer";

        // Создается ссылка на буфер обмена
        QClipboard *cbuf = QApplication::clipboard();

        // Создается объект с данными для заполнения буфера обмена
        // Если в буфере есть какие-то старые данные, они удаляются
        static int fillflag = 0;

        if(fillflag == 1) {
            const ClipboardBranch *branch_clipboard_data_previos;
            branch_clipboard_data_previos = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());

            if(branch_clipboard_data_previos != nullptr)delete branch_clipboard_data_previos;

            fillflag = 0;
        }

        ClipboardBranch *_sub_branch = new ClipboardBranch();
        fillflag = 1;

        //        QString _clipboard_root_parent_id = get_unical_id();

        //        for(auto item : items) {


        //            //            // Получение ссылки на узел, который соответствует выделенной ветке
        //            //            auto item = _source_model->item(_index_current);

        //            //            //        // Получение пути к выделенной ветке
        //            //            //        QStringList _current_item_absolute_path = item->path_absolute();

        //            //            //        // Получение путей ко всем подветкам
        //            //            //        QList<QStringList> sub_branches_absolute_path = item->path_children_all();


        //            // Проверка, содержит ли данная ветка как шифрованные
        //            // так и незашифрованные данные
        //            bool nocrypt_presence = false;
        //            bool encrypt_presence = false;

        //            // Флаги на основе состояния текущей ветки
        //            if(_know_model_board->item(_index_current)->field("crypt") == "1")   //_current_item_absolute_path

        //                encrypt_presence = true;
        //            else
        //                nocrypt_presence = true;

        //            // Флаги на основе состояния подветок
        //            for(int i = 0; i < item->count_direct(); i++) { //foreach(QStringList curr_absolute_path, sub_branches_absolute_path)
        //                if( //_know_model_board->item(curr_absolute_path)
        //                    item->item_direct(i)->field("crypt") == "1") {
        //                    encrypt_presence = true;
        //                } else {
        //                    nocrypt_presence = true;
        //                }
        //            }

        //            // Если ветка содержит как шифрованные так и нешифрованные данные
        //            // то такую ветку копировать в буфер нельзя
        //            if(nocrypt_presence == true && encrypt_presence == true) {
        //                //                QMessageBox messageBox(this);
        //                //                messageBox.setWindowTitle(tr("Unavailable action"));
        //                //                messageBox.setText(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."));
        //                //                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //                //                messageBox.exec();
        //                globalparameters.status_bar()->showMessage(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."), 2000);
        //                break;  // return copy_result;
        //            }


        //            //            // -------------------
        //            //            // Копирование в буфер
        //            //            // -------------------

        //            //            qDebug() << "Branch copy to buffer";

        //            //            //            // Создается ссылка на буфер обмена
        //            //            //            QClipboard *cbuf = QApplication::clipboard();

        //            //            // Создается объект с данными для заполнения буфера обмена
        //            //            // Если в буфере есть какие-то старые данные, они удаляются
        //            //            static int fillflag = 0;

        //            //            if(fillflag == 1) {
        //            //                const ClipboardBranch *branch_clipboard_data_previos;
        //            //                branch_clipboard_data_previos = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());

        //            //                if(branch_clipboard_data_previos != nullptr)delete branch_clipboard_data_previos;

        //            //                fillflag = 0;
        //            //            }

        //            //            ClipboardBranch *branch_clipboard_data = new ClipboardBranch();

        //            //            fillflag = 1;



        //        item->field("id", _clipboard_root_parent_id);
        // Добавление корневой ветки
        _sub_branch->branch_push(TreeModel::ModelIndex(_source_model, _tree_view->current_index()), items); // , _clipboard_root_parent_id
        // _current_item_absolute_path
        // branch_add_to_clipboard(branch_clipboard_data, _source_model_cur_item_absolute_path);   // , true

        //        // Добавление прочих веток
        //        foreach(QStringList curr_path_absolute, sub_branches_absolute_path) {
        //            branch_clipboard_data->branch_copy(curr_path_absolute);   // branch_add_to_clipboard(branch_clipboard_data, curr_path_absolute); // , false
        //        }

        auto records = _sub_branch->records_by_parent_id(_sub_branch->clip_blank_header_id());
        assert(records.size() == items.size());

        _sub_branch->print();

        //            // Объект с ветками помещается в буфер обмена
        //            cbuf->setMimeData(branch_clipboard_data);

        _actionlist["pasteBranch"]->setEnabled(true);
        _actionlist["pasteSubbranch"]->setEnabled(true);


        //        }

        // Объект с ветками помещается в буфер обмена
        if(_sub_branch->clip_blank_header_id() != "") {cbuf->setMimeData(_sub_branch); copy_result = true;}
        else {delete _sub_branch;}
    } else {
        _actionlist["pasteBranch"]->setEnabled(false);
        _actionlist["pasteSubbranch"]->setEnabled(false);
        //        copy_result = false;
    }

    return _selectitems;    // copy_result;
}


void TreeScreen::view_edit(void)
{
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        //        QMessageBox messageBox(this);
        //        messageBox.setWindowTitle(tr("Unavailable action"));
        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling edit operation."));
        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //        messageBox.exec();
        //        return;
        _current_index = selectitems.first();
    } else {

        // Получение индекса выделенной строки
        _current_index = _tree_view->current_index();
    }

    if(_current_index.isValid()) {
        // Получение ссылки на узел, который соответствует выделенной строке
        auto item = _tree_view->source_model()->item(_current_index);

        // Если ветка зашифрована и пароль не был введен
        if(item->field("crypt") == "1" &&
           globalparameters.crypt_key().length() == 0)
            return;

        // Получение имени ветки
        QString name = item->field("name");

        // Создается окно ввода данных
        bool ok;
        QString newname = QInputDialog::getText(this,
                                                tr("Edit item name"),
                                                tr("Item name:"),
                                                QLineEdit::Normal,
                                                name,
                                                &ok);

        // Если была нажата отмена
        if(!(ok && !newname.isEmpty())) return;

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setDisabled(true);

        item->field("name", newname);

        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        know_model_save();

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }
}


//// Вспомогательная функция при копировании ветки в буфер
//void TreeScreen::branch_add_to_clipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root)
//{
//    boost::intrusive_ptr<TreeItem> curr_item;
//    QMap<QString, QString> curr_item_fields;
//    QString branch_id;
//    //    std::shared_ptr<RecordTable> curr_item_record_table;

//    // Добавление ветки
//    curr_item = _know_model_board->item(path);
//    curr_item_fields = curr_item->fields_all(); // Раньше было getAllFieldsDirect()
//    branch_id = curr_item_fields["id"];

//    if(is_root)
//        branch_clipboard_data->addBranch("-1", curr_item_fields);
//    else
//        branch_clipboard_data->addBranch(curr_item->parent_id(), curr_item_fields);

//    // Добавление конечных записей
//    //    auto curr_item_record_table = curr_item->record_table();

//    for(int i = 0; i < curr_item->count_direct(); i++) {
//        // Полный образ записи (с файлами и текстом)
//        boost::intrusive_ptr<TreeItem> record = curr_item->item_fat(i);

//        branch_clipboard_data->addRecord(branch_id, record);
//    }
//}


//// Вставка ветки из буфера на том же уровне, что и выбранная
//void TreeScreen::branch_paste(void)
//{
//    qDebug() << "In paste_branch";

//    branch_paste_func(std::bind(&KnowModel::lock_paste_as_sibling, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));  // branch_paste_smart(true);
//}


//// Вставка ветки из буфера в виде подветки выбранной ветки
//void TreeScreen::branch_paste_sub(void)
//{
//    qDebug() << "In paste_subbranch";

//    branch_paste_func(std::bind(&KnowModel::lock_paste_as_child, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));  // branch_paste_smart(false);
//}




//void TreeScreen::branch_paste_smart(bool is_branch)
//{
//    // Проверяется, содержит ли буфер обмена данные нужного формата
//    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

//    if(mimeData == nullptr)
//        return;

//    if(!(mimeData->hasFormat("mytetra/branch")))
//        return;

//    // Получение списка индексов QModelIndex выделенных элементов
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

//    // Если выбрано более одной ветки
//    if(selectitems.size() > 1) {
//        QMessageBox messageBox(this);
//        messageBox.setWindowTitle(tr("Unavailable action"));
//        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling paste operation."));
//        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
//        messageBox.exec();
//        return;
//    }

//    // Блокируется главное окно, чтобы при продолжительном выполнении
//    // небыло возможности сделать другие действия
//    //    find_object<MainWindow>("mainwindow")
//    globalparameters.mainwindow()->setDisabled(true);


//    // Создается ссылка на буфер обмена
//    QClipboard *cbuf = QApplication::clipboard();

//    // Извлечение объекта из буфера обмена
//    const ClipboardBranch *branch;
//    branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
//    branch->print();
//    branch->printIdTree();


//    // Получение индекса выделенной строки
//    QModelIndex index = _tree_view->index_current();

//    if(index.isValid()) {
//        // Добавление ветки
//        QString pasted_branch_id;

//        if(is_branch)
//            pasted_branch_id = _tree_view->source_model()->lock_paste_as_sibling(index, (ClipboardBranch *)branch);
//        else
//            pasted_branch_id = _tree_view->source_model()->lock_paste_as_child(index, (ClipboardBranch *)branch);

//        branch_update_on_screen(index.parent());

//        // Установка курсора на новую созданную ветку
//        auto pasted_branch_item = _tree_view->source_model()->item_by_id(pasted_branch_id);
//        QStringList pasted_branch_path = pasted_branch_item->path_absolute();
//        //    find_object<MainWindow>("mainwindow")
//        globalparameters.mainwindow()->set_tree_position(_tree_view->source_model()->root_item()->id(), pasted_branch_path);

//        _know_model_board->synchronized(false);
//        // Сохранение дерева веток
//        //    find_object<TreeScreen>(tree_screen_singleton_name)->
//        knowtree_save();

//        // Разблокируется главное окно
//        //    find_object<MainWindow>("mainwindow")
//        globalparameters.mainwindow()->setEnabled(true);
//    }
//}


boost::intrusive_ptr<TreeItem> TreeScreen::view_merge_to_left(const std::function<KnowModel *()> &_current_model, boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source)
{
    boost::intrusive_ptr<TreeItem> result(nullptr);
    //    boost::intrusive_ptr<TreeItem> keep;
    //    boost::intrusive_ptr<TreeItem> remove;

    //    if(target->id() == _session_id) {keep = target; remove = source;}
    // else
    auto target_on_tree = _current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == target->id();});

    if(target != source
       && target_on_tree
      ) {
        if(source->id() == static_cast<KnowModel *>(_current_model())->session_id()) {
            //        keep = source; remove = target;
            if(target->parent())
                static_cast<KnowModel *>(_current_model())->session_id(target->parent()->id());
            else
                static_cast<KnowModel *>(_current_model())->session_id(target->id());
        }

        //    else {keep = target; remove = source;}

        //    //    //    KnowModel *(TreeScreen::*_know_model_board)() = &TreeScreen::know_model_board;
        //    //    //    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
        //    //    auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);

        globalparameters.mainwindow()->setDisabled(true);
        result = static_cast<KnowModel *>(_current_model())->model_merge_to_left(
                     std::bind(&TreeScreen::view_delete_permantent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
                     , target   // keep
                     , source   // remove   // std::bind(_know_model_board, this)
                 );
        assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));
        know_model_save();

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    } else if(target_on_tree) {
        result = target_on_tree;
    }

    return result;
}

// Шифрование ветки
void TreeScreen::view_encrypt(void)
{
    auto item_encrypt = [&](void)->void {
        auto _index = _tree_view->current_index();

        if(_index.isValid())
        {
            auto _source_model = _tree_view->source_model();
            // Получаем указатель на текущую выбранную ветку дерева
            auto item = _source_model->item(_index);

            // Шифрация ветки и всех подветок
            item->to_encrypt();

            // Сохранение дерева веток
            //    find_object<TreeScreen>(tree_screen_singleton_name)->
            know_model_save();

            // Обновляеются на экране ветка и ее подветки
            _source_model->update_index(_index);
        }
    };


    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            item_encrypt(); // Ветка шифруется
    } else {
        // Иначе считается, что шифрующий ключ уже был задан и он правильный

        item_encrypt(); // Ветка шифруется
    }
}



// Расшифровка ветки
void TreeScreen::view_decrypt(void)
{

    auto item_decrypt = [&](void)->void {
        auto _index = _tree_view->current_index();

        if(_index.isValid())
        {
            auto _source_model = _tree_view->source_model();
            // Получаем указатель на текущую выбранную ветку дерева
            auto item = _source_model->item(_index);

            // Расшифровка ветки и всех подветок
            item->to_decrypt();

            // Сохранение дерева веток
            //    find_object<TreeScreen>(tree_screen_singleton_name)->
            know_model_save();

            // Обновляеются на экране ветка и ее подветки
            _source_model->update_index(_index);

            // Проверяется, остались ли в дереве зашифрованные данные
            // если зашифрованных данных нет, будет предложено сбросить пароль
            tree_crypt_control();
        }
    };


    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            item_decrypt(); // Ветка расшифровывается
    } else {
        // Иначе пароль в данной сессии вводился и он правильный

        item_decrypt(); // Ветка расшифровывается
    }
}


//void TreeScreen::item_encrypt(void)
//{
//    auto _index = _tree_view->current_index();

//    if(_index.isValid()) {
//        auto _source_model = _tree_view->source_model();
//        // Получаем указатель на текущую выбранную ветку дерева
//        auto item = _source_model->item(_index);

//        // Шифрация ветки и всех подветок
//        item->to_encrypt();

//        // Сохранение дерева веток
//        //    find_object<TreeScreen>(tree_screen_singleton_name)->
//        know_model_save();

//        // Обновляеются на экране ветка и ее подветки
//        _source_model->update_index(_index);
//    }
//}


//void TreeScreen::item_decrypt(void)
//{
//    auto _index = _tree_view->current_index();

//    if(_index.isValid()) {
//        auto _source_model = _tree_view->source_model();
//        // Получаем указатель на текущую выбранную ветку дерева
//        auto item = _source_model->item(_index);

//        // Расшифровка ветки и всех подветок
//        item->to_decrypt();

//        // Сохранение дерева веток
//        //    find_object<TreeScreen>(tree_screen_singleton_name)->
//        know_model_save();

//        // Обновляеются на экране ветка и ее подветки
//        _source_model->update_index(_index);

//        // Проверяется, остались ли в дереве зашифрованные данные
//        // если зашифрованных данных нет, будет предложено сбросить пароль
//        tree_crypt_control();
//    }
//}



//void TreeScreen::view_update_selected_indexes(void)
//{
//    // Получение списка выделенных Item-элементов
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

//    // Обновление на экране
//    for(int i = 0; i < selectitems.size(); ++i) {
//        _tree_view->update(selectitems.at(i));
//    }
//}

//void TreeScreen::indexes_update_for_selected(void)
//{
//    // Получение списка выделенных Item-элементов
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
//    std::set<QModelIndex> index_set;

//    // Обновление на экране
//    for(int i = 0; i < selectitems.size(); ++i) {
//        if(index_set.find(selectitems.at(i)) == index_set.end())index_set.insert(selectitems.at(i));
//        //        _tree_view->update(selectitems.at(i));
//    }

//    for(auto &i : index_set) {
//        _tree_view->update(i);
//    }
//}


//// Обновление на экране ветки и подветок
//void TreeScreen::branch_update_from_model(const QModelIndex &index)
//{
//    auto _source_model = _tree_view->source_model();
//    // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
//    _source_model->emit_datachanged_signal(index);

//    // По модельному индексу выясняется указатель на ветку
//    auto _item = _source_model->item(index);

//    // Перебираются подветки
//    QList<QStringList> updatePathts = _item->path_children_all();

//    foreach(QStringList currentPath, updatePathts) {
//        auto currentItem = _source_model->item(currentPath);

//        QModelIndex currentIndex = _source_model->index(currentItem);

//        // Для подветки дается команда чтобы модель сообщила о своем изменении
//        _source_model->emit_datachanged_signal(currentIndex);
//    }
//}




void TreeScreen::on_pressed(const QModelIndex &_index)
{
    if(_index.isValid()) {
        _tree_view->select_as_current(_index);
        //        _tree_view->selectionModel()->select(_index, current_tree_selection_mode);    //, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
        //        _tree_view->selectionModel()->setCurrentIndex(_index, current_tree_current_index_mode);    //

        // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current


    }
}

void TreeScreen::on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)
}

void TreeScreen::on_current_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    //    _tree_view->selectionModel()->select(previous, current_tree_selection_mode);
    if(_tree_view->currentIndex() != current)_tree_view->select_as_current(current);
}

void TreeScreen::on_current_row_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    //    auto index_list = _tree_view->selectionModel()->selectedIndexes();

    //    for(auto _i : index_list) { // always one element
    //        _tree_view->selectionModel()->select(_i, current_tree_selection_mode);
    //    }

    //    if(previous.isValid())
    //        _tree_view->selectionModel()->select(previous, current_tree_selection_mode);    // dangerous

    //    auto it = _tree_view->source_model()->item(current);

    //    if(it && it != _tree_view->source_model()->root_item()) {
    //        if(current.isValid() && _tree_view->current_index() != current && !it->is_activated()) {


    if(_tree_view->currentIndex() != current)_tree_view->select_as_current(current);

    //    index_invoke(_tree_view->current_index());    // current

    //        }
    //    }
}


// Открытие контекстного меню в дереве разделов
void TreeScreen::on_custom_contextmenu_requested(const QPoint &_position)
{
    qDebug() << "In TreeScreen::on_customContextMenuRequested";

    //    // Конструирование меню
    //    QMenu _context_menu(this);
    //    _context_menu->clear();

    auto _current_model = [&]() {return _tree_view->source_model();};
    //    //select item at cursor position
    //    //    QPersistentModelIndex
    //    QModelIndex index = _tree_view->indexAt(_position);
    //    _tree_view->select_and_current(index);

    // Получение индекса выделенной ветки
    QModelIndex index = _tree_view->current_index();

    if(index.isValid()) {
        // Выясняется, зашифрована ли ветка или нет
        QString cryptFlag = _current_model()->item(index)->field("crypt");
        auto current_item = _current_model()->item(index);
        assert(current_item);
        auto _parent = _current_model()->item(index)->parent();
        assert(_parent);
        // Выясняется, зашифрована ли родительская ветка
        QString parentCryptFlag = _parent ? _parent->field("crypt") : "0";

        // Если ветка не зашифрована
        // Или ветка зашифрована, но пароль успешно введен
        if(cryptFlag != "1" ||
           (cryptFlag == "1" && globalparameters.crypt_key().length() > 0)) {

            // Если в буфере есть ветки, соответсвующие пункты становятся активными
            bool is_branch = false;
            const QMimeData *mimeData = QApplication::clipboard()->mimeData();

            if(mimeData != nullptr)
                if(mimeData->hasFormat("mytetra/branch"))
                    is_branch = true;

            enable_up_action();


            // ----------------------------
            // Подсветка пунктов шифрования
            // ----------------------------

            // Если ветка незашифрована
            if(cryptFlag != "1") {
                // Шифровать можно
                // Дешифровать нельзя
                _actionlist["encryptBranch"]->setEnabled(true);
                _actionlist["decryptBranch"]->setEnabled(false);
            } else {
                // Ветка зашифрована

                // Шифровать нельзя
                _actionlist["encryptBranch"]->setEnabled(false);

                // Дешифровать можно только если верхнележащая ветка незашифрована
                if(parentCryptFlag != "1")
                    _actionlist["decryptBranch"]->setEnabled(true);
                else
                    _actionlist["decryptBranch"]->setEnabled(false);
            }
        }
    }



    //    //    _context_menu->addAction(_table_screen->_pin);
    //    _context_menu->addAction(_table_screen->_addnew_to_end);
    //    _context_menu->addAction(_table_screen->_edit_field);
    //    _context_menu->addAction(_table_screen->_delete);

    //    _context_menu->addSeparator();

    //    _context_menu->addAction(_table_screen->_addnew_before);
    //    _context_menu->addAction(_table_screen->_addnew_after);
    //    //    contextMenu->addAction(parentPointer->actionMoveUp);
    //    //    contextMenu->addAction(parentPointer->actionMoveDn);
    //    //    contextMenu->addAction(parentPointer->actionSyncro);

    //    _context_menu->addSeparator();

    //    _context_menu->addAction(_table_screen->_cut);
    //    _context_menu->addAction(_table_screen->_copy);
    //    _context_menu->addAction(_table_screen->_paste);

    //    _context_menu->addSeparator();

    //    _context_menu->addAction(_table_screen->_sort);
    //    _context_menu->addAction(_table_screen->_print);
    //    _context_menu->addAction(_table_screen->_settings);

    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    _context_menu->exec(_tree_view->viewport()->mapToGlobal(_position));
}


//bool TreeScreen::is_index_localized(const QModelIndexList _origin_index_list)const
//{
//    bool result = true;

//    if(_origin_index_list.size() > 0) {
//        //        assert(_origin_index_list.last() == _tree_view->current_index()); // means duplicated item found
//        //preparations
//        auto _index_current = _origin_index_list.last();    // _tree_view->current_index();    //

//        auto _index_common_parent = _index_current.parent();


//        for(int i = 0; i < _origin_index_list.size(); ++i) {
//            QModelIndex index = _origin_index_list.at(i);

//            if(index.isValid() && index.parent() != _index_common_parent) {
//                result = false; break;
//            }
//        }
//    }

//    return result;
//}

//QModelIndexList TreeScreen::index_localize(const QModelIndexList _origin_index_list)
//{
//    QModelIndexList _selectitems;

//    if(_origin_index_list.size() > 0) {
//        //        auto _source_model = _tree_view->source_model();
//        // На время удаления блокируется главное окно
//        //        //    find_object<MainWindow>("mainwindow")
//        //        globalparameters.mainwindow()->setDisabled(true);
//        //        //    find_object<MainWindow>("mainwindow")
//        //        globalparameters.mainwindow()->blockSignals(true);

//        //        assert(_origin_index_list.first() == _tree_view->current_index());

//        if(!_origin_index_list.contains(_tree_view->current_index())) {

//            //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
//            auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);

//            //            QModelIndex _current_index;
//            boost::intrusive_ptr<TreeItem> duplicated_item = _tree_view->current_item();
//            boost::intrusive_ptr<TreeItem> found;

//            for(int i = 0; i < _origin_index_list.size(); ++i) {
//                auto _index = _origin_index_list.at(i);

//                if(duplicated_item->id() == _source_model()->item(_index)->id()) {
//                    //                    _current_index = _index;
//                    found = _source_model()->item(_index);
//                    break;
//                }
//            }

//            if(found && duplicated_item && found != duplicated_item) {
//                found = _source_model()->model_duplicated_merge_to_left(
//                            std::bind(&TreeScreen::view_delete_items, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
//                            , found, duplicated_item
//                        );
//            }
//        }


//        //preparations
//        auto _index_current         = _tree_view->current_index();    //_origin_index_list.first();
//        auto _index_common_parent   = _index_current.parent();
//        //    auto index_to_be_delete_last = _index_list.last();
//        //        auto _item_to_be_deleted_first = _source_model->item(index_to_be_delete_first);
//        //        auto deleted_position_first = _item_to_be_deleted_first->sibling_order(); // items_update_selected();
//        //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();
//        //        auto _item_parent = _item_to_be_deleted_first->parent();



//        //        // Список имен веток, которые нужно удалить
//        //        QStringList branches_name;

//        for(int i = 0; i < _origin_index_list.size(); ++i) {
//            QModelIndex index = _origin_index_list.at(i);

//            if(index.isValid() && index.parent() == _index_common_parent) {
//                //                auto it = _source_model->item(index);
//                //                branches_name << it->field("name");
//                _selectitems.push_back(index);
//            }
//        }
//    }

//    return _selectitems;
//}

void TreeScreen::index_invoke(const QModelIndex &_index)
{

    if(_index.isValid()) { //       && _tree_view->index_current() != _index

        //        KnowModel *(TreeScreen::*_source_model_func)() = &TreeScreen::know_model_board;
        //        auto _source_model = std::bind(_source_model_func, this);

        //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
        auto _source_model = [&]() {return _tree_view->source_model();}; // std::bind(_source_model_func, _tree_view);

        // Получаем указатель на текущую выбранную ветку дерева
        auto result_item = _tree_view->source_model()->item(_index);
        auto duplicated_items_list_on_board_by_url = _source_model()->items([&](boost::intrusive_ptr<const TreeItem> it) {return it->field("url") == result_item->field("url");});    // _tree_view->source_model()


        if(duplicated_items_list_on_board_by_url.size() > 1) {
            for(auto _i : duplicated_items_list_on_board_by_url) {
                auto duplicated_item = _i;

                if(duplicated_item != result_item) { // (duplicated_item->id() != result_item->id()) ||
                    result_item = view_merge_to_left(_source_model, result_item, duplicated_item);
                }
            }
        }

        //        auto duplicated_item_index_list_on_view = _source_model()->indexes([&](boost::intrusive_ptr<TreeItem> it) {return it->field("url") == result_item->field("url");});
        auto duplicated_items_list_on_board_by_id = _source_model()->items([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == result_item->id();});


        if(duplicated_items_list_on_board_by_id.size() > 1) {
            for(auto _i : duplicated_items_list_on_board_by_id) {
                auto duplicated_item = _i;

                if(duplicated_item != result_item) { // (duplicated_item->id() != result_item->id()) ||
                    result_item = view_merge_to_left(_source_model, result_item, duplicated_item);
                }
            }
        }

        if(!result_item->is_activated()) {
            // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

            // Сохраняется текст в окне редактирования в соответсвующий файл
            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->save_text_area();

            _tree_view->setExpanded(_index.parent(), true);   // (_tree_view->isExpanded(_index)) ? _tree_view->setExpanded(_index, false) : _tree_view->setExpanded(_index, true);


            if(result_item->count_direct() > 0) {    // && (result_item->field("url") != "" || result_item->field("url") == "" )
                // Вначале все инструменты работы с веткой включаются
                QMapIterator<QString, QAction *> i(_actionlist);

                while(i.hasNext()) {
                    i.next();
                    i.value()->setEnabled(true);
                }

                // Проверяется, происходит ли клик по зашифрованной ветке
                if(result_item->field("crypt") == "1") {
                    // Если пароль доступа к зашифрованным данным не вводился в этой сессии
                    if(globalparameters.crypt_key().length() == 0) {
                        // Запрашивается пароль
                        Password password;

                        if(password.retrievePassword() == false) {
                            //                    // Устанавливаем пустые данные для отображения таблицы конечных записей
                            //                    // find_object<RecordController>("table_screen_controller")

                            //                    //                    result_item->unique_page()->view()->record_controller()->source_model()->clear();



                            //                    _tree_view->setup_model(boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, QMap<QString, QString>())));   // _know_branch->clear();  // root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));




                            //                    //                    init_source_model(
                            //                    //                        nullptr
                            //                    //                        , result_item->unique_page()->view()->record_controller()
                            //                    //                        , result_item->unique_page()->view()->record_controller()->record_screen()
                            //                    //                        , globalparameters.mainwindow()
                            //                    //                        , globalparameters.meta_editor()
                            //                    //                        // , globalparameters.record_screens()
                            //                    //                    );

                            //                    //                        reset_tabledata(nullptr);

                            //                    // Все инструменты работы с веткой отключаются
                            QMapIterator<QString, QAction *> i(_actionlist);

                            while(i.hasNext()) {
                                i.next();
                                i.value()->setEnabled(false);
                            }

                            return; // Программа дальше не идет, доделать...
                        }
                    }
                }

                //        if(_tree_view->source_model()->root_item() != result_item) setup_model(result_item); // _know_branch->intercept(result_item);     //? never do this!!!

                //        //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
                //        //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

                //        // Получаем указатель на данные таблицы конечных записей
                //        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
                //        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();



                //        // setup_model(_know_branch);

                //        enable_up_action(); // _know_branch->root_item()->id() != static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id()   // _know_branch->root_item()->id() != know_root()->root_item()->id()


                //        //    for(int i = 0; i < item->tabledata()->size(); i++) {

                //        //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
                //        //            _shadow_candidate_model->_root_item->tabledata()->size()
                //        //            , item->tabledata()->record(i)
                //        //        );
                //        //    }

                //        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
                //        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();


                //        //    _shadow_candidate_model->_root_item = result_item; // ->record_table(item->record_table()->export_to_dom());







                //        //        // Устанавливаем данные таблицы конечных записей
                //        //        find_object<RecordController>(table_controller_singleton_name)->source_model()->tree_item(_shadow_candidate_model->_root_item);
                //        //        // need lock?


                //        //            reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item, item->tabledata()->export_to_dom(doc)));


                // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
                if(appconfig.interface_mode() == "mobile") {

                    QStringList path = result_item->path_absolute_as_name();

                    // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                    int emptyStringIndex = path.indexOf("");
                    path.removeAt(emptyStringIndex);

                    //            find_object<RecordScreen>(table_screen_singleton_name)
                    globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

                }



                // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
                _tree_view->resizeColumnToContents(0);

                //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);

            }

            //        else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
            //            //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
            //            // Переключаются окна (используется для мобильного интерфейса)
            //            //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
            //        }

            if(result_item->field("url") != "") {
                //            _tree_view->select_and_current(result_item);
                if(!result_item->binder() || (result_item->binder() && !result_item->binder()->integrity_internal()))
                    item_bind(result_item, std::bind(&TreeScreen::view_paste_child, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->activate();
                else {
                    result_item->activate();
                }
            }
        }
    }
}

//// deprecated
//void TreeScreen::index_invoke_in_sub_branch(const QModelIndex &_index)
//{
//    if(_index.isValid() //       && _tree_view->index_current() != _index
//       && !_tree_view->source_model()->item(_index)->is_activated()) {
//        view_step_into(_index);

//#ifdef _with_record_table
//        _treemodelknow->record_to_item();
//#endif
//        //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
//    }
//}


boost::intrusive_ptr<TreeItem> TreeScreen::session_root_item()
{
    //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
    auto view_source_model = [&]() {return this->tree_view()->source_model();}; // std::bind(_source_model_func, _tree_screen->tree_view());

    auto view_session_root_item  = [&]() {
        return view_source_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == view_source_model()->session_id();});
    };

    auto board_session_root_item  = [&]() {
        return _know_model_board->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == _know_model_board->session_id();});
    };

    if(!board_session_root_item()) {
        view_source_model()->session_id(this->tree_view()->current_item()->id());
        this->know_model_save();
        assert(view_session_root_item());
    }

    //    else {
    //        intercept(view_session_root_item()->id());
    //    }

    while(!view_session_root_item() && view_source_model()->root_item() != _know_model_board->root_item()) {
        this->cursor_follow_up_one_level();
        view_source_model()->session_id(view_source_model()->root_item()->id());
    }

    //    if(!board_session_root_item()) {
    //        this->_session_id = this->tree_view()->current_item()->id();
    //        this->know_model_save();
    //    }

    assert(view_session_root_item());
    return view_session_root_item(); // _session_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();});
}

//QString TreeScreen::session_root_id()
//{
//    if(_session_id == global_root_id) {
//        _session_id = _tree_view->current_item()->id();
//    }

//    return _session_id;
//}

//void TreeScreen::session_root_id(QString root_id)
//{
//    _session_id = root_id;
//}

//QString TreeScreen::session_root() {return _session_id;}
void TreeScreen::session_root_id(bool checked)
{
    Q_UNUSED(checked);
    _tree_view->source_model()->session_id(_tree_view->source_model()->item(_tree_view->current_index())->id());
}




// Действия при клике на ветку дерева
void TreeScreen::cursor_step_into(const QModelIndex &_index)
{
    if(_index.isValid() && !_tree_view->source_model()->item(_index)->is_activated()) {
        // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

        // Сохраняется текст в окне редактирования в соответсвующий файл
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->save_text_area();

        // Получаем указатель на текущую выбранную ветку дерева
        auto result_item = _tree_view->source_model()->item(_index);

        if(result_item->count_direct() > 0) {    // && (result_item->field("url") != "" || result_item->field("url") == "" )
            // Вначале все инструменты работы с веткой включаются
            QMapIterator<QString, QAction *> i(_actionlist);

            while(i.hasNext()) {
                i.next();
                i.value()->setEnabled(true);
            }

            // Проверяется, происходит ли клик по зашифрованной ветке
            if(result_item->field("crypt") == "1") {
                // Если пароль доступа к зашифрованным данным не вводился в этой сессии
                if(globalparameters.crypt_key().length() == 0) {
                    // Запрашивается пароль
                    Password password;

                    if(password.retrievePassword() == false) {
                        // Устанавливаем пустые данные для отображения таблицы конечных записей
                        // find_object<RecordController>("table_screen_controller")

                        //                    result_item->unique_page()->view()->record_controller()->source_model()->clear();



                        _tree_view->source_model(boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, QMap<QString, QString>())));   // _know_branch->clear();  // root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));




                        //                    init_source_model(
                        //                        nullptr
                        //                        , result_item->unique_page()->view()->record_controller()
                        //                        , result_item->unique_page()->view()->record_controller()->record_screen()
                        //                        , globalparameters.mainwindow()
                        //                        , globalparameters.meta_editor()
                        //                        // , globalparameters.record_screens()
                        //                    );

                        //                        reset_tabledata(nullptr);

                        // Все инструменты работы с веткой отключаются
                        QMapIterator<QString, QAction *> i(_actionlist);

                        while(i.hasNext()) {
                            i.next();
                            i.value()->setEnabled(false);
                        }

                        return; // Программа дальше не идет, доделать...
                    }
                }
            }

            if(_tree_view->source_model()->root_item() != result_item) resetup_model(result_item); // _know_branch->intercept(result_item);     //? never do this!!!

            _tree_view->setExpanded(QModelIndex(), true);
            //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
            //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

            // Получаем указатель на данные таблицы конечных записей
            //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
            //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();



            // setup_model(_know_branch);

            enable_up_action(); // _know_branch->root_item()->id() != static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id()   // _know_branch->root_item()->id() != know_root()->root_item()->id()


            //    for(int i = 0; i < item->tabledata()->size(); i++) {

            //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
            //            _shadow_candidate_model->_root_item->tabledata()->size()
            //            , item->tabledata()->record(i)
            //        );
            //    }

            //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
            //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();


            //    _shadow_candidate_model->_root_item = result_item; // ->record_table(item->record_table()->export_to_dom());







            //        // Устанавливаем данные таблицы конечных записей
            //        find_object<RecordController>(table_controller_singleton_name)->source_model()->tree_item(_shadow_candidate_model->_root_item);
            //        // need lock?


            //            reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item, item->tabledata()->export_to_dom(doc)));


            // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
            if(appconfig.interface_mode() == "mobile") {

                QStringList path = result_item->path_absolute_as_name();

                // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                int emptyStringIndex = path.indexOf("");
                path.removeAt(emptyStringIndex);

                //            find_object<RecordScreen>(table_screen_singleton_name)
                globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

            }



            // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
            _tree_view->resizeColumnToContents(0);

            //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);

        }

        //        else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
        //            //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        //            // Переключаются окна (используется для мобильного интерфейса)
        //            //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
        //        }

#ifdef _with_record_table   // deprecated, just for transform old data
        _know_model_board->record_to_item();
#endif

        if(result_item->field("url") != "")
            item_bind(result_item, std::bind(&TreeScreen::view_paste_sibling, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))->activate();
    }
}




boost::intrusive_ptr<TreeItem> TreeScreen::cursor_follow_up(boost::intrusive_ptr<TreeItem> _new_session_root_item)
{
    auto current_root = _tree_view->source_model()->root_item();

    while(_new_session_root_item->is_ancestor_of(current_root)
          && _new_session_root_item->id() != current_root->id()
         ) {
        //        _tree_view->reset();
        //        _tree_view->source_model(_new_session_root_item->parent());
        current_root = cursor_follow_up_one_level();
    }

    _tree_view->select_as_current(_new_session_root_item);

    //    auto entrance = globalparameters.entrance();

    //    for(auto browser : entrance->browsers()) {
    //        for(int i = 0; i < browser->tabmanager()->count(); i++) {
    //            auto view = browser->tabmanager()->webView(i);
    //            auto binder = view->page()->record_binder();

    //            if(binder) {
    //                view_paste_as_child(_tree_view->source_model(), _tree_view->source_model()->index(_new_session_root_item), binder->bounded_item());
    //            }
    //        }
    //    }

    return current_root;    // _new_session_root_item;
}

boost::intrusive_ptr<TreeItem> TreeScreen::cursor_follow_up_one_level(void)
{

    std::function<bool(boost::intrusive_ptr<const TreeItem>)>
    _check_crypt_conflict = [&](boost::intrusive_ptr<const TreeItem> current_root) {
        bool _result = false;
        // Вначале все инструменты работы с веткой включаются
        QMapIterator<QString, QAction *> i(_actionlist);

        while(i.hasNext()) {
            i.next();
            i.value()->setEnabled(true);
        }

        // Проверяется, происходит ли клик по зашифрованной ветке
        if(current_root->field("crypt") == "1") {
            // Если пароль доступа к зашифрованным данным не вводился в этой сессии
            if(globalparameters.crypt_key().length() == 0) {
                // Запрашивается пароль
                Password password;

                if(password.retrievePassword() == false) {
                    // Устанавливаем пустые данные для отображения таблицы конечных записей

                    //                    find_object<RecordController>("table_screen_controller")

                    //                    current_root->unique_page()->view()->record_controller()->source_model()->clear();

                    // _know_branch
                    // current_treemodel
                    _tree_view->source_model()->clear();  //root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));

                    //                    init_source_model(
                    //                        nullptr
                    //                        , current_root->unique_page()->view()->record_controller()
                    //                        , current_root->unique_page()->view()->record_controller()->record_screen()
                    //                        , globalparameters.mainwindow()
                    //                        , globalparameters.meta_editor()
                    //                        // , result_item->unique_page()->view()->record_controller()->record_screen()
                    //                    );

                    //                        reset_tabledata(nullptr);

                    // Все инструменты работы с веткой отключаются
                    QMapIterator<QString, QAction *> i(_actionlist);

                    while(i.hasNext()) {
                        i.next();
                        i.value()->setEnabled(false);
                    }

                    // return current_root; // Программа дальше не идет, доделать...
                } else {_result = true;}
            }
        }

        return _result;
    };

    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    //    KnowModel *current_treemodel = _tree_view->source_model();  //static_cast<KnowModel *>(this->_tree_view->model());
    boost::intrusive_ptr<TreeItem> current_root = _tree_view->source_model()->root_item();    // ->parent();
    auto _index = _tree_view->current_index();

    //    if(!index.isValid()) {
    //        _tree_view->selection_to_pos(_tree_view->source_model()->root_item()->current_count() - 1);
    //        index = _tree_view->view_index();
    //        assert(index.isValid());
    //    }

    auto current_item = _tree_view->source_model()->item(_index);
    //    assert(current_treemodel == _know_branch);
    int origin_count = _tree_view->source_model()->count_records_all();

    if(current_root) {
        //        //        current_treemodelknow->root(parent);   // QModelIndex index = _root->index(current_treemodelknow->root()->parent());
        //        //        //        candidate_from_knowtree_item(index);
        //        //        setup_model(current_treemodelknow);

        //        //        if(current_treemodelknow->root() != _root->root()) {
        //        //            enable_up_action(true);
        //        //        } else {
        //        //            enable_up_action(false);
        //        //        }

        //        // Вначале все инструменты работы с веткой включаются
        //        QMapIterator<QString, QAction *> i(_actionlist);

        //        while(i.hasNext()) {
        //            i.next();
        //            i.value()->setEnabled(true);
        //        }

        //        // Проверяется, происходит ли клик по зашифрованной ветке
        //        if(current_root->field("crypt") == "1") {
        //            // Если пароль доступа к зашифрованным данным не вводился в этой сессии
        //            if(globalparameters.crypt_key().length() == 0) {
        //                // Запрашивается пароль
        //                Password password;

        //                if(password.retrievePassword() == false) {
        //                    // Устанавливаем пустые данные для отображения таблицы конечных записей

        //                    //                    find_object<RecordController>("table_screen_controller")

        //                    //                    current_root->unique_page()->view()->record_controller()->source_model()->clear();

        //                    // _know_branch
        //                    current_treemodel->clear();  //root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));

        //                    //                    init_source_model(
        //                    //                        nullptr
        //                    //                        , current_root->unique_page()->view()->record_controller()
        //                    //                        , current_root->unique_page()->view()->record_controller()->record_screen()
        //                    //                        , globalparameters.mainwindow()
        //                    //                        , globalparameters.meta_editor()
        //                    //                        // , result_item->unique_page()->view()->record_controller()->record_screen()
        //                    //                    );

        //                    //                        reset_tabledata(nullptr);

        //                    // Все инструменты работы с веткой отключаются
        //                    QMapIterator<QString, QAction *> i(_actionlist);

        //                    while(i.hasNext()) {
        //                        i.next();
        //                        i.value()->setEnabled(false);
        //                    }

        //                    return current_root; // Программа дальше не идет, доделать...
        //                }
        //            }
        //        }
        if(!_check_crypt_conflict(current_root)) {
            //        auto new_parent = current_item->parent()->parent();

            if( //current_root->parent() &&
                current_item->parent() != current_root && current_item->parent()->path_absolute().size() > current_root->path_absolute().size()) {
                //            setup_model(current_root->parent());  //_know_branch->intercept(current_root->parent());     //? dangerous to do this!!!
                _tree_view->select_as_current(_tree_view->source_model()->index(current_item->parent()));
                //            _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode);
                //            _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item->parent()), current_tree_current_index_mode);  // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                //            enable_up_action();
            } else if(current_item->parent() == current_root) {
                if(current_root->parent()) {
                    current_root = current_root->parent();
                    resetup_model(current_root);
                    _tree_view->select_as_current(_tree_view->source_model()->index(current_item->parent()));
                    //                _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode); // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                    //                _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item->parent()), current_tree_current_index_mode); // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                    //            enable_up_action();
                }
            }

            //        else if(!current_root->parent() && current_item->parent()) {
            //            //            if(_root->root_item()->parent())
            //            //                _selected_branch->root_item(_root->root_item()->parent());
            //            //            else
            //            //            QString path = (current_root->absolute_path().length() > 1) ? current_root->absolute_path().at(1) : current_root->absolute_path().at(0);
            //            setup_model(_know_model_board->root_item());  // _know_branch->intercept(know_root()->root_item());
            //            _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item->parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
            //            //            enable_up_action();
            //        }





            //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
            //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

            // Получаем указатель на данные таблицы конечных записей
            //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
            //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();

            assert(_tree_view->source_model()->count_records_all() >= origin_count);

            //        //        setup_model(_know_branch);

            //        enable_up_action() ;

            //        //    for(int i = 0; i < item->tabledata()->size(); i++) {

            //        //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
            //        //            _shadow_candidate_model->_root_item->tabledata()->size()
            //        //            , item->tabledata()->record(i)
            //        //        );
            //        //    }

            //        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
            //        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();


            //        //    _shadow_candidate_model->_root_item = result_item; // ->record_table(item->record_table()->export_to_dom());







            //        //        // Устанавливаем данные таблицы конечных записей
            //        //        find_object<RecordController>(table_controller_singleton_name)->source_model()->tree_item(_shadow_candidate_model->_root_item);
            //        //        // need lock?


            //        //            reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item, item->tabledata()->export_to_dom(doc)));


            // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
            if(appconfig.interface_mode() == "mobile") {

                QStringList path = current_root->path_absolute_as_name();

                // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                int emptyStringIndex = path.indexOf("");
                path.removeAt(emptyStringIndex);

                //            find_object<RecordScreen>(table_screen_singleton_name)
                globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

            }



            // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
            _tree_view->resizeColumnToContents(0);
        }

    } else if(current_item->parent()) {
        current_item = current_item->parent();

        if(current_item->parent()) {
            current_root = current_item->parent();

            if(!_check_crypt_conflict(current_root)) {
                resetup_model(current_root);  // _know_branch->intercept(know_root()->root_item());
            }
        } else {
            current_root = _know_model_board->root_item();

            if(!_check_crypt_conflict(current_root)) {
                resetup_model(current_root);  // _know_branch->intercept(know_root()->root_item());
            }
        }

        _tree_view->select_as_current(_tree_view->source_model()->index(current_item));
        //        _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item), current_tree_selection_mode);
        //        _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item), current_tree_current_index_mode);   // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

    } else {
        current_root = _know_model_board->root_item();

        if(!_check_crypt_conflict(current_root)) {
            current_root = cursor_follow_up_to_root();
        }

    }

    //    if(current_treemodelknow->root()->parent() != _root->root()->parent()) {
    //        assert(current_treemodelknow->root() != _root->root());

    //        if(current_treemodelknow->root()->parent())current_treemodelknow->root(current_treemodelknow->root()->parent());

    //        setup_model(current_treemodelknow);

    //        //        if(current_treemodelknow->root() != _root->root()) {
    //        enable_up_action(true);
    //        //        } else {
    //        //            enable_up_action(false);
    //        //        }
    //    } else {
    //        setup_model(_root);
    //        enable_up_action(false);
    //    }

    //    enable_up_action(current_treemodelknow->root() != _root->root());
    //    }
    enable_up_action();
    return current_root;

}

boost::intrusive_ptr<TreeItem> TreeScreen::cursor_follow_up_to_root(void)
{
    boost::intrusive_ptr<TreeItem> current_root;
    KnowModel *current_source_model = _tree_view->source_model();  //static_cast<KnowModel *>(this->_tree_view->model());
    //    boost::intrusive_ptr<TreeItem> current_root = current_treemodel->root_item();
    auto _index = _tree_view->current_index();

    //    if(!index.isValid()) {
    //        _tree_view->selection_to_pos(_tree_view->source_model()->root_item()->current_count() - 1);
    //        index = _tree_view->view_index();
    //        assert(index.isValid());
    //    }

    if(_index.isValid()) {
        auto current_item = current_source_model->item(_index);

        while(current_item->parent()->parent()) {current_item = current_item->parent();}

        //    QString path = current_root->absolute_path().at(1);
        // _know_branch->intercept(know_root()->root_item());
        current_root = _know_model_board->root_item();
        resetup_model(current_root);  // setup_model(_know_branch);
        _tree_view->select_as_current(_tree_view->source_model()->index(current_item));
        //        _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item), current_tree_selection_mode);
        //        _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item), current_tree_current_index_mode);    // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
        enable_up_action(); // false
    }

    return current_root;

}





/*
void treescreen::on_knowTreeView_repaint(void)
{
 // Попытка расширить нулевую колонку с деревом, если содержимое слишком широкое
 knowTreeView->resizeColumnToContents(0);
}
*/






//void TreeScreen::cursor_to_index(QModelIndex _index)
//{
//    if(_index.isValid() //       && _tree_view->index_current() != _index
//       && !_tree_view->source_model()->item(_index)->is_activated()) {
//        // Курсор устанавливается на нужный элемент дерева
//        // В desktop-варианте на сигнал currentRowChanged() будет вызван слот on_knowtree_clicked()
//        _tree_view->select_and_current(_index);
//        //        _tree_view->selectionModel()->select(_index, current_tree_selection_mode);
//        //        _tree_view->selectionModel()->setCurrentIndex(_index, current_tree_current_index_mode);   // ClearAndSelect

//        // В мобильной версии реакции на выбор ветки нет (не обрабатывается сигнал смены строки в модели выбора)
//        // Поэтому по ветке должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
//        // Метод clicked() публичный начиная с Qt5 (мобильный интерфейс возможен только в Qt5)
//#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

//        if(_appconfig.getInterfaceMode() == "mobile")
//            emit _tree_view->clicked(_index);

//#endif
//    }
//}



//QItemSelectionModel *TreeScreen::selection_model(void)
//{
//    return _tree_view->selectionModel();
//}


//// Получение номера первого выделенного элемента
//int TreeScreen::item_first_selected(void)
//{
//    // Получение списка выделенных Item-элементов
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

//    if(selectitems.isEmpty())
//        return -1; // Если ничего не выделено
//    else
//        return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
//}



// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
void TreeScreen::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count " << _know_model_board->rowCount();

    if(_know_model_board->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";

        //        std::function<KnowModel *()> _know_model_board_functor = [&]() {return _know_model_board;};
        view_add_new(TreeModel::ModelIndex([&]()->KnowModel* { return _tree_view->source_model();  // _know_model_board;
                                                             }, QModelIndex())
        , tr("Rename me"), [&](TreeModel::ModelIndex _modelindex, QString _id, QString _name)->boost::intrusive_ptr<TreeItem> {
            auto _current_model = _modelindex.current_model();
            auto _current_index = _modelindex.current_index();
            return _current_model()->model_add_child_new(_current_index, _id, _name);
        });
    }
}


// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void TreeScreen::tree_crypt_control(void)
{
    // Если в дереве нет шифрования
    if(_know_model_board->is_contains_crypt_branches() == false) {
        // Запускается диалог сброса пароля шифрования
        Password pwd;
        pwd.resetPassword();
    }
}


boost::intrusive_ptr<TreeItem> TreeScreen::intercept(QString id)
{
    //    auto prepared = know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id)));
    auto result = _know_model_board->item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == id;});     // _know_model_board->index(KnowModel::delegater(id))
    // _know_branch->intercept(prepared);    //item(_know_branch->index(prepared));

    resetup_model(result);   //    return _know_branch->intercept(know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id))));

    return result;
}


//boost::intrusive_ptr<TreeItem> TreeScreen::synchronize()
//{
//    boost::intrusive_ptr<TreeItem> result(nullptr);

//    if(!_know_branch->synchronized())
//        result = know_root_holder::know_root()->synchronize(_know_branch->root_item());

//    return result;
//}

// Сохранение дерева веток на диск
void TreeScreen::know_model_save(void)
{
    //    if(!_know_branch->synchronized())
    //        know_root_holder::know_root()->synchronize(_know_branch->root_item());

    //    know_root_holder::know_root()->save();
    _tree_view->sychronize();
    _know_model_board->save();

}


// Перечитывание дерева веток с диска
void TreeScreen::know_model_reload(void)
{
    _know_model_board->reload(); // do not do this if xml_file_name of this object is null
    //    know_root_holder::know_root()->reload();

}

//boost::intrusive_ptr<TreeItem> TreeScreen::item(QStringList path) const
//{
//    return know_root_holder::know_root()->item(path);
//}


//boost::intrusive_ptr<TreeItem> TreeScreen::item(TreeModel::delegater _del) const
//{
//    return know_root_holder::know_root()->item(_del);
//}

KnowModel *TreeScreen::know_model_board()const {return _know_model_board;}
//KnowModel *TreeScreen::know_model_board() {return _know_model_board;}

void TreeScreen::synchronized(bool _synchronized) {_know_model_board->synchronized(_synchronized);}

bool TreeScreen::synchronized() {return _know_model_board->synchronized();}

boost::intrusive_ptr<TreeItem> TreeScreen::view_paste_strategy(
    TreeModel::ModelIndex                                       _modelindex
    , boost::intrusive_ptr<TreeItem>                            _result
    , const TreeScreen::substitute_condition                    &_substitute_condition
    , const TreeScreen::paste_strategy                          &_view_paste_strategy_impl
    , const bool                                                _item_is_brand_new
    , const QUrl                                                &_find_url
    , const std::function<bool(boost::intrusive_ptr<TreeItem>)> &_check_url
)
{
    auto _current_model = _modelindex.current_model();
    auto _current_item = _current_model()->item(_modelindex.current_index());

    auto _tree_screen = globalparameters.tree_screen();

    //        //                    auto _index = _tree_screen->tree_view()->current_index();
    //        // if(idx.isValid()) {
    //        //        _current_view_model = _tree_screen->tree_view()->source_model();
    //        assert(_session_root_item());

    //        //        auto _session_root_item  = _root;    // _current_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();}); // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());

    //        auto _session_root_index = _session_view_model()->index(_session_root_item()); //current_index();

    //        //        if(_session_root_index.isValid()) {
    //        //            _session_root_item  = [&]() {return _session_view_model()->item(_tree_screen->tree_view()->current_index());}; // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
    //        //            _session_root_index = _tree_screen->tree_view()->current_index(); //current_index();
    //        //        }

    //        //        assert(_session_root_item());

    //        assert(_session_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _session_root_item()->id();}));

    //        //        if(_item->is_lite())_item->to_fat();

    if(_item_is_brand_new || !_current_model()->item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == _result->id();})) {
        assert(_check_url(_result));   // assert(_result->url() == item->url());
        // assert(_result->fragment() == item->fragment());
        // int pos
        _result = _view_paste_strategy_impl(_modelindex, _result, _substitute_condition); // it->insert_new_item(it->current_count() - 1, _result);
        assert(_result);
        assert(_check_url(_result) || _result->field("url") == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);

        if(_result->field("url") == browser::Browser::_defaulthome) {
            _result->field("url", _find_url.toString());  // item->field("url")
        }

        _tree_screen->synchronized(false);
        // assert(_result == it->child(pos));
    } else if(_current_item != _result) {
        if(_result->is_ancestor_of(_current_item)) {
            //                _tree_screen->session_root_id(_result->id());

            //                if(_result != _current_item->parent()) {
            _result = _tree_screen->cursor_follow_up(_result);
            assert(_result);
            //                }
            _tree_screen->synchronized(false);

        } else if(session_root_item() != _result->parent()) { // if(session_root_item->is_ancestor_of(_result) && session_root_item != _result->parent())
            _result = _tree_screen->view_paste_child(_modelindex, _result, _substitute_condition);
            assert(_result);
            _tree_screen->synchronized(false);
            // }
            // else {
            // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
        }

        // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);

    }


    _tree_screen->know_model_save();
    // }
    assert(_result);
    return _result;
}


// boost::intrusive_ptr<TreeItem> item_request_from_tree_impl(const QUrl &_url);
//    template<typename url_type = url_full>
boost::intrusive_ptr<TreeItem> TreeScreen::item_register(//        std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal  //
    const QUrl &_find_url
    , const TreeScreen::paste_strategy &_view_paste_strategy
    , equal_url_t _equal
)
{
    boost::intrusive_ptr<TreeItem> _result(nullptr);    // =  _know_model_board->root_item();
    TreeScreen *tree_screen    = globalparameters.tree_screen();
    //    auto _know_model_root = tree_screen->know_root();
    auto know_model_board      = tree_screen->know_model_board();

    //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
    auto view_source_model = [&]() {return tree_screen->tree_view()->source_model();}; // std::bind(_source_model_func, _tree_screen->tree_view());

    //        auto session_root_item = [&]() {
    //            auto internal_session_root_item = [&]() {return view_source_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();});};

    //            while(!internal_session_root_item() && view_source_model()->root_item() != know_model_board->root_item()) {
    //                tree_screen->cursor_follow_up_one_level();
    //                tree_screen->session_root_id(view_source_model()->root_item()->id());
    //            }

    //            if(!know_model_board->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();})) {
    //                tree_screen->know_model_save();
    //                tree_screen->session_root_id(tree_screen->tree_view()->current_item()->id());
    //            }
    //            assert(internal_session_root_item());

    //            return internal_session_root_item(); // _session_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();});
    //        };   // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());



    auto check_euqal = [&](boost::intrusive_ptr<const TreeItem> it) {return _equal(it, _find_url);};

    //    boost::intrusive_ptr<TreeItem> _source_root_item = tree_screen->know_branch()->item(TreeModel::delegater(_url));    // on know_root semantic
    boost::intrusive_ptr<TreeItem> in_tree = know_model_board->item(check_euqal);

    //    if(_source_root_item && !_source_item) {
    //        auto result = tree_screen->cut_from_root(_source_root_item);

    //        if(result)_source_item = tree_screen->paste_to_branch(result, _know_model_branch);

    //        assert(result);
    //        assert(_source_item);
    //        assert((_source_item == result) && (result == _source_root_item));
    //    }

    bool item_is_brand_new = false;
    //    //    if(_record_controller) {
    //    auto browser_pages = this->_source_model->browser_pages();
    //    assert(browser_pages);

    //    if(browser_pages) {
    auto v = globalparameters.entrance()->find(check_euqal);

    boost::intrusive_ptr<TreeItem> in_browser;

    if(v) {
        in_browser = v->page()->binder()->item();
        assert(check_euqal(in_browser));   // assert(_result->url<url_type>() == url_type()(_find_url));
    }

    if(in_tree && in_tree != know_model_board->root_item()) {
        if(!in_browser) {
            assert(check_euqal(in_tree));  // assert(_source_item->url<url_type>() == url_type()(_find_url));

            if(in_tree->is_lite())in_tree->to_fat();

            //            //            _source_item->binder(generator);
            //            //            _source_item->activator(activator);

            //            //            _item = register_item_to_browser_source_model(_source_item);

            //            _source_item->is_registered_to_record_controller_and_tabmanager(false);
            //            // _source_item->self_bind();
            _result = in_tree;
        } else {
            assert(check_euqal(in_browser));   // assert(_result->url<url_type>() == url_type()(_find_url));
            assert(check_euqal(in_tree));  // assert(_source_item->url<url_type>() == url_type()(_find_url));
            assert(in_browser == in_tree);
            //            assert(in_browser->fragment() == in_tree->fragment());

            //            if(_result != in_tree) {
            //                _result = in_tree; // assert(_item.get() == _source_item.get());
            //                // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, _source_item);
            //            }

            _result = in_tree;

            if(_result->is_lite())_result->to_fat();

            if(_result->field("id") == "")_result->field("id", get_unical_id());

            assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

            //            //            _result->binder(generator);
            //            //            _result->activator(activator);

            //            _result->is_registered_to_record_controller_and_tabmanager(false);
            //            // _item->self_bind();
        }

        assert(!_result->is_lite());
        //        assert(_result->is_registered_to_record_controller_and_tabmanager());

    } else {
        item_is_brand_new = true;

        if(!in_browser) {

            //                int pos = _record_ontroller->getFirstSelectionPos();
            //                Record *previous_record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

            //                if(previous_record) {

            //                    Record record;

            //                    if(record.isLite())record.switchToFat();

            //                    //QString title = d->view->title(); // not ready yet
            //                    //record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));   // id concept?
            //                    record.setNaturalFieldSource("pin",   "");
            //                    record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
            //                    record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
            //                    record.setNaturalFieldSource("url",    _url.toString());    // only changed
            //                    record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

            //                    _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
            //                    _record = recordtabledata->getRecordByUrl(_url);
            //                    //                int pos = _record_ontroller->getFirstSelectionPos();
            //                    //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);
            //                } else {




            //    record.generator(generator);


            // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
            QString directory = DiskHelper::create_temp_directory();  //

            QMap<QString, QString> data;
            data["id"]      = get_unical_id();
            data["pin"]     = _string_from_check_state[Qt::Unchecked];
            data["name"]    = "";
            data["author"]  = "";
            data["home"]    = _find_url.toString();
            data["url"]     = _find_url.toString();
            data["tags"]    = "";
            data["dir"]     = data["id"];
            data["file"]    = "text.html";

            boost::intrusive_ptr<TreeItem> new_item
                = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

            //                if(record.isLite())
            new_item->to_fat();
            new_item->text_to_fat("");
            //            //                QString title = _url.toString(); // not ready yet
            //            item->field("id",       get_unical_id());
            //            item->field("pin",      _check_state[Qt::Unchecked]);
            //            item->field("name",     "");
            //            item->field("author",   "");
            //            item->field("home",     _url.toString());    // only changed
            //            item->field("url",      _url.toString());    // only changed
            //            item->field("tags",     "");

            //            //                _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
            //            //                _record = recordtabledata->getRecordByUrl(_url);
            //            //                //                int pos = _record_ontroller->getFirstSelectionPos();
            //            //                //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

            //            //                //            }

            //            //                record->binder(generator);
            //            //                record->activator(activator);

            new_item->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));


            // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
            // Запись должна быть создана, потом можно аттачить файлы.
            // Это ограничение для "ленивого" программинга, но пока так
            // record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

            // Временная директория с картинками и приаттаченными файлами удаляется
            DiskHelper::remove_directory(directory);

            if(new_item->is_lite())new_item->to_fat();

            //            //            item->binder(generator);
            //            //            item->activator(activator);

            //            //            _item = register_item_to_browser_source_model(item);
            //            item->is_registered_to_record_controller_and_tabmanager(true);
            //            // item->self_bind();
            //            //                assert(_record);
            //            //                assert(_record->is_registered());
            //            //                _record->active_immediately(active_immediately);
            //            //                _record->generator(generator);


            _result = new_item; // assert(_item.get() == item.get());
        } else {
            assert(check_euqal(in_browser));   // assert(_result->url<url_type>() == url_type()(_find_url));

            _result = in_browser;

            if(_result->is_lite())_result->to_fat();

            if(_result->field("id") == "")_result->field("id", get_unical_id());

            assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

            if(_result->field("url") == browser::Browser::_defaulthome) {
                _result->field("url", _find_url.toString());
            }

            //            //            _result->binder(generator);
            //            //            _result->activator(activator);

            //            _result->is_registered_to_record_controller_and_tabmanager(true);
            //            // _item->self_bind();
        }


    }

    //        {
    //            //                auto _index = _tree_screen->tree_view()->current_index();

    //            // if(idx.isValid()) {
    //            _current_view_model = _tree_screen->tree_view()->source_model();

    //            auto session_root_item = _current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();}); // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
    //            auto session_root_index = _tree_screen->tree_view()->source_model()->index(session_root_item); //current_index();
    //            assert(session_root_index.isValid());
    //            assert(session_root_item);
    //            assert(_current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == session_root_item->id();}));

    //            //        if(_item->is_lite())_item->to_fat();
    //            if(session_root_item != _result) {
    //                if(item_is_brand_new) {
    //                    assert(_check_url(_result));   // assert(_result->url<url_type>() == url_type()(_find_url));
    //                    // assert(_result->fragment() == _url.fragment());
    //                    // int pos
    //                    _result = _view_paste_strategy(_current_view_model, session_root_index, _result); // it->insert_new_item(it->current_count() - 1, _result);
    //                    assert(_result);
    //                    assert(_check_url(_result) || _result->field("url") == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);

    //                    if(_result->field("url") == browser::Browser::_defaulthome) {
    //                        _result->field("url", _find_url.toString());
    //                    }

    //                    _tree_screen->synchronized(false);
    //                    // assert(_result == it->child(pos));

    //                } else {
    //                    _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
    //                }
    //            }

    //            _tree_screen->knowtree_save();
    //            // }
    //        }


    _result = view_paste_strategy(TreeModel::ModelIndex(view_source_model, view_source_model()->index(tree_screen->session_root_item()))
                                  , _result
                                  , std::bind([&](boost::intrusive_ptr<const TreeItem::linker> target, boost::intrusive_ptr<const TreeItem::linker> source)->bool {return target->host()->field("url") == source->host()->field("url");}, std::placeholders::_1, _result->up_linker())
                                  , _view_paste_strategy, item_is_brand_new, _find_url, check_euqal
                                 );
    assert(_result);

    if(_result->is_lite())_result->to_fat();

    //    //        //            else {
    //    //        //                //                assert(_record->is_registered());
    //    //        //                _record->binder(generator);
    //    //        //                _record->activator(activator);
    //    //        //                //                _record->generate();    // why?
    //    //        //            }

    //    _result->record_binder(_record_binder);
    //    //    _result->activator(activator);

    assert(_result != know_model_board->root_item());
    //    //    assert(_result->is_registered_to_record_controller_and_tabmanager());
    //    assert(_result->field("url") == _url.toString());   // maybe other url loaded !
    //    //    } // browser_pages

    //    //    }

    //    //    }

    if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

    if(_result->field("file") == "")_result->field("file", "text.html");

    //    //    assert(_record);
    assert(check_euqal(_result) || _result->field("url") == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);

    if(_result->field("url") == browser::Browser::_defaulthome && _find_url.toString() != browser::Browser::_defaulthome) {
        _result->field("url", _find_url.toString());
    }

    // assert(_result->fragment() == _url.fragment());
    return _result;
}

//        boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url);
//    template<typename url_type = url_full>
boost::intrusive_ptr<TreeItem> TreeScreen::item_bind(
    const QUrl &_find_url
    , const TreeScreen::paste_strategy &_view_paste_strategy
    , equal_url_t _equal
)
{
    boost::intrusive_ptr<TreeItem> re;
    auto it = item_register(_find_url, _view_paste_strategy, _equal);

    re = globalparameters.entrance()->item_bind(it);

    return re;

}


boost::intrusive_ptr<TreeItem> TreeScreen::item_register(
    boost::intrusive_ptr<TreeItem> target
    , const TreeScreen::paste_strategy &_view_paste_strategy
    , equal_t _equal
)
{

    boost::intrusive_ptr<TreeItem> _result(nullptr);    // =  _know_model_board->root_item();

    auto find_url = QUrl(target->field("url"));

    auto check_euqal = [&](boost::intrusive_ptr<const TreeItem> it) {return _equal(it, target);};

    TreeScreen *tree_screen    = globalparameters.tree_screen();
    //    auto _know_model_root = tree_screen->know_root();
    auto know_model_board      = tree_screen->know_model_board();

    //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
    auto view_source_model = [&]() {return tree_screen->tree_view()->source_model();}; // std::bind(_source_model_func, _tree_screen->tree_view());


    //        auto session_root_item = [&]() {

    //            auto internal_session_root_item  = [&]() {return view_source_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();});};

    //            while(!internal_session_root_item() && view_source_model()->root_item() != know_model_board->root_item()) {
    //                tree_screen->cursor_follow_up_one_level();
    //                tree_screen->session_root_id(view_source_model()->root_item()->id());
    //            }

    //            if(!know_model_board->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == tree_screen->session_root_id();})) {
    //                tree_screen->session_root_id(tree_screen->tree_view()->current_item()->id());
    //                tree_screen->know_model_save();
    //            }
    //            assert(internal_session_root_item());
    //            return internal_session_root_item(); // _session_view_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();});

    //        };   // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());


    //        auto _current_view_model    = _tree_screen->tree_view()->source_model();

    if(target->is_lite())target->to_fat();

    //    item->record_binder(_record_binder);
    //    //    item->activator(activator);

    //    //    item->is_registered_to_record_controller_and_tabmanager(false);

    if(!target->binder()) {

        //        if(item->is_lite())item->to_fat();

        //        item->binder(generator);
        //        item->activator(activator);
        //        item->is_registered_to_record_controller(true);



        //    TableController *_record_controller = globalparameters.table_screen()->table_controller();
        //    assert(_record_controller);



        //            QUrl find_url = QUrl(item->field("url"));

        //    boost::intrusive_ptr<TreeItem> _source_root_item = tree_screen->know_branch()->item(TreeModel::delegater(find_url));        // on know_root semantic    // item won't work, for it is not inside _know_model_root if it is come from _know_model_branch
        boost::intrusive_ptr<TreeItem> in_tree = know_model_board->item(check_euqal);

        //    if(_source_root_item && !_source_item) {
        //        auto result = tree_screen->cut_from_root(_source_root_item);

        //        if(result)_source_item = tree_screen->paste_to_branch(result, _know_model_branch);

        //        assert(result);
        //        assert(_source_item);
        //        assert((_source_item == result) && (result == _source_root_item));
        //    }

        bool item_is_brand_new = false;
        //    if(_record_controller) {
        //    auto browser_pages = this->_source_model->browser_pages();
        //    assert(browser_pages);

        //    if(_source_model->count() > 0) {
        auto v = globalparameters.entrance()->find(check_euqal);

        boost::intrusive_ptr<TreeItem> in_browser;

        if(v) {
            in_browser = v->page()->binder()->item();
            assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
            // assert(_result->fragment() == item->fragment());
        }

        if(in_tree && in_tree != know_model_board->root_item()) {

            if(!in_browser) {
                assert(check_euqal(in_tree));   // assert(_source_item->url() == item->url());
                // assert(item->fragment() == _source_item->fragment());

                if(target != in_tree) {
                    _result = in_tree; // assert(_item.get() == _source_item.get());
                    // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(item, _source_item);
                } else {
                    _result = in_tree;
                }

                //                //                record->binder(generator);
                //                //                record->activator(activator);
                if(_result->is_lite())_result->to_fat();

                //                _source_item->binder(generator);
                //                _source_item->activator(activator);

                //                //            _item = register_item_to_browser_source_model(_source_item);
                //                _source_item->is_registered_to_record_controller(true);
                //                // _source_item->self_bind();
                //                //                assert(_record);

                //                //                _record->active_immediately(active_immediately);
                //                //                _record->generator(generator);



            } else {
                assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
                assert(_equal(in_browser, in_tree));   // assert(_result->url() == _source_item->url());
                // assert(_result->fragment() == _source_item->fragment());

                //                if(in_browser != in_tree) {
                //                    _result = in_tree; // assert(_item.get() == _source_item.get());
                //                    // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, _source_item);
                //                }

                _result = in_tree;
                assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);
            }

            assert(!_result->is_lite());
            //            assert(_result->is_registered_to_browser());

        } else {

            item_is_brand_new = true;

            if(!in_browser) {


                //                if(item->is_lite())item->to_fat();

                if(target->field("id") == "")target->field("id", get_unical_id());

                //                item->binder(generator);
                //                item->activator(activator);

                //                //            _item = register_item_to_browser_source_model(item);
                //                item->is_registered_to_record_controller(true);
                //                // item->self_bind();
                _result = target; // assert(_item.get() == item.get());
            } else {
                assert(check_euqal(in_browser));   // assert(_result->url() == item->url());
                // assert(_result->fragment() == item->fragment());

                if(in_browser != target) {
                    _result = in_browser;   // target; // assert(_item.get() == _source_item.get());
                    // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, item);
                }

                if(_result->is_lite())_result->to_fat();

                if(_result->field("id") == "")_result->field("id", get_unical_id());

                assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

                if(_result->field("url") == browser::Browser::_defaulthome) {
                    _result->field("url",  target->field("url"));
                }

                //                _item->binder(generator);
                //                _item->activator(activator);
                //                _item->is_registered_to_record_controller(true);
                //                // _item->self_bind();
            }


        }

        //            {
        //                //                    auto _index = _tree_screen->tree_view()->current_index();

        //                // if(idx.isValid()) {
        //                _current_view_model = _tree_screen->tree_view()->source_model();

        //                auto session_root_item = _current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _tree_screen->session_root();}); // item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
        //                auto session_root_index = _tree_screen->tree_view()->source_model()->index(session_root_item); //current_index();
        //                assert(session_root_index.isValid());
        //                assert(session_root_item);
        //                assert(_current_view_model->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == session_root_item->id();}));

        //                //        if(_item->is_lite())_item->to_fat();

        //                if(item_is_brand_new) {
        //                    assert(_check_url(_result));   // assert(_result->url() == item->url());
        //                    // assert(_result->fragment() == item->fragment());
        //                    // int pos
        //                    _result = _view_paste_strategy(_current_view_model, session_root_index, _result); // it->insert_new_item(it->current_count() - 1, _result);

        //                    assert(_check_url(_result) || _result->field("url") == browser::Browser::_defaulthome || _find_url.toString() == browser::Browser::_defaulthome);

        //                    if(_result->field("url") == browser::Browser::_defaulthome) {
        //                        _result->field("url"
        //                                       , _find_url.toString()  // item->field("url")
        //                                      );
        //                    }

        //                    _tree_screen->synchronized(false);
        //                    // assert(_result == it->child(pos));
        //                } else if(session_root_item != _result) {
        //                    if(_result->is_ancestor_of(session_root_item)) {
        //                        if(_result != session_root_item->parent()) {
        //                            _result = _tree_screen->view_follow_direct(_result);
        //                        }

        //                        _tree_screen->session_root(_result->id());
        //                        _tree_screen->synchronized(false);
        //                    } else if(session_root_item != _result->parent()) { // if(session_root_item->is_ancestor_of(_result) && session_root_item != _result->parent())
        //                        _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
        //                        _tree_screen->synchronized(false);
        //                        // }
        //                        // else {
        //                        // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);
        //                    }

        //                    // _result = _tree_screen->view_paste_as_child(_current_view_model, session_root_index, _result);

        //                }


        //                _tree_screen->knowtree_save();
        //                // }
        //            }


        _result = view_paste_strategy(TreeModel::ModelIndex(view_source_model, view_source_model()->index(tree_screen->session_root_item()))
                                      , _result
                                      , std::bind([&](boost::intrusive_ptr<const TreeItem::linker> target, boost::intrusive_ptr<const TreeItem::linker> source)->bool {return target->host()->field("url") == source->host()->field("url");}, std::placeholders::_1, _result->up_linker())
                                      , _view_paste_strategy, item_is_brand_new, find_url, check_euqal
                                     );

        if(_result->is_lite())_result->to_fat();

        //        //            else {
        //        _item->binder(generator);
        //        _item->activator(activator);
        //        //                _record->generate();
        //        //            }

        assert(_result != know_model_board->root_item());

        //        assert(_result->is_registered_to_browser());
        assert(check_euqal(_result));   // assert(_result->url() == item->url());
        // assert(_result->fragment() == item->fragment());
        //    }

        //    }

        //    }

        //    assert(_record);
    } else {
        _result = target;
    }

    if(_result->is_lite())_result->to_fat();

    assert(_result != know_model_board->root_item());

    if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

    if(_result->field("file") == "")_result->field("file", "text.html");

    assert(_result->field("url") == target->field("url") || _result->field("url") == browser::Browser::_defaulthome || find_url.toString() == browser::Browser::_defaulthome);


    if((_result->field("url") == browser::Browser::_defaulthome) && (target->field("url") != browser::Browser::_defaulthome)) {
        _result->field("url", target->field("url"));
    }

    // assert(_result->url() == item->url());
    // assert(_result->fragment() == item->fragment());
    return _result;
}

boost::intrusive_ptr<TreeItem> TreeScreen::item_bind(
    boost::intrusive_ptr<TreeItem> target
    , const TreeScreen::paste_strategy &_view_paste_strategy
    , equal_t _equal
)
{
    boost::intrusive_ptr<TreeItem> re;

    if(!_tree_view->source_model()->index(target).isValid()) {
        re = item_register(target, _view_paste_strategy, _equal);
    } else
        re = target;

    assert(re->field("url") == target->field("url"));
    //        if( // !it->is_registered_to_browser() &&
    //            !it->record_binder())
    re = globalparameters.entrance()->item_bind(re);
    assert(re->field("url") == target->field("url"));
    //        else
    //            re = it;

    return re;
    //        auto ar = boost::make_shared<TabWidget::Coupler>(this, item, true);

    //        //        bind_helper binder = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<TreeItem> // , boost::intrusive_ptr<TreeItem>(TreeItem::*)(WebPage *)
    //        //                             >>("", &TabWidget::RecordBinder::binder, ar);
    //        //        active_helper activator = std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *>>("", &TabWidget::RecordBinder::activator, ar);


    //        return _record_controller->item_request_from_tree(item
    //                                                          , std::make_shared<CouplerDelegation>(
    //                                                              std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, ar)
    //                                                              , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, ar)
    //                                                              , std::make_shared<bind_interface>("", &Coupler::binder, ar)
    //                                                              , std::make_shared<activate_interface> ("", &Coupler::activator, ar)
    //                                                          ));
}






