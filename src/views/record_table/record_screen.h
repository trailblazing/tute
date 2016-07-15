#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_

#include <wobjectdefs.h>
#include <QObject>
#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QItemSelection>
#include <QLabel>
// #include "models/tree/TreeItem.h"


class rctl_t;
class VerticalScrollArea;
class FlatToolButton;
class TreeItem;
class ts_t;
class FindScreen;
class MetaEditor;
class wn_t;
class AppConfig;
class HidableTabWidget;

struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;


namespace browser {
    class Entrance;
    class Browser;
    class ToolbarSearch;
    class TabWidget;
    class Profile;
}

class rs_t : public QWidget {
    W_OBJECT(rs_t)

    public:
	rs_t(ts_t *_tree_screen, FindScreen *_find_screen, MetaEditor *_editor_screen, browser::Entrance *_entrance, browser::Browser *_browser, wn_t *_main_window, browser::Profile *_profile);

	virtual ~rs_t();

	// int     first_selection_pos(void);
	// QString first_selection_id(void);
	// void    select_pos(PosProxy pos_proxy_);
	// void    select_id(IdType id);

	void	tree_path(QString path);
	QString tree_path(void);

	rctl_t *record_controller();

	// bool                inited() {return _inited;}
	browser::TabWidget	*tabmanager();
	browser::Browser	*browser();
	ts_t			*tree_screen();
	QAction			*tree_show();
//	void			restore_menubar();
    public slots:

	// Обновление панели инструментов
	void tools_update();	// W_SLOT(tools_update)


    protected:
	void resizeEvent(QResizeEvent *e);
    private slots:

	void	on_syncro_click(void);	// W_SLOT(on_syncro_click, W_Access::Private)
	void	on_walkhistory_previous_click(void);
	void	on_walkhistory_next_click(void);
	void	on_back_click(void);

    private:
	// bool                _inited = false;
	browser::Browser	*_browser;
	ts_t			*_tree_screen;
	wn_t			*_main_window;

	// These staffs used on the toolbar and in the context menu entries    // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
	QAction			*_record_hide;
	QAction			*_save_in_new_branch;
	QAction			*_pin;
	QAction			*_addnew_to_end;
	QAction			*_addnew_before;
	QAction			*_addnew_after;
	QAction			*_edit_field;
	QAction			*_close;
	QAction			*_delete;
	QAction			*_cut;
	QAction			*_copy;
	QAction			*_paste;
	QAction			*_editor;
	QAction			*_settings;
	QAction			*_action_move_up;
	QAction			*_action_move_dn;
	QAction			*_find_in_base;
	QAction			*_action_syncro;
	QAction			*_action_walk_history_previous;
	QAction			*_action_walk_history_next;
	QAction			*_back;
	QAction			*_sort;
	QAction			*_print;

	QToolBar		*_toolsline;
	QToolBar		*_extra_toolsline;

	QLabel			*_treepathlabel;
//	FlatToolButton		*_treepath_button;
	QString	_treepath;



	browser::TabWidget	*_tabmanager;
	rctl_t			*_record_controller;
	VerticalScrollArea	*_vertical_scrollarea;



	QHBoxLayout         *_records_toolslayout;
	// browser::ToolbarSearch  *_recordtree_search;
	// QHBoxLayout             *_recordtree_searchlayout;
	QVBoxLayout         *_records_screenlayout;



	void	setup_ui(void);
	void	setup_signals(void);
	void	setup_actions(void);
	void	assembly(void);

	void	disable_all_actions(void);
	void	save_in_new_branch(bool checked = false);
	friend class rv_t;
	friend class rctl_t;
	friend class wn_t;
//	friend class ts_t;
};

#endif /* RECORDTABLESCREEN_H_ */
