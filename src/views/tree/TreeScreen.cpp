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
#include "TreeScreen.h"
#include "KnowTreeView.h"

#include "models/record_table/RecordTable.h"
#include "views/record_table/RecordScreen.h"
#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "models/tree/TreeItem.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/ClipboardBranch.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "libraries/crypt/Password.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/record_table/RecordController.h"
#include "views/browser/toolbarsearch.h"
#include "models/record_table/RecordModel.h"


extern AppConfig appconfig;
extern GlobalParameters globalparameters;


TreeScreen::TreeScreen(QString      object_name
                       , const AppConfig &appconfig
                       , QMenu      *_filemenu
                       , QMenu      *_toolsmenu
                       , QWidget    *parent
                      )
    : QWidget(parent)
      //, ui(new Ui::MainWindow)
      //, _menubar(new QMenuBar(this))
    , _knowtreemodel(new KnowTreeModel(this))
    , _shadow_candidate_model(new KnowTreeModel(this))
    , _shadow_page_model(new KnowTreeModel(this))
      //    , _tree_controller(new TreeController())
    , _toolsline(new QToolBar(this))
    , _menubar(new QToolBar(this))
    , _menubutton(new QPushButton("", _menubar))
    , _menuaction(new QWidgetAction(_menubutton))
    , _menus_in_button(new QMenu("buttonmenu"))
    , _knowtreeview(new KnowTreeView(this))
    , _toolslayout(new QHBoxLayout())
      //    , _recordtree_search(new browser::ToolbarSearch(this))
      //    , _recordtree_searchlayout(new QHBoxLayout())
    , _treescreenlayout(new QVBoxLayout())
    , _appconfig(appconfig)
{
    _shadow_candidate_model->init_from_xml(_appconfig.get_tetradir() + "/candidate.xml");
    _shadow_candidate_model->_root_item->field("id", get_unical_id());
    _shadow_candidate_model->_root_item->field("name", "Candidate");
    _shadow_page_model->init_from_xml(_appconfig.get_tetradir() + "/default_page.xml");
    _shadow_page_model->_root_item->field("id", get_unical_id());
    _shadow_page_model->_root_item->field("name", "Current Session");

    //    _recordtree_search->setMinimumWidth(100);
    //    _recordtree_search->sizePolicy().setHorizontalStretch(10);
    setObjectName(object_name);
    setup_actions();
    setup_ui(_filemenu, _toolsmenu);
    setup_models();
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
    if(_toolsline) {delete _toolsline; _toolsline = nullptr;}

    //    delete _treescreenlayout;
    //    delete _toolslayout;
    delete _knowtreeview;
    delete _shadow_page_model;
    delete _shadow_candidate_model;
    delete _knowtreemodel;
}


void TreeScreen::setup_actions(void)
{
    QAction *ac;

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
    connect(ac, &QAction::triggered, this, &TreeScreen::move_up_branch);
    _actionlist["moveUpBranch"] = ac;

    // Перемещение ветки вниз
    ac = new QAction(tr("Move item down"), this);
    ac->setStatusTip(tr("Move item down"));
    ac->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::move_dn_branch);
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

    // Удаление ветки с сохранением копии в буфер обмена
    ac = new QAction(tr("Cut item"), this);
    ac->setStatusTip(tr("Cut item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_cut.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::cut_branch);
    _actionlist["cutBranch"] = ac;

    // Копирование ветки в буфер обмена
    ac = new QAction(tr("Copy item"), this);
    ac->setStatusTip(tr("Copy item including sub items"));
    ac->setIcon(QIcon(":/resource/pic/branch_copy.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::copy_branch);
    _actionlist["copyBranch"] = ac;

    // Вставка ветки из буфера обмена
    ac = new QAction(tr("Paste item"), this);
    ac->setStatusTip(tr("Paste sibling item after selected"));
    ac->setIcon(QIcon(":/resource/pic/branch_paste.svg"));
    connect(ac, &QAction::triggered, this, &TreeScreen::paste_branch);
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

    insert_action_as_button<QToolButton>(_toolsline, _actionlist["insBranch"]);
    insert_action_as_button<QToolButton>(_toolsline, _actionlist["insSubbranch"]);

    if(appconfig.getInterfaceMode() == "desktop") {
        insert_action_as_button<QToolButton>(_toolsline, _actionlist["editBranch"]);
        insert_action_as_button<QToolButton>(_toolsline, _actionlist["delBranch"]);
    }

    _toolsline->addSeparator();

    insert_action_as_button<QToolButton>(_toolsline, _actionlist["expandAllSubbranch"]);
    insert_action_as_button<QToolButton>(_toolsline, _actionlist["collapseAllSubbranch"]);

    _toolsline->addSeparator();

    insert_action_as_button<QToolButton>(_toolsline, _actionlist["moveUpBranch"]);
    insert_action_as_button<QToolButton>(_toolsline, _actionlist["moveDnBranch"]);

    if(appconfig.getInterfaceMode() == "mobile") {
        _toolsline->addSeparator();
        insert_action_as_button<QToolButton>(_toolsline, _actionlist["freeze_browser_view"]);
        insert_action_as_button<QToolButton>(_toolsline, _actionlist["find_in_base"]); // Клик по этой кнопке связывается с действием в MainWindow
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
    _knowtreeview->setObjectName(knowtreeview_singleton_name);
    _knowtreeview->setMinimumSize(1     // 150
                                  , 1   // 250
                                 );
    _knowtreeview->setWordWrap(true);

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
    _knowtreeview->setSelectionMode(QAbstractItemView::
                                    SingleSelection // MultiSelection  //
                                   );

    _knowtreeview->setHorizontalScrollBarPolicy(Qt::
                                                ScrollBarAsNeeded   // ScrollBarAlwaysOn
                                               );

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    _knowtreeview->setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    _knowtreeview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //    QSize size = appconfig.treescreensize();
    //    resize(size);
}


void TreeScreen::setup_models(void)
{
    // Создание и первичная настройка модели
    //    _knowtreemodel = new KnowTreeModel(this);

    // Установка заголовка
    // QStringList headers;
    // headers << tr("Info groups");
    // knowTreeModel->setHeaders(headers);

    // Загрузка данных
    _knowtreemodel->init_from_xml(_appconfig.get_tetradir() + "/mytetra.xml");

    // Модель подключется к виду
    _knowtreeview->setModel(_knowtreemodel);
}


// Открытие контекстного меню в дереве разделов
void TreeScreen::on_custom_contextmenu_requested(const QPoint &pos)
{
    qDebug() << "In TreeScreen::on_customContextMenuRequested";

    // Конструирование меню
    QMenu menu(this);

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
    QModelIndex index = currentitem_index();

    // Выясняется, зашифрована ли ветка или нет
    QString cryptFlag = _knowtreemodel->item(index)->field("crypt");

    // Выясняется, зашифрована ли родительская ветка
    QString parentCryptFlag = _knowtreemodel->item(index)->parent()->field("crypt");

    // Если ветка не зашифрована
    // Или ветка зашифрована, но пароль успешно введен
    if(cryptFlag != "1" ||
       (cryptFlag == "1" && globalparameters.crypt_key().length() > 0)) {

        // Если в буфере есть ветки, соответсвующие пункты становятся активными
        bool isBranch = false;
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();

        if(mimeData != nullptr)
            if(mimeData->hasFormat("mytetra/branch"))
                isBranch = true;

        if(isBranch) {
            _actionlist["pasteBranch"]->setEnabled(true);
            _actionlist["pasteSubbranch"]->setEnabled(true);
        } else {
            _actionlist["pasteBranch"]->setEnabled(false);
            _actionlist["pasteSubbranch"]->setEnabled(false);
        }


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


    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    menu.exec(_knowtreeview->viewport()->mapToGlobal(pos));
}


void TreeScreen::setup_signals(void)
{
    //    connect(_recordtree_search, &browser::ToolbarSearch::textChanged, globalparameters.getFindScreen(), &FindScreen::enableFindButton);
    //    connect(_recordtree_search, &browser::ToolbarSearch::returnPressed, globalparameters.getFindScreen(), &FindScreen::findClicked);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику на ветке
    connect(_knowtreeview, &KnowTreeView::customContextMenuRequested, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(_knowtreeview, &KnowTreeView::tapAndHoldGestureFinished, this, &TreeScreen::on_custom_contextmenu_requested);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if(appconfig.getInterfaceMode() == "desktop")
        connect(_knowtreeview->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TreeScreen::candidate_from_knowtree_item);

    if(appconfig.getInterfaceMode() == "mobile")
        connect(_knowtreeview, &KnowTreeView::clicked, this, &TreeScreen::candidate_from_knowtree_item);

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // connect(knowTreeView, SIGNAL(doubleClicked(const QModelIndex &)), actionList["editBranch"], SLOT(trigger(void)));
    if(appconfig.getInterfaceMode() == "desktop")
        connect(_knowtreeview, &KnowTreeView::doubleClicked, this, &TreeScreen::to_candidate_screen);

    // Сигнал что ветка выбрана мышкой
    // connect(knowTreeView,SIGNAL(pressed(const QModelIndex &)), this,SLOT(on_knowTreeView_clicked(const QModelIndex &)));
    // connect(knowTreeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(on_knowTreeView_clicked(const QModelIndex &)));

}


void TreeScreen::assembly(void)
{
    _toolslayout->addWidget(_toolsline);
    _toolslayout->addStretch();
    _toolslayout->addWidget(_menubar);
    //    _treescreenlayout = new QVBoxLayout();
    _treescreenlayout->setObjectName("treescreen_QVBoxLayout");

    //    _treescreenlayout->addLayout(_recordtree_searchlayout);
    _treescreenlayout->addLayout(_toolslayout);
    _treescreenlayout->addWidget(_knowtreeview);

    setLayout(_treescreenlayout);

    // The boundaries are removed, as this facility will be used as a widget    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);
}


void TreeScreen::expand_all_subbranch(void)
{
    // Получение индексов выделенных строк
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), true);
}


void TreeScreen::collapse_all_subbranch(void)
{
    // Получение индексов выделенных строк
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), false);
}


void TreeScreen::expand_or_collapse_recurse(QModelIndex index, bool mode)
{
    _knowtreeview->setExpanded(index, mode);

    int i = 0;

    while((index.child(i, 0)).isValid()) {
        expand_or_collapse_recurse(index.child(i, 0), mode);
        i++;
    }

}


void TreeScreen::move_up_branch(void)
{
    moveupdn_branch(1);
}


void TreeScreen::move_dn_branch(void)
{
    moveupdn_branch(-1);
}


void TreeScreen::moveupdn_branch(int direction)
{
    // Если ветку нельзя перемещать
    if(!move_checkenable()) return;

    // Получение индекса выделенной строки
    QModelIndex index = currentitem_index();

    // Ветка перемещается
    QModelIndex index_after_move;

    if(direction == 1) index_after_move = _knowtreemodel->move_up_branch(index);
    else             index_after_move = _knowtreemodel->move_dn_branch(index);

    // Установка курсора на позицию, куда была перенесена ветка
    if(index_after_move.isValid()) {
        _knowtreeview->selectionModel()->setCurrentIndex(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        _knowtreeview->selectionModel()->select(index_after_move, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
    }

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
}


bool TreeScreen::move_checkenable(void)
{
    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

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
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();
    QModelIndex current_index;

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
    current_index = currentitem_index();
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
    return insert_branch_process(current_index, name, insert_sibling_branch);
}

boost::intrusive_ptr<TreeItem> TreeScreen::add_branch(QModelIndex current_index, QString name, bool insert_sibling_branch)
{
    return insert_branch_process(current_index, name, insert_sibling_branch);
}

boost::intrusive_ptr<TreeItem> TreeScreen::insert_branch_process(QModelIndex current_index, QString name, bool insert_sibling_branch)
{
    // Получение ссылки на узел, который соответствует выделенной строке
    auto item = _knowtreemodel->item(current_index);

    find_object<MainWindow>("mainwindow")->setDisabled(true);

    // Получение уникального идентификатора
    QString id = get_unical_id();
    QModelIndex setto;

    // Вставка данных и установка курсора

    // Одноранговая ветка
    if(insert_sibling_branch) {
        // Вставка новых данных в модель дерева записей
        _knowtreemodel->add_new_sibling_branch(current_index, id, name);

        // Установка курсора на только что созданную позицию

        // Чтобы вычислить позицию, надо синхронно получить parent элемента,
        // на уровне которого должен был создасться новый элемент.
        // Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
        // Затем у объекта Item выяснить количество элементов, и установить
        // засветку через метод index() относительно parent в виде QModelIndex
        //        QModelIndex
        setto = _knowtreemodel->index(item->parent()->child_count() - 1, 0, current_index.parent());
        _knowtreeview->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);
    } else {
        // Подветка

        // Вставка новых данных в модель дерева записей
        _knowtreemodel->add_new_child_branch(current_index, id, name);

        // Установка курсора на только что созданную позицию
        //        QModelIndex
        setto = _knowtreemodel->index_children(current_index, item->child_count() - 1);
        _knowtreeview->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

        // А можно было установить курсор на нужную позицию и так
        // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
        //                                             QItemSelectionModel::ClearAndSelect);
    }

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    find_object<MainWindow>("mainwindow")->setEnabled(true);
    return  _knowtreemodel->item(setto);
}


void TreeScreen::edit_branch(void)
{
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

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
    QModelIndex index = currentitem_index();

    // Получение ссылки на узел, который соответствует выделенной строке
    auto item = _knowtreemodel->item(index);

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

    find_object<MainWindow>("mainwindow")->setDisabled(true);

    item->field("name", newname);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    find_object<MainWindow>("mainwindow")->setEnabled(true);
}


// Удаление выбранных веток, вызывается при выборе соотвествущей
// кнопки или пункта меню
// Delete the selected branches, called when the corresponding
// button or menu item
void TreeScreen::delete_branchs(QString mode)
{
    qDebug() << "In del_branch()";

    // На время удаления блокируется главное окно
    find_object<MainWindow>("mainwindow")->setDisabled(true);
    find_object<MainWindow>("mainwindow")->blockSignals(true);


    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

    // Список имен веток, которые нужно удалить
    QStringList branches_name;

    for(int i = 0; i < selectitems.size(); ++i) {
        QModelIndex index = selectitems.at(i);
        auto item = _knowtreemodel->item(index);
        branches_name << item->field("name");
    }


    // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
    if(globalparameters.crypt_key().size() == 0) {
        bool disableFlag = false;

        // Перебираются удаляемые ветки
        for(int i = 0; i < selectitems.size(); ++i) {
            QModelIndex index = selectitems.at(i);
            auto item = _knowtreemodel->item(index);

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
            find_object<MainWindow>("mainwindow")->setEnabled(true);
            find_object<MainWindow>("mainwindow")->blockSignals(false);

            return;
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
        find_object<MainWindow>("mainwindow")->saveTextarea();

        // Сортировка списка индексов по вложенности методом пузырька
        // Индексы с длинным путем перемещаются в начало списка
        for(int i = 0; i < selectitems.size(); i++)
            for(int j = selectitems.size() - 1; j > i; j--) {
                QStringList path_1 = (_knowtreemodel->item(selectitems.at(j - 1)))->path();
                QStringList path_2 = (_knowtreemodel->item(selectitems.at(j)))->path();

                if(path_1.size() < path_2.size())
                    selectitems.swap(j - 1, j);
            }

        qDebug() << "Path for delete";

        for(int i = 0; i < selectitems.size(); ++i)
            qDebug() << (_knowtreemodel->item(selectitems.at(i)))->path();

        // Вызов удаления веток
        for(int i = 0; i < selectitems.size(); ++i)
            delete_one_branch(selectitems.at(i));

        qDebug() << "Delete finish";

        // Сохранение дерева веток
        find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

        qDebug() << "Save new tree finish";
    }

    // Разблокируется главное окно
    find_object<MainWindow>("mainwindow")->setEnabled(true);
    find_object<MainWindow>("mainwindow")->blockSignals(false);

    tree_empty_controll();
    tree_crypt_control();
}


// Удаление одной ветки и её подветок
// Delete one branch and its podvetok / garter
void TreeScreen::delete_one_branch(QModelIndex index)
{

    if(!index.isValid()) return;

    qDebug() << "Label 1";

    // Получение узла, который соответствует обрабатываемому индексу
    auto item = _knowtreemodel->item(index);

    qDebug() << "Label 2, branch id:" << item->field("id") << "name:" << item->field("name");

    // Получение пути к элементу
    QStringList path = item->path();

    qDebug() << "Label 3";

    // Получение путей ко всем подветкам
    QList<QStringList> subbranchespath = item->all_children_path();

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
        qDebug() << "Delete subbranch, id:" << _knowtreemodel->item(subbranchespath.at(i))->field("id") << "name:" << _knowtreemodel->item(subbranchespath.at(i))->field("name");
        (_knowtreemodel->item(subbranchespath.at(i)))->clear_tabledata();
        // ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
    }

    qDebug() << "Label 6";

    // Удаление таблицы конечных записей для самой удаляемой ветки
    // Удаление подчиненных элементов для самой удаляемой ветки
    qDebug() << "Delete rootbranch, id:" << _knowtreemodel->item(path)->field("id") << "name:" << _knowtreemodel->item(path)->field("name");
    (_knowtreemodel->item(path))->clear_tabledata();
    // ( knowTreeModel->getItem(path) )->removeAllChildren();

    qDebug() << "Label 7";

    // Удаление ветки на экране, при этом удалятся все подветки
    qDebug() << "This branch have row() as" << index.row();

    if(index.isValid()) qDebug() << "Index valid";
    else qDebug() << "Index non valid";

    _knowtreemodel->removeRows(index.row(), 1, index.parent());

    /*
    // Удаление всех нужных подветок
    // Пробегаются самые длинные ветки а потом более короткие
    for (int i=subbranchespath.size()-1;i>=0;i--)
     if(knowTreeModel->isItemValid(subbranchespath.at(i)))
      {
       TreeItem *current_item=knowTreeModel->getItem(subbranchespath.at(i));
       delete current_item;
      }
    */

    qDebug() << "Label 8";
}


void TreeScreen::cut_branch(void)
{
    bool copy_result;

    copy_result = copy_branch();

    if(copy_result)
        delete_branchs("cut");
}


bool TreeScreen::copy_branch(void)
{
    qDebug() << "In copy_branch()";

    // Сохраняется текст в окне редактирования
    find_object<MainWindow>("mainwindow")->saveTextarea();

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

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
    QModelIndex index = currentitem_index();

    // Получение ссылки на узел, который соответствует выделенной ветке
    auto item = _knowtreemodel->item(index);

    // Получение пути к выделенной ветке
    QStringList path = item->path();

    // Получение путей ко всем подветкам
    QList<QStringList> subbranchespath = item->all_children_path();


    // Проверка, содержит ли данная ветка как шифрованные
    // так и незашифрованные данные
    bool nocryptPresence = false;
    bool encryptPresence = false;

    // Флаги на основе состояния текущей ветки
    if(_knowtreemodel->item(path)->field("crypt") == "1")
        encryptPresence = true;
    else
        nocryptPresence = true;

    // Флаги на основе состояния подветок
    foreach(QStringList currPath, subbranchespath)
        if(_knowtreemodel->item(currPath)->field("crypt") == "1")
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
    std::shared_ptr<RecordTable> curr_item_record_table;

    // Добавление ветки
    curr_item = _knowtreemodel->item(path);
    curr_item_fields = curr_item->all_fields(); // Раньше было getAllFieldsDirect()
    branch_id = curr_item_fields["id"];

    if(is_root)
        branch_clipboard_data->addBranch("-1", curr_item_fields);
    else
        branch_clipboard_data->addBranch(curr_item->parent_id(),
                                         curr_item_fields);

    // Добавление конечных записей
    curr_item_record_table = curr_item->tabledata();

    for(int i = 0; i < curr_item_record_table->size(); i++) {
        // Полный образ записи (с файлами и текстом)
        std::shared_ptr<Record> record = curr_item_record_table->record_fat(i);

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
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

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
    find_object<MainWindow>("mainwindow")->setDisabled(true);


    // Создается ссылка на буфер обмена
    QClipboard *cbuf = QApplication::clipboard();

    // Извлечение объекта из буфера обмена
    const ClipboardBranch *branch;
    branch = qobject_cast<const ClipboardBranch *>(cbuf->mimeData());
    branch->print();
    branch->printIdTree();


    // Получение индекса выделенной строки
    QModelIndex index = currentitem_index();

    // Добавление ветки
    QString pasted_branch_id;

    if(is_branch)
        pasted_branch_id = _knowtreemodel->paste_new_sibling_branch(index, (ClipboardBranch *)branch);
    else
        pasted_branch_id = _knowtreemodel->paste_new_child_branch(index, (ClipboardBranch *)branch);


    // Установка курсора на новую созданную ветку
    auto pasted_branch_item = _knowtreemodel->item_by_id(pasted_branch_id);
    QStringList pasted_branch_path = pasted_branch_item->path();
    find_object<MainWindow>("mainwindow")->set_tree_position(pasted_branch_path);

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    // Разблокируется главное окно
    find_object<MainWindow>("mainwindow")->setEnabled(true);
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
    auto item = _knowtreemodel->item(currentitem_index());

    // Шифрация ветки и всех подветок
    item->to_encrypt();

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    // Обновляеются на экране ветка и ее подветки
    update_branch_on_screen(currentitem_index());
}


void TreeScreen::decrypt_branch_item(void)
{
    // Получаем указатель на текущую выбранную ветку дерева
    auto item = _knowtreemodel->item(currentitem_index());

    // Расшифровка ветки и всех подветок
    item->to_decrypt();

    // Сохранение дерева веток
    find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

    // Обновляеются на экране ветка и ее подветки
    update_branch_on_screen(currentitem_index());

    // Проверяется, остались ли в дереве зашифрованные данные
    // если зашифрованных данных нет, будет предложено сбросить пароль
    tree_crypt_control();
}


// Обновление на экране ветки и подветок
void TreeScreen::update_branch_on_screen(const QModelIndex &index)
{
    // Для корневой ветки дается команда чтобы модель сообщила о своем изменении
    _knowtreemodel->emit_datachanged_signal(index);

    // По модельному индексу выясняется указатель на ветку
    auto item = _knowtreemodel->item(index);

    // Перебираются подветки
    QList<QStringList> updatePathts = item->all_children_path();

    foreach(QStringList currentPath, updatePathts) {
        auto currentItem = _knowtreemodel->item(currentPath);

        QModelIndex currentIndex = _knowtreemodel->index_item(currentItem);

        // Для подветки дается команда чтобы модель сообщила о своем изменении
        _knowtreemodel->emit_datachanged_signal(currentIndex);
    }
}

void TreeScreen::to_candidate_screen(const QModelIndex &index)
{
    candidate_from_knowtree_item(index);
    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
}


void TreeScreen::candidate_from_search_result(boost::intrusive_ptr<TreeItem> resultset_item)
{

    RecordController *controller = globalparameters.table_screen()->table_controller();
    RecordModel *source_model = controller->table_model();

    assert(source_model->tree_item() == _shadow_candidate_model->_root_item);
    source_model->reset_internal_data();  // delete source_model;   // dangerous!!!
    //    std::shared_ptr<RecordTable> target_data = source_model->tree_item()->tabledata();    // resultset_item->tabledata();
    //    controller->init();




    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    auto dommodel = resultset_item->tabledata()->export_to_dom();    // source->init(startItem, QDomElement());
    resultset_item->remove_all_children();              // dommodel has noting to do with children!

    auto result_data = std::make_shared<RecordTable>(dommodel); //    resultset_item->tabledata(dommodel);               //    resultset_item->table_clear();    // already insert into table_init

    //    resultset_data->init(resultset_item, dommodel);     // just for init the item bind for reault set record table

    //    _knowtreemodel->save();

    controller->table_model()->tree_item()->tabledata(result_data);

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
    _knowtreeview->resizeColumnToContents(0);

    // Переключаются окна (используется для мобильного интерфейса)
    globalparameters.window_switcher()->switchFromTreeToRecordtable();
    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());


}

// Действия при клике на ветку дерева
void TreeScreen::candidate_from_knowtree_item(const QModelIndex &index)
{
    // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

    // Сохраняется текст в окне редактирования в соответсвующий файл
    find_object<MainWindow>("mainwindow")->saveTextarea();

    // Получаем указатель на текущую выбранную ветку дерева
    auto item = _knowtreemodel->item(index);

    // Вначале все инструменты работы с веткой включаются
    QMapIterator<QString, QAction *> i(_actionlist);

    while(i.hasNext()) {
        i.next();
        i.value()->setEnabled(true);
    }

    // Проверяется, происходит ли клик по зашифрованной ветке
    if(item->field("crypt") == "1") {
        // Если пароль доступа к зашифрованным данным не вводился в этой сессии
        if(globalparameters.crypt_key().length() == 0) {
            // Запрашивается пароль
            Password password;

            if(password.retrievePassword() == false) {
                // Устанавливаем пустые данные для отображения таблицы конечных записей
                find_object<RecordController>("table_screen_controller")->reset_tabledata(nullptr);

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

    //    _shadow_candidate_model->_root_item = item;    //? never do this!!!
    _shadow_candidate_model->_root_item->field("id", item->field("id"));
    _shadow_candidate_model->_root_item->field("name", item->field("name"));

    // Получаем указатель на данные таблицы конечных записей
    //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();







    //    for(int i = 0; i < item->tabledata()->size(); i++) {

    //        _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
    //            _shadow_candidate_model->_root_item->tabledata()->size()
    //            , item->tabledata()->record(i)
    //        );
    //    }

    //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
    //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
    _shadow_candidate_model->_root_item->tabledata(item->tabledata()->export_to_dom());







    // Устанавливаем данные таблицы конечных записей
    find_object<RecordController>(table_controller_singleton_name)->table_model()->tree_item(_shadow_candidate_model->_root_item);


    //            reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item, item->tabledata()->export_to_dom(doc)));

    // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
    if(appconfig.getInterfaceMode() == "mobile") {

        QStringList path = item->path_as_name();

        // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
        int emptyStringIndex = path.indexOf("");
        path.removeAt(emptyStringIndex);

        find_object<RecordScreen>(table_screen_singleton_name)->tree_path(path.join(" > "));

    }



    // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
    _knowtreeview->resizeColumnToContents(0);

    // Переключаются окна (используется для мобильного интерфейса)
    globalparameters.window_switcher()->switchFromTreeToRecordtable();
}


/*
void treescreen::on_knowTreeView_repaint(void)
{
 // Попытка расширить нулевую колонку с деревом, если содержимое слишком широкое
 knowTreeView->resizeColumnToContents(0);
}
*/


void TreeScreen::update_selected_branchs(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

    // Обновление на экране
    for(int i = 0; i < selectitems.size(); ++i)
        _knowtreeview->update(selectitems.at(i));
}


QItemSelectionModel *TreeScreen::selection_model(void)
{
    return _knowtreeview->selectionModel();
}


void TreeScreen::cursor_to_index(QModelIndex index)
{
    // Курсор устанавливается на нужный элемент дерева
    // В desktop-варианте на сигнал currentRowChanged() будет вызван слот on_knowtree_clicked()
    _knowtreeview->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);

    // В мобильной версии реакции на выбор ветки нет (не обрабатывается сигнал смены строки в модели выбора)
    // Поэтому по ветке должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
    // Метод clicked() публичный начиная с Qt5 (мобильный интерфейс возможен только в Qt5)
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

    if(_appconfig.getInterfaceMode() == "mobile")
        emit _knowtreeview->clicked(index);

#endif
}


// Получение номера первого выделенного элемента
int TreeScreen::first_selecteditem_row(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = _knowtreeview->selectionModel()->selectedIndexes();

    if(selectitems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectitems.at(0)).row(); // Индекс первого выделенного элемента
}


// Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::currentitem_index(void)
{
    if(0 == _knowtreemodel->_root_item->child_count()) {
        _knowtreemodel->_root_item->add_children();
    }

    if(!_knowtreeview->selectionModel()->currentIndex().isValid()) {
        _knowtreeview->selectionModel()->setCurrentIndex(
            _knowtreemodel->index_item(_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1))
            //            _knowtreemodel->createIndex(0, 0, static_cast<void *>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
            //        _knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item), 0)
            , QItemSelectionModel::SelectCurrent);
    }

    assert(_knowtreeview->selectionModel()->currentIndex().isValid());
    return _knowtreeview->selectionModel()->currentIndex();
}

// Получение индекса текущего элемента на котором стоит курсор
QModelIndex TreeScreen::last_index(void)
{
    if(0 == _knowtreemodel->_root_item->child_count()) {
        _knowtreemodel->_root_item->add_children();
    }

    //    if(!_knowtreeview->selectionModel()->currentIndex().isValid()) {
    _knowtreeview->selectionModel()->setCurrentIndex(
        _knowtreemodel->index_item(_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1))
        //            _knowtreemodel->createIndex(0, 0, static_cast<void *>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
        //        _knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item), 0)
        , QItemSelectionModel::SelectCurrent);
    //    }

    assert(_knowtreeview->selectionModel()->currentIndex().isValid());
    return _knowtreeview->selectionModel()->currentIndex();
}


// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
void TreeScreen::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count " << _knowtreemodel->rowCount();

    if(_knowtreemodel->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";

        insert_branch_process(QModelIndex(), tr("Rename me"), false);
    }
}


// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void TreeScreen::tree_crypt_control(void)
{
    // Если в дереве нет шифрования
    if(_knowtreemodel->is_contains_crypt_branches() == false) {
        // Запускается диалог сброса пароля шифрования
        Password pwd;
        pwd.resetPassword();
    }
}


// Сохранение дерева веток на диск
void TreeScreen::save_knowtree(void)
{
    _knowtreemodel->save();
}


// Перечитывание дерева веток с диска
void TreeScreen::reload_knowtree(void)
{
    _knowtreemodel->reload();
}
