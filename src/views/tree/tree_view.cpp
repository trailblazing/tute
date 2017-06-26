#include <thread>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QAbstractItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QMimeData>
#include <QScrollArea>

//#include "models/tree/tree_item.dec"
#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_branch.h"
#include "libraries/clipboard_records.h"
#include "libraries/crypt/password.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "tree_screen.h"
#include "tree_view.h"
#include "utility/lease.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"

extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<AppConfig> appconfig;
extern const char* clipboard_items_root;

// extern const char	*action_set_as_session_root;
extern const char* action_find_in_base;
extern const char* action_cursor_follow_root;
extern const char* action_cursor_follow_up;
extern const char* action_expand_all_subbranch;
extern const char* action_collapse_all_subbranch;
extern const char* action_move_up_branch;
extern const char* action_move_dn_branch;
extern const char* action_insert_sub_branch;
extern const char* action_insert_branch;
extern const char* action_edit_branch;
extern const char* action_delete_branch;
extern const char* action_cut_branch;
extern const char* action_copy_branch;
extern const char* action_paste_branch;
extern const char* action_paste_sub_branch;
extern const char* action_encrypt_branch;
extern const char* action_decrypt_branch;
// extern const char	*action_freeze_browser_view;
extern const char* action_edit_field;
extern const char* action_editor_switch;

//#ifdef QT_DEBUG
// const char *index_xml_file_name = "index.xml"; // "index_debug.xml";
//#else
// const char *index_xml_file_name = "index.xml";
//#endif

enum QItemSelectionModel::SelectionFlag current_tree_selection_mode =
    QItemSelectionModel::SelectionFlag::
        Select; // ClearAndSelect   Current       // ClearAndSelect //  |
                // QItemSelectionModel::SelectionFlag::Rows
enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode =
    QItemSelectionModel::SelectionFlag::SelectCurrent; // Select   //
                                                       // SelectCurrent

const char* knowtreeview_singleton_name = "knowtreeview";

// W_OBJECT_IMPL(HtmlDelegate)

HtmlDelegate::HtmlDelegate(tv_t* _tree_view)
    : _tree_view(_tree_view)
{
}

// http://stackoverflow.com/questions/1956542/how-to-make-item-view-render-rich-html-text-in-qt
void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // QStyleOptionViewItemV4
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle* style =
        optionV4.widget ? optionV4.widget->style() : QApplication::style();

    auto source_model = [&] { return _tree_view->source_model(); };
    auto index_item_ = source_model()->item(index);
    QTextDocument doc;
    // if(index ==
    // static_cast<QModelIndex>(source_model()->index([&](boost::intrusive_ptr<const
    // Linker> it){return it->host()->id() ==
    // source_model()->session_id();})))optionV4.text = "<b>" + optionV4.text +
    // "</b>";
    if (index_item_->id() ==
        _tree_view->current_item()->id() // source_model()->session_id()
        )
        optionV4.text = "<b>" + optionV4.text + "</b>";
    doc.setHtml(optionV4.text);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;
    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(
            QPalette::Text,
            optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
    if (0 == index_item_->count_direct() &&
        !(optionV4.state & QStyle::State_Selected))
        ctx.palette.setColor(
            QPalette::Text,
            optionV4.palette.color(QPalette::Inactive, QPalette::Shadow));
    QRect textRect =
        style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);

    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // QStyleOptionViewItemV4
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());

    return QSize(doc.idealWidth(), doc.size().height());
}

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(tv_t)
#endif

tv_t::tv_t(QString name, ts_t* tree_screen)
    : QTreeView(tree_screen)
    , _tree_screen(tree_screen)
    , _know_model_board(new tkm_t(this))
    , _know_root(new tkm_t(_know_model_board->root_item(), this))
    , _delegate(new HtmlDelegate(this)) // create custom delegate
{
    setModel(_know_root);
    // _know_model_board->init_from_xml(appconfig->tetra_dir() + "/" +
    // index_xml_file_name);  //
    // _know_branch->intercept(know_root_holder::know_root()->root_item());    //
    // init_from_xml(xml);  //
    // int all_count = _know_model_board->count_records_all();
    // assert(all_count > 0);

    // _know_model_board->synchronized(true);

    // source_model(_know_model_board->root_item());

    // азрешение принимать Drop-события
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    setObjectName(name);

    setMinimumSize(1, 1); // 150, 250

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

    setSelectionMode(QAbstractItemView::ExtendedSelection); // SingleSelection
                                                            // //// MultiSelection
                                                            // //

    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // ScrollBarAlwaysOn

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Представление не должно позволять редактировать элементы обычным путем
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // азрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);
    setDragDropMode(QAbstractItemView::InternalMove);

    setMinimumSize(size());
    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView*>(this));

    setItemDelegate(_delegate);

    setIndentation(0);
    setRootIsDecorated(false);
    // setStyleSheet("QTreeView::branch {  border-image: url(none.png); }");

    // deleted after introduce intercept method
    // connect(static_cast<QTreeView *>(const_cast<KnowView *>(this)),
    // &QTreeView::setModel, this, [&](QAbstractItemModel * model) {   // does not
    // work
    // _know_root = static_cast<KnowModel *>(model);
    // });

    // void clearSelection();
    // virtual void clearCurrentIndex();

    // Q_SIGNALS:
    // void selectionChanged(const QItemSelection &selected, const QItemSelection
    // &deselected);

    // TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
    // // globalparameters.tree_screen();
    // //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(tree_screen);

    // QScrollArea* qscrollarea = new QScrollArea(this);
    ////        qscrollarea->setWidget(this);
    // auto l = new QVBoxLayout();
    // l->addWidget(qscrollarea);
    // this->setLayout(l);

    // qscrollarea->show();
    auto selectionmodel = this->selectionModel();
    assert(selectionmodel);
    // Сигналы для обновления панели инструментов при изменении в selectionModel()
    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &tv_t::on_current_changed);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &tv_t::on_selection_changed);
    // Соединение сигнал-слот что ветка выбрана мышкой или стрелками на клавиатуре
    if (appconfig->interface_mode() == "desktop")
        connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &tv_t::on_current_row_changed); // &TreeScreen::step_into_sub_branch
    // connect(this, &KnowView::pressed, this, &KnowView::on_pressed);
    if (appconfig->interface_mode() == "mobile")
        connect(this, &tv_t::clicked, this, [&](const QModelIndex& index) {
            cursor_focus(source_model()->item(index));
        }); // (index_tree(index));
    if (appconfig->interface_mode() == "desktop") {
        connect(this, &tv_t::clicked, this, [&](const QModelIndex& index) {
            auto _item = _know_root->child(index);
            if (_item)
                index_invoke(
                    TreeIndex::item_require_treeindex([&] { return _know_root; }, _item));
        }); // globalparameters.main_window()->vtab_record()->activated_browser()->tabmanager()->currentWebView(),
        // [&](const QModelIndex index) {
        ////            collapse(index);
        ////            expand(index);
        // (isExpanded(index)) ? setExpanded(index, false) : setExpanded(index,
        // true);
        // auto result_item = source_model()->item(index);

        // if(result_item->field("url") != "")
        // globalparameters.entrance()->activate(result_item);
        // }//&TreeScreen::candidate_from_knowtree_item)

        connect(this, &tv_t::doubleClicked, this, [&](const QModelIndex& index) {
            cursor_focus(source_model()->item(index));
        }); // (index_tree(index));	// index_invoke_in_sub_branch
    }
}

tv_t::~tv_t()
{
    if (_know_root) { // && is_owner()
        // delete
        _know_root->deleteLater();
        _know_root = nullptr;
    }
    if (_know_model_board) {
        // delete
        _know_model_board->deleteLater();
        _know_model_board = nullptr;
    }
}

void tv_t::setModel(QAbstractItemModel* model)
{
    _know_root = dynamic_cast<tkm_t*>(model);
    QTreeView::setModel(model);
}

// void KnowView::setModel(QAbstractItemModel *model)
// {
// _know_root = static_cast<KnowModel *>(model);
// QTreeView::setModel(model);
// }

// bool KnowView::is_owner()
// {
////    auto tree_screen = static_cast<TreeScreen *>(this->parent());
// assert(_know_model_board->root_item()->parent() == nullptr);
// return (_know_root != _know_model_board)
// && (_know_root->root_item()->parent() != nullptr);
// }

// void tv_t:: sychronize(){
//// TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
///// globalparameters.tree_screen();
/////find_object<TreeScreen>(tree_screen_singleton_name);
// assert(_tree_screen);
// auto tree_view = _tree_screen->view();
// if(_know_root){					// && is_owner()
//// _know_root != _tree_screen->know_branch() // KnowView is the owner of
///_know_root
// web::Entrance *_entrance = globalparameters.entrance();
// assert(_entrance);
// if(_tree_screen && _entrance && _know_root->root_item()->count_direct() > 0){
////
// QMap<QString, QString> data;
//// auto _source_model = [&]() {return
///_tree_screen->tree_view()->source_model();};
// data["id"]		= get_unical_id();
// // _know_root->root_item()->id();
// data["name"]	= "current session branch item";
// // _know_root->root_item()->item_direct(0)->name();

//// assert(_know_root->root_item()->parent());

// boost::intrusive_ptr<TreeItem> branch_item = TreeItem::dangle_instance(data);
// // boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));
// // _know_root->root_item()

// bool modified = false;
// for(auto &browser : _entrance->browsers()){
// auto tabmanager = browser->tabmanager();
// // record_ctrl()->source_model();  // ->record_table();
// for(int i = 0; i < tabmanager->count(); i ++){
// auto page_item = tabmanager->webView(i)->page()->host();	// should never
// occur in current implement, 2016-07-10
// if(! _know_model_board->child([=](boost::intrusive_ptr<const TreeItem>
// t){return t->id() == page_item->field<id_type>();})){
// if(page_item->is_lite())page_item->to_fat();
//// page_item->parent(branch_item);
//// _source_model()->model_move_as_child_impl(branch_item, page_item,
///branch_item->work_pos());  //
///new_branch_item->child_insert(new_branch_item->work_pos(), item);
// branch_item << page_item;
// modified = true;
// }
// }
// }
// if(modified){
//// new_branch_item->field("id", _know_root->root_item()->id());
//// new_branch_item->field("name", _know_root->root_item()->name());
// assert(_know_model_board->child([=](boost::intrusive_ptr<const TreeItem>
// t){return t->id() == _know_root->root_item()->id();}));
// // _know_root->root_item()->id()

////// || _know_root->root_item()->id() ==
///_tree_screen->know_branch()->root_item()->id()

//////
///assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));
//// boost::intrusive_ptr<TreeIndex> tree_index;

//// try {
//// tree_index = new TreeIndex([&] () -> KnowModel * {
//// return _know_root;
//// }, tree_view->session_root_auto());
//// } catch(std::exception &) {}

//// if(tree_index) {
// paste_children_from_children(TreeIndex::instance([&]() -> tkm_t * {return
// _know_root;}, tree_view->session_root_auto(),
// tree_view->session_root_auto()->parent())// tree_index
// , branch_item
// , [&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const
// Linker> source) -> bool {return target->host()->field<url_type>() ==
// source->host()->field<url_type>() && target->host()->field<name_type>() ==
// source->host()->field<name_type>();}
// );

// synchronized(false);
//// }

////
///tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
// }
// _know_root->update_index(_know_root->index(tree_view->session_root_auto()));
// // _know_root->update_index(current_index());  //
// selectionModel()->currentIndex()    // _tree_screen->view_index()
// }
// }
// }

tkm_t* tv_t::source_model() const
{
    return _know_root;
}

tkm_t* tv_t::source_model()
{
    return _know_root;
}

// void KnowView::source_model(boost::intrusive_ptr<TreeItem> _item)
// {
////    TreeScreen *_tree_screen = globalparameters.tree_screen();
/////find_object<TreeScreen>(tree_screen_singleton_name);
////    assert(_tree_screen);

////    if(_know_root && is_owner()) {  // _know_root !=
///_tree_screen->know_branch() // KnowView is the owner of _know_root
////        web::Entrance *_entrance = globalparameters.entrance();
////        assert(_entrance);

////        if(_tree_screen && _entrance &&
///_know_root->root_item()->current_count() > 0) {

////            QMap<QString, QString> data;

////            data["id"]      =  get_unical_id();
/////_know_root->root_item()->id();
////            data["name"]    =  _know_root->root_item()->child(0)->name();

////            assert(_know_root->root_item()->parent());

////            boost::intrusive_ptr<TreeItem> new_branch_item =
///boost::intrusive_ptr<TreeItem>(new TreeItem(_know_root->root_item(), data));

////            bool modified = false;

////            for(int w = 0; w < _entrance->browsers().size(); w++) {
////                auto tabmanager =
///_entrance->browsers().at(w)->record_screen()->tabmanager();  //
///record_ctrl()->source_model();  // ->record_table();

////                for(int i = 0; i < tabmanager->count(); i++) {
////                    auto item =
///tabmanager->webView(i)->page()->current_item();

////
///if(!_tree_screen->know_model_board()->is_item_id_exists(item->field("id"))) {

////                        if(item->is_lite())item->to_fat();

////                        item->parent(new_branch_item);
//// new_branch_item->insert_new_item(new_branch_item->work_pos(), item);
////                        modified = true;
////                    }
////                }
////            }

////            if(modified) {
////                //                new_branch_item->field("id",
///_know_root->root_item()->id());
////                //                new_branch_item->field("name",
///_know_root->root_item()->name());
////
///assert(_tree_screen->know_model_board()->is_item_id_exists(_know_root->root_item()->id()));
///// || _know_root->root_item()->id() ==
///_tree_screen->know_branch()->root_item()->id()
////                //
///assert(_tree_screen->know_branch()->is_item_id_exists(_know_root->root_item()->parent()->id()));

////                _tree_screen->insert_children(   //
///_tree_screen->know_branch()->index(0,
///_tree_screen->know_branch()->root_item()->current_count() - 1,
///QModelIndex())//,
////                    new_branch_item
////                    , _know_root    // _tree_screen->know_branch()
////                );

////                _tree_screen->know_model_board()->synchronized(false);
////                _tree_screen->save_knowtree();
////                //
///tree_screen->to_candidate_screen(entrance->shadow_branch()->index(tree_item));
////            }
////        }
////    }

////    static_cast<TreeScreen *>(this->parent())->know_model_save();

// sychronize();
// _know_model_board->save();

////    if(_know_root && is_owner()) {
////        delete _know_root;  // dangerous!
////        _know_root = nullptr;
////    }

// reset();
// _know_root->intercept(_item);   // =  new KnowModel(_item, this);
////    setModel(_know_root);
// }

void tv_t::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
    (void)painter;
    (void)rect;
    (void)index;
    // There is a 0px identation set in the constructor, with setIndentation(0).
    // Because of that, no branches are shown, so they should be disabled
    // completely (overriding drawBranches).
    // Leaving branches enabled with 0px identation results in a 1px branch line
    // on the left of all items,
    // which looks like an artifact.
}

void tv_t::resizeEvent(QResizeEvent* e)
{
    // (void)e;
    resize(_tree_screen->width(), height());
    QTreeView::resizeEvent(e);
}

// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool tv_t::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event
                 << " Event type: " << event->type();

        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QTreeView::event(event);
}

// Обработчик жестов
// Вызывается из обработчика событий
bool tv_t::gestureEvent(QGestureEvent* event)
{
    qDebug() << "In gestureEvent()" << event;
    if (QGesture* gesture = event->gesture(Qt::TapAndHoldGesture))
        tapandhold_gesture_triggered(static_cast<QTapAndHoldGesture*>(gesture));
    return true;
}

// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void tv_t::tapandhold_gesture_triggered(QTapAndHoldGesture* gesture)
{
    qDebug() << "In tapandhold_gesture_triggered()" << gesture;
    if (gesture->state() == Qt::GestureFinished)
        if (gl_paras->target_os() == "android")
            emit tapandhold_gesture_finished(
                mapFromGlobal(gesture->position().toPoint()));
}

void tv_t::mousePressEvent(QMouseEvent* event)
{ // Q_DECL_OVERRIDE
    // get the buttons type
    Qt::MouseButtons mouse_button = event->buttons();
    QModelIndex _index = indexAt(event->pos());
    assert(_index.isValid());
    // only the right mouse buton
    if (mouse_button == Qt::RightButton) {
        // TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
        // select item at cursor position
        // QPersistentModelIndex
        QModelIndex next_index = indexAt(event->pos());
        if (next_index.isValid()) {
            ////            //        // Получение списка индексов QModelIndex
            ///выделенных элементов
            ////            //        qSort(_selectitems.begin(), _selectitems.end());

            ////            selectionModel()->clear();
            // auto selection = LocalLizeInitializer(this)();
            // selectionModel()->select(selection, current_tree_selection_mode);
            auto it = _know_root->child(next_index);
            // boost::intrusive_ptr<TreeIndex> tree_index = [&]
            // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
            // TreeIndex([&] {return _know_root; }, it->parent(),
            // it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
            // {
            // return il == it->linker() && il->host() == it && it->parent() ==
            // il->host_parent();
            // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            select_as_current(
                TreeIndex::item_require_treeindex([&] { return _know_root; }, it));

            //// start the context menu
            // QModelIndexList select_indexes(selectedIndexes());

            // if(select_indexes.size() > 0 && select_indexes[0].isValid()) {
            // _tree_screen->_context_menu->exec(event->pos());  // QCursor::pos()
            // }
        }
    } else {
        // call the parents function
        QTreeView::mousePressEvent(event);
    }
}

void tv_t::dragEnterEvent(QDragEnterEvent* event)
{
    if (is_dragable(event)) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void tv_t::dragMoveEvent(QDragMoveEvent* event)
{
    if (is_dragable(event)) {
        event->acceptProposedAction();

        // Выясняется элемент дерева, над которым находится курсор
        QModelIndex index = indexAt(event->pos());

        //// Указатель на родительский элемент, чтобы далее получить модель данных
        // TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // В модели данных отмечается элемент дерева, над которым находится курсор
        _know_root->setData(index, QVariant(true), Qt::UserRole);

        synchronized(false);
    } else
        event->ignore();
}

// template <class X>
// bool KnowView::isDragableData(X *event)
// {
//// Проверяется, содержит ли объект переноса данные нужного формата
// const QMimeData *mimeData = event->mimeData();

// if(mimeData == nullptr)
// return false;

// if(!(mimeData->hasFormat("tute/records")))
// return false;

// QObject *sourceObject = qobject_cast<QObject *>(event->source());

// if(sourceObject->objectName() == "record_view") // "recordTableView"
// return true;
// else
// return false;
// }

void tv_t::dropEvent(QDropEvent* event)
{
    qDebug() << "dropEvent() - Start";
    if (is_dragable(event)) {
        qDebug() << "Try move record by drag and drop";

        // Извлечение объекта
        const ClipboardRecords* clipboard_records;
        clipboard_records =
            qobject_cast<const ClipboardRecords*>(event->mimeData());

        // Печать в консоль содержимого перетаскиваемого объекта (для отладки)
        clipboard_records->print();

        // Выясняется элемент дерева, над которым был сделан Drop
        QModelIndex index_to = indexAt(event->pos());
        // Если отпускание мышки произошло не на ветке дерева, а на свободном пустом
        // месте в области виджета дерева
        if (!index_to.isValid())
            return;
        //// Указатель на родительский элемент
        // TreeScreen *_tree_screen = qobject_cast<TreeScreen *>(parent());

        // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен
        // Drop
        auto tree_item_drop = _know_root->child(index_to);

        //// Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
        // auto tree_item_drop = tree_item_drop;    // ->record_table();

        auto check_crypt = [&](boost::intrusive_ptr<i_t> tree_item_drag) -> bool {
            bool result = true;
            //// Исходная ветка в момент Drop (откуда переностся запись) - это
            ///выделенная курсором ветка
            // QModelIndex index_from = _know_root->index(tree_item_drag);   //
            // current_index(); // selectionModel()->currentIndex();    //
            // find_object<TreeScreen>(tree_screen_singleton_name)
            ////            // Выясняется ссылка на элемент дерева (на ветку), откуда
            ///переностся запись
            ////            auto tree_item_drag = _know_root->item(index_from);
            // Если перенос происходит в ту же самую ветку
            if (tree_item_drag != tree_item_drop) {
                // Если перенос происходит из не зашифрованной ветки в зашифрованную, а
                // пароль не установлен
                if (tree_item_drag->field<crypt_key>() != crypt_value(true) && tree_item_drop->field<crypt_key>() == crypt_value(true) && gl_paras->crypt_key().length() == 0) {
                    // Выводится уведомление что невозможен перенос без пароля
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("Warning!"));
                    msgBox.setText(tr(
                        "Can\'t move this item to encrypt item. Please "
                        "open crypt item (entry password) before."));
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.exec();

                    result = false;
                }
            } else
                result = false;
            return result;
        };

        // Перенос записей, хранящихся в MimeData
        // В настоящий момент в MimeData попадает только одна запись,
        // но в дальнейшем планируется переносить несколько записей
        // и здесь код подготовлен для переноса нескольких записей

        int worked = 0;
        for (int i = 0; i < clipboard_records->size(); i++) {
            // Полные данные записи
            boost::intrusive_ptr<i_t> tree_item_drag = clipboard_records->record(i);

            auto item_on_tree = _know_root->child([&](boost::intrusive_ptr<const i_t> it) {
                return it->id() == tree_item_drag->id();
            });
            assert(item_on_tree);
            if (item_on_tree)
                tree_item_drag = item_on_tree;
            auto rctrl_ = tree_item_drag->page()->record_ctrl(); // find_object<RecordController>("table_screen_controller");
                                                                 // // Указатель на контроллер таблицы
                                                                 // конечных записей
            if (rctrl_) {
                auto web_view = tree_item_drag->page()->view();
                if (web_view) {
                    web::TabWidget* _tab_widget = web_view->tabmanager();
                    if (_tab_widget) {
                        if (check_crypt(tree_item_drag)) {
                            // Удаление записи из исходной ветки, удаление должно быть вначале,
                            // чтобы сохранился ID записи
                            // В этот момент вид таблицы конечных записей показывает таблицу, из
                            // которой совершается Drag
                            // TreeItem
                            // *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
                            if (tree_item_drag->binder())
                                _tab_widget->closeTab(_tab_widget->indexOf(tree_item_drag->page()->view())); // _record_controller->remove_child(record->field("id"));
                            // Если таблица конечных записей после удаления перемещенной записи
                            // стала пустой
                            if (rctrl_->row_count() == 0) {
// find_object<MetaEditor>(meta_editor_singleton_name)
#ifdef USE_FILE_PER_TREEITEM
                                _record_controller->blogger()->clear_all(); // Нужно очистить
                                                                            // поле
                                                                            // редактирования
                                                                            // чтобы не видно
                                                                            // было текста
                                                                            // последней
                                                                            // удаленной записи
#else
                                rctrl_->blogger()->close();
#endif // USE_FILE_PER_TREEITEM
                            }
                            // find_object<RecordScreen>(table_screen_singleton_name)
                            rctrl_->record_screen()->tools_update();

                            // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                            // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
                            // TreeIndex([&] (){return _know_root;}, tree_item_drop, 0); }
                            // catch(std::exception &e) {throw e; } return tree_index; } ();
                            // Добавление записи в базу
                            TreeLevel::instance(TreeIndex::item_require_treeindex([&]() { return _know_root; }, tree_item_drop),
                                tree_item_drag)
                                ->move(); // _know_root// tree_item_drop->child_insert(0, record,
                                          // ADD_NEW_RECORD_TO_END);

                            auto index_from = _know_root->index(tree_item_drag);
                            // Обновление исходной ветки чтобы было видно что записей убавилось
                            _know_root->update_index(index_from);

                            update(static_cast<QModelIndex>(index_from).parent());

                            // Сохранение дерева веток
                            // find_object<TreeScreen>(tree_screen_singleton_name)
                            worked++;
                        }
                    }
                }
            }
        }
        if (worked > 0) {
            // static_cast<TreeScreen *>(this->parent())->tree_view()->
            know_model_save();

            // Обновлении конечной ветки чтобы было видно что записей прибавилось
            _know_root->update_index(index_tree(index_to));

            update(static_cast<QModelIndex>(index_to));

            // В модели данных обнуляется элемент, который подсвечивался при Drag And
            // Drop
            _know_root->setData(QModelIndex(), QVariant(false), Qt::UserRole);

            synchronized(false);
        }
    }
}

boost::intrusive_ptr<i_t> tv_t::session_root_auto()
{
    auto _current_model = [&]() { return _know_root; };
    auto find_root = [&]() -> boost::intrusive_ptr<i_t> {
        auto index_list = selectionModel()->selectedIndexes();

        QList<boost::intrusive_ptr<i_t>> items;
        QList<QStringList> items_path_list;
        if (index_list.size() == 0) {
            boost::intrusive_ptr<i_t> it;
            while (_know_root->root_item()->count_direct() == 0)
                cursor_focus();
            if (_know_root->root_item()->count_direct() == 0)
                it = tree_empty_controll();
            else
                it = _know_root->root_item()->child_direct(0);
            items << it;
        } else
            for (auto& _i : index_list)
                items << _know_root->child(_i);
        assert(items.size() > 0);
        boost::intrusive_ptr<i_t> final_common_parent(nullptr);
        for (auto it : items)
            items_path_list << it->path_list<id_key>();
        auto find_shortest_list = [&]() -> QStringList {
            QStringList result = items_path_list.at(0);
            for (auto& list : items_path_list)
                if (list.size() < result.size())
                    result = list;
            return result;
        };

        auto final_common_path_list = find_shortest_list();
        assert(final_common_path_list.size() > 0);
        final_common_parent = _know_root->child(final_common_path_list);
        assert(final_common_parent);
        for (auto it : items) {
            if (final_common_parent != it &&
                !TreeIndex::is_ancestor_of(final_common_parent, it)) {
                if (TreeIndex::is_ancestor_of(it, final_common_parent))
                    final_common_parent = it;
                else {
                    auto near_root = final_common_parent->path_list<id_key>().length() >
                            it->path_list<id_key>().length() ?
                        it :
                        final_common_parent;
                    while (near_root->parent()) {
                        final_common_parent = near_root = near_root->parent();
                        if (TreeIndex::is_ancestor_of(near_root, it) &&
                            TreeIndex::is_ancestor_of(near_root, final_common_parent))
                            break;
                    }
                }
            }
        }
        assert(final_common_parent);

        return final_common_parent;
    };

    // KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model; //
    // std::bind(_source_model_func, _tree_screen->tree_view());

    auto view_session_id_defined = [&]() {
        return _current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
            return t->id() == _current_model()->session_id();
        });
    };

    auto board_session_id_defined = [&]() {
        return _know_model_board->session_id() != "" &&
            _know_model_board->child([=](boost::intrusive_ptr<const i_t> t) {
                return t->id() == _know_model_board->session_id();
            });
    };

    // (void)board_session_id_defined;
    auto fix_root = [&] {
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
        // TreeIndex(_current_model, find_root()); } catch(std::exception &e) {throw
        // e; } return tree_index; } ();
        _current_model()->session_id(
            TreeIndex::item_require_treeindex(_current_model, find_root()));
        know_model_save();
        assert(view_session_id_defined());
    };
    if (!board_session_id_defined())
        fix_root(); // if manual setted, cancel this
    if (!view_session_id_defined())
        fix_root(); // while(!view_session_id_defined() &&
                    // _current_model()->root_item() !=
                    // know_model_board()->root_item()) {
    // auto new_root = this->cursor_follow_up_one_level();

    // assert(new_root->count_direct() != 0);

    // _current_model()->session_id(TreeIndex(_current_model, find_root()));
    // }

    assert(_current_model()->session_id() != "");
    assert(view_session_id_defined());

    return view_session_id_defined();
}

// QString TreeScreen::session_root() {return _session_id;}
void tv_t::session_root_manual(bool checked)
{
    Q_UNUSED(checked);
    auto _source_model = [&]() { return source_model(); };
    // boost::intrusive_ptr<TreeIndex> tree_index = [&]
    // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
    // TreeIndex(_source_model, _source_model()->item(current_index())); }
    // catch(std::exception &e) {throw e; } return tree_index; } ();
    auto it = _source_model()->item(current_index());
    _source_model()->session_id(TreeIndex::item_require_treeindex(_source_model, it));
}

boost::intrusive_ptr<i_t> tv_t::current_item()
{
    boost::intrusive_ptr<i_t> item_result(nullptr);
    auto index_result = current_index();
    if (index_result.isValid())
        item_result =
            _know_root->child(index_result); // selectionModel()->currentIndex()
    else {
        // if(_previous_index.isValid()){
        // item_result = _know_root->child(_previous_index);
        // if(item_result)index_result =
        // select_as_current(TreeIndex::instance([&](){return _know_root;},
        // item_result));
        // }else{
        // auto	vtab_record_	= globalparameters.main_window()->vtab_record();
        // auto	view_		=
        // vtab_record_->activated_browser()->tabmanager()->currentWebView();
        // if(view_){
        // item_result = view_->page()->host();
        // if(item_result)index_result =
        // select_as_current(TreeIndex::instance([&](){return _know_root;},
        // item_result));
        // }
        // }
        if (!item_result && selectionModel()->selectedIndexes().size() > 0) {
            item_result =
                _know_root->child(selectionModel()->selectedIndexes().last());
            // boost::intrusive_ptr<TreeIndex> tree_index;
            // try {tree_index = new TreeIndex([&] {return _know_root; }, r->parent(),
            // r->linker()->sibling_order()); } catch(std::exception &e) {}

            // if(tree_index)
            index_result = select_as_current(TreeIndex::item_require_treeindex([&] { return _know_root; }, item_result));
        }
        if (!index_result.isValid()) {
            item_result = session_root_auto();
            assert(item_result);
            if (item_result == _know_model_board->root_item()) { // !result.isValid()
                // it = _know_root->item(result);
                // } else {
                assert(item_result == _know_root->root_item());
                if (_know_root->root_item()->count_direct() == 0) {
                    ////
                    ///selection_to_pos(_know_root->root_item()->item_direct(0));
                    /////_know_root->root_item()->count_direct() - 1
                    ////        } else {
                    ////                TreeScreen *_tree_screen = qobject_cast<TreeScreen
                    ///*>(parent());
                    ////                auto view = _tree_screen->tree_view();

                    // boost::intrusive_ptr<TreeIndex> tree_root_index;

                    // try {
                    // tree_root_index = new TreeIndex([&] () -> KnowModel* {return
                    // _know_root;}, _know_root->root_item(), 0);
                    // } catch(std::exception &e) {throw e; }

                    item_result = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(web::Browser::_defaulthome,
                        [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                            return TreeIndex::item_require_treeindex(
                                [&]() -> tkm_t* { return _know_root; }, new i_t(item_result))
                                ->url_bind_browser(
                                    real_target_url_, //real_url_t<url_value>::instance(web::Browser::_defaulthome),
                                    std::bind(&tv_t::move, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                                    [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                                        return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(it_->field<url_key>(), web::Browser::_defaulthome);
                                    },
                                    browser_ref());
                        });
                    // new_item = _know_root->root_item()->item_direct(0); //
                    // _know_root->root_item()->count_direct() - 1
                }
                // else if(_know_root->root_item()->parent()) {
                ////            auto parent_tree_screen_pointer =
                ///globalparameters.tree_screen();
                // do {
                // if(!_know_root->root_item()->parent())break;

                //// _tree_screen->tree_view()->reset();
                //// KnowModel*(KnowView::*source_model_)() = &KnowView::source_model;
                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex([&]()->KnowModel* {return source_model();},
                // _know_root->root_item()->parent());} catch(std::exception &e) {throw
                // e;} return tree_index;}();
                // intercept(tree_index);    // std::bind(source_model_, this)

                // } while(_know_root->root_item()->parent() &&
                // _know_root->root_item()->parent()->count_direct() == 0);

                ////
                ///_tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
                // }

                item_result = _know_root->root_item()->child_direct(0);
            }
            // else {
            // auto entrance = globalparameters.entrance();
            // auto browser = entrance->activated_browser();

            ////        if(!browser) {
            ////            browser = entrance->new_browser();
            ////        }

            // if(browser->tabmanager()->count() > 0) {
            // it = browser->tabmanager()->currentWebView()->page()->binder()->item();
            // } else {
            // RecordModel::ModelIndex record_index([&] {return
            // browser->record_screen()->record_ctrl()->source_model();},
            // nullptr, it);
            // it = browser->item_bind(record_index);
            // }

            // result = select_as_current(TreeIndex([&] {return _know_root;},
            // it->parent(),
            // it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il)
            // {return il == it->linker() && il->host() == it && it->parent() ==
            // il->host_parent();})));

            ////        if(selectionModel()->selectedIndexes().size() > 1) {
            ////            result = selectionModel()->selectedIndexes().first();
            ////        }
            // }

            // boost::intrusive_ptr<TreeIndex> tree_index = [&]
            // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
            // TreeIndex([&] {return _know_root; }, it->parent(),
            // it->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
            // {
            // return il == it->linker() && il->host() == it && it->parent() ==
            // il->host_parent();
            // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            index_result = select_as_current(TreeIndex::item_require_treeindex([&] { return _know_root; }, item_result));
            // result = selectionModel()->currentIndex();

            assert(index_result.isValid()); // this line is to be recovery
        }
    }
    assert(index_result.isValid());
    assert(item_result);

    return item_result;
}

// QModelIndex KnowView::current_index(void)const
// {
// QModelIndex result = selectionModel()->currentIndex();

// if(!result.isValid()) {
// result = QModelIndex();
// QMessageBox msgBox;
// msgBox.setWindowTitle(tr("Warning!"));
// msgBox.setText(tr("Invalid index, please select an item from tree screen."));
// msgBox.setIcon(QMessageBox::Information);
// msgBox.exec();

// }

// return result;
// }

// Get the index of the current element on which the cursor is positioned   //
// Получение индекса текущего элемента на котором стоит курсор
QModelIndex tv_t::current_index(void) const
{
    QModelIndex result = selectionModel()->currentIndex();
    // if(! result.isValid()){
    // boost::intrusive_ptr<TreeItem>		item_result(nullptr);
    // auto					vtab_record_	=
    // globalparameters.main_window()->vtab_record();
    // auto					view_		=
    // vtab_record_->activated_browser()->tabmanager()->currentWebView();
    // if(view_){
    // item_result = view_->page()->host();
    // if(item_result) result = _know_root->index(item_result);
    // // select_as_current(TreeIndex::instance([&](){return _know_root;},
    // item_result));
    // }
    // }
    //// if(!selectionModel()->currentIndex().isValid()) {

    //// boost::intrusive_ptr<TreeItem> _item = nullptr;
    //// TreeKnowModel *current_model = static_cast<TreeKnowModel *>(model());

    //// if(0 == current_model->root_item()->current_count()) {
    //// return_to_root();   //
    //// current_model = static_cast<TreeKnowModel *>(model());
    //// _item = current_model->root_item()->add_child();
    //// } else {
    //// _item = current_model->root_item()->child(0);  //
    ///_know_branch->root_item()->current_count() - 1

    //// }

    //// assert(_item);

    //// QModelIndex cur_index;

    //////        reset();
    //////        setModel(current_model);

    //// if(_item) {
    //// cur_index = current_model->index(_item);    //
    ///item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent())
    /////
    ///_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count()
    ///- 1)

    //// assert(cur_index.isValid());
    //// assert(current_model->item(cur_index) == _item);
    //////            candidate_from_knowtree_item(cur_index);

    //// assert(model());

    //// selectionModel()->setCurrentIndex(cur_index // ,
    ///current_tree_selection_mode    //
    //// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //// );
    //// selectionModel()->select(cur_index  //, current_tree_selection_mode
    //// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
    //// );
    //////            clicked(cur_index);
    //// }

    //////        else {
    //////            //            cur_index = _knowtreemodel->index_child(
    //////            //
    ///_knowtreemodel->index(_knowtreemodel->_root_item)   // does not work!
    //////            //                            ,
    ///_knowtreemodel->_root_item->child_count() - 1);

    //////            cur_index = _knowtreemodel->index(
    //////                            _knowtreemodel->_root_item->child_count() -
    ///1, 0, _knowtreemodel->index(item->parent()) //
    ///_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count()
    ///- 1)
    //////                        );

    //////            assert(cur_index.isValid());
    //////            _knowtreeview->selectionModel()->setCurrentIndex(
    //////                cur_index
    //////                //                _knowtreemodel->index_child(
    //////                // _knowtreemodel->index(_knowtreemodel->_root_item)
    //////                //                    ,
    ///_knowtreemodel->_root_item->child_count() - 1)

    //////                //                _knowtreemodel->index(
    //////                //                    _knowtreemodel->_root_item->child
    //////                //                    (
    //////                // _knowtreemodel->_root_item->child_count() - 1
    //////                //                    )
    //////                //                )
    //////                //            _knowtreemodel->createIndex(0, 0,
    ///static_cast<void *>(_knowtreemodel->_root_item.get())),
    ///QItemSelectionModel::ClearAndSelect);
    //////                //
    ///_knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item),
    ///0)
    //////                , current_tree_selection_mode
    //////            );
    //////        }
    //// }

    //// auto v = _treeknowview->selectionModel()->currentIndex();
    //// candidate_from_knowtree_item(cur_index);

    //// if(!result.isValid()) {
    //// boost::intrusive_ptr<TreeItem> it = session_root_auto();
    //// assert(it);

    //// if(!it || it == _know_model_board->root_item()) { // !result.isValid()

    //////            it = _know_root->item(result);
    //////        } else {

    //// if(_know_root->root_item()->count_direct() == 0) {
    //////            selection_to_pos(_know_root->root_item()->item_direct(0));
    /////_know_root->root_item()->count_direct() - 1
    //////        } else {
    //////                TreeScreen *_tree_screen = qobject_cast<TreeScreen
    ///*>(parent());
    //////                auto view = _tree_screen->tree_view();

    //// if(_know_root->root_item()->parent()) {
    //////            auto parent_tree_screen_pointer =
    ///globalparameters.tree_screen();
    //// do {
    //// if(!_know_root->root_item()->parent())break;

    ////// _tree_screen->tree_view()->reset();
    ////// KnowModel*(KnowView::*source_model_)() = &KnowView::source_model;
    //// boost::intrusive_ptr<TreeIndex> tree_index = [&]
    ///{boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
    ///TreeIndex([&]()->KnowModel* {return source_model();},
    ///_know_root->root_item()->parent());} catch(std::exception &e) {throw e;}
    ///return tree_index;}();
    //// intercept(tree_index);    // std::bind(source_model_, this)

    //// } while(_know_root->root_item()->parent() &&
    ///_know_root->root_item()->parent()->count_direct() == 0);

    //////
    ///_tree_screen->tree_view()->source_model(_know_root->root_item()->parent());
    //// } else if(_know_root->root_item()->count_direct() == 0) {
    //// boost::intrusive_ptr<TreeIndex> tree_root_index;

    //// try {
    //// tree_root_index = new TreeIndex([&]()->KnowModel* {return _know_root;},
    ///_know_root->root_item(), 0);
    //// } catch(std::exception &e) {throw e;}

    //// item_bind(
    //// nullptr
    //// , QUrl(web::Browser::_defaulthome)
    //// , std::bind(&KnowView::view_paste_child, this, tree_root_index,
    ///std::placeholders::_2, std::placeholders::_3)
    //// );
    //// }

    //////            new_item = _know_root->root_item()->item_direct(0); //
    ///_know_root->root_item()->count_direct() - 1
    //// }

    //////            else {
    //// it = _know_root->root_item()->item_direct(0);
    //////            }

    //// }

    //////        else {
    //////            auto entrance = globalparameters.entrance();
    //////            auto browser = entrance->activated_browser();

    //////            //        if(!browser) {
    //////            //            browser = entrance->new_browser();
    //////            //        }

    //////            if(browser->tabmanager()->count() > 0) {
    //////                it =
    ///browser->tabmanager()->currentWebView()->page()->binder()->item();
    //////            } else {
    //////                RecordModel::ModelIndex record_index([&] {return
    ///browser->record_screen()->record_ctrl()->source_model();}, nullptr,
    ///it);
    //////                it = browser->item_bind(record_index);
    //////            }

    //////            result = select_as_current(TreeIndex([&] {return
    ///_know_root;}, it->parent(),
    ///it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il)
    ///{return il == it->linker() && il->host() == it && it->parent() ==
    ///il->host_parent();})));

    //////            //        if(selectionModel()->selectedIndexes().size() > 1)
    ///{
    //////            //            result =
    ///selectionModel()->selectedIndexes().first();
    //////            //        }
    //////        }
    //// boost::intrusive_ptr<TreeIndex> tree_index = [&]
    ///{boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
    ///TreeIndex([&] {return _know_root;}, it->parent(),
    ///it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il)
    ///{return il == it->linker() && il->host() == it && it->parent() ==
    ///il->host_parent();}));} catch(std::exception &e) {throw e;} return
    ///tree_index;}();
    //// result = select_as_current(tree_index);
    //////        result = selectionModel()->currentIndex();

    //// assert(result.isValid());    // this line is to be recovery
    //// }

    //// assert(cur_index.isValid());
    return result; // cur_index;   // temporary setting???   //
                   // selectionModel()->currentIndex();
}

//// Получение индекса текущего элемента на котором стоит курсор
// QModelIndex KnowView::view_index_last(void) const {
////    if(!selectionModel()->currentIndex().isValid()) {
// boost::intrusive_ptr<TreeItem> _item = nullptr;
////    KnowModel *_know_root = _know_root;  //source_model();
// if(0 == _know_root->root_item()->current_count()){
// static_cast<TreeScreen *>(parent())->view_return_to_root();	//
////        _know_root = source_model();
// _item = _know_root->root_item()->add_child();
// }else _item =
// _know_root->root_item()->child(_know_root->root_item()->current_count() - 1);
// assert(_item);
// QModelIndex cur_index = _know_root->index(_item);	//
// item->parent()->child_count() - 1, 0, _knowtreemodel->index(item->parent())
// //
// _knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count() -
// 1)

// assert(cur_index.isValid());
// assert(_know_root->item(cur_index) == _item);
////        candidate_from_knowtree_item(cur_index);

////    if(!_knowtreeview->selectionModel()->currentIndex().isValid()) {
// selectionModel()->setCurrentIndex(
// cur_index
////
///_knowtreemodel->index(_knowtreemodel->_root_item->child(_knowtreemodel->_root_item->child_count()
///- 1))
////            _knowtreemodel->createIndex(0, 0, static_cast<void
///*>(_knowtreemodel->_root_item.get())), QItemSelectionModel::ClearAndSelect);
////
///_knowtreemodel->indexChildren(_knowtreemodel->index_from_item(_knowtreemodel->_root_item),
///0)
//// , current_tree_selection_mode
// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
// );
////    }
// selectionModel()->select(cur_index
//// , current_tree_selection_mode
// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
// );
////        clicked(cur_index);
////    candidate_from_knowtree_item(cur_index);

////    }

////    assert(selectionModel()->currentIndex().isValid());    // this line is
///to be recovery
// return selectionModel()->currentIndex();
// }

// QModelIndex KnowView::select_as_current_item(boost::intrusive_ptr<TreeItem>
// _item
// , select_strategy _select_strategy
// , current_strategy _current_strategy){
// auto select_as_current_index = [&](const QModelIndex &_index
// , select_strategy _select_strategy = [](KnowView *const v, const QModelIndex
// &_i) -> QModelIndex
// {assert(v);assert(v->selectionModel());assert(_i.isValid());v->selectionModel()->select(_i,
// current_tree_selection_mode);return _i;}
// , current_strategy _current_strategy = [](KnowView *const v, const
// QModelIndex &_i, const int dummy = 0) -> QModelIndex {(void)
// (dummy);assert(v);assert(v->selectionModel());assert(_i.isValid());v->selectionModel()->setCurrentIndex(_i,
// current_tree_current_index_mode);return _i;}
// ) -> QModelIndex {
// QModelIndex _result;
////    int pos = index.row();

////    // todo: Если это условие ни разу не сработает, значит преобразование
///ipos - pos надо просто убрать
////    if(pos != to_pos) {
////        QMessageBox msgBox;
////        msgBox.setText("In RecordView::setSelectionToPos() input pos not
///equal model pos");
////        msgBox.exec();
////    }

////    int rowCount = _know_root->root_item()->count_direct();

////    if(pos < rowCount) {  // pos > (rowCount - 1)   // return;
// _result	= _select_strategy(this, _index);
// _result	= _current_strategy(this, _result, 0);

////    //    // Простой механизм выбора строки. Похоже, что его использовать не
///получится
////    //    selectionModel()->select(_result
////    //                             , (_result == _index) ?
///QItemSelectionModel::SelectionFlag::Select :
///QItemSelectionModel::SelectionFlag::Deselect
////    //                            ); // QItemSelectionModel::SelectCurrent
///// current_tree_selection_mode
////    // , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

////    //    auto recordSourceModel = controller->getRecordTableModel();
////    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

////    // Установка засветки на нужный индекс
////    // Set the backlight to the desired index
////    selectionModel()->setCurrentIndex(_result,
///current_tree_current_index_mode); // QItemSelectionModel::SelectCurrent    //
///ClearAndSelect   // current_tree_selection_mode

////    //    assert(_result == selectionModel()->currentIndex());

// #if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
//// В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал
///смены строки в модели выбора)
//// Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась
///таблица конечных записей
//// In response to the mobile version of the record is no choice (not processed
///signal line change to the selection model)
//// Therefore, the recording must be made a virtual click to fill the final
///table of records
// if(appconfig->interface_mode() == "mobile") emit this->clicked(_result);
// // QModelIndex selIdx = recordSourceModel->index(pos, 0);

// #endif

//// emit this->clicked(index);
////    _result = currentIndex();

// scrollTo(_result);	// QAbstractItemView::PositionAtCenter

////    this->setFocus();   // ?

////    }
// return _result;
// };

// TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());	//
// globalparameters.tree_screen();
// //find_object<TreeScreen>(tree_screen_singleton_name);
// assert(_tree_screen);
//// auto _tree_screen = globalparameters.tree_screen();

// QModelIndex _index = _know_root->index(_item);	// =
// _tree_screen->tree_view()->source_model()->index(_item);
// while(  ! _index.isValid()
// && _know_root->root_item() != _know_model_board->root_item()
// ){
// cursor_follow_up_one_level();
// _index = _know_root->index(_item);
////                _index_item =
///_tree_screen->tree_view()->source_model()->index(_item);
// }
// _item->binder() ? [&] {auto c =
// _item->binder()->page()->record_ctrl();auto v =
// _item->binder()->page()->record_ctrl()->view();c->update_record_view(_item);return
// c->cursor_to_index(_item);} () : void (0);
////    QModelIndex _index_current;
////    try {
////        _index_current = selectionModel()->currentIndex();
////    } catch(std::exception &e) {qDebug() << e.what();}
// if(_index.isValid() && _index != selectionModel()->currentIndex()){
// // _index_current //
//// _tree_screen->tree_view()->selectionModel()->setCurrentIndex(_index,
///QItemSelectionModel::SelectionFlag::SelectCurrent);
// _index = select_as_current_index(_index, _select_strategy,
// _current_strategy);	// [](KnowView * v, const QModelIndex & _i)
// ->QModelIndex{v->selectionModel()->select(_i, current_tree_selection_mode);
// return _i;}
// }
////    QModelIndex _index = _know_root->index(_item);
/////_record_controller->pos_to_proxyindex(to_pos); // Модельный индекс в Proxy
///модели

////    _index = select_and_current(_index, _strategy);

// return _index;
// }

// QModelIndex KnowView::deselect(const QModelIndex &_index){
// QModelIndex _result;

// selectionModel()->select(_index,
// QItemSelectionModel::SelectionFlag::Deselect);
// _result = selectionModel()->selectedIndexes().last();
// selectionModel()->setCurrentIndex(_result,
// QItemSelectionModel::SelectionFlag::Current);
// assert(_result == currentIndex());
//// В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал
///смены строки в модели выбора)
//// Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась
///таблица конечных записей
//// In response to the mobile version of the record is no choice (not processed
///signal line change to the selection model)
//// Therefore, the recording must be made a virtual click to fill the final
///table of records
// if(appconfig->getInterfaceMode() == "mobile") emit this->clicked(_result);
// // QModelIndex selIdx=recordSourceModel->index(pos, 0);

//// emit this->clicked(index);
////    _result = currentIndex();

// scrollTo(_result);	// QAbstractItemView::PositionAtCenter

// this->setFocus();	// ?
// return _result;
// }

// QModelIndex KnowView::deselect(boost::intrusive_ptr<TreeItem> _item){
// QModelIndex _index = _know_root->index(_item);
// _index = deselect(_index);

// return _index;
// }

// QModelIndex KnowView::selection_to_pos(int _index){
////    bool result = false;
// QModelIndex index;
// if(_index != - 1){
// auto item = _know_root->root_item()->item_direct(_index);
// index = selection_to_pos(item);
////        result = true;
// }
// return index;
// }

QModelIndex tv_t::select_as_current(boost::intrusive_ptr<TreeIndex> _tree_index, select_strategy _select_strategy, current_strategy _current_strategy)
{
    QModelIndex _result;
    // QModelIndex _current_index = _modelindex.parent_index();
    // auto _current_model = _modelindex->current_model();
    QModelIndex _host_index = _tree_index->host_index(); // index(_modelindex->host());
    // QModelIndex _host_parent_index = _modelindex->host_parent_index();

    // if(_host_parent_index.isValid()) {

    auto _item = _tree_index->host();

    // TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
    // // globalparameters.tree_screen();
    // //find_object<TreeScreen>(tree_screen_singleton_name);
    assert(_tree_screen);

    // auto _item = _know_root->item(_parent_index);

    // auto v =
    // globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const
    // ::Binder> b){return url_equal(b->host()->field<home_type>().toStdString(),
    // _item->field<home_type>().toStdString());});
    // if(v){
    // auto	ctrl			= v->tabmanager()->record_ctrl();
    // auto	alternative_item	= v->page()->host();
    // if(ctrl)
    RecordIndex::select_as_current(_item); // alternative_item
    // }
    //
    // _item->binder() ? _item->binder()->page() ?
    // _item->binder()->page()->record_ctrl() ? [&] {
    // auto record_controller = _item->binder()->page()->record_ctrl();
    //// if(_record_controller->view()->selection_first<IdType>() !=
    ///_item->field("id")) {
    ////
    ///_record_controller->cursor_to_index(_record_controller->index<PosProxy>(_item));
    ///// IdType(it->field("id"))
    //// }
    // return record_controller->synchronize_record_view(_item);
    //// return c->cursor_to_index(c->index<PosProxy>(_item));
    // } () : nullptr : nullptr : nullptr;
    if (_host_index.isValid() &&
        _host_index != selectionModel()->currentIndex()) { // _index_current //
        // int pos = index.row();

        //// todo: Если это условие ни разу не сработает, значит преобразование ipos
        ///- pos надо просто убрать
        // if(pos != to_pos) {
        // QMessageBox msgBox;
        // msgBox.setText("In RecordView::setSelectionToPos() input pos not equal
        // model pos");
        // msgBox.exec();
        // }

        // int rowCount = _know_root->root_item()->count_direct();

        // if(pos < rowCount) {  // pos > (rowCount - 1)   // return;
        _result = _select_strategy(this, _host_index);
        _result = _current_strategy(this, _host_index, 0);
        assert(_host_index == _result);

        ////    // Простой механизм выбора строки. Похоже, что его использовать не
        ///получится
        ////    selectionModel()->select(_result
        ////                             , (_result == _index) ?
        ///QItemSelectionModel::SelectionFlag::Select :
        ///QItemSelectionModel::SelectionFlag::Deselect
        ////                            ); // QItemSelectionModel::SelectCurrent //
        ///current_tree_selection_mode
        //// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

        ////    auto recordSourceModel = controller->getRecordTableModel();
        ////    QModelIndex selIdx = recordSourceModel->index(pos, 0);

        //// Установка засветки на нужный индекс
        //// Set the backlight to the desired index
        // selectionModel()->setCurrentIndex(_result,
        // current_tree_current_index_mode); // QItemSelectionModel::SelectCurrent
        // // ClearAndSelect   // current_tree_selection_mode

        ////    assert(_result == selectionModel()->currentIndex());

        // _result = currentIndex();

        scrollTo(_host_index); // QAbstractItemView::PositionAtCenter

// this->setFocus();   // ?

// }
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
        // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал
        // смены строки в модели выбора)
        // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась
        // таблица конечных записей
        // In response to the mobile version of the record is no choice (not
        // processed signal line change to the selection model)
        // Therefore, the recording must be made a virtual click to fill the final
        // table of records
        if (appconfig->interface_mode() == "mobile")
            emit this->clicked(_host_index); // QModelIndex selIdx =
// recordSourceModel->index(pos, 0);

#endif

        // emit this->clicked(index);
    } else if (!_host_index.isValid())
        _result = selectionModel()->currentIndex();
    else
        _result = _host_index;
    // assert(_aim_to_be_current_index == selectionModel()->currentIndex());
    assert(_result == selectionModel()->currentIndex());
    // }else{
    // qDebug() << "index to root item";
    // throw std::exception();
    // }
    //	if (_item) {
    //		// auto vr = globalparameters.main_window()->vtab_record();
    //		auto v = gl_paras->main_window()->find([&](boost::intrusive_ptr<const Binder> b) { return b->host() == _item; });
    //		if (v) {
    //			auto p = v->page();
    //			if (p) {
    //				if (p->tabmanager()->current_item() != _item) p->tabmanager()->select_as_current(v);
    //				auto _rctrl = p->record_ctrl();
    //				if (_rctrl) {
    //#ifdef USE_HAS_SELECTION
    //					auto _record_view = _rctrl->view();
    //					QItemSelectionModel* item_selection_model = _record_view->selectionModel();
    //					bool has_selection = item_selection_model->hasSelection();
    //#endif // USE_HAS_SELECTION
    //					if (_rctrl->view()->current_item() != _item
    //#ifdef USE_HAS_SELECTION
    //					    || !has_selection
    //#endif // USE_HAS_SELECTION
    //					    )
    //						_rctrl->select_as_current(_rctrl->index<pos_proxy>(_rctrl->source_model()->index(_item)));
    //					//
    //				}
    //			}
    //		}
    //	}
    return _result;
}

web::WebView* tv_t::index_invoke(boost::intrusive_ptr<TreeIndex> _tree_index)
{ // web::WebView *view, const index_tree &_index
    auto _index = _tree_index->host_index();
    auto _host = _tree_index->host();
    auto _page = _host->page();
    web::WebView* v = nullptr;

    //
    //	_host->activate(std::bind(&wn_t::find, gl_paras->main_window(),
    //std::placeholders::_1));
    //	v = _page->view();

    web::Browser* browser_ = nullptr;
    if (_page) {
        browser_ = _page->browser();
    }

    if (!browser_) browser_ = gl_paras->main_window()->browser(_host, true); //_host
    web::WebView* activated_view = browser_->currentTab();                   // globalparameters.main_window()->vtab_record()->find([&](boost::intrusive_ptr<const
                                                                             // ::Binder> b) -> bool {return
                                                                             // url_equal((b->host()->field<home_type>()).toStdString(),
                                                                             // _host->field<home_type>().toStdString()) ||
                                                                             // url_equal((b->host()->field<url_type>()).toStdString(),
                                                                             // _host->field<url_type>().toStdString());});;
    // const index_tree &_index = _know_root->index(_item);
    if (static_cast<QModelIndex>(_index)
            .isValid()) { // && index_current() != _index
        gl_paras->main_window()->setDisabled(true);
        // KnowModel *(TreeScreen::*_source_model_func)() =
        // &TreeScreen::know_model_board;
        // auto _source_model = std::bind(_source_model_func, this);

        // KnowModel *(KnowView::*_source_model_func)() = &KnowView::source_model;
        auto _source_model = [&]() {
            return source_model();
        }; // std::bind(_source_model_func, _tree_view);

        // auto tree_screen = dynamic_cast<TreeScreen *>(static_cast<QObject
        // *>(this)->parent());
        // auto name = tree_screen->objectName();
        // assert(tree_screen->objectName() == tree_screen_singleton_name);

        // auto _tree_screen = globalparameters.tree_screen();
        assert(_tree_screen->objectName() == tree_screen_singleton_name);
        // Получаем указатель на текущую выбранную ветку дерева
        auto result_item = _host; // source_model()->child(_index);

        auto merge_by_url = [&]() {
            auto duplicated_items_list_on_board_by_url =
                _source_model()->children([&](boost::intrusive_ptr<const i_t> it) {
                    return url_equal(detail::to_string(it->field<home_key>()), detail::to_string(result_item->field<home_key>())) && !url_equal(url_value(detail::to_qstring(it->field<home_key>())), web::Browser::_defaulthome);
                }); // source_model()
            if (duplicated_items_list_on_board_by_url.size() > 1) {
                for (auto _i : duplicated_items_list_on_board_by_url) {
                    auto duplicated_item = _i;
                    if (duplicated_item != result_item)
                        result_item = TreeLevel::instance(TreeIndex::item_require_treeindex(_source_model, result_item),
                            duplicated_item)
                                          ->merge(); // (duplicated_item->id() !=
                                                     // result_item->id()) ||
                                                     // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                                                     // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                                                     // TreeIndex(_source_model, result_item); } catch(std::exception &e)
                                                     // {throw e; } return tree_index; } ();
                }
            }
        };

        (void)merge_by_url;

        // auto duplicated_item_index_list_on_view =
        // _source_model()->indexes([&](boost::intrusive_ptr<TreeItem> it) {return
        // it->field("url") == result_item->field("url");});
        auto duplicated_items_list_on_board_by_id =
            _source_model()->children([&](boost::intrusive_ptr<const i_t> it) {
                return it->id() == result_item->id();
            });
        if (duplicated_items_list_on_board_by_id.size() > 1) {
            for (auto _i : duplicated_items_list_on_board_by_id) {
                auto duplicated_item = _i;
                if (duplicated_item != result_item)
                    result_item =
                        TreeLevel::instance(
                            TreeIndex::item_require_treeindex(_source_model, result_item),
                            duplicated_item)
                            ->merge(); // (duplicated_item->id() != result_item->id()) ||
                                       // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                                       // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                                       // TreeIndex(_source_model, result_item); } catch(std::exception &e)
                                       // {throw e; } return tree_index; } ();
            }
        }
        if (!gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) { // !result_item->is_activated()
                bool result = false;
                // auto result_item = source_model()->item(_index);
                if (result_item->binder()) {
                    if (url_equal(detail::to_string(b->host()->field<home_key>()), detail::to_string(result_item->field<home_key>())))
                        if (result_item->binder()->page())
                            if (result_item->binder()->page()->view()->load_finished())
                                result = true;
                }
                return result;
            })) {
            // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

            // Save the text in the edit box in the corresponding file// Сохраняется
            // текст в окне редактирования в соответсвующий файл
            // find_object<MainWindow>("mainwindow")
            auto rs = dynamic_cast<rs_t*>(gl_paras->vtab_record()->currentWidget());
            if (rs)
                rs->blogger()->save_text_context();
            //
            setExpanded(static_cast<QModelIndex>(_index).parent(), true); // (isExpanded(_index)) ? setExpanded(_index, false) :
                                                                          // setExpanded(_index, true);
            if (result_item->count_direct() > 0) {                        // && (result_item->field("url") !=
                                                                          // "" || result_item->field("url")
                                                                          // == "" )
                // Вначале все инструменты работы с веткой включаются
                QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
                while (i.hasNext()) {
                    i.next();
                    i.value()->setEnabled(true);
                }
                // Проверяется, происходит ли клик по зашифрованной ветке
                if (result_item->field<crypt_key>() == crypt_value(true)) {
                    // Если пароль доступа к зашифрованным данным не вводился в этой
                    // сессии
                    if (gl_paras->crypt_key().length() == 0) {
                        // Запрашивается пароль
                        Password password;
                        if (password.retrievePassword() == false) {
                            //// Устанавливаем пустые данные для отображения таблицы конечных
                            ///записей
                            //// find_object<RecordController>("table_screen_controller")

                            ////
                            ///result_item->unique_page()->view()->record_ctrl()->source_model()->clear();

                            // setup_model(boost::intrusive_ptr<TreeItem>(new
                            // TreeItem(nullptr, QMap<QString, QString>())));   //
                            // _know_branch->clear();  //
                            // root_item(boost::intrusive_ptr<TreeItem>(new
                            // TreeItem(QMap<QString, QString>(), nullptr)));

                            ////                    init_source_model(
                            ////                        nullptr
                            ////                        ,
                            ///result_item->unique_page()->view()->record_ctrl()
                            ////                        ,
                            ///result_item->unique_page()->view()->record_ctrl()->record_screen()
                            ////                        , globalparameters.mainwindow()
                            ////                        , globalparameters.meta_editor()
                            ////                        // , globalparameters.record_screens()
                            ////                    );

                            ////                        reset_tabledata(nullptr);

                            //// Все инструменты работы с веткой отключаются
                            QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
                            while (i.hasNext()) {
                                i.next();
                                i.value()->setEnabled(false);
                            }
                            return v; // Программа дальше не идет, доделать...
                        }
                    }
                }
                // if(source_model()->root_item() != result_item)
                // setup_model(result_item); // _know_branch->intercept(result_item);
                // //? never do this!!!
                ////        _shadow_candidate_model->_root_item->field("id",
                ///result_item->field("id"));
                ////        _shadow_candidate_model->_root_item->field("name",
                ///result_item->field("name"));
                //// Получаем указатель на данные таблицы конечных записей
                ////    std::shared_ptr<TableData> record_tabledata = item->tabledata();
                ////    std::shared_ptr<QDomDocument> doc =
                ///std::make_shared<QDomDocument>();
                //// setup_model(_know_branch);
                // enable_up_action(); // _know_branch->root_item()->id() !=
                // static_cast<TreeKnowModel *>(model())->root_item()->id()   //
                // _know_branch->root_item()->id() != know_root()->root_item()->id()
                ////    for(int i = 0; i < item->tabledata()->size(); i++) {
                ////
                ///_shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
                ////            _shadow_candidate_model->_root_item->tabledata()->size()
                ////            , item->tabledata()->record(i)
                ////        );
                ////    }
                ////    std::shared_ptr<TableData> record_tabledata = item->tabledata();
                ////    std::shared_ptr<QDomDocument> doc =
                ///std::make_shared<QDomDocument>();
                ////    _shadow_candidate_model->_root_item = result_item; //
                ///->record_table(item->record_table()->export_to_dom());
                ////        // Устанавливаем данные таблицы конечных записей
                ////
                ///find_object<RecordController>(table_controller_singleton_name)->source_model()->tree_item(_shadow_candidate_model->_root_item);
                ////        // need lock?
                ////
                ///reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item,
                ///item->tabledata()->export_to_dom(doc)));
                // Set the text path to the final table entries for the mobile interface
                // options    // Устанавливается текстовый путь в таблице конечных
                // записей для мобильного варианта интерфейса
                if (appconfig->interface_mode() == "mobile") {
                    QStringList path = result_item->path_list<name_key>();

                    // Remove the empty element, if it exists (this can be the root, it
                    // has no name)    // Убирается пустой элемент, если он есть (это
                    // может быть корень, у него нет названия)
                    int emptyStringIndex = path.indexOf("");
                    path.removeAt(emptyStringIndex);

                    // find_object<RecordScreen>(table_screen_singleton_name)
                    browser_->record_screen()->tree_path(path.join(" > "));
                }
                // Ширина колонки дерева устанавливается так чтоб всегда вмещались
                // данные
                resizeColumnToContents(0);

                // if(result_item->field("url") != "")
                // globalparameters.entrance()->activate(result_item);
            }
            // else {      //        if(result_item->field("url") !=
            // web::Browser::_defaulthome)
            ////        if(result_item->field("url") != "")
            ///globalparameters.entrance()->activate(result_item);
            //// Переключаются окна (используется для мобильного интерфейса)
            //// globalparameters.window_switcher()->switchFromTreeToRecordtable();
            // }
            if (result_item->field<home_key>() == home_value("") && result_item->field<url_key>() != url_value(""))
                result_item->field<home_key>(home_value(detail::to_qstring(result_item->field<url_key>())));
            if (result_item->field<home_key>() != home_value("")) {
                // TreeIndex modelindex([&] {return _know_root;},
                // current_item()->parent(),
                // current_item()->parent()->sibling_order(current_item()));
                ////            select_and_current(result_item);
                // auto item_bind_ = [&]() {return item_bind(current_item(),
                // result_item, std::bind(&KnowView::view_paste_child, this, modelindex,
                // std::placeholders::_2, std::placeholders::_3))->activate();};
                //				auto browser_nearest =
                //gl_paras->main_window()->activated_browser();
                // auto previous_item = _know_root->item(_previous_index);
                boost::intrusive_ptr<i_t> record_previous_item(nullptr);
                if (activated_view)
                    record_previous_item = activated_view->page()->host();
                // record_previous_item =
                // browser->record_screen()->record_ctrl()->source_model()->item(pos_source(browser->record_screen()->record_ctrl()->view()->previous_index().row()));
                if (record_previous_item != result_item) {
                    boost::intrusive_ptr<RecordIndex> record_index =
                        RecordIndex::instance(
                            [&] {
                                RecordModel* rm = nullptr;
                                auto rctrl = browser_->tab_widget()->record_screen()->record_ctrl();
                                if (rctrl) rm = rctrl->source_model();
                                return rm;
                            },
                            result_item); // record_previous_item,

                    auto browser_bind_activate =
                        [&](boost::intrusive_ptr<RecordIndex> _record_index)
                        -> web::WebView* {
                        return browser_->bind(_record_index)
                            ->activate(
                                std::bind(&wn_t::find, gl_paras->main_window(),
                                    std::placeholders::_1)); // item_bind_();
                    };
                    // if(record_index && ! result_item->binder())
                    v = browser_bind_activate(
                        record_index); // browser->item_bind(record_index)->activate();
                                       // // item_bind_();
                                       // else if(record_index && result_item->binder() && !
                                       // result_item->binder()->page())
                                       // // !result_item->binder()->integrity_internal()){
                                       // v = browser_bind_activate(record_index);
                                       // // browser->item_bind(record_index)->activate();   // item_bind_();
                                       // else v = result_item->activate(std::bind(&HidableTabWidget::find,
                                       // globalparameters.main_window()->vtab_record(),
                                       // std::placeholders::_1));
                } else
                    v = activated_view;
            }
        } else {
            // auto c = _binder->page()->record_ctrl();
            v = result_item->page() ? result_item->page()->view() : nullptr;
            if (v) {
                auto tabmanager = v->tabmanager();
                auto index = tabmanager->webViewIndex(v);
                if (index != tabmanager->currentIndex())
                    tabmanager->setCurrentIndex(
                        index); // c->index<PosSource>(c->source_model()->index(_binder->item()))
            }
        }
        if (v)
            v->current_view_global_consistency();
        gl_paras->main_window()->setEnabled(true);
    }
    return v;
}

void tv_t::update_selected_indexes(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectitems = selectionModel()->selectedIndexes();
    // Обновление на экране
    for (int i = 0; i < selectitems.size(); ++i)
        update(selectitems.at(i));
}

tv_t::LocalLizeInitializer::LocalLizeInitializer(tv_t* _tree_view)
    : _tree_view(_tree_view)
{
    // , _index_list(_index_list)
    // , _index(_index)
    // assert(_index == _tree_view->current_index());
    if (-1 == _tree_view->selectionModel()->selectedIndexes().indexOf(_tree_view->current_index()))
        throw std::runtime_error(formatter() << "! locallize invalid index");
    assert(_tree_view->selectionModel()->selectedIndexes().indexOf(_tree_view->current_index()) != -1);
}

QItemSelection tv_t::LocalLizeInitializer::operator()()
{
    std::function<bool(const QModelIndex&, const QModelIndex&)>
        find_in_ancestor = [](const QModelIndex& _i, const QModelIndex& _p) {
            bool result = false;
            QModelIndex _i_copy = _i;
            while (_i_copy.parent()
                       .isValid()) { // && _i_copy.parent().column() >= _p.column()?
                _i_copy = _i_copy.parent();
                if (_i_copy == _p) {
                    result = true;
                    break;
                }
            }
            return result;
        };

    auto is_index_localized =
        [&](const QModelIndexList _origin_index_list) -> bool {
        bool result = true;
        if (_origin_index_list.size() > 0) {
            // int deepest_column = 0;

            // for(int i = 0; i < _origin_index_list.size(); ++i) {
            // QModelIndex index = _origin_index_list.at(i);

            // if(index.column() > deepest_column)deepest_column = index.column();
            // }

            ////        assert(_origin_index_list.last() == current_index()); // means
            ///duplicated item found
            // preparations
            auto _index_current =
                _tree_view->current_index(); // _origin_index_list.last();    // //
            if (_index_current.isValid()) {
                auto _index_common_parent = _index_current.parent();
                for (int i = 0; i < _origin_index_list.size(); ++i) {
                    QModelIndex index = _origin_index_list.at(i);
                    if (index.isValid() &&
                        (index.parent() != _index_common_parent ||
                            !find_in_ancestor(index,
                                _index_common_parent))) { // || index.column()
                                                          // != deepest_column
                        result = false;
                        break;
                    }
                }
            }
        }
        return result;
    };

    auto index_localize =
        [&](const QModelIndexList& _origin_index_list) -> QModelIndexList {
        QModelIndexList _selectitems;
        int deepest_column = 0;
        if (_origin_index_list.size() > 0) {
            // auto _source_model = source_model();
            // На время удаления блокируется главное окно
            ////    find_object<MainWindow>("mainwindow")
            // globalparameters.mainwindow()->setDisabled(true);
            ////    find_object<MainWindow>("mainwindow")
            // globalparameters.mainwindow()->blockSignals(true);
            // assert(_origin_index_list.first() == current_index());
            if (!_origin_index_list.contains(_tree_view->current_index())) { // ?
                // KnowModel *(KnowView::*_source_model_func)() =
                // &KnowView::source_model;
                auto _source_model = [&]() {
                    return _tree_view->_know_root;
                }; // std::bind(_source_model_func, _tree_view);

                // QModelIndex _current_index;
                boost::intrusive_ptr<i_t> duplicated_item = _tree_view->current_item();
                boost::intrusive_ptr<i_t> found;
                for (int i = 0; i < _origin_index_list.size(); ++i) {
                    auto _index = _origin_index_list.at(i);
                    if (_index.column() > deepest_column)
                        deepest_column = _index.column();
                    if (duplicated_item->id() == _source_model()->item(_index)->id()) {
                        // _current_index = _index;
                        found = _source_model()->item(_index);
                        break;
                    }
                }
                // TreeScreen *_tree_screen = static_cast<TreeScreen *>(this->parent());
                if (found && duplicated_item && found != duplicated_item) {
                    found =
                        static_cast<tkm_t*>(_source_model())
                            ->merge(
                                TreeLevel::instance(
                                    TreeIndex::item_require_treeindex(_source_model, found),
                                    duplicated_item),
                                std::bind(&tv_t::delete_permanent, _tree_view, std::placeholders::_1, std::placeholders::_2, &tkm_t::delete_permanent, "cut",
                                    false) // , std::bind(&tv_t::delete_permanent,
                                // _tree_view, std::placeholders::_1,
                                // std::placeholders::_2,
                                // &tkm_t::delete_permanent_recursive,
                                // "cut", false)
                                );
                }
            }
            std::set<QModelIndex> indexes;
            for (auto idx : _origin_index_list)
                if (indexes.find(idx) == indexes.end())
                    indexes.insert(idx);
            QModelIndexList new_list;
            for (auto i : indexes)
                new_list << i;
            // preparations
            auto _index_current =
                _tree_view->current_index(); // _origin_index_list.first();
            if (_index_current.isValid()) {
                auto _index_common_parent = _index_current.parent();
                // auto index_to_be_delete_last = _index_list.last();
                // auto _item_to_be_deleted_first =
                // _source_model->item(index_to_be_delete_first);
                // auto deleted_position_first =
                // _item_to_be_deleted_first->sibling_order(); //
                // items_update_selected();
                // auto deleted_position_last =
                // _source_model->item(index_to_be_delete_last)->sibling_order();
                // auto _item_parent = _item_to_be_deleted_first->parent();
                //// Список имен веток, которые нужно удалить
                // QStringList branches_name;
                for (int i = 0; i < new_list.size(); ++i) {
                    QModelIndex index = new_list.at(i);
                    if (index.isValid() &&
                        (index.parent() == _index_common_parent ||
                            find_in_ancestor(index,
                                _index_common_parent))) { // && index.column()
                                                          // == deepest_column
                        // auto it = _source_model->item(index);
                        // branches_name << it->field("name");
                        _selectitems.push_back(index);
                    }
                }
            }
        }
        return _selectitems;
    };

    auto remove_multi_columns =
        [&](const QModelIndexList _origin_index_list) -> QModelIndexList {
        QModelIndexList _index_list;
        std::set<int> row_list;
        for (auto idx : _origin_index_list)
            if (row_list.find(idx.row()) == row_list.end()) {
                row_list.insert(idx.row());
                if (idx.column() == 0)
                    _index_list << idx;
            }
        return _index_list;
    };

    auto _index_list = _tree_view->selectionModel()->selectedIndexes();
    // Получение списка индексов QModelIndex выделенных элементов
    auto _selectitems = is_index_localized(_index_list) ? remove_multi_columns(_index_list) : remove_multi_columns(index_localize(_index_list));

    qSort(_selectitems.begin(), _selectitems.end());

    return QItemSelection(_selectitems.first(), _selectitems.last());
}

// QModelIndexList KnowView::index_localize(const QModelIndexList
// _origin_index_list)
// {
// QModelIndexList _selectitems;
// int deepest_column = 0;

// if(_origin_index_list.size() > 0) {
////        auto _source_model = source_model();
//// На время удаления блокируется главное окно
////        //    find_object<MainWindow>("mainwindow")
////        globalparameters.mainwindow()->setDisabled(true);
////        //    find_object<MainWindow>("mainwindow")
////        globalparameters.mainwindow()->blockSignals(true);

////        assert(_origin_index_list.first() == current_index());

// if(!_origin_index_list.contains(current_index())) {

////            KnowModel *(KnowView::*_source_model_func)() =
///&KnowView::source_model;
// auto _source_model = [&]() {return _know_root;}; //
// std::bind(_source_model_func, _tree_view);

////            QModelIndex _current_index;
// boost::intrusive_ptr<TreeItem> duplicated_item = current_item();
// boost::intrusive_ptr<TreeItem> found;

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// auto _index = _origin_index_list.at(i);

// if(_index.column() > deepest_column)deepest_column = _index.column();

// if(duplicated_item->id() == _source_model()->item(_index)->id()) {
////                    _current_index = _index;
// found = _source_model()->item(_index);
// break;
// }
// }

////            TreeScreen *_tree_screen = static_cast<TreeScreen
///*>(this->parent());

// if(found && duplicated_item && found != duplicated_item) {
// found = static_cast<KnowModel *>(_source_model())->model_merge_to_left(
// std::bind(&KnowView::view_delete_permanent, this, std::placeholders::_1,
// std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
// , found, duplicated_item
// );
// }
// }

////preparations
// auto _index_current         = current_index(); //_origin_index_list.first();
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

// if(index.isValid() && index.parent() == _index_common_parent &&
// index.column() == deepest_column) {
////                auto it = _source_model->item(index);
////                branches_name << it->field("name");
// _selectitems.push_back(index);
// }
// }
// }

// return _selectitems;
// }

// bool KnowView::is_index_localized(const QModelIndexList
// _origin_index_list)const
// {
// bool result = true;

// if(_origin_index_list.size() > 0) {
// int deepest_column = 0;

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// QModelIndex index = _origin_index_list.at(i);

// if(index.column() > deepest_column)deepest_column = index.column();
// }

////        assert(_origin_index_list.last() == current_index()); // means
///duplicated item found
////preparations
// auto _index_current = _origin_index_list.last();    // current_index();    //

// auto _index_common_parent = _index_current.parent();

// for(int i = 0; i < _origin_index_list.size(); ++i) {
// QModelIndex index = _origin_index_list.at(i);

// if(index.isValid() && (index.parent() != _index_common_parent ||
// index.column() != deepest_column)) {
// result = false; break;
// }
// }
// }

// return result;
// }

tkm_t* tv_t::know_model_board() const
{
    return _know_model_board;
}

// KnowModel *TreeScreen::know_model_board() {return _know_model_board;}

// Шифрование ветки
void tv_t::encrypt(void)
{
    auto item_encrypt = [&](void) -> void {
        auto _index = current_index();
        if (_index.isValid()) {
            // auto _know_root = source_model();

            // Получаем указатель на текущую выбранную ветку дерева
            auto item = _know_root->child(_index);

            // Шифрация ветки и всех подветок
            item->to_encrypt();
            //			gl_paras->main_window()->setDisabled(true);
            // Сохранение дерева веток
            // find_object<TreeScreen>(tree_screen_singleton_name)->
            // sychronize();
            know_model_save(); //std::thread(&tkm_t::save, _know_model_board).join(); //.hardware_concurrency(); //.detach(); // __know_model_board->save();
            //			gl_paras->main_window()->setEnabled(true);
            // Обновляеются на экране ветка и ее подветки
            _know_root->update_index(index_tree(_index));
        }
    };
    // Если пароль в данной сессии не вводился
    if (gl_paras->crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;
        if (password.retrievePassword() == true) // Если пароль введен правильно
            item_encrypt();                      // Ветка шифруется
    } else {
        // Иначе считается, что шифрующий ключ уже был задан и он правильный

        item_encrypt(); // Ветка шифруется
    }
}

// асшифровка ветки
void tv_t::decrypt(void)
{
    auto item_decrypt = [&](void) -> void {
        auto _index = current_index();
        if (_index.isValid()) {
            // auto _know_root = source_model();
            // Получаем указатель на текущую выбранную ветку дерева
            auto item = _know_root->child(_index);

            // асшифровка ветки и всех подветок
            item->to_decrypt();
            //			gl_paras->main_window()->setDisabled(true);
            // Сохранение дерева веток
            // find_object<TreeScreen>(tree_screen_singleton_name)->
            // sychronize();
            know_model_save(); //std::thread(&tkm_t::save, _know_model_board).join(); //.hardware_concurrency(); //.detach(); // __know_model_board->save();
            //			gl_paras->main_window()->setEnabled(true);
            // Обновляеются на экране ветка и ее подветки
            _know_root->update_index(index_tree(_index));

            // Проверяется, остались ли в дереве зашифрованные данные
            // если зашифрованных данных нет, будет предложено сбросить пароль
            // static_cast<TreeScreen *>(parent())->
            tree_crypt_control();
        }
    };
    // Если пароль в данной сессии не вводился
    if (gl_paras->crypt_key().size() == 0) {
        // Запрашивается пароль
        Password password;
        if (password.retrievePassword() == true) // Если пароль введен правильно
            item_decrypt();                      // Ветка расшифровывается
    } else {
        // Иначе пароль в данной сессии вводился и он правильный

        item_decrypt(); // Ветка расшифровывается
    }
}

void tv_t::expand_selected(bool expand)
{
    std::function<void(QModelIndex index, bool mode)> expand_or_collapse_recurse =
        [&](QModelIndex index, bool mode) {
            setExpanded(index, mode);

            int i = 0;
            while ((index.child(i, 0)).isValid()) {
                expand_or_collapse_recurse(index.child(i, 0), mode);
                i++;
            }
        };

    // Получение индексов выделенных строк
    QModelIndexList selectitems = selectionModel()->selectedIndexes();
    for (int i = 0; i < selectitems.size(); ++i)
        expand_or_collapse_recurse(selectitems.at(i), expand);
}

// template<>
boost::intrusive_ptr<i_t> tv_t::new_child(boost::intrusive_ptr<TreeIndex> _modelindex, // std::function<KnowModel *()> _current_model, QModelIndex _current_index
    QString _name, const add_new& _branch_add_new_impl)
{
    auto _current_model = _modelindex->current_model();
    // auto _current_index = _modelindex.current_index();
    boost::intrusive_ptr<i_t> result(nullptr);
    // Получение ссылки на узел, который соответствует выделенной строке
    // auto item = _current_model->item(_current_index);

    // find_object<MainWindow>("mainwindow")
    gl_paras->main_window()->setDisabled(true);

    auto current_item = _modelindex->host(); // _current_model()->item(_current_index);
    auto host_parent = current_item->parent();
    QList<boost::intrusive_ptr<i_t>> _alternative_items;
    if (host_parent) {
        _alternative_items = host_parent->children_direct([&](boost::intrusive_ptr<const Linker> il) { return il->host()->name() == _name; });
    }
    // auto _name_same_no_child_first = [&] {boost::intrusive_ptr<TreeItem>
    // result; for(auto i : _items_with_same_name) {if(i->count_direct() == 0)
    // {result = i; break;}} return result;}();
    ////    QModelIndex setto;
    if (_alternative_items.size() > 0) {
        // for(int i = 1; i < _items_with_same_name.size(); i++) {
        // result = view_merge_to_left(_current_model, _items_with_same_name[0],
        // _items_with_same_name[i]);
        // }
        QMutableListIterator<boost::intrusive_ptr<i_t>> it(_alternative_items);
        result = it.next();
        while (it.hasNext()) {
            auto _it = it.next();
            if (_it->id() == result->id()) { // prevent error merge
                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
                // TreeIndex(_current_model, result); } catch(std::exception &e) {throw
                // e; } return tree_index; } ();
                result = TreeLevel::instance(TreeIndex::item_require_treeindex(_current_model, result), _it)->merge(); // TreeIndex::instance(_current_model, result,
                                                                                                                       // result->parent()), _it);
            }
        }
        // result = _name_same_no_child_first ? _name_same_no_child_first :
        // _items_with_same_name[0];
    } else {
        // Получение уникального идентификатора
        QString id = detail::to_qstring(get_unical_id());

        // Вставка данных и установка курсора

        // Одноранговая ветка

        // Подветка

        // Вставка новых данных в модель дерева записей
        result = _branch_add_new_impl(_modelindex, id, _name); // _current_model->lock_child_add(_current_index, id, _name);
    }
    // Установка курсора на только что созданную позицию
    ////        QModelIndex
    ////    setto = _current_model()->index(result);
    /////index_child(_current_index, item->current_count() - 1);
    // boost::intrusive_ptr<TreeIndex> tree_index = [&]
    // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
    // TreeIndex([&] {return _know_root; }, result->parent(),
    // result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
    // {
    // return il == result->linker() && il->host() == result && result->parent()
    // == il->host_parent();
    // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
    select_as_current(TreeIndex::item_require_treeindex([&] { return _know_root; }, result));
    // selectionModel()->select(setto, current_tree_selection_mode);
    // selectionModel()->setCurrentIndex(setto, current_tree_current_index_mode);
    // // ClearAndSelect

    // А можно было установить курсор на нужную позицию и так
    // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()
    // - 1, 0, index), QItemSelectionModel::ClearAndSelect);

    _know_model_board->synchronized(false);
    // Сохранение дерева веток
    // find_object<TreeScreen>(tree_screen_singleton_name)->
    know_model_save(); //std::thread(&tkm_t::save, _know_model_board).join(); //.hardware_concurrency(); //.detach(); // __know_model_board->save();  // know_model_save();

    // find_object<MainWindow>("mainwindow")
    gl_paras->main_window()
        ->setEnabled(true);
    assert(result->name() == _name);

    // assert(result == _current_model()->item(setto));
    return result; // _current_model->item(setto);
}

// Метод, следящий, не обнулилось ли дерево
// Если дерево стало пустым, данный метод добавит одну ветку
// Method that monitors are not reset to zero if the tree was empty
// If the tree was empty, the method will add one branch
boost::intrusive_ptr<i_t> tv_t::tree_empty_controll(void)
{
    qDebug() << "treescreen::tree_empty_control() : Tree branch count "
             << _know_model_board->rowCount();
    boost::intrusive_ptr<i_t> result(nullptr);
    if (_know_model_board->rowCount() == 0) {
        qDebug()
            << "treescreen::tree_empty_control() : Tree empty, create blank branch";
        auto it = _know_model_board->root_item();

        _know_root->intercept(it);
        reset();
        setModel(_know_root);
        cursor_focus(it);
        // new_child(TreeIndex::instance([&]() -> km_t * {return source_model();},
        // new TreeItem(it), it), tr("Rename me")																				//
        // TreeIndex([&]()->KnowModel* { return source_model();},
        // source_model()->root_item())
        // , std::bind(&km_t::new_child, _know_root, std::placeholders::_1,
        // std::placeholders::_2, std::placeholders::_3)						//
        // std::placeholders::_1    // std::placeholders::_3

        // );
        result = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(web::Browser::_defaulthome,
            [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
                return TreeIndex::item_require_treeindex([&]() -> tkm_t* { return _know_root; }, new i_t(it))
                    ->url_bind_browser(real_target_url_, std::bind(&tv_t::move, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const i_t> it_) -> bool {
                        return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(it_->field<url_key>(), web::Browser::_defaulthome);
                    },
                        browser_ref());
            });
    } else
        result = _know_model_board->root_item()->child_direct(0);
    assert(result);

    return result;
}

// Метод, следящий, не стало ли дерево содержать только незашифрованные записи
// Если в дереве нет шифрования, задается вопрос, нужно ли сбросить пароль
void tv_t::tree_crypt_control(void)
{
    // Если в дереве нет шифрования
    if (_know_model_board->is_contains_crypt_branches() == false) {
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
boost::intrusive_ptr<i_t> tv_t::new_item(
    boost::intrusive_ptr<TreeIndex> _modelindex // std::function<KnowModel *()>
                                                // _current_model, QModelIndex
                                                // _current_index
    ,
    const tv_t::add_new_delegate& _branch_add_new,
    const tv_t::add_new& _branch_add_new_impl
    // , const std::function < boost::intrusive_ptr<TreeItem> (TreeIndex,
    // QString
    // , const std::function < boost::intrusive_ptr<TreeItem>(TreeIndex,
    // QString, QString) >&) > &_branch_add_new
    // , const std::function < boost::intrusive_ptr<TreeItem> (TreeIndex,
    // QString, QString) > &_branch_add_new_impl
    )
{ // bool insert_sibling_branch
    auto _current_model = _modelindex->current_model();
    auto _target_index = _modelindex->host_index();
    boost::intrusive_ptr<i_t> result(nullptr);
    //// Получение списка индексов QModelIndex выделенных элементов
    // QModelIndexList selectitems = selectionModel()->selectedIndexes();
    ////    QModelIndex _current_index;
    //// Если выбрано более одной ветки
    // if(selectitems.size() > 1) {
    ////        QMessageBox messageBox(this);
    ////        messageBox.setWindowTitle(tr("Unavailable action"));
    ////        messageBox.setText(tr("You've selected ") +
    ///QString::number(selectitems.size()) + tr(" items.\nPlease select single
    ///item for enabling insert operation."));
    ////        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    ////        messageBox.exec();
    ////        //        current_index = selectitems.first();
    ////        return nullptr;
    // _current_index = selectitems.last();
    // } else {
    ////    else if(selectitems.size() == 0) {
    ////        current_index = last_index();
    ////    } else {
    // _current_index = current_index();
    // }
    if (static_cast<QModelIndex>(_target_index).isValid()) {
        // }

        // if(!_current_index.isValid()) {
        // selectionModel()->setCurrentIndex(_current_model->index(0, 0,
        // QModelIndex()), QItemSelectionModel::ClearAndSelect);
        // assert(_current_index.isValid());
        // }

        // Создается окно ввода данных
        bool ok;
        QString title = tr("Create new item"), text = tr("Item name:");

        // if(insert_sibling_branch) {
        // title = tr("Create new item");
        // text = tr("Item name:");
        // } else {
        // title = tr("Create new sub item");
        // text = tr("Sub item name:");
        // }

        QString _name =
            QInputDialog::getText(this, title, text, QLineEdit::Normal, "", &ok);
        if (ok &&
            !_name.isEmpty()) { // return nullptr;    // Если была нажата отмена
            auto target_item =
                _modelindex->host(); // _current_model()->item(_current_index);
            auto target_parent = target_item->parent();
            QList<boost::intrusive_ptr<i_t>> _alternative_items;
            if (target_parent)
                _alternative_items = target_parent->children_direct(
                    [&](boost::intrusive_ptr<const Linker> il) {
                        return il->host()->name() == _name;
                    });
            // auto _name_same_no_child_first = [&] {boost::intrusive_ptr<TreeItem>
            // result; for(auto i : _items_with_same_name) {if(i->count_direct() == 0)
            // {result = i; break;}} return result;}();
            if (_alternative_items.size() > 0) {
                // result = _name_same_no_child_first ? _name_same_no_child_first :
                // _items_with_same_name[0];

                QMutableListIterator<boost::intrusive_ptr<i_t>> it(_alternative_items);
                result = it.next();
                while (it.hasNext()) {
                    auto _it = it.next();
                    if (_it->id() == result->id()) { // prevent error merge
                        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                        // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index =
                        // new TreeIndex(_current_model, result); } catch(std::exception &e)
                        // {throw e; } return tree_index; } ();
                        result =
                            TreeLevel::instance(
                                TreeIndex::item_require_treeindex(_current_model, result), _it)
                                ->merge();
                    }
                }
            } else {
                // Получение индекса выделенной строки
                // QModelIndex
                // current_index = currentitem_index();

                // KnowModel *(KnowView::*_source_model_func)() =
                // &KnowView::source_model;
                // auto _source_model = [&]() {return source_model();}; //
                // std::bind(_source_model_func, _tree_view);
                result = _branch_add_new(
                    _modelindex, _name // source_model()->item(_current_index)->name()
                    ,
                    _branch_add_new_impl); // nonsense
            }
            // boost::intrusive_ptr<TreeIndex> tree_index = [&]
            // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
            // TreeIndex([&] {return _know_root; }, result->parent(),
            // result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker>
            // il)
            // {
            // return il == result->linker() && il->host() == result &&
            // result->parent() == il->host_parent();
            // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
            select_as_current(
                TreeIndex::item_require_treeindex([&] { return _know_root; }, result));
            // , insert_sibling_branch
        }
    }
    // Введенные данные добавляются
    return result;
}

// move children from children
QList<boost::intrusive_ptr<i_t>> tv_t::move_children(
    boost::intrusive_ptr<TreeIndex>
        _parent_modelindex // std::function<KnowModel *()> _current_model,
                           // QModelIndex _current_index
    ,
    boost::intrusive_ptr<i_t> _blank_header,
    const tv_t::substitute_condition_double& _substitute_condition)
{
    auto _current_model = _parent_modelindex->current_model();
    auto host_index = _parent_modelindex->host_index();
    // auto					host_parent	=
    // _parent_modelindex->host()->parent();
    auto host = _parent_modelindex->host();
    QList<boost::intrusive_ptr<i_t>> result;
    // std::vector<boost::intrusive_ptr<TreeItem>> results;

    auto view =
        static_cast<tv_t*>(static_cast<QObject*>(_current_model())->parent());
    assert(view);
    if (static_cast<QModelIndex>(_current_model()->index(_blank_header))
            .isValid() &&
        _current_model()->index(_blank_header) != _current_model()->index(host) &&
        _substitute_condition(_blank_header->linker(), host->linker()))
        TreeLevel::instance(_parent_modelindex, _blank_header)
            ->merge(); // && _know_root->index(_blank_header).isValid()   //
                       // _blank_header->field("name") != clipboard_items_root
    // TreeIndex(_current_model, _current_parent)
    // ? should this function ignore the _blank_header?
    if (_blank_header->count_direct() > 0) {
        // for(int i = 0; i < it->current_count(); i++) {
        // auto current_item = it->child(i);
        // if(_current_model->is_item_id_exists(it->id())) {
        // return add_branch(_current_model->index(it), it, insert_sibling_branch,
        // _current_model);
        // auto _current_index = index_current();
        // //selectionModel()->currentIndex(); //_current_model->index(it);
        if (static_cast<QModelIndex>(host_index).isValid()) {
            // Получение ссылки на узел, который соответствует выделенной строке
            // auto current_item = _current_model->item(_current_index);

            // if(!_current_index.isValid()) {
            // selectionModel()->setCurrentIndex(_current_model->index(0, 0,
            // QModelIndex()), QItemSelectionModel::ClearAndSelect);
            // assert(_current_index.isValid());
            // }

            // find_object<MainWindow>("mainwindow")
            gl_paras->main_window()->setDisabled(true);

            // if(new_branch_root->id() == "") {
            //// Получение уникального идентификатора
            // QString id = get_unical_id();
            // new_branch_root->field("id", id);
            // }

            QModelIndex setto;
            int current_count = 0;
            ////            auto begin_item = _blank_header->item_direct(0);

            // int reserved = 0;
            ////            _current_model->beginInsertRows(_current_index.parent()
            ////                                            , _current_index.row()
            ///// parent->count_direct()
            ////                                            , _current_index.row()
            ///// parent->count_direct()
            ////                                           );

            //// Вставка данных и установка курсора
            // while(( // current_count =
            // _blank_header->count_direct()) > 0)
            _blank_header->traverse_direct([&](boost::intrusive_ptr<Linker> il) { // != reserved
                // for(int index = 0; index < _blank_root->direct_count(); index++) {
                auto candidate = il->host(); // _blank_header->item_direct(0);
                if (candidate) {
                    // {// time consuming
                    // auto found_item =
                    // _current_model()->item([=](boost::intrusive_ptr<const TreeItem> t)
                    // -> bool {return t->name() == candidate->name() && t->id() ==
                    // candidate->id();});
                    //// Подветка
                    // if(found_item)found_item =
                    // TreeLevel::instance(TreeIndex::instance(_current_model,
                    // found_item), candidate)->merge();	// candidate->id()
                    //// boost::intrusive_ptr<TreeIndex> tree_index = [&]
                    ///{boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
                    ///TreeIndex(_current_model, found_item); } catch(std::exception &e)
                    ///{throw e; } return tree_index; } ();
                    //// reserved++;
                    // else found_item = candidate;
                    // auto r = move(_parent_modelindex, found_item,
                    // std::bind(_substitute_condition, std::placeholders::_1,
                    // found_item->linker()));
                    // }
                    auto r = move(_parent_modelindex, candidate, std::bind(_substitute_condition, std::placeholders::_1,
                                                                     candidate->linker())); //

                    // assert(_know_model_board->is_item_exists(candidate->id()));

                    result << r;
                    current_count++;

                    //// Установка курсора на только что созданную позицию
                    ////        QModelIndex
                    // setto = _current_model->index_child(_current_index,
                    // current_item->current_count() - 1);
                    // selectionModel()->setCurrentIndex(setto,
                    // QItemSelectionModel::ClearAndSelect);

                    //// А можно было установить курсор на нужную позицию и так
                    ////
                    ///knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                    //// QItemSelectionModel::ClearAndSelect);
                }
            });
            // if(current_count > 0)result = host_parent;
            // // _current_model()->item(_current_parent_index);    // _blank_header;
            if (result.size() >
                0) { // && _blank_header->field("name") != clipboard_items_root
                // _current_model->endInsertRows();

                // Установка курсора на только что созданную позицию
                // QModelIndex
                setto = _current_model()->index(
                    std::bind(_substitute_condition, std::placeholders::_1,
                        result[0]->linker())); // [&](boost::intrusive_ptr<const
                                               // TreeItem> it)->bool{return
                                               // it->id() == begin_item->id();}
                // TreeModel::delegater(begin_item->id())

                // index_child(_current_index.parent(), _current_index.row() + 1);   //
                // current_item->current_count() - 1

                assert(setto.isValid());

                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try {tree_index = new
                // TreeIndex([&] {return _know_root; }, result->parent(),
                // result->parent()->sibling_order([&] (boost::intrusive_ptr<const
                // Linker> il)
                // {
                // return il == result->linker() && il->host() == result &&
                // result->parent() == il->host_parent();
                // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                select_as_current(TreeIndex::item_require_treeindex([&] { return _know_root; }, result[0])); // setto
                // selectionModel()->select(setto, current_tree_selection_mode);
                // selectionModel()->setCurrentIndex(setto,
                // current_tree_current_index_mode);   // ClearAndSelect

                // Сохранение дерева веток
                // find_object<TreeScreen>(tree_screen_singleton_name)->
                know_model_save();

                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setEnabled(true);
                // return
                auto result_check =
                    _current_model()->item([=](boost::intrusive_ptr<const i_t> it) {
                        return it->id() == result[0]->id();
                    }); // setto

                assert(result_check);
                // assert(results[0] == result);

                // } else if(_current_model->root_item()->id() == it->id()) {

                // }

                // }
            }
        }
    }
    return result;
}

// paste children from clipboard
void tv_t::paste_clipboard(
    boost::intrusive_ptr<TreeIndex>
        _sibling_tree_index)
{ // std::function<QString(const QModelIndex &,
    // ClipboardBranch *)>
    // _model_paste_from_clipboard
    // Добавление подветки из буфера обмена относительно указанного элемента
    // Функция возвращает новый идентификатор стартовой добавленной подветки
    auto paste_children_from_clipboard = [](
                                             boost::intrusive_ptr<i_t> _blank_header,
                                             ClipboardBranch* _sub_branch // , int _sibling_order
                                             ) -> boost::intrusive_ptr<i_t> {
        qDebug() << "In paste_subbranch()";
        boost::intrusive_ptr<i_t> _result(nullptr);
        // assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
        // _target_item->id();}));  // maybe a dangling pointer, for further
        // operations

        auto move_as_child_from_clipboard_object =
            [](boost::intrusive_ptr<i_t> _parent,
                boost::intrusive_ptr<i_t> _to_be_operated, int _pos,
                int _mode = add_new_record_after) -> boost::intrusive_ptr<i_t> {
            // auto item_from_model = item([&](boost::intrusive_ptr<const TreeItem>
            // it)->bool{return it->id() == _source_item->id();});  // clipboard
            // implementation have no model

            // if(item_from_model) {
            // _source_item = item_from_model;
            // }
            assert(_parent);
            boost::intrusive_ptr<i_t> result(nullptr); // 1-1?
            // auto _index_parent = index(_parent);
            // auto _index_origin = index(_source_item);
            // assert(!_index_origin.isValid());
            if (_parent &&
                !(_to_be_operated->parent() == _parent &&
                    _parent->contains_direct(_to_be_operated->linker()))) {
                ////                _source_item->self_remove_from_parent();    //
                // _source_item->parent()->remove(_source_item->up_linker());    // ->
                // this content is not must a logic error  1-2? // inside in 1-3
                // _source_item->parent calling

                ////        assert(!_index_parent.isValid());

                // result = _source_item->parent(_parent, _pos, _mode)->host();

                assert(
                    !_parent->path_list<id_key>().contains(_to_be_operated->field<id_key>()));

                auto _linker = _to_be_operated->linker(); // _parent->linker();
                if (!_linker) {
                    _to_be_operated->linker(boost::intrusive_ptr<Linker>(new Linker(_parent, _to_be_operated))); // , pos, mode
                    _linker = _to_be_operated->linker();
                    auto link_result = _linker->parent(_parent, _pos, _mode);
                    assert(link_result == _linker);
                    assert(_linker->integrity_external(_to_be_operated, _parent));
                } else if (_linker && _linker->host_parent() != _parent) {
                    auto parent = _linker->host_parent();
                    if (parent && parent != _parent)
                        parent->release([&](boost::intrusive_ptr<const Linker> il) {
                            return il->host()->id() == _to_be_operated->id() && il == _linker;
                        });
                    if (_linker->host() != _to_be_operated)
                        _linker->host(std::forward<boost::intrusive_ptr<i_t>>(_to_be_operated)); // std::move(boost::intrusive_ptr<TreeItem>(this))
                    auto link_result = _linker->parent(
                        _parent, _pos,
                        _mode); // _linker->host_parent()->release(this->linker());
                    assert(link_result == _linker);
                    assert(_linker->integrity_external(_to_be_operated, _parent));
                }
                // 1-3?  // *_parent << _source_item; //
                if (_parent->field<crypt_key>() == crypt_value(true) && result)
                    result->to_encrypt(); // Новая ветка превращается в зашифрованную
                                          // assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id()
                                          // == _parent->id();}));

                // if(result)
                // assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id()
                // == result->id();})); // may be parent is not exist?

                //// Подузел прикрепляется к указанному элементу
                //// в конец списка подчиненных элементов
                ////    boost::intrusive_ptr<TreeItem> current =
            }
            return result; // return model_move_as_child_impl(_parent, _source, _pos,
                           // _mode);
        };

        int record_move_times = 0;

        std::function<boost::intrusive_ptr<i_t>(
            boost::intrusive_ptr<i_t>, boost::intrusive_ptr<i_t>,
            ClipboardBranch*)> // , const int// , const QString &
            child_assembly_recursive =
                [&](boost::intrusive_ptr<i_t> _current_clip_parent_parent,
                    boost::intrusive_ptr<i_t>
                        _current_clip_parent // , const QString
                    // &_current_clip_records_parent_id
                    ,
                    ClipboardBranch* _sub_branch
                    // , const int                       _sibling_order
                    ) -> boost::intrusive_ptr<i_t> {
            // QString id = _current_clip_parent->id();
            boost::intrusive_ptr<i_t> contain_current_parent(nullptr);
            if (_current_clip_parent_parent)
                contain_current_parent = _current_clip_parent_parent->contains_direct(
                    _current_clip_parent->linker());
            else
                contain_current_parent = _current_clip_parent;
            if (contain_current_parent) {
                // if(_clip_parent->parent()) {
                qDebug() << "In child_assembly_recursive()";

                // Выясняются данные конечных записей
                QList<boost::intrusive_ptr<i_t>> records =
                    _sub_branch->records_by_parent_id(_current_clip_parent->id());
                for (int i = 0; i < records.size();
                     i++) { // foreach(boost::intrusive_ptr<TreeItem> record, records) {
                    auto record = records.at(i);
                    if (record && _current_clip_parent != record) {
                        boost::intrusive_ptr<i_t> contain_current_record(nullptr);
                        contain_current_record =
                            _current_clip_parent->contains_direct(record->linker());
                        // _new_item
                        if (!contain_current_record) {
                            auto record_shadow = move_as_child_from_clipboard_object(
                                _current_clip_parent, record, i); // _sibling_order + i
                            // _clip_parent->count_direct()
                            qDebug() << "Add table record : " + record->field<name_key>() +
                                    " to " + _current_clip_parent->name();
                            // assert(record->parent_id() == _current_clip_parent->id());
                            assert(record_shadow->id() == record->id());
                            assert(record->id() != _current_clip_parent->id());
                            record_move_times++;
                            // _clip_parent->child_move(_clip_parent->count_direct(), record,
                            // ADD_NEW_RECORD_TO_END);    // _new_item
                            // assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return
                            // it->id() == record_shadow->id();})); // dangling parent lead
                            // all do not exist in model
                            if (record_shadow) { // you can not use result->count_direct()
                                                 // because there are no children there
                                // QList< boost::intrusive_ptr<TreeItem> > record_children =
                                // _sub_branch->records_by_parent_id(record_shadow->id());

                                // for(int i = 0; i < record_children.size(); i++) {
                                // //foreach(QString current_subbranch_id, subbranch_id_list) {

                                // assert(record_children[i]->parent_id() ==
                                // record_shadow->id());

                                // assert(0 == record_shadow->count_direct());
                                child_assembly_recursive(_current_clip_parent, record_shadow,
                                    _sub_branch // , 0
                                    );

                                // }
                            }
                        }
                    }
                }
            }
            // }

            // assert(id != "");
            return _current_clip_parent; // id;
        };

        ////    // Выясняется линейная структура добавляемого дерева
        ////    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->id_tree_get();

        ////    // Идентификатор стартовой ветки лежит в первом элементе списка
        ////    QString start_branch_id = tree[0]._branch_id;

        ////    auto _clip_root_items_parent_id_list =
        ///subbranch->clip_root_items_parent_id_list();
        // auto _clip_blank_header_id = _sub_branch->clip_blank_header_id();

        ////        //    int offset = 0;
        ////        if(_clip_blank_header_id.size() > 0)
        ////        {
        ////            size_t calling_count = 0;

        // for(auto id_iterator = _clip_blank_header_id.begin(); id_iterator !=
        // _clip_blank_header_id.end(); id_iterator++) {
        _blank_header->field<id_key>(_sub_branch->clip_blank_header_id()); // "id",
        _result = child_assembly_recursive(
            nullptr, _blank_header,
            _sub_branch); // , _clip_blank_header_id // *id_iterator
        // , _sibling_order  // + offset

        ////        offset++;
        // calling_count++;
        ////            }

        // assert(calling_count == _clip_blank_header_id.size());
        assert(record_move_times <= _blank_header->count_children_recursive());
        // _result = _blank_header;
        // }

        return _result; // _target_item->id();
    };

    auto _current_model = _sibling_tree_index->current_model();
    auto host_index = _sibling_tree_index->host_index();
    boost::intrusive_ptr<i_t> _result(nullptr);
    // Проверяется, содержит ли буфер обмена данные нужного формата
    const QMimeData* mime_data = QApplication::clipboard()->mimeData();
    if (mime_data == nullptr)
        return;
    if (!(mime_data->hasFormat(QString(gl_para::_program_instance_name) + "/branch")))
        return;
    ////    // Получение списка индексов QModelIndex выделенных элементов
    ////    QModelIndexList selectitems = selectionModel()->selectedIndexes();
    // QModelIndex _current_index;
    ////    // Если выбрано более одной ветки
    ////    if(selectitems.size() > 1) {
    ////        //        QMessageBox messageBox(this);
    ////        //        messageBox.setWindowTitle(tr("Unavailable action"));
    ////        //        messageBox.setText(tr("You've selected ") +
    ///QString::number(selectitems.size()) + tr(" items.\nPlease select single
    ///item for enabling paste operation."));
    ////        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    ////        //        messageBox.exec();
    ////        //        return;
    ////        _current_index = selectitems.first();
    ////    } else {
    // _current_index = current_index();
    ////    }
    if (static_cast<QModelIndex>(host_index).isValid()) {
        // Блокируется главное окно, чтобы при продолжительном выполнении
        // небыло возможности сделать другие действия
        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->setDisabled(true);

        // Создается ссылка на буфер обмена
        QClipboard* cbuf = QApplication::clipboard();

        // Извлечение объекта из буфера обмена
        const ClipboardBranch* _sub_branch;

        _sub_branch = qobject_cast<const ClipboardBranch*>(cbuf->mimeData());
        _sub_branch->print();
        _sub_branch->id_tree_print();

        ////        KnowModel *(KnowView::*_source_model_func)() =
        ///&KnowView::source_model;
        // auto _current_model = [&]() {return source_model();}; //
        // std::bind(_source_model_func, _tree_view);

        ////        // delete items from view
        ////        //        auto _source_model = source_model();
        ////        auto _current_item = current_item();

        ////        assert(_source_model()->item([ =
        ///](boost::intrusive_ptr<TreeItem> t) {return t->id() ==
        ///_current_item->id();})); // _current_item

        ////        //        // ?
        ////        //        auto _clip_root_items_parent_id_list =
        ///_sub_branch->clip_root_items_parent_id_list();
        ////        //        QModelIndexList _index_list;
        ////        //        QList<boost::intrusive_ptr<TreeItem>>
        ///clip_board_items;

        ////        //        for(auto _id : _clip_root_items_parent_id_list) {
        ////        //            QList<boost::intrusive_ptr<TreeItem>>records =
        ///_sub_branch->records_by_parent_id(_id);

        ////        //            for(auto record : records) {
        ////        //                auto _index = _source_model()->index(record);

        ////        //                if(_index.isValid()) {
        ////        //                    _index_list.push_back(_index);
        ////        //                    clip_board_items << record;
        ////        //                }
        ////        //            }
        ////        //        }

        ////        //        if(!_index_list.isEmpty())
        ///view_delete_items(_source_model, clip_board_items, "cut");

        ////        select_and_current(_current_item);

        ////        assert(_source_model()->item([ =
        ///](boost::intrusive_ptr<TreeItem> t) {return t->id() ==
        ///_current_item->id();}));

        full_fields_map data;

        // QDateTime	ctime_dt	= QDateTime::currentDateTime();
        QString ctime_ = get_qtime();
        boost::fusion::at_key<id_key>(data) = get_unical_id();
        boost::fusion::at_key<name_key>(data) = clipboard_items_root;
        boost::fusion::at_key<ctime_key>(data) = ctime_;
        boost::fusion::at_key<dir_key>(data) =
            boost::fusion::at_key<id_key>(data);
        boost::fusion::at_key<file_key>(data) = "text.html";

        boost::intrusive_ptr<i_t> blank_header(
            i_t::dangle_instance(data)); // new TreeItem(nullptr, data)
        blank_header = paste_children_from_clipboard(
            blank_header, (ClipboardBranch*)_sub_branch); // , 0
        // _current_model()->
        QList<boost::intrusive_ptr<i_t>> buffer;
        blank_header->traverse_direct([&](boost::intrusive_ptr<Linker> il_in_clipboard) {
            auto index =
                static_cast<tkm_t*>(_current_model())
                    ->index([&](boost::intrusive_ptr<const Linker> link_in_tree) {
                        return link_in_tree->host()->id() ==
                            il_in_clipboard->host()->id();
                    });
            if (static_cast<QModelIndex>(index).isValid()) {
                auto it_on_tree = _current_model()->item(index);
                if (static_cast<QModelIndex>(index).parent().isValid()) {
                    auto parent_item =
                        _current_model()->item(static_cast<QModelIndex>(index).parent());
                    assert(static_cast<QModelIndex>(_know_root->index(parent_item)).isValid()); // assert(parent_item->field("name") !=
                                                                                                // clipboard_items_root);
                    if ((static_cast<tkm_t*>(_current_model())
                                ->child([&](boost::intrusive_ptr<const i_t> t) {
                                    return t->id() == parent_item->id();
                                })) // if(it_on_tree->up_linker()->host_parent()->field("name")
                                    // ==
                                    // clipboard_items_root)it_on_tree->parent(parent_item);
                        && !buffer.contains(it_on_tree))
                        buffer << it_on_tree;
                }
            }
        });
        //
        if (buffer.size() > 0) { // == blank_header->count_direct()
            blank_header->clear();
            for (auto it_on_tree : buffer) {
                move(_sibling_tree_index, it_on_tree, [&](boost::intrusive_ptr<const Linker> il) {
                    return il->host()->field<name_key>() ==
                        it_on_tree->field<name_key>();
                }); // it->parent(blank_header);
            }
            _result = _current_model()->item(host_index);
        } else if (blank_header) {
            _result =
                move_children(_sibling_tree_index, blank_header, [&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source) -> bool {
                    return target->host()->field<url_key>() ==
                        source->host()->field<url_key>() &&
                        target->host()->field<name_key>() ==
                        source->host()->field<name_key>();
                })[0];
        }
        //
        if (_result) {
            // Добавление ветки
            QString pasted_branch_id =
                _result->id(); // _model_paste_from_clipboard(_current_index,
                               // (ClipboardBranch *)_sub_branch);
            // if(is_branch)
            // pasted_branch_id = source_model()->lock_paste_as_sibling(index,
            // (ClipboardBranch *)branch);
            // else
            // pasted_branch_id = source_model()->lock_paste_as_child(index,
            // (ClipboardBranch *)branch);
            // update indexes
            auto host_parent_index = static_cast<QModelIndex>(host_index).parent();
            if (host_parent_index.isValid())
                _current_model()->update_index(index_tree(host_parent_index));
            setExpanded(host_index, true);

            // Установка курсора на новую созданную ветку
            auto pasted_branch_item =
                _current_model()->item([=](boost::intrusive_ptr<const i_t> it) {
                    return it->id() == pasted_branch_id;
                });
            QStringList pasted_branch_path = pasted_branch_item->path_list();
            // find_object<MainWindow>("mainwindow")
            gl_paras->main_window()->set_tree_position(
                source_model()->root_item()->id(), pasted_branch_path);

            _know_model_board->synchronized(false);
            // Сохранение дерева веток
            // find_object<TreeScreen>(tree_screen_singleton_name)->
            know_model_save();

            // }
        }
        // азблокируется главное окно
        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->setEnabled(true);
    }
}

boost::intrusive_ptr<i_t>
tv_t::move(boost::intrusive_ptr<TreeIndex> _treeindex, // std::function<KnowModel*()> _current_model, QModelIndex _current_index
    boost::intrusive_ptr<i_t> _source_item,
    const tv_t::substitute_condition& _substitute_condition,
    bool save_immediately)
{
    //
    auto paste_sibling_impl =
        [&](boost::intrusive_ptr<TreeIndex> _modelindex,
            boost::intrusive_ptr<i_t> _source_item,
            const tv_t::substitute_condition& substitute_condition_)
        -> boost::intrusive_ptr<i_t> { // ->boost::intrusive_ptr<TreeItem>
        boost::intrusive_ptr<i_t> result(nullptr);
        auto current_model = _modelindex->current_model();
        // auto host_parent_index = _modelindex->host_parent_index();
        auto host = _modelindex->host(); // _current_model()->item(_current_index);

        assert(host); // make it automatically?

        assert(host != _source_item); // mayb be equal because integrity is false
        auto current_parent = host->parent();
        // auto current_parent_    =
        // _current_model()->item(_current_index.parent());
        assert(current_parent);
        // assert(_current_parent == current_parent_);
        if (current_parent && current_parent->id() != "") {
            ////            bool current_parent_valid = [&]() {
            ////                if(!current_model()->item([ =
            ///](boost::intrusive_ptr<const TreeItem> it) {return it->id() ==
            ///current_parent->id();}) && current_parent->id() != "") {
            ////                    intercept(TreeIndex(current_model,
            ///current_parent));
            ////                }
            ////                return
            ///(current_model()->index(current_parent).isValid())
            ////                && (current_model()->item([ =
            ///](boost::intrusive_ptr<const TreeItem> it) {return it->id() ==
            ///current_parent->id();}));
            ////            }();

            // boost::intrusive_ptr<TreeIndex> _parent_modelindex;
            // try {
            // _parent_modelindex = new TreeIndex(current_model, current_parent,
            // current_parent->sibling_order([&] (boost::intrusive_ptr<const
            // Linker> il) {
            // return il == host->linker() && il->host() == host && host->parent()
            // == il->host_parent();
            // }));
            // } catch(std::exception &e) {throw e; }

            ////    for(int i = 0; i < it->current_count(); i++) {

            ////        auto current_item = it->child(i);
            ////    if(_current_model->is_item_id_exists(it->id())) {
            ////    return add_branch(_current_model->index(it), it,
            ///insert_sibling_branch, _current_model);

            ////    auto _current_index = index_current();
            /////selectionModel()->currentIndex(); //_current_model->index(it);

            // if(host_parent_index.isValid()) { // root_item can do this!!!

            // Получение ссылки на узел, который соответствует выделенной строке
            // auto current_item = _current_model->item(_current_index);

            // if(!_current_index.isValid()) {
            // selectionModel()->setCurrentIndex(_current_model->index(0, 0,
            // QModelIndex()), QItemSelectionModel::ClearAndSelect);
            // assert(_current_index.isValid());
            // }

            // find_object<MainWindow>("mainwindow")
            gl_paras->main_window()->setDisabled(true);
            // Получение уникального идентификатора
            if (detail::to_qstring(_source_item->id()).length() == 0)
                _source_item->field<id_key>(detail::to_qstring(_source_item->field<dir_key>()).length() > 0 ? id_value(detail::to_qstring(_source_item->field<dir_key>())) : get_unical_id());
            // QModelIndex setto;

            // Вставка данных и установка курсора

            // Одноранговая ветка
            // if(insert_sibling_branch) {

            // auto current_parent =
            // _current_model()->item(_current_index)->parent();
            // assert(current_parent);

            QList<boost::intrusive_ptr<i_t>> _alternative_items;
            // auto check = _current_model->item(_current_index);
            // assert(_current_model->item_by_id(check->id()));
            // assert(_current_model->item_by_name(check->name()));
            // assert(_know_model_board->item_by_name(check->name()));
            // assert(_know_model_board->item_by_id(check->id()));

            // if(current_parent_valid) {
            _alternative_items = current_parent->children_direct(substitute_condition_);
            // auto _alternative_item_has_no_child_first = [&]
            // {boost::intrusive_ptr<TreeItem> result; for(auto i :
            // _alternative_items) {if(i->count_direct() == 0) {result = i;
            // break;}} return result;}();
            if (_alternative_items.size() > 0) { // && same->is_empty()
                ////            if(_alternative_item_has_no_child_first) {
                ////
                ///assert(_alternative_item_has_no_child_first->name() ==
                ///_source_item->name());
                ////                result = view_merge_to_left(_current_model,
                ///_alternative_item_has_no_child_first, _source_item);
                ////            } else
                // for(int i = 1; i < _alternative_items.size(); i++) {
                // result = view_merge_to_left(_current_model,
                // _alternative_items[0], _alternative_items[i]);
                // }

                QMutableListIterator<boost::intrusive_ptr<i_t>> it(
                    _alternative_items);
                result = it.next(); // _source_item;  // _source_item might be a
                // clipboard item
                assert(result);
                while (it.hasNext()) {
                    // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                    // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index =
                    // new TreeIndex(current_model, result); } catch(std::exception
                    // &e) {throw e; } return tree_index; } ();
                    result = TreeLevel::instance(
                        TreeIndex::item_require_treeindex(current_model, result),
                        it.next())
                                 ->merge();
                    assert(result);
                    assert(
                        current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                            return t->id() == result->id();
                        }));
                }
                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex(current_model, result); } catch(std::exception &e)
                // {throw e; } return tree_index; } ();
                result = TreeLevel::instance(
                    TreeIndex::item_require_treeindex(current_model, result),
                    _source_item)
                             ->merge();
                assert(result);
                if (result) {
                    assert(
                        current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                            return t->id() == result->id();
                        }));
                }
                ////                        assert(result == _source_item);
                ////                    if(result != _source_item) {
                ////                        result =
                ///view_merge_to_left(_current_model, result, _source_item);
                ////                        assert(result);
                ////                        assert(_current_model()->item([ =
                ///](boost::intrusive_ptr<const TreeItem> t) {return t->id() ==
                ///result->id();}));

                ////                    }
                // boost::intrusive_ptr<TreeIndex> tree_index_ = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex([&] {return _know_root; }, result->parent(),
                // result->parent()->sibling_order([&] (boost::intrusive_ptr<const
                // Linker> il)
                // {
                // return il == result->linker() && il->host() == result &&
                // result->parent() == il->host_parent();
                // })); } catch(std::exception &e) {throw e; } return tree_index; }
                // ();

                // view_paste_from_children_move(_modelindex, _source_item,
                // [&](boost::intrusive_ptr<TreeItem> target,
                // boost::intrusive_ptr<TreeItem> source)->bool {return
                // target->field("url") == source->field("url") &&
                // target->field("name") == source->field("name");});  // ,
                // _substitute_condition

                assert(result != _know_model_board->root_item());
            } else {
                // Вставка новых данных в модель дерева записей
                result = TreeLevel::instance(TreeIndex::item_require_treeindex(current_model, current_parent),
                    _source_item)
                             ->move(); // static_cast<tkm_t *>(current_model())//
                                       // TreeIndex(current_model, current_parent,
                                       // _modelindex.sibling_order())

                assert(
                    current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                        return t->id() == result->id();
                    }));
                assert(result != _know_model_board->root_item());
            }
            // select_as_current(TreeIndex::instance(current_model,
            // result->parent(), result));

            assert(result);
            // Установка курсора на только что созданную позицию

            // Чтобы вычислить позицию, надо синхронно получить parent элемента,
            // на уровне которого должен был создасться новый элемент.
            // Parent надо получить и в виде объекта QModelIndex, и в виде объекта
            // Item
            // Затем у объекта Item выяснить количество элементов, и установить
            // засветку через метод index() относительно parent в виде QModelIndex
            // QModelIndex
            assert(current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                return t->id() == result->id();
            }));

            //// select_as_current(TreeIndex::instance(current_model,
            ///result->parent(), result));
            //// Сохранение дерева веток
            // know_model_save();

            select_as_current(TreeIndex::item_require_treeindex(current_model, result));
            assert(
                (_source_item == result) ||
                substitute_condition_(
                    result->linker()) // (_source_item->name() == result->name())
                );

            // } else if(_current_model->root_item()->id() == it->id()) {

            // assert(new_branch_root == result);  // not must, if you already
            // have it

            // }

            // }
            // }   // if(current_parent_valid)

            // find_object<MainWindow>("mainwindow")
            gl_paras->main_window()->setEnabled(true);

            // }

            // else {

            // }
        }
        assert(result);

        return result;
    };
    auto paste_sibling =
        [&](boost::intrusive_ptr<TreeIndex> _modelindex,
            boost::intrusive_ptr<i_t> _source_item,
            const tv_t::substitute_condition& substitute_condition_)
        -> boost::intrusive_ptr<i_t> {
        auto host = _modelindex->host(); // _current_model()->item(_current_index);

        assert(host); // make it automatically?
        return (host == _source_item) ? host : paste_sibling_impl(_modelindex, _source_item, substitute_condition_);
    };

    boost::intrusive_ptr<i_t> result(nullptr);
    auto current_model = _treeindex->current_model();
    // auto host_index = _treeindex->host_index();
    auto host = _treeindex->host();
    // assert(host_index.isValid() ? current_model()->item(host_index) != nullptr
    // : true);
    // for(int i = 0; i < it->current_count(); i++) {
    // auto current_item = it->child(i);
    // if(_current_model->is_item_id_exists(it->id())) {
    // return add_branch(_current_model->index(it), it, insert_sibling_branch,
    // _current_model);
    // auto _current_index = index_current(); //selectionModel()->currentIndex();
    // //_current_model->index(it);
    if (host && _source_item) { // current_index.isValid()
        // boost::intrusive_ptr<TreeItem> current_item = _modelindex.parent(); //
        // _current_model()->item(_current_index);
        // assert(!current_item->contains_direct(std::forward<const
        // boost::intrusive_ptr<Linker>>(_source_item->linker())));
        // assert(!current_item->contains_direct(std::forward<const
        // boost::intrusive_ptr<TreeItem>>(_source_item)));
        if (!host->contains_direct(
                _source_item
                    ->linker())) { // current_item->contains_direct(_source_item->linker())
                                   // ||
            if (host->count_direct() > 0) {
                auto _child = host->child_direct(0); // _treeindex->sibling_order()   //
                                                     // current_item->count_direct() - 1

                assert(_child != _source_item); // may be equal because integrity is false

                // if(current_item->count_direct() > 0) {
                // assert(current_item->item_direct(0) != _source_item);
                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex(current_model, _child); } catch(std::exception &e) {throw
                // e; } return tree_index; } ();
                result = paste_sibling(
                    TreeIndex::item_require_treeindex(current_model, _child), _source_item,
                    _substitute_condition); // _current_model,
                                            // _current_model()->index(current_item->item_direct(0))
                assert(result);
            } else {
                // Получение ссылки на узел, который соответствует выделенной строке
                // auto current_item = _current_model->item(_current_index);

                // if(!_current_index.isValid()) {
                // selectionModel()->setCurrentIndex(_current_model->index(0, 0,
                // QModelIndex()), QItemSelectionModel::ClearAndSelect);
                // assert(_current_index.isValid());
                // }

                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setDisabled(true);
                if (detail::to_qstring(_source_item->id()).length() == 0)
                    _source_item->field<id_key>(
                        detail::to_qstring(_source_item->field<dir_key>()).length() > 0 ? id_value(detail::to_qstring(_source_item->field<dir_key>())) : get_unical_id()); // Получение уникального идентификатора
                // QString id = get_unical_id();

                // QModelIndex setto;

                // Вставка данных и установка курсора

                // Одноранговая ветка

                // Подветка

                // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex(current_model, host_parent, _modelindex->sibling_order());
                // } catch(std::exception &e) {throw e; } return tree_index; } ();
                // Вставка новых данных в модель дерева записей
                result =
                    TreeLevel::instance(TreeIndex::item_require_treeindex(current_model, host), _source_item)->move();
                assert(result);
                assert(result != _know_model_board->root_item());
                // Установка курсора на только что созданную позицию
                // QModelIndex
                assert(current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                    return t->id() == result->id();
                }));
                ////            setto = _current_model()->index(result);
                /////index_child(_current_index, current_item->current_count() - 1);
                ////            assert(setto.isValid());
                // boost::intrusive_ptr<TreeIndex> tree_index_ = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex([&] {return _know_root; }, result->parent(),
                // result->parent()->sibling_order([&] (boost::intrusive_ptr<const
                // Linker> il)
                // {
                // return il == result->linker() && il->host() == result &&
                // result->parent() == il->host_parent();
                // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
                select_as_current(TreeIndex::item_require_treeindex(current_model, result)); // setto
                // selectionModel()->select(setto, current_tree_selection_mode);
                // selectionModel()->setCurrentIndex(setto,
                // current_tree_current_index_mode);   // ClearAndSelect
                // А можно было установить курсор на нужную позицию и так
                // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
                // QItemSelectionModel::ClearAndSelect);
                // Сохранение дерева веток
                // find_object<TreeScreen>(tree_screen_singleton_name)->
                if (save_immediately)
                    know_model_save();
                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setEnabled(true);
                // return
                result = current_model()->item([=](boost::intrusive_ptr<const i_t> t) {
                    return t->id() == result->id();
                });
                //				assert((_source_item == result) || (_source_item->name() == result->name()));
                // } else if(_current_model->root_item()->id() == it->id()) {

                // assert(new_branch_root == result);  // not must, if you already have
                // it

                // }

                // }
            }
        } else // for current_item->contains_direct(_source_item->linker()) !=
            // nullptr, maybe just some merge, go view_paste_sibling upon
            result = _source_item;
        assert(result);

        select_as_current(TreeIndex::item_require_treeindex(current_model, result));
    }
    return result;
}

QList<boost::intrusive_ptr<i_t>>
tv_t::delete_permanent(const std::function<tkm_t*()>& _current_model, QList<boost::intrusive_ptr<i_t>> _items, delete_strategy _delete_strategy, const QString& _mode, const bool _cut_branch_confirm)
{
    QList<boost::intrusive_ptr<i_t>> items_candidate;
    QList<boost::intrusive_ptr<i_t>> result_items;
    // auto _tree_screen = static_cast<TreeScreen *>(parent());
    // QModelIndexList _index_list = is_index_localized(_origin_index_list) ?
    // _origin_index_list : index_localize(_origin_index_list);
    if (_items.size() > 0) {
        //		auto page_ = _items[0]->page();

        //		auto current_browser = page_ ? page_->browser() :
        //gl_paras->main_window()->browser(_items[0]);
        std::function<bool(QList<boost::intrusive_ptr<i_t>>)> has_encrypt = [&](
                                                                                QList<boost::intrusive_ptr<i_t>> _result) {
            bool disableFlag = false;
            // Перебираются удаляемые ветки
            for (auto it : _result) { // for(int i = 0; i < _index_list.size(); ++i) {
                // QModelIndex index = _index_list.at(i);
                // auto it = _current_model()->item(index);
                // Если у ветки установлен флаг шифрования
                if (it->field<crypt_key>() == crypt_value(true)) {
                    disableFlag = true;
                    break;
                }
                // Проверяется наличие флага шифрования у всех подветок
                QList<QStringList> cryptFlagsList = it->path_children_all<crypt_key>(); //boost::mpl::c_str<crypt_key>::value

                foreach (QStringList cryptFlags, cryptFlagsList) {
                    if (cryptFlags.contains("1")) {
                        disableFlag = true;
                        break;
                    }
                }
                if (disableFlag)
                    break;
            } // Закрылся цикл перебора всех выделенных для удаления веток
            // Если в какой-то ветке обнаружено шифрование
            if (disableFlag) {
                QMessageBox messageBox(this);
                messageBox.setWindowTitle(tr("Unavailable action"));
                messageBox.setText(
                    tr("In your selected data found closed item. Action canceled."));
                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
                messageBox.exec();

                // азблокируется главное окно
                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setEnabled(true);
                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->blockSignals(false);

                // return _result;
            }
            return disableFlag;
        };

        // auto _source_model = source_model();

        // На время удаления блокируется главное окно
        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->setDisabled(true);
        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->blockSignals(true);

        // preparations

        std::sort(_items.begin(), _items.end(), [&](boost::intrusive_ptr<i_t> t0, boost::intrusive_ptr<i_t> t1) {
            return static_cast<QModelIndex>(_current_model()->index(t0)).row() <
                static_cast<QModelIndex>(_current_model()->index(t1)).row();
        });

        auto index_to_be_delete_first = _current_model()->index(_items.first());
        auto _index_common_parent =
            static_cast<QModelIndex>(index_to_be_delete_first).parent();

        // auto index_to_be_delete_last = _index_list.last();
        auto _item_to_be_deleted_first = _items.first();
        auto _item_common_parent =
            _item_to_be_deleted_first
                ->parent(); // may be a clipboard root item : blank header
        assert(_item_common_parent);
        if (static_cast<QModelIndex>(_know_root->index(_item_common_parent))
                .isValid()) { // _item_common_parent->field("name") !=
                              // clipboard_items_root
            auto deleted_position_first = _item_common_parent->sibling_order(
                [&](boost::intrusive_ptr<const Linker> il) {
                    return il->host()->id() == _item_to_be_deleted_first->id() &&
                        _item_to_be_deleted_first->linker() == il &&
                        _item_to_be_deleted_first->parent() == il->host_parent();
                }); // items_update_selected();
            // auto deleted_position_last =
            // _source_model->item(index_to_be_delete_last)->sibling_order();

            // Список имен веток, которые нужно удалить
            QStringList branches_name;
            for (auto it : _items)
                // QModelIndex index = _index_list.at(i);
                if (it->parent() == _item_common_parent) { // if(index.isValid() &&
                                                           // index.parent() ==
                                                           // _index_common_parent) {
                    if (it->id() == _current_model()->session_id())
                        _current_model()->session_id(TreeIndex::item_require_treeindex(_current_model, _item_common_parent)); ////        keep =
                                                                                                                              ///source; remove =
                                                                                                                              ///target;
                    // boost::intrusive_ptr<TreeIndex> tree_index = [&]
                    // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                    // TreeIndex(_current_model, _item_common_parent); }
                    // catch(std::exception &e) {throw e; } return tree_index; } ();
                    // auto it = _current_model()->item(index);
                    branches_name << it->field<name_key>();
                    // _index_list.push_back(index);
                    items_candidate << it;
                }
            // Если системный пароль не установлен, зашифрованные ветки удалять нельзя
            if (!((gl_paras->crypt_key().size() == 0) &&
                    has_encrypt(items_candidate))) {
                // Создается окно с вопросом, нужно удалять ветки или нет
                QString title, text, del_button;
                bool enable_question = true;
                if (_mode == "delete") {
                    title = tr("Delete item(s)");
                    text = tr("Are you sure you wish to delete item(s) <b>") +
                        branches_name.join(", ") + tr("</b> and all sub items?");
                    del_button = tr("Delete");

                    enable_question = true;
                } else if (_mode == "cut") {
                    title = tr("Cut item");
                    text = tr("Are you sure you wish to cut item <b>") +
                        branches_name.join(", ") + tr("</b> and all sub items?");
                    del_button = tr("Cut");
                    if (appconfig->cut_branch_confirm() && _cut_branch_confirm)
                        enable_question = true;
                    else
                        enable_question = false;
                }
                bool enable_delete = true;
                if (enable_question) {
                    QMessageBox message_box(this);
                    message_box.setWindowTitle(title);
                    message_box.setText(text);
                    message_box.addButton(tr("Cancel"), QMessageBox::RejectRole);
                    QAbstractButton* delete_button =
                        message_box.addButton(del_button, QMessageBox::AcceptRole);
                    message_box.exec();
                    if (message_box.clickedButton() == delete_button)
                        enable_delete = true;
                    else
                        enable_delete = false;
                }
                // Если удаление подтверждено
                if (enable_delete) {
                    // Сохраняется текст в окне редактирования
                    // Нужно, чтобы нормально удалилась текущая редактируемая запись,
                    // если она находится в удаляемой ветке
                    // find_object<MainWindow>("mainwindow")
                    auto rs =
                        dynamic_cast<rs_t*>(gl_paras->vtab_record()->currentWidget());
                    if (rs) {
                        auto blogger = rs->blogger();
                        if (blogger)
                            blogger->save_text_context();
                    }
                    // Сортировка списка индексов по вложенности методом пузырька
                    // Индексы с длинным путем перемещаются в начало списка
                    for (int i = 0; i < items_candidate.size(); i++)
                        for (int j = items_candidate.size() - 1; j > i; j--) {
                            QStringList path_1 =
                                items_candidate.at(j - 1)->path_list<id_key>(); // (_current_model()->item(  // ))
                            QStringList path_2 =
                                items_candidate.at(j)->path_list<id_key>(); // (_current_model()->item(  // ))
                            if (path_1.size() < path_2.size())
                                items_candidate.swap(j - 1, j);
                        }
                    qDebug() << "Path for delete";
                    for (int i = 0; i < items_candidate.size(); ++i)
                        qDebug() << items_candidate.at(i)
                                        ->path_list<id_key>(); // (_current_model()->item(  //))
                    // auto _source_model = source_model();
                    // _source_model->beginRemoveRows(_index_common_parent,
                    // _index_list.first().row(), _index_list.last().row());
                    // Вызов удаления веток
                    for (auto it :
                        items_candidate) { // for(int i = 0; i < _result.size(); ++i) {
                        if (it->id() ==
                            static_cast<tkm_t*>(_current_model())->session_id())
                            static_cast<tkm_t*>(_current_model())
                                ->session_id(TreeIndex::item_require_treeindex(_current_model, it->parent()));
                        if (it->linker()->host() && it->linker()->host_parent()) {
                            auto result =
                                (static_cast<tkm_t*>(_current_model())->*_delete_strategy)(
                                    it->linker());
                            // just for counting, result.get() == 0 is fine
                            result_items.append(
                                result); // model_delete_one( // just for counting, 0 is fine
                                         // _current_model()->model_delete_item(it)
                        }
                    }
                    if (result_items.size() > 0) {
                        // _source_model->endRemoveRows();
                        // _source_model = source_model();
                        // _current_model()->update_index(index_tree(_index_common_parent));
                        emit _current_model()->layoutChanged(QList<QPersistentModelIndex>() << _index_common_parent);

                        QModelIndex setto;
                        boost::intrusive_ptr<i_t> left_sibling_item(nullptr);
                        auto get_left_sibling_from_tree =
                            [&]() -> boost::intrusive_ptr<i_t> {
                            boost::intrusive_ptr<i_t> left_sibling_(nullptr);
                            assert(_item_common_parent && static_cast<QModelIndex>(_know_root->index(_item_common_parent)).isValid());
                            if (_item_common_parent &&
                                static_cast<QModelIndex>(
                                    _know_root->index(_item_common_parent))
                                    .isValid()) { // _item_common_parent->field("name") !=
                                                  // clipboard_items_root
                                int new_count = _item_common_parent->count_direct();
                                // Одноранговая ветка
                                if (new_count > 0) {
                                    int new_position = 0;
                                    if ((deleted_position_first - 1) >= 0)
                                        new_position = deleted_position_first - 1;
                                    else
                                        new_position = deleted_position_first;
                                    setto = _current_model()->index(
                                        new_position, 0,
                                        static_cast<QModelIndex>(index_to_be_delete_first)
                                            .parent());
                                    left_sibling_ =
                                        _item_common_parent->child_direct(new_position);
                                    assert(left_sibling_);
                                    assert(!result_items.contains(left_sibling_));
                                    setto = _current_model()->index(left_sibling_);
                                    assert(setto.isValid());
                                    // select_as_current(TreeIndex::instance(_current_model,
                                    // left_sibling_item->parent(), left_sibling_item));
                                } else {
                                    while (_item_common_parent &&
                                        _item_common_parent !=
                                            _know_model_board->root_item()) {
                                        if (_item_common_parent != _current_model()->root_item() &&
                                            static_cast<QModelIndex>(
                                                _current_model()->index(_item_common_parent))
                                                .isValid()) {
                                            left_sibling_ = _item_common_parent;
                                            setto = _current_model()->index(left_sibling_);
                                            break;
                                        } else { // if(_parent != _know_model_board->root_item())
                                            cursor_focus();
                                            _item_common_parent = _item_common_parent->parent();
                                            if (_item_common_parent) {
                                                left_sibling_ = _item_common_parent;
                                                setto = _current_model()->index(left_sibling_);
                                            } else {
                                                tree_empty_controll();
                                                left_sibling_ =
                                                    source_model()->root_item()->child_direct(0);
                                                setto = _current_model()->index(left_sibling_);
                                            }
                                        }
                                    }
                                    assert(left_sibling_);
                                    assert(!result_items.contains(left_sibling_));
                                    setto = _current_model()->index(left_sibling_);
                                    assert(setto.isValid());

                                    // select_as_current(TreeIndex::instance(_current_model,
                                    // left_sibling_item->parent(), left_sibling_item));
                                }
                            }
                            return left_sibling_;
                        };
                        // might be deleting currently
                        //                                                {
                        //                                                        if (gl_paras->main_window()->browsers().size() > 0) {
                        //                                                                //							web::Browser
                        //                                                                //*current_browser = gl_paras->main_window()->activated_browser();
                        //                                                                auto w = gl_paras->vtab_record()->currentWidget();
                        //                                                                auto rs = static_cast<rs_t*>(w);
                        //                                                                auto tabmanager = rs->browser()->tab_widget(); // current_browser->tabmanager();
                        //                                                                QList<boost::intrusive_ptr<i_t>> good_list;
                        //                                                                for (int i = 0; i < tabmanager->count(); i++) {
                        //                                                                        auto v = tabmanager->webView(i);
                        //                                                                        auto it = v->page()->binder()->host();
                        //                                                                        if (!result_items.contains(it))
                        //                                                                                good_list << it;
                        //                                                                }
                        //                                                                if (good_list.size() > 0) {
                        //                                                                        // auto v = tabmanager->currentWebView();
                        //                                                                        auto sibling = good_list[0]; // tabmanager->sibling(v);
                        //                                                                        left_sibling_item = sibling->page()->binder()->host();
                        //                                                                        assert(left_sibling_item);
                        //                                                                        assert(!result_items.contains(left_sibling_item));
                        //                                                                        setto = _current_model()->index(left_sibling_item);
                        //                                                                        assert(setto.isValid());
                        //                                                                } else
                        //                                                                        left_sibling_item = get_left_sibling_from_tree();
                        //                                                        } else
                        //                                                                left_sibling_item = get_left_sibling_from_tree();
                        //                                                }
                        left_sibling_item = get_left_sibling_from_tree();
                        select_as_current(TreeIndex::item_require_treeindex(_current_model, left_sibling_item));
                        qDebug() << "Delete finish";

                        // update_model(_current_know_branch);

                        // Сохранение дерева веток
                        // find_object<TreeScreen>(tree_screen_singleton_name)->
                        know_model_save();
                        // reload_knowtree();
                        qDebug() << "Save new tree finish";

                        // _actionlist[action_paste_branch]->setEnabled(false);
                        // _actionlist[action_paste_sub_branch]->setEnabled(false);
                    }
                } else {
                    _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
                    _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);
                }
                // азблокируется главное окно
                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setEnabled(true);
                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->blockSignals(false);

                tree_empty_controll();
                tree_crypt_control();
            }
        } // Закрылось условие что системный пароль не установлен
    }
    return result_items;
}

void tv_t::cut(bool _cut_branch_confirm)
{
    // bool copy_result = false;
    // auto _tree_screen = static_cast<TreeScreen *>(parent());
    // copy_result
    auto index_list = copy();
    if (index_list.size() > 0) { // copy_result
        delete_permanent([&]() { return source_model(); }, [&]() {
                       QList<boost::intrusive_ptr<i_t>> r;
                       for (auto ix : index_list)
                         r << source_model()->child(ix);
                       return r; }(), &tkm_t::delete_permanent_recursive, "cut", _cut_branch_confirm);
        // view_remove(index_list, "cut", _cut_branch_confirm);
        // //this->_know_branch,
    }
    _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
    _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);
}

QModelIndexList tv_t::copy(void)
{ // const
    // bool copy_result = false;
    // auto _tree_screen = static_cast<TreeScreen *>(parent());
    //// Вспомогательная функция при копировании ветки в буфер
    // std::function<void(ClipboardBranch *, QStringList)> //  , bool    //void
    // TreeScreen::
    // branch_add_to_clipboard
    // = [&](ClipboardBranch * branch_clipboard_data, QStringList path) {  // ,
    // bool is_root

    //// boost::intrusive_ptr<TreeItem> curr_item;
    //// QMap<QString, QString> curr_item_fields;
    //// QString branch_id;
    ////    std::shared_ptr<RecordTable> curr_item_record_table;

    //// Добавление ветки
    // boost::intrusive_ptr<TreeItem> curr_item = _know_model_board->item(path);
    // QMap<QString, QString> curr_item_fields = curr_item->fields_all(); //
    // Раньше было getAllFieldsDirect()
    // QString branch_id = curr_item_fields["id"];

    ////        if(is_root)
    ////            branch_clipboard_data->branch_add(curr_item->parent_id(),
    ///curr_item_fields);
    ////        else
    // branch_clipboard_data->branch_add(curr_item->parent_id(),
    // curr_item_fields);
    // branch_clipboard_data->record_add(curr_item->parent_id(), curr_item);
    //// Добавление конечных записей
    ////    auto curr_item_record_table = curr_item->record_table();

    // for(int i = 0; i < curr_item->count_direct(); i++) {
    //// Полный образ записи (с файлами и текстом)
    // boost::intrusive_ptr<TreeItem> record = curr_item->item_fat(i);
    // branch_clipboard_data->branch_add(branch_id, record->fields_all());
    // branch_clipboard_data->record_add(branch_id, record);
    // }
    // };

    qDebug() << "In branch_copy()";

    // Сохраняется текст в окне редактирования
    // find_object<MainWindow>("mainwindow")
    auto rs = dynamic_cast<rs_t*>(gl_paras->vtab_record()->currentWidget());
    if (rs) {
        auto blogger = rs->blogger();
        if (blogger)
            blogger->save_text_context();
    }
    // Получение списка индексов QModelIndex выделенных элементов
    // QModelIndexList _origin_index_list = selectionModel()->selectedIndexes();
    QModelIndexList _selectitems =
        LocalLizeInitializer(this)()
            .indexes(); // selectionModel()->selectedIndexes(); //
    // is_index_localized(_origin_index_list) ? _origin_index_list :
    // index_localize(_origin_index_list);

    QModelIndex _index_current;
    bool indexs_valid = true;
    QList<boost::intrusive_ptr<i_t>> items;
    auto _source_model = [&]() { return source_model(); };
    // Если выбрано более одной ветки
    if (_selectitems.size() >= 1) {
        // QMessageBox messageBox(this);
        // messageBox.setWindowTitle(tr("Unavailable action"));
        // messageBox.setText(tr("Please select a single item for copy."));
        // messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        // messageBox.exec();
        // return copy_result;
        for (auto i : _selectitems) {
            if (!i.isValid()) {
                indexs_valid = false;
                break;
            } else {
                auto it = static_cast<tkm_t*>(_source_model())->child(i);
                if (!items.contains(it))
                    items << it;
            }
        }
        if (indexs_valid)
            _index_current = static_cast<tkm_t*>(_source_model())
                                 ->index(items[0]); // _selectitems[0];
    }
    // Получение индекса выделенной ветки
    // _index_current = index_current();
    if (indexs_valid) { // _index_current.isValid()
        // -------------------
        // Копирование в буфер
        // -------------------

        qDebug() << "Branch copy to buffer";

        // Создается ссылка на буфер обмена
        QClipboard* cbuf = QApplication::clipboard();

        // Создается объект с данными для заполнения буфера обмена
        // Если в буфере есть какие-то старые данные, они удаляются
        static int fillflag = 0;
        if (fillflag == 1) {
            const ClipboardBranch* branch_clipboard_data_previos;
            branch_clipboard_data_previos =
                qobject_cast<const ClipboardBranch*>(cbuf->mimeData());
            if (branch_clipboard_data_previos != nullptr)
                delete branch_clipboard_data_previos;
            fillflag = 0;
        }
        ClipboardBranch* _sub_branch = new ClipboardBranch();
        fillflag = 1;

        // QString _clipboard_root_parent_id = get_unical_id();

        // for(auto item : items) {

        ////            // Получение ссылки на узел, который соответствует
        ///выделенной ветке
        ////            auto item = _source_model->item(_index_current);

        ////            //        // Получение пути к выделенной ветке
        ////            //        QStringList _current_item_absolute_path =
        ///item->path_absolute();

        ////            //        // Получение путей ко всем подветкам
        ////            //        QList<QStringList> sub_branches_absolute_path =
        ///item->path_children_all();

        //// Проверка, содержит ли данная ветка как шифрованные
        //// так и незашифрованные данные
        // bool nocrypt_presence = false;
        // bool encrypt_presence = false;

        //// Флаги на основе состояния текущей ветки
        // if(_know_model_board->item(_index_current)->field(boost::mpl::c_str <
        // crypt_type > ::value) == "1")   //_current_item_absolute_path

        // encrypt_presence = true;
        // else
        // nocrypt_presence = true;

        //// Флаги на основе состояния подветок
        // for(int i = 0; i < item->count_direct(); i++) { //foreach(QStringList
        // curr_absolute_path, sub_branches_absolute_path)
        // if( //_know_model_board->item(curr_absolute_path)
        // item->item_direct(i)->field(boost::mpl::c_str < crypt_type > ::value) ==
        // "1") {
        // encrypt_presence = true;
        // } else {
        // nocrypt_presence = true;
        // }
        // }

        //// Если ветка содержит как шифрованные так и нешифрованные данные
        //// то такую ветку копировать в буфер нельзя
        // if(nocrypt_presence == true && encrypt_presence == true) {
        ////                QMessageBox messageBox(this);
        ////                messageBox.setWindowTitle(tr("Unavailable action"));
        ////                messageBox.setText(tr("This item contains both
        ///unencrypted and encrypted data. Copy/paste operation is possible only for
        ///item that contain similar type data."));
        ////                messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        ////                messageBox.exec();
        // globalparameters.status_bar()->showMessage(tr("This item contains both
        // unencrypted and encrypted data. Copy/paste operation is possible only for
        // item that contain similar type data."), 2000);
        // break;  // return copy_result;
        // }

        ////            // -------------------
        ////            // Копирование в буфер
        ////            // -------------------

        ////            qDebug() << "Branch copy to buffer";

        ////            //            // Создается ссылка на буфер обмена
        ////            //            QClipboard *cbuf = QApplication::clipboard();

        ////            // Создается объект с данными для заполнения буфера обмена
        ////            // Если в буфере есть какие-то старые данные, они удаляются
        ////            static int fillflag = 0;

        ////            if(fillflag == 1) {
        ////                const ClipboardBranch *branch_clipboard_data_previos;
        ////                branch_clipboard_data_previos = qobject_cast<const
        ///ClipboardBranch *>(cbuf->mimeData());

        ////                if(branch_clipboard_data_previos != nullptr)delete
        ///branch_clipboard_data_previos;

        ////                fillflag = 0;
        ////            }

        ////            ClipboardBranch *branch_clipboard_data = new
        ///ClipboardBranch();

        ////            fillflag = 1;

        // item->field("id", _clipboard_root_parent_id);
        // Добавление корневой ветки
        _sub_branch->branch_push(
            TreeIndex::item_require_treeindex(_source_model, current_item()),
            items); // , _clipboard_root_parent_id
        // _current_item_absolute_path
        // branch_add_to_clipboard(branch_clipboard_data,
        // _source_model_cur_item_absolute_path);   // , true

        //// Добавление прочих веток
        // foreach(QStringList curr_path_absolute, sub_branches_absolute_path) {
        // branch_clipboard_data->branch_copy(curr_path_absolute);   //
        // branch_add_to_clipboard(branch_clipboard_data, curr_path_absolute); // ,
        // false
        // }

        auto records =
            _sub_branch->records_by_parent_id(_sub_branch->clip_blank_header_id());
        assert(records.size() == items.size());

        _sub_branch->print();

        //// Объект с ветками помещается в буфер обмена
        // cbuf->setMimeData(branch_clipboard_data);

        _tree_screen->_actionlist[action_paste_branch]->setEnabled(true);
        _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(true);
        // }
        // Объект с ветками помещается в буфер обмена
        if (_sub_branch->clip_blank_header_id() != "")
            cbuf->setMimeData(_sub_branch); // copy_result = true;
        else
            delete _sub_branch;
    } else {
        _tree_screen->_actionlist[action_paste_branch]->setEnabled(false);
        _tree_screen->_actionlist[action_paste_sub_branch]->setEnabled(false);
        // copy_result = false;
    }
    return _selectitems; // copy_result;
}

void tv_t::modify(void)
{
    qDebug() << "In edit_branch()";

    // Получение списка индексов QModelIndex выделенных элементов
    // QModelIndexList selectitems = selectionModel()->selectedIndexes();
    QModelIndex _current_index;

    //// Если выбрано более одной ветки
    // if(selectitems.size() > 1) {
    ////        QMessageBox messageBox(this);
    ////        messageBox.setWindowTitle(tr("Unavailable action"));
    ////        messageBox.setText(tr("You've selected ") +
    ///QString::number(selectitems.size()) + tr(" items.\nPlease select single
    ///item for enabling edit operation."));
    ////        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
    ////        messageBox.exec();
    ////        return;
    // _current_index = selectitems.first();
    // } else {

    // Получение индекса выделенной строки
    _current_index = current_index();
    // }
    if (_current_index.isValid()) {
        // Получение ссылки на узел, который соответствует выделенной строке
        auto item = source_model()->child(_current_index);
        // Если ветка зашифрована и пароль не был введен
        if (item->field<crypt_key>() == crypt_value(true) && gl_paras->crypt_key().length() == 0)
            return;
        // Получение имени ветки
        QString name = item->field<name_key>();

        // Создается окно ввода данных
        bool ok;
        QString newname =
            QInputDialog::getText(this, tr("Edit item name"), tr("Item name:"), QLineEdit::Normal, name, &ok);
        // Если была нажата отмена
        if (!(ok && !newname.isEmpty()))
            return;
        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->setDisabled(true);

        item->field<name_key>(name_value(newname));

        // Сохранение дерева веток
        // find_object<TreeScreen>(tree_screen_singleton_name)->
        know_model_save();

        // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->setEnabled(true);
    }
}

boost::intrusive_ptr<i_t>
tv_t::merge(boost::intrusive_ptr<TreeLevel> _tree_merge)
{
    boost::intrusive_ptr<TreeIndex> tree_index = _tree_merge->tree_index(); // const std::function<KnowModel *()>
                                                                            // &_current_model,
                                                                            // boost::intrusive_ptr<TreeItem> target
    boost::intrusive_ptr<i_t> _to_be_operated = _tree_merge->to_be_operated();
    web::WebView* web_view = nullptr;
    web_view = gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) { return b->host()->id() == _to_be_operated->id() && b->host() == _to_be_operated; });
    boost::intrusive_ptr<i_t> result = tree_index->host();
    if (_to_be_operated) {
        auto _current_model = tree_index->current_model();
        //// auto target = modelindex->host();   // host_parent();
        ////    auto source = source;	//
        ///_current_model()->item([=](boost::intrusive_ptr<const TreeItem> t){return
        ///t->id() == source->id();});
        // if(source->is_ancestor_of([&](boost::intrusive_ptr<const TreeItem>
        // it_){return it_ == result;})){
        // auto temp = result;
        // result = source;	// paste_sibling(TreeIndex::instance(_current_model,
        // source, source->parent()), result, [&](boost::intrusive_ptr<const Linker>
        // il){return il->host()->id() == result->id();});
        // source = temp;
        // tree_index = TreeIndex::instance(_current_model, result);
        // }
        if (result) {
            if (result != _to_be_operated) { // && target_on_tree
                bool session_id_changed = false;
                bool _to_be_operated_is_current_of_rocord_view = false;
                if (_to_be_operated->id() ==
                    static_cast<tkm_t*>(_current_model())->session_id())
                    session_id_changed = true;

                rctrl_t* _rctrl = nullptr;
                if (_to_be_operated->binder()) {
                    //					auto b = _to_be_operated->binder();
                    if (web_view) {

                        auto p = web_view->page(); // b->page();
                        if (p) {
                            //                                                        auto web_view = p->view();
                            _rctrl = p->record_ctrl();
                            if (web_view && _rctrl) {
                                auto rv = _rctrl->view();
                                if (rv)
                                    if (rv->current_item() == _to_be_operated)
                                        _to_be_operated_is_current_of_rocord_view = true;
                            }
                        }
                    }
                }
                gl_paras->main_window()->setDisabled(true);
                result =
                    static_cast<tkm_t*>(_current_model())
                        ->merge(TreeLevel::instance(tree_index, _to_be_operated),                                                                            // target_on_tree  // keep// remove std::bind(_know_model_board, this)
                            std::bind(&tv_t::delete_permanent, this, std::placeholders::_1, std::placeholders::_2, &tkm_t::delete_permanent, "cut", false)); // , std::bind(&tv_t::delete_permanent, this, std::placeholders::_1, std::placeholders::_2, &tkm_t::delete_permanent_recursive, "cut", false)

                assert(_current_model()->item([=](boost::intrusive_ptr<const i_t> t) { return t->id() == result->id(); }));
                if (session_id_changed)
                    static_cast<tkm_t*>(_current_model())->session_id(TreeIndex::item_require_treeindex(_current_model, result));
                if (_to_be_operated_is_current_of_rocord_view)
                    if (_rctrl)
                        _rctrl->select_as_current(_rctrl->index<pos_proxy>(result)); //_to_be_operated
                know_model_save();

                // find_object<MainWindow>("mainwindow")
                gl_paras->main_window()->setEnabled(true);
            }
            // else
            // // if(target_on_tree)
            // result = result;
        }
    }
    // }else result = target_on_tree;
    return result;
}

void tv_t::on_current_changed(const QModelIndex& current, const QModelIndex& previous)
{
    // Q_UNUSED(previous);
    if (previous.isValid())
        _previous_index = previous;
    // selectionModel()->select(previous, current_tree_selection_mode);
    auto result = _know_root->child(current);
    if (currentIndex() != current) {
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
        // TreeIndex([&] {return _know_root; }, result->parent(),
        // result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker>
        // il)
        // {
        // return il == result->linker() && il->host() == result && result->parent()
        // == il->host_parent();
        // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        select_as_current(
            TreeIndex::item_require_treeindex([&] { return _know_root; }, result));
    }
}

void tv_t::on_selection_changed(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)
}

void tv_t::on_current_row_changed(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(current);
    // Q_UNUSED(previous);
    // auto index_list = selectionModel()->selectedIndexes();
    // for(auto _i : index_list) { // always one element
    // selectionModel()->select(_i, current_tree_selection_mode);
    // }
    if (previous.isValid() && _previous_index != previous)
        _previous_index = previous;
    // selectionModel()->select(previous, current_tree_selection_mode);    //
    // dangerous

    // auto it = source_model()->item(current);

    // if(it && it != source_model()->root_item()) {
    // if(current.isValid() && current_index() != current && !it->is_activated())
    // {

    auto result = _know_root->child(current);
    if (currentIndex() != current) {
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
        // TreeIndex([&] {return _know_root; }, result->parent(),
        // result->parent()->sibling_order([&] (boost::intrusive_ptr<const Linker>
        // il)
        // {
        // return il == result->linker() && il->host() == result && result->parent()
        // == il->host_parent();
        // })); } catch(std::exception &e) {throw e; } return tree_index; } ();
        select_as_current(
            TreeIndex::item_require_treeindex([&] { return _know_root; }, result));
    }
    // index_invoke(current_index());    // current

    // }
    // }
}

// void KnowView::on_pressed(const QModelIndex &_index)
// {
// if(_index.isValid()) {
// auto result = _know_root->item(_index);
// auto in = result->parent()->sibling_order([&](boost::intrusive_ptr<const
// Linker> il) {
// return il == result->linker()
// && il->host() == result
// && result->parent() == il->host_parent();
// });
// assert(in != -1);
// auto result_index = select_as_current(TreeIndex([&] {return _know_root;},
// result->parent(),
// result->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
// return il == result->linker()
// && il->host() == result
// && result->parent() == il->host_parent();
// })));
// assert(result_index == _index); // failure!!!

////        selectionModel()->select(_index, current_tree_selection_mode);
/////, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current
////        selectionModel()->setCurrentIndex(_index,
///current_tree_current_index_mode);    //

//// , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

// }
// }

// Сохранение дерева веток на диск
void tv_t::know_model_save(void)
{
    // if(!_know_branch->synchronized())
    // know_root_holder::know_root()->synchronize(_know_branch->root_item());
    auto check_load_finished = [&] {
        bool load_finished = true;
        auto main_win = gl_paras->main_window();
        if (main_win) {
            auto browsers = main_win->browsers();
            for (auto& bro : browsers) {
                auto tab = bro->tab_widget();
                if (tab) {
                    if (tab->count() > 0) {
                        for (int index = 0; index < tab->count(); index++) {
                            auto view = tab->webView(index);
                            if (view) {
                                if (!view->load_finished()) {
                                    load_finished = false;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (load_finished == false) break;
            }
        }
        return load_finished;
    };

    // know_root_holder::know_root()->save();
    // sychronize();
    if (check_load_finished()) {
        if (!_know_model_board->synchronized()) {
            //	_know_model_board->save(); //
            std::thread(&tkm_t::save, _know_model_board) //
                                                         //		    .join();                                 //.hardware_concurrency(); //
                .detach();                               // _know_model_board->save();
        }
    }
}

// Перечитывание дерева веток с диска
void tv_t::know_model_reload(void)
{
    _know_model_board->reload(); // do not do this if xml_file_name of this object is null
                                 // know_root_holder::know_root()->reload();
}

QModelIndex tv_t::previous_index() const
{
    return _previous_index;
}

#ifdef USE_INDEX_FOCUS
// Действия при клике на ветку дерева
void tv_t::cursor_focus(const index_tree& _index)
{
    // auto _tree_screen = static_cast<TreeScreen *>(parent());
    if (static_cast<QModelIndex>(_index)
            .isValid() && // !source_model()->item(_index)->is_activated()
        !globalparameters.main_window()->vtab_record()->find(
            [&](boost::intrusive_ptr<const ::Binder> b) -> bool {
                bool result = false;
                auto ref_it = source_model()->child(_index);
                if (ref_it->binder()) {
                    if (url_equal(b->host()->field<home_type>().toStdString(), ref_it->field<home_type>().toStdString())) {
                        if (ref_it->binder()->page())
                            if (ref_it->binder()->page()->view()->load_finished())
                                result = true;
                    }
                }
                return result;
            })) {
        // QModelIndex index = nodetreeview->selectionModel()->currentIndex();

        // Сохраняется текст в окне редактирования в соответсвующий файл
        // find_object<MainWindow>("mainwindow")
        globalparameters.main_window()->save_text_area();
        auto _soure_model = [&]() { return source_model(); };
        // Получаем указатель на текущую выбранную ветку дерева
        auto selected_item = _soure_model()->item(_index);
        if (selected_item->count_direct() > 0) { // && (result_item->field("url") !=
                                                 // "" || result_item->field("url")
                                                 // == "" )
            // Вначале все инструменты работы с веткой включаются
            QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
            while (i.hasNext()) {
                i.next();
                i.value()->setEnabled(true);
            }
            // Проверяется, происходит ли клик по зашифрованной ветке
            if (selected_item->field<crypt_type>() == "1") {
                // Если пароль доступа к зашифрованным данным не вводился в этой сессии
                if (globalparameters.crypt_key().length() == 0) {
                    // Запрашивается пароль
                    Password password;
                    if (password.retrievePassword() == false) {
                        // Устанавливаем пустые данные для отображения таблицы конечных
                        // записей
                        // find_object<RecordController>("table_screen_controller")

                        // result_item->unique_page()->view()->record_ctrl()->source_model()->clear();

                        // boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&]
                        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                        // TreeIndex(_soure_model, boost::intrusive_ptr<TreeItem>(new
                        // TreeItem(nullptr, QMap<QString, QString>()))); }
                        // catch(std::exception &e) {throw e; } return tree_index; } ();
                        auto parent = TreeItem::dangle_instance(
                            QMap<QString, QString>()); // boost::intrusive_ptr<TreeItem>(new
                                                       // TreeItem(nullptr, QMap<QString,
                                                       // QString>()));
                        auto host = boost::intrusive_ptr<TreeItem>(
                            new TreeItem(parent, QMap<QString, QString>()));
                        intercept(host); // TreeIndex::instance(_soure_model, host,
                                         // parent)// _know_branch->clear();  //
                                         // root_item(boost::intrusive_ptr<TreeItem>(new
                                         // TreeItem(QMap<QString, QString>(), nullptr)));

                        // init_source_model(
                        // nullptr
                        // , result_item->unique_page()->view()->record_ctrl()
                        // ,
                        // result_item->unique_page()->view()->record_ctrl()->record_screen()
                        // , globalparameters.mainwindow()
                        // , globalparameters.meta_editor()
                        //// , globalparameters.record_screens()
                        // );

                        // reset_tabledata(nullptr);

                        // Все инструменты работы с веткой отключаются
                        QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
                        while (i.hasNext()) {
                            i.next();
                            i.value()->setEnabled(false);
                        }
                        return; // Программа дальше не идет, доделать...
                    }
                }
            }
            if (_soure_model()->root_item() != selected_item) {
                // boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&]
                // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
                // TreeIndex(_soure_model, selected_item); } catch(std::exception &e)
                // {throw e; } return tree_index; } ();
                intercept(selected_item); // TreeIndex::instance(_soure_model,
                                          // selected_item, selected_item->parent())
            }                             // _know_branch->intercept(result_item);     //? never do this!!!

            setExpanded(QModelIndex(), true);
            // _shadow_candidate_model->_root_item->field("id",
            // result_item->field("id"));
            // _shadow_candidate_model->_root_item->field("name",
            // result_item->field("name"));

            // Получаем указатель на данные таблицы конечных записей
            // std::shared_ptr<TableData> record_tabledata = item->tabledata();
            // std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();

            // setup_model(_know_branch);

            _tree_screen->enable_up_action(); // _know_branch->root_item()->id() !=
                                              // static_cast<TreeKnowModel
                                              // *>(model())->root_item()->id()   //
                                              // _know_branch->root_item()->id() !=
                                              // know_root()->root_item()->id()
            // for(int i = 0; i < item->tabledata()->size(); i++) {
            // _shadow_candidate_model->_root_item->tabledata()->shadow_record_lite(
            // _shadow_candidate_model->_root_item->tabledata()->size()
            // , item->tabledata()->record(i)
            // );
            // }
            // std::shared_ptr<TableData> record_tabledata = item->tabledata();
            // std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
            // _shadow_candidate_model->_root_item = result_item; //
            // ->record_table(item->record_table()->export_to_dom());
            //// Устанавливаем данные таблицы конечных записей
            // find_object<RecordController>(table_controller_singleton_name)->source_model()->tree_item(_shadow_candidate_model->_root_item);
            //// need lock?
            // reset_tabledata(std::make_shared<RecordTable>(_shadow_candidate_model->_root_item,
            // item->tabledata()->export_to_dom(doc)));
            // Set the text path to the final table entries for the mobile interface
            // options    // Устанавливается текстовый путь в таблице конечных записей
            // для мобильного варианта интерфейса
            if (appconfig->interface_mode() == "mobile") {
                QStringList path = selected_item->path_list("name");

                // Remove the empty element, if it exists (this can be the root, it has
                // no name)    // Убирается пустой элемент, если он есть (это может быть
                // корень, у него нет названия)
                int emptyStringIndex = path.indexOf("");
                path.removeAt(emptyStringIndex);

                // find_object<RecordScreen>(table_screen_singleton_name)
                globalparameters.main_window()
                    ->vtab_record()
                    ->activated_browser()
                    ->record_screen()
                    ->tree_path(path.join(" > "));
            }
            // Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
            resizeColumnToContents(0);

            // if(result_item->field("url") != "")
            // globalparameters.entrance()->activate(result_item);
        }
// else {      //        if(result_item->field("url") !=
// web::Browser::_defaulthome)
////        if(result_item->field("url") != "")
///globalparameters.entrance()->activate(result_item);
//// Переключаются окна (используется для мобильного интерфейса)
////        globalparameters.window_switcher()->switchFromTreeToRecordtable();
// }

#ifdef _with_record_table // deprecated, just for transform old data
        _know_model_board->record_to_item();
#endif

        // if(selected_item->field("url") != "")// logic error, root_item can not
        // get activate!!!
        // item_bind(selected_item, std::bind(&TreeScreen::view_paste_child, this
        // , TreeIndex(soure_model, selected_item->parent(), 0)    //
        // std::placeholders::_1
        // , std::placeholders::_2, std::placeholders::_3))->activate();
    }
}

#endif

std::pair<boost::intrusive_ptr<i_t>, boost::intrusive_ptr<i_t>>
tv_t::cursor_focus(
    boost::intrusive_ptr<i_t> to_be_contained)
{ // _new_session_root_item
    auto source_model_ = [&]() { return source_model(); };
    auto current_root_ = source_model()->root_item();
    auto current_item_ = current_item();
    auto absolute_root_ = _know_model_board->root_item();
    auto current_browser = current_item_->page()->browser();
    if (!current_root_ && absolute_root_)
        intercept(absolute_root_); // be merged?
    assert(current_root_);
    assert(absolute_root_);
    //
    std::pair<boost::intrusive_ptr<i_t>, boost::intrusive_ptr<i_t>> result{
        current_root_, current_item_};
    //
    auto cursor_follow_up_impl = [&](void) -> std::pair<
                                               boost::intrusive_ptr<i_t>,
                                               boost::intrusive_ptr<i_t>> { // std::pair<boost::intrusive_ptr<TreeItem>,
        // boost::intrusive_ptr<TreeItem> >
        // tv_t::cursor_follow_up(void){
        // boost::intrusive_ptr<TreeItem>		current_root_(nullptr);
        boost::intrusive_ptr<i_t> current(nullptr);
        // auto _tree_screen = static_cast<TreeScreen *>(parent());
        std::function<bool(boost::intrusive_ptr<const i_t>)> _check_crypt_conflict =
            [&](boost::intrusive_ptr<const i_t> current_root) {
                bool _result = false;
                // Вначале все инструменты работы с веткой включаются
                QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
                while (i.hasNext()) {
                    i.next();
                    i.value()->setEnabled(true);
                }
                // Проверяется, происходит ли клик по зашифрованной ветке
                if (current_root->field<crypt_key>() == crypt_value(true)) {
                    // Если пароль доступа к зашифрованным данным не вводился в этой
                    // сессии
                    if (gl_paras->crypt_key().length() == 0) {
                        // Запрашивается пароль
                        Password password;
                        if (password.retrievePassword() == false) {
                            // Устанавливаем пустые данные для отображения таблицы конечных
                            // записей

                            // find_object<RecordController>("table_screen_controller")

                            // current_root->unique_page()->view()->record_ctrl()->source_model()->clear();

                            // _know_branch
                            // current_treemodel
                            source_model()
                                ->clear(); // root_item(boost::intrusive_ptr<TreeItem>(new
                                           // TreeItem(QMap<QString, QString>(), nullptr)));

                            // init_source_model(
                            // nullptr
                            // , current_root->unique_page()->view()->record_ctrl()
                            // ,
                            // current_root->unique_page()->view()->record_ctrl()->record_screen()
                            // , globalparameters.mainwindow()
                            // , globalparameters.meta_editor()
                            //// ,
                            ///result_item->unique_page()->view()->record_ctrl()->record_screen()
                            // );

                            // reset_tabledata(nullptr);

                            // Все инструменты работы с веткой отключаются
                            QMapIterator<QString, QAction*> i(_tree_screen->_actionlist);
                            while (i.hasNext()) {
                                i.next();
                                i.value()->setEnabled(false);
                            }
                            // return current_root; // Программа дальше не идет, доделать...
                        } else
                            _result = true;
                    }
                }
                return _result;
            };

        // Сохраняется текст в окне редактирования в соответсвующий файл
        auto rs = dynamic_cast<rs_t*>(gl_paras->vtab_record()->currentWidget());
        if (rs)
            rs->blogger()->save_text_context();
        // auto	source_model_	= [&](){return source_model();};
        // auto	current_item_	= current_item();
        int origin_count = source_model_()->count_records_all();
        // current_root_ = source_model_()->root_item();

        auto root_up_input = [&](boost::intrusive_ptr<i_t>& root_item) {
            assert(root_item);
            boost::intrusive_ptr<i_t> result(root_item);
            root_item = root_item->parent();
            if (root_item &&
                !_check_crypt_conflict(root_item)) { // && _root_item->parent() !=
                                                     // _know_model_board->root_item()
                result = root_item;
                if (0 == root_item->count_direct())
                    tree_empty_controll(); // boost::intrusive_ptr<TreeIndex>
                                           // tree_absolute_index = [&]
                                           // {boost::intrusive_ptr<TreeIndex> tree_index;
                                           // try{tree_index = new
                                           // TreeIndex(current_model, _root_up); }
                                           // catch(std::exception &e) {throw e; } return
                                           // tree_index; } ();
                intercept(root_item);      // TreeIndex::instance(current_model, _root_up,
                                           // _root_up->parent())
                if (!TreeIndex::is_ancestor_of(current_item_->parent(), root_item))
                    current_item_ =
                        current_item_
                            ->parent(); // if(!
                                        // current_item_->parent()->path_list().contains(root_item->field<id_type>()))
                select_as_current(TreeIndex::item_require_treeindex([&] { return _know_root; }, current_item_));

                // Получаем указатель на данные таблицы конечных записей

                assert(source_model()->count_records_all() >= origin_count);
                // Set the text path to the final table entries for the mobile interface
                // options    // Устанавливается текстовый путь в таблице конечных
                // записей для мобильного варианта интерфейса
                if (appconfig->interface_mode() == "mobile") {
                    QStringList path = root_item->path_list<name_key>(); //"name"

                    // Remove the empty element, if it exists (this can be the root, it
                    // has no name)    // Убирается пустой элемент, если он есть (это
                    // может быть корень, у него нет названия)
                    int empty_string_index = path.indexOf("");
                    path.removeAt(empty_string_index);

                    // find_object<RecordScreen>(table_screen_singleton_name)
                    current_browser->record_screen()->tree_path(path.join(" > "));
                }
                // Ширина колонки дерева устанавливается так чтоб всегда вмещались
                // данные
                resizeColumnToContents(0);
            }
            assert(result);

            return result;
        };
        if (current_item_) {
            auto parent_ = current_item_->parent();
            if (parent_ && parent_ != current_root_ &&
                !TreeIndex::is_ancestor_of(
                    parent_,
                    current_root_)) { // current_item_->parent()->path_list().size() >
                // root_to_update->path_list().size()
                current = parent_;

                select_as_current(
                    TreeIndex::item_require_treeindex([&] { return _know_root; }, current));
            } else if (parent_ == current_root_ && current_root_)
                current_root_ = root_up_input(current_root_);
        } else if (current_root_->count_direct() == current_root_->count_children_recursive() && current_root_)
            current_root_ = root_up_input(current_root_);
        // auto try_root_up    = current_model()->root_item()->parent();
        // boost::intrusive_ptr<TreeItem> _root_up = try_root_up ? try_root_up :
        // current_model()->root_item();
        // assert(_root_up);

        // if(!_check_crypt_conflict(_root_up)) {
        // resetup_model(_root_item);  //
        // _know_branch->intercept(current_root->parent());     //? dangerous to do
        // this!!!

        // if(_root_up != _know_model_board->root_item()) {

        // if(!_check_crypt_conflict(_root_up)) {

        //// Получаем указатель на данные таблицы конечных записей

        // assert(source_model()->count_records_all() >= origin_count);

        //// Set the text path to the final table entries for the mobile interface
        ///options    // Устанавливается текстовый путь в таблице конечных записей
        ///для мобильного варианта интерфейса
        // if(appconfig->interface_mode() == "mobile") {

        // QStringList path = _root_up->path_absolute_as_name();

        //// Remove the empty element, if it exists (this can be the root, it has no
        ///name)    // Убирается пустой элемент, если он есть (это может быть
        ///корень, у него нет названия)
        // int emptyStringIndex = path.indexOf("");
        // path.removeAt(emptyStringIndex);

        ////            find_object<RecordScreen>(table_screen_singleton_name)
        // globalparameters.entrance()->activated_browser()->record_screen()->tree_path(path.join("
        // > "));

        // }

        //// Ширина колонки дерева устанавливается так чтоб всегда вмещались данные
        // resizeColumnToContents(0);
        // }

        // } else if(current_item->parent()) { // _root_item ==
        // _know_model_board->root_item()
        // current_item = current_item->parent();

        // if(current_item->parent()) {
        // _root_up = current_item->parent();

        // if(!_check_crypt_conflict(_root_up)) {
        // resetup_model(_root_up);  //
        // _know_branch->intercept(know_root()->root_item());
        // }
        // } else {
        // _root_up = _know_model_board->root_item();

        // if(!_check_crypt_conflict(_root_up)) {
        // resetup_model(_root_up);  //
        // _know_branch->intercept(know_root()->root_item());
        // }
        // }

        // select_as_current(current_item);

        // } else {    // _root_item == _know_model_board->root_item() &&
        // (current_item->parent() == _know_model_board->root_item())?
        // if(!_check_crypt_conflict(_root_up)) {
        // _root_up = cursor_follow_up_to_root();
        // }
        // }

        // enable_up_action();
        // }

        _tree_screen->enable_up_action();
        assert(current_root_);

        return {current_root_, current_item()}; // current_item();	// root_updated;
    };
    if (!to_be_contained)
        result = cursor_follow_up_impl();
    else if (to_be_contained != absolute_root_) {
        while (current_root_ &&
            !TreeIndex::
                is_ancestor_of(
                    current_root_,
                    to_be_contained)) { // while(TreeIndex::is_ancestor_of([&]
                                        // {return _know_root;}, to_be_contained,
                                        // current_item_)){		//
                                        // current_root// &&
                                        // _new_session_root_item->id() !=
                                        // _current_item->id()		//
                                        // current_root->id()
            // reset();
            // source_model(_new_session_root_item->parent());
            // current_root =
            result = cursor_follow_up_impl(); // current_item();
            current_root_ = result.first;
            current_item_ = result.second;
            // }
        }
        assert(static_cast<QModelIndex>(source_model()->index(result.second)).isValid());
        assert(TreeIndex::is_ancestor_of(current_root_, to_be_contained));
        // auto result = _new_session_root_item;
        // boost::intrusive_ptr<TreeIndex> tree_index = [&]
        // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
        // TreeIndex([&] {return _know_root;}, result->parent(),
        // result->parent()->sibling_order([&](boost::intrusive_ptr<const Linker>
        // il) {return il == result->linker() && il->host() == result &&
        // result->parent() == il->host_parent();}));} catch(std::exception &e)
        // {throw e;} return tree_index;}();
        // select_as_current(tree_index);
    } else { // to_be_contained != absolute_root
        // boost::intrusive_ptr<TreeItem> tv_t::cursor_follow_up(void){
        // boost::intrusive_ptr<TreeItem>		absolute_root;
        // auto source_model_ = [&](){return source_model();};
        // auto	current_item_	= current_item();
        // auto _tree_screen = static_cast<TreeScreen *>(parent());
        // absolute_root = _know_model_board->root_item();
        if (source_model_()->root_item() != absolute_root_) {
            // boost::intrusive_ptr<TreeIndex> tree_absolute_index = [&]
            // {boost::intrusive_ptr<TreeIndex> tree_index; try{tree_index = new
            // TreeIndex(_source_model, absolute_root); } catch(std::exception &e)
            // {throw e; } return tree_index; } ();
            source_model_()->intercept(absolute_root_);
        } // _know_branch->intercept(know_root()->root_item());
        if (current_item_) {
            while (current_item_->parent()->parent())
                current_item_ = current_item_->parent();
            select_as_current(
                TreeIndex::item_require_treeindex(source_model_, current_item_));
        }
        _tree_screen->enable_up_action();

        result.first = absolute_root_; // return absolute_root;
        result.second = current_item_;
        // }
    }
    return result; // _new_session_root_item;
}

boost::intrusive_ptr<i_t> tv_t::intercept(
    boost::intrusive_ptr<i_t> result)
{ // boost::intrusive_ptr<TreeIndex>
    // modelindex){   // QString id
    // auto prepared =
    // know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id)));
    // auto result = modelindex->host();					// _item;    //
    // _know_model_board->item([&](boost::intrusive_ptr<const TreeItem> it)
    // {return it->id() == id;});     //
    // _know_model_board->index(KnowModel::delegater(id))
    ////    auto absolute_source_model = []() {return
    ///globalparameters.tree_screen()->know_model_board();};
    ////    assert(_item == absolute_source_model()->root_item() ||
    ///absolute_source_model()->item([&](boost::intrusive_ptr<const TreeItem> it)
    ///{return it->id() == _item->id();}));

    ////    // _know_branch->intercept(prepared);
    /////item(_know_branch->index(prepared));

    // resetup_model(result);   //    return
    // _know_branch->intercept(know_root_holder::know_root()->item(know_root_holder::know_root()->index(TreeKnowModel::delegater(id))));

    // reset();
    //	gl_paras->main_window()->setDisabled(true);
    // sychronize();
    know_model_save(); //std::thread(&tkm_t::save, _know_model_board).join(); //.hardware_concurrency(); //.detach(); // __know_model_board->save();
    //	gl_paras->main_window()->setEnabled(true);
    // if(_know_root && is_owner()) {
    // delete _know_root;  // dangerous!
    // _know_root = nullptr;
    // }

    // reset();
    // Модель подключется к виду
    _know_root->intercept(result); // source_model(result);
    reset();
    setModel(_know_root);
    if (result != _know_model_board->root_item())
        result = cursor_focus(result).first;
    return result;
}

void tv_t::synchronized(bool _synchronized)
{
    _know_model_board->synchronized(_synchronized);
}

bool tv_t::synchronized()
{
    return _know_model_board->synchronized();
}

// void KnowView::zoom(double scale_)
// {
// double tmp = scale_ / 1;
// double reset = 1 / _previous_scale;

// if(scale_ == _previous_scale) {
// return;
// }

////    scale(reset, reset);
// resize(sizeHint() * reset);

////    scale(tmp, tmp);
// _previous_scale = scale_;

// resize(sizeHint() * tmp);
// updateGeometry();
// update();
// emit zoomChanged();
// }

// *  Contact List Widget
// *  Copyright (C) 2011  Martin Klapetek <martin.klapetek@gmail.com>
// *
// *  This library is free software; you can redistribute it and/or
// *  modify it under the terms of the GNU Lesser General Public
// *  License as published by the Free Software Foundation; either
// *  version 2.1 of the License, or (at your option) any later version.
// *
// *  This library is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// *  Lesser General Public License for more details.
// *
// *  You should have received a copy of the GNU Lesser General Public
// *  License along with this library; if not, write to the Free Software
// *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA

// #include "contact-list-widget.h"
// #include "contact-list-widget_p.h"

// #include <TelepathyQt/AccountManager>
// #include <TelepathyQt/PendingChannelRequest>
// #include <TelepathyQt/PendingReady>

// #include <KTp/Models/accounts-model.h>
// #include <KTp/Models/groups-model.h>
// #include <KTp/Models/accounts-filter-model.h>
// #include <KTp/Models/contact-model-item.h>
// #include <KTp/Models/accounts-model-item.h>
// #include <KTp/Models/groups-model-item.h>

// #include <KGlobal>
// #include <KSharedConfig>
// #include <KConfigGroup>
// #include <KDebug>
// #include <KMessageBox>
// #include <KLocalizedString>
// #include <KDialog>
// #include <KFileDialog>
// #include <KSettings/Dialog>

// #include <QHeaderView>
// #include <QLabel>
// #include <QApplication>
// #include <QDropEvent>
// #include <QDragMoveEvent>
// #include <QDragEnterEvent>
// #include <QDragLeaveEvent>
// #include <QPainter>
// #include <QPixmap>

// #include "contact-delegate.h"
// #include "contact-delegate-compact.h"
// #include "contact-overlays.h"

// #define PREFERRED_TEXTCHAT_HANDLER
// "org.freedesktop.Telepathy.Client.KTp.TextUi"
// #define PREFERRED_FILETRANSFER_HANDLER
// "org.freedesktop.Telepathy.Client.KTp.FileTransfer"
// #define PREFERRED_AUDIO_VIDEO_HANDLER
// "org.freedesktop.Telepathy.Client.KTp.CallUi"
// #define PREFERRED_RFB_HANDLER
// "org.freedesktop.Telepathy.Client.krfb_rfb_handler"

// ContactListWidget::ContactListWidget(QWidget *parent)
// : QTreeView(parent)
// , d_ptr(new ContactListWidgetPrivate){
// Q_D(ContactListWidget);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");

// d->delegate		= new ContactDelegate(this);
// d->compactDelegate	= new ContactDelegateCompact(this);

// d->model		= new AccountsModel(this);
// d->groupsModel	= new GroupsModel(d->model, this);
// d->modelFilter	= new AccountsFilterModel(this);
// d->modelFilter->setDynamicSortFilter(true);
// d->modelFilter->setSortRole(Qt::DisplayRole);

// setModel(d->modelFilter);
// setSortingEnabled(true);
// sortByColumn(0, Qt::AscendingOrder);

// connect(d->modelFilter, SIGNAL(rowsInserted(QModelIndex, int, int))
// , this, SLOT(onNewGroupModelItemsInserted(QModelIndex, int, int)));

// connect(d->groupsModel, SIGNAL(operationFinished(Tp::PendingOperation *))
// , this, SIGNAL(genericOperationFinished(Tp::PendingOperation *)));

// header()->hide();
// setRootIsDecorated(false);
// setSortingEnabled(true);
// setContextMenuPolicy(Qt::CustomContextMenu);
// setIndentation(0);
// setMouseTracking(true);
// setExpandsOnDoubleClick(false);	// the expanding/collapsing is handled
// manually
// setDragEnabled(true);
// viewport()->setAcceptDrops(true);
// setDropIndicatorShown(true);
// if(guiConfigGroup.readEntry("selected_delegate", "compact") ==
// QLatin1String("compact"))setItemDelegate(d->compactDelegate);
// else setItemDelegate(d->delegate);
// addOverlayButtons();
// emit enableOverlays(guiConfigGroup.readEntry("selected_delegate", "compact")
// == QLatin1String("full"));

// connect(this, SIGNAL(clicked(QModelIndex))
// , this, SLOT(onContactListClicked(QModelIndex)));

// connect(this, SIGNAL(doubleClicked(QModelIndex))
// , this, SLOT(onContactListDoubleClicked(QModelIndex)));

// connect(d->delegate, SIGNAL(repaintItem(QModelIndex))
// , this->viewport(), SLOT(repaint()));// update(QModelIndex)
// }

// ContactListWidget::~ContactListWidget(){
// delete d_ptr;
// }

// void ContactListWidget:: setAccountManager(const Tp::AccountManagerPtr
// &accountManager){
// Q_D(ContactListWidget);
// d->model->setAccountManager(accountManager);

// connect(accountManager.data(), SIGNAL(newAccount(Tp::AccountPtr))
// , this, SLOT(onNewAccountAdded(Tp::AccountPtr)));

// QList<Tp::AccountPtr> accounts = accountManager->allAccounts();
// if(accounts.count() == 0){
// if(KMessageBox::questionYesNo(this
// , i18n("You have no IM accounts configured. Would you like to do that now?")
// , i18n("No Accounts Found")) == KMessageBox::Yes){
// showSettingsKCM();
// }
// }
// foreach(const Tp::AccountPtr & account, accounts){
// onNewAccountAdded(account);
// }
// }

// AccountsModel *ContactListWidget:: accountsModel(){
// Q_D(ContactListWidget);

// return d->model;
// }

// void ContactListWidget:: showSettingsKCM(){
// KSettings::Dialog *dialog = new KSettings::Dialog(this);

// KService::Ptr tpAccKcm = KService::serviceByDesktopName("kcm_ktp_accounts");
// if(! tpAccKcm){
// KMessageBox::error(this
// , i18n("It appears you do not have the IM Accounts control module installed.
// Please install telepathy-accounts-kcm package.")
// , i18n("IM Accounts KCM Plugin Is Not Installed"));
// }
// dialog->addModule("kcm_ktp_accounts");
// dialog->addModule("kcm_ktp_integration_module");

// dialog->setAttribute(Qt::WA_DeleteOnClose);
// dialog->exec();
// }

// void ContactListWidget:: onNewAccountAdded(const Tp::AccountPtr &account){
// Q_ASSERT(account->isReady(Tp::Account::FeatureCore));

// connect(account.data()
// , SIGNAL(connectionStatusChanged(Tp::ConnectionStatus))
// , this, SLOT(onAccountConnectionStatusChanged(Tp::ConnectionStatus)));

//// FIXME get rid of that thing already
////     m_avatarButton->loadAvatar(account);
////     KSharedConfigPtr config = KGlobal::config();
////     KConfigGroup avatarGroup(config, "Avatar");
////     if (avatarGroup.readEntry("method", QString()) ==
///QLatin1String("account")) {
////         //this also updates the avatar if it was changed somewhere else
////
///m_avatarButton->selectAvatarFromAccount(avatarGroup.readEntry("source",
///QString()));
////     }
// }

// void ContactListWidget:: onContactListClicked(const QModelIndex &index){
// if(! index.isValid())return;
// if(  index.data(AccountsModel::ItemRole).userType() ==
// qMetaTypeId<AccountsModelItem *>()
// || index.data(AccountsModel::ItemRole).userType() ==
// qMetaTypeId<GroupsModelItem *>()){
// KSharedConfigPtr	config		=
// KSharedConfig::openConfig(QLatin1String("ktelepathyrc"));
// KConfigGroup		groupsConfig	= config->group("GroupsState");
// if(isExpanded(index)){
// collapse(index);
// groupsConfig.writeEntry(index.data(AccountsModel::IdRole).toString(), false);
// }else{
// expand(index);
// groupsConfig.writeEntry(index.data(AccountsModel::IdRole).toString(), true);
// }
// groupsConfig.config()->sync();
// }
// }

// void ContactListWidget:: onContactListDoubleClicked(const QModelIndex
// &index){
// if(! index.isValid())return;
// if(index.data(AccountsModel::ItemRole).userType() ==
// qMetaTypeId<ContactModelItem *>()){
// kDebug() << "Text chat requested for index" << index;
// startTextChannel(index.data(AccountsModel::ItemRole).value<ContactModelItem
// *>());
// }
// }

// void ContactListWidget:: addOverlayButtons(){
// Q_D(ContactListWidget);

// TextChannelContactOverlay	*textOverlay	= new
// TextChannelContactOverlay(this);
// AudioChannelContactOverlay	*audioOverlay	= new
// AudioChannelContactOverlay(this);
// VideoChannelContactOverlay	*videoOverlay	= new
// VideoChannelContactOverlay(this);

// FileTransferContactOverlay		*fileOverlay	= new
// FileTransferContactOverlay(this);
// DesktopSharingContactOverlay	*desktopOverlay = new
// DesktopSharingContactOverlay(this);

// d->delegate->installOverlay(textOverlay);
// d->delegate->installOverlay(audioOverlay);
// d->delegate->installOverlay(videoOverlay);
// d->delegate->installOverlay(fileOverlay);
// d->delegate->installOverlay(desktopOverlay);

// d->delegate->setViewOnAllOverlays(this);
// d->delegate->setAllOverlaysActive(true);

// connect(textOverlay, SIGNAL(overlayActivated(QModelIndex))
// , d->delegate, SLOT(hideStatusMessageSlot(QModelIndex)));

// connect(textOverlay, SIGNAL(overlayHidden())
// , d->delegate, SLOT(reshowStatusMessageSlot()));

// connect(textOverlay, SIGNAL(activated(ContactModelItem *))
// , this, SLOT(startTextChannel(ContactModelItem *)));

// connect(fileOverlay, SIGNAL(activated(ContactModelItem *))
// , this, SLOT(startFileTransferChannel(ContactModelItem *)));

// connect(audioOverlay, SIGNAL(activated(ContactModelItem *))
// , this, SLOT(startAudioChannel(ContactModelItem *)));

// connect(videoOverlay, SIGNAL(activated(ContactModelItem *))
// , this, SLOT(startVideoChannel(ContactModelItem *)));

// connect(desktopOverlay, SIGNAL(activated(ContactModelItem *))
// , this, SLOT(startDesktopSharing(ContactModelItem *)));

// connect(this, SIGNAL(enableOverlays(bool))
// , textOverlay, SLOT(setActive(bool)));

// connect(this, SIGNAL(enableOverlays(bool))
// , audioOverlay, SLOT(setActive(bool)));

// connect(this, SIGNAL(enableOverlays(bool))
// , videoOverlay, SLOT(setActive(bool)));

// connect(this, SIGNAL(enableOverlays(bool))
// , fileOverlay, SLOT(setActive(bool)));

// connect(this, SIGNAL(enableOverlays(bool))
// , desktopOverlay, SLOT(setActive(bool)));
// }

// void ContactListWidget:: toggleGroups(bool show){
// Q_D(ContactListWidget);
// if(show)d->modelFilter->setSourceModel(d->groupsModel);
// else d->modelFilter->setSourceModel(d->model);
// }

// void ContactListWidget:: toggleOfflineContacts(bool show){
// Q_D(ContactListWidget);

// d->showOffline = show;
// d->modelFilter->setPresenceTypeFilterFlags(show ?
// AccountsFilterModel::DoNotFilterByPresence :
// AccountsFilterModel::ShowOnlyConnected);
// }

// void ContactListWidget:: toggleSortByPresence(bool sort){
// Q_D(ContactListWidget);

// d->modelFilter->setSortMode(sort ? AccountsFilterModel::SortByPresence :
// AccountsFilterModel::DoNotSort);
// }

// void ContactListWidget:: startTextChannel(ContactModelItem *contactItem){
// Q_D(ContactListWidget);

// Q_ASSERT(contactItem);
// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting chat for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// Tp::ChannelRequestHints hints;
// hints.setHint("org.freedesktop.Telepathy.ChannelRequest",
// "DelegateToPreferredHandler", QVariant(true));

// Tp::PendingChannelRequest *channelRequest = account->ensureTextChat(contact
// , QDateTime::currentDateTime()
// , PREFERRED_TEXTCHAT_HANDLER
// , hints);
// connect(channelRequest, SIGNAL(finished(Tp::PendingOperation *))
// , SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }

// void ContactListWidget:: startAudioChannel(ContactModelItem *contactItem){
// Q_D(ContactListWidget);

// Q_ASSERT(contactItem);
// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting audio for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// Tp::PendingChannelRequest *channelRequest = account->ensureAudioCall(contact
// , QLatin1String("audio"), QDateTime::currentDateTime(),
// PREFERRED_AUDIO_VIDEO_HANDLER);

// connect(channelRequest, SIGNAL(finished(Tp::PendingOperation *))
// , SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }

// void ContactListWidget:: startVideoChannel(ContactModelItem *contactItem){
// Q_D(ContactListWidget);

// Q_ASSERT(contactItem);
// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting video for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// Tp::PendingChannelRequest *channelRequest =
// account->ensureAudioVideoCall(contact
// , QLatin1String("audio"), QLatin1String("video")
// , QDateTime::currentDateTime(), PREFERRED_AUDIO_VIDEO_HANDLER);

// connect(channelRequest, SIGNAL(finished(Tp::PendingOperation *))
// , SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }

// void ContactListWidget:: startDesktopSharing(ContactModelItem *contactItem){
// Q_D(ContactListWidget);

// Q_ASSERT(contactItem);
// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting desktop sharing for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// Tp::PendingChannelRequest *channelRequest = account->createStreamTube(contact
// , QLatin1String("rfb")
// , QDateTime::currentDateTime()
// , PREFERRED_RFB_HANDLER);

// connect(channelRequest, SIGNAL(finished(Tp::PendingOperation *))
// , SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }

// void ContactListWidget:: startFileTransferChannel(ContactModelItem
// *contactItem){
// Q_D(ContactListWidget);

// Q_ASSERT(contactItem);
// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting file transfer for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// QStringList filenames =
// KFileDialog::getOpenFileNames(KUrl("kfiledialog:///FileTransferLastDirectory")
// , QString()
// , this
// , i18n("Choose files to send to %1", contact->alias()));
// if(filenames.isEmpty())	// User hit cancel button
// return;
// QDateTime now = QDateTime::currentDateTime();

// requestFileTransferChannels(account, contact, filenames, now);
// }

// void ContactListWidget:: requestFileTransferChannels(const Tp::AccountPtr
// &account, const Tp::ContactPtr &contact, const QStringList &filenames, const
// QDateTime &userActionTime){
// Q_FOREACH(const QString &filename, filenames){
// kDebug() << "Filename:" << filename;
// kDebug() << "Content type:" <<
// KMimeType::findByFileContent(filename)->name();

// Tp::FileTransferChannelCreationProperties fileTransferProperties(filename
// , KMimeType::findByFileContent(filename)->name());

// Tp::PendingChannelRequest *channelRequest =
// account->createFileTransfer(contact
// , fileTransferProperties
// , userActionTime
// , PREFERRED_FILETRANSFER_HANDLER);

// connect(channelRequest, SIGNAL(finished(Tp::PendingOperation *))
// , SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }
// }

// void ContactListWidget:: onNewGroupModelItemsInserted(const QModelIndex
// &index, int start, int end){
// Q_UNUSED(start);
// Q_UNUSED(end);
// if(! index.isValid())return;
//// if there is no parent, we deal with top-level item that we want to
///expand/collapse, ie. group or account
// if(! index.parent().isValid()){
// KSharedConfigPtr	config		=
// KSharedConfig::openConfig(QLatin1String("ktelepathyrc"));
// KConfigGroup		groupsConfig	= config->group("GroupsState");
//// we're probably dealing with group item, so let's check if it is expanded
///first
// if(! isExpanded(index)){
//// if it's not expanded, check the config if we should expand it or not
// if(groupsConfig.readEntry(index.data(AccountsModel::IdRole).toString(),
// false))expand(index);
// }
// }
// }

// void ContactListWidget:: onSwitchToFullView(){
// Q_D(ContactListWidget);

// setItemDelegate(d->delegate);
// doItemsLayout();

// emit enableOverlays(true);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");
// guiConfigGroup.writeEntry("selected_delegate", "full");
// guiConfigGroup.config()->sync();
// }

// void ContactListWidget:: onSwitchToCompactView(){
// Q_D(ContactListWidget);

// setItemDelegate(d->compactDelegate);
// doItemsLayout();

// emit enableOverlays(false);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");
// guiConfigGroup.writeEntry("selected_delegate", "compact");
// guiConfigGroup.config()->sync();
// }

// void ContactListWidget:: onShowAllContacts(){
// Q_D(ContactListWidget);

// d->modelFilter->setSubscriptionStateFilterFlags(AccountsFilterModel::DoNotFilterBySubscription);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");
// guiConfigGroup.writeEntry("shown_contacts", "all");
// guiConfigGroup.config()->sync();
// }

// void ContactListWidget:: onShowUnblockedContacts(){
// Q_D(ContactListWidget);

// d->modelFilter->setSubscriptionStateFilterFlags(AccountsFilterModel::HideBlocked);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");
// guiConfigGroup.writeEntry("shown_contacts", "unblocked");
// guiConfigGroup.config()->sync();
// }

// void ContactListWidget:: onShowBlockedContacts(){
// Q_D(ContactListWidget);

// d->modelFilter->setSubscriptionStateFilterFlags(AccountsFilterModel::ShowOnlyBlocked);

// KSharedConfigPtr	config = KGlobal::config();
// KConfigGroup	guiConfigGroup(config, "GUI");
// guiConfigGroup.writeEntry("shown_contacts", "blocked");
// guiConfigGroup.config()->sync();
// }

// void ContactListWidget:: setFilterString(const QString &string){
// Q_D(ContactListWidget);

// d->modelFilter->setPresenceTypeFilterFlags(string.isEmpty() && !
// d->showOffline ? AccountsFilterModel::ShowOnlyConnected :
// AccountsFilterModel::DoNotFilterByPresence);
// d->modelFilter->setDisplayNameFilterString(string);
// }

// void ContactListWidget:: setDropIndicatorRect(const QRect &rect){
// Q_D(ContactListWidget);
// if(d->dropIndicatorRect != rect){
// d->dropIndicatorRect = rect;
// viewport()->update();
// }
// }

// bool ContactListWidget:: event(QEvent *event){
// Q_D(ContactListWidget);
// if(event->type() == QEvent::Leave && d->delegate){
// d->delegate->reshowStatusMessageSlot();

// return true;
// }
// return QTreeView::event(event);
// }

// void ContactListWidget:: mousePressEvent(QMouseEvent *event){
// Q_D(ContactListWidget);

// QTreeView::mousePressEvent(event);

// QModelIndex index = indexAt(event->pos());
// d->shouldDrag = false;
//// if no contact, no drag
// if(! index.data(AccountsModel::ItemRole).canConvert<ContactModelItem
// *>())return;
// if(event->button() == Qt::LeftButton){
// d->shouldDrag		= true;
// d->dragStartPosition	= event->pos();
// }
// }

// void ContactListWidget:: mouseMoveEvent(QMouseEvent *event){
// Q_D(ContactListWidget);

// QTreeView::mouseMoveEvent(event);

// QModelIndex index = indexAt(event->pos());
// if(! (event->buttons() & Qt::LeftButton))return;
// if(! d->shouldDrag)return;
// if((event->pos() - d->dragStartPosition).manhattanLength() <
// QApplication::startDragDistance())return;
// QMimeData	*mimeData = new QMimeData;
// QByteArray	encodedData;
// QDataStream stream(&encodedData, QIODevice::WriteOnly);
// if(index.isValid()){
// ContactModelItem *contactItem =
// index.data(AccountsModel::ItemRole).value<ContactModelItem *>();
//// We put a contact ID and its account ID to the stream, so we can later
///recreate the contact using AccountsModel
// stream << contactItem->contact().data()->id() <<
// d->model->accountForContactItem(contactItem).data()->objectPath();
// }
// mimeData->setData("application/vnd.telepathy.contact", encodedData);
// QPixmap dragIndicator = QPixmap::grabWidget(this,
// visualRect(index).adjusted(3, 3, 3, 3));

// QDrag *drag = new QDrag(this);
// drag->setMimeData(mimeData);
// drag->setPixmap(dragIndicator);

// drag->exec(Qt::CopyAction);
// }

// void ContactListWidget:: dropEvent(QDropEvent *event){
// Q_D(ContactListWidget);

// QModelIndex index = indexAt(event->pos());
// if(event->mimeData()->hasUrls()){
// kDebug() << "It's a file!";

// ContactModelItem *contactItem =
// index.data(AccountsModel::ItemRole).value<ContactModelItem *>();
// Q_ASSERT(contactItem);

// Tp::ContactPtr contact = contactItem->contact();

// kDebug() << "Requesting file transfer for contact" << contact->alias();

// Tp::AccountPtr account = d->model->accountForContactItem(contactItem);

// QStringList filenames;
// Q_FOREACH(const QUrl &url, event->mimeData()->urls()){
// filenames << url.toLocalFile();
// }
// if(filenames.isEmpty())return;
// QDateTime now = QDateTime::currentDateTime();
// requestFileTransferChannels(account, contact, filenames, now);

// event->acceptProposedAction();
// }else if(event->mimeData()->hasFormat("application/vnd.telepathy.contact")){
// kDebug() << "It's a contact!";

// QByteArray			encodedData =
// event->mimeData()->data("application/vnd.telepathy.contact");
// QDataStream			stream(&encodedData, QIODevice::ReadOnly);
// QList<ContactModelItem *>	contacts;
// while(! stream.atEnd()){
// QString	contact;
// QString	account;

//// get contact and account out of the stream
// stream >> contact >> account;

// Tp::AccountPtr accountPtr = d->model->accountPtrForPath(account);

//// casted pointer is checked below, before first use
// contacts.append(qobject_cast<ContactModelItem
// *>(d->model->contactItemForId(accountPtr->uniqueIdentifier(), contact)));
// }
// Q_FOREACH(ContactModelItem * contact, contacts){
// Q_ASSERT(contact);
// QString group;
// if(index.data(AccountsModel::ItemRole).canConvert<GroupsModelItem *>()){
//// contact is dropped on a group, so take it's name
// group = index.data(GroupsModel::GroupNameRole).toString();
// }else{
//// contact is dropped on another contact, so take it's parents (group) name
// group = index.parent().data(GroupsModel::GroupNameRole).toString();
// }
// kDebug() << contact->contact().data()->alias() << "added to group" << group;
// if(! group.isEmpty()){
// Tp::PendingOperation *op = contact->contact().data()->addToGroup(group);

// connect(op, SIGNAL(finished(Tp::PendingOperation *))
// , this, SIGNAL(genericOperationFinished(Tp::PendingOperation *)));
// }
// }
// event->acceptProposedAction();
// }else event->ignore();
// setDropIndicatorRect(QRect());
// }

// void ContactListWidget:: dragEnterEvent(QDragEnterEvent *event){
// if(event->mimeData()->hasUrls()){
// bool accepted = true;
//// check if one of the urls isn't a local file and abort if so
// Q_FOREACH(const QUrl &url, event->mimeData()->urls()){
// if(! QFileInfo(url.toLocalFile()).isFile())accepted = false;
// }
// if(accepted)event->acceptProposedAction();
// else event->ignore();
// }else
// if(event->mimeData()->hasFormat("application/vnd.telepathy.contact"))event->acceptProposedAction();
// else event->ignore();
// }

// void ContactListWidget:: dragMoveEvent(QDragMoveEvent *event){
// Q_D(ContactListWidget);

// QModelIndex index = indexAt(event->pos());
// setDropIndicatorRect(QRect());
//// urls can be dropped on a contact with file transfer capability,
//// contacts can be dropped either on a group or on another contact if
///GroupsModel is used
// if(event->mimeData()->hasUrls() &&
// index.data(AccountsModel::FileTransferCapabilityRole).toBool()){
// event->acceptProposedAction();
// setDropIndicatorRect(visualRect(index));
// }else if(event->mimeData()->hasFormat("application/vnd.telepathy.contact") &&
// d->modelFilter->sourceModel() == d->groupsModel &&
// (index.data(AccountsModel::ItemRole).canConvert<GroupsModelItem *>() ||
// index.data(AccountsModel::ItemRole).canConvert<ContactModelItem *>())){
// event->acceptProposedAction();
// setDropIndicatorRect(visualRect(index));
// }else event->ignore();
// }

// void ContactListWidget:: dragLeaveEvent(QDragLeaveEvent *event){
// Q_UNUSED(event);
// setDropIndicatorRect(QRect());
// }

// void ContactListWidget:: paintEvent(QPaintEvent *event){
// Q_D(ContactListWidget);

// QTreeView::paintEvent(event);
// if(! d->dropIndicatorRect.isNull()){
// QStyleOption option;
// option.init(this);
// option.rect = d->dropIndicatorRect.adjusted(0, 0, - 1, - 1);
// QPainter painter(viewport());
// style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemDrop, &option,
// &painter, this);
// }
// }

// void ContactListWidget:: drawBranches(QPainter *painter, const QRect &rect,
// const QModelIndex &index) const {
// Q_UNUSED(painter);
// Q_UNUSED(rect);
// Q_UNUSED(index);

//// There is a 0px identation set in the constructor, with setIndentation(0).
//// Because of that, no branches are shown, so they should be disabled
///completely (overriding drawBranches).
//// Leaving branches enabled with 0px identation results in a 1px branch line
///on the left of all items,
//// which looks like an artifact.
// }
