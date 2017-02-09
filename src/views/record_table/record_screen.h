#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_


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





#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif






class rctrl_t;
class VerticalScrollArea;
class FlatToolButton;
class TreeItem;
class ts_t;
class FindScreen;
class Editentry;
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


#ifndef USE_WITHOUT_REGISTERED_TREEITEM
#endif


#ifndef USE_BLANK_ITEM
#endif


#ifndef USE_BUTTON_PIN
#endif

#ifndef USE_BUTTON_CLOSE
#endif

class rs_t : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(rs_t)
#else
	Q_OBJECT
#endif

	public:
		rs_t(ts_t *_tree_screen, FindScreen *_find_screen, Editentry *_editentry, browser::Entrance *_entrance, HidableTabWidget *_vtab_record, const QString &_style_source, browser::Profile *_profile);

		virtual ~rs_t();

		// int     first_selection_pos(void);
		// QString first_selection_id(void);
		// void    select_pos(PosProxy pos_proxy_);
		// void    select_id(IdType id);

		void tree_path(QString path);
		QString tree_path(void);

		rctrl_t *record_controller();

		// bool                inited() {return _inited;}
//	browser::TabWidget	*tabmanager();
		browser::Browser	*browser();
		ts_t			*tree_screen();
//	QAction			*record_hide();	// move to main_window::_vtab_record->tabBar()->tabBarClicked
//	void			restore_menubar();

	public slots:

		// Обновление панели инструментов
		void tools_update();	// W_SLOT(tools_update)


	protected:
		void resizeEvent(QResizeEvent *e);
	private slots:

		void on_syncro_click(void);	// W_SLOT(on_syncro_click, W_Access::Private)
		void on_walkhistory_previous_click(void);
		void on_walkhistory_next_click(void);
		void on_back_click(void);

	private:
		// bool                _inited = false;

		ts_t			*_tree_screen;
		Editentry		*_editentry;

		// These staffs used on the toolbar and in the context menu entries    // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
		QAction			*_record_hide;
//	QAction			*_save_in_new_branch;
#ifdef USE_BUTTON_PIN
		QAction			*_pin;
#endif

#ifdef USE_BLANK_ITEM
		QAction			*_addnew_to_end;
		QAction			*_addnew_before;
		QAction			*_addnew_after;
#endif
		QAction			*_edit_field;
#ifdef USE_BUTTON_CLOSE
		QAction			*_close;
#endif
		QAction			*_delete;
#ifdef USE_WITHOUT_REGISTERED_TREEITEM
		QAction			*_cut;
		QAction			*_copy;
		QAction			*_paste;
#endif
		QAction			*_editor;
		QAction			*_settings;
		QAction			*_action_move_top;
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

		QLabel			*_treepathlabel;//	FlatToolButton		*_treepath_button;
		QString	_treepath;


		browser::Browser	*_browser;
		rctrl_t			*_record_controller;	//	browser::TabWidget	*_tabmanager;
		VerticalScrollArea	*_vertical_scrollarea;

		QHBoxLayout         *_records_toolslayout;
		// browser::ToolbarSearch  *_recordtree_search;
		// QHBoxLayout             *_recordtree_searchlayout;
		QVBoxLayout         *_records_screenlayout;

		void setup_ui(void);
		void setup_signals(void);
		void setup_actions(void);
		void assembly(void);

		void disable_all_actions(void);
//	void	save_in_new_branch(bool checked = false);
		friend class rv_t;
		friend class rctrl_t;
		friend class wn_t;
//	friend class ts_t;
};

#endif /* RECORDTABLESCREEN_H_ */
