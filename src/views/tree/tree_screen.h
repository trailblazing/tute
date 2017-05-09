#ifndef _TREESCREEN_H_
#define _TREESCREEN_H_

#include <map>
#include <memory>

#include <QEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QListView>
#include <QMenuBar>
#include <QScrollArea>
#include <QStackedLayout>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include "utility/delegate.h"
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
// #include "models/tree/TreeItem.h"
// #include "views/tree/KnowView.h"
// #include "views/main_window/MainWindow.h"
// #include "models/tree/TreeModel.h"
// #include "models/tree/KnowModel.h"
// #include "libraries/GlobalParameters.h"
#include "libraries/global_parameters.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif
struct id_value;
class gl_para;
extern id_value get_unical_id(void);
extern std::shared_ptr<gl_para> gl_paras;
extern const char* global_root_id;

// extern const char *action_edit_switch;
extern const char* action_hide_tree_screen;
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
extern const char* action_main_menu;

extern const char* tree_screen_viewer_name;

class tkm_t;
class tv_t;
class ClipboardBranch;
class AppConfig;
class QMenuBar;
class QWidgetAction;
class vector_t;
class i_t;
class tm_t;
class TreeController;
class rctrl_t;
class ts_t;
class InfoFieldsEditor;
class rs_t;
#ifdef USE_TREE_SCREEN_VIEW
class tsv_t;
#endif // USE_TREE_SCREEN_VIEW
class wn_t;
class Blogger;
struct TreeIndex;
struct Linker;

namespace web {
	class ToolbarSearch;
	class WebPage;
	class TabWidget;
	class DownloadManager;
	template <typename>
	class Docker;
}

class AdjustingScrollArea : public QScrollArea {
	bool eventFilter(QObject* obj, QEvent* ev);
	tv_t* _tree_view;

    protected:
	void resizeEvent(QResizeEvent* e);

    public:
	AdjustingScrollArea(tv_t* _tree_view, QWidget* parent = 0);

	void setWidget(tv_t* view);
	friend class ts_t;
};

class tv_t;

// class TreeViewHelpWidget: public QWidget {
// KnowView        *_tree_view;
// QScrollArea     *_scroll_area;
// public:
// TreeViewHelpWidget(TreeScreen *_tree_screen);
// void repaintZoom();
// };

// struct know_root_holder {
// protected:  //public:
// know_root_holder(const AppConfig &_appconfig, TreeScreen *_this);

// KnowModel *know_root() {return _know_root;}
// KnowModel const *know_root()const {return _know_root;}
// ~know_root_holder();
// private:
// KnowModel   *_know_root;        // for tree screen
// };

class ts_t : public QWidget { // , public know_root_holder
#if QT_VERSION == 0x050600
	W_OBJECT(ts_t)
#else
	Q_OBJECT
#endif

    public:
	typedef std::function<bool(boost::intrusive_ptr<const Linker>)>
	    substitute_condition;
	typedef std::function<bool(boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)>
	    substitute_condition_double;
	typedef std::function<boost::intrusive_ptr<i_t>(
	    TreeIndex, boost::intrusive_ptr<i_t>, const substitute_condition&)>
	    paste_strategy;

	ts_t(QString object_name, web::Docker<Blogger>* editor_docker_, wn_t* main_window = 0);
	virtual ~ts_t();

	QMap<QString, QAction*> _actionlist;

	// TreeKnowModel const *know_root()const {return
	// know_root_holder::know_root();}
	// TreeKnowModel *know_root_modify() {return know_root_holder::know_root();}

	// std::shared_ptr<sd::_interface<sd::meta_info<void >, RecordController *>>
	// reocrd_controller;  // for entrance
	// std::shared_ptr<sd::_interface<sd::meta_info<void >, web::TabWidget *>>
	// tabmanager;         // for entrance

	// boost::intrusive_ptr<TreeItem> synchronize();

	// void view_update_selected_indexes(void);
	// void indexes_update_for_selected(void);

	// int item_first_selected(void);

	// QItemSelectionModel *selection_model(void);

	//// Установка курсора на указанный элемент
	// void cursor_to_index(QModelIndex _index);

	// void branch_update_from_model(const QModelIndex &index);

	QMenu* menus_in_button();
	// {
	// return _main_menu_in_button;
	// }

	// QToolBar *menubar()
	// {
	// return _main_menu_bar;
	// }

	// boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index,
	// QString name, bool insert_sibling_branch);

	// boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index,
	// boost::intrusive_ptr<TreeItem> it, bool insert_sibling_branch, KnowModel
	// *_current_model);

	// boost::intrusive_ptr<TreeItem> view_paste_sibling(TreeIndex _modelindex,
	// boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition
	// &_substitute_condition);

	// boost::intrusive_ptr<TreeItem> branch_add(QModelIndex _current_index,
	// QString name, bool insert_sibling_branch, KnowModel *_current_model);

	// boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index,
	// QString name, bool insert_sibling_branch, std::shared_ptr<KnowModel>
	// _current_model);

	// TreeController *treecontroller() {return _tree_controller;}

	void enable_up_action();
	// TreeModelKnow *shadow_branch() {return _shadow_branch;}

	// boost::intrusive_ptr<Linker> view_cut(boost::intrusive_ptr<TreeItem>
	// target);

	// boost::intrusive_ptr<TreeItem> branch_paste(boost::intrusive_ptr<TreeItem>
	// item, KnowModel *_current_know_branch);

	// QStringList record_path(QString _record_id) {return
	// know_root_holder::know_root()->record_path(_record_id);}
	// bool is_item_valid(QStringList _path) {return
	// know_root_holder::know_root()->is_item_valid(_path);}
	// boost::intrusive_ptr<TreeItem> item(QStringList path) const;
	// boost::intrusive_ptr<TreeItem> item(TreeModel::delegater _del) const;
	tv_t* view();
// {
// return _tree_view;
// }

// void resetup_model(boost::intrusive_ptr<TreeItem> _item);

// void restore_menubar();
#ifdef USE_TREE_SCREEN_VIEW
	tsv_t* viewer() const;

	void viewer(tsv_t* v);
#endif

	static void open(bool activate_browser = false, bool show_tree = true);
    public slots:
	// void view_paste_from_search(TreeIndex _modelindex,
	// boost::intrusive_ptr<TreeItem> _result_item,
	// std::function<bool(boost::intrusive_ptr<TreeItem>)> _substitute_condition);
	// // , std::shared_ptr<RecordTable> resultset_data

	// void index_invoke_in_sub_branch(const QModelIndex &_index);

	// QString session_root_id();
	// void session_root_id(QString root_id);
	// QString session_root();
	void init_context_menu();

	void append_main_menu();

    protected:
	void resizeEvent(QResizeEvent* e);
    private slots:

	// void view_collapse_all_sub(void);
	// void expand_or_collapse_recurse(QModelIndex index, bool mode);

	// boost::intrusive_ptr<TreeItem> branch_insert_sub(void);
	// boost::intrusive_ptr<TreeItem> branch_insert_sibling(void);

	// QList<boost::intrusive_ptr<TreeItem>>
	// view_delete_indexes(std::function<KnowModel *()> _current_model
	// , QModelIndexList            _origin_index_list
	// , QString                    _mode
	// , bool                       _cut_branch_confirm = false
	// );

	// QList<boost::intrusive_ptr<Linker> > view_remove(const QModelIndexList
	// &index_list, const QString &mode = "delete", const bool _cut_branch_confirm
	// = false);

	// boost::intrusive_ptr<TreeItem> model_delete_one(std::function<KnowModel
	// *()> _current_model, QModelIndex _index_delete);

	// void item_move_up_branch(void);
	// void item_move_dn_branch(void);

	// void branch_paste(void);
	// void branch_paste_sub(void);

	// Открытие контекстного меню
	void on_custom_contextmenu_requested(const QPoint& _position);

	void edit_field_context(QModelIndex index_current);
	// void edit_field(QString pin, QString name, QString author, QString home,
	// QString url, QString tags);

    private:
	// TreeController  *_tree_controller;
	web::Docker<Blogger>* _editor_docker;
	wn_t* _main_window;
	QToolBar* _tools_line;
#ifdef USE_MAIN_MENU_IN_BUTTON
	QToolBar* _extra_tools_line;
#endif // USE_MAIN_MENU_IN_BUTTON
	// QToolBar        *_main_menu_bar;                  // QMenuBar *_menubar;
	// QPushButton     *_main_menu_button;
	// QWidgetAction   *_main_menu_action;

	QMenu* _main_menu_in_button;
	QMenu* _context_menu;
	tv_t* _tree_view;
	// KnowModel       *_know_model_board;      // for tree screen, move to
	// KnowView
	QHBoxLayout* _tools_layout;
	// web::ToolbarSearch  *_recordtree_search;
	// QHBoxLayout             *_recordtree_searchlayout;
	AdjustingScrollArea* _adjustingscrollarea;
	// TreeViewHelpWidget *_treeviewhelpwidget;

	QVBoxLayout* _treescreenlayout;

#ifdef USE_TREE_SCREEN_VIEW
	tsv_t* _viewer = nullptr;
#endif // USE_TREE_SCREEN_VIEW

	// std::shared_ptr<AppConfig> _appconfig;
	// QString         _session_id = global_root_id;

	void assembly_menubar();
	void setup_ui();

	// void setup_model(KnowModel *treemodel);
	// void update_model(KnowModel *_current_know_branch);

	void setup_signals(void);
	void setup_actions(void);
	void assembly(); // QToolBar *menubar_

	void item_move_up_dn_branch(int (i_t::*_move)());
	bool move_checkenable(void);

	// boost::intrusive_ptr<TreeItem> insert_branch_process(QModelIndex
	// current_index, QString name, bool insert_sibling_branch);

	// void branch_add_to_clipboard(ClipboardBranch *branch_clipboard_data,
	// QStringList path, bool is_root);

	// void branch_paste_smart(bool is_branch);

	// void item_encrypt(void);
	// void item_decrypt(void);

	// bool is_index_localized(const QModelIndexList _origin_index_list)const;
	// QModelIndexList index_localize(const QModelIndexList _origin_index_list);

	// KnowModel *know_model_board();    // don't recommand to operate on this
	// know_model directly

	// friend class web::WebPage;
	// friend void web::WebPage::onUrlChanged(const QUrl &url);
	friend class tv_t;
	friend class wn_t;
};

//// Вспомогательная функция, используется при добавлении ветки
//// Если is_branch=true, то добавляется ветка на тот же уровень
//// Если is_branch=false, то добавляется подветка
//// Helper function is used when you add branches
//// Eli is branch = true, the branch is added to the same level
//// If is_branch = false, then added podvetka
// template<bool insert_sibling_branch>
// inline boost::intrusive_ptr<TreeItem> TreeScreen::branch_insert_smart()
// // bool insert_sibling_branch
// {

// boost::intrusive_ptr<TreeItem> result(nullptr);
//// Получение списка индексов QModelIndex выделенных элементов
// QModelIndexList selectitems =
// _tree_view->selectionModel()->selectedIndexes();
// QModelIndex _current_index;

//// Если выбрано более одной ветки
// if(selectitems.size() > 1) {
////        QMessageBox messageBox(this);
////        messageBox.setWindowTitle(tr("Unavailable action"));
////        messageBox.setText(tr("You've selected ") +
///QString::number(selectitems.size()) + tr(" items.\nPlease select single item
///for enabling insert operation."));
////        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
////        messageBox.exec();
////        //        current_index = selectitems.first();
////        return nullptr;
// _current_index = selectitems.first();
// } else {

////    else if(selectitems.size() == 0) {
////        current_index = last_index();
////    } else {
// _current_index = _tree_view->index_current();
// }

// if(_current_index.isValid()) {
////    }

////    if(!_current_index.isValid()) {
////
///_tree_view->selectionModel()->setCurrentIndex(_current_model->index(0, 0,
///QModelIndex()), QItemSelectionModel::ClearAndSelect);
////        assert(_current_index.isValid());
////    }

//// Создается окно ввода данных
// bool ok;
// QString title, text;

// if(insert_sibling_branch) {
// title = tr("Create new item");
// text = tr("Item name:");
// } else {
// title = tr("Create new sub item");
// text = tr("Sub item name:");
// }

// QString name = QInputDialog::getText(this, title, text, QLineEdit::Normal,
// "", &ok);

// if(!(ok && !name.isEmpty())) return nullptr;    // Если была нажата отмена

//// Получение индекса выделенной строки
////    QModelIndex
////    current_index = currentitem_index();
// result = branch_add<insert_sibling_branch>(_current_index, name
//// , insert_sibling_branch
// , _tree_view->source_model());
// }

//// Введенные данные добавляются
// return result;
// }

// template<>
// inline boost::intrusive_ptr<TreeItem>
// TreeScreen::branch_add<false>(QModelIndex _current_index, QString name,
// KnowModel *_current_model)
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);
//// Получение ссылки на узел, который соответствует выделенной строке
////    auto item = _current_model->item(_current_index);

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);

//// Получение уникального идентификатора
// QString id = get_unical_id();
// QModelIndex setto;

//// Вставка данных и установка курсора

//// Одноранговая ветка

//// Подветка

//// Вставка новых данных в модель дерева записей
// result = _current_model->lock_child_add(_current_index, id, name);

//// Установка курсора на только что созданную позицию
////        QModelIndex
// setto = _current_model->index(result);  //index_child(_current_index,
// item->current_count() - 1);
// _tree_view->selectionModel()->setCurrentIndex(setto,
// QItemSelectionModel::ClearAndSelect);

//// А можно было установить курсор на нужную позицию и так
////
///knowTreeView->selectionModel()->setCurrentIndex(kntrmodel->index(item->childCount()-1,0,index),
//// QItemSelectionModel::ClearAndSelect);

// _know_model_board->synchronized(false);
//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// knowtree_save();

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
// assert(result->name() == name);
// assert(result == _current_model->item(setto));
// return  result; // _current_model->item(setto);
// }

// template<>
// inline boost::intrusive_ptr<TreeItem>
// TreeScreen::branch_add<true>(QModelIndex _current_index, QString name,
// KnowModel *_current_model)
// {
// boost::intrusive_ptr<TreeItem> result(nullptr);
//// Получение ссылки на узел, который соответствует выделенной строке
////    auto item = _current_model->item(_current_index);

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setDisabled(true);

//// Получение уникального идентификатора
// QString id = get_unical_id();
// QModelIndex setto;

//// Вставка данных и установка курсора

//// Одноранговая ветка
////    if(insert_sibling_branch) {

// auto parent = _tree_view->source_model()->item(_current_index)->parent();
// assert(parent);
// auto same = parent->child(parent->is_name_exists(name));

// if(same) {  // && same->is_empty()
// assert(same->name() == name);
// result = same;
////            children_transfer(_new_item, _current_model);
// } else {
//// Вставка новых данных в модель дерева записей
// result = _current_model->lock_sibling_add(_current_index, id, name);
// }

//// Установка курсора на только что созданную позицию

//// Чтобы вычислить позицию, надо синхронно получить parent элемента,
//// на уровне которого должен был создасться новый элемент.
//// Parent надо получить и в виде объекта QModelIndex, и в виде объекта Item
//// Затем у объекта Item выяснить количество элементов, и установить
//// засветку через метод index() относительно parent в виде QModelIndex
////        QModelIndex
// setto = _current_model->index(result);  //(item->parent()->current_count() -
// 1, 0, _current_index.parent());
// _tree_view->selectionModel()->setCurrentIndex(setto,
// QItemSelectionModel::ClearAndSelect);

////    }

// _know_model_board->synchronized(false);
//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)->
// knowtree_save();

////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->setEnabled(true);
// assert(result->name() == name);
// assert(result == _current_model->item(setto));
// return  result; // _current_model->item(setto);
// }
//
#ifdef USE_TREE_SCREEN_VIEW
// deprecated
class tsv_t : public QFrame {
    public:
	tsv_t(wn_t* main_window, ts_t* tree_screen, QWidget* widget_right);

	~tsv_t();
	ts_t* tree_screen() const;
	int tree_screen(ts_t* tree);

	QWidget* widget_right() const;
	void widget_right(QWidget* rs);

    private:
	wn_t* _main_window;
	ts_t* _tree_screen;
	QWidget* _widget_right;
	QStackedLayout* _layout;
};
#endif // USE_TREE_SCREEN_VIEW
//
#endif // _TREESCREEN_H_
