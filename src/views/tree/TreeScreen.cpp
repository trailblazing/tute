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


know_root_holder::know_root_holder(const AppConfig &_appconfig, TreeScreen *_this)
    : _know_root(new KnowModel(_this))
{
    // Загрузка данных
    // auto xml =
    _know_root->init_from_xml(_appconfig.get_tetradir() + "/" + globalparameters.main_program_file() + ".xml");
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
    , _tree_view(new KnowView(this))
    , _tools_layout(new QHBoxLayout())
      //    , _recordtree_search(new browser::ToolbarSearch(this))
      //    , _recordtree_searchlayout(new QHBoxLayout())
    , _treescreenlayout(new QVBoxLayout())
    , _appconfig(_appconfig)
      //    , _shadow_branch(new TreeModelKnow(this))
{
    _know_model_board->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");    // _know_branch->intercept(know_root_holder::know_root()->root_item());    // init_from_xml(xml);  //
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
    setup_model(_know_model_board->root_item());
    setup_signals();
    assembly();
}


TreeScreen::~TreeScreen()
{
    //appconfig.treescreensize(QSize(this->width(), this->height()));


    //    QToolBar        *_menubar; // QMenuBar *_menubar;
    //    QPushButton     *_menubutton;
    //    QWidgetAction   *_menuaction;
    //    QMenu           *_menus_in_button;

    if(_menus_in_button) {delete _menus_in_button; _menus_in_button = nullptr;}

    if(_context_menu) {delete _context_menu; _context_menu = nullptr;}

    //    if(_menuaction) {delete _menuaction; _menuaction = nullptr;}

    //    if(_menubutton) {delete _menubutton; _menubutton = nullptr;}

    if(_menubar) {delete _menubar; _menubar = nullptr;}

    //    delete _recordtree_search;
    if(_tools_line) {delete _tools_line; _tools_line = nullptr;}

    //    delete _treescreenlayout;
    //    delete _toolslayout;
    delete _tree_view;
    //    delete _shadow_branch;
    delete _know_model_board;

}


void TreeScreen::setup_actions(void)
{
    QAction *ac;

    ac = new QAction(tr("View up one level"), this);
    ac->setStatusTip(tr("View up one level"));
    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::cursor_up_one_level);
    _actionlist["view_up_one_level"] = ac;

    ac = new QAction(tr("Return to root"), this);
    ac->setStatusTip(tr("Return to root"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::cursor_up_to_root);
    _actionlist["return_to_root"] = ac;

    // Разворачивание всех подветок
    // a->setShortcut(tr("Ctrl+X"));
    ac = new QAction(tr("Expand all sub items"), this);
    ac->setStatusTip(tr("Expand all sub items"));
    ac->setIcon(QIcon(":/resource/pic/expand_all_subbranch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_expand_all_sub);
    _actionlist["expandAllSubbranch"] = ac;

    // Сворачивание всех подветок
    ac = new QAction(tr("Collapse all sub items"), this);
    ac->setStatusTip(tr("Collapse all sub items"));
    ac->setIcon(QIcon(":/resource/pic/collapse_all_subbranch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_collapse_all_sub);
    _actionlist["collapseAllSubbranch"] = ac;

    // Перемещение ветки вверх
    ac = new QAction(tr("Move item up"), this);
    ac->setStatusTip(tr("Move item up"));
    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(ac, &QAction::triggered, this
    , [&]() {item_move_up_dn_branch(&TreeItem::move_up);}   // &TreeScreen::item_move_up_branch
           );
    _actionlist["moveUpBranch"] = ac;

    // Перемещение ветки вниз
    ac = new QAction(tr("Move item down"), this);
    ac->setStatusTip(tr("Move item down"));
    ac->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(ac, &QAction::triggered, this
    , [&]() {item_move_up_dn_branch(&TreeItem::move_dn);}   // &TreeScreen::item_move_dn_branch
           );
    _actionlist["moveDnBranch"] = ac;

    // Вставка новой подветки
    ac = new QAction(tr("Insert a new sub item"), this);
    ac->setStatusTip(tr("Insert a new sub item into selected"));
    ac->setIcon(QIcon(":/resource/pic/add_subbranch.svg"));
    connect(ac, &QAction::triggered, this
    , [&]() {
        branch_insert_new(std::bind(&TreeScreen::branch_add_new, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](KnowModel * _current_model, QModelIndex _current_index, QString _id, QString _name) {
            boost::intrusive_ptr<TreeItem> result;
            auto parent = _current_model->item(_current_index)->parent();
            assert(parent);
            auto same = parent->child(parent->is_name_exists(_name));

            if(same) {  // && same->is_empty()
                assert(same->name() == _name);
                result = same;
                //            children_transfer(_new_item, _current_model);
            } else {
                // Вставка новых данных в модель дерева записей
                result = _current_model->lock_sibling_add(_current_index, _id, _name);
            }

            return result;
        });
    }
           );
    _actionlist["insSubbranch"] = ac;

    // Вставка новой ветки
    ac = new QAction(tr("Insert a new sibling item"), this);
    ac->setStatusTip(tr("Insert a new sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/add_branch.svg"));
    connect(ac, &QAction::triggered, this
    , [&]() {
        branch_insert_new(std::bind(&TreeScreen::branch_add_new, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](KnowModel * _current_model, QModelIndex _current_index, QString _id, QString _name) {
            return _current_model->lock_child_add_new(_current_index, _id, _name);
        });
    }
           );
    _actionlist["insBranch"] = ac;

    // Редактирование ветки
    ac = new QAction(tr("Edit item name"), this);
    ac->setStatusTip(tr("Edit name of selected item"));
    ac->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_edit);
    _actionlist["editBranch"] = ac;

    // Удаление ветки
    ac = new QAction(tr("Delete item"), this);
    ac->setStatusTip(tr("Delete selected item and all sub items"));
    ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(ac, &QAction::triggered, this
    , [this](bool checked = false) {Q_UNUSED(checked); branchs_delete("cut");} //, &TreeScreen::del_branch
           );
    _actionlist["delBranch"] = ac;

    void(TreeScreen::*_cut)(bool) = &TreeScreen::branch_cut;
    // Удаление ветки с сохранением копии в буфер обмена
    ac = new QAction(tr("Cut item"), this);
    ac->setStatusTip(tr("Cut item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
    connect(ac, &QAction::triggered, this, _cut);
    _actionlist["cutBranch"] = ac;

    // Копирование ветки в буфер обмена
    ac = new QAction(tr("Copy item"), this);
    ac->setStatusTip(tr("Copy item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_copy);
    _actionlist["copyBranch"] = ac;

    //    void(TreeScreen::*paste)() = &TreeScreen::branch_paste;
    // Вставка ветки из буфера обмена
    ac = new QAction(tr("Paste item"), this);
    ac->setStatusTip(tr("Paste sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, [&](bool checked) {Q_UNUSED(checked); branch_paste_func(std::bind(&KnowModel::lock_sibling_paste, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));}); //, paste

    _actionlist["pasteBranch"] = ac;


    //    QString(KnowModel::*_lock_child_paste)(const QModelIndex & _index, ClipboardBranch * subbranch) = &KnowModel::lock_child_paste;
    // Вставка ветки из буфера обмена в виде подветки
    ac = new QAction(tr("Paste as sub item"), this);
    ac->setStatusTip(tr("Paste item as sub item for selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, [&](bool checked) {Q_UNUSED(checked); branch_paste_func(std::bind(&KnowModel::lock_child_paste, _tree_view->source_model(), std::placeholders::_1, std::placeholders::_2));}); // branch_paste_sub

    _actionlist["pasteSubbranch"] = ac;

    // Шифрование ветки
    ac = new QAction(tr("Encrypt item"), this);
    ac->setStatusTip(tr("Encrypt item and all subitem"));
    // actionEncryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_encrypt);
    _actionlist["encryptBranch"] = ac;

    // Расшифровка ветки (снятие пароля)
    ac = new QAction(tr("Decrypt item"), this);
    ac->setStatusTip(tr("Decrypt item and all subitem"));
    // actionDecryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::branch_decrypt);
    _actionlist["decryptBranch"] = ac;

    ac = new QAction(tr("Pin / freeze browser view"), this);
    ac->setStatusTip(tr("Pin / freeze browser view"));  // _pin->setStatusTip(tr("Pin a note"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg")); // _pin->setIcon(QIcon(":/resource/pic/pin.svg"));
    connect(ac, &QAction::triggered
    , [&](bool checked = false) {
        Q_UNUSED(checked)
        // Обновление инфополей в области редактирования записи
        MetaEditor *metaeditor = globalparameters.meta_editor();  //MetaEditor *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);

        if(metaeditor)metaeditor->switch_pin();
    }
           );
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

    QAction *_edit_field = (new QAction(tr("Edit properties (pin, name, author, url, tags...)"), this));
    _edit_field->setStatusTip(tr("Edit note properties (pin, name, author, url, tags...)"));
    _edit_field->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(_edit_field, &QAction::triggered, this, [&](bool checked = false) {Q_UNUSED(checked); edit_field_context(this->_tree_view->index_current());});
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
    auto item = _tree_view->source_model()->item(_tree_view->index_current());

    for(QMap<QString, QString>::iterator i = edit_data.begin(); i != edit_data.end(); i++) {
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
    knowtree_save();
}

void TreeScreen::setup_ui(QMenu *_filemenu, QMenu *_toolsmenu)
{
    //_toolsline = new QToolBar(this);

    /*
    QSize tool_bar_icon_size(16,16);
    toolsLine->setIconSize(tool_bar_icon_size);
    */

    //    _recordtree_searchlayout = new QHBoxLayout();
    //    _recordtree_searchlayout->addWidget(_recordtree_search);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["view_up_one_level"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["return_to_root"]);

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["insBranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["insSubbranch"]);

    if(appconfig.getInterfaceMode() == "desktop") {
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["editBranch"]);
        insert_action_as_button<QToolButton>(_tools_line, _actionlist["delBranch"]);
    }

    _tools_line->addSeparator();

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["expandAllSubbranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["collapseAllSubbranch"]);

    _tools_line->addSeparator();

    insert_action_as_button<QToolButton>(_tools_line, _actionlist["moveUpBranch"]);
    insert_action_as_button<QToolButton>(_tools_line, _actionlist["moveDnBranch"]);

    if(appconfig.getInterfaceMode() == "mobile") {
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

    _menus_in_button->addMenu(
        _filemenu   // globalparameters.mainwindow()->filemenu()
    );
    _menus_in_button->addMenu(
        _toolsmenu   // globalparameters.mainwindow()->toolsmenu()
    );
    //_buttonmenu->addAction(_menuaction);

    _menubutton
    //_menuaction
    ->setMenu(_menus_in_button);    // Ownership of the menu is not transferred to the push button

    //connect(_menuaction, &QWidgetAction::triggered, _menubar, &QMenuBar::show);
    //connect(menubutton, &QPushButton::clicked, _menubar, &QMenuBar::show);

    _menuaction->setDefaultWidget(  //_menubar//
        //_buttonmenu//
        _menubutton // The ownership is transferred to QWidgetAction!!!
    );

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
    _tree_view->setObjectName(knowtreeview_singleton_name);
    _tree_view->setMinimumSize(1, 1);     // 150, 250

    _tree_view->setWordWrap(true);

    // Временно сделан одинарный режим выбора пунктов
    // todo: Множественный режим надо выставить тогда, когда
    // станет ясно, как удалять несколько произвольных веток так, чтобы
    // в процессе удаления QModelIndex нижестоящих еще не удаленных
    // веток не менялся
    // Time to do single mode select items
    // todo: Multi Mode should be exposed when
    // it will be clear how to remove some arbitrary branches so that the
    // in the removal of the lower QModelIndex not yet deleted
    // branches has not changed
    // knowTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _tree_view->setSelectionMode(QAbstractItemView::    //SingleSelection //
                                 ExtendedSelection  // MultiSelection  //
                                );

    _tree_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded   // ScrollBarAlwaysOn
                                            );

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    _tree_view->setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    _tree_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //    QSize size = appconfig.treescreensize();
    //    resize(size);
}

void TreeScreen::setup_model(boost::intrusive_ptr<TreeItem> _item)
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
//    //    _treemodelknow->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");
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

// Открытие контекстного меню в дереве разделов
void TreeScreen::on_custom_contextmenu_requested(const QPoint &pos)
{
    qDebug() << "In TreeScreen::on_customContextMenuRequested";

    //    // Конструирование меню
    //    QMenu _context_menu(this);

    _context_menu->addAction(_actionlist["freeze_browser_view"]);
    _context_menu->addAction(_actionlist["find_in_base"]);
    _context_menu->addAction(_actionlist["edit_field"]);
    _context_menu->addAction(_actionlist["editor_switch"]);

    _context_menu->addAction(_actionlist["view_up_one_level"]);
    _context_menu->addAction(_actionlist["return_to_root"]);

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

    // Получение индекса выделенной ветки
    QModelIndex index = _tree_view->index_current();

    if(index.isValid()) {
        // Выясняется, зашифрована ли ветка или нет
        QString cryptFlag = _tree_view->source_model()->item(index)->field("crypt");

        // Выясняется, зашифрована ли родительская ветка
        QString parentCryptFlag = _tree_view->source_model()->item(index)->parent()->field("crypt");

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
    _context_menu->exec(_tree_view->viewport()->mapToGlobal(pos));
}


void TreeScreen::setup_signals(void)
{
    //    connect(_recordtree_search, &browser::ToolbarSearch::textChanged, globalparameters.getFindScreen(), &FindScreen::enableFindButton);
    //    connect(_recordtree_search, &browser::ToolbarSearch::returnPressed, globalparameters.getFindScreen(), &FindScreen::findClicked);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
    connect(_tree_view, &KnowView::customContextMenuRequested, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(_tree_view, &KnowView::tapAndHoldGestureFinished, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if(appconfig.getInterfaceMode() == "desktop")
        connect(_tree_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TreeScreen::index_invoke_in_current_branch);   // &TreeScreen::step_into_sub_branch


    if(appconfig.getInterfaceMode() == "mobile")
        connect(_tree_view, &KnowView::clicked, this, &TreeScreen::branch_step_into);

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)), actionList["editBranch"], SLOT(trigger(void)));
    if(appconfig.getInterfaceMode() == "desktop") {
        connect(_tree_view, &KnowView::clicked, this, &TreeScreen::index_invoke_in_current_branch);
        //                [&](const QModelIndex index) {
        //            //            _tree_view->collapse(index);
        //            //            _tree_view->expand(index);
        //            (_tree_view->isExpanded(index)) ? _tree_view->setExpanded(index, false) : _tree_view->setExpanded(index, true);
        //            auto result_item = _tree_view->source_model()->item(index);

        //            if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        //        }//&TreeScreen::candidate_from_knowtree_item)

        connect(_tree_view, &KnowView::doubleClicked, this, &TreeScreen::index_invoke_in_sub_branch);
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


void TreeScreen::branch_expand_all_sub(void)
{
    // Получение индексов выделенных строк
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), true);
}


void TreeScreen::branch_collapse_all_sub(void)
{
    // Получение индексов выделенных строк
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), false);
}


void TreeScreen::expand_or_collapse_recurse(QModelIndex index, bool mode)
{
    _tree_view->setExpanded(index, mode);

    int i = 0;

    while((index.child(i, 0)).isValid()) {
        expand_or_collapse_recurse(index.child(i, 0), mode);
        i++;
    }

}


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
    QModelIndex index = _tree_view->index_current();

    if(index.isValid()) {
        // Ветка перемещается
        QModelIndex index_after_move;

        //        if(direction == 1)
        //            index_after_move = _tree_view->source_model()->branch_move_up(index);
        //        else
        //            index_after_move = _tree_view->source_model()->branch_move_dn(index);

        index_after_move = _tree_view->source_model()->branch_move_up_dn(index, _move);

        _know_model_board->synchronized(false);

        // Установка курсора на позицию, куда была перенесена ветка
        if(index_after_move.isValid() && index_after_move != index) {
            _tree_view->selectionModel()->select(index_after_move, current_tree_selection_mode);    // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
            _tree_view->selectionModel()->setCurrentIndex(index_after_move, current_tree_selection_mode);   //  | QItemSelectionModel::Current

        }

        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)
        globalparameters.tree_screen()->knowtree_save();
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


// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
// Helper function is used when you add branches
// Eli is branch = true, the branch is added to the same level
// If is_branch = false, then added podvetka
//template<bool insert_sibling_branch>
boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_new(
    std::function < boost::intrusive_ptr<TreeItem> (KnowModel *, QModelIndex, QString
                                                    , std::function < boost::intrusive_ptr<TreeItem>(KnowModel *, QModelIndex, QString, QString) >) > _branch_add
    , std::function < boost::intrusive_ptr<TreeItem>(KnowModel *, QModelIndex, QString, QString) > _branch_add_impl
)     // bool insert_sibling_branch
{

    boost::intrusive_ptr<TreeItem> result(nullptr);
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        //        QMessageBox messageBox(this);
        //        messageBox.setWindowTitle(tr("Unavailable action"));
        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling insert operation."));
        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //        messageBox.exec();
        //        //        current_index = selectitems.first();
        //        return nullptr;
        _current_index = selectitems.first();
    } else {

        //    else if(selectitems.size() == 0) {
        //        current_index = last_index();
        //    } else {
        _current_index = _tree_view->index_current();
    }

    if(_current_index.isValid()) {
        //        //    }

        //        //    if(!_current_index.isValid()) {
        //        //        _tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
        //        //        assert(_current_index.isValid());
        //        //    }

        //        // Создается окно ввода данных
        //        bool ok;
        //        QString title, text;

        //        if(insert_sibling_branch) {
        //            title = tr("Create new item");
        //            text = tr("Item name:");
        //        } else {
        //            title = tr("Create new sub item");
        //            text = tr("Sub item name:");
        //        }

        //        QString name = QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);

        //        if(!(ok && !name.isEmpty())) return nullptr;    // Если была нажата отмена

        //        // Получение индекса выделенной строки
        //        //    QModelIndex
        //        //    current_index = currentitem_index();

        result = _branch_add(_tree_view->source_model(), _current_index, _tree_view->source_model()->item(_current_index)->name(), _branch_add_impl);   // nonsense
        // , insert_sibling_branch

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

std::vector<boost::intrusive_ptr<TreeItem>> TreeScreen::branch_paste_new_children_only(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _blank_root)
{
    boost::intrusive_ptr<TreeItem>  result(nullptr);
    std::vector<boost::intrusive_ptr<TreeItem>> results;

    if(_blank_root->count_direct() > 0) {
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
            int count_blank = -1;
            auto begin_item = _blank_root->child(0);;

            int count_board = 0;
            _current_model->beginInsertRows(_current_index.parent()
                                            , _current_index.row()   // parent->count_direct()
                                            , _current_index.row()   // parent->count_direct()
                                           );

            // Вставка данных и установка курсора
            while((count_blank = _blank_root->count_direct()) != count_board) {
                //            for(int index = 0; index < _blank_root->direct_count(); index++) {
                auto candidate = _blank_root->child(0);

                // Подветка
                if(!_know_model_board->is_id_exists(candidate->id())) {
                    // Вставка новых данных в модель дерева записей
                    results.push_back(
                        branch_paste(_current_model, _current_index, candidate)  // _current_model->add_child_item(_current_model->root_item(), new_branch_root->child(index))
                    );

                    if(_blank_root->is_id_exists(candidate->id())) {
                        _blank_root->child_remove(candidate);
                        assert(count_blank > _blank_root->count_direct());
                    }

                } else {
                    count_board++;
                }

                //                assert(_know_model_board->is_item_exists(candidate->id()));





                //            // Установка курсора на только что созданную позицию
                //            //        QModelIndex
                //            setto = _current_model->index_child(_current_index, current_item->current_count() - 1);
                //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

                //            // А можно было установить курсор на нужную позицию и так
                //            // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                //            //                                             QItemSelectionModel::ClearAndSelect);
            }

            _current_model->endInsertRows();
            // Установка курсора на только что созданную позицию
            //        QModelIndex
            setto = _current_model->index(TreeModel::delegater(begin_item->id()));
            // index_child(_current_index.parent(), _current_index.row() + 1);   // current_item->current_count() - 1

            assert(setto.isValid());
            _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
            _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect

            // Сохранение дерева веток
            //    find_object<TreeScreen>(tree_screen_singleton_name)->
            knowtree_save();

            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->setEnabled(true);
            //        return
            result = _current_model->item(setto);

            assert(result);
            //            assert(results[0] == result);

            //    } else if(_current_model->root_item()->id() == it->id()) {



            //    }

            //    }
        }
    }

    return results;
}

void TreeScreen::branch_paste_from_search(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _result_item)
{
    if(_result_item->count_direct() > 0) {

        //        if(!_result_item->child(0)->unique_page()) {
        //            globalparameters.entrance()->new_browser(_result_item->child(0)->field("url"));
        //        }

        //        RecordController *_record_controller = // resultset_item->unique_page()
        //            _result_item->child(0)->unique_page()->view()->record_controller();   // globalparameters.record_screens()->record_controller();
        //        RecordModel *controller_source_model = _record_controller->source_model();

        //        //        setup_model(_know_branch);

        _actionlist["return_to_root"]->setEnabled(true);

        //        //    assert(controller_source_model->tree_item() == _selected_branch->_root_item);

        //        controller_source_model->reset_internal_data();  // delete source_model;   // dangerous!!!

        //        //    std::shared_ptr<RecordTable> target_data = source_model->tree_item()->tabledata();    // resultset_item->tabledata();
        //        //    controller->init();




        //        //    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
        //        //    auto dommodel = resultset_item->export_to_dom();    // source->init(startItem, QDomElement());
        //        //    resultset_item->remove_all_children();              // dommodel has noting to do with children!

        //        //    auto result_data = std::make_shared<RecordTable>(dommodel); //    resultset_item->tabledata(dommodel);               //    resultset_item->table_clear();    // already insert into table_init

        //        //    //    resultset_data->init(resultset_item, dommodel);     // just for init the item bind for reault set record table

        //        //    //    _knowtreemodel->save();

        //        //        controller_source_model->init_source_model(resultset_item   //? should we put it to treemodelknow? not the source of browser_pages
        //        //                                                   , _record_controller
        //        //                                                   , _record_controller->record_screen()
        //        //                                                   , globalparameters.mainwindow()
        //        //                                                   , globalparameters.meta_editor()
        //        //                                                   // , globalparameters.record_screens()
        //        //                                                  );



        auto it = branch_paste(_current_model, _current_index, _result_item); // setup_model(_result_item);   // _know_branch->intercept(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr, resultset_item)));
        auto idx = _tree_view->source_model()->index(it);
        _tree_view->selection_to_pos(idx.row());


        //    controller_source_model->root(resultset_item); // ->record_table(result_data);

        //    for(int i = 0; i < resultset_item->tabledata()->size(); i++) {

        //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
        //            _shadow_candidate_model->_root_item->tabledata()->size()
        //            , resultset_item->tabledata()->record(i)
        //        );
        //    }





        //    reset_tabledata(std::make_shared<RecordTable>(resultset_item, dommodel));

        //    controller->init();

        //    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
        //    if(appconfig.getInterfaceMode() == "mobile") {

        //        QStringList path = item->getPathAsName();

        //        // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
        //        int emptyStringIndex = path.indexOf("");
        //        path.removeAt(emptyStringIndex);

        //        find_object<RecordTableScreen>(table_screen_singleton_name)->setTreePath(path.join(" > "));

        //    }



        // Width of the tree column set as to always accommodates data  // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        _tree_view->resizeColumnToContents(0);

        // Переключаются окна (используется для мобильного интерфейса)
        //    globalparameters.window_switcher()->switchFromTreeToRecordtable();
        //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());

    }
}

boost::intrusive_ptr<TreeItem> TreeScreen::branch_paste(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _new_item)
{
    boost::intrusive_ptr<TreeItem>  result(nullptr);

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

        if(_new_item->id() == "") {
            // Получение уникального идентификатора
            QString id = get_unical_id();
            _new_item->field("id", id);
        }

        QModelIndex setto;

        // Вставка данных и установка курсора

        // Одноранговая ветка
        //        if(insert_sibling_branch) {
        auto parent = _current_model->item(_current_index)->parent();
        assert(parent);

        //            auto check = _current_model->item(_current_index);
        //            assert(_current_model->item_by_id(check->id()));
        //            assert(_current_model->item_by_name(check->name()));
        //            assert(_know_model_board->item_by_name(check->name()));
        //            assert(_know_model_board->item_by_id(check->id()));


        auto same = parent->child(parent->is_name_exists(_new_item->name()));

        if(same) {  // && same->is_empty()
            assert(same->name() == _new_item->name());
            result = same;
            branch_paste_new_children_only(_current_model, _current_index, _new_item);
            assert(result != _know_model_board->root_item());
        } else {
            // Вставка новых данных в модель дерева записей
            result = _current_model->lock_sibling_move(_current_index, _new_item);
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
        assert(_current_model->is_id_exists(result->id()));
        setto = _current_model->index(result);    //(current_item->parent()->current_count() - 1, 0, _current_index.parent());
        assert(setto.isValid());
        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect
        //        }


        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        knowtree_save();

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
        //        return
        result = _current_model->item(setto);
        assert((_new_item == result) || (_new_item->name() == result->name()));
        //    } else if(_current_model->root_item()->id() == it->id()) {

        //        assert(new_branch_root == result);  // not must, if you already have it

        //    }

        //    }
    }

    return result;
}

boost::intrusive_ptr<TreeItem> TreeScreen::branch_paste_to_child(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _new_item)
{
    boost::intrusive_ptr<TreeItem>  result(nullptr);

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

        if(_new_item->id() == "") {
            // Получение уникального идентификатора
            QString id = get_unical_id();
            _new_item->field("id", id);
        }

        QModelIndex setto;

        // Вставка данных и установка курсора

        // Одноранговая ветка

        // Подветка

        // Вставка новых данных в модель дерева записей
        result = _current_model->lock_child_move(_current_index, _new_item);
        assert(result);
        assert(result != _know_model_board->root_item());
        // Установка курсора на только что созданную позицию
        //        QModelIndex
        assert(_current_model->is_id_exists(result->id()));
        setto = _current_model->index(result);  //index_child(_current_index, current_item->current_count() - 1);
        assert(setto.isValid());
        _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
        _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect

        // А можно было установить курсор на нужную позицию и так
        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
        //                                             QItemSelectionModel::ClearAndSelect);


        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        knowtree_save();

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
        //        return
        result = _current_model->item(setto);
        assert((_new_item == result) || (_new_item->name() == result->name()));
        //    } else if(_current_model->root_item()->id() == it->id()) {

        //        assert(new_branch_root == result);  // not must, if you already have it

        //    }

        //    }
    }

    return result;
}

//template<>
boost::intrusive_ptr<TreeItem> TreeScreen::branch_add_new(KnowModel *_current_model, QModelIndex _current_index, QString _name
                                                          , std::function < boost::intrusive_ptr<TreeItem>(KnowModel *, QModelIndex, QString, QString) > _branch_add_impl)
{
    boost::intrusive_ptr<TreeItem> result(nullptr);
    // Получение ссылки на узел, который соответствует выделенной строке
    //    auto item = _current_model->item(_current_index);

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);

    // Получение уникального идентификатора
    QString id = get_unical_id();
    QModelIndex setto;

    // Вставка данных и установка курсора

    // Одноранговая ветка

    // Подветка

    // Вставка новых данных в модель дерева записей
    result = _branch_add_impl(_current_model, _current_index, id, _name);    // _current_model->lock_child_add(_current_index, id, _name);

    // Установка курсора на только что созданную позицию
    //        QModelIndex
    setto = _current_model->index(result);  //index_child(_current_index, item->current_count() - 1);
    _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
    _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect

    // А можно было установить курсор на нужную позицию и так
    // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
    //                                             QItemSelectionModel::ClearAndSelect);


    _know_model_board->synchronized(false);
    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    knowtree_save();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    assert(result->name() == _name);
    assert(result == _current_model->item(setto));
    return  result; // _current_model->item(setto);
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


void TreeScreen::branch_edit(void)
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
        _current_index = _tree_view->index_current();
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
        knowtree_save();

        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }
}


QList<boost::intrusive_ptr<TreeItem>> TreeScreen::branchs_delete(QModelIndexList _index_list , QString mode, bool _cut_branch_confirm)
{
    QList<boost::intrusive_ptr<TreeItem>> _result;
    auto _source_model = _tree_view->source_model();
    // На время удаления блокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->blockSignals(true);

    // Список имен веток, которые нужно удалить
    QStringList branches_name;

    for(int i = 0; i < _index_list.size(); ++i) {
        QModelIndex index = _index_list.at(i);
        auto it = _source_model->item(index);
        branches_name << it->field("name");
    }

    std::function<bool()> has_encrypt = [&]() {
        bool disableFlag = false;

        // Перебираются удаляемые ветки
        for(int i = 0; i < _index_list.size(); ++i) {
            QModelIndex index = _index_list.at(i);
            auto it = _source_model->item(index);

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

            if(disableFlag)
                break;

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

    // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
    if(!((globalparameters.crypt_key().size() == 0)
         && has_encrypt())
      ) {
        //        bool disableFlag = false;

        //        // Перебираются удаляемые ветки
        //        for(int i = 0; i < selectitems.size(); ++i) {
        //            QModelIndex index = selectitems.at(i);
        //            auto item = _tree_view->source_model()->item(index);

        //            // Если у ветки установлен флаг шифрования
        //            if(item->field("crypt") == "1") {
        //                disableFlag = true;
        //                break;
        //            }

        //            // Проверяется наличие флага шифрования у всех подветок
        //            QList<QStringList> cryptFlagsList = item->all_children_path_as_field("crypt");

        //            foreach(QStringList cryptFlags, cryptFlagsList)
        //                if(cryptFlags.contains("1")) {
        //                    disableFlag = true;
        //                    break;
        //                }

        //            if(disableFlag)
        //                break;

        //        } // Закрылся цикл перебора всех выделенных для удаления веток


        //        // Если в какой-то ветке обнаружено шифрование
        //        if(disableFlag) {
        //            QMessageBox messageBox(this);
        //            messageBox.setWindowTitle(tr("Unavailable action"));
        //            messageBox.setText(tr("In your selected data found closed item. Action canceled."));
        //            messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //            messageBox.exec();

        //            // Разблокируется главное окно
        //            //            find_object<MainWindow>("mainwindow")
        //            globalparameters.mainwindow()->setEnabled(true);
        //            //            find_object<MainWindow>("mainwindow")
        //            globalparameters.mainwindow()->blockSignals(false);

        //            return _result;
        //        }




        // Создается окно с вопросом, нужно удалять ветки или нет
        QString title, text, del_button;
        bool enable_question = true;

        if(mode == "delete") {
            title = tr("Delete item(s)");
            text = tr("Are you sure you wish to delete item(s) <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
            del_button = tr("Delete");

            enable_question = true;
        } else if(mode == "cut") {
            title = tr("Cut item");
            text = tr("Are you sure you wish to cut item <b>") + branches_name.join(", ") + tr("</b> and all sub items?");
            del_button = tr("Cut");

            if(appconfig.cut_branch_confirm() && _cut_branch_confirm) enable_question = true;
            else enable_question = false;
        }

        bool enable_del = true;

        if(enable_question) {
            QMessageBox messageBox(this);
            messageBox.setWindowTitle(title);
            messageBox.setText(text);
            messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            QAbstractButton *deleteButton = messageBox.addButton(del_button, QMessageBox::AcceptRole);
            messageBox.exec();

            if(messageBox.clickedButton() == deleteButton) enable_del = true;
            else enable_del = false;
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
            for(int i = 0; i < _index_list.size(); i++) {
                for(int j = _index_list.size() - 1; j > i; j--) {
                    QStringList path_1 = (_source_model->item(_index_list.at(j - 1)))->path_absolute();
                    QStringList path_2 = (_source_model->item(_index_list.at(j)))->path_absolute();

                    if(path_1.size() < path_2.size())
                        _index_list.swap(j - 1, j);
                }
            }

            qDebug() << "Path for delete";

            for(int i = 0; i < _index_list.size(); ++i)
                qDebug() << (_source_model->item(_index_list.at(i)))->path_absolute();

            // auto _source_model = _tree_view->source_model();

            // Вызов удаления веток
            for(int i = 0; i < _index_list.size(); ++i) {
                _result.append(branch_delete_one(_index_list.at(i), _source_model));
                branch_update_on_screen(_index_list.at(i).parent());
            }

            qDebug() << "Delete finish";

            //        update_model(_current_know_branch);

            // Сохранение дерева веток
            //        find_object<TreeScreen>(tree_screen_singleton_name)->
            knowtree_save();
            //        reload_knowtree();
            qDebug() << "Save new tree finish";

            //            _actionlist["pasteBranch"]->setEnabled(false);
            //            _actionlist["pasteSubbranch"]->setEnabled(false);
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

    } // Закрылось условие что системный пароль не установлен

    return _result;
}



// Удаление выбранных веток, вызывается при выборе соотвествущей
// кнопки или пункта меню
// Delete the selected branches, called when the corresponding
// button or menu item
QList<boost::intrusive_ptr<TreeItem>> TreeScreen::branchs_delete(QString mode, bool _cut_branch_confirm)
{
    qDebug() << "In del_branch()";

    // На время удаления блокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->blockSignals(true);


    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList _index_list = _tree_view->selectionModel()->selectedIndexes();

    if(_index_list.size() > 0) {
        auto _current_know_branch = _tree_view->source_model();
        _current_know_branch->synchronized(false);
    }

    return branchs_delete(_index_list, mode, _cut_branch_confirm);

}



// Удаление одной ветки и её подветок
// Delete one branch and its podvetok / garter
boost::intrusive_ptr<TreeItem> TreeScreen::branch_delete_one(QModelIndex index, KnowModel *_current_know_branch)
{

    boost::intrusive_ptr<TreeItem> item_to_be_deleted(nullptr);

    std::function<void(boost::intrusive_ptr<TreeItem>)> close_tab_recursive = [&](boost::intrusive_ptr<TreeItem> it) {
        if(it->is_registered_to_browser())   // item_to_be_deleted->unique_page()
            it->bounded_page()->record_controller()->remove_child(it->id()); // (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

        if(it->count_direct() > 0) {
            for(int i = 0; i < it->count_direct(); i++) {
                close_tab_recursive(it->child(i));
            }
        }
    };

    if(index.isValid()) {

        qDebug() << "Label 1";

        // Получение узла, который соответствует обрабатываемому индексу
        item_to_be_deleted = _current_know_branch->item(index);

        if(index.row() > 0 && _current_know_branch->root_item()->count_direct() > 0) {

        }

        int deleted_position = item_to_be_deleted->parent()->position_current(item_to_be_deleted);

        qDebug() << "Label 2, branch id:" << item_to_be_deleted->field("id") << "name:" << item_to_be_deleted->field("name");

        // Получение пути к элементу
        QStringList path = item_to_be_deleted->path_absolute();

        qDebug() << "Label 3";

        // Получение путей ко всем подветкам
        QList<QStringList> subbranchespath = item_to_be_deleted->path_children_all();

        qDebug() << "Label 4";

        // Сортировка массива веток по длине пути
        qSort(subbranchespath.begin(), subbranchespath.end()
        , [](const QStringList & list1, const QStringList & list2) {return list1.size() < list2.size();}    // compare_qstringlist_length
             );

        qDebug() << "Label 5";

        // Удаление всех таблиц конечных записей для нужных подветок
        // Удаление всех подчиненных элементов для нужных подветок
        // Вначале пробегаются самые длинные ветки а потом более короткие
        for(int i = subbranchespath.size() - 1; i >= 0; i--) {
            qDebug() << "Delete subbranch, id:" << _current_know_branch->item(subbranchespath.at(i))->field("id") << "name:" << _current_know_branch->item(subbranchespath.at(i))->field("name");
            (_current_know_branch->item(subbranchespath.at(i)))->children_clear();
            // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
        }

        qDebug() << "Label 6";

        // Удаление таблицы конечных записей для самой удаляемой ветки
        // Удаление подчиненных элементов для самой удаляемой ветки
        qDebug() << "Delete rootbranch, id:" << _current_know_branch->item(path)->field("id") << "name:" << _current_know_branch->item(path)->field("name");

        (_current_know_branch->item(path))->children_clear();
        // ( knowTreeModel->getItem(path) )->removeAllChildren();

        qDebug() << "Label 7";

        // Удаление ветки на экране, при этом удалятся все подветки
        qDebug() << "This branch have row() as" << index.row();

        if(index.isValid()) qDebug() << "Index valid";
        else qDebug() << "Index non valid";

        _current_know_branch->removeRows(index.row(), 1, index.parent());


        //    // Удаление всех нужных подветок
        //    // Пробегаются самые длинные ветки а потом более короткие
        //    for(int i = subbranchespath.size() - 1; i >= 0; i--)
        //        if(knowTreeModel->isItemValid(subbranchespath.at(i))) {
        //            TreeItem *current_item = knowTreeModel->getItem(subbranchespath.at(i));
        //            delete current_item;
        //        }

        //    if(!reocrd_controller) {
        //        globalparameters.entrance()->new_browser(QUrl(browser::Browser::_defaulthome));
        //    }

        //        if(item_to_be_deleted->is_registered_to_browser())   // item_to_be_deleted->unique_page()
        //            item_to_be_deleted->unique_page()->record_controller()->remove_child(item_to_be_deleted->id()); // (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

        close_tab_recursive(item_to_be_deleted);

        qDebug() << "Label 8";

        items_update_selected();
        QModelIndex setto;
        int new_count = item_to_be_deleted->parent()->count_direct();

        // Одноранговая ветка
        if(new_count > 0) {
            int new_pos = (deleted_position > new_count - 1) ? new_count - 1 : ((deleted_position - 1) >= 0) ? deleted_position : 0;

            setto = _current_know_branch->index(new_pos, 0, index.parent());
            _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
            _tree_view->selectionModel()->setCurrentIndex(setto, current_tree_selection_mode);   // ClearAndSelect
        } else {

            //            //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
            //            setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
            //            _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
            if(item_to_be_deleted->parent() == _current_know_branch->root_item()) {
                cursor_up_one_level();
            } else {

                if(item_to_be_deleted->parent() && item_to_be_deleted->parent() != _current_know_branch->root_item()) {
                    _tree_view->selectionModel()->select(_current_know_branch->index(item_to_be_deleted->parent()), current_tree_selection_mode);
                    _tree_view->selectionModel()->setCurrentIndex(_current_know_branch->index(item_to_be_deleted->parent()), current_tree_selection_mode);   // ClearAndSelect
                }
            }

            //            if(index.parent().parent().isValid()) {view_up_one_level();}
            //            else {view_return_to_root();}
        }

        //        assert(item_to_be_deleted == _current_know_branch->item(setto));
    }

    return item_to_be_deleted;
}

boost::intrusive_ptr<TreeItem> TreeScreen::branch_cut(boost::intrusive_ptr<TreeItem> item)
{
    QModelIndex _index = _tree_view->source_model()->index(item);    // know_root()

    assert(_index.isValid());
    QModelIndexList index_list;
    index_list.append(_index);
    auto results = branchs_delete(index_list, "cut");  // , _know_branch // know_root_holder::know_root()

    _actionlist["pasteBranch"]->setEnabled(true);
    _actionlist["pasteSubbranch"]->setEnabled(true);

    assert(results.size() == 1);
    assert(results.at(0) == item);
    return results.at(0);
}

void TreeScreen::branch_cut(bool _cut_branch_confirm)
{
    bool copy_result = false;

    copy_result = branch_copy();

    if(copy_result)
        branchs_delete("cut", _cut_branch_confirm); //this->_know_branch,

    _actionlist["pasteBranch"]->setEnabled(true);
    _actionlist["pasteSubbranch"]->setEnabled(true);

}

//boost::intrusive_ptr<TreeItem> TreeScreen::branch_paste(boost::intrusive_ptr<TreeItem> item, KnowModel *_current_know_branch)
//{
//    return branch_add(_tree_view->index_current(), item, false, _current_know_branch);
//}



bool TreeScreen::branch_copy(void)
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
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    QModelIndex _index_current;
    bool indexs_valid = true;
    QList<boost::intrusive_ptr<TreeItem>> items;
    auto _source_model = _tree_view->source_model();

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        //        QMessageBox messageBox(this);
        //        messageBox.setWindowTitle(tr("Unavailable action"));
        //        messageBox.setText(tr("Please select a single item for copy."));
        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //        messageBox.exec();
        //        return copy_result;
        for(auto i : selectitems) {
            if(!i.isValid()) {
                indexs_valid = false; break;
            } else {
                items << _source_model->item(i);
            }
        }

        _index_current = selectitems[0];
    }


    // Получение индекса выделенной ветки
    //    _index_current = _tree_view->index_current();

    if(indexs_valid // _index_current.isValid()
      ) {

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

        ClipboardBranch *branch_clipboard_data = new ClipboardBranch();
        fillflag = 1;

        for(auto item : items) {
            //            // Получение ссылки на узел, который соответствует выделенной ветке
            //            auto item = _source_model->item(_index_current);

            //            //        // Получение пути к выделенной ветке
            //            //        QStringList _current_item_absolute_path = item->path_absolute();

            //            //        // Получение путей ко всем подветкам
            //            //        QList<QStringList> sub_branches_absolute_path = item->path_children_all();


            // Проверка, содержит ли данная ветка как шифрованные
            // так и незашифрованные данные
            bool nocrypt_presence = false;
            bool encrypt_presence = false;

            // Флаги на основе состояния текущей ветки
            if(_know_model_board->item(_index_current   //_current_item_absolute_path
                                      )->field("crypt") == "1")
                encrypt_presence = true;
            else
                nocrypt_presence = true;

            // Флаги на основе состояния подветок
            for(int i = 0; i < item->count_direct(); i++) { //foreach(QStringList curr_absolute_path, sub_branches_absolute_path)
                if( //_know_model_board->item(curr_absolute_path)
                    item->child(i)->field("crypt") == "1") {
                    encrypt_presence = true;
                } else {
                    nocrypt_presence = true;
                }
            }

            // Если ветка содержит как шифрованные так и нешифрованные данные
            // то такую ветку копировать в буфер нельзя
            if(nocrypt_presence == true && encrypt_presence == true) {
                QMessageBox messageBox(this);
                messageBox.setWindowTitle(tr("Unavailable action"));
                messageBox.setText(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."));
                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
                messageBox.exec();
                break;  // return copy_result;
            }


            //            // -------------------
            //            // Копирование в буфер
            //            // -------------------

            //            qDebug() << "Branch copy to buffer";

            //            //            // Создается ссылка на буфер обмена
            //            //            QClipboard *cbuf = QApplication::clipboard();

            //            // Создается объект с данными для заполнения буфера обмена
            //            // Если в буфере есть какие-то старые данные, они удаляются
            //            static int fillflag = 0;

            //            if(fillflag == 1) {
            //                const ClipboardBranch *branch_clipboard_data_previos;
            //                branch_clipboard_data_previos = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());

            //                if(branch_clipboard_data_previos != nullptr)delete branch_clipboard_data_previos;

            //                fillflag = 0;
            //            }

            //            ClipboardBranch *branch_clipboard_data = new ClipboardBranch();

            //            fillflag = 1;

            // Добавление корневой ветки
            branch_clipboard_data->branch_push(item); // _current_item_absolute_path
            // branch_add_to_clipboard(branch_clipboard_data, _source_model_cur_item_absolute_path);   // , true

            //        // Добавление прочих веток
            //        foreach(QStringList curr_path_absolute, sub_branches_absolute_path) {
            //            branch_clipboard_data->branch_copy(curr_path_absolute);   // branch_add_to_clipboard(branch_clipboard_data, curr_path_absolute); // , false
            //        }

            branch_clipboard_data->print();

            //            // Объект с ветками помещается в буфер обмена
            //            cbuf->setMimeData(branch_clipboard_data);

            _actionlist["pasteBranch"]->setEnabled(true);
            _actionlist["pasteSubbranch"]->setEnabled(true);

            copy_result = true;
        }

        // Объект с ветками помещается в буфер обмена
        cbuf->setMimeData(branch_clipboard_data);
    } else {
        _actionlist["pasteBranch"]->setEnabled(false);
        _actionlist["pasteSubbranch"]->setEnabled(false);
        copy_result = false;
    }

    return copy_result;
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


void TreeScreen::branch_paste_func(std::function<QString(const QModelIndex &, ClipboardBranch *)>  lock_paste_as_if)
{
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if(mimeData == nullptr)
        return;

    if(!(mimeData->hasFormat("mytetra/branch")))
        return;

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        //        QMessageBox messageBox(this);
        //        messageBox.setWindowTitle(tr("Unavailable action"));
        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling paste operation."));
        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        //        messageBox.exec();
        //        return;
        _current_index = selectitems.first();
    } else {
        _current_index = _tree_view->index_current();
    }

    // Блокируется главное окно, чтобы при продолжительном выполнении
    // небыло возможности сделать другие действия
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);


    // Создается ссылка на буфер обмена
    QClipboard *cbuf = QApplication::clipboard();

    // Извлечение объекта из буфера обмена
    const ClipboardBranch *branch;
    branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
    branch->print();
    branch->id_tree_print();


    //    // Получение индекса выделенной строки
    //    QModelIndex _current_index = _tree_view->index_current();

    if(_current_index.isValid()) {
        // Добавление ветки
        QString pasted_branch_id;

        //        if(is_branch)
        //            pasted_branch_id = _tree_view->source_model()->lock_paste_as_sibling(index, (ClipboardBranch *)branch);
        //        else
        //            pasted_branch_id = _tree_view->source_model()->lock_paste_as_child(index, (ClipboardBranch *)branch);

        pasted_branch_id = lock_paste_as_if(_current_index, (ClipboardBranch *)branch);

        branch_update_on_screen(_current_index.parent());

        // Установка курсора на новую созданную ветку
        auto pasted_branch_item = _tree_view->source_model()->item_by_id(pasted_branch_id);
        QStringList pasted_branch_path = pasted_branch_item->path_absolute();
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->set_tree_position(_tree_view->source_model()->root_item()->id(), pasted_branch_path);

        _know_model_board->synchronized(false);
        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        knowtree_save();

        // Разблокируется главное окно
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }
}

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


// Шифрование ветки
void TreeScreen::branch_encrypt(void)
{
    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            branch_item_encrypt(); // Ветка шифруется
    } else {
        // Иначе считается, что шифрующий ключ уже был задан и он правильный

        branch_item_encrypt(); // Ветка шифруется
    }
}



// Расшифровка ветки
void TreeScreen::branch_decrypt(void)
{
    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            branch_item_decrypt(); // Ветка расшифровывается
    } else {
        // Иначе пароль в данной сессии вводился и он правильный

        branch_item_decrypt(); // Ветка расшифровывается
    }
}


void TreeScreen::branch_item_encrypt(void)
{
    auto index = _tree_view->index_current();

    if(index.isValid()) {
        // Получаем указатель на текущую выбранную ветку дерева
        auto item = _tree_view->source_model()->item(index);

        // Шифрация ветки и всех подветок
        item->to_encrypt();

        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        knowtree_save();

        // Обновляеются на экране ветка и ее подветки
        branch_update_on_screen(index);
    }
}


void TreeScreen::branch_item_decrypt(void)
{
    auto index = _tree_view->index_current();

    if(index.isValid()) {
        // Получаем указатель на текущую выбранную ветку дерева
        auto item = _tree_view->source_model()->item(index);

        // Расшифровка ветки и всех подветок
        item->to_decrypt();

        // Сохранение дерева веток
        //    find_object<TreeScreen>(tree_screen_singleton_name)->
        knowtree_save();

        // Обновляеются на экране ветка и ее подветки
        branch_update_on_screen(index);

        // Проверяется, остались ли в дереве зашифрованные данные
        // если зашифрованных данных нет, будет предложено сбросить пароль
        tree_crypt_control();
    }
}


// Обновление на экране ветки и подветок
void TreeScreen::branch_update_on_screen(const QModelIndex &index)
{
    auto source_model = _tree_view->source_model();
    // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
    source_model->emit_datachanged_signal(index);

    // По модельному индексу выясняется указатель на ветку
    auto item = source_model->item(index);

    // Перебираются подветки
    QList<QStringList> updatePathts = item->path_children_all();

    foreach(QStringList currentPath, updatePathts) {
        auto currentItem = source_model->item(currentPath);

        QModelIndex currentIndex = source_model->index(currentItem);

        // Для подветки дается команда чтобы модель сообщила о своем изменении
        source_model->emit_datachanged_signal(currentIndex);
    }
}

void TreeScreen::branchs_update_selected(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    std::set<QModelIndex> idxs;

    // Обновление на экране
    for(int i = 0; i < selectitems.size(); ++i) {
        if(idxs.find(selectitems.at(i)) == idxs.end())idxs.insert(selectitems.at(i));

        //        _tree_view->update(selectitems.at(i));
    }

    for(auto &i : idxs) {
        _tree_view->update(i);
    }
}

// Действия при клике на ветку дерева
void TreeScreen::branch_step_into(const QModelIndex &index)
{
    // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    // Получаем указатель на текущую выбранную ветку дерева
    auto result_item = _tree_view->source_model()->item(index);

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

        if(_tree_view->source_model()->root_item() != result_item) setup_model(result_item); // _know_branch->intercept(result_item);     //? never do this!!!

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
        if(appconfig.getInterfaceMode() == "mobile") {

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

    } else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
        //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        // Переключаются окна (используется для мобильного интерфейса)
        //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
    }

    if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
}





void TreeScreen::items_update_selected(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Обновление на экране
    for(int i = 0; i < selectitems.size(); ++i) {
        _tree_view->update(selectitems.at(i));
    }
}

void TreeScreen::index_invoke_in_current_branch(const QModelIndex &index)
{

    // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    (_tree_view->isExpanded(index)) ? _tree_view->setExpanded(index, false) : _tree_view->setExpanded(index, true);
    // Получаем указатель на текущую выбранную ветку дерева
    auto result_item = _tree_view->source_model()->item(index);

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
        if(appconfig.getInterfaceMode() == "mobile") {

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

    } else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
        //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        // Переключаются окна (используется для мобильного интерфейса)
        //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
    }

    if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
}

void TreeScreen::index_invoke_in_sub_branch(const QModelIndex &index)
{
    branch_step_into(index);
#ifdef _with_record_table
    _treemodelknow->record_to_item();
#endif
    //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
}


void TreeScreen::cursor_up_one_level(void)
{
    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    KnowModel *current_treemodel = _tree_view->source_model();  //static_cast<KnowModel *>(this->_tree_view->model());
    boost::intrusive_ptr<TreeItem> current_root = current_treemodel->root_item();    // ->parent();
    auto index = _tree_view->index_current();

    //    if(!index.isValid()) {
    //        _tree_view->selection_to_pos(_tree_view->source_model()->root_item()->current_count() - 1);
    //        index = _tree_view->view_index();
    //        assert(index.isValid());
    //    }

    auto current_item = current_treemodel->item(index);
    //    assert(current_treemodel == _know_branch);
    int origin_count = _tree_view->source_model()->count_records_all();

    if(current_root) {
        //        current_treemodelknow->root(parent);   // QModelIndex index = _root->index(current_treemodelknow->root()->parent());
        //        //        candidate_from_knowtree_item(index);
        //        setup_model(current_treemodelknow);

        //        if(current_treemodelknow->root() != _root->root()) {
        //            enable_up_action(true);
        //        } else {
        //            enable_up_action(false);
        //        }

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
                    current_treemodel->clear();  //root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));

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

                    return; // Программа дальше не идет, доделать...
                }
            }
        }

        //        auto new_parent = current_item->parent()->parent();

        if( //current_root->parent() &&
            current_item->parent() != current_root && current_item->parent()->path_absolute().size() > current_root->path_absolute().size()) {
            //            setup_model(current_root->parent());  //_know_branch->intercept(current_root->parent());     //? dangerous to do this!!!
            _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode);
            _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode);  // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
            //            enable_up_action();
        } else if(current_item->parent() == current_root) {
            if(current_root->parent()) {
                setup_model(current_root->parent());
                _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode); // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item->parent()), current_tree_selection_mode); // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
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
        if(appconfig.getInterfaceMode() == "mobile") {

            QStringList path = current_root->path_absolute_as_name();

            // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
            int emptyStringIndex = path.indexOf("");
            path.removeAt(emptyStringIndex);

            //            find_object<RecordScreen>(table_screen_singleton_name)
            globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

        }



        // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        _tree_view->resizeColumnToContents(0);

    } else if(current_item->parent()) {
        current_item = current_item->parent();

        if(current_item->parent()) {
            setup_model(current_item->parent());  // _know_branch->intercept(know_root()->root_item());
        } else {
            setup_model(_know_model_board->root_item());  // _know_branch->intercept(know_root()->root_item());
        }

        _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item), current_tree_selection_mode);
        _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item), current_tree_selection_mode);   // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

    } else {
        cursor_up_to_root();
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

}

void TreeScreen::cursor_up_to_root(void)
{
    KnowModel *current_source_model = _tree_view->source_model();  //static_cast<KnowModel *>(this->_tree_view->model());
    //    boost::intrusive_ptr<TreeItem> current_root = current_treemodel->root_item();
    auto index = _tree_view->index_current();

    //    if(!index.isValid()) {
    //        _tree_view->selection_to_pos(_tree_view->source_model()->root_item()->current_count() - 1);
    //        index = _tree_view->view_index();
    //        assert(index.isValid());
    //    }

    if(index.isValid()) {
        auto current_item = current_source_model->item(index);

        while(current_item->parent()->parent()) {current_item = current_item->parent();}

        //    QString path = current_root->absolute_path().at(1);
        // _know_branch->intercept(know_root()->root_item());
        setup_model(_know_model_board->root_item());  // setup_model(_know_branch);
        _tree_view->selectionModel()->select(_tree_view->source_model()->index(current_item), current_tree_selection_mode);
        _tree_view->selectionModel()->setCurrentIndex(_tree_view->source_model()->index(current_item), current_tree_selection_mode);    // QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
        enable_up_action(); // false
    }

}





/*
void treescreen::on_knowTreeView_repaint(void)
{
 // Попытка расширить нулевую колонку с деревом, если содержимое слишком широкое
 knowTreeView->resizeColumnToContents(0);
}
*/






void TreeScreen::cursor_to_index(QModelIndex index)
{
    // Курсор устанавливается на нужный элемент дерева
    // В desktop-варианте на сигнал currentRowChanged() будет вызван слот on_knowtree_clicked()
    _tree_view->selectionModel()->select(index, current_tree_selection_mode);
    _tree_view->selectionModel()->setCurrentIndex(index, current_tree_selection_mode);   // ClearAndSelect

    // В мобильной версии реакции на выбор ветки нет (не обрабатывается сигнал смены строки в модели выбора)
    // Поэтому по ветке должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
    // Метод clicked() публичный начиная с Qt5 (мобильный интерфейс возможен только в Qt5)
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

    if(_appconfig.getInterfaceMode() == "mobile")
        emit _tree_view->clicked(index);

#endif
}



QItemSelectionModel *TreeScreen::selection_model(void)
{
    return _tree_view->selectionModel();
}


// Получение номера первого выделенного элемента
int TreeScreen::item_first_selected(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    if(selectitems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
}



// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
void TreeScreen::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count " << _know_model_board->rowCount();

    if(_know_model_board->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";

        branch_add_new(_know_model_board, QModelIndex(), tr("Rename me"), [&](KnowModel * _current_model, QModelIndex _current_index, QString _id, QString _name) {
            return _current_model->lock_child_add_new(_current_index, _id, _name);
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
    auto result = _know_model_board->item(_know_model_board->index(KnowModel::delegater(id))); // _know_branch->intercept(prepared);    //item(_know_branch->index(prepared));

    setup_model(result);   //    return _know_branch->intercept(know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id))));

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
void TreeScreen::knowtree_save(void)
{
    //    if(!_know_branch->synchronized())
    //        know_root_holder::know_root()->synchronize(_know_branch->root_item());

    //    know_root_holder::know_root()->save();
    _tree_view->sychronize();
    _know_model_board->save();

}


// Перечитывание дерева веток с диска
void TreeScreen::knowtree_reload(void)
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

const KnowModel *TreeScreen::know_model_board()const {return _know_model_board;}

void TreeScreen::synchronized(bool _synchronized) {_know_model_board->synchronized(_synchronized);}

bool TreeScreen::sysynchronized() {return _know_model_board->synchronized();}

boost::intrusive_ptr<TreeItem> TreeScreen::duplicated_remove(boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source)
{
    return _know_model_board->duplicated_remove(target, source);
}
