#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__



#include <QStatusBar>



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


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

// W_REGISTER_ARGTYPE(rs_t)
// W_REGISTER_ARGTYPE(rs_t *)

class GlobalParameters : public QObject {
#if QT_VERSION == 0x050600
    W_OBJECT(GlobalParameters)
#else
    Q_OBJECT
#endif

    public:
	GlobalParameters(QObject *pobj = 0);
	virtual ~GlobalParameters();

	void	main_program_file(QString file);
	QString main_program_file(void);

	void init(void);

	QString application_mode() const;
	void	application_mode(const QString &mode);

	QString work_directory(void);

	QString target_os(void);
	QString application_name(void);


	browser::Profile	*profile();
	void			profile(browser::Profile *profile_);

	QSplitter	*find_splitter();
	void		find_splitter(QSplitter *_find_splitter);

	void		h_record_splitter(QSplitter *h_record_splitter);
	QSplitter	*h_record_splitter();

	void		h_tree_splitter(QSplitter *vleftsplitter);
	QSplitter	*h_tree_splitter();

	void		v_right_splitter(QSplitter *vrightsplitter);
	QSplitter	*v_right_splitter();

	void			vtab_record(HidableTabWidget *point);
	HidableTabWidget	*vtab_record();

//	void		vtab_tree(QTabWidget *point);
//	QTabWidget	*vtab_tree();

	void	tree_screen(ts_t *point);
	ts_t	*tree_screen();

	browser::Entrance	*entrance();
	void			entrance(browser::Entrance * &b);

	void			push_record_screen(rs_t *point);
	std::vector<rs_t *>	record_screens();

	//    void page_screen(RecordScreen *page);
	//    RecordScreen *page_screen();

	void		find_screen(FindScreen *point);
	FindScreen	*find_screen();

	void		meta_editor(MetaEditor *point);
	MetaEditor	*meta_editor();

	void		status_bar(QStatusBar *point);
	QStatusBar	*status_bar();

	void		window_switcher(WindowSwitcher *point);
	WindowSwitcher	*window_switcher();

	wn_t		*main_window();
	void		main_window(wn_t *main_window);
	void		crypt_key(QByteArray hash);
	QByteArray	crypt_key(void);

	// Файл стилей может создаваться и после развертывания начальных файлов MyTetra
	// Так как в более старых версиях MyTetra его еще небыло
	void create_stylesheet_file(QString dirname);

	void				style_source(const QString &source);
	QString				style_source() const;
	browser::DownloadManager	*request_download_manager();
	void				download_manager(browser::DownloadManager *dm);
	browser::DownloadManager	*download_manager() const;

	void			editor_config(EditorConfig *dialog);
	EditorConfig		*editor_config();
	AttachTableController	*attachtable_controller();
	void			attachtable_controller(AttachTableController *_attachtable_controller);
	bool			is_mytetra_ini_config_exist(QString filename);
//	QMap<QString, QString>	config_ini() const;
//	QMap<QString, QString>	mytetra_xml() const;
//	QMap<QString, QString>	editorconf() const;
    private:

	void	init_workdirectory(void);
	bool	find_workdirectory(void);

	void	create_standard_programfiles(void);
	void	create_portable_programfiles(void);
	void	create_first_programfiles(QString dirName);

	browser::Profile			*_profile			= nullptr;
	ts_t					*_tree_screen			= nullptr;
	browser::Entrance			*_entrance			= nullptr;
	EditorConfig				*_editor_config			= nullptr;
	FindScreen				*_find_screen			= nullptr;
	MetaEditor				*_meta_editor			= nullptr;
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

	QByteArray				_password_hash;
	QString					_style_source;
	QString					_main_program_file;
	QString					_work_directory;
	std::vector<rs_t *>			_table_screens;
};

#endif /* __GLOBALPARAMETERS_H__ */

