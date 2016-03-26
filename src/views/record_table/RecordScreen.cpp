#include <QObject>
#include <QMimeData>
#include <QProgressBar>
#include <QMap>
#include <QList>
#include <QFontMetrics>

#include "main.h"
#include "RecordView.h"
#include "RecordScreen.h"
#include "views/record_table/verticalscrollarea.h"
#include "views/main_window/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "models/app_config/AppConfig.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/GlobalParameters.h"
#include "controllers/record_table/RecordController.h"
#include "libraries/FlatControl.h"
#include "views/browser/toolbarsearch.h"
#include "views/tree/KnowView.h"
#include "views/tree/TreeScreen.h"
#include "views/browser/entrance.h"
#include "models/record_table/RecordModel.h"
#include "views/browser/browser.h"
#include "views/browser/tabwidget.h"
#include "models/tree/KnowModel.h"

extern GlobalParameters globalparameters;
extern AppConfig appconfig;


// Виджет, который отображает список записей в ветке
// c кнопками управления

RecordScreen::RecordScreen(//TreeScreen           *_tree_screen    //,
    FindScreen          *_find_screen
    , MetaEditor        *_editor_screen
    , browser::Browser  *_browser
    , HidableTabWidget  *_vtabwidget
    , MainWindow        *_main_window
)
    : QWidget(_vtabwidget)
    , _save_in_new_branch(new QAction(tr("Save in new branch"), this))
    , _pin(new QAction(tr("Pin note"), this))
    , _addnew_to_end(new QAction(tr("Add note"), this))
    , _addnew_before(new QAction(tr("Add note before"), this))
    , _addnew_after(new QAction(tr("Add note after"), this))
    , _edit_field(new QAction(tr("Edit properties (pin, name, author, url, tags...)"), this))
    , _delete(new QAction(tr("Delete note(s)"), this))
    , _cut(new QAction(tr("&Cut note(s)"), this))
    , _copy(new QAction(tr("&Copy note(s)"), this))
    , _paste(new QAction(tr("&Paste note(s)"), this))
    , _settings(new QAction(tr("&View settings"), this))
    , _action_move_up(new QAction(tr("&Move Up"), this))
    , _action_move_dn(new QAction(tr("&Move Down"), this))
    , _find_in_base(new QAction(tr("Find in base"), this))
    , _action_syncro(new QAction(tr("Synchronization"), this))
    , _action_walk_history_previous(new QAction(tr("Previous viewing note"), this))
    , _action_walk_history_next(new QAction(tr("Next viewing note"), this))
    , _back(new QAction(tr("Back to item tree"), this))
    , _sort(new QAction(tr("Toggle sorting"), this))
    , _print(new QAction(tr("Print table"), this))
    , _toolsline(new QToolBar(this))
    , _extra_toolsline(new QToolBar(this))
    , _treepathlabel(new QLabel(this))
    , _tabmanager(new browser::TabWidget(_find_screen          // _tree_screen,
                                         , _editor_screen
                                         , this
                                         , _browser
                                         , _main_window
                                        ))
    , _record_controller(_tabmanager->record_controller())
      //    , _record_controller(new RecordController(object_name
      //                                              , _tree_screen
      //                                              , _find_screen
      //                                              , _editor_screen
      //                                              , _tabmanager
      //                                              , _main_window
      //                                              , this))

    , _vertical_scrollarea(new VerticalScrollArea(
                               std::make_shared<sd::_interface<sd::meta_info<void *>, void, QResizeEvent * >>("", &RecordView::resizeEvent, _tabmanager->record_controller()->view())
                               , this
                           ))
    , _records_toolslayout(new QHBoxLayout())
    , _records_screenlayout(new QVBoxLayout())
    , _main_window(_main_window)
      //    , _recordtree_search(new browser::ToolbarSearch(this))
{
    // Инициализируется контроллер списка записей
    //    recordTableController = new RecordTableController(this);

    // setObjectName(object_name);

    //    _table_controller->setObjectName(object_name + "_controller");

    setup_actions();

    //    _record_controller->init();

    setup_ui();
    setup_signals();
    assembly();
    _inited = true;
}


RecordScreen::~RecordScreen()
{
    //    delete _recordtree_search;
    //    delete _record_controller;
    delete _tabmanager;
    delete _vertical_scrollarea;
}

void RecordScreen::save_in_new_branch(bool checked)
{

    Q_UNUSED(checked)
    TreeScreen *_tree_screen = globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);
    auto _entrance = globalparameters.entrance();
    assert(_entrance);
    auto _tree_source_model = _tree_screen->tree_view()->source_model();  // static_cast<TreeKnowModel *>(tree_screen->tree_view()->model());

    auto _index = _tree_screen->tree_view()->current_index();

    if(_index.isValid()) {
        if(_tree_screen && _entrance) {
            //            assert(objectName() != "");

            QMap<QString, QString> data;

            QDateTime ctime_dt = QDateTime::currentDateTime();
            QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
            //            _item->field("ctime", ctime);

            data["id"]      = get_unical_id();
            data["name"]    = ctime;
            data["ctime"]   = ctime;
            data["dir"]     = data["id"];
            data["file"]    = "text.html";

            boost::intrusive_ptr<TreeItem> _result_item = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));
            //                = tree_screen->branch_add(tree_source_model, _index
            //                                          , objectName()    // tree_screen->know_branch()->root_item()   // ->field("name") // ""
            //            , [&](KnowModel * _current_model, QModelIndex _current_index, QString _id, QString _name) {
            //                boost::intrusive_ptr<TreeItem> result;
            //                auto parent = _current_model->item(_current_index)->parent();
            //                assert(parent);
            //                auto same = parent->child(parent->is_name_exists(_name));

            //                if(same) {  // && same->is_empty()
            //                    assert(same->name() == _name);
            //                    result = same;
            //                    //            children_transfer(_new_item, _current_model);
            //                } else {
            //                    // Вставка новых данных в модель дерева записей
            //                    result = _current_model->lock_sibling_add(_current_index, _id, _name);
            //                }

            //                return result;
            //            });
            //            //            tree_item->field("name", tree_screen->_shadow_page_model->_root_item->field("name"));

            //            //            assert(objectName() != "");
            //            //            into_know_branch->field("name", objectName());

            //            //            auto target = new_tree_item_in_treeknow_root;    // ->record_table();   // std::make_shared<RecordTable>(tree_item);
            auto source_model = this->record_controller()->source_model();  // ->record_table();

            for(int i = 0; i < source_model->size(); i++) {
                if(!_tree_screen->know_model_board()->item([&](boost::intrusive_ptr<TreeItem> t) {return t->id() == source_model->item(i)->field("id");})) {   // source_model->item(i)->field("id")
                    if(source_model->item(i)->is_lite())source_model->item(i)->to_fat();

                    source_model->item(i)->parent(_result_item);
                    _result_item->child_duplicate(// _result_item->work_pos(),
                        source_model->item(i));
                }
            }

            _tree_screen->view_paste_children(_tree_source_model, _index, _result_item);
            //            new_tree_item_in_treeknow_root = target;
            _tree_screen->synchronized(false);
            _tree_screen->knowtree_save();
            // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
        }
    }
}

// Настройка возможных действий
void RecordScreen::setup_actions(void)
{
    //    _save_in_new_branch = new QAction(tr("Save in new branch"), this);
    _save_in_new_branch->setStatusTip(tr("Save new records in new branch"));
    _save_in_new_branch->setIcon(QIcon(":/resource/pic/trace.svg"));

    connect(_save_in_new_branch, &QAction::triggered, this
            , //&RecordScreen::save_in_new_branch
    [&](bool checked = false) {
        Q_UNUSED(checked)

        TreeScreen *_tree_screen = globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
        assert(_tree_screen);

        browser::Entrance *_entrance = globalparameters.entrance();
        assert(_entrance);

        auto _index = _tree_screen->tree_view()->current_index();

        if(_index.isValid()) {
            if(_tree_screen && _entrance) {

                QMap<QString, QString> data;
                auto source_model = _tree_screen->tree_view()->source_model();
                auto current_root_item = source_model->item(_index);

                data["id"]      =  get_unical_id(); //current_root_item->id();     // source_model->root_item()->id();     //
                data["name"]    =  this->tabmanager()->webView(0)->page()->bounded_item()->name();    //current_root_item->name();   // source_model->root_item()->name();   //

                boost::intrusive_ptr<TreeItem> new_branch_root = boost::intrusive_ptr<TreeItem>(new TreeItem(current_root_item, data));

                bool modified = false;

                for(int w = 0; w < _entrance->browsers().size(); w++) {
                    auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

                    for(int i = 0; i < tabmanager->count(); i++) {
                        auto item = tabmanager->webView(i)->page()->bounded_item();

                        if(!_tree_screen->know_model_board()->item([&](boost::intrusive_ptr<TreeItem> t) {return t->id() == item->field("id");})) {    // item->field("id")

                            if(item->is_lite())item->to_fat();

                            item->parent(new_branch_root);
                            new_branch_root->child_move(new_branch_root->work_pos(), item);
                            modified = true;
                        }
                    }
                }


                if(modified) {
                    //                auto tree_source_model =  _tree_screen->tree_view()->source_model();
                    //                assert(tree_source_model);

                    //                new_branch_item->field("id", tree_source_model->root_item()->id());
                    //                new_branch_item->field("name", tree_source_model->root_item()->name());

                    //                    assert( // source_model->root_item()->id() == new_branch_root->id() ||
                    //                        source_model->is_item_id_exists(new_branch_root->id()));

                    _tree_screen->view_paste_sibling(//_tree_screen->view_index() // _tree_screen->know_branch()->index(0, _tree_screen->know_branch()->root_item()->current_count() - 1, QModelIndex())
                        source_model             // _tree_screen->know_branch()
                        , _tree_screen->tree_view()->current_index() //,
                        , new_branch_root
                    );


                    _tree_screen->setup_model(source_model->root_item());
                    //                _tree_screen->know_branch()->synchronized(false);
                    //                _tree_screen->save_knowtree();
                    //                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
                }
            }
        }
    }

           );

    //    _pin = new QAction(tr("Pin note"), this);
    _pin->setStatusTip(tr("Pin a note"));
    _pin->setIcon(QIcon(":/resource/pic/pin.svg"));
    connect(_pin, &QAction::triggered
    , [&](bool checked = false) {
        Q_UNUSED(checked)
        // Обновление инфополей в области редактирования записи
        MetaEditor *metaeditor = globalparameters.meta_editor();  //MetaEditor *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);

        if(metaeditor)metaeditor->switch_pin();
    }
           );

    // Добавление записи
    // a->setShortcut(tr("Ctrl+X"));
    //    _addnew_to_end = new QAction(tr("Add note"), this);
    _addnew_to_end->setStatusTip(tr("Add a new note"));
    _addnew_to_end->setIcon(QIcon(":/resource/pic/note_add.svg"));
    //    setIcon(style()->standardIcon(QStyle::SP_FileIcon, 0, this));
    connect(_addnew_to_end, &QAction::triggered, _tabmanager, &browser::TabWidget::addnew_to_end);

    // Добавление записи до
    //    _addnew_before = new QAction(tr("Add note before"), this);
    _addnew_before->setStatusTip(tr("Add a note before selected"));
    connect(_addnew_before, &QAction::triggered, _tabmanager, &browser::TabWidget::addnew_before);

    // Добавление записи после
    //    _addnew_after = new QAction(tr("Add note after"), this);
    _addnew_after->setStatusTip(tr("Add a note after selected"));
    connect(_addnew_after, &QAction::triggered, _tabmanager, &browser::TabWidget::addnew_after);

    // Редактирование записи
    //    _edit_field = new QAction(tr("Edit properties (pin, name, author, url, tags...)"), this);
    _edit_field->setStatusTip(tr("Edit note properties (pin, name, author, url, tags...)"));
    _edit_field->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(_edit_field, &QAction::triggered, _tabmanager, &browser::TabWidget::on_edit_fieldcontext);

    // Удаление записи
    //    _delete = new QAction(tr("Delete note(s)"), this);
    _delete->setStatusTip(tr("Delete note(s)"));
    _delete->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(_delete, &QAction::triggered, _tabmanager, &browser::TabWidget::delete_context);

    // Удаление записи с копированием в буфер обмена
    //    _cut = new QAction(tr("&Cut note(s)"), this);
    _cut->setStatusTip(tr("Cut notes(s) to clipboard"));
    _cut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));
    connect(_cut, &QAction::triggered, _tabmanager, &browser::TabWidget::cut);

    // Копирование записи (записей) в буфер обмена
    //    _copy = new QAction(tr("&Copy note(s)"), this);
    _copy->setStatusTip(tr("Copy note(s) to clipboard"));
    _copy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
    connect(_copy, &QAction::triggered, _tabmanager, &browser::TabWidget::copy);

    // Вставка записи из буфера обмена
    //    _paste = new QAction(tr("&Paste note(s)"), this);
    _paste->setStatusTip(tr("Paste note(s) from clipboard"));
    _paste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));
    connect(_paste, &QAction::triggered, _tabmanager, &browser::TabWidget::paste);

    // Настройка внешнего вида таблицы конечных записей
    //    _settings = new QAction(tr("&View settings"), this);
    _settings->setStatusTip(tr("Setup table view settins"));
    _settings->setIcon(QIcon(":/resource/pic/cogwheel.svg"));
    connect(_settings, &QAction::triggered, _tabmanager, &browser::TabWidget::settings);

    // Перемещение записи вверх
    //    _action_move_up = new QAction(tr("&Move Up"), this);
    _action_move_up->setStatusTip(tr("Move note up"));
    _action_move_up->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(_action_move_up, &QAction::triggered, _tabmanager, &browser::TabWidget::move_up);

    // Перемещение записи вниз
    //    _action_move_dn = new QAction(tr("&Move Down"), this);
    _action_move_dn->setStatusTip(tr("Move note down"));
    _action_move_dn->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(_action_move_dn, &QAction::triggered, _tabmanager, &browser::TabWidget::move_dn);

    // Поиск по базе (клик связывается с действием в MainWindow)
    //    _find_in_base = new QAction(tr("Find in base"), this);
    _find_in_base->setStatusTip(tr("Find in base"));
    _find_in_base->setIcon(QIcon(":/resource/pic/find_in_base.svg"));

    // Синхронизация
    //    _action_syncro = new QAction(tr("Synchronization"), this);
    _action_syncro->setStatusTip(tr("Run synchronization"));
    _action_syncro->setIcon(QIcon(":/resource/pic/synchronization.svg"));
    connect(_action_syncro, &QAction::triggered, this, &RecordScreen::on_syncro_click);

    // Перемещение по истории посещаемых записей назад
    //    _action_walk_history_previous = new QAction(tr("Previous viewing note"), this);
    _action_walk_history_previous->setShortcut(tr("Ctrl+<"));
    _action_walk_history_previous->setStatusTip(tr("Previous note has been viewing"));
    _action_walk_history_previous->setIcon(QIcon(":/resource/pic/walk_history_previous.svg"));
    connect(_action_walk_history_previous, &QAction::triggered, this, &RecordScreen::on_walkhistory_previous_click);

    // Перемещение по истории посещаемых записей вперед
    //    _action_walk_history_next = new QAction(tr("Next viewing note"), this);
    _action_walk_history_next->setShortcut(tr("Ctrl+>"));
    _action_walk_history_next->setStatusTip(tr("Next note has been viewing"));
    _action_walk_history_next->setIcon(QIcon(":/resource/pic/walk_history_next.svg"));
    connect(_action_walk_history_next, &QAction::triggered, this, &RecordScreen::on_walkhistory_next_click);

    // Кнопка Назад (Back) в мобильном интерфейсе
    //    _back = new QAction(tr("Back to item tree"), this);
    _back->setStatusTip(tr("Back to item tree"));
    _back->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
    connect(_back, &QAction::triggered, this, &RecordScreen::on_back_click);

    // Действия по сортировке
    //    _sort = new QAction(tr("Toggle sorting"), this);
    _sort->setStatusTip(tr("Enable/disable sorting by column"));
    _sort->setIcon(QIcon(":/resource/pic/sort.svg"));
    connect(_sort, &QAction::triggered, _tabmanager,  &browser::TabWidget::on_sort_click);

    // Кнопка вызова печати таблицы конечных записей
    //    _print = new QAction(tr("Print table"), this);
    _print->setStatusTip(tr("Print current notes table"));
    _print->setIcon(QIcon(":/resource/pic/drops.svg"));   //actionPrint->setIcon(QIcon(":/resource/pic/print_record_table.svg"));
    connect(_print, &QAction::triggered, _tabmanager,  &browser::TabWidget::on_print_click);

    // Сразу после создания все действия запрещены
    disable_all_actions();
}


void RecordScreen::setup_ui(void)
{
    //    _toolsline = new QToolBar(this);

    /*
    QSize toolBarIconSize(16,16);
    toolsLine->setIconSize(toolBarIconSize);
    */

    if(appconfig.getInterfaceMode() == "mobile") {
        insert_action_as_button<QToolButton>(_toolsline, _back);
        _toolsline->addSeparator();
    }

    insert_action_as_button<QToolButton>(_toolsline, _save_in_new_branch);
    insert_action_as_button<QToolButton>(_toolsline, _pin);
    insert_action_as_button<QToolButton>(_toolsline, _addnew_to_end);



    if(appconfig.getInterfaceMode() == "desktop") {
        insert_action_as_button<QToolButton>(_toolsline, _edit_field);
        insert_action_as_button<QToolButton>(_toolsline, _delete);
    }

    insert_action_as_button<QToolButton>(_toolsline, _action_walk_history_previous);
    insert_action_as_button<QToolButton>(_toolsline, _action_walk_history_next);
    insert_action_as_button<QToolButton>(_toolsline, _action_move_up);
    insert_action_as_button<QToolButton>(_toolsline, _action_move_dn);

    _toolsline->addSeparator();

    insert_action_as_button<QToolButton>(_toolsline, _cut);
    insert_action_as_button<QToolButton>(_toolsline, _copy);
    insert_action_as_button<QToolButton>(_toolsline, _paste);

    _toolsline->addSeparator();

    insert_action_as_button<QToolButton>(_toolsline, _sort);
    insert_action_as_button<QToolButton>(_toolsline, _print);
    insert_action_as_button<QToolButton>(_toolsline, _settings);

    //    _extra_toolsline = new QToolBar(this);

    if(appconfig.getInterfaceMode() == "desktop") {
        insert_action_as_button<QToolButton>(_extra_toolsline, _action_syncro);
        //        insertActionAsButton(extraToolsLine, actionWalkHistoryPrevious);
        //        insertActionAsButton(extraToolsLine, actionWalkHistoryNext);
    }

    insert_action_as_button<QToolButton>(_extra_toolsline, _find_in_base);

    //    _treepathlabel = new QLabel(this);

    //    _treepath_button = new FlatToolButton(this);

    _treepathlabel->setWordWrap(true);
    //    treePathLabel->setLineWidth(contentsRect().width());
    //    treePathLabel->setAlignment(Qt::AlignRight);
    //    treePathLabel->baseSize();

    //    int width = recordTableController->getView()->contentsRect().width();
    //    treePathLabel->setMaximumWidth(contentsRect().width());
    //    treePathLabel->setMinimumWidth(contentsRect().width());
    if(appconfig.getInterfaceMode() == "desktop")
        _treepathlabel->hide();

    //    _vertical_scrollarea = new VerticalScrollArea(
    //        std::make_shared<sd::_interface<sd::meta_info<void *>, void, QResizeEvent *>>("", &RecordView::resizeEvent, _record_controller->view())
    //        , this
    //    );

    _main_window->vtab()->addTab(this, QIcon(":/resource/pic/clover.svg"), "Browser");
}


void RecordScreen::setup_signals(void)
{
    //    connect(this, &RecordTableScreen::resizeEvent, _treepath_button
    //            // , &QLabel::resizeEvent
    //    , [this](QResizeEvent * e) {
    //        //        int width = recordTableController->getView()->contentsRect().width();
    //        _treepath_button->setMaximumWidth(
    //            e->size().width()   //
    //            //            contentsRect().width()
    //        );
    //        _treepath_button->setMinimumWidth(
    //            e->size().width()   //
    //            //            contentsRect().width()
    //        );

    //    }
    //           );
    connect(this->_find_in_base, &QAction::triggered, globalparameters.window_switcher(), &WindowSwitcher::findInBaseClick);
}


void RecordScreen::assembly(void)
{
    //    _recordtable_toolslayout = new QHBoxLayout();
    _records_toolslayout->addWidget(_toolsline);
    _records_toolslayout->addStretch();
    _records_toolslayout->addWidget(_extra_toolsline);

    //    _recordtree_searchlayout = new QHBoxLayout();
    //    _recordtree_searchlayout->addWidget(_recordtree_search);
    //    _recordtable_screenlayout = new QVBoxLayout();
    _records_screenlayout->setObjectName(objectName() + "_qvboxlayout");



    //    //    QHBoxLayout *_treepath_button_layout = new QHBoxLayout();

    //    //    _treepath_button_layout->addWidget(_treepath_button);
    //    //    _treepath_button_layout->setMargin(0);
    //    //    _treepath_button_layout->setSpacing(0);
    //    ////    _treepath_button_layout->setSizeConstraint();

    //    //    if(appconfig.getInterfaceMode() == "mobile") {
    //    recordTableScreenLayout->addWidget(
    //        //        _treepath_button_layout    //
    //        _treepath_button
    //    );
    //    //    }

    //    _recordtable_screenlayout->addLayout(_recordtree_searchlayout);
    _records_screenlayout->addLayout(_records_toolslayout);






    // how to use VerticalScrollArea class:
    RecordView *rtview = _record_controller->view(); //    auto rtview = new QWidget(this);

    rtview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    _vertical_scrollarea->setWidget(rtview);
    rtview->viewport()->installEventFilter(_vertical_scrollarea);
    QVBoxLayout *baseLayout = new QVBoxLayout();
    baseLayout->addWidget(_vertical_scrollarea);
    //    _verticalscrollarea->viewport()->installEventFilter(rtview);


    _records_screenlayout->addLayout(baseLayout); //

    //    _recordtable_screenlayout->addWidget(_table_controller->view());

    //    recordTableScreenLayout->addWidget(_verticalscrollarea);

    //    recordTableController->getView()->viewport()->installEventFilter(someFilterWidget);







    //    _recordtable_screenlayout->setSizeConstraint(QLayout::SetNoConstraint);

    setLayout(_records_screenlayout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);
    lt->setSpacing(0);

}


void RecordScreen::resizeEvent(QResizeEvent *e)
{

    _record_controller->view()->resizeEvent(e);

    QWidget::resizeEvent(e);

    //    int rw = contentsRect().width();
    //    int vw = recordTableController->getView()->contentsRect().width();
    //    int vwe = recordTableController->getView()->width();
    //    int w = width();
    //    int bw = this->baseSize().width();
    //    int border = this->geometry().width();

    //    _treepath_button->setMaximumWidth(
    //        //        e->size().width() - 10 // contentsMargins().left() - contentsMargins().right() //
    //        contentsRect().width() - 1
    //    );

    //    _treepath_button->setMinimumWidth(
    //        //        e->size().width() - 10//contentsMargins().left() - contentsMargins().right() //
    //        contentsRect().width() - 1
    //    );

}

// Отключение всех действий над записями
// (но не всех действий на панели инструментов, так как на панели инструментов есть действия, не оказывающие воздействия на записи)
void RecordScreen::disable_all_actions(void)
{
    _pin->setEnabled(false);
    _addnew_to_end->setEnabled(false);
    _addnew_before->setEnabled(false);
    _addnew_after->setEnabled(false);
    _edit_field->setEnabled(false);
    _delete->setEnabled(false);

    _cut->setEnabled(false);
    _copy->setEnabled(false);
    _paste->setEnabled(false);

    _action_move_up->setEnabled(false);
    _action_move_dn->setEnabled(false);
}


void RecordScreen::tools_update(void)
{
    qDebug() << "recordtablescreen::tools_update()";

    // Отключаются все действия
    disable_all_actions();

    //    if(!_table_controller->is_tree_item_exists())
    //        return;

    // Выясняется, содержит ли текущая ветка подчиненные ветки
    /*
    QModelIndex index = find_object<TreeScreen>(tree_screen_singleton_name)->get_selection_model()->currentIndex();
    TreeItem *item = find_object<TreeScreen>(tree_screen_singleton_name)->kntrmodel->getItem(index);
    int branch_have_children=0;
    if(item->childCount()>0)branch_have_children=1;
    */

    // Включаются те действия которые разрешены

    _pin->setEnabled(true);
    // Добавление записи
    // Добавлять можно к любой ветке
    _addnew_to_end->setEnabled(true);

    QItemSelectionModel *item_selection_model = _record_controller->view()->selectionModel();
    RecordView *view = _record_controller->view();

    // Добавление записи до
    // Добавлять "до" можно только тогда, когда выбрана только одна строка
    // и не включена сортировка
    if(item_selection_model->hasSelection()
       && (item_selection_model->selectedRows()).size() == 1
       && view->isSortingEnabled() == false
      ) {
        _addnew_before->setEnabled(true);
    }

    // Добавление записи после
    // Добавлять "после" можно только тогда, когда выбрана только одна строка
    // и не включена сортировка
    if(item_selection_model->hasSelection()
       && (item_selection_model->selectedRows()).size() == 1
       && view->isSortingEnabled() == false
      ) {
        _addnew_after->setEnabled(true);
    }

    // Редактирование записи
    // Редактировать можно только тогда, когда выбрана только одна строка
    if(item_selection_model->hasSelection()
       && (item_selection_model->selectedRows()).size() == 1
      ) {
        _edit_field->setEnabled(true);
    }

    // Удаление записи
    // Пункт активен только если запись (или записи) выбраны в списке
    if(item_selection_model->hasSelection()) {
        _delete->setEnabled(true);
    }

    // Удаление с копированием записи в буфер обмена
    // Пункт активен только если запись (или записи) выбраны в списке
    if(item_selection_model->hasSelection()) {
        _cut->setEnabled(true);
    }

    // Копирование записи в буфер обмена
    // Пункт активен только если запись (или записи) выбраны в списке
    if(item_selection_model->hasSelection()) {
        _copy->setEnabled(true);
    }

    // Вставка записи из буфера обмена
    // Вставлять записи можно только тогда, когда выбрана
    // только одна строка (добавляется после выделеной строки)
    // или не выбрано ни одной строки (тогда добавляется в конец списка)
    // или записей вообще нет
    // И проверяется, содержит ли буфер обмена данные нужного формата
    if((item_selection_model->hasSelection() && (item_selection_model->selectedRows()).size() == 1)
       || item_selection_model->hasSelection() == false
       || view->model()->rowCount() == 0
      ) {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();

        if(mimeData != nullptr) {
            if(mimeData->hasFormat("mytetra/records")) {
                _paste->setEnabled(true);
            }
        }
    }

    // Перемещение записи вверх
    // Пункт возможен только когда выбрана одна строка
    // и указатель стоит не на начале списка
    // и не включена сортировка
    if(item_selection_model->hasSelection()
       && (item_selection_model->selectedRows()).size() == 1
       && view->is_selected_set_to_top() == false
       && view->isSortingEnabled() == false
      ) {
        _action_move_up->setEnabled(true);
    }

    // Перемещение записи вниз
    // Пункт возможен только когда выбрана одна строка
    // и указатель стоит не в конце списка
    // и не включена сортировка
    if(item_selection_model->hasSelection()
       && (item_selection_model->selectedRows()).size() == 1
       && view->is_selected_set_to_bottom() == false
       && view->isSortingEnabled() == false
      ) {
        _action_move_dn->setEnabled(true);
    }

    // Обновляется состояние области редактирования текста
    if(item_selection_model->hasSelection()
       && _record_controller->row_count() > 0
      ) {
        qDebug() << "In table select present";
        qDebug() << "In table row count is" << _record_controller->row_count();
        //        find_object<MetaEditor>(meta_editor_singleton_name)
        globalparameters.meta_editor()->textarea_editable(true);
    } else {
        qDebug() << "In table select non present";
        //        find_object<MetaEditor>(meta_editor_singleton_name)
        globalparameters.meta_editor()->textarea_editable(false);
    }
}


// Получение номера первого выделенного элемента в таблице записи на экране
int RecordScreen::first_selection_pos(void)
{
    return _record_controller->first_selectionpos();
}


// Получение ID первого выделенного элемента в таблице записи на экране
QString RecordScreen::first_selection_id(void)
{
    return _record_controller->first_selectionid();
}


// Установка засветки в нужную строку в таблице записи на экране
void RecordScreen::select_pos(int pos)
{
    _tabmanager->setCurrentIndex(pos);
    _record_controller->select_pos(pos);
}


// Установка засветки в нужную запись в таблице записей на экране
void RecordScreen::select_id(QString id)
{
    _tabmanager->setCurrentIndex(_record_controller->source_model()->position(id));

    if(_record_controller->view()->selection_first_id() != id)
        _record_controller->select_id(id);
}



// Действия при нажатии кнопки синхронизации
void RecordScreen::on_syncro_click(void)
{
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->synchronization();
}


void RecordScreen::on_walkhistory_previous_click(void)
{
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->go_walk_history_previous();
}


void RecordScreen::on_walkhistory_next_click(void)
{
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->go_walk_history_next();
}


// Возвращение к дереву разделов в мобильном интерфейсе
void RecordScreen::on_back_click(void)
{
    globalparameters.window_switcher()->switchFromRecordtableToTree();
}


void RecordScreen::tree_path(QString path)
{
    _treepath = path; // Запоминается путь к ветке в виде строки
    _treepathlabel->setText(tr("<b>Path:</b> ") + _treepath);
    //    _treepath_button->setText(tr("<b>Path:</b> ") + _treepath);
}


QString RecordScreen::tree_path(void)
{
    return _treepath;
}


