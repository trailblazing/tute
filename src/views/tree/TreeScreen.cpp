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
#include "models/tree/TreeKnowModel.h"
#include "TreeKnowView.h"
#include "libraries/ClipboardBranch.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/Password.h"
#include "libraries/WindowSwitcher.h"
#include "views/browser/toolbarsearch.h"
#include "views/browser/entrance.h"

#include "TreeScreen.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;


know_root_holder::know_root_holder(const AppConfig &_appconfig, TreeScreen *_this)
    : _know_root(new TreeKnowModel(_this))
{
    // Загрузка данных
    // auto xml =
    _know_root->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");
    int all_count = _know_root->all_records_count();
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
    , _know_branch(new TreeKnowModel(this))
      //    , _tree_controller(new TreeController())
    , _tools_line(new QToolBar(this))
    , _menubar(new QToolBar(this))
    , _menubutton(new QPushButton("", _menubar))
    , _menuaction(new QWidgetAction(_menubutton))
    , _menus_in_button(new QMenu("buttonmenu"))
    , _tree_view(new TreeKnowView(this))
    , _tools_layout(new QHBoxLayout())
      //    , _recordtree_search(new browser::ToolbarSearch(this))
      //    , _recordtree_searchlayout(new QHBoxLayout())
    , _treescreenlayout(new QVBoxLayout())
    , _appconfig(_appconfig)
      //    , _shadow_branch(new TreeModelKnow(this))
{
    _know_branch->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");    // _know_branch->intercept(know_root_holder::know_root()->root_item());    // init_from_xml(xml);  //
    int all_count = _know_branch->all_records_count();
    assert(all_count > 0);

    _know_branch->synchronized(true);

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
    setup_model(_know_branch);
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

    //    if(_menuaction) {delete _menuaction; _menuaction = nullptr;}

    //    if(_menubutton) {delete _menubutton; _menubutton = nullptr;}

    if(_menubar) {delete _menubar; _menubar = nullptr;}

    //    delete _recordtree_search;
    if(_tools_line) {delete _tools_line; _tools_line = nullptr;}

    //    delete _treescreenlayout;
    //    delete _toolslayout;
    delete _tree_view;
    //    delete _shadow_branch;
    delete _know_branch;

}


void TreeScreen::setup_actions(void)
{
    QAction *ac;

    ac = new QAction(tr("View up one level"), this);
    ac->setStatusTip(tr("View up one level"));
    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::view_up_one_level);
    _actionlist["view_up_one_level"] = ac;

    ac = new QAction(tr("Return to root"), this);
    ac->setStatusTip(tr("Return to root"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::return_to_root);
    _actionlist["return_to_root"] = ac;

    // Разворачивание всех подветок
    // a->setShortcut(tr("Ctrl+X"));
    ac = new QAction(tr("Expand all sub items"), this);
    ac->setStatusTip(tr("Expand all sub items"));
    ac->setIcon(QIcon(":/resource/pic/expand_all_subbranch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::expand_all_subbranch);
    _actionlist["expandAllSubbranch"] = ac;

    // Сворачивание всех подветок
    ac = new QAction(tr("Collapse all sub items"), this);
    ac->setStatusTip(tr("Collapse all sub items"));
    ac->setIcon(QIcon(":/resource/pic/collapse_all_subbranch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::collapse_all_subbranch);
    _actionlist["collapseAllSubbranch"] = ac;

    // Перемещение ветки вверх
    ac = new QAction(tr("Move item up"), this);
    ac->setStatusTip(tr("Move item up"));
    ac->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::move_item_up_branch);
    _actionlist["moveUpBranch"] = ac;

    // Перемещение ветки вниз
    ac = new QAction(tr("Move item down"), this);
    ac->setStatusTip(tr("Move item down"));
    ac->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::move_item_dn_branch);
    _actionlist["moveDnBranch"] = ac;

    // Вставка новой подветки
    ac = new QAction(tr("Insert a new sub item"), this);
    ac->setStatusTip(tr("Insert a new sub item into selected"));
    ac->setIcon(QIcon(":/resource/pic/add_subbranch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::insert_subbranch);
    _actionlist["insSubbranch"] = ac;

    // Вставка новой ветки
    ac = new QAction(tr("Insert a new sibling item"), this);
    ac->setStatusTip(tr("Insert a new sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/add_branch.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::insert_branch);
    _actionlist["insBranch"] = ac;

    // Редактирование ветки
    ac = new QAction(tr("Edit item name"), this);
    ac->setStatusTip(tr("Edit name of selected item"));
    ac->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::edit_branch);
    _actionlist["editBranch"] = ac;

    // Удаление ветки
    ac = new QAction(tr("Delete item"), this);
    ac->setStatusTip(tr("Delete selected item and all sub items"));
    ac->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(ac, &QAction::triggered, this
    , [this](bool checked = false) {Q_UNUSED(checked); delete_branchs("cut");} //, &TreeScreen::del_branch
           );
    _actionlist["delBranch"] = ac;

    void(TreeScreen::*cut)() = &TreeScreen::cut_branch;
    // Удаление ветки с сохранением копии в буфер обмена
    ac = new QAction(tr("Cut item"), this);
    ac->setStatusTip(tr("Cut item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
    connect(ac, &QAction::triggered, this, cut);
    _actionlist["cutBranch"] = ac;

    // Копирование ветки в буфер обмена
    ac = new QAction(tr("Copy item"), this);
    ac->setStatusTip(tr("Copy item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::copy_branch);
    _actionlist["copyBranch"] = ac;

    void(TreeScreen::*paste)() = &TreeScreen::paste_branch;
    // Вставка ветки из буфера обмена
    ac = new QAction(tr("Paste item"), this);
    ac->setStatusTip(tr("Paste sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, paste);
    _actionlist["pasteBranch"] = ac;

    // Вставка ветки из буфера обмена в виде подветки
    ac = new QAction(tr("Paste as sub item"), this);
    ac->setStatusTip(tr("Paste item as sub item for selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::paste_subbranch);
    _actionlist["pasteSubbranch"] = ac;

    // Шифрование ветки
    ac = new QAction(tr("Encrypt item"), this);
    ac->setStatusTip(tr("Encrypt item and all subitem"));
    // actionEncryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::encrypt_branch);
    _actionlist["encryptBranch"] = ac;

    // Расшифровка ветки (снятие пароля)
    ac = new QAction(tr("Decrypt item"), this);
    ac->setStatusTip(tr("Decrypt item and all subitem"));
    // actionDecryptBranch->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::decrypt_branch);
    _actionlist["decryptBranch"] = ac;

    ac = new QAction(tr("Pin / freeze browser view"), this);
    ac->setStatusTip(tr("Pin / freeze browser view"));
    ac->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
    _actionlist["freeze_browser_view"] = ac;

    // Открытие поиска по базе (связывание клика происходит в MainWindows)
    ac = new QAction(tr("Find in base"), this);
    ac->setStatusTip(tr("Find in base"));
    ac->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
    // connection in void MainWindow::setupSignals(void)
    _actionlist["find_in_base"] = ac;
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
    _tree_view->setSelectionMode(QAbstractItemView::SingleSelection // MultiSelection  //
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
    _tree_view->setup_model(_item);
}

void TreeScreen::setup_model(TreeKnowModel *treemodel)
{
    // Создание и первичная настройка модели
    //    _knowtreemodel = new KnowTreeModel(this);

    // Установка заголовка
    // QStringList headers;
    // headers << tr("Info groups");
    // knowTreeModel->setHeaders(headers);

    //    // Загрузка данных
    //    _treemodelknow->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");
    _tree_view->reset();
    // Модель подключется к виду
    _tree_view->setModel(treemodel);
}


void TreeScreen::update_model(TreeKnowModel *_current_know_branch)  // too heavy
{
    _know_branch->synchronized(false);
    auto source_model = static_cast<TreeKnowModel *>(_tree_view->model());
    assert(_current_know_branch == source_model);
    _tree_view->reset();
    //    _tree_view->
    setup_model(_current_know_branch);
}


void TreeScreen::enable_up_action(bool enable)
{
    if(enable) {
        _actionlist["pasteBranch"]->setEnabled(true);
        _actionlist["pasteSubbranch"]->setEnabled(true);
        _actionlist["view_up_one_level"]->setEnabled(true);
        _actionlist["return_to_root"]->setEnabled(true);
    } else if(
        static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id() != _know_branch->root_item()->id() // know_root()->root_item() == _know_branch->root_item()
        || !enable) {
        _actionlist["pasteBranch"]->setEnabled(false);
        _actionlist["pasteSubbranch"]->setEnabled(false);
        _actionlist["view_up_one_level"]->setEnabled(false);
        _actionlist["return_to_root"]->setEnabled(false);
    }
}

// Открытие контекстного меню в дереве разделов
void TreeScreen::on_custom_contextmenu_requested(const QPoint &pos)
{
    qDebug() << "In TreeScreen::on_customContextMenuRequested";

    // Конструирование меню
    QMenu menu(this);



    menu.addAction(_actionlist["view_up_one_level"]);
    menu.addAction(_actionlist["return_to_root"]);

    menu.addAction(_actionlist["insBranch"]);
    menu.addAction(_actionlist["insSubbranch"]);
    menu.addAction(_actionlist["editBranch"]);
    menu.addAction(_actionlist["delBranch"]);
    menu.addSeparator();
    menu.addAction(_actionlist["expandAllSubbranch"]);
    menu.addAction(_actionlist["collapseAllSubbranch"]);
    menu.addSeparator();
    menu.addAction(_actionlist["moveUpBranch"]);
    menu.addAction(_actionlist["moveDnBranch"]);
    menu.addSeparator();
    menu.addAction(_actionlist["cutBranch"]);
    menu.addAction(_actionlist["copyBranch"]);
    menu.addAction(_actionlist["pasteBranch"]);
    menu.addAction(_actionlist["pasteSubbranch"]);
    menu.addSeparator();
    menu.addAction(_actionlist["encryptBranch"]);
    menu.addAction(_actionlist["decryptBranch"]);

    // Получение индекса выделенной ветки
    QModelIndex index = current_index();

    if(index.isValid()) {
        // Выясняется, зашифрована ли ветка или нет
        QString cryptFlag = _know_branch->item(index)->field("crypt");

        // Выясняется, зашифрована ли родительская ветка
        QString parentCryptFlag = _know_branch->item(index)->parent()->field("crypt");

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

            enable_up_action(is_branch || _know_branch->root_item()->id() != static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id());


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

    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    menu.exec(_tree_view->viewport()->mapToGlobal(pos));
}


void TreeScreen::setup_signals(void)
{
    //    connect(_recordtree_search, &browser::ToolbarSearch::textChanged, globalparameters.getFindScreen(), &FindScreen::enableFindButton);
    //    connect(_recordtree_search, &browser::ToolbarSearch::returnPressed, globalparameters.getFindScreen(), &FindScreen::findClicked);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
    connect(_tree_view, &TreeKnowView::customContextMenuRequested, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(_tree_view, &TreeKnowView::tapAndHoldGestureFinished, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if(appconfig.getInterfaceMode() == "desktop")
        connect(_tree_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TreeScreen::candidate_from_knowtree_item);

    if(appconfig.getInterfaceMode() == "mobile")
        connect(_tree_view, &TreeKnowView::clicked, this, &TreeScreen::candidate_from_knowtree_item);

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)), actionList["editBranch"], SLOT(trigger(void)));
    if(appconfig.getInterfaceMode() == "desktop") {
        connect(_tree_view, &TreeKnowView::clicked, this, &TreeScreen::candidate_from_knowtree_item);
        connect(_tree_view, &TreeKnowView::doubleClicked, this, &TreeScreen::to_candidate_screen);
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


void TreeScreen::expand_all_subbranch(void)
{
    // Получение индексов выделенных строк
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), true);
}


void TreeScreen::collapse_all_subbranch(void)
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


void TreeScreen::move_item_up_branch(void)
{
    move_item_up_dn_branch(1);
}


void TreeScreen::move_item_dn_branch(void)
{
    move_item_up_dn_branch(-1);
}


void TreeScreen::move_item_up_dn_branch(int direction)
{
    // Если ветку нельзя перемещать
    if(!move_checkenable()) return;

    // Получение индекса выделенной строки
    QModelIndex index = current_index();

    // Ветка перемещается
    QModelIndex index_after_move;

    if(direction == 1)
        index_after_move = _know_branch->move_up_branch(index);
    else
        index_after_move = _know_branch->move_dn_branch(index);

    _know_branch->synchronized(false);

    // Установка курсора на позицию, куда была перенесена ветка
    if(index_after_move.isValid()) {
        _tree_view->selectionModel()->setCurrentIndex(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        _tree_view->selectionModel()->select(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
    }

    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)
    globalparameters.tree_screen()->save_knowtree();
}


bool TreeScreen::move_checkenable(void)
{
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for moving."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return false;
    } else
        return true;
}


boost::intrusive_ptr<TreeItem> TreeScreen::insert_branch(void)
{
    qDebug() << "In ins_branch()";

    return insert_branch_smart(true);
}


boost::intrusive_ptr<TreeItem> TreeScreen::insert_subbranch(void)
{
    qDebug() << "In ins_subbranch()";

    return insert_branch_smart(false);
}


// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
// Helper function is used when you add branches
// Eli is branch = true, the branch is added to the same level
// If is_branch = false, then added podvetka
boost::intrusive_ptr<TreeItem> TreeScreen::insert_branch_smart(bool insert_sibling_branch)
{
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling insert operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        //        current_index = selectitems.first();
        return nullptr;
    }

    //    else if(selectitems.size() == 0) {
    //        current_index = last_index();
    //    } else {
    _current_index = current_index();
    //    }

    // Создается окно ввода данных
    bool ok;
    QString title, text;

    if(insert_sibling_branch) {
        title = tr("Create new item");
        text = tr("Item name:");
    } else {
        title = tr("Create new sub item");
        text = tr("Sub item name:");
    }

    QString name = QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);

    if(!(ok && !name.isEmpty())) return nullptr;    // Если была нажата отмена

    // Получение индекса выделенной строки
    //    QModelIndex
    //    current_index = currentitem_index();

    // Введенные данные добавляются
    return add_branch(_current_index, name, insert_sibling_branch, _know_branch);
}

//boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch)
//{
//    return add_branch(_current_index, name, insert_sibling_branch);
//}

boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, boost::intrusive_ptr<TreeItem> it, bool insert_sibling_branch, TreeKnowModel *_current_model)
{

    // Получение ссылки на узел, который соответствует выделенной строке
    auto current_item = _current_model->item(_current_index);

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);

    // Получение уникального идентификатора
    QString id = get_unical_id();
    it->field("id", id);
    QModelIndex setto;

    // Вставка данных и установка курсора

    // Одноранговая ветка
    if(insert_sibling_branch) {
        // Вставка новых данных в модель дерева записей
        _current_model->add_sibling_branch(_current_index, it);

        // Установка курсора на только что созданную позицию

        // Чтобы вычислить позицию, надо синхронно получить parent элемента,
        // на уровне которого должен был создасться новый элемент.
        // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
        // Затем у объекта Item выяснить количество элементов, и установить
        // засветку через метод index() относительно parent в виде QModelIndex
        //        QModelIndex
        setto = static_cast<TreeModel *>(_current_model)->index(current_item->parent()->current_count() - 1, 0, _current_index.parent());
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    } else {
        // Подветка

        // Вставка новых данных в модель дерева записей
        _current_model->add_child_branch(_current_index, it);

        // Установка курсора на только что созданную позицию
        //        QModelIndex
        setto = _current_model->index_child(_current_index, current_item->current_count() - 1);
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

        // А можно было установить курсор на нужную позицию и так
        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
        //                                             QItemSelectionModel::ClearAndSelect);
    }

    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    save_knowtree();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    return  _current_model->item(setto);
}

boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch, TreeKnowModel *_current_model)
{
    // Получение ссылки на узел, который соответствует выделенной строке
    auto item = _current_model->item(_current_index);

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);

    // Получение уникального идентификатора
    QString id = get_unical_id();
    QModelIndex setto;

    // Вставка данных и установка курсора

    // Одноранговая ветка
    if(insert_sibling_branch) {
        // Вставка новых данных в модель дерева записей
        _current_model->add_sibling_branch(_current_index, id, name);

        // Установка курсора на только что созданную позицию

        // Чтобы вычислить позицию, надо синхронно получить parent элемента,
        // на уровне которого должен был создасться новый элемент.
        // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
        // Затем у объекта Item выяснить количество элементов, и установить
        // засветку через метод index() относительно parent в виде QModelIndex
        //        QModelIndex
        setto = static_cast<TreeModel *>(_current_model)->index(item->parent()->current_count() - 1, 0, _current_index.parent());
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    } else {
        // Подветка

        // Вставка новых данных в модель дерева записей
        _current_model->add_child_branch(_current_index, id, name);

        // Установка курсора на только что созданную позицию
        //        QModelIndex
        setto = _current_model->index_child(_current_index, item->current_count() - 1);
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

        // А можно было установить курсор на нужную позицию и так
        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
        //                                             QItemSelectionModel::ClearAndSelect);
    }

    _know_branch->synchronized(false);
    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    save_knowtree();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    return  _current_model->item(setto);
}


void TreeScreen::edit_branch(void)
{
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling edit operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
    }

    // Получение индекса выделенной строки
    QModelIndex index = current_index();

    // Получение ссылки на узел, который соответствует выделенной строке
    auto item = _know_branch->item(index);

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
    save_knowtree();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
}


QList<boost::intrusive_ptr<TreeItem>> TreeScreen::delete_branchs(QModelIndexList selectitems , QString mode)
{
    QList<boost::intrusive_ptr<TreeItem>> result;
    // На время удаления блокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->blockSignals(true);

    // Список имен веток, которые нужно удалить
    QStringList branches_name;

    for(int i = 0; i < selectitems.size(); ++i) {
        QModelIndex index = selectitems.at(i);
        auto item = _know_branch->item(index);
        branches_name << item->field("name");
    }


    // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
    if(globalparameters.crypt_key().size() == 0) {
        bool disableFlag = false;

        // Перебираются удаляемые ветки
        for(int i = 0; i < selectitems.size(); ++i) {
            QModelIndex index = selectitems.at(i);
            auto item = _know_branch->item(index);

            // Если у ветки установлен флаг шифрования
            if(item->field("crypt") == "1") {
                disableFlag = true;
                break;
            }

            // Проверяется наличие флага шифрования у всех подветок
            QList<QStringList> cryptFlagsList = item->all_children_path_as_field("crypt");

            foreach(QStringList cryptFlags, cryptFlagsList)
                if(cryptFlags.contains("1")) {
                    disableFlag = true;
                    break;
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

            return result;
        }

    } // Закрылось условие что системный пароль не установлен


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

        if(appconfig.get_cutbranchconfirm()) enable_question = true;
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
        for(int i = 0; i < selectitems.size(); i++)
            for(int j = selectitems.size() - 1; j > i; j--) {
                QStringList path_1 = (_know_branch->item(selectitems.at(j - 1)))->path();
                QStringList path_2 = (_know_branch->item(selectitems.at(j)))->path();

                if(path_1.size() < path_2.size())
                    selectitems.swap(j - 1, j);
            }

        qDebug() << "Path for delete";

        for(int i = 0; i < selectitems.size(); ++i)
            qDebug() << (_know_branch->item(selectitems.at(i)))->path();

        auto _current_know_branch = static_cast<TreeKnowModel *>(_tree_view->model());

        // Вызов удаления веток
        for(int i = 0; i < selectitems.size(); ++i) {
            result.append(delete_one_branch(selectitems.at(i), _current_know_branch));
            update_branch_on_screen(selectitems.at(i));
        }

        qDebug() << "Delete finish";

        //        update_model(_current_know_branch);

        // Сохранение дерева веток
        //        find_object<TreeScreen>(tree_screen_singleton_name)->
        save_knowtree();
        //        reload_knowtree();
        qDebug() << "Save new tree finish";
    }

    // Разблокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->blockSignals(false);

    tree_empty_controll();
    tree_crypt_control();

    return result;
}



// Удаление выбранных веток, вызывается при выборе соотвествущей
// кнопки или пункта меню
// Delete the selected branches, called when the corresponding
// button or menu item
QList<boost::intrusive_ptr<TreeItem>> TreeScreen::delete_branchs(QString mode)
{
    qDebug() << "In del_branch()";

    // На время удаления блокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->blockSignals(true);


    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    if(selectitems.size() > 0) {
        auto _current_know_branch = static_cast<TreeKnowModel *>(_tree_view->model());
        _current_know_branch->synchronized(false);
    }

    return delete_branchs(selectitems, mode);

}



// Удаление одной ветки и её подветок
// Delete one branch and its podvetok / garter
boost::intrusive_ptr<TreeItem> TreeScreen::delete_one_branch(QModelIndex index, TreeKnowModel *_current_know_branch)
{

    if(!index.isValid()) return nullptr;

    qDebug() << "Label 1";

    // Получение узла, который соответствует обрабатываемому индексу
    boost::intrusive_ptr<TreeItem> item_to_be_deleted = _current_know_branch->item(index);

    int position = item_to_be_deleted->parent()->list_position(item_to_be_deleted);

    qDebug() << "Label 2, branch id:" << item_to_be_deleted->field("id") << "name:" << item_to_be_deleted->field("name");

    // Получение пути к элементу
    QStringList path = item_to_be_deleted->path();

    qDebug() << "Label 3";

    // Получение путей ко всем подветкам
    QList<QStringList> subbranchespath = item_to_be_deleted->all_children_path();

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
        (_current_know_branch->item(subbranchespath.at(i)))->clear_children();
        // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
    }

    qDebug() << "Label 6";

    // Удаление таблицы конечных записей для самой удаляемой ветки
    // Удаление подчиненных элементов для самой удаляемой ветки
    qDebug() << "Delete rootbranch, id:" << _current_know_branch->item(path)->field("id") << "name:" << _current_know_branch->item(path)->field("name");

    (_current_know_branch->item(path))->clear_children();
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


    (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

    qDebug() << "Label 8";
    update_selected();
    QModelIndex setto;
    int count = item_to_be_deleted->parent()->current_count();

    // Одноранговая ветка
    if(count > 0) {
        int new_pos = (position > count - 1) ? count - 1 : ((position - 1) > 0) ? position : 0;
        setto = static_cast<TreeModel *>(_current_know_branch)->index(new_pos, 0, index.parent());
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    } else {
        //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
        setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
        _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    }

    return item_to_be_deleted;
}

boost::intrusive_ptr<TreeItem> TreeScreen::cut_branch(boost::intrusive_ptr<TreeItem> item)
{
    QModelIndex index = _know_branch    // know_root()
                        ->index(item);
    assert(index.isValid());
    QModelIndexList list;
    list.append(index);
    auto result = delete_branchs(list
                                 // , _know_branch // know_root_holder::know_root()
                                 , "cut");
    assert(result.size() > 0);
    return result.at(0);
}


boost::intrusive_ptr<TreeItem> TreeScreen::paste_branch(boost::intrusive_ptr<TreeItem> item, TreeKnowModel *_current_know_branch)
{
    return add_branch(current_index(), item, false, _current_know_branch);
}

void TreeScreen::cut_branch(void)
{
    bool copy_result;

    copy_result = copy_branch();

    if(copy_result)
        delete_branchs( //this->_know_branch,
            "cut");
}

bool TreeScreen::copy_branch(void)
{
    qDebug() << "In copy_branch()";

    // Сохраняется текст в окне редактирования
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("Please select a single item for copy."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return false;
    }


    // Получение индекса выделенной ветки
    QModelIndex index = current_index();

    // Получение ссылки на узел, который соответствует выделенной ветке
    auto item = _know_branch->item(index);

    // Получение пути к выделенной ветке
    QStringList path = item->path();

    // Получение путей ко всем подветкам
    QList<QStringList> subbranchespath = item->all_children_path();


    // Проверка, содержит ли данная ветка как шифрованные
    // так и незашифрованные данные
    bool nocryptPresence = false;
    bool encryptPresence = false;

    // Флаги на основе состояния текущей ветки
    if(_know_branch->item(path)->field("crypt") == "1")
        encryptPresence = true;
    else
        nocryptPresence = true;

    // Флаги на основе состояния подветок
    foreach(QStringList currPath, subbranchespath)
        if(_know_branch->item(currPath)->field("crypt") == "1")
            encryptPresence = true;
        else
            nocryptPresence = true;

    // Если ветка содержит как шифрованные так и нешифрованные данные
    // то такую ветку копировать в буфер нельзя
    if(nocryptPresence == true && encryptPresence == true) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("This item contains both unencrypted and encrypted data. Copy/paste operation is possible only for item that contain similar type data."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return false;
    }


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

    // Добавление корневой ветки
    add_branch_to_clipboard(branch_clipboard_data, path, true);

    // Добавление прочих веток
    foreach(QStringList curr_path, subbranchespath)
        add_branch_to_clipboard(branch_clipboard_data, curr_path, false);

    branch_clipboard_data->print();

    // Объект с ветками помещается в буфер обмена
    cbuf->setMimeData(branch_clipboard_data);

    return true;
}


// Вспомогательная функция при копировании ветки в буфер
void TreeScreen::add_branch_to_clipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root)
{
    boost::intrusive_ptr<TreeItem> curr_item;
    QMap<QString, QString> curr_item_fields;
    QString branch_id;
    //    std::shared_ptr<RecordTable> curr_item_record_table;

    // Добавление ветки
    curr_item = _know_branch->item(path);
    curr_item_fields = curr_item->all_fields(); // Раньше было getAllFieldsDirect()
    branch_id = curr_item_fields["id"];

    if(is_root)
        branch_clipboard_data->addBranch("-1", curr_item_fields);
    else
        branch_clipboard_data->addBranch(curr_item->parent_id(), curr_item_fields);

    // Добавление конечных записей
    //    auto curr_item_record_table = curr_item->record_table();

    for(int i = 0; i < curr_item->current_count(); i++) {
        // Полный образ записи (с файлами и текстом)
        boost::intrusive_ptr<TreeItem> record = curr_item->item_fat(i);

        branch_clipboard_data->addRecord(branch_id, record);
    }
}


// Вставка ветки из буфера на том же уровне, что и выбранная
void TreeScreen::paste_branch(void)
{
    qDebug() << "In paste_branch";

    paste_branch_smart(true);
}


// Вставка ветки из буфера в виде подветки выбранной ветки
void TreeScreen::paste_subbranch(void)
{
    qDebug() << "In paste_subbranch";

    paste_branch_smart(false);
}


void TreeScreen::paste_branch_smart(bool is_branch)
{
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    if(mimeData == nullptr)
        return;

    if(!(mimeData->hasFormat("mytetra/branch")))
        return;

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Если выбрано более одной ветки
    if(selectitems.size() > 1) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Unavailable action"));
        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling paste operation."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return;
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
    branch->printIdTree();


    // Получение индекса выделенной строки
    QModelIndex index = current_index();

    // Добавление ветки
    QString pasted_branch_id;

    if(is_branch)
        pasted_branch_id = _know_branch->paste_sibling_branch(index, (ClipboardBranch *)branch);
    else
        pasted_branch_id = _know_branch->paste_child_branch(index, (ClipboardBranch *)branch);


    // Установка курсора на новую созданную ветку
    auto pasted_branch_item = _know_branch->item_by_id(pasted_branch_id);
    QStringList pasted_branch_path = pasted_branch_item->path();
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->set_tree_position(_know_branch->root_item()->id(), pasted_branch_path);

    _know_branch->synchronized(false);
    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    save_knowtree();

    // Разблокируется главное окно
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
}


// Шифрование ветки
void TreeScreen::encrypt_branch(void)
{
    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            encrypt_branch_item(); // Ветка шифруется
    } else {
        // Иначе считается, что шифрующий ключ уже был задан и он правильный

        encrypt_branch_item(); // Ветка шифруется
    }
}



// Расшифровка ветки
void TreeScreen::decrypt_branch(void)
{
    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true) // Если пароль введен правильно
            decrypt_branch_item(); // Ветка расшифровывается
    } else {
        // Иначе пароль в данной сессии вводился и он правильный

        decrypt_branch_item(); // Ветка расшифровывается
    }
}


void TreeScreen::encrypt_branch_item(void)
{
    // Получаем указатель на текущую выбранную ветку дерева
    auto item = _know_branch->item(current_index());

    // Шифрация ветки и всех подветок
    item->to_encrypt();

    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    save_knowtree();

    // Обновляеются на экране ветка и ее подветки
    update_branch_on_screen(current_index());
}


void TreeScreen::decrypt_branch_item(void)
{
    // Получаем указатель на текущую выбранную ветку дерева
    auto item = _know_branch->item(current_index());

    // Расшифровка ветки и всех подветок
    item->to_decrypt();

    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    save_knowtree();

    // Обновляеются на экране ветка и ее подветки
    update_branch_on_screen(current_index());

    // Проверяется, остались ли в дереве зашифрованные данные
    // если зашифрованных данных нет, будет предложено сбросить пароль
    tree_crypt_control();
}


// Обновление на экране ветки и подветок
void TreeScreen::update_branch_on_screen(const QModelIndex &index)
{
    auto source_model = static_cast<TreeKnowModel *>(_tree_view->model());
    // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
    source_model->emit_datachanged_signal(index);

    // По модельному индексу выясняется указатель на ветку
    auto item = source_model->item(index);

    // Перебираются подветки
    QList<QStringList> updatePathts = item->all_children_path();

    foreach(QStringList currentPath, updatePathts) {
        auto currentItem = source_model->item(currentPath);

        QModelIndex currentIndex = source_model->index(currentItem);

        // Для подветки дается команда чтобы модель сообщила о своем изменении
        source_model->emit_datachanged_signal(currentIndex);
    }
}

void TreeScreen::update_selected_branchs(void)
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

void TreeScreen::update_selected(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    // Обновление на экране
    for(int i = 0; i < selectitems.size(); ++i) {
        _tree_view->update(selectitems.at(i));
    }
}

void TreeScreen::to_candidate_screen(const QModelIndex &index)
{
    candidate_from_knowtree_item(index);
#ifdef _with_record_table
    _treemodelknow->record_to_item();
#endif
    //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
}


void TreeScreen::view_up_one_level(void)
{
    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    TreeKnowModel *current_treemodel = static_cast<TreeKnowModel *>(this->_tree_view->model());
    boost::intrusive_ptr<TreeItem> current_root = current_treemodel->root_item();    // ->parent();
    //    assert(current_treemodel == _know_branch);
    int origin_count = _know_branch->all_records_count();

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

        if(current_root->parent()) {
            setup_model(current_root->parent());  //_know_branch->intercept(current_root->parent());     //? dangerous to do this!!!
            _tree_view->selectionModel()->setCurrentIndex(static_cast<TreeKnowModel *>(_tree_view->model())->index(current_root->parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
            enable_up_action(true                    // _selected_branch->root() != _root->root()
                            ) ;
        } else {
            //            if(_root->root_item()->parent())
            //                _selected_branch->root_item(_root->root_item()->parent());
            //            else
            setup_model(_know_branch->root_item());  // _know_branch->intercept(know_root()->root_item());
            _tree_view->selectionModel()->setCurrentIndex(static_cast<TreeKnowModel *>(_tree_view->model())->index(current_root), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
            enable_up_action(false) ;
        }

        //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
        //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

        // Получаем указатель на данные таблицы конечных записей
        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();

        assert(_know_branch->all_records_count() >= origin_count);

        //        setup_model(_know_branch);

        enable_up_action(
            true    // _selected_branch->root() != _root->root()
        ) ;

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

            QStringList path = current_root->path_as_name();

            // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
            int emptyStringIndex = path.indexOf("");
            path.removeAt(emptyStringIndex);

            //            find_object<RecordScreen>(table_screen_singleton_name)
            globalparameters.entrance()->activiated_registered().first->record_screen()->tree_path(path.join(" > "));

        }



        // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        _tree_view->resizeColumnToContents(0);

    } else {
        return_to_root();
        enable_up_action(false) ;
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

}

void TreeScreen::return_to_root(void)
{
    // _know_branch->intercept(know_root()->root_item());
    setup_model(_know_branch->root_item());  // setup_model(_know_branch);
    enable_up_action(false);

}

void TreeScreen::candidate_from_search_result(std::shared_ptr<ItemsFlat> resultset_item)
{
    if(resultset_item->current_count() > 0) {

        RecordController *_record_controller = // resultset_item->unique_page()
            resultset_item->child(0)->unique_page()->view()->record_controller();   // globalparameters.record_screens()->record_controller();
        RecordModel *controller_source_model = _record_controller->source_model();

        setup_model(_know_branch);
        _actionlist["return_to_root"]->setEnabled(true);

        //    assert(controller_source_model->tree_item() == _selected_branch->_root_item);
        controller_source_model->reset_internal_data();  // delete source_model;   // dangerous!!!
        //    std::shared_ptr<RecordTable> target_data = source_model->tree_item()->tabledata();    // resultset_item->tabledata();
        //    controller->init();




        //    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
        //    auto dommodel = resultset_item->export_to_dom();    // source->init(startItem, QDomElement());
        //    resultset_item->remove_all_children();              // dommodel has noting to do with children!

        //    auto result_data = std::make_shared<RecordTable>(dommodel); //    resultset_item->tabledata(dommodel);               //    resultset_item->table_clear();    // already insert into table_init

        //    //    resultset_data->init(resultset_item, dommodel);     // just for init the item bind for reault set record table

        //    //    _knowtreemodel->save();

        //        controller_source_model->init_source_model(resultset_item   //? should we put it to treemodelknow? not the source of browser_pages
        //                                                   , _record_controller
        //                                                   , _record_controller->record_screen()
        //                                                   , globalparameters.mainwindow()
        //                                                   , globalparameters.meta_editor()
        //                                                   // , globalparameters.record_screens()
        //                                                  );



        setup_model(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr, resultset_item)));   // _know_branch->intercept(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr, resultset_item)));



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



        // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        _tree_view->resizeColumnToContents(0);

        // Переключаются окна (используется для мобильного интерфейса)
        //    globalparameters.window_switcher()->switchFromTreeToRecordtable();
        //    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());

    }
}

// Действия при клике на ветку дерева
void TreeScreen::candidate_from_knowtree_item(const QModelIndex &index)
{
    // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

    // Сохраняется текст в окне редактирования в соответсвующий файл
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    // Получаем указатель на текущую выбранную ветку дерева
    auto result_item = _know_branch->item(index);

    if(result_item->count() > 0) {    // && (result_item->field("url") != "" || result_item->field("url") == "" )
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



                    _tree_view->source_model().reset(); // _know_branch->clear();  // root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));




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

        if(static_cast<TreeKnowModel *>(_tree_view->model())->root_item() != result_item)setup_model(result_item); // _know_branch->intercept(result_item);     //? never do this!!!

        //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
        //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

        // Получаем указатель на данные таблицы конечных записей
        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();



        // setup_model(_know_branch);

        enable_up_action(_know_branch->root_item()->id() != static_cast<TreeKnowModel *>(_tree_view->model())->root_item()->id()   // _know_branch->root_item()->id() != know_root()->root_item()->id()
                        ) ;

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

            QStringList path = result_item->path_as_name();

            // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
            int emptyStringIndex = path.indexOf("");
            path.removeAt(emptyStringIndex);

            //            find_object<RecordScreen>(table_screen_singleton_name)
            globalparameters.entrance()->activiated_registered().first->record_screen()->tree_path(path.join(" > "));

        }



        // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        _tree_view->resizeColumnToContents(0);

        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);

    } else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
        globalparameters.entrance()->activate(result_item);
        // Переключаются окна (используется для мобильного интерфейса)
        //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
    }
}


/*
void treescreen::on_knowTreeView_repaint(void)
{
 // Попытка расширить нулевую колонку с деревом, если содержимое слишком широкое
 knowTreeView->resizeColumnToContents(0);
}
*/




QItemSelectionModel *TreeScreen::selection_model(void)
{
    return _tree_view->selectionModel();
}


void TreeScreen::cursor_to_index(QModelIndex index)
{
    // Курсор устанавливается на нужный элемент дерева
    // В desktop-варианте на сигнал currentRowChanged() будет вызван слот on_knowtree_clicked()
    _tree_view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);

    // В мобильной версии реакции на выбор ветки нет (не обрабатывается сигнал смены строки в модели выбора)
    // Поэтому по ветке должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
    // Метод clicked() публичный начиная с Qt5 (мобильный интерфейс возможен только в Qt5)
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

    if(_appconfig.getInterfaceMode() == "mobile")
        emit _tree_view->clicked(index);

#endif
}


// Получение номера первого выделенного элемента
int TreeScreen::first_selecteditem_row(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();

    if(selectitems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
}


// Get the index of the current element on which the cursor is positioned   // Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::current_index(void)
{
    //    if(!_tree_view->selectionModel()->currentIndex().isValid()) {

    //        boost::intrusive_ptr<TreeItem> _item = nullptr;
    //        TreeKnowModel *current_model = static_cast<TreeKnowModel *>(_tree_view->model());

    //        if(0 == current_model->root_item()->current_count()) {
    //            return_to_root();   //
    //            current_model = static_cast<TreeKnowModel *>(_tree_view->model());
    //            _item = current_model->root_item()->add_child();
    //        } else {
    //            _item = current_model->root_item()->child(0);  // _know_branch->root_item()->current_count() - 1

    //        }

    //        assert(_item);

    //        QModelIndex cur_index;



    //        //        _tree_view->reset();
    //        //        _tree_view->setModel(current_model);

    //        if(_item) {
    //            cur_index = current_model->index(_item);    // item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent()) // _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1)

    //            assert(cur_index.isValid());
    //            assert(current_model->item(cur_index) == _item);
    //            //            candidate_from_knowtree_item(cur_index);

    //            assert(_tree_view->model());

    //            _tree_view->selectionModel()->setCurrentIndex(cur_index // , QItemSelectionModel::SelectCurrent    //
    //                                                          , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //                                                         );
    //            _tree_view->selectionModel()->select(cur_index  //, QItemSelectionModel::SelectCurrent
    //                                                 , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //                                                );
    //            //            _tree_view->clicked(cur_index);
    //        }

    //        //        else {
    //        //            //            cur_index = _knowtreemodel->index_child(
    //        //            //                            _knowtreemodel->index(_knowtreemodel->_root_item)   // does not work!
    //        //            //                            , _knowtreemodel->_root_item->child_count() - 1);

    //        //            cur_index = _knowtreemodel->index(
    //        //                            _knowtreemodel->_root_item->child_count() - 1, 0, _knowtreemodel->index(item->parent()) // _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1)
    //        //                        );

    //        //            assert(cur_index.isValid());
    //        //            _knowtreeview->selectionModel()->setCurrentIndex(
    //        //                cur_index
    //        //                //                _knowtreemodel->index_child(
    //        //                //                    _knowtreemodel->index(_knowtreemodel->_root_item)
    //        //                //                    , _knowtreemodel->_root_item->child_count() - 1)



    //        //                //                _knowtreemodel->index(
    //        //                //                    _knowtreemodel->_root_item->child
    //        //                //                    (
    //        //                //                        _knowtreemodel->_root_item->child_count() - 1
    //        //                //                    )
    //        //                //                )
    //        //                //            _knowtreemodel->createIndex(0, 0, static_cast<void *>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
    //        //                //        _knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item), 0)
    //        //                , QItemSelectionModel::SelectCurrent
    //        //            );
    //        //        }
    //    }

    //    auto v = _treeknowview->selectionModel()->currentIndex();
    //    candidate_from_knowtree_item(cur_index);
    //    assert(_tree_view->selectionModel()->currentIndex().isValid());    // this line is to be recovery
    //    assert(cur_index.isValid());
    return // cur_index;   // temporary setting???   //
        _tree_view->selectionModel()->currentIndex();
}

// Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::last_index(void)
{
    //    if(!_tree_view->selectionModel()->currentIndex().isValid()) {
    boost::intrusive_ptr<TreeItem> _item = nullptr;

    TreeKnowModel *current_model = static_cast<TreeKnowModel *>(_tree_view->model());

    if(0 == current_model->root_item()->current_count()) {
        return_to_root();   //
        current_model = static_cast<TreeKnowModel *>(_tree_view->model());
        _item = current_model->root_item()->add_child();
    } else {
        _item = current_model->root_item()->child(current_model->root_item()->current_count() - 1);
    }

    assert(_item);
    QModelIndex cur_index = current_model->index(_item);    // item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent()) // _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1)

    assert(cur_index.isValid());
    assert(current_model->item(cur_index) == _item);
    //        candidate_from_knowtree_item(cur_index);

    //    if(!_knowtreeview->selectionModel()->currentIndex().isValid()) {
    _tree_view->selectionModel()->setCurrentIndex(
        cur_index
        //        _knowtreemodel->index(_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1))
        //            _knowtreemodel->createIndex(0, 0, static_cast<void *>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
        //        _knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item), 0)
        // , QItemSelectionModel::SelectCurrent
        , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    );
    //    }
    _tree_view->selectionModel()->select(cur_index
                                         // , QItemSelectionModel::SelectCurrent
                                         , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                                        );
    //        _tree_view->clicked(cur_index);
    //    candidate_from_knowtree_item(cur_index);

    //    }

    //    assert(_tree_view->selectionModel()->currentIndex().isValid());    // this line is to be recovery
    return _tree_view->selectionModel()->currentIndex();
}


// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
void TreeScreen::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count " << _know_branch->rowCount();

    if(_know_branch->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";

        add_branch(QModelIndex(), tr("Rename me"), false, _know_branch);
    }
}


// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void TreeScreen::tree_crypt_control(void)
{
    // Если в дереве нет шифрования
    if(_know_branch->is_contains_crypt_branches() == false) {
        // Запускается диалог сброса пароля шифрования
        Password pwd;
        pwd.resetPassword();
    }
}


boost::intrusive_ptr<TreeItem> TreeScreen::intercept(QString id)
{
    //    auto prepared = know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id)));
    auto result = _know_branch->item(_know_branch->index(TreeKnowModel::delegater(id))); // _know_branch->intercept(prepared);    //item(_know_branch->index(prepared));

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
void TreeScreen::save_knowtree(void)
{
    //    if(!_know_branch->synchronized())
    //        know_root_holder::know_root()->synchronize(_know_branch->root_item());

    //    know_root_holder::know_root()->save();

    _know_branch->save();

}


// Перечитывание дерева веток с диска
void TreeScreen::reload_knowtree(void)
{
    _know_branch->reload(); // do not do this if xml_file_name of this object is null
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
