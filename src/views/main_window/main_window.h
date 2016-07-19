#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <wobjectdefs.h>
#include <QObject>

#include <QDialog>
#include <QMainWindow>
#include <QWidget>
#include <QTextListFormat>
#include <QTextBlockFormat>

#include <QtXml>
#include <QtGui>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QList>
#include <QHBoxLayout>
#include <QSplitter>
#include <QSystemTrayIcon>

#include <QStatusBar>
#include "hidable_tabwidget.h"

// #include "libraries/GlobalParameters.h"
// #include "models/app_config/AppConfig.h"
// #include "models/database_config/DataBaseConfig.h"


extern const char *meta_editor_singleton_name;

namespace browser {
    class Entrance;
    class Browser;
    class DownloadManager;
    class HistoryMenu;
    class BookmarksMenu;
    class Profile;
}

class ts_t;
class MetaEditor;
class rs_t;
class FindScreen;
class WindowSwitcher;
class sapp_t;
class rctl_t;
class GlobalParameters;
class AppConfig;
class DataBaseConfig;
class tsv_t;


class wn_t
    : public QMainWindow {
    W_OBJECT(wn_t)

    public:
	wn_t(GlobalParameters &_globalparameters
	    , AppConfig &_appconfig
	    , DataBaseConfig &_databaseconfig
	    , browser::Profile *_profile, QString style_source);

	~wn_t();

	void	restore_geometry(void);
	void	restore_tree_position(void);
//    void restore_recordtable_position(void);// too many _record_screen objects, deprecated
	void	restore_editor_cursor_position(void);
	void	restore_editor_scrollbar_position(void);
	void	restore_find_in_base_visible(void);

	void	set_tree_position(QString view_root_id, QStringList current_item_absolute_path);
	bool	is_tree_position_crypt();

//    void select_id(QString id);// too many _record_screen objects, deprecated

	void synchronization(void);

	void	go_walk_history_previous(void);
	void	go_walk_history_next(void);

	void save_text_area(void);

	void save_all_state(void);

	HidableTabWidget	*vtab_record();
	HidableTabWidget	*vtab_tree();

	QMenu			*file_menu();
	QMenu			*edit_menu();
	QMenu			*view_menu();
	browser::HistoryMenu	*histry_menu();
	browser::BookmarksMenu	*bookmark_menu();
	QMenu			*window_menu();
	QMenu			*tools_menu();
	QMenu			*help_menu();
	QSplitter		*find_splitter();
	QSplitter		*h_record_splitter();
	QSplitter		*h_tree_splitter();
//	std::vector<tsv_t *>	tree_viewers() const;
    public slots:
	void	application_exit(void);
	void	application_fast_exit(void);
	void	commit_data(QSessionManager &manager);
	void	editor_switch(void);

    private slots:
	void	file_new(void);
	void	file_open(void);
	bool	file_save(void);
	bool	file_save_as(void);
	void	file_print(void);
	void	file_print_preview(void);
	void	file_print_pdf(void);
	void	tools_find(void);
	void	tools_preferences(void);
//    void editor_switch(void);


	void on_expand_edit_area(bool flag);

	void	on_click_help_about_mytetra(void);
	void	on_click_help_about_qt(void);

	void icon_activated(QSystemTrayIcon::ActivationReason reason);

	void on_focus_changed(QWidget *, QWidget *);

    private:

	void	setup_ui(void);
	void	setup_signals(void);
	void	assembly(void);

	void	init_file_menu(void);
	void	append_quit_menu();
	void	init_tools_menu(void);
	void	init_preferences_menu(QMenu *menu);
	void	init_help_menu(void);

	void init_itemsflat_actions(void);

	void	setup_icon_actions(void);
	void	create_tray_icon(void);
	void	set_icon(void);

	void	save_geometry(void);
	void	save_tree_position(void);
//    void save_recordtable_position(void);// too many _record_screen objects, deprecated
	void	save_editor_cursor_position(void);
	void	save_editor_scrollbar_position(void);


	QString _style;

	GlobalParameters	&_globalparameters;
	AppConfig		&_appconfig;
	DataBaseConfig		&_databaseconfig;



	QAction			*_action_tray_restore;
	QAction			*_action_tray_maximize;
	QAction			*_action_tray_minimize;
	QAction			*_action_tray_quit;

	QSystemTrayIcon		*_tray_icon;
	QMenu			*_tray_icon_menu;

	QSplitter		*_v_right_splitter;
	QSplitter		*_v_find_splitter;

//	HidableTabWidget	*_vtab_tree;
	QSplitter		*_h_record_splitter;
	QSplitter		*_h_tree_splitter;
//    QSplitter           *_h_splitter;


	QMenu				*_filemenu;
	QMenu				*_editmenu;
	QMenu				*_viewmenu;
	browser::HistoryMenu		*_histrymenu;
	browser::BookmarksMenu		*_bookmarkmenu;
	QMenu				*_windowmenu;
	QMenu				*_toolsmenu;
	QMenu				*_helpmenu;

	ts_t				*_tree_screen;
	FindScreen			*_find_screen;
	MetaEditor			*_editor_screen;
	browser::Entrance		*_entrance;
//	browser::DownloadManager	*_download;
	HidableTabWidget	*_vtab_record;
	QStatusBar		*_statusbar;
	WindowSwitcher		*_switcher;



//    bool _treetable_hidden;       // = globalparameters.getTreeScreen()->isHidden();
//    bool _recordtable_hidden;     // = globalparameters.getRecordTableScreen()->isHidden();

    protected:



	void closeEvent(QCloseEvent *event);

	bool eventFilter(QObject *o, QEvent *e);	// Отслеживание прочих событий

	void go_walk_history(void);

	bool _enable_real_close;

	friend class browser::Browser;
};
#endif
