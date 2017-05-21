#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_

#include <QByteArray>
#include <QHBoxLayout>
#include <QItemSelection>
#include <QLabel>
#include <QListView>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
// #include "models/tree/TreeItem.h"
#include "libraries/global_parameters.h"
#include "utility/lease.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class rctrl_t;
class VerticalScrollArea;
class FlatToolButton;
class i_t;
class ts_t;
class FindScreen;
class EditorDock;
class wn_t;
class AppConfig;
class HidableTab;
class Blogger;

struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;

namespace web {
	template <typename>
	class Docker;
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

class rs_t : public QWidget
#ifdef USE_SIGNAL_CLOSE
	     ,
//	     public boost::intrusive_ref_counter<rs_t, boost::thread_safe_counter>,
	     public sd::renter //<rs_t>
#endif                         //USE_SIGNAL_CLOSE
{
#if QT_VERSION == 0x050600
	W_OBJECT(rs_t)
#else
	Q_OBJECT
#endif

    public:
	rs_t(Blogger* blogger_, web::Browser* browser_, web::TabWidget *tabmanager_);

	virtual ~rs_t();

	// int     first_selection_pos(void);
	// QString first_selection_id(void);
	// void    select_pos(PosProxy pos_proxy_);
	// void    select_id(IdType id);

	void tree_path(QString path);
	QString tree_path(void);

	rctrl_t *record_ctrl() const;

	// bool                inited() {return _inited;}
	// web::TabWidget	*tabmanager();
	web::Browser* browser();
	ts_t* tree_screen();
	// QAction *record_hide();	// move to
	// main_window::_vtab_record->tabBar()->tabBarClicked
	// void restore_menubar();

	//	HidableTab *vtab_record();
	Blogger* blogger();
	std::string close_sender_id() const;

	void close_sender_id(const std::string& close_sender_id_);

    public slots:

	// Обновление панели инструментов
	void tools_update(); // W_SLOT(tools_update)

	//	void on_blogger_close_requested();
	//	void on_browser_close_requested();


	void on_topic_changed(const QString& original_topic_, const QString& new_topic_, bool append_mode = false);

    protected:
	void resizeEvent(QResizeEvent* e);
    private slots:
	//	bool close();
	void on_syncro_click(void); // W_SLOT(on_syncro_click, W_Access::Private)
	void on_walkhistory_previous_click(void);
	void on_walkhistory_next_click(void);
	void on_back_click(void);

    private:
	// bool                _inited = false;
	//	HidableTab *_vtab_record;
	ts_t* _tree_screen;
	web::Docker<Blogger>* _editor_docker;
	blogger_ref _blogger;

	// These staffs used on the toolbar and in the context menu entries    //
	// Действия, используемые как на тулбаре, так и в контекстном меню списка
	// записей
	QAction* _record_hide;
// QAction			*_save_in_new_branch;
#ifdef USE_BUTTON_PIN
	QAction* _pin;
#endif

#ifdef USE_BLANK_ITEM
	QAction* _addnew_to_end;
	QAction* _addnew_before;
	QAction* _addnew_after;
#endif
	QAction* _edit_field;
#ifdef USE_BUTTON_CLOSE
	QAction* _close;
#endif
	QAction* _delete;
#ifdef USE_WITHOUT_REGISTERED_TREEITEM
	QAction* _cut;
	QAction* _copy;
	QAction* _paste;
#endif
	QAction* _editor;
	QAction* _settings;
	QAction* _action_move_top;
	QAction* _action_move_up;
	QAction* _action_move_dn;
	QAction* _find_in_base;
	QAction* _action_syncro;
	QAction* _action_walk_history_previous;
	QAction* _action_walk_history_next;
	QAction* _back;
	QAction* _sort;
	QAction* _print;

	QToolBar* _toolsline;
	QToolBar* _extra_toolsline;

	QLabel* _treepathlabel; // FlatToolButton		*_treepath_button;
	QString _treepath;
	browser_ref _browser;
	// EditingWindow *_editing_window = nullptr;
	//	web::Browser* _browser;
	//	web::TabWidget* _tab_widget;
	tabwidget_ref _tab_widget;
	rctrl_ref _rctrl; //

//	VerticalScrollArea* _vertical_scrollarea;

	QHBoxLayout* _records_toolslayout;
	// web::ToolbarSearch  *_recordtree_search;
	// QHBoxLayout             *_recordtree_searchlayout;
	QVBoxLayout* _records_screenlayout;
	std::string _close_sender_id;
	//	bool _closed = false;
	void setup_ui(void);
	void setup_signals(void);
	void setup_actions(void);
	void assembly(void);

	void disable_all_actions(void);
	// void	save_in_new_branch(bool checked = false);
	friend class rv_t;
	friend class rctrl_t;
	friend class wn_t;
	// friend class ts_t;
    signals:
	//	void close_request(QWidget*);
};

#endif /* RECORDTABLESCREEN_H_ */
