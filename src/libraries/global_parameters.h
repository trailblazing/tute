#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__


#include <functional>
#include <memory>
#include <set>

#include <QDir>
#include <QStandardPaths>
#include <QStatusBar>
#include <QUrl>

// #include <QCoreApplication>


namespace web {
	template <typename real_t>
	class Docker;
	class DownloadManager;
	class Profile;
	class Browser;
	class TabWidget;
	class ChaseWidget;
}

class ts_t;
class EditorWrap;
class rs_t;
class FindScreen;
class WindowSwitcher;
class wn_t;
class QTabWidget;
class QSplitter;
class EditorConfigDialog;
class EditorConfig;
class AttachTableController;
class HidableTab;
class EditorDock;
class SysTrayIcon;
class QUrl;
class Blogger;
class rctrl_t;
class rv_t;


#include "utility/lease.h"


#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif


extern const char* standardItem;
extern const char* portableItem;

// #ifndef QWEBENGINEPAGE_HITTESTCONTENT
// #define QWEBENGINEPAGE_HITTESTCONTENT
// #endif



#ifdef USE_SIGNAL_CLOSE
typedef sd::intrusive_ptr<Blogger> blogger_ref;
typedef sd::intrusive_ptr<web::Browser> browser_ref;
typedef sd::intrusive_ptr<web::TabWidget> tabwidget_ref;
typedef sd::intrusive_ptr<rs_t> rs_ref;
typedef sd::intrusive_ptr<rctrl_t> rctrl_ref;
typedef sd::intrusive_ptr<rv_t> rv_ref;
typedef sd::intrusive_ptr<web::ChaseWidget> chasewidget_ref;

#else
typedef Blogger* blogger_ref;
typedef web::Browser* browser_ref;
typedef web::TabWidget* tabwidget_ref;
typedef rs_t* rs_ref;
typedef rctrl_t* rctrl_ref;
typedef rv_t* rv_ref;
typedef ChaseWidget* chasewidget_ref;
#endif // USE_SIGNAL_CLOSE


#ifndef USE_QTM
#define USE_QTM
#endif

// W_REGISTER_ARGTYPE(rs_t)
// W_REGISTER_ARGTYPE(rs_t *)

class gl_para : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(GlobalParameters)
#else
	Q_OBJECT
#endif

    public:
	gl_para(QObject* pobj = 0);
	virtual ~gl_para();

	// void main_program_full_file(QString file);
	QString main_program_full_file(void) const;
	// QString main_program_dir() const;

	// void init();// const QString &app_name

	// QString app_mode() const;
	// void app_mode(const QString &mode);

	QString root_path(void) const;
	// bool permanent_root_path_to_standard_path(QString path_name);

	// QString root_path(void) const;
	// QString config_filename(void) const;

	QString target_os(void) const;
	QString application_name(void) const;

	web::Profile* profile() const;
	void profile(web::Profile* profile_);

	QSplitter* find_splitter() const;
	void find_splitter(QSplitter* _find_splitter);

	void h_record_splitter(QSplitter* h_record_splitter);
	QSplitter* h_record_splitter() const;

	void h_tree_splitter(QSplitter* vleftsplitter);
	QSplitter* h_tree_splitter() const;

	void v_right_splitter(QSplitter* vrightsplitter);
	QSplitter* v_right_splitter() const;

	void vtab_record(HidableTab* point);
	HidableTab* vtab_record() const;

	// void		vtab_tree(QTabWidget *point);
	// QTabWidget	*vtab_tree();

	void tree_screen(ts_t* point);
	ts_t* tree_screen() const;

	web::Docker<web::Browser>* browser_docker() const;
	void browser_docker(web::Docker<web::Browser>*& b);

	void push_record_screen(rs_t* point);
//#ifdef USE_SIGNAL_CLOSE
//	std::set<sd::intrusive_ptr<rs_t>>
//#else
	std::set<rs_t*>
//#endif //USE_SIGNAL_CLOSE
	record_screens() const;

	// void page_screen(RecordScreen *page);
	// RecordScreen *page_screen();

	void find_screen(FindScreen* point);
	FindScreen* find_screen() const;

	void editor_docker(web::Docker<Blogger>* point);
	web::Docker<Blogger>* editor_docker() const;

	void status_bar(QStatusBar* point);
	QStatusBar* status_bar() const;

	void window_switcher(WindowSwitcher* point);
	WindowSwitcher* window_switcher() const;

	wn_t* main_window() const;
	void main_window(wn_t* main_window);

	void crypt_key(QByteArray hash);
	QByteArray crypt_key(void) const;

	//// Файл стилей может создаваться и после развертывания начальных файлов
	//// Так как в более старых версиях его еще небыло
	// void create_stylesheet_file(QString config_dir);

	void style_source(const QString& source);
	QString style_source() const;

	web::DownloadManager* request_download_manager();

	void download_manager(web::DownloadManager* dm);
	web::DownloadManager* download_manager() const;

	//	std::shared_ptr<EditorConfig> editor_config() const;
	//	void editor_config(std::shared_ptr<EditorConfig> dialog);

	//	AttachTableController* attachtable_controller() const;
	//	void attachtable_controller(AttachTableController* _attachtable_controller);
	// bool is_ini_config_exist(const QString &filename) const;
	// QMap<QString, QString>	config_ini() const;
	// QMap<QString, QString>	index_xml() const;
	// QMap<QString, QString>	editorconf() const;

	// static constexpr const char *_defaulthome = "about:blank"; // can work without definition in global_parameters.cpp but for lease.h it is an undefined reference
	static constexpr char _program_root_dir_name[] = "rootdir";
	static constexpr char _program_instance_name[] = "tute"; // QCoreApplication::applicationName();// won't work
	static constexpr char _index_xml_file_name[] = "index.xml";
	static constexpr char _mode_filename[] = "mode.ini";              // static constexpr char _mode_filename[]	= "mode.ini";
	static constexpr char _conf_filename[] = "conf.ini";              // static constexpr char _conf_filename[]	= "conf.ini";
	static constexpr char _browser_conf_filename[] = "browser.conf";  // static constexpr char _browser_conf_filename[] = "browser.conf";
	static constexpr char _dock_conf_filename[] = "dock.ini";         // static constexpr char _dock_conf_filename[] = "dock.ini";
	static constexpr char _dock_settings_section_name[] = "system";   // static constexpr char _dock_settings_section_name[] = "system";
	static constexpr char _editor_conf_filename[] = "editorconf.ini"; // static constexpr char _editor_conf_filename[] = "editorconf.ini";
	static constexpr char _data_conf_filename[] = "database.ini";     //	static constexpr char *_browser_dock_conf_filename = "entrance.ini";//static constexpr char _entrance_conf_filename[] = "entrance.ini";
	static constexpr char _stylesheet_filename[] = "stylesheet.css";  // static constexpr char _stylesheet_filename[] = "stylesheet.css";
	// static constexpr char _document_config_name[] = "document.ini";
	static constexpr char _file_menu_name[] = "file_menu";
	static constexpr char _tools_menu_name[] = "tools_menu";
	static constexpr char _edit_menu_name[] = "edit_menu";
	static constexpr char _view_menu_name[] = "view_menu";
	static constexpr char _window_menu_name[] = "window_menu";
	static constexpr char _help_menu_name[] = "help_menu";
	static constexpr char _history_menu_name[] = "history_menu";
	static constexpr char _bookmark_menu_name[] = "bookmark_menu";

	static constexpr char _default_topic[] = "undefined";
	static constexpr char _default_post[] = "Welcome to \"undefined\" topic";

//	static constexpr const char *_what_ever_topic	= "what_ever";
#ifdef USE_CURRENT_BROWSER_KEY_WORD
	static constexpr char _current_browser[] = "current_browser";
#endif // USE_CURRENT_BROWSER_KEY_WORD
	static constexpr char _browser_navigater_name[] = "browser_navigater";
	static constexpr char _browser_lineedit_stack_name[] = "browser_lineedit_stack";
	static constexpr char _blog_root_dir[] = "blog";
	static constexpr char _custom_hidable_tab_style[] = "custom_hidable_tab_style.css";
	static constexpr char _custom_browser_tabbar_style[] = "custom_browser_tabbar_style.css";
	static constexpr int initialize_priority = 0;
	template <typename T>
	struct initialize_prior_to;
	SysTrayIcon* tray_icon();
	void tray_icon(SysTrayIcon* ti);

	void global_home(const QUrl& url_);
	QUrl global_home() const;
	QString editors_shared_full_path_name() const;
	QString app_data_path_system() const;

    private:
#define STANDARD_MODE true
#define PORTABLE_MODE false
	std::tuple<const bool, const QString>
	permanent_coordinate_root(const QString& recommend_root_path_ = "", bool force = false);
	// bool check_workdirectory(bool enablePortable);

	// void create_root_standard(void);
	// void create_root_portable(void);
	// void initialize_root_impl(const QString &root_dir);
	QString _app_data_path_system = QStandardPaths::writableLocation(
	    QStandardPaths::AppConfigLocation); // QDir::homePath() + "/.config/" +
						// gl_para::_program_instance_name;
#ifdef USE_ALTERNATIVE_PATH
	std::pair<QString, QString> _candidate_mode_paths_by_system = {
	    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
		gl_para::_program_instance_name,
	    QDir::homePath() + "/." + gl_para::_program_instance_name};
#endif // USE_ALTERNATIVE_PATH
	std::function<QString()> mode_full_name_in_app_data_path_system;
	std::function<std::tuple<const bool, const QString>()> init;
	QString _editors_shared_full_path_name = "";
	web::Profile* _profile = nullptr;
	ts_t* _tree_screen = nullptr;
	web::Docker<web::Browser>* _browser_docker = nullptr;
	std::shared_ptr<EditorConfig> _editor_config;
	FindScreen* _find_screen = nullptr;
	web::Docker<Blogger>* _editor_docker = nullptr;
	QStatusBar* _statusbar = nullptr;
	WindowSwitcher* _window_switcher = nullptr;
	QSplitter* _find_splitter = nullptr;
	QSplitter* _v_left_splitter = nullptr;
	QSplitter* _h_record_splitter = nullptr;
	QSplitter* _v_right_splitter = nullptr;
	HidableTab* _vtab_record = nullptr; // HidableTabWidget
	// *_vtab_tree		= nullptr;
	wn_t* _mainwindow = nullptr;
	web::DownloadManager* _download_manager = nullptr;
	AttachTableController* _attachtable_controller = nullptr;

	QByteArray _password_hash;
	QString _style_source = "";
	QString _main_program_full_file = "";
	// QString _program_instance_name = "";
	// QString _main_program_path	= "";
	// QString	_root_path				= "./";

	std::vector<rs_t*> _table_screens;
	SysTrayIcon* _tray_icon;

	QUrl _global_home;
	friend class AppConfigPageMain;
};

#endif /* __GLOBALPARAMETERS_H__ */
