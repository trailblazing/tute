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
#include "models/tree/KnowModel.h"



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
    : public QWidget
//    , public know_root_holder
{
    Q_OBJECT

public:
    TreeScreen(QString object_name, const AppConfig &_appconfig, QMenu *_filemenu, QMenu *_toolsmenu, QWidget *_parent = 0);
    virtual ~TreeScreen();

    QMap<QString, QAction *> _actionlist;

    //    TreeKnowModel const *know_root()const {return know_root_holder::know_root();}
    //    TreeKnowModel *know_root_modify() {return know_root_holder::know_root();}

    const KnowModel *know_model_board()const {return _know_model_board;}
    void synchronized(bool _synchronized) {_know_model_board->synchronized(_synchronized);}
    bool sysynchronized() {return _know_model_board->synchronized();}
    //    std::shared_ptr<sd::_interface<sd::meta_info<void *>, RecordController *>>      reocrd_controller;  // for entrance
    //    std::shared_ptr<sd::_interface<sd::meta_info<void *>, browser::TabWidget *>>    tabmanager;         // for entrance

    boost::intrusive_ptr<TreeItem> intercept(QString id);
    //    boost::intrusive_ptr<TreeItem> synchronize();
    void save_knowtree(void);
    void reload_knowtree(void);

    void update_selected(void);
    void update_selected_branchs(void);

    int first_selecteditem_row(void);



    QItemSelectionModel *selection_model(void);

    // Установка курсора на указанный элемент
    void cursor_to_index(QModelIndex index);

    void update_branch_on_screen(const QModelIndex &index);

    QMenu *buttonmenu() {return _menus_in_button;}

    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch);

    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, boost::intrusive_ptr<TreeItem> it, bool insert_sibling_branch, KnowModel *_current_model);
    boost::intrusive_ptr<TreeItem> insert_children(boost::intrusive_ptr<TreeItem> new_branch_root, KnowModel *_current_model);
    boost::intrusive_ptr<TreeItem> add_branch(boost::intrusive_ptr<TreeItem> new_branch_root, bool insert_sibling_branch, KnowModel *_current_model);
    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch, KnowModel *_current_model);
    //    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch, std::shared_ptr<KnowModel> _current_model);

    //    TreeController *treecontroller() {return _tree_controller;}
    void invoke_index(const QModelIndex &index);
    void enable_up_action();
    //    TreeModelKnow *shadow_branch() {return _shadow_branch;}
    boost::intrusive_ptr<TreeItem> cut_branch(boost::intrusive_ptr<TreeItem> item);
    boost::intrusive_ptr<TreeItem> paste_branch(boost::intrusive_ptr<TreeItem> item, KnowModel *_current_know_branch);


    //    QStringList record_path(QString _record_id) {return know_root_holder::know_root()->record_path(_record_id);}
    //    bool is_item_valid(QStringList _path) {return know_root_holder::know_root()->is_item_valid(_path);}
    //    boost::intrusive_ptr<TreeItem> item(QStringList path) const;
    //    boost::intrusive_ptr<TreeItem> item(TreeModel::delegater _del) const;
    KnowView *tree_view() {return _tree_view;}
    void setup_model(boost::intrusive_ptr<TreeItem> _item);

public slots:
    void candidate_from_search_result(boost::intrusive_ptr<TreeItem> _result_item); // , std::shared_ptr<RecordTable> resultset_data

private slots:

    void expand_all_subbranch(void);
    void collapse_all_subbranch(void);
    void expand_or_collapse_recurse(QModelIndex index, bool mode);
    boost::intrusive_ptr<TreeItem> insert_subbranch(void);
    boost::intrusive_ptr<TreeItem> insert_branch(void);
    void edit_branch(void);

    QList<boost::intrusive_ptr<TreeItem>> delete_branchs(QModelIndexList selectitems, QString mode);
    QList<boost::intrusive_ptr<TreeItem>> delete_branchs(QString mode = "delete");
    boost::intrusive_ptr<TreeItem> delete_one_branch(QModelIndex index, KnowModel *_current_know_branch);


    void view_up_one_level(void);
    void view_return_to_root(void);
    void move_item_up_branch(void);
    void move_item_dn_branch(void);
    void cut_branch(void);
    bool copy_branch(void);
    void paste_branch(void);
    void paste_subbranch(void);

    void encrypt_branch(void);
    void decrypt_branch(void);

    // Действия при клике на ветку дерева
    void candidate_from_knowtree_item(const QModelIndex &index);

    // Открытие контекстного меню
    void on_custom_contextmenu_requested(const QPoint &pos);

private:


    KnowModel       *_know_model_board;      // for tree screen
    //    TreeController  *_tree_controller;

    QToolBar        *_tools_line;
    QToolBar        *_menubar;              // QMenuBar *_menubar;
    QPushButton     *_menubutton;
    QWidgetAction   *_menuaction;
    QMenu           *_menus_in_button;

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

    void move_item_up_dn_branch(int direction);
    bool move_checkenable(void);

    boost::intrusive_ptr<TreeItem> insert_branch_smart(bool insert_sibling_branch);
    //    boost::intrusive_ptr<TreeItem> insert_branch_process(QModelIndex current_index, QString name, bool insert_sibling_branch);

    void add_branch_to_clipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root);

    void paste_branch_smart(bool is_branch);

    void tree_empty_controll(void);
    void tree_crypt_control(void);

    void encrypt_branch_item(void);
    void decrypt_branch_item(void);
    //    friend class browser::WebPage;
    // friend void browser::WebPage::onUrlChanged(const QUrl &url);
    friend class KnowView;
};


#endif  // _TREESCREEN_H_

