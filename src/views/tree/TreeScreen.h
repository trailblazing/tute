#ifndef _TREESCREEN_H_
#define _TREESCREEN_H_

#include <QtGlobal>
#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QMenuBar>

class KnowTreeModel;
class KnowTreeView;
class ClipboardBranch;
class AppConfig;
class QMenuBar;
class QWidgetAction;



namespace  browser {
    class WebPage;
}


class TreeScreen : public QWidget {
    Q_OBJECT

public:
    TreeScreen(const AppConfig &appconfig, QWidget *parent = 0);
    virtual ~TreeScreen();

    QMap<QString, QAction *> _actionlist;

    KnowTreeModel *_knowtreemodel;

    void saveKnowTree(void);
    void reloadKnowTree(void);

    void updateSelectedBranch(void);

    int getFirstSelectedItemIndex(void);
    QModelIndex getCurrentItemIndex(void);

    QItemSelectionModel *getSelectionModel(void);

    // Установка курсора на указанный элемент
    void setCursorToIndex(QModelIndex index);

    void updateBranchOnScreen(const QModelIndex &index);

    QMenu *buttonmenu() {return _menus_in_button;}

private slots:

    void expand_all_subbranch(void);
    void collapse_all_subbranch(void);
    void expand_or_collapse_recurse(QModelIndex index, bool mode);
    void ins_subbranch(void);
    void ins_branch(void);
    void edit_branch(void);

    void del_branch(QString mode = "delete");
    void del_one_branch(QModelIndex index);

    void move_up_branch(void);
    void move_dn_branch(void);
    void cut_branch(void);
    bool copy_branch(void);
    void paste_branch(void);
    void paste_subbranch(void);

    void encrypt_branch(void);
    void decrypt_branch(void);

    // Действия при клике на ветку дерева
    void on_knowtree_clicked(const QModelIndex &index);

    // Открытие контекстного меню
    void on_customContextMenuRequested(const QPoint &pos);

private:

    QToolBar        *_toolsline;
    QToolBar        *_menubar; // QMenuBar *_menubar;
    QMenu           *_menus_in_button;
    QPushButton     *_menubutton;
    QWidgetAction   *_menuaction;


    KnowTreeView    *_knowtreeview;
    QHBoxLayout     *_toolslayout;
    QVBoxLayout     *_treescreenlayout;

    const AppConfig &_appconfig;

    void setupUI(void);
    void setupModels(void);
    void setupSignals(void);
    void setupActions(void);
    void assembly(void);

    void moveUpDnBranch(int direction);
    bool moveCheckEnable(void);

    void insBranchSmart(bool is_branch);
    void insBranchProcess(QModelIndex index, QString name, bool is_branch);

    void addBranchToClipboard(ClipboardBranch *branch_clipboard_data, QStringList path, bool is_root);

    void pasteBranchSmart(bool is_branch);

    void treeEmptyControl(void);
    void treeCryptControl(void);

    void encryptBranchItem(void);
    void decryptBranchItem(void);
    friend class browser::WebPage;
    // friend void browser::WebPage::onUrlChanged(const QUrl &url);
};


#endif  // _TREESCREEN_H_

