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
#include "models/tree/TreeModelKnow.h"



class TreeModelKnow;
class TreeViewKnow;
class ClipboardBranch;
class AppConfig;
class QMenuBar;
class QWidgetAction;
class ItemsFlat;
class TreeItem;
class TreeController;

namespace  browser {
    class ToolbarSearch;
    class WebPage;
}


class TreeScreen : public QWidget {
    Q_OBJECT

public:
    TreeScreen(QString object_name, const AppConfig &appconfig, QMenu *_filemenu, QMenu *_toolsmenu, QWidget *_parent = 0);
    virtual ~TreeScreen();

    QMap<QString, QAction *> _actionlist;

    TreeModelKnow   *_root;             // for tree screen
    TreeModelKnow   *_selected_branch;  // for tree screen
    TreeModelKnow   *_shadow_branch;    // for record screen

    void save_knowtree(void);
    void reload_knowtree(void);

    void update_selected_branchs(void);

    int first_selecteditem_row(void);

    QModelIndex current_index(void);
    QModelIndex last_index(void);

    QItemSelectionModel *selection_model(void);

    // Установка курсора на указанный элемент
    void cursor_to_index(QModelIndex index);

    void update_branch_on_screen(const QModelIndex &index);

    QMenu *buttonmenu() {return _menus_in_button;}

    boost::intrusive_ptr<TreeItem> add_branch(QModelIndex _current_index, QString name, bool insert_sibling_branch);
    boost::intrusive_ptr<TreeItem> insert_branch_process(QModelIndex _current_index, QString name, bool insert_sibling_branch);

    //    TreeController *treecontroller() {return _tree_controller;}
    void to_candidate_screen(const QModelIndex &index);

public slots:
    void candidate_from_search_result(boost::intrusive_ptr<TreeItem> resultset_item); // , std::shared_ptr<RecordTable> resultset_data

private slots:

    void expand_all_subbranch(void);
    void collapse_all_subbranch(void);
    void expand_or_collapse_recurse(QModelIndex index, bool mode);
    boost::intrusive_ptr<TreeItem> insert_subbranch(void);
    boost::intrusive_ptr<TreeItem> insert_branch(void);
    void edit_branch(void);

    void delete_branchs(QString mode = "delete");
    void delete_one_branch(QModelIndex index);

    void move_up_one_level(void);
    void return_to_root(void);
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


    //    TreeController  *_tree_controller;

    QToolBar        *_toolsline;
    QToolBar        *_menubar; // QMenuBar *_menubar;
    QPushButton     *_menubutton;
    QWidgetAction   *_menuaction;
    QMenu           *_menus_in_button;

    TreeViewKnow    *_treeviewknow;
    QHBoxLayout     *_toolslayout;
    //    browser::ToolbarSearch  *_recordtree_search;
    //    QHBoxLayout             *_recordtree_searchlayout;
    QVBoxLayout     *_treescreenlayout;

    const AppConfig &_appconfig;

    void setup_ui(QMenu *main_menu, QMenu *_toolsmenu);
    void setup_model(TreeModelKnow *treemodel);
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
    friend class browser::WebPage;
    // friend void browser::WebPage::onUrlChanged(const QUrl &url);
};


#endif  // _TREESCREEN_H_

