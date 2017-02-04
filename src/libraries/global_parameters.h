#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__



#include <QStatusBar>



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

extern const char	*standardItem;
extern const char	*portableItem;

// #ifndef QWEBENGINEPAGE_HITTESTCONTENT
// #define QWEBENGINEPAGE_HITTESTCONTENT
// #endif

namespace browser {
    class Entrance;
    class DownloadManager;
    class Profile;
}

class ts_t;
class MetaEditor;
class rs_t;
class FindScreen;
class WindowSwitcher;
class wn_t;
class QTabWidget;
class QSplitter;
class EditorConfigDialog;
class EditorConfig;
class AttachTableController;
class HidableTabWidget;
class Editentry;


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

//	void main_program_full_file(QString file);
	QString main_program_full_file(void) const;
//	QString main_program_dir() const;

	void init(const QString &app_name);

//	QString app_mode() const;
//	void app_mode(const QString &mode);

	QString root_path(void) const;
	bool root_path(QString path_name);

//	QString root_path(void) const;
	QString config_filename(void) const;

	QString target_os(void) const;
	QString application_name(void) const;


	browser::Profile	*profile() const;
	void profile(browser::Profile *profile_);

	QSplitter	*find_splitter() const;
	void find_splitter(QSplitter *_find_splitter);

	void h_record_splitter(QSplitter *h_record_splitter);
	QSplitter	*h_record_splitter() const;

	void h_tree_splitter(QSplitter *vleftsplitter);
	QSplitter	*h_tree_splitter() const;

	void v_right_splitter(QSplitter *vrightsplitter);
	QSplitter	*v_right_splitter() const;

	void vtab_record(HidableTabWidget *point);
	HidableTabWidget	*vtab_record() const;

//	void		vtab_tree(QTabWidget *point);
//	QTabWidget	*vtab_tree();

	void tree_screen(ts_t *point);
	ts_t	*tree_screen() const;

	browser::Entrance	*entrance() const;
	void entrance(browser::Entrance * &b);

	void push_record_screen(rs_t *point);
	std::vector<rs_t *>	record_screens() const;

	//    void page_screen(RecordScreen *page);
	//    RecordScreen *page_screen();

	void find_screen(FindScreen *point);
	FindScreen	*find_screen() const;

	void meta_editor(Editentry *point);
	Editentry *meta_editor() const;

	void status_bar(QStatusBar *point);
	QStatusBar	*status_bar() const;

	void window_switcher(WindowSwitcher *point);
	WindowSwitcher	*window_switcher() const;

	wn_t		*main_window() const;
	void main_window(wn_t *main_window);

	void crypt_key(QByteArray hash);
	QByteArray crypt_key(void) const;

//	// Файл стилей может создаваться и после развертывания начальных файлов Hapnote
//	// Так как в более старых версиях Hapnote его еще небыло
//	void create_stylesheet_file(QString config_dir);

	void style_source(const QString &source);
	QString	style_source() const;

	browser::DownloadManager	*request_download_manager();

	void download_manager(browser::DownloadManager *dm);
	browser::DownloadManager	*download_manager() const;

	EditorConfig		*editor_config() const;
	void editor_config(EditorConfig *dialog);

	AttachTableController	*attachtable_controller() const;
	void attachtable_controller(AttachTableController *_attachtable_controller);
//	bool is_hapnote_ini_config_exist(const QString &filename) const;
//	QMap<QString, QString>	config_ini() const;
//	QMap<QString, QString>	hapnote_xml() const;
//	QMap<QString, QString>	editorconf() const;
	const QString _mode_filename			= "mode.ini";
	const QString _conf_filename			= "conf.ini";
	const QString _browser_conf_filename		= "browser.conf";
	const QString _browserview_conf_filename	= "browserview.ini";
	const QString _editor_conf_filename		= "editorconf.ini";
	const QString _entrance_conf_filename		= "entrance.ini";
	const QString _stylesheet_filename		= "stylesheet.css";
    private:
#define STANDARD_MODE		true
#define PORTABLE_MODE		false
	std::tuple<const bool, const QString> coordinate_root(const QString &root_path_ = "");
//	bool check_workdirectory(bool enablePortable);

//	void create_root_standard(void);
//	void create_root_portable(void);
//	void initialize_root_impl(const QString &root_dir);

	browser::Profile			*_profile			= nullptr;
	ts_t					*_tree_screen			= nullptr;
	browser::Entrance			*_entrance			= nullptr;
	EditorConfig				*_editor_config			= nullptr;
	FindScreen				*_find_screen			= nullptr;
	Editentry				*_meta_editor			= nullptr;
	QStatusBar				*_statusbar			= nullptr;
	WindowSwitcher				*_window_switcher		= nullptr;
	QSplitter				*_find_splitter			= nullptr;
	QSplitter				*_v_left_splitter		= nullptr;
	QSplitter				*_h_record_splitter		= nullptr;
	QSplitter				*_v_right_splitter		= nullptr;
	HidableTabWidget			*_vtab_record			= nullptr;	//	HidableTabWidget			*_vtab_tree		= nullptr;
	wn_t					*_mainwindow			= nullptr;
	browser::DownloadManager		*_download_manager		= nullptr;
	AttachTableController			*_attachtable_controller	= nullptr;

	QByteArray _password_hash;
	QString	_style_source		= "";
	QString	_main_program_full_file	= "";
	QString _application_name	= "";
//	QString _main_program_path	= "";
//	QString	_root_path				= "./";

	QString _standard_path				= "";
	std::pair<QString, QString>_standard_paths	= {"", ""};
	std::vector<rs_t *>			_table_screens;
	friend class AppConfigPageMain;
};

#endif /* __GLOBALPARAMETERS_H__ */

