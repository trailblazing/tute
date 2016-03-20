#ifndef _TREESCREEN_H_
#define _TREESCREEN_H_

#include <memory>
#include <QtGlobal>
#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QMenuBar>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include "utility/delegate.h"
#include "models/tree/TreeItem.h"
//#include "views/tree/KnowView.h"
//#include "views/main_window/MainWindow.h"
//#include "models/tree/KnowModel.h"
//#include "libraries/GlobalParameters.h"


class GlobalParameters;
extern QString get_unical_id(void);
extern GlobalParameters globalparameters;


class KnowModel;
class KnowView;
class ClipboardBranch;
class AppConfig;
class QMenuBar;
class QWidgetAction;
class ItemsFlat;
class TreeItem;
class TreeController;
class RecordController;
class TreeScreen;
class InfoFieldsEditor;

namespace  browser {
    class ToolbarSearch;
    class WebPage;
    class TabWidget;
}


struct know_root_holder {
protected:  //public:
    know_root_holder(const AppConfig &_appconfig, TreeScreen *_this);

    KnowModel *know_root() {return _know_root;}
    KnowModel const *know_root()const {return _know_root;}
    ~know_root_holder();
private:
    KnowModel   *_know_root;        // for tree screen
};

class TreeScreen
    : public QWidget    //    , public know_root_holder
{
    Q_OBJECT

public:
    TreeScreen(QString object_name, const AppConfig &_appconfig, QMenu *_filemenu, QMenu *_toolsmenu, QWidget *_parent = 0);
    virtual ~TreeScreen();

    QMap<QString, QAction *> _actionlist;

    //    TreeKnowModel const *know_root()const {return know_root_holder::know_root();}
    //    TreeKnowModel *know_root_modify() {return know_root_holder::know_root();}

    const KnowModel *know_model_board()const;   // {return _know_model_board;}
    void synchronized(bool _synchronized);  // {_know_model_board->synchronized(_synchronized);}
    bool sysynchronized();  // {return _know_model_board->synchronized();}
    //    std::shared_ptr<sd::_interface<sd::meta_info<void *>, RecordController *>>      reocrd_controller;  // for entrance
    //    std::shared_ptr<sd::_interface<sd::meta_info<void *>, browser::TabWidget *>>    tabmanager;         // for entrance

    boost::intrusive_ptr<TreeItem> intercept(QString id);
    //    boost::intrusive_ptr<TreeItem> synchronize();
    void knowtree_save(void);
    void knowtree_reload(void);

    void items_update_selected(void);
    void branchs_update_selected(void);

    int item_first_selected(void);



    QItemSelectionModel *selection_model(void);

    // Установка курсора на указанный элемент
    void cursor_to_index(QModelIndex index);

    void branch_update_on_screen(const QModelIndex &index);

    QMenu *buttonmenu() {return _menus_in_button;}

    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch);

    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, boost::intrusive_ptr<TreeItem> it, bool insert_sibling_branch, KnowModel *_current_model);
    std::vector<boost::intrusive_ptr<TreeItem>> branch_paste_new_children_only(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _blank_root);
    boost::intrusive_ptr<TreeItem> branch_paste_to_child(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _new_item);
    boost::intrusive_ptr<TreeItem> branch_paste(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _new_item);

    //    boost::intrusive_ptr<TreeItem> branch_add(QModelIndex _current_index, QString name, bool insert_sibling_branch, KnowModel *_current_model);
    //    template<bool insert_sibling_branch = true>
    boost::intrusive_ptr<TreeItem> branch_add_new(KnowModel *_current_model, QModelIndex _current_index, QString _name
                                              , std::function<boost::intrusive_ptr<TreeItem> (KnowModel *, QModelIndex, QString, QString)> _branch_add_impl);

    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch, std::shared_ptr<KnowModel> _current_model);

    //    TreeController *treecontroller() {return _tree_controller;}
    void index_invoke_in_current_branch(const QModelIndex &index);
    void index_invoke_in_sub_branch(const QModelIndex &index);
    void enable_up_action();
    //    TreeModelKnow *shadow_branch() {return _shadow_branch;}
    boost::intrusive_ptr<TreeItem> branch_cut(boost::intrusive_ptr<TreeItem> item);
    //    boost::intrusive_ptr<TreeItem> branch_paste(boost::intrusive_ptr<TreeItem> item, KnowModel *_current_know_branch);


    //    QStringList record_path(QString _record_id) {return know_root_holder::know_root()->record_path(_record_id);}
    //    bool is_item_valid(QStringList _path) {return know_root_holder::know_root()->is_item_valid(_path);}
    //    boost::intrusive_ptr<TreeItem> item(QStringList path) const;
    //    boost::intrusive_ptr<TreeItem> item(TreeModel::delegater _del) const;
    KnowView *tree_view() {return _tree_view;}
    void setup_model(boost::intrusive_ptr<TreeItem> _item);
    void cursor_up_one_level(void);
    void cursor_up_to_root(void);
    boost::intrusive_ptr<TreeItem> duplicated_remove(boost::intrusive_ptr<TreeItem> target, boost::intrusive_ptr<TreeItem> source);

public slots:
    void branch_paste_from_search(KnowModel *_current_model, QModelIndex _current_index, boost::intrusive_ptr<TreeItem> _result_item); // , std::shared_ptr<RecordTable> resultset_data

private slots:

    void branch_expand_all_sub(void);
    void branch_collapse_all_sub(void);
    void expand_or_collapse_recurse(QModelIndex index, bool mode);
    //    boost::intrusive_ptr<TreeItem> branch_insert_sub(void);
    //    boost::intrusive_ptr<TreeItem> branch_insert_sibling(void);
    void branch_edit(void);

    QList<boost::intrusive_ptr<TreeItem>> branchs_delete(QModelIndexList _index_list, QString mode, bool _cut_branch_confirm = false);
    QList<boost::intrusive_ptr<TreeItem>> branchs_delete(QString mode = "delete", bool _cut_branch_confirm = false);
    boost::intrusive_ptr<TreeItem> branch_delete_one(QModelIndex index, KnowModel *_current_know_branch);



    //    void item_move_up_branch(void);
    //    void item_move_dn_branch(void);
    void branch_cut(bool _cut_branch_confirm = false);
    bool branch_copy(void);
    //    void branch_paste(void);
    //    void branch_paste_sub(void);

    void branch_encrypt(void);
    void branch_decrypt(void);

    // Действия при клике на ветку дерева
    void branch_step_into(const QModelIndex &index);

    // Открытие контекстного меню
    void on_custom_contextmenu_requested(const QPoint &pos);

    void edit_field_context(QModelIndex index_current);
    void edit_field(QString pin
                    , QString name
                    , QString author
                    , QString home
                    , QString url
                    , QString tags
                   );
private:


    KnowModel       *_know_model_board;      // for tree screen
    //    TreeController  *_tree_controller;

    QToolBar        *_tools_line;
    QToolBar        *_menubar;              // QMenuBar *_menubar;
    QPushButton     *_menubutton;
    QWidgetAction   *_menuaction;
    QMenu           *_menus_in_button;
    QMenu           *_context_menu;

    KnowView        *_tree_view;
    QHBoxLayout     *_tools_layout;
    //    browser::ToolbarSearch  *_recordtree_search;
    //    QHBoxLayout             *_recordtree_searchlayout;
    QVBoxLayout     *_treescreenlayout;

    const AppConfig &_appconfig;

    void setup_ui(QMenu *main_menu, QMenu *_toolsmenu);

    //    void setup_model(KnowModel *treemodel);
    //    void update_model(KnowModel *_current_know_branch);

    void setup_signals(void);
    void setup_actions(void);
    void assembly(void);

    void item_move_up_dn_branch(int(TreeItem::*_move)());
    bool move_checkenable(void);

    //    template<bool insert_sibling_branch = true>
    boost::intrusive_ptr<TreeItem> branch_insert_new(
        std::function < boost::intrusive_ptr<TreeItem> (KnowModel *, QModelIndex, QString
                                                        , std::function < boost::intrusive_ptr<TreeItem>(KnowModel *, QModelIndex, QString, QString) >) > _branch_add
        , std::function < boost::intrusive_ptr<TreeItem>(KnowModel *, QModelIndex, QString, QString) > _branch_add_impl
    );
    //    boost::intrusive_ptr<TreeItem> insert_branch_process(QModelIndex current_index, QString name, bool insert_sibling_branch);

    //    void branch_add_to_clipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root);

    void branch_paste_func(std::function<QString(const QModelIndex &, ClipboardBranch *)> lock_paste_as);
    //    void branch_paste_smart(bool is_branch);

    void tree_empty_controll(void);
    void tree_crypt_control(void);

    void branch_item_encrypt(void);
    void branch_item_decrypt(void);
    //    friend class browser::WebPage;
    // friend void browser::WebPage::onUrlChanged(const QUrl &url);
    friend class KnowView;
};

//// Вспомогательная функция, используется при добавлении ветки
//// Если is_branch=true, то добавляется ветка на тот же уровень
//// Если is_branch=false, то добавляется подветка
//// Helper function is used when you add branches
//// Eli is branch = true, the branch is added to the same level
//// If is_branch = false, then added podvetka
//template<bool insert_sibling_branch>
//inline boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_smart()     // bool insert_sibling_branch
//{

//    boost::intrusive_ptr<TreeItem> result(nullptr);
//    // Получение списка индексов QModelIndex выделенных элементов
//    QModelIndexList selectitems = _tree_view->selectionModel()->selectedIndexes();
//    QModelIndex _current_index;

//    // Если выбрано более одной ветки
//    if(selectitems.size() > 1) {
//        //        QMessageBox messageBox(this);
//        //        messageBox.setWindowTitle(tr("Unavailable action"));
//        //        messageBox.setText(tr("You've selected ") + QString::number(selectitems.size()) + tr(" items.\nPlease select single item for enabling insert operation."));
//        //        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
//        //        messageBox.exec();
//        //        //        current_index = selectitems.first();
//        //        return nullptr;
//        _current_index = selectitems.first();
//    } else {

//        //    else if(selectitems.size() == 0) {
//        //        current_index = last_index();
//        //    } else {
//        _current_index = _tree_view->index_current();
//    }

//    if(_current_index.isValid()) {
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
//        result = branch_add<insert_sibling_branch>(_current_index, name
//                                                   // , insert_sibling_branch
//                                                   , _tree_view->source_model());
//    }

//    // Введенные данные добавляются
//    return result;
//}

//template<>
//inline boost::intrusive_ptr<TreeItem> TreeScreen::branch_add<false>(QModelIndex _current_index, QString name, KnowModel *_current_model)
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

//    // Подветка

//    // Вставка новых данных в модель дерева записей
//    result = _current_model->lock_child_add(_current_index, id, name);

//    // Установка курсора на только что созданную позицию
//    //        QModelIndex
//    setto = _current_model->index(result);  //index_child(_current_index, item->current_count() - 1);
//    _tree_view->selectionModel()->setCurrentIndex(setto, QItemSelectionModel::ClearAndSelect);

//    // А можно было установить курсор на нужную позицию и так
//    // knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
//    //                                             QItemSelectionModel::ClearAndSelect);


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

//template<>
//inline boost::intrusive_ptr<TreeItem> TreeScreen::branch_add<true>(QModelIndex _current_index, QString name, KnowModel *_current_model)
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



#endif  // _TREESCREEN_H_




