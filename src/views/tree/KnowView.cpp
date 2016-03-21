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

enum QItemSelectionModel::SelectionFlag current_tree_selection_mode = QItemSelectionModel::SelectionFlag::Select;

const char *knowtreeview_singleton_name = "knowtreeview";

KnowView::KnowView(QWidget *parent) : QTreeView(parent), _know_root(nullptr)
{
    // Разрешение принимать Drop-события
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);
    setDragDropMode(QAbstractItemView::InternalMove);
    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));
    connect(static_cast<QTreeView *>(const_cast<KnowView *>(this)), &QTreeView::setModel, this, [&](QAbstractItemModel * model) {   // does not work
        _know_root = static_cast<KnowModel *>(model);
    });
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

            data["id"]      =  get_unical_id(); //_know_root->root_item()->id();
            data["name"]    =  _know_root->root_item()->child(0)->name();

            assert(_know_root->root_item()->parent());

            boost::intrusive_ptr<TreeItem> new_branch_item = boost::intrusive_ptr<TreeItem>(new TreeItem(_know_root->root_item(), data));

            bool modified = false;

            for(int w = 0; w < _entrance->browsers().size(); w++) {
                auto tabmanager = _entrance->browsers().at(w)->record_screen()->tabmanager();  // record_controller()->source_model();  // ->record_table();

                for(int i = 0; i < tabmanager->count(); i++) {
                    auto item = tabmanager->webView(i)->page()->bounded_item();

                    if(!_tree_screen->know_model_board()->is_id_exists(item->field("id"))) {

                        if(item->is_lite())item->to_fat();

                        item->parent(new_branch_item);
                        new_branch_item->child_move(new_branch_item->work_pos(), item);
                        modified = true;
                    }
                }
            }


            if(modified) {
                //                new_branch_item->field("id", _know_root->root_item()->id());
                //                new_branch_item->field("name", _know_root->root_item()->name());
                assert(_tree_screen->know_model_board()->is_id_exists(_know_root->root_item()->id()));    // || _know_root->root_item()->id() == _tree_screen->know_branch()->root_item()->id()
                // assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));

                _tree_screen->branch_paste_new_children_only(   // _tree_screen->know_branch()->index(0, _tree_screen->know_branch()->root_item()->current_count() - 1, QModelIndex())//,
                    _know_root    // _tree_screen->know_branch()
                    , index_current()
                    , new_branch_item
                );

                _tree_screen->synchronized(false);

                // tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
            }

            _tree_screen->branch_update_on_screen(
                index_current() // selectionModel()->currentIndex()    // _tree_screen->view_index()
            );
        }
    }
}

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

    static_cast<TreeScreen *>(this->parent())->knowtree_save();

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
        _tree_screen->tree_view()->source_model()->setData(index, QVariant(true), Qt::UserRole);

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
        auto tree_item_drop = _tree_screen->tree_view()->source_model()->item(index);

        //        // Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
        //        auto tree_item_drop = tree_item_drop;    // ->record_table();

        // Исходная ветка в момент Drop (откуда переностся запись) - это выделенная курсором ветка
        QModelIndex indexFrom = index_current(); // selectionModel()->currentIndex();    // find_object<TreeScreen>(tree_screen_singleton_name)
        // static_cast<TreeScreen *>(this->parent())->view_index();

        // Выясняется ссылка на элемент дерева (на ветку), откуда переностся запись
        auto treeItemDrag = _tree_screen->tree_view()->source_model()->item(indexFrom);

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
            if(_record_controller->row_count() == 0)
                //                find_object<MetaEditor>(meta_editor_singleton_name)
                globalparameters.meta_editor()->clear_all(); // Нужно очистить поле редактирования чтобы не видно было текста последней удаленной записи

            //            find_object<RecordScreen>(table_screen_singleton_name)
            _record_controller->record_screen()->tools_update();

            // Добавление записи в базу
            tree_item_drop->child_move(0, record, ADD_NEW_RECORD_TO_END);

            // Сохранение дерева веток
            //            find_object<TreeScreen>(tree_screen_singleton_name)
            static_cast<TreeScreen *>(this->parent())->knowtree_save();
        }

        // Обновление исходной ветки чтобы было видно что записей убавилось
        _tree_screen->branch_update_on_screen(indexFrom);

        // Обновлении конечной ветки чтобы было видно что записей прибавилось
        _tree_screen->branch_update_on_screen(index);

        // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
        _tree_screen->tree_view()->source_model()->setData(QModelIndex(), QVariant(false), Qt::UserRole);

        _tree_screen->synchronized(false);
    }
}



// Get the index of the current element on which the cursor is positioned   // Получение индекса текущего элемента на котором стоит курсор
QModelIndex KnowView::index_current(void)
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

    if(selectionModel()->selectedIndexes().size() > 0) {
        result = selectionModel()->selectedIndexes().first();
    }

    if(!result.isValid()) {
        if(_know_root->root_item()->count_direct() > 0) {
            selection_to_pos(_know_root->root_item()->count_direct() - 1);
        } else {
            TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

            //            auto parent_tree_screen_pointer = globalparameters.tree_screen();
            while(_know_root->root_item()->parent() && _know_root->root_item()->parent()->count_direct() == 0) {
                _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
            }

            if(_know_root->root_item()->parent()) {
                _tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
            } else if(_know_root->root_item()->count_direct() == 0) {
                globalparameters.entrance()->activate<url_full>(QUrl(browser::Browser::_defaulthome));
            }

            selection_to_pos(_know_root->root_item()->count_direct() - 1);
        }

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

QModelIndex KnowView::selection_to_pos(boost::intrusive_ptr<TreeItem> _item)
{
    QModelIndex index = _know_root->index(_item);//_record_controller->pos_to_proxyindex(to_pos); // Модельный индекс в Proxy модели
    int pos = index.row();

    //    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
    //    if(pos != to_pos) {
    //        QMessageBox msgBox;
    //        msgBox.setText("In RecordView::setSelectionToPos() input pos not equal model pos");
    //        msgBox.exec();
    //    }

    int rowCount = _know_root->root_item()->count_direct();

    if(pos < rowCount) {  // pos > (rowCount - 1)   // return ;


        // Простой механизм выбора строки. Похоже, что его использовать не получится
        selectionModel()->select(index, current_tree_selection_mode // QItemSelectionModel::SelectCurrent
                                 // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
                                );

        //    auto recordSourceModel = controller->getRecordTableModel();
        //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

        // Установка засветки на нужный индекс
        // Set the backlight to the desired index
        selectionModel()->setCurrentIndex(index, current_tree_selection_mode // QItemSelectionModel::SelectCurrent    // ClearAndSelect
                                         );

        // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
        // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
        // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
        // Therefore, the recording must be made a virtual click to fill the final table of records
        if(appconfig.getInterfaceMode() == "mobile")
            emit this->clicked(index); // QModelIndex selIdx=recordSourceModel->index(pos, 0);

        // emit this->clicked(index);

        scrollTo(currentIndex());   // QAbstractItemView::PositionAtCenter

        this->setFocus();   // ?
    }

    return index;
}

QModelIndex KnowView::selection_to_pos(int to_pos)
{
    //    bool result = false;
    QModelIndex index;

    if(to_pos != -1) {
        auto item = _know_root->root_item()->item(to_pos);
        index = selection_to_pos(item);
        //        result = true;
    }

    return index;
}
