#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__

#include <wobjectdefs.h>
#include <QObject>

#include <QStatusBar>

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

// W_REGISTER_ARGTYPE(rs_t)
// W_REGISTER_ARGTYPE(rs_t *)

class GlobalParameters : public QObject {
    W_OBJECT(GlobalParameters)

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

	void		vtab_record(QTabWidget *point);
	QTabWidget	*vtab_record();

	void		vtab_tree(QTabWidget *point);
	QTabWidget	*vtab_tree();

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

	wn_t		*mainwindow()				{return _mainwindow;}
	void		mainwindow(wn_t *mainwindow)		{_mainwindow = mainwindow;}
	void		crypt_key(QByteArray hash);
	QByteArray	crypt_key(void);

	// Файл стилей может создаваться и после развертывания начальных файлов MyTetra
	// Так как в более старых версиях MyTetra его еще небыло
	void create_stylesheet_file(QString dirname);

	void				style_source(const QString &source)		{_style_source = source;}
	QString				style_source() const				{return _style_source;}
	void				download_manager(browser::DownloadManager *dm)	{_download_manager = dm;}
	browser::DownloadManager	*download_manager();

	void			editor_config(EditorConfig *dialog)					{_editor_config = dialog;}
	EditorConfig		*editor_config()							{return _editor_config;}
	AttachTableController	*attachtable_controller()						{return _attachtable_controller;}
	void			attachtable_controller(AttachTableController *_attachtable_controller)	{this->_attachtable_controller = _attachtable_controller;}
    private:

	void	init_workdirectory(void);
	bool	find_workdirectory(void);
	bool	is_mytetra_ini_config(QString filename);
	void	create_standard_programfiles(void);
	void	create_portable_programfiles(void);
	void	create_first_programfiles(QString dirName);

	browser::Profile		*_profile;
	ts_t				*_tree_screen;
	browser::Entrance		*_entrance;
	std::vector<rs_t *>		_table_screens;
	EditorConfig			*_editor_config;
	FindScreen			*_find_screen;
	MetaEditor			*_meta_editor;
	QStatusBar			*_statusbar;
	WindowSwitcher			*_window_switcher;
	QSplitter			*_find_splitter;
	QSplitter			*_v_left_splitter;
	QSplitter			*_h_record_splitter;
	QSplitter			*_v_right_splitter;
	QTabWidget			*_vtab_record;
	QTabWidget			*_vtab_tree;
	wn_t				*_mainwindow;
	browser::DownloadManager	*_download_manager;
	QString				_main_program_file;
	QString				_work_directory;
	AttachTableController		*_attachtable_controller;

	QByteArray	_password_hash;
	QString		_style_source;
};

#endif /* __GLOBALPARAMETERS_H__ */

