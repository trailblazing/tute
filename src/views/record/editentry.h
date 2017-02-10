#ifndef _EDITORMANAGER_H_
#define _EDITORMANAGER_H_

#include <set>

#include "libraries/qtm/editing_window.h"
#include "utility/delegate.h"
#include <QDockWidget>
#include <QList>
#include <QTextDocument>
#include <QUrl>
#include <QWidget>
// #include "models/record_table/Record.h"
// #include "libraries/GlobalParameters.h"
// #include "browser.h"
// #include "webview.h"
// #include "views/record/MetaEditor.h"
// #include "models/tree/TreeItem.h"

// #include "views/browser/tabwidget.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class tkm_t;
class QNetworkReply;
class QSslError;
class sapp_t;
class rctrl_t;
class HidableTabWidget;
class ts_t;
class FindScreen;
class EditorWrap;
class wn_t;
class AppConfig;
class Record;
class TreeItem;
class gl_para;
class EditorWrap;
class EditorTextArea;
class FlatToolButton;
class QVBoxLayout;
class EditingWindow;
class SuperMenu;

extern gl_para globalparameters;

namespace browser {
	class WebView;
	class Browser;
	class ToolbarSearch;
	class Profile;
	class Entrance;
}

// QT_BEGIN_NAMESPACE

// namespace browser {
// Architecture:
// Entrance(<=>DockWidget)__DockedWindow(s)
//               |
//               |____Browser(s)
//                         |
//                         |____TabWidget
//                                  |
//                                  |____WebView(s)
//                                           |
//                                           |____WebPage

class Editentry : public QDockWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(Entrance)
#else
	Q_OBJECT
#endif

	public:
		Editentry(QString object_name
				 , ts_t *tree_screen
				 , FindScreen *_find_screen
				 , browser::Entrance *entrance
				 , wn_t *main_window
				 , AppConfig &appconfig
				 , const QString &style_source
				 , browser::Profile *profile
				 , Qt::WindowFlags flags = 0);
		~Editentry();

		void style_source(const QString &style_source);
		QString style_source();

		void name(const QString &nm);
//		void tree_path(const QString &path);
		void pin(const QString &pin_);
//		void switch_pin(bool checked);
		void author(const QString &author_);
		void home(const QString &home_);
		void url(const QString &url_);
		void tags(const QString &tags_);
		boost::intrusive_ptr<TreeItem> item();
		void bind(boost::intrusive_ptr<TreeItem> item_to_be_bound);

		// #ifdef USE_QTM
		//    public:
		////	FlatToolButton		*_to_attach = nullptr;
		////	QIcon _icon_attach_exists;		// Иконка, когда аттачи есть
		////	QIcon _icon_attach_not_exists;		// Иконка, когда аттачей нет

		// Методы установки нестандартных процедур чтения и сохранения текста
		const std::function<void (QObject *editor, QString saveString)>save_callback() const;
		void save_callback(const std::function<void (QObject *editor, QString saveString)> &func);

		std::function<void (QObject *editor, QString &String)> load_callback() const;
		void load_callback(const std::function<void (QObject *editor, QString &String)> &func);

//		static void editor_load_callback(QObject *editor, QString &load_text);
//		static void editor_save_callback(QObject *editor, const QString &save_text);

		// Абсолютный или относительный путь (т.е. директория),
		// куда будет сохраняться текст. Без завершающего слеша
		bool work_directory(QString dir_name);
		QString work_directory(void);

		// Имя файла, куда должен сохраняться текст
		// Без пути, только имя
		void file_name(QString file_name_);
		QString file_name(void);
		void save_textarea();

		void dir_file_empty_reaction(int mode);
		int dir_file_empty_reaction(void);

		//	enum {
		//	    DIRFILEEMPTY_REACTION_SHOW_ERROR,
		//	    DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
		//	};
		// Методы установки и чтения произвольных нестандартных данных
		// которые может хранить объект редактора
		void misc_field(QString name, QString value);
		QString misc_field(QString name);
		bool load_textarea();
		int cursor_position(void);
		void cursor_position(int n);

		int scrollbar_position(void);
		void scrollbar_position(int n);

		//	void send_expand_edit_area(bool
		// flag){_editor->send_expand_edit_area(flag);}
		// #if QT_VERSION == 0x050600
		//	W_SIGNAL(send_expand_edit_area, (bool), flag)	//
		// #else
		////	;
		// #endif

		//	void
		// wyedit_find_in_base_clicked(){_editor->wyedit_find_in_base_clicked();}
		// #if QT_VERSION == 0x050600
		//	W_SIGNAL(wyedit_find_in_base_clicked)	//
		// #else
		////	;
		// #endif

		QTextDocument *textarea_document(void);
		void textarea_editable(bool editable);
		//	// Метод позволяющий управлять доступностью инструментов редактирования
		//	void disable_tool_list(QStringList
		// toolNames){_editor->disable_tool_list(toolNames);}

		//	void init_enable_assembly(bool
		// flag){_editor->init_enable_assembly(flag);}
		//	void init_config_file_name(QString
		// name){_editor->init_config_file_name(name);}
		//	void init_enable_random_seed(bool
		// flag){_editor->init_enable_random_seed(flag);}
		//	void init(int mode){_editor->init(mode);}

		// Метод установки функции переключения на предыдущее окно (для мобильного
		// интерфейса)
		void back_callback(void (*func)(void));

		//	// Метод установки функции нажатия на кнопку Attach
		//	void attach_callback(void
		// (*func)(void)){_editor->attach_callback(func);}
		//	void
		// update_indentline_geometry(){_editor->update_indentline_geometry();}
		//	void textarea(QString text){_editor->textarea(text);}
		//	void clear_all_misc_field(void){_editor->clear_all_misc_field();}
		////	// Горизонтальная линейка, содержащая кнопки форматирования
		////	QVBoxLayout		*_textformat_buttons_layout = nullptr;
		////	// Область редактирования текста
		////	EditorTextArea *_text_area;

		void clear_all(void);
		FlatToolButton *to_attach() const;

		QIcon icon_attach_exists() const;
		QIcon icon_attach_not_exists() const;

		QVBoxLayout *textformat_buttons_layout() const;
		EditorTextArea *text_area() const;

		void to_editor_layout(void);
		void to_attach_layout(void);
		EditorWrap *editor();

		// #endif

		EditingWindow *blog_editor();
		EditingWindow *add_blog_editor(EditingWindow *ew);
		SuperMenu *super_menu();

		// Q_SIGNALS:
	signals:
		void editing_activated(EditingWindow *w);
	public slots:
		void editor_switch(void);
		void init_setting(void);

		void set_scrollbars(bool hide);
		void set_cache(bool cache, int cache_size);
		void finished(QNetworkReply *reply);
		void ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors);

		void setup_actions(void);

		Editentry *prepend(browser::Browser *);
		void on_activate_window();

#if defined(Q_OS_OSX)
		void lastWindowClosed();
#endif

	private slots:

	protected:
		void resizeEvent(QResizeEvent *e);

	private:
		void setup_ui(void);

		void assembly(void);

		QList<EditingWindow *> _editing_list;
		ts_t *_tree_screen			= nullptr;
		FindScreen *_find_screen	= nullptr;
		//	MetaEditor                          *_editor_screen;
		//        HidableTabWidget                *_vtab_tree;
		browser::Entrance *_entrance	= nullptr;
		wn_t *_main_window				= nullptr;
		AppConfig &_appconfig;
		// RecordController                *_record_controller;
		QString _style_source;
		browser::Profile *_profile	= nullptr;
		Qt::WindowFlags _flags		= 0;
		QWidget *_hidetitlebar		= nullptr;

		SuperMenu *_super_menu		= nullptr;
		EditingWindow *_blog_editor = nullptr;
		//		MetaEditor                          *_editor		=
		// nullptr;

//		QSplitter *_splitter;
//		std::shared_ptr<QSettings> _splitter_config;
//		QString _splitter_groupname;
//		QString _splitter_sizelist;

		QMetaObject::Connection _home_connection;	// for disconnect

		friend class sapp_t;
		friend class EditorWrap;
		friend class Editor;
		friend class EditingWindow;
		friend class SideTabWidget;
		EditingWindow *blog_editor() const;
};
// }

// QT_END_NAMESPACE

#endif	// _EDITORMANAGER_H_
