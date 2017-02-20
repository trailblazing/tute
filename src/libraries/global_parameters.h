#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__

#include <QDir>
#include <QStandardPaths>
#include <QStatusBar>
#include <functional>
#include <memory>
#include <set>
// #include <QCoreApplication>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

extern const char *standardItem;
extern const char *portableItem;

// #ifndef QWEBENGINEPAGE_HITTESTCONTENT
// #define QWEBENGINEPAGE_HITTESTCONTENT
// #endif

namespace browser {
	class BrowserDock;
	class DownloadManager;
	class Profile;
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
	gl_para(QObject *pobj = 0);
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

	browser::Profile *profile() const;
	void profile(browser::Profile *profile_);

	QSplitter *find_splitter() const;
	void find_splitter(QSplitter *_find_splitter);

	void h_record_splitter(QSplitter *h_record_splitter);
	QSplitter *h_record_splitter() const;

	void h_tree_splitter(QSplitter *vleftsplitter);
	QSplitter *h_tree_splitter() const;

	void v_right_splitter(QSplitter *vrightsplitter);
	QSplitter *v_right_splitter() const;

	void vtab_record(HidableTab *point);
	HidableTab *vtab_record() const;

	// void		vtab_tree(QTabWidget *point);
	// QTabWidget	*vtab_tree();

	void tree_screen(ts_t *point);
	ts_t *tree_screen() const;

	browser::BrowserDock *browser_dock() const;
	void browser_dock(browser::BrowserDock * &b);

	void push_record_screen(rs_t *point);
	std::set<rs_t *> record_screens() const;

	// void page_screen(RecordScreen *page);
	// RecordScreen *page_screen();

	void find_screen(FindScreen *point);
	FindScreen *find_screen() const;

	void editor_dock(EditorDock *point);
	EditorDock *editor_dock() const;

	void status_bar(QStatusBar *point);
	QStatusBar *status_bar() const;

	void window_switcher(WindowSwitcher *point);
	WindowSwitcher *window_switcher() const;

	wn_t *main_window() const;
	void main_window(wn_t *main_window);

	void crypt_key(QByteArray hash);
	QByteArray crypt_key(void) const;

	//// Файл стилей может создаваться и после развертывания начальных файлов
	//// Так как в более старых версиях его еще небыло
	// void create_stylesheet_file(QString config_dir);

	void style_source(const QString &source);
	QString style_source() const;

	browser::DownloadManager *request_download_manager();

	void download_manager(browser::DownloadManager *dm);
	browser::DownloadManager *download_manager() const;

	std::shared_ptr<EditorConfig> editor_config() const;
	void editor_config(std::shared_ptr<EditorConfig> dialog);

	AttachTableController *attachtable_controller() const;
	void attachtable_controller(AttachTableController *_attachtable_controller);
	// bool is_ini_config_exist(const QString &filename) const;
	// QMap<QString, QString>	config_ini() const;
	// QMap<QString, QString>	index_xml() const;
	// QMap<QString, QString>	editorconf() const;
	static constexpr char _program_instance_name[] = "tute"; // QCoreApplication::applicationName();// won't work
	static constexpr char _mode_filename[]	= "mode.ini";
	static constexpr char _conf_filename[]	= "conf.ini";
	static constexpr char _browser_conf_filename[] = "browser.conf";
	static constexpr char _browserview_conf_filename[] = "browserview.ini";
	static constexpr char _editor_conf_filename[] = "editorconf.ini";
	static constexpr char _entrance_conf_filename[] = "entrance.ini";
	static constexpr char _stylesheet_filename[] = "stylesheet.css";
	// static constexpr char _document_config_name[] = "document.ini";
	SysTrayIcon *tray_icon();
	void tray_icon(SysTrayIcon *ti);

private:
#define STANDARD_MODE	true
#define PORTABLE_MODE	false
	std::tuple<const bool, const QString> permanent_coordinate_root(const QString &recommend_root_path_ = "", bool force = false);
	// bool check_workdirectory(bool enablePortable);

	// void create_root_standard(void);
	// void create_root_portable(void);
	// void initialize_root_impl(const QString &root_dir);

	browser::Profile *_profile = nullptr;
	ts_t *_tree_screen = nullptr;
	browser::BrowserDock *_entrance = nullptr;
	std::shared_ptr<EditorConfig> _editor_config;
	FindScreen *_find_screen	= nullptr;
	EditorDock *_editor_dock	= nullptr;
	QStatusBar *_statusbar = nullptr;
	WindowSwitcher *_window_switcher = nullptr;
	QSplitter *_find_splitter = nullptr;
	QSplitter *_v_left_splitter = nullptr;
	QSplitter *_h_record_splitter	= nullptr;
	QSplitter *_v_right_splitter	= nullptr;
	HidableTab *_vtab_record = nullptr; // HidableTabWidget			*_vtab_tree		= nullptr;
	wn_t *_mainwindow = nullptr;
	browser::DownloadManager *_download_manager = nullptr;
	AttachTableController *_attachtable_controller = nullptr;

	QByteArray _password_hash;
	QString _style_source = "";
	QString _main_program_full_file = "";
	// QString _program_instance_name = "";
	// QString _main_program_path	= "";
	// QString	_root_path				= "./";

	QString _root_path_given_by_system = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); // QDir::homePath() + "/.config/" + gl_para::_program_instance_name;
#ifdef USE_ALTERNATIVE_PATH
	std::pair<QString, QString> _candidate_mode_paths_by_system = {QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + gl_para::_program_instance_name, QDir::homePath() + "/." + gl_para::_program_instance_name};
#endif // USE_ALTERNATIVE_PATH
	std::vector<rs_t *> _table_screens;
	SysTrayIcon *_tray_icon;
	std::function<QString()> mode_file_full_name_by_system;
	std::function<std::tuple<const bool, const QString>()> init;
	friend class AppConfigPageMain;
};

#endif /* __GLOBALPARAMETERS_H__ */
