#include <QDebug>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QScrollArea>


#include "main.h"
#include "KnowView.h"
#include "TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/binder.hxx"
#include "models/record_table/linker.hxx"
#include "models/tree/treeindex.hxx"
#include "models/tree/KnowModel.h"
#include "models/record_table/recordindex.hxx"
#include "models/record_table/ItemsFlat.h"
#include "models/tree/TreeItem.h"
#include "models/record_table/RecordModel.h"
#include "views/main_window/MainWindow.h"
#include "views/record_table/RecordScreen.h"
#include "views/record/MetaEditor.h"
#include "controllers/record_table/RecordController.h"
#include "views/browser/webview.h"
#include "views/browser/tabwidget.h"
#include "views/browser/entrance.h"
#include "models/tree/TreeItem.h"
#include "libraries/crypt/Password.h"
#include "libraries/ClipboardBranch.h"





extern GlobalParameters globalparameters;
extern AppConfig appconfig;
extern const char *clipboard_items_root;



extern const char *action_set_as_session_root;
extern const char *action_find_in_base;
extern const char *action_cursor_follow_root;
extern const char *action_cursor_follow_up;
extern const char *action_expand_all_subbranch;
extern const char *action_collapse_all_subbranch;
extern const char *action_move_up_branch;
extern const char *action_move_dn_branch;
extern const char *action_insert_sub_branch;
extern const char *action_insert_branch;
extern const char *action_edit_branch;
extern const char *action_delete_branch;
extern const char *action_cut_branch;
extern const char *action_copy_branch;
extern const char *action_paste_branch;
extern const char *action_paste_sub_branch;
extern const char *action_encrypt_branch;
extern const char *action_decrypt_branch;
extern const char *action_freeze_browser_view;
extern const char *action_edit_field;
extern const char *action_editor_switch;




#ifdef QT_DEBUG
const char *index_xml_file_name = "mytetra.xml";  // "mytetra_debug.xml";
#else
const char *index_xml_file_name = "mytetra.xml";
#endif

enum QItemSelectionModel::SelectionFlag current_tree_selection_mode = QItemSelectionModel::SelectionFlag::Select;   // ClearAndSelect   Current       // ClearAndSelect //  | QItemSelectionModel::SelectionFlag::Rows
enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode = QItemSelectionModel::SelectionFlag::SelectCurrent;    // Select   // SelectCurrent

const char *knowtreeview_singleton_name = "knowtreeview";

KnowView::KnowView(QString _name, TreeScreen *_tree_screen)
    : QTreeView(_tree_screen)
    , _tree_screen(_tree_screen)
    , _know_model_board(new KnowModel(index_xml_file_name, this))
    , _know_root(new KnowModel(_know_model_board->root_item(), this))
    // create custom delegate
    , _delegate(new HtmlDelegate(this))
{

    setModel(_know_root);
    //    _know_model_board->init_from_xml(appconfig.tetra_dir() + "/" + index_xml_file_name);  // _know_branch->intercept(know_root_holder::know_root()->root_item());    // init_from_xml(xml);  //
    //    int all_count = _know_model_board->count_records_all();
    //    assert(all_count > 0);

    //    _know_model_board->synchronized(true);

    //    source_model(_know_model_board->root_item());

    // Разрешение принимать Drop-события
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    setObjectName(_name);

    setMinimumSize(1, 1);                     // 150, 250

    setWordWrap(true);

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

    setSelectionMode(QAbstractItemView::ExtendedSelection);                     // SingleSelection //// MultiSelection  //

    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);                        // ScrollBarAlwaysOn


    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);
    setDragDropMode(QAbstractItemView::InternalMove);

    setMinimumSize(size());
    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));

    setItemDelegate(_delegate);





    // deleted after introduce intercept method
    //    connect(static_cast<QTreeView *>(const_cast<KnowView *>(this)), &QTreeView::setModel, this, [&](QAbstractItemModel * model) {   // does not work
    //        _know_root = static_cast<KnowModel *>(model);
    //    });






    //    void clearSelection();
    //    virtual void clearCurrentIndex();

    //Q_SIGNALS:
    //    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    //    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());                   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);

    //        QScrollArea* qscrollarea = new QScrollArea(this);
    ////        qscrollarea->setWidget(this);
    //        auto l = new QVBoxLayout();
    //        l->addWidget(qscrollarea);
    //        this->setLayout(l);

    //        qscrollarea->show();

    // Сигналы для обновления панели инструментов при изменении в selectionModel()
    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &KnowView::on_current_changed);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &KnowView::on_selection_changed);

    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if(appconfig.interface_mode() == "desktop")
        connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &KnowView::on_current_row_changed);                                  // &TreeScreen::step_into_sub_branch

    //    connect(this, &KnowView::pressed, this, &KnowView::on_pressed);


    if(appconfig.interface_mode() == "mobile")
        connect(this, &KnowView::clicked, this, &KnowView::cursor_step_into);


    if(appconfig.interface_mode() == "desktop") {
        connect(this, &KnowView::clicked, this, &KnowView::index_invoke);
        //                [&](const QModelIndex index) {
        //            //            collapse(index);
        //            //            expand(index);
        //            (isExpanded(index)) ? setExpanded(index, false) : setExpanded(index, true);
        //            auto result_item = source_model()->item(index);

        //            if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
        //        }//&TreeScreen::candidate_from_knowtree_item)

        connect(this, &KnowView::doubleClicked, this, &KnowView::cursor_step_into);                                                 // index_invoke_in_sub_branch
    }

}


KnowView::~KnowView()
{
    if(_know_root) {                   // && is_owner()
        // delete
        _know_root->deleteLater(); _know_root = nullptr;
    }

    if(_know_model_board) {
        // delete
        _know_model_board->deleteLater();
        _know_model_board = nullptr;
    }
}

void KnowView::setModel(QAbstractItemModel *model)
{
    _know_root = dynamic_cast<KnowModel *>(model);
    QTreeView::setModel(model);
}

//void KnowView::setModel(QAbstractItemModel *model)
//{
//    _know_root = static_cast<KnowModel *>(model);
//    QTreeView::setModel(model);
//}

//bool KnowView::is_owner()
//{
//    //    auto tree_screen = static_cast<TreeScreen *>(this->parent());
//    assert(_know_model_board->root_item()->parent() == nullptr);
//    return (_know_root != _know_model_board)
//           && (_know_root->root_item()->parent() != nullptr);
//}


void KnowView::sychronize()
{

    //    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());                   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);
    auto tree_view = _tree_screen->tree_view();

    if(_know_root                 // && is_owner()
        ) {               // _know_root != _tree_screen->know_branch() // KnowView is the owner of _know_root
        browser::Entrance *_entrance = globalparameters.entrance();
        assert(_entrance);

        if(_tree_screen && _entrance && _know_root->root_item()->count_direct() > 0) {

            QMap<QString, QString> data;
            //            auto _source_model = [&]() {return _tree_screen->tree_view()->source_model();};
            data["id"] = get_unical_id();                                                                                 //_know_root->root_item()->id();
            data["name"] = "current session branch item";                                                                                 // _know_root->root_item()->item_direct(0)->name();

            //            assert(_know_root->root_item()->parent());

            boost::intrusive_ptr<TreeItem> branch_item = boost::intrusive_ptr<TreeItem>(new TreeItem(
                        nullptr                                                                                                                                                   // _know_root->root_item()
                        , data
                        ));

            bool modified = false;

            for(auto &browser : _entrance->browsers()) {
                auto tabmanager = browser->tabmanager();                                                                                                                 // record_controller()->source_model();  // ->record_table();

                for(int i = 0; i < tabmanager->count(); i++) {
                    auto page_item = tabmanager->webView(i)->page()->item();

                    if(!_know_model_board->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                        return t->id() == page_item->field("id");
                    })) {

                        if(page_item->is_lite()) page_item->to_fat();

                        //                        page_item->parent(branch_item);
                        //                        _source_model()->model_move_as_child_impl(branch_item, page_item, branch_item->work_pos());  // new_branch_item->child_insert(new_branch_item->work_pos(), item);
                        branch_item << page_item;
                        modified = true;
                    }
                }
            }


            if(modified) {
                //                new_branch_item->field("id", _know_root->root_item()->id());
                //                new_branch_item->field("name", _know_root->root_item()->name());
                assert(_know_model_board->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                    return t->id() == _know_root->root_item()->id();
                }));                                                                                                                                                                                               //_know_root->root_item()->id()

                // || _know_root->root_item()->id() == _tree_screen->know_branch()->root_item()->id()

                // assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));
                boost::intrusive_ptr<TreeIndex> tree_index;

                try {
                    tree_index = new TreeIndex([&] () -> KnowModel * {
                        return _know_root;
                    }, tree_view->session_root_auto());
                } catch(std::exception &) {}

                if(tree_index) {
                    view_paste_children_from_children(
                        tree_index
                        , branch_item
                        , [&] (boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source) -> bool {
                        return target->host()->field("url") == source->host()->field("url") && target->host()->field("name") == source->host()->field("name");
                    }
                        );

                    synchronized(false);
                }

                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
            }

            _know_root->update_index(_know_root->index(tree_view->session_root_auto()));                                                                                 // _know_root->update_index(current_index());  // selectionModel()->currentIndex()    // _tree_screen->view_index()

        }
    }
}

KnowModel *KnowView::source_model() const
{
    return _know_root;
}
KnowModel *KnowView::source_model()
{
    return _know_root;
}

//void KnowView::source_model(boost::intrusive_ptr<TreeItem> _item)
//{
//    //    TreeScreen *_tree_screen = globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
//    //    assert(_tree_screen);

//    //    if(_know_root && is_owner()) {  // _know_root != _tree_screen->know_branch() // KnowView is the owner of _know_root
//    //        browser::Entrance *_entrance = globalparameters.entrance();
//    //        assert(_entrance);

//    //        if(_tree_screen && _entrance && _know_root->root_item()->current_count() > 0) {

//    //            QMap<QString, QString> data;

//    //            data["id"]      =  get_unical_id(); //_know_root->root_item()->id();
//    //            data["name"]    =  _know_root->root_item()->child(0)->name();

//    //            assert(_know_root->root_item()->parent());

//    //            boost::intrusive_ptr<TreeItem> new_branch_item = boost::intrusive_ptr<TreeItem>(new TreeItem(_know_root->root_item(), data));

//    //            bool modified = false;

//    //            for(int w = 0; w < _entrance->browsers().size(); w++) {
//    //                auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

//    //                for(int i = 0; i < tabmanager->count(); i++) {
//    //                    auto item = tabmanager->webView(i)->page()->current_item();

//    //                    if(!_tree_screen->know_model_board()->is_item_id_exists(item->field("id"))) {

//    //                        if(item->is_lite())item->to_fat();

//    //                        item->parent(new_branch_item);
//    //                        new_branch_item->insert_new_item(new_branch_item->work_pos(), item);
//    //                        modified = true;
//    //                    }
//    //                }
//    //            }


//    //            if(modified) {
//    //                //                new_branch_item->field("id", _know_root->root_item()->id());
//    //                //                new_branch_item->field("name", _know_root->root_item()->name());
//    //                assert(_tree_screen->know_model_board()->is_item_id_exists(_know_root->root_item()->id()));    // || _know_root->root_item()->id() == _tree_screen->know_branch()->root_item()->id()
//    //                // assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));

//    //                _tree_screen->insert_children(   // _tree_screen->know_branch()->index(0, _tree_screen->know_branch()->root_item()->current_count() - 1, QModelIndex())//,
//    //                    new_branch_item
//    //                    , _know_root    // _tree_screen->know_branch()
//    //                );

//    //                _tree_screen->know_model_board()->synchronized(false);
//    //                _tree_screen->save_knowtree();
//    //                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
//    //            }
//    //        }
//    //    }


//    //    static_cast<TreeScreen *>(this->parent())->know_model_save();

//    sychronize();
//    _know_model_board->save();

//    //    if(_know_root && is_owner()) {
//    //        delete _know_root;  // dangerous!
//    //        _know_root = nullptr;
//    //    }

//    reset();
//    _know_root->intercept(_item);   // =  new KnowModel(_item, this);
//    //    setModel(_know_root);
//}

void KnowView::resizeEvent(QResizeEvent *e)
{
    //    (void)e;
    resize(_tree_screen->width(), height());
    QTreeView::resizeEvent(e);
}

// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool KnowView::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }

    return QTreeView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool KnowView::gestureEvent(QGestureEvent *event)
{
    qDebug() << "In gestureEvent()" << event;

    if(QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

    return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void KnowView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

    if(gesture->state() == Qt::GestureFinished) {
        if(globalparameters.target_os() == "android") {
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
        }
    }
}


void KnowView::mousePressEvent(QMouseEvent *event)  // Q_DECL_OVERRIDE
{
    // get the buttons type
    Qt::MouseButtons mouse_button = event->buttons();
    QModelIndex _index = indexAt(event->pos());
    assert(_index.isValid());

    // only the right mouse buton
    if(mouse_button == Qt::RightButton) {
        //        TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
        //select item at cursor position
        //        QPersistentModelIndex
        QModelIndex next_index = indexAt(event->pos());

        if(next_index.isValid()) {

            //            //            //        // Получение списка индексов QModelIndex выделенных элементов
            //            //            //        qSort(_selectitems.begin(), _selectitems.end());

            //            //            selectionModel()->clear();
            //            auto selection = LocalLizeInitializer(this)();
            //            selectionModel()->select(selection, current_tree_selection_mode);
            auto it = _know_root->item(next_index);
            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root; }, it->parent(), it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                    {
                        return il == it->linker() && il->host() == it && it->parent() == il->host_parent();
                    })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            select_as_current(tree_index);

            //        // start the context menu
            //        QModelIndexList select_indexes(selectedIndexes());

            //        if(select_indexes.size() > 0 && select_indexes[0].isValid()) {
            //            _tree_screen->_context_menu->exec(event->pos());  // QCursor::pos()
            //        }
        }
    } else {
        //call the parents function
        QTreeView::mousePressEvent(event);
    }
}


void KnowView::dragEnterEvent(QDragEnterEvent *event)
{
    if(isDragableData(event)) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}


void KnowView::dragMoveEvent(QDragMoveEvent *event)
{
    if(isDragableData(event)) {
        event->acceptProposedAction();

        // Выясняется элемент дерева, над которым находится курсор
        QModelIndex index = indexAt(event->pos());

        //        // Указатель на родительский элемент, чтобы далее получить модель данных
        //        TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // В модели данных отмечается элемент дерева, над которым находится курсор
        _know_root->setData(index, QVariant(true), Qt::UserRole);

        synchronized(false);

    } else
        event->ignore();



}


//template <class X>
//bool KnowView::isDragableData(X *event)
//{
//    // Проверяется, содержит ли объект переноса данные нужного формата
//    const QMimeData *mimeData = event->mimeData();

//    if(mimeData == nullptr)
//        return false;

//    if(!(mimeData->hasFormat("mytetra/records")))
//        return false;

//    QObject *sourceObject = qobject_cast<QObject *>(event->source());

//    if(sourceObject->objectName() == "record_view") // "recordTableView"
//        return true;
//    else
//        return false;
//}


void KnowView::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent() - Start";

    if(isDragableData(event)) {
        qDebug() << "Try move record by drag and drop";

        // Извлечение объекта
        const ClipboardRecords *clipboard_records;
        clipboard_records = qobject_cast<const ClipboardRecords *>(event->mimeData());

        // Печать в консоль содержимого перетаскиваемого объекта (для отладки)
        clipboard_records->print();

        // Выясняется элемент дерева, над которым был сделан Drop
        QModelIndex index_to = indexAt(event->pos());

        // Если отпускание мышки произошло не на ветке дерева, а на свободном пустом месте в области виджета дерева
        if(!index_to.isValid())
            return;

        //        // Указатель на родительский элемент
        //        TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен Drop
        auto tree_item_drop = _know_root->item(index_to);

        //        // Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
        //        auto tree_item_drop = tree_item_drop;    // ->record_table();


        auto check_crypt = [&] (boost::intrusive_ptr<TreeItem> tree_item_drag) -> bool {
                bool result = true;
                //            // Исходная ветка в момент Drop (откуда переностся запись) - это выделенная курсором ветка
                //            QModelIndex index_from = _know_root->index(tree_item_drag);   // current_index(); // selectionModel()->currentIndex();    // find_object<TreeScreen>(tree_screen_singleton_name)

                //            //            // Выясняется ссылка на элемент дерева (на ветку), откуда переностся запись
                //            //            auto tree_item_drag = _know_root->item(index_from);

                // Если перенос происходит в ту же самую ветку
                if(tree_item_drag != tree_item_drop)
                {
                    // Если перенос происходит из не зашифрованной ветки в зашифрованную, а пароль не установлен
                    if(tree_item_drag->field("crypt") != "1" &&
                        tree_item_drop->field("crypt") == "1" &&
                        globalparameters.crypt_key().length() == 0) {
                        // Выводится уведомление что невозможен перенос без пароля
                        QMessageBox msgBox;
                        msgBox.setWindowTitle(tr("Warning!"));
                        msgBox.setText(tr("Can\'t move this item to encrypt item. Please open crypt item (entry password) before."));
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.exec();

                        result = false;
                    }
                } else{result = false; }

                return result;
            };

        // Перенос записей, хранящихся в MimeData
        // В настоящий момент в MimeData попадает только одна запись,
        // но в дальнейшем планируется переносить несколько записей
        // и здесь код подготовлен для переноса нескольких записей

        int worked = 0;

        for(int i = 0; i < clipboard_records->size(); i++) {
            // Полные данные записи
            boost::intrusive_ptr<TreeItem> tree_item_drag = clipboard_records->record(i);

            auto item_on_tree = _know_root->item([&] (boost::intrusive_ptr<const TreeItem> it) {
                return it->id() == tree_item_drag->id();
            });
            assert(item_on_tree);

            if(item_on_tree) tree_item_drag = item_on_tree;

            RecordController *_record_controller = tree_item_drag->page()->record_controller();                                                                                 // find_object<RecordController>("table_screen_controller"); // Указатель на контроллер таблицы конечных записей

            browser::TabWidget *_tabmanager = tree_item_drag->page()->view()->tabmanager();

            if(check_crypt(tree_item_drag)) {
                // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
                // В этот момент вид таблицы конечных записей показывает таблицу, из которой совершается Drag
                // TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
                if(tree_item_drag->binder())
                    _tabmanager->closeTab(_tabmanager->indexOf(tree_item_drag->page()->view()));                                                                                                                                 // _record_controller->remove_child(record->field("id"));

                // Если таблица конечных записей после удаления перемещенной записи стала пустой
                if(_record_controller->row_count() == 0) {
                    //                find_object<MetaEditor>(meta_editor_singleton_name)
                    globalparameters.meta_editor()->clear_all();                                                                                                                                                 // Нужно очистить поле редактирования чтобы не видно было текста последней удаленной записи
                }

                //            find_object<RecordScreen>(table_screen_singleton_name)
                _record_controller->record_screen()->tools_update();

                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] ()
                        {
                            return _know_root;
                        }, tree_item_drop, 0); } catch(std::exception &e) {throw e; } return tree_index; } ();
                // Добавление записи в базу
                _know_root->model_move_as_child(tree_index, tree_item_drag);                                                                                                                 // tree_item_drop->child_insert(0, record, ADD_NEW_RECORD_TO_END);

                QModelIndex index_from = _know_root->index(tree_item_drag);
                // Обновление исходной ветки чтобы было видно что записей убавилось
                _know_root->update_index(index_from);

                update(index_from.parent());

                // Сохранение дерева веток
                //            find_object<TreeScreen>(tree_screen_singleton_name)
                worked++;
            }
        }

        if(worked > 0) {
            // static_cast<TreeScreen *>(this->parent())->tree_view()->
            know_model_save();


            // Обновлении конечной ветки чтобы было видно что записей прибавилось
            _know_root->update_index(index_to);

            update(index_to);

            // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
            _know_root->setData(QModelIndex(), QVariant(false), Qt::UserRole);

            synchronized(false);
        }
    }
}



boost::intrusive_ptr<TreeItem> KnowView::session_root_auto()
{
    auto find_root = [&] () -> boost::intrusive_ptr<TreeItem> {

            auto index_list = selectionModel()->selectedIndexes();
            QList<boost::intrusive_ptr<TreeItem> > items;
            QList<QStringList> items_path_list;

            for(auto &_i : index_list) {items << _know_root->item(_i); }

            boost::intrusive_ptr<TreeItem>  final_common_parent(nullptr);

            for(auto it : items) {items_path_list << it->path_list(); }

            auto find_shortest_list = [&] () -> QStringList {
                    QStringList result;

                    for(auto &list : items_path_list)
                    {
                        if(list.size() < result.size()) result = list;
                    }

                    return result;
                };

            auto final_common_path_list = find_shortest_list();
            final_common_parent = _know_root->item(final_common_path_list);

            for(auto it : items)
            {
                if(final_common_parent != it && !final_common_parent->is_ancestor_of(it)) {
                    if(it->is_ancestor_of(final_common_parent)) final_common_parent = it;
                    else {
                        auto near_root = final_common_parent->path_list().length() > it->path_list().length() ? it : final_common_parent;

                        while(near_root->parent()) {
                            final_common_parent = near_root = near_root->parent();

                            if(near_root->is_ancestor_of(it) && near_root->is_ancestor_of(final_common_parent)) {

                                break;
                            }
                        }
                    }
                }
            }

            assert(final_common_parent);
            return final_common_parent;

        };

    auto _current_model = [&] () {
            return _know_root;
        };                                                                          // KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model; // std::bind(_source_model_func, _tree_screen->tree_view());

    auto view_session_id_defined = [&] () {
            return _current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
            return t->id() == _current_model()->session_id();
        });
        };

    auto board_session_id_defined = [&] () {
            return _know_model_board->session_id() != "" && _know_model_board->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
            return t->id() == _know_model_board->session_id();
        });
        };

    //    (void)board_session_id_defined;
    auto fix_root = [&] {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex(_current_model, find_root()); } catch(std::exception &e) {throw e; } return tree_index; } ();
        _current_model()->session_id(tree_index);
        know_model_save();
        assert(view_session_id_defined());
    };

    if(!board_session_id_defined()) {                   // if manual setted, cancel this
        fix_root();
    }

    if(!view_session_id_defined()) {
        fix_root();
    }

    //    while(!view_session_id_defined() && _current_model()->root_item() != know_model_board()->root_item()) {
    //        auto new_root = this->cursor_follow_up_one_level();

    //        assert(new_root->count_direct() != 0);

    //        _current_model()->session_id(TreeIndex(_current_model, find_root()));
    //    }

    assert(_current_model()->session_id() != "");
    assert(view_session_id_defined());
    return view_session_id_defined();
}

//QString TreeScreen::session_root() {return _session_id;}
void KnowView::session_root_manual(bool checked)
{
    Q_UNUSED(checked);
    auto _source_model = [&] () {
            return source_model();
        };
    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex(_source_model, _source_model()->item(current_index())); } catch(std::exception &e) {throw e; } return tree_index; } ();
    _source_model()->session_id(tree_index);
}

boost::intrusive_ptr<TreeItem> KnowView::current_item()
{
    boost::intrusive_ptr<TreeItem> r(nullptr);
    auto result = current_index();

    if(result.isValid())
        r = _know_root->item(result);                                  // selectionModel()->currentIndex()
    else {
        r = _know_root->item(_previous_index);

        if(!r && selectionModel()->selectedIndexes().size() > 0) {
            r = _know_root->item(selectionModel()->selectedIndexes().last());
            boost::intrusive_ptr<TreeIndex> tree_index;
            try {tree_index = new TreeIndex([&] {return _know_root; }, r->parent(), r->linker()->sibling_order()); } catch(std::exception &e) {}

            if(tree_index) result = select_as_current(tree_index);
        }

        if(!result.isValid()) {
            boost::intrusive_ptr<TreeItem> it = session_root_auto();
            assert(it);

            if(it == _know_model_board->root_item()) {                                                                                 // !result.isValid()

                //            it = _know_root->item(result);
                //        } else {

                if(_know_root->root_item()->count_direct() == 0) {
                    //            selection_to_pos(_know_root->root_item()->item_direct(0));    //_know_root->root_item()->count_direct() - 1
                    //        } else {
                    //                TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());
                    //                auto view = _tree_screen->tree_view();

                    boost::intrusive_ptr<TreeIndex> tree_root_index;

                    try {
                        tree_root_index = new TreeIndex([&] () -> KnowModel* {
                            return _know_root;
                        }, _know_root->root_item(), 0);
                    } catch(std::exception &e) {throw e; }

                    it = tree_root_index->item_bind(
                        nullptr
                        , QUrl(browser::Browser::_defaulthome)
                        , std::bind(&KnowView::view_paste_child, this, tree_root_index, std::placeholders::_2, std::placeholders::_3)
                        , [&] (boost::intrusive_ptr<const TreeItem> it_) -> bool {
                        return it_->field("url") == browser::Browser::_defaulthome;
                    }
                        );


                    //            new_item = _know_root->root_item()->item_direct(0); // _know_root->root_item()->count_direct() - 1
                }

                //                else if(_know_root->root_item()->parent()) {
                //                    //            auto parent_tree_screen_pointer = globalparameters.tree_screen();
                //                    do {
                //                        if(!_know_root->root_item()->parent())break;

                //                        // _tree_screen->tree_view()->reset();
                //                        // KnowModel*(KnowView::*source_model_)() = &KnowView::source_model;
                //                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&]()->KnowModel* {return source_model();}, _know_root->root_item()->parent());} catch(std::exception &e) {throw e;} return tree_index;}();
                //                        intercept(tree_index);    // std::bind(source_model_, this)

                //                    } while(_know_root->root_item()->parent() && _know_root->root_item()->parent()->count_direct() == 0);

                //                    //                _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
                //                }


                it = _know_root->root_item()->item_direct(0);


            }

            //        else {
            //            auto entrance = globalparameters.entrance();
            //            auto browser = entrance->activated_browser();

            //            //        if(!browser) {
            //            //            browser = entrance->new_browser();
            //            //        }

            //            if(browser->tabmanager()->count() > 0) {
            //                it = browser->tabmanager()->currentWebView()->page()->binder()->item();
            //            } else {
            //                RecordModel::ModelIndex record_modelindex([&] {return browser->record_screen()->record_controller()->source_model();}, nullptr, it);
            //                it = browser->item_bind(record_modelindex);
            //            }


            //            result = select_as_current(TreeIndex([&] {return _know_root;}, it->parent(), it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == it->linker() && il->host() == it && it->parent() == il->host_parent();})));

            //            //        if(selectionModel()->selectedIndexes().size() > 1) {
            //            //            result = selectionModel()->selectedIndexes().first();
            //            //        }
            //        }
            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root; }, it->parent(), it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                    {
                        return il == it->linker() && il->host() == it && it->parent() == il->host_parent();
                    })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            result = select_as_current(tree_index);
            //        result = selectionModel()->currentIndex();

            assert(result.isValid());                                                                                 // this line is to be recovery
        }

    }

    return r;
}


//QModelIndex KnowView::current_index(void)const
//{
//    QModelIndex result = selectionModel()->currentIndex();

//    if(!result.isValid()) {
//        result = QModelIndex();
//        QMessageBox msgBox;
//        msgBox.setWindowTitle(tr("Warning!"));
//        msgBox.setText(tr("Invalid index, please select an item from tree screen."));
//        msgBox.setIcon(QMessageBox::Information);
//        msgBox.exec();

//    }

//    return result;
//}

// Get the index of the current element on which the cursor is positioned   // Получение индекса текущего элемента на котором стоит курсор
QModelIndex KnowView::current_index(void) const
{
    QModelIndex result = selectionModel()->currentIndex();
    //    if(!selectionModel()->currentIndex().isValid()) {

    //        boost::intrusive_ptr<TreeItem> _item = nullptr;
    //        TreeKnowModel *current_model = static_cast<TreeKnowModel *>(model());

    //        if(0 == current_model->root_item()->current_count()) {
    //            return_to_root();   //
    //            current_model = static_cast<TreeKnowModel *>(model());
    //            _item = current_model->root_item()->add_child();
    //        } else {
    //            _item = current_model->root_item()->child(0);  // _know_branch->root_item()->current_count() - 1

    //        }

    //        assert(_item);

    //        QModelIndex cur_index;



    //        //        reset();
    //        //        setModel(current_model);

    //        if(_item) {
    //            cur_index = current_model->index(_item);    // item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent()) // _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1)

    //            assert(cur_index.isValid());
    //            assert(current_model->item(cur_index) == _item);
    //            //            candidate_from_knowtree_item(cur_index);

    //            assert(model());

    //            selectionModel()->setCurrentIndex(cur_index // , current_tree_selection_mode    //
    //                                                          , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //                                                         );
    //            selectionModel()->select(cur_index  //, current_tree_selection_mode
    //                                                 , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //                                                );
    //            //            clicked(cur_index);
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
    //        //                , current_tree_selection_mode
    //        //            );
    //        //        }
    //    }

    //    auto v = _treeknowview->selectionModel()->currentIndex();
    //    candidate_from_knowtree_item(cur_index);



    //    if(!result.isValid()) {
    //        boost::intrusive_ptr<TreeItem> it = session_root_auto();
    //        assert(it);

    //        if(!it || it == _know_model_board->root_item()) { // !result.isValid()

    //            //            it = _know_root->item(result);
    //            //        } else {

    //            if(_know_root->root_item()->count_direct() == 0) {
    //                //            selection_to_pos(_know_root->root_item()->item_direct(0));    //_know_root->root_item()->count_direct() - 1
    //                //        } else {
    //                //                TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());
    //                //                auto view = _tree_screen->tree_view();

    //                if(_know_root->root_item()->parent()) {
    //                    //            auto parent_tree_screen_pointer = globalparameters.tree_screen();
    //                    do {
    //                        if(!_know_root->root_item()->parent())break;

    //                        // _tree_screen->tree_view()->reset();
    //                        // KnowModel*(KnowView::*source_model_)() = &KnowView::source_model;
    //                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&]()->KnowModel* {return source_model();}, _know_root->root_item()->parent());} catch(std::exception &e) {throw e;} return tree_index;}();
    //                        intercept(tree_index);    // std::bind(source_model_, this)

    //                    } while(_know_root->root_item()->parent() && _know_root->root_item()->parent()->count_direct() == 0);

    //                    //                _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
    //                } else if(_know_root->root_item()->count_direct() == 0) {
    //                    boost::intrusive_ptr<TreeIndex> tree_root_index;

    //                    try {
    //                        tree_root_index = new TreeIndex([&]()->KnowModel* {return _know_root;}, _know_root->root_item(), 0);
    //                    } catch(std::exception &e) {throw e;}

    //                    item_bind(
    //                        nullptr
    //                        , QUrl(browser::Browser::_defaulthome)
    //                        , std::bind(&KnowView::view_paste_child, this, tree_root_index, std::placeholders::_2, std::placeholders::_3)
    //                    );
    //                }

    //                //            new_item = _know_root->root_item()->item_direct(0); // _know_root->root_item()->count_direct() - 1
    //            }

    //            //            else {
    //            it = _know_root->root_item()->item_direct(0);
    //            //            }

    //        }

    //        //        else {
    //        //            auto entrance = globalparameters.entrance();
    //        //            auto browser = entrance->activated_browser();

    //        //            //        if(!browser) {
    //        //            //            browser = entrance->new_browser();
    //        //            //        }

    //        //            if(browser->tabmanager()->count() > 0) {
    //        //                it = browser->tabmanager()->currentWebView()->page()->binder()->item();
    //        //            } else {
    //        //                RecordModel::ModelIndex record_modelindex([&] {return browser->record_screen()->record_controller()->source_model();}, nullptr, it);
    //        //                it = browser->item_bind(record_modelindex);
    //        //            }


    //        //            result = select_as_current(TreeIndex([&] {return _know_root;}, it->parent(), it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == it->linker() && il->host() == it && it->parent() == il->host_parent();})));

    //        //            //        if(selectionModel()->selectedIndexes().size() > 1) {
    //        //            //            result = selectionModel()->selectedIndexes().first();
    //        //            //        }
    //        //        }
    //        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root;}, it->parent(), it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == it->linker() && il->host() == it && it->parent() == il->host_parent();}));} catch(std::exception &e) {throw e;} return tree_index;}();
    //        result = select_as_current(tree_index);
    //        //        result = selectionModel()->currentIndex();

    //        assert(result.isValid());    // this line is to be recovery
    //    }

    //    assert(cur_index.isValid());
    return result;                  // cur_index;   // temporary setting???   //
    //        selectionModel()->currentIndex();
}

//// Получение индекса текущего элемента на котором стоит курсор
//QModelIndex KnowView::view_index_last(void)const
//{
//    //    if(!selectionModel()->currentIndex().isValid()) {
//    boost::intrusive_ptr<TreeItem> _item = nullptr;

//    //    KnowModel *_know_root = _know_root;  //source_model();

//    if(0 == _know_root->root_item()->current_count()) {
//        static_cast<TreeScreen *>(parent())->view_return_to_root();  //
//        //        _know_root = source_model();
//        _item = _know_root->root_item()->add_child();
//    } else {
//        _item = _know_root->root_item()->child(_know_root->root_item()->current_count() - 1);
//    }

//    assert(_item);
//    QModelIndex cur_index = _know_root->index(_item);    // item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent()) // _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1)

//    assert(cur_index.isValid());
//    assert(_know_root->item(cur_index) == _item);
//    //        candidate_from_knowtree_item(cur_index);

//    //    if(!_knowtreeview->selectionModel()->currentIndex().isValid()) {
//    selectionModel()->setCurrentIndex(
//        cur_index
//        //        _knowtreemodel->index(_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() - 1))
//        //            _knowtreemodel->createIndex(0, 0, static_cast<void *>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
//        //        _knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item), 0)
//        // , current_tree_selection_mode
//        , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
//    );
//    //    }
//    selectionModel()->select(cur_index
//                             // , current_tree_selection_mode
//                             , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
//                            );
//    //        clicked(cur_index);
//    //    candidate_from_knowtree_item(cur_index);

//    //    }

//    //    assert(selectionModel()->currentIndex().isValid());    // this line is to be recovery
//    return selectionModel()->currentIndex();
//}

QModelIndex KnowView::select_as_current(boost::intrusive_ptr<TreeIndex> _modelindex
    , select_strategy _select_strategy
    , current_strategy _current_strategy
    )
{
    QModelIndex _result;
    //    QModelIndex _current_index = _modelindex.parent_index();
    QModelIndex _aim_to_be_current_index = _modelindex->current_model()()->index(_modelindex->current_item());
    auto _item = _modelindex->current_item();


    //    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());                   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);

    //    auto _item = _know_root->item(_parent_index);


    _item->binder() ? _item->binder()->page() ? _item->binder()->page()->record_controller() ?[&] {auto c = _item->binder()->page()->record_controller(); c->update_record_view(_item); return c->cursor_to_index(c->index<PosProxy>(_item)); } () : void(0) : void(0) : void(0);



    if(_aim_to_be_current_index.isValid() && _aim_to_be_current_index != selectionModel()->currentIndex()) {                   // _index_current //


        //    int pos = index.row();


        //    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
        //    if(pos != to_pos) {
        //        QMessageBox msgBox;
        //        msgBox.setText("In RecordView::setSelectionToPos() input pos not equal model pos");
        //        msgBox.exec();
        //    }

        //    int rowCount = _know_root->root_item()->count_direct();

        //    if(pos < rowCount) {  // pos > (rowCount - 1)   // return;
        _result = _select_strategy(this, _aim_to_be_current_index);
        _result = _current_strategy(this, _aim_to_be_current_index, 0);
        assert(_aim_to_be_current_index == _result);

        //    //    // Простой механизм выбора строки. Похоже, что его использовать не получится
        //    //    selectionModel()->select(_result
        //    //                             , (_result == _index) ? QItemSelectionModel::SelectionFlag::Select : QItemSelectionModel::SelectionFlag::Deselect
        //    //                            ); // QItemSelectionModel::SelectCurrent // current_tree_selection_mode
        //    // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current


        //    //    auto recordSourceModel = controller->getRecordTableModel();
        //    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

        //    // Установка засветки на нужный индекс
        //    // Set the backlight to the desired index
        //    selectionModel()->setCurrentIndex(_result, current_tree_current_index_mode); // QItemSelectionModel::SelectCurrent    // ClearAndSelect   // current_tree_selection_mode

        //    //    assert(_result == selectionModel()->currentIndex());

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

        // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
        // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
        // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
        // Therefore, the recording must be made a virtual click to fill the final table of records
        if(appconfig.interface_mode() == "mobile")
            emit this->clicked(_aim_to_be_current_index);                                                                 // QModelIndex selIdx = recordSourceModel->index(pos, 0);

#endif

        // emit this->clicked(index);
        //    _result = currentIndex();

        scrollTo(_aim_to_be_current_index);                                                 // QAbstractItemView::PositionAtCenter

        //    this->setFocus();   // ?

        //    }
    } else if(!_aim_to_be_current_index.isValid()) {
        _result = selectionModel()->currentIndex();
    } else {
        _result = _aim_to_be_current_index;
    }


    //    assert(_aim_to_be_current_index == selectionModel()->currentIndex());
    assert(_result == selectionModel()->currentIndex());
    return _result;
}


//QModelIndex KnowView::select_as_current_item(boost::intrusive_ptr<TreeItem> _item
//                                             , select_strategy _select_strategy
//                                             , current_strategy _current_strategy
//                                            )
//{

//    auto select_as_current_index = [&](const QModelIndex & _index
//                                       , select_strategy _select_strategy = [](KnowView * const v, const QModelIndex & _i) ->QModelIndex{assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
//                                               , current_strategy _current_strategy = [](KnowView * const v, const QModelIndex & _i, const int dummy = 0) ->QModelIndex{(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i;}
//    )->QModelIndex {
//        QModelIndex _result;
//        //    int pos = index.row();

//        //    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
//        //    if(pos != to_pos) {
//        //        QMessageBox msgBox;
//        //        msgBox.setText("In RecordView::setSelectionToPos() input pos not equal model pos");
//        //        msgBox.exec();
//        //    }

//        //    int rowCount = _know_root->root_item()->count_direct();

//        //    if(pos < rowCount) {  // pos > (rowCount - 1)   // return;
//        _result = _select_strategy(this, _index);
//        _result = _current_strategy(this, _result, 0);

//        //    //    // Простой механизм выбора строки. Похоже, что его использовать не получится
//        //    //    selectionModel()->select(_result
//        //    //                             , (_result == _index) ? QItemSelectionModel::SelectionFlag::Select : QItemSelectionModel::SelectionFlag::Deselect
//        //    //                            ); // QItemSelectionModel::SelectCurrent // current_tree_selection_mode
//        //    // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current


//        //    //    auto recordSourceModel = controller->getRecordTableModel();
//        //    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

//        //    // Установка засветки на нужный индекс
//        //    // Set the backlight to the desired index
//        //    selectionModel()->setCurrentIndex(_result, current_tree_current_index_mode); // QItemSelectionModel::SelectCurrent    // ClearAndSelect   // current_tree_selection_mode

//        //    //    assert(_result == selectionModel()->currentIndex());

//#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

//        // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
//        // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
//        // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
//        // Therefore, the recording must be made a virtual click to fill the final table of records
//        if(appconfig.interface_mode() == "mobile")
//            emit this->clicked(_result); // QModelIndex selIdx = recordSourceModel->index(pos, 0);

//#endif

//        // emit this->clicked(index);
//        //    _result = currentIndex();

//        scrollTo(_result);   // QAbstractItemView::PositionAtCenter

//        //    this->setFocus();   // ?

//        //    }
//        return _result;
//    };

//    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
//    assert(_tree_screen);
//    // auto _tree_screen = globalparameters.tree_screen();

//    QModelIndex _index = _know_root->index(_item);  // = _tree_screen->tree_view()->source_model()->index(_item);

//    while(!_index.isValid()
//          && _know_root->root_item() != _know_model_board->root_item()
//         ) {
//        cursor_follow_up_one_level();
//        _index = _know_root->index(_item);
//        //                _index_item = _tree_screen->tree_view()->source_model()->index(_item);
//    }


//    _item->binder() ? [&] {auto c = _item->binder()->page()->record_controller(); auto v = _item->binder()->page()->record_controller()->view(); c->update_record_view(_item); return c->cursor_to_index(_item);}() : void(0);

//    //    QModelIndex _index_current;

//    //    try {
//    //        _index_current = selectionModel()->currentIndex();
//    //    } catch(std::exception &e) {qDebug() << e.what();}

//    if(_index.isValid() && _index != selectionModel()->currentIndex()) {   // _index_current //
//        // _tree_screen->tree_view()->selectionModel()->setCurrentIndex(_index, QItemSelectionModel::SelectionFlag::SelectCurrent);
//        _index = select_as_current_index(_index, _select_strategy, _current_strategy);  // [](KnowView * v, const QModelIndex & _i) ->QModelIndex{v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
//    }






//    //    QModelIndex _index = _know_root->index(_item);  //_record_controller->pos_to_proxyindex(to_pos); // Модельный индекс в Proxy модели

//    //    _index = select_and_current(_index, _strategy);

//    return _index;
//}

//QModelIndex KnowView::deselect(const QModelIndex &_index)
//{
//    QModelIndex _result;

//    selectionModel()->select(_index, QItemSelectionModel::SelectionFlag::Deselect);
//    _result = selectionModel()->selectedIndexes().last();
//    selectionModel()->setCurrentIndex(_result, QItemSelectionModel::SelectionFlag::Current);
//    assert(_result == currentIndex());

//    // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
//    // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
//    // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
//    // Therefore, the recording must be made a virtual click to fill the final table of records
//    if(appconfig.getInterfaceMode() == "mobile")
//        emit this->clicked(_result); // QModelIndex selIdx=recordSourceModel->index(pos, 0);

//    // emit this->clicked(index);
//    //    _result = currentIndex();

//    scrollTo(_result);   // QAbstractItemView::PositionAtCenter

//    this->setFocus();   // ?
//    return _result;
//}

//QModelIndex KnowView::deselect(boost::intrusive_ptr<TreeItem> _item)
//{
//    QModelIndex _index = _know_root->index(_item);
//    _index = deselect(_index);

//    return _index;
//}

//QModelIndex KnowView::selection_to_pos(int _index)
//{
//    //    bool result = false;
//    QModelIndex index;

//    if(_index != -1) {
//        auto item = _know_root->root_item()->item_direct(_index);
//        index = selection_to_pos(item);
//        //        result = true;
//    }

//    return index;
//}


void KnowView::update_selected_indexes(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = selectionModel()->selectedIndexes();

    // Обновление на экране
    for(int i = 0; i < selectitems.size(); ++i) {
        update(selectitems.at(i));
    }
}


KnowView::LocalLizeInitializer::LocalLizeInitializer(KnowView *_tree_view)
    : _tree_view(_tree_view)
    //    , _index_list(_index_list)
    //    , _index(_index)
{
    //    assert(_index == _tree_view->current_index());
    if(-1 == _tree_view->selectionModel()->selectedIndexes().indexOf(_tree_view->current_index())) throw std::exception();

    assert(_tree_view->selectionModel()->selectedIndexes().indexOf(_tree_view->current_index()) != -1);


}


QItemSelection KnowView::LocalLizeInitializer::operator()()
{

    std::function<bool(const QModelIndex &, const QModelIndex &)> find_in_ancestor = [] (const QModelIndex & _i, const QModelIndex & _p) {
            bool result = false;
            QModelIndex _i_copy = _i;

            while(_i_copy.parent().isValid()) {                                             // && _i_copy.parent().column() >= _p.column()?
                _i_copy = _i_copy.parent();

                if(_i_copy == _p) {
                    result = true;
                    break;
                }
            }

            return result;
        };

    auto is_index_localized = [&] (const QModelIndexList _origin_index_list) -> bool {
            bool result = true;

            if(_origin_index_list.size() > 0)
            {
                //            int deepest_column = 0;

                //            for(int i = 0; i < _origin_index_list.size(); ++i) {
                //                QModelIndex index = _origin_index_list.at(i);

                //                if(index.column() > deepest_column)deepest_column = index.column();
                //            }

                //            //        assert(_origin_index_list.last() == current_index()); // means duplicated item found
                //preparations
                auto _index_current = _tree_view->current_index();                                                                             // _origin_index_list.last();    //     //

                if(_index_current.isValid()) {
                    auto _index_common_parent = _index_current.parent();


                    for(int i = 0; i < _origin_index_list.size(); ++i) {
                        QModelIndex index = _origin_index_list.at(i);

                        if(index.isValid() && (index.parent() != _index_common_parent || !find_in_ancestor(index, _index_common_parent))) {                                                                                                                                             // || index.column() != deepest_column
                            result = false; break;
                        }
                    }
                }
            }

            return result;
        };

    auto index_localize = [&] (const QModelIndexList & _origin_index_list) -> QModelIndexList {
            QModelIndexList _selectitems;
            int deepest_column = 0;

            if(_origin_index_list.size() > 0)
            {
                //        auto _source_model = source_model();
                // На время удаления блокируется главное окно
                //        //    find_object<MainWindow>("mainwindow")
                //        globalparameters.mainwindow()->setDisabled(true);
                //        //    find_object<MainWindow>("mainwindow")
                //        globalparameters.mainwindow()->blockSignals(true);

                //        assert(_origin_index_list.first() == current_index());

                if(!_origin_index_list.contains(_tree_view->current_index())) {

                    //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
                    auto _source_model = [&] () {
                            return _tree_view->_know_root;
                        };                                                                                                                                                                   // std::bind(_source_model_func, _tree_view);

                    //            QModelIndex _current_index;
                    boost::intrusive_ptr<TreeItem> duplicated_item = _tree_view->current_item();
                    boost::intrusive_ptr<TreeItem> found;

                    for(int i = 0; i < _origin_index_list.size(); ++i) {
                        auto _index = _origin_index_list.at(i);

                        if(_index.column() > deepest_column) deepest_column = _index.column();

                        if(duplicated_item->id() == _source_model()->item(_index)->id()) {
                            //                    _current_index = _index;
                            found = _source_model()->item(_index);
                            break;
                        }
                    }

                    //            TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());

                    if(found && duplicated_item && found != duplicated_item) {
                        found = static_cast<KnowModel *>(_source_model())->model_merge_to_left(found, duplicated_item
                                , std::bind(&KnowView::view_delete_permanent, _tree_view, std::placeholders::_1, std::placeholders::_2, &KnowModel::model_delete_permanent_recursive, "cut", false)                                                                                                                                             // , std::placeholders::_4, std::placeholders::_5
                                );
                    }
                }


                //preparations
                auto _index_current = _tree_view->current_index();                                                                                //_origin_index_list.first();

                if(_index_current.isValid()) {
                    auto _index_common_parent = _index_current.parent();
                    //    auto index_to_be_delete_last = _index_list.last();
                    //        auto _item_to_be_deleted_first = _source_model->item(index_to_be_delete_first);
                    //        auto deleted_position_first = _item_to_be_deleted_first->sibling_order(); // items_update_selected();
                    //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();
                    //        auto _item_parent = _item_to_be_deleted_first->parent();



                    //        // Список имен веток, которые нужно удалить
                    //        QStringList branches_name;

                    for(int i = 0; i < _origin_index_list.size(); ++i) {
                        QModelIndex index = _origin_index_list.at(i);

                        if(index.isValid() && (index.parent() == _index_common_parent || find_in_ancestor(index, _index_common_parent))) {                                                                                                                                             //  && index.column() == deepest_column
                            //                auto it = _source_model->item(index);
                            //                branches_name << it->field("name");
                            _selectitems.push_back(index);
                        }
                    }
                }
            }

            return _selectitems;
        };

    auto _index_list = _tree_view->selectionModel()->selectedIndexes();
    // Получение списка индексов QModelIndex выделенных элементов
    auto _selectitems = is_index_localized(_index_list) ? _index_list : index_localize(_index_list);

    qSort(_selectitems.begin(), _selectitems.end());
    return QItemSelection(_selectitems.first(), _selectitems.last());
}

//QModelIndexList KnowView::index_localize(const QModelIndexList _origin_index_list)
//{
//    QModelIndexList _selectitems;
//    int deepest_column = 0;

//    if(_origin_index_list.size() > 0) {
//        //        auto _source_model = source_model();
//        // На время удаления блокируется главное окно
//        //        //    find_object<MainWindow>("mainwindow")
//        //        globalparameters.mainwindow()->setDisabled(true);
//        //        //    find_object<MainWindow>("mainwindow")
//        //        globalparameters.mainwindow()->blockSignals(true);

//        //        assert(_origin_index_list.first() == current_index());

//        if(!_origin_index_list.contains(current_index())) {

//            //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
//            auto _source_model = [&]() {return _know_root;}; // std::bind(_source_model_func, _tree_view);

//            //            QModelIndex _current_index;
//            boost::intrusive_ptr<TreeItem> duplicated_item = current_item();
//            boost::intrusive_ptr<TreeItem> found;

//            for(int i = 0; i < _origin_index_list.size(); ++i) {
//                auto _index = _origin_index_list.at(i);

//                if(_index.column() > deepest_column)deepest_column = _index.column();

//                if(duplicated_item->id() == _source_model()->item(_index)->id()) {
//                    //                    _current_index = _index;
//                    found = _source_model()->item(_index);
//                    break;
//                }
//            }

//            //            TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());

//            if(found && duplicated_item && found != duplicated_item) {
//                found = static_cast<KnowModel *>(_source_model())->model_merge_to_left(
//                            std::bind(&KnowView::view_delete_permanent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
//                            , found, duplicated_item
//                        );
//            }
//        }


//        //preparations
//        auto _index_current         = current_index();    //_origin_index_list.first();
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

//            if(index.isValid() && index.parent() == _index_common_parent && index.column() == deepest_column) {
//                //                auto it = _source_model->item(index);
//                //                branches_name << it->field("name");
//                _selectitems.push_back(index);
//            }
//        }
//    }

//    return _selectitems;
//}


//bool KnowView::is_index_localized(const QModelIndexList _origin_index_list)const
//{
//    bool result = true;

//    if(_origin_index_list.size() > 0) {
//        int deepest_column = 0;

//        for(int i = 0; i < _origin_index_list.size(); ++i) {
//            QModelIndex index = _origin_index_list.at(i);

//            if(index.column() > deepest_column)deepest_column = index.column();
//        }

//        //        assert(_origin_index_list.last() == current_index()); // means duplicated item found
//        //preparations
//        auto _index_current = _origin_index_list.last();    // current_index();    //

//        auto _index_common_parent = _index_current.parent();


//        for(int i = 0; i < _origin_index_list.size(); ++i) {
//            QModelIndex index = _origin_index_list.at(i);

//            if(index.isValid() && (index.parent() != _index_common_parent || index.column() != deepest_column)) {
//                result = false; break;
//            }
//        }
//    }

//    return result;
//}

HtmlDelegate::HtmlDelegate(KnowView *_tree_view) : _tree_view(_tree_view)
{

}

// http://stackoverflow.com/questions/1956542/how-to-make-item-view-render-rich-html-text-in-qt
void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // QStyleOptionViewItemV4
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    auto source_model = [&] () {
            return _tree_view->source_model();
        };
    auto current_item = source_model()->item(index);
    QTextDocument doc;

    if(index == source_model()->index([&] (boost::intrusive_ptr<const Linker> it) {
        return it->host()->id() == source_model()->session_id();
    })) {
        optionV4.text = "<b>" + optionV4.text + "</b>";
    }
    doc.setHtml(optionV4.text);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if(optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    if(0 == current_item->count_direct() && !(optionV4.state & QStyle::State_Selected))
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Inactive, QPalette::Shadow));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);

    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // QStyleOptionViewItemV4
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}

KnowModel *KnowView::know_model_board() const
{
    return _know_model_board;
}
//KnowModel *TreeScreen::know_model_board() {return _know_model_board;}


// Шифрование ветки
void KnowView::view_encrypt(void)
{
    auto item_encrypt = [&] (void) -> void {
            auto _index = current_index();

            if(_index.isValid())
            {
                //            auto _know_root = source_model();

                // Получаем указатель на текущую выбранную ветку дерева
                auto item = _know_root->item(_index);

                // Шифрация ветки и всех подветок
                item->to_encrypt();

                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                sychronize();
                _know_model_board->save();

                // Обновляеются на экране ветка и ее подветки
                _know_root->update_index(_index);
            }
        };


    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true)                                                 // Если пароль введен правильно
            item_encrypt();                                                                 // Ветка шифруется
    } else {
        // Иначе считается, что шифрующий ключ уже был задан и он правильный

        item_encrypt();                                                 // Ветка шифруется
    }
}



// Расшифровка ветки
void KnowView::view_decrypt(void)
{

    auto item_decrypt = [&] (void) -> void {
            auto _index = current_index();

            if(_index.isValid())
            {
                //            auto _know_root = source_model();
                // Получаем указатель на текущую выбранную ветку дерева
                auto item = _know_root->item(_index);

                // Расшифровка ветки и всех подветок
                item->to_decrypt();

                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                sychronize();
                _know_model_board->save();

                // Обновляеются на экране ветка и ее подветки
                _know_root->update_index(_index);

                // Проверяется, остались ли в дереве зашифрованные данные
                // если зашифрованных данных нет, будет предложено сбросить пароль
                // static_cast<TreeScreen *>(parent())->
                tree_crypt_control();
            }
        };


    // Если пароль в данной сессии не вводился
    if(globalparameters.crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;

        if(password.retrievePassword() == true)                                                 // Если пароль введен правильно
            item_decrypt();                                                                 // Ветка расшифровывается
    } else {
        // Иначе пароль в данной сессии вводился и он правильный

        item_decrypt();                                                 // Ветка расшифровывается
    }
}


void KnowView::view_expand_selected(bool expand)
{
    std::function<void(QModelIndex index, bool mode)> expand_or_collapse_recurse = [&] (QModelIndex index, bool mode) {
            setExpanded(index, mode);

            int i = 0;

            while((index.child(i, 0)).isValid()) {
                expand_or_collapse_recurse(index.child(i, 0), mode);
                i++;
            }
        };

    // Получение индексов выделенных строк
    QModelIndexList selectitems = selectionModel()->selectedIndexes();

    for(int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), expand);
}



//template<>
boost::intrusive_ptr<TreeItem> KnowView::view_add_new(boost::intrusive_ptr<TreeIndex> _modelindex   // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , QString _name
    , const add_new &_branch_add_new_impl
    )
{
    auto _current_model = _modelindex->current_model();
    //    auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<TreeItem> result(nullptr);
    // Получение ссылки на узел, который соответствует выделенной строке
    //    auto item = _current_model->item(_current_index);

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setDisabled(true);

    auto current_item = _modelindex->parent();                   // _current_model()->item(_current_index);
    auto current_parent = current_item->parent();
    QList<boost::intrusive_ptr<TreeItem> > _alternative_items;

    if(current_parent) {
        _alternative_items = current_parent->items_direct([&] (boost::intrusive_ptr<const Linker> il) {
            return il->host()->name() == _name;
        });
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
            auto _it = it.next();

            if(_it->id() == result->id()) {                                                                                 // prevent error merge
                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex(_current_model, result); } catch(std::exception &e) {throw e; } return tree_index; } ();
                result = view_merge(tree_index, _it);
            }
        }

        //            result = _name_same_no_child_first ? _name_same_no_child_first : _items_with_same_name[0];
    } else {

        // Получение уникального идентификатора
        QString id = get_unical_id();


        // Вставка данных и установка курсора

        // Одноранговая ветка

        // Подветка

        // Вставка новых данных в модель дерева записей
        result = _branch_add_new_impl(_modelindex, id, _name);                                                 // _current_model->lock_child_add(_current_index, id, _name);
    }

    // Установка курсора на только что созданную позицию
    //        QModelIndex
    //    setto = _current_model()->index(result);  //index_child(_current_index, item->current_count() - 1);
    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
            {
                return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
            })); } catch(std::exception &e) {throw e; } return tree_index; } ();
    select_as_current(tree_index);
    //    selectionModel()->select(setto, current_tree_selection_mode);
    //    selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

    // А можно было установить курсор на нужную позицию и так
    //    knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount() - 1, 0, index), QItemSelectionModel::ClearAndSelect);


    _know_model_board->synchronized(false);
    // Сохранение дерева веток
    //    find_object<TreeScreen>(tree_screen_singleton_name)->
    _know_model_board->save();                  // know_model_save();

    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->setEnabled(true);
    assert(result->name() == _name);
    //    assert(result == _current_model()->item(setto));
    return result;                  // _current_model->item(setto);
}


// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
void KnowView::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count " << _know_model_board->rowCount();

    if(_know_model_board->rowCount() == 0) {
        qDebug() << "treescreen::tree_empty_control() : Tree empty, create blank branch";
        auto _know_model = [&] () {
                return know_model_board();
            };
        boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_know_model, _know_model()->root_item()); } catch(std::exception &e) {throw e; } return tree_index; } ();
        intercept(tree_absolute_index);
        boost::intrusive_ptr<TreeIndex> _modelindex;

        try {
            _modelindex = new TreeIndex([&] () -> KnowModel* {
                return source_model();
            }, source_model()->root_item());
        } catch(std::exception &e) {throw e; }

        QString _name = tr("Rename me");
        //        std::function<KnowModel *()> _know_model_board_functor = [&]() {return _know_model_board;};
        view_add_new(_modelindex                                                 // TreeIndex([&]()->KnowModel* { return source_model();}, source_model()->root_item())
            , _name                                                                      // tr("Rename me")
            , std::bind(&KnowModel::model_new_child, _know_root
            , _modelindex                                                                                        // std::placeholders::_1
            , std::placeholders::_2
            , _name                                                                                              // std::placeholders::_3
            )

            //        [&](TreeIndex _modelindex, QString _id, QString _name)->boost::intrusive_ptr<TreeItem> {
            //            auto _current_model = _modelindex.current_model();
            //            //            auto _current_index = _modelindex.current_index();
            //            return _current_model()->model_new_child(_modelindex, _id, _name);
            //        }
            );
    }
}


// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void KnowView::tree_crypt_control(void)
{
    // Если в дереве нет шифрования
    if(_know_model_board->is_contains_crypt_branches() == false) {
        // Запускается диалог сброса пароля шифрования
        Password pwd;
        pwd.resetPassword();
    }
}


// Вспомогательная функция, используется при добавлении ветки
// Если is_branch=true, то добавляется ветка на тот же уровень
// Если is_branch=false, то добавляется подветка
// Helper function is used when you add branches
// Eli is branch = true, the branch is added to the same level
// If is_branch = false, then added podvetka
//template<bool insert_sibling_branch>
boost::intrusive_ptr<TreeItem> KnowView::view_insert_new(boost::intrusive_ptr<TreeIndex> _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , const KnowView::add_new_delegate &_branch_add_new
    , const KnowView::add_new &_branch_add_new_impl
    //    , const std::function < boost::intrusive_ptr<TreeItem> (TreeIndex, QString
    //                                                            , const std::function < boost::intrusive_ptr<TreeItem>(TreeIndex, QString, QString) >&) > &_branch_add_new
    //    , const std::function < boost::intrusive_ptr<TreeItem> (TreeIndex, QString, QString) > &_branch_add_new_impl
    )                                                                                                             // bool insert_sibling_branch
{
    auto _current_model = _modelindex->current_model();
    auto _current_index = _modelindex->parent_index();
    boost::intrusive_ptr<TreeItem> result(nullptr);

    //    // Получение списка индексов QModelIndex выделенных элементов
    //    QModelIndexList selectitems = selectionModel()->selectedIndexes();

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
    //        _current_index = current_index();
    //    }

    if(_current_index.isValid()) {
        //    }

        //    if(!_current_index.isValid()) {
        //        selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
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

        if(ok && !_name.isEmpty()) {                                                 // return nullptr;    // Если была нажата отмена

            auto current_item = _modelindex->parent();                                                                                 //_current_model()->item(_current_index);
            auto current_parent = current_item->parent();
            QList<boost::intrusive_ptr<TreeItem> > _alternative_items;

            if(current_parent)
                _alternative_items = current_parent->items_direct([&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host()->name() == _name;
                });

            //            auto _name_same_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _items_with_same_name) {if(i->count_direct() == 0) {result = i; break;}} return result;}();

            if(_alternative_items.size() > 0) {

                //                result = _name_same_no_child_first ? _name_same_no_child_first : _items_with_same_name[0];

                QMutableListIterator<boost::intrusive_ptr<TreeItem> > it(_alternative_items);
                result = it.next();

                while(it.hasNext()) {
                    auto _it = it.next();

                    if(_it->id() == result->id()) {                                                                                                                                                 // prevent error merge
                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex(_current_model, result); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        result = view_merge(tree_index, _it);
                    }
                }

            } else {

                // Получение индекса выделенной строки
                //    QModelIndex
                //    current_index = currentitem_index();

                //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
                //                auto _source_model = [&]() {return source_model();}; // std::bind(_source_model_func, _tree_view);
                result = _branch_add_new(_modelindex
                        , _name                                                                                                                                       // source_model()->item(_current_index)->name()
                        , _branch_add_new_impl);                                                                                                                                         // nonsense
            }

            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                    {
                        return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                    })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            select_as_current(tree_index);
            // , insert_sibling_branch
        }
    }

    // Введенные данные добавляются
    return result;
}

boost::intrusive_ptr<TreeItem> KnowView::view_paste_children_from_children(boost::intrusive_ptr<TreeIndex> _parent_modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , boost::intrusive_ptr<TreeItem>            _blank_header
    , const KnowView::substitute_condition_double &_substitute_condition
    )
{
    auto _current_model = _parent_modelindex->current_model();
    auto _current_parent_index = _parent_modelindex->parent_index();
    auto _current_parent = _parent_modelindex->parent();
    boost::intrusive_ptr<TreeItem>  result(nullptr);

    //    std::vector<boost::intrusive_ptr<TreeItem>> results;
    if(_current_model()->index(_blank_header).isValid()
        && _current_model()->index(_blank_header) != _current_parent_index
        && _substitute_condition(_blank_header->linker(), _current_parent->linker())
        && _blank_header->field("name") != clipboard_items_root
        ) {
        view_merge(_parent_modelindex, _blank_header);                                                 // TreeIndex(_current_model, _current_parent)
    }                                 // ? should this function ignore the _blank_header?

    if(_blank_header->count_direct() > 0) {
        //    for(int i = 0; i < it->current_count(); i++) {

        //        auto current_item = it->child(i);
        //    if(_current_model->is_item_id_exists(it->id())) {
        //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

        //        auto _current_index = index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

        if(_current_parent_index.isValid()) {
            // Получение ссылки на узел, который соответствует выделенной строке
            //        auto current_item = _current_model->item(_current_index);

            //        if(!_current_index.isValid()) {
            //            selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
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
            _blank_header->traverse_direct([&] (boost::intrusive_ptr<Linker> il) {                                                                                  // != reserved

                //            for(int index = 0; index < _blank_root->direct_count(); index++) {
                auto candidate = il->host();                                                                    // _blank_header->item_direct(0);

                if(candidate) {
                    auto found_item =                                                                 // _know_model_board
                    _current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) -> bool {
                        return t->name() == candidate->name() && t->id() == candidate->id();
                    });

                    // Подветка
                    if(found_item) {                                                                 // candidate->id()
                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex(_current_model, found_item); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        found_item = view_merge(tree_index, candidate);
                        //                    reserved++;
                    } else {
                        found_item = candidate;
                    }

                    view_paste_child(_parent_modelindex, found_item, std::bind(_substitute_condition, std::placeholders::_1, found_item->linker()));

                    //                assert(_know_model_board->is_item_exists(candidate->id()));


                    current_count++;


                    //            // Установка курсора на только что созданную позицию
                    //            //        QModelIndex
                    //            setto = _current_model->index_child(_current_index, current_item->current_count() - 1);
                    //            selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

                    //            // А можно было установить курсор на нужную позицию и так
                    //            // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                    //            //                                             QItemSelectionModel::ClearAndSelect);
                }
            });

            if(current_count > 0)
                result = _current_parent;                                                                                                 // _current_model()->item(_current_parent_index);    // _blank_header;

            if(result) {                                                                                 // && _blank_header->field("name") != clipboard_items_root
                //            _current_model->endInsertRows();

                // Установка курсора на только что созданную позицию
                //        QModelIndex
                setto = _current_model()->index(
                    std::bind(_substitute_condition, std::placeholders::_1, result->linker())                                                                                                                         // [&](boost::intrusive_ptr<const TreeItem> it)->bool{return it->id() == begin_item->id();}
                    );                                                                                                                     // TreeModel::delegater(begin_item->id())

                // index_child(_current_index.parent(), _current_index.row() + 1);   // current_item->current_count() - 1

                assert(setto.isValid());

                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                        {
                            return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                select_as_current(tree_index);                                                                                                                 // setto
                //            selectionModel()->select(setto, current_tree_selection_mode);
                //            selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //        return
                result = _current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> it) {
                    return it->id() == result->id();
                });                                                                                                                                                                                 // setto

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


void KnowView::view_paste_children_from_clipboard(boost::intrusive_ptr<TreeIndex> _sibling_modelindex)  // std::function<QString(const QModelIndex &, ClipboardBranch *)>  _model_paste_from_clipboard
{

    // Добавление подветки из буфера обмена относительно указанного элемента
    // Функция возвращает новый идентификатор стартовой добавленной подветки
    auto paste_children_from_clipboard = [] (boost::intrusive_ptr<TreeItem> _blank_header, ClipboardBranch * _sub_branch                  // , int _sibling_order
            ) -> boost::intrusive_ptr<TreeItem> {
            qDebug() << "In paste_subbranch()";
            boost::intrusive_ptr<TreeItem> _result(nullptr);
            //    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == _target_item->id();}));  // maybe a dangling pointer, for further operations



            auto move_as_child_from_clipboard_object = [] (boost::intrusive_ptr<TreeItem> _parent, boost::intrusive_ptr<TreeItem> _source_item, int _pos, int _mode = add_new_record_after) -> boost::intrusive_ptr<TreeItem> {

                    //    auto item_from_model = item([&](boost::intrusive_ptr<const TreeItem> it)->bool{return it->id() == _source_item->id();});  // clipboard implementation have no model

                    //    if(item_from_model) {
                    //        _source_item = item_from_model;
                    //    }
                    assert(_parent);
                    boost::intrusive_ptr<TreeItem> result(nullptr);                                                                         // 1-1?

                    //        auto _index_parent = index(_parent);

                    //        auto _index_origin = index(_source_item);

                    //        assert(!_index_origin.isValid());
                    if(_parent
                        && !(
                            _source_item->parent() == _parent
                            && _parent->contains_direct(std::forward<const boost::intrusive_ptr<const Linker> >(_source_item->linker())))
                        )
                    {
                        //                //                _source_item->self_remove_from_parent();    //
                        //                _source_item->parent()->remove(_source_item->up_linker());    // -> this content is not must a logic error  1-2? // inside in 1-3 _source_item->parent calling

                        //                //        assert(!_index_parent.isValid());

                        result = _source_item->parent(_parent, _pos, _mode)->host();                                                                                                         // 1-3?  // *_parent << _source_item; //


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

                    return result;                                                                         // return model_move_as_child_impl(_parent, _source, _pos, _mode);
                };

            int record_move_times = 0;

            std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem>, ClipboardBranch *                                             // , const int
            )>                                                                                                                                 // , const QString &
            child_assembly_recursive
                = [&] (boost::intrusive_ptr<TreeItem>    _current_clip_parent_parent
                    , boost::intrusive_ptr<TreeItem>  _current_clip_parent                                               // , const QString &_current_clip_records_parent_id
                    , ClipboardBranch                 *_sub_branch
                    // , const int                       _sibling_order
                    ) -> boost::intrusive_ptr<TreeItem> {
                    //            QString id = _current_clip_parent->id();
                    boost::intrusive_ptr<TreeItem> contain_current_parent(nullptr);

                    if(_current_clip_parent_parent)
                    {
                        contain_current_parent = _current_clip_parent_parent->contains_direct(std::forward<const boost::intrusive_ptr<const Linker> >(_current_clip_parent->linker()));
                    } else{
                        contain_current_parent = _current_clip_parent;
                    }

                    if(contain_current_parent)
                    {
                        //        if(_clip_parent->parent()) {
                        qDebug() << "In child_assembly_recursive()";

                        // Выясняются данные конечных записей
                        QList< boost::intrusive_ptr<TreeItem> > records = _sub_branch->records_by_parent_id(_current_clip_parent->id());

                        for(int i = 0; i < records.size(); i++) {                                                                                                         //foreach(boost::intrusive_ptr<TreeItem> record, records) {
                            auto record = records.at(i);


                            if(record && _current_clip_parent != record) {
                                boost::intrusive_ptr<TreeItem> contain_current_record(nullptr);
                                contain_current_record = _current_clip_parent->contains_direct(std::forward<const boost::intrusive_ptr<const Linker> >(record->linker()));

                                // _new_item
                                if(!contain_current_record) {
                                    auto record_shadow = move_as_child_from_clipboard_object(_current_clip_parent, record, i                                                                                                                                                                                                         // _sibling_order + i
                                            );                                                                                                                                                                                                                                                                                 // _clip_parent->count_direct()
                                    qDebug() << "Add table record : " + record->field("name") + " to " + _current_clip_parent->name();
                                    // assert(record->parent_id() == _current_clip_parent->id());
                                    assert(record_shadow->id() == record->id());
                                    assert(record->id() != _current_clip_parent->id());
                                    record_move_times++;
                                    // _clip_parent->child_move(_clip_parent->count_direct(), record, ADD_NEW_RECORD_TO_END);    // _new_item

                                    //                assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() == record_shadow->id();})); // dangling parent lead all do not exist in model

                                    if(record_shadow) {                                                                                                                                                                                                         // you can not use result->count_direct() because there are no children there
                                        //                    QList< boost::intrusive_ptr<TreeItem> > record_children = _sub_branch->records_by_parent_id(record_shadow->id());

                                        //                    for(int i = 0; i < record_children.size(); i++) { //foreach(QString current_subbranch_id, subbranch_id_list) {

                                        //                    assert(record_children[i]->parent_id() == record_shadow->id());

                                        //                    assert(0 == record_shadow->count_direct());
                                        child_assembly_recursive(_current_clip_parent, record_shadow, _sub_branch                                                                                                                                                                                                                                         // , 0
                                            );

                                        //                    }
                                    }
                                }
                            }
                        }
                    }

                    //        }

                    //            assert(id != "");
                    return _current_clip_parent;                                                                         // id;
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
            _result = child_assembly_recursive(nullptr, _blank_header                                             // , _clip_blank_header_id // *id_iterator
                    , _sub_branch
                    // , _sibling_order  // + offset
                    );

            //        //        offset++;
            //        calling_count++;
            //        //            }

            //        assert(calling_count == _clip_blank_header_id.size());
            assert(record_move_times <= _blank_header->count_children_all());
            //        _result = _blank_header;
            //        }

            return _result;                                             // _target_item->id();
        };





    auto _current_model = _sibling_modelindex->current_model();
    auto _current_sibling_index = _sibling_modelindex->parent_index();
    boost::intrusive_ptr<TreeItem> _result(nullptr);
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData *mime_data = QApplication::clipboard()->mimeData();

    if(mime_data == nullptr)
        return;

    if(!(mime_data->hasFormat("mytetra/branch")))
        return;

    //    //    // Получение списка индексов QModelIndex выделенных элементов
    //    //    QModelIndexList selectitems = selectionModel()->selectedIndexes();
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
    //    _current_index = current_index();
    //    //    }


    if(_current_sibling_index.isValid()) {
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
        //        auto _current_model = [&]() {return source_model();}; // std::bind(_source_model_func, _tree_view);




        //        //        // delete items from view
        //        //        //        auto _source_model = source_model();
        //        //        auto _current_item = current_item();

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






        //        //        select_and_current(_current_item);

        //        //        assert(_source_model()->item([ = ](boost::intrusive_ptr<TreeItem> t) {return t->id() == _current_item->id();}));

        QMap<QString, QString> data;
        QDateTime ctime_dt = QDateTime::currentDateTime();
        QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
        //        data["id"]      = get_unical_id();
        data["name"] = clipboard_items_root;                                                 // "Clipboard items root";
        data["ctime"] = ctime;
        data["dir"] = data["id"];
        data["file"] = "text.html";

        boost::intrusive_ptr<TreeItem> blank_header(new TreeItem(nullptr, data));
        blank_header = paste_children_from_clipboard(blank_header, (ClipboardBranch *)_sub_branch                                                 // , 0
                );                                                                                                                      // _current_model()->
        QList<boost::intrusive_ptr<TreeItem> > buffer;
        blank_header->traverse_direct([&] (boost::intrusive_ptr<Linker> il_in_clipboard) {
            auto index = static_cast<KnowModel *>(_current_model())->index([&] (boost::intrusive_ptr<const Linker> link_in_tree) {
                return link_in_tree->host()->id() == il_in_clipboard->host()->id();
            });

            if(index.isValid()) {
                auto it_on_tree = _current_model()->item(index);

                if(index.parent().isValid()) {
                    auto parent_item = _current_model()->item(index.parent());
                    assert(parent_item->field("name") != clipboard_items_root);

                    if((static_cast<KnowModel *>(_current_model())->item([&] (boost::intrusive_ptr<const TreeItem> t) {
                        return t->id() == parent_item->id();
                    }))                                                                                                                                                                                                                           // if(it_on_tree->up_linker()->host_parent()->field("name") == clipboard_items_root)it_on_tree->parent(parent_item);
                    && !buffer.contains(it_on_tree)) { buffer << it_on_tree; }
                }
            }
        });

        if(buffer.size() > 0) {                                                 // == blank_header->count_direct()
            blank_header->clear();

            for(auto it_on_tree : buffer) {
                view_paste_child(_sibling_modelindex, it_on_tree, [&] (boost::intrusive_ptr<const Linker> il) {
                    return il->host()->field("name") == it_on_tree->field("name");
                });                                                                                                                                                                                                                             //it->parent(blank_header);
            }

            _result = _current_model()->item(_current_sibling_index);
        } else if(blank_header) {
            _result = view_paste_children_from_children(_sibling_modelindex, blank_header, [&] (boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source) -> bool {
                return target->host()->field("url") == source->host()->field("url") && target->host()->field("name") == source->host()->field("name");
            });

        }

        if(_result) {
            // Добавление ветки
            QString pasted_branch_id = _result->id();                                                                                 // _model_paste_from_clipboard(_current_index, (ClipboardBranch *)_sub_branch);

            //        if(is_branch)
            //            pasted_branch_id = source_model()->lock_paste_as_sibling(index, (ClipboardBranch *)branch);
            //        else
            //            pasted_branch_id = source_model()->lock_paste_as_child(index, (ClipboardBranch *)branch);

            // update indexes
            if(_current_sibling_index.parent().isValid()) _current_model()->update_index(_current_sibling_index.parent());

            setExpanded(_current_sibling_index, true);

            // Установка курсора на новую созданную ветку
            auto pasted_branch_item = _current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> it) {
                return it->id() == pasted_branch_id;
            });
            QStringList pasted_branch_path = pasted_branch_item->path_list();
            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->set_tree_position(source_model()->root_item()->id(), pasted_branch_path);

            _know_model_board->synchronized(false);
            // Сохранение дерева веток
            //    find_object<TreeScreen>(tree_screen_singleton_name)->
            know_model_save();


            //        }

        }

        // Разблокируется главное окно
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setEnabled(true);
    }
}



boost::intrusive_ptr<TreeItem> KnowView::view_paste_child(boost::intrusive_ptr<TreeIndex>   _modelindex   // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    , boost::intrusive_ptr<TreeItem>              _source_item
    , const KnowView::substitute_condition        &_substitute_condition
    )
{


    auto view_paste_sibling = [&] (                  // boost::intrusive_ptr<TreeItem> TreeScreen::view_paste_sibling(
        boost::intrusive_ptr<TreeIndex>   _modelindex                                                                      // std::function<KnowModel *()> _current_model, QModelIndex _current_index
        , boost::intrusive_ptr<TreeItem>              _source_item
        , const KnowView::substitute_condition      & substitute_condition_
        ) -> boost::intrusive_ptr<TreeItem> {
            boost::intrusive_ptr<TreeItem>  result(nullptr);
            auto current_model = _modelindex->current_model();
            auto current_index = _modelindex->parent_index();
            auto current_item = _modelindex->parent();                                             // _current_model()->item(_current_index);

            assert(current_item);                                             // make it automatically?
            assert(current_item != _source_item);                                             // mayb be equal because integrity is false
            auto current_parent = current_item->parent();
            //    auto current_parent_    = _current_model()->item(_current_index.parent());
            assert(current_parent);
            //    assert(_current_parent == current_parent_);

            if(current_parent && current_parent->id() != "")
            {

                //            bool current_parent_valid = [&]() {
                //                if(!current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == current_parent->id();}) && current_parent->id() != "") {
                //                    intercept(TreeIndex(current_model, current_parent));
                //                }
                //                return (current_model()->index(current_parent).isValid())
                //                && (current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() == current_parent->id();}));
                //            }();
                boost::intrusive_ptr<TreeIndex> _parent_modelindex;

                try {
                    _parent_modelindex = new TreeIndex(current_model, current_parent, current_parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                    return il == current_item->linker() && il->host() == current_item && current_item->parent() == il->host_parent();
                }));
                } catch(std::exception &e) {throw e; }

                //    for(int i = 0; i < it->current_count(); i++) {

                //        auto current_item = it->child(i);
                //    if(_current_model->is_item_id_exists(it->id())) {
                //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

                //    auto _current_index = index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

                if(current_index.isValid()) {
                    // Получение ссылки на узел, который соответствует выделенной строке
                    //    auto current_item = _current_model->item(_current_index);

                    //    if(!_current_index.isValid()) {
                    //        selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
                    //        assert(_current_index.isValid());
                    //    }

                    //    find_object<MainWindow>("mainwindow")
                    globalparameters.mainwindow()->setDisabled(true);

                    if((QString)_source_item->id() == "") {
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

                    QList<boost::intrusive_ptr<TreeItem> > _alternative_items;
                    //            auto check = _current_model->item(_current_index);
                    //            assert(_current_model->item_by_id(check->id()));
                    //            assert(_current_model->item_by_name(check->name()));
                    //            assert(_know_model_board->item_by_name(check->name()));
                    //            assert(_know_model_board->item_by_id(check->id()));

                    //                if(current_parent_valid) {
                    _alternative_items = current_parent->items_direct(substitute_condition_);

                    //        auto _alternative_item_has_no_child_first = [&] {boost::intrusive_ptr<TreeItem> result; for(auto i : _alternative_items) {if(i->count_direct() == 0) {result = i; break;}} return result;}();

                    if(_alternative_items.size() > 0) {                                                                                                             // && same->is_empty()
                        //            //            if(_alternative_item_has_no_child_first) {
                        //            //                assert(_alternative_item_has_no_child_first->name() == _source_item->name());
                        //            //                result = view_merge_to_left(_current_model, _alternative_item_has_no_child_first, _source_item);
                        //            //            } else
                        //            for(int i = 1; i < _alternative_items.size(); i++) {
                        //                result = view_merge_to_left(_current_model, _alternative_items[0], _alternative_items[i]);
                        //            }

                        QMutableListIterator<boost::intrusive_ptr<TreeItem> > it(_alternative_items);
                        result = it.next();                                                                                                                                             // _source_item;  // _source_item might be a clipboard item

                        while(it.hasNext()) {
                            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(current_model, result); } catch(std::exception &e) {throw e; } return tree_index; } ();
                            result = view_merge(tree_index, it.next());
                            assert(result);
                            assert(current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                            return t->id() == result->id();
                        }));
                        }

                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(current_model, result); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        result = view_merge(tree_index, _source_item);
                        assert(result);

                        if(result) {
                            assert(current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                            return t->id() == result->id();
                        }));
                        }

                        //                        assert(result == _source_item);
                        //                    if(result != _source_item) {
                        //                        result = view_merge_to_left(_current_model, result, _source_item);
                        //                        assert(result);
                        //                        assert(_current_model()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == result->id();}));

                        //                    }
                        boost::intrusive_ptr<TreeIndex> tree_index_ = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                            {
                                return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                            })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        select_as_current(tree_index_);
                        //            view_paste_from_children_move(_modelindex, _source_item, [&](boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source)->bool {return target->field("url") == source->field("url") && target->field("name") == source->field("name");});  // , _substitute_condition

                        assert(result != _know_model_board->root_item());
                    } else {
                        // Вставка новых данных в модель дерева записей
                        result = static_cast<KnowModel *>(current_model())->model_move_as_child(
                            _parent_modelindex                                                                                                                                                      // TreeIndex(current_model, current_parent, _modelindex.sibling_order())
                            , _source_item);
                        assert(current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                        return t->id() == result->id();
                    }));
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
                    assert(current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                    return t->id() == result->id();
                }));

                    //        setto = _current_model()->index(result);    //(current_item->parent()->current_count() - 1, 0, _current_index.parent());
                    //        assert(setto.isValid());
                    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                        {
                            return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                    select_as_current(tree_index);

                    //        selectionModel()->select(setto, current_tree_selection_mode);
                    //        selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect
                    //        }


                    // Сохранение дерева веток
                    //    find_object<TreeScreen>(tree_screen_singleton_name)->
                    know_model_save();


                    //        return

                    //        result = _current_model()->item(setto);
                    boost::intrusive_ptr<TreeIndex> tree_index_ = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                        {
                            return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                    select_as_current(tree_index_);
                    assert(
                        (_source_item == result) ||
                        substitute_condition_(result->linker())                                                                                                             // (_source_item->name() == result->name())
                        );

                    //    } else if(_current_model->root_item()->id() == it->id()) {

                    //        assert(new_branch_root == result);  // not must, if you already have it

                    //    }

                    //    }
                    //                }   // if(current_parent_valid)


                    //    find_object<MainWindow>("mainwindow")
                    globalparameters.mainwindow()->setEnabled(true);
                }

                //        else {

                //        }


            }

            return result;
        };


    boost::intrusive_ptr<TreeItem> result(nullptr);
    auto current_model = _modelindex->current_model();
    auto current_index = _modelindex->parent_index();
    auto current_item = _modelindex->parent();
    assert(current_index.isValid() ? current_model()->item(current_index) != nullptr : true);


    //    for(int i = 0; i < it->current_count(); i++) {

    //        auto current_item = it->child(i);
    //    if(_current_model->is_item_id_exists(it->id())) {
    //    return add_branch(_current_model->index(it), it, insert_sibling_branch, _current_model);

    //    auto _current_index = index_current(); //selectionModel()->currentIndex(); //_current_model->index(it);

    if(current_item && _source_item) {                  // current_index.isValid()

        //        boost::intrusive_ptr<TreeItem> current_item = _modelindex.parent(); // _current_model()->item(_current_index);

        //        assert(!current_item->contains_direct(std::forward<const boost::intrusive_ptr<Linker>>(_source_item->linker())));
        //        assert(!current_item->contains_direct(std::forward<const boost::intrusive_ptr<TreeItem>>(_source_item)));

        if(!current_item->contains_direct(std::forward<const boost::intrusive_ptr<const Linker> >(_source_item->linker()))) {                                                 // current_item->contains_direct(_source_item->linker()) ||
            if(current_item->count_direct() > 0) {
                auto _child = current_item->item_direct(_modelindex->sibling_order());                                                                                                                 // current_item->count_direct() - 1

                assert(_child != _source_item);                                                                                                                 // may be equal because integrity is false

                //            if(current_item->count_direct() > 0) {
                //                assert(current_item->item_direct(0) != _source_item);
                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(current_model, _child); } catch(std::exception &e) {throw e; } return tree_index; } ();
                result = view_paste_sibling(tree_index                                                                                                                 //_current_model, _current_model()->index(current_item->item_direct(0))
                        , _source_item, _substitute_condition);
            } else {

                // Получение ссылки на узел, который соответствует выделенной строке
                //    auto current_item = _current_model->item(_current_index);

                //    if(!_current_index.isValid()) {
                //        selectionModel()->setCurrentIndex(_current_model->index(0, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect);
                //        assert(_current_index.isValid());
                //    }

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setDisabled(true);

                if((QString)_source_item->id() == "") {
                    // Получение уникального идентификатора
                    QString id = get_unical_id();
                    _source_item->field("id", id);
                }

                //            QModelIndex setto;

                // Вставка данных и установка курсора

                // Одноранговая ветка

                // Подветка

                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(current_model, current_item, _modelindex->sibling_order()); } catch(std::exception &e) {throw e; } return tree_index; } ();
                // Вставка новых данных в модель дерева записей
                result = static_cast<KnowModel *>(current_model())->model_move_as_child(tree_index, _source_item);
                assert(result);
                assert(result != _know_model_board->root_item());
                // Установка курсора на только что созданную позицию
                //        QModelIndex
                assert(current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                    return t->id() == result->id();
                }));
                //            setto = _current_model()->index(result);  //index_child(_current_index, current_item->current_count() - 1);
                //            assert(setto.isValid());
                boost::intrusive_ptr<TreeIndex> tree_index_ = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                        {
                            return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                select_as_current(tree_index_);                                                                                                                 // setto
                //        selectionModel()->select(setto, current_tree_selection_mode);
                //        selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // ClearAndSelect

                // А можно было установить курсор на нужную позицию и так
                // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                //                                             QItemSelectionModel::ClearAndSelect);


                // Сохранение дерева веток
                //    find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();

                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //        return
                result = current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                    return t->id() == result->id();
                });
                assert((_source_item == result) || (_source_item->name() == result->name()));
                //    } else if(_current_model->root_item()->id() == it->id()) {

                //        assert(new_branch_root == result);  // not must, if you already have it

                //    }

                //    }
            }
        } else {                                                                 // for current_item->contains_direct(_source_item->linker()) != nullptr, maybe just some merge, go view_paste_sibling upon

            result = _source_item;
        }

        assert(result);
    }

    return result;
}


QList<boost::intrusive_ptr<Linker> > KnowView::view_delete_permanent(const std::function<KnowModel *()>       &_current_model
    , QList<boost::intrusive_ptr<TreeItem> >  _items
    , delete_strategy _delete_strategy
    , const QString                          &_mode
    , const bool _cut_branch_confirm
    )
{
    QList<boost::intrusive_ptr<TreeItem> > items_candidate;
    QList<boost::intrusive_ptr<Linker> > result_linkers;
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());
    //    QModelIndexList _index_list = is_index_localized(_origin_index_list) ? _origin_index_list : index_localize(_origin_index_list);

    if(_items.size() > 0) {

        std::function<bool(QList<boost::intrusive_ptr<TreeItem> >)>
        has_encrypt = [&] (QList<boost::intrusive_ptr<TreeItem> > _result) {
                bool disableFlag = false;

                // Перебираются удаляемые ветки
                for(auto it : _result) {                                                                             //for(int i = 0; i < _index_list.size(); ++i) {
                    // QModelIndex index = _index_list.at(i);
                    // auto it = _current_model()->item(index);

                    // Если у ветки установлен флаг шифрования
                    if(it->field("crypt") == "1") {
                        disableFlag = true;
                        break;
                    }

                    // Проверяется наличие флага шифрования у всех подветок
                    QList<QStringList> cryptFlagsList = it->path_children_all("crypt");

                    foreach(QStringList cryptFlags, cryptFlagsList) {
                        if(cryptFlags.contains("1")) {
                            disableFlag = true;
                            break;
                        }
                    }

                    if(disableFlag) break;

                }                                                                                             // Закрылся цикл перебора всех выделенных для удаления веток


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

        //        auto _source_model = source_model();

        // На время удаления блокируется главное окно
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->setDisabled(true);
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->blockSignals(true);

        //preparations

        std::sort(_items.begin(), _items.end(), [&] (boost::intrusive_ptr<TreeItem> t0, boost::intrusive_ptr<TreeItem> t1) {
            return _current_model()->index(t0).row() < _current_model()->index(t1).row();
        });

        auto index_to_be_delete_first = _current_model()->index(_items.first());
        auto _index_common_parent = index_to_be_delete_first.parent();

        //    auto index_to_be_delete_last = _index_list.last();
        auto _item_to_be_deleted_first = _items.first();
        auto _item_common_parent = _item_to_be_deleted_first->parent();                                                 // may be a clipboard root item : blank header
        assert(_item_common_parent);

        if(_item_common_parent->field("name") != clipboard_items_root) {
            auto deleted_position_first = _item_common_parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il) {
                return il->host()->id() == _item_to_be_deleted_first->id() && _item_to_be_deleted_first->linker() == il && _item_to_be_deleted_first->parent() == il->host_parent();
            });                                                                                                                                                                                                                                                                                                                                  // items_update_selected();
            //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();



            // Список имен веток, которые нужно удалить
            QStringList branches_name;

            for(auto it : _items) {
                // QModelIndex index = _index_list.at(i);

                if(it->parent() == _item_common_parent) {                                                                                                                 //if(index.isValid() && index.parent() == _index_common_parent) {

                    if(it->id() == _current_model()->session_id()) {
                        //        keep = source; remove = target;
                        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_current_model, _item_common_parent); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        _current_model()->session_id(tree_index);

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
                            QStringList path_1 = items_candidate.at(j - 1)->path_list();                                                                                                                                                                                                                //(_current_model()->item(  // ))
                            QStringList path_2 = items_candidate.at(j)->path_list();                                                                                                                                                                                                                 //(_current_model()->item(  // ))

                            if(path_1.size() < path_2.size()) items_candidate.swap(j - 1, j);
                        }
                    }

                    qDebug() << "Path for delete";

                    for(int i = 0; i < items_candidate.size(); ++i)
                        qDebug() << items_candidate.at(i)->path_list();                                                                                                                                                                 //(_current_model()->item(  //))

                    // auto _source_model = source_model();
                    //_source_model->beginRemoveRows(_index_common_parent, _index_list.first().row(), _index_list.last().row());

                    // Вызов удаления веток
                    for(auto it : items_candidate) {                                                                                                                                                 // for(int i = 0; i < _result.size(); ++i) {
                        if(it->id() == static_cast<KnowModel *>(_current_model())->session_id()) {
                            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_current_model, it->parent()); } catch(std::exception &e) {throw e; } return tree_index; } ();
                            static_cast<KnowModel *>(_current_model())->session_id(tree_index);
                        }

                        auto result = (static_cast<KnowModel *>(_current_model())->*_delete_strategy)(it->linker());

                        // just for counting, result.get() == 0 is fine

                        result_linkers.append(result);                                                                                                                                                                                 // model_delete_one( // just for counting, 0 is fine


                        // _current_model()->model_delete_item(it)

                    }

                    if(result_linkers.size() > 0) {
                        // _source_model->endRemoveRows();
                        //                _source_model = source_model();
                        _current_model()->update_index(_index_common_parent);
                        emit _current_model()->layoutChanged(QList<QPersistentModelIndex>() << _index_common_parent);

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
                                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, left_sibling_item->parent(), left_sibling_item->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                                        {
                                            return il == left_sibling_item->linker() && il->host() == left_sibling_item && left_sibling_item->parent() == il->host_parent();
                                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                                select_as_current(tree_index);                                                                                                                                                                                                                                                 // setto


                                //                        selectionModel()->select(setto, current_tree_selection_mode);   // current_tree_selection_mode
                                //                        selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);   // current_tree_selection_mode    // ClearAndSelect

                            } else {

                                //            //        setto = _root_model->index_child(_current_index, current_item->direct_children_count() - 1);
                                //            setto = static_cast<TreeModel *>(_current_know_branch)->index(index.parent().row(), 0, index.parent().parent());
                                //            selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

                                while(_item_common_parent && _item_common_parent != _know_model_board->root_item()) {

                                    if(_item_common_parent != _current_model()->root_item() && _current_model()->index(_item_common_parent).isValid()) {

                                        left_sibling_item = _item_common_parent;
                                        setto = _current_model()->index(left_sibling_item);

                                        //                                select_and_current(setto);

                                        //                                selectionModel()->select(_source_model->index(_item_parent), current_tree_selection_mode);
                                        //                                selectionModel()->setCurrentIndex(_source_model->index(_item_parent), current_tree_current_index_mode);   // ClearAndSelect

                                        break;
                                    } else {                                                                                                                                                                                                                                                                                                 // if(_parent != _know_model_board->root_item())
                                        cursor_follow_up();
                                        _item_common_parent = _item_common_parent->parent();

                                        if(_item_common_parent) {
                                            left_sibling_item = _item_common_parent;
                                            setto = _current_model()->index(left_sibling_item);

                                        } else {
                                            tree_empty_controll();
                                            left_sibling_item = source_model()->root_item()->item_direct(0);
                                            setto = _current_model()->index(left_sibling_item);
                                        }
                                    }
                                }

                                assert(left_sibling_item);
                                setto = _current_model()->index(left_sibling_item);
                                assert(setto.isValid());

                                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, left_sibling_item->parent(), left_sibling_item->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                                        {
                                            return il == left_sibling_item->linker() && il->host() == left_sibling_item && left_sibling_item->parent() == il->host_parent();
                                        })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                                select_as_current(tree_index);                                                                                                                                                                                                                                                 // setto

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

                        //            _actionlist[action_paste_branch]->setEnabled(false);
                        //            _actionlist[action_paste_sub_branch]->setEnabled(false);
                    }
                } else {
                    _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
                    _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);
                }

                // Разблокируется главное окно
                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->setEnabled(true);
                //    find_object<MainWindow>("mainwindow")
                globalparameters.mainwindow()->blockSignals(false);

                tree_empty_controll();
                tree_crypt_control();

            }
        }                                                                 // Закрылось условие что системный пароль не установлен
    }

    return result_linkers;
}

void KnowView::view_cut(bool _cut_branch_confirm)
{
    //    bool copy_result = false;
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());
    //    copy_result
    auto index_list = view_copy();

    if(index_list.size() > 0) {                // copy_result
        view_delete_permanent([&] () {
            return source_model();
        }, [&] () {
            QList<boost::intrusive_ptr<TreeItem> > r;

            for(auto ix : index_list) r << source_model()->item(ix);

            return r;
        } ()
            , &KnowModel::model_delete_permanent_recursive
            , "cut", _cut_branch_confirm);
        // view_remove(index_list, "cut", _cut_branch_confirm); //this->_know_branch,
    }

    _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
    _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);

}



QModelIndexList KnowView::view_copy(void)    // const
{

    //    bool copy_result = false;
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());
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
    //    QModelIndexList _origin_index_list = selectionModel()->selectedIndexes();
    QModelIndexList _selectitems = LocalLizeInitializer(this)().indexes();                  // selectionModel()->selectedIndexes(); //
    // is_index_localized(_origin_index_list) ? _origin_index_list : index_localize(_origin_index_list);

    QModelIndex _index_current;
    bool indexs_valid = true;
    QList<boost::intrusive_ptr<TreeItem> > items;
    auto _source_model = [&] () {
            return source_model();
        };

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
            _index_current = static_cast<KnowModel *>(_source_model())->index(items[0]);                                                                 // _selectitems[0];
    }


    // Получение индекса выделенной ветки
    //    _index_current = index_current();

    if(indexs_valid) {                 // _index_current.isValid()


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

            if(branch_clipboard_data_previos != nullptr) delete branch_clipboard_data_previos;

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
        _sub_branch->branch_push(TreeIndex(_source_model, current_item()), items);                                                 // , _clipboard_root_parent_id
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

        _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
        _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);


        //        }

        // Объект с ветками помещается в буфер обмена
        if(_sub_branch->clip_blank_header_id() != "") {cbuf->setMimeData(_sub_branch); }                                                 // copy_result = true;
        else {delete _sub_branch; }
    } else {
        _tree_screen->_actionlist[action_paste_branch]->setEnabled(false);
        _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(false);
        //        copy_result = false;
    }

    return _selectitems;                    // copy_result;
}




void KnowView::view_edit(void)
{
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    QModelIndexList selectitems = selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    //    // Если выбрано более одной ветки
    //    if(selectitems.size() > 1) {
    //        //        QMessageBox messageBox(this);
    //        //        messageBox.setWindowTitle(tr("Unavailable action"));
    //        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling edit operation."));
    //        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    //        //        messageBox.exec();
    //        //        return;
    //        _current_index = selectitems.first();
    //    } else {

    // Получение индекса выделенной строки
    _current_index = current_index();
    //    }

    if(_current_index.isValid()) {
        // Получение ссылки на узел, который соответствует выделенной строке
        auto item = source_model()->item(_current_index);

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



boost::intrusive_ptr<TreeItem> KnowView::view_merge(boost::intrusive_ptr<TreeIndex> modelindex    // const std::function<KnowModel *()> &_current_model, boost::intrusive_ptr<TreeItem> target
    , boost::intrusive_ptr<TreeItem> source
    )
{
    boost::intrusive_ptr<TreeItem> result(nullptr);
    auto _current_model = modelindex->current_model();
    auto target = modelindex->parent();
    //    boost::intrusive_ptr<TreeItem> keep;
    //    boost::intrusive_ptr<TreeItem> remove;

    //    if(target->id() == _session_id) {keep = target; remove = source;}
    // else
    auto target_on_tree = _current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
        return t->id() == target->id();
    });

    if(target_on_tree) {
        if(target != source) {                                                 // && target_on_tree

            if(source->id() == static_cast<KnowModel *>(_current_model())->session_id()) {
                //        keep = source; remove = target;
                if(target->parent()) {
                    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_current_model, target->parent()); } catch(std::exception &e) {throw e; } return tree_index; } ();
                    static_cast<KnowModel *>(_current_model())->session_id(tree_index);
                } else {
                    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_current_model, target); } catch(std::exception &e) {throw e; } return tree_index; } ();
                    static_cast<KnowModel *>(_current_model())->session_id(tree_index);
                }
            }

            //    else {keep = target; remove = source;}

            //    //    //    KnowModel *(TreeScreen::*_know_model_board)() = &TreeScreen::know_model_board;
            //    //    //    KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
            //    //    auto _source_model = [&]() {return source_model();}; // std::bind(_source_model_func, _tree_view);

            globalparameters.mainwindow()->setDisabled(true);
            result = static_cast<KnowModel *>(_current_model())->model_merge_to_left(
                target_on_tree                                                                                             // keep
                , source                                                                                             // remove   // std::bind(_know_model_board, this)
                , std::bind(&KnowView::view_delete_permanent, this, std::placeholders::_1, std::placeholders::_2, &KnowModel::model_delete_permanent_recursive, "cut", false)                                                                                              //
                );
            assert(_current_model()->item([ = ] (boost::intrusive_ptr<const TreeItem> t) {
                return t->id() == result->id();
            }));
            know_model_save();

            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->setEnabled(true);
        } else {                                                                 // if(target_on_tree)
            result = target_on_tree;
        }
    }

    return result;
}


void KnowView::on_current_changed(const QModelIndex &current, const QModelIndex &previous)
{
    //    Q_UNUSED(previous);
    if(previous.isValid()) {
        _previous_index = previous;
    }

    //    selectionModel()->select(previous, current_tree_selection_mode);
    auto result = _know_root->item(current);


    if(currentIndex() != current) {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                {
                    return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        select_as_current(tree_index);
    }
}

void KnowView::on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)
}

void KnowView::on_current_row_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current);
    //    Q_UNUSED(previous);

    //    auto index_list = selectionModel()->selectedIndexes();

    //    for(auto _i : index_list) { // always one element
    //        selectionModel()->select(_i, current_tree_selection_mode);
    //    }

    if(previous.isValid() && _previous_index != previous) {
        _previous_index = previous;
    }

    //        selectionModel()->select(previous, current_tree_selection_mode);    // dangerous

    //    auto it = source_model()->item(current);

    //    if(it && it != source_model()->root_item()) {
    //        if(current.isValid() && current_index() != current && !it->is_activated()) {

    auto result = _know_root->item(current);

    if(currentIndex() != current) {
        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                {
                    return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        select_as_current(tree_index);
    }

    //    index_invoke(current_index());    // current

    //        }
    //    }
}


//void KnowView::on_pressed(const QModelIndex &_index)
//{
//    if(_index.isValid()) {
//        auto result = _know_root->item(_index);
//        auto in = result->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
//            return il == result->linker()
//                   && il->host() == result
//                   && result->parent() == il->host_parent();
//        });
//        assert(in != -1);
//        auto result_index = select_as_current(TreeIndex([&] {return _know_root;}, result->parent(), result->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
//            return il == result->linker()
//                   && il->host() == result
//                   && result->parent() == il->host_parent();
//        })));
//        assert(result_index == _index); // failure!!!

//        //        selectionModel()->select(_index, current_tree_selection_mode);    //, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
//        //        selectionModel()->setCurrentIndex(_index, current_tree_current_index_mode);    //

//        // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current


//    }
//}


// Сохранение дерева веток на диск
void KnowView::know_model_save(void)
{
    //    if(!_know_branch->synchronized())
    //        know_root_holder::know_root()->synchronize(_know_branch->root_item());

    //    know_root_holder::know_root()->save();
    sychronize();
    _know_model_board->save();

}


// Перечитывание дерева веток с диска
void KnowView::know_model_reload(void)
{
    _know_model_board->reload();                 // do not do this if xml_file_name of this object is null
    //    know_root_holder::know_root()->reload();

}





// Действия при клике на ветку дерева
void KnowView::cursor_step_into(const QModelIndex &_index)
{
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());

    if(_index.isValid() && // !source_model()->item(_index)->is_activated()
        !globalparameters.entrance()->find([&] (boost::intrusive_ptr<const ::Binder> b) -> bool {
        bool result = false;
        auto ref_it = source_model()->item(_index);
        if(ref_it->binder()) {
            if(b->host()->field("url") == ref_it->field("url")) {
                if(ref_it->binder()->page())
                    if(ref_it->binder()->page()->view()->load_finished())
                        result = true;
            }
        }
        return result;
    })
        ) {
        // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

        // Сохраняется текст в окне редактирования в соответсвующий файл
        //    find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->save_text_area();
        auto _soure_model = [&] () {
                return source_model();
            };
        // Получаем указатель на текущую выбранную ветку дерева
        auto selected_item = _soure_model()->item(_index);

        if(selected_item->count_direct() > 0) {                                                 // && (result_item->field("url") != "" || result_item->field("url") == "" )
            // Вначале все инструменты работы с веткой включаются
            QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

            while(i.hasNext()) {
                i.next();
                i.value()->setEnabled(true);
            }

            // Проверяется, происходит ли клик по зашифрованной ветке
            if(selected_item->field("crypt") == "1") {
                // Если пароль доступа к зашифрованным данным не вводился в этой сессии
                if(globalparameters.crypt_key().length() == 0) {
                    // Запрашивается пароль
                    Password password;

                    if(password.retrievePassword() == false) {
                        // Устанавливаем пустые данные для отображения таблицы конечных записей
                        // find_object<RecordController>("table_screen_controller")

                        //                    result_item->unique_page()->view()->record_controller()->source_model()->clear();


                        boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_soure_model, boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, QMap<QString, QString>()))); } catch(std::exception &e) {throw e; } return tree_index; } ();
                        intercept(tree_absolute_index);                                                                                                                                                                                 // _know_branch->clear();  // root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));




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
                        QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

                        while(i.hasNext()) {
                            i.next();
                            i.value()->setEnabled(false);
                        }

                        return;                                                                                                                                                                                 // Программа дальше не идет, доделать...
                    }
                }
            }

            if(_soure_model()->root_item() != selected_item) {
                boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_soure_model, selected_item); } catch(std::exception &e) {throw e; } return tree_index; } ();
                intercept(tree_absolute_index);
            }                                                                                                 // _know_branch->intercept(result_item);     //? never do this!!!

            setExpanded(QModelIndex(), true);
            //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
            //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

            // Получаем указатель на данные таблицы конечных записей
            //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
            //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();



            // setup_model(_know_branch);

            _tree_screen->enable_up_action();                                                                                 // _know_branch->root_item()->id() != static_cast<TreeKnowModel *>(model())->root_item()->id()   // _know_branch->root_item()->id() != know_root()->root_item()->id()


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

                QStringList path = selected_item->path_list("name");

                // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                int emptyStringIndex = path.indexOf("");
                path.removeAt(emptyStringIndex);

                //            find_object<RecordScreen>(table_screen_singleton_name)
                globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

            }



            // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
            resizeColumnToContents(0);

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

        //        if(selected_item->field("url") != "")// logic error, root_item can not get activate!!!
        //            item_bind(selected_item, std::bind(&TreeScreen::view_paste_child, this
        //                                             , TreeIndex(soure_model, selected_item->parent(), 0)    // std::placeholders::_1
        //                                             , std::placeholders::_2, std::placeholders::_3))->activate();
    }
}

void KnowView::index_invoke(const QModelIndex &_index)
{


    if(_index.isValid()) {                 //       && index_current() != _index

        //        KnowModel *(TreeScreen::*_source_model_func)() = &TreeScreen::know_model_board;
        //        auto _source_model = std::bind(_source_model_func, this);

        //        KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
        auto _source_model = [&] () {
                return source_model();
            };                                                                                                       // std::bind(_source_model_func, _tree_view);

        //        auto tree_screen = dynamic_cast<TreeScreen *>(static_cast<QObject *>(this)->parent());
        //        auto name = tree_screen->objectName();
        //        assert(tree_screen->objectName() == tree_screen_singleton_name);

        //        auto _tree_screen = globalparameters.tree_screen();
        assert(_tree_screen->objectName() == tree_screen_singleton_name);
        // Получаем указатель на текущую выбранную ветку дерева
        auto result_item = source_model()->item(_index);

        auto merge_by_url = [&] () {
                auto duplicated_items_list_on_board_by_url = _source_model()->items([&] (boost::intrusive_ptr<const TreeItem> it) {
                return (it->field("url") == result_item->field("url")) && (it->field("url") != browser::Browser::_defaulthome);
            });                                                                                                                                                                                                                                                                                  // source_model()


                if(duplicated_items_list_on_board_by_url.size() > 1) {
                    for(auto _i : duplicated_items_list_on_board_by_url) {
                        auto duplicated_item = _i;

                        if(duplicated_item != result_item) {                                                                                                                                             // (duplicated_item->id() != result_item->id()) ||
                            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_source_model, result_item); } catch(std::exception &e) {throw e; } return tree_index; } ();
                            result_item = view_merge(tree_index, duplicated_item);
                        }
                    }
                }
            };

        (void)merge_by_url;

        //        auto duplicated_item_index_list_on_view = _source_model()->indexes([&](boost::intrusive_ptr<TreeItem> it) {return it->field("url") == result_item->field("url");});
        auto duplicated_items_list_on_board_by_id = _source_model()->items([&] (boost::intrusive_ptr<const TreeItem> it) {
            return it->id() == result_item->id();
        });


        if(duplicated_items_list_on_board_by_id.size() > 1) {
            for(auto _i : duplicated_items_list_on_board_by_id) {
                auto duplicated_item = _i;

                if(duplicated_item != result_item) {                                                                                                                 // (duplicated_item->id() != result_item->id()) ||
                    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_source_model, result_item); } catch(std::exception &e) {throw e; } return tree_index; } ();
                    result_item = view_merge(tree_index, duplicated_item);
                }
            }
        }

        if(// !result_item->is_activated()
            !globalparameters.entrance()->find([&] (boost::intrusive_ptr<const ::Binder> b){
            bool result = false;
//            auto result_item = source_model()->item(_index);
            if(result_item->binder()) {
                if(b->host()->field("url") == result_item->field("url")) {
                    if(result_item->binder()->page())
                        if(result_item->binder()->page()->view()->load_finished())
                            result = true;
                }
            }
            return result;
        })
            ) {
            // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

            // Сохраняется текст в окне редактирования в соответсвующий файл
            //    find_object<MainWindow>("mainwindow")
            globalparameters.mainwindow()->save_text_area();

            setExpanded(_index.parent(), true);                                                                                 // (isExpanded(_index)) ? setExpanded(_index, false) : setExpanded(_index, true);


            if(result_item->count_direct() > 0) {                                                                                 // && (result_item->field("url") != "" || result_item->field("url") == "" )
                // Вначале все инструменты работы с веткой включаются
                QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

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



                            //                    setup_model(boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, QMap<QString, QString>())));   // _know_branch->clear();  // root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));




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
                            QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

                            while(i.hasNext()) {
                                i.next();
                                i.value()->setEnabled(false);
                            }

                            return;                                                                                                                                                                                                                 // Программа дальше не идет, доделать...
                        }
                    }
                }

                //        if(source_model()->root_item() != result_item) setup_model(result_item); // _know_branch->intercept(result_item);     //? never do this!!!

                //        //        _shadow_candidate_model->_root_item->field("id", result_item->field("id"));
                //        //        _shadow_candidate_model->_root_item->field("name", result_item->field("name"));

                //        // Получаем указатель на данные таблицы конечных записей
                //        //    std::shared_ptr<TableData> record_tabledata = item->tabledata();
                //        //    std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();



                //        // setup_model(_know_branch);

                //        enable_up_action(); // _know_branch->root_item()->id() != static_cast<TreeKnowModel *>(model())->root_item()->id()   // _know_branch->root_item()->id() != know_root()->root_item()->id()


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

                    QStringList path = result_item->path_list("name");

                    // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                    int emptyStringIndex = path.indexOf("");
                    path.removeAt(emptyStringIndex);

                    //            find_object<RecordScreen>(table_screen_singleton_name)
                    globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

                }



                // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
                resizeColumnToContents(0);

                //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);

            }

            //        else {      //        if(result_item->field("url") != browser::Browser::_defaulthome)
            //            //        if(result_item->field("url") != "") globalparameters.entrance()->activate(result_item);
            //            // Переключаются окна (используется для мобильного интерфейса)
            //            //        globalparameters.window_switcher()->switchFromTreeToRecordtable();
            //        }

            if(result_item->field("url") != "") {

                //                TreeIndex modelindex([&] {return _know_root;}, current_item()->parent(), current_item()->parent()->sibling_order(current_item()));
                //                //            select_and_current(result_item);
                //                auto item_bind_ = [&]() {return item_bind(current_item(), result_item, std::bind(&KnowView::view_paste_child, this, modelindex, std::placeholders::_2, std::placeholders::_3))->activate();};
                auto browser = globalparameters.entrance()->activated_browser();
                //                auto previous_item = _know_root->item(_previous_index);
                auto record_previous_item = browser->record_screen()->record_controller()->source_model()->item(PosSource(browser->record_screen()->record_controller()->view()->previous_index().row()));
                boost::intrusive_ptr<RecordIndex> record_modelindex(nullptr);

                try {
                    record_modelindex = new RecordIndex([&] {return browser->record_screen()->record_controller()->source_model(); }, record_previous_item, result_item);
                } catch(std::exception &) {}

                auto activate = [&] (boost::intrusive_ptr<RecordIndex> record_modelindex) -> browser::WebView * {
                        return browser->item_bind(record_modelindex)->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));                                                                                                                                             // item_bind_();
                    };

                if(record_modelindex && !result_item->binder()) {
                    activate(record_modelindex);                                                                                                                                                 // browser->item_bind(record_modelindex)->activate();   // item_bind_();
                } else if(record_modelindex && result_item->binder() && !result_item->binder()->page()) {                                                                                                                                 // !result_item->binder()->integrity_internal()){
                    activate(record_modelindex);                                                                                                                                                 // browser->item_bind(record_modelindex)->activate();   // item_bind_();
                } else {
                    result_item->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));
                }

            }
        } else {
            //        auto c = _binder->page()->record_controller();
            auto v = result_item->binder()->page()->view();
            auto t = v->tabmanager();
            auto index = t->webViewIndex(v);

            if(index != t->currentIndex()) {                                                                                 // c->index<PosSource>(c->source_model()->index(_binder->item()))
                t->setCurrentIndex(index);
            }
        }
    }
}

QModelIndex KnowView::previous_index() const
{
    return _previous_index;
}


boost::intrusive_ptr<TreeItem> KnowView::cursor_follow_up(boost::intrusive_ptr<TreeItem> _new_session_root_item)
{
    auto current_root = source_model()->root_item();

    while(_new_session_root_item->is_ancestor_of(current_root)
        && _new_session_root_item->id() != current_root->id()
        ) {
        //        reset();
        //        source_model(_new_session_root_item->parent());
        current_root = cursor_follow_up();
    }

    //    auto result = _new_session_root_item;
    //    boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root;}, result->parent(), result->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == result->linker() && il->host() == result && result->parent() == il->host_parent();}));} catch(std::exception &e) {throw e;} return tree_index;}();
    //    select_as_current(tree_index);

    return current_root;                    // _new_session_root_item;
}



boost::intrusive_ptr<TreeItem> KnowView::cursor_follow_up(void)
{
    boost::intrusive_ptr<TreeItem> r;
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());
    std::function<bool(boost::intrusive_ptr<const TreeItem>)>
    _check_crypt_conflict = [&] (boost::intrusive_ptr<const TreeItem> current_root) {
            bool _result = false;
            // Вначале все инструменты работы с веткой включаются
            QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

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
                        source_model()->clear();                                                                                                                                             //root_item(boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(), nullptr)));

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
                        QMapIterator<QString, QAction *> i(_tree_screen->_actionlist);

                        while(i.hasNext()) {
                            i.next();
                            i.value()->setEnabled(false);
                        }

                        // return current_root; // Программа дальше не идет, доделать...
                    } else {_result = true; }
                }
            }

            return _result;
        };

    // Сохраняется текст в окне редактирования в соответсвующий файл
    globalparameters.mainwindow()->save_text_area();

    auto current_model = [&] () {
            return source_model();
        };
    auto _current_item = current_item();
    int origin_count = current_model()->count_records_all();
    r = current_model()->root_item();



    auto root_up_impl = [&] (boost::intrusive_ptr<TreeItem> root_item) {
            boost::intrusive_ptr<TreeItem> r;
            auto _root_up = root_item->parent();

            if(_root_up && !_check_crypt_conflict(_root_up)) {                                             // && _root_item->parent() != _know_model_board->root_item()
                r = root_item = _root_up;

                if(0 == _root_up->count_direct()) {tree_empty_controll(); }

                boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(current_model, _root_up); } catch(std::exception &e) {throw e; } return tree_index; } ();
                intercept(tree_absolute_index);
                auto it = _current_item->parent();
                boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, it->parent(), it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                    {
                        return il == it->linker() && il->host() == it && it->parent() == il->host_parent();
                    })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                select_as_current(tree_index);

                // Получаем указатель на данные таблицы конечных записей

                assert(source_model()->count_records_all() >= origin_count);

                // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
                if(appconfig.interface_mode() == "mobile") {

                    QStringList path = _root_up->path_list("name");

                    // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
                    int emptyStringIndex = path.indexOf("");
                    path.removeAt(emptyStringIndex);

                    //            find_object<RecordScreen>(table_screen_singleton_name)
                    globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

                }

                // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
                resizeColumnToContents(0);
            }

            return r;
        };

    if(_current_item) {

        if(_current_item->parent() && _current_item->parent() != r && _current_item->parent()->path_list().size() > r->path_list().size()) {
            auto result = _current_item->parent();
            boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, result->parent(), result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                    {
                        return il == result->linker() && il->host() == result && result->parent() == il->host_parent();
                    })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            select_as_current(tree_index);
        } else if(_current_item->parent() == r) {
            r = root_up_impl(r);
        }

    } else if(r->count_direct() == r->count_children_all()) {
        r = root_up_impl(r);
    }

    //    auto try_root_up    = current_model()->root_item()->parent();
    //    boost::intrusive_ptr<TreeItem> _root_up = try_root_up ? try_root_up : current_model()->root_item();
    //    assert(_root_up);

    //    if(!_check_crypt_conflict(_root_up)) {
    //        resetup_model(_root_item);  // _know_branch->intercept(current_root->parent());     //? dangerous to do this!!!

    //        if(_root_up != _know_model_board->root_item()) {

    //            if(!_check_crypt_conflict(_root_up)) {


    //                // Получаем указатель на данные таблицы конечных записей

    //                assert(source_model()->count_records_all() >= origin_count);

    //                // Set the text path to the final table entries for the mobile interface options    // Устанавливается текстовый путь в таблице конечных записей для мобильного варианта интерфейса
    //                if(appconfig.interface_mode() == "mobile") {

    //                    QStringList path = _root_up->path_absolute_as_name();

    //                    // Remove the empty element, if it exists (this can be the root, it has no name)    // Убирается пустой элемент, если он есть (это может быть корень, у него нет названия)
    //                    int emptyStringIndex = path.indexOf("");
    //                    path.removeAt(emptyStringIndex);

    //                    //            find_object<RecordScreen>(table_screen_singleton_name)
    //                    globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join(" > "));

    //                }

    //                // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
    //                resizeColumnToContents(0);
    //            }

    //        } else if(current_item->parent()) { // _root_item == _know_model_board->root_item()
    //            current_item = current_item->parent();

    //            if(current_item->parent()) {
    //                _root_up = current_item->parent();

    //                if(!_check_crypt_conflict(_root_up)) {
    //                    resetup_model(_root_up);  // _know_branch->intercept(know_root()->root_item());
    //                }
    //            } else {
    //                _root_up = _know_model_board->root_item();

    //                if(!_check_crypt_conflict(_root_up)) {
    //                    resetup_model(_root_up);  // _know_branch->intercept(know_root()->root_item());
    //                }
    //            }

    //            select_as_current(current_item);

    //        } else {    // _root_item == _know_model_board->root_item() && (current_item->parent() == _know_model_board->root_item())?
    //            if(!_check_crypt_conflict(_root_up)) {
    //                _root_up = cursor_follow_up_to_root();
    //            }
    //        }

    //        enable_up_action();
    //    }

    _tree_screen->enable_up_action();
    return r;

}



boost::intrusive_ptr<TreeItem> KnowView::cursor_follow_root(void)
{
    boost::intrusive_ptr<TreeItem> absolute_root;
    auto _source_model = [&] () {
            return source_model();
        };
    auto _current_item = current_item();
    //    auto _tree_screen = static_cast<TreeScreen *>(parent());

    absolute_root = _know_model_board->root_item();

    if(_source_model()->root_item() != absolute_root) {
        boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex(_source_model, absolute_root); } catch(std::exception &e) {throw e; } return tree_index; } ();
        intercept(tree_absolute_index);
    }                                 // _know_branch->intercept(know_root()->root_item());

    if(_current_item) {

        while(_current_item->parent()->parent()) {_current_item = _current_item->parent(); }

        boost::intrusive_ptr<TreeIndex> tree_index = [&] {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new TreeIndex([&] {return _know_root; }, _current_item->parent(), _current_item->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
                {
                    return il == _current_item->linker() && il->host() == _current_item && _current_item->parent() == il->host_parent();
                })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        select_as_current(tree_index);

    }

    _tree_screen->enable_up_action();
    return absolute_root;

}



boost::intrusive_ptr<TreeItem> KnowView::intercept(
    boost::intrusive_ptr<TreeIndex> modelindex    // boost::intrusive_ptr<TreeItem> _item    // QString id
    )
{
    //    auto prepared = know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id)));
    auto result = modelindex->parent();                  // _item;    // _know_model_board->item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == id;});     // _know_model_board->index(KnowModel::delegater(id))
    //    //    auto absolute_source_model = []() {return globalparameters.tree_screen()->know_model_board();};
    //    //    assert(_item == absolute_source_model()->root_item() || absolute_source_model()->item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() == _item->id();}));

    //    //    // _know_branch->intercept(prepared);    //item(_know_branch->index(prepared));

    //    resetup_model(result);   //    return _know_branch->intercept(know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id))));

    //    reset();


    sychronize();
    _know_model_board->save();

    //    if(_know_root && is_owner()) {
    //        delete _know_root;  // dangerous!
    //        _know_root = nullptr;
    //    }

    //    reset();
    // Модель подключется к виду
    _know_root->intercept(result);                  // source_model(result);
    reset();
    setModel(_know_root);
    cursor_follow_up(result);
    return result;
}


void KnowView::synchronized(bool _synchronized)
{
    _know_model_board->synchronized(_synchronized);
}

bool KnowView::synchronized()
{
    return _know_model_board->synchronized();
}

//void KnowView::zoom(double scale_)
//{
//    double tmp = scale_ / 1;
//    double reset = 1 / _previous_scale;

//    if(scale_ == _previous_scale) {
//        return;
//    }

//    //    scale(reset, reset);
//    resize(sizeHint() * reset);

//    //    scale(tmp, tmp);
//    _previous_scale = scale_;

//    resize(sizeHint() * tmp);
//    updateGeometry();
//    update();
//    emit zoomChanged();
//}
