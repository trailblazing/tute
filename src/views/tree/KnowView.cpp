#include <QDebug>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QMessageBox>

#include "main.h"
#include "KnowView.h"
#include "TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/KnowModel.h"
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

extern GlobalParameters globalparameters;
extern AppConfig appconfig;

enum QItemSelectionModel::SelectionFlag current_tree_selection_mode = QItemSelectionModel::SelectionFlag::Select;   // ClearAndSelect   Current       // ClearAndSelect //  | QItemSelectionModel::SelectionFlag::Rows
enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode = QItemSelectionModel::SelectionFlag::SelectCurrent;    // Select   // SelectCurrent

const char *knowtreeview_singleton_name = "knowtreeview";

KnowView::KnowView(QString _name, QWidget *_parent) : QTreeView(_parent), _know_root(nullptr)
{
    // Разрешение принимать Drop-события
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    setObjectName(_name);

    setMinimumSize(1, 1);     // 150, 250

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

    setSelectionMode(QAbstractItemView::ExtendedSelection);     // SingleSelection //// MultiSelection  //

    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);        // ScrollBarAlwaysOn


    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);
    setDragDropMode(QAbstractItemView::InternalMove);
    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));
    connect(static_cast<QTreeView *>(const_cast<KnowView *>(this)), &QTreeView::setModel, this, [&](QAbstractItemModel * model) {   // does not work
        _know_root = static_cast<KnowModel *>(model);
    });

    //    void clearSelection();
    //    virtual void clearCurrentIndex();

    //Q_SIGNALS:
    //    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);
    // Сигналы для обновления панели инструментов при изменении в selectionModel()
    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, _tree_screen, &TreeScreen::on_current_changed);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, _tree_screen, &TreeScreen::on_selection_changed);
}


KnowView::~KnowView()
{
    if(_know_root && is_owner()) {delete _know_root; _know_root = nullptr;}
}

void KnowView::setModel(KnowModel *model)
{
    _know_root = model;
    QTreeView::setModel(model);
}

//void KnowView::setModel(QAbstractItemModel *model)
//{
//    _know_root = static_cast<KnowModel *>(model);
//    QTreeView::setModel(model);
//}

bool KnowView::is_owner()
{
    assert(static_cast<TreeScreen *>(this->parent())->know_model_board()->root_item()->parent() == nullptr);
    return (_know_root != static_cast<TreeScreen *>(this->parent())->know_model_board()) && (_know_root->root_item()->parent() != nullptr);
}


void KnowView::sychronize()
{

    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);

    if(_know_root && is_owner()) {  // _know_root != _tree_screen->know_branch() // KnowView is the owner of _know_root
        browser::Entrance *_entrance = globalparameters.entrance();
        assert(_entrance);

        if(_tree_screen && _entrance && _know_root->root_item()->count_direct() > 0) {

            QMap<QString, QString> data;
            auto _source_model = [&]() {return _tree_screen->tree_view()->source_model();};
            data["id"]      =  get_unical_id(); //_know_root->root_item()->id();
            data["name"]    =  _know_root->root_item()->item_direct(0)->name();

            assert(_know_root->root_item()->parent());

            boost::intrusive_ptr<TreeItem> new_branch_item = boost::intrusive_ptr<TreeItem>(new TreeItem(_know_root->root_item(), data));

            bool modified = false;

            for(auto &browser : _entrance->browsers()) {
                auto tabmanager = browser->tabmanager();  // record_controller()->source_model();  // ->record_table();

                for(int i = 0; i < tabmanager->count(); i++) {
                    auto item = tabmanager->webView(i)->page()->bounded_item();

                    if(!_tree_screen->know_model_board()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == item->field("id");})) {

                        if(item->is_lite())item->to_fat();

                        item->parent(new_branch_item);
                        _source_model()->model_move_as_child_impl(new_branch_item, item, new_branch_item->work_pos());  // new_branch_item->child_insert(new_branch_item->work_pos(), item);
                        modified = true;
                    }
                }
            }


            if(modified) {
                //                new_branch_item->field("id", _know_root->root_item()->id());
                //                new_branch_item->field("name", _know_root->root_item()->name());
                assert(_tree_screen->know_model_board()->item([ = ](boost::intrusive_ptr<const TreeItem> t) {return t->id() == _know_root->root_item()->id();})); //_know_root->root_item()->id()

                // || _know_root->root_item()->id() == _tree_screen->know_branch()->root_item()->id()

                // assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));

                _tree_screen->view_paste_as_children_from_children_move(   // _tree_screen->know_branch()->index(0, _tree_screen->know_branch()->root_item()->current_count() - 1, QModelIndex())//,
                    TreeModel::ModelIndex([&]()->KnowModel * {return _know_root;}   // _tree_screen->know_branch()
                                          , _know_root->index(_know_root->item([&](boost::intrusive_ptr<const TreeItem> it)->bool {return it->id() == _tree_screen->session_root();})))   // current_index()
                    , new_branch_item
                    , [&](boost::intrusive_ptr<const TreeItem> target, boost::intrusive_ptr<const TreeItem> source)->bool {return target->field("url") == source->field("url") && target->field("name") == source->field("name");}
                );

                _tree_screen->synchronized(false);

                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
            }

            _know_root->update_index(current_index());  // selectionModel()->currentIndex()    // _tree_screen->view_index()

        }
    }
}

KnowModel *KnowView::source_model()const {return _know_root;}
KnowModel *KnowView::source_model() {return _know_root;}

void KnowView::source_model(boost::intrusive_ptr<TreeItem> _item)
{
    //    TreeScreen *_tree_screen = globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    //    assert(_tree_screen);

    //    if(_know_root && is_owner()) {  // _know_root != _tree_screen->know_branch() // KnowView is the owner of _know_root
    //        browser::Entrance *_entrance = globalparameters.entrance();
    //        assert(_entrance);

    //        if(_tree_screen && _entrance && _know_root->root_item()->current_count() > 0) {

    //            QMap<QString, QString> data;

    //            data["id"]      =  get_unical_id(); //_know_root->root_item()->id();
    //            data["name"]    =  _know_root->root_item()->child(0)->name();

    //            assert(_know_root->root_item()->parent());

    //            boost::intrusive_ptr<TreeItem> new_branch_item = boost::intrusive_ptr<TreeItem>(new TreeItem(_know_root->root_item(), data));

    //            bool modified = false;

    //            for(int w = 0; w < _entrance->browsers().size(); w++) {
    //                auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

    //                for(int i = 0; i < tabmanager->count(); i++) {
    //                    auto item = tabmanager->webView(i)->page()->current_item();

    //                    if(!_tree_screen->know_model_board()->is_item_id_exists(item->field("id"))) {

    //                        if(item->is_lite())item->to_fat();

    //                        item->parent(new_branch_item);
    //                        new_branch_item->insert_new_item(new_branch_item->work_pos(), item);
    //                        modified = true;
    //                    }
    //                }
    //            }


    //            if(modified) {
    //                //                new_branch_item->field("id", _know_root->root_item()->id());
    //                //                new_branch_item->field("name", _know_root->root_item()->name());
    //                assert(_tree_screen->know_model_board()->is_item_id_exists(_know_root->root_item()->id()));    // || _know_root->root_item()->id() == _tree_screen->know_branch()->root_item()->id()
    //                // assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));

    //                _tree_screen->insert_children(   // _tree_screen->know_branch()->index(0, _tree_screen->know_branch()->root_item()->current_count() - 1, QModelIndex())//,
    //                    new_branch_item
    //                    , _know_root    // _tree_screen->know_branch()
    //                );

    //                _tree_screen->know_model_board()->synchronized(false);
    //                _tree_screen->save_knowtree();
    //                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
    //            }
    //        }
    //    }

    static_cast<TreeScreen *>(this->parent())->know_model_save();

    if(_know_root && is_owner()) {
        delete _know_root;  // dangerous!
        _know_root = nullptr;
    }

    reset();
    _know_root = new KnowModel(_item, this->parent());
    setModel(_know_root);
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

    if(gesture->state() == Qt::GestureFinished)
        if(globalparameters.target_os() == "android")
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
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

        // Указатель на родительский элемент, чтобы далее получить модель данных
        TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // В модели данных отмечается элемент дерева, над которым находится курсор
        _know_root->setData(index, QVariant(true), Qt::UserRole);

        _tree_screen->synchronized(false);

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
        const ClipboardRecords *clipboardRecords;
        clipboardRecords = qobject_cast<const ClipboardRecords *>(event->mimeData());

        // Печать в консоль содержимого перетаскиваемого объекта (для отладки)
        clipboardRecords->print();

        // Выясняется элемент дерева, над которым был сделан Drop
        QModelIndex index = indexAt(event->pos());

        // Если отпускание мышки произошло не на ветке дерева, а на свободном пустом месте в области виджета дерева
        if(!index.isValid())
            return;

        // Указатель на родительский элемент
        TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен Drop
        auto tree_item_drop = _know_root->item(index);

        //        // Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
        //        auto tree_item_drop = tree_item_drop;    // ->record_table();

        // Исходная ветка в момент Drop (откуда переностся запись) - это выделенная курсором ветка
        QModelIndex indexFrom = current_index(); // selectionModel()->currentIndex();    // find_object<TreeScreen>(tree_screen_singleton_name)
        // static_cast<TreeScreen *>(this->parent())->view_index();

        // Выясняется ссылка на элемент дерева (на ветку), откуда переностся запись
        auto treeItemDrag = _know_root->item(indexFrom);

        // Если перенос происходит в ту же самую ветку
        if(indexFrom == index)
            return;

        // Если перенос происходит из не зашифрованной ветки в зашифрованную, а пароль не установлен
        if(treeItemDrag->field("crypt") != "1" &&
           tree_item_drop->field("crypt") == "1" &&
           globalparameters.crypt_key().length() == 0) {
            // Выводится уведомление что невозможен перенос без пароля
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Warning!"));
            msgBox.setText(tr("Cant move this item to encrypt item. Please open crypt item (entry password) before."));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();

            return;
        }


        // Перенос записей, хранящихся в MimeData
        // В настоящий момент в MimeData попадает только одна запись,
        // но в дальнейшем планируется переносить несколько записей
        // и здесь код подготовлен для переноса нескольких записей
        RecordController *_record_controller = treeItemDrag->bounded_page()->record_controller(); // find_object<RecordController>("table_screen_controller"); // Указатель на контроллер таблицы конечных записей
        browser::TabWidget *_tabmanager = treeItemDrag->bounded_page()->view()->tabmanager();

        for(int i = 0; i < clipboardRecords->size(); i++) {
            // Полные данные записи
            boost::intrusive_ptr<TreeItem> record = clipboardRecords->record(i);

            // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
            // В этот момент вид таблицы конечных записей показывает таблицу, из которой совершается Drag
            // TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);

            _tabmanager->closeTab(_tabmanager->indexOf(record->bounded_page()->view()));    // _record_controller->remove_child(record->field("id"));

            // Если таблица конечных записей после удаления перемещенной записи стала пустой
            if(_record_controller->row_count() == 0) {
                //                find_object<MetaEditor>(meta_editor_singleton_name)
                globalparameters.meta_editor()->clear_all(); // Нужно очистить поле редактирования чтобы не видно было текста последней удаленной записи
            }

            //            find_object<RecordScreen>(table_screen_singleton_name)
            _record_controller->record_screen()->tools_update();

            // Добавление записи в базу
            _know_root->model_move_as_child_impl(tree_item_drop, record, 0);  // tree_item_drop->child_insert(0, record, ADD_NEW_RECORD_TO_END);

            // Сохранение дерева веток
            //            find_object<TreeScreen>(tree_screen_singleton_name)
            static_cast<TreeScreen *>(this->parent())->know_model_save();
        }

        // Обновление исходной ветки чтобы было видно что записей убавилось
        _know_root->update_index(indexFrom);

        // Обновлении конечной ветки чтобы было видно что записей прибавилось
        _know_root->update_index(index);

        // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
        _know_root->setData(QModelIndex(), QVariant(false), Qt::UserRole);

        _tree_screen->synchronized(false);
    }
}

boost::intrusive_ptr<TreeItem> KnowView::current_item()
{
    return _know_root->item(current_index());   // selectionModel()->currentIndex()

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
QModelIndex KnowView::current_index(void)
{
    QModelIndex result = selectionModel()->currentIndex();
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

    //            _tree_view->selectionModel()->setCurrentIndex(cur_index // , current_tree_selection_mode    //
    //                                                          , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //                                                         );
    //            _tree_view->selectionModel()->select(cur_index  //, current_tree_selection_mode
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
    //        //                , current_tree_selection_mode
    //        //            );
    //        //        }
    //    }

    //    auto v = _treeknowview->selectionModel()->currentIndex();
    //    candidate_from_knowtree_item(cur_index);



    if(!result.isValid()) {
        boost::intrusive_ptr<TreeItem> new_item;

        if(selectionModel()->selectedIndexes().size() > 1) {
            result = selectionModel()->selectedIndexes().first();
        }

        if(result.isValid()) {
            new_item = _know_root->item(result);
        } else {
            if(_know_root->root_item()->count_direct() == 0) {
                //            selection_to_pos(_know_root->root_item()->item_direct(0));    //_know_root->root_item()->count_direct() - 1
                //        } else {
                TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

                if(_know_root->root_item()->parent()) {
                    //            auto parent_tree_screen_pointer = globalparameters.tree_screen();
                    do {
                        _tree_screen->tree_view()->reset();
                        _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
                    } while(_know_root->root_item()->parent() && _know_root->root_item()->parent()->count_direct() == 0);

                    //                _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
                } else if(_know_root->root_item()->count_direct() == 0) {
                    globalparameters.entrance()->activate(
                        QUrl(browser::Browser::_defaulthome)
                        , std::bind(&TreeScreen::view_paste_as_sibling, globalparameters.tree_screen(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
                    );
                }

                //            new_item = _know_root->root_item()->item_direct(0); // _know_root->root_item()->count_direct() - 1
            }

            //            else {
            new_item = _know_root->root_item()->item_direct(0);
            //            }
        }

        select_as_current(new_item);

        result = selectionModel()->currentIndex();
        //        selectionModel()->setCurrentIndex(_know_root->index(_know_root->root_item()->child(0)), QItemSelectionModel::ClearAndSelect);
        assert(result.isValid());    // this line is to be recovery
    }

    //    assert(cur_index.isValid());
    return result;  // cur_index;   // temporary setting???   //
    //        selectionModel()->currentIndex();
}

//// Получение индекса текущего элемента на котором стоит курсор
//QModelIndex KnowView::view_index_last(void)const
//{
//    //    if(!_tree_view->selectionModel()->currentIndex().isValid()) {
//    boost::intrusive_ptr<TreeItem> _item = nullptr;

//    //    KnowModel *_know_root = _know_root;  //_tree_view->source_model();

//    if(0 == _know_root->root_item()->current_count()) {
//        static_cast<TreeScreen *>(parent())->view_return_to_root();  //
//        //        _know_root = _tree_view->source_model();
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
//    //        _tree_view->clicked(cur_index);
//    //    candidate_from_knowtree_item(cur_index);

//    //    }

//    //    assert(_tree_view->selectionModel()->currentIndex().isValid());    // this line is to be recovery
//    return selectionModel()->currentIndex();
//}

QModelIndex KnowView::select_as_current(const QModelIndex &_index, std::function<QModelIndex(KnowView *, const QModelIndex &)> _strategy)
{
    QModelIndex _result;
    //    int pos = index.row();

    //    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
    //    if(pos != to_pos) {
    //        QMessageBox msgBox;
    //        msgBox.setText("In RecordView::setSelectionToPos() input pos not equal model pos");
    //        msgBox.exec();
    //    }

    //    int rowCount = _know_root->root_item()->count_direct();

    //    if(pos < rowCount) {  // pos > (rowCount - 1)   // return;
    _result = _strategy(this, _index);

    //    // Простой механизм выбора строки. Похоже, что его использовать не получится
    //    selectionModel()->select(_result
    //                             , (_result == _index) ? QItemSelectionModel::SelectionFlag::Select : QItemSelectionModel::SelectionFlag::Deselect
    //                            ); // QItemSelectionModel::SelectCurrent // current_tree_selection_mode
    // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current


    //    auto recordSourceModel = controller->getRecordTableModel();
    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

    // Установка засветки на нужный индекс
    // Set the backlight to the desired index
    selectionModel()->setCurrentIndex(_result, current_tree_current_index_mode); // QItemSelectionModel::SelectCurrent    // ClearAndSelect   // current_tree_selection_mode

    //    assert(_result == selectionModel()->currentIndex());

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000

    // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
    // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
    // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
    // Therefore, the recording must be made a virtual click to fill the final table of records
    if(appconfig.interface_mode() == "mobile")
        emit this->clicked(_result); // QModelIndex selIdx = recordSourceModel->index(pos, 0);

#endif

    // emit this->clicked(index);
    //    _result = currentIndex();

    scrollTo(_result);   // QAbstractItemView::PositionAtCenter

    //    this->setFocus();   // ?

    //    }
    return _result;
}


QModelIndex KnowView::select_as_current(boost::intrusive_ptr<TreeItem> _item, std::function<QModelIndex(KnowView *, const QModelIndex &)> _strategy)
{

    TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());   // globalparameters.tree_screen();   //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);
    // auto _tree_screen = globalparameters.tree_screen();

    QModelIndex _index = _know_root->index(_item);  // = _tree_screen->tree_view()->source_model()->index(_item);

    while(!_index.isValid()
          && _know_root->root_item() != _tree_screen->know_model_board()->root_item()
         ) {
        _tree_screen->cursor_follow_up_one_level();
        _index = _know_root->index(_item);
        //                _index_item = _tree_screen->tree_view()->source_model()->index(_item);
    }

    if(_index.isValid() && _index != selectionModel()->currentIndex()) {
        //                _tree_screen->tree_view()->selectionModel()->setCurrentIndex(_index, QItemSelectionModel::SelectionFlag::SelectCurrent);
        _index = select_as_current(_index, _strategy);

    }

    //    QModelIndex _index = _know_root->index(_item);  //_record_controller->pos_to_proxyindex(to_pos); // Модельный индекс в Proxy модели

    //    _index = select_and_current(_index, _strategy);

    return _index;
}

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

void KnowView::mousePressEvent(QMouseEvent *event)  // Q_DECL_OVERRIDE
{
    // get the buttons type
    Qt::MouseButtons mouse_button = event->buttons();

    // only the right mouse buton
    if(mouse_button == Qt::RightButton) {
        //        TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
        //select item at cursor position
        //        QPersistentModelIndex
        QModelIndex next_index = indexAt(event->pos());
        //        selectionModel()->select(next_index, QItemSelectionModel::SelectCurrent);
        select_as_current(next_index);

        //        // start the context menu
        //        QModelIndexList select_indexes(selectedIndexes());

        //        if(select_indexes.size() > 0 && select_indexes[0].isValid()) {
        //            _tree_screen->_context_menu->exec(event->pos());  // QCursor::pos()
        //        }

    } else {
        //call the parents function
        QTreeView::mousePressEvent(event);
    }
}



QModelIndexList KnowView::index_localize(const QModelIndexList _origin_index_list)
{
    QModelIndexList _selectitems;

    if(_origin_index_list.size() > 0) {
        //        auto _source_model = _tree_view->source_model();
        // На время удаления блокируется главное окно
        //        //    find_object<MainWindow>("mainwindow")
        //        globalparameters.mainwindow()->setDisabled(true);
        //        //    find_object<MainWindow>("mainwindow")
        //        globalparameters.mainwindow()->blockSignals(true);

        //        assert(_origin_index_list.first() == _tree_view->current_index());

        if(!_origin_index_list.contains(current_index())) {

            //            KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
            auto _source_model = [&]() {return _know_root;}; // std::bind(_source_model_func, _tree_view);

            //            QModelIndex _current_index;
            boost::intrusive_ptr<TreeItem> duplicated_item = current_item();
            boost::intrusive_ptr<TreeItem> found;

            for(int i = 0; i < _origin_index_list.size(); ++i) {
                auto _index = _origin_index_list.at(i);

                if(duplicated_item->id() == _source_model()->item(_index)->id()) {
                    //                    _current_index = _index;
                    found = _source_model()->item(_index);
                    break;
                }
            }

            TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());

            if(found && duplicated_item && found != duplicated_item) {
                found = _source_model()->model_duplicated_merge_to_left(
                            std::bind(&TreeScreen::view_delete_items, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
                            , found, duplicated_item
                        );
            }
        }


        //preparations
        auto _index_current         = current_index();    //_origin_index_list.first();
        auto _index_common_parent   = _index_current.parent();
        //    auto index_to_be_delete_last = _index_list.last();
        //        auto _item_to_be_deleted_first = _source_model->item(index_to_be_delete_first);
        //        auto deleted_position_first = _item_to_be_deleted_first->sibling_order(); // items_update_selected();
        //            auto deleted_position_last = _source_model->item(index_to_be_delete_last)->sibling_order();
        //        auto _item_parent = _item_to_be_deleted_first->parent();



        //        // Список имен веток, которые нужно удалить
        //        QStringList branches_name;

        for(int i = 0; i < _origin_index_list.size(); ++i) {
            QModelIndex index = _origin_index_list.at(i);

            if(index.isValid() && index.parent() == _index_common_parent) {
                //                auto it = _source_model->item(index);
                //                branches_name << it->field("name");
                _selectitems.push_back(index);
            }
        }
    }

    return _selectitems;
}


bool KnowView::is_index_localized(const QModelIndexList _origin_index_list)const
{
    bool result = true;

    if(_origin_index_list.size() > 0) {
        //        assert(_origin_index_list.last() == _tree_view->current_index()); // means duplicated item found
        //preparations
        auto _index_current = _origin_index_list.last();    // _tree_view->current_index();    //

        auto _index_common_parent = _index_current.parent();


        for(int i = 0; i < _origin_index_list.size(); ++i) {
            QModelIndex index = _origin_index_list.at(i);

            if(index.isValid() && index.parent() != _index_common_parent) {
                result = false; break;
            }
        }
    }

    return result;
}







